//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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
//;   SecureBootSupport.c
//;
//; Abstract:
//;
//;  Common secure boot relative supports functions
//;
//;

#include "VariableSupportLib.h"
#include "EfiCommonLib.h"
#include "EfiFirmwareVolumeHeader.h"
#include "AdmiSecureBoot.h"

#include EFI_GUID_DEFINITION (GlobalVariable)
#include EFI_GUID_DEFINITION (ImageAuthentication)
#include EFI_GUID_DEFINITION (GenericVariable)

BOOLEAN
IsSecureDatabaseVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is secure database relative variable.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is secure database relative variable.
  FALSE              - This isn't secure database relative variable.

--*/
{

  if (IsPkVariable (VariableName, VendorGuid) || IsKekVariable (VariableName, VendorGuid) ||
      IsDbVariable (VariableName, VendorGuid) || IsDbxVariable (VariableName, VendorGuid)) {
    return TRUE;
  }
  return FALSE;
}


BOOLEAN
IsPkVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is PK or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is PK variable.
  FALSE              - This isn't PK variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_PLATFORM_KEY_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsKekVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is KEK or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is KEK variable.
  FALSE              - This isn't KEK variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_KEY_EXCHANGE_KEY_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsDbVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is db or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is db variable.
  FALSE              - This isn't db variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_IMAGE_SECURITY_DATABASE) == 0 && EfiCompareGuid (VendorGuid, &gEfiImageSecurityDatabaseGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsDbxVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is dbx or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is dbx variable.
  FALSE              - This isn't dbx variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_IMAGE_SECURITY_DATABASE1) == 0 && EfiCompareGuid (VendorGuid, &gEfiImageSecurityDatabaseGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsSecureDatabaseDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is secure database default relative variable.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is secure database default relative variable.
  FALSE              - This isn't secure database default relative variable.

--*/
{
  if (IsPkDefaultVariable (VariableName, VendorGuid) || IsKekDefaultVariable (VariableName, VendorGuid) ||
      IsDbDefaultVariable (VariableName, VendorGuid) || IsDbxDefaultVariable (VariableName, VendorGuid)) {
    return TRUE;
  }
  return FALSE;
}

BOOLEAN
IsPkDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is PKDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is PKDefault variable.
  FALSE              - This isn't PKDefault variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_PLATFORM_KEY_DEFAULT_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsKekDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is KEKDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is KEKDefault variable.
  FALSE              - This isn't KEKDefault variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_KEY_EXCHANGE_KEY_DEFAULT_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsDbDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is dbDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is dbDefault variable.
  FALSE              - This isn't dbDefault variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_IMAGE_SECURITY_DEFAULT_DATABASE) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
IsDbxDefaultVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is dbxDefault or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is dbxDefault variable.
  FALSE              - This isn't dbxDefault variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_IMAGE_SECURITY_DEFAULT_DATABASE1) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}


STATIC
BOOLEAN
IsCustomSecurityVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is CustomSecurity or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is CustomSecurity variable.
  FALSE              - This isn't CustomSecurity variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_CUSTOM_SECURITY_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGenericVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

