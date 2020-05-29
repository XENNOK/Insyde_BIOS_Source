/** @file
  PlatformBdsLib

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

#ifndef _BOOT_DEVICES_DISPLAY_H_
#define _BOOT_DEVICES_DISPLAY_H_

#include "BdsPlatform.h"
#include "String.h"

#define MAX_DISPLAY_DEVICE_COUNT      0x08

#define BOOT_FAIL_STRING_LENTH        79

typedef struct {
  UINT32        Signiture;
  UINT16        StrSegment;
  UINT16        StrOffset;
  UINT32        Attributes;
  UINT8         String;
} BOOT_MESSAGE;

EFI_STATUS
ShowAllDevice (
  VOID
);

#endif
