/** @file

  PFAT Driver implements the PFAT Host Controller Compatibility Interface.                                 

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include <CpuAccess.h>
#include <PfatServicesSmm.h>

///
/// Global variables
///
//[-start-130121-IB05160402-modify]//
EFI_SMM_SYSTEM_TABLE2             *mSmst;
//[-end-130121-IB05160402-modify]//
PFAT_PROTOCOL                     *mPfatProtocol;
PFAT_HOB                          *mPfatHobPtr;
EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL *mPchIoTrap;

/**
  This function is triggered by the BIOS update tool with an IO trap. It executres
  Pfat protocol execute with the true flag indicating that there is an update package
  in the DPR region of memory.

  @param[in] DispatchHandle      Not used
  @param[in] CallbackContext     Not used
**/
VOID
EFIAPI
PfatUpdateBios (
  IN EFI_HANDLE                                DispatchHandle,
  IN EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT *CallbackContext
  )
{
  ///
  /// Invoke PFAT Services for updating BIOS
  ///
  mPfatProtocol->Execute(mPfatProtocol, TRUE);
  CopyMem (mPfatUpdatePackagePtr, &mPfatFullStatus, sizeof (UINT64));

  return;
}

/**
  This method registers and sets up the IOTRAP and NVS area for the PFAT tools interface

  @param[in] DispatchHandle      Not used
  @param[in] CallbackContext     Not used
**/
VOID
EFIAPI
PfatToolsInterfaceInit (
  IN EFI_HANDLE                                DispatchHandle,
  IN EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT *CallbackContext
  )
{
  EFI_STATUS                                Status;
  EFI_GUID                                  EfiGlobalNvsAreaProtocolGuid = EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID;
  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA                       *GlobalNvsArea;
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT PchIoTrapContext;

  ///
  /// Locate Global NVS and update PFAT DPR size & Memory address for ACPI tables
  ///
  Status = gBS->LocateProtocol (&EfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  GlobalNvsArea                     = GlobalNvsAreaProtocol->Area;
  GlobalNvsArea->PfatMemAddress     = 0;
  GlobalNvsArea->PfatMemSize        = 0;
  GlobalNvsArea->PfatIoTrapAddress  = 0;
  ///
  /// Locate PFAT SMM Protocol
  ///
//[-start-130121-IB05160402-modify]//
  Status = mSmst->SmmLocateProtocol (
                    &gSmmPfatProtocolGuid,
                    NULL,
                    (VOID **)&mPfatProtocol
                    );
//[-end-130121-IB05160402-modify]//
  ASSERT_EFI_ERROR (Status);
  if (mPfatProtocol != NULL) {
    ///
    /// Register PFAT IO TRAP handler
    ///
    PchIoTrapContext.Type         = ReadWriteTrap;
    PchIoTrapContext.Length       = 4;
    PchIoTrapContext.Address      = 0;
    PchIoTrapContext.Context      = NULL;
    PchIoTrapContext.MergeDisable = FALSE;
    Status = mPchIoTrap->Register (
                    mPchIoTrap,
                    PfatUpdateBios,
                    &PchIoTrapContext,
                    &PchIoTrapHandle
                    );
    ASSERT_EFI_ERROR (Status);
    ///
    /// Initialize ASL manipulation library
    ///
    InitializeAslUpdateLib ();
    Status = UpdateAslCode (
               (SIGNATURE_32 ('I', 'O', '_', 'P')),
               PchIoTrapContext.Address,
               (UINT8) PchIoTrapContext.Length
             );
    ASSERT_EFI_ERROR (Status);
    GlobalNvsArea->PfatMemAddress     = mPfatMemAddress;
    GlobalNvsArea->PfatMemSize        = (UINT8) RShiftU64(mPfatMemSize, 20);
    GlobalNvsArea->PfatIoTrapAddress  = PchIoTrapContext.Address;
  }
}

/**
  Entry point for the PFAT protocol driver.

  @param[in] ImageHandle        Image handle of this driver.
  @param[in] SystemTable        Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_ERROR             Driver exits abnormally.
**/
EFI_STATUS
EFIAPI
InstallPfatProtocol (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                                Status;
//[-start-130121-IB05160402-modify]//
  EFI_SMM_BASE2_PROTOCOL                    *SmmBase;
//[-end-130121-IB05160402-modify]//
  PFAT_INSTANCE                             *PfatInstance;
  VOID                                      *PfatProtocolAddr;
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT PchIoTrapContext;
  EFI_GUID                                  EfiSmmIoTrapDispatchProtocolGuid = EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL_GUID;

  ///
  /// Locate SMM Base Protocol
  ///
//[-start-130121-IB05160402-modify]//
  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **) &SmmBase);
