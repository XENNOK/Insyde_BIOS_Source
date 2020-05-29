/** @file
  Helper functions for registering new IRSI functions

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
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <Library/IrsiRegistrationLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/LoadedImage.h>
//
// The RuntimeServiceEntry contains virtually converted pointer of runtime services entry
//
typedef struct {
  UINT64             RuntimeServicesEntry;
} RUNTIME_SERVICES_CONFIG_TABLE;


IRSI_REGISTRATION_PROTOCOL    *mIrsiReg = NULL;
EFI_EVENT                     mIrsiRegEvent;
EFI_EVENT                     mExitBootServicesEvent;
BOOLEAN                       mIsRuntimeCode;


/**
  Irsi AddVirtualPointer function

  This routine adds pointer to the virtual conversion list

  @param Pointer                pointer to be converted

  @retval EFI_SUCCESS:          Pointer successfully added to virtual conversion list
  @retval EFI_ABORTED           Failed to add virtual pointer conversion list

**/
EFI_STATUS
EFIAPI
IrsiAddVirtualPointer (
  VOID **Pointer
  )
{
  if (mIrsiReg == NULL || !mIsRuntimeCode) {
    return EFI_ABORTED;
  } else {
    return mIrsiReg->AddVirtualPointer(Pointer);
  }
}

/**
  Irsi RemoveVirtualPointer function

  This routine removes pointer from the virtual conversion list

  @param[in] Pointer            pointer to be removed

  @retval EFI_SUCCESS           Pointer successfully removed from virtual conversion list.
  @retval EFI_NOT_FOUND         pointer isn't in the conversion list.
  @retval EFI_ABORTED           Failed to remove virtual pointer conversion list.
**/
EFI_STATUS
EFIAPI
IrsiRemoveVirtualPointer (
  VOID **Pointer
  )
{
  if (mIrsiReg == NULL || !mIsRuntimeCode) {
    return EFI_ABORTED;
  } else {
    return mIrsiReg->RemoveVirtualPointer(Pointer);
  }
}

/**
  Irsi Registerfunction

  This routine provides Irsi function registration

  @param FunctionName           Name of the IRSI function
  @param Guid                   IRSI function GUID
  @param IrsiFunction           The IRSI function pointer

  @retval EFI_SUCCESS           IRSI function successfully registered
  @retval EFI_ABORTED           System aborted in OS runtime

**/
EFI_STATUS
EFIAPI
IrsiRegisterFunction (
  EFI_GUID       *Guid,
  UINT32         FunctionNumber,
  IRSI_FUNCTION IrsiFunction
  )
{
  if (mIrsiReg == NULL ) {
    return EFI_ABORTED;
  } else {
    return mIrsiReg->RegisterFunction(Guid, FunctionNumber, IrsiFunction);
  }
}

/**
  Execute IRSI function

  @param  Guid                  IRSI Service Type GUID
  @param  FunctionNumber        Command code of IRSI function
  @param  CommBuf               Communication Buffer
  @param  BufSize               Buffer size

  @retval EFI_SUCCESS           IRSI function successfully executed
  @retval EFI_ABORT             Abort without executing IRSI function
  @return others                IRSI function executed with error
**/
EFI_STATUS
EFIAPI
IrsiExecuteFunction (
  EFI_GUID       *Guid,
  UINT32         FunctionNumber,
  VOID           *CommBuf,
  UINT32         BufSize
  )
{
  if (mIrsiReg == NULL) {
    return EFI_ABORTED;
  } else {
    return mIrsiReg->ExecuteFunction(Guid, FunctionNumber, CommBuf, BufSize);
  }
}

/**
  IRSI Get Runtime Buffer

  During runtime, the memory cannot be allocated by AllocatePool()
  This routine provides the pre-allocated IRSI runtime buffer for use at runtime

  @param IrsiBuffer             Pointer to the pre-allocated runtime buffer pointer
  @param BufferSize             Pointer to the IRSI runtime buffer size

  @retval EFI_SUCCESS           Function returns successfully
  @retval EFI_INVALID_PARAMETER Invalid parameter list with NULL IrsiBuffer or BufferSize pointer
  @retval EFI_ABORTED           Function aborted, IRSI Registration protocol not installed

**/
EFI_STATUS
EFIAPI
IrsiGetRuntimeBuffer (
  VOID    **IrsiBuffer,
  UINTN   *BufferSize
  )
{
  if (IrsiBuffer == NULL || BufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  } else if (mIrsiReg == NULL) {
    return EFI_ABORTED;
  } else {
    return mIrsiReg->GetRuntimeBuffer(IrsiBuffer, BufferSize);
  }
}


