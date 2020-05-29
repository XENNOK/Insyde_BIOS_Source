/** @file
  Common variable supports functions for user can extract whole variables
  (including variable header) from variable store.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/VariableSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>

#include <Pi/PiFirmwareVolume.h>

/**
  This code gets the current status of Variable Store.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @retval EfiRaw             Variable store status is raw.
  @retval EfiValid           Variable store status is valid.
  @retval EfiInvalid         Variable store status is invalid.
**/
VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN CONST VARIABLE_STORE_HEADER *VarStoreHeader
  )
{
  ECP_VARIABLE_STORE_HEADER        *EcpVarStoreHeader;

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    EcpVarStoreHeader = (ECP_VARIABLE_STORE_HEADER *) VarStoreHeader;
    if (EcpVarStoreHeader->Signature == ECP_VARIABLE_STORE_SIGNATURE &&
        EcpVarStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
        EcpVarStoreHeader->State == VARIABLE_STORE_HEALTHY
        ) {

      return EfiValid;
    }

    if (EcpVarStoreHeader->Signature == 0xffffffff &&
        EcpVarStoreHeader->Size == 0xffffffff &&
        EcpVarStoreHeader->Format == 0xff &&
        EcpVarStoreHeader->State == 0xff
        ) {

      return EfiRaw;
    } else {

      return EfiInvalid;
    }
  } else {
    if (CompareGuid (&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) &&
        VarStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
        VarStoreHeader->State == VARIABLE_STORE_HEALTHY
        ) {

      return EfiValid;
    }

    if (((UINT32 *)(&VarStoreHeader->Signature))[0] == 0xffffffff &&
        ((UINT32 *)(&VarStoreHeader->Signature))[1] == 0xffffffff &&
        ((UINT32 *)(&VarStoreHeader->Signature))[2] == 0xffffffff &&
        ((UINT32 *)(&VarStoreHeader->Signature))[3] == 0xffffffff &&
        VarStoreHeader->Size == 0xffffffff &&
        VarStoreHeader->Format == 0xff &&
        VarStoreHeader->State == 0xff
        ) {

      return EfiRaw;
    } else {
      return EfiInvalid;
    }
  }
}

/**
  This code checks if variable header is valid or not.

  @param[in] Variable  Pointer to the Variable Header.

  @retval TRUE         Variable header is valid.
  @retval FALSE        Variable header is not valid.
**/
BOOLEAN
IsValidVariableHeader (
  IN CONST VARIABLE_HEADER   *Variable
  )
{
  if (Variable == NULL || Variable->StartId != VARIABLE_DATA) {
    return FALSE;
  }

  if ((GetVariableStoreHeaderSize () + Variable->NameSize + Variable->DataSize) > MAX_VARIABLE_SIZE) {
    return FALSE;
  }

  return TRUE;
}

/**
  This code gets the size of name of variable.

  @param[in] Variable  Pointer to the Variable Header.

  @return UINTN        Size of variable in bytes.
**/
UINTN
NameSizeOfVariable (
  IN CONST VARIABLE_HEADER   *Variable
  )
{
  if (Variable->State    == (UINT8) (-1) ||
      Variable->DataSize == (UINT32) (-1) ||
      Variable->NameSize == (UINT32) (-1) ||
      Variable->Attributes == (UINT32) (-1)) {
    return 0;
  }
  return (UINTN) Variable->NameSize;
}

/**
  This code gets the pointer to the variable data.

  @param[in] Variable  Pointer to the Variable Header.

  @retval NULL         Variable strat id is incorrect.
  @return UINT8*       Pointer to Variable Data.
**/
UINT8 *
GetVariableDataPtr (
  IN CONST VARIABLE_HEADER   *Variable
  )
{
  if (Variable->StartId != VARIABLE_DATA) {
    return NULL;
  }
  //
  // Be careful about pad size for alignment
  //
  return (UINT8 *) ((UINTN) GET_VARIABLE_NAME_PTR (Variable) + Variable->NameSize + GET_PAD_SIZE (Variable->NameSize));
}

