//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    JpegDecoder.h

Abstract:

    EFI JPEG Decoder Protocol

Revision History

--*/

#ifndef _EFI_JPEG_DECODER_H
#define _EFI_JPEG_DECODER_H

//
// Global ID for the JPEG Decoder Protocol
//
#define EFI_JPEG_DECODER_PROTOCOL_GUID \
  { 0xa9396a81, 0x6231, 0x4dd7, 0xbd, 0x9b, 0x2e, 0x6b, 0xf7, 0xec, 0x73, 0xc2 }


EFI_FORWARD_DECLARATION (EFI_JPEG_DECODER_PROTOCOL);

//
// JPEG decoder status for the JPEG Decoder Protocol
//

typedef enum {
  EFI_JPEG_DECODE_SUCCESS          = 0,
  EFI_JPEG_INVALID_PARAMETER       = 1,
  EFI_JPEG_IMAGE_UNSUPPORTED       = 2,
  EFI_JPEG_QUANTIZATIONTABLE_ERROR = 3,
  EFI_JPEG_HUFFMANTABLE_ERROR      = 4,
  EFI_JPEG_SOS_ERROR               = 5,
  EFI_JPEG_SOF_ERROR               = 6,
  EFI_JPEG_DECODEDATA_ERROR        = 7,
  EFI_JPEG_DECODEHT_ERROR          = 8,
  EFI_JPEG_DECODERST_ERROR         = 9,
  EFI_JPEG_DECODEAC_ERROR          = 10
} EFI_JPEG_DECODER_STATUS;

//
// JPEG marker type for the JPEG Decoder Protocol
//
typedef enum {
  JPEG_ANY   = 0x0,
  JPEG_SOF0  = 0xc0,

  JPEG_DHT   = 0xc4,

  JPEG_RST0  = 0xd0,
  JPEG_RST1  = 0xd1,
  JPEG_RST2  = 0xd2,
  JPEG_RST3  = 0xd3,
  JPEG_RST4  = 0xd4,
  JPEG_RST5  = 0xd5,
  JPEG_RST6  = 0xd6,
  JPEG_RST7  = 0xd7,

  JPEG_SOI   = 0xd8,
  JPEG_EOI   = 0xd9,
  JPEG_SOS   = 0xda,
  JPEG_DQT   = 0xdb,
  JPEG_DNL   = 0xdc,
  JPEG_DRI   = 0xdd,

  JPEG_APP0  = 0xe0,
  JPEG_APP1  = 0xe1,
  JPEG_APP2  = 0xe2,
  JPEG_APP3  = 0xe3,
  JPEG_APP4  = 0xe4,
  JPEG_APP5  = 0xe5,
  JPEG_APP6  = 0xe6,
  JPEG_APP7  = 0xe7,
  JPEG_APP8  = 0xe8,
  JPEG_APP9  = 0xe9,
  JPEG_APP10 = 0xea,
  JPEG_APP11 = 0xeb,
  JPEG_APP12 = 0xec,
  JPEG_APP13 = 0xed,
  JPEG_APP14 = 0xee,
  JPEG_APP15 = 0xef,

  JPEG_COM   = 0xfe,

 } EFI_JPEG_MARKER_TYPE;

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_JPEG_DECODER_PROTOCOL EFI_JPEG_DECODER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_JPEG_DECODER_DECODE_IMAGE) (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width,
     OUT EFI_JPEG_DECODER_STATUS       *DecoderStatus
  );

typedef
EFI_STATUS
(EFIAPI *EFI_JPEG_DECODER_GET_MARKER_DATA) (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *Start,
  IN     UINT8                         *End,
  IN OUT EFI_JPEG_MARKER_TYPE          *MarkerType,
     OUT UINT8                         **MarkerData,
     OUT UINT32                        *DataSize,
     OUT UINT8                         **Next  OPTIONAL
  );

//
// Interface structure for the JPEG Decoder Protocol
//
typedef struct _EFI_JPEG_DECODER_PROTOCOL {
  EFI_JPEG_DECODER_DECODE_IMAGE         DecodeImage;
  EFI_JPEG_DECODER_GET_MARKER_DATA      GetMarkerData;
} EFI_JPEG_DECODER_PROTOCOL;


extern EFI_GUID gEfiJpegDecoderProtocolGuid;

#endif
