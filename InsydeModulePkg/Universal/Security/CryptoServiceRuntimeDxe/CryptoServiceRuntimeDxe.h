/** @file
  CryptoServices protocol and Hash protocol driver instance

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef CRYPTO_SERVICE_RUNTIME_DXE_H
#define CRYPTO_SERVICE_RUNTIME_DXE_H

#define CRYPTO_SERVICE_BOOT_SERVICES_PROTOCOL    FALSE
#define CRYPTO_SERVICE_RUNTIME_PROTOCOL          TRUE
#define CRYPTO_SERVICE_CHANGE_TO_RUNTIME         FALSE

#define CRYPTO_SERVICE_BOOT_SERVICES_CRYPTO_PROTOCOL_GUID \
  { 0x2edad2d, 0x3b30, 0x4482, { 0xb2, 0x2f, 0x30, 0x34, 0x7b, 0x2b, 0xf, 0x65 } }

#define CRYPTO_SERVICE_BOOT_SERVICES_HASH_PROTOCOL_GUID \
{ 0x728d5c4a, 0x9601, 0x49b9, { 0xb8, 0x72, 0x72, 0x14, 0x35, 0x84, 0x7e, 0xeb } }


#endif