//[-end-130121-IB05160402-modify]//
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize our module variables
  ///
  Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate pool for PFAT protocol instance
  ///
  Status = mSmst->SmmAllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (PFAT_INSTANCE),
                  (VOID **) &PfatInstance
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (PfatInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) PfatInstance, sizeof (PFAT_INSTANCE));
  PfatInstance->Handle = NULL;
  PfatProtocolAddr     = NULL;

  if ((AsmReadMsr64 (MSR_PLATFORM_INFO)) & B_MSR_PLATFORM_INFO_PFAT_AVAIL) {
    if ((AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL)) & B_MSR_PLAT_FRMW_PROT_CTRL_EN) {
      ///
      /// Initialize the PFAT protocol instance
      ///
      Status = PfatProtocolConstructor (PfatInstance);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      PfatProtocolAddr = &(PfatInstance->PfatProtocol);
      Status = gBS->LocateProtocol (&EfiSmmIoTrapDispatchProtocolGuid, NULL, (VOID **) &mPchIoTrap);
      ASSERT_EFI_ERROR (Status);

      PchIoTrapContext.Type         = ReadTrap;
      PchIoTrapContext.Length       = 4;
      PchIoTrapContext.Address      = 0;
      PchIoTrapContext.Context      = NULL;
      PchIoTrapContext.MergeDisable = FALSE;
      Status = mPchIoTrap->Register (
                      mPchIoTrap,
                      PfatToolsInterfaceInit,
                      &PchIoTrapContext,
                      &PchIoTrapHandle
                      );
      ASSERT_EFI_ERROR (Status);

      mPfatHobPtr->PfatToolsIntIoTrapAdd = PchIoTrapContext.Address;
    } else {
      DEBUG ((EFI_D_INFO, "PFAT Feature supported but disabled\n"));
    }
  } else {
    DEBUG ((EFI_D_WARN, "PFAT Feature is not supported\n"));
  }

  ///
  /// Install the SMM PFAT_PROTOCOL interface
  ///
//[-start-130121-IB05160402-modify]//
//  Status = gBS->InstallMultipleProtocolInterfaces (
//                  &(PfatInstance->Handle),
//                  &gSmmPfatProtocolGuid,
//                  PfatProtocolAddr,
//                  NULL
//                  );
  mSmst->SmmInstallProtocolInterface (
    &(PfatInstance->Handle),
    &gSmmPfatProtocolGuid,
    EFI_NATIVE_INTERFACE,
    PfatProtocolAddr
    );
//[-end-130121-IB05160402-modify]//
  if (EFI_ERROR (Status)) {
    mSmst->SmmFreePool (PfatInstance);
  }

  return Status;
}