STATIC
BOOLEAN
VariableNeedReserve (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine this variable whether need reserve during
  restore factory default process.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is CustomSecurity variable.
  FALSE              - This isn't CustomSecurity variable.

--*/
{
  //
  // Needn't reserve EFI_CUSTOM_SECURITY_NAME, and then system will create new variable to indicate system is in
  // standard mode.
  //
  if (IsSecureDatabaseVariable (VariableName, VendorGuid) || IsCustomSecurityVariable (VariableName, VendorGuid)) {
    return FALSE;
  }
  return TRUE;
}


EFI_STATUS
MergeVariableToFactoryCopy (
  IN OUT UINT8        *FactorycopyBuffer,
  IN OUT UINTN        *FactoryBufferLength,
  IN     UINT8        *VariableBuffer,
  IN     UINTN        VariableBufferLength
  )
/*++

Routine Description:

  Merge all of variables aren't secure boot relative variables and doesn't saved in
  factory copy region from variable store to factory copy region.

Arguments:

  FactorycopyBuffer       - On input, the buffer contained all of factory copy data.
                            On output, the buffer has been appended data from VariableBuffer.
  FactoryBufferLength     - On input, the size of FactorycopyBuffer.
                            On output, the total used size in FactorycopyBuffer.
  VariableBuffer          - Buffer to save all of variable data.
  VariableBufferLength    - The size of VariableBuffer.


Returns:

  EFI_SUCCESS             - Merge varialbe data to factory copy region successful.
  EFI_INVALID_PARAMETER   - FactorycopyBuffer, FactoryBufferLength or VariableBuffer is NULL.
                            The data in FactorycopyBuffer in incorrect.
  EFI_BUFFER_TOO_SMALL    - FactoryBufferLength is too small to contain merged data.

--*/
{

  UINTN               LastVariableOffset;
  UINTN               VariableSize;
  UINTN               HeaderSize;
  VARIABLE_HEADER     *VariableHeader;
  VARIABLE_HEADER     *NextVariable;
  EFI_STATUS          Status;

  if (FactorycopyBuffer == NULL || FactoryBufferLength == NULL || VariableBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the factory defualt data is whether valid
  //
  for (LastVariableOffset = *FactoryBufferLength; LastVariableOffset > 0; LastVariableOffset--) {
    if (FactorycopyBuffer[LastVariableOffset - 1] != 0xFF) {
      break;
    }
  }
  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER);
  if (LastVariableOffset + 1 < HeaderSize) {
   return EFI_INVALID_PARAMETER;
  }

  //
  // Find last variable offset in working buffer
  //
  NextVariable  = (VARIABLE_HEADER *) (FactorycopyBuffer + HeaderSize);
  while (IsValidVariableHeader (NextVariable)) {
    NextVariable = GetNextVariablePtr (NextVariable);
  }
  LastVariableOffset = (UINTN) NextVariable - (UINTN) FactorycopyBuffer;
  //
  // Copy original firmware volume and variable store header information to exported factory default buffer.
  // This action can prevent from the information of firmware volume header and variable store header in
  // factory default region is incorrect.
  //
  EfiCommonLibCopyMem (FactorycopyBuffer, VariableBuffer, HeaderSize);
  //
  // merge variable data to working buffer
  //
  Status = EFI_SUCCESS;
  VariableHeader = (VARIABLE_HEADER *) (VariableBuffer + HeaderSize);
  while (IsValidVariableHeader (VariableHeader)) {
    NextVariable = GetNextVariablePtr (VariableHeader);
    //
    // Collect all of valid variables. these variables should include variable state in added state and in deleted transition state.
    // We also can remove the variable with deleted transition if variable store has the variable with added state.
    // For current design, variable should do this check, so we needn't do this check here.
    //
    if (VariableHeader->State == VAR_ADDED || VariableHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
      if (VariableNeedReserve ((CHAR16 *) (VariableHeader + 1), &VariableHeader->VendorGuid) &&
          !DoesVariableExist ((CHAR16 *) (VariableHeader + 1), &VariableHeader->VendorGuid, FactorycopyBuffer, *FactoryBufferLength)) {
        VariableSize = (UINTN) NextVariable - (UINTN) VariableHeader;
        if (LastVariableOffset + VariableSize <= VariableBufferLength - 1) {
          EfiCommonLibCopyMem (&FactorycopyBuffer[LastVariableOffset], VariableHeader, VariableSize);
        }
        LastVariableOffset += VariableSize;
      }
    }
    VariableHeader = NextVariable;
  }

  if (LastVariableOffset > VariableBufferLength) {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *FactoryBufferLength = LastVariableOffset;
  return Status;
}