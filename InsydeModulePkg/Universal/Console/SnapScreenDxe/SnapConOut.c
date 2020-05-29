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


#include "SnapScreen.h"
#include "SnapConOut.h"
#include "SnapTextOutHook.h"
#include <Protocol/GraphicsOutput.h>

#define  CHAR_SIZE          sizeof(CHAR16)
#define  ATTR_SIZE          sizeof(UINT8)

static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *mSnapConOut = NULL;

VOID
GraphicsGetBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  )
{
  EFI_STATUS  Status;
  UINTN       GX;
  UINTN       GY;
  UINTN       GWidth;
  UINTN       GHeight;      
  
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *Gop;

  GetActiveVgaGop (&Gop);

  GX = X * GLYPH_WIDTH;
  GY = Y * GLYPH_HEIGHT;
  GWidth = Width * GLYPH_WIDTH;
  GHeight = (((Height * 192)/190) + 1) * GLYPH_HEIGHT;  
  
  Status = Gop->Blt (
                      Gop,
                      Buffer,  // Buffer
                      EfiBltVideoToBltBuffer,   // Blt Mode
                      GX,      // Source X
                      GY,      // Source Y
                      0,      // Dest X
                      0,      // Dest Y
                      GWidth,
                      GHeight,
                      0       // Delta
                     );
  
}

VOID 
GraphicsPutBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  )
{
  EFI_STATUS  Status;
  UINTN       GX;
  UINTN       GY;
  UINTN       GWidth;
  UINTN       GHeight;
  
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *Gop;

  GetActiveVgaGop (&Gop);

  GX = X * GLYPH_WIDTH;
  GY = Y * GLYPH_HEIGHT;
  GWidth = Width * GLYPH_WIDTH;
  GHeight = (((Height * 192)/190) + 1) * GLYPH_HEIGHT;  

  Status = Gop->Blt (
                      Gop,
                      Buffer,  // Buffer
                      EfiBltBufferToVideo,   // Blt Mode
                      0,      // Source X
                      0,      // Source Y
                      GX,      // Dest X
                      GY,      // Dest Y
                      GWidth,
                      GHeight,
                      0       // Delta
                     );
                     
}

//
// Hardware abstract Console
//

VOID
DrawHorizontalLine (
  IN  UINTN       x,
  IN  UINTN       y,
  IN  UINTN       Len
  )
{
  UINTN     i;
  CHAR16    ChStr[2];
  
  mSnapConOut->SetCursorPosition (mSnapConOut, x, y);

  ChStr[0] = BOXDRAW_HORIZONTAL;
  ChStr[1] = 0;

  for (i = 0; i < Len; i++, x++) 
    mSnapConOut->OutputString (mSnapConOut, ChStr);
}

VOID
DrawVerticalLine (
  IN  UINTN       x,
  IN  UINTN       y,
  IN  UINTN       Len
  )
{
  UINTN     i;
  CHAR16    ChStr[2];


  ChStr[0] = BOXDRAW_VERTICAL;
  ChStr[1] = 0;
  
  for (i = 0; i < Len; i++, y++) { 
    mSnapConOut->SetCursorPosition (mSnapConOut, x, y);
    mSnapConOut->OutputString (mSnapConOut, ChStr);
  }
}

VOID
ScoPrintCharAt (
  UINTN     ScrX,
  UINTN     ScrY,
  CHAR16    Ch
  )
{
  CHAR16    ChStr[2];
  
  mSnapConOut->SetCursorPosition (mSnapConOut, ScrX, ScrY);
  
  ChStr[0] = Ch;
  ChStr[1] = 0;
  mSnapConOut->OutputString (mSnapConOut, ChStr);
}

EFI_STATUS
ScoSetAttribute (
  IN UINTN    Attribute
  )
{
  return mSnapConOut->SetAttribute (mSnapConOut, Attribute);
}

EFI_STATUS
ScoSetCursorPosition (
  IN UINTN Column,
  IN INTN Row
  )
{
  return mSnapConOut->SetCursorPosition (mSnapConOut, Column, Row);
}

EFI_STATUS
ScoOutputString (
  IN CHAR16      *String
  )
{
  return mSnapConOut->OutputString (mSnapConOut, String);
}

EFI_STATUS
ScoQueryMode (
  OUT UINTN     *Columns,
  OUT UINTN     *Rows
  )
{
  return mSnapConOut->QueryMode (mSnapConOut, mSnapConOut->Mode->Mode, Columns, Rows);
}

EFI_STATUS
ScoEnableCursor (
  IN BOOLEAN    Enable
  )
{
  return mSnapConOut->EnableCursor (mSnapConOut, Enable);
}

EFI_STATUS
ScoGetModeInfo (
  IN INT32    *CursorColumn,
  IN INT32    *CursorRow,
  IN BOOLEAN  *CursorVisible,
  IN INT32    *Attribute
  )
{
  *CursorColumn  = mSnapConOut->Mode->CursorColumn;
  *CursorRow     = mSnapConOut->Mode->CursorRow;
  *CursorVisible = mSnapConOut->Mode->CursorVisible;
  *Attribute     = mSnapConOut->Mode->Attribute;

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeSnapConOut (
  VOID
  )
{
  
  // Set the global ConOut protocol
  mSnapConOut = gST->ConOut;

  return EFI_SUCCESS;
}

