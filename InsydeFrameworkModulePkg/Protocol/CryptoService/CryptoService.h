//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    CryptoService.h

Abstract:

  Crypto Service Protocol

--*/
/*++ @file
  The crypto service protocol.
  Only limited crypto primitives (SHA-256 and RSA) are provided for runtime
  authenticated variable service.

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

--*/

#ifndef __EFI_CRYPTO_SERVICE_PROTOCOL_H__
#define __EFI_CRYPTO_SERVICE_PROTOCOL_H__
//
// Crypto Service Protocol GUID.
//
#define EFI_CRYPTO_SERVICE_PROTOCOL_GUID \
  { \
    0xe1475e0c, 0x1746, 0x4802, { 0x86, 0x2e, 0x1, 0x1c, 0x2c, 0x2d, 0x9d, 0x86 } \
  }

EFI_FORWARD_DECLARATION (EFI_CRYPTO_SERVICE_PROTOCOL);

///
/// MD4 digest size in bytes
///
#define MD4_DIGEST_SIZE     16

///
/// MD5 digest size in bytes
///
#define MD5_DIGEST_SIZE     16

///
/// SHA-1 digest size in bytes.
///
#define SHA1_DIGEST_SIZE    20

///
/// SHA-256 digest size in bytes
///
#define SHA256_DIGEST_SIZE  32

///
/// TDES block size in bytes
///
#define TDES_BLOCK_SIZE     8

///
/// AES block size in bytes
///
#define AES_BLOCK_SIZE      16

//
// Max Known Digest Size is SHA256 Output (32 bytes) by far
//
#define MAX_DIGEST_SIZE  SHA256_DIGEST_SIZE
///
/// RSA Key Tags Definition used in RsaSetKey() function for key component identification.
///
typedef enum {
  RsaKeyN,      ///< RSA public Modulus (N)
  RsaKeyE,      ///< RSA Public exponent (e)
  RsaKeyD,      ///< RSA Private exponent (d)
  RsaKeyP,      ///< RSA secret prime factor of Modulus (p)
  RsaKeyQ,      ///< RSA secret prime factor of Modules (q)
  RsaKeyDp,     ///< p's CRT exponent (== d mod (p - 1))
  RsaKeyDq,     ///< q's CRT exponent (== d mod (q - 1))
  RsaKeyQInv    ///< The CRT coefficient (== 1/q mod p)
} RSA_KEY_TAG;

typedef
UINTN
(EFIAPI *EFI_CRYPTO_SERVICE_GET_CONTEXT_SIZE) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_DUPLICATE) (
  IN   CONST VOID                           *Context,
  OUT  VOID                                 *NewContext
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_UPDATE) (
  IN OUT  VOID                              *Context,
  IN      CONST VOID                        *Data,
  IN      UINTN                             DataSize
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_FINAL) (
  IN OUT  VOID                              *Context,
  OUT     UINT8                             *Value
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_SHA1_INIT) (
  OUT  VOID  *Sha1Context
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_INIT) (
  OUT  VOID                                 *Context,
  IN   CONST UINT8                          *Key,
  IN   UINTN                                KeyLength
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_ECB_ENCRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_ECB_DECRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_CBC_ENCRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  IN   CONST UINT8                          *Ivec,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_CBC_DECRYPT) (
  IN   VOID                                  *Context,
  IN   CONST UINT8                           *Input,
  IN   UINTN                                 InputSize,
  IN   CONST UINT8                           *Ivec,
  OUT  UINT8                                 *Output
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_GET_KEY) (
  IN OUT VOID                                  *RsaContext,
  IN     RSA_KEY_TAG                           KeyTag,
  OUT    UINT8                                 *BigNumber,
  IN OUT UINTN                                 *BnLength
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_PKCS1_SIGN) (
  IN      VOID                              *RsaContext,
  IN      CONST UINT8                       *MessageHash,
  IN      UINTN                             HashSize,
  OUT     UINT8                             *Signature,
  IN OUT  UINTN                             *SigSize
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_GET_PRIVATE_KEY_FROM_PEM) (
  IN   CONST UINT8                                       *PemData,
  IN   UINTN                                             PemSize,
  IN   CONST CHAR8                                       *Password,
  OUT  VOID                                              **RsaContext
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_GET_PUBLIC_KEY_FROM_X509) (
  IN   CONST UINT8                                       *Cert,
  IN   UINTN                                             CertSize,
  OUT  VOID                                              **RsaContext
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_X509_GET_SUBJECT_NAME) (
  IN      CONST UINT8                                    *Cert,
  IN      UINTN                                          CertSize,
  OUT     UINT8                                          *CertSubject,
  IN OUT  UINTN                                          *SubjectSize
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_X509_VERIFY_CERT) (
  IN  CONST UINT8                                    *Cert,
  IN  UINTN                                          CertSize,
  IN  CONST UINT8                                    *CACert,
  IN  UINTN                                          CACertSize
  );

typedef
VOID *
(EFIAPI *EFI_CRYPTO_SERVICE_DH_NEW) (
  VOID
  );

