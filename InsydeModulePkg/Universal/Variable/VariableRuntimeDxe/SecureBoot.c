/** @file
  Provide support functions for Secure Boot.

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


#include "SecureBoot.h"
#include "AuthService.h"
#include "VariableLock.h"
#include "SensitiveVariable.h"

SMI_SUB_FUNCTION_MAP mSecureBootFunctionsTable [] = {
  { ADD_HASH_IMAGE_FUN_NUM,                   SmmAddFileHashImage           }, \
  { SECURE_BOOT_ENFORCE_FUN_NUM,              SmmUpdateSecureBootEnforce    }, \
  { CLEAR_ALL_SECURE_SETTINGS_FUN_NUM,        SmmClearAllSecureSettings     }, \
  { RESTORE_FACOTRY_DEFAULT_FUN_NUM,          SmmRestoreFactoryDefault      }, \
  { UPDATE_PK_FUN_NUM,                        SmmUpdatePkVariable           }, \
  { UPDATE_KEK_FUN_NUM,                       SmmUpdateKekVariable          }, \
  { UPDATE_DB_FUN_NUM,                        SmmUpdateDbVariable           }, \
  { UPDATE_DBX_FUN_NUM,                       SmmUpdateDbxVariable          }, \
  { SMM_CONTROL_SENSITIVE_CAPABILITY_FUN_NUM, SmmControlSensitiveCapability }, \
  { SET_SENSITIVE_VARIABLE_FUN_NUM,           SmmSetSensitiveVariable       }, \
  { SMM_VARIABLE_LOCK_FUN_NUM,                SmmCreateVariableLockList     }, \
  { LEGACY_BOOT_SMI_FUN_NUM,                  SmmLegacyBootEvent            }, \
  { SMM_SET_VARIABLE_SMI_FUN_NUM,             SmmInternalSetVariable        }, \
  { DISABLE_VARIABLE_CACHE_SMI_FUN_NUM,       SmmDisableVariableCache       }, \
  { DISABLE_SECURE_BOOT_SMI_FUN_NUM,          SmmDisableSecureBootSmi       }, \
  { 0,                                        NULL                          }
  };

SECURE_BOOT_SUB_FUNCTION_MAP mNonSmiSecureBootFunctionsTable[] = {
  { EFI_ADD_HASH_IMMAGE_NAME,             RuntimeDxeAddFileHashImage         }, \
  { EFI_SECURE_BOOT_ENFORCE_NAME,         RuntimeDxeUpdateSecureBootEnforce  }, \
  { EFI_CLEAR_ALL_SECURE_SETTINGS_NAME,   RuntimeDxeClearAllSecureSettings   }, \
  { EFI_RESTORE_FACOTRY_DEFAULT_NAME,     RuntimeDxeRestoreFactoryDefault    }, \
  { EFI_PLATFORM_KEY_NAME,                RuntimeDxeUpdatePkVariable         }, \
  { EFI_KEY_EXCHANGE_KEY_NAME,            RuntimeDxeUpdateKekVariable        }, \
  { EFI_IMAGE_SECURITY_DATABASE,          RuntimeDxeUpdateDbVariable         }, \
  { EFI_IMAGE_SECURITY_DATABASE1,         RuntimeDxeUpdateDbxVariable        }, \
  { NULL,                                 NULL                               }
  };


/**
  This function is an internal function which used for update normal
  authenticated

  @param  VariableName          Name of Variable to be found
  @param  VendorGuid            Variable vendor GUID
  @param  DataSize              Size of Data found. If size is less than the data, this value contains the required size.
  @param  Data                  Data pointer
  @param  UpdateKey             Boolean value to indicate want to update key or not.

Retruns:

  @retval EFI_SUCCESS           The update operation is success.
  @return EFI_OUT_OF_RESOURCES  Variable region is full, can not write other data into this region.

**/
STATIC
EFI_STATUS
InternalUpdateAuthVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINTN                   DataSize,
  IN VOID                    *Data,
  IN BOOLEAN                 UpdateKey
  )
{
  UINT8                            *VariableBuffer;
  EFI_VARIABLE_AUTHENTICATION      *CertData;
  UINTN                            VariableCount;
  VARIABLE_POINTER_TRACK           Variable;
  UINT32                           KeyIndex;
  EFI_CERT_BLOCK_RSA_2048_SHA256   *CertBlock;
  EFI_STATUS                       Status;


  if (UpdateKey) {
    if (DataSize < AUTHINFO_SIZE) {
      return EFI_UNSUPPORTED;
    }

    VariableBuffer = (UINT8 *) Data;
    CertData = (EFI_VARIABLE_AUTHENTICATION *) VariableBuffer;

    FindVariableByLifetime (
      VariableName,
      VendorGuid,
      &Variable,
      &VariableCount,
      &mVariableModuleGlobal->VariableBase
      );

    KeyIndex = 0;
    if (Variable.CurrPtr == NULL || (Variable.CurrPtr->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0) {
      CertBlock = (EFI_CERT_BLOCK_RSA_2048_SHA256 *) (CertData->AuthInfo.CertData);
      Status = AddPubKeyInStore (CertBlock->PublicKey, &mVariableModuleGlobal->VariableBase, &KeyIndex);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    return UpdateVariable (
             VariableName,
             VendorGuid,
             VariableBuffer + AUTHINFO_SIZE,
             DataSize - AUTHINFO_SIZE,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS,
             KeyIndex,
             CertData->MonotonicCount,
             &Variable,
             NULL,
             &mVariableModuleGlobal->VariableBase
             );
  } else {
    FindVariableByLifetime (
      VariableName,
      VendorGuid,
      &Variable,
      &VariableCount,
      &mVariableModuleGlobal->VariableBase
      );

   return UpdateVariable (
             VariableName,
             VendorGuid,
             Data,
             DataSize,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS,
             0,
             0,
             &Variable,
             NULL,
             &mVariableModuleGlobal->VariableBase
             );
  }
}

/**
  Internal function to update secure boot database variable.

  @param[in]  VariableName  Name of Variable to be found
  @param[in]  VendorGuid    Variable vendor GUID
  @param[in]  DataSize      Size of Data found. If size is less than the data, this value contains the required size.
  @param[in]  Data          Data pointer
  @param[in]  UpdateType    The Input update type. This update type should be UPDATE_AUTHENTICATED_VARIABLE
                            or APPEND_AUTHENTICATED_VARIABLE.

  @retval EFI_SUCCESS            update secure boot database variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval Other                  Set variable failed in this function.
**/
STATIC
EFI_STATUS
InternalUpdateSecureDatabaseVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINTN                   DataSize,
  IN VOID                    *Data,
  IN UINT8                   UpdateType
  )
{
  EFI_VARIABLE_AUTHENTICATION_2    *CertData;
  UINT8                            *VariableBuffer;
  EFI_SIGNATURE_LIST               *SigList;
  UINTN                            VariableCount;
  VARIABLE_POINTER_TRACK           Variable;
  EFI_STATUS                       Status;
  UINT32                           Attribute;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (UpdateType != UPDATE_AUTHENTICATED_VARIABLE && UpdateType != APPEND_AUTHENTICATED_VARIABLE) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsSecureDatabaseVariable (VariableName, VendorGuid)) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsPkVariable (VariableName, VendorGuid) && UpdateType != UPDATE_AUTHENTICATED_VARIABLE) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataSize < AUTHINFO2_SIZE (Data)) {
    return EFI_INVALID_PARAMETER;
  }

  VariableBuffer = (UINT8 *) Data;
  CertData = (EFI_VARIABLE_AUTHENTICATION_2 *) VariableBuffer;
  if ((CertData->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||
      !CompareGuid (&CertData->AuthInfo.CertType, &gEfiCertPkcs7Guid)) {
    return EFI_INVALID_PARAMETER;;
  }

  SigList  = (EFI_SIGNATURE_LIST *) (VariableBuffer + AUTHINFO2_SIZE (VariableBuffer));

  FindVariableByLifetime (
    VariableName,
    VendorGuid,
    &Variable,
    &VariableCount,
    &mVariableModuleGlobal->VariableBase
    );

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  if (UpdateType == APPEND_AUTHENTICATED_VARIABLE) {
    Attribute |= EFI_VARIABLE_APPEND_WRITE;
  }

  Status = UpdateVariable (
             VariableName,
             VendorGuid,
             SigList,
             DataSize - AUTHINFO2_SIZE (VariableBuffer),
             Attribute,
             0,
             0,
             &Variable,
             &CertData->TimeStamp,
             &mVariableModuleGlobal->VariableBase
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
   return Status;
  }
  //
  // Set "CustomSecurity" varialbe to 1 indicates secure boot database has been modified by user.
  //
  Status = UpdateCustomSecurityStatus (1);
  ASSERT_EFI_ERROR (Status);

  return Status;

}

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
CommonAddFileHashImage (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  )
{
  EFI_SIGNATURE_LIST               *SigList;
  EFI_VARIABLE_AUTHENTICATION_2    *CertData2;
  UINTN                            VariableCount;
  VARIABLE_POINTER_TRACK           Variable;
  EFI_STATUS                       Status;

  if (DataSize != (AUTHINFO2_SIZE (Data) + sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + SHA256_DIGEST_SIZE) &&
      DataSize != (AUTHINFO2_SIZE (Data) + sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + SHA1_DIGEST_SIZE)) {
    return EFI_UNSUPPORTED;
  }

  CertData2 = (EFI_VARIABLE_AUTHENTICATION_2 *) Data;
  SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) Data + AUTHINFO2_SIZE (Data));

  if (!CompareGuid (&SigList->SignatureType, &gEfiCertSha256Guid) &&
      !CompareGuid (&SigList->SignatureType, &gEfiCertSha1Guid)) {
    return EFI_UNSUPPORTED;
  }

  FindVariableByLifetime (
    EFI_IMAGE_SECURITY_DATABASE,
    &gEfiImageSecurityDatabaseGuid,
    &Variable,
    &VariableCount,
    &mVariableModuleGlobal->VariableBase
    );

  Status = UpdateVariable (
             VariableName,
             VendorGuid,
             SigList,
             DataSize - AUTHINFO2_SIZE (Data),
             Attributes,
             0,
             0,
             &Variable,
             &CertData2->TimeStamp,
             &mVariableModuleGlobal->VariableBase
             );
 ASSERT_EFI_ERROR (Status);
 if (EFI_ERROR (Status)) {
  return Status;
 }
 //
 // Set "CustomSecurity" varialbe to 1 indicates secure boot database has been modified by user.
 //
 Status = UpdateCustomSecurityStatus (1);
 ASSERT_EFI_ERROR (Status);

 return Status;
}

