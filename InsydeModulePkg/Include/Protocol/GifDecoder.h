/** @file
  EFI Gif decoder protocol

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

#ifndef _EFI_GIF_DECODER_H_
#define _EFI_GIF_DECODER_H_

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

//
// Global ID for the GIF Decoder Protocol
//
#define EFI_GIF_DECODER_PROTOCOL_GUID \
  {0xd3e104cb, 0xd03e, 0x44b3, 0x85, 0xcf, 0x13, 0x6, 0x74, 0x84, 0xcb, 0x11}


//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_GIF_DECODER_PROTOCOL EFI_GIF_DECODER_PROTOCOL;

#define ANIM_STATUS_STOP  0
#define ANIM_STATUS_PLAY  1

typedef struct _ANIMATION_FRAME {
  UINT8                          Disposal; 
  UINT16                         OffsetX; 
  UINT16                         OffsetY; 
  UINT16                         Width; 
  UINT16                         Height; 
  UINT16                         Delay; 
  BOOLEAN                        Transparent; 
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *Bitmap; 
  struct _ANIMATION_FRAME        *Prev; 
  struct _ANIMATION_FRAME        *Next;     
} ANIMATION_FRAME; 

typedef struct {
  LIST_ENTRY                         Link;
  VOID                               *Data; 
  UINTN                              Width;
  UINTN                              Height; 
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      BkColor;   
  ANIMATION_FRAME                    *Frames;  
} ANIMATION; 

typedef struct {  
  LIST_ENTRY         Link;
  VOID               *Data;
  ANIMATION          *Animation;
  ANIMATION_FRAME    *Current; 
  UINTN              BltWidth;
  UINTN              BltHeight;
  UINTN              X;
  UINTN              Y;
  UINT32             Status; 
  UINT64             RecordTick; 
  BOOLEAN            AutoLoop; 
} ANIMATION_REFRESH_ENTRY; 


typedef
EFI_STATUS
(EFIAPI *EFI_GIF_DECODER_CREATE_ANIMATION_FROM_MEM) (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      UINT8                        *FileData,
  IN      UINTN                        FileSize,
  IN      VOID                         *Data, 
     OUT  ANIMATION                    **Animation  
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GIF_DECODER_DESTROY_ANIMATION) (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION                    *Animation
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GIF_DECODER_NEXT_ANIMATION_FRAME) (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION_REFRESH_ENTRY      *AnimationRefrshEntry,
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop  
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GIF_DECODER_REFRESH_ANIMATION) (
  IN      EFI_GIF_DECODER_PROTOCOL     *This,
  IN      ANIMATION_REFRESH_ENTRY      *AnimationRefrshEntry,
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
  IN      UINT64                       CpuFrequency OPTIONAL     
  );


//
// Interface structure for the GIF Decoder Protocol
//
struct _EFI_GIF_DECODER_PROTOCOL {
  EFI_GIF_DECODER_CREATE_ANIMATION_FROM_MEM     CreateAnimationFromMem; 
  EFI_GIF_DECODER_DESTROY_ANIMATION             DestroyAnimation; 
  EFI_GIF_DECODER_NEXT_ANIMATION_FRAME          NextAnimationFrame; 
  EFI_GIF_DECODER_REFRESH_ANIMATION             RefreshAnimation;   
};


extern EFI_GUID gEfiGifDecoderProtocolGuid;

#endif
