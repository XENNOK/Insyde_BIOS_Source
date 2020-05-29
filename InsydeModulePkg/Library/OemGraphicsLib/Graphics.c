/** @file
  Support for Basic Graphics operations.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Library/OemGraphicsLib.h"
#include <IndustryStandard/Bmp.h>

#include <Protocol/JpegDecoder.h>
#include <Protocol/PcxDecoder.h>
#include <Protocol/TgaDecoder.h>
#include <Protocol/GifDecoder.h>
#include <Protocol/BootLogo.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/EndOfDisableQuietBoot.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/DynamicHotKey.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/VariableLib.h>

#include <Guid/Jpeg.h>
#include <Guid/ConsoleOutDevice.h>

#include <VesaBiosExtensions.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MediaPlayerLib.h>
//
//  Record the current VGA Mode from EFI code
//
#define EFI_CURRENT_VGA_MODE_ADDRESS                      0x4A3
//
//  Record the current VGA Mode from VGA OpRom
//
#define CURRENT_VGA_MODE_ADDRESS                          0x449

//
// BMP compression type.
//
#define BMP_UNCOMPRESSION_TYPE 0
#define BMP_RLE8BIT_TYPE       1

//
// RLE Mode.
//
#define RLE8_ENCODE_MODE_END_OF_LINE   0x00
#define RLE8_ENCODE_MODE_END_OF_BITMAP 0x01
#define RLE8_ENCODE_MODE_DELTA         0x02
#define RLE8_ABSOLUTE_MODE_FLAG        0x00

UINTN                                  mAutoPlaceStrDestY = (UINTN) (-1);
UINTN                                  mAutoPlaceStrCount;
BOOLEAN
IsModeSync (
  VOID
);


EFI_STATUS
BgrtDecodeImageToBlt (
  IN       UINT8                           *ImageData,
  IN       UINTN                           ImageSize,
  IN       EFI_BADGING_SUPPORT_FORMAT      ImageFormat,
  IN OUT   UINT8                           **Blt,
  IN OUT   UINTN                           *Width,
  IN OUT   UINTN                           *Height
  )
{
  EFI_STATUS                                    Status;
  UINTN                                         BltSize;
  EFI_JPEG_DECODER_PROTOCOL                     *JpegDecoder;
  EFI_PCX_DECODER_PROTOCOL                      *PcxDecoder;
  EFI_TGA_DECODER_PROTOCOL                      *TgaDecoder;
  EFI_GIF_DECODER_PROTOCOL                      *GifDecoder;
  BOOLEAN                                       HasAlphaChannel;
  TGA_FORMAT                                    TgaFormate;
  EFI_JPEG_DECODER_STATUS                       DecoderStatus;
  ANIMATION                                     *Animation;

  JpegDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, (VOID **)&JpegDecoder);

  PcxDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiPcxDecoderProtocolGuid, NULL, (VOID **)&PcxDecoder);

  TgaDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiTgaDecoderProtocolGuid, NULL, (VOID **)&TgaDecoder);

  GifDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, (VOID **)&GifDecoder);

  switch (ImageFormat) {

  case EfiBadgingSupportFormatBMP:
    Status = ConvertBmpToGopBlt (
               ImageData,
               ImageSize,
               (VOID *)Blt,
               &BltSize,
               Height,
               Width
               );
    break;

  case EfiBadgingSupportFormatTGA:
    if (TgaDecoder == NULL) {
      Status = EFI_UNSUPPORTED;
      break;
    }
    Status = TgaDecoder->DecodeImage (
                           TgaDecoder,
                           ImageData,
                           ImageSize,
                           Blt,
                           &BltSize,
                           Height,
                           Width,
                           &TgaFormate,
                           &HasAlphaChannel
                           );
    break;

  case EfiBadgingSupportFormatJPEG:
    if (JpegDecoder == NULL) {
      Status = EFI_UNSUPPORTED;
      break;
    }
    Status = JpegDecoder->DecodeImage (
                            JpegDecoder,
                            ImageData,
                            ImageSize,
                            Blt,
                            &BltSize,
                            Height,
                            Width,
                            &DecoderStatus
                            );
    break;

  case EfiBadgingSupportFormatPCX:
    if (PcxDecoder == NULL) {
      Status = EFI_UNSUPPORTED;
      break;
    }

    Status = PcxDecoder->DecodeImage (
                           PcxDecoder,
                           ImageData,
                           ImageSize,
                           Blt,
                           &BltSize,
                           Height,
                           Width
                           );
    break;

  case EfiBadgingSupportFormatGIF:
    if (GifDecoder == NULL) {
      Status = EFI_UNSUPPORTED;
      break;
    }
    //
    // BGRT only support the first image if .gif file.
    //
    Status = GifDecoder->CreateAnimationFromMem (
                           GifDecoder,
                           ImageData,
                           ImageSize,
                           NULL,
                           &Animation
                           );
    if (!EFI_ERROR (Status)) {
      *Width  = Animation->Width;
      *Height = Animation->Height;

      BltSize = Animation->Width * Animation->Height * sizeof (EFI_UGA_PIXEL);
      *Blt = AllocateZeroPool (BltSize);
      if (*Blt != NULL) {
        CopyMem (*Blt, Animation->Frames->Bitmap, BltSize);
        GifDecoder->DestroyAnimation (GifDecoder, Animation);
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
    }
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
BgrtGetImageByBadging (
  IN OUT  UINT8                          **ImageData,
  IN OUT  UINTN                          *ImageSize,
  IN OUT  EFI_BADGING_SUPPORT_FORMAT     *ImageFormat
  )
{
  EFI_STATUS                                    Status;
  UINT32                                        Instance;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         Attribute;
  UINTN                                         CoordinateX;
  UINTN                                         CoordinateY;
  EFI_BADGING_SUPPORT_IMAGE_TYPE                Type;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL              *Badging;

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Instance   = 0;
  Type       = EfiBadgingSupportImageBoot;

  Status = Badging->GetImage (
                      Badging,
                      &Instance,
                      &Type,
                      ImageFormat,
                      ImageData,
                      ImageSize,
                      &Attribute,
                      &CoordinateX,
                      &CoordinateY
                      );
  return Status;
}

VOID
BgrtGetLogoCoordinate (
  IN   UINTN      ResolutionX,
  IN   UINTN      ResolutionY,
  IN   UINTN      ImageWidth,
  IN   UINTN      ImageHeight,
  OUT  UINTN      *CoordinateX,
  OUT  UINTN      *CoordinateY
  )
{
  //
  // According to BGRT requirement, the logo must place at 38.2% from top of screen
  // and the width/height should be less than 40% of screen.
  //
  if ((ImageWidth >= ((ResolutionX * 40) / 100)) || (ImageHeight >= ((ResolutionY * 40) / 100))) {
    DEBUG ((EFI_D_ERROR, "Warning! The logo location did not meet BGRT requirement.\n"));
  }  //
  // Current setting: Image on the middle of the screen, and 38.2% from top.
  //
  *CoordinateX = (UINTN)(ResolutionX - ImageWidth) / 2;
  *CoordinateY = (UINTN)((ResolutionY * 382 / 1000) - (ImageHeight / 2));
}

VOID
EFIAPI
BgrtClearOemStringReadyToBootEvent (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  EFI_GRAPHICS_OUTPUT_PROTOCOL      *GraphicsOutput;
  UINTN                             StringCount;
  UINTN                             Index;
  EFI_STATUS                        Status;
  UINTN                             LocX;
  UINTN                             LocY;
  EFI_UGA_PIXEL                     Foreground;
  EFI_UGA_PIXEL                     Background;
  CHAR16                            *StringData;
  UINTN                             MinLocY;
  UINTN                             MaxLocY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Blt;
  BOOLEAN                           Found;

  MinLocY = 0xFFFFFFFF;
  MaxLocY = 0;
  Found   = FALSE;

  gBS->CloseEvent (Event);

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return;
  }

  Badging->GetStringCount (Badging, &StringCount);

  if (StringCount) {
    for (Index = 0; Index < StringCount; Index++) {
      if (Badging->GetOemString (Badging, Index, FALSE, 0, &StringData, &LocX, &LocY, &Foreground, &Background)) {
        Found = TRUE;
        gBS->FreePool (StringData);
        if (LocX == (UINTN) (-1) || LocY == (UINTN) (-1)) {
          continue;
        }
        if (MinLocY > LocY) {
          MinLocY = LocY;
        }
        if (LocY > MaxLocY) {
          MaxLocY = LocY;
        }
      }
    }

    if (!Found) {
      return;
    }

    SetMem (&Blt, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x00);
    //
    // First clear string which position is defined by OEM, then clear auto place strings.
    //
    if (MinLocY != 0xFFFFFFFF) {
      Status = GraphicsOutput->Blt (
                 GraphicsOutput,
                 &Blt,
                 EfiBltVideoFill,
                 0,
                 0,
                 (UINTN) 0,
                 (UINTN) MinLocY,
                 GraphicsOutput->Mode->Info->HorizontalResolution,
                 //
                 // GLYPH_HEIGHT is 19.
                 //
                 (MaxLocY - MinLocY + 20),
                 0
                 );
    }
    if (mAutoPlaceStrCount != 0 && mAutoPlaceStrDestY != (UINTN) (-1)) {
      Status = GraphicsOutput->Blt (
                                 GraphicsOutput,
                                 &Blt,
                                 EfiBltVideoFill,
                                 0,
                                 0,
                                 (UINTN) 0,
                                 (UINTN) mAutoPlaceStrDestY,
                                 GraphicsOutput->Mode->Info->HorizontalResolution,
                                 mAutoPlaceStrCount * EFI_GLYPH_HEIGHT,
                                 0
                                 );
      mAutoPlaceStrCount = 0;
      mAutoPlaceStrDestY = (UINTN) (-1);
    }
  }
  return;
}

EFI_STATUS
BgrtGetBootImageInfo (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
{
  EFI_STATUS                                    Status;
  UINT8                                         *Blt;
  UINT8                                         *ImageData;
  UINTN                                         ImageSize;
  UINTN                                         ResolutionX;
  UINTN                                         ResolutionY;
  UINTN                                         ImageWidth;
  UINTN                                         ImageHeight;
  EFI_BADGING_SUPPORT_FORMAT                    BadgingImageFormat;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *GraphicsOutput;

  Blt                = NULL;
  ImageData          = NULL;
  ImageSize          = 0;
  ImageWidth         = 0;
  ImageHeight        = 0;
  BadgingImageFormat = EfiBadgingSupportFormatUnknown;

  Status = BgrtGetImageByBadging (&ImageData, &ImageSize, &BadgingImageFormat);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResolutionX  = GraphicsOutput->Mode->Info->HorizontalResolution;
  ResolutionY  = GraphicsOutput->Mode->Info->VerticalResolution;

  Status = BgrtDecodeImageToBlt (
             ImageData,
             ImageSize,
             BadgingImageFormat,
             &Blt,
             &ImageWidth,
             &ImageHeight
             );
  if (!EFI_ERROR(Status)) {
    *BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)Blt;
    *Width     = ImageWidth;
    *Height    = ImageHeight;
  }

  BgrtGetLogoCoordinate (
    ResolutionX,
    ResolutionY,
    ImageWidth,
    ImageHeight,
    DestinationX,
    DestinationY
    );
  if (((*(INTN*)DestinationX) < 0) || ((*(INTN*)DestinationY) < 0)) {
    Status = EFI_UNSUPPORTED;
  } 

  if (ImageData != NULL) {
    gBS->FreePool (ImageData);
  }

  return Status;
}

EFI_STATUS
EFIAPI
BgrtEnableQuietBoot (
  EFI_BOOT_LOGO_PROTOCOL              *SetBootLogo
  )
{
  EFI_STATUS                           Status;
  EFI_EVENT                            ReadyToBootEvent;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  EFI_CONSOLE_CONTROL_PROTOCOL         *ConsoleControl;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL     *Badging;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuffer;
  UINTN                                DestinationX;
  UINTN                                DestinationY;
  UINTN                                Width;
  UINTN                                Height;
  EFI_STATUS                           ShowStringStatus;

  BltBuffer    = NULL;
  DestinationX = 0;
  DestinationY = 0;
  Width        = 0;
  Height       = 0;

  if (SetBootLogo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The procedure of BgrtEnableQuietBoot:
  //
  // 1. Get Boot image by OemBadgingSupport with Type == EfiBadgingSupportImageBoot.
  // 2. Update Boot image to BGRT table.
  // 3. Show logo. (According to the bgrt spec, the background always black)
  // 4. Show OEM String.
  // 5. Clear OEM String by ReadyToBoot Event.
  //
  Status = BgrtGetBootImageInfo (
             &BltBuffer,
             &DestinationX,
             &DestinationY,
             &Width,
             &Height
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  //
  // Play media file instead of logo.
  // If the feature (play media file) is not enabled, go back to show logo.
  //
  Status = PlayMediaFileDuringPost ();
  if (EFI_ERROR (Status)) {
    //
    // BIOS show logo.
    //
    Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
    if (!EFI_ERROR (Status)) {
      //
      // switch to the Graphical mode.
      //
      ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
    } else {
      gST->ConOut->ClearScreen (gST->ConOut);
    }

    //
    // Erase Cursor from screen
    //
    gST->ConOut->EnableCursor (gST->ConOut, FALSE);

    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = GraphicsOutput->Blt (
                               GraphicsOutput,
                               BltBuffer,
                               EfiBltBufferToVideo,
                               0,
                               0,
                               DestinationX,
                               DestinationY,
                               Width,
                               Height,
                               Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                               );
    ASSERT_EFI_ERROR (Status);
    Status = SetBootLogo->SetBootLogo (
                            SetBootLogo,
                            BltBuffer,
                            DestinationX,
                            DestinationY,
                            Width,
                            Height
                            );
    ASSERT_EFI_ERROR (Status);
  }
  
  //
  // Y vaule of auto place OEM badging string starts at y value of image bottom plus 2 glyph heigh.
  //
  mAutoPlaceStrDestY = DestinationY + Height + 2 * EFI_GLYPH_HEIGHT;
  //
  // show OemString.
  //
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ShowStringStatus = ShowOemString (Badging, FALSE, 0);

  if (EFI_ERROR (ShowStringStatus)) {
    if (BltBuffer != NULL) {
      gBS->FreePool (BltBuffer);
    }
    return Status;
  }
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             BgrtClearOemStringReadyToBootEvent,
             NULL,
             &ReadyToBootEvent
             );

  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (ReadyToBootEvent);
  }

  if (BltBuffer != NULL) {
    gBS->FreePool (BltBuffer);
  }

  return Status;
}

EFI_STATUS
RLE8ToBlt (
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
  UINT8                         *RLE8Image,
  BMP_COLOR_MAP                 *BmpColorMap,
  BMP_IMAGE_HEADER              *BmpHeader
  )
{
  UINT8                         FirstByte;
  UINT8                         SecondByte;
  UINT8                         EachValue;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  BOOLEAN                       EndOfLine;
  BOOLEAN                       EndOfBMP;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;

  EndOfLine = FALSE;
  EndOfBMP = FALSE;
  Height = 0;

  while (Height <= BmpHeader->PixelHeight && EndOfBMP == FALSE) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    Width = 0;
    EndOfLine = FALSE;

    while (Width <= BmpHeader->PixelWidth && EndOfLine == FALSE) {
      FirstByte = *RLE8Image;
      RLE8Image++;
      SecondByte = *RLE8Image;
      RLE8Image++;

      if (FirstByte > RLE8_ABSOLUTE_MODE_FLAG) {
        for (Index = 0; Index < FirstByte; Index++, Width++, Blt++) {
          Blt->Red    = BmpColorMap[SecondByte].Red;
          Blt->Green  = BmpColorMap[SecondByte].Green;
          Blt->Blue   = BmpColorMap[SecondByte].Blue;
        }
      } else {
        switch (SecondByte) {

        case RLE8_ENCODE_MODE_END_OF_LINE:
          EndOfLine = TRUE;
          break;

        case RLE8_ENCODE_MODE_END_OF_BITMAP:
          EndOfBMP = TRUE;
          EndOfLine = TRUE;
          break;

        case RLE8_ENCODE_MODE_DELTA:
          FirstByte = *RLE8Image;
          RLE8Image++;
          SecondByte= *RLE8Image;
          RLE8Image++;
          Width = Width + FirstByte;
          Height = Height + SecondByte;
          Blt = &BltBuffer[((BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth) + (Width)];
          break;

        default:
          //
          // Absolute mode.
          //
          for (Index = 0; Index < SecondByte; Index++, Width++, Blt++) {
            EachValue = *RLE8Image;
            RLE8Image++;
            Blt->Red    = BmpColorMap[EachValue].Red;
            Blt->Green  = BmpColorMap[EachValue].Green;
            Blt->Blue   = BmpColorMap[EachValue].Blue;
          }
          //
          // align on word boundary
          //
          if (Index % 2 == 1)
            RLE8Image++;
          break;
        }
      }
    }
    Height++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
LockKeyboards (
  IN  CHAR16    *Password
  )
{
  EFI_STATUS                    Status;
  EFI_CONSOLE_CONTROL_PROTOCOL  *ConsoleControl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = ConsoleControl->LockStdIn (ConsoleControl, Password);
  return Status;
}

STATIC
EFI_STATUS
DiaplayMessage(
  IN     UINTN          PosX,
  IN     UINTN          PosY,
  IN     EFI_UGA_PIXEL  ForegroundRGB,
  IN     EFI_UGA_PIXEL  BackgroundRGB,
  IN     CHAR16         *TmpStr
  )
{
  EFI_STATUS                    Status;
  EFI_UGA_DRAW_PROTOCOL         *UgaDraw;
  UINT32                        SizeOfX;
  UINT32                        SizeOfY;
  UINT32                        ColorDepth;
  UINT32                        RefreshRate;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;

  UgaDraw = NULL;
  GraphicsOutput = NULL;
  CopyMem (&Foreground, &ForegroundRGB, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&Background, &BackgroundRGB, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    (VOID **)&UgaDraw
                    );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  if (GraphicsOutput == NULL && UgaDraw == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
  } else {
    Status = UgaDraw->GetMode (
                        UgaDraw,
                        &SizeOfX,
                        &SizeOfY,
                        &ColorDepth,
                        &RefreshRate
                        );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  //
  // If X or Y value of OEM badging string is -1, system will auto place this string.
  //
  if ((PosX == (UINTN) (-1) || PosY == (UINTN) (-1))) {
    if (mAutoPlaceStrDestY == (UINTN) (-1)) {
      return EFI_UNSUPPORTED;
    }
    PosX = (SizeOfX - StrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2;
    PosY = mAutoPlaceStrDestY + mAutoPlaceStrCount * EFI_GLYPH_HEIGHT;
    mAutoPlaceStrCount++;
  }

  if (PosX > SizeOfX || PosY > SizeOfY) {
    ASSERT (0);
    return EFI_UNSUPPORTED;
  }

  PrintXY (
    PosX,
    PosY,
    &Foreground,
    &Background,
    TmpStr
    );
  return EFI_SUCCESS;
}

/**
  This function uses to check GOP hardware device is whether ready.

  @retval TRUE        GOP hardware device is ready.
  @retval FALSE       GOP hardware device isn't ready.

**/
STATIC
BOOLEAN
IsGopReady (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = GraphicsOutput->QueryMode (
                             GraphicsOutput,
                             GraphicsOutput->Mode->Mode,
                             &SizeOfInfo,
                             &Info
                             );
  if (!EFI_ERROR (Status)) {
    gBS->FreePool (Info);
    return TRUE;
  }
  return FALSE;
}


