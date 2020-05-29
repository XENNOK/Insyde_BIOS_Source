/** @file
  Provides bit BLT (Block Transfer) related functions.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BitBltLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

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
  )
{
  INTN                          Index;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Bitmap;

  if (Image == NULL || Image->Bitmap == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if ((Image->Flags & H2O_IMAGE_ALPHA_CHANNEL) == H2O_IMAGE_ALPHA_CHANNEL) {
    return EFI_SUCCESS;
  }

  Bitmap    = Image->Bitmap;

  Index = Image->Width * Image->Height;
  if ((Image->Flags & EFI_IMAGE_TRANSPARENT) == EFI_IMAGE_TRANSPARENT) {
    while (--Index >= 0) {
      if (*(UINT32 *)Bitmap != 0) {
        Bitmap->Reserved = 0xFF;
      }
      ++Bitmap;
    }
  } else {
    while (--Index >= 0) {
      (Bitmap++)->Reserved = 0xFF;
    }
  }

  Image->Flags = H2O_IMAGE_ALPHA_CHANNEL;

  return EFI_SUCCESS;
}

#define muldiv255(a,b)		((unsigned char)((((a)+1)*(b))>>8))		/* very fast, 92% accurate*/
/**
 Internal function to blend pixle according to alpha channel.

 @param[in]   Dst      Input destination pixel in format EFI_GRAPHICS_OUTPUT_BLT_PIXEL.
 @param[in]   Src      Input source pixel in format EFI_GRAPHICS_OUTPUT_BLT_PIXEL.
**/
VOID
BlendPixel (
  IN OUT   EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *Dst,
  IN       EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *Src
  )
{
  UINT8        Alpha;

  ASSERT (Dst != NULL);
  ASSERT (Src != NULL);

  Alpha = Src->Reserved;
  if (Alpha == 255) {
    *(UINT32*) Dst = *(UINT32 *) Src;
  } else if (Alpha != 0) {
    Dst->Red   += muldiv255 (Alpha, Src->Red - Dst->Red);
    Dst->Green += muldiv255 (Alpha, Src->Green - Dst->Green);
    Dst->Blue  += muldiv255 (Alpha, Src->Blue - Dst->Blue);
  }

  if (Dst->Reserved != 255) {
    if (Alpha == 255) {
      Dst->Reserved = 255;
    } else {
      Dst->Reserved  += muldiv255 (Alpha, 255 - Dst->Reserved);
    }
  }
}