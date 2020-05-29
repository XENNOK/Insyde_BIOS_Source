/** @file
  EFI TGA decoder protocol

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_TGA_DECODER_H_
#define _EFI_TGA_DECODER_H_

//
// Global ID for the TGA Decoder Protocol
//
#define EFI_TGA_DECODER_PROTOCOL_GUID \
  {0xd7b3a214, 0x29b0, 0x499e, 0xa7, 0xdd, 0x73, 0x53, 0xb1, 0x66, 0x20, 0xbb}

typedef struct _EFI_TGA_DECODER_PROTOCOL EFI_TGA_DECODER_PROTOCOL;

//
// Definitions for TGA files
//
#pragma pack(1)

typedef struct {
  UINT8   Blue;
  UINT8   Green;
  UINT8   Red;
  UINT8   Alpha;
} TGA_COLOR_MAP;

typedef struct {
  CHAR8         IdLength;
  CHAR8         ColorMapType;
  CHAR8         DataTypeCode;
  UINT16        ColorMapOrigin;
  UINT16        ColorMapLength;
  CHAR8         ColorMapEntrySize;
  UINT16        XOrigin;
  UINT16        YOrigin;
  INT16         Width;
  INT16         Height;
  CHAR8         BitsPerPixel;
  CHAR8         ImageDescriptor;
} TGA_IMAGE_HEADER;

#pragma pack()

typedef enum {
  Targa16Format,
  Targa24Format,
  Targa32Format,
  UnsupportedTgaFormat
} TGA_FORMAT;


typedef
EFI_STATUS
(EFIAPI *EFI_TGA_DECODER_DECODE_IMAGE) (
  IN      EFI_TGA_DECODER_PROTOCOL     *This,
  IN      UINT8                        *ImageData,
  IN      UINTN                        ImageDataSize,
     OUT  UINT8                        **DecodedData,
     OUT  UINTN                        *DecodedDataSize,
     OUT  UINTN                        *PixelHeight,
     OUT  UINTN                        *PixelWidth,
     OUT  TGA_FORMAT                   *TgaFormat,
     OUT  BOOLEAN                      *HasAlphaChannel
  );


typedef
EFI_STATUS
(EFIAPI *EFI_TGA_DECODER_CALCULATE_BLT_IMAGE) (
  IN      EFI_TGA_DECODER_PROTOCOL     *This,
  IN      UINT8                        *SourceImageData,
  IN      UINTN                        SourceImageDataSize,
  IN OUT  UINT8                        *DestinationImageData,
  IN      UINTN                        DestinationImageDataSize,
  IN      TGA_FORMAT                   TgaFormat
  );


//
// Interface structure for the TGA Decoder Protocol
//
struct _EFI_TGA_DECODER_PROTOCOL {
  EFI_TGA_DECODER_DECODE_IMAGE         DecodeImage;
  EFI_TGA_DECODER_CALCULATE_BLT_IMAGE  CalculateBltImage;
};


extern EFI_GUID gEfiTgaDecoderProtocolGuid;

#endif
