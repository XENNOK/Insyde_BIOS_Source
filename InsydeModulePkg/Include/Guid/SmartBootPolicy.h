/** @file

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

#ifndef _SMART_BOOT_POLICY_H_
#define _SMART_BOOT_POLICY_H_

#define SMART_BOOT_POLICY_GUID  \
  { \
    0x9da50e9e, 0x5f10, 0x4cae, 0xa9, 0xc9, 0x7f, 0xe9, 0x15, 0x19, 0xbc, 0x8b \
  }
extern EFI_GUID gSmartBootPolicyGuid;

#define FAST_BOOT_DISABLE_STATE      0x00
#define FAST_BOOT_ENABLE_STATE       0x01

typedef struct {
  UINT64       WatchdogTimer;
  UINT8        QuickBootEnable :   1;
  UINT8        HotKeyPressed :     1;
  UINT8        PreFastBootEnable:  1;
  UINT8        Reserved :          5;
} SMART_BOOT_POLICY_DATA;

#endif