EFI_STATUS
ShowOemString (
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL      *Badging,
  IN BOOLEAN                               AfterSelect,
  IN UINT8                                 SelectedStringNum
  )
{
  EFI_STATUS                     Status;
  UINTN                          LocX;
  UINTN                          LocY;
  EFI_UGA_PIXEL                  Foreground;
  EFI_UGA_PIXEL                  Background;
  CHAR16                         *StringData;
  UINTN                          StringCount;
  UINTN                          Index;

  if (!IsGopReady ()) {
    return EFI_NOT_STARTED;
  }

  LocX = 0;
  LocY = 0;
  StringCount = 0;
  Index = 0;
  
  Status = EFI_SUCCESS;
  if (FeaturePcdGet (PcdDynamicHotKeySupported)) {    
    DYNAMIC_HOTKEY_PROTOCOL        *DynamicHotKey;
	
	DynamicHotKey = NULL;
    Status = gBS->LocateProtocol (&gDynamicHotKeyProtocolGuid, NULL, (VOID **)&DynamicHotKey);
    if (!EFI_ERROR (Status)) {
      //
      //  Get POST string count
      //
      Status = DynamicHotKey->GetDynamicStringCount (DynamicHotKey, AfterSelect, &StringCount);
      if (!EFI_ERROR (Status)) {
        for (Index = 0 ; Index < StringCount ; Index++) {
          //
          //  Get POST string
          //
          Status = DynamicHotKey->GetDynamicString (DynamicHotKey, Index, AfterSelect, &StringData, &LocX, &LocY);
          if (!EFI_ERROR (Status)) {
            //
            //  Get POST string foreground and background color
            //
            DynamicHotKey->GetDynamicStringColor (Index, AfterSelect, &Foreground, &Background);
            DynamicHotKey->AdjustStringPosition (DynamicHotKey, FALSE, TRUE, mAutoPlaceStrDestY, StringData, &LocX, &LocY);
            DiaplayMessage (LocX, LocY, Foreground, Background, StringData);
            gBS->FreePool (StringData);
          }
        }
        DynamicHotKey->AdjustStringPosition (DynamicHotKey, TRUE, TRUE, 0, NULL, NULL, NULL);
      }
    }  
  } else {
    ZeroMem (&Foreground, sizeof (EFI_UGA_PIXEL));
    Status = Badging->GetStringCount(Badging, &StringCount);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    mAutoPlaceStrCount = 0;
    Status = EFI_UNSUPPORTED;

    for ( Index = 0 ; Index < StringCount ; Index++ ) {
      if (Badging->GetOemString(Badging, Index, AfterSelect, SelectedStringNum, &StringData, &LocX, &LocY, &Foreground, &Background) ) {
        if (TextOnlyConsole ()) { 
          DisplayMessageToConsoleRedirection (StringData);
        }
        DiaplayMessage (
                  LocX,
                  LocY,
                  Foreground,
                  Background,
                  StringData
                  );
        gBS->FreePool (StringData);
        Status = EFI_SUCCESS;
      }
    }
  }

  return Status;
}

