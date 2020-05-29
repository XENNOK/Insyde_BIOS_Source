/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SECURE_BOOT_H_
#define _SECURE_BOOT_H_

#include "Variable.h"

typedef
EFI_STATUS
(*SMI_SUB_FUNCTION) (
  VOID
  );

typedef struct {
  UINT8                                 FunNum;
  SMI_SUB_FUNCTION                      SmiSubFunction;
} SMI_SUB_FUNCTION_MAP;


//
// Define callback function prototype for non-SMI platform
//
typedef
EFI_STATUS
(*SECURE_BOOT_SUB_FUNCTION) (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     VOID                               *Data,
  IN     UINTN                              DataSize,
  IN     UINT32                             Attributes
  );

typedef struct {
  CHAR16                                *VariableName;
  SECURE_BOOT_SUB_FUNCTION              SecureBootSubFunction;
} SECURE_BOOT_SUB_FUNCTION_MAP;


/**
  Add selected file hash image to allowed database (db)

  @retval EFI_SUCCESS     Add image hash to allowed database (db) successful.
  @return Other           Any erro occured while udpating hash to allowed database (db)

**/
EFI_STATUS
SmmAddFileHashImage (
  VOID
  );

/**
  Update "SecureBootEnforce" EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS varialbe

  @retval EFI_SUCCESS   Update "SecureBootEnforce" variable successful.
  @return Other         Any erro occured while udpating "SecureBootEnforce" variable

**/
EFI_STATUS
SmmUpdateSecureBootEnforce (
  VOID
  );

/**
  This function uses to clear PK, KEK, db and dbx variable

  @retval EFI_SUCCESS  Clear secure settins successful.
  @return Other        Any erro occured while clearing secure settins

**/
EFI_STATUS
SmmClearAllSecureSettings (
  VOID
  );

/**
  Update "RestoreFactoryDefault" EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS varialbe

  @retval EFI_SUCCESS  Update "RestoreFactoryDefault" variable successful.
  @return Other        Any erro occured while udpating "RestoreFactoryDefault" variable

**/
EFI_STATUS
SmmRestoreFactoryDefault (
  VOID
  );

/**
  This function uses to update PK variable.

  @retval EFI_SUCCESS            Update PK variable successfully.
  @retval Other                  Any error occurred while updating PK variable.
**/
EFI_STATUS
SmmUpdatePkVariable (
  VOID
  );

/**
  This function uses to update KEK variable.

  @retval EFI_SUCCESS            Update KEK variable successfully.
  @retval Other                  Any error occurred while updating KEK variable.
**/
EFI_STATUS
SmmUpdateKekVariable (
  VOID
  );

/**
  This function uses to update db variable.

  @retval EFI_SUCCESS            Update db variable successfully.
  @retval Other                  Any error occurred while updating db variable
**/
EFI_STATUS
SmmUpdateDbVariable (
  VOID
  );

/**
  This function uses to update dbx variable.

  @retval EFI_SUCCESS            Update dbx variable successfully.
  @retval Other                  Any error occurred while updating dbx variable
**/
EFI_STATUS
SmmUpdateDbxVariable (
  VOID
  );


/**
  This fucnitons uses to disable all of secure boot SMI functions

  @retval EFI_SUCCESS   Disable all secure boot SMI functions successful.
  @return Other         Any erro occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SmmDisableSecureBootSmi (
  VOID
  );

/**
  Append new hash image to EFI_IMAGE_SECURITY_DATABASE (db)

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.
  @param  Attributes                  Attribute value of the variable.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
RuntimeDxeAddFileHashImage (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );

/**
  Update EFI_SECURE_BOOT_ENFORCE_NAME variable.

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.
  @param  Attributes                  Attribute value of the variable.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
RuntimeDxeUpdateSecureBootEnforce (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );


/**
  clear all secure boot settings.

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.
  @param  Attributes                  Attribute value of the variable.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
RuntimeDxeClearAllSecureSettings (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );

/**
  Update EFI_RESTORE_FACOTRY_DEFAULT_NAME varialbe and then system will restore secure boot
  database to factory default during next POST.

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.
  @param  Attributes                  Attribute value of the variable.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
RuntimeDxeRestoreFactoryDefault (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );

/**
  This function uses to update PK variable.

  @param[in] VariableName    Name of Variable to be found.
  @param[in] VendorGuid      Variable vendor GUID.
  @param[in] Data            Data pointer.
  @param[in] DataSize        Size of Data found. If size is less than the
                             data, this value contains the required size.
  @param[in] Attributes      Attribute value of the variable.

  @retval EFI_SUCCESS        Update PK variable successfully.
  @retval Other              Any error occurred while updating PK variable.
**/
EFI_STATUS
RuntimeDxeUpdatePkVariable (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );

