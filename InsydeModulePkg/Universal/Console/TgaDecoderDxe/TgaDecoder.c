/** @file
  This code supports a the private implementation
  of the TGA Decoder protocol

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

#include "TgaDecoder.h"

TGA_DECODER_INSTANCE    mPrivateData = {
  TGA_DECODER_INSTANCE_SIGNATURE,
  NULL,
  {
   TgaDecoderDecodeImage,
   TgaCalculateBltImage
  }
};


/**
 Install Driver to produce Tga Decoder protocol.

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            TGA Decoder protocol installed
 @return Other        No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
TgaDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;


  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiTgaDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.TgaDecoder
                  );
  return Status;
}

/**
 According TGA format to convert pixel data from TGA image file and save the
 pixle data in Pixel arrary.

 @param [out]  Pixel            Pointer to TGA_COLOR_MAP structure
 @param [in]   Image            Pointer to TGA file pixle data
 @param [in]   TgaFormat        TGA file format type.

 @retval EFI_SUCCESS            Convert TGA data successful.
 @retval EFI_UNSUPPORTED        TGA format type doesn't support
 @retval EFI_INVALID_PARAMETER  Invalid input pointer

**/
EFI_STATUS
ConvertTgaPixel (
     OUT TGA_COLOR_MAP            *Pixel,
  IN     UINT8                    *Image,
  IN     TGA_FORMAT               TgaFormat
  )
{
   if (Pixel == NULL || Image == NULL) {
     return EFI_INVALID_PARAMETER;
   }
   switch (TgaFormat) {

   case Targa16Format:
     Pixel->Blue = (Image[0] & 0x1F) << 3;
     Pixel->Green = ((Image[1] & 0x03) << 6) | ((Image[0] & 0xE0) >> 2);
     Pixel->Red = (Image[1] & 0x7C) << 1;
     Pixel->Alpha = (Image[1] & 0x80);
     break;

   case Targa24Format:
     Pixel->Blue  = Image[0];
     Pixel->Green = Image[1];
     Pixel->Red   = Image[2];
     Pixel->Alpha = 0;
     break;

   case Targa32Format:
     Pixel->Blue  = Image[0];
     Pixel->Green = Image[1];
     Pixel->Red   = Image[2];
     Pixel->Alpha = Image[3];
     break;

   default:
     return EFI_UNSUPPORTED;
   }

   return EFI_SUCCESS;
}

VOID
ReverseColorArray (
  IN TGA_COLOR_MAP *ColorMap,
  IN UINTN         Length
  )
{
  TGA_COLOR_MAP     *Left;
  TGA_COLOR_MAP     *Right;
  TGA_COLOR_MAP     Temp;

  for (Left = ColorMap, Right = ColorMap + Length - 1; Left < Right; Left++, Right--) {
    Temp = *Left;
    *Left = *Right;
    *Right = Temp;
  }
}