VOID
SetModeByGraphicOutput (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                        SizeOfX,
  IN  UINT32                        SizeOfY
  )
{
  UINT32                                Index;
  UINT32                                OriginalMode;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT8                                 *EFIBdaVGAMode;
  UINT8                                 *BdaVGAMode;
  EFI_STATUS                            Status;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;

  EFIBdaVGAMode = (UINT8 *)(UINTN)EFI_CURRENT_VGA_MODE_ADDRESS;
  BdaVGAMode    = (UINT8 *)(UINTN)CURRENT_VGA_MODE_ADDRESS;

  OriginalMode = GraphicsOutput->Mode->Mode;
  GraphicsOutput->QueryMode (GraphicsOutput, OriginalMode, &SizeOfInfo, &Info);

  if (Info->HorizontalResolution != SizeOfX || Info->VerticalResolution != SizeOfY || !IsModeSync ()) {

    Status = EFI_SUCCESS;
    for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
      GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
      if (SizeOfX == Info->HorizontalResolution &&
          SizeOfY == Info->VerticalResolution) {
          Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
          gBS->FreePool (Info);
          break;
      }
      gBS->FreePool (Info);
    }

    //
    // Cannot find suitable GOP mode, set mode with default resolution.
    //
    if (EFI_ERROR (Status) || (Index == GraphicsOutput->Mode->MaxMode)) {
      for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
        GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (Info->HorizontalResolution == PcdGet32 (PcdDefaultHorizontalResolution) &&
            Info->VerticalResolution == PcdGet32 (PcdDefaultVerticalResolution)) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
            gBS->FreePool (Info);
            break;
        }
        gBS->FreePool (Info);
      }
    }

    if (EFI_ERROR (Status) || (Index == GraphicsOutput->Mode->MaxMode)) {
      for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
        Status = GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (Info->HorizontalResolution == 640 &&
            Info->VerticalResolution == 480) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
            break;
        }
      }
    }

    ASSERT_EFI_ERROR (Status);

  } else {
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
    if (!EFI_ERROR (Status)) {
      if (*EFIBdaVGAMode != *BdaVGAMode) {
        GraphicsOutput->SetMode (GraphicsOutput, OriginalMode);
      }
    }
  }
}

EFI_STATUS
OnEndOfDisableQuietBoot (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEndOfDisableQuietBootGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  return EFI_SUCCESS;
}

/**
  Consume by Bgrt driver to get the logo infomation.
  In some platform won't called EnableQuietBoot to update the Bgrt logo info,
  if the logo info won't update when ReadyToBoot event trigger, the Bgrt driver will
  get the logo info automatically by this function.

  @retval EFI_SUCCESS              Get boot logo info success.

**/
EFI_STATUS
LogoLibSetBootLogo (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_LOGO_PROTOCOL         *BootLogoProtocol;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BltBuffer = NULL;
  UINTN                          DestinationX = 0;
  UINTN                          DestinationY = 0;
  UINTN                          Width = 0;
  UINTN                          Height = 0;

  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **)&BootLogoProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = BgrtGetBootImageInfo (
             &BltBuffer,
             &DestinationX,
             &DestinationY,
             &Width,
             &Height
             );
  if (!EFI_ERROR(Status)) {
    Status = BootLogoProtocol->SetBootLogo (
                                 BootLogoProtocol,
                                 BltBuffer,
                                 DestinationX,
                                 DestinationY,
                                 Width,
                                 Height
                                 );
    if (BltBuffer != NULL) {
      gBS->FreePool (BltBuffer);
    }
  }

  return Status;
}

