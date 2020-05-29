/** @file
  This code supports a the private implementation
  of the GIF Decoder protocol

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

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Protocol/Cpu.h>
#include "Gif.h"
#include "GifDecoder.h"

UINT8      *mOutStack = NULL;
LZW_ENTRY  *mLzwTable = NULL;

GIF_DECODER_INSTANCE    mPrivateData = {
  GIF_DECODER_INSTANCE_SIGNATURE,
  NULL,
  {
   CreateAnimationFromMem,
   DestroyAnimation,
   NextAnimationFrame,
   RefreshAnimation
  }
};


/**
 Install Driver to produce Gif Decoder protocol.

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            Gif Decoder protocol installed
 @return Other        No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
GifDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiGifDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.GifDecoder
                  );
  return Status;
}


STATIC
VOID
FixColorMapForTransparent(
  UINT32                        SizeOfColorTable,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ColorMap
  )
{
  UINT32 Index;
  UINT32 MapSize;

  //
  // in 24bit mode, (0,0,0) is transparent; (0,0,1) drak red is black
  //
  MapSize = 1 << (SizeOfColorTable + 1);
  for (Index = 0; Index < MapSize; Index++) {
    if (ColorMap[Index].Blue == 0 &&
        ColorMap[Index].Green == 0 &&
        ColorMap[Index].Red == 0) {
       ColorMap[Index].Red = 1;
    }
  }
}



/**
 Calculate the frequency.

 @param        Frequency

 @retval EFI_SUCCESS            The frequency is calculated.
 @return other        Some error occurs.

**/
EFI_STATUS
GetFrequency (
  UINT64  *Frequency
  )
{
  *Frequency = GetPerformanceCounterProperties (NULL, NULL);
  *Frequency = DivU64x32 (*Frequency, 1000);

  return EFI_SUCCESS;
}

