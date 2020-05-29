/** @file
    Provide support functions for insyde secure variable.

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

#include "InsydeSecureVariable.h"
#include "AuthService.h"

/**
  Check the input Insyde variable password size is correct.

  @param[in] Data      Pointer to a buffer of the variable contents.
  @param[in] DataSize  Size of Data buffer.

  @retval TRUE         The password size is correct.
  @retval FALSE        The password size is incorrect.
**/
STATIC
BOOLEAN
IsPasswordSizeCorrect (
  IN CONST UINT8  *Data,
  IN       UINTN  DataSize
  )
{
  UINT8   PasswordLen;

  if (Data == NULL) {
    return FALSE;
  }

  PasswordLen = *(Data + INSYDE_VARIABLE_PASSWORD_HEADER_SIZE);
  if (INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + sizeof (UINT8) + PasswordLen + sizeof (UINT8) != DataSize) {
    return FALSE;
  }
  return TRUE;
}


/**
  Check Insyde password state is locked or unlocked.

  @retval TRUE             Insyde password is in locked state.
  @retval FALSE            Insyde password is in unloced state.
**/
STATIC
BOOLEAN
IsInsydePasswordLocked (
  VOID
  )
{
  if (mSmst == NULL) {
    return mVariableModuleGlobal->InsydeSecureVariableLocked;
  } else {
    return mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->InsydeSecureVariableLocked;
  }
}

