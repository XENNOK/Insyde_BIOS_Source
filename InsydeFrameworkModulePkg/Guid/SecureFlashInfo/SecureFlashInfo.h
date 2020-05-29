//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   GUID for Secure Flash.
//;

#ifndef _SECURE_FLASH_H_
#define _SECURE_FLASH_H_


#pragma pack(1)

typedef struct {
  UINT32                        ImageSize;
  EFI_PHYSICAL_ADDRESS          ImageAddress;
  BOOLEAN                       FlashMode;
  BOOLEAN                       Compressed;
} IMAGE_INFO;

#pragma pack()


#define SECURE_FLASH_INFO_GUID  \
  { \
    0x382AF2BB, 0xFFFF, 0xABCD, 0xAA, 0xEE, 0xCC, 0xE0, 0x99, 0x33, 0x88, 0x77 \
  }
extern EFI_GUID gSecureFlashInfoGuid;

#endif
