/** @file
  This code supports a the private implementation
  of the PCX Decoder protocol

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

#include "PcxDecoder.h"
#include <Library/DebugLib.h>

PCX_DECODER_INSTANCE    mPrivateData = {
  PCX_DECODER_INSTANCE_SIGNATURE,
  NULL,
  PcxDecoderDecodeImage
};

/**
 Convert a *.PCX graphics image to a UGA blt buffer.
 If it isn't a PCX format graphics, it will return error.

 @param [in]   This
 @param [in]   ImageData        Pointer to PCX file
 @param [in]   ImageDataSize    Number of bytes in ImageData
 @param [out]  DecodedData      Buffer containing UGA version of BmpImage.
 @param [out]  DecodedDataSize  Size of DecodedData in bytes.
 @param [out]  Height           Height of DecodedData/BmpImage in pixels
 @param [out]  Width            Width of DecodedData/BmpImage in pixels

 @retval EFI_SUCCESS            DecodedData and DecodedDataSize are returned.
 @retval EFI_UNSUPPORTED        ImageData is not a valid *.PCX image

**/
EFI_STATUS
EFIAPI
PcxDecoderDecodeImage (
  IN     EFI_PCX_DECODER_PROTOCOL      *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width
  )
{
  UINT8                                 *PcxBuffer = NULL;
  EFI_UGA_PIXEL                         *ImageBuffer = NULL;
  PCX_IMAGE_HEADER                      *PcxHeader = NULL;
  EFI_UGA_PIXEL                         Palette[COLOR_NUMBER];
  UINTN                                 NPlane;
  UINTN                                 BytesPerLine;
  UINTN                                 TotalBytes;
  UINTN                                 LineBreak;
  UINTN                                 Index;
  UINTN                                 Total;
  UINTN                                 Counter;
  UINTN                                 Tmp;
  UINT64                                BltBufferSize;

  Total = 0;
  Index = 0;
  LineBreak = 0;
  //
  //  check if it is a PCX format file.
  //  Manufacturer  0x0A  - ZSoft
  //  Version       0x05  - Version 3.0
  //  Encodeing     0x01  - run length encoding
  //  BitsPerPixel  0x08  - Number of bits to represent a pixel
  //
  PcxHeader = (PCX_IMAGE_HEADER *)ImageData;
  PcxBuffer = (UINT8 *)ImageData;

  if ((PcxHeader->Manufacturer != 0x0A) ||
      (PcxHeader->Version != 0x05) ||
      (PcxHeader->Encoding != 0x01) ||
      (PcxHeader->BitsPerPixel != 0x08)) {
    return EFI_UNSUPPORTED;
  }
  //
  //  Count Width,Height...from header
  //
  *Width = PcxHeader->Xmax - PcxHeader->Xmin + 0x1;
  *Height= PcxHeader->Ymax - PcxHeader->Ymin + 0x1;
  NPlane = PcxHeader->NPlanes;
  BytesPerLine = PcxHeader->BytesPerLine;
  TotalBytes = NPlane * BytesPerLine;
  //
  //  Set palette from the end of pcx image.
  //
  for (Tmp = 0; Tmp < COLOR_NUMBER; Tmp ++) {
    Palette[Tmp].Red = PcxBuffer[ImageDataSize - PALETTE_SIZE + Tmp * 3];
    Palette[Tmp].Green = PcxBuffer[ImageDataSize - PALETTE_SIZE + Tmp * 3 + 1];
    Palette[Tmp].Blue = PcxBuffer[ImageDataSize - PALETTE_SIZE + Tmp * 3 + 2];
  }
  //
  //  read form the end of Header
  //
  PcxBuffer = &PcxBuffer[sizeof(PCX_IMAGE_HEADER)];
  BltBufferSize = (*Width) * (*Height) * sizeof (EFI_UGA_PIXEL);
  if (BltBufferSize >= 0x100000000LL) {
    return EFI_UNSUPPORTED;
  }
  ImageBuffer = (EFI_UGA_PIXEL *)AllocatePool ((UINTN) BltBufferSize);
  ASSERT (ImageBuffer != NULL);
  if (ImageBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // 24-Bit .PCX Files.
  // 24 bit images are stored as version 5 or above as 8 bit, 3 plane images.
  // 24 bit images do not contain a palette.
  // Bit planes are ordered as lines of red, green, blue in that order.
  //
  if ((PcxHeader->NPlanes == 0x03) && (PcxHeader->BitsPerPixel == 0x08)) {
    UINT8 *ScanLine;
    UINTN Xaxis;
    UINTN Yaxis;
    UINTN i;
    UINT8 Value;

    ScanLine = (UINT8 *)AllocateZeroPool ((UINTN) TotalBytes + 0x32);
    if (ScanLine == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Yaxis = 0; Yaxis < *Height; Yaxis++) {
      i = 0;
      while (i < TotalBytes) {
        Counter = 1;
        Value   = PcxBuffer[Index];
        if ((Value & 0xC0) == 0xC0) {
          Counter = (UINTN) (Value & 0x3F);
          Index++;
          Value = PcxBuffer[Index];
        }
        while ((i < TotalBytes) && Counter--) {
          ScanLine[i++] = Value;
        }
        Index++;
      }

      for (Xaxis = 0; Xaxis < *Width; Xaxis++) {
        ImageBuffer[(Yaxis * (*Width)) + Xaxis].Red   = ScanLine[Xaxis];
        ImageBuffer[(Yaxis * (*Width)) + Xaxis].Green = ScanLine[Xaxis + BytesPerLine];
        ImageBuffer[(Yaxis * (*Width)) + Xaxis].Blue  = ScanLine[Xaxis + (BytesPerLine << 1)];
      }

    }

    *DecodedData = (UINT8 *)ImageBuffer;
    FreePool (ScanLine);
    return EFI_SUCCESS;
  }

  while (Total < ((*Width) * (*Height))) {
    //
    //  skip the blank area at the end of scanline
    //
    if (LineBreak == (*Width)) {
      while (LineBreak < TotalBytes) {
        LineBreak++;
        Index++;
      }
      LineBreak = 0;
    }
    //
    //  decode the color map
    //
    if ((PcxBuffer[Index] & 0xC0) == 0xC0) {
      Counter = (UINTN) (PcxBuffer[Index] & 0x3F);
      Index++;
      for (Tmp = 0; Tmp < Counter; Tmp++) {
        ImageBuffer[Total] = Palette[PcxBuffer[Index]];
        Total++;
        LineBreak++;
        if (LineBreak == (*Width)) {
          break;
        }
      }
    } else {
      ImageBuffer[Total] = Palette[PcxBuffer[Index]];
      Total++;
      LineBreak++;
    }
    Index++;
  }
  //
  //  Set return buffer
  //
  *DecodedData = (UINT8 *)ImageBuffer;
  return EFI_SUCCESS;
}


/**
 Install Driver to produce PCX Decoder protocol.

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            PCX Decoder protocol installed
 @return Other        No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
PcxDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiPcxDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.PcxDecoder
                  );
  return Status;
}