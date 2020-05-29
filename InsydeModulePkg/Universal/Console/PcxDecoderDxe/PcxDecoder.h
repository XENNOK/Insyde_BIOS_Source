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

#ifndef _PCX_DECODER_H_
#define _PCX_DECODER_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/PcxDecoder.h>
#include <Protocol/UgaDraw.h>

#include <Guid/Pcx.h>

//  256 color, 256*3 bytes Palette.
#define COLOR_NUMBER          256
#define PALETTE_SIZE          0x300

#define PCX_DECODER_INSTANCE_SIGNATURE   SIGNATURE_32('P','c','x','D')
typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  //
  // Produced protocol(s)
  //
  EFI_PCX_DECODER_PROTOCOL           PcxDecoder;

} PCX_DECODER_INSTANCE;

#define PCX_DECODER_INSTANCE_FROM_THIS(This) \
  CR(This, PCX_DECODER_INSTANCE, PcxDecoder, PCX_DECODER_INSTANCE_SIGNATURE)


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
  );

#endif
