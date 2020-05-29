/** @file
  Defines data structure that is the volume header found. These data is
  intented to decouple FVB driver with FV header.

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

#include "FvbServicesRuntimeDxe.h"

typedef struct {
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  EFI_FIRMWARE_VOLUME_HEADER  FvbInfo;
  //
  // EFI_FV_BLOCK_MAP_ENTRY    ExtraBlockMap[n];//n=0
  //
  EFI_FV_BLOCK_MAP_ENTRY      End[1];
} EFI_FVB_MEDIA_INFO;

EFI_FVB_MEDIA_INFO  mPlatformFvbMediaInfo[] = {
  //
  // System NvStorage FVB
  //
  {
    FixedPcdGet32 (PcdFlashNvStorageVariableBase),
    {
      {
        0,
      },  /// ZeroVector[16]
      EFI_SYSTEM_NV_DATA_FV_GUID,
      FixedPcdGet32 (PcdFlashNvStorageVariableSize) +
      FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) +
      FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize) +
      FixedPcdGet32 (PcdFlashNvStorageFactoryCopySize),
      EFI_FVH_SIGNATURE,
      EFI_FVB2_MEMORY_MAPPED |
        EFI_FVB2_READ_ENABLED_CAP |
        EFI_FVB2_READ_STATUS |
        EFI_FVB2_WRITE_ENABLED_CAP |
        EFI_FVB2_WRITE_STATUS |
        EFI_FVB2_ERASE_POLARITY |
        EFI_FVB2_ALIGNMENT_16,
      sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY),
      0,  /// CheckSum
      0,  /// ExtHeaderOffset
      {
        0,
      },  /// Reserved[1]
      2,  /// Revision
      {
       (FixedPcdGet32 (PcdFlashNvStorageVariableSize) +
        FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) +
        FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize) +
        FixedPcdGet32 (PcdFlashNvStorageFactoryCopySize)) / FixedPcdGet32 (PcdFirmwareBlockSize),
        FixedPcdGet32 (PcdFirmwareBlockSize),
      }
    },
    {
      0,
      0
    }
  }
};

/**
  Get backup firmware volume header contents according to physical base address.

  @param[in]  BaseAddress  Firmware volume start address.
  @param[out] FvbInfo      Double pointer to output firmware volume header.

  @retval EFI_SUCCESS      Get firmware volume contents successful.
  @retval EFI_NOT_FOUND    Cannot find corresponding firmware volume header.
**/
EFI_STATUS
GetFvbInfo (
  IN  EFI_PHYSICAL_ADDRESS          BaseAddress,
  OUT EFI_FIRMWARE_VOLUME_HEADER    **FvbInfo
  )
{
  UINTN Index;

  for (Index = 0; Index < sizeof (mPlatformFvbMediaInfo) / sizeof (EFI_FVB_MEDIA_INFO); Index += 1) {
    if (mPlatformFvbMediaInfo[Index].BaseAddress == BaseAddress) {
      *FvbInfo = &mPlatformFvbMediaInfo[Index].FvbInfo;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Calculate correct checksum for all of backup firmware volume headers and put the
  checksum result to all of backup firmware volume headers.
**/
VOID
RecalculateFvHeaderChecksum (
  VOID
  )
{
  UINTN                       Index;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;

  for (Index = 0; Index < sizeof (mPlatformFvbMediaInfo) / sizeof (EFI_FVB_MEDIA_INFO); Index++) {
    FvHeader            = &mPlatformFvbMediaInfo[Index].FvbInfo;
    FvHeader->Checksum  = 0;
    FvHeader->Checksum  = CalculateCheckSum16 ((UINT16 *) FvHeader, FvHeader->HeaderLength);
  }
}
