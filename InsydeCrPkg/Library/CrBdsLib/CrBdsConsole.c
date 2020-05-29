/** @file
  CrBdsLib

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


#include <Library/CrBdsLib.h>


/**
  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param  Name                  String part of EFI variable name
  @param  VendorGuid            GUID part of EFI variable name
  @param  VariableSize          Returns the size of the EFI variable that was read

  @return                       Dynamically allocated memory that contains a copy of the EFI variable
                                Caller is responsible freeing the buffer.
  @retval NULL                  Variable was not read

**/
VOID *
EFIAPI
CRBdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;
  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      *VariableSize = 0;
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  *VariableSize = BufferSize;
  return Buffer;
}


EFI_STATUS
CRUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_GUID                  *ConVarGuid,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     DevicePathSize;
  BOOLEAN                   VarChanged;
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole;
  EFI_DEVICE_PATH_PROTOCOL  *OldVarConsole;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *PrevDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *VarConsoleInstance;
  EFI_DEVICE_PATH_PROTOCOL  *CustomizedInstance;


  VarChanged      = FALSE;
  VarConsole      = NULL;
  DevicePathSize  = 0;
  NewDevicePath   = NULL;
  Status          = EFI_UNSUPPORTED;

  if (CustomizedConDevicePath == ExclusiveDevicePath) {
    return EFI_UNSUPPORTED;
  }

  VarConsole = CommonGetVariableData (ConVarName, ConVarGuid);

  OldVarConsole = VarConsole;

  if (ExclusiveDevicePath != NULL && VarConsole != NULL) {
    while (VarConsole != NULL) {

      VarConsoleInstance = GetNextDevicePathInstance (&VarConsole, &DevicePathSize);

      if (!CRBdsLibMatchDevicePaths (ExclusiveDevicePath, VarConsoleInstance)) {
        PrevDevicePath = NewDevicePath;
        NewDevicePath = AppendDevicePathInstance (PrevDevicePath, VarConsoleInstance);
        if  (PrevDevicePath != NULL)
          gBS->FreePool (PrevDevicePath);
      }
      else {
        VarChanged = TRUE;
      }
      gBS->FreePool (VarConsoleInstance);
    }
  }
  else {
    NewDevicePath = DuplicateDevicePath (VarConsole);
  }

  while (CustomizedConDevicePath != NULL) {

    CustomizedInstance = GetNextDevicePathInstance (&CustomizedConDevicePath, &DevicePathSize);

    if (!CRBdsLibMatchDevicePaths (NewDevicePath, CustomizedInstance)) {
      VarChanged = TRUE;
      PrevDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (PrevDevicePath, CustomizedInstance);
      if  (PrevDevicePath != NULL)
        gBS->FreePool (PrevDevicePath);
    }

    gBS->FreePool (CustomizedInstance);

  }

  if (OldVarConsole != NULL) {
    gBS->FreePool (OldVarConsole);
  }

  //
  // Update the variable of the default console
  //
  if (VarChanged) {
    UINT32    Attributes;
    CHAR16    *StringPtr;

    StringPtr = ConVarName;

    //
    // If the variable includes "Dev" at last, we consider
    // it does not support NV attribute.
    //
    while (*StringPtr != L'\0') {
      StringPtr++;
    }

    if ( (((INTN)((UINTN)StringPtr - (UINTN)ConVarName) / sizeof (CHAR16)) > 3) &&
        (*(StringPtr - 3) == L'D') &&
        (*(StringPtr - 2) == L'e') &&
        (*(StringPtr - 1) == L'v')
       ) {
      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    } else {
      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    }

    CommonSetVariable (
      ConVarName,
      ConVarGuid,
      Attributes,
      GetDevicePathSize (NewDevicePath),
      NewDevicePath
      );

    return EFI_SUCCESS;
  }


  return EFI_ABORTED;

}
