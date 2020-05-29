/** @file

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

    BmpDecoder.h

Abstract:

    EFI BMP Decoder Protocol

Revision History

--*/

#ifndef _EFI_BMP_DECODER_H
#define _EFI_BMP_DECODER_H

//
// Global ID for the Bmp Decoder Protocol
//
#define EFI_BMP_DECODER_PROTOCOL_GUID \
  { 0xa6396a81, 0x8031, 0x4fd7, 0xbd, 0x14, 0x2e, 0x6b, 0xfb, 0xec, 0x83, 0xc2 }
//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_BMP_DECODER_PROTOCOL EFI_BMP_DECODER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_BMP_DECODER_DECODE_IMAGE) (
  IN     EFI_BMP_DECODER_PROTOCOL     *This,
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
struct _EFI_BMP_DECODER_PROTOCOL {
  EFI_BMP_DECODER_DECODE_IMAGE         DecodeImage;
};


extern EFI_GUID gEfiBmpDecoderProtocolGuid;

#endif
