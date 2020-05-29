/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _SNAP_CONSOLE_H_
#define _SNAP_CONSOLE_H_

#include <Uefi.h>
#include <Protocol/SimpleTextOut.h>


#define GLYPH_WIDTH         EFI_GLYPH_WIDTH
#define GLYPH_HEIGHT        EFI_GLYPH_HEIGHT

VOID
GraphicsGetBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

VOID 
GraphicsPutBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

VOID
DrawHorizontalLine (
  IN  UINTN       x,
  IN  UINTN       y,
  IN  UINTN       Len
  );

VOID
DrawVerticalLine (
  IN  UINTN       x,
  IN  UINTN       y,
  IN  UINTN       Len
  );

VOID
ScoPrintCharAt (
  UINTN     ScrX,
  UINTN     ScrY,
  CHAR16    Ch
  );

EFI_STATUS
ScoSetAttribute (
  IN UINTN    Attribute
  );

EFI_STATUS
ScoSetCursorPosition (
  IN UINTN Column,
  IN INTN Row
  );
  
EFI_STATUS
ScoOutputString (
  IN CHAR16      *String
  );

EFI_STATUS
ScoQueryMode (
  OUT UINTN     *Columns,
  OUT UINTN     *Rows
  );

EFI_STATUS
ScoEnableCursor (
  IN BOOLEAN    Enable
  );

EFI_STATUS
ScoGetModeInfo (
  IN INT32    *CursorColumn,
  IN INT32    *CursorRow,
  IN BOOLEAN  *CursorVisible,
  IN INT32    *Attribute
  );

EFI_STATUS
InitializeSnapConOut (
  VOID
  );

#endif // _SNAP_CONSOLE_H_
