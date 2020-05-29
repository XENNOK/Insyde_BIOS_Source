/** @file
  Crypto Hash protocol implement

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

#include "CryptoHash.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/CryptoServices.h>

#define HASH_BLOCK_SIZE  64
extern EFI_RUNTIME_SERVICES         *gRT;

EFI_STATUS
EFIAPI
CryptoHashCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  );

EFI_STATUS
EFIAPI
CryptoHashDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL      *This,
  IN EFI_HANDLE                        ChildHandle
  );

CRYPTO_SELECTOR
mCryptoSelector [CRYPTO_ALGORITHM_MAX] = {
  { &gEfiHashAlgorithmSha1NoPadGuid,   TRUE,  NoPadSha1GetContextSize,   NoPadSha1Init,   NoPadSha1Duplicate,   NoPadSha1Update,   NoPadSha1Final   },  // NO PAD SHA 1
  { &gEfiHashAlgorithmSha256NoPadGuid, TRUE,  NoPadSha256GetContextSize, NoPadSha256Init, NoPadSha256Duplicate, NoPadSha256Update, NoPadSha256Final },  // NO PAD SHA 256
  { &gEfiHashAlgorithmSha1Guid,        FALSE, Sha1GetContextSize,        Sha1Init,        Sha1Duplicate,        Sha1Update,        Sha1Final        },  // SHA 1
  { &gEfiHashAlgorithmSha224Guid,      FALSE, NULL,                      NULL,            NULL,                 NULL,              NULL             },  // SHA 224
  { &gEfiHashAlgorithmSha256Guid,      FALSE, Sha256GetContextSize,      Sha256Init,      Sha256Duplicate,      Sha256Update,      Sha256Final      },  // SHA 256
  { &gEfiHashAlgorithmSha384Guid,      FALSE, NULL,                      NULL,            NULL,                 NULL,              NULL             },  // SHA 384
  { &gEfiHashAlgorithmSha512Guid,      FALSE, NULL,                      NULL,            NULL,                 NULL,              NULL             },  // SHA 512
  { &gEfiHashAlgorithmMD5Guid,         FALSE, Md5GetContextSize,         Md5Init,         Md5Duplicate,         Md5Update,         Md5Final         }   // MD5
  };

EFI_SERVICE_BINDING_PROTOCOL
HashServiceBindingProtocol = {
  CryptoHashCreateChild,
  CryptoHashDestroyChild
};

EFI_STATUS
EFIAPI
RuntimeCryptoHashCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
RuntimeCryptoHashDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL      *This,
  IN EFI_HANDLE                        ChildHandle
  )
{
  return EFI_UNSUPPORTED;
}

/*
  Service Binding protocol, create child function.

  @param This                            Indicate the instance condition.
  @param ChildHandle                     Indicate the handle of the child to create.
                                         If handle is NULL, function will create a new and install protocol.

  @retval EFI_SUCCESS                    Protocol create success.
  @retval EFI_INVALID_PARAMETER          ChildHandle is NULL.
                                         This is not belong this driver.
  @retval EFI_UNSUPPORTED                The protocol was not belong this driver or the protocol is root create.
  @retval other                          Error status from UninstallProtocolInterface.

*/
EFI_STATUS
EFIAPI
CryptoHashCreateChild (
  IN     EFI_SERVICE_BINDING_PROTOCOL  *This,
  IN OUT EFI_HANDLE                    *ChildHandle
  )
{
  CRYPTO_HASH_INSTANCE                 *Instance;

  if ((This->CreateChild != CryptoHashCreateChild) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = RTAllocatePool (sizeof (CRYPTO_HASH_INSTANCE));
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (Instance, sizeof (CRYPTO_HASH_INSTANCE));

  Instance->Signature = CRYPTO_HASH_SIGNATURE;
  Instance->HashProtocol.Hash        = CryptHash;
  Instance->HashProtocol.GetHashSize = CryptGetHashSize;
  return gBS->InstallProtocolInterface (
                ChildHandle,
                &gEfiHashProtocolGuid,
                EFI_NATIVE_INTERFACE,
                (VOID *) &(Instance->HashProtocol)
                );
}

/*
  Service Binding protocol, Destroy child function.

  @param This                            Indicate the instance condition.
  @param ChildHandle                     The Handle has the Hash protocol and create by CreateChild.

  @retval EFI_SUCCESS                    Protocol destroy success.
  @retval EFI_INVALID_PARAMETER          ChildHandle is NULL.
  @retval EFI_UNSUPPORTED                The protocol was not belong this driver or the protocol is root create.
  @retval other                          Error status from UninstallProtocolInterface.

*/
EFI_STATUS
EFIAPI
CryptoHashDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL      *This,
  IN EFI_HANDLE                        ChildHandle
  )
{
  EFI_STATUS                           Status;
  EFI_HASH_PROTOCOL                    *Hash;
  CRYPTO_HASH_INSTANCE                 *Instance;
  UINTN                                Index;

  if ((This->DestroyChild != CryptoHashDestroyChild) || (ChildHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  ChildHandle,
                  &gEfiHashProtocolGuid,
                  (VOID *) &Hash
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Instance = CRYPTO_HASH_INSTANCE_FROM_THIS (Hash);
  if ((Instance->Signature == CRYPTO_HASH_SIGNATURE) && (Instance->IsRoot)) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->UninstallProtocolInterface (
                  ChildHandle,
                  &gEfiHashProtocolGuid,
                  (VOID *) Hash
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < CRYPTO_ALGORITHM_MAX; Index++) {
    if (Instance->Context[Index] != NULL) {
      RTFreePool (Instance->Context[Index]);
    }
  }

  RTFreePool (Instance);
  return EFI_SUCCESS;
}

/*
  Change address to virtual memory address
*/
VOID
CryptHashVirtualAddressChange (
  )
{
  UINTN    Index;

  //
  // Modify the ServiceBinding protocol function for fake function, to avoid it was called in runtime.
  //
  HashServiceBindingProtocol.CreateChild  = RuntimeCryptoHashCreateChild;
  HashServiceBindingProtocol.DestroyChild = RuntimeCryptoHashDestroyChild;
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(HashServiceBindingProtocol.CreateChild));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(HashServiceBindingProtocol.DestroyChild));

  for (Index = 0; Index < CRYPTO_ALGORITHM_MAX; Index++) {
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(mCryptoSelector [Index].Index));
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(mCryptoSelector [Index].GetContextSize));
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(mCryptoSelector [Index].Init));
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(mCryptoSelector [Index].Duplicate));
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(mCryptoSelector [Index].Update));
    gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(mCryptoSelector [Index].Final));
  }
}

