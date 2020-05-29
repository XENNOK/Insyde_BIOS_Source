/** @file
  Crypto Service Protocol

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

/*++
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

#ifndef __CRYPTO_SERVICES_PROTOCOL_H__
#define __CRYPTO_SERVICES_PROTOCOL_H__

#include <Library/BaseCryptLib.h>

//
// Crypto Service Protocol GUID.
//
#define CRYPTO_SERVICES_PROTOCOL_GUID \
  { \
    0xe1475e0c, 0x1746, 0x4802, { 0x86, 0x2e, 0x1, 0x1c, 0x2c, 0x2d, 0x9d, 0x86 } \
  }

typedef struct _CRYPTO_SERVICES_PROTOCOL CRYPTO_SERVICES_PROTOCOL;

typedef
UINTN
(EFIAPI *CRYPTO_SERVICES_GET_CONTEXT_SIZE) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DUPLICATE) (
  IN   CONST VOID                           *Context,
  OUT  VOID                                 *NewContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_UPDATE) (
  IN OUT  VOID                              *Context,
  IN      CONST VOID                        *Data,
  IN      UINTN                             DataSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_FINAL) (
  IN OUT  VOID                              *Context,
  OUT     UINT8                             *Value
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_SHA1_INIT) (
  OUT  VOID  *Sha1Context
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_INIT) (
  OUT  VOID                                 *Context,
  IN   CONST UINT8                          *Key,
  IN   UINTN                                KeyLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_ECB_ENCRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_ECB_DECRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_CBC_ENCRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  IN   CONST UINT8                          *Ivec,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_CBC_DECRYPT) (
  IN   VOID                                  *Context,
  IN   CONST UINT8                           *Input,
  IN   UINTN                                 InputSize,
  IN   CONST UINT8                           *Ivec,
  OUT  UINT8                                 *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_GET_KEY) (
  IN OUT VOID                                  *RsaContext,
  IN     RSA_KEY_TAG                           KeyTag,
  OUT    UINT8                                 *BigNumber,
  IN OUT UINTN                                 *BnLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_PKCS1_SIGN) (
  IN      VOID                              *RsaContext,
  IN      CONST UINT8                       *MessageHash,
  IN      UINTN                             HashSize,
  OUT     UINT8                             *Signature,
  IN OUT  UINTN                             *SigSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_GET_PRIVATE_KEY_FROM_PEM) (
  IN   CONST UINT8                                       *PemData,
  IN   UINTN                                             PemSize,
  IN   CONST CHAR8                                       *Password,
  OUT  VOID                                              **RsaContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_GET_PUBLIC_KEY_FROM_X509) (
  IN   CONST UINT8                                       *Cert,
  IN   UINTN                                             CertSize,
  OUT  VOID                                              **RsaContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_GET_SUBJECT_NAME) (
  IN      CONST UINT8                                    *Cert,
  IN      UINTN                                          CertSize,
  OUT     UINT8                                          *CertSubject,
  IN OUT  UINTN                                          *SubjectSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_VERIFY_CERT) (
  IN  CONST UINT8                                    *Cert,
  IN  UINTN                                          CertSize,
  IN  CONST UINT8                                    *CACert,
  IN  UINTN                                          CACertSize
  );

typedef
VOID *
(EFIAPI *CRYPTO_SERVICES_DH_NEW) (
  VOID
  );

typedef
VOID
(EFIAPI *CRYPTO_SERVICES_DH_FREE) (
  IN  VOID                                               *DhContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_GENERATE_PARAMETER) (
  IN OUT  VOID                                           *DhContext,
  IN      UINTN                                          Generator,
  IN      UINTN                                          PrimeLength,
  OUT     UINT8                                          *Prime
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_SET_PARAMETER) (
  IN OUT  VOID                                           *DhContext,
  IN      UINTN                                          Generator,
  IN      UINTN                                          PrimeLength,
  IN      CONST UINT8                                    *Prime
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_GENERATE_KEY) (
  IN OUT  VOID                                           *DhContext,
  OUT     UINT8                                          *PublicKey,
  IN OUT  UINTN                                          *PublicKeySize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_COMPUTE_KEY) (
  IN OUT  VOID                                           *DhContext,
  IN      CONST UINT8                                    *PeerPublicKey,
  IN      UINTN                                          PeerPublicKeySize,
  OUT     UINT8                                          *Key,
  IN OUT  UINTN                                          *KeySize
  );

typedef
VOID *
(EFIAPI *CRYPTO_SERVICES_RSA_NEW) (
  VOID
  );

typedef
VOID
(EFIAPI *CRYPTO_SERVICES_RSA_FREE) (
  IN  VOID                                  *RsaContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_SET_KEY) (
  IN OUT VOID                                  *RsaContext,
  IN     RSA_KEY_TAG                           KeyTag,
  IN     CONST UINT8                           *BigNumber,
  IN     UINTN                                 BnLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_PKCS1_VERIFY) (
  IN  VOID                                  *RsaContext,
  IN  CONST UINT8                           *MessageHash,
  IN  UINTN                                 HashLength,
  IN  UINT8                                 *Signature,
  IN  UINTN                                 SigLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_PKCS7_VERIFY) (
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
(EFIAPI *CRYPTO_SERVICES_PKCS7_VERIFY_USING_PUBKEY) (
  IN	CONST UINT8                                        *P7Data,
  IN  UINTN 	                                           P7Length,
  IN	CONST UINT8                                        *TrustedPubKey,
  IN	UINTN		                                           TrustedPubKeyLen,
  IN  CONST UINT8                                        *InData,
  IN  UINTN 	                                           DataLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_AUTHENTICODE_VERIFY) (
  IN  CONST UINT8                                        *AuthData,
  IN  UINTN                                              DataLength,
  IN  CONST UINT8                                        *TrustedCert,
  IN  UINTN                                              CertLength,
  IN  CONST UINT8                                        *ImageHash,
  IN  UINTN                                              HashLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RAMDOM_SEED) (
  IN  CONST  UINT8                                       *Seed  OPTIONAL,
  IN  UINTN                                              SeedSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RANDOM_BYTES) (
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

/**
  Decrypt the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
  RSA PKCS#1.

  @param[in]     RsaContext     Pointer to RSA context for signature verification.
  @param[in]     Signature      Pointer to RSA PKCS1-v1_5 signature to be verified.
  @param[in]     SignatureSize  Size of signature in bytes.
  @param[out]    Message        Pointer to buffer to receive RSA PKCS1-v1_5 decrypted message.
  @param[in,out] MessageSize    Size of the decrypted message in bytes.

  @retval  EFI_SUCCESS            Decrypt input signature successfully.
  @retval  EFI_BUFFER_TOO_SMALL   The Message buffer was too small. The current buffer size needed to hold the
                                  message is returned in MessageSize.
  @retval  EFI_INVALID_PARAMETER  1. RsaContext is NULL, Signature is NULL or SignatureSize is 0
                                  2. MessageSize is NULL or the Content of MessageSize isn't 0 and Message is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT) (
  IN     VOID        *RsaContext,
  IN     CONST UINT8 *Signature,
  IN     UINTN       SignatureSize,
  OUT    UINT8       *Message,
  IN OUT UINTN       *MessageSize
  );

//
// Crypto Service Protocol Structure.
//
struct _CRYPTO_SERVICES_PROTOCOL {
  CRYPTO_SERVICES_GET_CONTEXT_SIZE              Sha1GetContextSize;
  CRYPTO_SERVICES_SHA1_INIT                     Sha1Init;
  CRYPTO_SERVICES_DUPLICATE                     Sha1Duplicate;
  CRYPTO_SERVICES_UPDATE                        Sha1Update;
  CRYPTO_SERVICES_FINAL                         Sha1Final;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              HmacSha1GetContextSize;
  CRYPTO_SERVICES_INIT                          HmacSha1Init;
  CRYPTO_SERVICES_DUPLICATE                     HmacSha1Duplicate;
  CRYPTO_SERVICES_UPDATE                        HmacSha1Update;
  CRYPTO_SERVICES_FINAL                         HmacSha1Final;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              TdesGetContextSize;
  CRYPTO_SERVICES_INIT                          TdesInit;
  CRYPTO_SERVICES_ECB_ENCRYPT                   TdesEcbEncrypt;
  CRYPTO_SERVICES_ECB_DECRYPT                   TdesEcbDecrypt;
  CRYPTO_SERVICES_CBC_ENCRYPT                   TdesCbcEncrypt;
  CRYPTO_SERVICES_CBC_DECRYPT                   TdesCbcDecrypt;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              AesGetContextSize;
  CRYPTO_SERVICES_INIT                          AesInit;
  CRYPTO_SERVICES_ECB_ENCRYPT                   AesEcbEncrypt;
  CRYPTO_SERVICES_ECB_DECRYPT                   AesEcbDecrypt;
  CRYPTO_SERVICES_CBC_ENCRYPT                   AesCbcEncrypt;
  CRYPTO_SERVICES_CBC_DECRYPT                   AesCbcDecrypt;

  CRYPTO_SERVICES_RSA_NEW                       RsaNew;
  CRYPTO_SERVICES_RSA_FREE                      RsaFree;
  CRYPTO_SERVICES_RSA_GET_KEY                   RsaGetKey;
  CRYPTO_SERVICES_RSA_SET_KEY                   RsaSetKey;
  CRYPTO_SERVICES_RSA_PKCS1_SIGN                RsaPkcs1Sign;
  CRYPTO_SERVICES_RSA_PKCS1_VERIFY              RsaPkcs1Verify;
  CRYPTO_SERVICES_RSA_GET_PRIVATE_KEY_FROM_PEM  RsaGetPrivateKeyFromPem;
  CRYPTO_SERVICES_RSA_GET_PUBLIC_KEY_FROM_X509  RsaGetPublicKeyFromX509;
  CRYPTO_SERVICES_X509_GET_SUBJECT_NAME         X509GetSubjectName;
  CRYPTO_SERVICES_X509_VERIFY_CERT              X509VerifyCert;

  CRYPTO_SERVICES_PKCS7_VERIFY                  Pkcs7Verify;
  CRYPTO_SERVICES_PKCS7_VERIFY_USING_PUBKEY     Pkcs7VerifyUsingPubKey;

  CRYPTO_SERVICES_AUTHENTICODE_VERIFY           AuthenticodeVerify;

  CRYPTO_SERVICES_DH_NEW                        DhNew;
  CRYPTO_SERVICES_DH_FREE                       DhFree;
  CRYPTO_SERVICES_DH_GENERATE_PARAMETER         DhGenerateParameter;
  CRYPTO_SERVICES_DH_SET_PARAMETER              DhSetParameter;
  CRYPTO_SERVICES_DH_GENERATE_KEY               DhGenerateKey;
  CRYPTO_SERVICES_DH_COMPUTE_KEY                DhComputeKey;

  CRYPTO_SERVICES_RAMDOM_SEED                   RandomSeed;
  CRYPTO_SERVICES_RANDOM_BYTES                  RandomBytes;

  //
  // For compatibility, add new supported funtion at the end of
  // EFI_CRYPTO_SERVICE_PROTOCOL.
  //
  EFI_CRYPTO_SERVICE_PKCS7_GET_SIGNERS           Pkcs7GetSigners;
  EFI_CRYPTO_SERVICE_PKCS7_FREE_SIGNERS          Pkcs7FreeSigners;
  EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT           RsaPkcs1Decrypt;

};

extern EFI_GUID gCryptoServicesProtocolGuid;

#endif