/**
 Create animation from file image buffer

 @param [in]   This             Pointer to protocol instance.
 @param [in]   FileData
 @param [in]   FileSize
 @param [in]   Data             Save private data information.
 @param [out]  Animation        Contain all decompress images which are BLT format in GIF image data.

 @retval EFI_SUCCESS            create success
 @retval EFI_INVALID_PARAMETER  invalid parameter
 @retval EFI_OUT_OF_RESOURCES   allocate memory fail
 @retval EFI_UNSUPPORTED        format error

**/
EFI_STATUS
EFIAPI
CreateAnimationFromMem (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      UINT8                        *FileData,
  IN      UINTN                        FileSize,
  IN      VOID                         *Data,
     OUT  ANIMATION                    **Animation
  )
{

  EFI_STATUS                    Status;
  ANIMATION                     *AnimationEntry;
  ANIMATION_FRAME               *AnimationFrame;
  GIF_LOGICAL_SCREEN_DESCRIPTOR GifScreen;
  GIF_IMAGE_DESCRIPTOR          ImageDesc;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ImageData;
  UINT32                        ImageSize;
  GIF_GRAPHIC_CONTROL_EXTENSION GraphicControl;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL GlobalColorMap[256];
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL LocalColorMap[256];
  UINT8                         *CompressedData;
  UINTN                         CompressedDataSize;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackupColor;
  ANIMATION_FRAME               *CurrentFrame;


  Status             = EFI_SUCCESS;
  CompressedData     = NULL;
  CompressedDataSize = 0;
  CurrentFrame       = NULL;
  ZeroMem (&BackupColor, sizeof (BackupColor));

  ASSERT (Animation != NULL);

  if (Animation == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mOutStack == NULL) {
    mOutStack = AllocateZeroPool (4100 * sizeof (UINT8));
  }

  if (mLzwTable == NULL) {
    mLzwTable = AllocateZeroPool (4096 * sizeof (LZW_ENTRY));
  }
  AnimationEntry = (ANIMATION *) AllocateZeroPool (sizeof (ANIMATION));
  if (AnimationEntry == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Err;
  }

  //
  // Verify Gif Header and Get Logical Screen Descriptor and Global Color Map
  //
  Status = GifDecoderGetLogicalScreen (
             &FileData,
             &FileSize,
             &GifScreen,
             GlobalColorMap
             );

  if (EFI_ERROR (Status)) {
    goto Err;
  }

  FixColorMapForTransparent (GifScreen.PackedFields.SizeOfGlobalColorTable, GlobalColorMap);

  AnimationEntry->Data    = Data;
  AnimationEntry->Width   = GifScreen.LogicalScreenWidth;
  AnimationEntry->Height  = GifScreen.LogicalScreenHeight;
  AnimationEntry->BkColor = GlobalColorMap[GifScreen.BackGroundColorIndex];

  //
  // default graphic control
  //
  GraphicControl.ExtensionIntroducer               = EXTENSION_INTRODUCER;
  GraphicControl.GraphicControlLabel               = GRAPHIC_CONTROL_LABEL;
  GraphicControl.BlockSize                         = 4;
  GraphicControl.PackedFields.DisposalMethod       = 0;
  GraphicControl.PackedFields.UserInputFlag        = 0;
  GraphicControl.PackedFields.TransparentColorFlag = 0;
  GraphicControl.DelayTime                         = 10;
  GraphicControl.TransparentColorIndex             = 0;

  for (;;) {
    //
    // Get single image descriptor
    //
    Status = GifDecoderGetImageDesc (
               &FileData,
               &FileSize,
               &ImageDesc,
               LocalColorMap,
               &GraphicControl
               );
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    if (EFI_ERROR (Status)) {
      goto Err;
    }


    if (ImageDesc.PackedFields.LocalColorTableFlag) {
      FixColorMapForTransparent (ImageDesc.PackedFields.SizeOfLocalColorTable, LocalColorMap);
    }

    //
    // transparent, set color to (0,0,0)
    //
    if (GraphicControl.PackedFields.TransparentColorFlag) {
      if (!ImageDesc.PackedFields.LocalColorTableFlag) {
        BackupColor = GlobalColorMap[GraphicControl.TransparentColorIndex];
        GlobalColorMap[GraphicControl.TransparentColorIndex].Blue  = 0;
        GlobalColorMap[GraphicControl.TransparentColorIndex].Green = 0;
        GlobalColorMap[GraphicControl.TransparentColorIndex].Red   = 0;
      } else {
        LocalColorMap[GraphicControl.TransparentColorIndex].Blue  = 0;
        LocalColorMap[GraphicControl.TransparentColorIndex].Green = 0;
        LocalColorMap[GraphicControl.TransparentColorIndex].Red   = 0;
      }
    }


    //
    // Get single image data
    //
    ImageSize = ImageDesc.ImageWidth * ImageDesc.ImageHeight * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    ImageData = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocatePool (ImageSize);
    if (ImageData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Err;
    }

    CompressedData = NULL;
    CompressedDataSize = 0;
    Status = GifDecoderGetImageData (
               &FileData,
               &FileSize,
               &ImageDesc,
               GlobalColorMap,
               LocalColorMap,
               &GraphicControl,
               ImageData,
               ImageSize,
               CompressedData,
               &CompressedDataSize
               );

    if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
      goto Err;
    }

    CompressedData = (UINT8 *)AllocatePool (CompressedDataSize);
    if (CompressedData == NULL) {
      goto Err;
    }

    Status = GifDecoderGetImageData (
               &FileData,
               &FileSize,
               &ImageDesc,
               GlobalColorMap,
               LocalColorMap,
               &GraphicControl,
               ImageData,
               ImageSize,
               CompressedData,
               &CompressedDataSize
               );
    if (EFI_ERROR (Status)) {
      goto Err;
    }
    gBS->FreePool (CompressedData);
    CompressedData = NULL;

    //
    // restore color map
    //
    if (GraphicControl.PackedFields.TransparentColorFlag) {
      if (!ImageDesc.PackedFields.LocalColorTableFlag) {
        GlobalColorMap[GraphicControl.TransparentColorIndex] = BackupColor;
      }
    }

    //
    // process animation frame
    //
    AnimationFrame = (ANIMATION_FRAME *) AllocateZeroPool (sizeof (ANIMATION_FRAME));
    if (AnimationFrame == NULL) {
      goto Err;
    }

    AnimationFrame->Bitmap      = ImageData;
    AnimationFrame->Disposal    = GraphicControl.PackedFields.DisposalMethod;
    AnimationFrame->OffsetX     = ImageDesc.ImageLeftPosition;
    AnimationFrame->OffsetY     = ImageDesc.ImageTopPosition;
    AnimationFrame->Width       = ImageDesc.ImageWidth;
    AnimationFrame->Height      = ImageDesc.ImageHeight;
    AnimationFrame->Delay       = GraphicControl.DelayTime;
    if (GraphicControl.PackedFields.TransparentColorFlag) {
      AnimationFrame->Transparent = TRUE;
    } else {
      AnimationFrame->Transparent = FALSE;
    }

    //
    // add frame to animation frame linked list
    //

    AnimationFrame->Next = NULL;
    if (AnimationEntry->Frames == NULL) {
      AnimationEntry->Frames  = AnimationFrame;
      AnimationFrame->Prev = NULL;
    } else if (CurrentFrame != NULL) {
      CurrentFrame->Next   = AnimationFrame;
      AnimationFrame->Prev = CurrentFrame;
    }
    CurrentFrame = AnimationFrame;
  }

  *Animation = AnimationEntry;
  return EFI_SUCCESS;