typedef
VOID
(EFIAPI *EFI_CRYPTO_SERVICE_DH_FREE) (
  IN  VOID                                               *DhContext
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_DH_GENERATE_PARAMETER) (
  IN OUT  VOID                                           *DhContext,
  IN      UINTN                                          Generator,
  IN      UINTN                                          PrimeLength,
  OUT     UINT8                                          *Prime
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_DH_SET_PARAMETER) (
  IN OUT  VOID                                           *DhContext,
  IN      UINTN                                          Generator,
  IN      UINTN                                          PrimeLength,
  IN      CONST UINT8                                    *Prime
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_DH_GENERATE_KEY) (
  IN OUT  VOID                                           *DhContext,
  OUT     UINT8                                          *PublicKey,
  IN OUT  UINTN                                          *PublicKeySize
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_DH_COMPUTE_KEY) (
  IN OUT  VOID                                           *DhContext,
  IN      CONST UINT8                                    *PeerPublicKey,
  IN      UINTN                                          PeerPublicKeySize,
  OUT     UINT8                                          *Key,
  IN OUT  UINTN                                          *KeySize
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_DH_COMPUTE_KEY) (
  IN OUT  VOID                                           *DhContext,
  IN      CONST UINT8                                    *PeerPublicKey,
  IN      UINTN                                          PeerPublicKeySize,
  OUT     UINT8                                          *Key,
  IN OUT  UINTN                                          *KeySize
  );

typedef
VOID *
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_NEW) (
  VOID
  );

typedef
VOID
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_FREE) (
  IN  VOID                                  *RsaContext
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_SET_KEY) (
  IN OUT VOID                                  *RsaContext,
  IN     RSA_KEY_TAG                           KeyTag,
  IN     CONST UINT8                           *BigNumber,
  IN     UINTN                                 BnLength
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_PKCS1_VERIFY) (
  IN  VOID                                  *RsaContext,
  IN  CONST UINT8                           *MessageHash,
  IN  UINTN                                 HashLength,
  IN  CONST UINT8                           *Signature,
  IN  UINTN                                 SigLength
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_PKCS7_VERIFY) (
  IN  CONST UINT8                           *P7Data,
  IN  UINTN                                 P7Length,
  IN  CONST UINT8                           *TrustedCert,
  IN  UINTN                                 CertLength,
  IN  CONST UINT8                           *InData,
  IN  UINTN                                 DataLength,
  IN  BOOLEAN                               AuthentiCodeFlag
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_PKCS7_VERIFY_USING_PUBKEY) (
  IN	CONST UINT8                                        *P7Data,
  IN  UINTN 	                                           P7Length,
  IN	CONST UINT8                                        *TrustedPubKey,
  IN	UINTN		                                           TrustedPubKeyLen,
  IN  CONST UINT8                                        *InData,
  IN  UINTN 	                                           DataLength
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_AUTHENTICODE_VERIFY) (
  IN  CONST UINT8                                        *AuthData,
  IN  UINTN                                              DataLength,
  IN  CONST UINT8                                        *TrustedCert,
  IN  UINTN                                              CertLength,
  IN  CONST UINT8                                        *ImageHash,
  IN  UINTN                                              HashLength
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RAMDOM_SEED) (
  IN  CONST  UINT8                                       *Seed  OPTIONAL,
  IN  UINTN                                              SeedSize
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_RANDOM_BYTES) (
  OUT  UINT8                                             *Output,
  IN   UINTN                                             Size
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_PKCS7_GET_SIGNERS) (
  IN  CONST UINT8  *P7Data,
  IN  UINTN        P7Length,
  OUT UINT8        **CertStack,
  OUT UINTN        *StackLength,
  OUT UINT8        **TrustedCert,
  OUT UINTN        *CertLength
  );

typedef
VOID
(EFIAPI *EFI_CRYPTO_SERVICE_PKCS7_FREE_SIGNERS) (
  IN  UINT8        *Certs
  );

typedef
EFI_STATUS
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT) (
  IN     VOID        *RsaContext,
  IN     CONST UINT8 *Signature,
  IN     UINTN       SignatureSize,
  OUT    UINT8       *Message,
  IN OUT UINTN       *MessageSize
  )
/*++

Routine Description:

  Decrypt the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
  RSA PKCS#1.

Arguments:

  RsaContext     Pointer to RSA context for signature verification.
  Signature      Pointer to RSA PKCS1-v1_5 signature to be verified.
  SignatureSize  Size of signature in bytes.
  Message        Pointer to buffer to receive RSA PKCS1-v1_5 decrypted message.
  MessageSize    Size of the decrypted message in bytes.

Returns:

  EFI_SUCCESS            Decrypt input signature successfully.
  EFI_BUFFER_TOO_SMALL   The Message buffer was too small. The current buffer size needed to hold the
                         message is returned in MessageSize.
  EFI_INVALID_PARAMETER  1. RsaContext is NULL, Signature is NULL or SignatureSize is 0
                         2. MessageSize is NULL or the Content of MessageSize isn't 0 and Message is NULL.

--*/
;

