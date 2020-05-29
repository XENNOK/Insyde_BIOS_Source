//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
//;   Graphics.c
//;
//; Abstract:
//;
//;   Support for Basic Graphics operations.
//;

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "OemGraphicsLib.h"
#include "EfiPrintLib.h"
#include "OemServicesSupport.h"
#include "OemLogoResolution.h"
#include "EfiHii.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)
#include EFI_PROTOCOL_DEFINITION (JpegDecoder)
#include EFI_PROTOCOL_DEFINITION (PcxDecoder)
#include EFI_PROTOCOL_DEFINITION (TgaDecoder)
#include EFI_PROTOCOL_DEFINITION (GifDecoder)
#include EFI_GUID_DEFINITION     (Jpeg)
#include EFI_GUID_DEFINITION     (Pcx)
#include EFI_PROTOCOL_PRODUCER   (EndOfDisableQuietBoot)
#include EFI_PROTOCOL_DEFINITION (BootLogo)
#include "VesaBiosExtensions.h"
#define DEFAULT_HorizontalResolution  640
#define DEFAULT_VerticalResolution    480

#define GLYPH_WIDTH  8
#define GLYPH_HEIGHT  19
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
UINTN                                mAutoPlaceStrDestY = (UINTN) (-1);
UINTN                                mAutoPlaceStrCount;
BOOLEAN
IsModeSync (
  VOID
);

EFI_STATUS
GetGraphicsBitMapFromFV (
  IN  EFI_GUID      *FileNameGuid,
  OUT VOID          **Image,
  OUT UINTN         *ImageSize
  )
/*++

Routine Description:

  Return the graphics image file named FileNameGuid into Image and return it's
  size in ImageSize. All Firmware Volumes (FV) in the system are searched for the
  file name.

Arguments:

  FileNameGuid  - File Name of graphics file in the FV(s).

  Image         - Pointer to pointer to return graphics image.  If NULL, a
                  buffer will be allocated.

  ImageSize     - Size of the graphics Image in bytes. Zero if no image found.


Returns:

  EFI_SUCCESS          - Image and ImageSize are valid.
  EFI_BUFFER_TOO_SMALL - Image not big enough. ImageSize has required size
  EFI_NOT_FOUND        - FileNameGuid not found

--*/
{
  EFI_STATUS                    Status;
  UINTN                         FvProtocolCount;
  EFI_HANDLE                    *FvHandles;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  UINTN                         Index;
  UINT32                        AuthenticationStatus;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &FvProtocolCount,
                  &FvHandles
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < FvProtocolCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandles[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **) &Fv
                    );

    //
    // Assuming Image and ImageSize are correct on input.
    //
    Status = Fv->ReadSection (
                  Fv,
                  &gEfiDefaultBmpLogoGuid,
                  EFI_SECTION_RAW,
                  0,
                  Image,
                  ImageSize,
                  &AuthenticationStatus
                  );
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    } else if (Status == EFI_BUFFER_TOO_SMALL) {
      //
      // ImageSize updated to needed size so return
      //
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  return EFI_NOT_FOUND;
}

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
  Status = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, &JpegDecoder);

  PcxDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiPcxDecoderProtocolGuid, NULL, &PcxDecoder);

  TgaDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiTgaDecoderProtocolGuid, NULL, &TgaDecoder);

  GifDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, &GifDecoder);

  switch (ImageFormat) {

    case EfiBadgingSupportFormatBMP:
      Status = ConvertBmpToGopBlt (
            ImageData,
            ImageSize,
            Blt,
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
        *Blt = EfiLibAllocateZeroPool (BltSize);
        if (*Blt != NULL) {
          EfiCopyMem (*Blt, Animation->Frames->Bitmap, BltSize);
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

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, &Badging);
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
  }
  //
  // Current setting: Image on the middle of the screen, and 38.2% from top.
  //
  *CoordinateX = (UINTN)(ResolutionX - ImageWidth) / 2;
  *CoordinateY = (UINTN)((ResolutionY * 382 / 1000) - (ImageHeight / 2));
}

