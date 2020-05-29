//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
//;   SetupMouse.c
//;
//; Abstract:
//;
//;   Setup Mouse Protocol implementation
//;
//;
#include "SetupMouse.h"

extern PRIVATE_MOUSE_DATA          *mPrivate;

EFI_STATUS
SetupMouseScreenBltWorker (
  IN  PRIVATE_MOUSE_DATA                 *Private,
  IN  GOP_ENTRY                          *GopEntry,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION  BltOperation,
  IN  UINTN                              SourceX,
  IN  UINTN                              SourceY,
  IN  UINTN                              DestinationX,
  IN  UINTN                              DestinationY,
  IN  UINTN                              Width,
  IN  UINTN                              Height,
  IN  UINTN                              Delta
  )
/*++

Routine Description:

  SetupMouseScreenBltWorker Blt function to process keyboard display

Arguments:

  Private       - Setup mouse private data
  GopEntry      - Gop Entry
  BltBuffer     - The data to transfer to screen
  BltOperation  - The operation to perform
  SourceX       - The X coordinate of the source for BltOperation
  SourceY       - The Y coordinate of the source for BltOperation
  DestinationX  - The X coordinate of the destination for BltOperation
  DestinationY  - The Y coordinate of the destination for BltOperation
  Width         - The width of a rectangle in the blt rectangle in pixels
  Height        - The height of a rectangle in the blt rectangle in pixels
  Delta         - Not used for EfiBltVideoFill and EfiBltVideoToVideo operation.
                  If a Delta of 0 is used, the entire BltBuffer will be operated on.
                  If a subrectangle of the BltBuffer is used, then Delta represents
                  the number of bytes in a row of the BltBuffer.

Returns:

  return Origianl BLT function status

--*/
{
  EFI_STATUS                      Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *Gop;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *ScreenFrameBuffer;
  EFI_TPL                         OriginalTPL;
  UINTN                           DstY;
  UINTN                           SrcY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *VScreen;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *VScreenSrc;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Blt;
  UINTN                           Index;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *FillPixel;
  UINT32                          VerticalResolution;
  UINT32                          HorizontalResolution;
  RECT                            InvalidateRc;

  if ((BltOperation < 0) || (BltOperation >= EfiGraphicsOutputBltOperationMax)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width == 0 || Height == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If Delta is zero, then the entire BltBuffer is being used, so Delta
  // is the number of bytes in each row of BltBuffer.  Since BltBuffer is Width pixels size,
  // the number of bytes in each row can be computed.
  //
  if (Delta == 0) {
    Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  }

  //
  // We need to fill the Virtual Screen buffer with the blt data.
  // The virtual screen is upside down, as the first row is the bootom row of
  // the image.
  //
  Gop                  = GopEntry->Gop;
  VerticalResolution   = Gop->Mode->Info->VerticalResolution;
  HorizontalResolution = Gop->Mode->Info->HorizontalResolution;
  ScreenFrameBuffer    = GopEntry->Screen.Image;

  if (BltOperation == EfiBltVideoToBltBuffer) {

    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    if (SourceY + Height > VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (SourceX + Width > HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // We have to raise to TPL Notify, so we make an atomic write the frame buffer.
    // We would not want a timer based event (Cursor, ...) to come in while we are
    // doing this operation.
    //
    OriginalTPL = gBS->RaiseTPL (EFI_TPL_NOTIFY);

    for (SrcY = SourceY, DstY = DestinationY; DstY < (Height + DestinationY); SrcY++, DstY++) {
      Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) ((UINT8 *) BltBuffer + (DstY * Delta) + DestinationX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      VScreen = &ScreenFrameBuffer[SrcY * HorizontalResolution + SourceX];
      EfiCopyMem (Blt, VScreen, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
    }

    gBS->RestoreTPL (OriginalTPL);
    return EFI_SUCCESS;
  }

  //
  // BltBuffer to Video: Source is BltBuffer, destination is Video
  //
  if (DestinationY + Height > VerticalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  if (DestinationX + Width > HorizontalResolution) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // We have to raise to TPL Notify, so we make an atomic write the frame buffer.
  // We would not want a timer based event (Cursor, ...) to come in while we are
  // doing this operation.
  //
  OriginalTPL = gBS->RaiseTPL (EFI_TPL_NOTIFY);

  if (BltOperation == EfiBltVideoFill) {
    FillPixel = BltBuffer;
    for (Index = 0; Index < Width; Index++) {
      GopEntry->FillLine[Index] = *FillPixel;
    }
  }

  for (Index = 0; Index < Height; Index++) {
    if (DestinationY <= SourceY) {
      SrcY  = SourceY + Index;
      DstY  = DestinationY + Index;
    } else {
      SrcY  = SourceY + Height - Index - 1;
      DstY  = DestinationY + Height - Index - 1;
    }

    VScreen = &ScreenFrameBuffer[DstY * HorizontalResolution + DestinationX];
    switch (BltOperation) {
    case EfiBltBufferToVideo:
      Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) ((UINT8 *) BltBuffer + (SrcY * Delta) + SourceX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      EfiCopyMem (VScreen, Blt, Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      break;

    case EfiBltVideoToVideo:
      VScreenSrc = &ScreenFrameBuffer[SrcY * HorizontalResolution + SourceX];
      EfiCopyMem (VScreen, VScreenSrc, Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      break;

    case EfiBltVideoFill:
      EfiCopyMem (VScreen, GopEntry->FillLine, Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      break;
    }
  }

  SetRect (
    &InvalidateRc,
    (INT32)DestinationX,
    (INT32)DestinationY,
    (INT32)(DestinationX + Width),
    (INT32)(DestinationY + Height)
    );
  UnionRect (&GopEntry->InvalidateRc, &GopEntry->InvalidateRc, &InvalidateRc);
  Status = RenderImage (Private, GopEntry, FALSE);

  gBS->RestoreTPL (OriginalTPL);

  return Status;
}



EFI_STATUS
SetupMouseScreenBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta         OPTIONAL
  )
/*++

Routine Description:

  SetupMouseCursorBlt Blt function to process keyboard display

Arguments:

  This          - Pointer to Graphics Output protocol instance
  BltBuffer     - The data to transfer to screen
  BltOperation  - The operation to perform
  SourceX       - The X coordinate of the source for BltOperation
  SourceY       - The Y coordinate of the source for BltOperation
  DestinationX  - The X coordinate of the destination for BltOperation
  DestinationY  - The Y coordinate of the destination for BltOperation
  Width         - The width of a rectangle in the blt rectangle in pixels
  Height        - The height of a rectangle in the blt rectangle in pixels
  Delta         - Not used for EfiBltVideoFill and EfiBltVideoToVideo operation.
                  If a Delta of 0 is used, the entire BltBuffer will be operated on.
                  If a subrectangle of the BltBuffer is used, then Delta represents
                  the number of bytes in a row of the BltBuffer.

Returns:

  return SetupMouseScreenBltWork function status

--*/
{
  PRIVATE_MOUSE_DATA *Private;
  UINTN              Index;

  Private   = mPrivate;

  for (Index = 0; Index < Private->GopCount; Index++) {
    if (Private->GopList[Index].Gop == This) {
      break;
    }
  }

  ASSERT (Index != Private->GopCount);
  if (Index == Private->GopCount) {
    return EFI_UNSUPPORTED;
  }

  return SetupMouseScreenBltWorker (
           Private,
           &Private->GopList[Index],
           BltBuffer,
           BltOperation,
           SourceX,
           SourceY,
           DestinationX,
           DestinationY,
           Width,
           Height,
           Delta
           );
}


VOID
SyncScreenImage (
  IN  PRIVATE_MOUSE_DATA  *Private,
  IN  GOP_ENTRY           *GopEntry,
  IN  RECT                *Rc,
  OUT BOOLEAN             *ImageIsSame
  )
/*++

  Routine Description:
    Copy image to blend image

  Arguments:
    Private               - Setup mouse private data
    GopEntry              - Gop Entry
    Rc                    - Sync rectangle
    ImageIsSame           - Screen Image is same as GOP

  Returns:
    N/A

--*/
{

  INT32                           X;
  INT32                           Y;
  INT32                           Width;
  INT32                           Height;
  UINT32                          *BlendBlt;
  UINT32                          *CheckBlt;
  UINT32                          *ScreenBlt;
  UINTN                           HorizontalResolution;

  ASSERT (ImageIsSame != NULL);

  HorizontalResolution = GopEntry->Gop->Mode->Info->HorizontalResolution;
  Width  = Rc->right - Rc->left;
  Height = Rc->bottom - Rc->top;

  //
  // read image from gop
  //
  GopEntry->OriginalBlt (
              GopEntry->Gop,
              Private->CheckBuffer,
              EfiBltVideoToBltBuffer,
              Rc->left,
              Rc->top,
              Rc->left,
              Rc->top,
              Width,
              Height,
              sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * HorizontalResolution
              );

  *ImageIsSame = TRUE;
  for (Y = Rc->top; Y < Rc->bottom; Y++) {
    BlendBlt  = (UINT32 *)&GopEntry->BlendBuffer[Y * HorizontalResolution + Rc->left];
    CheckBlt  = (UINT32 *)&Private->CheckBuffer[Y * HorizontalResolution + Rc->left];
    ScreenBlt = (UINT32 *)&GopEntry->Screen.Image[Y * HorizontalResolution + Rc->left];
    for (X = 0; X < Width; X++) {
      //
      // we need ignore alpha (reserved) channel, because some platform doesn't save alpha channel
      //
      if ((*BlendBlt & 0x00FFFFFF) != (*CheckBlt & 0x00FFFFFF)) {
        *ScreenBlt = *CheckBlt;
        *ImageIsSame = FALSE;
      }
      BlendBlt++;
      ScreenBlt++;
      CheckBlt++;
    }
  }
}

VOID
UpdateImage (
  IN PRIVATE_MOUSE_DATA   *Private,
  IN GOP_ENTRY            *GopEntry,
  IN IMAGE_INFO           *ImageInfo,
  IN RECT                 *UpdateRc,
  IN BOOLEAN              Transparent
  )
/*++

  Routine Description:
    Copy image to blend image

  Arguments:
    Private               - Setup mouse private data
    GopEntry              - Gop Entry
    ImageInfo             - Source image information
    UpdateRc              - Need update region
    Transparent           - Whether have transparent color

  Returns:
    N/A

--*/
{
  INT32                           X;
  INT32                           Y;
  INT32                           Width;
  INT32                           Height;
  INT32                           ImageWidth;
  INT32                           ImageHeight;
  UINT32                          *ImageBlt;
  UINT32                          *BlendBlt;
  RECT                            *ImageRc;
  UINTN                           HorizontalResolution;


  Width       = UpdateRc->right - UpdateRc->left;
  Height      = UpdateRc->bottom - UpdateRc->top;
  ImageWidth  = ImageInfo->ImageRc.right - ImageInfo->ImageRc.left;
  ImageHeight = ImageInfo->ImageRc.bottom - ImageInfo->ImageRc.top;

  ImageRc     = &ImageInfo->ImageRc;
  HorizontalResolution = GopEntry->Gop->Mode->Info->HorizontalResolution;

  ASSERT (Width  <= ImageWidth);
  ASSERT (Height <= ImageHeight);

  for (Y = UpdateRc->top; Y < UpdateRc->bottom; Y++) {
    ImageBlt = (UINT32*)&ImageInfo->Image[(Y - ImageRc->top) * ImageWidth + UpdateRc->left - ImageRc->left];
    BlendBlt = (UINT32*)&GopEntry->BlendBuffer[Y * HorizontalResolution + UpdateRc->left];
    if (!Transparent) {
      EfiCopyMem (BlendBlt, ImageBlt, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
    } else {
      for (X = 0; X < Width; X++) {
        if (*ImageBlt != 0) {
          *BlendBlt = *ImageBlt;
        }
        BlendBlt++;
        ImageBlt++;
      }
    }
  }
}

EFI_STATUS
RenderImage (
  IN PRIVATE_MOUSE_DATA   *Private,
  IN GOP_ENTRY            *GopEntry,
  IN BOOLEAN              NeedSyncScreen
  )
/*++

  Routine Description:
    Render Image to GOP

  Arguments:
    Private               - Setup mouse private data
    GopEntry              - Gop Entry
    NeedSyncScreen        - Whether need sync screen image

  Returns:
    N/A

--*/
{

  RECT        UpdateRc;
  RECT        InvalidateRc;
  BOOLEAN     ImageIsSame;
  EFI_STATUS  Status;

  EfiCopyMem (&InvalidateRc, &GopEntry->InvalidateRc, sizeof (RECT));
  IntersectRect (&InvalidateRc, &InvalidateRc, &GopEntry->Screen.ImageRc);

  if (IsRectEmpty (&InvalidateRc)) {
    return EFI_SUCCESS;
  }

  if (NeedSyncScreen) {
    SyncScreenImage (Private, GopEntry, &InvalidateRc, &ImageIsSame);
  }

  if (IntersectRect (&UpdateRc, &GopEntry->Screen.ImageRc, &InvalidateRc)) {
    UpdateImage (Private, GopEntry, &GopEntry->Screen, &UpdateRc, FALSE);
  }

  if (Private->Keyboard.Visible && IntersectRect (&UpdateRc, &Private->Keyboard.ImageRc, &InvalidateRc)) {
    UpdateImage (Private, GopEntry, &Private->Keyboard, &UpdateRc, TRUE);
  }

  if (Private->Cursor.Visible && IntersectRect (&UpdateRc, &Private->Cursor.ImageRc, &InvalidateRc)) {
    UpdateImage (Private, GopEntry, &Private->Cursor, &UpdateRc, TRUE);
  }

  Status = GopEntry->OriginalBlt (
                       GopEntry->Gop,
                       GopEntry->BlendBuffer,
                       EfiBltBufferToVideo,
                       InvalidateRc.left,
                       InvalidateRc.top,
                       InvalidateRc.left,
                       InvalidateRc.top,
                       InvalidateRc.right - InvalidateRc.left,
                       InvalidateRc.bottom - InvalidateRc.top,
                       sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * GopEntry->Gop->Mode->Info->HorizontalResolution
                       );

  SetRectEmpty (&GopEntry->InvalidateRc);

  return Status;
}

VOID
RenderImageForAllGop (
  IN PRIVATE_MOUSE_DATA   *Private
  )
/*++

  Routine Description:
    Render Image for All GOPs

  Arguments:
    Private               - Setup mouse private data

  Returns:
    N/A

--*/

{
  UINTN Index;

  for (Index = 0; Index < Private->GopCount; Index++) {
    RenderImage (Private, &Private->GopList[Index], TRUE);
  }
}