/**
  This function uses to update KEK variable.

  @param[in] VariableName    Name of Variable to be found.
  @param[in] VendorGuid      Variable vendor GUID.
  @param[in] Data            Data pointer.
  @param[in] DataSize        Size of Data found. If size is less than the
                             data, this value contains the required size.
  @param[in] Attributes      Attribute value of the variable.

  @retval EFI_SUCCESS        Update KEK variable successfully.
  @retval Other              Any error occurred while updating KEK variable.
**/
EFI_STATUS
RuntimeDxeUpdateKekVariable (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );

/**
  This function uses to update db variable.

  @param[in] VariableName    Name of Variable to be found.
  @param[in] VendorGuid      Variable vendor GUID.
  @param[in] Data            Data pointer.
  @param[in] DataSize        Size of Data found. If size is less than the
                             data, this value contains the required size.
  @param[in] Attributes      Attribute value of the variable.

  @retval EFI_SUCCESS        Update db variable successfully.
  @retval Other              Any error occurred while updating db variable.
**/
EFI_STATUS
RuntimeDxeUpdateDbVariable (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  );

/**
  This function uses to update dbx variable.

  @param[in] VariableName    Name of Variable to be found.
  @param[in] VendorGuid      Variable vendor GUID.
  @param[in] Data            Data pointer.
  @param[in] DataSize        Size of Data found. If size is less than the
                             data, this value contains the required size.
  @param[in] Attributes      Attribute value of the variable.

  @retval EFI_SUCCESS        Update dbx variable successfully.
  @retval Other              Any error occurred while updating dbx variable.
**/
EFI_STATUS
RuntimeDxeUpdateDbxVariable (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  )
;

/**
  Use variable name and variable GUID to check this variable is administer secure boot relative variable

  @param  VariableName       Name of Variable to be found.
  @param  VendorGuid         Variable vendor GUID.

  @retval TRUE               This variable is a administer secure boot relative variable.
  @retval FALSE              This variable isn't a administer secure boot relative variable
**/
BOOLEAN
IsAdministerSecureVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  );

/**
  Update administer secure boot relative variable

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.
  @param  Attributes                  Attribute value of the variable.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
UpdateAdministerSecureVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     VOID                               *Data,
  IN     UINTN                              DataSize,
  IN     UINT32                             Attributes
  );

/**
  Initialize all of administer secure boot relative authenticated varialbes.

  @retval EFI_SUCCESS  Update all of secure boot relative authenticated varialbes successful
  @retval Other        The driver failded to start the device

**/
EFI_STATUS
InitializeAdministerSecureBootVariables (
  VOID
  );

/**
  According input data to update L"SecureBootEnforce" variable

  @param  Data         Input data to update "SecureBootEnforce" variable

  @retval EFI_SUCCESS  update "SecureBootEnforce" successful.
  @retval Other        The driver failded to start the device

**/
EFI_STATUS
UpdateSecureBootEnforceVariable (
  UINT8           Data
  );

/**
  According input data to update L"RestoreFactoryDefault" variable

  @param  Data         Input data to update "RestoreFactoryDefault" variable

  @retval EFI_SUCCESS  update "RestoreFactoryDefault" successful.
  @retval Other        The driver failded to start the device

**/
EFI_STATUS
UpdateRestoreFactoryDefaultVariable (
  UINT8           Data
  );


/**
  According to input data to update custom security status.

  In this function, it will update "CustomSecurity" variable and "vendorKeys" variable.
  If input Data is 0, it will update "CustomSecurity" to 0 and "vendorKeys" to 1.
  If input Data is 1, it will update "CustomSecurity" to 1 and "vendorKeys" to 0.

  @param[in] Data                Input data to update custom security status.

  @retval EFI_SUCCESS            Update custom security status successful.
  @retval EFI_INVALID_PARAMETER  The value of Data isn't 0 or 1.
  @retval Other                  Any error occurred while updating variable.
**/
EFI_STATUS
UpdateCustomSecurityStatus (
  IN  UINT8               Data
  );

extern SMI_SUB_FUNCTION_MAP mSecureBootFunctionsTable[];
extern SMI_SUB_FUNCTION_MAP mNonSecureBootFunctionsTable[];
#endif
