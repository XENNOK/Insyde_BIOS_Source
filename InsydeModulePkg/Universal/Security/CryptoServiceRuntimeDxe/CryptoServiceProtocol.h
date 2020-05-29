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

#ifndef __CRYPTO_PROTOCOL_H__
#define __CRYPTO_PROTOCOL_H__

#include <Protocol/Hash.h>
#include <Protocol/CryptoServices.h>
#include <Library/BaseCryptLib.h>

#define CERT_BUFFER_SIZE               0x2000

#define CRYPTO_SERVICE_SIGNATURE       SIGNATURE_32 ('c', 'r', 'y', 'p')

typedef struct {
  UINTN                                Signature;
  EFI_HANDLE                           Handle;
  CRYPTO_HASH_INSTANCE                 HashInstance;
  CRYPTO_SERVICES_PROTOCOL             CryptoServiceProtocol;
} CRYPTO_SERVICE;

VOID
SetupCryptoService (
  IN     EFI_HANDLE                    ImageHandle,
  IN OUT CRYPTO_SERVICE                *ProtocolServices,
  IN     BOOLEAN                       ByProtocol,
  IN     EFI_HASH_PROTOCOL             *HashProtocol,
  IN     CRYPTO_SERVICES_PROTOCOL      *CryptoService
  );

#endif
