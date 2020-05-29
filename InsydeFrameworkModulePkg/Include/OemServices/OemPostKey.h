//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_POST_KEY_H_
#define _OEM_POST_KEY_H_

#include "Tiano.h"

typedef enum {
    NO_OPERATION,
    FRONT_PAGE_HOT_KEY,
    MAX_HOT_KEY_SUPPORT
} POST_SCREEN_OPERATION;

typedef struct {
  UINT32              KeyBit;
  UINT16              ScanCode;
  UINT8               PostOperation;
} SCAN_TO_OPERATION;

#endif
