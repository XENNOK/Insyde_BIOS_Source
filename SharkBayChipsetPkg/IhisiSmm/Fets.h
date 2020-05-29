/** @file
  This driver provides IHISI interface in SMM mode

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

#ifndef _IHISI_FETS_H_
#define _IHISI_FETS_H_

#include "IhisiSmm.h"
#include "FetsOemHook.h"

#define FETS_FUNCTIONS           \
          { 0x20, FetsWrite   }, \
          { 0x21, GetPartInfo }

#define EC64K          0x00
#define EC128K         0x01
#define EC256K         0x02
#define EC512K         0x03
#define EC1024K        0x04
#define EC2048K        0x05

typedef enum {
  EcFlashDoNothing       = 0x00,    // Do nothing
  EcFlashDosReboot       = 0x01,    // DOS Shutdown
  EcFlashOSShutdown      = 0x02,    // OS Shutdown
  EcFlashOSReboot        = 0x04,    // OS Reboot
  EcFlashDoshutdown      = 0x05,    // DOS Reboot
  EcFlashContinueToFlash = 0xEF     // Continue to flash
} IHISI_EC_FLASH_ACTION_AFTER_FLASHING;

/**
  AH=20h, Flash EC through SMI (FETS) Write.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
FetsWrite (
  VOID
  );

/**
  AH=21h, Get EC part information.

  @retval EFI_SUCCESS     Get EC part information successful.
**/
EFI_STATUS
GetPartInfo (
  VOID
  );



#endif
