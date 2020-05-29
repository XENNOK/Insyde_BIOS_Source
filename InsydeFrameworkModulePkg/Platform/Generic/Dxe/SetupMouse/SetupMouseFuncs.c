//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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
//;   MouseFuncs.c
//;
//; Abstract:
//;
//;   Support functions for Setup Mouse Protocol
//;
//;
#include "SetupMouse.h"
#include "TianoHii.h"
#include EFI_PROTOCOL_DEFINITION (HiiFont)
#include EFI_PROTOCOL_DEFINITION (GifDecoder)

extern EFI_HII_FONT_PROTOCOL             *mHiiFont;
extern EFI_HII_ANIMATION_PACKAGE_HDR     *mAnimationPackage;
extern EFI_GIF_DECODER_PROTOCOL          *mGifDecoder;

EFI_STATUS
GetStartOffset (
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput,
  OUT     UINTN                          *OffsetX,
  OUT     UINTN                          *OffsetY
  )
/*++

Routine Description:

  Get setup mouse start offset.

Arguments:

  GraphicsOutput - Pointer to EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  OffsetX        - Pointer to setup mouse start x-axis.
  OffsetY        - Pointer to setup mouse start y-axis.

Returns:

  EFI_INVALID_PARAMETER - Some input parameters are invalid
  EFI_SUCCESS           - Get Setup mouse start offset successful.

--*/
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

  *OffsetX = ((GraphicsOutput->Mode->Info->HorizontalResolution - Columns * GLYPH_WIDTH)) >> 1;
  *OffsetY = ((GraphicsOutput->Mode->Info->VerticalResolution   - Rows * GLYPH_HEIGHT))   >> 1;

  return EFI_SUCCESS;
}

BOOLEAN
IsRectEmpty (
  IN CONST RECT *Rc
  )
/*++

Routine Description:

  Test rectangle is empty

Arguments:

  Rc            - Rectangle

Returns:

  TRUE          - Rectangle is empty
  FALSE         - Rectangle isn't empty

--*/
{
  ASSERT (Rc->right >= Rc->left);
  ASSERT (Rc->bottom >= Rc->top);
  return (BOOLEAN) (Rc->right == Rc->left || Rc->bottom == Rc->top);
}

VOID
SetRectEmpty (
  IN RECT *Rc
  )
/*++

Routine Description:

  Set rect to empty

Arguments:

  Rc            - Rectangle

Returns:

  N/A

--*/
{
  ASSERT (Rc);

  Rc->left = Rc->right = Rc->top = Rc->bottom = 0;
}

BOOLEAN
IntersectRect(
  OUT      RECT *DstRc,
  IN CONST RECT *Src1,
  IN CONST RECT *Src2
  )
/*++

Routine Description:

  Calcualte intersection range between two rectangle

Arguments:

  DstRc           - intersection rectangle
  Src1            - first intersection to check
  Src2            - second intersection to check

Returns:

  Does have a intersection rectangle

--*/
{
  if (IsRectEmpty(Src1) || IsRectEmpty(Src2) ||
      (Src1->left >= Src2->right) || (Src2->left >= Src1->right) ||
      (Src1->top >= Src2->bottom) || (Src2->top >= Src1->bottom)) {
    SetRectEmpty( DstRc );
    return FALSE;
  }
  DstRc->left   = EFI_MAX(Src1->left, Src2->left);
  DstRc->right  = EFI_MIN(Src1->right, Src2->right);
  DstRc->top    = EFI_MAX(Src1->top, Src2->top);
  DstRc->bottom = EFI_MIN(Src1->bottom, Src2->bottom);
  return TRUE;
}

BOOLEAN
OffsetRect(
  RECT *lprc,
  INT32 dx,
  INT32 dy
  )
{
  lprc->left += dx;
  lprc->right += dx;
  lprc->top += dy;
  lprc->bottom += dy;
  return TRUE;
}


VOID
SetRect(
  IN OUT RECT   *Rc,
  IN     INT32  Left,
  IN     INT32  Top,
  IN     INT32  Right,
  IN     INT32  Bottom
  )
