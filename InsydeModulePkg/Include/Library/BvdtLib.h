/** @file
  BVDT library include file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BVDT_LIB_H_
#define _BVDT_LIB_H_

#include <Uefi.h>

#define BVDT_BASE_ADDRESS               PcdGet32 (PcdFlashNvStorageBvdtBase)
#define BVDT_SIZE                       PcdGet32 (PcdFlashNvStorageBvdtSize)


#define BIOS_BUILD_DATE_OFFSET          0x6
#define BIOS_BUILD_TIME_OFFSET          0xA
#define BIOS_VERSION_OFFSET             0xE
#define PRODUCT_NAME_OFFSET             0x27
#define CCB_VERSION_OFFSET              0x41
#define MULTI_BIOS_VERSION_OFFSET       0x15B
#define BME_OFFSET                      0x12F
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

/**
  Get BIOS version, product name, CCB version, multiple BIOS version, multiple product name
  or multiple CCB verion from BVDT region.

  @param[in]      Type          Information type of BVDT.
  @param[in, out] StrBufferLen  Input : string buffer length
                                Output: length of BVDT information string.
  @param[out]     StrBuffer     BVDT information string.

  @retval EFI_SUCCESS           Successly get string.
  @retval EFI_BUFFER_TOO_SMALL  Buffer was too small. The current length of information string
                                needed to hold the string is returned in BufferSize.
  @retval EFI_INVALID_PARAMETER Input invalid type of BVDT information.
                                BufferSize or Buffer is NULL.
  @retval EFI_NOT_FOUND         Can not find information of multiple version, multiple product name
                                or multiple CCB verion or BVDT build time is invalid

**/
EFI_STATUS
EFIAPI
GetBvdtInfo (
  IN     BVDT_TYPE           Type,
  IN OUT UINTN               *StrBufferLen,
  OUT    CHAR16              *StrBuffer
  );

/**
  Get ESRT System Firmware GUID and Version information from BVDT $ESRT tag

  @param[out]  FirmwareGuid     Pointer to the system firmware version GUID
  @param[out]  FirmwareVersion  Pointer to the system firmware version

  @retval      EFI_SUCCESS      System firmware GUID and system firmware version
                                are successfully retrieved
               EFI_NOT_FOUND    Unable to find system firmware GUID or system firmware
                                version in the BVDT table
**/

EFI_STATUS
EFIAPI
GetEsrtFirmwareInfo (
  OUT EFI_GUID          *FirmwareGuid,
  OUT UINT32            *FirmwareVersion
  );

#endif
