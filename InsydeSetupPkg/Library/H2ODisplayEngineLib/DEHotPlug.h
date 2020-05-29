/** @file

Hot plug for formBrowser
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

#ifndef _DE_HOT_PLUG_H_
#define _DE_HOT_PLUG_H_

#include "H2ODisplayEngineLib.h"

EFI_STATUS
EFIAPI
FakeTextOutputReset (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FakeTextOutputOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN CHAR16                                *WString
  );

EFI_STATUS
EFIAPI
FakeTextOutputTestString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN CHAR16                                *WString
  );

EFI_STATUS
EFIAPI
FakeTextOutputQueryMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL      *This,
  IN  UINTN                                ModeNumber,
  OUT UINTN                                *Columns,
  OUT UINTN                                *Rows
  );

EFI_STATUS
EFIAPI
FakeTextOutputSetMode (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 ModeNumber
  );

EFI_STATUS
EFIAPI
FakeTextOutputSetAttribute (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 Attribute
  );

EFI_STATUS
EFIAPI
FakeTextOutputClearScreen (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This
  );

EFI_STATUS
EFIAPI
FakeTextOutputSetCursorPosition (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 Column,
  IN UINTN                                 Row
  );

EFI_STATUS
EFIAPI
FakeTextOutputEnableCursor (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN BOOLEAN                               Visible
  );

EFI_STATUS
EFIAPI
FakeGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL         *This,
  IN  UINT32                               ModeNumber,
  OUT UINTN                                *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
  );

EFI_STATUS
EFIAPI
FakeGraphicsOutputSetMode (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN UINT32                                ModeNumber
  );

EFI_STATUS
EFIAPI
FakeGraphicsOutputBlt (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer OPTIONAL,
  IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
  IN UINTN                                 SourceX,
  IN UINTN                                 SourceY,
  IN UINTN                                 DestinationX,
  IN UINTN                                 DestinationY,
  IN UINTN                                 Width,
  IN UINTN                                 Height,
  IN UINTN                                 Delta      OPTIONAL
  );

#endif
