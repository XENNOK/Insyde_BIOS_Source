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

#ifndef __EFI_CRYPTO_HASH_H__
#define __EFI_CRYPTO_HASH_H__

#include <Protocol/CryptoServices.h>
#include <Protocol/Hash.h>
#include <Protocol/ServiceBinding.h>
#include <Library/DebugLib.h>

#define CRYPTO_HASH_SIGNATURE          SIGNATURE_32 ('C', 'R', 'H','A')

typedef
UINTN
(EFIAPI *CRYPTO_GET_CONTEXT_SIZE) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_INIT) (
  OUT  VOID  *HashContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_DUPLICATE) (
  IN   CONST VOID  *HashContext,
  OUT  VOID        *NewHashContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_UDPATE) (
  IN OUT  VOID        *HashContext,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_FINAL) (
  IN OUT  VOID   *HashContext,
  OUT     UINT8  *HashValue
  );

typedef struct _CRYPTO_SELECTOR {
  EFI_GUID                 *Index;
  BOOLEAN                  NoPad;
  CRYPTO_GET_CONTEXT_SIZE  GetContextSize;
  CRYPTO_INIT              Init;
  CRYPTO_DUPLICATE         Duplicate;
  CRYPTO_UDPATE            Update;
  CRYPTO_FINAL             Final;
} CRYPTO_SELECTOR;

enum {
  CRYPTO_ALGORITHM_NO_PAD_SHA1,
  CRYPTO_ALGORITHM_NO_PAD_SHA256,
  CRYPTO_ALGORITHM_SHA1,
  CRYPTO_ALGORITHM_SHA224,
  CRYPTO_ALGORITHM_SHA256,
  CRYPTO_ALGORITHM_SHA384,
  CRYPTO_ALGORITHM_SHA512,
  CRYPTO_ALGORITHM_MD5,
  CRYPTO_ALGORITHM_MAX
} CRYPTO_SELETOR_TYPE;

/*
  Returns the size of the hash which results from a specific algorithm.

  @parma This                        Points to this instance of EFI_HASH_PROTOCOL.
  @parma HashAlgorithm               Points to the EFI_GUID which identifies the algorithm to use.
  @parma HashSize                    Holds the returned size of the algorithm¡¯s hash.

  @retval EFI_SUCCESS                Hash size returned successfully.
  @retval EFI_INVALID_PARAMETER      HashSize is NULL
  @retval EFI_UNSUPPORTED            The algorithm specified by HashAlgorithm is not supported
                                     by this driver.
*/
EFI_STATUS
EFIAPI
CryptGetHashSize (
  IN  CONST EFI_HASH_PROTOCOL     *This,
  IN  CONST EFI_GUID              *HashAlgorithm,
  OUT UINTN                       *HashSize
  );

/*
  Returns the size of the hash which results from a specific algorithm.

  @parma This                        Points to this instance of EFI_HASH_PROTOCOL.
  @parma HashAlgorithm               Points to the EFI_GUID which identifies the algorithm to use.
  @parma Extend                      Specifies whether to create a new hash (FALSE) or extend the specified
                                     existing hash (TRUE).
  @parma Message                     Points to the start of the message.
  @parma MessageSize                 The size of Message, in bytes.
  @parma Hash                        On input, if Extend is TRUE, then this holds the hash to extend. On
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
  );

/*
  Change address to virtual memory address
*/
VOID
CryptHashVirtualAddressChange (
  );

typedef struct _CRYPTO_HASH_INSTANCE {
  UINT32                               Signature;
  EFI_HASH_PROTOCOL                    HashProtocol;
  VOID                                 *Context[CRYPTO_ALGORITHM_MAX];
  BOOLEAN                              IsRoot;
} CRYPTO_HASH_INSTANCE;

#define CRYPTO_HASH_INSTANCE_FROM_THIS(a) \
  CR (a, \
      CRYPTO_HASH_INSTANCE, \
      HashProtocol, \
      CRYPTO_HASH_SIGNATURE \
      )

extern EFI_SERVICE_BINDING_PROTOCOL HashServiceBindingProtocol;

//
//Allocates memory blocks
//
void *malloc (UINTN size);
//
//Deallocates or frees a memory block
//
void free (void *ptr);

#define RTAllocatePool    malloc
#define RTFreePool        free

#endif