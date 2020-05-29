//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
//;   VariableSupportLib.c
//;
//; Abstract:
//;
//;  Common variable supports functions for user can extract whole variables
//;  (including variable header) from variable store.
//;
//;

#include "VariableSupportLib.h"
#include "EfiCommonLib.h"
#include "EfiFirmwareVolumeHeader.h"



VARIABLE_STORE_STATUS
EFIAPI
GetVariableStoreStatus (
  IN CONST VARIABLE_STORE_HEADER *VarStoreHeader
  )
/*++

Routine Description:

  This code gets the pointer to the variable name.

Arguments:

  VarStoreHeader  - Pointer to the Variable Store Header.

Returns:

  EfiHealthy      - Variable store is healthy.
  EfiRaw          - Variable store is raw.
  EfiInvalid      - Variable store is invalid.

--*/
{
  if (VarStoreHeader->Signature == VARIABLE_STORE_SIGNATURE &&
      VarStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
      VarStoreHeader->State == VARIABLE_STORE_HEALTHY
      ) {

    return EfiValid;
  } else if (VarStoreHeader->Signature == 0xffffffff &&
           VarStoreHeader->Size == 0xffffffff &&
           VarStoreHeader->Format == 0xff &&
           VarStoreHeader->State == 0xff
          ) {

    return EfiRaw;
  } else {

    return EfiInvalid;
  }
}


BOOLEAN
EFIAPI
IsValidVariableHeader (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code checks if variable header is valid or not.

Arguments:

  Variable   - Pointer to the Variable Header.

Returns:

  TRUE       - Variable header is valid.
  FALSE      - Variable header is not valid.

--*/
{
  if (Variable == NULL || Variable->StartId != VARIABLE_DATA) {
    return FALSE;
  }

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
  //
  // Hardware error record variable needs larger size.
  //
  if ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    if ((sizeof (VARIABLE_HEADER) + Variable->NameSize + Variable->DataSize) > MAX_HARDWARE_ERROR_VARIABLE_SIZE) {
      return FALSE;
    }
  } else
#endif
  if ((sizeof (VARIABLE_HEADER) + Variable->NameSize + Variable->DataSize) > MAX_VARIABLE_SIZE) {
    return FALSE;
  }

  return TRUE;
}


UINT8 *
EFIAPI
GetVariableDataPtr (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the pointer to the variable data.

Arguments:

  Variable    - Pointer to the Variable Header.

Returns:

  UINT8*      - Pointer to Variable Data.

--*/
{
  if (Variable->StartId != VARIABLE_DATA) {

    return NULL;
  }
  //
  // Be careful about pad size for alignment
  //
  return (UINT8 *) ((UINTN) GET_VARIABLE_NAME_PTR (Variable) + Variable->NameSize + GET_PAD_SIZE (Variable->NameSize));
}


VARIABLE_HEADER *
EFIAPI
GetNextVariablePtr (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the pointer to the next variable header.

Arguments:

  Variable             - Pointer to the Variable Header.

Returns:

  VARIABLE_HEADER*     - Pointer to next variable header.

--*/
{
  if (!IsValidVariableHeader (Variable)) {
    return NULL;
  }
  //
  // Be careful about pad size for alignment
  //
  return (VARIABLE_HEADER *) ((UINTN) GetVariableDataPtr (Variable) + Variable->DataSize + GET_PAD_SIZE (Variable->DataSize));
}

VARIABLE_HEADER *
GetStartPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  )
/*++

Routine Description:

  Gets the pointer to the first variable header in given variable store area.

Arguments:

  VarStoreHeader        - Pointer to the Variable Store Header.

Returns:

  VARIABLE_HEADER*      - Pointer to the first variable header.

--*/
{
  //
  // The end of variable store.
  //
  return (VARIABLE_HEADER *) (VarStoreHeader + 1);
}

VARIABLE_HEADER *
EFIAPI
GetEndPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  )
/*++

Routine Description:

  This code gets the pointer to the last variable memory pointer byte

Arguments:

  VarStoreHeader        - Pointer to the Variable Store Header.

Returns:

  VARIABLE_HEADER*      - Pointer to last unavailable Variable Header.

--*/
{
  //
  // The end of variable store
  //
  return (VARIABLE_HEADER *) ((UINTN) VarStoreHeader + VarStoreHeader->Size);
}

UINTN
DataSizeOfVariable (
  IN CONST VARIABLE_HEADER   *Variable
  )
/*++

Routine Description:

  This code gets the data size of specific variable.

Arguments:

  Variable       - Pointer to the Variable Header.

Returns:

  UINTN          - Data size of specific variable.

--*/
{
  if (Variable->State    == (UINT8)  (-1) ||
      Variable->DataSize == (UINT32) (-1) ||
      Variable->NameSize == (UINT32) (-1) ||
      Variable->Attributes == (UINT32) (-1)) {
    return 0;
  }
  return (UINTN) Variable->DataSize;
}

BOOLEAN
DoesVariableExist (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     UINT8                              *Buffer,
  IN     UINTN                              BufferSize
  )
/*++

Routine Description:

  According to variable name and GUID to find this variable is whether in input buffer.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.
  Buffer             - Pointer to variable buffer.
  BufferSize         - the size in bytes of the buffer.

Returns:

  TRUE               - This is in input buffer.
  FALSE              - This isn't in input buffer.

--*/
{
  UINTN                   HeaderSize;
  UINTN                   CurrentSearchedSize;
  VARIABLE_HEADER         *VariableHeader;
  VARIABLE_HEADER         *NexVariable;
  BOOLEAN                 VariableFound;

  VariableFound = FALSE;
  if (VariableName == NULL || VendorGuid == NULL || Buffer == NULL) {
    return VariableFound;
  }
  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER);
  if (BufferSize < HeaderSize) {
    return VariableFound;
  }

  VariableHeader = (VARIABLE_HEADER *) (Buffer + HeaderSize);
  CurrentSearchedSize = HeaderSize;
  while (IsValidVariableHeader (VariableHeader) && CurrentSearchedSize < BufferSize) {
    NexVariable = GetNextVariablePtr (VariableHeader);
    if (VariableHeader->State == VAR_ADDED && EfiStrCmp (VariableName, GET_VARIABLE_NAME_PTR (VariableHeader)) == 0 &&
        EfiCompareGuid (VendorGuid, &VariableHeader->VendorGuid)) {
      VariableFound = TRUE;
      break;
    }
    CurrentSearchedSize += ((UINTN) NexVariable - (UINTN) VariableHeader);
    VariableHeader = NexVariable;
  }

  return VariableFound;
}