/**
  Change the Insyde password state to unlocked state.

  @retval EFI_SUCCESS          Change Insyde variable state to unlocked state.
  @retval EFI_ALREADY_STARTED  The password state is already in unlocked state.
**/
STATIC
EFI_STATUS
UnlockInsydePassword (
  VOID
  )
{
  if (!IsInsydePasswordLocked ()) {
    return EFI_ALREADY_STARTED;
  }

  if (mSmst == NULL) {
    mVariableModuleGlobal->InsydeSecureVariableLocked = FALSE;
  } else {
    mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->InsydeSecureVariableLocked = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  Change the Insyde password state to locked state.

  @retval EFI_SUCCESS          Change Insyde variable state to locked state.
  @retval EFI_ALREADY_STARTED  The password state is already in locked state.
**/
STATIC
EFI_STATUS
LockInsydePassword (
  VOID
  )
{

  if (IsInsydePasswordLocked ()) {
    return EFI_ALREADY_STARTED;
  }

  if (mSmst == NULL) {
    mVariableModuleGlobal->InsydeSecureVariableLocked = TRUE;
  } else {
    mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->InsydeSecureVariableLocked = TRUE;
  }

  return EFI_SUCCESS;
}

/**
  This function uses input data to calculate encrypted hash data. User need allocate buffer
  for EncryptedHashData buffer before invoking this function.

  @param[in]  Data               Pointer to input originla data buffer.
  @param[out] EncryptedHashData  Pointer to output encrypted hash data buffer.

  @retval EFI_SUCCESS            The hash data is same as the content of AdminPassword.
  @retval EFI_UNSUPPORTED        System doesn't support hash protocol.
  @retval EFI_INVALID_PARAMETER  Data or EncryptedHashData is NULL.
**/
STATIC
EFI_STATUS
GetEncryptedHashData (
  IN   VOID      *Data,
  OUT  VOID      *EncryptedHashData
   )
{
  EFI_STATUS             Status;
  EFI_HASH_OUTPUT        Digest;
  UINTN                  PasswordLen;
  UINT8                  *WorkingData;
  CHAR8                  CombinedData[CERT_DATA_SIZE];

  if (Data == NULL || EncryptedHashData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mHash == NULL) {
    return EFI_UNSUPPORTED;
  }
  WorkingData = (UINT8 *) Data;
  PasswordLen = (UINTN) *WorkingData;
  ZeroMem (CombinedData, CERT_DATA_SIZE);
  GetCombinedData (WorkingData, PasswordLen + 1, CombinedData);
  Digest.Sha256Hash = (EFI_SHA256_HASH *) EncryptedHashData;

  if (PasswordLen + 1 > CERT_DATA_SIZE) {
    Status = mHash->Hash (
                      mHash,
                      &gEfiHashAlgorithmSha256Guid,
                      FALSE,
                      (UINT8 *)CombinedData,
                      CERT_DATA_SIZE,
                      (EFI_HASH_OUTPUT *) &Digest
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = mHash->Hash (
                      mHash,
                      &gEfiHashAlgorithmSha256Guid,
                      TRUE,
                      &WorkingData[CERT_DATA_SIZE],
                      PasswordLen + 1 - CERT_DATA_SIZE,
                      (EFI_HASH_OUTPUT *) &Digest
                      );
  } else {
    Status = mHash->Hash (
                      mHash,
                      &gEfiHashAlgorithmSha256Guid,
                      FALSE,
                      (UINT8 *)CombinedData,
                      CERT_DATA_SIZE,
                      (EFI_HASH_OUTPUT *) &Digest
                      );

  }
  return Status;

}

/**
  This function uses to hash input data to compare with hash data which saved in AdminPassword
  variable. If AdminPassword doesn't exit, return EFI_SUCCESS directly.

  @param[in] Data                 Pointer to data buffer.

  @retval EFI_SUCCESS             The hash data is same as the content of AdminPassword.
  @retval EFI_UNSUPPORTED         System doesn't support hash protocol.
  @retval EFI_SECURITY_VIOLATION  The hash data isn't same as the content of AdminPassword.
**/
STATIC
EFI_STATUS
CheckWithAdminPassword (
  IN   VOID      *Data
  )
{
  EFI_STATUS                         Status;
  UINTN                              VariableCount;
  VARIABLE_GLOBAL                    *Global;
  VARIABLE_POINTER_TRACK             Variable;

  VariableCount = 0;
  Global = &mVariableModuleGlobal->VariableBase;
  Status = FindVariableByLifetime (EFI_ADMIN_PASSWORD_NAME, &gInsydeSecureFirmwarePasswordGuid, &Variable, &VariableCount, Global);

  if (Variable.CurrPtr == NULL) {
    Status = EFI_SUCCESS;
  } else {
    Status = GetEncryptedHashData (Data, mSha256Hash);
    if (!EFI_ERROR (Status)) {
      if (CompareMem (GetVariableDataPtr (Variable.CurrPtr), mSha256Hash, SHA256_DIGEST_SIZE) != 0) {
        Status = EFI_SECURITY_VIOLATION;
      }
    }
  }

  return Status;
}

/**
  This function uses to execute unlock admin password process.

  @param[in] VariableName         A CHAR16 string of varialbe name.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Attributes           Attribute value of the variable found.
  @param[in] DataSize             Size of Data buffer.
  @param[in] Data                 Pointer to a buffer of the variable contents.

  @retval EFI_SUCCESS             System changes admin password state to unlocked successful.
  @retval EFI_INVALID_PARAMETER   Any input parameter is NULL.
  @retval EFI_UNSUPPORTED         The input data ins't a valid unlock admin password variable.
**/
STATIC
EFI_STATUS
ExecuteUnlockPasswordProcess (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid,
  IN        UINT32                             Attributes,
  IN        UINTN                              DataSize,
  IN CONST  VOID                               *Data
  )
{
  EFI_STATUS                         Status;
  EFI_VARIABLE_AUTHENTICATION        *CertData;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsUnlockPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return EFI_UNSUPPORTED;
  }

  CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;
  Status = CheckWithAdminPassword ((VOID *) &CertData->AuthInfo.CertData);
  if (!EFI_ERROR (Status)) {
    Status = UnlockInsydePassword ();
  }

  return Status;
}

/**
  This function uses to update the contents of admin password.

  @param[in] VariableName         A CHAR16 string of varialbe name.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Pointer to a buffer of the variable contents.
  @param[in] DataSize             Size of Data buffer.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable found.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             System updates admin password successful.
  @retval EFI_INVALID_PARAMETER   Any input parameter is NULL.
  @retval EFI_UNSUPPORTED         The input data ins't a valid admin password variable.
**/
STATIC
EFI_STATUS
UpdateAdminPassword (
  IN CONST CHAR16                             *VariableName,
  IN CONST EFI_GUID                           *VendorGuid,
  IN CONST VOID                               *Data,
  IN       UINTN                              DataSize,
  IN       VARIABLE_POINTER_TRACK             *Variable,
  IN       UINT32                             Attributes,
  IN       VARIABLE_GLOBAL                    *Global
  )
{
  EFI_VARIABLE_AUTHENTICATION        *CertData;
  EFI_STATUS                         Status;
  EFI_TIME                           TimeStamp;
  UINTN                              UpdateDatasize;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL ||
      Variable == NULL || Global == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (!IsValidAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return EFI_UNSUPPORTED;
  }

  CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;
  Status = GetEncryptedHashData (&CertData->AuthInfo.CertData, mSha256Hash);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UpdateDatasize = SHA256_DIGEST_SIZE;

  if (IsDeleteAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    //
    // Set DataSize to 0 to delete original variable.
    //
    UpdateDatasize = 0;
  }
  //
  // Set all of byte to 0xFF in EFI_TIME in variable header to indicate this is a insyde secure variable.
  //
  SetMem (&TimeStamp, sizeof (EFI_TIME), 0xFF);
  Status = UpdateVariable (
             VariableName,
             VendorGuid,
             (VOID *) mSha256Hash,
             UpdateDatasize,
             Attributes,
             0,
             0,
             Variable,
             &TimeStamp,
             Global
             );

  return Status;
}

/**
  This function uses to update normal Insyde secure variable.

  @param[in] VariableName         A CHAR16 string of varialbe name.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Pointer to a buffer of the variable contents.
  @param[in] DataSize             Size of Data buffer.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable found.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             System updates normal Insyde secure variable successful.
  @retval EFI_INVALID_PARAMETER   Any input parameter is NULL.
  @retval EFI_UNSUPPORTED         The input contents isn't a valid normal Insyde secure variable.
**/
STATIC
EFI_STATUS
UpdateNormalInsydeVariable (
  IN CONST CHAR16                             *VariableName,
  IN CONST EFI_GUID                           *VendorGuid,
  IN CONST VOID                               *Data,
  IN       UINTN                              DataSize,
  IN       VARIABLE_POINTER_TRACK             *Variable,
  IN       UINT32                             Attributes,
  IN       VARIABLE_GLOBAL                    *Global
  )
{
  EFI_TIME             TimeStamp;
  UINTN                UpdateDatasize;
  UINT8                *UpdateData;
  EFI_STATUS           Status;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL ||
      Variable == NULL || Global == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (!IsNormalInsydeSecureVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return EFI_UNSUPPORTED;
  }

  UpdateDatasize = DataSize - INSYDE_VARIABLE_HEADER_SIZE;
  UpdateData = (UINT8 *) Data;
  UpdateData += INSYDE_VARIABLE_HEADER_SIZE;
  //
  // Set all of byte to 0xFF in EFI_TIME in variable header to indicate this is a insyde secure variable.
  //
  SetMem (&TimeStamp, sizeof (EFI_TIME), 0xFF);
  Status = UpdateVariable (
             VariableName,
             VendorGuid,
             UpdateData,
             UpdateDatasize,
             Attributes,
             0,
             0,
             Variable,
             &TimeStamp,
             Global
             );
  return Status;
}

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
  )
{
  if (IsValidCheckAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data) ||
      IsValidAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data) ||
      IsNormalInsydeSecureVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  if (IsUnlockPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data) ||
      IsLockPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return TRUE;
  }

  return FALSE;

}

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
  )
{
  UINT8                         *Action;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return FALSE;
  }

  if (!IsCheckAdminPasswordVariable (VariableName, VendorGuid)) {
    return FALSE;
  }

  if (IsPasswordSizeCorrect (Data, DataSize)) {
    Action = (UINT8 *) Data;
    Action += (DataSize - sizeof (UINT8));
    if (IsInsydeSecureVariable (Attributes, DataSize, Data) && *Action == UNLOCK_SECURE_VARIABLE) {
      return TRUE;
    }
  }

  return FALSE;

}

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
  )
{
  UINT8                         *Action;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return FALSE;
  }

  if (!IsCheckAdminPasswordVariable (VariableName, VendorGuid)) {
    return FALSE;
  }

  if (DataSize == LOCK_PASSWORD_VARIABLE_SIZE) {
    Action = (UINT8 *) Data;
    Action += (LOCK_PASSWORD_VARIABLE_SIZE - sizeof (UINT8));
    if (IsInsydeSecureVariable (Attributes, DataSize, Data) && *Action == LOCK_SECURE_VARIABLE) {
      return TRUE;
    }
  }

  return FALSE;

}

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
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_CHECK_ADMIN_PASSWORD_NAME) == 0 && CompareGuid (VendorGuid, &gInsydeSecureFirmwarePasswordGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

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
  IN       UINT32                          Attributes,
  IN       UINTN                           DataSize,
  IN CONST VOID                            *Data
  )
{
  if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) ==  EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS &&
      IsInsydeSecureType (DataSize, Data)) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  EFI_VARIABLE_AUTHENTICATION   *CertData;

  if (Data == NULL) {
    return FALSE;
  }

  if (DataSize >= INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + sizeof (UINT8)) {
    CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;
    if (CertData->AuthInfo.Hdr.wCertificateType == WIN_CERT_TYPE_EFI_GUID &&
        CompareGuid (&CertData->AuthInfo.CertType, &gIbvCertTypeInsydeSecureGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

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
  )
{
  if (IsDeleteAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data) ||
      IsReplaceAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return FALSE;
  }

  if (!IsAdminPasswordVariable (VariableName, VendorGuid)) {
    return FALSE;
  }

  if (IsInsydeSecureVariable (Attributes, DataSize, Data) && DataSize == DELETE_PASSWORD_VARIABLE_SIZE) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  UINT8         *WorkingPtr;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return FALSE;
  }

  if (!IsAdminPasswordVariable (VariableName, VendorGuid)) {
    return FALSE;
  }

  WorkingPtr = (UINT8 *) Data;
  if (IsInsydeSecureVariable (Attributes, DataSize, Data) && IsPasswordSizeCorrect (Data, DataSize) &&
      WorkingPtr[DataSize - 1] == REPLACE_ADMIN_PASSWORD) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_ADMIN_PASSWORD_NAME) == 0 && CompareGuid (VendorGuid, &gInsydeSecureFirmwarePasswordGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

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
  )
{
  if (VariableName == NULL || VendorGuid == NULL || Data == NULL) {
    return FALSE;
  }

  if (IsInsydeSecureVariable (Attributes, DataSize, Data) && !IsCheckAdminPasswordVariable (VariableName, VendorGuid) &&
      !IsAdminPasswordVariable (VariableName, VendorGuid)) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  EFI_STATUS           Status;

  if (VariableName == NULL || VendorGuid == NULL || Data == NULL ||
      Variable == NULL || Global == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Variable->CurrPtr != NULL && !IsExistingInsydeSecureVariable (Variable)) {
    //
    // If caller attempts to change attributes from Insyde secure variable to other attributes, just return
    // EFI_WRITE_PROTECTED directly.
    //
    return EFI_WRITE_PROTECTED;
  }

  if (!IsValidInsydeSecureVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return EFI_UNSUPPORTED;
  }

  if (IsInsydePasswordLocked () &&
      !IsUnlockPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    return EFI_WRITE_PROTECTED;
  }

  if (IsUnlockPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    Status = ExecuteUnlockPasswordProcess (VariableName, VendorGuid, Attributes, DataSize, Data);
  } else if (IsLockPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    Status = LockInsydePassword ();
  } else if (IsValidAdminPasswordVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
    Status = UpdateAdminPassword (VariableName, VendorGuid, Data, DataSize, Variable, Attributes, Global);
  } else {
    Status = UpdateNormalInsydeVariable (VariableName, VendorGuid, Data, DataSize, Variable, Attributes, Global);
  }

  return Status;
}

/**
  This function uses VARIABLE_POINTER_TRACK to check this variable is whether an existing insyde secure variable.

  @param[in] Variable  The variable information which is used to keep track of variable usage.

  @retval TRUE         The variable is an existing Insyde secure variable.
  @retval FALSE        The variable isn't an existing Insyde secure variable.
**/
BOOLEAN
IsExistingInsydeSecureVariable (
  IN CONST VARIABLE_POINTER_TRACK               *Variable
  )
{
  EFI_TIME                           TimeStamp;

   if (Variable == NULL || Variable->CurrPtr == NULL) {
    return FALSE;
   }

   SetMem (&TimeStamp, sizeof (EFI_TIME), 0xFF);
   if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0 ||
        CompareMem (&TimeStamp, &Variable->CurrPtr->TimeStamp, sizeof (EFI_TIME)) != 0) {
     return FALSE;
   }

   return TRUE;

}