EFI_STATUS
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

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, &Badging);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
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
      return Status;
    }

    EfiSetMem (&Blt, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x00);

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
  return Status;
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

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
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

  if (ImageData != NULL) {
    gBS->FreePool (ImageData);
  }

  return Status;
}

EFI_STATUS
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
  // BIOS show logo.
  //
  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // switch to the Graphical mode.
  //
  ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
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
  //
  // Y vaule of auto place OEM badging string starts at y value of image bottom plus 2 glyph heigh.
  //
  mAutoPlaceStrDestY = DestinationY + Height + 2 * EFI_GLYPH_HEIGHT;

  //
  // show OemString.
  //
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, &Badging);
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
  Status = EfiCreateEventReadyToBoot (
             EFI_TPL_CALLBACK,
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
  UINT8         FirstByte;
  UINT8         SecondByte;
  UINT8         EachValue;
  UINTN         Index;
  UINTN         Height;
  UINTN         Width;
  BOOLEAN       EndOfLine;
  BOOLEAN       EndOfBMP;
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
      }
      else {
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
ConvertBmpToGopBlt (
  IN  VOID      *BmpImage,
  IN  UINTN     BmpImageSize,
  IN OUT VOID   **GopBlt,
  IN OUT UINTN  *GopBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth
  )
/*++

Routine Description:

  Convert a *.BMP graphics image to a GOP/UGA blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

Arguments:

  BmpImage      - Pointer to BMP file

  BmpImageSize  - Number of bytes in BmpImage

  GopBlt        - Buffer containing GOP version of BmpImage.

  GopBltSize    - Size of GopBlt in bytes.

  PixelHeight   - Height of GopBlt/BmpImage in pixels

  PixelWidth    - Width of GopBlt/BmpImage in pixels


Returns:

  EFI_SUCCESS           - GopBlt and GopBltSize are returned.
  EFI_UNSUPPORTED       - BmpImage is not a valid *.BMP image
  EFI_BUFFER_TOO_SMALL  - The passed in GopBlt buffer is not big enough.
                          GopBltSize will contain the required size.
  EFI_OUT_OF_RESOURCES  - No enough buffer to allocate

--*/
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
  BOOLEAN                       IsAllocated;

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;
  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  if (BmpHeader->CompressionType != BMP_UNCOMPRESSION_TYPE && BmpHeader->CompressionType != BMP_RLE8BIT_TYPE) {
    return EFI_UNSUPPORTED;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  BltBufferSize = BmpHeader->PixelWidth * BmpHeader->PixelHeight * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  if (BltBufferSize >= SIZE_4G) {
     //
     // The buffer size extends the limitation
     //
     return EFI_UNSUPPORTED;
  }

  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = EfiLibAllocatePool (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
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
        // Convert 1bit BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt --;
        Width --;
        break;

      case 4:
        //
        // Convert BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      case 32:
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image++;
        break;

      default:
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


EFI_STATUS
LockKeyboards (
  IN  CHAR16    *Password
  )
/*++

Routine Description:
  Use Console Control Protocol to lock the Console In Spliter virtual handle.
  This is the ConInHandle and ConIn handle in the EFI system table. All key
  presses will be ignored until the Password is typed in. The only way to
  disable the password is to type it in to a ConIn device.

Arguments:
  Password - Password used to lock ConIn device


Returns:

  EFI_SUCCESS     - ConsoleControl has been flipped to graphics and logo
                          displayed.
  EFI_UNSUPPORTED - Logo not found

--*/
{
  EFI_STATUS                    Status;
  EFI_CONSOLE_CONTROL_PROTOCOL  *ConsoleControl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
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
  EFI_UGA_PIXEL                 Foreground;
  EFI_UGA_PIXEL                 Background;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;

  UgaDraw = NULL;
  GraphicsOutput = NULL;
  EfiCopyMem (&Foreground, &ForegroundRGB, sizeof (EFI_UGA_PIXEL));
  EfiCopyMem (&Background, &BackgroundRGB, sizeof (EFI_UGA_PIXEL));


  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    &UgaDraw
                    );

    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
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
  if (PosX == (UINTN) (-1) || PosY == (UINTN) (-1)) {
    if (mAutoPlaceStrDestY == (UINTN) (-1)) {
      return EFI_UNSUPPORTED;
    }
    PosX = (SizeOfX - EfiStrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2;
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


STATIC
BOOLEAN
IsGopReady (
  VOID
  )
/*++
Routine Description:

  This function uses to check GOP hardware device is whether ready.

Arguments:

  None

Returns:

  TRUE  - GOP hardware device is ready.
  FALSE - GOP hardware device isn't ready.

--*/
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
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

  EfiZeroMem (&Foreground, sizeof (EFI_UGA_PIXEL));
  Status = Badging->GetStringCount(Badging, &StringCount);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  mAutoPlaceStrCount = 0;
  Status = EFI_UNSUPPORTED;

  for ( Index = 0 ; Index < StringCount ; Index++ ) {
    if (Badging->GetOemString(Badging, Index, AfterSelect, SelectedStringNum, &StringData, &LocX, &LocY, &Foreground, &Background)) {
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

  return Status;
}

VOID
SetModeByGraphicOutput (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput,
  IN UINT32                             SizeOfX,
  IN UINT32                             SizeOfY
  )
{
  UINT32                                Index;
  UINT32                                OriginalMode;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT8                                 *EFIBdaVGAMode;
  UINT8                                 *BdaVGAMode;
  EFI_STATUS                            Status;

  EFIBdaVGAMode = (UINT8 *)(UINTN)EFI_CURRENT_VGA_MODE_ADDRESS;
  BdaVGAMode    = (UINT8 *)(UINTN)CURRENT_VGA_MODE_ADDRESS;
  Status = EFI_UNSUPPORTED;

  OriginalMode = GraphicsOutput->Mode->Mode;
  GraphicsOutput->QueryMode (GraphicsOutput, OriginalMode, &SizeOfInfo, &Info);
  if (Info->HorizontalResolution != SizeOfX || Info->VerticalResolution != SizeOfY || !IsModeSync ()) {
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

    if (EFI_ERROR (Status) || (Index == GraphicsOutput->Mode->MaxMode)) {
      for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
        GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (Info->HorizontalResolution == DEFAULT_HORIZONTAL_RESOLUTION &&
            Info->VerticalResolution == DEFAULT_VERTICAL_RESOLUTION) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
            gBS->FreePool (Info);
            break;
        }
        gBS->FreePool (Info);
      }
    }

    if (EFI_ERROR (Status) || (Index == GraphicsOutput->Mode->MaxMode)) {
      for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
        GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (Info->HorizontalResolution == 640 &&
            Info->VerticalResolution == 480) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
            gBS->FreePool (Info);
            break;
        }
        gBS->FreePool (Info);
      }
    }

    ASSERT_EFI_ERROR (Status);

  } else {
    if (*EFIBdaVGAMode != *BdaVGAMode) {
      GraphicsOutput->SetMode (GraphicsOutput, OriginalMode);
    }
  }
}

EFI_STATUS
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
  )
/*++

Routine Description:

  Use Console Control to turn off GOP/UGA based Simple Text Out consoles from going
  to the GOP/UGA device. Put up LogoFile on every GOP/UGA device that is a console

Arguments:

  LogoFile - File name of logo to display on the center of the screen.


Returns:

  EFI_SUCCESS           - ConsoleControl has been flipped to graphics and logo
                          displayed.
  EFI_UNSUPPORTED       - Logo not found

--*/
{
  EFI_STATUS                                    Status;
  EFI_CONSOLE_CONTROL_PROTOCOL                  *ConsoleControl;
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
  EFI_CONSOLE_CONTROL_SCREEN_MODE               ScreenMode;
  EFI_BOOT_LOGO_PROTOCOL                        *SetBootLogo;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  SetBootLogo = NULL;
  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, &SetBootLogo);
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
  //
  // Try to open GOP first
  //
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    //
    // Open GOP failed, try to open UGA
    //
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, &UgaDraw);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  Badging = NULL;
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, &Badging);

  JpegDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, &JpegDecoder);

  PcxDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiPcxDecoderProtocolGuid, NULL, &PcxDecoder);

  TgaDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiTgaDecoderProtocolGuid, NULL, &TgaDecoder);

  GifDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, &GifDecoder);

  ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);

  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
    ColorDepth = 0;
    RefreshRate = 0;
  } else {
    Status = UgaDraw->GetMode (UgaDraw, &SizeOfX, &SizeOfY, &ColorDepth, &RefreshRate);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
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
      Status = GetGraphicsBitMapFromFV (LogoFile, &ImageData, &ImageSize);
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
            &Blt,
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
        }
        else {
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
                ConsoleControl->GetMode (ConsoleControl, &ScreenMode, NULL, NULL);
                if (ScreenMode == EfiConsoleControlScreenText) {
                  break;
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
              VideoBuffer  = EfiLibAllocatePool (BltSize);
              Status = GraphicsOutput->Blt (
                                  GraphicsOutput,
                                  (EFI_UGA_PIXEL *) VideoBuffer,
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
      } else {
        if (Format == EfiBadgingSupportFormatTGA && Blt != NULL && TgaDecoder != NULL) {
          if (HasAlphaChannel) {
            VideoBuffer  = EfiLibAllocatePool (BltSize);
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

EFI_STATUS
DisableQuietBoot (
  VOID
  )
/*++

Routine Description:

  Use Console Control to turn on GOP/UGA based Simple Text Out consoles. The GOP/UGA
  Simple Text Out screens will now be synced up with all non GOP/UGA output devices

Arguments:

  NONE

Returns:

  EFI_SUCCESS           - GOP/UGA devices are back in text mode and synced up.
  EFI_UNSUPPORTED       - Logo not found

--*/
{
  EFI_STATUS                      Status;
  EFI_CONSOLE_CONTROL_PROTOCOL    *ConsoleControl;
  EFI_UGA_DRAW_PROTOCOL           *UgaDraw;
  OEM_SERVICES_PROTOCOL           *OemServices;
  OEM_LOGO_RESOLUTION_DEFINITION  *OemLogoResolutionPtr;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  UINT32                          ModeNumber;
  UINTN                           MaxMode;
  UINT32                          ScuResolutionX;
  UINT32                          ScuResolutionY;
  UINTN                           Columns;
  UINTN                           Rows;

  UgaDraw = NULL;
  GraphicsOutput = NULL;
  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
  if (EFI_ERROR (Status)) {
    OnEndOfDisableQuietBoot ();

    return EFI_UNSUPPORTED;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, &UgaDraw);
    if (EFI_ERROR (Status)) {
      OnEndOfDisableQuietBoot ();

      return EFI_UNSUPPORTED;
    }
  }

  OemServices = NULL;
  OemLogoResolutionPtr = NULL;
  Status = gBS->LocateProtocol(&gOemServicesProtocolGuid, NULL, &OemServices);
  if (!EFI_ERROR(Status)) {
    OemServices->Funcs[COMMON_LOGO_RESOLUTION] (
                   OemServices,
                   COMMON_LOGO_RESOLUTION_ARG_COUNT,
                   &OemLogoResolutionPtr
                   );
  }

  if (OemLogoResolutionPtr != NULL) {
    ScuResolutionX = OemLogoResolutionPtr->SCUResolutionX;
    ScuResolutionY = OemLogoResolutionPtr->SCUResolutionY;
  } else {
    ScuResolutionX = DEFAULT_HORIZONTAL_RESOLUTION;
    ScuResolutionY = DEFAULT_VERTICAL_RESOLUTION;
  }

  Status = ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
  if (EFI_ERROR (Status)) {
   return Status;
  }

  MaxMode = gST->ConOut->Mode->MaxMode;
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
 
  if (EFI_ERROR (Status) || (ModeNumber == MaxMode)) {
    gST->ConOut->SetMode (gST->ConOut, 0);
  }
  
  OnEndOfDisableQuietBoot ();

  return Status;
}

EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  )
{
  UINTN        ModeNum;
  UINTN        Columns;
  UINTN        Rows;
  EFI_STATUS   Status;

  for (ModeNum = 0; ModeNum < gST->ConOut->Mode->MaxMode; ModeNum++) {
    gST->ConOut->QueryMode (gST->ConOut, ModeNum, &Columns, &Rows);
    if ((RequestedColumns == Columns) && (RequestedRows == Rows)) {
      *TextModeNum = ModeNum;
      break;
    }
  }

  if (ModeNum == gST->ConOut->Mode->MaxMode) {
    *TextModeNum = ModeNum;
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}


EFI_STATUS
LogoLibSetBootLogo (
  VOID
  )
/*++

Routine Description:

  Consume by Bgrt driver to get the logo infomation.
  In some platform won't called EnableQuietBoot to update the Bgrt logo info,
  if the logo info won't update when ReadyToBoot event trigger, the Bgrt driver will
  get the logo info automatically by this function.

Arguments:

  None.

Returns:

  EFI_SUCCESS             Get boot logo info success.

--*/
{
  EFI_STATUS                     Status;
  EFI_BOOT_LOGO_PROTOCOL         *BootLogoProtocol;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BltBuffer = NULL;
  UINTN                          DestinationX = 0;
  UINTN                          DestinationY = 0;
  UINTN                          Width = 0;
  UINTN                          Height = 0;

  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, &BootLogoProtocol);
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

EFI_STATUS
GetResolutionByEdid (
  IN  EFI_EDID_DISCOVERED_PROTOCOL      *EdidDiscovered,
  OUT UINT32                            *ResolutionX,
  OUT UINT32                            *ResolutionY
  )
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

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &ConOutGraphicsOutput);
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

  RemainingDevicePath = GetConOutVariable (L"ConOut");
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = OemGraphicsLiteDevicePathInstance (&RemainingDevicePath, &Size);
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
  EFI_SIMPLE_TEXT_OUT_PROTOCOL     *TextOutProto;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;

  RemainingDevicePath = GetConOutVariable (L"ConOut");
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = OemGraphicsLiteDevicePathInstance (&RemainingDevicePath, &Size);
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
GetConOutVariable (
  IN CHAR16 * Name
  )
{
  EFI_STATUS  Status;
  VOID        *Buffer;
  UINTN       BufferSize;

  BufferSize  = 0;
  Buffer      = NULL;

  //
  // Test to see if ConOut content exists
  //
  Status = gRT->GetVariable (
                  Name,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, &Buffer);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (
                    Name,
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  return Buffer;
}

EFI_DEVICE_PATH_PROTOCOL *
OemGraphicsLiteDevicePathInstance (
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  OUT UINTN                         *Size
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

  if (*DevicePath == NULL) {
    if (Size != NULL) {
      *Size = 0;
    }

    return NULL;
  }

  //
  // Find the end of the device path instance
  //
  DevPath = *DevicePath;
  while (!IsDevicePathEndType (DevPath)) {
    DevPath = NextDevicePathNode (DevPath);
  }

  //
  // Compute the size of the device path instance
  //
  if (Size != NULL) {
    *Size = ((UINTN) DevPath - (UINTN) (*DevicePath)) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }

  //
  // Make a copy and return the device path instance
  //
  Temp              = DevPath->SubType;
  DevPath->SubType  = END_ENTIRE_DEVICE_PATH_SUBTYPE;
  ReturnValue       = OemGraphicsLiteDuplicateDevicePath (*DevicePath);
  DevPath->SubType  = Temp;

  //
  // If DevPath is the end of an entire device path, then another instance
  // does not follow, so *DevicePath is set to NULL.
  //
  if (DevicePathSubType (DevPath) == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
    *DevicePath = NULL;
  } else {
    *DevicePath = NextDevicePathNode (DevPath);
  }

  return ReturnValue;
}

EFI_DEVICE_PATH_PROTOCOL *
OemGraphicsLiteDuplicateDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
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
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Size;

  if (DevicePath == NULL) {
    return NULL;
  }

  //
  // Compute the size
  //
  Size = OemGraphicsLiteDevicePathSize (DevicePath);
  if (Size == 0) {
    return NULL;
  }

  //
  // Allocate space for duplicate device path
  //
  NewDevicePath = EfiLibAllocateCopyPool (Size, DevicePath);

  return NewDevicePath;
}

UINTN
OemGraphicsLiteDevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
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
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!EfiIsDevicePathEnd (DevicePath)) {
    DevicePath = EfiNextDevicePathNode (DevicePath);
  }

  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}
