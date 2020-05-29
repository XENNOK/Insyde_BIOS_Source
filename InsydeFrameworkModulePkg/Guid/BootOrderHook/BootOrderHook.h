//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   GUID used to notify variable driver to hook BootOrder variable or not.
//;

#ifndef _BOOT_ORDER_HOOK_H_
#define _BOOT_ORDER_HOOK_H_

#define BOOT_ORDER_HOOK_ENABLE_GUID \
  { \
    0x6f2f4e41, 0x710, 0x4ab1, 0x8c, 0xdf, 0x2c, 0xd1, 0x5c, 0x23, 0x46, 0xea \
  };

#define BOOT_ORDER_HOOK_DISABLE_GUID \
  { \
    0xcc5b20b9, 0xda0a, 0x4164, 0x83, 0xfe, 0x4, 0xbf, 0x8, 0x2, 0xf2, 0x50 \
  };

extern EFI_GUID gBootOrderHookEnableGuid;
extern EFI_GUID gBootOrderHookDisableGuid;

#endif