/**
  Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  BmpImageSize  Number of bytes in BmpImage
  @param  GopBlt        Buffer containing GOP version of BmpImage.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  PixelHeight   Height of GopBlt/BmpImage in pixels
  @param  PixelWidth    Width of GopBlt/BmpImage in pixels

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *PixelHeight,
     OUT UINTN     *PixelWidth
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         ImageIndex;
  UINT32                        DataSizePerLine;
  BOOLEAN                       IsAllocated;
  UINT32                        ColorMapNum;

  if (sizeof (BMP_IMAGE_HEADER) > BmpImageSize) {
    return EFI_INVALID_PARAMETER;
  }

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;

  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  if (BmpHeader->CompressionType != BMP_UNCOMPRESSION_TYPE && BmpHeader->CompressionType != BMP_RLE8BIT_TYPE) {
    return EFI_UNSUPPORTED;
  }

  //
  // Only support BITMAPINFOHEADER format.
  // BITMAPFILEHEADER + BITMAPINFOHEADER = BMP_IMAGE_HEADER
  //
  if (BmpHeader->HeaderSize != sizeof (BMP_IMAGE_HEADER) - OFFSET_OF(BMP_IMAGE_HEADER, HeaderSize)) {
    return EFI_UNSUPPORTED;
  }

  //
  // The data size in each line must be 4 byte alignment.
  //
  DataSizePerLine = ((BmpHeader->PixelWidth * BmpHeader->BitPerPixel + 31) >> 3) & (~0x3);
  BltBufferSize = MultU64x32 (DataSizePerLine, BmpHeader->PixelHeight);
  if (BltBufferSize > (UINT32) ~0) {
    return EFI_INVALID_PARAMETER;
  }

//  if ((BmpHeader->Size != BmpImageSize) ||
//      (BmpHeader->Size < BmpHeader->ImageOffset) ||
//      (BmpHeader->Size - BmpHeader->ImageOffset !=  BmpHeader->PixelHeight * DataSizePerLine)) {
//    return EFI_INVALID_PARAMETER;
//  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));
  if (BmpHeader->ImageOffset < sizeof (BMP_IMAGE_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  ColorMapNum = 0;
  if (BmpHeader->ImageOffset > sizeof (BMP_IMAGE_HEADER)) {
    switch (BmpHeader->BitPerPixel) {

    case 1:
      ColorMapNum = 2;
      break;
    case 4:
      ColorMapNum = 16;
      break;
    case 8:
      ColorMapNum = 256;
      break;
    default:
      ColorMapNum = 0;
      break;
    }

    if (BmpHeader->NumberOfColors != 0) {
      ColorMapNum = BmpHeader->NumberOfColors;
    }

    if (BmpHeader->ImageOffset - sizeof (BMP_IMAGE_HEADER) != sizeof (BMP_COLOR_MAP) * ColorMapNum) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  //
  // Calculate the BltBuffer needed size.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->PixelHeight);
  //
  // Ensure the BltBufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BltBufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }
  BltBufferSize = MultU64x32 (BltBufferSize, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = AllocatePool (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    if (*GopBltSize < (UINTN) BltBufferSize) {
      *GopBltSize = (UINTN) BltBufferSize;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *PixelWidth   = BmpHeader->PixelWidth;
  *PixelHeight  = BmpHeader->PixelHeight;

  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = *GopBlt;
  if (BmpHeader->CompressionType == BMP_RLE8BIT_TYPE) {
    //
    // Background color is black.
    //
    gBS->SetMem (BltBuffer, (UINTN)BltBufferSize, 0x00);
    RLE8ToBlt (BltBuffer, Image, BmpColorMap, BmpHeader);
    return EFI_SUCCESS;
  }

  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {
      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          if ((UINT32) (((*Image) >> (7 - Index)) & 0x1) >= ColorMapNum) {
            return EFI_INVALID_PARAMETER;
          }
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt--;
        Width--;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        if ((UINT32) Index >= ColorMapNum) {
          return EFI_INVALID_PARAMETER;
        }
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          if ((UINT32) Index >= ColorMapNum) {
            return EFI_INVALID_PARAMETER;
          }
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        if ((UINT32) *Image >= ColorMapNum) {
          return EFI_INVALID_PARAMETER;
        }
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        //
        // It is 24-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      case 32:
        //
        // It is 32-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image++;
        break;

      default:
        //
        // Other bit format BMP is not supported.
        //
        if (IsAllocated) {
          gBS->FreePool (*GopBlt);
          *GopBlt = NULL;
        }
        return EFI_UNSUPPORTED;
        break;
      };

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;
}

/**
  Use SystemTable Conout to stop video based Simple Text Out consoles from going
  to the video device. Put up LogoFile on every video device that is a console.

  @param[in]  LogoFile   File name of logo to display on the center of the screen.

  @retval EFI_SUCCESS     ConsoleControl has been flipped to graphics and logo displayed.
  @retval EFI_UNSUPPORTED Logo not found

**/
EFI_STATUS
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
  )
{
  EFI_STATUS                                    Status;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL              *Badging;
  UINT32                                        SizeOfX;
  UINT32                                        SizeOfY;
  INTN                                          DestX;
  INTN                                          DestY;
  UINT8                                         *ImageData;
  UINTN                                         ImageSize;
  UINTN                                         BltSize;
  UINT32                                        Instance;
  EFI_BADGING_SUPPORT_FORMAT                    Format;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         Attribute;
  UINTN                                         CoordinateX;
  UINTN                                         CoordinateY;
  UINTN                                         Height;
  UINTN                                         Width;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                 *Blt;
  EFI_UGA_DRAW_PROTOCOL                         *UgaDraw;
  UINT32                                        ColorDepth;
  UINT32                                        RefreshRate;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *GraphicsOutput;
  EFI_BADGING_SUPPORT_IMAGE_TYPE                Type;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         BadgeAttribute;
  UINTN                                         BadgeCoordinateX;
  UINTN                                         BadgeCoordinateY;
  BOOLEAN                                       OverrideBadgeLocation;
  EFI_JPEG_DECODER_PROTOCOL                     *JpegDecoder;
  EFI_PCX_DECODER_PROTOCOL                      *PcxDecoder;
  EFI_JPEG_DECODER_STATUS                       DecoderStatus;
  JPG_BADGE_COMMENT                             *pJpegBadgeComment;
  UINTN                                         Index;
  UINTN                                         LocX;
  UINTN                                         LocY;
  UINT8                                         *VideoBuffer;
  TGA_FORMAT                                    TgaFormate;
  EFI_TGA_DECODER_PROTOCOL                      *TgaDecoder;
  BOOLEAN                                       HasAlphaChannel;
  EFI_GIF_DECODER_PROTOCOL                      *GifDecoder;
  ANIMATION                                     *Animation;
  ANIMATION_REFRESH_ENTRY                       AnimationRefreshEntry;
  EFI_BOOT_LOGO_PROTOCOL                        *SetBootLogo;
  EFI_CONSOLE_CONTROL_PROTOCOL                  *ConsoleControl;
  EFI_CONSOLE_CONTROL_SCREEN_MODE               ScreenMode;


  SetBootLogo = NULL;
  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **)&SetBootLogo);
  if (!EFI_ERROR(Status)) {
    //
    // Bgrt feature supported, it has the different showing logo policy.
    //
    Status = BgrtEnableQuietBoot (SetBootLogo);
    if (!EFI_ERROR(Status)) {
      return Status;
    }
  }

  UgaDraw = NULL;
  ColorDepth = 0;
  RefreshRate = 0;
  //
  // Try to open GOP first
  //
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdUgaConsumeSupport)) {
    GraphicsOutput = NULL;
    //
    // Open GOP failed, try to open UGA
    //
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, (VOID **) &UgaDraw);
  }
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Badging = NULL;
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);

  JpegDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, (VOID **)&JpegDecoder);

  PcxDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiPcxDecoderProtocolGuid, NULL, (VOID **)&PcxDecoder);

  TgaDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiTgaDecoderProtocolGuid, NULL, (VOID **)&TgaDecoder);

  GifDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, (VOID **)&GifDecoder);

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (!EFI_ERROR (Status)) {
    ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
  } else {
    ConsoleControl = NULL;
    gST->ConOut->ClearScreen (gST->ConOut);
  }

  //
  // Erase Cursor from screen
  //
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;

  } else if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
    Status = UgaDraw->GetMode (UgaDraw, &SizeOfX, &SizeOfY, &ColorDepth, &RefreshRate);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  //
  // Initialize the Badge location and attributes
  //
  BadgeAttribute = EfiBadgingSupportDisplayAttributeCustomized;
  BadgeCoordinateX = 0;
  BadgeCoordinateY = 0;
  OverrideBadgeLocation = FALSE;
  Height = 0;
  Width = 0;

  //
  // Set the request for the first image of type Logo
  //
  Instance = 0;
  Type = EfiBadgingSupportImageLogo;
  Format = EfiBadgingSupportFormatBMP;

  while (1) {
    ImageData = NULL;
    ImageSize = 0;

    if (Badging != NULL) {
      Status = Badging->GetImage (
                          Badging,
                          &Instance,
                          &Type,
                          &Format,
                          &ImageData,
                          &ImageSize,
                          &Attribute,
                          &CoordinateX,
                          &CoordinateY
                          );
      if (EFI_ERROR (Status)) {
        //
        // After the final logo, reset the instance counter and switch to type Badge
        // After the final badge exit the function
        //
        if (Type == EfiBadgingSupportImageLogo) {
          Type = EfiBadgingSupportImageBadge;
          Instance = 0;
          continue;
        }

        return Status;
      }

      switch (Type) {

      case EfiBadgingSupportImageLogo:
        switch (Format) {

        case EfiBadgingSupportFormatBMP:
          //
          // Upon finding the BMP Logo, save the Badge location from the Reserved values
          //  in the BMP Header
          //
          BadgeCoordinateX = ((BMP_IMAGE_HEADER *)ImageData)->Reserved[0];
          BadgeCoordinateY = ((BMP_IMAGE_HEADER *)ImageData)->Reserved[1];
          OverrideBadgeLocation = TRUE;
          break;

        case EfiBadgingSupportFormatTGA:
          //
          // Upon finding the TGA Logo, save the Badge location from the XOrigin
          // and YOrigin valuesin the TGA Header
          //
          BadgeCoordinateX = ((TGA_IMAGE_HEADER *)ImageData)->XOrigin;
          BadgeCoordinateY = ((TGA_IMAGE_HEADER *)ImageData)->YOrigin;
          OverrideBadgeLocation = TRUE;
          break;
        case EfiBadgingSupportFormatJPEG:
          //
          // Upon finding the JPG Logo, save the Badge location from the $LOC Comment
          //  in the JPG stream
          //
          for (Index = 0; Index < ImageSize; Index++) {
            if (*((UINT8 *)(ImageData + Index)) == 0xFF &&
                *((UINT8 *)(ImageData + Index + 1)) == 0xFE) {
              pJpegBadgeComment = (JPG_BADGE_COMMENT *)((UINT8 *)(ImageData + Index));
              if (pJpegBadgeComment->Signature == BADGE_SIGNATURE) {
                BadgeCoordinateX = pJpegBadgeComment->BadgeXLocation;
                BadgeCoordinateY = pJpegBadgeComment->BadgeYLocation;
                OverrideBadgeLocation = TRUE;
                break;
              }
            }
          }
          break;

        case EfiBadgingSupportFormatPCX:
        case EfiBadgingSupportFormatGIF:
          //
          // Not Support override badging location right now.
          //
          break;

        default:
          gBS->FreePool (ImageData);
          continue;
        }
        break;


      case EfiBadgingSupportImageBadge:
        if (OverrideBadgeLocation) {
          //
          // It needn't override attribute. If override this, all of the EfiBadgingSupportImageBadge type
          // pictures will be displayed in the same location (base on BadgeAttribute setting.
          //
          CoordinateX = BadgeCoordinateX;
          CoordinateY = BadgeCoordinateY;
        }
        break;

      default:
        return EFI_INVALID_PARAMETER;
      }
    } else {
      Status = GetSectionFromAnyFv (LogoFile, EFI_SECTION_RAW, 0, (VOID **) &ImageData, &ImageSize);
      if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
      }

      CoordinateX = 0;
      CoordinateY = 0;
      Attribute   = EfiBadgingSupportDisplayAttributeCenter;
    }

    Blt = NULL;
    BltSize = 0;
    TgaFormate = UnsupportedTgaFormat;
    HasAlphaChannel = FALSE;
    Animation = NULL;

    switch (Format) {

    case EfiBadgingSupportFormatBMP:
      Status = ConvertBmpToGopBlt (
            ImageData,
            ImageSize,
            (VOID **)&Blt,
            &BltSize,
            &Height,
            &Width
            );
      break;

    case EfiBadgingSupportFormatTGA:
      if (TgaDecoder == NULL) {
        Status = EFI_UNSUPPORTED;
        break;
      }
      Status = TgaDecoder->DecodeImage (
                                 TgaDecoder,
                                 ImageData,
                                 ImageSize,
                                 (UINT8 **)&Blt,
                                 &BltSize,
                                 &Height,
                                 &Width,
                                 &TgaFormate,
                                 &HasAlphaChannel
                                 );
      break;
    case EfiBadgingSupportFormatJPEG:
      if (JpegDecoder == NULL) {
        Status = EFI_UNSUPPORTED;
        break;
      }

      Status = JpegDecoder->DecodeImage (
                      JpegDecoder,
                      ImageData,
                      ImageSize,
                      (UINT8 **)&Blt,
                      &BltSize,
                      &Height,
                      &Width,
                      &DecoderStatus
                      );
      break;

    case EfiBadgingSupportFormatPCX:
      if (PcxDecoder == NULL) {
        Status = EFI_UNSUPPORTED;
        break;
      }

      Status = PcxDecoder->DecodeImage (
                      PcxDecoder,
                      ImageData,
                      ImageSize,
                      (UINT8 **)&Blt,
                      &BltSize,
                      &Height,
                      &Width
                      );
      break;

    case EfiBadgingSupportFormatGIF:
      if (GifDecoder == NULL) {
        Status = EFI_UNSUPPORTED;
        break;

      }

      Status = GifDecoder->CreateAnimationFromMem (
                             GifDecoder,
                             ImageData,
                             ImageSize,
                             NULL,
                             &Animation
                             );
      if (!EFI_ERROR (Status)) {
        Width  = Animation->Width;
        Height = Animation->Height;
      }
      break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
    }

    if (EFI_ERROR (Status)) {
      gBS->FreePool (ImageData);

      if (Badging == NULL) {
        return Status;
      } else {
        continue;
      }
    }

    //
    // Determine and change the video resolution based on the logo
    //

    if ((Type == EfiBadgingSupportImageLogo) && (Badging != NULL)) {
      if (Badging->OemVideoModeScrStrXY (Badging, OemSupportedVideoMode, (UINT32)Width, (UINT32)Height, &LocX, &LocY))  {
        SizeOfX = (UINT32)Width;
        SizeOfY = (UINT32)Height;
        if (GraphicsOutput != NULL) {
          SetModeByGraphicOutput (GraphicsOutput, SizeOfX, SizeOfY);
        } else if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
          Status = UgaDraw->SetMode (UgaDraw, SizeOfX, SizeOfY, ColorDepth, RefreshRate);
          if (EFI_ERROR (Status)) {
            gBS->FreePool (ImageData);
            return EFI_UNSUPPORTED;
          }
        }
      }
    }

    switch (Attribute) {

    case EfiBadgingSupportDisplayAttributeLeftTop:
      DestX = CoordinateX;
      DestY = CoordinateY;
      break;

    case EfiBadgingSupportDisplayAttributeCenterTop:
      DestX = (SizeOfX - Width) / 2;
      DestY = CoordinateY;
      break;

    case EfiBadgingSupportDisplayAttributeRightTop:
      DestX = (SizeOfX - Width - CoordinateX);
      DestY = CoordinateY;;
      break;

    case EfiBadgingSupportDisplayAttributeCenterRight:
      DestX = (SizeOfX - Width - CoordinateX);
      DestY = (SizeOfY - Height) / 2;
      break;

    case EfiBadgingSupportDisplayAttributeRightBottom:
      DestX = (SizeOfX - Width - CoordinateX);
      DestY = (SizeOfY - Height - CoordinateY);
      break;

    case EfiBadgingSupportDisplayAttributeCenterBottom:
      DestX = (SizeOfX - Width) / 2;
      DestY = (SizeOfY - Height - CoordinateY);
      break;

    case EfiBadgingSupportDisplayAttributeLeftBottom:
      DestX = CoordinateX;
      DestY = (SizeOfY - Height - CoordinateY);
      break;

    case EfiBadgingSupportDisplayAttributeCenterLeft:
      DestX = CoordinateX;
      DestY = (SizeOfY - Height) / 2;
      break;

    case EfiBadgingSupportDisplayAttributeCenter:
      DestX = (SizeOfX - Width) / 2;
      DestY = (SizeOfY - Height) / 2;
      break;

    default:
      DestX = CoordinateX;
      DestY = CoordinateY;
      break;
    }

    if ((DestX >= 0) && (DestY >= 0)) {
      if (GraphicsOutput != NULL) {
        if (Format == EfiBadgingSupportFormatGIF && GifDecoder != NULL && Animation != NULL) {
            AnimationRefreshEntry.Animation  = Animation;
            AnimationRefreshEntry.Data       = NULL;
            AnimationRefreshEntry.X          = DestX;
            AnimationRefreshEntry.Y          = DestY;
            AnimationRefreshEntry.BltWidth   = SizeOfX;
            AnimationRefreshEntry.BltHeight  = SizeOfY;
            AnimationRefreshEntry.Current    = NULL;
            AnimationRefreshEntry.RecordTick = 0;
            AnimationRefreshEntry.AutoLoop   = FALSE;
            AnimationRefreshEntry.Status     = ANIM_STATUS_PLAY;

            //
            // Only one picture, don't play animation
            //
            if (Animation->Frames != NULL && Animation->Frames->Next == NULL) {
              Status = GifDecoder->NextAnimationFrame (
                                     GifDecoder,
                                     &AnimationRefreshEntry,
                                     GraphicsOutput
                                     );

            } else {
              while (1) {
                if (ConsoleControl != NULL) {
                  ConsoleControl->GetMode (ConsoleControl, &ScreenMode, NULL, NULL);
                  if (ScreenMode == EfiConsoleControlScreenText) {
                    break;
                  }
                }
                Status = GifDecoder->RefreshAnimation (
                                       GifDecoder,
                                       &AnimationRefreshEntry,
                                       GraphicsOutput,
                                       0
                                       );
                if (EFI_ERROR (Status)) {
                  break;
                }
                //
                // wait 0.001 second
                //
                gBS->Stall (1000);
              };
            }
            GifDecoder->DestroyAnimation (GifDecoder, Animation);

        } else {
          if (Format == EfiBadgingSupportFormatTGA && Blt != NULL && TgaDecoder != NULL) {
            if (HasAlphaChannel) {
              VideoBuffer  = AllocatePool (BltSize);
              Status = GraphicsOutput->Blt (
                                  GraphicsOutput,
                                  (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) VideoBuffer,
                                  EfiBltVideoToBltBuffer,
                                  (UINTN) DestX,
                                  (UINTN) DestY,
                                  0,
                                  0,
                                  Width,
                                  Height,
                                  Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                  );
              Status = TgaDecoder->CalculateBltImage (
                                         TgaDecoder,
                                         (UINT8 *) Blt,
                                         BltSize,
                                         VideoBuffer,
                                         BltSize,
                                         TgaFormate
                                         );
              gBS->FreePool (Blt);
              Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) VideoBuffer;
            }
          }

          Status = GraphicsOutput->Blt (
                              GraphicsOutput,
                              Blt,
                              EfiBltBufferToVideo,
                              0,
                              0,
                              (UINTN) DestX,
                              (UINTN) DestY,
                              Width,
                              Height,
                              Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                              );
        }
      } else if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
        if (Format == EfiBadgingSupportFormatTGA && Blt != NULL && TgaDecoder != NULL) {
          if (HasAlphaChannel) {
            VideoBuffer  = AllocatePool (BltSize);
            Status = UgaDraw->Blt (
                              UgaDraw,
                              (EFI_UGA_PIXEL *) VideoBuffer,
                              EfiBltVideoToBltBuffer,
                              (UINTN) DestX,
                              (UINTN) DestY,
                              0,
                              0,
                              Width,
                              Height,
                              Width * sizeof (EFI_UGA_PIXEL)
                              );
            Status = TgaDecoder->CalculateBltImage (
                                       TgaDecoder,
                                       (UINT8 *) Blt,
                                       BltSize,
                                       VideoBuffer,
                                       BltSize,
                                       TgaFormate
                                       );
            gBS->FreePool (Blt);
            Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) VideoBuffer;
          }
        }

        Status = UgaDraw->Blt (
                            UgaDraw,
                            (EFI_UGA_PIXEL *) Blt,
                            EfiUgaBltBufferToVideo,
                            0,
                            0,
                            (UINTN) DestX,
                            (UINTN) DestY,
                            Width,
                            Height,
                            Width * sizeof (EFI_UGA_PIXEL)
                            );
      } else {
        Status = EFI_UNSUPPORTED;
      }
    }
    //
    //Display Oem String Message
    //
    if ((Type == EfiBadgingSupportImageLogo) && (Badging != NULL)) {
      mAutoPlaceStrDestY = SizeOfY / 2;
      ShowOemString(Badging, FALSE, 0);
    }

    gBS->FreePool (ImageData);
    gBS->FreePool (Blt);

    if (Badging == NULL) {
      break;
    }
  }

  return Status;
}

