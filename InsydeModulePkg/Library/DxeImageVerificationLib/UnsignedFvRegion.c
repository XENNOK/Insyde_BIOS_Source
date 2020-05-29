/** @file
  Helper functions for Unsigned FV Region feature

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>

#include <Ppi/FirmwareAuthentication.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/FirmwareFileSystem2.h>
#include "UnsignedFvRegion.h"

/**
  Retrieve firmware(BIOS) infomation from the Image.
  Search BIOS address from FirmwareAddress.

  @param [in, out] FirmwareAddress      the image address
  @param [in]      FirmwareSize         the size of the image
  @param [out]     FirmwareOffset       the offset of the BIOS address from the image

  @retval EFI_SUCCESS          found the BIOS address
  @retval EFI_NOT_FOUND        did not find one

**/
EFI_STATUS
RetrieveFirmware (
  IN OUT UINT8                  **FirmwareAddress,
  IN UINTN                      FirmwareSize,
  OUT UINTN                     *FirmwareOffset
  )
{
  UINTN                         Index;
  UINT8                         *BiosImgAddress;
  ISFLASH_DATA_REGION_HEADER    *BiosDataRegion;


  //
  // Search the location of the BIOS image.
  //
  BiosImgAddress = *FirmwareAddress;
  for (Index = 0; Index < FirmwareSize; Index++) {
    if (CompareMem (BiosImgAddress + Index, ISFLASH_BIOS_IMAGE_TAG, ISFLASH_TAG_SIZE) == 0) {
      BiosDataRegion = (ISFLASH_DATA_REGION_HEADER *)(BiosImgAddress + Index);
      *FirmwareOffset = Index + sizeof (ISFLASH_DATA_REGION_HEADER);
      *FirmwareAddress = *FirmwareAddress + *FirmwareOffset;
      break;
    }
  }
  if (Index >= (FirmwareSize - ISFLASH_TAG_SIZE)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Backup the data of the unsigned FV region.
  This function allocates a buffer to save the data.

  @param [in]   ImageAddress           the image address
  @param [in]   ImageSize              the size of the image
  @param [out]  SkipRegionOffset       the offset of the unsigned FV region from the image
  @param [out]  SkipRegionSize         the size of the unsigned FV region
  @param [out]  SkipRegionDataBuffer   the pointer of the backup buffer

  @retval EFI_SUCCESS          backup succuessfully
  @retval EFI_NOT_FOUND        cannot find the unsigned FV region
  @retval EFI_BUFFER_TOO_SMALL cannot allocate enough memory to use

**/
EFI_STATUS
BackupSkipRegion (
  IN UINT8                      *ImageAddress,
  IN UINTN                      ImageSize,
  OUT UINTN                     *SkipRegionOffset,
  OUT UINTN                     *SkipRegionSize,
  OUT UINT8                     **SkipRegionDataBuffer
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         Index2;
  UINT8                         *BiosImgAddress;
  UINTN                         BiosImgOffset;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeaderPtr;
  EFI_FFS_FILE_HEADER           *FileHeaderPtr;
  UINT32                        FileLength;
  BOOLEAN                       FileGuidFound;
  UINT8                         *SkipRegionAddress;


  FvHeaderPtr   = NULL;
  FileHeaderPtr = NULL;
  BiosImgOffset = 0;
  FileLength    = 0;
  FileGuidFound = FALSE;

  BiosImgAddress = ImageAddress;
  Status = RetrieveFirmware (&BiosImgAddress, ImageSize, &BiosImgOffset);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find the skip region from the image, preserve it before modifying it.
  //
  for (Index = 0; Index < (ImageSize - BiosImgOffset); Index += ALIGHMENT_SIZE) {
    FvHeaderPtr = (EFI_FIRMWARE_VOLUME_HEADER *)(BiosImgAddress + Index);
    if ((FvHeaderPtr->Signature == EFI_FVH_SIGNATURE) &&
        (CompareGuid (&FvHeaderPtr->FileSystemGuid, &gEfiFirmwareFileSystem2Guid))) {
      Index2 = 0;
      while (Index2 < FvHeaderPtr->FvLength) {
        FileHeaderPtr = (EFI_FFS_FILE_HEADER *)(BiosImgAddress + Index + FvHeaderPtr->HeaderLength + Index2);
        if (CompareGuid (PcdGetPtr (PcdUnsignedFvKeyFile), &FileHeaderPtr->Name) == TRUE) {
          FileGuidFound = TRUE;
          break;
        } else {
          FileLength = *(UINT32 *)(FileHeaderPtr->Size) & 0x00FFFFFF;
          Index2 += FileLength;
        }
      }
    }
    if (FileGuidFound) {
      break;
    }
  }
  if (!FileGuidFound) {
    return EFI_NOT_FOUND;
  }

  SkipRegionAddress = (UINT8 *)FvHeaderPtr;
  *SkipRegionOffset = Index;
  *SkipRegionSize = (UINTN)(FvHeaderPtr->FvLength);

  *SkipRegionDataBuffer = AllocatePool (*SkipRegionSize);
  if (*SkipRegionDataBuffer == NULL) {
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem (*SkipRegionDataBuffer, SkipRegionAddress, *SkipRegionSize);
  SetMem (SkipRegionAddress, *SkipRegionSize, 0xFF);

  return EFI_SUCCESS;
}

/**
  Restore the data of the unsigned FV region.

  @param [in]   ImageAddress           the image address
  @param [in]   ImageSize              the size of the image
  @param [in]   SkipRegionOffset       the offset of the unsigned FV region from the image
  @param [in]   SkipRegionSize         the size of the unsigned FV region
  @param [in]   SkipRegionDataBuffer   the pointer of the backup buffer

  @retval EFI_SUCCESS          backup succuessfully
  @retval EFI_NOT_FOUND        cannot find the BIOS address from the image

**/
EFI_STATUS
RestoreSkipRegion (
  IN UINT8                      *ImageAddress,
  IN UINTN                      ImageSize,
  IN UINTN                      SkipRegionOffset,
  IN UINTN                      SkipRegionSize,
  IN UINT8                      *SkipRegionDataBuffer
  )
{
  EFI_STATUS                    Status;
  UINTN                         BiosImgOffset;
  UINT8                         *BiosImgAddress;


  BiosImgOffset = 0;
  BiosImgAddress = ImageAddress;

  if ((ImageAddress != NULL) && (SkipRegionDataBuffer != NULL)) {
    Status = RetrieveFirmware (&BiosImgAddress, ImageSize, &BiosImgOffset);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    CopyMem (BiosImgAddress + SkipRegionOffset, SkipRegionDataBuffer, SkipRegionSize);
    FreePool (SkipRegionDataBuffer);
  }

  return EFI_SUCCESS;
}

