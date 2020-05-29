/** @file
  Smm Int15 Service Driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Int15ServiceSmm.h"

UINT16                                 mInt15Count = 0;
EFI_SMM_SYSTEM_TABLE2                  *mSmst;
INT15_SERVICE_INSTANCE                 *mInt15ServiceInstance = NULL;
EFI_SMM_CPU_PROTOCOL                   *mSmmCpu = NULL;

/**
  Install Int15 OpRom to shadow ram

  @retval EFI_SUCCESS           Install Int15 OpRom successfully.

**/
EFI_STATUS
EFIAPI
InstallInt15OpRom (
  );

/**
  Install SmmInt15service Protocol

  @retval EFI_SUCCESS           Install SmmInt15service Protocol successfully.

**/
EFI_STATUS
EFIAPI
InstallSmmInt15ServiceProtocol (
  );

/**
  This function is invoked when gEfiLegacyBiosProtocolGuid is installed

  @param[in]  Event             The triggered event.
  @param[in]  Context           Context for this event.

**/
VOID
EFIAPI
InstallInt15ServiceNotifyFunction (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  );

/**
  INT15 SW SMI callback

  @param      Handle            The unique handle assigned to this handler by SmiHandlerRegister().
  @param      Context           Points to an optional handler context which was specified when the
                                handler was registered.
  @param      CommBuffer        A pointer to a collection of data in memory that will
                                be conveyed from a non-SMM environment into an SMM environment.
  @param      CommBufferSize    The size of the CommBuffer.

  @retval     EFI_SUCCESS       The callabck was handled successfully.
  @retval     EFI_NOT_FOUND     Cannot determine the source of SMI.

**/
STATIC
EFI_STATUS
EFIAPI
INT15SwSmiCallback (
  IN     EFI_HANDLE                    Handle,
  IN     CONST VOID                    *Context,
  IN OUT VOID                          *CommBuffer,
  IN OUT UINTN                         *CommBufferSize
  );

/**
  INT15 Smm Callback of EFI_INT15_SERVICE_PROTOCOL

  @param[in]  CpuRegisters      Point to Cpu registers.

**/
STATIC
VOID
SmmInt15CallBack (
  IN  OUT EFI_IA32_REGISTER_SET   *CpuRegisters
  );

/**
  Initializes the INT15 SMM Dispatcher

  @param[in]  ImageHandle       Pointer to the loaded image protocol for this driver.
  @param[in]  SystemTable       Pointer to the EFI System Table

  @retval EFI_SUCCESS           The call returned successfully.

**/
EFI_STATUS
EFIAPI
Int15ServiceDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               InSmm;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;
  INT15_DATA_PACKET                     *Int15DataPacketDummy;
  EFI_OEM_INT15_CALLBACK                *OemInt15Callback;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  EFI_EVENT                             LegacyBiosEvent;
  VOID                                  *Registration;

  OemInt15Callback = NULL;
  InSmm = FALSE;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmBase->InSmm (SmmBase, &InSmm);
  if (!InSmm) {
    //
    // Locate LegacyBios Protocol
    //
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);

    if (!EFI_ERROR (Status)) {
      InstallInt15ServiceNotifyFunction (NULL, NULL);
    } else {
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      InstallInt15ServiceNotifyFunction,
                      NULL,
                      &LegacyBiosEvent
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = gBS->RegisterProtocolNotify (
                      &gEfiLegacyBiosProtocolGuid,
                      LegacyBiosEvent,
                      &Registration
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  } else {
    //
    // Great!  We're now in SMM!
    // Initialize global variables
    //
    Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = mSmst->SmmLocateProtocol (
                      &gEfiSmmSwDispatch2ProtocolGuid,
                      NULL,
                      (VOID **)&SwDispatch
                      );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Register SwSmi for ATA Legacy services
    //
    SwContext.SwSmiInputValue = COMMON_INT15_SMI;

    Status = SwDispatch->Register (
                           SwDispatch,
                           INT15SwSmiCallback,
                           &SwContext,
                           &SwHandle
                           );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Initialize INT15DataPacketDummy
    //
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (INT15_DATA_PACKET),
                      (VOID **)&Int15DataPacketDummy
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ZeroMem (Int15DataPacketDummy, sizeof (INT15_DATA_PACKET));

    Int15DataPacketDummy->Signature = INT15_DATA_SIGNATURE;
    InitializeListHead (&Int15DataPacketDummy->FunctionLinkList);

    //
    // Initialize OemInt15Callback
    //
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (EFI_OEM_INT15_CALLBACK),
                      (VOID **)&OemInt15Callback
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ZeroMem (OemInt15Callback, sizeof (EFI_OEM_INT15_CALLBACK));

    //
    // Initialize Int15ServiceInstance
    //
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (INT15_SERVICE_INSTANCE),
                      (VOID **)&mInt15ServiceInstance
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ZeroMem (mInt15ServiceInstance, sizeof (INT15_SERVICE_INSTANCE));

    mInt15ServiceInstance->Int15DataPacketDummy = Int15DataPacketDummy;
    mInt15ServiceInstance->OemInt15Callback     = OemInt15Callback;

    Status = InstallSmmInt15ServiceProtocol ();
  }

  return EFI_SUCCESS;
}

