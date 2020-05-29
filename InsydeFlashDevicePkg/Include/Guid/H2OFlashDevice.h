/** @file
   GUID definitions for Flash Devices
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

#ifndef _H2O_FLASH_DEVICE_H_
#define _H2O_FLASH_DEVICE_H_

#include <Uefi.h>

#define H2O_FLASH_DEVICE_GUID \
  { 0xcbdef572, 0x719d, 0x426e, {0x8a, 0x43, 0x75, 0xae, 0x91, 0x3c, 0xff, 0xb2}}

#define H2O_FLASH_DEVICE_ENABLE_GUID \
  { 0x993189c6, 0xdacf, 0x4e29, {0xb9, 0x44, 0xb0, 0x02, 0x3b, 0x6a, 0x1d, 0xa6}}

#define H2O_FLASH_DEVICE_MFR_NAME_GUID \
  { 0x2f779836, 0x610c, 0x4615, {0xa4, 0x62, 0x54, 0xe7, 0x3e, 0xf1, 0x61, 0x92}}

#define H2O_FLASH_DEVICE_PART_NAME_GUID \
  { 0xe9a68f2a, 0x13f1, 0x4505, {0xb6, 0xa1, 0xe1, 0x82, 0x18, 0x8a, 0xc4, 0x9a}}

#define H2O_FLASH_DEVICE_CONFIG_GUID \
  { 0xec17a54e, 0xfec1, 0x431b, {0xad, 0x9c, 0xb4, 0xf0, 0xbc, 0x6e, 0x2e, 0x56}}

extern EFI_GUID gH2OFlashDeviceGuid;
extern EFI_GUID gH2OFlashDeviceMfrNameGuid;
extern EFI_GUID gH2OFlashDevicePartNameGuid;
extern EFI_GUID gH2OFlashDeviceConfigGuid;
extern EFI_GUID gH2OFlashDeviceEnableGuid;

#endif