/**
  Use SystemTable Conout to turn on video based Simple Text Out consoles. The
  Simple Text Out screens will now be synced up with all non video output devices

  @retval EFI_SUCCESS     UGA devices are back in text mode and synced up.

**/
EFI_STATUS
DisableQuietBoot (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_UGA_DRAW_PROTOCOL           *UgaDraw;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  EFI_CONSOLE_CONTROL_PROTOCOL    *ConsoleControl;
  UINTN                           MaxMode;
  UINT32                          ModeNumber;
  UINT32                          ScuResolutionX;
  UINT32                          ScuResolutionY;
  UINT32                          GraphicsGopModeNumber;
  UINT32                          GraphicsResoltionX;
  UINT32                          GraphicsResoltionY;
  UINTN                           Columns;
  UINTN                           Rows;

  UgaDraw = NULL;
  GraphicsOutput = NULL;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, (VOID **)&UgaDraw);
    if (EFI_ERROR (Status)) {
      OnEndOfDisableQuietBoot ();

      return EFI_UNSUPPORTED;
    }
  }

  ConsoleControl = NULL;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (!EFI_ERROR (Status)) {
    Status = ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
  }


  Status = GetPreferredResolution (
             gST->ConsoleOutHandle,
             &ScuResolutionX,
             &ScuResolutionY,
             &GraphicsGopModeNumber,
             &GraphicsResoltionX,
             &GraphicsResoltionY
             );

  MaxMode    = gST->ConOut->Mode->MaxMode;
  ModeNumber = (UINT32) MaxMode;
  if (!EFI_ERROR (Status)) {
    for (ModeNumber = 2; ModeNumber < MaxMode; ModeNumber++) {
      //
      // Based on scu resoltuion, find the text mode that enables full screen text view.
      //       
      gST->ConOut->QueryMode (gST->ConOut, ModeNumber, &Columns, &Rows);
      if ((ScuResolutionX / EFI_GLYPH_WIDTH == Columns) &&
          (ScuResolutionY / EFI_GLYPH_HEIGHT == Rows) ) {
        Status = gST->ConOut->SetMode (gST->ConOut, ModeNumber);
        break;
      }
    }
  }

  //
  // When one video care with two display output, and plug-in two display,
  // the GOP.SetMode function maybe fail
  //
  if (EFI_ERROR (Status)) {
    for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
      gST->ConOut->QueryMode (gST->ConOut, ModeNumber, &Columns, &Rows);
      if ((PcdGet32 (PcdDefaultHorizontalResolution) / EFI_GLYPH_WIDTH == Columns) &&
          (PcdGet32 (PcdDefaultVerticalResolution) / EFI_GLYPH_HEIGHT == Rows)) {
        Status = gST->ConOut->SetMode (gST->ConOut, ModeNumber);
        break;
      }
    }
  }

  if (EFI_ERROR (Status) || ModeNumber == MaxMode) {
    gST->ConOut->SetMode (gST->ConOut, 0);
  }

  if (ConsoleControl == NULL) {
    gST->ConOut->ClearScreen (gST->ConOut);
  }
  //
  // Enable Cursor on Screen
  //
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  OnEndOfDisableQuietBoot ();

  return Status;
}