/**
  Install SmmInt15service Protocol

  @retval EFI_SUCCESS           Install SmmInt15service Protocol successfully.

**/
EFI_STATUS
EFIAPI
InstallSmmInt15ServiceProtocol (
  )
{
  INT15_PRIVATE_INFO                   *Int15PrivateInfo;
  EFI_HANDLE                           Handle;
  EFI_STATUS                           Status;
  INT15_PRIVATE_DATA                   *Int15PrivateData;
  EFI_SMM_RUNTIME_PROTOCOL             *SmmRT;

  SmmRT = NULL;
  Int15PrivateData = NULL;
  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  //
  // Locate Int15PrivateData from gInt15PrivateDataGuid
  //
  Status = gBS->LocateProtocol(
                  &gInt15PrivateDataGuid,
                  NULL,
                  (VOID **)&Int15PrivateData
                  );
  if (!EFI_ERROR (Status)) {

    Int15PrivateInfo = (INT15_PRIVATE_INFO*)((UINTN)(Int15PrivateData->CpuRegistersSegment << 4));

    Int15PrivateInfo->SmmInt15CallbackPtr = (UINTN)SmmInt15CallBack;

    mInt15ServiceInstance->Signature = INT15_SERVICE_INSTANCE_SIGNATURE;
    mInt15ServiceInstance->FunctionListSegment = Int15PrivateData->FunctionListSegment;
    mInt15ServiceInstance->CpuRegistersSegment = Int15PrivateData->CpuRegistersSegment;
    mInt15ServiceInstance->Int15Service.InstallInt15ProtocolInterface = InstallInt15ProtocolInterface;
    mInt15ServiceInstance->Int15Service.ReinstallInt15ProtocolInterface = ReinstallInt15ProtocolInterface;

    //
    // Install SmmInt15Service in SMM
    //
    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gEfiSmmInt15ServiceProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mInt15ServiceInstance->Int15Service
                      );

    if (SmmRT != NULL) {
      Status = SmmRT->InstallProtocolInterface (
                        &Handle,
                        &gEfiSmmInt15ServiceProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &mInt15ServiceInstance->Int15Service
                        );

    }
  }
  return EFI_SUCCESS;
}

