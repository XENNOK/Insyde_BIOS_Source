/** @file
  Serial driver for standard UARTS on an ISA bus.

;******************************************************************************
;* Copyright (c) 2006 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FileSelectUILib.h"
#include "FileConOut.h"
//#include "SnapTextOutHook.h"
#include <Protocol/GraphicsOutput.h>
#include <Protocol/UgaDraw.h>

static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *mFileConOut = NULL;

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

EFI_STATUS
GraphicsGetScreenSize (
  OUT  UINTN                                      *SizeOfX,
  OUT  UINTN                                      *SizeOfY
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  EFI_UGA_DRAW_PROTOCOL                 *UgaDraw = NULL;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput = NULL;
  UINT32                                ColorDepth;
  UINT32                                RefreshRate;
  UINT32                                TempX;
  UINT32                                TempY;

  
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    (VOID **)&UgaDraw
                    );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

      
  if (GraphicsOutput == NULL && UgaDraw == NULL) {
    return EFI_UNSUPPORTED;
  }


  if (GraphicsOutput != NULL) {
    *SizeOfX = (UINTN)GraphicsOutput->Mode->Info->HorizontalResolution;
    *SizeOfY = (UINTN)GraphicsOutput->Mode->Info->VerticalResolution;
  } else {
    Status = UgaDraw->GetMode (
                        UgaDraw,
                        &TempX,
                        &TempY,
                        &ColorDepth,
                        &RefreshRate
                        );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
    *SizeOfX = (UINTN)TempX;
    *SizeOfY = (UINTN)TempY;
  }
  return EFI_SUCCESS;
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
  
  mFileConOut->SetCursorPosition (mFileConOut, x, y);

  ChStr[0] = BOXDRAW_HORIZONTAL;
  ChStr[1] = 0;

  for (i = 0; i < Len; i++, x++) 
    mFileConOut->OutputString (mFileConOut, ChStr);
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
    mFileConOut->SetCursorPosition (mFileConOut, x, y);
    mFileConOut->OutputString (mFileConOut, ChStr);
  }
}

VOID
FcoPrintCharAt (
  UINTN     ScrX,
  UINTN     ScrY,
  CHAR16    Ch
  )
{
  CHAR16    ChStr[2];
  
  mFileConOut->SetCursorPosition (mFileConOut, ScrX, ScrY);
  
  ChStr[0] = Ch;
  ChStr[1] = 0;
  mFileConOut->OutputString (mFileConOut, ChStr);
}

EFI_STATUS
FcoSetAttribute (
  IN UINTN    Attribute
  )
{
  return mFileConOut->SetAttribute (mFileConOut, Attribute);
}

EFI_STATUS
FcoSetCursorPosition (
  IN UINTN Column,
  IN INTN Row
  )
{
  return mFileConOut->SetCursorPosition (mFileConOut, Column, Row);
}

EFI_STATUS
FcoOutputString (
  IN CHAR16      *String
  )
{
  return mFileConOut->OutputString (mFileConOut, String);
}

EFI_STATUS
FcoQueryMode (
  OUT UINTN     *Columns,
  OUT UINTN     *Rows
  )
{
  return mFileConOut->QueryMode (mFileConOut, mFileConOut->Mode->Mode, Columns, Rows);
}

EFI_STATUS
FcoEnableCursor (
  IN BOOLEAN    Enable
  )
{
  return mFileConOut->EnableCursor (mFileConOut, Enable);
}

EFI_STATUS
FcoGetModeInfo (
  IN INT32    *CursorColumn,
  IN INT32    *CursorRow,
  IN BOOLEAN  *CursorVisible,
  IN INT32    *Attribute
  )
{
  *CursorColumn  = mFileConOut->Mode->CursorColumn;
  *CursorRow     = mFileConOut->Mode->CursorRow;
  *CursorVisible = mFileConOut->Mode->CursorVisible;
  *Attribute     = mFileConOut->Mode->Attribute;

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeFileConOut (
  VOID
  )
{
  
  // Set the global ConOut protocol
  mFileConOut = gST->ConOut;

  return EFI_SUCCESS;
}