Err:
  if (AnimationEntry != NULL) {
    DestroyAnimation (This, AnimationEntry);
  }

  if (CompressedData != NULL) {
    gBS->FreePool (CompressedData);
  }

  return Status;
}


/**
 Draw one animation frame

 @param [in]   Animation        Animation contain all frame list
 @param [in]   Frame            Frame for draw
 @param [in]   Gop              Graphics output protocol interface
 @param [in]   BltWidth         Blt width limit
 @param [in]   BltHeight        Blt height limit
 @param [in]   BltX             Image start x
 @param [in]   BltY             Image start y

 @retval EFI_SUCCESS            draw frame success
 @return other         call blt fail

**/
STATIC
EFI_STATUS
DrawAnimationFrame (
  IN ANIMATION                     *Animation,
  IN ANIMATION_FRAME               *Frame,
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop,
  IN UINTN                         BltWidth,
  IN UINTN                         BltHeight,
  IN UINTN                         BltX,
  IN UINTN                         BltY
  )
{
  EFI_STATUS                        Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltBuf;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltPtr;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BitmapPtr;

  UINTN                             Width;
  UINTN                             Height;
  UINTN                             X;
  UINTN                             Y;

  ASSERT (Animation != NULL && Gop != NULL);

  if (Frame == NULL) {
    return EFI_SUCCESS;
  }

  //
  // out of display range, do nothing
  //
  if ((((UINT32)BltX + Frame->OffsetX) > BltWidth) ||
      (((UINT32)BltY + Frame->OffsetY) > BltHeight)) {
    return EFI_SUCCESS;
  }

  Width  = Frame->Width;
  Height = Frame->Height;

  if (Width > (BltWidth - BltX)) {
    Width = BltWidth - BltX;
  }

  if (Height > (BltHeight - BltY)) {
    Height = (BltHeight - BltY);
  }

  //
  // draw frame image
  //

  if (Frame->Transparent) {
    BltBuf = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocatePool (Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    Status = Gop->Blt (
                   Gop,
                   BltBuf,
                   EfiBltVideoToBltBuffer,
                   BltX + Frame->OffsetX,
                   BltY + Frame->OffsetY,
                   0,
                   0,
                   Width,
                   Height,
                   0
                   );

  } else {
    BltBuf = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocateZeroPool (Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  }

  ASSERT (BltBuf != NULL);
  if (BltBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Y = 0; Y < Height; Y++) {
    BltPtr    = &BltBuf[Y * Width];
    BitmapPtr = &Frame->Bitmap[Y * Frame->Width];
    for (X = 0; X < Width; X++) {
      if ((BitmapPtr->Blue != 0) || (BitmapPtr->Green != 0) || (BitmapPtr->Red != 0)) {
        *BltPtr = *BitmapPtr;
      }
      BltPtr++;
      BitmapPtr++;
    }
  }

  Status = Gop->Blt (
                 Gop,
                 BltBuf,
                 EfiBltBufferToVideo,
                 0,
                 0,
                 BltX + Frame->OffsetX,
                 BltY + Frame->OffsetY,
                 Width,
                 Height,
                 0
                 );

  gBS->FreePool (BltBuf);

  return EFI_SUCCESS;
}

/**
 Draw one animation frame for "Restore Previous Frame" disposal

 @param [in]   Animation        Animation contain all frame list
 @param [in]   Frame            Frame for draw
 @param [in]   Gop              Graphics output protocol interface
 @param [in]   BltWidth         Blt width limit
 @param [in]   BltHeight        Blt height limit
 @param [in]   BltX             Image start x
 @param [in]   BltY             Image start y

 @retval EFI_SUCCESS            draw frame success
 @return other         call blt fail

**/
STATIC
EFI_STATUS
RestorePrevFrame (
  IN ANIMATION                     *Animation,
  IN ANIMATION_FRAME               *Frame,
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop,
  IN UINTN                         BltWidth,
  IN UINTN                         BltHeight,
  IN UINTN                         BltX,
  IN UINTN                         BltY
  )
{

  UINT16                        Disposal;
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BkColor;
  UINTN                         Width;
  UINTN                         Height;
  UINTN                         X;
  UINTN                         Y;

  Disposal = 256;


  if (Frame != NULL) {
    //
    // out of display range, do nothing
    //
    if ((((UINT32)BltX + Frame->OffsetX) > BltWidth) ||
        (((UINT32)BltY + Frame->OffsetY) > BltHeight)) {
      return EFI_SUCCESS;
    }

    Disposal = Frame->Disposal;
    Width  = Frame->Width;
    Height = Frame->Height;
    X      = BltX + Frame->OffsetX;
    Y      = BltX + Frame->OffsetY;
  } else {
    Width  = Animation->Width;
    Height = Animation->Height;
    X      = BltX;
    Y      = BltY;
  }

  BkColor = Animation->BkColor;

  if (Width > (BltWidth - BltX)) {
    Width = BltWidth - BltX;
  }

  if (Height > (BltHeight - BltY)) {
    Height = (BltHeight - BltY);
  }

  switch (Disposal) {

  case   2: // background color
  case 256: // no frame
    Status = Gop->Blt (
                   Gop,
                   &BkColor,
                   EfiBltVideoFill,
                   0,
                   0,
                   X,
                   Y,
                   Width,
                   Height,
                   0
                   );
    if (Frame == NULL) {
      return EFI_SUCCESS;
    }
    break;

  case 3: // restore
    RestorePrevFrame (Animation, Frame->Prev, Gop, BltWidth, BltHeight, BltX, BltY);
    return EFI_SUCCESS;

  default:
    break;
  }

  DrawAnimationFrame (Animation, Frame, Gop, BltWidth, BltHeight, BltX, BltY);

  return EFI_SUCCESS;
}

/**
 Draw one animation frame by disposal
 case 0, 1: do nothing
 case    2: fill background color
 case    3: restore previous frame

 @param [in]   Animation        Animation contain all frame list
 @param [in]   Frame            Frame for draw
 @param [in]   Gop              Graphics output protocol interface
 @param [in]   BltWidth         Blt width limit
 @param [in]   BltHeight        Blt height limit
 @param [in]   BltX             Image start x
 @param [in]   BltY             Image start y

 @retval EFI_SUCCESS            draw frame success
 @return other         call blt fail

**/
STATIC
EFI_STATUS
DrawAnimationFrameDisposal (
  IN ANIMATION                     *Animation,
  IN ANIMATION_FRAME               *Frame,
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop,
  IN UINTN                         BltWidth,
  IN UINTN                         BltHeight,
  IN UINTN                         BltX,
  IN UINTN                         BltY
  )
{
  UINT16                        Disposal;
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BkColor;
  UINTN                         Width;
  UINTN                         Height;

  ASSERT (Animation->Frames != NULL);

  Disposal = 256;
  if (Frame != NULL) {
    Disposal = Frame->Disposal;
  } else {
    Frame = Animation->Frames;
  }

  BkColor = Animation->BkColor;

  //
  // out of display range, do nothing
  //
  if ((((UINT32)BltX + Frame->OffsetX) > BltWidth) ||
      (((UINT32)BltY + Frame->OffsetY) > BltHeight)) {
    return EFI_SUCCESS;
  }

  Width  = Frame->Width;
  Height = Frame->Height;
  if (Width > (BltWidth - BltX)) {
    Width = BltWidth - BltX;
  }

  if (Height > (BltHeight - BltY)) {
    Height = (BltHeight - BltY);
  }

  switch (Disposal) {

  case   2: // background color
  //case 256: // no frame
    Status = Gop->Blt (
                   Gop,
                   &BkColor,
                   EfiBltVideoFill,
                   0,
                   0,
                   BltX + Frame->OffsetX,
                   BltY + Frame->OffsetY,
                   Width,
                   Height,
                   0
                   );
    break;

  case 3: // restore
    RestorePrevFrame (Animation, Frame->Prev, Gop, BltWidth, BltHeight, BltX, BltY);
    break;

  default:
    break;
  }

  return EFI_SUCCESS;
}


/**
 Draw next animation frame

 @param [in]   This             Pointer to protocol instance.
 @param [in]   Entry            Animation Refresh Entry for animation information in screen
 @param [in]   Gop              Graphics output protocol interface

 @retval EFI_SUCCESS            draw frame success
 @retval EFI_NOT_FOUND          no next frame to draw

**/
EFI_STATUS
EFIAPI
NextAnimationFrame (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION_REFRESH_ENTRY      *Entry,
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
  )
{
  ASSERT (Entry != NULL && Entry->Animation != NULL &&
    Entry->Animation->Frames != NULL);

  if (Entry->Current == NULL) {
    Entry->Current = Entry->Animation->Frames;
  } else {
    if (Entry->Current->Next == NULL) {
      //
      // if autoloop is true, play animaiton from first image again,
      // else stop animation
      //
      if (Entry->AutoLoop) {
        Entry->Current = Entry->Animation->Frames;
      } else {
        Entry->Current = NULL;
        Entry->Status  = ANIM_STATUS_STOP;
        return EFI_NOT_FOUND;
      }
    } else {
      Entry->Current = Entry->Current->Next;
    }
  }

  if (Entry->Current != NULL) {
    // disposal
    DrawAnimationFrameDisposal (Entry->Animation, Entry->Current->Prev, Gop, Entry->BltWidth, Entry->BltHeight, Entry->X, Entry->Y);
    DrawAnimationFrame (Entry->Animation, Entry->Current, Gop, Entry->BltWidth, Entry->BltHeight, Entry->X, Entry->Y);
  }

  return EFI_SUCCESS;
}

/**
 By record tick and delay time to determinate draw image

 @param [in]   This             Pointer to protocol instance.
 @param [in]   AnimationRefrshEntry  Pointer to animation refresh entry
 @param [in]   Gop              Point to graphics output protocol
 @param [in]   CpuFrequency     Cpu Frequency for calculate tick elapsed time

 @retval EFI_SUCCESS            animation is playing
 @retval EFI_NOT_FOUND          animation is stopping

**/
EFI_STATUS
EFIAPI
RefreshAnimation (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION_REFRESH_ENTRY      *AnimationRefrshEntry,
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
  IN      UINT64                       CpuFrequency
  )
{
  UINT32                      DelayTime;
  UINT32                      ElapsedTime;
  UINT64                      CurrentTick;

  ASSERT (AnimationRefrshEntry != NULL);

  if (AnimationRefrshEntry->Status == ANIM_STATUS_PLAY) {

    if (CpuFrequency == 0) {
      GetFrequency (&CpuFrequency);
    }

    if (AnimationRefrshEntry->Current != NULL) {
      DelayTime = AnimationRefrshEntry->Current->Delay;
      DelayTime = (DelayTime < 3) ? 3 : DelayTime;
    } else {
      DelayTime = 0;
    }

    //
    // calculate elapsed time is 1/1000, but delay time is 1/100
    //
    CurrentTick = GetPerformanceCounter ();
    ElapsedTime = (UINT32) DivU64x32 (CurrentTick - AnimationRefrshEntry->RecordTick, (UINT32) (CpuFrequency));
    if (ElapsedTime > DelayTime * 10) {
      NextAnimationFrame (This, AnimationRefrshEntry, Gop);
      AnimationRefrshEntry->RecordTick = CurrentTick;
    }
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
 Draw next animation frame

 @param [in]   This             Pointer to protocol instance.
 @param [in]   Animation        Contain all decompress images which are BLT format in GIF image data.

 @retval EFI_SUCCESS            free memory success

**/
EFI_STATUS
EFIAPI
DestroyAnimation (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION                    *Animation
  )
{

  ANIMATION_FRAME *Next;
  ANIMATION_FRAME *Frame;

  ASSERT (Animation != NULL);

  Frame = Animation->Frames;
  while (Frame != NULL) {
    Next = Frame->Next;
    if (Frame->Bitmap != NULL) {
      gBS->FreePool (Frame->Bitmap);
    }
    gBS->FreePool (Frame);
    Frame = Next;
  };

  gBS->FreePool (Animation);

  return EFI_SUCCESS;
}



