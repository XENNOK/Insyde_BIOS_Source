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

#ifndef   _OEM_HOT_KEY_H_
#define   _OEM_HOT_KEY_H_

#include "BdsPlatform.h"
#include "PostKey.h"
#include "FrontPage.h"

EFI_STATUS
OemHotKeyCallback (
  IN UINT16                                    Selection,
  IN UINT16                                    Timeout,
  IN EFI_BOOT_MODE                             BootMode,
  IN BOOLEAN                                   NoBootDevices
  );

#endif
