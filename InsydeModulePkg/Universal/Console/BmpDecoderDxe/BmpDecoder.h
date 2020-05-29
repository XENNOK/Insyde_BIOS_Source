/** @file
  This code supports a the private implementation
  of the BMP Decoder protocol

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

#ifndef _BMP_DECODER_H_
#define _BMP_DECODER_H_

#include <Uefi.h>
#include <IndustryStandard/Bmp.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/BmpDecoder.h>
#include <Protocol/GraphicsOutput.h>

#define BMP_DECODER_INSTANCE_SIGNATURE   SIGNATURE_32('B','m','p','D')

typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  //
  // Produced protocol(s)
  //
  EFI_BMP_DECODER_PROTOCOL           BmpDecoder;
} BMP_DECODER_INSTANCE;

#define BMP_DECODER_INSTANCE_FROM_THIS(This) \
  CR(This, BMP_DECODER_INSTANCE, BmpDecoder, BMP_DECODER_INSTANCE_SIGNATURE)

EFI_STATUS
EFIAPI
BmpDecoderDecodeImage (
  IN     EFI_BMP_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width
  );

#endif