/**
  This function is invoked when gEfiLegacyBiosProtocolGuid is installed

  @param[in]  Event             The triggered event.
  @param[in]  Context           Context for this event.

**/
VOID
EFIAPI
InstallInt15ServiceNotifyFunction (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  EFI_HANDLE                            Handle;
  EFI_STATUS                            Status;
  EFI_SMM_INT15_SERVICE_PROTOCOL        *SmmInt15Service;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  //
  // Install common Int15 OpRom
  //
  InstallInt15OpRom ();

  //
  // Install gEfiSmmInt15ServiceProtocolGuid in DXE for dependency
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiSmmInt15ServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  //
  // Check the gEfiSmmInt15ServiceProtocolGuid in DXE whether installed or not
  //
  SmmInt15Service = NULL;
  Status = gBS->LocateProtocol (&gEfiSmmInt15ServiceProtocolGuid, NULL, (VOID **)&SmmInt15Service);
  if (!EFI_ERROR(Status)) {
    IoWrite8 (SW_SMI_PORT, COMMON_INT15_SMI);
  }
}

/**
  Install Int15 OpRom to shadow ram

  @retval EFI_SUCCESS           Install Int15 OpRom successfully.

**/
EFI_STATUS
EFIAPI
InstallInt15OpRom (
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *FvHandles;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  EFI_LEGACY_REGION2_PROTOCOL           *LegacyRegion;
  EFI_IA32_REGISTER_SET                 Regs;
  EFI_FIRMWARE_VOLUME2_PROTOCOL         *FwVol;
  UINTN                                 Index;
  UINTN                                 HandlesCounter;
  BOOLEAN                               ImageFound;
  VOID                                  *Table;
  UINTN                                 TableSize;
  UINTN                                 TablePtr;
  UINT16                                MaxOemInt15FuListSize;
  UINT16                                CpuRegistersSize;
  UINT8                                 *Ptr;
  EFI_COMPATIBILITY16_TABLE             *EfiTable;
  INT15_PRIVATE_DATA                    *Int15PrivateData;
  EFI_HANDLE                            Handle;
  UINT32                                FileAuthenticationStatus;

  Table      = NULL;
  TableSize  = 0;
  EfiTable   = NULL;
  ImageFound = FALSE;
  //
  // Get Int15 OpRom image
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandlesCounter,
                  &FvHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandlesCounter; Index++) {
    Status = gBS->HandleProtocol (
                    *(FvHandles + Index),
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    Status = FwVol->ReadSection (
                      FwVol,
                      PcdGetPtr (PcdInt15ServiceSmmRomFile),
                      EFI_SECTION_RAW,
                      0,
                      &Table,
                      &TableSize,
                      &FileAuthenticationStatus
                      );
    if (!EFI_ERROR (Status)) {
      ImageFound = TRUE;
      break;
    }
  }

  //
  // Locate LegacyBios Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Locate LegacyRegion Protocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiLegacyRegion2ProtocolGuid,
                  NULL,
                  (VOID **)&LegacyRegion
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get EFI table from F segment
  //
  for (Ptr = (UINT8 *)((UINTN)0xFE000); Ptr < (UINT8 *) ((UINTN) 0x100000); Ptr += 0x10) {
    if (*(UINT32 *) Ptr == SIGNATURE_32 ('I', 'F', 'E', '$')) {
      EfiTable = (EFI_COMPATIBILITY16_TABLE *) Ptr;
     break;
    }
  }

  if (EfiTable == NULL) {
    DEBUG ((EFI_D_ERROR, "No Legacy16 table found\n"));
    return EFI_NOT_FOUND;
  }

  Status = LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);
  //
  // 16 bytes boundary alignment
  //
  MaxOemInt15FuListSize = (MAX_OEM_INT15_FN_COUNT * 2) + ((0x10 - ((MAX_OEM_INT15_FN_COUNT * 2) % 0x10)) & ~0x10);

  CpuRegistersSize = sizeof (INT15_PRIVATE_INFO) + ((0x10 - (sizeof (INT15_PRIVATE_INFO) % 0x10)) & ~0x10);

  //
  // Allocate shadow ram for INT15 OpRom and private data(Function list / Cpu registers)
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.BX = 0x02;//E0000Region;
  Regs.X.CX = (UINT16) TableSize + MaxOemInt15FuListSize + CpuRegistersSize;
  Regs.X.DX = 1;
  Status = LegacyBios->FarCall86 (
                         LegacyBios,
                         EfiTable->Compatibility16CallSegment,
                         EfiTable->Compatibility16CallOffset,
                         &Regs,
                         NULL,
                         0
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TablePtr = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);

  //
  // Initialize Int15PrivateData
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (INT15_PRIVATE_DATA),
                  (VOID **)&Int15PrivateData
                  );

  ZeroMem (Int15PrivateData, sizeof (INT15_PRIVATE_DATA));

  Int15PrivateData->FunctionListSegment = (UINT16)(TablePtr >> 4);

  Int15PrivateData->CpuRegistersSegment =  Int15PrivateData->FunctionListSegment + (MaxOemInt15FuListSize >> 4);

  //
  // Initialize the OEM Int15 function list and CPU registers
  //
  ZeroMem ((VOID *)TablePtr, MaxOemInt15FuListSize + CpuRegistersSize);

  TablePtr = TablePtr + MaxOemInt15FuListSize + CpuRegistersSize;

  //
  // To copy the Int15 image to shadow memory
  //
  CopyMem((UINT8 *)TablePtr, (UINT8 *)Table, TableSize);

  //
  // call into our BIN file for change the INT15
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = COMMON_INT15_SMI;
  Regs.X.CX = MAX_OEM_INT15_FN_COUNT;
  Regs.X.DX = SW_SMI_PORT;
  Regs.X.DS = Int15PrivateData->FunctionListSegment;
  Regs.X.ES = Int15PrivateData->CpuRegistersSegment;
  Status = LegacyBios->FarCall86 (
                         LegacyBios,
                         (UINT16)(TablePtr >> 4),
                         0x03,
                         &Regs,
                         NULL,
                         0
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);

  //
  // Install gInt15PrivateDataGuid for Smm Int15ServiceInstance
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gInt15PrivateDataGuid,
                  EFI_NATIVE_INTERFACE,
                  Int15PrivateData
                  );
  return EFI_SUCCESS;
}

