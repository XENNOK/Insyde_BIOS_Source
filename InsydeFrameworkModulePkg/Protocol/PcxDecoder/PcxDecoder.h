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
//; Abstract:
//;   PCX Decoder protocol
//;

#ifndef _EFI_PCX_DECODER_H
#define _EFI_PCX_DECODER_H

//
// Global ID for the PCX Decoder Protocol
//
#define EFI_PCX_DECODER_PROTOCOL_GUID \
  {0x5cba0791, 0xe45b, 0x4b3b, 0xbe, 0xdb, 0x3, 0xfd, 0x2c, 0xdb, 0x53, 0x31}


EFI_FORWARD_DECLARATION (EFI_PCX_DECODER_PROTOCOL);

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_PCX_DECODER_PROTOCOL EFI_PCX_DECODER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_PCX_DECODER_DECODE_IMAGE) (
  IN     EFI_PCX_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width
  );

//
// Interface structure for the PCX Decoder Protocol
//
typedef struct _EFI_PCX_DECODER_PROTOCOL {
  EFI_PCX_DECODER_DECODE_IMAGE         DecodeImage;
} EFI_PCX_DECODER_PROTOCOL;


extern EFI_GUID gEfiPcxDecoderProtocolGuid;

#endif