/*++

Routine Description:
  Retrieve native resolution of a panel by reading its Edid information.

Arguments:
  *EdidDiscovered       - A pointer to Edid discovered protocol
  *ResolutionX          - A pointer to native horizontal resolution of a panel.
  *ResolutionY          - A pointer to native vertical resolution of a panel.

Returns:
  EFI_SUCCESS           - The function runs correctly.

-*/
EFI_STATUS
GetResolutionByEdid (
  IN  EFI_EDID_DISCOVERED_PROTOCOL      *EdidDiscovered,
  OUT UINT32                            *ResolutionX,
  OUT UINT32                            *ResolutionY
  )
{
  UINT8                                  TempBufferH;
  UINT8                                  TempBufferL;
  UINT32                                 NativeResolutionX;
  UINT32                                 NativeResolutionY;
  VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK   *EdidDataBlock;

  ASSERT (EdidDiscovered != NULL);
  ASSERT (ResolutionX != NULL);
  ASSERT (ResolutionY != NULL);

  if (EdidDiscovered->Edid == NULL) {
    return EFI_UNSUPPORTED;
  }
  EdidDataBlock = (VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK *) EdidDiscovered->Edid;

  TempBufferH = EdidDataBlock->DetailedTimingDescriptions[4];
  TempBufferL = EdidDataBlock->DetailedTimingDescriptions[2];
  NativeResolutionX = ((((TempBufferH>>4)&0x0F) * 256) + TempBufferL) & 0x0FFF;

  TempBufferH = EdidDataBlock->DetailedTimingDescriptions[7];
  TempBufferL = EdidDataBlock->DetailedTimingDescriptions[5];
  NativeResolutionY = ((((TempBufferH>>4)&0x0F) * 256) + TempBufferL) & 0x0FFF;

  *ResolutionX = NativeResolutionX;
  *ResolutionY = NativeResolutionY;

  return EFI_SUCCESS;
}

BOOLEAN
IsModeSync (
  VOID
)
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *ConOutGraphicsOutput;
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *GraphicsOutput;
  EFI_HANDLE                              *HandleBuffer;
  UINTN                                   NumberOfHandles;
  UINTN                                   Index;
  EFI_DEVICE_PATH_PROTOCOL                *GopDevicePath;
  EFI_STATUS                              Status;
  BOOLEAN                                 SyncStatus;

  SyncStatus = TRUE;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&ConOutGraphicsOutput);
  if (EFI_ERROR (Status)) {
    return SyncStatus;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return SyncStatus;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&GopDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **) &GraphicsOutput
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    if (ConOutGraphicsOutput->Mode->Info->HorizontalResolution !=
        GraphicsOutput->Mode->Info->HorizontalResolution ||
        ConOutGraphicsOutput->Mode->Info->VerticalResolution!=
        GraphicsOutput->Mode->Info->VerticalResolution) {
      SyncStatus = FALSE;
    }
  }

  gBS->FreePool (HandleBuffer);
  return SyncStatus;
  }
/**
  Calculate GCD(Greatest Common Divisor) value

  @param[in] A              First value
  @param[in] B              Second value

  @return UINT32            The GCD value

**/
UINT32
CalculateGcd (
  UINT32 A,
  UINT32 B
  )
{
  UINT32 Temp;

  while (B != 0) {
    Temp = A % B;
    A = B;
    B = Temp;
  }
  return A;
}

