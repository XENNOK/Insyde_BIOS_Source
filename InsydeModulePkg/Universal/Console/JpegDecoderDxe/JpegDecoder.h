/** @file

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
  This code supports a the private implementation
  of the JPEG Decoder protocol

--*/

#ifndef _JPEG_DECODER_H_
#define _JPEG_DECODER_H_

#include "JfifDecode.h"


#define JPEG_DECODER_INSTANCE_SIGNATURE   SIGNATURE_32('J','p','g','D')
typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  //
  // Produced protocol(s)
  //
  EFI_JPEG_DECODER_PROTOCOL           JpegDecoder;

} JPEG_DECODER_INSTANCE;

#define JPEG_DECODER_INSTANCE_FROM_THIS(this) \
  CR(this, JPEG_DECODER_INSTANCE, JpegDecoder, JPEG_DECODER_INSTANCE_SIGNATURE)


EFI_STATUS
EFIAPI
JpegDecoderDecodeImage (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width,
     OUT EFI_JPEG_DECODER_STATUS       *DecoderStatus
  );

EFI_STATUS
EFIAPI
JpegDecoderGetMarkerData (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *Start,
  IN     UINT8                         *End,
  IN OUT EFI_JPEG_MARKER_TYPE          *MarkerType,
     OUT UINT8                         **MarkerData,
     OUT UINT32                        *DataSize,
     OUT UINT8                         **Next  OPTIONAL
  );


#endif
