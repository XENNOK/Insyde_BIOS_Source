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
#include <Protocol/SimpleTextOut.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ConsoleLib.h>
#include "SnapLib.h"
#include "SnapTextOutHook.h"


#define NARROW_CHAR         0xFFF0
#define WIDE_CHAR           0xFFF1

typedef struct {
  BOOLEAN                       IsHooked;
  BOOLEAN                       InService;
  BOOLEAN                       InOutputString;
  UINTN                         Columns;
  UINTN                         Rows;
  CHAR16                        *CharBuffer;
  UINT8                         *AttrBuffer;
  EFI_SIMPLE_TEXT_OUTPUT_MODE   *Mode;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *PtrSimpleTextOut;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  OldSTO;
} HOOK_SIMPLE_TEXT_OUT;

static HOOK_SIMPLE_TEXT_OUT   mHookTextOut = {0};

static VOID              *mStoRegistration;
static EFI_EVENT         mSimpleTextOutEvent = NULL;

VOID
TextGetBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  )
{
  UINTN     Index;
  CHAR16    *CharSrc;
  UINT8     *AttrSrc;
  CHAR16    *CharBuffer;
  UINT8     *AttrBuffer;

  CharBuffer = Buffer;
  CharSrc = X + (Y * mHookTextOut.Columns) + mHookTextOut.CharBuffer;

  // copy screen text to buffer
  for (Index = 0; Index < Height; Index++) {
    CopyMem (CharBuffer, CharSrc, Width * sizeof(CHAR16));
    CharBuffer = CharBuffer + Width;
    CharSrc = CharSrc + mHookTextOut.Columns;
  }

  AttrBuffer = (UINT8 *)CharBuffer;
  AttrSrc = X + (Y * mHookTextOut.Columns) + mHookTextOut.AttrBuffer;

  // copy screen attribute to buffer
  for (Index = 0; Index < Height; Index++) {
    CopyMem(AttrBuffer, AttrSrc, Width);
    AttrBuffer = AttrBuffer + Width;
    AttrSrc = AttrSrc + mHookTextOut.Columns;
  }
  
}

VOID
TextPutBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  )
{
  INT32       OrgAttribute;
  INT32       Attribute;
  UINT8       *AttrPtr;
  CHAR16      *CharPtr;
  UINTN       Column;
  UINTN       Row;
  CHAR16      Str[2] = {0};

  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This;

  This = gST->ConOut;

  CharPtr = (CHAR16 *) Buffer;
  AttrPtr = (UINT8 *)((CHAR16 *)Buffer + Width * Height);
  
  OrgAttribute = This->Mode->Attribute;
  Attribute = OrgAttribute;
  
  for (Row = 0; Row < Height; Row++) {
  
    This->SetCursorPosition (This, X, Y+Row);

    for (Column = 0; Column < Width; Column++) {

      // check attribute 
      if (*AttrPtr != Attribute) {
        Attribute = *AttrPtr;
        This->SetAttribute (This, Attribute);
      }
      
      // Out string to conout device
      Str[0] = *CharPtr;
      This->OutputString (This, Str);

      // process wide char
      if (*AttrPtr & EFI_WIDE_ATTRIBUTE) {
        Column++;
        CharPtr++;
        AttrPtr++;
      }
      
      CharPtr++;
      AttrPtr++;
    }
  }

  This->SetAttribute (This, OrgAttribute);
}

VOID
ClearTextBuffer (
  HOOK_SIMPLE_TEXT_OUT     *SimpleTextOut
  )
{
  UINTN           CharsSize;
  CHAR16          *CharPtr;
  UINT8           *AttrPtr;

  CharsSize = SimpleTextOut->Columns * SimpleTextOut->Rows;
  CharPtr = SimpleTextOut->CharBuffer;
  AttrPtr = SimpleTextOut->AttrBuffer;
  
  while (CharsSize > 0) {
    *CharPtr++ = 0x0020;
    *AttrPtr++ = 0x07;
    CharsSize--;
  }
}

VOID
ScrollScreen (
  HOOK_SIMPLE_TEXT_OUT    *SimpleTextOut,
  UINTN                   MaxColumn,
  UINTN                   MaxRow
  )
{

  CHAR16    *CharDest;
  CHAR16    *CharSrc;
  UINT8     *AttrDest;
  UINT8     *AttrSrc;
  UINTN     Index;

  // scroll character
  CharDest = SimpleTextOut->CharBuffer;
  CharSrc = SimpleTextOut->CharBuffer + MaxColumn;
  
  CopyMem (CharDest, CharSrc, MaxColumn * (MaxRow - 1) * sizeof(CHAR16));

  Index = 0;
  CharDest = SimpleTextOut->CharBuffer + (MaxColumn * (MaxRow - 1));
  while (Index++ < MaxColumn) {
    SetMem (CharDest, 1, 0x20);
    CharDest++;
  }

  // scroll attribute  
  AttrDest = SimpleTextOut->AttrBuffer;
  AttrSrc = SimpleTextOut->AttrBuffer + MaxColumn;

  CopyMem (AttrDest, AttrSrc, MaxColumn * (MaxRow - 1));

  Index = 0;
  AttrDest = SimpleTextOut->AttrBuffer + (MaxColumn * (MaxRow - 1));
  while (Index++ < MaxColumn) {
    SetMem (AttrDest, 1, 0x07);
    AttrDest++;
  }  
}

