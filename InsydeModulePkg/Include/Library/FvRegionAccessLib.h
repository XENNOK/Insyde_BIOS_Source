/** @file
  Functions for read/write specific FV regions

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

#ifndef _FV_REGION_ACCESS_LIB_H_
#define _FV_REGION_ACCESS_LIB_H_

/**
  ReadFvRegion function

  This routine reads image from platform with specific image GUID defined in
  RegionBuf

  @param RegionBuf              pointer to FV Region Buffer

  @return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ReadFvRegion (
  IN EFI_GUID            *ImageTypeGuid,
  IN OUT UINTN           *ImageSize,
  OUT VOID               *RegionBuf,
  OUT UINT8              *Progress
  );

/**
  WriteFvRegion function

  This routine write image to platform with specific capsule GUID defined in
  RegionBuf

  @param[in]      ImageTypeGuid  Pointer to the image type GUID
  @param[in]      ImageSize      Pointer to the image size
  @param[in]      RegionBuf      The region buffer
  @param[out]     Progress       The current progress update (0 to 100)

  @retval EFI_SUCCESS            The FV region was successfully written
  @retval EFI_INVALID_PARAMETER  Invalid parameter list
  @retval EFI_BUFFER_TOO_SMALL   The given ImageSize is too small
  @retval EFI_UNSUPPORTED        Unsupported for the specified image type
  @return others                 FV Region write failed

**/
EFI_STATUS
EFIAPI
WriteFvRegion (
  IN EFI_GUID           *ImageTypeGuid,
  IN UINTN              ImageSize,
  IN VOID               *RegionBuf,
  OUT UINT8             *Progress
  );

#endif