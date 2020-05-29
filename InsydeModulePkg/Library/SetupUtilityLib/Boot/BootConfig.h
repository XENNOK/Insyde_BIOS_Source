/** @file
  Header file for boot configuration structure definition

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BOOT_CONFIG_H_
#define _BOOT_CONFIG_H_
#include "KernelSetupConfig.h"

#define BOOT_VARSTORE_ID    0x1489

#pragma pack(1)
typedef struct {
  UINT8  NoBootDevs[MAX_BOOT_ORDER_NUMBER];
  UINT16 EfiBootDevOrder[MAX_BOOT_DEVICES_NUMBER];
  UINT16 LegacyAdvBootDevOrder[128]; // MAX_BOOT_ORDER_NUMBER * BBS_TYPE_TABLE_NUM
  UINT16 LegacyTypeDevOrder[128];    // MAX_BOOT_ORDER_NUMBER * BBS_TYPE_TABLE_NUM

} BOOT_CONFIGURATION;
#pragma pack()
#endif