EFI_STATUS
TextReset  (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   * This,
  IN BOOLEAN                        ExtendedVerification
  )
{

  EFI_STATUS      Status;
  BOOLEAN         InService;

  InService = mHookTextOut.InService;
  
  if ( InService == FALSE) {
    mHookTextOut.InService = TRUE;
  }
  
  Status = mHookTextOut.OldSTO.Reset (This, ExtendedVerification);

  if ( InService == FALSE) {
  
    This->QueryMode (This, mHookTextOut.Mode->Mode, &mHookTextOut.Columns, &mHookTextOut.Rows);

    ClearTextBuffer (&mHookTextOut);
    
    mHookTextOut.InService = FALSE;
  }
  
  return Status;

}

EFI_STATUS
TextSetMode (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    * This,
  IN UINTN                           ModeNumber
  )
{

  EFI_STATUS      Status;
  BOOLEAN         InService;

  InService = mHookTextOut.InService;

  if ( InService == FALSE) {
    mHookTextOut.InService = TRUE;
  }
  
  Status = mHookTextOut.OldSTO.SetMode (This, ModeNumber);
  
  if (InService == FALSE) {

    This->QueryMode (This, mHookTextOut.Mode->Mode, &mHookTextOut.Columns, &mHookTextOut.Rows);
    
    ClearTextBuffer (&mHookTextOut);

    mHookTextOut.InService = FALSE;
  }
  
  return Status;
}

EFI_STATUS
TextClearScreen (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   * This
  )
{

  EFI_STATUS      Status;
  BOOLEAN         InService;
 
  InService = mHookTextOut.InService;
 
  if (InService == FALSE) {
  
    mHookTextOut.InService = TRUE;
    
    This->QueryMode (This, mHookTextOut.Mode->Mode, &mHookTextOut.Columns, &mHookTextOut.Rows);
  }
  
  Status = mHookTextOut.OldSTO.ClearScreen (This);
  
  if (InService == FALSE) {
  
    ClearTextBuffer (&mHookTextOut);
    
    mHookTextOut.InService = FALSE;
  }
  
  return Status;
}

EFI_STATUS
TextOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL     * This,
  IN CHAR16                              *String
  )
{

  EFI_STATUS      Status;
  UINTN           CursorColumn = 0;
  UINTN           CursorRow = 0;
  CHAR16          *WString = NULL;
  UINT32          GlyphWidth = 0;
  UINT8           AttributeSetting = 0;
  UINT8           CharAttrib;
  UINTN           MaxColumn = 0;
  UINTN           MaxRow = 0;
  BOOLEAN         InService;

  InService = mHookTextOut.InService;
  
  if (InService == FALSE) {

    mHookTextOut.InService = TRUE;
    
    WString = String;
    CursorRow = (UINTN) mHookTextOut.Mode->CursorRow;
    CursorColumn = (UINTN) mHookTextOut.Mode->CursorColumn;
    AttributeSetting = (UINT8)mHookTextOut.Mode->Attribute;
    This->QueryMode (This, mHookTextOut.Mode->Mode, &MaxColumn, &MaxRow);
  }
  
  Status = mHookTextOut.OldSTO.OutputString (This, String);

  if (InService == FALSE) {

    while (*WString != 0) {
    
      switch (*WString) {

        case CHAR_BACKSPACE:
        
          if (CursorColumn == 0 && CursorRow > 0) {
            CursorRow--;
            CursorColumn = MaxColumn - 1;
            mHookTextOut.CharBuffer [CursorRow * MaxColumn + CursorColumn] = 0x20;
          }
          else if (CursorColumn > 0) {
            CursorColumn--;
            mHookTextOut.CharBuffer [CursorRow * MaxColumn + CursorColumn] = 0x20;
          }
          
          break;

        case CHAR_LINEFEED:
          if (CursorRow < (MaxRow - 1))
            CursorRow++;
          else 
            ScrollScreen (&mHookTextOut, MaxColumn, MaxRow);
          break;

        case CHAR_CARRIAGE_RETURN:
          CursorColumn = 0;
          break;

        case WIDE_CHAR:
          AttributeSetting |= EFI_WIDE_ATTRIBUTE;
          break;

        case NARROW_CHAR:
          AttributeSetting &= (~ (UINT32) EFI_WIDE_ATTRIBUTE);
          break;

        default:
          
          GlyphWidth = 0;

          CharAttrib = AttributeSetting;
          if (AttributeSetting & EFI_WIDE_ATTRIBUTE) {
            GlyphWidth =2;
          }
          else {
            GlyphWidth = ConsoleLibGetGlyphWidth (*WString);
            if (GlyphWidth >= 2) 
              CharAttrib = CharAttrib | EFI_WIDE_ATTRIBUTE;
          }  
          
          mHookTextOut.CharBuffer[(CursorRow * MaxColumn) + CursorColumn] = *WString;
          mHookTextOut.AttrBuffer[(CursorRow * MaxColumn) + CursorColumn] = CharAttrib;

          CursorColumn = CursorColumn + GlyphWidth;
          
          if (CursorColumn >= MaxColumn) {
            CursorColumn = CursorColumn - MaxColumn;
            CursorRow++;
            if (CursorRow == MaxRow) {
              ScrollScreen(&mHookTextOut, MaxColumn, MaxRow);
              CursorRow = MaxRow - 1;
            }
          }
          break;
      };    

      WString++;
      
    }
    
    mHookTextOut.InService = FALSE;
  }

  return Status; 
}

