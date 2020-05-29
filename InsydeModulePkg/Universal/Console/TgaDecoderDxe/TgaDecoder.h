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

#ifndef _TGA_DECODER_H_
#define _TGA_DECODER_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/TgaDecoder.h>
#include <Protocol/UgaDraw.h>

#define TGA_DECODER_INSTANCE_SIGNATURE   SIGNATURE_32('t','g','a','D')
typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  //
  // Produced protocol(s)
  //
  EFI_TGA_DECODER_PROTOCOL        TgaDecoder;

} TGA_DECODER_INSTANCE;

#define PCX_DECODER_INSTANCE_FROM_THIS(This) \
  CR(This, TGA_DECODER_INSTANCE, TgaDecoder, TGA_DECODER_INSTANCE_SIGNATURE)

//
// define the screen destination of first pixel
//
#define BOTTOM_LEFT        0x00
#define BOTTOM_RIGHT       0x01
#define TOP_LEFT           0x02
#define TOP_RIGHT          0x03

//
// define TGA image type
//
#define UNCOMPRESSED_TRUE_COLOR_IMAGE           0x02
#define RUN_LENGTH_ENCODED_TRUE_COLOR_IMAGE     0x0A


EFI_STATUS
EFIAPI
TgaDecoderDecodeImage (
  IN     EFI_TGA_DECODER_PROTOCOL     *This,
  IN      UINT8                        *ImageData,
  IN      UINTN                        ImageDataSize,
     OUT  UINT8                        **DecodedData,
     OUT  UINTN                        *DecodedDataSize,
     OUT  UINTN                        *PixelHeight,
     OUT  UINTN                        *PixelWidth,
     OUT  TGA_FORMAT                   *TgaFormat,
     OUT  BOOLEAN                      *HasAlphaChannel
  );


EFI_STATUS
EFIAPI
TgaCalculateBltImage (
  IN      EFI_TGA_DECODER_PROTOCOL     *This,
  IN      UINT8                        *SourceImageData,
  IN      UINTN                        SourceImageDataSize,
  IN OUT  UINT8                        *DestinationImageData,
  IN      UINTN                        DestinationImageDataSize,
  IN      TGA_FORMAT                   TgaFormat
  );

#endif
