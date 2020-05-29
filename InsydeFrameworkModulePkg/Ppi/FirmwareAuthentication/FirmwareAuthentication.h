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
//; Module Name:
//;
//;   FirmwareAuthentication.h
//
//; Abstract:
//;
//;   Firmware Authentication PPI
//;

#ifndef _FIRMWARE_AUTHENTICATION_PPI_H_
#define _FIRMWARE_AUTHENTICATION_PPI_H_

#define FIRMWARE_AUTHENTICATION_PPI_GUID \
  { \
    0xdd12e306, 0xd1d3, 0x48ed, 0xaf, 0xf5, 0x95, 0xc9, 0x7d, 0xf3, 0x6f, 0x0e \
  }

#define SIGNATURE_SIZE              0x100
#define ISFLASH_TAG_SIZE            16
#define ISFLASH_BIOS_IMAGE_TAG      "$_IFLASH_BIOSIMG"
#define ISFLASH_IMG_SIGNATURE_TAG   "$_IFLASH_BIOSCER"
#define ALIGHMENT_SIZE              16

#pragma pack(1)
typedef struct {
  UINT8         Tag[ISFLASH_TAG_SIZE];
  UINT32        AllocatedSize;
  UINT32        DataSize;
} ISFLASH_DATA_REGION_HEADER;
#pragma pack()


typedef struct _FIRMWARE_AUTHENTICATION_PPI FIRMWARE_AUTHENTICATION_PPI;

typedef
EFI_STATUS
(EFIAPI *AUTHENTICATE_FIRMWARE)(
  IN UINT8             *FirmwareBin,
  IN UINTN             FirmwareSize
  );

struct _FIRMWARE_AUTHENTICATION_PPI {
  AUTHENTICATE_FIRMWARE AuthenticateFirmware;
};

extern EFI_GUID gFirmwareAuthenticationPpiGuid;
#endif