/**
  Initialize PFAT protocol instance.

  @param[in] PfatInstance  Pointer to PfatInstance to initialize

  @retval EFI_SUCCESS      The protocol instance was properly initialized
  @retval EFI_NOT_FOUND    PFAT Binary module was not found.
**/
EFI_STATUS
PfatProtocolConstructor (
  PFAT_INSTANCE *PfatInstance
  )
{
  EFI_STATUS                   Status;
  PPDT                         *Ppdt;
  UINTN                        i;
  UINTN                        NumHandles;
  EFI_HANDLE                   *Buffer;
  UINTN                        Size;
  UINT32                       FvStatus;
  EFI_FV_FILETYPE              FileType;
  EFI_FIRMWARE_VOLUME_PROTOCOL *FwVol;
  EFI_FV_FILE_ATTRIBUTES       Attributes;
  UINTN                        NumPages;
  EFI_PHYSICAL_ADDRESS         Addr;
  EFI_PHYSICAL_ADDRESS         PfatModule;
  SA_DATA_HOB                  *SaDataHobPtr;
  EFI_GUID                     PfatModuleGuid  = PFAT_MODULE_GUID;
  EFI_GUID                     PfatHobGuid     = PFAT_HOB_GUID;
  EFI_GUID                     SaDataHobGuid   = SA_DATA_HOB_GUID;
//[-start-130121-IB05160402-add]//
  INTN                         Instance = 0;
//[-end-130121-IB05160402-add]//

  FwVol           = NULL;
  Size            = 0;
  FvStatus        = 0;
  NumPages        = 0;
  Addr            = 0;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumHandles,
                  &Buffer
                  );
  ASSERT_EFI_ERROR (Status);

  for (i = 0; i < NumHandles; i++) {
    Status = gBS->HandleProtocol (
                    Buffer[i],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// Locate PFAT Binary.
    ///
    Status = FwVol->ReadFile (
                    FwVol,
                    &PfatModuleGuid,
                    NULL,
                    &Size,
                    &FileType,
                    &Attributes,
                    &FvStatus
                    );
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  FreePool (Buffer);
  ASSERT (Size);
  if ((FwVol == NULL) || (Size == 0)) {
    return EFI_NOT_FOUND;
  }
  NumPages = PFAT_MEMORY_PAGES + ALIGNMENT_IN_PAGES;

  ///
  /// Allocate memory buffer for PFAT Module
  ///
  Status = (mSmst->SmmAllocatePages)(AllocateAnyPages, EfiRuntimeServicesData, NumPages, &Addr);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Align address to 256K.
  ///
  PfatModule  = Addr &~(ALIGN_256KB - 1);
  PfatModule  = PfatModule < Addr ? (PfatModule + ALIGN_256KB) : PfatModule;

  ///
  /// Read PFAT Module into prepared buffer.
  ///
//[-start-130121-IB05160402-modify]//
////  Status = FwVol->ReadFile (
////                  FwVol,
////                  &PfatModuleGuid,
////                  ((VOID **) &PfatModule),
////                  &Size,
////                  &FileType,
////                  &Attributes,
////                  &FvStatus
////                  );
    Status = FwVol->ReadSection (
                          FwVol,
                          &PfatModuleGuid,
                          EFI_SECTION_RAW,
                          Instance,
                          (VOID **) &PfatModule,
                          &Size,
                          &FvStatus
                          );
//[-end-130121-IB05160402-modify]//
  
  ASSERT (Size);
  if ((FwVol == NULL) || (Size == 0)) {
    return EFI_NOT_FOUND;
  }

  mPfatHobPtr = GetFirstGuidHob (&PfatHobGuid);
  if (mPfatHobPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "PFAT HOB not available\n"));
    return EFI_NOT_FOUND;
  }

  SaDataHobPtr = GetFirstGuidHob (&SaDataHobGuid);
  if (SaDataHobPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "SA Data HOB not available\n"));
    return EFI_NOT_FOUND;
  }

  ///
  /// Allocate pool for PPDT Data
  ///
  Status = mSmst->SmmAllocatePool (
                  EfiRuntimeServicesData,
                  mPfatHobPtr->Ppdt.PpdtSize,
                  (VOID **) &Ppdt
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Ppdt, &mPfatHobPtr->Ppdt, mPfatHobPtr->Ppdt.PpdtSize);

  mPfatMemAddress        = SaDataHobPtr->DprDirectory[EnumDprDirectoryPfat].PhysBase;
  mPfatMemSize           = (UINT32) LShiftU64(SaDataHobPtr->DprDirectory[EnumDprDirectoryPfat].Size, 20);
  mPfatUpdatePackagePtr  = (PUP *)(UINTN)mPfatMemAddress;
  mPupCertificate        = (EFI_PHYSICAL_ADDRESS) (mPfatMemAddress + mPfatMemSize - PUPC_MEMORY_OFFSET);
  mPfatLogPtr            = (PFAT_LOG *)(UINTN)(mPfatMemAddress + mPfatMemSize - PFAT_LOG_MEMORY_OFFSET);

  CopyMem (&mPfatLogTemp, &mPfatHobPtr->PfatLog, sizeof (PFAT_LOG));
  CopyMem (&mPfatUpdatePackagePtr->PupHeader, &mPfatHobPtr->PupHeader, sizeof (PUP_HEADER));
  ZeroMem (mPfatUpdatePackagePtr->PupBuffer, PUP_BUFFER_SIZE);
  mPfatUpdatePackagePtr->PupHeader.ScriptSectionSize  = 0;
  mPfatUpdatePackagePtr->PupHeader.DataSectionSize    = 0;
  mPfatUpdateCounter = 0;

  ///
  /// Set Begin command
  ///
  mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] = PFAT_COMMAND_BEGIN;

  ///
  /// Initialize the PFAT protocol instance
  ///
  PfatInstance->Signature             = PFAT_SIGNATURE;
  PfatInstance->PfatProtocol.Write    = PfatProtocolWrite;
  PfatInstance->PfatProtocol.Erase    = PfatProtocolBlockErase;
  PfatInstance->PfatProtocol.Execute  = PfatProtocolExecute;
  PfatInstance->AddrMask              = 0;
  for (i = 0; i < mPfatHobPtr->NumSpiComponents; i++) {
    PfatInstance->AddrMask += (SPI_SIZE_BASE_512KB << mPfatHobPtr->ComponentSize[i]);
  }
  PfatInstance->AddrMask -= 1;

  PfatInstance->PfatDirectory[EnumPfatModule]        = PfatModule;
  PfatInstance->PfatDirectory[EnumPfatModule]       |= LShiftU64 (PFAT_DIRECTORY_PFAT_MODULE_ENTRY, 56);
  PfatInstance->PfatDirectory[EnumPpdt]              = (EFI_PHYSICAL_ADDRESS) Ppdt;
  PfatInstance->PfatDirectory[EnumPpdt]             |= LShiftU64 (PFAT_DIRECTORY_PPDT_ENTRY, 56);
  PfatInstance->PfatDirectory[EnumPup]               = (EFI_PHYSICAL_ADDRESS) mPfatUpdatePackagePtr;
  PfatInstance->PfatDirectory[EnumPup]              |= LShiftU64(PFAT_DIRECTORY_PUP_ENTRY, 56);
  PfatInstance->PfatDirectory[EnumPupCertificate]    = 0;
  PfatInstance->PfatDirectory[EnumPupCertificate]   |= LShiftU64(PFAT_DIRECTORY_UNDEFINED_ENTRY, 56);
  PfatInstance->PfatDirectory[EnumPfatLog]           = 0;
  PfatInstance->PfatDirectory[EnumPfatLog]          |= LShiftU64(PFAT_DIRECTORY_UNDEFINED_ENTRY, 56);
  PfatInstance->PfatDirectory[EnumPfatDirectoryEnd]  = 0;
  PfatInstance->PfatDirectory[EnumPfatDirectoryEnd] |= LShiftU64 (PFAT_DIRECTORY_END_MARKER, 56);

  return EFI_SUCCESS;
}