/**
  Install IRSI Runtime Services

  @param  RuntimeServicesGuid   IRSI Service Type GUID
  @param  BufSize               Buffer size

  @retval EFI_SUCCESS           IRSI function successfully executed
  @retval RuntimeServicesEntry  Runtime Services Entrypoint

  @retval EFI_SUCCESS           Runtime Services successfully installed
  @retval EFI_INVALID_PARAMETER NULL services GUID or NULL services entry given
  @retval EFI_DEVICE_ERROR      Memory allocation failed
  @return others                Runtime services Configuration table installation failed
**/
EFI_STATUS
EFIAPI
IrsiInstallRuntimeServices (
  EFI_GUID                     *RuntimeServicesGuid,
  IRSI_RUNTIME_SERVICES_ENTRY  RuntimeServicesEntry
  )
{
  EFI_STATUS                     Status;
  RUNTIME_SERVICES_CONFIG_TABLE  *ServicesTable;

  if (RuntimeServicesGuid == NULL || RuntimeServicesEntry == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ServicesTable = AllocateRuntimePool(sizeof(RUNTIME_SERVICES_CONFIG_TABLE));
  ASSERT (ServicesTable != NULL);
  if (ServicesTable == NULL) {
    return EFI_DEVICE_ERROR;
  }

  ServicesTable->RuntimeServicesEntry = (UINT64)(UINTN)RuntimeServicesEntry;
  Status = gBS->InstallConfigurationTable (RuntimeServicesGuid, (VOID *)ServicesTable);

  if (Status == EFI_SUCCESS) {
    IrsiAddVirtualPointer ((VOID **)&ServicesTable->RuntimeServicesEntry);
  }

  return Status;
}



/**
  Notification function for Irsi Registration Protocol

  This routine is the notification function for Irsi Registration Protocol

  @param EFI_EVENT              Event of the notification
  @param Context                not used in this function

  @retval none

**/
STATIC
VOID
EFIAPI
IrsiRegEventCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS      Status;

  Status = gBS->LocateProtocol (&gIrsiRegistrationProtocolGuid, NULL, (VOID **)&mIrsiReg);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (mIsRuntimeCode) {
    IrsiAddVirtualPointer ((VOID **)&mIrsiReg);
  }
  gBS->CloseEvent(Event);
}

/**
  Detect whether the system is at SMM mode.

  @retval TRUE                  System is at SMM mode.
  @retval FALSE                 System is not at SMM mode.
**/
STATIC
BOOLEAN
IsInSmm (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  BOOLEAN                        InSmm;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  InSmm = FALSE;
  SmmBase->InSmm (SmmBase, &InSmm);
  return InSmm;
}

/**
  Function uses image handle to check image is whther EfiRuntimeServicesCode image code type.

  @param[in] ImageHandle   Input Image handle.

  @retval TRUE             This is a EfiRuntimeServicesCode image code type iamge
  @retval FALSE            This isn't a EfiRuntimeServicesCode image code type iamge
**/
STATIC
BOOLEAN
IsRuntimeCodeImageType (
  IN EFI_HANDLE       ImageHandle
  )
{
  EFI_STATUS                       Status;
  EFI_LOADED_IMAGE_PROTOCOL        *LoadedImage;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );
  if (!EFI_ERROR (Status) && LoadedImage->ImageCodeType == EfiRuntimeServicesCode) {
    return TRUE;
  }

  return FALSE;
}

/**
  IrsiRegistrationLib LibraryClass constructor

  This routine is a LibraryClass constructor for IrsiRegistrationLib, it will
  initialize IrsiRegistrationProtocol

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @return others                Failed to install IrsiRegistrationLib

**/
EFI_STATUS
EFIAPI
IrsiRegistrationLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  VOID                           *Registration;

  //
  // System doesn't support Irsi related functions in SMM mode, so just return
  // EFI_SUCCESS and doesn't do any initialization.
  //
  if (IsInSmm ()) {
    return EFI_SUCCESS;
  }
  mIsRuntimeCode = IsRuntimeCodeImageType (ImageHandle);
  Status = gBS->LocateProtocol (&gIrsiRegistrationProtocolGuid, NULL, (VOID **)&mIrsiReg);
  if (EFI_ERROR(Status)) {
    mIrsiRegEvent = EfiCreateProtocolNotifyEvent  (
                          &gIrsiRegistrationProtocolGuid,
                          TPL_CALLBACK,
                          IrsiRegEventCallBack,
                          NULL,
                          &Registration
                          );
    ASSERT (mIrsiRegEvent != NULL);
  } else {
    if (mIsRuntimeCode) {
      IrsiAddVirtualPointer ((VOID **)&mIrsiReg);
    }
  }

  return EFI_SUCCESS;
}

/**
  If a runtime driver exits with an error, it must call this routine
  to free the allocated resource before the exiting.
  It will ASSERT() if gBS is NULL.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval  EFI_SUCCESS      The Runtime Driver Lib shutdown successfully.
**/
EFI_STATUS
EFIAPI
IrsiRegistrationLibDestruct (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  ASSERT (gBS != NULL);
  if (mIrsiRegEvent != NULL) {
    Status = gBS->CloseEvent (mIrsiRegEvent);
    ASSERT_EFI_ERROR (Status);
  } else if (mIrsiReg != NULL && mIsRuntimeCode) {
    IrsiRemoveVirtualPointer ((VOID **)&mIrsiReg);
  }

  return EFI_SUCCESS;
}