/** @file
 Support functions for Setup Mouse Protocol

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

#include "SetupMouse.h"
#include <Protocol/HiiFont.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiLib.h>

extern EFI_HII_HANDLE                    mImageHiiHandle;
/**
 Get setup mouse start offset.

 @param [in]   GraphicsOutput   Pointer to EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
 @param [out]  OffsetX          Pointer to setup mouse start x-axis.
 @param [out]  OffsetY          Pointer to setup mouse start y-axis.

 @retval EFI_INVALID_PARAMETER  Some input parameters are invalid
 @retval EFI_SUCCESS            Get Setup mouse start offset successful.

**/
EFI_STATUS
GetStartOffset (
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput,
  OUT     UINTN                          *OffsetX,
  OUT     UINTN                          *OffsetY
  )
{
  EFI_STATUS              Status;
  UINTN                   Rows;
  UINTN                   Columns;

  ASSERT (GraphicsOutput != NULL);
  ASSERT (OffsetX != NULL);
  ASSERT (OffsetY != NULL);

  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &Columns,
                          &Rows
                          );
  ASSERT_EFI_ERROR (Status);

  *OffsetX = ((GraphicsOutput->Mode->Info->HorizontalResolution - Columns *EFI_GLYPH_WIDTH)) >> 1;
  *OffsetY = ((GraphicsOutput->Mode->Info->VerticalResolution   - Rows *EFI_GLYPH_HEIGHT))   >> 1;

  return EFI_SUCCESS;
}

/**
 Invalidate Image and update GOP invalidate region

 @param [in]   Private          Setup mouse private data
 @param [in]   ImageInfo        Image information

 @retval N/A

**/
VOID
InvalidateImage (
  IN PRIVATE_MOUSE_DATA                    *Private,
  IN IMAGE_INFO                           *ImageInfo
  )

{
  GOP_ENTRY                     *GopEntry;
  LIST_ENTRY                    *Node;


  Private = mPrivate;
  if (!ImageInfo->Visible) {
    return ;
  }

  Node = GetFirstNode (&Private->GopList);
  while (!IsNull (&Private->GopList, Node)) {
    GopEntry = GOP_ENTRY_FROM_THIS (Node);
    UnionRect (&GopEntry->InvalidateRc, &GopEntry->InvalidateRc, &ImageInfo->ImageRc);
    Node = GetNextNode (&Private->GopList, Node);
  }
}


/**
 Hide Image and update GOP invalidate region

 @param [in]   ImageInfo        Image information

 @retval N/A

**/
VOID
HideImage (
  IN  IMAGE_INFO                           *ImageInfo
  )
{
  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;
  ASSERT_LOCKED (&Private->SetupMouseLock);

  if (!ImageInfo->Visible) {
    return ;
  }

  //
  // invalidate original image
  //
  InvalidateImage (Private, ImageInfo);
  ImageInfo->Visible = FALSE;
}


/**
 Show Image and update GOP invalidate region

 @param [in]   ImageInfo        Image information

 @retval N/A

**/
VOID
ShowImage (
  IN  IMAGE_INFO                           *ImageInfo
  )

{
  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;
  ASSERT_LOCKED (&Private->SetupMouseLock);
  if (ImageInfo->Visible) {
    return ;
  }

  //
  // invalidate original image
  //
  ImageInfo->Visible = TRUE;
  InvalidateImage (Private, ImageInfo);
}



/**
 Move Image

 @param [in]   ImageInfo        Pointer of image
 @param [in]   X                Image x axis
 @param [in]   Y                Image y axis

 @retval N/A

**/
VOID
MoveImage (
  IN  IMAGE_INFO                           *ImageInfo,
  IN  UINTN                                X,
  IN  UINTN                                Y
  )
{

  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;
  ASSERT_LOCKED (&Private->SetupMouseLock);

  //
  // invalidate original image
  //
  if (ImageInfo->Visible) {
    InvalidateImage (Private, ImageInfo);
  }

  OffsetRect (
    &ImageInfo->ImageRc,
    (INT32)(X - ImageInfo->ImageRc.left),
    (INT32)(Y - ImageInfo->ImageRc.top)
    );

  //
  // invalidate new position image
  //
  if (ImageInfo->Visible) {
    InvalidateImage (Private, ImageInfo);
  }
}

/**
 Create Image

 @param        Width            Image Width
 @param        Height           Image Height

 @return Image data

**/
IMAGE_INFO *
CreateImage (
  UINTN Width,
  UINTN Height
  )
{
  IMAGE_INFO *ImageInfo;

  ImageInfo = AllocateZeroPool (sizeof (IMAGE_INFO));
  if (ImageInfo == NULL) {
    return NULL;
  }

  ImageInfo->Image = AllocateZeroPool (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);
  if (ImageInfo->Image == NULL) {
    gBS->FreePool (ImageInfo);
    return NULL;
  }

  return ImageInfo;
}

/**
 Destroy image

 @param        ImageInfo        Image information

 @retval N/A

**/
VOID
DestroyImage (
  IMAGE_INFO *ImageInfo
  )
{
  if (ImageInfo->Image != NULL) {
    gBS->FreePool (ImageInfo->Image);
    ImageInfo->Image = NULL;
  }
}

