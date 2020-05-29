/** @file
  PNG Decoder protocol

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_PNG_DECODER_H
#define _EFI_PNG_DECODER_H

//
// Global ID for the PNG Decoder Protocol
//
#define EFI_PNG_DECODER_PROTOCOL_GUID \
  {0xdb585f02, 0x1dd1, 0x41e2, 0xa7, 0xe5, 0xd4, 0x7b, 0x79, 0x08, 0xcf, 0x7c}

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_PNG_DECODER_PROTOCOL EFI_PNG_DECODER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_PNG_DECODER_DECODE_IMAGE) (
  IN     EFI_PNG_DECODER_PROTOCOL      *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width
  );

//
// Interface structure for the PNG Decoder Protocol
//
struct _EFI_PNG_DECODER_PROTOCOL {
  EFI_PNG_DECODER_DECODE_IMAGE         DecodeImage;
};


extern EFI_GUID gEfiPngDecoderProtocolGuid;

#endif