/**
  Set MSR 0x115 with PFAT DIRECTORY Address.
  Trigger MSR 0x116 to invoke PFAT Binary.
  Read MSR 0x115 to get PFAT Binary Status.

  @param[in] PfatInstance              Pointer to PfatInstance to initialize
**/
VOID
EFIAPI
PfatModuleExecute (
  IN VOID          *PfatInstancePtr
  )
{
  PFAT_INSTANCE *PfatInstance;
  PfatInstance = PfatInstancePtr;

  AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_TRIG_PARAM, (UINT64) PfatInstance->PfatDirectory);
  AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_TRIGGER, 0);
  ///
  /// Read MSR_PLAT_FRMW_PROT_TRIG_PARAM to get PFAT Binary status
  ///
  PfatInstance->MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_TRIG_PARAM);
  return;
}

/**
  This service will write PFAT_DIRECTORY MSR and invoke the PFAT Module by writing to PLAT_FRMW_PROT_TRIGGER MSR for writing/erasing to flash.
  BIOS should invoke PFAT_PROTOCOL.Write() or PFAT_PROTOCOL.Erase() function prior to calling PFAT_PROTOCOL.Execute() for flash writes/erases (except for BiosUpdate).
  Write()/Erase() function will render PFAT script during execution.
  Execute() function will implement the following steps:
  1. Update PFAT directory with address of PUP.
  2. All the AP's except the master thread are put to sleep.
  3. PFAT module is invoked from BSP to execute desired operation.
  If BiosUpdate flag is set to true, PUP (PUP Header + PFAT Script + Update data) is part of data that is passed to SMI Handler. SMI Handler invokes PFAT module to process the update.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This                Pointer to the PFAT_PROTOCOL instance.
  @param[in] BiosUpdate          Flag to indicate flash update is requested by the Tool

  @retval EFI_SUCCESS            Successfully completed flash operation.
  @retval EFI_INVALID_PARAMETER  The parameters specified are not valid.
  @retval EFI_UNSUPPORTED        The CPU or SPI memory is not supported.
  @retval EFI_DEVICE_ERROR       Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
PfatProtocolExecute (
  IN PFAT_PROTOCOL *This,
  IN BOOLEAN       BiosUpdate
  )
{
  EFI_STATUS    Status;
  PFAT_INSTANCE *PfatInstance;
  UINT16        PfatStatus;
  UINT16        PfatAdditionalData;
  UINT16        PfatTerminalLine;
  UINT8         PfatSE;
  UINTN         Index;
  UINT8         RetryIteration;

  DEBUG ((EFI_D_INFO, "PfatProtocolExecute\n"));

  PfatInstance = PFAT_INSTANCE_FROM_PFATPROTOCOL (This);

  ///
  /// If Update Package has been created during runtime then complete the script
  /// and create update Pkg
  ///
  if (BiosUpdate == FALSE) {
    ///
    /// End command
    ///
    mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++]  = PFAT_COMMAND_END;
    mPfatUpdatePackagePtr->PupHeader.ScriptSectionSize      = (mPfatUpdateCounter * 8);
    CopyMem (
            &mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter],
            &mPfatUpdateData,
            (mPfatUpdatePackagePtr->PupHeader.DataSectionSize)
            );
  } else {
    CopyMem (mPfatLogPtr, &mPfatLogTemp, sizeof (PFAT_LOG));
    if (mPfatUpdatePackagePtr->PupHeader.PkgAttributes) {
      PfatInstance->PfatDirectory[EnumPupCertificate]   = mPupCertificate;
      PfatInstance->PfatDirectory[EnumPupCertificate]  |= LShiftU64(PFAT_DIRECTORY_PUP_CERTIFICATE_ENTRY, 56);
      PfatInstance->PfatDirectory[EnumPfatLog]          = (EFI_PHYSICAL_ADDRESS) mPfatLogPtr;
      PfatInstance->PfatDirectory[EnumPfatLog]         |= LShiftU64(PFAT_DIRECTORY_PFAT_LOG_ENTRY, 56);
    } else {
      PfatInstance->PfatDirectory[EnumPupCertificate]   = 0;
      PfatInstance->PfatDirectory[EnumPupCertificate]  |= LShiftU64(PFAT_DIRECTORY_UNDEFINED_ENTRY, 56);
      PfatInstance->PfatDirectory[EnumPfatLog]          = (EFI_PHYSICAL_ADDRESS) mPfatLogPtr;
      PfatInstance->PfatDirectory[EnumPfatLog]         |= LShiftU64(PFAT_DIRECTORY_PFAT_LOG_ENTRY, 56);
    }
  }

  PfatInstance->MsrValue = ERR_LAUNCH_FAIL;
  for (Index = 1; Index < mSmst->NumberOfCpus; Index++) {
    Status = EFI_NOT_READY;
    for (RetryIteration = 0; (RetryIteration < PFAT_AP_SAFE_RETRY_LIMIT) && (Status != EFI_SUCCESS); RetryIteration++) {
      Status = mSmst->SmmStartupThisAp (PfatModuleExecute, Index, (VOID *) PfatInstance);
      if (Status != EFI_SUCCESS) {
        ///
        /// SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
        ///
        PchPmTimerStall (PFAT_WAIT_PERIOD);
      }
    }
  }

  PfatModuleExecute ((VOID *) PfatInstance);

  if (BiosUpdate == FALSE) {
    PfatStatus = (UINT16) RShiftU64 (
                    (PfatInstance->MsrValue & LShiftU64 (V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_MASK,
                                                         N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_OFFSET)),
                    N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_OFFSET
                    );
    switch (PfatStatus) {
      case ERR_OK:
        Status = EFI_SUCCESS;
        break;

      case ERR_RANGE_VIOLATION:
      case ERR_SFAM_VIOLATION:
      case ERR_EXEC_LIMIT:
      case ERR_INTERNAL_ERROR:
        Status = EFI_DEVICE_ERROR;
        break;

      case ERR_UNSUPPORTED_CPU:
      case ERR_UNDEFINED_FLASH_OBJECT:
      case ERR_LAUNCH_FAIL:
        Status = EFI_UNSUPPORTED;
        break;

      default:

      case ERR_BAD_DIRECTORY:
      case ERR_BAD_PPDT:
      case ERR_BAD_PUP:
      case ERR_SCRIPT_SYNTAX:
      case ERR_INVALID_LINE:
      case ERR_BAD_PUPC:
      case ERR_BAD_SVN:
      case ERR_UNEXPECTED_OPCODE:
      case ERR_OVERFLOW:
        Status = EFI_INVALID_PARAMETER;
        break;
    }

    if (EFI_ERROR (Status)) {
      PfatAdditionalData = (UINT16) RShiftU64 (
                      (PfatInstance->MsrValue & LShiftU64 (V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_MASK,
                                                           N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_OFFSET)),
                      N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_OFFSET
                      );
      PfatTerminalLine = (UINT16) RShiftU64 (
                      (PfatInstance->MsrValue & LShiftU64 (V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_MASK,
                                                           N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_OFFSET)),
                      N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_OFFSET
                      );
      PfatSE = (UINT8) RShiftU64 (
                      (PfatInstance->MsrValue & B_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE),
                      N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE_OFFSET
                      );
      DEBUG ((EFI_D_ERROR, "PFAT Status          = 0x%X\n", PfatStatus));
      DEBUG ((EFI_D_ERROR, "PFAT Additional Data = 0x%X\n", PfatAdditionalData));
      DEBUG ((EFI_D_ERROR, "PFAT Terminal Line   = 0x%X\n", PfatTerminalLine));
      DEBUG ((EFI_D_ERROR, "PFAT SE              = 0x%X\n", PfatSE));
    }

    ZeroMem (mPfatUpdatePackagePtr->PupBuffer, PUP_BUFFER_SIZE);
    mPfatUpdatePackagePtr->PupHeader.ScriptSectionSize  = 0;
    mPfatUpdatePackagePtr->PupHeader.DataSectionSize    = 0;
    mPfatUpdateCounter = 0;

    ///
    /// Begin command
    ///
    mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] = PFAT_COMMAND_BEGIN;
  } else {
    mPfatFullStatus = PfatInstance->MsrValue;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  This service fills PFAT script buffer for flash writes.
  BIOS should invoke this function prior to calling PFAT_PROTOCOL.Execute() with all the relevant data required for flash write.
  This function will not invoke PFAT Module, only create script required for writing to flash.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This           Pointer to the PFAT_PROTOCOL instance.
  @param[in] Address        This value specifies the offset from the start of the SPI Flash component where BIOS Image is located.
  @param[in] DataByteCount  Number of bytes in the data portion.
  @param[in] Buffer         Pointer to caller-allocated buffer containing the dada sent.
**/
VOID
EFIAPI
PfatProtocolWrite (
  IN PFAT_PROTOCOL *This,
  IN UINTN         Address,
  IN UINT32        DataByteCount,
  IN OUT UINT8     *Buffer
  )
{
  PFAT_INSTANCE *PfatInstance;

  PfatInstance = PFAT_INSTANCE_FROM_PFATPROTOCOL (This);

  ///
  /// Set Buffer Offset Index immediate command
  ///
  mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] =
          (LShiftU64 ((UINTN) mPfatUpdatePackagePtr->PupHeader.DataSectionSize, 32)) |
          (LShiftU64 (PFAT_B0_INDEX, 16)) |
          PFAT_COMMAND_SET_BUFFER_INDEX;
  ///
  /// Set Flash Index immediate command
  ///
  mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] = (LShiftU64 ((Address & PfatInstance->AddrMask), 32)) | (LShiftU64 (PFAT_F0_INDEX, 16)) | PFAT_COMMAND_SET_FLASH_INDEX;
  ///
  /// Write to Flash Index from Buffer Offset Index with specific Size command
  ///
  mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] = (LShiftU64 (DataByteCount, 32)) | (LShiftU64 (PFAT_B0_INDEX, 24)) | (LShiftU64 (PFAT_F0_INDEX, 16)) | PFAT_COMMAND_WRITE_IMM;

  CopyMem (&mPfatUpdateData[mPfatUpdatePackagePtr->PupHeader.DataSectionSize], Buffer, DataByteCount);

  mPfatUpdatePackagePtr->PupHeader.DataSectionSize += DataByteCount;

  return;
}

