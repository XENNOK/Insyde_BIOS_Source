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

#ifndef _POST_KEY_H_
#define _POST_KEY_H_

#include <Uefi.h>

//
// remove unnecessary hot key, SETUP_HOT_KEY, BOOT_MANAGER_HOT_KEY and DEVICE_MANAGER_HOT_KEY are no longer used at MS37.
//
typedef enum {
    NO_OPERATION,
    FRONT_PAGE_HOT_KEY,
//[-start-140325-IB13080003-add]//
    SETUP_HOT_KEY,
    DEVICE_MANAGER_HOT_KEY,
    BOOT_MANAGER_HOT_KEY,
    BOOT_MAINTAIN_HOT_KEY,
    SECURE_BOOT_HOT_KEY,
//[-end-140325-IB13080003-add]//
//[-start-140321-IB10300105-add]//
    FLASH_BOX_HOT_KEY,
//[-end-140321-IB10300105-add]//
    MEBX_HOT_KEY,
    REMOTE_ASST_HOT_KEY,
    MAX_HOT_KEY_SUPPORT
} POST_SCREEN_OPERATION;

typedef struct {
  UINT32              KeyBit;
  UINT16              ScanCode;
  UINT8               PostOperation;
} SCAN_TO_OPERATION;


#define ESC_KEY   0x01
#define DEL_KEY   0x53
#define F1_KEY    0x3b
#define F2_KEY    0x3c
#define F3_KEY    0x3d
#define F4_KEY    0x3e
#define F5_KEY    0x3f
#define F6_KEY    0x40
#define F7_KEY    0x41
#define F8_KEY    0x42
#define F9_KEY    0x43
#define F10_KEY   0x44
#define F11_KEY   0x85
#define F12_KEY   0x86
#define UP_ARROW_KEY   0x18

#endif