/**
  This function uses to check secure boot enforce is enabled or disabled.

  @retval TRUE     Secure boot enforce is enabled.
  @retval FALSE    Secure boot enforce is disabled.
**/
BOOLEAN
IsSecureBootEnforceEnabled (
  VOID
  )
{
  UINTN                            VariableCount;
  VARIABLE_POINTER_TRACK           Variable;
  UINT8                            *SecureBootEnforceEnabled;

  VariableCount = 0;
  FindVariableByLifetime (
    EFI_SECURE_BOOT_ENFORCE_NAME,
    &gEfiGenericVariableGuid,
    &Variable,
    &VariableCount,
    &mVariableModuleGlobal->VariableBase
    );
  ASSERT (Variable.CurrPtr != NULL && DataSizeOfVariable (Variable.CurrPtr) == sizeof (UINT8));
  SecureBootEnforceEnabled = GetVariableDataPtr (Variable.CurrPtr);

  return (BOOLEAN) *SecureBootEnforceEnabled;
}

/**
  Update EFI_SECURE_BOOT_ENFORCE_NAME variable.

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
CommoneUpdateSecureBootEnforce (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize
  )
{
  EFI_STATUS        Status;

  if (DataSize != (AUTHINFO_SIZE + sizeof (UINT8))) {
    return EFI_UNSUPPORTED;
  }

  Status = InternalUpdateAuthVariable (
             VariableName,
             VendorGuid,
             DataSize,
             Data,
             TRUE
             );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
   return Status;
  }

  if (!IsSecureBootEnforceEnabled ()) {
    UpdatePlatformBootMode (SECURE_BOOT_MODE_DISABLE, &mVariableModuleGlobal->VariableBase);
  }

  return Status;
}
/**
  This function uses to clear PK, KEK, db and dbx variable

  @retval EFI_SUCCESS  Clear secure settins successful.
  @return Other        Any erro occured while clearing secure settins

**/
EFI_STATUS
CommonClearAllSecureSettings (
  VOID
  )
{
  EFI_STATUS     Status;

  Status = ClearSecureSettings (&mVariableModuleGlobal->VariableBase);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Set "CustomSecurity" varialbe to 1 indicates secure boot database has been modified by user.
  //
  Status = UpdateCustomSecurityStatus (1);
  ASSERT_EFI_ERROR (Status);
  return Status;
}
/**
  UUpdate "RestoreFactoryDefault" EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS varialbe.

  @param  VariableName                Name of Variable to be found.
  @param  VendorGuid                  Variable vendor GUID.
  @param  Data                        Data pointer.
  @param  DataSize                    Size of Data found. If size is less than the
                                      data, this value contains the required size.

  @return EFI_INVALID_PARAMETER       Invalid parameter.
  @return EFI_WRITE_PROTECTED         Variable is write-protected and needs authentication with
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_SECURITY_VIOLATION      The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                      set, but the AuthInfo does NOT pass the validation
                                      check carried out by the firmware.
  @return EFI_SUCCESS                 Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
CommoneRestoreFactoryDefault (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize
  )
{
  EFI_STATUS         Status;
  if (DataSize != (AUTHINFO_SIZE + sizeof (UINT8))) {
     return EFI_UNSUPPORTED;
  }

  Status = InternalUpdateAuthVariable (
             VariableName,
             VendorGuid,
             DataSize,
             Data,
             TRUE
             );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
   return Status;
  }
 //
 // Set "CustomSecurity" variable to 0 indicates secure boot database has been restored to factory by user.
 //
  Status = UpdateCustomSecurityStatus (0);
  ASSERT_EFI_ERROR (Status);
  return Status;
}


/**
  Add selected file hash image to allowed database (db)

  @retval EFI_SUCCESS     Add image hash to allowed database (db) successful.
  @return Other           Any erro occured while udpating hash to allowed database (db)

**/
EFI_STATUS
SmmAddFileHashImage (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);

  return CommonAddFileHashImage (
           EFI_IMAGE_SECURITY_DATABASE,
           &gEfiImageSecurityDatabaseGuid,
           VariableBuffer,
           BufferSize,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |
           EFI_VARIABLE_APPEND_WRITE
           );
}

