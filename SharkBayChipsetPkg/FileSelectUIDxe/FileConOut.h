/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FILE_CONSOLE_H_
#define _FILE_CONSOLE_H_

#include <Uefi.h>
#include <Protocol/SimpleTextOut.h>


#define GLYPH_WIDTH         EFI_GLYPH_WIDTH
#define GLYPH_HEIGHT        EFI_GLYPH_HEIGHT

#define CHAR_SIZE           sizeof(CHAR16)
#define ATTR_SIZE           sizeof(UINT8)

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

EFI_STATUS
GraphicsGetScreenSize (
  OUT  UINTN                                      *SizeOfX,
  OUT  UINTN                                      *SizeOfY
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
FcoPrintCharAt (
  UINTN     ScrX,
  UINTN     ScrY,
  CHAR16    Ch
  );

EFI_STATUS
FcoSetAttribute (
  IN UINTN    Attribute
  );

EFI_STATUS
FcoSetCursorPosition (
  IN UINTN Column,
  IN INTN Row
  );
  
EFI_STATUS
FcoOutputString (
  IN CHAR16      *String
  );

EFI_STATUS
FcoQueryMode (
  OUT UINTN     *Columns,
  OUT UINTN     *Rows
  );

EFI_STATUS
FcoEnableCursor (
  IN BOOLEAN    Enable
  );

EFI_STATUS
FcoGetModeInfo (
  IN INT32    *CursorColumn,
  IN INT32    *CursorRow,
  IN BOOLEAN  *CursorVisible,
  IN INT32    *Attribute
  );

EFI_STATUS
InitializeFileConOut (
  VOID
  );

#endif // _SNAP_CONSOLE_H_
