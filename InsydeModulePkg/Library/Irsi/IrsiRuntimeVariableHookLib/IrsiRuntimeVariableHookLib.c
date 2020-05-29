/** @file
  Helper Library Class for enabling Windows GetSystemEnvronmentVariable() API
  to call IRSI runtime services

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IrsiRegistrationLib.h>

#define IRSI_SERVICES_NAME  L"IrsiServices"

BOOLEAN          mVariableHooked = FALSE;
EFI_GET_VARIABLE mUefiGetVariable;

/**
  Hooked GetVariable function for IRSI functions

  @param  VariableName            the name of the vendor's variable, it's a Null-Terminated Unicode String
  @param  VendorGuid              Unify identifier for vendor.
  @param  Attributes              Point to memory location to return the attributes of variable. If the point
                                  is NULL, the parameter would be ignored.
  @param  DataSize                As input, point to the maximum size of return Data-Buffer.
                                  As output, point to the actual size of the returned Data-Buffer.
  @param  Data                    Point to return Data-Buffer.

  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_NOT_FOUND          The variable was not found.
  @retval  EFI_BUFFER_TOO_SMALL   The DataSize is too small for the result. DataSize has
                                  been updated with the size needed to complete the request.
  @retval  EFI_INVALID_PARAMETER  VariableName is NULL.
  @retval  EFI_INVALID_PARAMETER  VendorGuid is NULL.
  @retval  EFI_INVALID_PARAMETER  DataSize is NULL.
  @retval  EFI_INVALID_PARAMETER  The DataSize is not too small and Data is NULL.
  @retval  EFI_DEVICE_ERROR       The variable could not be retrieved due to a hardware error.
  @retval  EFI_SECURITY_VIOLATION The variable could not be retrieved due to an authentication failure.
**/
EFI_STATUS
EFIAPI
IrsiGetVariable(
  IN  CHAR16                       *VariableName,
  IN  EFI_GUID                     *VendorGuid,
  IN  UINT32                       *Attributes,
  IN  UINTN                        *DataSize,
  IN  VOID                         *Data
  )
{
  EFI_STATUS           Status;
  IRSI_HEADER          *IrsiHeader;

  Status = mUefiGetVariable (
                             VariableName,
                             VendorGuid,
                             Attributes,
                             DataSize,
                             Data
                            );
   if (Status == EFI_NOT_FOUND) {
     if (StrCmp(VariableName, IRSI_SERVICES_NAME)==0) {
       IrsiHeader = (IRSI_HEADER *)Data;
       Status = IrsiExecuteFunction (VendorGuid, IrsiHeader->FunctionNumber, Data, (UINT32)*DataSize);
     }
   }
   return Status;
}


/**
  Variable Arch. Protocol notification event handler.

  @param[in] Event              Event whose notification function is being invoked.
  @param[in] Context            Pointer to the notification function's context.

  @retval    None
**/
VOID
EFIAPI
IrsiHookVariable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{

  if (mVariableHooked) {
    return;
  }
  mUefiGetVariable = gRT->GetVariable;
  gRT->GetVariable = IrsiGetVariable;
  gRT->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 ((VOID *)gRT, gRT->Hdr.HeaderSize, &gRT->Hdr.CRC32);
  mVariableHooked = TRUE;
  IrsiAddVirtualPointer ((VOID **)&mUefiGetVariable);

  gBS->CloseEvent(Event);
}



/**
  IrsiRuntimeVariableHookLib LibraryClass constructor

  This routine will allow Windows GetSystemEnvronmentVariable() API to call IRSI runtime services

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS:          Module initialized successfully
**/
EFI_STATUS
EFIAPI
IrsiRuntimeVariableHookLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_EVENT                   ReadyToBootEvent;

  Status = EfiCreateEventReadyToBootEx (
                  TPL_CALLBACK,
                  IrsiHookVariable,
                  NULL,
                  &ReadyToBootEvent
                  );
  return Status;
}