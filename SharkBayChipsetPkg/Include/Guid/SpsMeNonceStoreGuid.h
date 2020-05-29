/** @file

  GUIDs used for storing SPS ME Nonce in Variable.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_ME_NONCE_STORE_GUID_H_
#define _SPS_ME_NONCE_STORE_GUID_H_

#define SPS_ME_NONCE_STORE_REVISION_1 1

#define SPS_ME_NONCE_STORE_GUID \
  { 0xD12B5C8E, 0x5C94, 0x4C0B, { 0xAC, 0xF0, 0xCF, 0x6A, 0xC7, 0xBA, 0x73, 0xBB }}

#define SPS_ME_NONCE_INFO_VARIABLE_NAME   L"SpsMeNonceStore"

extern EFI_GUID gSpsMeNonceStoreGuid;

#pragma pack(1)
#pragma warning ( disable : 4366 )

typedef struct _SPS_ME_NONCE_INFO {
  UINT8                           Revision;
  UINT64                          Nonce;
  UINT32                          FactoryDefaultBase;
  UINT32                          FactoryDefaultLimit;
  UINT8                           Result;
} SPS_ME_NONCE_INFO;

#pragma pack()

#endif // _SPS_ME_NONCE_STORE_GUID_H_
