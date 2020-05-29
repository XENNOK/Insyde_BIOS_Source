/** @file
    Provide authentication services for the authenticated variable service

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


#include "Variable.h"
#include "AuthService.h"
#include "InsydeSecureVariable.h"
#include "Reclaim.h"
#include <KernelSetupConfig.h>

//
// Global database array for scratch
//
UINT8                             mPubKeyStore[MAX_KEYDB_SIZE];
UINT32                            mPubKeyNumber;
UINT32                            mPlatformMode;
UINT32                            mPlatformBootMode;
CRYPTO_SERVICES_PROTOCOL          *mCryptoService = NULL;
EFI_HASH_PROTOCOL                 *mHash = NULL;
VOID                              *mSha256Hash;

EFI_GUID mSignatureSupport[SIGSUPPORT_NUM] = {EFI_CERT_RSA2048_SHA256_GUID, EFI_CERT_RSA2048_SHA1_GUID};
//
// Public Exponent of RSA Key.
//
CONST UINT8 mRsaE[] = { 0x01, 0x00, 0x01 };

SIGNATURE_SUPPORT_INFO         mSupportSigItem[] = {
  {EFI_CERT_RSA2048_GUID,           256 + sizeof (EFI_GUID),                TRUE,  FALSE},
  {EFI_CERT_X509_GUID,              0,                                      FALSE, FALSE},
  {EFI_CERT_SHA1_GUID,              SHA1_DIGEST_SIZE + sizeof (EFI_GUID),   TRUE,  TRUE },
  {EFI_CERT_SHA224_GUID,            28 + sizeof (EFI_GUID),                 TRUE,  TRUE },
  {EFI_CERT_SHA256_GUID,            SHA256_DIGEST_SIZE + sizeof (EFI_GUID), TRUE,  TRUE },
  {EFI_CERT_SHA384_GUID,            48 + sizeof (EFI_GUID),                 TRUE,  TRUE },
  {EFI_CERT_SHA512_GUID,            64 + sizeof (EFI_GUID),                 TRUE,  TRUE },
  {EFI_CERT_RSA2048_SHA1_GUID,      256 + sizeof (EFI_GUID),                TRUE,  TRUE },
  {EFI_CERT_RSA2048_SHA256_GUID,    256 + sizeof (EFI_GUID),                TRUE,  TRUE }
};

/**
  Internal function to delete a Variable given its name and GUID, no authentication
  required.

  @param[in] VariableName    Name of the Variable.
  @param[in] VendorGuid      GUID of the Variable.
  @param[in] Global          Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS        Variable deleted successfully.
  @return Others             The driver failded to start the device.
**/
STATIC
EFI_STATUS
DeleteVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid,
  IN  VARIABLE_GLOBAL           *Global
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VariableCount;


  Status = FindVariableByLifetime (VariableName, VendorGuid, &Variable, &VariableCount, Global);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  ASSERT (Variable.CurrPtr != NULL);
  return UpdateVariable (VariableName, VendorGuid, NULL, 0, 0, 0, 0, &Variable, NULL, Global);
}

/**
  Convert all of module authenticated service relative pointers to virtual address.
**/
VOID
AuthVariableClassAddressChange (
  VOID
)
{
  gRT->ConvertPointer (0x0, (VOID **) &mCertDbList);
  gRT->ConvertPointer (0x0, (VOID **) &mStorageArea);
  gRT->ConvertPointer (0x0, (VOID **) &mSha256Hash);
  gRT->ConvertPointer (0x0, (VOID **) &mCryptoService);
  gRT->ConvertPointer (0x0, (VOID **) &mHash);
}

