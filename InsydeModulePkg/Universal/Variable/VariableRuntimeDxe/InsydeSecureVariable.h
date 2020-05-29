/** @file
    The header file for  insyde secure variable functions.

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

#ifndef _INSYDE_SECURE_VARIABLE_H_
#define _INSYDE_SECURE_VARIABLE_H_
#include "Variable.h"

#include <Guid/AdminPassword.h>

/**
  This function uses to check the variable is whether a valid Insyde secure variable.

  @param[in] VariableName   A CHAR16 string of varialbe name.
  @param[in] VendorGuid     Variable vendor GUID.
  @param[in] Attributes     Attribute value of the variable found.
  @param[in] DataSize       Size of Data buffer.
  @param[in] Data           Pointer to a buffer of the variable contents.

  @retval TRUE              System updates normal Insyde secure variable successful.
  @retval FALSE             Any input parameter is NULL.
**/
BOOLEAN
IsValidInsydeSecureVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to check the variable is whether a valid check admin password variable.

  @param[in] VariableName   A CHAR16 string of varialbe name.
  @param[in] VendorGuid     Variable vendor GUID.
  @param[in] Attributes     Attribute value of the variable found.
  @param[in] DataSize       Size of Data buffer.
  @param[in] Data           Pointer to a buffer of the variable contents.

  @retval TRUE              This is  a valid check admin password variable.
  @retval FALSE             This isn't a valid check admin password variable.
**/
BOOLEAN
IsValidCheckAdminPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to check the variable is whether an unlock check admin password variable.

  @param[in] VariableName   A CHAR16 string of varialbe name.
  @param[in] VendorGuid     Variable vendor GUID.
  @param[in] Attributes     Attribute value of the variable found.
  @param[in] DataSize       Size of Data buffer.
  @param[in] Data           Pointer to a buffer of the variable contents.

  @retval TRUE              This is an unlock check admin password variable.
  @retval FALSE             This isn't an unlock check admin password variable.
**/
BOOLEAN
IsUnlockPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to check the variable is whether a lock check admin password variable.

  @param[in] VariableName   A CHAR16 string of varialbe name.
  @param[in] VendorGuid     Variable vendor GUID.
  @param[in] Attributes     Attribute value of the variable found.
  @param[in] DataSize       Size of Data buffer.
  @param[in] Data           Pointer to a buffer of the variable contents.

  @retval TRUE              This is a lock check admin password variable.
  @retval FALSE             This isn't a lock check admin password variable.
**/
BOOLEAN
IsLockPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  According to variable name and variable to check this variable is whether a CheckAdminPassword variable.

  @param[in] VariableName   A CHAR16 string of varialbe name.
  @param[in] VendorGuid     Variable vendor GUID.

  @retval TRUE              This is a CheckAdminPassword variable.
  @retval FALSE             This isn't a CheckAdminPassword variable.
**/
BOOLEAN
IsCheckAdminPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  Check the data contents and attributes to determine this is whether a insyde secure variable.

  @param[in] Attributes  Attribute value of the variable found.
  @param[in] DataSize    Size of Data buffer.
  @param[in] Data        Pointer to a buffer of the variable contents.

  @retval TRUE           This is a AdminPassword variable.
  @retval FALSE          This isn't a AdminPassword variable.
**/
BOOLEAN
IsInsydeSecureVariable (
  IN      UINT32                          Attributes,
  IN      UINTN                           DataSize,
  IN CONST VOID                            *Data
  );

/**
  Using the data contents to check this is whether a Insyde secure type.

  @param[in] DataSize    Size of Data buffer.
  @param[in] Data        Pointer to a buffer of the variable contents.

  @retval TRUE           This is Insyde secure type.
  @retval FALSE          This isn't Insyde secure type.
**/
BOOLEAN
IsInsydeSecureType (
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to check the variable is whether a valid admin password variable.

  @param[in] VariableName  A CHAR16 string of varialbe name.
  @param[in] VendorGuid    Variable vendor GUID.
  @param[in] Attributes    Attribute value of the variable found.
  @param[in] DataSize      Size of Data buffer.
  @param[in] Data          Pointer to a buffer of the variable contents.

  @retval TRUE             This is a valid admin password variable.
  @retval FALSE            This isn't a valid admin password variable.
**/
BOOLEAN
IsValidAdminPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to check the variable is whether a delete admin password variable.

  @param[in] VariableName  A CHAR16 string of varialbe name.
  @param[in] VendorGuid    Variable vendor GUID.
  @param[in] Attributes    Attribute value of the variable found.
  @param[in] DataSize      Size of Data buffer.
  @param[in] Data          Pointer to a buffer of the variable contents.

  @retval TRUE             This is a delete admin password variable.
  @retval FALSE            This isn't a delete admin password variable.
**/
BOOLEAN
IsDeleteAdminPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to check the variable is whether a replace admin password variable.

  @param[in] VariableName  A CHAR16 string of varialbe name.
  @param[in] VendorGuid    Variable vendor GUID.
  @param[in] Attributes    Attribute value of the variable found.
  @param[in] DataSize      Size of Data buffer.
  @param[in] Data          Pointer to a buffer of the variable contents.

  @retval TRUE             This is a replace admin password variable.
  @retval FALSE            This isn't a replace admin password variable.
**/
BOOLEAN
IsReplaceAdminPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  According to variable name and variable to check this variable is whether a AdminPassword variable.

  @param[in] VariableName  A CHAR16 string of varialbe name.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             This is a AdminPassword variable.
  @retval FALSE            This isn't a AdminPassword variable.
**/
BOOLEAN
IsAdminPasswordVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  );

/**
  This function uses to check the variable is whether a valid admin password variable.

  @param[in] VariableName  A CHAR16 string of varialbe name.
  @param[in] VendorGuid    Variable vendor GUID.
  @param[in] Attributes    Attribute value of the variable found.
  @param[in] DataSize      Size of Data buffer.
  @param[in] Data          Pointer to a buffer of the variable contents.

  @retval TRUE             This is a valid admin password variable.
  @retval FALSE            This isn't a valid admin password variable.
**/
BOOLEAN
IsNormalInsydeSecureVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  );

/**
  This function uses to process Insyde secure varialbes.

  @param[in] VariableName         A CHAR16 string of varialbe name.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Pointer to a buffer of the variable contents.
  @param[in] DataSize             Size of Data buffer.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable found.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Change Insyde variable state to unlocked state.
  @retval EFI_INVALID_PARAMETER   Any input parameter is NULL.
  @retval EFI_WRITE_PROTECTED     Udpate any insyde secure variable except for Unlock password state
                                  when the insyde secure variable state is locked.
**/
EFI_STATUS
ProcessInsydeSecureVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN CONST  VOID                               *Data,
  IN        UINTN                              DataSize,
  IN        VARIABLE_POINTER_TRACK             *Variable,
  IN        UINT32                             Attributes,
  IN        VARIABLE_GLOBAL                    *Global
  );

/**
  This function uses VARIABLE_POINTER_TRACK to check this variable is whether an existing insyde secure variable.

  @param[in] Variable  The variable information which is used to keep track of variable usage.

  @retval TRUE         The variable is an existing Insyde secure variable.
  @retval FALSE        The variable isn't an existing Insyde secure variable.
**/
BOOLEAN
IsExistingInsydeSecureVariable (
  IN CONST VARIABLE_POINTER_TRACK               *Variable
  );

#endif