//
// Crypto Service Protocol Structure.
//
typedef struct _EFI_CRYPTO_SERVICE_PROTOCOL{
  EFI_CRYPTO_SERVICE_GET_CONTEXT_SIZE            Sha1GetContextSize;
  EFI_CRYPTO_SERVICE_SHA1_INIT                   Sha1Init;
  EFI_CRYPTO_SERVICE_DUPLICATE                   Sha1Duplicate;
  EFI_CRYPTO_SERVICE_UPDATE                      Sha1Update;
  EFI_CRYPTO_SERVICE_FINAL                       Sha1Final;

  EFI_CRYPTO_SERVICE_GET_CONTEXT_SIZE            HmacSha1GetContextSize;
  EFI_CRYPTO_SERVICE_INIT                        HmacSha1Init;
  EFI_CRYPTO_SERVICE_DUPLICATE                   HmacSha1Duplicate;
  EFI_CRYPTO_SERVICE_UPDATE                      HmacSha1Update;
  EFI_CRYPTO_SERVICE_FINAL                       HmacSha1Final;

  EFI_CRYPTO_SERVICE_GET_CONTEXT_SIZE            TdesGetContextSize;
  EFI_CRYPTO_SERVICE_INIT                        TdesInit;
  EFI_CRYPTO_SERVICE_ECB_ENCRYPT                 TdesEcbEncrypt;
  EFI_CRYPTO_SERVICE_ECB_DECRYPT                 TdesEcbDecrypt;
  EFI_CRYPTO_SERVICE_CBC_ENCRYPT                 TdesCbcEncrypt;
  EFI_CRYPTO_SERVICE_CBC_DECRYPT                 TdesCbcDecrypt;

  EFI_CRYPTO_SERVICE_GET_CONTEXT_SIZE            AesGetContextSize;
  EFI_CRYPTO_SERVICE_INIT                        AesInit;
  EFI_CRYPTO_SERVICE_ECB_ENCRYPT                 AesEcbEncrypt;
  EFI_CRYPTO_SERVICE_ECB_DECRYPT                 AesEcbDecrypt;
  EFI_CRYPTO_SERVICE_CBC_ENCRYPT                 AesCbcEncrypt;
  EFI_CRYPTO_SERVICE_CBC_DECRYPT                 AesCbcDecrypt;

  EFI_CRYPTO_SERVICE_RSA_NEW                     RsaNew;
  EFI_CRYPTO_SERVICE_RSA_FREE                    RsaFree;
  EFI_CRYPTO_SERVICE_RSA_GET_KEY                 RsaGetKey;
  EFI_CRYPTO_SERVICE_RSA_SET_KEY                 RsaSetKey;
  EFI_CRYPTO_SERVICE_RSA_PKCS1_SIGN              RsaPkcs1Sign;
  EFI_CRYPTO_SERVICE_RSA_PKCS1_VERIFY            RsaPkcs1Verify;
  EFI_CRYPTO_SERVICE_RSA_GET_PRIVATE_KEY_FROM_PEM  RsaGetPrivateKeyFromPem;
  EFI_CRYPTO_SERVICE_RSA_GET_PUBLIC_KEY_FROM_X509  RsaGetPublicKeyFromX509;
  EFI_CRYPTO_SERVICE_X509_GET_SUBJECT_NAME       X509GetSubjectName;
  EFI_CRYPTO_SERVICE_X509_VERIFY_CERT            X509VerifyCert;

  EFI_CRYPTO_SERVICE_PKCS7_VERIFY                Pkcs7Verify;
  EFI_CRYPTO_SERVICE_PKCS7_VERIFY_USING_PUBKEY   Pkcs7VerifyUsingPubKey;

  EFI_CRYPTO_SERVICE_AUTHENTICODE_VERIFY         AuthenticodeVerify;

  EFI_CRYPTO_SERVICE_DH_NEW                      DhNew;
  EFI_CRYPTO_SERVICE_DH_FREE                     DhFree;
  EFI_CRYPTO_SERVICE_DH_GENERATE_PARAMETER       DhGenerateParameter;
  EFI_CRYPTO_SERVICE_DH_SET_PARAMETER            DhSetParameter;
  EFI_CRYPTO_SERVICE_DH_GENERATE_KEY             DhGenerateKey;
  EFI_CRYPTO_SERVICE_DH_COMPUTE_KEY              DhComputeKey;

  EFI_CRYPTO_SERVICE_RAMDOM_SEED                 RandomSeed;
  EFI_CRYPTO_SERVICE_RANDOM_BYTES                RandomBytes;

  //
  // For compatibility, add new supported funtion at the end of
  // EFI_CRYPTO_SERVICE_PROTOCOL.
  //
  EFI_CRYPTO_SERVICE_PKCS7_GET_SIGNERS           Pkcs7GetSigners;
  EFI_CRYPTO_SERVICE_PKCS7_FREE_SIGNERS          Pkcs7FreeSigners;
  EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT           RsaPkcs1Decrypt;

} EFI_CRYPTO_SERVICE_PROTOCOL;

extern EFI_GUID gEfiCryptoServiceProtocolGuid;

#endif
