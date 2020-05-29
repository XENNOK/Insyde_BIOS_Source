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
//;   ReadOnlyVariables.c
//;
//; Abstract:
//;
//;  Common functions to support read-only variables related interfaces
//;
//;

#include "VariableSupportLib.h"
#include "EfiCommonLib.h"

#include EFI_GUID_DEFINITION (GlobalVariable)
#include EFI_GUID_DEFINITION (ImageAuthentication)

STATIC
BOOLEAN
IsOsIndicationsSupportedVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is OsIndicationsSupported or not.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is OsIndicationsSupported variable.
  FALSE              - This isn't OsIndicationsSupported variable.

--*/
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, OS_INDICATIONS_SUPPORTED_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}


STATIC
BOOLEAN
IsSetupModeVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
/*++

Routine Description:

  According to variable name and variable GUID to check this variable is whether
  SetupMode variable.

Argments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Retruns:

  TRUE               - This is SetupMode variable.
  FALSE              - This isn't SetupMode variable.

--*/
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_SETUP_MODE_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
BOOLEAN
IsSecureBootVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
/*++

Routine Description:

  According to variable name and variable GUID to check this variable is whether
  SecureBoot variable.

Argments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Retruns:

  TRUE               - This is SecureBoot variable.
  FALSE              - This isn't SecureBoot variable.

--*/
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_SECURE_BOOT_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
BOOLEAN
IsSignatureSupportVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
/*++

Routine Description:

  According to variable name and variable GUID to check this variable is whether
  SignatureSupport variable.

Argments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Retruns:

  TRUE               - This is SignatureSupport variable.
  FALSE              - This isn't SignatureSupport variable.

--*/
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, EFI_SIGNATURE_SUPPORT_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
BOOLEAN
IsVendorKeysVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
/*++

Routine Description:

  According to variable name and variable GUID to check this variable is whether
  VendorKeys variable.

Argments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Retruns:

  TRUE               - This is VendorKeys variable.
  FALSE              - This isn't VendorKeys variable.

--*/
{

  if (VariableName != NULL && VendorGuid != NULL) {
    if (EfiStrCmp (VariableName, VENDOR_KEYS_NAME) == 0 && EfiCompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}


BOOLEAN
IsReadOnlyVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
/*++

Routine Description:

  According to variable name and GUID to Determine the variable is whether a read-only variable.

Arguments:

  VariableName       - Name of Variable to be found.
  VendorGuid         - Variable vendor GUID.

Returns:

  TRUE               - This is a read-only variable.
  FALSE              - This isn't a read-only variable.

--*/
{
  if (IsSecureDatabaseDefaultVariable  (VariableName, VendorGuid) || IsOsIndicationsSupportedVariable (VariableName, VendorGuid) ||
      IsSetupModeVariable              (VariableName, VendorGuid) || IsSecureBootVariable             (VariableName, VendorGuid) ||
      IsSignatureSupportVariable       (VariableName, VendorGuid) || IsVendorKeysVariable             (VariableName, VendorGuid)) {
    return TRUE;
  }
  return FALSE;
}