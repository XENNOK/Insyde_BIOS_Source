//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
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
//;   BvdtLib.h
//;
//; Abstract:
//;
//;   BVDT library include file
//;
//;

#ifndef _BVDT_LIB_H_
#define _BVDT_LIB_H_

#include "Tiano.h"
#include "FlashMap.h"
#include "EfiCommonLib.h"

#ifdef  FLASH_REGION_NV_COMMON_STORE_SUBREGION_NV_BVDT_BASE
#define BVDT_BASE_ADDRESS               FLASH_REGION_NV_COMMON_STORE_SUBREGION_NV_BVDT_BASE
#define BVDT_SIZE                       FLASH_REGION_NV_COMMON_STORE_SUBREGION_NV_BVDT_SIZE
#else
#ifdef FLASH_REGION_NVSTORAGE_SUBREGION_NV_BVDT_BASE
#define BVDT_BASE_ADDRESS               FLASH_REGION_NVSTORAGE_SUBREGION_NV_BVDT_BASE
#define BVDT_SIZE                       FLASH_REGION_NVSTORAGE_SUBREGION_NV_BVDT_SIZE
#endif
#endif

#define BIOS_BUILD_DATE_OFFSET          0x6
#define BIOS_BUILD_TIME_OFFSET          0xA
#define BIOS_VERSION_OFFSET             0xE
#define PRODUCT_NAME_OFFSET             0x27
#define CCB_VERSION_OFFSET              0x41
#define MULTI_BIOS_VERSION_OFFSET       0x15B
#define BVDT_MAX_STR_SIZE               32
#define ESRT_TAG                        {'$','E','S','R','T'}
#define RELEASE_DATE_TAG                {'$','R','D','A','T','E'}

typedef enum {
  BvdtBuildDate,
  BvdtBuildTime,
  BvdtBiosVer,
  BvdtProductName,
  BvdtCcbVer,
  BvdtMultiBiosVer,
  BvdtMultiProductName,
  BvdtMultiCcbVer,
  BvdtReleaseDate
} BVDT_TYPE;

EFI_STATUS
GetBvdtInfo (
  IN     BVDT_TYPE           Type,
  IN OUT UINTN               *StrBufferLen,
  OUT    CHAR16              *StrBuffer
  );

EFI_STATUS
EFIAPI
GetEsrtFirmwareInfo (
  OUT EFI_GUID          *FirmwareGuid,
  OUT UINT32            *FirmwareVersion
  );

#endif
