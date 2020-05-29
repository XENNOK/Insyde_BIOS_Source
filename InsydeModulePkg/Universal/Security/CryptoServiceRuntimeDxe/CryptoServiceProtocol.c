/** @file
  CryptoService common function defination

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

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include "CryptoHash.h"
#include "CryptoServiceProtocol.h"

/*
  Setup CryptoService Protocol Functions

  @parma ImageHandle          Image Handle
  @parma ProtocolServices     Protocol Services Interface
  @parma ByProtocol           @TRUE, the CryptoService protocol will replace with HashProtocol and CryptoService.
                              @FALSE, it will be put driver's function.
  @parma HashProtocol         Replace Hash protocol
  @parma CryptoService        Replace CryptoService protocol
*/
VOID
SetupCryptoService (
  IN     EFI_HANDLE                    ImageHandle,
  IN OUT CRYPTO_SERVICE                *ProtocolServices,
  IN     BOOLEAN                       ByProtocol,
  IN     EFI_HASH_PROTOCOL             *HashProtocol,
  IN     CRYPTO_SERVICES_PROTOCOL      *CryptoProtocol
  )
{
   EFI_HASH_PROTOCOL                   *Hash;
   CRYPTO_SERVICES_PROTOCOL            *Crypto;

  if (ByProtocol) {
    if ((HashProtocol == NULL) || (CryptoProtocol == NULL)) {
      ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
      return;
    }
  }

  ZeroMem (ProtocolServices, sizeof (CRYPTO_SERVICE));
  Crypto                          = &(ProtocolServices->CryptoServiceProtocol);
  Hash                            = &(ProtocolServices->HashInstance.HashProtocol);

  ProtocolServices->Signature     = CRYPTO_SERVICE_SIGNATURE;
  ProtocolServices->Handle        = ImageHandle;

  ProtocolServices->HashInstance.Signature = CRYPTO_HASH_SIGNATURE;
  ProtocolServices->HashInstance.IsRoot    = TRUE;
  //
  // Hash Protocol Initial
  //
  Hash->GetHashSize               = (!ByProtocol) ? CryptGetHashSize        : HashProtocol->GetHashSize;
  Hash->Hash                      = (!ByProtocol) ? CryptHash               : HashProtocol->Hash;
  //
  // Crytpto Services Protocol Initial
  //
  Crypto->Sha1GetContextSize      = (!ByProtocol) ? Sha1GetContextSize      : CryptoProtocol->Sha1GetContextSize;
  Crypto->Sha1Init                = (!ByProtocol) ? Sha1Init                : CryptoProtocol->Sha1Init;
  Crypto->Sha1Duplicate           = (!ByProtocol) ? Sha1Duplicate           : CryptoProtocol->Sha1Duplicate;
  Crypto->Sha1Update              = (!ByProtocol) ? Sha1Update              : CryptoProtocol->Sha1Update;
  Crypto->Sha1Final               = (!ByProtocol) ? Sha1Final               : CryptoProtocol->Sha1Final;

  Crypto->HmacSha1GetContextSize  = (!ByProtocol) ? HmacSha1GetContextSize  : CryptoProtocol->HmacSha1GetContextSize;
  Crypto->HmacSha1Init            = (!ByProtocol) ? HmacSha1Init            : CryptoProtocol->HmacSha1Init;
  Crypto->HmacSha1Duplicate       = (!ByProtocol) ? HmacSha1Duplicate       : CryptoProtocol->HmacSha1Duplicate;
  Crypto->HmacSha1Update          = (!ByProtocol) ? HmacSha1Update          : CryptoProtocol->HmacSha1Update;
  Crypto->HmacSha1Final           = (!ByProtocol) ? HmacSha1Final           : CryptoProtocol->HmacSha1Final;

  Crypto->TdesGetContextSize      = (!ByProtocol) ? TdesGetContextSize      : CryptoProtocol->TdesGetContextSize;
  Crypto->TdesInit                = (!ByProtocol) ? TdesInit                : CryptoProtocol->TdesInit;
  Crypto->TdesEcbEncrypt          = (!ByProtocol) ? TdesEcbEncrypt          : CryptoProtocol->TdesEcbEncrypt;
  Crypto->TdesEcbDecrypt          = (!ByProtocol) ? TdesEcbDecrypt          : CryptoProtocol->TdesEcbDecrypt;
  Crypto->TdesCbcEncrypt          = (!ByProtocol) ? TdesCbcEncrypt          : CryptoProtocol->TdesCbcEncrypt;
  Crypto->TdesCbcDecrypt          = (!ByProtocol) ? TdesCbcDecrypt          : CryptoProtocol->TdesCbcDecrypt;

  Crypto->AesGetContextSize       = (!ByProtocol) ? AesGetContextSize       : CryptoProtocol->AesGetContextSize;
  Crypto->AesInit                 = (!ByProtocol) ? AesInit                 : CryptoProtocol->AesInit;
  Crypto->AesEcbEncrypt           = (!ByProtocol) ? AesEcbEncrypt           : CryptoProtocol->AesEcbEncrypt;
  Crypto->AesEcbDecrypt           = (!ByProtocol) ? AesEcbDecrypt           : CryptoProtocol->AesEcbDecrypt;
  Crypto->AesCbcEncrypt           = (!ByProtocol) ? AesCbcEncrypt           : CryptoProtocol->AesCbcEncrypt;
  Crypto->AesCbcDecrypt           = (!ByProtocol) ? AesCbcDecrypt           : CryptoProtocol->AesCbcDecrypt;

  Crypto->RsaNew                  = (!ByProtocol) ? RsaNew                  : CryptoProtocol->RsaNew;
  Crypto->RsaFree                 = (!ByProtocol) ? RsaFree                 : CryptoProtocol->RsaFree;
  Crypto->RsaGetKey               = (!ByProtocol) ? RsaGetKey               : CryptoProtocol->RsaGetKey;
  Crypto->RsaSetKey               = (!ByProtocol) ? RsaSetKey               : CryptoProtocol->RsaSetKey;

  Crypto->RsaPkcs1Sign            = (!ByProtocol) ? RsaPkcs1Sign            : CryptoProtocol->RsaPkcs1Sign;
  Crypto->RsaPkcs1Verify          = (!ByProtocol) ? RsaPkcs1Verify          : CryptoProtocol->RsaPkcs1Verify;

  Crypto->Pkcs7Verify             = (!ByProtocol) ? Pkcs7Verify             : CryptoProtocol->Pkcs7Verify;
  Crypto->Pkcs7VerifyUsingPubKey  = (!ByProtocol) ? Pkcs7VerifyUsingPubKey  : CryptoProtocol->Pkcs7VerifyUsingPubKey;

  Crypto->RsaGetPrivateKeyFromPem = (!ByProtocol) ? RsaGetPrivateKeyFromPem : CryptoProtocol->RsaGetPrivateKeyFromPem;
  Crypto->RsaGetPublicKeyFromX509 = (!ByProtocol) ? RsaGetPublicKeyFromX509 : CryptoProtocol->RsaGetPublicKeyFromX509;

  Crypto->X509GetSubjectName      = (!ByProtocol) ? X509GetSubjectName      : CryptoProtocol->X509GetSubjectName;
  Crypto->X509VerifyCert          = (!ByProtocol) ? X509VerifyCert          : CryptoProtocol->X509VerifyCert;

  Crypto->DhNew                   = (!ByProtocol) ? DhNew                   : CryptoProtocol->DhNew;
  Crypto->DhFree                  = (!ByProtocol) ? DhFree                  : CryptoProtocol->DhFree;
  Crypto->DhGenerateParameter     = (!ByProtocol) ? DhGenerateParameter     : CryptoProtocol->DhGenerateParameter;
  Crypto->DhSetParameter          = (!ByProtocol) ? DhSetParameter          : CryptoProtocol->DhSetParameter;
  Crypto->DhGenerateKey           = (!ByProtocol) ? DhGenerateKey           : CryptoProtocol->DhGenerateKey;
  Crypto->DhComputeKey            = (!ByProtocol) ? DhComputeKey            : CryptoProtocol->DhComputeKey;

  Crypto->RandomSeed              = (!ByProtocol) ? RandomSeed              : CryptoProtocol->RandomSeed;
  Crypto->RandomBytes             = (!ByProtocol) ? RandomBytes             : CryptoProtocol->RandomBytes;

  Crypto->AuthenticodeVerify      = (!ByProtocol) ? AuthenticodeVerify      : CryptoProtocol->AuthenticodeVerify;

  Crypto->Pkcs7GetSigners         = (!ByProtocol) ? Pkcs7GetSigners         : CryptoProtocol->Pkcs7GetSigners;
  Crypto->Pkcs7FreeSigners        = (!ByProtocol) ? Pkcs7FreeSigners        : CryptoProtocol->Pkcs7FreeSigners;
  Crypto->RsaPkcs1Decrypt         = (!ByProtocol) ? RsaPkcs1Decrypt         : CryptoProtocol->RsaPkcs1Decrypt;

}
