/** @file

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

#include "LayoutSupportLib.h"

EFI_STATUS
BltCopy(
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Blt,
  IN     UINTN                           BltStartX,
  IN     UINTN                           BltStartY,
  IN     UINTN                           BltWidth,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *DestBlt,
  IN     UINTN                           DestStartX,
  IN     UINTN                           DestStartY,
  IN     UINTN                           DestWidth,
  IN     UINTN                           DestHeight,
  IN     UINTN                           CopyWidth,
  IN     UINTN                           CopyHeight
  )
{
  UINT32                                 *BltPtr;
  UINT32                                 *DestBltPtr;
  UINTN                                  X;
  UINTN                                  Y;

  if (Blt == NULL || DestBlt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BltWidth == 0 || DestWidth == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (CopyHeight == 0 || CopyWidth == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check CopyWidth and CopyHeight
  //
  CopyWidth  = MIN (CopyWidth,  DestWidth - DestStartX);
  CopyHeight = MIN (CopyHeight, DestHeight - DestStartY);

  for (Y = 0; Y < CopyHeight; Y ++) {
    DestBltPtr = (UINT32 *)(&DestBlt[(Y + DestStartY) * DestWidth + DestStartX]); 
    BltPtr = (UINT32 *)(&Blt[(Y + BltStartY) * BltWidth + BltStartX]);
    for (X = 0; X < CopyWidth; X ++) {
      *DestBltPtr = *BltPtr;
      DestBltPtr ++;
      BltPtr ++;
    }
  };

  return EFI_SUCCESS;
}

EFI_STATUS
BltCombine(
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **DestBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BgBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *FgBlt,
  IN     UINTN                           BltWidth,
  IN     UINTN                           BltHeight
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL          *ResultBlt;
  UINT32                                 *DestBltPtr;
  UINT32                                 *BgBltPtr;
  UINT32                                 *FgBltPtr;
  UINTN                                  X;
  UINTN                                  Y;

  if (DestBlt == NULL || BgBlt == NULL || FgBlt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BltWidth == 0 || BltHeight == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ResultBlt = AllocateZeroPool (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * BltWidth * BltHeight);
  if (ResultBlt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Y = 0; Y < BltHeight; Y ++) {
    DestBltPtr = (UINT32 *)(&ResultBlt[Y * BltWidth]); 
    BgBltPtr = (UINT32 *)(&BgBlt[Y * BltWidth]);
    FgBltPtr = (UINT32 *)(&FgBlt[Y * BltWidth]);
    for (X = 0; X < BltWidth; X ++) {
      *DestBltPtr = *BgBltPtr;
      if (*FgBltPtr != 0) {
        *DestBltPtr = *FgBltPtr;
      }
      DestBltPtr ++;
      BgBltPtr ++;
      FgBltPtr ++;
    }
  };

  if (*DestBlt != NULL) {
    SafeFreePool ((VOID **) DestBlt);
  }
  *DestBlt = ResultBlt;

  return EFI_SUCCESS;
}

EFI_STATUS
BltTransparent(
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **DestBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *SrcBlt,
  IN     UINTN                           BltWidth,
  IN     UINTN                           BltHeight
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL          *ResultBlt;
  UINT32                                 *DestBltPtr;
  UINT32                                 *SrcBltPtr;
  UINTN                                  X;
  UINTN                                  Y;

  if (DestBlt == NULL || SrcBlt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BltWidth == 0 || BltHeight == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ResultBlt = AllocateZeroPool (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * BltWidth * BltHeight);
  if (ResultBlt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Y = 0; Y < BltHeight; Y ++) {
    DestBltPtr = (UINT32 *)(&ResultBlt[Y * BltWidth]); 
    SrcBltPtr = (UINT32 *)(&SrcBlt[Y * BltWidth]);
    for (X = 0; X < BltWidth; X ++) {
      if (*SrcBltPtr == 0) {
        *DestBltPtr = 1;
      } else if ((*SrcBltPtr &= 0xffffff) == 0xffffff) {
        *DestBltPtr = 0;
      } else {
        *DestBltPtr = *SrcBltPtr;
      }
      DestBltPtr ++;
      SrcBltPtr ++;
    }
  };

  if (*DestBlt != NULL) {
    SafeFreePool ((VOID **) DestBlt);
  }
  *DestBlt = ResultBlt;

  return EFI_SUCCESS;
}

EFI_STATUS
CreateImageByColor (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Rgb,
  IN     RECT                                *Field,
  IN OUT EFI_IMAGE_INPUT                     *ImageInput
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL              *Bitmap;
  UINT32                                     Width;
  UINT32                                     Height;
  UINT32                                     BitmapSize;

  EFI_GRAPHICS_OUTPUT_BLT_PIXEL              *BltPtr;
  UINT32                                     X;
  UINT32                                     Y;

  Width = (UINT16)(Field->right - Field->left);
  Height = (UINT16)(Field->bottom - Field->top);
  BitmapSize = sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height;
  Bitmap = AllocateZeroPool (BitmapSize);
  if (Bitmap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Y = 0; Y < Height; Y++) {
    BltPtr = &Bitmap[Y * Width];
    for (X = 0; X < Width; X++) {
      CopyMem (BltPtr, Rgb, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      BltPtr ++;
    }
  }

  ImageInput->Width = (UINT16)Width;
  ImageInput->Height = (UINT16)Height;
  ImageInput->Bitmap = Bitmap;

  return EFI_SUCCESS;
}

EFI_STATUS
GetImageById (
  IN     UINT32                                ImageId,
  OUT    EFI_IMAGE_INPUT                       *ImageInput
  )
{
  EFI_STATUS                                   Status;

  EFI_HII_HANDLE                               HiiHandle;
  LAYOUT_DATABASE_PROTOCOL                     *LayoutDatabase;
  EFI_HII_IMAGE_PROTOCOL                       *HiiImage;


  if (ImageId == 0 || ImageInput == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiHiiImageProtocolGuid, NULL, (VOID **) &HiiImage);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiHandle = LayoutDatabase->ImagePkgHiiHandle;

  ZeroMem (ImageInput, sizeof (EFI_IMAGE_INPUT));
  Status = HiiImage->GetImage (
                       HiiImage,
                       HiiHandle,
                       (EFI_IMAGE_ID) ImageId,
                       ImageInput
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