/*++

Routine Description:

  Set rectangle

Arguments:

  Rc              - Rectangle
  Left            - Left position
  Top             - Top position
  Right           - Right position
  Bottom          - Bottom position

Returns:

  N/A

--*/
{
  Rc->left   = Left;
  Rc->top    = Top;
  Rc->right  = Right;
  Rc->bottom = Bottom;
}

BOOLEAN
PtInRect (
  IN RECT  *Rc,
  IN POINT Pt
  )
/*++

Routine Description:

  Point is in rectangle range

Arguments:

  Rc              - Test rectangle
  Pt              - Test point

Returns:

  Point is in rectangle range

--*/
{
  return (BOOLEAN) (Pt.x >= Rc->left && Pt.x < Rc->right &&
                    Pt.y >= Rc->top && Pt.y < Rc->bottom);
}


BOOLEAN
UnionRect(
  OUT      RECT *DstRc,
  IN CONST RECT *Src1,
  IN CONST RECT *Src2
  )
/*++

Routine Description:

  Calcualte union range between two rectangle

Arguments:

  DstRc           - intersection rectangle
  Src1            - first union rectangle
  Src2            - second union rectangle

Returns:

  Does have a union rectangle

--*/

{
  if (IsRectEmpty(Src1)) {
    if (IsRectEmpty(Src2)) {
      SetRectEmpty( DstRc );
      return FALSE;
    } else {
      *DstRc = *Src2;
    }
  } else {
    if (IsRectEmpty(Src2)) {
      *DstRc = *Src1;
    } else {
      DstRc->left   = EFI_MIN( Src1->left, Src2->left );
      DstRc->right  = EFI_MAX( Src1->right, Src2->right );
      DstRc->top    = EFI_MIN( Src1->top, Src2->top );
      DstRc->bottom = EFI_MAX( Src1->bottom, Src2->bottom );
    }
  }
  return TRUE;
}

VOID
InvalidateImage (
  IN PRIVATE_MOUSE_DATA                    *Private,
  IN IMAGE_INFO                           *ImageInfo
  )
/*++

Routine Description:

  Invalidate Image and update GOP invalidate region

Arguments:
  Private     - Setup mouse private data
  ImageInfo   - Image information

Returns:

  N/A

--*/

{

  GOP_ENTRY                   *GopEntry;
  UINTN                       Index;


  Private = mPrivate;
  if (!ImageInfo->Visible) {
    return ;
  }

  for (Index = 0; Index < Private->GopCount; Index++) {
    GopEntry = &Private->GopList[Index];
    UnionRect (&GopEntry->InvalidateRc, &GopEntry->InvalidateRc, &ImageInfo->ImageRc);
  }

}


VOID
HideImage (
  IN  IMAGE_INFO                           *ImageInfo
  )
/*++

Routine Description:

  Hide Image and update GOP invalidate region

Arguments:
  ImageInfo   - Image information

Returns:

  N/A

--*/
{
  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;

  if (!ImageInfo->Visible) {
    return ;
  }

  //
  // invalidate original image
  //
  InvalidateImage (Private, ImageInfo);
  ImageInfo->Visible = FALSE;
}


VOID
ShowImage (
  IN  IMAGE_INFO                           *ImageInfo
  )
/*++

Routine Description:

  Show Image and update GOP invalidate region

Arguments:
  ImageInfo   - Image information

Returns:

  N/A

--*/

{
  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;

  if (ImageInfo->Visible) {
    return ;
  }

  //
  // invalidate original image
  //
  ImageInfo->Visible = TRUE;
  InvalidateImage (Private, ImageInfo);
}



VOID
MoveImage (
  IN  IMAGE_INFO                           *ImageInfo,
  IN  UINTN                                X,
  IN  UINTN                                Y
  )