/**
  Install Int15 function interface

  @param[in]  This              Point to EFI_INT15_SERVICE_PROTOCOL.
  @param[in]  FunctionNum       Oem Int15 function number
  @param[in]  FunctionPtr       Oem Int15 function point
  @param[in]  Context           Oem Int15 Context point

  @retval EFI_SUCCESS           The call returned successfully.
  @retval EFI_ALREADY_STARTED   The same as INT15 function number
  @retval EFI_INVALID_PARAMETER Invalid parameter
  @retval EFI_OUT_OF_RESOURCES  The Int15 function count is greater than MAX_OEM_INT15_FN_COUNT

**/
EFI_STATUS
EFIAPI
InstallInt15ProtocolInterface (
  IN EFI_SMM_INT15_SERVICE_PROTOCOL   *This,
  IN UINT16                           FunctionNum,
  IN EFI_OEM_INT15_CALLBACK_FUNCTION  FunctionPtr,
  IN VOID                             *Context  OPTIONAL
  )
{
  EFI_STATUS                Status;
  INT15_DATA_PACKET         *Int15DataPacket;
  INT15_DATA_PACKET         *Int15DataPacketDummy;
  LIST_ENTRY                *Link;
  UINT16                    *FunctionListAddress;

  if (FunctionNum == 0 || FunctionPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mInt15Count >= MAX_OEM_INT15_FN_COUNT) {
    return EFI_OUT_OF_RESOURCES;
  }

  Int15DataPacketDummy = mInt15ServiceInstance->Int15DataPacketDummy;

  //
  // Check the same as INT15 function number
  //
  for (Link = Int15DataPacketDummy->FunctionLinkList.ForwardLink;
       Link != &Int15DataPacketDummy->FunctionLinkList;
       Link = Link->ForwardLink) {

    Int15DataPacket = CR(Link, INT15_DATA_PACKET, FunctionLinkList, INT15_DATA_SIGNATURE);

    if (Int15DataPacket->FunctionNum == FunctionNum) {
      //
      // Already install the Int15 function
      //
      return EFI_ALREADY_STARTED;
    }
  }

  //
  // allocate new Int15 Data Packet
  //
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (INT15_DATA_PACKET),
                    (VOID **)&Int15DataPacket
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Int15DataPacket, sizeof (INT15_DATA_PACKET));
  //
  // Initialize new INT15 data structure
  //
  Int15DataPacket->Signature = INT15_DATA_SIGNATURE;
  InsertTailList (&Int15DataPacketDummy->FunctionLinkList, &Int15DataPacket->FunctionLinkList);

  Int15DataPacket->FunctionNum = FunctionNum;
  Int15DataPacket->FunctionPtr = (UINTN)FunctionPtr;
  if (Context != NULL) Int15DataPacket->ContextPtr = (UINTN)Context;

  //
  // Save INT15 function number for INT15 OpRom
  //
  FunctionListAddress = (UINT16 *)(UINTN)((mInt15ServiceInstance->FunctionListSegment << 4) + mInt15Count * 2);
  *FunctionListAddress = FunctionNum;

  mInt15Count++;

  return EFI_SUCCESS;
}

