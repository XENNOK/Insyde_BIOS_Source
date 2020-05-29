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
#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Protocol/SimpleTextOut.h>
#include <Library/MemoryAllocationLib.h>
#include "SnapScreen.h"
#include "SnapWin.h"
#include "SnapConOut.h"
#include "SnapTextOutHook.h"

#define  CHAR_SIZE          sizeof(CHAR16)
#define  ATTR_SIZE          sizeof(UINT8)

#define  MAX_PRINT_BUFFER   256

//#define  CONOUT             gST->ConOut

CHAR16 mBlankLine[MAX_PRINT_BUFFER] = {
  /*00*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*10*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*20*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*30*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*40*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*50*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*60*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*70*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*80*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*90*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*A0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*B0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*C0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*D0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*E0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  /*F0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};



VOID
TWinClearArea (
  IN  TWINDOW    *Win,
  IN  RECT       *Rect,
  IN  UINTN       Attrib
  )
{
  UINTN         Row;

  mBlankLine[Rect->Width] = 0;

  // set console attribute
  ScoSetAttribute (Attrib);

  // clean the box Row
  for (Row = 0; Row < Rect->Height; Row++) {
    TWinTextOut ( Win, (INT16)Rect->X, (INT16) (Rect->Y+Row), mBlankLine);
  }
  
  mBlankLine[Rect->Width] = 0x20;

}

VOID 
TWinDrawBorder (
  TWINDOW     *Win,
  UINTN       Attrib
  )
{
  RECT      *Rect;
  UINTN     BoxTop, BoxBottom, BoxLeft, BoxRight;

  Rect = &Win->Area;
  
  BoxLeft = Rect->X;
  BoxRight = Rect->X + Rect->Width - 1;
  BoxTop = Rect->Y;
  BoxBottom = Rect->Y + Rect->Height - 1;

  // set console attribute
  ScoSetAttribute (Attrib);

  //2 Draw Horizontal line
  DrawHorizontalLine( BoxLeft+1, BoxTop, Rect->Width - 2);
  DrawHorizontalLine( BoxLeft+1, BoxBottom, Rect->Width - 2);

  //2 Draw Vertical line
  DrawVerticalLine ( BoxLeft, BoxTop+1, Rect->Height - 2);
  DrawVerticalLine ( BoxRight, BoxTop+1, Rect->Height - 2);

  //2 Draw box coner
  ScoPrintCharAt (BoxLeft, BoxTop, BOXDRAW_DOWN_RIGHT);
  ScoPrintCharAt (BoxRight, BoxTop, BOXDRAW_DOWN_LEFT);
  ScoPrintCharAt (BoxLeft, BoxBottom, BOXDRAW_UP_RIGHT);
  ScoPrintCharAt (BoxRight, BoxBottom, BOXDRAW_UP_LEFT);

}

VOID 
TWinGetCursor (
  TWINDOW    *Win,
  UINT16     *CursorX,
  UINT16     *CursorY
  )
{
  *CursorX = Win->CursorX;
  *CursorY = Win->CursorY;
}

VOID
TWinSetCursor (
  TWINDOW    *Win,
  UINT16     CursorX,
  UINT16     CursorY
  )
{

  UINTN   ScrX;
  UINTN   ScrY;

  Win->CursorX = CursorX;
  Win->CursorY = CursorY;

  ScrX = CursorX + Win->ClientArea.X;
  ScrY = CursorY + Win->ClientArea.Y;
  
  ScoSetCursorPosition (ScrX, ScrY);
  
}

VOID
TWinSetAttribue (
  TWINDOW    *Win,
  UINTN      Attribute
  )
{
  ScoSetAttribute (Attribute);
}

EFI_STATUS
TWinShow (
  TWINDOW  *Win
  )
{

  UINTN     Attrib;
  RECT      Rect;
  UINTN     TitleLen;
  UINTN     ScrX;
  UINTN     ScrY;

  Win->IsShow = TRUE;

  TextGetBackground (Win->Area.X, Win->Area.Y, Win->Area.Width, Win->Area.Height, Win->TextBuffer);
  GraphicsGetBackground (Win->Area.X, Win->Area.Y, Win->Area.Width, Win->Area.Height, Win->GraphicBuffer);
  Attrib = EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLUE);

  if (Win->Border) {
    TWinDrawBorder (Win, Attrib);
  }

  //
  // Draw Title
  //
  
  if (Win->Title) {
    // clear title bar
    mBlankLine[Win->ClientArea.Width] = 0;
    ScrX = Win->Area.X + 1;
    ScrY = Win->Area.Y + 1;
    ScoSetCursorPosition (ScrX, ScrY);
    ScoOutputString (mBlankLine);
    mBlankLine[Win->ClientArea.Width] = 0x20;
      
    // print title string
    TitleLen = StrLen (Win->Title);
    ScrX = Win->Area.X + Win->Area.Width / 2 - TitleLen /2;
    ScrY = Win->Area.Y + 1;
    ScoSetCursorPosition (ScrX, ScrY);
    ScoOutputString (Win->Title);

    // draw title horizontal line
    ScrX = Win->Area.X + 1;
    ScrY = Win->Area.Y + 2;
    DrawHorizontalLine (ScrX, ScrY, Win->Area.Width -2);
  }
  
  Rect.X = 0;
  Rect.Y = 0;
  Rect.Width = Win->ClientArea.Width;
  Rect.Height = Win->ClientArea.Height;
  
  TWinClearArea (Win, &Rect, Attrib);

  return EFI_SUCCESS;
}

EFI_STATUS
TWinScrollUp (
  TWINDOW   *Win,
  UINT16      Lines
  )
{
  return EFI_SUCCESS;
}

VOID
TWinPrintChar (
  TWINDOW   *Win,
  CHAR16      Ch
  )
{

  UINTN   ScrX;
  UINTN   ScrY;

  switch (Ch) {
  
    case '\n' :
      Win->CursorY++;
      Win->CursorX = 0;
      
      break;
      
    default :
      ScrX = Win->CursorX + Win->ClientArea.X;
      ScrY = Win->CursorY + Win->ClientArea.Y;
      ScoPrintCharAt(ScrX, ScrY, Ch);
      Win->CursorX++;
      
      break;
  }

  if (Win->CursorX >= Win->ClientArea.Width) {
    Win->CursorY++;
    Win->CursorX = 0;
  }

  if (Win->CursorY >= Win->ClientArea.Height) {
    TWinScrollUp ( Win, 1);
    Win->CursorY = Win->ClientArea.Height - 1;
  }
  
  return;
  
}

  
EFI_STATUS
TWinPrint (
  TWINDOW   *Win,
  CHAR16    *Format,
  ...  
  )
{
  
  CHAR16    *Buffer;
  UINTN     Return;
  VA_LIST   Marker;
  CHAR16    *cptr;

  Buffer = AllocateZeroPool (MAX_PRINT_BUFFER);

  VA_START ( Marker, Format);
  Return = UnicodeVSPrint (Buffer, MAX_PRINT_BUFFER, Format, Marker);
  VA_END (Marker);

  cptr = Buffer;

  while (*cptr != 0) {
    TWinPrintChar (Win, *cptr);
    cptr++;
  }
  
  FreePool (Buffer);
  return EFI_SUCCESS;
}

EFI_STATUS
TWinPrintAt (
  TWINDOW   *Win,
  UINT16    X,
  UINT16    Y,
  CHAR16    *Format,
  ...  
  )
{

  CHAR16    *Buffer;
  UINTN     Return;
  VA_LIST   Marker;
  CHAR16    *cptr;

  Buffer = AllocateZeroPool (MAX_PRINT_BUFFER);

  VA_START (Marker, Format);
  Return = UnicodeVSPrint (Buffer, MAX_PRINT_BUFFER, Format, Marker);
  VA_END (Marker);

  TWinSetCursor ( Win, X, Y);

  cptr = Buffer;
  
  while (*cptr != 0) {
    TWinPrintChar (Win, *cptr);
    cptr++;
  }
  
  FreePool (Buffer);
  return EFI_SUCCESS;
}

VOID
TWinClearLine (
  TWINDOW  *Win,
  UINT16   LineNumber
  )
{

  TWinSetCursor( Win, 0, LineNumber);

  mBlankLine[Win->ClientArea.Width] = 0;
  
  ScoOutputString (mBlankLine);

  mBlankLine[Win->ClientArea.Width] = 0x20;
  
}

VOID
TWinTextOut (
  TWINDOW     *Win,
  INT16       X,
  INT16       Y,
  CHAR16      *Str
  )
{

  CHAR16    *Cptr;
  UINTN     StrLength;

  StrLength = StrLen(Str);
  Cptr = Str;

  if(X == TEXTOUT_CENTER)
    X = (INT16)(Win->ClientArea.Width / 2) - (INT16)(StrLength / 2);

  if (Y == TEXTOUT_CENTER)
    Y = Win->ClientArea.Height / 2;

  if ( Y < 0 || Y >= Win->ClientArea.Height || X >= Win->ClientArea.Width)
    return;
    
  if (X < 0) {
    if ((StrLength + X) <= 0)
      return;

    Cptr = Str + (-X);
    StrLength = StrLength - (-X);
    X = 0;
  }

  if (X + StrLength > Win->ClientArea.Width) {
    Cptr[Win->ClientArea.Width - X] = 0;
  } 

  TWinSetCursor ( Win, (UINT16)X, (UINT16)Y);
  
  ScoOutputString (Cptr);
  
}

EFI_STATUS
TWinDestroy (
  TWINDOW     *Win
  )
{

  if (Win->IsShow == TRUE) {
    TextPutBackground(Win->Area.X, Win->Area.Y, Win->Area.Width, Win->Area.Height, Win->TextBuffer);
    GraphicsPutBackground(Win->Area.X, Win->Area.Y, Win->Area.Width, Win->Area.Height, Win->GraphicBuffer);
  }
  gBS->FreePool(Win);
  
  return EFI_SUCCESS;
}

EFI_STATUS
TWinCreate (
  INT16       X,
  INT16       Y,
  UINT16      CWidth,
  UINT16      CHeight,
  BOOLEAN     Border,
  CHAR16      *Title,
  TWINDOW     **AWin
  )
{
  UINTN       Size;
  TWINDOW     *Win;
  UINT8       *MemPtr;
  UINTN       Columns;
  UINTN       Rows;
  RECT        ClientArea;
  UINT16      Width;
  UINT16      Height;
  UINTN       TitleSize = 0;
  UINTN       TextBufferSize;
  UINTN       GraphicBufferSize;
  UINT16      GraphicHeight;

  if (CWidth == 0 || CHeight == 0 || CWidth >= WIN_MAX_WIDTH || CHeight >= WIN_MAX_HEIGHT)
    return EFI_INVALID_PARAMETER;

  Width = CWidth;
  Height = CHeight;
  
  if (Border == TRUE) {
    Width = CWidth + 2;
    Height = CHeight + 2;
  }

  if (Title != NULL) {
    Height = Height + 2;
  }

  ScoQueryMode (&Columns, &Rows);
  
  if (X == WIN_SCR_CENTER) {
    X = (INT16)(Columns / 2) - (INT16)(Width /2);
  }

  if (Y == WIN_SCR_CENTER) {
    Y = (INT16)(Rows / 2) - (INT16)(Height /2);
  }

  if (Border == TRUE) {
    ClientArea.X = X + 1;
    ClientArea.Y = Y + 1;
  }
  else {
    ClientArea.X = X;
    ClientArea.Y = Y;
  }

  if (Title != NULL) {
    ClientArea.Y = ClientArea.Y + 2;
  }
  
  ClientArea.Width = CWidth;
  ClientArea.Height = CHeight;

  if (Title != NULL)
    TitleSize = StrSize (Title);

  TextBufferSize = Width * Height * (CHAR_SIZE + ATTR_SIZE);
  GraphicHeight = ((Height * 192)/190) + 1; 
  GraphicBufferSize = (Width * GLYPH_WIDTH) * (GraphicHeight * GLYPH_HEIGHT) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  
  Size = sizeof(TWINDOW) + TitleSize + TextBufferSize + GraphicBufferSize;
  
  MemPtr = AllocateZeroPool(Size);
  
  if (MemPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "%s Allocate Memory Fail.  Size=%d", __FUNCTION__, Size));
    return EFI_OUT_OF_RESOURCES;
  }

  Win = (TWINDOW *) MemPtr;
  Win->Border = Border;
  Win->Area.X = X;
  Win->Area.Y = Y;
  Win->Area.Width = Width;
  Win->Area.Height = Height;
  Win->ClientArea = ClientArea;
  
  if (Title != NULL) {
    Win->Title = (CHAR16 *)(MemPtr + sizeof(TWINDOW));
    StrCpy (Win->Title, Title);
  }
  
  Win->TextBuffer = (CHAR16 *)(MemPtr + sizeof(TWINDOW) + TitleSize);
  Win->GraphicBuffer = (CHAR16 *)(MemPtr + sizeof(TWINDOW) + TitleSize + TextBufferSize);

  *AWin = Win;
  
  return EFI_SUCCESS;
}

