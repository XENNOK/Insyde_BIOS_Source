/** @file
  Provides bit BLT (Block Transfer) related functions.

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

#ifndef _BIT_BLT_LIB_H_
#define _BIT_BLT_LIB_H_

#include <Uefi.h>
#include <InternalFormRepresentation.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiImage.h>

/**
 Convert image to H2O_IMAGE_ALPHA_CHANNEL image format.

 if input image is H2O_IMAGE_ALPHA_CHANNEL image format, return EFI_SUCCESS directly.

 @param[in] *Image              Pointer to EFI_IMAGE_INPUT instance.

 @retval EFI_SUCCESS            Convert to  H2O_IMAGE_ALPHA_CHANNEL image format successfully.
 @retval EFI_INVALID_PARAMETER  Image is NULL or Image->Bitmap is NULL.
**/
EFI_STATUS
ConvertToAlphaChannelImage (
  IN OUT   EFI_IMAGE_INPUT    *Image
  );

/**
 Internal function to blend pixle according to alpha channel.

 @param[in]   Dst      Input destination pixel in format EFI_GRAPHICS_OUTPUT_BLT_PIXEL.
 @param[in]   Src      Input source pixel in format EFI_GRAPHICS_OUTPUT_BLT_PIXEL.
**/
VOID
BlendPixel (
  IN OUT   EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *Dst,
  IN       EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *Src
  );

#endif