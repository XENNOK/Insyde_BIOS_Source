/** @file
  FV Region Info Protocol for Firmware Volume Block services

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

#ifndef _FV_REGION_INFO_H_
#define _FV_REGION_INFO_H_

#define FV_REGION_INFO_PROTOCOL_GUID  \
{ \
  0x5a25cbb6, 0x4db1, 0x4e08, 0xba, 0x7b, 0x57, 0x02, 0x50, 0xeb, 0xfc, 0x9e \
}

typedef struct {
  EFI_GUID                    ImageTypeGuid;
  UINTN                       ImageOffset;
  UINTN                       ImageSize;
} FV_REGION_INFO;

typedef struct {
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  FV_REGION_INFO              *FvRegionInfo;
  EFI_FIRMWARE_VOLUME_HEADER  FvHeader;
  EFI_FV_BLOCK_MAP_ENTRY      End[1];
} FVB_MEDIA_INFO;

extern EFI_GUID gFvRegionInfoProtocolGuid;

#endif