/**
  Update "SecureBootEnforce" EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS varialbe

  @retval EFI_SUCCESS   Update "SecureBootEnforce" variable successful.
  @return Other         Any erro occured while udpating "SecureBootEnforce" variable

**/
EFI_STATUS
SmmUpdateSecureBootEnforce (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);

  return CommoneUpdateSecureBootEnforce (
           EFI_SECURE_BOOT_ENFORCE_NAME,
           &gEfiGenericVariableGuid,
           VariableBuffer,
           BufferSize
           );
}

/**
  This function uses to clear PK, KEK, db and dbx variable

  @retval EFI_SUCCESS  Clear secure settins successful.
  @return Other        Any erro occured while clearing secure settins

**/
EFI_STATUS
SmmClearAllSecureSettings (
  VOID
  )
{
  return CommonClearAllSecureSettings ();
}

/**
  Update "RestoreFactoryDefault" EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS varialbe

  @retval EFI_SUCCESS  Update "RestoreFactoryDefault" variable successful.
  @return Other        Any erro occured while udpating "RestoreFactoryDefault" variable

**/
EFI_STATUS
SmmRestoreFactoryDefault (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);

  return CommoneRestoreFactoryDefault (
           EFI_RESTORE_FACOTRY_DEFAULT_NAME,
           &gEfiGenericVariableGuid,
           VariableBuffer,
           BufferSize
           );
}

