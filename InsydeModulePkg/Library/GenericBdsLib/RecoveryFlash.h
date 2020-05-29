/** @file
  This file include all BDS platform recovery flash functions.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _RECOVERY_FLASH_H
#define _RECOVERY_FLASH_H

#include "InternalBdsLib.h"
#include "String.h"
#include <Library/SeamlessRecoveryLib.h>
#include <Guid/ImageAuthentication.h>

#define FLASH_SECTOR_SIZE         4096
#define FLASH_BLOCK_SIZE          0x10000
#define SMI_FLASH_UNIT_BYTES      256
#define FLASH_SMI_PORT            PcdGet16 (PcdSoftwareSmiPort)

//
// Define the action for flash completation
//
#define IHISI_DO_NOTHING     0x00
#define IHISI_CLEAR_CMOS     0x100
#define IHISI_SHOUTDOWN      0x01
#define IHISI_REBOOT         0x02

EFI_STATUS
ResetCommand (
  VOID
  );

VOID
DrawDialogBlock (
  VOID
  );

VOID
Drawpercentage (
  IN   UINTN        PercentageValue
  );

#endif
