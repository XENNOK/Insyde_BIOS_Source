/** @file
  This code supports a the private implementation
  of the GIF Decoder protocol

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

#ifndef _GIF_DECODER_H_
#define _GIF_DECODER_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/GifDecoder.h>
#include <Protocol/GraphicsOutput.h>

#define GIF_DECODER_INSTANCE_SIGNATURE   SIGNATURE_32('g','i','f','D')

typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  //
  // Produced protocol(s)
  //
  EFI_GIF_DECODER_PROTOCOL        GifDecoder;

} GIF_DECODER_INSTANCE;

#define GIF_DECODER_INSTANCE_FROM_THIS(This) \
  CR(This, GIF_DECODER_INSTANCE, GifDecoder, GIF_DECODER_INSTANCE_SIGNATURE)

EFI_STATUS
EFIAPI
CreateAnimationFromMem (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      UINT8                        *FileData,
  IN      UINTN                        FileSize,
  IN      VOID                         *Data, 
     OUT  ANIMATION                    **Animation
  );

EFI_STATUS
EFIAPI
DestroyAnimation (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION                    *Animation
  );

EFI_STATUS
EFIAPI
NextAnimationFrame (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION_REFRESH_ENTRY      *Entry,
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop 
  );

EFI_STATUS 
EFIAPI
RefreshAnimation (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION_REFRESH_ENTRY      *AnimationRefrshEntry,
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
  IN      UINT64                       CpuFrequency  
  );

#endif