/**
  This function uses to update PK variable.

  @retval EFI_SUCCESS            Update PK variable successfully.
  @retval Other                  Any error occurred while updating PK variable.
**/
EFI_STATUS
SmmUpdatePkVariable (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;
  EFI_STATUS                       Status;
  UINT8                            Updatetype;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
  Updatetype = *(VariableBuffer + AUTHINFO2_SIZE (VariableBuffer) - 1);

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid,
             BufferSize,
             VariableBuffer,
             Updatetype
             );
  return Status;
}

/**
  This function uses to update KEK variable.

  @retval EFI_SUCCESS            Update KEK variable successfully.
  @retval Other                  Any error occurred while updating KEK variable.
**/
EFI_STATUS
SmmUpdateKekVariable (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;
  EFI_STATUS                       Status;
  UINT8                            Updatetype;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
  Updatetype = *(VariableBuffer + AUTHINFO2_SIZE (VariableBuffer) - 1);

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_KEY_EXCHANGE_KEY_NAME,
             &gEfiGlobalVariableGuid,
             BufferSize,
             VariableBuffer,
             Updatetype
             );
  return Status;
}

/**
  This function uses to update db variable.

  @retval EFI_SUCCESS            Update db variable successfully.
  @retval Other                  Any error occurred while updating db variable
**/
EFI_STATUS
SmmUpdateDbVariable (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;
  EFI_STATUS                       Status;
  UINT8                            Updatetype;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
  Updatetype = *(VariableBuffer + AUTHINFO2_SIZE (VariableBuffer) - 1);

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_IMAGE_SECURITY_DATABASE,
             &gEfiImageSecurityDatabaseGuid,
             BufferSize,
             VariableBuffer,
             Updatetype
             );
  return Status;
}