/**
  Search current specific mode supported the user defined resolution
  for the Graphics Console device based on Graphics Output Protocol.

  @param[in] GraphicsOutput         Graphics Output Protocol instance pointer.
  @param[in]  HorizontalResolution  User defined horizontal resolution
  @param[in]  VerticalResolution    User defined vertical resolution.
  @param[out]  GopModeNum           Current specific mode to be check.
  @param[out]  ResolutionX          Preffered resoltion X
  @param[out]  ResolutionY          Preffered resoltion Y

  @retval EFI_SUCCESS               The mode is supported.
  @retval EFI_NOT_FOUND             The specific mode is out of range of graphics
                                    device supported.

**/
EFI_STATUS
FindNearestGopMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput,
  IN  UINT32                         HorizontalResolution,
  IN  UINT32                         VerticalResolution,
  OUT UINT32                         *GopModeNum,
  OUT UINT32                         *ResolutionX,
  OUT UINT32                         *ResolutionY
  )
{
  EFI_STATUS                            Status;
  UINT32                                SelResolutionX;
  UINT32                                SelResolutionY;
  UINT32                                SelModeNum;
  UINT32                                ModeNum;
  UINT32                                MaxMode;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT32                                RatioX;
  UINT32                                RatioY;
  UINT32                                GcdValue;
  UINTN                                 Count;

  ASSERT (GraphicsOutput != NULL);
  ASSERT (GopModeNum != NULL);
  ASSERT (ResolutionX != NULL);
  ASSERT (ResolutionY != NULL);
  ASSERT (HorizontalResolution != 0 || VerticalResolution != 0);

  if (GraphicsOutput == NULL ||
      HorizontalResolution == 0 ||
      VerticalResolution == 0) {
    return EFI_INVALID_PARAMETER;
  }

  MaxMode  = GraphicsOutput->Mode->MaxMode;
  GcdValue = CalculateGcd (HorizontalResolution, VerticalResolution);
  RatioX   = HorizontalResolution / GcdValue;
  RatioY   = VerticalResolution / GcdValue;

  //
  // count == 0, search same aspect ratio
  // count == 1, search all aspect ratio
  //
  for (Count = 0; Count < 2; Count++) {

    SelModeNum     = MaxMode;
    SelResolutionX = (UINT32)-1;
    SelResolutionY = (UINT32)-1;
    for (ModeNum = 0; ModeNum < MaxMode; ModeNum++) {
      Status = GraphicsOutput->QueryMode (
                                 GraphicsOutput,
                                 ModeNum,
                                 &SizeOfInfo,
                                 &Info
                                 );
      if (EFI_ERROR (Status) ||
          Info->HorizontalResolution < HorizontalResolution ||
          Info->VerticalResolution < VerticalResolution) {
        FreePool (Info);
        continue;
      }

      if (Count == 0 &&
          ((Info->HorizontalResolution / GcdValue != RatioX) ||
          (Info->VerticalResolution / GcdValue != RatioY))) {
        FreePool (Info);
        continue;
      }

      if ((Info->HorizontalResolution < SelResolutionX) ||
          ((Info->HorizontalResolution == SelResolutionX) &&
           (Info->VerticalResolution <= SelResolutionY))) {
        SelModeNum     = ModeNum;
        SelResolutionX = Info->HorizontalResolution;
        SelResolutionY = Info->VerticalResolution;
        FreePool (Info);
      }
    }

    if (SelModeNum != MaxMode) {
      *GopModeNum  = SelModeNum;
      *ResolutionX = SelResolutionX;
      *ResolutionY = SelResolutionY;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Check if the current specific mode supported the user defined resolution
  for the Graphics Console device based on Graphics Output Protocol.

  If yes, set the graphic devcice's current mode to this specific mode.

  @param  GraphicsOutput        Graphics Output Protocol instance pointer.
  @param  HorizontalResolution  User defined horizontal resolution
  @param  VerticalResolution    User defined vertical resolution.
  @param  CurrentModeNumber     Current specific mode to be check.

  @retval EFI_SUCCESS       The mode is supported.
  @retval EFI_UNSUPPORTED   The specific mode is out of range of graphics
                            device supported.
  @retval other             The specific mode does not support user defined
                            resolution or failed to set the current mode to the
                            specific mode on graphics device.

**/
EFI_STATUS
CheckModeSupported (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                    HorizontalResolution,
  IN  UINT32                    VerticalResolution,
  OUT UINT32                    *CurrentModeNumber
  )
{
  UINT32     ModeNumber;
  EFI_STATUS Status;
  UINTN      SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINT32     MaxMode;

  ASSERT (CurrentModeNumber != NULL);

  Status  = EFI_SUCCESS;
  MaxMode = GraphicsOutput->Mode->MaxMode;

  for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
    Status = GraphicsOutput->QueryMode (
                       GraphicsOutput,
                       ModeNumber,
                       &SizeOfInfo,
                       &Info
                       );
    if (!EFI_ERROR (Status)) {
      if ((Info->HorizontalResolution == HorizontalResolution) &&
          (Info->VerticalResolution == VerticalResolution)) {
        break;
      }
      FreePool (Info);
    }
  }

  if (ModeNumber == MaxMode) {
    return EFI_UNSUPPORTED;
  }

  *CurrentModeNumber = ModeNumber;
  return Status;
}

/*++

 Retrieve preferred resolution of a GOP.

 @param[in]  GraphicsOutput         Instance of console out handle
 @param[out] ScuResolutionX         X axis of text (SCU) resoliton
 @param[out] ScuResolutionY         Y axis of text (SCU) resoliton
 @param[out] GraphicsResoltionX     X axis of graphics resoliton
 @param[out] GraphicsResoltionY     Y axis of graphics resoliton

 @retval EFI_SUCCESS  Function complete successfully.

**/
EFI_STATUS
GetPreferredResolution (
  IN  EFI_HANDLE                        ConOutHandle,
  OUT UINT32                            *ScuResolutionX,
  OUT UINT32                            *ScuResolutionY,
  OUT UINT32                            *GraphicsGopModeNumber,
  OUT UINT32                            *GraphicsResoltionX,
  OUT UINT32                            *GraphicsResoltionY
  )
{
  EFI_STATUS                             Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL           *GraphicsOutput;
  EFI_EDID_DISCOVERED_PROTOCOL           *EdidDiscovered;
  OEM_LOGO_RESOLUTION_DEFINITION         LogoResolution;
  OEM_LOGO_RESOLUTION_DEFINITION         *OemLogoResolutionPtr;
  UINT32                                 NativeResolutionX;
  UINT32                                 NativeResolutionY;
  UINT32                                 ModeNumber;

  ASSERT (ConOutHandle != NULL);
  ASSERT (ScuResolutionX != NULL);
  ASSERT (ScuResolutionY != NULL);
  ASSERT (GraphicsResoltionX != NULL);
  ASSERT (GraphicsResoltionY != NULL);

  if (ConOutHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OemLogoResolutionPtr = (OEM_LOGO_RESOLUTION_DEFINITION *)PcdGetPtr (PcdDefaultLogoResolution);

  Status = gBS->HandleProtocol (
                  ConOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // According to Window 8 logo requirement (System.Client.Firmware.UEFI.GOP)
  // If Edid mode is not supported, UEFI must select an alternate mode that matches
  // the same aspect ratio as the native resolution of the display.
  // If the display device does not provide an EDID, UEFI must set a mode of
  // 1024 x 768
  //
  EdidDiscovered    = NULL;
  NativeResolutionX = 0;
  NativeResolutionY = 0;
  Status = gBS->HandleProtocol (
                  ConOutHandle,
                  &gEfiEdidDiscoveredProtocolGuid,
                  (VOID **)&EdidDiscovered
                  );
  if (!EFI_ERROR (Status)) {
    Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
    if (EFI_ERROR (Status)) {
      EdidDiscovered = NULL;
    } else {
      Status = FindNearestGopMode (
                 GraphicsOutput,
                 NativeResolutionX,
                 NativeResolutionY,
                 &ModeNumber,
                 &NativeResolutionX,
                 &NativeResolutionY
                 );

      if (EFI_ERROR (Status)) {
        EdidDiscovered = NULL;
      }
    }
  }

  *ScuResolutionX     = PcdGet32 (PcdDefaultHorizontalResolution);
  *ScuResolutionY     = PcdGet32 (PcdDefaultVerticalResolution);
  *GraphicsResoltionX = 0;
  *GraphicsResoltionY = 0;

  if (EdidDiscovered != NULL) {
    LogoResolution.LogoResolutionX = NativeResolutionX;
    LogoResolution.LogoResolutionY = NativeResolutionY;
    OemLogoResolutionPtr->LogoResolutionX = LogoResolution.LogoResolutionX;
    OemLogoResolutionPtr->LogoResolutionY = LogoResolution.LogoResolutionY;
  }

  //
  // OemServices
  //
  Status = OemSvcLogoResolution (&OemLogoResolutionPtr);

  if (OemLogoResolutionPtr != NULL) {
    Status = CheckModeSupported (
               GraphicsOutput,
               OemLogoResolutionPtr->LogoResolutionX,
               OemLogoResolutionPtr->LogoResolutionY,
               &ModeNumber
               );
    if (!EFI_ERROR (Status)) {
      *GraphicsGopModeNumber = ModeNumber;
      *GraphicsResoltionX = OemLogoResolutionPtr->LogoResolutionX;
      *GraphicsResoltionY = OemLogoResolutionPtr->LogoResolutionY;
    }
    Status = CheckModeSupported (
               GraphicsOutput,
               OemLogoResolutionPtr->ScuResolutionX,
               OemLogoResolutionPtr->ScuResolutionY,
               &ModeNumber
               );
    if (!EFI_ERROR (Status)) {
      *ScuResolutionX = OemLogoResolutionPtr->ScuResolutionX;
      *ScuResolutionY = OemLogoResolutionPtr->ScuResolutionY;
    }
  }

  if (*GraphicsResoltionX == 0 && *GraphicsResoltionY == 0) {
    Status = FindNearestGopMode (
               GraphicsOutput,
               PcdGet32 (PcdDefaultHorizontalResolution),
               PcdGet32 (PcdDefaultVerticalResolution),
               &ModeNumber,
               GraphicsResoltionX,
               GraphicsResoltionY
               );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    *GraphicsGopModeNumber = ModeNumber;
  }

  return EFI_SUCCESS;
}

BOOLEAN
TextOnlyConsole (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *NextDevPathInstance;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  BOOLEAN                          FoundTextBasedCon;
  UINTN                            Size;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;

  RemainingDevicePath = OemGraphicsGetAllActiveConOutDevPath ();
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = OemGraphicsGetNextDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInstance != NULL) {
      gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &NextDevPathInstance, &Handle);
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID*)&Gop
                      );
       if (EFI_ERROR (Status)) {
         //
         //found text-based console
         //
         FoundTextBasedCon = TRUE;
         break;
       }
    }
  } while (RemainingDevicePath != NULL);

  return FoundTextBasedCon;
}


