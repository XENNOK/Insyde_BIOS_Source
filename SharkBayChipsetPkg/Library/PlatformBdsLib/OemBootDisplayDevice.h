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

#ifndef _OEM_BOOT_DISPLAY_DEVICE_H_
#define _OEM_BOOT_DISPLAY_DEVICE_H_

#include "BdsPlatform.h"

EFI_STATUS
OemBootDisplayDeviceCallBack (
  IN  CHIPSET_CONFIGURATION      *SetupNVRam,
  IN  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios
  );

#endif
