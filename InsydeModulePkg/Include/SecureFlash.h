/** @file

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

#ifndef _SECURE_FLASH_H_
#define _SECURE_FLASH_H_

#pragma pack(1)
typedef struct {
  UINT32                 ImageSize;
  EFI_PHYSICAL_ADDRESS   ImageAddress;
  BOOLEAN                FlashMode;
  BOOLEAN                Compressed;
} IMAGE_INFO;
#pragma pack()

#define SECURE_FLASH_SETUP_MODE_NAME   L"SecureFlashSetupMode"
#define SECURE_FLASH_INFORMATION_NAME  L"SecureFlashInfo"

extern EFI_GUID gSecureFlashInfoGuid;

#endif