VOID
DisplayMessageToConsoleRedirection (
  IN     CHAR16   *LogoStr
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *NextDevPathInstance;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  BOOLEAN                          FoundTextBasedCon;
  UINTN                            Size;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *TextOutProto;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;

  RemainingDevicePath = OemGraphicsGetAllActiveConOutDevPath ();
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = OemGraphicsGetNextDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInstance != NULL) {
      gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &NextDevPathInstance, &Handle);
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID*)&Gop
                      );
       if (EFI_ERROR (Status)) {
         //
         //found text-based console
         //
         Status = gBS->HandleProtocol (
                         Handle,
                         &gEfiSimpleTextOutProtocolGuid,
                         (VOID*) &TextOutProto
                         );
         if (!EFI_ERROR (Status)) {
           TextOutProto->SetCursorPosition (TextOutProto, 0, 0);
           TextOutProto->OutputString (TextOutProto, LogoStr);
         }
       }
    }
  } while (RemainingDevicePath != NULL);

}

VOID *
OemGraphicsGetAllActiveConOutDevPath (
  VOID
  )
{
  UINTN                      NoHandles;
  EFI_HANDLE                 *HandleBuf;
  EFI_DEVICE_PATH_PROTOCOL   *DevPath;
  EFI_DEVICE_PATH_PROTOCOL   *ConsoleDevPath;
  EFI_DEVICE_PATH_PROTOCOL   *TempDevPath;
  EFI_STATUS                 Status;
  UINT8                      Index;

  //
  //init local
  //
  NoHandles = 0;
  HandleBuf = NULL;
  DevPath = NULL;
  ConsoleDevPath = NULL;
  TempDevPath = NULL;

  Status =  gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiConsoleOutDeviceGuid,
              NULL,
              &NoHandles,
              &HandleBuf
              );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status =  gBS->HandleProtocol (
                HandleBuf [Index],
                &gEfiDevicePathProtocolGuid,
                (VOID **)&DevPath
                );
    if (EFI_ERROR (Status) || (DevPath == NULL)) {
      continue;
    }

    TempDevPath = ConsoleDevPath;
    ConsoleDevPath = AppendDevicePathInstance (ConsoleDevPath, DevPath);
    if (TempDevPath != NULL) {
      FreePool (TempDevPath);
    }
  }

  FreePool (HandleBuf);
  return ConsoleDevPath;
}


EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsGetNextDevicePathInstance (
  IN OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath,
  OUT UINTN                          *Size
  )
/*++

Routine Description:
  Function retrieves the next device path instance from a device path data structure.

Arguments:
  DevicePath           - A pointer to a device path data structure.

  Size                 - A pointer to the size of a device path instance in bytes.

Returns:

  This function returns a pointer to the current device path instance.
  In addition, it returns the size in bytes of the current device path instance in Size,
  and a pointer to the next device path instance in DevicePath.
  If there are no more device path instances in DevicePath, then DevicePath will be set to NULL.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  EFI_DEVICE_PATH_PROTOCOL  *ReturnValue;
  UINT8                     Temp;

  ASSERT (Size != NULL);

  if (DevicePath == NULL || *DevicePath == NULL) {
    *Size = 0;
    return NULL;
  }

  if (!OemGraphicsIsDevicePathValid (*DevicePath, 0)) {
    return NULL;
  }

  //
  // Find the end of the device path instance
  //
  DevPath = *DevicePath;
  while (!OemGraphicsIsDevicePathEndType (DevPath)) {
    DevPath = OemGraphicsNextDevicePathNode (DevPath);
  }

  //
  // Compute the size of the device path instance
  //
  *Size = ((UINTN) DevPath - (UINTN) (*DevicePath)) + sizeof (EFI_DEVICE_PATH_PROTOCOL);

  //
  // Make a copy and return the device path instance
  //
  Temp              = DevPath->SubType;
  DevPath->SubType  = END_ENTIRE_DEVICE_PATH_SUBTYPE;
  ReturnValue       = OemGraphicsDuplicateDevicePath (*DevicePath);
  DevPath->SubType  = Temp;

  //
  // If DevPath is the end of an entire device path, then another instance
  // does not follow, so *DevicePath is set to NULL.
  //
  if (OemGraphicsDevicePathSubType (DevPath) == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
    *DevicePath = NULL;
  } else {
    *DevicePath = OemGraphicsNextDevicePathNode (DevPath);
  }

  return ReturnValue;
}

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsDuplicateDevicePath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Duplicate a device path structure.

Arguments:

  DevicePath  - The device path to duplicated.

Returns:

  The duplicated device path.

--*/
{
  UINTN                     Size;

  //
  // Compute the size
  //
  Size = OemGraphicsGetDevicePathSize (DevicePath);
  if (Size == 0) {
    return NULL;
  }

  //
  // Allocate space for duplicate device path
  //

  return AllocateCopyPool (Size, DevicePath);
}

UINTN
EFIAPI
OemGraphicsGetDevicePathSize (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Calculate the space size of a device path.

Arguments:

  DevicePath  - A specified device path

Returns:

  The size.

--*/
{
  CONST EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }

  if (!OemGraphicsIsDevicePathValid (DevicePath, 0)) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!OemGraphicsIsDevicePathEnd (DevicePath)) {
    DevicePath = OemGraphicsNextDevicePathNode (DevicePath);
  }

  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + OemGraphicsDevicePathNodeLength (DevicePath);
}

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathValid (
  IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN       UINTN                    MaxSize
  )
{
  UINTN Count;
  UINTN Size;
  UINTN NodeLength;

  ASSERT (DevicePath != NULL);

  for (Count = 0, Size = 0; !OemGraphicsIsDevicePathEnd (DevicePath); DevicePath = OemGraphicsNextDevicePathNode (DevicePath)) {
    NodeLength = OemGraphicsDevicePathNodeLength (DevicePath);
    if (NodeLength < sizeof (EFI_DEVICE_PATH_PROTOCOL)) {
      return FALSE;
    }

    if (MaxSize > 0) {
      Size += NodeLength;
      if (Size + END_DEVICE_PATH_LENGTH > MaxSize) {
        return FALSE;
      }
    }

    if (PcdGet32 (PcdMaximumDevicePathNodeCount) > 0) {
      Count++;
      if (Count >= PcdGet32 (PcdMaximumDevicePathNodeCount)) {
        return FALSE;
      }
    }
  }

  //
  // Only return TRUE when the End Device Path node is valid.
  //
  return (BOOLEAN) (OemGraphicsDevicePathNodeLength (DevicePath) == END_DEVICE_PATH_LENGTH);
}

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathEndType (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  return (BOOLEAN) (OemGraphicsDevicePathType (Node) == END_DEVICE_PATH_TYPE);
}

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsNextDevicePathNode (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  return (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)(Node) + OemGraphicsDevicePathNodeLength(Node));
}

UINT8
EFIAPI
OemGraphicsDevicePathSubType (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  return ((EFI_DEVICE_PATH_PROTOCOL *)(Node))->SubType;
}

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathEnd (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  return (BOOLEAN) (OemGraphicsIsDevicePathEndType (Node) && OemGraphicsDevicePathSubType(Node) == END_ENTIRE_DEVICE_PATH_SUBTYPE);
}

UINTN
EFIAPI
OemGraphicsDevicePathNodeLength (
  IN CONST VOID  *Node
  )
{
  UINTN Length;

  ASSERT (Node != NULL);
  Length = ReadUnaligned16 ((UINT16 *)&((EFI_DEVICE_PATH_PROTOCOL *)(Node))->Length[0]);
  ASSERT (Length >= sizeof (EFI_DEVICE_PATH_PROTOCOL));
  return Length;
}

UINT8
EFIAPI
OemGraphicsDevicePathType (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  return ((EFI_DEVICE_PATH_PROTOCOL *)(Node))->Type;
}
