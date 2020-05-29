/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_ME_REGION_H
#define _SMM_ME_REGION_H

#define SMM_ME_REGION_PROTOCOL_GUID \
  { \
    0xF5236E81, 0x6F85, 0x4827, 0x96, 0x73, 0xD7, 0xFF, 0x3E, 0x28, 0x44, 0x3E \
  }



typedef struct _SMM_ME_REGION_PROTOCOL {
  UINT64                          Nonce;
  UINT32                          FactoryDefaultBase;
  UINT32                          FactoryDefaultLimit;
  UINT8                           Result;
  BOOLEAN                         MEUnlock;
} SMM_ME_REGION_PROTOCOL;

extern EFI_GUID gSmmMERegionProtocolGuid;

#endif // _SMM_ME_REGION_H
