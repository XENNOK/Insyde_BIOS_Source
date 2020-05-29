/** @file
  Gif processor header

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

#ifndef _EFI_GIF_H_
#define _EFI_GIF_H_

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

#define GIF_TRAILER                      0x3B
#define EXTENSION_INTRODUCER             0x21
#define GRAPHIC_CONTROL_LABEL            0xF9
#define IMAGE_SEPARATOR                  0x2C

//
// LZW decoder relative definition
//
#define NONCODE 0xFFFF

typedef struct _LZW_ENTRY {
  INT16  Prefix;
  UINT8  Suffix;
} LZW_ENTRY;

#pragma pack(1)

//
// GIF header
//
typedef struct _GIF_HEADER {
  UINT8  Signature[3];
  UINT8  Version[3];
} GIF_HEADER;

//
// Logical screen descriptor
//
typedef struct _GIF_LOGICAL_SCREEN_PACKED_FIELDS {
  UINT8 SizeOfGlobalColorTable : 3;
  UINT8 SortFlag               : 1;
  UINT8 ColorResolution        : 3;
  UINT8 GlobalColorTableFlag   : 1;
} GIF_LOGICAL_SCREEN_PACKED_FIELDS;

typedef struct _GIF_LOGICAL_SCREEN_DESCRIPTOR {
  UINT16                           LogicalScreenWidth;
  UINT16                           LogicalScreenHeight;
  GIF_LOGICAL_SCREEN_PACKED_FIELDS PackedFields;
  UINT8                            BackGroundColorIndex;
  UINT8                            PixelAspectRatio;
} GIF_LOGICAL_SCREEN_DESCRIPTOR;

//
// Graphic control extension
//
typedef struct _GIF_GRAPHIC_CONTROL_PACKED_FIELDS {
  UINT8 TransparentColorFlag : 1;
  UINT8 UserInputFlag        : 1;
  UINT8 DisposalMethod       : 3;
  UINT8 Reserved             : 3;
} GIF_GRAPHIC_CONTROL_PACKED_FIELDS;

typedef struct _GIF_GRAPHIC_CONTROL_EXTENSION {
  UINT8                              ExtensionIntroducer;
  UINT8                              GraphicControlLabel;
  UINT8                              BlockSize;
  GIF_GRAPHIC_CONTROL_PACKED_FIELDS  PackedFields;
  UINT16                             DelayTime;
  UINT8                              TransparentColorIndex;
  UINT8                              BlockTerminator;
} GIF_GRAPHIC_CONTROL_EXTENSION;


//
// Image descriptor
//
typedef struct _GIF_IMAGE_PACKED_FIELDS {
  UINT8 SizeOfLocalColorTable : 3;
  UINT8 Reserved              : 2;
  UINT8 SortFlag              : 1;
  UINT8 InterlaceFlag         : 1;
  UINT8 LocalColorTableFlag   : 1;
} GIF_IMAGE_PACKED_FIELDS;


typedef struct _GIF_IMAGE_DESCRIPTOR {
  UINT8                    ImageSeparator;
  UINT16                   ImageLeftPosition;
  UINT16                   ImageTopPosition;
  UINT16                   ImageWidth;
  UINT16                   ImageHeight;
  GIF_IMAGE_PACKED_FIELDS  PackedFields;
} GIF_IMAGE_DESCRIPTOR;

#pragma pack()

extern UINT8      *mOutStack;
extern LZW_ENTRY  *mLzwTable;


EFI_STATUS
GifDecoderGetLogicalScreen (
  IN OUT UINT8                         **GifFileImage, 
  IN OUT UINTN                         *GifFileSize,
  OUT    GIF_LOGICAL_SCREEN_DESCRIPTOR *GifScreen,
  OUT    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *GlobalColorMap
  );

EFI_STATUS 
GifDecoderGetFirstGraphicControl (
  IN  UINT8                           *FileImage, 
  IN  UINTN                           FileSize, 
  OUT GIF_GRAPHIC_CONTROL_EXTENSION   *GraphicControl 
  );

EFI_STATUS 
GifDecoderGetImageDesc (
  IN      UINT8                           **GifFileImage, 
  IN      UINTN                           *GifFileSize, 
  OUT     GIF_IMAGE_DESCRIPTOR            *ImageDesc, 
  OUT     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *LocalColorMap,
  OUT     GIF_GRAPHIC_CONTROL_EXTENSION   *GraphicControl
  );

EFI_STATUS
GifDecoderGetImageData (
  IN     UINT8                           **GifFileImage, 
  IN     UINTN                           *GifFileSize, 
  IN     GIF_IMAGE_DESCRIPTOR            *ImageDesc, 
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *GlobalColorMap, 
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *LocalColorMap,
  IN     GIF_GRAPHIC_CONTROL_EXTENSION   *GraphicControl, 
  OUT    EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *ImageData, 
  IN     UINT32                           ImageSize,
  IN     UINT8                           *CompressedData,
  IN OUT UINTN                           *CompressedDataSize
  );

#endif                                                       