/**
  Re-Install Int15 function interface

  @param[in]  This              Point to EFI_INT15_SERVICE_PROTOCOL.
  @param[in]  FunctionNum       Oem Int15 function number
  @param[in]  FunctionPtr       Oem Int15 function point
  @param[in]  Context           Oem Int15 Context point

  @retval EFI_SUCCESS           The call returned successfully.
  @retval EFI_NOT_FOUND         Can't find the Int15 function number
  @retval EFI_INVALID_PARAMETER Invalid parameter

**/
EFI_STATUS
EFIAPI
ReinstallInt15ProtocolInterface (
  IN EFI_SMM_INT15_SERVICE_PROTOCOL   *This,
  IN UINT16                           FunctionNum,
  IN EFI_OEM_INT15_CALLBACK_FUNCTION  FunctionPtr,
  IN VOID                             *Context  OPTIONAL
  )
{
  INT15_DATA_PACKET         *Int15DataPacket;
  LIST_ENTRY                *Link;
  INT15_DATA_PACKET         *Int15DataPacketDummy;

  if (FunctionNum == 0 || FunctionPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Int15DataPacketDummy = mInt15ServiceInstance->Int15DataPacketDummy;

  //
  // Reinstall INT15 function
  //
  for (Link = Int15DataPacketDummy->FunctionLinkList.ForwardLink;
       Link != &Int15DataPacketDummy->FunctionLinkList;
       Link = Link->ForwardLink) {

    Int15DataPacket = CR(Link, INT15_DATA_PACKET, FunctionLinkList, INT15_DATA_SIGNATURE);

    if (Int15DataPacket->FunctionNum == FunctionNum) {
      //
      // Already install the Int15 function
      //
      Int15DataPacket->FunctionPtr = (UINTN)FunctionPtr;
      if (Context != NULL) Int15DataPacket->ContextPtr = (UINTN)Context;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/**
  INT15 Smm Callback of EFI_INT15_SERVICE_PROTOCOL

  @param[in]  CpuRegisters      Point to Cpu registers.

**/
STATIC
VOID
SmmInt15CallBack (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegisters
  )
{
  LIST_ENTRY                            *Link;
  INT15_DATA_PACKET                     *Int15DataPacket;
  INT15_DATA_PACKET                     *Int15DataPacketDummy;
  EFI_OEM_INT15_CALLBACK                *OemInt15Callback;
  VOID                                  *OemContext;

  OemInt15Callback     = NULL;
  OemContext           = NULL;
  Int15DataPacketDummy = mInt15ServiceInstance->Int15DataPacketDummy;

  //
  // Search the INT15 function number
  //
  for (Link = Int15DataPacketDummy->FunctionLinkList.ForwardLink;
       Link != &Int15DataPacketDummy->FunctionLinkList;
       Link = Link->ForwardLink) {

    Int15DataPacket = CR(Link, INT15_DATA_PACKET, FunctionLinkList, INT15_DATA_SIGNATURE);

    if (Int15DataPacket->FunctionNum == CpuRegisters->X.AX) {
      //
      // Find the INT15 function
      //
      OemInt15Callback = mInt15ServiceInstance->OemInt15Callback;
      OemInt15Callback->Funcs = (EFI_OEM_INT15_CALLBACK_FUNCTION)(UINTN)Int15DataPacket->FunctionPtr;
      if (Int15DataPacket->ContextPtr) {
        OemContext = (VOID *)(UINTN)Int15DataPacket->ContextPtr;
      }
      //
      // Oem Int15 callback function
      //
      OemInt15Callback->Funcs (CpuRegisters, OemContext);
      break;
    }
  }
  return;
}

/**
  INT15 SW SMI callback

  @param      Handle            The unique handle assigned to this handler by SmiHandlerRegister().
  @param      Context           Points to an optional handler context which was specified when the
                                handler was registered.
  @param      CommBuffer        A pointer to a collection of data in memory that will
                                be conveyed from a non-SMM environment into an SMM environment.
  @param      CommBufferSize    The size of the CommBuffer.

  @retval     EFI_SUCCESS       The callabck was handled successfully.
  @retval     EFI_NOT_FOUND     Cannot determine the source of SMI.

**/
STATIC
EFI_STATUS
EFIAPI
INT15SwSmiCallback (
  IN     EFI_HANDLE                    Handle,
  IN     CONST VOID                    *Context,
  IN OUT VOID                          *CommBuffer,
  IN OUT UINTN                         *CommBufferSize
  )
{
  EFI_IA32_REGISTER_SET         *CpuRegisters;
  EFI_SMM_CPU_PROTOCOL          *SmmCpu;
  UINTN                         Index;
  EFI_STATUS                    Status;
  UINT32                        Eax;
  UINT32                        Edx;

  CpuRegisters = NULL;
  SmmCpu       = NULL;
  Eax          = 0;
  Edx          = 0;

  if (mInt15ServiceInstance->CpuRegistersSegment == 0) {
    Status = InstallSmmInt15ServiceProtocol ();
    return Status;
  }
  //
  // Locate Smm Cpu protocol for Cpu save state manipulation
  //
  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **)&SmmCpu
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find out which CPU triggered the S/W SMI
  //
  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    SmmCpu->ReadSaveState (
              SmmCpu,
              sizeof (UINT32),
              EFI_SMM_SAVE_STATE_REGISTER_RAX,
              Index,
              &Eax
              );

    SmmCpu->ReadSaveState (
              SmmCpu,
              sizeof (UINT32),
              EFI_SMM_SAVE_STATE_REGISTER_RDX,
              Index,
              &Edx
              );

    if (((Eax & 0xff) == COMMON_INT15_SMI) && ((Edx & 0xffff) == SW_SMI_PORT)) {
      //
      // CPU found!
      //
      break;
    }
  }
  if (Index == mSmst->NumberOfCpus) {
    //
    // Error out due to CPU not found
    //
    return EFI_NOT_FOUND;
  }

  //
  // Get Cpu registers of caller
  //
  CpuRegisters = (EFI_IA32_REGISTER_SET*)((UINTN)(mInt15ServiceInstance->CpuRegistersSegment << 4));

  //
  // Call Oem Int15 callback function
  //
  SmmInt15CallBack (CpuRegisters);

  return EFI_SUCCESS;
}