EFI_STATUS
HookSimpleTextOut (
  EFI_HANDLE        Handle
  )
{

  EFI_STATUS                      Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *TextOut;
  UINTN                           Mode;
  UINTN                           Columns;
  UINTN                           Rows;
  UINTN                           Product = 0;
  UINTN                           MaxProduct = 0;
  
  // Check if hooked
  if (mHookTextOut.IsHooked == TRUE)
    return EFI_SUCCESS;
    
  Status = gBS->HandleProtocol (Handle, &gEfiSimpleTextOutProtocolGuid, (VOID **)&TextOut);
  if (EFI_ERROR(Status))
    return Status;
    
  //
  // Calculate the max buffer size
  //
  for (Mode = 0; Mode < (UINTN)TextOut->Mode->MaxMode; Mode++) {
  
    Status = TextOut->QueryMode (TextOut, Mode, &Columns, &Rows);
    if (EFI_ERROR(Status))
      continue;
      
    Product = Columns * Rows;
    if (Product > MaxProduct)
      MaxProduct = Product;
  }

  mHookTextOut.CharBuffer = AllocatePool (MaxProduct * sizeof(UINT8) + MaxProduct * sizeof(CHAR16));
  mHookTextOut.AttrBuffer = (UINT8 *)((UINT8 *)mHookTextOut.CharBuffer + MaxProduct * sizeof(CHAR16));
  mHookTextOut.Mode = TextOut->Mode;
  mHookTextOut.PtrSimpleTextOut = TextOut;

  // backup the original protocol
  CopyMem ( &mHookTextOut.OldSTO, TextOut, sizeof(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL));

  //
  // Hook simple text out protocol function
  //
  TextOut->Reset             = TextReset;
  TextOut->OutputString      = TextOutputString;
  TextOut->SetMode           = TextSetMode;
  TextOut->ClearScreen       = TextClearScreen;

  // set Hooked flag
  mHookTextOut.IsHooked = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
UnHookSimpleTextOut (
  VOID
  )
{

  if (mHookTextOut.IsHooked == FALSE)
    return EFI_SUCCESS;
    
  CopyMem( mHookTextOut.PtrSimpleTextOut, &mHookTextOut.OldSTO, sizeof(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL));
  
  mHookTextOut.IsHooked = FALSE;

  return EFI_SUCCESS;
}

VOID
EFIAPI
SimpleTextOutCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{

  EFI_STATUS                      Status;
  UINTN                           HandleSize;
  EFI_HANDLE                      Handle;
  EFI_HANDLE                      VgaHandle;

  Status = GetActiveVgaHandle (&VgaHandle);

  if (EFI_ERROR(Status))
    return;
  
  while (TRUE) {
    HandleSize = sizeof (EFI_HANDLE);
    
    Status = gBS->LocateHandle (ByRegisterNotify, NULL, mStoRegistration, &HandleSize, &Handle);

    if (Status == EFI_NOT_FOUND) {
      return;
    }
    
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR(Status))
      continue;

    if (Handle == VgaHandle) {
      HookSimpleTextOut (Handle);
      gBS->CloseEvent (mSimpleTextOutEvent);
      mSimpleTextOutEvent = NULL;
      return;
    }
    
  }

  return;
}

EFI_STATUS
InitializeTextOutHook (
  VOID
  )
{

  EFI_STATUS    Status;
  EFI_HANDLE    VgaHandle;
  
  // Hook VGA simpleTextOut protocol
  Status = GetActiveVgaHandle(&VgaHandle);
  if (EFI_SUCCESS == Status) {
    HookSimpleTextOut (VgaHandle);
  }
  else {
    // Register SimpleTextOut protocol notify for save screen text
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SimpleTextOutCallback,
                    NULL,
                    &mSimpleTextOutEvent
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }
    
    Status = gBS->RegisterProtocolNotify (
                    &gEfiSimpleTextOutProtocolGuid,
                    mSimpleTextOutEvent,
                    &mStoRegistration
                    );
  }
  return Status;

}

EFI_STATUS
FinalizeTextOutHook (
  VOID
  )
{
  UnHookSimpleTextOut();

  return EFI_SUCCESS;
}

