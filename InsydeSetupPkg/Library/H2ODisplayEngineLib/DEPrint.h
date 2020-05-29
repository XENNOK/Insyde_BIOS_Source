/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DE_PRINT_H_
#define _DE_PRINT_H_

EFI_STATUS
DEConOutRest (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN BOOLEAN                                ExtendedVerification
  );

EFI_STATUS
DEConOutOutputString (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN CHAR16                                 *String
  );

EFI_STATUS
DEConOutTestString (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN CHAR16                                 *String
  );

EFI_STATUS
DEConOutSetMode (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN UINTN                                  ModeNumber
  );

EFI_STATUS
DEConOutSetAttribute (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN UINTN                                  Attribute
  );

EFI_STATUS
DEConOutSetNarrowAttribute (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData
  );

EFI_STATUS
DEConOutSetWideAttribute (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData
  );

EFI_STATUS
DEConOutClearScreen (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData
  );

EFI_STATUS
DEConOutSetCursorPosition (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN UINTN                                  Column,
  IN UINTN                                  Row
  );

EFI_STATUS
DEConOutEnableCursor (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN BOOLEAN                                Visible
  );

EFI_STATUS
DEConOutGetAttribute (
  IN    H2O_DISPLAY_ENGINE_PRIVATE_DATA     *DEPrivateData,
  OUT   INT32                               *Attribute
  );

EFI_STATUS
DEGraphicsOutQueryMode (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  );

EFI_STATUS
DEGraphicsOutSetMode (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  IN  UINT32                                ModeNumber
  );

EFI_STATUS
DEGraphicsOutBlt (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer   OPTIONAL,
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
  IN  UINTN                                 SourceX,
  IN  UINTN                                 SourceY,
  IN  UINTN                                 DestinationX,
  IN  UINTN                                 DestinationY,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height,
  IN  UINTN                                 Delta         OPTIONAL
  );

EFI_STATUS
PrintString (
  IN     CHAR16                              *String
  );

EFI_STATUS
PrintChar (
  IN     CHAR16                              Character
  );

EFI_STATUS
PrintAt (
  IN     UINTN                               Column,
  IN     UINTN                               Row,
  IN     CHAR16                              *Fmt,
  ...
  );

EFI_STATUS
PrintStringAt (
  IN     UINTN                               Column,
  IN     UINTN                               Row,
  IN     CHAR16                              *String
  );

EFI_STATUS
PrintCharAt (
  IN     UINTN                               Column,
  IN     UINTN                               Row,
  IN     CHAR16                              Character
  );

#endif