/**
  This funciton uses to clear all of secure settings. These variable
  includes PK, KEK, db, dbx.

  @param[in] Global    Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS  Clear secure settings successful.
--*/
EFI_STATUS
ClearSecureSettings (
  IN  VARIABLE_GLOBAL  *Global
  )
{
  UINTN                   VariableCount;
  VARIABLE_POINTER_TRACK  Variable;
  UINT32                  VarAttr;
  EFI_STATUS              Status;

  VariableCount = 0;
  FindVariableByLifetime (
    EFI_SETUP_MODE_NAME,
    &gEfiGlobalVariableGuid,
    &Variable,
    &VariableCount,
    Global
    );

  mPlatformMode = SETUP_MODE;
  if (Variable.CurrPtr != NULL) {
    VarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
    Status  = UpdateVariable (
                EFI_SETUP_MODE_NAME,
                &gEfiGlobalVariableGuid,
                &mPlatformMode,
                0,
                VarAttr,
                0,
                0,
                &Variable,
                NULL,
                Global
                );
    ASSERT_EFI_ERROR (Status);
  }
  //
  // Clear PK, KEK, db and dbx
  //
  Status = DeleteVariable (EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid, Global);
  ASSERT_EFI_ERROR (Status);
  Status = DeleteVariable (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid, Global);
  ASSERT_EFI_ERROR (Status);
  Status = DeleteVariable (EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid, Global);
  ASSERT_EFI_ERROR (Status);
  Status = DeleteVariable (EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid, Global);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  This function only uses for create dummy dbx data when dbx variable doesn't exist.

  @param[in, out] BufferSize      On input, this is input buffer size.
                                  On output, this is buffer size of dummy data.
  @param[out]     Buffer          Pointer to buffer to save dummy dbx data.

  @retval EFI_SUCCESS             Create dummy dbx data successful.
  @retval EFI_INVALID_PARAMETER   BufferSize or Buffer is NULL.
  @retval EFI_BUFFER_TOO_SMALL    Input buffer size is too small.

**/
STATIC
EFI_STATUS
CreateDummyDbxData (
  IN OUT   UINTN    *BufferSize,
  OUT      VOID     *Buffer
  )
{
  EFI_SIGNATURE_LIST      *SignatureList;
  UINT32                   RequestBufferSize;

  if (BufferSize == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RequestBufferSize = sizeof (EFI_SIGNATURE_LIST) + SHA256_DIGEST_SIZE + sizeof (EFI_GUID);
  if (*BufferSize < RequestBufferSize) {
    *BufferSize = RequestBufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *BufferSize   = RequestBufferSize;
  SignatureList = Buffer;
  ZeroMem (SignatureList, RequestBufferSize);
  SignatureList->SignatureListSize = RequestBufferSize;
  SignatureList->SignatureSize = RequestBufferSize - sizeof (EFI_SIGNATURE_LIST);
  CopyMem (&SignatureList->SignatureType, &gEfiCertSha256Guid, sizeof (EFI_GUID));

  return EFI_SUCCESS;
}

/**
  Initializes for authenticated varibale service.

  @param[in] Global            Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resources.
**/
EFI_STATUS
AutenticatedVariableServiceInitialize (
  IN  VARIABLE_GLOBAL         *Global
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINT32                  VarAttr;
  UINTN                   VariableCount;
  EFI_TIME                TimeStamp;
  UINTN                   VariableSize;
  UINT32                  ListSize;

  if (mSmst == NULL) {
    Status = gBS->LocateProtocol (
                    &gCryptoServicesProtocolGuid,
                    NULL,
                    (VOID **)&mCryptoService
                    );

    ASSERT (mCryptoService != NULL);

    Status = gBS->LocateProtocol (
                    &gEfiHashProtocolGuid,
                    NULL,
                    (VOID **)&mHash
                    );

    ASSERT (mHash != NULL);
  } else {
    Status = mSmst->SmmLocateProtocol (
                      &gCryptoServicesProtocolGuid,
                      NULL,
                      (VOID **)&mCryptoService
                      );
    ASSERT (mCryptoService != NULL);

    Status = mSmst->SmmLocateProtocol (
                      &gEfiHashProtocolGuid,
                      NULL,
                      (VOID **)&mHash
                      );
    ASSERT (mHash != NULL);
  }

  SyncAuthData (Global);
  mSha256Hash = VariableAllocateZeroBuffer (SHA256_DIGEST_SIZE, TRUE);
  //
  // Check PK database's existence to determine the SetupMode value.
  // Then create a new "SetupMode" with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  //
  VariableCount = 0;
  Status = FindVariableByLifetime (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );

  VarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
  Status  = UpdateVariable (
              EFI_SETUP_MODE_NAME,
              &gEfiGlobalVariableGuid,
              &mPlatformMode,
              sizeof(UINT8),
              VarAttr,
              0,
              0,
              &Variable,
              NULL,
              Global
              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check "SecureBoot" variable existence.  This variable must match the platform booting mode.
  // With current code, the setting depends only on the presence of the PK variable.
  // NOTE: if there is any setup option to skip secure boot, that must be checked here!!!
  //
  if (mSmst == NULL) {
    VariableCount = 0;
    Status = FindVariableByLifetime (
               EFI_SECURE_BOOT_MODE_NAME,
               &gEfiGlobalVariableGuid,
               &Variable,
               &VariableCount,
               Global
               );
    VarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
    Status  = UpdateVariable (
                EFI_SECURE_BOOT_MODE_NAME,
                &gEfiGlobalVariableGuid,
                &mPlatformBootMode,
                sizeof (UINT8),
                VarAttr,
                0,
                0,
                &Variable,
                NULL,
                Global
                );
    if (EFI_ERROR (Status)) {
      return Status;
    }


    //
    // Check "SignatureSupport" variable's existence.
    // If it doesn't exist, then create a new one with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
    //
    VariableCount = 0;
    Status = FindVariableByLifetime (
               EFI_SIGNATURE_SUPPORT_NAME,
               &gEfiGlobalVariableGuid,
               &Variable,
               &VariableCount,
               Global
               );

    if (Variable.CurrPtr == NULL) {
      VarAttr = EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS;
      Status  = UpdateVariable (
                  EFI_SIGNATURE_SUPPORT_NAME,
                  &gEfiGlobalVariableGuid,
                  mSignatureSupport,
                  SIGSUPPORT_NUM * sizeof (EFI_GUID),
                  VarAttr,
                  0,
                  0,
                  &Variable,
                  NULL,
                  Global
                  );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // Check "certdb" variable's existence.
    // If it doesn't exist, then create a new one with
    // EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set.
    //
    VariableCount = 0;
    Status = FindVariableByLifetime (
               EFI_CERT_DB_NAME,
               &gEfiGenericVariableGuid,
               &Variable,
               &VariableCount,
               Global
               );

    if (Variable.CurrPtr == NULL) {
      VarAttr  = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
      ListSize = 0;
      ZeroMem (&TimeStamp, sizeof (TimeStamp));
      Status   = UpdateVariable (
                   EFI_CERT_DB_NAME,
                   &gEfiGenericVariableGuid,
                   &ListSize,
                   sizeof (UINT32),
                   VarAttr,
                   0,
                   0,
                   &Variable,
                   &TimeStamp,
                   Global
                   );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // Check "dbx" variable's existence.
    // If it doesn't exist, then create a new empty one with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set.
    //
    VariableCount = 0;
    Status = FindVariableByLifetime (
               EFI_IMAGE_SECURITY_DATABASE1,
               &gEfiImageSecurityDatabaseGuid,
               &Variable,
               &VariableCount,
               Global
               );

    if (Variable.CurrPtr == NULL) {
      VariableSize = APPEND_BUFF_SIZE;
      Status = CreateDummyDbxData (&VariableSize, mStorageArea);
      ASSERT_EFI_ERROR (Status);
      VarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
      ZeroMem (&TimeStamp, sizeof (TimeStamp));
      Status  = UpdateVariable (
                  EFI_IMAGE_SECURITY_DATABASE1,
                  &gEfiImageSecurityDatabaseGuid,
                  mStorageArea,
                  VariableSize,
                  VarAttr,
                  0,
                  0,
                  &Variable,
                  &TimeStamp,
                  Global
                  );
    }
  }

  Status = InitializeAdministerSecureBootVariables ();
  return Status;
}

/**
  Check the PubKeyIndex is a valid key or not.

  This function will iterate the NV storage to see if this PubKeyIndex is still referenced
  by any valid count-based auth variabe.

  @param[in] PubKeyIndex  Index of the public key in public key store.

  @retval TRUE            The PubKeyIndex is still in use.
  @retval FALSE           The PubKeyIndex is not referenced by any count-based auth variabe.
**/
STATIC
BOOLEAN
IsValidPubKeyIndex (
  IN CONST  UINT32      PubKeyIndex
  )
{
  VARIABLE_HEADER             *Variable;

  if (PubKeyIndex > mPubKeyNumber) {
    return FALSE;
  }
  Variable = GetStartPointer ((VARIABLE_STORE_HEADER *) ((UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase));
  while (IsValidVariableHeader (Variable)) {
    if ((Variable->State == VAR_ADDED || Variable->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) &&
        Variable->PubKeyIndex == PubKeyIndex) {
      return TRUE;
    }
    Variable = GetNextVariablePtr (Variable);
  }

  return FALSE;
}

/**
  Get the number of valid public key in PubKeyStore.

  @param[in]  PubKeyNumber     Number of the public key in public key store.

  @return     Number of valid public key in PubKeyStore.
**/
STATIC
UINT32
GetValidPubKeyNumber (
  IN CONST  UINT32       PubKeyNumber
  )
{
  UINT32       PubKeyIndex;
  UINT32       Counter;

  Counter = 0;

  for (PubKeyIndex = 1; PubKeyIndex <= PubKeyNumber; PubKeyIndex++) {
    if (IsValidPubKeyIndex (PubKeyIndex)) {
      Counter++;
    }
  }

  return Counter;
}

/**
  Filter the useless key in public key store.

  This function will find out all valid public keys in public key database, save them in new allocated
  buffer NewPubKeyStore, and give the new PubKeyIndex. The caller is responsible for freeing buffer
  NewPubKeyIndex and NewPubKeyStore with FreePool().

  @param[in] PubKeyStore      Point to the public key database.
  @param[in] PubKeyNumber     Number of the public key in PubKeyStore.
  @param[out] NewPubKeyIndex  Point to an array of new PubKeyIndex corresponds to NewPubKeyStore.
  @param[out] NewPubKeyStore  Saved all valid public keys in PubKeyStore.
  @param[out] NewPubKeySize   Buffer size of the NewPubKeyStore.

  @retval EFI_SUCCESS            Trim operation is complete successfully.
  @retval EFI_OUT_OF_RESOURCES   No enough memory resources, or no useless key in PubKeyStore.
**/
STATIC
EFI_STATUS
PubKeyStoreFilter (
  IN   CONST UINT8         *PubKeyStore,
  IN   CONST UINT32        PubKeyNumber,
  OUT        UINT32        **NewPubKeyIndex,
  OUT        UINT8         **NewPubKeyStore,
  OUT        UINT32        *NewPubKeySize
  )
{
  UINT32        PubKeyIndex;
  UINT32        CopiedKey;
  UINT32        NewPubKeyNumber;

  NewPubKeyNumber = GetValidPubKeyNumber (PubKeyNumber);
  if (NewPubKeyNumber == PubKeyNumber) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (NewPubKeyNumber != 0) {
    *NewPubKeySize = NewPubKeyNumber * EFI_CERT_TYPE_RSA2048_SIZE;
  } else {
    *NewPubKeySize = sizeof (UINT8);
  }

  *NewPubKeyStore = VariableAllocateZeroBuffer (*NewPubKeySize, FALSE);
  if (*NewPubKeyStore == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *NewPubKeyIndex = VariableAllocateZeroBuffer ((PubKeyNumber + 1) * sizeof (UINT32), FALSE);
  if (*NewPubKeyIndex == NULL) {
    EFI_FREE_POOL (*NewPubKeyStore);
    return EFI_OUT_OF_RESOURCES;
  }

  CopiedKey = 0;
  for (PubKeyIndex = 1; PubKeyIndex <= PubKeyNumber; PubKeyIndex++) {
    if (IsValidPubKeyIndex (PubKeyIndex)) {
      CopyMem (
        *NewPubKeyStore + CopiedKey * EFI_CERT_TYPE_RSA2048_SIZE,
        PubKeyStore + (PubKeyIndex - 1) * EFI_CERT_TYPE_RSA2048_SIZE,
        EFI_CERT_TYPE_RSA2048_SIZE
        );
      (*NewPubKeyIndex)[PubKeyIndex] = ++CopiedKey;
    }
  }
  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
ReclaimPubKeyDataBase (
  VOID
  )
/*++

Routine Description:

  Public key database garbage collection and reclaim operation.

  This function checks is whether any public key will be removed before reclaim public database.
  The reclaim operation only occurred if there are public keys will be removed from public key database.

Arguments:

  PubKeyStore    - Point to the public key database.
  PubKeyNumber   - Number of the public key in PubKeyStore.
  NewPubKeyIndex - Point to an array of new PubKeyIndex corresponds to NewPubKeyStore.
  NewPubKeyStore - Saved all valid public keys in PubKeyStore.
  NewPubKeySize  - Buffer size of the NewPubKeyStore.

Returns:

  EFI_SUCCESS          - Trim operation is complete successfully.
  EFI_OUT_OF_RESOURCES - No enough memory resources, or no useless key in PubKeyStore.

--*/
{
  EFI_STATUS                  Status;
  UINT32                      *NewPubKeyIndex;
  UINT8                       *NewPubKeyStore;
  UINT32                      NewPubKeySize;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  VARIABLE_HEADER             *PubKeyHeader;
  VARIABLE_HEADER             *Variable;
  VARIABLE_HEADER             *NextVariable;
  UINT8                       *ValidBuffer;
  UINTN                       VariableSize;
  UINTN                       TotalBufferSize;
  UINT8                       *CurrPtr;

  NewPubKeyIndex = NULL;
  NewPubKeyStore = NULL;
  NewPubKeySize  = 0;
  Status = PubKeyStoreFilter (
             mPubKeyStore,
             mPubKeyNumber,
             &NewPubKeyIndex,
             &NewPubKeyStore,
             &NewPubKeySize
             );
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  if (NewPubKeySize / EFI_CERT_TYPE_RSA2048_SIZE == MAX_KEYDB_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  TotalBufferSize     = GetNonVolatileVariableStoreSize ();
  ValidBuffer         = VariableAllocateZeroBuffer (TotalBufferSize, FALSE);
  if (ValidBuffer == NULL) {
    EFI_FREE_POOL (NewPubKeyIndex);
    EFI_FREE_POOL (NewPubKeyStore);
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem (ValidBuffer, TotalBufferSize, 0xff);
  CurrPtr             = ValidBuffer;
  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase);
  CopyMem (CurrPtr, VariableStoreHeader, GetVariableStoreHeaderSize ());
  CurrPtr        += GetVariableStoreHeaderSize ();
  for (Variable = GetStartPointer (VariableStoreHeader), PubKeyHeader = NULL;
       IsValidVariableHeader (Variable); Variable = NextVariable) {
    NextVariable = GetNextVariablePtr (Variable);
    //
    // Check the contents of this variable is in variable range.
    //
    if ((UINTN) NextVariable - (UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase > TotalBufferSize) {
      ASSERT (FALSE);
      break;
    }
    if (Variable->NameSize != StrSize (GET_VARIABLE_NAME_PTR (Variable))) {
      ASSERT (FALSE);
      continue;
    }
    if (Variable->State != VAR_ADDED && Variable->State != (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
      continue;
    }
    if ((StrCmp (GET_VARIABLE_NAME_PTR (Variable), AUTHVAR_KEYDB_NAME) == 0) &&
        (CompareGuid (&Variable->VendorGuid, &gEfiAuthenticatedVariableGuid))) {
      //
      // Skip the public key database, it will be reinstalled later.
      //
      PubKeyHeader = Variable;
      continue;
    }
    VariableSize = (UINTN) NextVariable - (UINTN) Variable;
    CopyMem (CurrPtr, (UINT8 *) Variable, VariableSize);
    ASSERT (((VARIABLE_HEADER*) CurrPtr)->PubKeyIndex <= NewPubKeySize / EFI_CERT_TYPE_RSA2048_SIZE);
    if (((VARIABLE_HEADER*) CurrPtr)->PubKeyIndex <= NewPubKeySize / EFI_CERT_TYPE_RSA2048_SIZE) {
      ((VARIABLE_HEADER*) CurrPtr)->PubKeyIndex = NewPubKeyIndex[Variable->PubKeyIndex];
    }
    CurrPtr += VariableSize;
  }

  if (PubKeyHeader == NULL) {
    EFI_FREE_POOL (NewPubKeyIndex);
    EFI_FREE_POOL (NewPubKeyStore);
    EFI_FREE_POOL (ValidBuffer);
    return EFI_NOT_FOUND;
  }
  //
  // Add new public key database.
  //
  CopyMem (CurrPtr, (UINT8*) PubKeyHeader, sizeof (VARIABLE_HEADER));
  Variable = (VARIABLE_HEADER*) CurrPtr;
  Variable->DataSize = NewPubKeySize;
  Variable->State    = VAR_ADDED;
  StrCpy (GET_VARIABLE_NAME_PTR (Variable), GET_VARIABLE_NAME_PTR (PubKeyHeader));
  CopyMem (GetVariableDataPtr (Variable), NewPubKeyStore, NewPubKeySize);
  CurrPtr = (UINT8*) GetNextVariablePtr (Variable);

  Status = FtwVariableSpace (mVariableModuleGlobal->VariableBase.NonVolatileVariableBase, ValidBuffer);

  if (!EFI_ERROR (Status)) {
    //
    // Synchronize current public key database to module variable.
    //
    CopyMem (mPubKeyStore, (UINT8 *) NewPubKeyStore, NewPubKeySize);
    mPubKeyNumber = (UINT32) (NewPubKeySize / EFI_CERT_TYPE_RSA2048_SIZE);
  }
  if (!EFI_ERROR (Status) && mVariableModuleGlobal->NonVolatileVariableCache != NULL) {
    //
    // Synchronize NV storage data to variable cache if reclaim successes.
    //
    SetMem (mVariableModuleGlobal->NonVolatileVariableCache, TotalBufferSize, 0xff);
    CopyMem (
      mVariableModuleGlobal->NonVolatileVariableCache,
      (VOID *) (UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
      (UINTN) (CurrPtr - ValidBuffer)
      );
  } else if (mVariableModuleGlobal->NonVolatileVariableCache != NULL){
    //
    // reclaim failed, turn off variable chache to prevent from data corrupted
    //
    DisableVariableCache ();
  }
  EFI_FREE_POOL (NewPubKeyIndex);
  EFI_FREE_POOL (NewPubKeyStore);
  EFI_FREE_POOL (ValidBuffer);
  return Status;
}

/**
  Add public key to database and return its index.

  @param[in]  PubKey             Input pointer to Public Key database.
  @param[in]  Global             Pointer to VARIABLE_GLOBAL instance.
  @param[out] KeyIndex           Pointer to output key index.

  @retval EFI_SUCCESS            Add public key to database successfully
  @retval EFI_OUT_OF_RESOURCES   Database is full.
  @retval EFI_NOT_FOUND          Cannot find AUTHVAR_KEYDB_NAME variable.
  @retval EFI_INVALID_PARAMETER  PubKey is NULL, Global is NULL or KeyIndex is NULL.
**/
EFI_STATUS
AddPubKeyInStore (
  IN  UINT8               *PubKey,
  IN  VARIABLE_GLOBAL     *Global,
  OUT UINT32              *KeyIndex
  )
{
  EFI_STATUS              Status;
  BOOLEAN                 IsFound;
  UINT32                  Index;
  VARIABLE_POINTER_TRACK  Variable;
  UINT8                   *Ptr;
  UINTN                   VariableCount=0;

  if (PubKey == NULL || Global == NULL || KeyIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VariableCount = 0;
  Status = FindVariableByLifetime (
             AUTHVAR_KEYDB_NAME,
             &gEfiAuthenticatedVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check whether the public key entry does exist.
  //
  IsFound = FALSE;
  for (Ptr = mPubKeyStore, Index = 1; Index <= mPubKeyNumber; Index++) {
    if (CompareMem (Ptr, PubKey, EFI_CERT_TYPE_RSA2048_SIZE) == 0) {
      IsFound = TRUE;
      break;
    }
    Ptr += EFI_CERT_TYPE_RSA2048_SIZE;
  }

  if (!IsFound) {
    //
    // Add public key in database.
    //
    if (mPubKeyNumber == MAX_KEY_NUM) {
      //
      // Public key dadatase is full, try to reclaim invalid key.
      //
      if (AtRuntime ()) {
        //
        // NV storage can't reclaim at runtime.
        //
        return EFI_OUT_OF_RESOURCES;
      }
      Status = ReclaimPubKeyDataBase ();
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = FindVariableByLifetime (
                 AUTHVAR_KEYDB_NAME,
                 &gEfiAuthenticatedVariableGuid,
                 &Variable,
                 &VariableCount,
                 Global
                 );
      ASSERT_EFI_ERROR (Status);
    }


    CopyMem (mPubKeyStore + mPubKeyNumber * EFI_CERT_TYPE_RSA2048_SIZE, PubKey, EFI_CERT_TYPE_RSA2048_SIZE);
    Index = ++mPubKeyNumber;
    //
    // Update public key database variable.
    //
    Status = UpdateVariable (
               AUTHVAR_KEYDB_NAME,
               &gEfiAuthenticatedVariableGuid,
               mPubKeyStore,
               mPubKeyNumber * EFI_CERT_TYPE_RSA2048_SIZE,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS,
               0,
               0,
               &Variable,
               NULL,
               Global
               );
    ASSERT_EFI_ERROR (Status);
  }
  if (!EFI_ERROR (Status)) {
    *KeyIndex = Index;
  }
  return Status;
}

/**
  Verify data payload with AuthInfo in EFI_CERT_TYPE_RSA2048_SHA256 type.
  Follow the steps in UEFI specification.

  @param[in] Data                  Pointer to data with AuthInfo.
  @param[in] DataSize              Size of Data by byte.
  @param[in] PubKey                Public key used for verification.

  @retval EFI_SUCCESS              Authentication successful.
  @retval EFI_INVALID_PARAMETER    Invalid parameter.
  @retval EFI_SECURITY_VIOLATION   If authentication failed.
**/
STATIC
EFI_STATUS
VerifyCounterBasedPayload (
  IN     UINT8          *Data,
  IN     UINTN          DataSize,
  IN     UINT8          *PubKey
  )
{
  EFI_STATUS                      Status;
  EFI_VARIABLE_AUTHENTICATION     *CertData;
  EFI_CERT_BLOCK_RSA_2048_SHA256  *CertBlock;
  EFI_HASH_OUTPUT                 Digest;
  VOID                            *Rsa;

  ASSERT (mCryptoService != NULL);

  if (Data == NULL || PubKey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Rsa       = NULL;
  CertData  = (EFI_VARIABLE_AUTHENTICATION *) Data;
  CertBlock = (EFI_CERT_BLOCK_RSA_2048_SHA256 *) (CertData->AuthInfo.CertData);

  //
  // wCertificateType should be WIN_CERT_TYPE_EFI_GUID.
  // Cert type should be EFI_CERT_TYPE_RSA2048_SHA256.
  //
  if ((CertData->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||
      !CompareGuid (&CertData->AuthInfo.CertType, &gEfiCertTypeRsa2048Sha256Guid)
      ) {
    //
    // Invalid AuthInfo type, return EFI_SECURITY_VIOLATION.
    //
    return EFI_SECURITY_VIOLATION;
  }
  //
  // Hash data payload with SHA256.
  //
  Digest.Sha256Hash = (EFI_SHA256_HASH *) mSha256Hash;

  Status = mHash->Hash (
                    mHash,
                    &gEfiHashAlgorithmSha256Guid,
                    FALSE,
                    Data + AUTHINFO_SIZE,
                    (UINTN) (DataSize - AUTHINFO_SIZE),
                    (EFI_HASH_OUTPUT *) &Digest
                    );
  if (EFI_ERROR(Status)) {
    goto Done;
  }
  //
  // Hash Monotonic Count.
  //
  Status = mHash->Hash (
                    mHash,
                    &gEfiHashAlgorithmSha256Guid,
                    TRUE,
                    (UINT8 *) (&CertData->MonotonicCount),
                    sizeof (UINT64),
                    (EFI_HASH_OUTPUT *) &Digest
                    );
  if (EFI_ERROR(Status)) {
    goto Done;
  }
  //
  // Generate & Initialize RSA Context.
  //
  Rsa = mCryptoService->RsaNew ();
  ASSERT (Rsa != NULL);
  //
  // Set RSA Key Components.
  // NOTE: Only N and E are needed to be set as RSA public key for signature verification.
  //
  Status = mCryptoService->RsaSetKey (Rsa, RsaKeyN, PubKey, EFI_CERT_TYPE_RSA2048_SIZE);
  if (!Status) {
    goto Done;
  }
  Status = mCryptoService->RsaSetKey (Rsa, RsaKeyE, mRsaE, sizeof (mRsaE));
  if (!Status) {
    goto Done;
  }
  //
  // Convert RsaPkcs1Verify status to EFI status
  //
  Status = mCryptoService->RsaPkcs1Verify (
                             Rsa,
                             (UINT8*) Digest.Sha256Hash,
                             SHA256_DIGEST_SIZE,
                             CertBlock->Signature,
                             EFI_CERT_TYPE_RSA2048_SHA256_SIZE
                             )
           ? EFI_SUCCESS : EFI_SECURITY_VIOLATION;
Done:

  if (Rsa != NULL) {
    mCryptoService->RsaFree (Rsa);
  }

  return Status;

}

/**
  According to variable name and vendor GUID to check the name and GUID in certificated data
  is matched.

  @param[in] VariableName   Name of Variable to be found.
  @param[in] VendorGuid     Variable vendor GUID.
  @param[in] CertDbData     Pointer to AUTH_CERT_DB_DATA instance.

  @retval TRUE              Variable name and variable vendor GUID is matched.
  @retval FALSE             Variable name and variable vendor GUID isn't matched.
**/
STATIC
BOOLEAN
IsMatchedCertData (
  IN    CHAR16                *VariableName,
  IN    EFI_GUID              *VendorGuid,
  IN    AUTH_CERT_DB_DATA     *CertDbData
  )
{
  if (VariableName == NULL || VendorGuid == NULL || CertDbData == NULL) {
    return FALSE;
  }

  if (CompareGuid (&CertDbData->VendorGuid, VendorGuid) &&
      StrCmp (VariableName, (CHAR16 *) (CertDbData + 1)) == 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Find matching signer's certificates for common authenticated variable
  by corresponding VariableName and VendorGuid from "certdb".

  The data format of "certdb":
  //
  //     UINT32 CertDbListSize;
  // /// AUTH_CERT_DB_DATA Certs1[];
  // /// AUTH_CERT_DB_DATA Certs2[];
  // /// ...
  // /// AUTH_CERT_DB_DATA Certsn[];
  //

  @param[in]  VariableName       Name of Variable to be found.
  @param[in]  VendorGuid         Vendor GUID of authenticated Variable.
  @param[in]  Data               Pointer to variable "certdb".
  @param[in]  DataSize           Size of variable "certdb".
  @param[out] Variable           Pointer to AUTH_CERT_DB_DATA instance.

  @retval EFI_SUCCESS            Find matching certs and output parameters.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Fail to find matching certs.
**/
STATIC
EFI_STATUS
FindCertsFromDb (
  IN     CHAR16               *VariableName,
  IN     EFI_GUID             *VendorGuid,
  IN     UINT8                *Data,
  IN     UINTN                DataSize,
  OUT    AUTH_CERT_DB_DATA    **CertDbData   OPTIONAL
  )
{
  UINT32                  Offset;
  AUTH_CERT_DB_DATA       *WorkingCert;
  UINT32                  CertDbListSize;


  if (VariableName == NULL || VendorGuid == NULL || Data == NULL || DataSize < sizeof (UINT32)) {
    return EFI_INVALID_PARAMETER;
  }

  CertDbListSize = *((UINT32 *) Data);
  if (CertDbListSize != (UINT32) DataSize) {
    return EFI_INVALID_PARAMETER;
  }

  Offset = sizeof (UINT32);
  //
  // Get corresponding certificates by VendorGuid and VariableName.
  //
  while (Offset < (UINT32) CertDbListSize) {
    WorkingCert = (AUTH_CERT_DB_DATA *) (Data + Offset);
    if (IsMatchedCertData (VariableName, VendorGuid, WorkingCert)) {
      if (CertDbData != NULL) {
        *CertDbData = WorkingCert;
      }
      return EFI_SUCCESS;
    }
    Offset += WorkingCert->CertNodeSize;
  }

  return EFI_NOT_FOUND;
}

/**
  Retrieve signer's certificates for common authenticated variable
  by corresponding VariableName and VendorGuid from "certdb".

  @param[in]  VariableName       Name of Variable to be found.
  @param[in]  VendorGuid         Variable vendor GUID.
  @param[out] CertDbData         Data pointer.

  @retval EFI_SUCCESS            Get signer's certificates successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Fail to find matching certs.
**/
STATIC
EFI_STATUS
GetCertsFromDb (
  IN     CHAR16             *VariableName,
  IN     EFI_GUID           *VendorGuid,
  OUT    AUTH_CERT_DB_DATA  **CertDbData
  )
{
  VARIABLE_POINTER_TRACK  CertDbVariable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;

  if ((VariableName == NULL) || (VendorGuid == NULL) || (CertDbData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get variable "certdb".
  //
  Status = FindVariableByLifetime (
             EFI_CERT_DB_NAME,
             &gEfiGenericVariableGuid,
             &CertDbVariable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FindCertsFromDb (
             VariableName,
             VendorGuid,
             GetVariableDataPtr (CertDbVariable.CurrPtr),
             DataSizeOfVariable (CertDbVariable.CurrPtr),
             CertDbData
             );

  return Status;
}

/**
  Get certificate from AUTH_CERT_DB_DATA instance.

  @param[in] CertDbData  Pointer to AUTH_CERT_DB_DATA instance.

  @return NULL           Cannot find signature in AUTH_CERT_DB_DATA instance.
  @return Other          The start address of certificated data.
**/
STATIC
VOID *
GetCertDbData (
  IN    AUTH_CERT_DB_DATA     *CertDbData
  )
{
  UINT8      *CertData;

  if (CertDbData == NULL) {
    return NULL;
  }

  CertData = ((UINT8 *) (CertDbData + 1)) + CertDbData->NameSize;

  return (VOID *) CertData;
}

/**
  Delete matching signer's certificates when deleting common authenticated
  variable by corresponding VariableName and VendorGuid from "certdb".

  @param[in] VariableName        Name of authenticated Variable.
  @param[in] VendorGuid          Vendor GUID of authenticated Variable.

  @retval EFI_SUCCESS            IThe operation is completed successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Fail to find "certdb" or matching certs.
  @retval EFI_OUT_OF_RESOURCES   The operation is failed due to lack of resources.
**/
STATIC
EFI_STATUS
DeleteCertsFromDb (
  IN     CHAR16           *VariableName,
  IN     EFI_GUID         *VendorGuid
  )
{
  VARIABLE_POINTER_TRACK  CertDbVariable;
  EFI_STATUS              Status;
  UINT32                  VarAttr;
  UINT32                  CertNodeOffset;
  UINT8                   *NewCertDb;
  UINT32                  NewCertDbSize;
  UINTN                   VariableCount;
  AUTH_CERT_DB_DATA      *CertDbData;
  EFI_TIME               TimeStamp;

  if ((VariableName == NULL) || (VendorGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get variable "certdb".
  //
  Status = FindVariableByLifetime (
             EFI_CERT_DB_NAME,
             &gEfiGenericVariableGuid,
             &CertDbVariable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (DataSizeOfVariable (CertDbVariable.CurrPtr) == sizeof (UINT32)) {
    //
    // There is no certs in certdb.
    //
    return EFI_NOT_FOUND;
  }

  //
  // Get corresponding cert node from certdb.
  //
  Status = FindCertsFromDb (
             VariableName,
             VendorGuid,
             GetVariableDataPtr (CertDbVariable.CurrPtr),
             DataSizeOfVariable (CertDbVariable.CurrPtr),
             &CertDbData
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Construct new data content of variable "certdb".
  // 1.Copy the DB entries before deleting node.
  // 2.Update CertDbListSize.
  // 3.Copy the DB entries after deleting node.
  //
  NewCertDbSize  = (UINT32) DataSizeOfVariable (CertDbVariable.CurrPtr) - CertDbData->CertNodeSize;
  NewCertDb      = mCertDbList;
  CertNodeOffset = (UINT32) (UINTN) ((UINT8 *) CertDbData - GetVariableDataPtr (CertDbVariable.CurrPtr));

  CopyMem (NewCertDb, GetVariableDataPtr (CertDbVariable.CurrPtr), CertNodeOffset);
  CopyMem (NewCertDb, &NewCertDbSize, sizeof (UINT32));
  CopyMem(
    NewCertDb + CertNodeOffset,
    GetVariableDataPtr (CertDbVariable.CurrPtr) + CertNodeOffset + CertDbData->CertNodeSize,
    DataSizeOfVariable (CertDbVariable.CurrPtr) - CertNodeOffset - CertDbData->CertNodeSize
    );

  VarAttr  = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  ZeroMem (&TimeStamp, sizeof (TimeStamp));
  Status   = UpdateVariable (
               EFI_CERT_DB_NAME,
               &gEfiGenericVariableGuid,
               NewCertDb,
               NewCertDbSize,
               VarAttr,
               0,
               0,
               &CertDbVariable,
               &TimeStamp,
               &mVariableModuleGlobal->VariableBase
               );

  return Status;
}

/**
  Insert signer's certificates for common authenticated variable with VariableName
  and VendorGuid in AUTH_CERT_DB_DATA to "certdb".

  @param[in] VariableName        Name of authenticated Variable.
  @param[in] VendorGuid          Vendor GUID of authenticated Variable.
  @param[in] CertData            Pointer to signer's certificates.
  @param[in] CertDataSize        Length of CertData in bytes.

  @retval EFI_SUCCESS            Insert an AUTH_CERT_DB_DATA entry to "certdb".
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_ACCESS_DENIED      An AUTH_CERT_DB_DATA entry with same VariableName and VendorGuid
                                 already exists.
  @retval EFI_OUT_OF_RESOURCES   Certificated database list is large than 56KB bytes.

**/
STATIC
EFI_STATUS
InsertCertsToDb (
  IN     CHAR16           *VariableName,
  IN     EFI_GUID         *VendorGuid,
  IN     UINT8            *CertData,
  IN     UINTN            CertDataSize
  )
{
  VARIABLE_POINTER_TRACK  CertDbVariable;
  EFI_STATUS              Status;
  UINT32                  VarAttr;
  UINT8                   *NewCertDb;
  UINT32                  NewCertDbSize;
  UINT32                  CertNodeSize;
  UINT32                  NameSize;
  AUTH_CERT_DB_DATA       *CurrentCertDbData;
  UINTN                   VariableCount;
  EFI_TIME                TimeStamp;


  if ((VariableName == NULL) || (VendorGuid == NULL) || (CertData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get variable "certdb".
  //
 Status = FindVariableByLifetime (
             EFI_CERT_DB_NAME,
             &gEfiGenericVariableGuid,
             &CertDbVariable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find whether matching cert node already exists in "certdb".
  // If yes return error.
  //
  Status = FindCertsFromDb (
             VariableName,
             VendorGuid,
             GetVariableDataPtr (CertDbVariable.CurrPtr),
             DataSizeOfVariable (CertDbVariable.CurrPtr),
             NULL
             );

  if (!EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_ACCESS_DENIED;
  }

  //
  // Construct new data content of variable "certdb".
  //
  NameSize      = (UINT32) StrSize (VariableName);
  CertNodeSize  = sizeof (AUTH_CERT_DB_DATA) + (UINT32) CertDataSize + NameSize;
  NewCertDbSize = (UINT32) DataSizeOfVariable (CertDbVariable.CurrPtr) + CertNodeSize;
  NewCertDb     = mCertDbList;
  if (NewCertDbSize > MAX_VARIABLE_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // 1.Copy the whole original DB entries .
  // 2.Update CertDbListSize.
  // 3.Append new certificated database at end of original DB list.
  //
  CopyMem (NewCertDb, GetVariableDataPtr (CertDbVariable.CurrPtr), DataSizeOfVariable (CertDbVariable.CurrPtr));
  CopyMem (NewCertDb, &NewCertDbSize, sizeof (UINT32));
  //
  // Construct new cert node header.
  //
  CurrentCertDbData = (AUTH_CERT_DB_DATA *) (NewCertDb + DataSizeOfVariable (CertDbVariable.CurrPtr));
  CopyMem (&CurrentCertDbData->VendorGuid, VendorGuid, sizeof (EFI_GUID));
  CopyMem (&CurrentCertDbData->CertNodeSize, &CertNodeSize, sizeof (UINT32));
  CopyMem (&CurrentCertDbData->NameSize, &NameSize, sizeof (UINT32));
  CopyMem (&CurrentCertDbData->CertDataSize, &CertDataSize, sizeof (UINT32));

  CopyMem (
    CurrentCertDbData + 1,
    VariableName,
    NameSize
    );

  CopyMem (
    ((UINT8 *) (CurrentCertDbData + 1)) + NameSize,
    CertData,
    CertDataSize
    );

  VarAttr  = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  ZeroMem (&TimeStamp, sizeof (TimeStamp));
  Status   = UpdateVariable (
               EFI_CERT_DB_NAME,
               &gEfiGenericVariableGuid,
               NewCertDb,
               NewCertDbSize,
               VarAttr,
               0,
               0,
               &CertDbVariable,
               &TimeStamp,
               &mVariableModuleGlobal->VariableBase
               );

  return Status;
}

/**
  Process variable with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.

  @param[in]  VariableName        Name of Variable to be found.
  @param[in]  VendorGuid          Variable vendor GUID.
  @param[in]  Data                Data pointer.
  @param[in]  DataSize            Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in]  Variable            The variable information which is used to keep track of variable usage.
  @param[in]  Attributes          Attribute value of the variable.
  @param[in]  AuthVarType         Verify against PK or KEK database or private database.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.
  @param[out] VarDel              Delete the variable or not.

  @retval EFI_SUCCESS             Variable pass validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation
                                  check carried out by the firmware.
  @retval EFI_OUT_OF_RESOURCES    Failed to process variable due to lack
                                  of resources.
**/
STATIC
EFI_STATUS
WriteTimeBasedPayload (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     VOID                               *Data,
  IN     UINTN                              DataSize,
  IN     VARIABLE_POINTER_TRACK             *Variable,
  IN     UINT32                             Attributes,
  IN     VARIABLE_GLOBAL                    *Global,
  IN     AUTHVAR_TYPE                       AuthVarType,
  OUT    BOOLEAN                            *VarDel
  )
{
  UINT8                            *RootCert;
  UINT8                            *SigData;
  UINT8                            *PayLoadPtr;
  UINTN                            RootCertSize;
  UINTN                            Index;
  UINTN                            CertCount;
  UINTN                            PayloadSize;
  UINT32                           Attr;
  UINT32                           SigDataSize;
  UINT32                           KekDataSize;
  BOOLEAN                          VerifyStatus;
  EFI_STATUS                       Status;
  EFI_SIGNATURE_LIST               *CertList;
  EFI_SIGNATURE_DATA               *Cert;
  VARIABLE_POINTER_TRACK           PkVariable;
  VARIABLE_POINTER_TRACK           KekVariable;
  EFI_VARIABLE_AUTHENTICATION_2    *CertData;
  UINT8                            *NewData;
  UINTN                            NewDataSize;
  UINTN                            VariableCount;
  UINT8                            *SignerCerts;
  UINTN                            CertStackSize;
  AUTH_CERT_DB_DATA                *CertDbData;

  if (mCryptoService == NULL) {
    return EFI_UNSUPPORTED;
  }

  VerifyStatus           = FALSE;
  CertData               = NULL;
  NewData                = NULL;
  Attr                   = Attributes;
  RootCert               = NULL;
  SignerCerts            = NULL;

  //
  // When the attribute EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS is
  // set, then the Data buffer shall begin with an instance of a complete (and serialized)
  // EFI_VARIABLE_AUTHENTICATION_2 descriptor. The descriptor shall be followed by the new
  // variable value and DataSize shall reflect the combined size of the descriptor and the new
  // variable value. The authentication descriptor is not part of the variable data and is not
  // returned by subsequent calls to GetVariable().
  //
  CertData = (EFI_VARIABLE_AUTHENTICATION_2 *) Data;

  //
  // Verify that Pad1, Nanosecond, TimeZone, Daylight and Pad2 components of the
  // TimeStamp value are set to zero.
  //
  if ((CertData->TimeStamp.Pad1 != 0) ||
      (CertData->TimeStamp.Nanosecond != 0) ||
      (CertData->TimeStamp.TimeZone != 0) ||
      (CertData->TimeStamp.Daylight != 0) ||
      (CertData->TimeStamp.Pad2 != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Variable->CurrPtr != NULL) && ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0)) {
    if (CompareTimeStamp (&CertData->TimeStamp, &Variable->CurrPtr->TimeStamp)) {
      //
      // TimeStamp check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
      //
      return EFI_SECURITY_VIOLATION;
    }
  }

  //
  // wCertificateType should be WIN_CERT_TYPE_EFI_GUID.
  // Cert type should be EFI_CERT_TYPE_PKCS7_GUID.
  //
  if ((CertData->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||
      !CompareGuid (&CertData->AuthInfo.CertType, &gEfiCertPkcs7Guid)) {
    //
    // Invalid AuthInfo type, return EFI_SECURITY_VIOLATION.
    //
    return EFI_SECURITY_VIOLATION;
  }
  //
  // Find out Pkcs7 SignedData which follows the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
  // AuthInfo.Hdr.dwLength is the length of the entire certificate, including the length of the header.
  //
  SigData = (UINT8*) ((UINTN)Data + (UINTN)(((EFI_VARIABLE_AUTHENTICATION_2 *) 0)->AuthInfo.CertData));
  SigDataSize = CertData->AuthInfo.Hdr.dwLength - (UINT32)(UINTN)(((WIN_CERTIFICATE_UEFI_GUID *) 0)->CertData);

  //
  // Find out the new data payload which follows Pkcs7 SignedData directly.
  //
  PayLoadPtr = (UINT8*) ((UINTN) SigData + (UINTN) SigDataSize);
  PayloadSize = DataSize - (UINTN)(((EFI_VARIABLE_AUTHENTICATION_2 *) 0)->AuthInfo.CertData) - (UINTN) SigDataSize;


  //
  // Construct a buffer to fill with (VariableName, VendorGuid, Attributes, TimeStamp, Data).
  //
  NewDataSize = PayloadSize + sizeof (EFI_TIME) + sizeof (UINT32) +
                sizeof (EFI_GUID) + StrSize (VariableName) - sizeof (CHAR16);
  NewData = (UINT8 *) mStorageArea;
  if (NewData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (NewData, VariableName, StrSize (VariableName) - sizeof (CHAR16));
  CopyMem (NewData + StrSize (VariableName) - sizeof (CHAR16), VendorGuid, sizeof (EFI_GUID));
  CopyMem (
    NewData + StrSize (VariableName) - sizeof (CHAR16) + sizeof (EFI_GUID),
    &Attr,
    sizeof (UINT32)
    );

  CopyMem (
    NewData + StrSize (VariableName) - sizeof (CHAR16) + sizeof (EFI_GUID) + sizeof (UINT32),
    &CertData->TimeStamp,
    sizeof (EFI_TIME)
    );

  CopyMem (NewData + (NewDataSize - PayloadSize), PayLoadPtr, PayloadSize);

  if (AuthVarType == AuthVarTypePk) {

    //
    // Get platform key from variable.
    //
    VariableCount = 0;
    Status = FindVariableByLifetime (
               EFI_PLATFORM_KEY_NAME,
               &gEfiGlobalVariableGuid,
               &PkVariable,
               &VariableCount,
               Global
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    CertList = (EFI_SIGNATURE_LIST *) GetVariableDataPtr (PkVariable.CurrPtr);
    Cert     = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
    RootCert      = Cert->SignatureData;
    RootCertSize  = CertList->SignatureSize - sizeof (EFI_GUID);
    if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
        //
        // Verify Pkcs7 SignedData via Pkcs7Verify library.
        // NOTE THAT: Since we have no Pkcs7 SignedData Parser, there is no way to know what exactly
        // digestAlgorithms caller uses, here suppose SHA1 is used by caller for hash.
        //
        VerifyStatus = mCryptoService->Pkcs7Verify (
                                         SigData,
                                         SigDataSize,
                                         RootCert,
                                         RootCertSize,
                                         NewData,
                                         NewDataSize,
                                         FALSE   // no need to do authenticode workaround
                                         );
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid)) {
      VerifyStatus = mCryptoService->Pkcs7VerifyUsingPubKey (
                                       SigData,
                                       SigDataSize,
                                       RootCert,
                                       EFI_CERT_TYPE_RSA2048_SIZE,
                                       NewData,
                                       NewDataSize
                                       );
    }
  } else if (AuthVarType == AuthVarTypeKek){

    //
    // Get KEK database from variable.
    //
    VariableCount = 0;
    Status = FindVariableByLifetime (
               EFI_KEY_EXCHANGE_KEY_NAME,
               &gEfiGlobalVariableGuid,
               &KekVariable,
               &VariableCount,
               Global
               );
    if (EFI_ERROR (Status)){
      return Status;
    }
    //
    // Ready to verify Pkcs7 SignedData. Go through KEK Signature Database to find out X.509 CertList.
    //
    KekDataSize      = KekVariable.CurrPtr->DataSize;
    CertList         = (EFI_SIGNATURE_LIST *) GetVariableDataPtr (KekVariable.CurrPtr);
    while ((KekDataSize > 0) && (KekDataSize >= CertList->SignatureListSize)) {
      if (CertList->SignatureListSize < sizeof (EFI_SIGNATURE_LIST)) {
        return EFI_INVALID_PARAMETER;
      }
      if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
        Cert       = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
        CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
        for (Index = 0; Index < CertCount; Index++) {
          //
          // Iterate each Signature Data Node within this CertList for a verify
          //
          RootCert      = Cert->SignatureData;
          //
          // SignatureSize includes the owner GUID
          //
          RootCertSize  = CertList->SignatureSize - sizeof(EFI_GUID);

          //
          // Verify Pkcs7 SignedData via Pkcs7Verify library.
          // NOTE THAT: Since we have no Pkcs7 SignedData Parser, there is no way to know what exactly
          // digestAlgorithms caller uses, here suppose SHA1 is used by caller for hash.
          //
          VerifyStatus = mCryptoService->Pkcs7Verify (
                                           SigData,
                                           SigDataSize,
                                           RootCert,
                                           RootCertSize,
                                           NewData,
                                           NewDataSize,
                                           FALSE // no need for authenticode workaround
                                           );
          if (VerifyStatus) {
            goto Exit;
          }
          Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
        }
      } else if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid)) {
        Cert       = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
        CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
        for (Index = 0; Index < CertCount; Index++) {
          //
          // Iterate each Signature Data Node within this CertList for a verify
          //
          RootCert      = Cert->SignatureData;

          VerifyStatus = mCryptoService->Pkcs7VerifyUsingPubKey (
                                           SigData,
                                           SigDataSize,
                                           RootCert,
                                           EFI_CERT_TYPE_RSA2048_SIZE,
                                           NewData,
                                           NewDataSize
                                           );
          if (VerifyStatus) {
            goto Exit;
          }
          Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
        }
      }
      KekDataSize -= CertList->SignatureListSize;
      CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
    }
  } else if (AuthVarType == AuthVarTypePriv){

    //
    // Process common authenticated variable except PK/KEK/DB/DBX.
    // Get signer's certificates from SignedData.
    //
    VerifyStatus = mCryptoService->Pkcs7GetSigners (
                                     SigData,
                                     SigDataSize,
                                     &SignerCerts,
                                     &CertStackSize,
                                     &RootCert,
                                     &RootCertSize
                                     );
    if (!VerifyStatus) {
      goto Exit;
    }

    //
    // Get previously stored signer's certificates from certdb for existing
    // variable. Check whether they are identical with signer's certificates
    // in SignedData. If not, return error immediately.
    //
    if ((Variable->CurrPtr != NULL)) {
      VerifyStatus = FALSE;

      Status = GetCertsFromDb (VariableName, VendorGuid, &CertDbData);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }

      if ((CertStackSize != CertDbData->CertDataSize) ||
          (CompareMem (SignerCerts, GetCertDbData (CertDbData), CertDbData->CertDataSize) != 0)) {
        goto Exit;
      }

    }

    VerifyStatus = mCryptoService->Pkcs7Verify (
                                     SigData,
                                     SigDataSize,
                                     RootCert,
                                     RootCertSize,
                                     NewData,
                                     NewDataSize,
                                     FALSE
                                     );
    if (!VerifyStatus) {
      goto Exit;
    }
    //
    // Delete signer's certificates when delete the common authenticated variable.
    //
    if ((PayloadSize == 0) && (Variable->CurrPtr != NULL) && ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0)) {
      Status = DeleteCertsFromDb (VariableName, VendorGuid);
      if (EFI_ERROR (Status)) {
        VerifyStatus = FALSE;
        goto Exit;
      }
    } else if (Variable->CurrPtr == NULL && PayloadSize != 0) {
      //
      // Insert signer's certificates when adding a new common authenticated variable.
      //
      Status = InsertCertsToDb (VariableName, VendorGuid, SignerCerts, CertStackSize);
      if (EFI_ERROR (Status)) {
        VerifyStatus = FALSE;
        goto Exit;
      }
    }
  } else {
    return EFI_SECURITY_VIOLATION;
  }


Exit:

  if (AuthVarType == AuthVarTypePriv) {
    mCryptoService->Pkcs7FreeSigners (RootCert);
    mCryptoService->Pkcs7FreeSigners (SignerCerts);
  }


  if (!VerifyStatus) {
    return EFI_SECURITY_VIOLATION;
  }

  if ((PayloadSize == 0) && (VarDel != NULL)) {
    *VarDel = TRUE;
  }
  //
  // Final step: Update/Append Variable if it pass Pkcs7Verify
  //
  return UpdateVariable (
           VariableName,
           VendorGuid,
           PayLoadPtr,
           PayloadSize,
           Attributes,
           0,
           0,
           Variable,
           &CertData->TimeStamp,
           Global
           );
}

/**
  Update platform mode.

  @param[in] Mode      SETUP_MODE or USER_MODE.
  @param[in] Global    Pointer to VARIABLE_GLOBAL instance.
**/
STATIC
VOID
UpdatePlatformMode (
  IN  UINT32              Mode,
  IN  VARIABLE_GLOBAL     *Global
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINT32                  VarAttr;
  UINTN                   VariableCount;

  VariableCount = 0;
  Status = FindVariableByLifetime (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );
  ASSERT_EFI_ERROR (Status);

  mPlatformMode  = Mode;
  VarAttr        = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
  Status         = UpdateVariable (
                     EFI_SETUP_MODE_NAME,
                     &gEfiGlobalVariableGuid,
                     &mPlatformMode,
                     sizeof(UINT8),
                     VarAttr,
                     0,
                     0,
                     &Variable,
                     NULL,
                     Global
                     );
  ASSERT_EFI_ERROR (Status);
}

/**
  Process variable with platform key for verification.

  @param[in] VariableName         Name of Variable to be found.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Data pointer.
  @param[in] DataSize             Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable
  @param[in] IsPk                 Indicate whether it is to process pk.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Variable passed validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation.
                                  check carried out by the firmware.
**/
EFI_STATUS
ProcessVarWithPk (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid,
  IN  VOID                      *Data,
  IN  UINTN                     DataSize,
  IN  VARIABLE_POINTER_TRACK    *Variable,
  IN  UINT32                    Attributes OPTIONAL,
  IN  BOOLEAN                   IsPk,
  IN  VARIABLE_GLOBAL           *Global
  )
{
  EFI_STATUS                      Status;
  VARIABLE_POINTER_TRACK          PkVariable;
  EFI_SIGNATURE_LIST              *OldPkList;
  EFI_SIGNATURE_DATA              *OldPkData;
  EFI_VARIABLE_AUTHENTICATION     *CertData;
  EFI_TIME                        *TimeStamp;
  BOOLEAN                         TimeBase;
  BOOLEAN                         Del;
  UINTN                           VariableCount;
  UINT64                          MonotonicCount;

  if (mCryptoService == NULL && mPlatformMode == USER_MODE) {
    return EFI_UNSUPPORTED;
  }

  //
  // UEFI 2.3.1 - do not support append for PK
  //
  if (((Attributes & EFI_VARIABLE_APPEND_WRITE) == EFI_VARIABLE_APPEND_WRITE) &&
      (IsPkVariable (VariableName, VendorGuid))) {
    //
    // PK does not support append.
    //
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    //
    // PK and KEK should set EFI_VARIABLE_NON_VOLATILE attribute.
    //
    return EFI_INVALID_PARAMETER;
  }

  if (mPlatformMode == USER_MODE) {
    if (((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0) &&
        ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == 0)) {
      //
      // In user mode, PK and KEK should set EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute.
      //   or in UEFI 2.3.1 can set EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
      //
      return EFI_INVALID_PARAMETER;
    }
    TimeBase = FALSE;
    if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS attribute means time-based X509 Cert PK.
      //
      TimeBase = TRUE;
    } else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute means counter-based RSA-2048 Cert PK.
      //
      TimeBase = FALSE;
    } else {
      return EFI_INVALID_PARAMETER;
    }
    if (TimeBase) {
      Del    = FALSE;
      Status = WriteTimeBasedPayload (VariableName, VendorGuid, Data, DataSize, Variable, Attributes, Global, AuthVarTypePk, &Del);
      if (!EFI_ERROR (Status)) {
        //
        // If delete PK in user mode, need change to setup mode.
        //
        if (Del && IsPk) {
          UpdatePlatformMode (SETUP_MODE, Global);
          //
          // boot mode must change at this point, as untrusted code can now be run
          //
          UpdatePlatformBootMode (SECURE_BOOT_MODE_DISABLE, Global);
        }
      }
    } else {
      //
      // Verify against RSA2048 Cert PK.
      //
      CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;
      if ((Variable->CurrPtr != NULL) && (CertData->MonotonicCount <= Variable->CurrPtr->MonotonicCount)) {
        //
        // Monotonic count check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
        //
        return EFI_SECURITY_VIOLATION;
      }
      //
      // Get platform key from variable.
      //
      Status = FindVariableByLifetime (
                 EFI_PLATFORM_KEY_NAME,
                 &gEfiGlobalVariableGuid,
                 &PkVariable,
                 &VariableCount,
                 Global
                 );
      ASSERT_EFI_ERROR (Status);

      OldPkList = (EFI_SIGNATURE_LIST *) GetVariableDataPtr (PkVariable.CurrPtr);
      OldPkData = (EFI_SIGNATURE_DATA *) ((UINT8 *) OldPkList + sizeof (EFI_SIGNATURE_LIST) + OldPkList->SignatureHeaderSize);
      Status	  = VerifyCounterBasedPayload (Data, DataSize, OldPkData->SignatureData);
      if (!EFI_ERROR (Status)) {
        Status = UpdateVariable (
                   VariableName,
                   VendorGuid,
                   (UINT8*)Data + AUTHINFO_SIZE,
                   DataSize - AUTHINFO_SIZE,
                   Attributes,
                   0,
                   CertData->MonotonicCount,
                   Variable,
                   NULL,
                   Global
                   );
        if (!EFI_ERROR (Status)) {
          if ((DataSize == AUTHINFO_SIZE) && IsPk) {
            UpdatePlatformMode (SETUP_MODE, Global);
            //
            // boot mode must change at this point, as untrusted code can now be run
            //
            UpdatePlatformBootMode (SECURE_BOOT_MODE_DISABLE, Global);
          }
        }
      }
    }
  } else {
    TimeStamp = NULL;
    MonotonicCount = 0;
    if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // Time-based Authentication descriptor.
      //
      TimeStamp = &((EFI_VARIABLE_AUTHENTICATION_2 *) Data)->TimeStamp;
      DataSize -= AUTHINFO2_SIZE (Data);
      Data = (UINT8*) Data + AUTHINFO2_SIZE (Data);
    } else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // skip AuthInfo
      //
      MonotonicCount = ((EFI_VARIABLE_AUTHENTICATION *) Data)->MonotonicCount;
      Data = (UINT8*)Data + AUTHINFO_SIZE;
      DataSize = DataSize - AUTHINFO_SIZE;
    }

    Status = UpdateVariable (VariableName, VendorGuid, Data, DataSize, Attributes, 0, MonotonicCount, Variable, TimeStamp, Global);
    //
    // If enroll PK in setup mode, need change to user mode.
    //
    if ((!EFI_ERROR (Status)) && (DataSize != 0) && IsPk) {
      UpdatePlatformMode (USER_MODE, Global);
      //
      // NOTE: boot mode does NOT change here, since the PK just was enrolled, and untrusted
      //		   code may have run already since reset.
      //
    }
  }
  return Status;

}

/**
  Process variable with key exchange key for verification.

  @param[in] VariableName         Name of Variable to be found.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Data pointer.
  @param[in] DataSize             Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Variable pass validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation
                                  check carried out by the firmware.
**/
EFI_STATUS
ProcessVarWithKek (
  IN  CHAR16                               *VariableName,
  IN  EFI_GUID                             *VendorGuid,
  IN  VOID                                 *Data,
  IN  UINTN                                DataSize,
  IN  VARIABLE_POINTER_TRACK               *Variable,
  IN  UINT32                               Attributes OPTIONAL,
  IN  VARIABLE_GLOBAL                      *Global
  )
{
  EFI_STATUS                       Status;
  BOOLEAN                          Validated;
  VARIABLE_POINTER_TRACK           KekVariable;
  EFI_SIGNATURE_LIST               *KekList;
  EFI_SIGNATURE_DATA               *KekItem;
  UINT32                           KekCount;
  EFI_VARIABLE_AUTHENTICATION      *CertData;
  EFI_CERT_BLOCK_RSA_2048_SHA256   *CertBlock;
  BOOLEAN                          IsFound;
  UINT32                           Index;
  UINT32                           KekDataSize;
  UINTN                            VariableCount;
  BOOLEAN                          TimeBase;
  EFI_TIME                        *TimeStamp;
  UINT64                          MonotonicCount;

  if (mCryptoService == NULL && mPlatformMode == USER_MODE) {
    return EFI_UNSUPPORTED;
  }

  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    //
    // DB and DBX should set EFI_VARIABLE_NON_VOLATILE attribute.
    //
    return EFI_INVALID_PARAMETER;
  }

  if (mPlatformMode == USER_MODE) {
    if (((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0) &&
    ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == 0)) {
      //
      // In user mode, PK and KEK should set EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute.
      //   or in UEFI 2.3.1 can set EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
      //
      return EFI_INVALID_PARAMETER;
    }
    TimeBase = FALSE;
    if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS attribute means time-based X509 Cert PK.
      //
      TimeBase = TRUE;
    } else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute means counter-based RSA-2048 Cert PK.
      //
      TimeBase = FALSE;
    } else {
      return EFI_INVALID_PARAMETER;
    }

    if (TimeBase) {
      Status = WriteTimeBasedPayload (VariableName, VendorGuid, Data, DataSize, Variable, Attributes, Global, AuthVarTypeKek, NULL);
    } else {
      //
      // Verify against RSA2048 Cert PK.
      //
      CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;
      if ((Variable->CurrPtr != NULL) && (CertData->MonotonicCount <= Variable->CurrPtr->MonotonicCount)) {
        //
        // Monotonic count check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
        //
        return EFI_SECURITY_VIOLATION;
      }
      CertBlock = (EFI_CERT_BLOCK_RSA_2048_SHA256 *) (CertData->AuthInfo.CertData);
      //
      // Get KEK database from variable.
      //
      VariableCount = 0;
      Status = FindVariableByLifetime (
                 EFI_KEY_EXCHANGE_KEY_NAME,
                 &gEfiGlobalVariableGuid,
                 &KekVariable,
                 &VariableCount,
                 Global
                 );
      ASSERT_EFI_ERROR (Status);

      KekDataSize = KekVariable.CurrPtr->DataSize;
      KekList     = (EFI_SIGNATURE_LIST *) GetVariableDataPtr (KekVariable.CurrPtr);
      //
      // original UEFI behavior
      //
      IsFound = FALSE;
      Validated = FALSE;
      while ((KekDataSize > 0) && (KekDataSize >= KekList->SignatureListSize)) {
        if (CompareGuid (&KekList->SignatureType, &gEfiCertRsa2048Guid)) {
          KekItem   = (EFI_SIGNATURE_DATA *) ((UINT8 *) KekList + sizeof (EFI_SIGNATURE_LIST) + KekList->SignatureHeaderSize);
          KekCount  = (KekList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - KekList->SignatureHeaderSize) / KekList->SignatureSize;
          for (Index = 0; Index < KekCount; Index++) {
            if (CompareMem (KekItem->SignatureData, CertBlock->PublicKey, EFI_CERT_TYPE_RSA2048_SIZE) == 0) {
              IsFound = TRUE;
              break;
            }
            KekItem = (EFI_SIGNATURE_DATA *) ((UINT8 *) KekItem + KekList->SignatureSize);
          }
        }
        KekDataSize -= KekList->SignatureListSize;
        KekList = (EFI_SIGNATURE_LIST *) ((UINT8 *) KekList + KekList->SignatureListSize);
      }

      if (!IsFound) {
        return EFI_SECURITY_VIOLATION;
      }

      Status = VerifyCounterBasedPayload (Data, DataSize, CertBlock->PublicKey);
      if (!EFI_ERROR (Status)) {
        Validated = TRUE;
      }
      if (Validated) {
        Status = UpdateVariable (
                   VariableName,
                   VendorGuid,
                   (UINT8*)Data + AUTHINFO_SIZE,
                   DataSize - AUTHINFO_SIZE,
                   Attributes,
                   0,
                   CertData->MonotonicCount,
                   Variable,
                   NULL,
                   Global
                   );

      }
    }
  } else {
    TimeStamp = NULL;
    MonotonicCount = 0;
    if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // Time-based Authentication descriptor.
      //
      TimeStamp = &((EFI_VARIABLE_AUTHENTICATION_2 *) Data)->TimeStamp;
      DataSize -= AUTHINFO2_SIZE (Data);
      Data = (UINT8*) Data + AUTHINFO2_SIZE (Data);
    } else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
      //
      // skip AuthInfo
      //
      MonotonicCount = ((EFI_VARIABLE_AUTHENTICATION *) Data)->MonotonicCount;
      Data = (UINT8*)Data + AUTHINFO_SIZE;
      DataSize = DataSize - AUTHINFO_SIZE;
    }

    Status = UpdateVariable (VariableName, VendorGuid, Data, DataSize, Attributes, 0, MonotonicCount, Variable, TimeStamp, Global);
  }

  return Status;
}


/**
  Process variable with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS/EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set

  @param[in] VariableName         Name of Variable to be found.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Data pointer.
  @param[in] DataSize             Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Variable is not write-protected or pass validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_WRITE_PROTECTED     Variable is write-protected and needs authentication with
                                  EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @retval EFI_SECURITY_VIOLATION  The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                  set, but the AuthInfo does NOT pass the validation
                                  check carried out by the firmware.
**/
EFI_STATUS
ProcessVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     VOID                               *Data,
  IN     UINTN                              DataSize,
  IN     VARIABLE_POINTER_TRACK             *Variable,
  IN     UINT32                             Attributes,
  IN     VARIABLE_GLOBAL                    *Global
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         IsDeletion;
  BOOLEAN                         IsFirstTime;
  UINT8                           *PubKey;
  EFI_VARIABLE_AUTHENTICATION     *CertData;
  EFI_CERT_BLOCK_RSA_2048_SHA256  *CertBlock;
  UINT32                          KeyIndex;
  UINT64                          MonotonicCount;

  KeyIndex    = 0;
  CertData    = NULL;
  CertBlock   = NULL;
  PubKey      = NULL;
  IsDeletion  = FALSE;


  if (mVariableModuleGlobal->SecureBootCallbackEnabled && IsAdministerSecureVariable (VariableName, VendorGuid)) {
    return UpdateAdministerSecureVariable (
             VariableName,
             VendorGuid,
             Data,
             DataSize,
             Attributes
             );
  }

  if (mCryptoService == NULL && mPlatformMode == USER_MODE) {
    return EFI_UNSUPPORTED;
  }

  if (IsExistingInsydeSecureVariable (Variable)) {
    //
    // Insyde secure variable only can update in unlocked state, so just return EFI_WRITE_PROTECTED directly.
    //
    return EFI_WRITE_PROTECTED;
  }
  //
  // Process Time-based Authenticated variable.
  //
  if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
    return WriteTimeBasedPayload (VariableName, VendorGuid, Data, DataSize, Variable, Attributes, Global, AuthVarTypePriv, NULL);
  }

  //
  // Determine if first time SetVariable with the EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS.
  //
  if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
    //
    // Determine current operation type.
    //
    if (DataSize == AUTHINFO_SIZE) {
      IsDeletion = TRUE;
    }
    //
    // Determine whether this is the first time with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
    //
    if (Variable->CurrPtr == NULL) {
      IsFirstTime = TRUE;
    } else if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0) {
      IsFirstTime = TRUE;
    } else {
      KeyIndex   = Variable->CurrPtr->PubKeyIndex;
      IsFirstTime = FALSE;
    }
  } else if ((Variable->CurrPtr != NULL) &&
           (Variable->CurrPtr->Attributes & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) != 0) {

    //
    // If the variable is already write-protected, it always needs authentication before update.
    //
    return EFI_WRITE_PROTECTED;
  } else {
    //
    // If without EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS, set and attributes collision.
    // That means it is not authenticated variable, just update variable as usual.
    //
    Status = UpdateVariable (VariableName, VendorGuid, Data, DataSize, Attributes, 0, 0, Variable, NULL, Global);
    return Status;
  }

  //
  // Get PubKey and check Monotonic Count value corresponding to the variable.
  //
  CertData  = (EFI_VARIABLE_AUTHENTICATION *) Data;
  CertBlock = (EFI_CERT_BLOCK_RSA_2048_SHA256 *) (CertData->AuthInfo.CertData);
  PubKey    = CertBlock->PublicKey;

  //
  // Update Monotonic Count value.
  //
  MonotonicCount = CertData->MonotonicCount;

  if (!IsFirstTime) {
    //
    // Check input PubKey.
    //
    if (CompareMem (PubKey, mPubKeyStore + (KeyIndex - 1) * EFI_CERT_TYPE_RSA2048_SIZE, EFI_CERT_TYPE_RSA2048_SIZE) != 0) {
      return EFI_SECURITY_VIOLATION;
    }
    //
    // Compare the current monotonic count and ensure that it is greater than the last SetVariable
    // operation with the EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute set.
    //
    if (CertData->MonotonicCount <= Variable->CurrPtr->MonotonicCount) {
      //
      // Monotonic count check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
      //
      return EFI_SECURITY_VIOLATION;
    }
  }
  //
  // Verify the certificate in Data payload.
  //
  Status = VerifyCounterBasedPayload (Data, DataSize, PubKey);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Now, the signature has been verified!
  //
  if (IsFirstTime && !IsDeletion) {
    //
    // Update public key database variable if need.
    //
    Status = AddPubKeyInStore (PubKey, Global, &KeyIndex);
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Verification pass.
  //
  Status = UpdateVariable (VariableName, VendorGuid, (UINT8*)Data + AUTHINFO_SIZE, DataSize - AUTHINFO_SIZE, Attributes, KeyIndex, MonotonicCount, Variable, NULL, Global);

  return Status;
}
/**
  According to input EFI_SIGNATURE_LIST instance to check this signature is whether a
  valid X509 certificate.

  @param[in] SigList  Pointer to the start address of signature list.

  @retval TRUE        Input data is a valid X509 certificate or system doesn't support check mechanism.
  @retval FALSE       Input data is an invalid X509 certificate.
**/
STATIC
BOOLEAN
IsValidX509Cert (
  IN  EFI_SIGNATURE_LIST       *SigList
  )
{
  VOID                   *RsaContext;
  EFI_SIGNATURE_DATA     *CertData;
  UINTN                  CertLen;

  if (SigList == NULL || !CompareGuid (&SigList->SignatureType, &gEfiCertX509Guid)) {
    return FALSE;
  }

  //
  // If system doesn't support RsaNew function, we have no way to check input certifcate.
  // Therefore, suppose the input certificate is valid.
  //
  RsaContext = mCryptoService->RsaNew ();
  if (RsaContext == NULL) {
    return TRUE;
  }
  mCryptoService->RsaFree (RsaContext);

  CertData = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigList + sizeof (EFI_SIGNATURE_LIST) + SigList->SignatureHeaderSize);
  CertLen = SigList->SignatureSize - sizeof (EFI_GUID);
  if (!mCryptoService->RsaGetPublicKeyFromX509 (CertData->SignatureData, CertLen, &RsaContext)) {
    return FALSE;
  }
  mCryptoService->RsaFree (RsaContext);

  return TRUE;
}

/**
  Perform a sanity check on the data to be written to secure boot variables.

  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Variable vendor GUID.
  @param[in] Data               Variable data.
  @param[in] DataSize           Size of data. 0 means delete.

  @retval EFI_SUCCESS           The sanity check operation is success.
  @retval EFI_INVALID_PARAMETER Variable data has some data structure problem.
**/
EFI_STATUS
CheckSecureBootVarData (
  IN CONST  CHAR16          *VariableName,
  IN CONST  EFI_GUID        *VendorGuid,
  IN CONST  VOID            *Data,
  IN        UINTN           DataSize
  )
{
  EFI_SIGNATURE_LIST		         *SigList;
  UINTN                          CheckDataSize;
  UINTN						               Index;
  UINTN                          TableCnt;
  BOOLEAN                        FindSigType;
  EFI_STATUS                     Status;

  //
  // make sure there is data
  //
  if ((Data == NULL) || (DataSize == 0)) {
    return EFI_SUCCESS;
  }

  //
  // walk through the lists present in the data, checking for supported types, and correct sizes
  //
  SigList = (EFI_SIGNATURE_LIST *)Data;
  CheckDataSize = DataSize;

  if (SigList->SignatureListSize > CheckDataSize) {
  	return EFI_INVALID_PARAMETER;
  }

  Status   = EFI_SUCCESS;
  TableCnt = sizeof (mSupportSigItem) / sizeof (SIGNATURE_SUPPORT_INFO);
  while (CheckDataSize > 0) {
    //
    // check the size of singature list size first to make sure singature data integrity
    //
    if (SigList->SignatureListSize < sizeof (EFI_SIGNATURE_LIST) || CheckDataSize < SigList->SignatureListSize) {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    FindSigType = FALSE;
    for (Index = 0; Index < TableCnt; Index++) {
      //
      // compare singnature type first
      //
      if (!CompareGuid (&SigList->SignatureType, &mSupportSigItem[Index].SignatureType)) {
        continue;
      }
      //
      // check realtive supports this signature type or not
      //
      if (mSupportSigItem[Index].OnlyForSecurityImageDatabase && !IsDbVariable (VariableName, VendorGuid) &&
          !IsDbxVariable (VariableName, VendorGuid)) {
        continue;
      }
      //
      // check signature size is correct or not.
      //
      if (mSupportSigItem[Index].FixedSigSize && mSupportSigItem[Index].SignatureSize != SigList->SignatureSize) {
        continue;
      }
      //
      // Check input certificate is whether a valid certificate if signature type is X509 certificate
      //
      if (CompareGuid (&SigList->SignatureType, &gEfiCertX509Guid) && !IsValidX509Cert (SigList)) {
        break;
      }

      FindSigType = TRUE;
      break;
    }

    if (!FindSigType) {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    CheckDataSize -= SigList->SignatureListSize;
    SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
  }

  return Status;

}

/**
  This function used to check the signature list node is whether can merge any of the signature list.
  If yes, return the pointer to the signature list which can be merged.
  If no, return NULL pointer.

  @param[in] SigList            Pointer to the start address of whole signature list.
  @param[in] BufferSize         The buffer size of whole signature list.
  @param[in] SigListNode        Pointer to siganture list node which want to check.

  @retval NULL                  Cannot find can merged signature list.
  @return EFI_SIGNATURE_LIST *  Pointer to the can merged signature list.
**/
STATIC
EFI_SIGNATURE_LIST *
FindCanMergeSig (
  IN     EFI_SIGNATURE_LIST     *SigList,
  IN     UINTN                  BufferSize,
  IN     EFI_SIGNATURE_LIST     *SigListNode
  )
{
  UINTN                    DataOffest;
  EFI_SIGNATURE_LIST       *WorkingSig;
  BOOLEAN                  SigFound;


  if (SigList == NULL || BufferSize == 0 || SigListNode == NULL) {
    return NULL;
  }
  //
  // if singnature type isn't a fixed length singnature type, return NULL pointer directly.
  //
  if (CompareGuid (&SigListNode->SignatureType, &gEfiCertX509Guid)) {
    return NULL;
  }

  SigFound = FALSE;
  WorkingSig = SigList;
  DataOffest = 0;
  while (DataOffest < BufferSize) {
    if (CompareGuid (&WorkingSig->SignatureType, &SigListNode->SignatureType)) {
      SigFound = TRUE;
      break;
    }
    DataOffest += WorkingSig->SignatureListSize;
    WorkingSig = (EFI_SIGNATURE_LIST *) ((UINT8 *) WorkingSig + WorkingSig->SignatureListSize);
  }

  return SigFound ? WorkingSig : NULL;
}


/**
  Internal function to get the start address of next signature list.

  @param[in]      SignatureList       Pointer to EFI_SIGNATURE_LIST instance.

  @retval NULL                   SignatureList is NULL or the data in SignatureList is incorrect.
  @return EFI_SIGNATURE_LIST *   Pointer to next signature list.
**/
STATIC
EFI_SIGNATURE_LIST *
GetNextSignatureList (
  IN EFI_SIGNATURE_LIST        *SignatureList
  )
/*++

Routine Description:

  Internal function to get the start address of next signature list.

Arguments:

  SignatureList         - Pointer to EFI_SIGNATURE_LIST instance.

Returns:

  NULL                  - SignatureList is NULL of the data in SignatureList is incorrect.
  EFI_SIGNATURE_LIST *  - Pointer to next signature list.

--*/
{
  if (SignatureList == NULL || SignatureList->SignatureListSize < sizeof (EFI_SIGNATURE_LIST)) {
    return NULL;
  }

  return (EFI_SIGNATURE_LIST *) ((UINT8 *) SignatureList + SignatureList->SignatureListSize);
}

/**
  Merge two buffers which formatted as EFI_SIGNATURE_LIST. Only the new EFI_SIGNATURE_DATA+
  will be appended to the original EFI_SIGNATURE_LIST, duplicate EFI_SIGNATURE_DATA
  will be ignored.

  @param[in]      AppendDataSize       Size of NewData buffer.
  @param[in]      AppendDataBuffer     Pointer to new EFI_SIGNATURE_LIST to be appended.
  @param[in]      TotalDataBufferSize  Total size of data buffer which can be used.
  @param[in, out] UsedDataBufferSize   [in]  Used data size by byte before invoking this function.
                                       [out] Used data size by byte after invoking this function.
  @param[in, out] DataBuffer           [in]  Buffer to save original variable data.
                                       [out] Buffer to save merged variable data.

  @retval EFI_SUCCESS                  Append data to singnature list successful.
  @retval EFI_INVALID_PARAMETER        Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES         Total variable data size is large than maximum buffer size.
**/
EFI_STATUS
AppendSignatureList (
  IN       UINTN               AppendDataSize,
  IN CONST UINT8               *AppendDataBuffer,
  IN       UINTN               TotalDataBufferSize,
  IN OUT   UINTN               *UsedDataBufferSize,
  IN OUT   UINT8               *DataBuffer
  )
{
  UINTN                     AppendDataOffset;
  UINTN                     CurrentUsedDataSize;
  EFI_SIGNATURE_LIST        *WorkingAppendSig;
  EFI_SIGNATURE_LIST        *WorkingSig;
  EFI_SIGNATURE_LIST        *NextSig;
  EFI_SIGNATURE_DATA        *WorkingAppendSigData;
  EFI_SIGNATURE_DATA        *WorkingSigData;
  UINTN                     DataOffest;
  UINTN                     AppendSigCount;
  UINTN                     SigCount;
  UINTN                     AppendIndex;
  UINTN                     CurrentIndex;
  UINT32                    CurrentSigSize;
  UINT32                    AppendSigSize;
  BOOLEAN                   CertFound;


  if (AppendDataSize == 0) {
    return EFI_SUCCESS;
  }

  if (AppendDataBuffer == NULL || UsedDataBufferSize == NULL || DataBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CurrentUsedDataSize = *UsedDataBufferSize;
  WorkingAppendSig = (EFI_SIGNATURE_LIST *) AppendDataBuffer;

  //
  // Paring whole appended signature list
  //
  AppendDataOffset = 0;
  while (AppendDataOffset < AppendDataSize) {
    //
    // check append singnature list is a valid signature list
    //
    if (WorkingAppendSig->SignatureListSize < sizeof (EFI_SIGNATURE_LIST)) {
      return EFI_INVALID_PARAMETER;
    }
    AppendSigSize        = WorkingAppendSig->SignatureSize;
    AppendSigCount       = (WorkingAppendSig->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - WorkingAppendSig->SignatureHeaderSize) / AppendSigSize;
    WorkingAppendSigData = (EFI_SIGNATURE_DATA *) (((UINT8 *) WorkingAppendSig) + sizeof (EFI_SIGNATURE_LIST) + WorkingAppendSig->SignatureHeaderSize);
    //
    // Parsing specific appended signature list
    //
    for (AppendIndex = 0; AppendIndex < AppendSigCount; AppendIndex++) {
      WorkingSig     = (EFI_SIGNATURE_LIST *) DataBuffer;
      CertFound      = FALSE;
      //
      // Try to find matched signature (use signature type, signature owner and signature data) from whole existed signature list
      //
      DataOffest = 0;
      while (DataOffest < CurrentUsedDataSize) {
        if (CompareGuid (&WorkingSig->SignatureType, &WorkingAppendSig->SignatureType) &&
            WorkingAppendSig->SignatureSize == WorkingSig->SignatureSize) {
          CurrentSigSize = WorkingSig->SignatureSize;
          SigCount       = (WorkingSig->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - WorkingSig->SignatureHeaderSize) / CurrentSigSize;
          WorkingSigData = (EFI_SIGNATURE_DATA *) (((UINT8 *) WorkingSig) + sizeof (EFI_SIGNATURE_LIST) + WorkingSig->SignatureHeaderSize);
          for (CurrentIndex = 0; CurrentIndex < SigCount; CurrentIndex++) {
            //
            // compare certificated data and signature owner to confirm the existence of appened signature data
            //
            if (CompareMem (WorkingSigData, WorkingAppendSigData, CurrentSigSize) == 0) {
              CertFound = TRUE;
              break;
            }
            WorkingSigData =  (EFI_SIGNATURE_DATA *) ((UINT8 *) WorkingSigData + CurrentSigSize);
          }
        }
        if (CertFound) {
          break;
        }
        DataOffest += WorkingSig->SignatureListSize;
        WorkingSig = GetNextSignatureList (WorkingSig);
        if (WorkingSig == NULL) {
          return EFI_INVALID_PARAMETER;
        }
      }

      //
      // Add signature data to current signature list if can not find mathced signature data
      //
      if (!CertFound) {
        WorkingSig = FindCanMergeSig ((EFI_SIGNATURE_LIST *) DataBuffer, CurrentUsedDataSize, WorkingAppendSig);
        if (WorkingSig != NULL) {
          if (CurrentUsedDataSize + AppendSigSize > TotalDataBufferSize) {
            return EFI_OUT_OF_RESOURCES;
          }
          //
          // merge appended signature node to signature list
          //
          NextSig = GetNextSignatureList (WorkingSig);
          CopyMem (
            (UINT8 *) NextSig + AppendSigSize,
            NextSig,
            CurrentUsedDataSize - (UINTN) ((UINT8 *) NextSig - DataBuffer)
            );
          CopyMem (NextSig, WorkingAppendSigData, AppendSigSize);

          WorkingSig->SignatureListSize += AppendSigSize;
          CurrentUsedDataSize += AppendSigSize;
        } else {
          if (CurrentUsedDataSize + sizeof (EFI_SIGNATURE_LIST) + AppendSigSize > TotalDataBufferSize) {
            return EFI_OUT_OF_RESOURCES;
          }
          //
          // Append signature list to the trail of signature list.
          //
          WorkingSig = (EFI_SIGNATURE_LIST *) (DataBuffer + CurrentUsedDataSize);
          CopyMem (WorkingSig, WorkingAppendSig, sizeof (EFI_SIGNATURE_LIST));
          WorkingSig->SignatureListSize = AppendSigSize + sizeof (EFI_SIGNATURE_LIST);
          WorkingSig++;
          CopyMem (WorkingSig, WorkingAppendSigData, AppendSigSize);
          CurrentUsedDataSize += (sizeof (EFI_SIGNATURE_LIST) + AppendSigSize);
        }
      }
      WorkingAppendSigData =  (EFI_SIGNATURE_DATA *) ((UINT8 *) WorkingAppendSigData + AppendSigSize);
    }
    AppendDataOffset += WorkingAppendSig->SignatureListSize;
    WorkingAppendSig = GetNextSignatureList (WorkingAppendSig);
    if (WorkingAppendSig == NULL) {
      return EFI_INVALID_PARAMETER;
    }
  }
  *UsedDataBufferSize = CurrentUsedDataSize;

  return EFI_SUCCESS;

}

/**
  Compare two EFI_TIME data.

  @param[in] FirstTime   A pointer to the first EFI_TIME data.
  @param[in] SecondTime  A pointer to the second EFI_TIME data.

  @retval TRUE           The FirstTime is not later than the SecondTime.
  @retval FALSE          The FirstTime is later than the SecondTime.
**/
BOOLEAN
CompareTimeStamp (
  IN EFI_TIME               *FirstTime,
  IN EFI_TIME               *SecondTime
  )
{
  if (FirstTime->Year != SecondTime->Year) {
    return (BOOLEAN) (FirstTime->Year < SecondTime->Year);
  } else if (FirstTime->Month != SecondTime->Month) {
    return (BOOLEAN) (FirstTime->Month < SecondTime->Month);
  } else if (FirstTime->Day != SecondTime->Day) {
    return (BOOLEAN) (FirstTime->Day < SecondTime->Day);
  } else if (FirstTime->Hour != SecondTime->Hour) {
    return (BOOLEAN) (FirstTime->Hour < SecondTime->Hour);
  } else if (FirstTime->Minute != SecondTime->Minute) {
    return (BOOLEAN) (FirstTime->Minute < SecondTime->Minute);
  }

  return (BOOLEAN) (FirstTime->Second <= SecondTime->Second);
}


/**
  This function uses to synchronize authenticated variable data and state between
  runtime and SMM.

  @param[in] Global    Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS  Synchronize authenticated data and state successful.
**/
EFI_STATUS
SyncAuthData (
  IN  VARIABLE_GLOBAL         *Global
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINT8                   VarValue;
  UINT32                  VarAttr;
  UINT8                   *Data;
  UINTN                   DataSize;
  UINTN                   VariableCount;
  UINT8                   SecureBootEnforce;
  UINT8                   *BootType;
  //
  // Check "AuthVarKeyDatabase" variable's existence.
  // If it doesn't exist, create a new one with initial value of 0 and EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  //
  VariableCount = 0;
  Status = FindVariableByLifetime (
             AUTHVAR_KEYDB_NAME,
             &gEfiAuthenticatedVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );

  if (Variable.CurrPtr == NULL) {
    VarAttr       = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
    VarValue      = 0;
    mPubKeyNumber = 0;
    Status        = UpdateVariable (
                      AUTHVAR_KEYDB_NAME,
                      &gEfiAuthenticatedVariableGuid,
                      &VarValue,
                      sizeof(UINT8),
                      VarAttr,
                      0,
                      0,
                      &Variable,
                      NULL,
                      Global
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    //
    // Load database in global variable for cache.
    //
    DataSize  = DataSizeOfVariable (Variable.CurrPtr);
    Data      = GetVariableDataPtr (Variable.CurrPtr);
    ASSERT ((DataSize != 0) && (Data != NULL));
    CopyMem (mPubKeyStore, (UINT8 *) Data, DataSize);
    mPubKeyNumber = (UINT32) (DataSize / EFI_CERT_TYPE_RSA2048_SIZE);
  }

  //
  // Check PK database's existence to determine the SetupMode value.
  // Then create a new "SetupMode" with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  //
  VariableCount = 0;
  Status = FindVariableByLifetime (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );
  if (Variable.CurrPtr == NULL) {
    mPlatformMode = SETUP_MODE;
    mPlatformBootMode = SECURE_BOOT_MODE_DISABLE;
  } else {
    mPlatformMode = USER_MODE;
    mPlatformBootMode = SECURE_BOOT_MODE_ENABLE;
    Status = FindVariableByLifetime (
               L"BootType",
               &gSystemConfigurationGuid,
               &Variable,
               &VariableCount,
               Global
               );
    //
    // Disable secure boot if boot mode isn't UEFI boot mode.
    //
    if (!EFI_ERROR (Status)) {
      BootType = GetVariableDataPtr (Variable.CurrPtr);
      if (*BootType != EFI_BOOT_TYPE) {
        mPlatformBootMode = SECURE_BOOT_MODE_DISABLE;
      }
    }
    //
    // Disable secure boot, if SecureBootEnforce is disabled
    //
    if (mPlatformBootMode == SECURE_BOOT_MODE_ENABLE) {
      Status = FindVariableByLifetime (
                 EFI_SECURE_BOOT_ENFORCE_NAME,
                 &gEfiGenericVariableGuid,
                 &Variable,
                 &VariableCount,
                 Global
                 );
      SecureBootEnforce = 1;
      if (!EFI_ERROR (Status)) {
        Data = GetVariableDataPtr (Variable.CurrPtr);
        SecureBootEnforce = (*Data == 1) ? 1 : 0;
      }
      mPlatformBootMode = (SecureBootEnforce == 1) ? SECURE_BOOT_MODE_ENABLE : SECURE_BOOT_MODE_DISABLE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This function uses to find secure boot database default variable.

  @param[in]  VariableName       Name of Variable to be found.
  @param[in]  VendorGuid         Variable vendor GUID.
  @param[out] DataSize           Size of variable data by byte.
  @param[out] Data               Data buffer to save variable data.

  @retval EFI_SUCCESS            Find input secure boot database default variable successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Cannot find secure boot database default variable.
**/
EFI_STATUS
FindSecureDatabaseDefaultVariables (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINTN             *DataSize    OPTIONAL,
  OUT     VOID              **Data       OPTIONAL
  )
{
  EFI_STATUS                Status;
  UINTN                     HeaderSize;
  UINTN                     CurrentSearchedSize;
  VARIABLE_HEADER           *VariableHeader;
  VARIABLE_HEADER           *NexVariable;
  UINTN                     Index;
  BOOLEAN                   CheckFunFound;
  BOOLEAN                   VariableFound;
  SPECIFIC_VARIABLE         CheckFunTalbe[DEFAULT_VARIALBE_NUM][2] = {
                              {IsPkVariable,   IsPkDefaultVariable},
                              {IsKekVariable,  IsKekDefaultVariable},
                              {IsDbVariable,   IsDbDefaultVariable},
                              {IsDbxVariable,  IsDbxDefaultVariable}
                              };

  if (!IsSecureDatabaseDefaultVariable (VariableName, VendorGuid)) {
    return EFI_INVALID_PARAMETER;
  }

  HeaderSize = sizeof (UINT64) + sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  if (HeaderSize > mVariableModuleGlobal->FactoryDefaultSize) {
    return EFI_NOT_FOUND;
  }

  CheckFunFound = FALSE;
  for (Index = 0; Index < DEFAULT_VARIALBE_NUM; Index++) {
    if (CheckFunTalbe[Index][1] (VariableName, VendorGuid)) {
      CheckFunFound = TRUE;
      break;
    }
  }

  if (!CheckFunFound) {
    return EFI_NOT_FOUND;
  }

  VariableHeader      = (VARIABLE_HEADER *) ((UINTN) mVariableModuleGlobal->FactoryDefaultBase + HeaderSize);
  CurrentSearchedSize = HeaderSize;
  VariableFound       = FALSE;
  while (IsValidVariableHeader (VariableHeader) && CurrentSearchedSize < mVariableModuleGlobal->FactoryDefaultSize) {
    NexVariable = GetNextVariablePtr (VariableHeader);
    if (VariableHeader->State == VAR_ADDED && CheckFunTalbe[Index][0] (GET_VARIABLE_NAME_PTR (VariableHeader), &VariableHeader->VendorGuid)) {
      VariableFound = TRUE;
      break;
    }
    CurrentSearchedSize += ((UINTN) NexVariable - (UINTN) VariableHeader);
    VariableHeader = NexVariable;
  }

  if (!VariableFound && !IsDbxDefaultVariable (VariableName, VendorGuid)) {
    return EFI_NOT_FOUND;
  }

  //
  // Needn't output data size and data, so just return EFI_SUCCESS.
  //
  if (DataSize == NULL || Data == NULL) {
    return EFI_SUCCESS;
  }

  //
  // Get variable data and variable size
  //
  if (VariableFound) {
    *Data = GetVariableDataPtr (VariableHeader);
    *DataSize = VariableHeader->DataSize;
  } else {
    *DataSize = APPEND_BUFF_SIZE;
    Status = CreateDummyDbxData (DataSize, mStorageArea);
    ASSERT_EFI_ERROR (Status);
    *Data = mStorageArea;
  }

  return EFI_SUCCESS;
}

/**
  This function uses to get secure boot database default variable.

  @param[in]      VariableName   Name of Variable to be found.
  @param[in]      VendorGuid     Variable vendor GUID.
  @param[out]     Attributes     Attribute value of the variable found.
  @param[in, out] DataSize       Size of Data found. If size is less than the
                                 data, this value contains the required size.
  @param[out]     Data           Data buffer to save variable data.

  @retval EFI_SUCCESS            Get secure boot database default variable successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Cannot find secure boot database default variable.
  @retval EFI_BUFFER_TOO_SMALL   Find Secure boot database default variable but input buffer size
                                 is too small.
**/
EFI_STATUS
GetSecureDatabaseDefaultVariables (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  )
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;
  UINT8                     *VariableData;

  Status = FindSecureDatabaseDefaultVariables (VariableName, VendorGuid, &VariableSize, (VOID **)&VariableData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (VariableSize > *DataSize) {
    *DataSize = VariableSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  if (Attributes != NULL) {
    *Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;;
  }

  CopyMem (Data, VariableData, VariableSize);
  *DataSize = VariableSize;

  return EFI_SUCCESS;
}

/**
  Update platform boot mode.

  @param[in] Mode      SECURE_BOOT_MODE_ENABLE or SECURE_BOOT_MODE_DISABLE.
  @param[in] Global    Pointer to VARIABLE_GLOBAL instance.
**/
VOID
UpdatePlatformBootMode (
  IN  UINT32              Mode,
  IN  VARIABLE_GLOBAL     *Global
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINT32                  VarAttr;
  UINTN                   VariableCount;


  if (AtRuntime ()) {
    //
    // SecureBoot Variable indicates whether the platform firmware is operating
    // in Secure boot mode (1) or not (0), so we should not change SecureBoot
    // Variable in runtime.
    //
    return;
  }
  VariableCount = 0;
  Status = FindVariableByLifetime (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );
  ASSERT_EFI_ERROR (Status);

  mPlatformBootMode  = Mode;

  VarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
  Status  = UpdateVariable (
              EFI_SECURE_BOOT_MODE_NAME,
              &gEfiGlobalVariableGuid,
              &mPlatformBootMode,
              sizeof (UINT8),
              VarAttr,
              0,
              0,
              &Variable,
              NULL,
              Global
              );
  ASSERT_EFI_ERROR (Status);
}
