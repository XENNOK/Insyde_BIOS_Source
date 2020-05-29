//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   RecoveryFlash.h
//;
//; Abstract:
//;
//;   This file include all BDS platform recovery flash functions.
//;

#ifndef _RECOVERY_FLASH_H
#define _RECOVERY_FLASH_H

#include "Tiano.h"
#include "EfiPrintLib.h"
#include "FlashMap.h"
#include "EfiFlashMap.h"
#include "EfiHobLib.h"
#include "SmiTable.h"
#include "OemGraphicsLib.h"
#include "String.h"
#include "GenericBdsLib.h"
#include "PostCode.h"

#include EFI_PROTOCOL_CONSUMER (CpuIo)
#include EFI_PROTOCOL_CONSUMER (Hii)
#include EFI_PROTOCOL_CONSUMER (OemFormBrowser)

#include EFI_GUID_DEFINITION (FlashMapHob)
#include EFI_GUID_DEFINITION (RecoveryFileAddress)
#include EFI_GUID_DEFINITION (Hob)

#define	FLASH_SECTOR_SIZE	        4096
#define	FLASH_BLOCK_SIZE		      0x10000
#define	SMI_FLASH_UNIT_BYTES		  256
#define DEFAULT_FLASH_DEVICE_TYPE 0
#define SPI_FLASH_DEVICE_TYPE     1
#define OTHER_FLASH_DEVICE_TYPE   2
#define	FLASH_SMI_PORT		        SW_SMI_PORT

//
// Define the action for flash completation
//
#define IHISI_DO_NOTHING     0x00
#define IHISI_CLEAR_CMOS     0x100
#define IHISI_SHOUTDOWN      0x01
#define IHISI_REBOOT         0x02

EFI_STATUS
RecoveryPopUp (
  IN  UINTN     FlashMode
  );

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

VOID
RecoveryFlash (
  IN  UINTN     FlashMode
  );

#endif