/**
  This service fills PFAT script buffer for erasing blocks in flash.
  BIOS should invoke this function prior to calling PFAT_PROTOCOL.Execute() with all the relevant data required for flash erase.
  This function will not invoke PFAT module, only create script required for erasing each block in the flash.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This     Pointer to the PFAT_PROTOCOL instance.
  @param[in] Address  This value specifies the offset from the start of the SPI Flash component where BIOS Image is located.
**/
VOID
EFIAPI
PfatProtocolBlockErase (
  IN PFAT_PROTOCOL *This,
  IN UINTN         Address
  )
{
  PFAT_INSTANCE *PfatInstance;

  PfatInstance = PFAT_INSTANCE_FROM_PFATPROTOCOL (This);

  ///
  /// Set Flash Index immediate command
  ///
  mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] = (LShiftU64 ((Address & PfatInstance->AddrMask), 32)) | (LShiftU64 (PFAT_F0_INDEX, 16)) | PFAT_COMMAND_SET_FLASH_INDEX;
  ///
  /// Erase Flash Index command
  ///
  mPfatUpdatePackagePtr->PupBuffer[mPfatUpdateCounter++] = (LShiftU64 (PFAT_F0_INDEX, 16)) | PFAT_COMMAND_ERASE_BLK;

  mPfatUpdatePackagePtr->PupHeader.DataSectionSize += 0;

  return;
}