/*
  Compares two GUIDs

  @param Guid1                        guid to compare
  @param Guid2                        guid to compare

  @retval TRUE                        Guid is the same
  @retval FALSE                       Guid if different
*/
BOOLEAN
CryptCompareGuid (
  IN CONST EFI_GUID *Guid1,
  IN CONST EFI_GUID *Guid2
  )
{
  UINTN Index;

  //
  // Compare byte by byte
  //
  for (Index = 0; Index < 16; ++Index) {
    if (*(((UINT8*) Guid1) + Index) != *(((UINT8*) Guid2) + Index)) {
      return FALSE;
    }
  }
  return TRUE;
}

/*
  Search Crypto algorithm by EFI 2.3.1 definaion GUID

  @parma  GuidKey                    The Crypto algorithm chose
  @parma  Index                      The Algorithm array index

  @retval EFI_SUCCESS                Crypto has found
  @retval EFI_NOT_FOUND              Crypto algorithm not found or not support
*/
EFI_STATUS
CryptGetIndexByGuid (
  IN   CONST EFI_GUID    *GuidKey,
  OUT  UINTN             *Index
  )
{
  UINTN    LoopIndex;
  *Index = 0;
  for (LoopIndex = CRYPTO_ALGORITHM_NO_PAD_SHA1; LoopIndex < CRYPTO_ALGORITHM_MAX; LoopIndex++) {
    if (CryptCompareGuid ( mCryptoSelector[LoopIndex].Index, GuidKey)) {
      *Index = LoopIndex;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/*
  Returns the size of the hash which results from a specific algorithm.

  @parma  HashAlgorithm              Points to the EFI_GUID which identifies the algorithm to use.
  @parma  HashSize                   Holds the returned size of the algorithm¡¯s hash.
  @parma  Index                      Index of Algorithm selector

  @retval EFI_SUCCESS                Hash size returned successfully.
  @retval EFI_NOT_FOUND              Algorithm not found
  @retval EFI_INVALID_PARAMETER      HashSize is NULL
  @retval EFI_UNSUPPORTED            The algorithm specified by HashAlgorithm is not supported
                                     by this driver.
--*/
EFI_STATUS
CryptGetHashSizeIndex (
  IN  CONST EFI_GUID              *HashAlgorithm,
  OUT UINTN                       *HashSize,
  OUT UINTN                       *Index  OPTIONAL
  )
{
  EFI_STATUS   Status;
  UINTN        SelectorIndex;

  Status = CryptGetIndexByGuid ( (EFI_GUID *)HashAlgorithm, &SelectorIndex);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (mCryptoSelector[SelectorIndex].GetContextSize == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (Index != NULL) {
    *Index = SelectorIndex;
  }
  *HashSize = mCryptoSelector[SelectorIndex].GetContextSize ();

  return EFI_SUCCESS;
}

/*++
  Returns the size of the hash which results from a specific algorithm.

  @parma  This                       Points to this instance of EFI_HASH_PROTOCOL.
  @parma  HashAlgorithm              Points to the EFI_GUID which identifies the algorithm to use.
  @parma  HashSize                   Holds the returned size of the algorithm¡¯s hash.

  @retval EFI_SUCCESS                Hash size returned successfully.
  @retval EFI_NOT_FOUND              Algorithm not found
  @retval EFI_INVALID_PARAMETER      HashSize is NULL
  @retval EFI_UNSUPPORTED            The algorithm specified by HashAlgorithm is not supported
                                     by this driver.
--*/
EFI_STATUS
EFIAPI
CryptGetHashSize (
  IN  CONST EFI_HASH_PROTOCOL     *This,
  IN  CONST EFI_GUID              *HashAlgorithm,
  OUT UINTN                       *HashSize
  )
{
  EFI_STATUS   Status;
  UINTN        AlgoIndex;

  if ((This == NULL) || (HashSize == NULL) || (HashAlgorithm == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CryptGetIndexByGuid ( (EFI_GUID *)HashAlgorithm, &AlgoIndex);
  if (EFI_ERROR (Status)) {
    //
    // If not found
    //
    return EFI_UNSUPPORTED;
  }

  if (mCryptoSelector[AlgoIndex].Init == NULL) {
    //
    // Unsupport this algorithm
    //
    return EFI_UNSUPPORTED;
  }

  //
  // Translate Algorithm to HASH size
  //
  switch (AlgoIndex) {
  case CRYPTO_ALGORITHM_NO_PAD_SHA1:
  case CRYPTO_ALGORITHM_SHA1:
    *HashSize = sizeof (EFI_SHA1_HASH);
    break;

  case CRYPTO_ALGORITHM_SHA224:
    *HashSize = sizeof (EFI_SHA224_HASH);
    break;

  case CRYPTO_ALGORITHM_NO_PAD_SHA256:
  case CRYPTO_ALGORITHM_SHA256:
    *HashSize = sizeof (EFI_SHA256_HASH);
    break;

  case CRYPTO_ALGORITHM_SHA384:
    *HashSize = sizeof (EFI_SHA384_HASH);
    break;

  case CRYPTO_ALGORITHM_SHA512:
    *HashSize = sizeof (EFI_SHA512_HASH);
    break;

  case CRYPTO_ALGORITHM_MD5:
    *HashSize = sizeof (EFI_MD5_HASH);
    break;

  default:
    *HashSize = 0;
  }
  return EFI_SUCCESS;
}

/*++
  Returns the size of the hash which results from a specific algorithm.

  @parma  This                       Points to this instance of EFI_HASH_PROTOCOL.
  @parma  HashAlgorithm              Points to the EFI_GUID which identifies the algorithm to use.
  @parma  Extend                     Specifies whether to create a new hash (FALSE) or extend the specified
                                     existing hash (TRUE).
  @parma  Message                    Points to the start of the message.
  @parma  MessageSize                The size of Message, in bytes.
  @parma  Hash                       On input, if Extend is TRUE, then this holds the hash to extend. On
                                     output, holds the resulting hash computed from the message.

  @retval EFI_SUCCESS                Hash returned successfully.
  @retval EFI_INVALID_PARAMETER      Message or Hash is NULL
  @retval EFI_UNSUPPORTED            The algorithm specified by HashAlgorithm is not supported by this
                                     driver. Or extend is TRUE and the algorithm doesn¡¯t support extending the hash.
--*/
EFI_STATUS
EFIAPI
CryptHash (
  IN CONST EFI_HASH_PROTOCOL      *This,
  IN CONST EFI_GUID               *HashAlgorithm,
  IN BOOLEAN                      Extend,
  IN CONST UINT8                  *Message,
  IN UINT64                       MessageSize,
  IN OUT EFI_HASH_OUTPUT          *Hash
  )
{
  EFI_STATUS               Status;
  CRYPTO_HASH_INSTANCE     *Instance;
  BOOLEAN                  Result;
  UINTN                    CtxSize;
  UINTN                    HashSize;
  UINTN                    Index;
  CRYPTO_SELECTOR          *CryptoHash;
  VOID                     *HashCtx;
  VOID                     *ResultHash;

  Result     = FALSE;
  CtxSize    = 0;
  Status     = EFI_SUCCESS;
  ResultHash = NULL;

  if (
      (This == NULL) ||
      (Hash == NULL) ||
      (Hash->Sha256Hash == NULL) ||
      (Message == NULL) ||
      (HashAlgorithm == NULL)
    ) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = CRYPTO_HASH_INSTANCE_FROM_THIS (This);

  //
  // Get Hash result size and hash context size, and detect the function capability.
  //
  Status = CryptGetHashSizeIndex (HashAlgorithm, &CtxSize, &Index);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  Status = CryptGetHashSize (This, HashAlgorithm, &HashSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mCryptoSelector[Index].NoPad && MessageSize % HASH_BLOCK_SIZE != 0) {
    return EFI_INVALID_PARAMETER;
  }

  ResultHash = RTAllocatePool (CtxSize);
  if (ResultHash == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ERROR_EXIT;
  }

  //
  // Check HASH context, if Extend then initialize it.
  //
  CryptoHash = &(mCryptoSelector[Index]);
  HashCtx    = Instance->Context[Index];
  if (HashCtx == NULL) {
    if (Extend) {
      Status = EFI_UNSUPPORTED;
      goto ERROR_EXIT;
    }
    HashCtx = RTAllocatePool (CtxSize);
    if (HashCtx == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ERROR_EXIT;
    }
  }

  //
  // If Extend is FALSE, initial the Hash context for new HASH.
  //
  if (!Extend) {
    CryptoHash->Init (HashCtx);
  }

  CryptoHash->Update (HashCtx, Message, (UINTN)MessageSize);
  CryptoHash->Duplicate (HashCtx, ResultHash);
  CryptoHash->Final (ResultHash, (UINT8*)(UINTN)Hash->Sha256Hash);
  Instance->Context[Index] = HashCtx;
  Status = EFI_SUCCESS;

ERROR_EXIT:
  if (ResultHash != NULL) {
    RTFreePool (ResultHash);
    ResultHash = NULL;
  }
  return Status;
}
