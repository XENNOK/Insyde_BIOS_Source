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

#ifndef _EFI_OEM_GRAPHICS_LIB_H_
#define _EFI_OEM_GRAPHICS_LIB_H_

#include "Uefi.h"
#include <PiDxe.h>

#include <Protocol/GraphicsOutput.h>
#include <Protocol/OEMBadgingSupport.h>
#include <Protocol/EdidDiscovered.h>

#define SIZE_4G                       0x100000000
#define DEFAULT_HORIZONTAL_RESOLUTION 800
#define DEFAULT_VERTICAL_RESOLUTION   600

EFI_STATUS
ConvertBmpToGopBlt (
  IN  VOID      *BmpImage,
  IN  UINTN     BmpImageSize,
  IN OUT VOID   **GopBlt,
  IN OUT UINTN  *GopBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth
  );

EFI_STATUS
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
  );

EFI_STATUS
DisableQuietBoot (
  VOID
  );

EFI_STATUS
LockKeyboards (
  IN  CHAR16    *Password
  );

EFI_STATUS
ShowOemString (
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL      *Badging,
  IN BOOLEAN                               AfterSelect,
  IN UINT8                                 SelectedStringNum
);

EFI_STATUS
OnEndOfDisableQuietBoot (
  VOID
);

EFI_STATUS
LogoLibSetBootLogo (
  VOID
  );

EFI_STATUS
BgrtDecodeImageToBlt (
  IN       UINT8                           *ImageData,
  IN       UINTN                           ImageSize,
  IN       EFI_BADGING_SUPPORT_FORMAT      ImageFormat,
  IN OUT   UINT8                           **Blt,
  IN OUT   UINTN                           *Width,
  IN OUT   UINTN                           *Height
  );

EFI_STATUS
BgrtDecodeImageToBlt (
  IN       UINT8                           *ImageData,
  IN       UINTN                           ImageSize,
  IN       EFI_BADGING_SUPPORT_FORMAT      ImageFormat,
  IN OUT   UINT8                           **Blt,
  IN OUT   UINTN                           *Width,
  IN OUT   UINTN                           *Height
  );

EFI_STATUS
GetResolutionByEdid (
  IN  EFI_EDID_DISCOVERED_PROTOCOL      *EdidDiscovered,
  OUT UINT32                            *ResolutionX,
  OUT UINT32                            *ResolutionY
  );

BOOLEAN
IsModeSync (
  VOID
  );

EFI_STATUS
GetPreferredResolution (
  IN  EFI_HANDLE                        ConOutHandle,
  OUT UINT32                            *ScuResolutionX,
  OUT UINT32                            *ScuResolutionY,
  OUT UINT32                            *GraphicsGopModeNumber,
  OUT UINT32                            *GraphicsResoltionX,
  OUT UINT32                            *GraphicsResoltionY
  );

EFI_STATUS
FindNearestGopMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput,
  IN  UINT32                         HorizontalResolution,
  IN  UINT32                         VerticalResolution,
  OUT UINT32                         *GopModeNum,
  OUT UINT32                         *ResolutionX,
  OUT UINT32                         *ResolutionY
  );

EFI_STATUS
BgrtGetImageByBadging (
  IN OUT  UINT8                          **ImageData,
  IN OUT  UINTN                          *ImageSize,
  IN OUT  EFI_BADGING_SUPPORT_FORMAT     *ImageFormat
  );

VOID
BgrtGetLogoCoordinate (
  IN   UINTN      ResolutionX,
  IN   UINTN      ResolutionY,
  IN   UINTN      ImageWidth,
  IN   UINTN      ImageHeight,  
  OUT  UINTN      *CoordinateX,
  OUT  UINTN      *CoordinateY
  );

EFI_STATUS
BgrtGetBootImageInfo (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  );

BOOLEAN
TextOnlyConsole (
  VOID
  );

VOID
DisplayMessageToConsoleRedirection (
  IN     CHAR16   *LogoStr
  );

VOID *
OemGraphicsGetAllActiveConOutDevPath (
  VOID
  );

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsGetNextDevicePathInstance (
  IN OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath,
  OUT UINTN                          *Size
  );

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsDuplicateDevicePath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsDuplicateDevicePath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathValid (
  IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN       UINTN                    MaxSize
  );

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathEndType (
  IN CONST VOID  *Node
  );

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsNextDevicePathNode (
  IN CONST VOID  *Node
  );

UINT8
EFIAPI
OemGraphicsDevicePathSubType (
  IN CONST VOID  *Node
  );

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathEnd (
  IN CONST VOID  *Node
  );

UINTN
EFIAPI
OemGraphicsDevicePathNodeLength (
  IN CONST VOID  *Node
  );

UINT8
EFIAPI
OemGraphicsDevicePathType (
  IN CONST VOID  *Node
  );

UINTN
EFIAPI
OemGraphicsGetDevicePathSize (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

#endif