/**
  This code gets the pointer to the next variable header.

  @param[in] Variable       Pointer to the Variable Header.

  @return VARIABLE_HEADER*  Pointer to next variable header.
**/
VARIABLE_HEADER *
GetNextVariablePtr (
  IN CONST VARIABLE_HEADER   *Variable
  )
{
  UINTN Value;

  if (!IsValidVariableHeader (Variable)) {
    return NULL;
  }
  //
  // Be careful about pad size for alignment
  //
  Value =  (UINTN) GetVariableDataPtr (Variable);
  Value += Variable->DataSize;
  Value += GET_PAD_SIZE (Variable->DataSize);

  //
  // Be careful about pad size for alignment.
  //
  return (VARIABLE_HEADER *) HEADER_ALIGN (Value);
}

/**
  Gets the pointer to the first variable header in given variable store area.

  @param[in] VarStoreHeader Pointer to the Variable Store Header.

  @return VARIABLE_HEADER*  Pointer to the first variable header.
**/
VARIABLE_HEADER *
GetStartPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  return (VARIABLE_HEADER *) HEADER_ALIGN (((UINT8 *) VarStoreHeader) + GetVariableStoreHeaderSize ());
}

/**
  Gets the pointer to the end of the variable storage area.

  This function gets pointer to the end of the variable storage
  area, according to the input variable store header.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return VARIABLE_HEADER*   Pointer to the end of the variable storage area.
**/
VARIABLE_HEADER *
GetEndPointer (
  IN CONST VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  return (VARIABLE_HEADER *) HEADER_ALIGN ((UINTN) VarStoreHeader + GetVariableStoreSize (VarStoreHeader));

}

/**
  This code gets the size of variable data.

  @param[in] Variable  Pointer to the Variable Header.

  @return UINTN        Size of variable in bytes.
**/
UINTN
DataSizeOfVariable (
  IN CONST VARIABLE_HEADER   *Variable
  )
{
  if (Variable->State    == (UINT8)  (-1) ||
      Variable->DataSize == (UINT32) (-1) ||
      Variable->NameSize == (UINT32) (-1) ||
      Variable->Attributes == (UINT32) (-1)) {
    return 0;
  }
  return (UINTN) Variable->DataSize;
}

/**
  This code gets the size of variable store.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return UINT32             Total size of variable store.
**/
UINT32
GetVariableStoreSize (
  IN CONST VARIABLE_STORE_HEADER       *VariableStoreHeader
  )
{
  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    return ((ECP_VARIABLE_STORE_HEADER *) VariableStoreHeader)->Size;
  } else {
    return VariableStoreHeader->Size;
  }
}

/**
  Gets the size of variable store header.

  @return UINTN  size by byte of variable store header.
**/
UINTN
GetVariableStoreHeaderSize (
  VOID
  )
{
  return PcdGetBool (PcdUseEcpVariableStoreHeader) ? sizeof (ECP_VARIABLE_STORE_HEADER) : sizeof (VARIABLE_STORE_HEADER);
}

/**
  According to variable name and GUID to find this variable is whether in input buffer.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.
  @param[in] Buffer        Pointer to variable buffer.
  @param[in] BufferSize    the size in bytes of the buffer

  @retval TRUE             The function completed successfully.
  @retval FALSE            The function could not complete successfully.
**/
BOOLEAN
DoesVariableExist (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN CONST  UINT8                              *Buffer,
  IN        UINTN                              BufferSize
  )
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

  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  if (BufferSize < HeaderSize) {
    return VariableFound;
  }

  VariableHeader = (VARIABLE_HEADER *) (Buffer + HeaderSize);
  CurrentSearchedSize = HeaderSize;
  while (IsValidVariableHeader (VariableHeader) && CurrentSearchedSize < BufferSize) {
    NexVariable = GetNextVariablePtr (VariableHeader);
    if (VariableHeader->State == VAR_ADDED && StrCmp (VariableName, GET_VARIABLE_NAME_PTR (VariableHeader)) == 0 &&
        CompareGuid (VendorGuid, &VariableHeader->VendorGuid)) {
      VariableFound = TRUE;
      break;
    }
    CurrentSearchedSize += ((UINTN) NexVariable - (UINTN) VariableHeader);
    VariableHeader = NexVariable;
  }

  return VariableFound;
}