/** @file
  Runtime DXE driver implementation for the IRSI Registration

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/VariableWrite.h>
#include <Guid/EventGroup.h>
#include <Guid/ImageAuthentication.h>
#include "IrsiRegistrationRuntimeDxe.h"
#include "Crc32.h"


IRSI_REGISTRATION_CONTEXT    mIrsiReg;
EFI_SYSTEM_TABLE             *mSystemTable;

/**
  Helper function for adding pointer to virtual conversion list

  @param  Pointer               pointer to be converted

  @retval EFI_SUCCESS           function returned successfully
  @retval EFI_ABORTED           System is at OS runtime, function aborted
**/
EFI_STATUS
EFIAPI
IrsiAddVirtualPointer (
  IN  VOID **Pointer
  )
{
  IRSI_VIRTUAL_POINTER_ENTRY   *Node;
  LIST_ENTRY                   *Link;

  if (EfiAtRuntime ()) {
    return EFI_ABORTED;
  }

  Link = GetFirstNode (&mIrsiReg.VirtualPointerList);
  while (!IsNull (&mIrsiReg.VirtualPointerList, Link)) {
    Node = VIRTUAL_POINTER_ENTRY_FROM_LINK (Link);
    if (Node->Pointer == Pointer) {
      //
      // Pointer already in the conversion list
      //
      return EFI_SUCCESS;
    }
    Link = GetNextNode (&mIrsiReg.VirtualPointerList, Link);
  }

  Node = AllocateRuntimePool (sizeof (IRSI_VIRTUAL_POINTER_ENTRY));
  if (Node == NULL) {
    ASSERT (Node != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  Node->Signature = VIRTUAL_POINTER_SIGNATURE;
  Node->Pointer = Pointer;
  InsertTailList (&mIrsiReg.VirtualPointerList, &Node->Link);
  return EFI_SUCCESS;
}

/**
  Helper function for removing pointer from virtual conversion list

  @param[in] Pointer            pointer to be removed

  @retval EFI_SUCCESS           function returned successfully.
  @retval EFI_NOT_FOUND         pointer isn't in the conversion list.
  @retval EFI_ABORTED           System is at OS runtime, function aborted.
**/
EFI_STATUS
EFIAPI
IrsiRemoveVirtualPointer (
  IN  VOID **Pointer
  )
{

  IRSI_VIRTUAL_POINTER_ENTRY   *Node;
  LIST_ENTRY                   *Link;

  if (EfiAtRuntime ()) {
    return EFI_ABORTED;
  }

  Link = GetFirstNode (&mIrsiReg.VirtualPointerList);
  while (!IsNull (&mIrsiReg.VirtualPointerList, Link)) {
    Node = VIRTUAL_POINTER_ENTRY_FROM_LINK (Link);
    if (Node->Pointer == Pointer) {
      RemoveEntryList (&Node->Link);
      FreePool (Node);
      return EFI_SUCCESS;
    }
    Link = GetNextNode (&mIrsiReg.VirtualPointerList, Link);
  }

  return EFI_NOT_FOUND;
}

/**
  Get IRSI function from IRSI function list

  @param  Guid                  IRSI Service Type GUID
  @param  FunctionNumber        Command code of the IRSI function

  @return                       IRSI function pointer
**/
IRSI_FUNCTION
EFIAPI
IrsiGetFunction (
  IN  EFI_GUID       *Guid,
  IN  UINT32         FunctionNumber
  )
{
  LIST_ENTRY            *Link;
  IRSI_FUNCTION_ENTRY   *FunctionEntry;

  Link = GetFirstNode (&mIrsiReg.FunctionList);
  while (!IsNull (&mIrsiReg.FunctionList, Link)) {
    FunctionEntry = IRSI_FUNCTION_ENTRY_FROM_LINK (Link);
    if (CompareGuid (Guid, &FunctionEntry->Guid) && (FunctionNumber == FunctionEntry->FunctionNumber)) {
      return FunctionEntry->Function;
    }
    Link = GetNextNode (&mIrsiReg.FunctionList, Link);
  }
  return NULL;
}

/**
  Execute IRSI function

  @param  Guid                  IRSI Service Type GUID
  @param  FunctionNumber        Command code of IRSI function
  @param  CommBuf               Communication Buffer
  @param  BufSize               Buffer size

  @return                       IRSI Status
**/
EFI_STATUS
EFIAPI
IrsiExecuteFunction (
  IN  EFI_GUID       *Guid,
  IN  UINT32         FunctionNumber,
  IN  VOID           *CommBuf,
  IN  UINT32         BufSize
  )
{
  IRSI_FUNCTION        IrsiFunction;
  IRSI_HEADER          *IrsiHeader;
  EFI_STATUS           Status;
  UINT32               HeaderChecksum;

  IrsiFunction = IrsiGetFunction (Guid, FunctionNumber);
  if (IrsiFunction == NULL) {
    return EFI_NOT_FOUND;
  }
  IrsiHeader = (IRSI_HEADER *)CommBuf;

  //
  // Validate checksum of the input IRSI header
  //
  HeaderChecksum = IrsiHeader->HeaderChecksum;
  IrsiHeader->HeaderChecksum = 0;
  CalculateCrc32 ((VOID *)IrsiHeader, sizeof (IRSI_HEADER), &IrsiHeader->HeaderChecksum);
  if (IrsiHeader->HeaderChecksum != HeaderChecksum) {
    IrsiHeader->HeaderChecksum = HeaderChecksum;
    return EFI_INVALID_PARAMETER;
  }


  Status = IrsiFunction (CommBuf);

  IrsiHeader->HeaderChecksum = 0;
  CalculateCrc32 (IrsiHeader, sizeof (IRSI_HEADER), &IrsiHeader->HeaderChecksum);

  return Status;
}

/**
  Helper function for geting the pre-allocated IRSI runtime buffer

  @param  IrsiBuf                pointer to the IRSI runtime buffer
  @param  IrsiBufSize            pointer to the IRSI buffer size

  @retval EFI_SUCCESS            function returned successfully
  @retval EFI_INVALIDE_PARAMETER Invalid parameter list
**/
EFI_STATUS
EFIAPI
IrsiGetRuntimeBuffer (
  VOID  **IrsiBuf,
  UINTN *IrsiBufSize
  )
{
  if (IrsiBuf == NULL || IrsiBufSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *IrsiBufSize = mIrsiReg.IrsiRuntimeBufferSize;
  *IrsiBuf     = mIrsiReg.IrsiRuntimeBuffer;

  return EFI_SUCCESS;
}


/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
IrsiVirtualPointerConversionNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  LIST_ENTRY                   *Link;
  IRSI_VIRTUAL_POINTER_ENTRY  *PointerEntry;

  Link = GetFirstNode (&mIrsiReg.VirtualPointerList);
  while (!IsNull (&mIrsiReg.VirtualPointerList, Link)) {
    PointerEntry = VIRTUAL_POINTER_ENTRY_FROM_LINK(Link);
    EfiConvertPointer (0, (VOID **)PointerEntry->Pointer);
    Link = GetNextNode (&mIrsiReg.VirtualPointerList, Link);
  }

  EfiConvertList (0, &mIrsiReg.FunctionList);
  EfiConvertList (0, &mIrsiReg.VirtualPointerList);
}


/**
  IRSI Registration Protocol function for registering IRSI functions

  @param Guid                   Runtime Services Type GUID
  @param FunctionNumber         Command code of the IRSI function
  @param IrsiFunction           The IRSI function pointer

  @retval EFI_SUCCESS           IRSI function successfully registered
  @retval EFI_ABORTED           faile to register IRSI function
  @retval EFI_ALREADY_STARTED   IRSI function already registered
**/
EFI_STATUS
EFIAPI
IrsiRegisterFunction (
  EFI_GUID       *Guid,
  UINT32         FunctionNumber,
  IRSI_FUNCTION  IrsiFunction
  )
{
   IRSI_FUNCTION_ENTRY       *Node;

   if (EfiAtRuntime ()) {
     return EFI_ABORTED;
   } else if (IrsiGetFunction (Guid, FunctionNumber) != NULL) {
     return EFI_ALREADY_STARTED;
   }

   Node = AllocateRuntimePool (sizeof (IRSI_FUNCTION_ENTRY));
   if (Node == NULL) {
     ASSERT (Node != NULL);
     return EFI_OUT_OF_RESOURCES;
   }
   Node->Signature = IRSI_FUNCTION_SIGNATURE;
   Node->FunctionNumber = FunctionNumber;
   Node->Function = IrsiFunction;

   CopyMem (&Node->Guid, Guid, sizeof (EFI_GUID));

   InsertTailList (&mIrsiReg.FunctionList, &Node->Link);

   IrsiAddVirtualPointer ((VOID **)&Node->Function);

   return EFI_SUCCESS;
}

VOID
EFIAPI
InstallIrsiInfoVariable (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  EFI_STATUS                    Status;
  IRSI_INFO                     IrsiInfo;


  IrsiInfo.Signature            = IRSI_REGISTRATION_SIGNATURE;
  IrsiInfo.NumberOfTableEntries = (UINT32)mSystemTable->NumberOfTableEntries;
  IrsiInfo.ConfigurationTable   = (UINT64)(UINTN)mSystemTable->ConfigurationTable;

  Status = EfiSetVariable (
             L"IrsiInfo",
             &gIrsiFeatureGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             sizeof (IrsiInfo),
             &IrsiInfo
             );
  DEBUG((EFI_D_INFO, "Set IrsiInfo Variable Status = %r\n", Status));
}


/**
  IrsiRegistration RuntimeDxe module entry point


  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS:          Module initialized successfully
  @retval Others                Module initialized unsuccessfully
**/
EFI_STATUS
EFIAPI
InstallIrsiRegistrationRuntime (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_EVENT                   Event;
  IRSI_REGISTRATION_PROTOCOL *IrsiReg;

  mSystemTable = SystemTable;
  InitializeCrc32Table ();
  Status = gBS->LocateProtocol (&gIrsiRegistrationProtocolGuid, NULL, (VOID **)&IrsiReg);
  if (Status == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  InitializeListHead (&mIrsiReg.VirtualPointerList);
  InitializeListHead (&mIrsiReg.FunctionList);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  InstallIrsiInfoVariable,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );

  ASSERT_EFI_ERROR (Status);


  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  IrsiVirtualPointerConversionNotify,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mIrsiReg.VirtualNotifyEvent
                  );
  ASSERT_EFI_ERROR (Status);

  mIrsiReg.IrsiRuntimeBufferSize                 = PcdGet32 (PcdIrsiRuntimeBufferSize);
  mIrsiReg.IrsiRuntimeBuffer                     = AllocateRuntimePages (EFI_SIZE_TO_PAGES (mIrsiReg.IrsiRuntimeBufferSize));
  mIrsiReg.IrsiRegistration.RegisterFunction     = IrsiRegisterFunction;
  mIrsiReg.IrsiRegistration.ExecuteFunction      = IrsiExecuteFunction;
  mIrsiReg.IrsiRegistration.AddVirtualPointer    = IrsiAddVirtualPointer;
  mIrsiReg.IrsiRegistration.RemoveVirtualPointer = IrsiRemoveVirtualPointer;
  mIrsiReg.IrsiRegistration.GetRuntimeBuffer     = IrsiGetRuntimeBuffer;

  IrsiAddVirtualPointer((VOID **)&mIrsiReg.IrsiRuntimeBuffer);
  IrsiAddVirtualPointer ((VOID **)&mIrsiReg.IrsiRegistration.RegisterFunction);
  IrsiAddVirtualPointer ((VOID **)&mIrsiReg.IrsiRegistration.ExecuteFunction);
  IrsiAddVirtualPointer ((VOID **)&mIrsiReg.IrsiRegistration.AddVirtualPointer);
  IrsiAddVirtualPointer ((VOID **)&mIrsiReg.IrsiRegistration.GetRuntimeBuffer);

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gIrsiRegistrationProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mIrsiReg.IrsiRegistration
                  );

  return Status;
}
