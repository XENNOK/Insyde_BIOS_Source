//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/** @file
  Boot Logo protocol is used to convey information of Logo dispayed during boot.

Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOOT_LOGO_H_
#define _BOOT_LOGO_H_

#include EFI_PROTOCOL_DEFINITION (GraphicsOutput)

#define EFI_BOOT_LOGO_PROTOCOL_GUID \
  { \
    0xcdea2bd3, 0xfc25, 0x4c1c, { 0xb9, 0x7c, 0xb3, 0x11, 0x86, 0x6, 0x49, 0x90 } \
  }

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_BOOT_LOGO_PROTOCOL  EFI_BOOT_LOGO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_SET_BOOT_LOGO)(
  IN EFI_BOOT_LOGO_PROTOCOL            *This,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltBuffer       OPTIONAL,
  IN UINTN                             DestinationX,
  IN UINTN                             DestinationY,
  IN UINTN                             Width,
  IN UINTN                             Height
  );
/*++

Routine Description:

  Update information of logo image drawn on screen.

Arguments:

  This           The pointer to the Boot Logo protocol instance.
  BltBuffer      The BLT buffer for logo drawn on screen. If BltBuffer
                 is set to NULL, it indicates that logo image is no
                 longer on the screen.
  DestinationX   X coordinate of destination for the BltBuffer.
  DestinationY   Y coordinate of destination for the BltBuffer.
  Width          Width of rectangle in BltBuffer in pixels.
  Height         Hight of rectangle in BltBuffer in pixels.

Returns:

  EFI_SUCCESS             The boot logo information was updated.
  EFI_INVALID_PARAMETER   One of the parameters has an invalid value.
  EFI_OUT_OF_RESOURCES    The logo information was not updated due to
                          insufficient memory resources.

--*/

struct _EFI_BOOT_LOGO_PROTOCOL {
  EFI_SET_BOOT_LOGO        SetBootLogo;
};

extern EFI_GUID gEfiBootLogoProtocolGuid;

#endif