/**
 Get font information, prefer TTF font

 @param[in]  TextColor          Font text color
 @param[in]  BackgroundColor    Font background color
 @param[in]  FontSize           Font size

 @return EFI_FONT_DISPLAY_INFO  TTF font or system font

**/
STATIC
EFI_FONT_DISPLAY_INFO  *
GetFontInfo (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TextColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BackgroundColor,
  IN UINT16                        FontSize
  )

{
  STATIC EFI_FONT_DISPLAY_INFO  *FontInfo = NULL;
  EFI_FONT_HANDLE               FontHandle;
  EFI_STATUS                    Status;

  if (FontInfo != NULL) {
    CopyMem (&FontInfo->ForegroundColor, TextColor, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    CopyMem (&FontInfo->BackgroundColor, BackgroundColor, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

    if (FontInfo->FontInfo.FontName[0] != '\0') {
      FontInfo->FontInfo.FontSize = FontSize;
    }
    return FontInfo;
  }

  FontInfo = AllocateZeroPool (sizeof (EFI_FONT_DISPLAY_INFO) + StrSize (L"ttf") - sizeof (CHAR16));
  if (FontInfo == NULL) {
    return NULL;
  }

  StrCpy (FontInfo->FontInfo.FontName, L"ttf");
  FontInfo->FontInfoMask = EFI_FONT_INFO_ANY_STYLE | EFI_FONT_INFO_ANY_SIZE;
  FontHandle = NULL;
  Status = gHiiFont->GetFontInfo (
                       gHiiFont,
                       &FontHandle,
                       FontInfo,
                       NULL,
                       NULL
                       );
  if (EFI_ERROR (Status)) {
    FontInfo->FontInfo.FontName[0] = 0;
    FontInfo->FontInfo.FontSize = 19;
  } else {
    FontInfo->FontInfo.FontSize = FontSize;
  }
  CopyMem (&FontInfo->ForegroundColor, TextColor, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&FontInfo->BackgroundColor, BackgroundColor, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  FontInfo->FontInfoMask = 0;

  return FontInfo;
}

/**
 Draw text to blt buffer by TTF font.

 @param [in]   String           String
 @param [in]   Blt              Output blt buffer
 @param [in]   BackgroundColor  Background color
 @param [in]   String           Destination LineLen
 @param [in]   TextRc           Destination rectangle
 @param [in]   FontInfo         Points to the string font and color information
                                or NULL if the string should use the default
                                system font and color.

 @return return StringToImage status.
**/
EFI_STATUS
TrueTypeStringToImage (
  IN CHAR16                            *String,
  EFI_FONT_DISPLAY_INFO                *FontInfo,
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt,
  IN UINT16                            Width,
  IN UINT16                            Height,
  IN UINT16                            Delta
  )
{

  EFI_STATUS                            Status;
  INTN                                  Y;
  INTN                                  X;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer2;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *Bitmap;
  EFI_IMAGE_OUTPUT                      *Image;
  UINTN                                 BltBufferWidth;
  UINT16                                ImageWidth;

  ASSERT (String != NULL);
  ASSERT (Blt != NULL);

  BltBuffer = Blt;
  while (*String != 0) {
    Image = NULL;
    Status = gHiiFont->GetGlyph (
                         gHiiFont,
                         *String,
                         FontInfo,
                         &Image,
                         NULL
                         );
    if (Status != EFI_SUCCESS) {
      //
      // avoid status is REPLACE_UNKNOWN_GLYPH
      //
      String++;
      continue;
    }

    ImageWidth = Image->Width;
    if (ImageWidth >= Width) {
      FreePool (Image->Image.Bitmap);
      FreePool (Image);
      break;
    }

    //
    // glyph to buffer
    //
    Y              = Image->Height;
    Bitmap         = Image->Image.Bitmap;
    BltBuffer2     = BltBuffer;
    BltBufferWidth = ImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    while (--Y >= 0) {
      X = Image->Width;
      while (--X >= 0) {
        BlendPixel (BltBuffer2 + X, Bitmap + X);
      }
      BltBuffer2 += Delta;
      Bitmap     += ImageWidth;
    }

    Width     -= ImageWidth;
    BltBuffer += ImageWidth;
    String++;

    FreePool (Image->Image.Bitmap);
    FreePool (Image);
  }

  return EFI_SUCCESS;
}

/**
  Get the width of a unicode string.

  @param[in] UnicodeChar        The unicode character to be inquired

  @return UINTN                 The width of the query string.

**/
STATIC
UINTN
SetupMouseGetStringWidth (
  IN EFI_FONT_DISPLAY_INFO      *FontInfo,
  IN CHAR16                     *String
  )
{
  EFI_STATUS                    Status;
  CHAR16                        Char;
  UINTN                         Width;
  EFI_IMAGE_OUTPUT              *Image;

  ASSERT (FontInfo != NULL);
  ASSERT (String != NULL);

  if (FontInfo->FontInfo.FontName[0] == 0) {
    return UnicodeStringDisplayLength (String) * EFI_GLYPH_WIDTH;
  }

  Width = 0;
  while ((Char = *String++) != '\0') {


    Image = NULL;
    Status = gHiiFont->GetGlyph (
                         gHiiFont,
                         Char,
                         FontInfo,
                         &Image,
                         NULL
                         );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    Width += Image->Width;
    FreePool (Image->Image.Bitmap);
    FreePool (Image);
  }

  return Width;
}



/**
 Use HiiFont to draw text to blt buffer by font info.

 @param [in]   ImageInfo        Destination Image
 @param [in]   TextColor        Text Color
 @param [in]   BackgroundColor  Background color
 @param [in]   String           Destination LineLen
 @param [in]   TextRc           Destination rectangle
 @param [in]   FontInfo         Points to the string font and color information
                                or NULL if the string should use the default
                                system font and color.

 @return return StringToImage status.
**/
STATIC
EFI_STATUS
SetupMouseDrawTextByFontInfo (
  IN IMAGE_INFO                    *ImageInfo,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TextColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BackgroundColor,
  IN EFI_STRING                    String,
  IN RECT                          *TextRc,
  IN EFI_FONT_DISPLAY_INFO         *FontInfo
  )

{
  EFI_STATUS                    Status;
  EFI_IMAGE_OUTPUT              Blt;
  EFI_IMAGE_OUTPUT              *BltPtr;
  UINTN                         OffsetX;
  UINTN                         OffsetY;
  UINTN                         StrWidth;

  BltPtr           = &Blt;
  ZeroMem (&Blt, sizeof (EFI_IMAGE_OUTPUT));

  Blt.Image.Bitmap = ImageInfo->Image;
  Blt.Width        = (UINT16) (ImageInfo->ImageRc.right - ImageInfo->ImageRc.left);
  Blt.Height       = (UINT16) (ImageInfo->ImageRc.bottom - ImageInfo->ImageRc.top);
  //
  // text align: center
  //
  StrWidth = SetupMouseGetStringWidth (FontInfo, String);
  if ((UINTN)(TextRc->right - TextRc->left) >= StrWidth) {
    OffsetX  = ((TextRc->right + TextRc->left) - StrWidth) / 2;
  } else {
    OffsetX = TextRc->left;
  }
  if ((UINTN)(TextRc->bottom - TextRc->top) >= EFI_GLYPH_HEIGHT) {
    OffsetY  = ((TextRc->bottom + TextRc->top) - EFI_GLYPH_HEIGHT) / 2;
  } else {
    OffsetY  = TextRc->top;
  }

  if (FontInfo->FontInfo.FontName[0] == '\0') {
    Status = gHiiFont->StringToImage (
                         gHiiFont,
                         EFI_HII_OUT_FLAG_CLIP | EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_TRANSPARENT,
                         String,
                         FontInfo,
                         &BltPtr,
                         OffsetX,
                         OffsetY,
                         NULL,
                         NULL,
                         NULL
                         );
  } else {

    Status = TrueTypeStringToImage (
               (CHAR16 *) String,
               FontInfo,
               Blt.Image.Bitmap + Blt.Width * OffsetY + OffsetX,
               (UINT16) Blt.Width,
               (UINT16) Blt.Height,
               (UINT16) Blt.Width
               );
  }

  return Status;
}

/**
 Use HiiFont to draw text to blt buffer

 @param [in]   ImageInfo        Destination Image
 @param [in]   TextColor        Text Color
 @param [in]   BackgroundColor  Background color
 @param [in]   String           Destination LineLen
 @param [in]   TextRc           Destination rectangle

 @return return StringToImage status

**/
EFI_STATUS
SetupMouseDrawText (
  IN IMAGE_INFO                    *ImageInfo,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TextColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BackgroundColor,
  IN EFI_STRING                    String,
  IN RECT                          *TextRc
  )

{
  EFI_FONT_DISPLAY_INFO      *FontInfo;
  EFI_STATUS                 Status;

  FontInfo = GetFontInfo (TextColor, BackgroundColor, 19);
  if (FontInfo == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = SetupMouseDrawTextByFontInfo (
             ImageInfo,
             TextColor,
             BackgroundColor,
             String,
             TextRc,
             FontInfo
             );

  return Status;

}

#define SIGN(x) (((x) > 0) ? 1 : (((x) == 0) ? 0 : -1))


/**
 Stretch image function from microwindows fblin32alpha function

 @param        DstBlt           Destination Blt Buffer
 @param        SrcBlt           Source Blt Buffer
 @param        DstLineLen       Destination LineLen
 @param        SrcLineLen       Source LineLen
 @param        dest_x_start     Destination X
 @param        dest_y_start     Destination Y
 @param        width            Width
 @param        height           Height
 @param        x_denominator    X denominator
 @param        y_denominator    Y denominator
 @param        src_x_fraction   Source X fraction
 @param        src_y_fraction   Source Y fraction
 @param        x_step_fraction  X step fraction
 @param        y_step_fraction  Y step fraction
 @param        op               Directly src copy to dst

 @retval N/A

**/
VOID
SetupMouseLinear32aStretchBlitEx(
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *DstBlt,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *SrcBlt,
  INT32                         DstLineLen,
  INT32                         SrcLineLen,
  INT32                         dest_x_start,
  INT32                         dest_y_start,
  INT32                         width,
  INT32                         height,
  INT32                         x_denominator,
  INT32                         y_denominator,
  INT32                         src_x_fraction,
  INT32                         src_y_fraction,
  INT32                         x_step_fraction,
  INT32                         y_step_fraction,
  INT32                         op
  )
{
  /* Pointer to the current pixel in the source image */
  UINT32 *src_ptr;

  /* Pointer to x=xs1 on the next line in the source image */
  UINT32 *next_src_ptr;

  /* Pointer to the current pixel in the dest image */
  UINT32 *dest_ptr;

  /* Pointer to x=xd1 on the next line in the dest image */
  UINT32 *next_dest_ptr;

  /* Keep track of error in the source co-ordinates */
  int x_error;
  int y_error;

  /* 1-unit steps "forward" through the source image, as steps in the image
   * byte array.
   */
  int src_x_step_one;
  int src_y_step_one;

  /* normal steps "forward" through the source image, as steps in the image
   * byte array.
   */
  int src_x_step_normal;
  int src_y_step_normal;

  /* 1-unit steps "forward" through the source image, as steps in the image
   * byte array.
   */
  int x_error_step_normal;
  int y_error_step_normal;

  /* Countdown to the end of the destination image */
  int x_count;
  int y_count;

  /* Start position in source, in whole pixels */
  int src_x_start;
  int src_y_start;

  /* Error values for start X position in source */
  int x_error_start;

  /* 1-unit step down dest, in bytes. */
  int dest_y_step;

  /*DPRINTF("Nano-X: linear32_stretchflipblit( dest=(%d,%d) %dx%d )\n",
     dest_x_start, dest_y_start, width, height); */

  /* We add half a dest pixel here so we're sampling from the middle of
   * the dest pixel, not the top left corner.
   */
  src_x_fraction += (x_step_fraction >> 1);
  src_y_fraction += (y_step_fraction >> 1);

  /* Seperate the whole part from the fractions.
   *
   * Also, We need to do lots of comparisons to see if error values are
   * >= x_denominator.  So subtract an extra x_denominator for speed - then
   * we can just check if it's >= 0.
   */
  src_x_start = src_x_fraction / x_denominator;
  src_y_start = src_y_fraction / y_denominator;
  x_error_start = src_x_fraction - (src_x_start + 1) * x_denominator;
  y_error = src_y_fraction - (src_y_start + 1) * y_denominator;

  /* precalculate various deltas */

  src_x_step_normal = x_step_fraction / x_denominator;
  src_x_step_one = SIGN(x_step_fraction);
  x_error_step_normal = ABS(x_step_fraction) - ABS(src_x_step_normal) * x_denominator;

  src_y_step_normal = y_step_fraction / y_denominator;
  src_y_step_one = SIGN(y_step_fraction) * SrcLineLen;
  y_error_step_normal = ABS(y_step_fraction) - ABS(src_y_step_normal) * y_denominator;
  src_y_step_normal *= SrcLineLen;

  /* DPRINTF("linear32alpha_stretchblitex: X: One step=%d, err-=%d; normal step=%d, err+=%d\n"
    "Y: One step=%d, err-=%d; normal step=%d, err+=%d\n",
     src_x_step_one, x_denominator, src_x_step_normal, x_error_step_normal,
     src_y_step_one, y_denominator, src_y_step_normal, y_error_step_normal);
   */

  /* Pointer to the first source pixel */
  next_src_ptr = ((UINT32 *) SrcBlt) +
    src_y_start * SrcLineLen + src_x_start;

  /* Cache the width of a scanline in dest */
  dest_y_step = DstLineLen;

  /* Pointer to the first dest pixel */
  next_dest_ptr = ((UINT32 *) DstBlt) + (dest_y_start * dest_y_step) + dest_x_start;

  /*
   * Note: The MWROP_SRC and MWROP_XOR_FGBG cases below are simple
   * expansions of the default case.  They can be removed without
   * significant speed penalty if you need to reduce code size.
   *
   * The SRC_OVER case cannot be removed (since applyOp doesn't
   * handle it correctly).
   *
   * The MWROP_CLEAR case could be removed.  But it is a large
   * speed increase for a small quantity of code.
   *
   * FIXME Porter-Duff rules other than SRC_OVER not handled!!
   */
  switch (op) {
  case 0:
    /* Benchmarking shows that this while loop is faster than the equivalent
     * for loop: for(y_count=0; y_count<height; y_count++) { ... }
     */
    y_count = height;
    while (y_count-- > 0) {
      src_ptr = next_src_ptr;
      dest_ptr = next_dest_ptr;

      x_error = x_error_start;

      x_count = width;
      while (x_count-- > 0) {
        BlendPixel ((EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) dest_ptr, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)  src_ptr);
        dest_ptr++;

        src_ptr += src_x_step_normal;
        x_error += x_error_step_normal;

        if (x_error >= 0) {
          src_ptr += src_x_step_one;
          x_error -= x_denominator;
        }
      }

      next_dest_ptr += dest_y_step;
      next_src_ptr += src_y_step_normal;
      y_error += y_error_step_normal;

      if (y_error >= 0) {
        next_src_ptr += src_y_step_one;
        y_error -= y_denominator;
      }
    }
    break;
  }
}


/**
 Stretch image function from microwindows fblin32alpha function

 @param        DstBlt           Destination Blt Buffer
 @param        d1_x             Destination X1
 @param        d1_y             Destination Y1
 @param        d2_x             Destination X2
 @param        d2_y             Destination Y2
 @param        DstLineLen       Destination LineLen
 @param        SrcBlt           Source Blt Buffer
 @param        s1_x             Source X1
 @param        s1_y             Source Y1
 @param        s2_x             Source X2
 @param        s2_y             Source Y2
 @param        SrcLineLen

 @retval N/A

**/
void
SetupMouseStretchBlitEx (
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL * DstBlt,
  INT32                           d1_x,
  INT32                           d1_y,
  INT32                           d2_x,
  INT32                           d2_y,
  INT32                           DstLineLen,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *SrcBlt,
  INT32                           s1_x,
  INT32                           s1_y,
  INT32                           s2_x,
  INT32                           s2_y,
  INT32                           SrcLineLen
  )
{
  /* Scale factors (as fractions, numerator/denominator) */
  int src_x_step_numerator;
  int src_x_step_denominator;
  int src_y_step_numerator;
  int src_y_step_denominator;

  /* Clipped dest co-ords */
  INT32 c1_x;
  INT32 c1_y;
  INT32 c2_x;
  INT32 c2_y;

  /* Initial source co-ordinates, as a fraction (denominators as above) */
  int src_x_start_exact;
  int src_y_start_exact;

  /* Sort co-ordinates so d1 is top left, d2 is bottom right. */
  if (d1_x > d2_x) {
    register INT32 tmp = d2_x;
    d2_x = d1_x;
    d1_x = tmp;
    tmp = s2_x;
    s2_x = s1_x;
    s1_x = tmp;
  }

  if (d1_y > d2_y) {
    register INT32 tmp = d2_y;
    d2_y = d1_y;
    d1_y = tmp;
    tmp = s2_y;
    s2_y = s1_y;
    s1_y = tmp;
  }

  /* Need to preserve original values, so make a copy we can clip. */
  c1_x = d1_x;
  c1_y = d1_y;
  c2_x = d2_x;
  c2_y = d2_y;

  /* Calculate how far in source co-ordinates is
   * equivalent to one pixel in dest co-ordinates.
   * This is stored as a fraction (numerator/denominator).
   * The numerator may be > denominator.  The numerator
   * may be negative, the denominator is always positive.
   *
   * We need half this distance for some purposes,
   * hence the *2.
   *
   * The +1s are because we care about *sizes*, not
   * deltas.  (Without the +1s it just doesn't
   * work properly.)
   */
  src_x_step_numerator = (s2_x - s1_x + 1) << 1;
  src_x_step_denominator = (d2_x - d1_x + 1) << 1;
  src_y_step_numerator = (s2_y - s1_y + 1) << 1;
  src_y_step_denominator = (d2_y - d1_y + 1) << 1;

///* Clip against dest window (NOT dest clipping region). */
  if (c1_x < 0)
    c1_x = 0;
  if (c1_y < 0)
    c1_y = 0;
//if (c2_x > DstLineLen)
//  c2_x = dstpsd->xvirtres;
//if (c2_y > dstpsd->yvirtres)
//  c2_y = dstpsd->yvirtres;

  /* Final fully-offscreen check */
  if ((c1_x >= c2_x) || (c1_y >= c2_y)) {
    /* DPRINTF("Nano-X: GdStretchBlitEx: CLIPPED OFF (final check)\n"); */
    return;
  }

  /* Well, if we survived that lot, then we now have a destination
   * rectangle defined in (c1_x,c1_y)-(c2_x,c2_y).
   */

  /* DPRINTF("Nano-X: GdStretchBlitEx: Clipped rect: (%d,%d)-(%d,%d)\n",
         (int) c1_x, (int) c1_y, (int) c2_x, (int) c2_y); */

  /* Calculate the position in the source rectange that is equivalent
   * to the top-left of the destination rectangle.
   */
  src_x_start_exact = s1_x * src_x_step_denominator + (c1_x - d1_x) * src_x_step_numerator;
  src_y_start_exact = s1_y * src_y_step_denominator + (c1_y - d1_y) * src_y_step_numerator;

  /* OK, clipping so far has been against physical bounds, we now have
   * to worry about user defined clip regions.
   */

  /* FIXME: check cursor in src region */
  /* GdCheckCursor(srcpsd, c1_x, c1_y, c2_x-1, c2_y-1); */
  /* DPRINTF("Nano-X: GdStretchBlitEx: no more clipping needed\n"); */
  SetupMouseLinear32aStretchBlitEx (
    DstBlt,
    SrcBlt,
    DstLineLen,
    SrcLineLen,
    c1_x, c1_y,
    c2_x - c1_x,
    c2_y - c1_y,
    src_x_step_denominator,
    src_y_step_denominator,
    src_x_start_exact,
    src_y_start_exact,
    src_x_step_numerator,
    src_y_step_numerator,
    0
    );


}

#define ROP_TRANSPARENT    34


/**
 BitBlt function from microwindows fblin32alpha function

 @param        DstImage         Destination Image information
 @param        DstX             Destination X
 @param        DstY             Destination Y
 @param        Width            Width
 @param        Height           Height
 @param        SrcImage         Destination LineLen
 @param        SrcX             Source X
 @param        SrcY             Source Y
 @param        Rop              copy src to destinamtion or with transparent color

 @retval EFI_SUCCESS            BitBlt is successful

**/
VOID
SetupMouseBitBlt (
  IMAGE_INFO                    *DstImage,
  INTN                          DstX,
  INTN                          DstY,
  UINTN                         Width,
  UINTN                         Height,
  IMAGE_INFO                    *SrcImage,
  INTN                          SrcX,
  INTN                          SrcY,
  UINTN                         Rop
  )
{
  UINT32                        *Dst32;
  UINT32                        *Src32;
  UINTN                         DstDelta;
  UINTN                         SrcDelta;
  UINTN                         Index;
  UINTN                         SrcImageWidth;
  UINTN                         DstImageWidth;
  UINTN                         SrcImageHeight;
  UINTN                         DstImageHeight;

  //
  // Clip blit rectange to source bitmap size
  //
  if (SrcX < 0) {
    Width += SrcX;
    DstX  -= SrcX;
    SrcX  = 0;
  }

  if (SrcY < 0) {
    Height += SrcY;
    DstY   -= SrcY;
    SrcY   = 0;
  }

  Dst32    = (UINT32 *)DstImage->Image;
  Src32    = (UINT32 *)SrcImage->Image;

  DstImageWidth  = (UINTN) DstImage->ImageRc.right - DstImage->ImageRc.left;
  SrcImageWidth  = (UINTN) SrcImage->ImageRc.right - SrcImage->ImageRc.left;
  DstImageHeight = (UINTN) DstImage->ImageRc.bottom - DstImage->ImageRc.top;
  SrcImageHeight = (UINTN) SrcImage->ImageRc.bottom - SrcImage->ImageRc.top;
  DstDelta       = DstImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  SrcDelta       = SrcImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);


  if ((SrcX + Width) > SrcImageWidth) {
    Width = SrcImageWidth - SrcX;
  }

  if ((SrcY + Height) > SrcImageHeight) {
    Height = SrcImageHeight - SrcY;
  }



  ASSERT (Dst32 != NULL && Src32 != NULL);
  ASSERT (DstX >= 0 && DstX <= (INTN)DstImageWidth);
  ASSERT (DstY >= 0 && DstY <= (INTN)DstImageHeight);
  ASSERT (Width > 0 && Height > 0);
  ASSERT (SrcX >= 0 && SrcX <= (INTN)SrcImageWidth);
  ASSERT (SrcY >= 0 && SrcY <= (INTN)SrcImageHeight);
  ASSERT (DstX + Width  <= DstImageWidth);
  ASSERT (DstY + Height <= DstImageHeight);
  ASSERT (SrcX + Width  <= SrcImageWidth);
  ASSERT (SrcY + Height <= SrcImageHeight);

  Dst32 = (UINT32 *)((UINT8 *)Dst32 + DstY * DstDelta + DstX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  Src32 = (UINT32 *)((UINT8 *)Src32 + SrcY * SrcDelta + SrcX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  if (Rop == ROP_TRANSPARENT) {
    DstDelta = DstDelta - Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    SrcDelta = SrcDelta - Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

    while (Height > 0) {
      for (Index = 0; Index < Width; Index++) {
        if (*Src32 != 0) {
          *Dst32 = *Src32 & 0x00FFFFFF;
        }
        Src32++;
        Dst32++;
      }
      Dst32 = (UINT32 *)((UINT8 *)Dst32 + DstDelta);
      Src32 = (UINT32 *)((UINT8 *)Src32 + SrcDelta);
      Height--;
    }
  } else {
    DstDelta = DstDelta - Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    SrcDelta = SrcDelta - Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

    while (Height > 0) {
      for (Index = 0; Index < Width; Index++) {
        *Dst32 = *Src32;
        Src32++;
        Dst32++;
      }
      Dst32 = (UINT32 *)((UINT8 *)Dst32 + DstDelta);
      Src32 = (UINT32 *)((UINT8 *)Src32 + SrcDelta);
      Height--;
    }
  }
}

/**
 StretchBlt function from microwindows fblin32alpha function

 @param        DstX             Destination X
 @param        DstY             Destination Y
 @param        DstWidth         Destination Width
 @param        DstHeight        Destination Height
 @param        SrcBlt           Source Blt buffer
 @param        SrcX             Source X
 @param        SrcY             Source Y
 @param        SrcWidth         Source Width
 @param        SrcHeight        Source Height
 @param        SrcDelta         Source buffer LineLen
 @param        Rop              directly copy source to destination or with transparent

 @retval EFI_SUCCESS            BitBlt is successful

**/
EFI_STATUS
SetupMouseStretchBlt (
  INTN                          DstX,
  INTN                          DstY,
  UINTN                         DstWidth,
  UINTN                         DstHeight,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *SrcBlt,
  INTN                          SrcX,
  INTN                          SrcY,
  UINTN                         SrcWidth,
  UINTN                         SrcHeight,
  INT32                         SrcDelta,
  UINT16                        Rop
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltPtr;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *ScreenPtr;
  UINTN                             Width;
  UINTN                             Height;
  UINTN                             X;
  INT32                             KeyboardWidth;
  INT32                             KeyboardHeight;

  KeyboardWidth  = mPrivate->Keyboard.ImageRc.right - mPrivate->Keyboard.ImageRc.left;
  KeyboardHeight = mPrivate->Keyboard.ImageRc.bottom - mPrivate->Keyboard.ImageRc.top;

  if (SrcWidth != DstWidth || SrcHeight != DstHeight) {
    SetupMouseStretchBlitEx (
      mPrivate->Keyboard.Image,
      (INT32)DstX,
      (INT32)DstY,
      (INT32)DstX + (INT32)DstWidth,
      (INT32)DstY + (INT32)DstHeight,
      (INT32)KeyboardWidth,
      SrcBlt,
      (INT32)SrcX,
      (INT32)SrcY,
      (INT32)SrcX + (INT32)SrcWidth,
      (INT32)SrcY + (INT32)SrcHeight,
      SrcDelta / sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
      );
  } else {
    Width  = SrcWidth;
    Height = SrcHeight;
    ScreenPtr = &mPrivate->Keyboard.Image[DstY * KeyboardWidth + DstX];
    while (Height) {
      BltPtr = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)
                ((UINT8 *) SrcBlt + SrcY * SrcDelta + SrcX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

      for (X = 0; X < Width; X++) {
        if (*(UINT32 *)&BltPtr[X] != 0) {
          BlendPixel ((EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (&ScreenPtr[X]), (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)  (&BltPtr[X]));
        }
      }
      ScreenPtr += KeyboardWidth;
      SrcY ++;
      Height--;
    }
  }

  return EFI_SUCCESS;
}

/**
 Scale image by Scale9Grid algorithm

 @param [in]   DstRc            Destination Rectangle
 @param [in]   Scale9Grid       scale 9 grid
 @param [in]   ImageBlt         Image Blt buffer
 @param [in]   ImageWidth
 @param [in]   ImageHeight
 @param [in]   ImageDelta

 @retval None

**/
VOID
SetupMouseDrawImage (
  IN RECT                          *DstRc,
  IN RECT                          *Scale9Grid,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBlt,
  IN INT32                         ImageWidth,
  IN INT32                         ImageHeight,
  IN INT32                         ImageDelta
  )
{

  RECT DstHoldDstRc;
  RECT SrcHoldDstRc;

  //
  //
  //
  DstHoldDstRc.left    = DstRc->left   + Scale9Grid->left;
  DstHoldDstRc.top     = DstRc->top    + Scale9Grid->top;
  DstHoldDstRc.right   = DstRc->right  - Scale9Grid->right;
  DstHoldDstRc.bottom  = DstRc->bottom - Scale9Grid->bottom;

  SrcHoldDstRc.left    = Scale9Grid->left;
  SrcHoldDstRc.top     = Scale9Grid->top;
  SrcHoldDstRc.right   = ImageWidth  - Scale9Grid->right;
  SrcHoldDstRc.bottom  = ImageHeight - Scale9Grid->bottom;

  //
  // hold
  //
  SetupMouseStretchBlt (
    DstHoldDstRc.left, DstHoldDstRc.top, DstHoldDstRc.right - DstHoldDstRc.left, DstHoldDstRc.bottom - DstHoldDstRc.top,
    ImageBlt, SrcHoldDstRc.left, SrcHoldDstRc.top, SrcHoldDstRc.right - SrcHoldDstRc.left, SrcHoldDstRc.bottom - SrcHoldDstRc.top,
    ImageDelta, 0
    );


  //
  // left - top
  //
  SetupMouseStretchBlt (
    DstRc->left, DstRc->top, Scale9Grid->left, Scale9Grid->top,
    ImageBlt, 0, 0, Scale9Grid->left, Scale9Grid->top,
    ImageDelta, 0
    );


  //
  // right - top
  //
  SetupMouseStretchBlt (
    DstHoldDstRc.right, DstRc->top, Scale9Grid->right, Scale9Grid->top,
    ImageBlt, SrcHoldDstRc.right, 0, Scale9Grid->right, Scale9Grid->top,
    ImageDelta, 0
    );

  //
  // left - bottom
  //
  SetupMouseStretchBlt (
    DstRc->left, DstHoldDstRc.bottom, Scale9Grid->left, Scale9Grid->bottom,
    ImageBlt, 0, SrcHoldDstRc.bottom, Scale9Grid->left, Scale9Grid->bottom,
    ImageDelta, 0
    );


  //
  // right - bottom
  //
  SetupMouseStretchBlt (
    DstHoldDstRc.right, DstHoldDstRc.bottom, Scale9Grid->right, Scale9Grid->bottom,
    ImageBlt, SrcHoldDstRc.right, SrcHoldDstRc.bottom, Scale9Grid->right, Scale9Grid->bottom,
    ImageDelta, 0
    );

  //
  // top
  //
  SetupMouseStretchBlt (
    DstHoldDstRc.left, DstRc->top, DstHoldDstRc.right - DstHoldDstRc.left, Scale9Grid->top,
    ImageBlt, SrcHoldDstRc.left,       0, SrcHoldDstRc.right - SrcHoldDstRc.left, Scale9Grid->top,
    ImageDelta, 0
    );


  //
  // left
  //
  SetupMouseStretchBlt (
    DstRc->left, DstHoldDstRc.top, Scale9Grid->left, DstHoldDstRc.bottom - DstHoldDstRc.top,
    ImageBlt,        0, SrcHoldDstRc.top, Scale9Grid->left, SrcHoldDstRc.bottom - SrcHoldDstRc.top,
    ImageDelta, 0
    );

  //
  // right
  //
  SetupMouseStretchBlt (
    DstHoldDstRc.right, DstHoldDstRc.top, Scale9Grid->right, DstHoldDstRc.bottom - DstHoldDstRc.top,
    ImageBlt, SrcHoldDstRc.right, SrcHoldDstRc.top, Scale9Grid->right, SrcHoldDstRc.bottom - SrcHoldDstRc.top,
    ImageDelta, 0
    );

  //
  // bottom
  //
  SetupMouseStretchBlt (
    DstHoldDstRc.left, DstHoldDstRc.bottom, DstHoldDstRc.right - DstHoldDstRc.left, Scale9Grid->bottom,
    ImageBlt, SrcHoldDstRc.left, SrcHoldDstRc.bottom, SrcHoldDstRc.right - SrcHoldDstRc.left, Scale9Grid->bottom,
    ImageDelta, 0
    );
}


/**
 Scale image by Scale9Grid algorithm

 @param        DstRc            Destination Rectangle
 @param        Scale9Grid       scale 9 grid
 @param        ImageId

 @retval None

**/
EFI_STATUS
SetupMouseShowBitmap (
  RECT                          *DstRc,
  RECT                          *Scale9Grid,
  UINT16                        ImageId
  )
{
  INT32                   Width;
  INT32                   Height;
  EFI_STATUS              Status;
  IMAGE_INFO              BitmapImage;
  INT32                   KeyboardWidth;
  INT32                   KeyboardHeight;
  PRIVATE_MOUSE_DATA      *Private;
  EFI_IMAGE_INPUT         Image;

  Private = mPrivate;

  if (mImageHiiHandle == NULL) {
    return EFI_NOT_READY;
  }

  Status = gHiiImage->GetImage (
                        gHiiImage,
                        mImageHiiHandle,
                        ImageId,
                        &Image
                        );
  if (EFI_ERROR (Status) || Image.Bitmap == NULL) {
    return Status;
  }

  if ((Image.Flags & H2O_IMAGE_ALPHA_CHANNEL) != H2O_IMAGE_ALPHA_CHANNEL) {
    ConvertToAlphaChannelImage (&Image);
  }

  Width  = MIN ((DstRc->right - DstRc->left), (INT32) Image.Width);
  Height = MIN ((DstRc->bottom - DstRc->top), (INT32) Image.Height);

  KeyboardWidth  = Private->Keyboard.ImageRc.right - Private->Keyboard.ImageRc.left;
  KeyboardHeight = Private->Keyboard.ImageRc.bottom - Private->Keyboard.ImageRc.top;
  BitmapImage.Image    = Image.Bitmap;
  SetRect (&BitmapImage.ImageRc, 0, 0, Image.Width, Image.Height);

  if (Scale9Grid->left == 0 && Scale9Grid->right == 0 && Scale9Grid->top == 0 && Scale9Grid->bottom == 0) {
    SetupMouseBitBlt (
      &Private->Keyboard,
      DstRc->left,
      DstRc->top,
      Width,
      Height,
      &BitmapImage,
      0,
      0,
      ROP_TRANSPARENT
      );
  } else {
    SetupMouseDrawImage (
      DstRc,
      Scale9Grid,
      Image.Bitmap,
      (INT32) Image.Width,
      (INT32) Image.Height,
      sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Image.Width
      );
  }
  FreePool (Image.Bitmap);
  return EFI_SUCCESS;
}

VOID
AcquireSetupMouseLock (
  PRIVATE_MOUSE_DATA *Private
  )
{
  EFI_STATUS                    Status;
  Status = EfiAcquireLockOrFail (&Private->SetupMouseLock);
  ASSERT_EFI_ERROR (Status);
}

VOID
ReleaseSetupMouseLock (
  PRIVATE_MOUSE_DATA *Private
  )
{
  EfiReleaseLock (&Private->SetupMouseLock);
}