/*++

Routine Description:

  Move Image

Arguments:
  ImageInfo   - Pointer of image
  X           - Image x axis
  Y           - Image y axis

Returns:

  N/A

--*/
{

  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;

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

IMAGE_INFO *
CreateImage (
  UINTN Width,
  UINTN Height
  )
/*++

Routine Description:

  Create Image

Arguments:

  Width          - Image Width
  Height         - Image Height

Returns:

  Animation data

--*/
{
  IMAGE_INFO *ImageInfo;

  ImageInfo = EfiLibAllocateZeroPool (sizeof (IMAGE_INFO));
  if (ImageInfo == NULL) {
    return NULL;
  }

  ImageInfo->Image = EfiLibAllocateZeroPool (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);
  if (ImageInfo->Image == NULL) {
    gBS->FreePool (ImageInfo);
    return NULL;
  }

  return ImageInfo;
}

VOID
DestroyImage (
  IMAGE_INFO *ImageInfo
  )
/*++

Routine Description:

  Destroy image

Arguments:
  ImageInfo   - Image information

Returns:

  N/A

--*/
{
  if (ImageInfo->Image != NULL) {
    gBS->FreePool (ImageInfo->Image);
    ImageInfo->Image = NULL;
  }
}

UINT8 *
SetupMouseGetAnimation (
  IN UINT8             *AnimationBlock,
  IN EFI_ANIMATION_ID  AnimationId
  )
/*++

Routine Description:

  Get animation data by animation id

Arguments:

  AnimationBlock - Animation Package start pointer
  AnimationId    - Animation id

Returns:

  Animation data

--*/
{
  EFI_ANIMATION_ID              CurrentAnimationId;


  CurrentAnimationId = 1;
  while (*AnimationBlock != EFI_HII_AIBT_END) {
    if (CurrentAnimationId == AnimationId) {
        return AnimationBlock;
    }

    if (*AnimationBlock == EFI_HII_AIBT_EXT4) {
      AnimationBlock += ((EFI_HII_AIBT_EXT4_BLOCK *)AnimationBlock)->Length;
      CurrentAnimationId++;
    } else {
      //
      // not yet support type, so return not fonud
      //
      return NULL;
    }
  }

  return NULL;
}

UINTN
GetStringWidth (
  IN CHAR16                              *String
  )
/*++

Routine Description:

  Get string width

Arguments:

  String              - Process string

Returns:

  String width

--*/

{
  UINTN Index;
  UINTN Count;
  UINTN IncrementValue;

  Index           = 0;
  Count           = 0;
  IncrementValue  = 1;

  do {
    //
    // Advance to the null-terminator or to the first width directive
    //
    for (;
         (String[Index] != NARROW_CHAR) && (String[Index] != WIDE_CHAR) && (String[Index] != 0);
         Index++, Count = Count + IncrementValue
        )
      ;

    //
    // We hit the null-terminator, we now have a count
    //
    if (String[Index] == 0) {
      break;
    }
    //
    // We encountered a narrow directive - strip it from the size calculation since it doesn't get printed
    // and also set the flag that determines what we increment by.(if narrow, increment by 1, if wide increment by 2)
    //
    if (String[Index] == NARROW_CHAR) {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 1;
    } else {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 2;
    }
  } while (String[Index] != 0);

  return Count;
}


EFI_STATUS
SetupMouseDrawText (
  IN IMAGE_INFO                    *ImageInfo,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TextColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BackgroundColor,
  IN EFI_STRING                    String,
  IN RECT                          *TextRc
  )
/*++

Routine Description:

  Use HiiFont to draw text to blt buffer

Arguments:

  ImageInfo               - Destination Image
  TextColor         - Text Color
  BackgroundColor         - Background color
  String                  - Destination LineLen
  TextRc                  - Destination rectangle

Returns:

  return StringToImage status

--*/

{
  EFI_STATUS                        Status;
  EFI_IMAGE_OUTPUT                  Blt;
  EFI_IMAGE_OUTPUT                  *BltPtr;
  EFI_FONT_DISPLAY_INFO             FontInfo;
  UINTN                             OffsetX;
  UINTN                             OffsetY;
  UINTN                             StrWidth;


  if (mHiiFont == NULL) {
    return EFI_UNSUPPORTED;
  }

  BltPtr           = &Blt;


  EfiZeroMem (&Blt, sizeof (EFI_IMAGE_OUTPUT));
  EfiZeroMem (&FontInfo, sizeof (EFI_FONT_DISPLAY_INFO));

  Blt.Image.Bitmap = ImageInfo->Image;
  Blt.Width        = (UINT16) (ImageInfo->ImageRc.right - ImageInfo->ImageRc.left);
  Blt.Height       = (UINT16) (ImageInfo->ImageRc.bottom - ImageInfo->ImageRc.top);

  FontInfo.ForegroundColor   = *TextColor;
  FontInfo.BackgroundColor   = *BackgroundColor;
  FontInfo.FontInfo.FontSize = 19;

  //
  // text align: center
  //
  StrWidth = GetStringWidth (String) * GLYPH_WIDTH;
  if ((UINTN)(TextRc->right - TextRc->left) >= StrWidth) {
    OffsetX  = ((TextRc->right + TextRc->left) - StrWidth) / 2;
  } else {
    OffsetX = TextRc->left;
  }
  if ((UINTN)(TextRc->bottom - TextRc->top) >= GLYPH_HEIGHT) {
    OffsetY  = ((TextRc->bottom + TextRc->top) - GLYPH_HEIGHT) / 2;
  } else {
    OffsetY  = TextRc->top;
  }

  Status = mHiiFont->StringToImage (
                       mHiiFont,
                       EFI_HII_OUT_FLAG_CLIP | EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_TRANSPARENT,
                       String,
                       &FontInfo,
                       &BltPtr,
                       OffsetX,
                       OffsetY,
                       NULL,
                       NULL,
                       NULL
                       );

  return Status;
}

#define SIGN(x) (((x) > 0) ? 1 : (((x) == 0) ? 0 : -1))
#define ABS(x) (((x) >= 0) ? (x) : -(x))

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
/*++

Routine Description:

  Stretch image function from microwindows fblin32alpha function

Arguments:

  DstBlt                  - Destination Blt Buffer
  SrcBlt                  - Source Blt Buffer
  DstLineLen              - Destination LineLen
  SrcLineLen              - Source LineLen
  dest_x_start            - Destination X
  dest_y_start            - Destination Y
  width                   - Width
  height                  - Height
  x_denominator           - X denominator
  y_denominator           - Y denominator
  src_x_fraction          - Source X fraction
  src_y_fraction          - Source Y fraction
  x_step_fraction         - X step fraction
  y_step_fraction         - Y step fraction
  op                      - Directly src copy to dst

Returns:

  N/A

--*/
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
        *dest_ptr++ = *src_ptr;

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
/*++

Routine Description:

  Stretch image function from microwindows fblin32alpha function

Arguments:

  DstBlt                  - Destination Blt Buffer
  d1_x                    - Destination X1
  d1_y                    - Destination Y1
  d2_x                    - Destination X2
  d2_y                    - Destination Y2
  DstLineLen              - Destination LineLen
  SrcBlt                  - Source Blt Buffer
  s1_x                    - Source X1
  s1_y                    - Source Y1
  s2_x                    - Source X2
  s2_y                    - Source Y2
  SrcL                    - Source LineLen

Returns:

  N/A

--*/
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
/*++

Routine Description:

  BitBlt function from microwindows fblin32alpha function

Arguments:

  DstImage                - Destination Image information
  DstX                    - Destination X
  DstY                    - Destination Y
  Width                   - Width
  Height                  - Height
  SrcImage                - Destination LineLen
  SrcX                    - Source X
  SrcY                    - Source Y
  Rop                     - copy src to destinamtion or with transparent color

Returns:

  EFI_SUCCESS             - BitBlt is successful

--*/
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
/*++

Routine Description:

  StretchBlt function from microwindows fblin32alpha function

Arguments:

  DstX                    - Destination X
  DstY                    - Destination Y
  DstWidth                - Destination Width
  DstHeight               - Destination Height
  SrcBlt                  - Source Blt buffer
  SrcX                    - Source X
  SrcY                    - Source Y
  SrcWidth                - Source Width
  SrcHeight               - Source Height
  SrcDelta                - Source buffer LineLen
  Rop                     - directly copy source to destination or with transparent

Returns:

  EFI_SUCCESS             - BitBlt is successful

--*/
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
          ScreenPtr[X] = BltPtr[X];
        }
      }
      ScreenPtr += KeyboardWidth;
      SrcY ++;
      Height--;
    }
  }

  return EFI_SUCCESS;
}