/**
  This function uses to update dbx variable.

  @retval EFI_SUCCESS            Update dbx variable successfully.
  @retval Other                  Any error occurred while updating dbx variable
**/
EFI_STATUS
SmmUpdateDbxVariable (
  VOID
  )
{
  UINT32                           BufferSize;
  UINT8                            *VariableBuffer;
  EFI_STATUS                       Status;
  UINT8                            Updatetype;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
  Updatetype = *(VariableBuffer + AUTHINFO2_SIZE (VariableBuffer) - 1);

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_IMAGE_SECURITY_DATABASE1,
             &gEfiImageSecurityDatabaseGuid,
             BufferSize,
             VariableBuffer,
             Updatetype
             );
  return Status;
}

/**
  This fucnitons uses to disable all of secure boot SMI functions

  @retval EFI_SUCCESS   Disable all secure boot SMI functions successful.
  @return Other         Any erro occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SmmDisableSecureBootSmi (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL      *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT        SwContext;

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&SwDispatch
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SwDispatch->UnRegister (SwDispatch, mSmmVariableGlobal->SwHandle);

  SwContext.SwSmiInputValue = SECURE_BOOT_SW_SMI;
  Status = SwDispatch->Register (
                         SwDispatch,
                         NonSecureBootCallback,
                         &SwContext,
                         &mSmmVariableGlobal->SwHandle
                         );

  return Status;

}


/**
  Use variable name and variable GUID to get relative SMM function index.

  @param  VariableName       Name of Variable to be found.
  @param  VendorGuid         Variable vendor GUID.
  @param  FunctionIndex      Pointer to save output function index, if this pointer isn't NULL.

  @retval TRUE               This variable is a administer secure boot relative variable.
  @retval FALSE              This variable isn't a administer secure boot relative variable
**/
EFI_STATUS
GetSecureBootFunctionIndex (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  OUT    UINTN                              *FunctionIndex  OPTIONAL
  )
{

  UINTN          Index;
  EFI_STATUS     Status;

  Status = EFI_NOT_FOUND;
  if (VariableName == NULL || VendorGuid == NULL) {
    return Status;
  }

  if (!CompareGuid (VendorGuid, &gEfiGenericVariableGuid)) {
   return Status;
  }

  for (Index = 0; mNonSmiSecureBootFunctionsTable[Index].VariableName != NULL; Index++) {
    if (!StrCmp (mNonSmiSecureBootFunctionsTable[Index].VariableName, VariableName)) {
      Status = EFI_SUCCESS;
      break;
    }
  }

  if (Status == EFI_SUCCESS && FunctionIndex != NULL) {
    *FunctionIndex = Index;
  }

  return Status;
}

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
  )
{
  EFI_STATUS       Status;

  Status = GetSecureBootFunctionIndex (
             VariableName,
             VendorGuid,
             NULL
             );
 return Status == EFI_SUCCESS ? TRUE : FALSE;
}

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
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     VOID            *Data,
  IN     UINTN           DataSize,
  IN     UINT32          Attributes
  )
{
  EFI_STATUS            Status;
  UINTN                 FunctionIndex;


  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetSecureBootFunctionIndex (VariableName, VendorGuid, &FunctionIndex);
  if (!EFI_ERROR (Status)) {
    Status = mNonSmiSecureBootFunctionsTable[FunctionIndex].SecureBootSubFunction (
                                                              VariableName,
                                                              VendorGuid,
                                                              Data,
                                                              DataSize,
                                                              Attributes
                                                              );
  }

  return Status;
}

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
  )
{
  if (Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE |
      EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_APPEND_WRITE)) {
    return EFI_INVALID_PARAMETER;
  }
  return CommonAddFileHashImage (
           EFI_IMAGE_SECURITY_DATABASE,
           &gEfiImageSecurityDatabaseGuid,
           Data,
           DataSize,
           Attributes
           );
}

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
  )
{
  if (Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS)) {
    return EFI_INVALID_PARAMETER;
  }

  return CommoneUpdateSecureBootEnforce (
           VariableName,
           VendorGuid,
           Data,
           DataSize
           );
}

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
  )
{

  if (DataSize != (AUTHINFO_SIZE + sizeof (UINT8))) {
     return EFI_UNSUPPORTED;
  }
  //
  // Cehck input is whether want to clear all secure settings
  //
  if (*((UINT8 *) Data + AUTHINFO_SIZE) != 1) {
    return EFI_UNSUPPORTED;
  }

  return CommonClearAllSecureSettings ();
}

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
  )
{

  if (Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS)) {
    return EFI_INVALID_PARAMETER;
  }

  return CommoneRestoreFactoryDefault (
           VariableName,
           VendorGuid,
           Data,
           DataSize
           );
}

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
  )
{
  EFI_STATUS     Status;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  if (Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid,
             DataSize,
             Data,
             UPDATE_AUTHENTICATED_VARIABLE
             );
  return Status;
}

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
  )
{
  EFI_STATUS     Status;
  UINT8          UpdateType;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  if (Attributes == (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    UpdateType = UPDATE_AUTHENTICATED_VARIABLE;
  } else if (Attributes == (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_APPEND_WRITE)) {
    UpdateType = APPEND_AUTHENTICATED_VARIABLE;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_KEY_EXCHANGE_KEY_NAME,
             &gEfiGlobalVariableGuid,
             DataSize,
             Data,
             UpdateType
             );
  return Status;
}

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
  )
{
  EFI_STATUS     Status;
  UINT8          UpdateType;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  if (Attributes == (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    UpdateType = UPDATE_AUTHENTICATED_VARIABLE;
  } else if (Attributes == (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_APPEND_WRITE)) {
    UpdateType = APPEND_AUTHENTICATED_VARIABLE;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_IMAGE_SECURITY_DATABASE,
             &gEfiImageSecurityDatabaseGuid,
             DataSize,
             Data,
             UpdateType
             );
  return Status;
}

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
{
  EFI_STATUS     Status;
  UINT8          UpdateType;

  if (!PcdGetBool(PcdUpdateSecureBootVariablesSupported)) {
    return EFI_UNSUPPORTED;
  }

  if (Attributes == (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    UpdateType = UPDATE_AUTHENTICATED_VARIABLE;
  } else if (Attributes == (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_APPEND_WRITE)) {
    UpdateType = APPEND_AUTHENTICATED_VARIABLE;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  Status = InternalUpdateSecureDatabaseVariable (
             EFI_IMAGE_SECURITY_DATABASE1,
             &gEfiImageSecurityDatabaseGuid,
             DataSize,
             Data,
             UpdateType
             );
  return Status;
}

/**
  Initialize all of administer secure boot relative authenticated varialbes.

  @retval EFI_SUCCESS  Update all of secure boot relative authenticated varialbes successful
  @retval Other        The driver failded to start the device

**/
EFI_STATUS
InitializeAdministerSecureBootVariables (
  VOID
  )
{
  UINTN                            VariableCount;
  VARIABLE_POINTER_TRACK           Variable;
  EFI_STATUS                       Status;

  //
  // create all of Administer secure boot variables if they don't exist to prevent from other tool or
  // application can set these variables.
  //
  VariableCount = 0;
  FindVariableByLifetime (
    EFI_SECURE_BOOT_ENFORCE_NAME,
    &gEfiGenericVariableGuid,
    &Variable,
    &VariableCount,
    &mVariableModuleGlobal->VariableBase
    );
  if (Variable.CurrPtr == NULL) {
    Status = UpdateSecureBootEnforceVariable (1);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  VariableCount = 0;
  FindVariableByLifetime (
    EFI_RESTORE_FACOTRY_DEFAULT_NAME,
    &gEfiGenericVariableGuid,
    &Variable,
    &VariableCount,
    &mVariableModuleGlobal->VariableBase
    );
  if (Variable.CurrPtr == NULL) {
    Status = UpdateRestoreFactoryDefaultVariable (0);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  VariableCount = 0;
  FindVariableByLifetime (
    EFI_CUSTOM_SECURITY_NAME,
    &gEfiGenericVariableGuid,
    &Variable,
    &VariableCount,
    &mVariableModuleGlobal->VariableBase
    );
  if (Variable.CurrPtr == NULL) {
    Status = UpdateCustomSecurityStatus (0);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    Status = UpdateCustomSecurityStatus (*(GetVariableDataPtr (Variable.CurrPtr)));
  }

  return EFI_SUCCESS;
}

/**
  According input data to update L"SecureBootEnforce" variable

  @param  Data         Input data to update "SecureBootEnforce" variable

  @retval EFI_SUCCESS  update "SecureBootEnforce" successful.
  @retval Other        The driver failded to start the device

**/
EFI_STATUS
UpdateSecureBootEnforceVariable (
  UINT8           Data
  )
{
  return InternalUpdateAuthVariable (
           EFI_SECURE_BOOT_ENFORCE_NAME,
           &gEfiGenericVariableGuid,
           sizeof (UINT8),
           &Data,
           FALSE
           );
}
/**
  According input data to update L"RestoreFactoryDefault" variable

  @param  Data         Input data to update "RestoreFactoryDefault" variable

  @retval EFI_SUCCESS  update "RestoreFactoryDefault" successful.
  @retval Other        The driver failded to start the device

**/
EFI_STATUS
UpdateRestoreFactoryDefaultVariable (
  UINT8           Data
  )
{
  return InternalUpdateAuthVariable (
           EFI_RESTORE_FACOTRY_DEFAULT_NAME,
           &gEfiGenericVariableGuid,
           sizeof (UINT8),
           &Data,
           FALSE
           );
}


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
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VariableCount;
  UINT8                   VendorKeyData;

  if (Data != 0 && Data != 1) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Update "CustomSecurity" variable
  //
  Status = InternalUpdateAuthVariable (
             EFI_CUSTOM_SECURITY_NAME,
             &gEfiGenericVariableGuid,
             sizeof (UINT8),
             &Data,
             FALSE
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Update "VendorKeys" variable
  //
  VendorKeyData = Data == 0 ? 1 : 0;
  VariableCount = 0;
  Status = FindVariableByLifetime (
             VENDOR_KEYS_NAME,
             &gEfiGlobalVariableGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );

  Status  = UpdateVariable (
              VENDOR_KEYS_NAME,
              &gEfiGlobalVariableGuid,
              &VendorKeyData,
              sizeof (UINT8),
              EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
              0,
              0,
              &Variable,
              NULL,
              &mVariableModuleGlobal->VariableBase
              );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;


}