/**
 Convert a *.TGA graphics image to a TGA DecodedData buffer. If a NULL DecodedData buffer
 is passed in a DecodedData buffer will be allocated by this routine. If a DecodedData
 buffer is passed in it will be used if it is big enough.

 @param [in]   This             Pointer to protocol instance
 @param [in]   ImageData        Pointer to TGA file
 @param [in]   ImageDataSize    Number of bytes in ImageData
 @param [in, out] DecodedData   Buffer containing Tga pixle color map.
 @param [in, out] DecodedDataSize  Size of DecodedData in bytes.
 @param [out]  PixelHeight      Height of DecodedData in pixels
 @param [out]  PixelWidth       Width of DecodedData in pixels
 @param [out]  TgaFormat        TGA file format
 @param [out]  HasAlphaChannel  This image type has alpha channel or not

 @retval EFI_SUCCESS            DecodedData and DecodedDataSize are returned.
 @retval EFI_INVALID_PARAMETER  Input parameter is invalid
 @retval EFI_UNSUPPORTED        ImageData is not a valid *.TGA image
 @retval EFI_BUFFER_TOO_SMALL   The passed in DecodedData buffer is not big enough.
                                DecodedDataSize will contain the required size.
 @retval EFI_OUT_OF_RESOURCES   No enough buffer to allocate

**/
EFI_STATUS
EFIAPI
TgaDecoderDecodeImage (
  IN      EFI_TGA_DECODER_PROTOCOL     *This,
  IN      UINT8                        *ImageData,
  IN      UINTN                        ImageDataSize,
  IN OUT  UINT8                        **DecodedData,
  IN OUT  UINTN                        *DecodedDataSize,
     OUT  UINTN                        *PixelHeight,
     OUT  UINTN                        *PixelWidth,
     OUT  TGA_FORMAT                   *TgaFormat,
     OUT  BOOLEAN                      *HasAlphaChannel
  )
{
  UINT8                         *Image;
  UINT64                         BltBufferSize;
  TGA_IMAGE_HEADER              *TgaHeader;
  TGA_COLOR_MAP                 *TgaBuffer;
  TGA_COLOR_MAP                 *TempTgaBuffer;
  TGA_COLOR_MAP                 *Src;
  TGA_COLOR_MAP                 *Dst;
  UINTN                         BytesPerLine;
  UINT8                         BytesToRead;
  UINTN                         Index;
  UINTN                         PixelCnt;
  UINTN                         Offset;
  UINT8                         ImageOrigin;
  UINTN                         EntrySize;

  TgaHeader = (TGA_IMAGE_HEADER *) ImageData;

  if (ImageData == NULL || PixelHeight == NULL || PixelWidth == NULL ||
      TgaFormat == NULL || HasAlphaChannel == NULL) {

    return EFI_INVALID_PARAMETER;
  }
  //
  // Only support true-color image, and to comply with TGA 1.0, we don't check
  // TGA signature TRUEVISION-XFILE
  //
  if (TgaHeader->DataTypeCode != UNCOMPRESSED_TRUE_COLOR_IMAGE &&
      TgaHeader->DataTypeCode != RUN_LENGTH_ENCODED_TRUE_COLOR_IMAGE) {

    return EFI_UNSUPPORTED;
  }

  if (TgaHeader->ColorMapType != 0 && TgaHeader->ColorMapType != 1) {

    return EFI_UNSUPPORTED;
  }

  //
  // Check the true-color TGA format and only support TARGA16, TARGA24 and TARGA32 format
  //
  if (TgaHeader->BitsPerPixel == 16) {
    *TgaFormat = Targa16Format;
    *HasAlphaChannel = TRUE;
  } else if (TgaHeader->BitsPerPixel == 24) {
    *TgaFormat = Targa24Format;
    *HasAlphaChannel = FALSE;
  } else if (TgaHeader->BitsPerPixel == 32) {
    *TgaFormat = Targa32Format;
    *HasAlphaChannel = TRUE;
  } else {
    *TgaFormat = UnsupportedTgaFormat;
    *HasAlphaChannel = FALSE;

    return EFI_UNSUPPORTED;
  }

  BltBufferSize = TgaHeader->Width * TgaHeader->Height * sizeof(TGA_COLOR_MAP);
  if (BltBufferSize >= 0x100000000LL) {
     //
     // The buffer size extends the limitation
     //
     return EFI_UNSUPPORTED;
  }
  if (*DecodedData == NULL) {
    *DecodedDataSize = (UINTN) BltBufferSize;
    *DecodedData     = (UINT8 *)AllocatePool (*DecodedDataSize);
    if (*DecodedData == NULL) {

      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    if (*DecodedDataSize < (UINTN) BltBufferSize) {
      *DecodedDataSize = (UINTN) BltBufferSize;

      return EFI_BUFFER_TOO_SMALL;
    }
  }

  TempTgaBuffer = (TGA_COLOR_MAP *)AllocatePool (*DecodedDataSize);
  if (TempTgaBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }


  *PixelHeight = (UINTN) TgaHeader->Height;
  *PixelWidth = (UINTN) TgaHeader->Width;

  //
  // Skip unnecessary data (Image ID and Color Map Data) and pointer to image data.
  //
  Image = ImageData;
  Image = Image + sizeof (TGA_IMAGE_HEADER) + TgaHeader->IdLength;
  EntrySize = (TgaHeader->ColorMapEntrySize % 8 == 0) ? TgaHeader->ColorMapEntrySize / 8 :
              TgaHeader->ColorMapEntrySize / 8 + 1;
  Image = Image + (EntrySize * TgaHeader->ColorMapType * TgaHeader->ColorMapLength);

  TgaBuffer = (TGA_COLOR_MAP *) *DecodedData;
  BytesToRead = TgaHeader->BitsPerPixel / 8;

  switch (TgaHeader->DataTypeCode) {

  //
  // Convert True-color uncompressed TGA image
  //
  case UNCOMPRESSED_TRUE_COLOR_IMAGE:
    for (Offset = 0; Offset < (UINTN) TgaHeader->Height * TgaHeader->Width; Offset++) {
       ConvertTgaPixel (&(TempTgaBuffer[Offset]) , Image, *TgaFormat);
       Image += BytesToRead;
    }
    break;

  //
  // Convert True-color compressed TGA image
  //
  case RUN_LENGTH_ENCODED_TRUE_COLOR_IMAGE:
    Offset = 0;
    while (Offset < (UINTN) TgaHeader->Height * TgaHeader->Width) {
      PixelCnt = *Image & 0x7F;
      ConvertTgaPixel (&(TempTgaBuffer[Offset]) , Image + 1, *TgaFormat);
      Offset++;
      //
      // Raw packet
      //
      if ((*Image & 0x80) == 0) {
        Image += (BytesToRead + 1) ;
        for (Index = 0; Index < PixelCnt; Index++) {
          ConvertTgaPixel (&(TempTgaBuffer[Offset]) , Image, *TgaFormat);
          Image += BytesToRead;
          Offset++;
        }
      //
      // Run-Length packet
      //
      } else {
        for (Index = 0; Index < PixelCnt; Index++) {
          ConvertTgaPixel (&(TempTgaBuffer[Offset]) , Image + 1, *TgaFormat);
          Offset++;
        }
        Image += (BytesToRead + 1);
      }
    }
    break;
  }

  //
  // Adjust image according the screen destination of first pixel
  //
  ImageOrigin = (TgaHeader->ImageDescriptor & 0x30) >> 4;
  Offset = 0;
  BytesPerLine = TgaHeader->Width * sizeof (TGA_COLOR_MAP);

  switch (ImageOrigin) {

  case BOTTOM_LEFT:
    Src = TempTgaBuffer;
    Dst = TgaBuffer + (TgaHeader->Height - 1) * TgaHeader->Width;
    for (Index = 0; Index < (UINTN)TgaHeader->Height; Index++, Dst-=TgaHeader->Width, Src+=TgaHeader->Width) {
      CopyMem (Dst, Src, BytesPerLine);
    }
    gBS->FreePool (TempTgaBuffer);
    break;

  case BOTTOM_RIGHT:
    Src = TempTgaBuffer;
    Dst = TgaBuffer + (TgaHeader->Height - 1) * TgaHeader->Width;
    for (Index = 0; Index < (UINTN)TgaHeader->Height; Index++, Dst-=TgaHeader->Width, Src+=TgaHeader->Width) {
      CopyMem (Dst, Src, BytesPerLine);
      ReverseColorArray (Dst, TgaHeader->Width);
    }
    gBS->FreePool (TempTgaBuffer);
    break;

  case TOP_LEFT:
    //
    // It doesn't need adjust, so point to TempTgaBuffer directly
    //
    *DecodedData = (UINT8 *) TempTgaBuffer;
    gBS->FreePool (TgaBuffer);
    break;

  case TOP_RIGHT:

    //
    // reverse array
    //
    Src = TempTgaBuffer;
    for (Index = 0; Index < (UINTN)TgaHeader->Height; Index++, Src+=TgaHeader->Width) {
      ReverseColorArray (Src, TgaHeader->Width);
    }
    *DecodedData = (UINT8 *) TempTgaBuffer;
    gBS->FreePool (TgaBuffer);
    break;
  }

  return EFI_SUCCESS;
}

/**
 Based on source Image and destionation image to Calculate the final blt
 buffer and the final blt data is put in DestinationImageData

 @param [in]   This             Pointer to protocol instance
 @param [in]   SourceImageData  Pointer to source image data (Converted data which is read
                                from file)
 @param [in]   SourceImageDataSize  Total size of source image data
 @param [in, out] DestinationImageData  IN:  Pointer to destination image data(read from video)
                                OUT: The final blt data
 @param [in]   DestinationImageDataSize  Size of DecodedData in bytes.
 @param [in]   TgaFormat        Tga file format

 @retval EFI_SUCCESS            Calculate final blt data successfully.
 @retval EFI_INVALID_PARAMETER  Input parameter is invalid
 @retval EFI_UNSUPPORTED        It doesn't support this TGA format

**/
EFI_STATUS
EFIAPI
TgaCalculateBltImage (
  IN      EFI_TGA_DECODER_PROTOCOL     *This,
  IN      UINT8                        *SourceImageData,
  IN      UINTN                        SourceImageDataSize,
  IN OUT  UINT8                        *DestinationImageData,
  IN      UINTN                        DestinationImageDataSize,
  IN      TGA_FORMAT                   TgaFormat
  )
{
  UINTN             Index;
  EFI_UGA_PIXEL     *Destination;
  TGA_COLOR_MAP     *Source;

  if (SourceImageData == NULL || DestinationImageData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!((TgaFormat == Targa16Format) || (TgaFormat == Targa24Format) ||
      (TgaFormat == Targa32Format))) {
     return EFI_UNSUPPORTED;
  }

  Destination = (EFI_UGA_PIXEL *) DestinationImageData;
  Source      = (TGA_COLOR_MAP *) SourceImageData;

  //
  // Calculate finally Blt buffer
  //
  if (TgaFormat == Targa32Format) {
    for (Index = 0; Index < DestinationImageDataSize / sizeof (EFI_UGA_PIXEL); Index++) {
      Destination[Index].Blue  = Destination[Index].Blue * (0xFF - Source[Index].Alpha) / 0xFF +
                                 Source[Index].Blue * Source[Index].Alpha / 0xFF;
      Destination[Index].Red   = Destination[Index].Red * (0xFF - Source[Index].Alpha) / 0xFF +
                                 Source[Index].Red * Source[Index].Alpha / 0xFF;
      Destination[Index].Green = Destination[Index].Green * (0xFF - Source[Index].Alpha) / 0xFF +
                                 Source[Index].Green * Source[Index].Alpha / 0xFF;
    }
  }

  if (TgaFormat == Targa24Format) {
    for (Index = 0; Index < DestinationImageDataSize / sizeof (EFI_UGA_PIXEL); Index++) {
      Destination[Index].Blue = Source[Index].Blue;
      Destination[Index].Red = Source[Index].Red;
      Destination[Index].Green = Source[Index].Green;
    }
  }

  if (TgaFormat == Targa16Format) {
    for (Index = 0; Index < DestinationImageDataSize / sizeof (EFI_UGA_PIXEL); Index++) {
      Destination[Index].Blue  = (Source[Index].Alpha == 0) ? Source[Index].Blue : Destination[Index].Blue;
      Destination[Index].Red   = (Source[Index].Alpha == 0) ? Source[Index].Red : Destination[Index].Red;
      Destination[Index].Green = (Source[Index].Alpha == 0) ? Source[Index].Green : Destination[Index].Green;
    }
  }

  return EFI_SUCCESS;
}

