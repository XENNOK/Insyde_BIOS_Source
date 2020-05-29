/** @file
  Helper functions for Unsigned FV region feature

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

#ifndef _UNSIGNED_FV_REGION_H_
#define _UNSIGNED_FV_REGION_H_



EFI_STATUS
BackupSkipRegion (
  IN UINT8                      *ImageAddress,
  IN UINTN                      ImageSize,
  OUT UINTN                     *SkipRegionOffset,
  OUT UINTN                     *SkipRegionSize,
  OUT UINT8                     **SkipRegionDataBuffer
  );

EFI_STATUS
RestoreSkipRegion (
  IN UINT8                      *ImageAddress,
  IN UINTN                      ImageSize,
  IN UINTN                      SkipRegionOffset,
  IN UINTN                      SkipRegionSize,
  IN UINT8                      *SkipRegionDataBuffer
  );

#endif

