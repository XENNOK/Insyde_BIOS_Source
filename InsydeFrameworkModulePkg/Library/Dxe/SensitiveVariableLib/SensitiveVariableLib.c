//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   SensitiveVariableLib.c
//;
//; Abstract:
//;
//;   Provide related functions to access sensitive variables.
//;
//;

#include "SensitiveVariableLib.h"
#include "VariableSupportLib.h"
#include "EfiDriverLib.h"
#include "SmiTable.h"

UINT8
EFIAPI
SmmSensitiveVariableCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );

STATIC
EFI_STATUS
CreateSensitiveAuthenData (
  IN  UINTN                                 InputSize,
  IN  UINT8                                 *InputBuffer,
  OUT SENSITIVE_VARIABLE_AUTHENTICATION     **OutputBuffer
  )
/*++

Routine Description:

  Internal function uses input certificate, certificate size to create sensitive setup data which
  which uses to update sensitive setup variable through SMI.

Arguments:

  InputSize          - The size in bytes of the input certificate.
  InputBuffer        - The content of input certificate.
  OutputBuffer       - A pointer to the output buffer to save the SENSITIVE_VARIABLE_AUTHENTICATION descriptor.
                       This buffer is allocated with a call to  the Boot Service AllocatePool().  It is the
                       caller's responsibility to call the Boot Service FreePool() when the caller no longer
                       requires the contents of Buffer.

Returns:

  EFI_SUCCESS           - Create SENSITIVE_VARIABLE_AUTHENTICATION descriptor successfully.
  EFI_INVALID_PARAMETER - Any input parameter in incorrect.
  EFI_OUT_OF_RESOURCES  - There is not enough pool memory to store the SENSITIVE_VARIABLE_AUTHENTICATION descriptor.

--*/
{
  UINTN                              DataSize;
  SENSITIVE_VARIABLE_AUTHENTICATION   *Data;

  if (InputBuffer == NULL || InputSize == 0 || OutputBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  DataSize = sizeof (SENSITIVE_VARIABLE_AUTHENTICATION) + InputSize;
  Data = EfiLibAllocatePool (DataSize);
  if (Data == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Data->Signature = SET_SENSITIVE_VARIABLE_SIGNATURE;
  Data->DataSize  = (UINT32) DataSize;
  Data->Status    =  EFI_UNSUPPORTED;
  EfiCopyMem (Data + 1, InputBuffer, InputSize);

  *OutputBuffer = Data;

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateSetupSensitiveVariableThroughSmi (
  IN   UINTN              DataSize,
  IN   VOID               *Data
  )
/*++

Routine Description:

  Set sensitive variable to variable store through SMI.

Arguments:

  DataSize              - The data size by byte of found variable
  Data                  - Pointer to the start address of found data.

Returns:

  EFI_SUCCESS           - Update sensitive variable successfully.
  EFI_INVALID_PARAMETER - Any input parameter is incorrect.
  Other                 - Any other error occurred while updating sensitive variable.

--*/
{
  SENSITIVE_VARIABLE_AUTHENTICATION  *WriteData;
  EFI_STATUS                         Status;

  Status = CreateSensitiveAuthenData (DataSize, Data, &WriteData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmSensitiveVariableCall ((UINT8 *) WriteData, (UINTN) WriteData->DataSize, SET_SENSITIVE_VARIABLE_FUN_NUM, SW_SMI_PORT);
  Status = WriteData->Status;
  gBS->FreePool (WriteData);
  return Status;
}

EFI_STATUS
SetVariableToSensitiveVariable (
  IN   CHAR16                           *VariableName,
  IN   EFI_GUID                         *VendorGuid,
  IN   UINT32                           Attributes,
  IN   UINTN                            DataSize,
  IN   VOID                             *Data
  )
/*++

Routine Description:

  Set changed sensitive setup data to sensitive setup variable.

Arguments:

  VariableName          - A pointer to a null-terminated string that is the variable's name.
  VendorGuid            - A pointer to an EFI_GUID that is the variable's GUID. The combination of
                          VendorGuid and VariableName must be unique.
  Attributes            - Attributes bitmask to set for the variable.
  DataSize              - The data size by byte of variable
  Data                  - Pointer to the start address of data.

Returns:

  EFI_SUCCESS           - Set data to sensitive variable successfully.
  EFI_INVALID_PARAMETER - Any input parameter is incorrect.
  EFI_NOT_FOUND         - Cannot set specific variable in sensitive setup variable.

--*/
{
  SENSITIVE_VARIABLE_STORE_HEADER *CurrentStoreHeader;
  SENSITIVE_VARIABLE_HEADER       *CurrentVariable;
  EFI_STATUS                      Status;
  UINTN                           BufferSize;

  if (VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (DataSize != 0 && Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  BufferSize = sizeof (SENSITIVE_VARIABLE_STORE_HEADER) + sizeof (SENSITIVE_VARIABLE_HEADER) + EfiStrSize (VariableName) + DataSize;

  CurrentStoreHeader = EfiLibAllocatePool (BufferSize);
  if (CurrentStoreHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CurrentStoreHeader->VariableCount = 1;
  CurrentStoreHeader->HeaderSize = sizeof (SENSITIVE_VARIABLE_STORE_HEADER);
  CurrentVariable = (SENSITIVE_VARIABLE_HEADER *) (CurrentStoreHeader + 1);

  CurrentVariable->Attributes = Attributes;
  CurrentVariable->DataSize   = (UINT32) DataSize;
  EfiCopyMem (&CurrentVariable->VendorGuid, VendorGuid, sizeof (EFI_GUID));
  EfiCopyMem (CurrentVariable + 1, VariableName, EfiStrSize (VariableName));
  EfiCopyMem (GetSensitiveVariableDataPtr (CurrentVariable), Data, DataSize);
  Status = UpdateSetupSensitiveVariableThroughSmi (BufferSize, (UINT8 *) CurrentStoreHeader);

  gBS->FreePool (CurrentStoreHeader);
  return Status;
}