VOID
SetupMouseDrawImage (
  IN RECT                          *DstRc,
  IN RECT                          *Scale9Grid,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageBlt,
  IN INT32                         ImageWidth,
  IN INT32                         ImageHeight,
  IN INT32                         ImageDelta
  )
/*++

Routine Description:
  Scale image by Scale9Grid algorithm

Arguments:
  DstRc      - Destination Rectangle
  Scale9Grid - scale 9 grid
  ImageBlt   - Image Blt buffer
  ImageBlt   - Image Width
  ImageBlt   - Image Height
  ImageBlt   - Image buffer linelen

Returns:
  None

--*/
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


EFI_STATUS
SetupMouseShowBitmap(
  RECT                          *DstRc,
  RECT                          *Scale9Grid,
  UINT16                        ImageId
  )
/*++

Routine Description:
  Scale image by Scale9Grid algorithm

Arguments:
  DstRc      - Destination Rectangle
  Scale9Grid - scale 9 grid
  ImageBlt   - Image Blt buffer
  ImageBlt   - Image Width
  ImageBlt   - Image Height
  ImageBlt   - Image buffer linelen

Returns:
  None

--*/
{
  EFI_HII_AIBT_EXT4_BLOCK *Ext4Block;
  INT32                   Width;
  INT32                   Height;
  ANIMATION               *Animation;
  EFI_STATUS              Status;
  IMAGE_INFO              BitmapImage;
  INT32                   KeyboardWidth;
  INT32                   KeyboardHeight;
  PRIVATE_MOUSE_DATA      *Private;

  Private = mPrivate;

  if (mAnimationPackage == NULL) {
    return EFI_NOT_READY;
  }

  if (mGifDecoder == NULL) {
    return EFI_NOT_READY;
  }

  Ext4Block = (EFI_HII_AIBT_EXT4_BLOCK *) SetupMouseGetAnimation (
    (UINT8 *)mAnimationPackage + mAnimationPackage->AnimationInfoOffset,
    (EFI_ANIMATION_ID) ImageId
    );

  if (Ext4Block == NULL) {
    return EFI_NOT_FOUND;
  }

  if (*(UINT8 *)Ext4Block != EFI_HII_AIBT_EXT4) {
    return EFI_NOT_FOUND;
  }

  Status = mGifDecoder->CreateAnimationFromMem (
                          mGifDecoder,
                          (UINT8 *)(Ext4Block + 1),
                          Ext4Block->Length,
                          (VOID *)(UINTN)ImageId,
                          &Animation
                          );

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }


  Width  = EFI_MIN ((DstRc->right - DstRc->left), (INT32)Animation->Width);
  Height = EFI_MIN ((DstRc->bottom - DstRc->top), (INT32)Animation->Height);

  KeyboardWidth  = Private->Keyboard.ImageRc.right - Private->Keyboard.ImageRc.left;
  KeyboardHeight = Private->Keyboard.ImageRc.bottom - Private->Keyboard.ImageRc.top;

  BitmapImage.Image    = Animation->Frames->Bitmap;
  SetRect (&BitmapImage.ImageRc, 0, 0, Animation->Frames->Width, Animation->Frames->Height);

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
      Animation->Frames->Bitmap,
      (INT32)Animation->Frames->Width,
      (INT32)Animation->Frames->Height,
      sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Animation->Frames->Width
      );
  }

  mGifDecoder->DestroyAnimation (mGifDecoder, Animation);
  return EFI_SUCCESS;
}

