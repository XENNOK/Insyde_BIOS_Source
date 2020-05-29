/** @file
  Common functions to support read-only variables related interfaces

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
#include <Guid/DebugMask.h>

/**
  According to variable name and GUID to Determine the variable is OsIndicationsSupported or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is OsIndicationsSupported variable.
  @retval     FALSE          This isn't OsIndicationsSupported variable.
--*/
STATIC
BOOLEAN
IsOsIndicationsSupportedVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, OS_INDICATIONS_SUPPORTED_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and variable GUID to check this variable is whether
  SetupMode variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is SetupMode variable.
  @retval     FALSE          This isn't SetupMode variable.
--*/
STATIC
BOOLEAN
IsSetupModeVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_SETUP_MODE_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and variable GUID to check this variable is whether
  SecureBoot variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is SecureBoot variable.
  @retval     FALSE          This isn't SecureBoot variable.
--*/
STATIC
BOOLEAN
IsSecureBootVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_SECURE_BOOT_MODE_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and variable GUID to check this variable is whether
  SignatureSupport variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is SignatureSupport variable.
  @retval     FALSE          This isn't SignatureSupport variable.
--*/
STATIC
BOOLEAN
IsSignatureSupportVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_SIGNATURE_SUPPORT_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and variable GUID to check this variable is whether
  VendorKeys variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is VendorKeys variable.
  @retval     FALSE          This isn't VendorKeys variable.
--*/
STATIC
BOOLEAN
IsVendorKeysVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, VENDOR_KEYS_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is whether a read-only variable.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is a read-only variable.
  @retval     FALSE          This isn't a read-only variable.
--*/
BOOLEAN
IsReadOnlyVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (IsSecureDatabaseDefaultVariable  (VariableName, VendorGuid) || IsOsIndicationsSupportedVariable (VariableName, VendorGuid) ||
      IsSetupModeVariable              (VariableName, VendorGuid) || IsSecureBootVariable             (VariableName, VendorGuid) ||
      IsSignatureSupportVariable       (VariableName, VendorGuid) || IsVendorKeysVariable             (VariableName, VendorGuid)) {
    return TRUE;
  }
  return FALSE;
}