/** @file
  Common secure boot relative supports functions

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
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/AdmiSecureBoot.h>
#include <Guid/DebugMask.h>

#include <Pi/PiFirmwareVolume.h>

/**
  According to variable name and GUID to Determine the variable is secure database relative variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is secure database relative variable.
  @retval     FALSE          This isn't secure database relative variable.
--*/
BOOLEAN
IsSecureDatabaseVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{

  if (IsPkVariable (VariableName, VendorGuid) || IsKekVariable (VariableName, VendorGuid) ||
      IsDbVariable (VariableName, VendorGuid) || IsDbxVariable (VariableName, VendorGuid)) {
    return TRUE;
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is PK or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is PK variable.
  @retval     FALSE          This isn't PK variable.
--*/
BOOLEAN
IsPkVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_PLATFORM_KEY_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is KEK or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is KEK variable.
  @retval     FALSE          This isn't KEK variable.
--*/
BOOLEAN
IsKekVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_KEY_EXCHANGE_KEY_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is db or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is db variable.
  @retval     FALSE          This isn't db variable.
--*/
BOOLEAN
IsDbVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_IMAGE_SECURITY_DATABASE) == 0 && CompareGuid (VendorGuid, &gEfiImageSecurityDatabaseGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is dbx or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is dbx variable.
  @retval     FALSE          This isn't dbx variable.
--*/
BOOLEAN
IsDbxVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_IMAGE_SECURITY_DATABASE1) == 0 && CompareGuid (VendorGuid, &gEfiImageSecurityDatabaseGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is secure database default
  relative variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is secure database default relative variable.
  @retval     FALSE          This isn't secure database default relative variable.
--*/
BOOLEAN
IsSecureDatabaseDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (IsPkDefaultVariable (VariableName, VendorGuid) || IsKekDefaultVariable (VariableName, VendorGuid) ||
      IsDbDefaultVariable (VariableName, VendorGuid) || IsDbxDefaultVariable (VariableName, VendorGuid)) {
    return TRUE;
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is PKDefault or not.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             This is PKDefault variable.
  @retval FALSE            This isn't PKDefault variable.

--*/
BOOLEAN
IsPkDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_PLATFORM_KEY_DEFAULT_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is KEKDefault or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is KEKDefault variable.
  @retval     FALSE          This isn't KEKDefault variable.
--*/
BOOLEAN
IsKekDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_KEY_EXCHANGE_KEY_DEFAULT_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is dbDefault or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is dbDefault variable.
  @retval     FALSE          This isn't dbDefault variable.
--*/
BOOLEAN
IsDbDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_IMAGE_SECURITY_DEFAULT_DATABASE) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is dbxDefault or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is dbxDefault variable.
  @retval     FALSE          This isn't dbxDefault variable.
--*/
BOOLEAN
IsDbxDefaultVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_IMAGE_SECURITY_DEFAULT_DATABASE1) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is CustomSecurity or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is CustomSecurity variable.
  @retval     FALSE          This isn't CustomSecurity variable.
--*/
STATIC
BOOLEAN
IsCustomSecurityVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_CUSTOM_SECURITY_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGenericVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine this variable whether need reserve during
  restore factory default process.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is CustomSecurity variable.
  @retval     FALSE          This isn't CustomSecurity variable.
--*/
STATIC
BOOLEAN
VariableNeedReserve (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
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

/**
  Merge all of variables aren't secure boot relative variables and doesn't saved in factory copy
  region from variable store to factory copy region.

  @param[in, out]  FactorycopyBuffer      [in]:The buffer contained all of factory copy data.
                                          [out]:The buffer has been appended data from VariableBuffer.
  @param[in, out]  FactoryBufferLength    [in]:The size of FactorycopyBuffer.
                                          [out]:The total used size in FactorycopyBuffer.
  @param[in]       VariableBuffer         Buffer to save all of variable data.
  @param[in]       VariableBufferLength   The size of VariableBuffer.

  @retval EFI_SUCCESS                     Merge varialbe data to factory copy region successful.
  @retval EFI_INVALID_PARAMETER           FactorycopyBuffer, FactoryBufferLength or VariableBuffer is NULL.
                                          The data in FactorycopyBuffer in incorrect.
  @retval EFI_BUFFER_TOO_SMALL            FactoryBufferLength is too small to contain merged data.
--*/
EFI_STATUS
MergeVariableToFactoryCopy (
  IN OUT   UINT8        *FactorycopyBuffer,
  IN OUT   UINTN        *FactoryBufferLength,
  IN CONST UINT8        *VariableBuffer,
  IN       UINTN        VariableBufferLength
  )
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
  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
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
  CopyMem (FactorycopyBuffer, VariableBuffer, HeaderSize);
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
          CopyMem (&FactorycopyBuffer[LastVariableOffset], VariableHeader, VariableSize);
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