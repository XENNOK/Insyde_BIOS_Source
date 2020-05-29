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
//; Module Name:
//;
//;   EfiOEMBadgingSupport.h
//;
//; Abstract:
//;
#ifndef _EFI_OEM_BADGING_SUPPORT_H_
#define _EFI_OEM_BADGING_SUPPORT_H_

#include EFI_PROTOCOL_DEFINITION(UgaDraw)
//
// GUID for EFI OEM Badging Protocol
//
#define EFI_OEM_BADGING_SUPPORT_PROTOCOL_GUID \
  { 0x4b11ff5b, 0x590c, 0x4bfe, 0x96, 0xa5, 0x4, 0xbc, 0x5c, 0xca, 0x5c, 0x11 }

EFI_FORWARD_DECLARATION (EFI_OEM_BADGING_SUPPORT_PROTOCOL);

typedef enum {
  EfiBadgingSupportFormatBMP,
  EfiBadgingSupportFormatJPEG,
  EfiBadgingSupportFormatTIFF,
  EfiBadgingSupportFormatGIF,
  EfiBadgingSupportFormatPCX,
  EfiBadgingSupportFormatTGA,
  EfiBadgingSupportFormatUnknown
} EFI_BADGING_SUPPORT_FORMAT;

typedef enum {
  EfiBadgingSupportDisplayAttributeLeftTop,
  EfiBadgingSupportDisplayAttributeCenterTop,
  EfiBadgingSupportDisplayAttributeRightTop,
  EfiBadgingSupportDisplayAttributeCenterRight,
  EfiBadgingSupportDisplayAttributeRightBottom,
  EfiBadgingSupportDisplayAttributeCenterBottom,
  EfiBadgingSupportDisplayAttributeLeftBottom,
  EfiBadgingSupportDisplayAttributeCenterLeft,
  EfiBadgingSupportDisplayAttributeCenter,
  EfiBadgingSupportDisplayAttributeCustomized
} EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE;

typedef enum {
  EfiBadgingSupportImageLogo,
  EfiBadgingSupportImageBadge,
  EfiBadgingSupportImageBoot
} EFI_BADGING_SUPPORT_IMAGE_TYPE;

typedef enum {
  OemSupportedVideoMode,
  OemEnterSetupStr,
  OemPort80CodeStr,
  OemBuildQualityStr
} EFI_OEM_VIDEO_MODE_SCREEN_XY_TYPE;

typedef
EFI_STATUS
(EFIAPI *EFI_BADGING_SUPPORT_GET_IMAGE) (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL          *This,
  IN OUT UINT32                                    *Instance,
  IN OUT EFI_BADGING_SUPPORT_IMAGE_TYPE            *Type,
     OUT EFI_BADGING_SUPPORT_FORMAT                *Format,
     OUT UINT8                                     **ImageData,
     OUT UINTN                                     *ImageSize,
     OUT EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE     *Attribute,
     OUT UINTN                                     *CoordinateX,
     OUT UINTN                                     *CoordinateY
);

typedef
BOOLEAN
(EFIAPI *EFI_OEM_VIDEO_MODE_SCREEN_XY) (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN EFI_OEM_VIDEO_MODE_SCREEN_XY_TYPE     ServiceType,
  IN UINT32                                VideoWidth,
  IN UINT32                                VideoHeight,
  IN OUT UINTN                             *CursorLocationX,
  IN OUT UINTN                             *CursorLocationY
);

typedef
BOOLEAN
(EFIAPI *EFI_OEM_STRING_SUPPORT_GET_STRING) (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
  OUT    CHAR16                            **StringData,
  OUT    UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background
);

typedef
EFI_STATUS
(EFIAPI *EFI_OEM_STRING_SUPPORT_STRING_COUNT) (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  OUT    UINTN                             *StringCount
);

typedef struct _EFI_OEM_BADGING_SUPPORT_PROTOCOL {
  EFI_BADGING_SUPPORT_GET_IMAGE       GetImage;
  EFI_OEM_VIDEO_MODE_SCREEN_XY        OemVideoModeScrStrXY;
  EFI_OEM_STRING_SUPPORT_GET_STRING   GetOemString;
  EFI_OEM_STRING_SUPPORT_STRING_COUNT GetStringCount;
} EFI_OEM_BADGING_SUPPORT_PROTOCOL;

extern EFI_GUID gEfiOEMBadgingSupportProtocolGuid;

#endif
