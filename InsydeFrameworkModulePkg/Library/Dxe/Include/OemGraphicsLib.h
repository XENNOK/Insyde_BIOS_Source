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
//;   OemGraphicsLib.h
//;

#ifndef _EFI_OEM_GRAPHICS_LIB_H_
#define _EFI_OEM_GRAPHICS_LIB_H_

#include EFI_PROTOCOL_DEFINITION (ConsoleControl)
#include EFI_PROTOCOL_DEFINITION (FirmwareVolume)
#include EFI_PROTOCOL_DEFINITION (GraphicsOutput)
#include EFI_PROTOCOL_DEFINITION (SimpleTextOut)
#include EFI_PROTOCOL_DEFINITION (UgaDraw)
#include EFI_PROTOCOL_DEFINITION (EfiOEMBadgingSupport)

#include EFI_GUID_DEFINITION (Bmp)
#include EFI_GUID_DEFINITION (GlobalVariable)
#include EFI_PROTOCOL_DEFINITION (EdidDiscovered)
#define SIZE_4G     0x100000000
#define DEFAULT_HORIZONTAL_RESOLUTION     800
#define DEFAULT_VERTICAL_RESOLUTION       600
#define DEFAULT_CONSOLE_TEXT_COLOR        EFI_LIGHTGRAY
#define DEFAULT_COLSOLE_BACKGROUND_COLOR  EFI_BLACK

typedef struct {
  UINT32                        SizeOfX;
  UINT32                        SizeOfY;
  UINT32                        ColorDepth;
  UINT32                        RefreshRate;
} EFI_QUIET_BOOT_INFO;

EFI_STATUS
GetGraphicsBitMapFromFV (
  IN  EFI_GUID      *FileNameGuid,
  OUT VOID          **Image,
  OUT UINTN         *ImageSize
  )
/*++

Routine Description:

  Return the graphics image file named FileNameGuid into Image and return it's
  size in ImageSize. All Firmware Volumes (FV) in the system are searched for the
  file name.

Arguments:

  FileNameGuid  - File Name of graphics file in the FV(s).

  Image         - Pointer to pointer to return graphics image.  If NULL, a
                  buffer will be allocated.

  ImageSize     - Size of the graphics Image in bytes. Zero if no image found.


Returns:

  EFI_SUCCESS          - Image and ImageSize are valid.
  EFI_BUFFER_TOO_SMALL - Image not big enough. ImageSize has required size
  EFI_NOT_FOUND        - FileNameGuid not found

--*/
;

EFI_STATUS
ConvertBmpToGopBlt (
  IN  VOID      *BmpImage,
  IN  UINTN     BmpImageSize,
  IN OUT VOID   **GopBlt,
  IN OUT UINTN  *GopBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth
  )
/*++

Routine Description:

  Convert a *.BMP graphics image to a GOP/UGA blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

Arguments:

  BmpImage      - Pointer to BMP file

  BmpImageSize  - Number of bytes in BmpImage

  GopBlt        - Buffer containing GOP version of BmpImage.

  GopBltSize    - Size of GopBlt in bytes.

  PixelHeight   - Height of GopBlt/BmpImage in pixels

  PixelWidth    - Width of GopBlt/BmpImage in pixels


Returns:

  EFI_SUCCESS           - GopBlt and GopBltSize are returned.
  EFI_UNSUPPORTED       - BmpImage is not a valid *.BMP image
  EFI_BUFFER_TOO_SMALL  - The passed in GopBlt buffer is not big enough.
                          GopBltSize will contain the required size.
  EFI_OUT_OF_RESOURCES  - No enough buffer to allocate

--*/
;

EFI_STATUS
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
  )
/*++

Routine Description:

  Use Console Control to turn off UGA based Simple Text Out consoles from going
  to the UGA device. Put up LogoFile on every UGA device that is a console

Arguments:

  LogoFile - File name of logo to display on the center of the screen.


Returns:

  EFI_SUCCESS           - ConsoleControl has been flipped to graphics and logo
                          displayed.
  EFI_UNSUPPORTED       - Logo not found

--*/
;

EFI_STATUS
DisableQuietBoot (
  VOID
  )
/*++

Routine Description:

  Use Console Control to turn on UGA based Simple Text Out consoles. The UGA
  Simple Text Out screens will now be synced up with all non UGA output devices

Arguments:

  NONE

Returns:

  EFI_SUCCESS           - UGA devices are back in text mode and synced up.
  EFI_UNSUPPORTED       - Logo not found

--*/
;

EFI_STATUS
LockKeyboards (
  IN  CHAR16    *Password
  )
/*++

Routine Description:
  Use Console Control Protocol to lock the Console In Spliter virtual handle.
  This is the ConInHandle and ConIn handle in the EFI system table. All key
  presses will be ignored until the Password is typed in. The only way to
  disable the password is to type it in to a ConIn device.

Arguments:
  Password - Password used to lock ConIn device


Returns:

  EFI_SUCCESS     - ConsoleControl has been flipped to graphics and logo
                    displayed.
  EFI_UNSUPPORTED - Logo not found

--*/
;

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

VOID
SetModeByGraphicOutput (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                        SizeOfX,
  IN  UINT32                        SizeOfY
  );

EFI_STATUS
LogoLibSetBootLogo (
  VOID
  )
/*++

Routine Description:

  Consume by Bgrt driver to get the logo infomation.
  In some platform won't called EnableQuietBoot to update the Bgrt logo info, 
  if the logo info won't update when ReadyToBoot event trigger, the Bgrt driver will
  get the logo info automatically by this function. 

Arguments:

  None.

Returns:

  EFI_SUCCESS             Get boot logo info success.

--*/
;

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
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
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
GetConOutVariable (
  IN CHAR16 * Name
  );

EFI_DEVICE_PATH_PROTOCOL *
OemGraphicsLiteDevicePathInstance (
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  OUT UINTN                         *Size
  );

EFI_DEVICE_PATH_PROTOCOL *
OemGraphicsLiteDuplicateDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );

UINTN
OemGraphicsLiteDevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

#endif
