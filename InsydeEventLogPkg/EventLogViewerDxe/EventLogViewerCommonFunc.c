/** @file

  Event Log Viewer Common Function implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <EventLogViewer.h>
#include <EventLogViewerCommonFunc.h>

#define CHAR_SPACE 0x0020

UINTN                     gDaysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
STORAGE_EVENT_LOG_INFO    *EventLogData;

STORAGE_EVENT_LOG_INFO     gFilteredEventLogData;
UINT16                     gCurrentStorageId=NO_STORAGE_BE_SELECTED;
UINTN                      gFilterBitMap=0;

SELECT_ITEM_INFO          SelectItemTable[] = {
  {L"Date", TypeDate},
  {L"Time", TypeTime},
  {L"Generator ID", TypeGeneratorID},
  {L"Event ID", TypeEventID},
  {L"Sensor Type", TypeSensorType},
  {L"Sensor Num", TypeSensorNum},
  {L"Event Type", TypeEventType},
  {L"Event Type ID", TypeEventTypeID},    
  {L"Clear Filtered Condition", TypeClearFilteredCondition},
  {NULL, TypeEnd},
};

VOID
EFIAPI
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  )
/*++

Routine Description:

  Set Buffer to Value for Size bytes.

Arguments:

  Buffer  - Memory to set.

  Size    - Number of bytes to set

  Value   - Value of the set operation.

Returns:

  None

--*/
{
  CHAR16  *Ptr;

  Ptr = Buffer;
  while (Size--) {
    *(Ptr++) = Value;
  }
}

VOID
EFIAPI
ClearLines (
  UINTN                                       LeftColumn,
  UINTN                                       RightColumn,
  UINTN                                       TopRow,
  UINTN                                       BottomRow,
  UINTN                                       TextAttribute
  )
{
  CHAR16      *Buffer;
  UINTN       Row;
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  EnCursor(FALSE);
  
  //
  // For now, allocate an arbitrarily long buffer
  //
  Status = gBS->AllocatePool (EfiBootServicesData, 0x10000, (VOID **)&Buffer);
  ASSERT (Buffer != NULL);
  ZeroMem (Buffer, 0x10000);

  //
  // Set foreground and background as defined
  //
  SetColor(TextAttribute);

  //
  // Much faster to buffer the long string instead of print it a character at a time
  //
  SetUnicodeMem (Buffer, RightColumn - LeftColumn, L' ');

  //
  // Clear the desired area with the appropriate foreground/background
  //
  for (Row = TopRow; Row <= BottomRow; Row++) {
    SetCurPos (LeftColumn, Row);
    Print (L"%s", Buffer);
  }

  SetCurPos(LeftColumn,TopRow);

  gBS->FreePool (Buffer);
  return ;
}

UINTN
EFIAPI
GetStringWidth (
  CHAR16                                      *String
  )
{
  UINTN Index;
  UINTN Count;
  UINTN IncrementValue;

  Index           = 0;
  Count           = 0;
  IncrementValue  = 1;

  do {
    //
    // Advance to the null-terminator or to the first width directive
    //
    for (;
         (String[Index] != NARROW_CHAR) && (String[Index] != WIDE_CHAR) && (String[Index] != 0);
         Index++, Count = Count + IncrementValue
        )
      ;

    //
    // We hit the null-terminator, we now have a count
    //
    if (String[Index] == 0) {
      break;
    }
    //
    // We encountered a narrow directive - strip it from the size calculation since it doesn't get printed
    // and also set the flag that determines what we increment by.(if narrow, increment by 1, if wide increment by 2)
    //
    if (String[Index] == NARROW_CHAR) {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 1;
    } else {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 2;
    }
  } while (String[Index] != 0);

  //
  // Increment by one to include the null-terminator in the size
  //
  Count++;

  return Count * sizeof (CHAR16);
}

VOID
EFIAPI
ProcessString (
  IN  CHAR16  *StringPtr,
  OUT CHAR16  **FormattedString,
  IN  UINTN   StringWidth,
  IN  UINTN   RowCount,
  OUT UINTN   *TotalLines
  )
/*++

Routine Description:

Arguments:

Returns:
  None.

--*/
{
  UINTN BlockWidth;
  UINTN AllocateSize;
  //
  // [PrevCurrIndex, CurrIndex) forms a range of a screen-line
  //
  UINTN CurrIndex;
  UINTN PrevCurrIndex;
  UINTN LineCount;
  UINTN VirtualLineCount;
  //
  // GlyphOffset stores glyph width of current screen-line
  //
  UINTN GlyphOffset;
  //
  // GlyphWidth equals to 2 if we meet width directive
  //
  UINTN GlyphWidth;
  //
  // during scanning, we remember the position of last space character
  // in case that if next word cannot put in current line, we could restore back to the position
  // of last space character
  // while we should also remmeber the glyph width of the last space character for restoring
  //
  UINTN LastSpaceIndex;
  UINTN LastSpaceGlyphWidth;
  //
  // every time we begin to form a new screen-line, we should remember glyph width of single character
  // of last line
  //
  UINTN LineStartGlyphWidth;
  UINTN *IndexArray;
  UINTN *OldIndexArray;

  BlockWidth = StringWidth - 1;
  
  //
  // every three elements of IndexArray form a screen-line of string:[ IndexArray[i*3], IndexArray[i*3+1] )
  // IndexArray[i*3+2] stores the initial glyph width of single character. to save this is because we want
  // to bring the width directive of the last line to current screen-line.
  // e.g.: "\wideabcde ... fghi", if "fghi" also has width directive but is splitted to the next screen-line
  // different from that of "\wideabcde", we should remember the width directive.
  //
  AllocateSize  = 0x20;
  gBS->AllocatePool (EfiBootServicesData, AllocateSize * sizeof (UINTN) * 3, (VOID **)&IndexArray);

  if (*FormattedString != NULL) {
    gBS->FreePool (*FormattedString);
    *FormattedString = NULL;
  }

  for (PrevCurrIndex = 0, CurrIndex  = 0, LineCount   = 0, LastSpaceIndex = 0,
       IndexArray[0] = 0, GlyphWidth = 1, GlyphOffset = 0, LastSpaceGlyphWidth = 1, LineStartGlyphWidth = 1;
       (StringPtr[CurrIndex] != CHAR_NULL);
       CurrIndex ++) {

    if (LineCount == AllocateSize) {
      AllocateSize += 0x10;
      OldIndexArray  = IndexArray;
      gBS->AllocatePool (EfiBootServicesData, AllocateSize * sizeof (UINTN) * 3, (VOID **)&IndexArray);
      CopyMem (IndexArray, OldIndexArray, LineCount * sizeof (UINTN) * 3);
      gBS->FreePool (OldIndexArray);
    }
    switch (StringPtr[CurrIndex]) {

    case NARROW_CHAR:
    case WIDE_CHAR:
      GlyphWidth = ((StringPtr[CurrIndex] == WIDE_CHAR) ? 2 : 1);
      if (CurrIndex == 0) {
        LineStartGlyphWidth = GlyphWidth;
      }
      break;

    //
    // char is '\n'
    // "\r\n" isn't handled here, handled by case CHAR_CARRIAGE_RETURN
    //
    case CHAR_LINEFEED:
      //
      // Store a range of string as a line
      //
      IndexArray[LineCount*3]   = PrevCurrIndex;
      IndexArray[LineCount*3+1] = CurrIndex;
      IndexArray[LineCount*3+2] = LineStartGlyphWidth;
      LineCount ++;
      //
      // Reset offset and save begin position of line
      //
      GlyphOffset = 0;
      LineStartGlyphWidth = GlyphWidth;
      PrevCurrIndex = CurrIndex + 1;
      break;

    //
    // char is '\r'
    // "\r\n" and "\r" both are handled here
    //
    case CHAR_CARRIAGE_RETURN:
      if (StringPtr[CurrIndex + 1] == CHAR_LINEFEED) {
        //
        // next char is '\n'
        //
        IndexArray[LineCount*3]   = PrevCurrIndex;
        IndexArray[LineCount*3+1] = CurrIndex;
        IndexArray[LineCount*3+2] = LineStartGlyphWidth;
        LineCount ++;
        CurrIndex ++;
      }
      GlyphOffset = 0;
      LineStartGlyphWidth = GlyphWidth;
      PrevCurrIndex = CurrIndex + 1;
      break;

    //
    // char is space or other char
    //
    default:
      GlyphOffset     += GlyphWidth;
      if (GlyphOffset > BlockWidth) {
        if (LastSpaceIndex > PrevCurrIndex) {
          //
          // LastSpaceIndex points to space inside current screen-line,
          // restore to LastSpaceIndex
          // (Otherwise the word is too long to fit one screen-line, just cut it)
          //
          CurrIndex  = LastSpaceIndex;
          GlyphWidth = LastSpaceGlyphWidth;
        } else if (GlyphOffset > BlockWidth) {
          //
          // the word is too long to fit one screen-line and we don't get the chance
          // of GlyphOffset == BlockWidth because GlyphWidth = 2
          //
          CurrIndex --;
        }

        IndexArray[LineCount*3]   = PrevCurrIndex;
        IndexArray[LineCount*3+1] = CurrIndex + 1;
        IndexArray[LineCount*3+2] = LineStartGlyphWidth;
        LineStartGlyphWidth = GlyphWidth;
        LineCount ++;
        //
        // Reset offset and save begin position of line
        //
        GlyphOffset                 = 0;
        PrevCurrIndex               = CurrIndex + 1;
      }

      //
      // LastSpaceIndex: remember position of last space
      //
      if (StringPtr[CurrIndex] == CHAR_SPACE) {
        LastSpaceIndex      = CurrIndex;
        LastSpaceGlyphWidth = GlyphWidth;
      }
      break;
    }
  }

  if (GlyphOffset > 0) {
    IndexArray[LineCount*3]   = PrevCurrIndex;
    IndexArray[LineCount*3+1] = CurrIndex;
    IndexArray[LineCount*3+2] = GlyphWidth;
    LineCount ++;
  }

  if (LineCount == 0) {
    //
    // in case we meet null string
    //
    IndexArray[0] = 0;
    IndexArray[1] = 1;
    //
    // we assume null string's glyph width is 1
    //
    IndexArray[1] = 1;
    LineCount ++;
  }

  *TotalLines = LineCount;
  
  VirtualLineCount = RowCount * (LineCount / RowCount + (LineCount % RowCount > 0));
  gBS->AllocatePool (EfiBootServicesData, VirtualLineCount * (BlockWidth + 1) * sizeof (CHAR16) * 2, (VOID **)FormattedString);
  ZeroMem (*FormattedString, VirtualLineCount * (BlockWidth + 1) * sizeof (CHAR16) * 2);
        
  for (CurrIndex = 0; CurrIndex < LineCount; CurrIndex ++) {
    *(*FormattedString + CurrIndex * 2 * (BlockWidth + 1)) = (IndexArray[CurrIndex*3+2] == 2) ? WIDE_CHAR : NARROW_CHAR;
    StrnCpy (
      *FormattedString + CurrIndex * 2 * (BlockWidth + 1) + 1,
      StringPtr + IndexArray[CurrIndex*3],
      IndexArray[CurrIndex*3+1]-IndexArray[CurrIndex*3]
      );
  }

  gBS->FreePool (IndexArray);
}

UINTN
EFIAPI
DisplayPopupMessage (
  IN CHAR16                      *String,
  IN VE_DIALOG_OPERATION         DialogOperator,
  IN EFI_SCREEN_DESCRIPTOR       LocalScreen
  )
{
  UINTN                                       Index;
  UINTN                                       Lines;
  UINTN                                       DlgNum;
  UINTN                                       RangeNum;
  UINTN                                       Select;
  UINTN                                       StrLenth;
  UINTN                                       DialogTop;
  UINTN                                       DialogBottom;
  UINTN                                       DialogLeft;
  UINTN                                       DialogRight;
  CHAR16                                      *ResStr[10];
  CHAR16                                      *FormattedString;
  BOOLEAN                                     LoopEnable;
  EFI_INPUT_KEY                               Key;

  Index = 0;
  Lines = 0;
  DlgNum = 0;
  FormattedString = NULL;
  
  StrLenth = GetStringWidth (String)/2;
  if (StrLenth < 10) {
    StrLenth = 10;
  } else if (StrLenth > (LocalScreen.RightColumn- 20)) {
    StrLenth = LocalScreen.RightColumn - 20;
  }
  
  ProcessString (String, &FormattedString, StrLenth, 10, &Lines);
  
  DialogTop     = LocalScreen.BottomRow/3 - 2;
  if (DialogOperator == VeDlgNoOperation) {
    DialogBottom  = DialogTop + Lines + 1;
  } else {
    DialogBottom  = DialogTop + Lines + 3;
  }
  DialogLeft    = LocalScreen.RightColumn/2 - (StrLenth + 2)/2;
  DialogRight   = LocalScreen.RightColumn/2 + (StrLenth + 2)/2;

  //
  // Print  border line
  // +-------------------------------------------+
  // |                                                                        |
  // |                                                                        |
  // |                                                                        |
  // |-------------------------------------------|
  // |                                                                        |
  // +-------------------------------------------+
  SetColor (VE_NORMAL_POPUP_TEXT);
  SetCurPos (DialogLeft, DialogTop);
  Print (L"%c", BOXDRAW_DOWN_RIGHT);
  
  SetCurPos (DialogLeft, DialogBottom);
  Print (L"%c", BOXDRAW_UP_RIGHT);
  
  SetCurPos (DialogRight, DialogTop);
  Print (L"%c", BOXDRAW_DOWN_LEFT);
  
  SetCurPos (DialogRight, DialogBottom);
  Print (L"%c", BOXDRAW_UP_LEFT);

  for (Index = DialogLeft + 1; Index < DialogRight; Index++) {
    SetCurPos (Index, DialogTop);
    Print (L"%c", BOXDRAW_HORIZONTAL);
    
    if (DialogOperator != VeDlgNoOperation) {
      SetCurPos (Index, DialogBottom - 2);
      Print (L"%c", BOXDRAW_HORIZONTAL);
    }
    
    SetCurPos (Index, DialogBottom);
    Print (L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = DialogTop + 1; Index < DialogBottom; Index++) {
    SetCurPos (DialogLeft, Index);
    Print (L"%c", BOXDRAW_VERTICAL);
    
    SetCurPos (DialogRight, Index);
    Print (L"%c", BOXDRAW_VERTICAL);
  }

  ClearLines (
    DialogLeft + 1, 
    DialogRight, 
    DialogTop + 1, 
    DialogTop + Lines, 
    VE_NORMAL_POPUP_TEXT);
  
  for (Index = 0; Index < Lines; Index++) {
    SetCurPos (DialogLeft + 1, DialogTop + Index + 1);
    Print (&FormattedString[Index * StrLenth * 2]);
  }

  if (DialogOperator == VeDlgNoOperation) {
    return 0;
  }
  
  switch (DialogOperator) {

  case VeDlgYesNo:
    ResStr [0] = L"Yes";
    ResStr [1] = L"No";
    DlgNum = 2;
    break;

  case VeDlgOk:
    ResStr [0] = L"OK";
    DlgNum = 1;
    break;
  } 
  
  LoopEnable = TRUE;
  Select     = 2;
  RangeNum   = StrLenth / (2 * DlgNum + 2);
  
  ClearLines (
    DialogLeft + 1, 
    DialogRight, 
    DialogBottom - 1, 
    DialogBottom - 1, 
    VE_NORMAL_POPUP_TEXT);
  
  while (LoopEnable){
    for (Index = 1; Index <= DlgNum; Index++) {
      if (Index == Select) {
        SetColor (VE_NORMAL_POPUP_SELECTED_TEXT);
      } else {
        SetColor (VE_NORMAL_POPUP_TEXT);
      }
      SetCurPos (DialogLeft + RangeNum * (2 * Index), DialogBottom - 1);
      Print (ResStr [Index - 1]);
    }
  
    gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
    gST->ConIn->ReadKeyStroke (gST->ConIn, &Key); 
  
    switch (Key.UnicodeChar) {

    case CHAR_CARRIAGE_RETURN:
      LoopEnable = FALSE;
      break;

    case CHAR_NULL:
      if (Key.ScanCode == SCAN_LEFT) {
        if (Select > 1) {
          Select--;
        }
      } else if (Key.ScanCode == SCAN_RIGHT) {
        if (Select < DlgNum) {
          Select++;
        }
      } else if (Key.ScanCode == SCAN_ESC) {
        LoopEnable = FALSE;
        Select = 0;
      }
      break;
    }
  }
  
  ClearLines (
    DialogLeft,
    DialogRight + 1,
    DialogTop,
    DialogBottom,
    VE_NORMAL_MENU_TEXT);

  if (FormattedString) {
    gBS->FreePool (FormattedString);
  }

  return Select;
}

VOID
EFIAPI
DisplayScrollbar (
  IN  UINTN                     CurrentLine,
  IN  UINTN                     TotalLines,
  IN  UINTN                     LinesPerPage,
  IN  UINTN                     PrintColumn,
  IN  UINTN                     PrintRow,
  IN  BOOLEAN                   UpArrow,
  IN  BOOLEAN                   DownArrow
  )
{
  UINTN               BarRow;
  UINTN               Index;
  UINTN               SkipSize;
  UINTN               PagebarSize;

  BarRow = 0;
  Index = 0;
  SkipSize = 0;
  PagebarSize = 0;
  
  BarRow = PrintRow + 1;

  if (TotalLines <= LinesPerPage) {
    for (Index = 0; Index < LinesPerPage; Index++) {
      SetColor (VE_NORMAL_MENU_TEXT);
      SetCurPos (PrintColumn, BarRow + Index + 1);
      Print (L" ");
    }
  } else {
    PagebarSize = (LinesPerPage * (LinesPerPage - 1))/TotalLines;
    if (PagebarSize < 1) PagebarSize = 1;
    
    if (CurrentLine > (TotalLines - LinesPerPage + 1)) {
      CurrentLine = TotalLines - LinesPerPage + 1;
    }
    SkipSize = (CurrentLine * (LinesPerPage - 1))/TotalLines;

    for (Index = 0; Index < (LinesPerPage - 1); Index++) {
      if ((UpArrow) && (DownArrow)) {
        if ((Index >= SkipSize) && 
            (Index < (SkipSize + PagebarSize))) {
          SetColor (VE_SELECT_BAR);
          SetCurPos (PrintColumn, BarRow + Index);
          Print (L"%c", BLOCKELEMENT_FULL_BLOCK);
        } else {
          SetColor (VE_UNSELECT_BAR);
          SetCurPos (PrintColumn, BarRow + Index);
          Print (L"%c", BLOCKELEMENT_FULL_BLOCK);
        }
      }
      if ((!UpArrow) && (DownArrow)) {
        if (Index < PagebarSize) {
          SetColor (VE_SELECT_BAR);
          SetCurPos (PrintColumn, BarRow + Index);
          Print (L"%c", BLOCKELEMENT_FULL_BLOCK);
        } else {
          SetColor (VE_UNSELECT_BAR);
          SetCurPos (PrintColumn, BarRow + Index);
          Print (L"%c", BLOCKELEMENT_FULL_BLOCK);
        }
      }
      if ((UpArrow) && (!DownArrow)) {
        if (Index >= (LinesPerPage - PagebarSize - 1)) {
          SetColor (VE_SELECT_BAR);
          SetCurPos (PrintColumn, BarRow + Index);
          Print (L"%c", BLOCKELEMENT_FULL_BLOCK);
        } else {
          SetColor (VE_UNSELECT_BAR);
          SetCurPos (PrintColumn, BarRow + Index);
          Print (L"%c", BLOCKELEMENT_FULL_BLOCK);
        }
      }
    }
    
    SetColor (VE_UNSELECT_BAR);
    SetCurPos (PrintColumn, PrintRow);
    Print (L"%c", GEOMETRICSHAPE_UP_TRIANGLE);
  
    SetCurPos (PrintColumn, PrintRow + LinesPerPage);
    Print (L"%c", GEOMETRICSHAPE_DOWN_TRIANGLE);
  }
  return;
}

EFI_INPUT_KEY
EFIAPI
VeMenuOperation (
  IN     UINTN        TopRow,
  IN     UINTN        MvLimit,
  IN     UINTN        EventCount,
  IN     UINTN        MaxItemPerPage,
  IN OUT UINTN        *Row,
  IN OUT UINTN        *TopOfScreen,
  IN OUT UINTN        *PagSelected
  )
{
  EFI_INPUT_KEY         Key;
  UINTN                 Index;

  Index = 0;
  
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);  

  switch (Key.UnicodeChar) {
    
  case 0:
    switch (Key.ScanCode) {
                
    case SCAN_PAGE_UP:
      if (EventCount > MaxItemPerPage) {
        if (*TopOfScreen >=  MaxItemPerPage) {
          *TopOfScreen = *TopOfScreen - MaxItemPerPage;
        } else {
          *TopOfScreen = 0;
        }
      }

      *Row = TopRow + 1;
      *PagSelected = 0;
      break;
      
    case SCAN_PAGE_DOWN:
      if (EventCount > MaxItemPerPage) {
        Index = EventCount - MaxItemPerPage;
        *TopOfScreen = *TopOfScreen + MaxItemPerPage;
        if (*TopOfScreen >  Index) {
          *TopOfScreen = Index;
        }
      }

      *Row = TopRow + 1;
      *PagSelected = 0;
      break;
      
    case SCAN_UP:
      if(*Row <= TopRow + 1) {
        *Row = TopRow + 1;
        
        if (EventCount > MaxItemPerPage) {
          if (*TopOfScreen > 0) {
            *TopOfScreen = *TopOfScreen - 1;
          }
        }
      } else {
        *Row = *Row - 1;
      }
      
      *PagSelected = *Row - TopRow - 1;

      break;
 
    case SCAN_DOWN:
      if(*Row >= (MvLimit + TopRow)) {
        *Row = MvLimit + TopRow;  
        
        if (EventCount > MaxItemPerPage) {
          Index = EventCount - MaxItemPerPage;
          if (*TopOfScreen < Index) {
            *TopOfScreen = *TopOfScreen + 1;
          }
        }
      } else {
        *Row = *Row + 1;
      }
      *PagSelected = *Row - TopRow - 1;

      break;
      
    default:
      break;
    }
  }

  return Key;
}

VOID
EFIAPI
DsplayHotkeyInfo1 (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  )
{
  ClearLines (
    LocalScreen.LeftColumn, 
    LocalScreen.RightColumn - 1, 
    LocalScreen.BottomRow - 2, 
    LocalScreen.BottomRow - 1, 
    VE_BOTTOM_HELP_BACKGROUND);

  SetCurPos (LocalScreen.LeftColumn + 1, LocalScreen.BottomRow - 2);
  Print (L"%c%c    ", ARROW_UP, ARROW_DOWN);
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Select Item  ");
  
  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 20, LocalScreen.BottomRow - 2);
  Print (L"[Enter] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Select%cItem  ", GEOMETRICSHAPE_RIGHT_TRIANGLE);
  
  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 41, LocalScreen.BottomRow - 2);
  Print (L"[Esc] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Exit  ");

  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 53, LocalScreen.BottomRow - 2);
  Print (L"[F1] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Show/Hide Version  ");

  if (gShowUtilVer == TRUE) {
    SetColor (VE_BOTTOM_HELP_BACKGROUND);
    SetCurPos (LocalScreen.RightColumn - 16, LocalScreen.BottomRow - 1);
    Print (L"%s", gUtilityVersion);
  }
  return;
}

VOID
EFIAPI
DsplayHotkeyInfo2 (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  )
{
  ClearLines (
    LocalScreen.LeftColumn, 
    LocalScreen.RightColumn - 1, 
    LocalScreen.BottomRow - 2, 
    LocalScreen.BottomRow - 1, 
    VE_BOTTOM_HELP_BACKGROUND);

  SetCurPos (LocalScreen.LeftColumn + 1, LocalScreen.BottomRow - 2);
  Print (L"%c%c    ", ARROW_UP, ARROW_DOWN);
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Select Item  ");
  
  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 28, LocalScreen.BottomRow - 2);
  Print (L"[PagUp][PagDn] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Cange Page  ");
  
  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 55, LocalScreen.BottomRow - 2);
  Print (L"[Enter] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Select%cItem  ", GEOMETRICSHAPE_RIGHT_TRIANGLE);
  
  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 1, LocalScreen.BottomRow - 1);
  Print (L"[Esc] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Return to Main menu  ");

  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 28, LocalScreen.BottomRow - 1);
  Print (L"[F3] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Filter Event Log  ");

  return;
}

VOID
EFIAPI
DsplayHotkeyInfo3 (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  )
{
  ClearLines (
    LocalScreen.LeftColumn, 
    LocalScreen.RightColumn - 1, 
    LocalScreen.BottomRow - 2, 
    LocalScreen.BottomRow - 1, 
    VE_BOTTOM_HELP_BACKGROUND);

  SetCurPos (LocalScreen.LeftColumn + 1, LocalScreen.BottomRow - 2);
  Print (L"%c%c    ", ARROW_UP, ARROW_DOWN);
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Change Item  ");
  
  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 26, LocalScreen.BottomRow - 2);
  Print (L"[Q] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Return to Main menu  ");

  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 1, LocalScreen.BottomRow - 1);
  Print (L"[ESC] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Back to last page  ");

  SetColor (VE_BOTTOM_HELP_BACKGROUND);
  SetCurPos (LocalScreen.LeftColumn + 26, LocalScreen.BottomRow - 1);
  Print (L"[F2] ");
  SetColor (VE_BOTTOM_HELP_TEXT);
  Print (L"Save Event to File  ");

  return;
}

EFI_STATUS
EFIAPI
CatViewEventLogString (
  IN     CHAR16            *StorageName,
  IN OUT CHAR16            **ViewLogString
)
/*++

Routine Description:

  Concatenate View Event Log String with Storage name string.
  
Arguments:

  StorageName    Name string of storage.
  ViewLogString  View Event Log String.
  
Returns:

  EFI_SUCCESS
  
--*/
{
  CHAR16         *Str1 = L"  ";
  CHAR16         *Str2 = L" Event Log Viewer";
  CHAR16         *TempStr;
  UINTN          TempStrLen;

  TempStrLen = (StrLen (Str1) + StrLen (Str2) + StrLen (StorageName)) * 2 + 2; 

  gBS->AllocatePool (EfiBootServicesData, TempStrLen, (VOID **)&TempStr);

  StrCpy (TempStr, Str1);
  StrCat (TempStr, StorageName);
  StrCat (TempStr, Str2);

  *ViewLogString = TempStr;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CatClearEventLogString (
  IN     CHAR16            *StorageName,
  IN OUT CHAR16            **ClearLogString
)
/*++

Routine Description:

  Concatenate Clear Event Log String with Storage name string.
  
Arguments:

  StorageName     Name string of storage.
  ClearLogString  Clear Event Log String.
  
Returns:

  EFI_SUCCESS
  
--*/
{
  CHAR16         *Str1 = L"  Clear ";
  CHAR16         *Str2 = L" Event Log";
  CHAR16         *TempStr;
  UINTN          TempStrLen;

  TempStrLen = (StrLen (Str1) + StrLen (Str2) + StrLen (StorageName)) * 2 + 2; 
  gBS->AllocatePool (EfiBootServicesData, TempStrLen, (VOID **)&TempStr);

  StrCpy (TempStr, Str1);
  StrCat (TempStr, StorageName);
  StrCat (TempStr, Str2);

  *ClearLogString = TempStr;
  return EFI_SUCCESS;
}

VOID
EFIAPI
StrIndexedCat (
  IN     CHAR16   *SrcStr,
  IN     UINTN    Index,
  IN     CHAR16   *InsertStr,
  IN OUT CHAR16   **OutStr
  )
/*++

Routine Description:

  Insert a string to an original string.

Arguments:

  SrcStr      - Original source string.
  Index       - Index of inserting a string to the source string.
  InsertStr   - String to added to the index of the source string.
  OutStr      - Return modified string.

Returns:
  NONE

--*/
{   
  UINTN                        StrLength;

  StrLength = (StrLen(SrcStr) + StrLen(InsertStr)) * 2 + 2;

  gBS->AllocatePool (EfiBootServicesData, StrLength, (VOID **)OutStr);

  if (Index == 0) {
    StrnCpy (*OutStr, InsertStr, StrLen(InsertStr));    
    StrCpy (&((*OutStr)[StrLen(InsertStr)]), SrcStr);
  } else {
    StrnCpy (*OutStr, SrcStr, Index);
    StrnCpy (&((*OutStr)[Index]), InsertStr, StrLen(InsertStr));
    StrCpy (&((*OutStr)[Index + StrLen(InsertStr)]), &(SrcStr[Index]));
  }
  
}


EFI_STATUS
EFIAPI
ViewStorageEventLog (
  IN  UINTN                             StorageId,
  IN  EFI_SCREEN_DESCRIPTOR             LocalScreen
  )
/*++

Routine Description:

  View event logged data list of specific storage.

Arguments:

  StorageId       Index ID for specific storage.
  LocalScreen     Local screen descriptor.

Returns:

  EFI_STATUS

--*/
{
  BOOLEAN                               Quit;
  BOOLEAN                               UpArrow;
  BOOLEAN                               DownArrow;
  EFI_STATUS                            Status;
  UINTN                                 EventCount;
  UINTN                                 Index;
  UINTN                                 TopOfScreen;
  UINTN                                 PagSelected;
  UINTN                                 MvLimit;
  UINTN                                 Row;
  EFI_INPUT_KEY                         Key;
  STORAGE_EVENT_LOG_INFO                *EventData;
  CHAR16                                *EventLogWindowStr;

  Quit = FALSE;
  UpArrow = FALSE;
  DownArrow = FALSE;
  EventCount = 0;
  MvLimit = gMaxItemPerPage;
  gCurrentStorageId = (UINT16)StorageId;

  //
  // Set default filter condition
  //
  SetMem(&gFilteredEventLogData, sizeof(STORAGE_EVENT_LOG_INFO), 0xFF);
  gFilterBitMap = 0;

  if (gEventHandler == NULL) {
    DisplayPopupMessage (L"EfiEventHandlerProtocol not found!)!", VeDlgOk, LocalScreen);
    return EFI_UNSUPPORTED;
  }

  DisplayPopupMessage (L"Loading......", VeDlgNoOperation, LocalScreen);
  Status = gEventHandler->RefreshDatabase (MenuList[StorageId].StorageString, &EventCount);
  if (EFI_ERROR (Status)) {
    //
    // If returned status is not EFI_SUCCESS, show the error messsage.
    //
    DisplayPopupMessage (L"WARNING!!! Please try again.", VeDlgOk, LocalScreen);
    return Status;
  }

  //
  // If there are no logs exist, return to main menu
  //
  if (EventCount == 0) {
    DisplayPopupMessage (L"There are no error logs exists!!", VeDlgOk, LocalScreen);
    return Status;
  }
  
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof (STORAGE_EVENT_LOG_INFO) * EventCount, 
                          (VOID **)&EventLogData
                          );
  ASSERT_EFI_ERROR (Status);
  ZeroMem (EventLogData, sizeof (STORAGE_EVENT_LOG_INFO) * EventCount);

  for (Index = 0; Index < EventCount; Index++) {
    Status = gEventHandler->GetEvent (MenuList[StorageId].StorageString, Index, (UINT8 **)&EventData);
    if (EFI_ERROR (Status)) {
      break;
    }
    CopyMem (&EventLogData[EventCount - Index - 1], EventData, sizeof (STORAGE_EVENT_LOG_INFO));
  }

  
  PagSelected = 0;
  Row = PagSelected + gTopRow + 1;
  TopOfScreen = 0;
  
  //
  // Display Subtitle information
  //
  SetColor (VE_SUBTITLE_TEXT);
  SetCurPos (LocalScreen.LeftColumn + 1, LocalScreen.TopRow + 1);
  StrIndexedCat (L" Event Log Viewer", 0, MenuList[StorageId].StorageString, &EventLogWindowStr);
  Print(L"%s", EventLogWindowStr);
  gBS->FreePool (EventLogWindowStr);
  
  SetCurPos (LocalScreen.RightColumn - 18, LocalScreen.TopRow + 1);
  Print (L"Total Logs :     ");
  
  SetColor (VE_SUBTITLE_DATA);
  SetCurPos (LocalScreen.RightColumn - 5, LocalScreen.TopRow + 1);
  Print (L"%4d", EventCount );
  
  //
  // Show Event Log From BIOS ROM Part
  //
  ClearLines (
    gColumn,
    LocalScreen.RightColumn - 1,
    gTopRow,
    gBottomRow,
    VE_NORMAL_MENU_TEXT);
  
  DsplayHotkeyInfo2 (LocalScreen);
  
  while (!Quit) {  
    PrintStorageEventList (LocalScreen, EventCount, TopOfScreen, PagSelected);

    if (EventCount < gMaxItemPerPage) {
      MvLimit = EventCount;  
      ClearLines (
        gColumn,
        LocalScreen.RightColumn - DEFAULT_SCROLLBAR_WIDTH - 2,
        MvLimit + gTopRow + 1,
        gBottomRow,
        VE_NORMAL_MENU_TEXT);
    } else {
      MvLimit = gMaxItemPerPage;
    }
    
    if (TopOfScreen > 0) {
      UpArrow = TRUE;
    } else {
      UpArrow = FALSE;
    }
    
    if (TopOfScreen +  gMaxItemPerPage < EventCount) {
      DownArrow = TRUE;
    } else {
      DownArrow = FALSE;
    }

    if (EventCount > gMaxItemPerPage) {
      DisplayScrollbar (
        TopOfScreen,
        EventCount,
        gMaxItemPerPage,
        LocalScreen.RightColumn - 2,
        gTopRow,
        UpArrow,
        DownArrow
        );
    }
    
    SetCurPos (gColumn, PagSelected + gTopRow + 1);

    Key = VeMenuOperation (
            gTopRow,
            MvLimit,
            EventCount,
            gMaxItemPerPage,
            &Row,
            &TopOfScreen,
            &PagSelected
            );
    
    switch (Key.UnicodeChar) {
      
    default:
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        if (DumpStorageEvent (MenuList[StorageId].StorageString, LocalScreen, EventCount, TopOfScreen + PagSelected, &TopOfScreen, &PagSelected)) {
          Quit = TRUE;
          EnCursor (FALSE);
        }
        Row = PagSelected + gTopRow + 1;
        DsplayHotkeyInfo2 (LocalScreen);
        break;
      }
      
    case 0:
      switch (Key.ScanCode) {
                
      case SCAN_ESC:
        Quit = TRUE;
        break;

      case SCAN_F3:       
        Status = EventLogFilter (LocalScreen, &EventCount, TopOfScreen, PagSelected);
        if (Status == EFI_SUCCESS) {
          //
          // Reset cursor position
          //
          PagSelected = 0;
          Row = PagSelected + gTopRow + 1;
          TopOfScreen = 0;
          SetCurPos (LocalScreen.RightColumn - 18, LocalScreen.TopRow + 1);
          Print (L"Total Logs :     ");
          SetColor (VE_SUBTITLE_DATA);
          SetCurPos (LocalScreen.RightColumn - 5, LocalScreen.TopRow + 1);
          Print (L"%4d", EventCount );
        } 
        break;
        
      default:
        break;
      }
    }
  }

  ClearLines (
    LocalScreen.LeftColumn, 
    LocalScreen.RightColumn - 1, 
    LocalScreen.TopRow + 1, 
    LocalScreen.TopRow + 1, 
    VE_SUBTITLE_TEXT);

  ClearLines (
    gColumn,
    LocalScreen.RightColumn - 2,
    gTopRow,
    gBottomRow,
    VE_NORMAL_MENU_TEXT);
  
  gBS->FreePool (EventLogData);
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EventLogFilter (
  IN EFI_SCREEN_DESCRIPTOR     LocalScreen,
  IN OUT UINTN                 *EventCount,
  IN OUT UINTN                 TopOfScreen,
  IN OUT UINTN                 PagSelected
  ) 
{
  EFI_STATUS                            Status;
  UINT32                                SelectedNum=NO_DIALOGUE_RESPONSE;
  CHAR16                                *KeyWord = NULL;
  FILTER_EVENT_INFO                     FilterEventInfo;
  UINTN                                 UpdateEventCount=0;

  //
  //Show Current condition of searching
  //
  if (!StrCmp(MenuList[gCurrentStorageId].StorageString, L"BMC SEL")) {
    ShowCurrentConditionMsg (
      LocalScreen, 
      FALSE,
      &SelectItemTable[TypeDate],
      &SelectItemTable[TypeTime],
      &SelectItemTable[TypeGeneratorID],
      &SelectItemTable[TypeSensorType],
      &SelectItemTable[TypeSensorNum],
      &SelectItemTable[TypeEventType],
      &SelectItemTable[TypeEnd]
      );    
  } else {
    ShowCurrentConditionMsg (
      LocalScreen, 
      FALSE,
      &SelectItemTable[TypeDate],
      &SelectItemTable[TypeTime],
      &SelectItemTable[TypeEventID],
      &SelectItemTable[TypeEventTypeID],
      &SelectItemTable[TypeEnd]
      );    
  }
  SetMem (&FilterEventInfo, sizeof(FILTER_EVENT_INFO), 0xFF);
  //
  // 1. Display Message and Get FilterType
  //  
  if (!StrCmp(MenuList[gCurrentStorageId].StorageString, L"BMC SEL")) {
    SelectedNum = DisplayDialogueMessage (
                                           LocalScreen,
                                           &SelectItemTable[TypeDate],
                                           &SelectItemTable[TypeTime],
                                           &SelectItemTable[TypeGeneratorID],
                                           &SelectItemTable[TypeSensorType],
                                           &SelectItemTable[TypeSensorNum],
                                           &SelectItemTable[TypeEventType],
                                           &SelectItemTable[TypeClearFilteredCondition],
                                           &SelectItemTable[TypeEnd]
                                           );
  } else {
    SelectedNum = DisplayDialogueMessage (
                                         LocalScreen,
                                         &SelectItemTable[TypeDate],
                                         &SelectItemTable[TypeTime],
                                         &SelectItemTable[TypeEventID],
                                         &SelectItemTable[TypeEventTypeID],
                                         &SelectItemTable[TypeClearFilteredCondition],
                                         &SelectItemTable[TypeEnd]
                                         );
  }
  if (SelectedNum == NO_DIALOGUE_RESPONSE) {
    //
    //clear Current condition of searching message
    //
    ShowCurrentConditionMsg (LocalScreen, TRUE, &SelectItemTable[TypeEnd]);
    return EFI_UNSUPPORTED;
  }
  
  //
  // clear Filter Message
  //
  PrintStorageEventList (LocalScreen, *EventCount, TopOfScreen, PagSelected);

  //
  //Show Current condition of searching
  //
  if (!StrCmp(MenuList[gCurrentStorageId].StorageString, L"BMC SEL")) {
    ShowCurrentConditionMsg (
      LocalScreen, 
      FALSE,
      &SelectItemTable[TypeDate],
      &SelectItemTable[TypeTime],
      &SelectItemTable[TypeGeneratorID],
      &SelectItemTable[TypeSensorType],
      &SelectItemTable[TypeSensorNum],
      &SelectItemTable[TypeEventType],
      &SelectItemTable[TypeEnd]
      );    
  } else {
    ShowCurrentConditionMsg (
      LocalScreen, 
      FALSE,
      &SelectItemTable[TypeDate],
      &SelectItemTable[TypeTime],
      &SelectItemTable[TypeEventID],
      &SelectItemTable[TypeEventTypeID],
      &SelectItemTable[TypeEnd]
      );    
  }

  if (SelectedNum == TypeClearFilteredCondition) {
    ArrangeFilterCondition (TRUE, &FilterEventInfo);
    Status = SortOutEventLogDataBaseByFilterCondition (LocalScreen, &UpdateEventCount);
    if (Status==EFI_SUCCESS) {
      *EventCount = UpdateEventCount;
    }
    return Status;
  }
  
  //
  // 2. Display message and get filter string
  //  
  KeyWord = DisplayMsgAndGetKeyWordByFilterType (SelectedNum, LocalScreen);
  if (KeyWord == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // 3. Transfer String to data format
  //
  FilterEventInfo.FilterType = SelectedNum;
  Status = TransferStringToData (KeyWord, &FilterEventInfo);
  FreePool (KeyWord);
  if (Status != EFI_SUCCESS) {
    //
    // clear Filter Message
    //
    PrintStorageEventList (LocalScreen, *EventCount, TopOfScreen, PagSelected);
    DisplayPopupMessage (L"Invalid Parameter!", VeDlgNoOperation, LocalScreen);
    WaitForAKey (0);
    return EFI_UNSUPPORTED;
  }

  ArrangeFilterCondition (FALSE, &FilterEventInfo);

  Status = SortOutEventLogDataBaseByFilterCondition (LocalScreen, &UpdateEventCount);
  if (Status==EFI_SUCCESS) {
    *EventCount = UpdateEventCount;
  }

  return Status;
}

VOID 
EFIAPI
ArrangeFilterCondition (
  IN  BOOLEAN                                       ResetFilter,
  IN  FILTER_EVENT_INFO                             *Data
  ) 
{
  if (ResetFilter==TRUE) {   
    //
    // Make sue the event log data base has no filtered condition.
    //
    SetMem(&gFilteredEventLogData, sizeof(STORAGE_EVENT_LOG_INFO), 0xFF);
    gFilterBitMap = 0;
    return;
  }

  if (Data==NULL) {    
    return;
  }

  if (Data->Year != 0xFFFF) {
    gFilteredEventLogData.Year = Data->Year;
    gFilterBitMap |= B_FILTERED_BY_YEAR;
  }
  if (Data->Month!= 0xFF) {
    gFilteredEventLogData.Month = Data->Month;
    gFilterBitMap |= B_FILTERED_BY_MONTH;
  }
  if (Data->Date != 0xFF) {
    gFilteredEventLogData.Date = Data->Date;    
    gFilterBitMap |= B_FILTERED_BY_DATE;
  }
  if (Data->Hour != 0xFF) {
    gFilteredEventLogData.Hour = Data->Hour;
    gFilterBitMap |= B_FILTERED_BY_HOUR;
  }
  if (Data->Minute != 0xFF) {
    gFilteredEventLogData.Minute = Data->Minute;
    gFilterBitMap |= B_FILTERED_BY_MINUTE;
  }
  if (Data->Second != 0xFF) {
    gFilteredEventLogData.Second = Data->Second;
    gFilterBitMap |= B_FILTERED_BY_SECOND;
  }
  if (Data->GeneratorId != 0xFFFF) {
    gFilteredEventLogData.GeneratorId = Data->GeneratorId;
    gFilterBitMap |= B_FILTERED_BY_GENERATOR_ID;
  }
  if (Data->EventID.SensorType != 0xFF) {
    gFilteredEventLogData.EventID.SensorType = Data->EventID.SensorType;
    gFilterBitMap |= B_FILTERED_BY_SENSOR_TYPE;
  }
  if (Data->EventID.SensorNum != 0xFF) {
    gFilteredEventLogData.EventID.SensorNum = Data->EventID.SensorNum;
    gFilterBitMap |= B_FILTERED_BY_SENSOR_NUM;
  }
  if (Data->EventID.EventType != 0xFF) {
    gFilteredEventLogData.EventID.EventType = Data->EventID.EventType;
    gFilterBitMap |= B_FILTERED_BY_EVENT_TYPE;
  }
  if (Data->EventTypeID != 0xFF) {
    gFilteredEventLogData.EventTypeID = Data->EventTypeID;
    gFilterBitMap |= B_FILTERED_BY_EVENT_TYPE_ID;
  }

  return;
}

EFI_STATUS 
EFIAPI
SortOutEventLogDataBaseByFilterCondition (
  IN EFI_SCREEN_DESCRIPTOR                         LocalScreen,  
  IN OUT UINTN                                     *EventCount
  ) 
{
  EFI_STATUS                            Status;
  UINTN                                 LocalEventCount;
  UINTN                                 Index;
  STORAGE_EVENT_LOG_INFO                *EventData;
  UINTN                                 MatchFiterCount=0;
  UINTN                                 FilterIndex=0;
    
  if (gCurrentStorageId==NO_STORAGE_BE_SELECTED) {    
    return EFI_UNSUPPORTED;
  }

  DisplayPopupMessage (L"Filtering......", VeDlgNoOperation, LocalScreen);
  Status = gEventHandler->RefreshDatabase (MenuList[gCurrentStorageId].StorageString, &LocalEventCount);
  if (EFI_ERROR (Status)) {
    //
    // If returned status is not EFI_SUCCESS, show the error messsage.
    //
    DisplayPopupMessage (L"WARNING!!! Please try again.", VeDlgOk, LocalScreen);
    return Status;
  }

  //
  // If there are no logs exist, return to main menu
  //
  if (LocalEventCount == 0) {
    DisplayPopupMessage (L"There are no error logs exists!!", VeDlgOk, LocalScreen);
    return Status;
  }

  //
  // Count event which are matched filter condition
  //
  if (gFilterBitMap!=0) {
    //
    // Filter event data
    //
    for (Index = 0; Index < LocalEventCount; Index++) {
      Status = gEventHandler->GetEvent (MenuList[gCurrentStorageId].StorageString, Index, (UINT8 **)&EventData);
      if (EFI_ERROR (Status)) {
        break;
      }
      if (IsMatchFilterCondition(EventData)) {
        MatchFiterCount++;
      }
    }
  }else {
    //
    // Don't filter any event data. Show all event data
    //
    MatchFiterCount = LocalEventCount;
  }
  if (MatchFiterCount != 0) {
    if (EventLogData!=NULL) {
      //
      // Free old Event Log Data base for to create new filtered Event Log Data base
      //
      gBS->FreePool (EventLogData);
    }
    //
    // Reset event log data base by filter condition
    //
    Status = gBS->AllocatePool (
                            EfiBootServicesData, 
                            sizeof (STORAGE_EVENT_LOG_INFO) * MatchFiterCount, 
                            (VOID **)&EventLogData
                            );
    ASSERT_EFI_ERROR (Status);
    ZeroMem (EventLogData, sizeof (STORAGE_EVENT_LOG_INFO) * MatchFiterCount);

    for (Index = 0; Index < LocalEventCount; Index++) {
      Status = gEventHandler->GetEvent (MenuList[gCurrentStorageId].StorageString, Index, (UINT8 **)&EventData);
      if (EFI_ERROR (Status)) {
        break;
      }
      if (IsMatchFilterCondition(EventData) && (gFilterBitMap != 0)) {
        CopyMem (&EventLogData[MatchFiterCount - FilterIndex - 1], EventData, sizeof (STORAGE_EVENT_LOG_INFO));
        FilterIndex++;
      } else if (gFilterBitMap == 0) { 
        CopyMem (&EventLogData[MatchFiterCount - FilterIndex - 1], EventData, sizeof (STORAGE_EVENT_LOG_INFO));
        FilterIndex++;
      }
    }
    *EventCount = MatchFiterCount;
    return EFI_SUCCESS;
  } else {
    //
    // Not found matched event. Don't do anthing.
    //
    DisplayPopupMessage (L"WARNING!!! No filter condition be matched", VeDlgOk, LocalScreen);
  }
  
  return EFI_NOT_FOUND;
  
}

BOOLEAN 
EFIAPI
IsMatchFilterCondition (
  IN STORAGE_EVENT_LOG_INFO                *EventData
  ) 
{
  UINTN                                    LocalFilterBitMap=0;

  if (gFilterBitMap==0) {
    //
    // No filter existence
    //
    return FALSE;
  }
  
  if ((gFilteredEventLogData.Year != 0xFFFF) &&
      (gFilteredEventLogData.Year == EventData->Year)) {
    LocalFilterBitMap |= B_FILTERED_BY_YEAR;
  }
  if ((gFilteredEventLogData.Month != 0xFF) &&
      (gFilteredEventLogData.Month == EventData->Month)) {
    LocalFilterBitMap |= B_FILTERED_BY_MONTH;
  }
  if ((gFilteredEventLogData.Date != 0xFF) &&
      (gFilteredEventLogData.Date == EventData->Date)) {   
    LocalFilterBitMap |= B_FILTERED_BY_DATE;
  }
  if ((gFilteredEventLogData.Hour != 0xFF) &&
      (gFilteredEventLogData.Hour == EventData->Hour)) {
    LocalFilterBitMap |= B_FILTERED_BY_HOUR;
  }
  if ((gFilteredEventLogData.Minute != 0xFF) &&
      (gFilteredEventLogData.Minute == EventData->Minute)) {
    LocalFilterBitMap |= B_FILTERED_BY_MINUTE;
  }
  if ((gFilteredEventLogData.Second != 0xFF) &&
      (gFilteredEventLogData.Second == EventData->Second)) {
    LocalFilterBitMap |= B_FILTERED_BY_SECOND;
  }
  if ((gFilteredEventLogData.GeneratorId != 0xFFFF) &&
      (gFilteredEventLogData.GeneratorId == EventData->GeneratorId)) {
    LocalFilterBitMap |= B_FILTERED_BY_GENERATOR_ID;
  }
  if ((gFilteredEventLogData.EventID.SensorType != 0xFF) &&
      (gFilteredEventLogData.EventID.SensorType == EventData->EventID.SensorType)) {
    LocalFilterBitMap |= B_FILTERED_BY_SENSOR_TYPE;
  }
  if ((gFilteredEventLogData.EventID.SensorNum != 0xFF) &&
      (gFilteredEventLogData.EventID.SensorNum == EventData->EventID.SensorNum)) {
    LocalFilterBitMap |= B_FILTERED_BY_SENSOR_NUM;
  }
  if ((gFilteredEventLogData.EventID.EventType != 0xFF) &&
      (gFilteredEventLogData.EventID.EventType == EventData->EventID.EventType)) {
    LocalFilterBitMap |= B_FILTERED_BY_EVENT_TYPE;
  }
  if ((gFilteredEventLogData.EventTypeID != 0xFF) &&
      (gFilteredEventLogData.EventTypeID == EventData->EventTypeID)) {
    LocalFilterBitMap |= B_FILTERED_BY_EVENT_TYPE_ID;
  }

  if (gFilterBitMap==LocalFilterBitMap) {
    //
    // Match all of filtered condition
    //
    return TRUE;
  }
  return FALSE;
}

VOID
EFIAPI
PrintStorageEventList (
  IN EFI_SCREEN_DESCRIPTOR     LocalScreen,
  IN UINTN                     EventCount,
  IN OUT UINTN                 TopOfScreen,
  IN OUT UINTN                 PagSelected
  )
/*++

Routine Description:

  Print event data list for selected page.

Arguments:

  LocalScreen     Local screen descriptor.
  EventCount      Count of event data list.
  TopOfScreen     Top of screen. 
  PagSelected     Selected page.

Returns:

  VOID

--*/
{ 
  UINTN                           Index, Index2, Limit;
  UINTN                           Row;
  UINTN                           TempLen;
  UINTN                           CurPosX;
  UINTN                           StrLenLimit;
  UINTN                           StringSize;  
  UINT8                           EventType;
  CHAR16                          *EventString;
  CHAR16                          *TempString;

  TempLen = 0;
  CurPosX = 0;
  StrLenLimit = 0;
  StringSize = 0;  
  EventType = 0xFF;
  EventString = NULL;
  TempString = NULL;
  
  Row = 0;
  Index = 0;
  Index2 = TopOfScreen;
  Limit = gMaxItemPerPage;
  if (EventCount < gMaxItemPerPage) {
    Limit = EventCount;
  }
  
  SetCurPos (gColumn, gTopRow);
  SetColor (VE_LIST_TITLE_TEXT);
  Print (L"Index   Date       Time      String");

  while(Index < Limit) {
    if ((Index % gMaxItemPerPage) == PagSelected) {
      SetColor (VE_SELECTED_MENU_TEXT);
    } else {
      SetColor (VE_NORMAL_MENU_TEXT);
    }
    
    SetCurPos (gColumn, gTopRow + Row + 1);
    
    Print (L"[%04d]  ", Index2 + 1);
    Print (L"%d/%02d/%02d %02d:%02d:%02d  ", 
           EventLogData[Index2].Year, 
           EventLogData[Index2].Month, 
           EventLogData[Index2].Date, 
           EventLogData[Index2].Hour, 
           EventLogData[Index2].Minute, 
           EventLogData[Index2].Second);
    
    CurPosX = CURRENT_POSX;
    StrLenLimit = LocalScreen.RightColumn - CurPosX - 3;

    if (!GetEventString ((VOID *)&EventLogData[Index2], &EventString, &StringSize)) {

      TempLen = StrLen (EventString);
      if (TempLen > StrLenLimit) {
        gBS->AllocatePool (EfiBootServicesData, (StrLenLimit+1) * sizeof (CHAR16), (VOID **)&TempString);
        ZeroMem (TempString, (StrLenLimit+1) * sizeof (CHAR16));
        StrnCpy (TempString, EventString, StrLenLimit);
        Print (TempString);
        gBS->FreePool (TempString);
      } else if (TempLen < StrLenLimit) {
        Print (EventString);
        CurPosX = CURRENT_POSX;
        StrLenLimit = LocalScreen.RightColumn - CurPosX - 3;
        if (StrLenLimit > 0) {
          gBS->AllocatePool (EfiBootServicesData, (StrLenLimit+1) * sizeof (CHAR16), (VOID **)&TempString);
          ZeroMem (TempString, (StrLenLimit+1) * sizeof (CHAR16));
          SetUnicodeMem (TempString, StrLenLimit, L' ');
          Print (TempString);
          gBS->FreePool (TempString);
        }
      } else {
        Print (EventString);
      }
    
      if (TempLen != 0) {
        gBS->FreePool (EventString);
      }
    } else {
      Print (L"Undefined");
      CurPosX = CURRENT_POSX;
      StrLenLimit = LocalScreen.RightColumn - CurPosX - 3;
      gBS->AllocatePool (EfiBootServicesData, (StrLenLimit+1) * sizeof (CHAR16), (VOID **)&TempString);
      ZeroMem (TempString, (StrLenLimit+1) * sizeof (CHAR16));
      SetUnicodeMem (TempString, StrLenLimit, L' ');
      Print (TempString);
      gBS->FreePool (TempString);
    } 
    
    Print (L"\n");
    
    Index++;
    Index2++;
    Row++;
  }
  
  //Restore color setting
  SetColor (VE_NORMAL_MENU_TEXT);
}

BOOLEAN
EFIAPI
DumpStorageEvent (  
  IN CHAR16                    *StorageName,
  IN EFI_SCREEN_DESCRIPTOR     LocalScreen,
  IN UINTN                     EventCount,
  IN UINTN                     SelectedLog,
  IN OUT UINTN                 *TopOfScreen,
  IN OUT UINTN                 *PagSelected
  )
/*++

Routine Description:

  Dump event data of selected storage.
  
Arguments:

  LocalScreen     Local screen descriptor.
  EventCount      Count of event data list.
  SelectedLog     Selected event data.
  TopOfScreen     Top of screen. 
  PagSelected     Selected page.

Returns:

  FALSE           Return.

--*/

{
  EFI_STATUS                                Status;
  EFI_INPUT_KEY                             Key;
  UINTN                                     Index;

  Index = 0;
  
  ClearLines (
    gColumn,
    LocalScreen.RightColumn - 1,
    gTopRow,
    gBottomRow,
    VE_NORMAL_MENU_TEXT);
  
  DsplayHotkeyInfo3 (LocalScreen);
  
  do {
    ShowStorageEventLog (SelectedLog, LocalScreen, &EventLogData[SelectedLog]);
    
    SetCurPos (gColumn, gTopRow);

    Key.ScanCode = SCAN_NULL;
    Key.UnicodeChar = CHAR_NULL;
    
    gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
    gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);  
    
    switch (Key.UnicodeChar) {

    case 'Q':
    case 'q':
      return TRUE;
   
    default:
      break;
      
    case 0:
      switch (Key.ScanCode) {
        
      case SCAN_ESC:
        if (EventCount > gMaxItemPerPage) {
          Index = EventCount - gMaxItemPerPage;
          *TopOfScreen = SelectedLog;
          *PagSelected = 0;
          if (*TopOfScreen > Index) {
            *PagSelected = *TopOfScreen - Index;
            *TopOfScreen = Index;
          }
        } else {
          *TopOfScreen = 0;
          *PagSelected = SelectedLog%gMaxItemPerPage;
        }
        
        ClearLines (
          gColumn,
          LocalScreen.RightColumn - 2,
          gTopRow,
          gBottomRow,
          VE_NORMAL_MENU_TEXT); 
        return FALSE;
        
      case SCAN_UP:
        if (SelectedLog == 0) {
          SelectedLog = EventCount - 1;
        } else {
          SelectedLog--;
        }
        break;
        
      case SCAN_DOWN:
        if (SelectedLog == EventCount- 1) {
          SelectedLog = 0;
        } else {
          SelectedLog++;
        }
        break;

      case SCAN_F2:  // sava this event as a file to device stoarge
        if (DisplayPopupMessage (L"Save all event logs to file?", VeDlgYesNo, LocalScreen) == 1) {    
          DisplayPopupMessage (L"Saving......", VeDlgNoOperation, LocalScreen);          
          Status = SaveEventToFile (StorageName, EventCount, EventLogData, TRUE);          
        } else {
          if (DisplayPopupMessage (L"Only save this event log to file?", VeDlgYesNo, LocalScreen) == 1) {    
            DisplayPopupMessage (L"Saving......", VeDlgNoOperation, LocalScreen);
            Status = SaveEventToFile (StorageName, SelectedLog, EventLogData, FALSE);
          } else {
            break;
          }
        }
        if (!EFI_ERROR (Status)) {      
          DisplayPopupMessage (L"Event log was saved", VeDlgOk, LocalScreen);
        } else {
          //
          // If returned status is not EFI_SUCCESS, show the error messsage.
          //
          DisplayPopupMessage (L"WARNING!!! Please try again later.", VeDlgOk, LocalScreen);
        }
        break;
        
      default:
        break;
      }
    }
    
  }while(TRUE);

}

EFI_STATUS
EFIAPI
ClearStorageEventLog (
  IN  UINTN                             StorageId,
  IN  EFI_SCREEN_DESCRIPTOR             LocalScreen
  )
/*++

Routine Description:

  Clear all event log data of selected storage.

Arguments:

  StorageId       Index ID for specific storage.
  LocalScreen     Local screen descriptor.
  
Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                            Status;
  CHAR16                                *ClearDialogStr1;
  CHAR16                                *ClearDialogStr2;

  Status = EFI_SUCCESS;
  
  if (gEventHandler == NULL) {
    DisplayPopupMessage (L"EfiEventHandlerProtocol not found!!", VeDlgOk, LocalScreen);
    return EFI_UNSUPPORTED;
  }

  
  StrIndexedCat (L"Clear all  Event Logs?", 10, MenuList[StorageId].StorageString, &ClearDialogStr1);
  if (DisplayPopupMessage (ClearDialogStr1, VeDlgYesNo, LocalScreen) == 1) {    
    DisplayPopupMessage (L"Loading......", VeDlgNoOperation, LocalScreen);
    Status = gEventHandler->ClearStorage (MenuList[StorageId].StorageString);
    if (!EFI_ERROR (Status)) {      
      StrIndexedCat (L" event logs were cleared!!", 0, MenuList[StorageId].StorageString, &ClearDialogStr2);
      DisplayPopupMessage (ClearDialogStr2, VeDlgOk, LocalScreen);
      gBS->FreePool (ClearDialogStr2);
    } else {
      //
      // If returned status is not EFI_SUCCESS, show the error messsage.
      //
      DisplayPopupMessage (L"WARNING!!! Please try again later.", VeDlgOk, LocalScreen);
    }
  }

  gBS->FreePool (ClearDialogStr1);
  
  return Status;
}

EFI_STATUS
EFIAPI
ShowStorageEventLog (
  IN UINTN                                 EventCount,
  IN EFI_SCREEN_DESCRIPTOR                 LocalScreen,
  IN STORAGE_EVENT_LOG_INFO                *EventLog  
  )
/*++

Routine Description:

  Show content of the selected event data.

Arguments:

  EventCount        Total event count of the selected storage 
  LocalScreen       Index ID for specific storage.
  EventLog          Information of the selected event data.
  
Returns:

  EFI_STATUS

--*/
{
  BOOLEAN                                HaveEvent;
  UINTN                                  Index, Index2;
  UINTN                                  Column; 
  UINTN                                  Row;
  UINTN                                  PreColumn, PreRow;
  UINTN                                  LineCount;
  UINTN                                  StringSize;  
  UINTN                                  StringWidth;  
  UINT8                                  EventType;
  CHAR16                                 *EventString;
  CHAR16                                 *FormattedString;
  
  HaveEvent = FALSE;
  LineCount = 0;
  StringSize = 0;  
  StringWidth = 0;  
  EventType = 0xFF;
  EventString = NULL;
  FormattedString = NULL;

  
  Row = 0;

  //
  //--------------------------------------------------------------------------------
  // Entry Num       : XXXX
  // Event Type ID   : 0xXX
  // Time Stamp      : year/month/day Hour:Minute:Second
  // Event Data      : 0xXX 0xXX 0xXX 0xXX
  // Event Raw Data  : 0xXX 0xXX 0xXX 0xXX
  // Event String    : String
  //--------------------------------------------------------------------------------
  //  
  SetColor (VE_ITEM_TEXT);
  SetCurPos (gColumn, gTopRow + Row);
  Print (L"Entry Num        : ");
  SetColor (VE_ITEM_DATA);
  Print (L"%04d", EventCount + 1);
  Row++;

  if (EventLog->EventTypeIdEnable) {
    SetColor (VE_ITEM_TEXT);
    SetCurPos (gColumn, gTopRow + Row);
    Print (L"Event Type ID    : ");
    SetColor (VE_ITEM_DATA);
    Print (L"0x%02x", EventLog->EventTypeID);
    Row++;
  }

  SetColor (VE_ITEM_TEXT);
  SetCurPos (gColumn, gTopRow + Row);
  Print (L"Time Stamp       : ");
  SetColor (VE_ITEM_DATA);
  Print (L"%d/%02d/%02d %02d:%02d:%02d", 
         EventLog->Year, 
         EventLog->Month, 
         EventLog->Date, 
         EventLog->Hour, 
         EventLog->Minute, 
         EventLog->Second);
  Row++;

  if (EventLog->GeneratorIdEnable) {    
    SetColor (VE_ITEM_TEXT);
    SetCurPos (gColumn, gTopRow + Row);
    Print (L"Generator ID     : ");    
    SetColor (VE_ITEM_DATA);
    Print (L"0x%04x", EventLog->GeneratorId); 
    Row++;
  }

  if (EventLog->EventIdEnable) {
    //
    // For storage driver that don't want to show Sensor Type, Sensor Num, Event Type.
    // Just want to show Event ID.
    // Like BIOS =>  Event ID     : 0xXX 0xXX 0xXX.
    //
    SetColor (VE_ITEM_TEXT);
    SetCurPos (gColumn, gTopRow + Row);
    Print (L"Event ID         : ");
    SetColor (VE_ITEM_DATA);
    if ((EventLog->Data == NULL) && ((EventLog->EventID.SensorType == 0xFF) && (EventLog->EventID.SensorNum == 0xFF) && (EventLog->EventID.EventType == 0xFF))) {
      Print (L"NULL                ");
    } else {
      Print (L"0x%02x ", EventLog->EventID.SensorType);
      Print (L"0x%02x ", EventLog->EventID.SensorNum);
      Print (L"0x%02x ", EventLog->EventID.EventType);
    }
    Row++;
  } else {
    //
    // For storage driver that want to show Sensor Type, Sensor Num, Event Type.
    // Like BMC SEL =>  Sensor Type     : 0xXX
    //                  Sensor Num     : 0xXX
    //                  Event  Type     : 0xXX
    //
    SetColor (VE_ITEM_TEXT);
    SetCurPos (gColumn, gTopRow + Row);
    Print (L"Sensor Type      : ");
    SetColor (VE_ITEM_DATA);
    Print (L"0x%02x", EventLog->EventID.SensorType);
    Row++;

    SetColor (VE_ITEM_TEXT);
    SetCurPos (gColumn, gTopRow + Row);
    Print (L"Sensor Num       : ");    
    SetColor (VE_ITEM_DATA);
    Print (L"0x%02x", EventLog->EventID.SensorNum);  
    Row++;  

    SetColor (VE_ITEM_TEXT);
    SetCurPos (gColumn, gTopRow + Row);
    Print (L"Event Type       : ");    
    SetColor (VE_ITEM_DATA);
    Print (L"0x%02x", EventLog->EventID.EventType);    
    Row++;

  }

  SetColor (VE_ITEM_TEXT);
  SetCurPos (gColumn, gTopRow + Row);
  Print (L"Event Data       : ");
  SetColor (VE_ITEM_DATA);
  if (EventLog->Data != NULL) {
    Print (L"0x%02x 0x%02x 0x%02x", 
    EventLog->Data[0], 
    EventLog->Data[1], 
    EventLog->Data[2]);
  } else {
    Print (L"NULL                ");
  }
  Row++;
  
  ClearLines (
    gColumn, 
    LocalScreen.RightColumn - 1, 
    gTopRow + Row, 
    gBottomRow, 
    VE_ITEM_DATA);
  SetColor (VE_ITEM_TEXT);
  SetCurPos (gColumn, gTopRow + Row);
  Print (L"Event Raw Data   : ");
  Column = gST->ConOut->Mode->CursorColumn;
  SetColor (VE_ITEM_DATA);
  Index2 = 0;
  for (Index=0; Index < (EventLog->EventRawDataSize); Index++) {   
    Print (L"0x%02x ", EventLog->EventRawData[Index]);
    HaveEvent=TRUE;
    
    Index2++;
    if (Index2 == 8) {
      Row++;
      Index2 = 0;
      SetCurPos (Column, gTopRow + Row);
    }
  }
  if (Index2 != 0) {
    Row++;
  }

  if (!HaveEvent) {
    Print (L"NULL");
  }

  SetColor (VE_ITEM_TEXT);
  SetCurPos (gColumn, gTopRow + Row);
  Print (L"Event String     : ");
  PreColumn = gST->ConOut->Mode->CursorColumn;
  PreRow = gST->ConOut->Mode->CursorRow;
  StringWidth = LocalScreen.RightColumn - PreColumn - 3;
  ClearLines (
    PreColumn, 
    LocalScreen.RightColumn - 1, 
    PreRow, 
    gBottomRow, 
    VE_ITEM_DATA);

  if (!GetEventString ((VOID *)EventLog, &EventString, &StringSize)) {
    if (EventString != NULL) {
      ProcessString (EventString, &FormattedString, StringWidth, 10, &LineCount);
      gBS->FreePool (EventString);
    }
    
    for (Index = 0; Index < LineCount; Index++) {
      SetCurPos (PreColumn, PreRow + Index);
      Print (&FormattedString[Index * StringWidth * 2]);
    }
    
    if (FormattedString) {
      gBS->FreePool (FormattedString);
    }    
  } else {
    Print (L"Undefined");
  }
  Row++;
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveEventToFile (
  IN CHAR16                                *StorageName,
  IN UINTN                                 EventCount,
  IN STORAGE_EVENT_LOG_INFO                *EventLog,
  IN BOOLEAN                               SaveAllEvents
  )
/*++

Routine Description:

 Save event as a file to device storage

Arguments:

  EventCount        Total event count of the selected storage 
  EventLog          Information of the selected event data.
  SaveAllEvents     Sava all or single event(s)
  
Returns:

  EFI_STATUS

--*/
{
  UINTN                                 Index;  
  UINTN                                 Index2;
  CHAR16                                *EventString=NULL;
  UINTN                                 StringSize=0;  
  UINTN                                 TotalStringSize=0;    
  VOID                                  *FinalString=NULL;
  VOID                                  *TempString=NULL;
  UINTN                                 TempStringSize=0;
  BOOLEAN                               HaveEvent=FALSE;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL       *Volume=NULL;
  CHAR16                                *FileName=NULL;
  CHAR8                                  *AsciiString=NULL;
  BOOLEAN                               SaveEvent=FALSE;

  //
  // 1. Get removable or non-removable device for saving event(s)
  //
  
  // Firstly, search removable device
  Volume = SearchFileSystem (TRUE);
  if (Volume == NULL) {
    // Search non-removable device
    Volume = SearchFileSystem (FALSE);
  }
  
  if (Volume == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // 2. Create a file name. ex: 20091111171800BIOS.log
  //
  ObtainSavingFileNameByTime (StorageName, &FileName);

  //
  // 3. Arrange data format as below.
  //
  //--------------------------------------------------------------------------------
  // Entry Num       : XXXX
  // Event Type ID   : 0xXX
  // Time Stamp      : year/month/day Hour:Minute:Second
  // Event Data      : 0xXX 0xXX 0xXX 0xXX
  // Event Raw Data  : 0xXX 0xXX 0xXX 0xXX
  // Event String    : String
  //--------------------------------------------------------------------------------
  //  
  if (SaveAllEvents==FALSE) {
    //
    // save current viewed event
    //
    Index=EventCount;
  } else {
    //
    // save all logged event
    //
    Index=0;
    EventCount -=1;
  }

  //
  // Reserve 32M bytes to collect events
  //
  gBS->AllocatePool (EfiBootServicesData, 0x2000000, (VOID **)&FinalString);
  ZeroMem(FinalString, 0x2000000);
  for (; Index<=EventCount; Index++) {    
    TempStringSize += StrLen (L"Entry Num        : DDDD ")*2;
    gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
    UnicodeSPrint (
      TempString, 
      TempStringSize, 
      L"Entry Num        : %04d",
      Index + 1      
    );  
    StrCat(FinalString, TempString);
    gBS->FreePool(TempString);
    TotalStringSize += TempStringSize;    
    StrCat(FinalString, L"\n");
//    TotalStringSize += StrLen (L" \n")*2;

    if (EventLog[Index].EventTypeIdEnable) {
      TempStringSize = StrLen (L"Event Type ID    : 0xXX ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"Event Type ID    : 0x%02x",
        EventLog[Index].EventTypeID     
      );  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);
      TotalStringSize += TempStringSize;
      StrCat(FinalString, L"\n");
//      TotalStringSize += StrLen (L" \n")*2;
    }

    TempStringSize = StrLen (L"Time Stamp       : DDDD/DD/DD DD:DD:DD ")*2;
    gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
    UnicodeSPrint (
      TempString, 
      TempStringSize, 
      L"Time Stamp       : %04d/%02d/%02d %02d:%02d:%02d",
      EventLog[Index].Year,
      EventLog[Index].Month,
      EventLog[Index].Date,
      EventLog[Index].Hour,
      EventLog[Index].Minute,
      EventLog[Index].Second
    );  
    StrCat(FinalString, TempString);
    gBS->FreePool(TempString);
    TotalStringSize += TempStringSize;
    StrCat(FinalString, L"\n");
//    TotalStringSize += StrLen (L" \n")*2;

    if (EventLog[Index].GeneratorIdEnable) {    
      TempStringSize = StrLen (L"Generator ID     : 0xXXXX ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"Generator ID     : 0x%04x",
        EventLog[Index].GeneratorId
      );  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);
      TotalStringSize += TempStringSize;
      StrCat(FinalString, L"\n");
//      TotalStringSize += StrLen (L" \n")*2;
    }

    if (EventLog[Index].EventIdEnable) {
      //
      // For storage driver that don't want to show Sensor Type, Sensor Num, Event Type.
      // Just want to show Event ID.
      // Like BIOS =>  Event ID     : 0xXX 0xXX 0xXX.
      //
      TempStringSize = StrLen (L"Event ID         : ")*2;
      if ((EventLog[Index].Data == NULL) && ((EventLog[Index].EventID.SensorType == 0xFF) && (EventLog[Index].EventID.SensorNum == 0xFF) && (EventLog[Index].EventID.EventType == 0xFF))) {
        TempStringSize += StrLen (L"NULL \n")*2;
        StrCat(FinalString, L"Event ID         : ");        
        StrCat(FinalString, L"NULL \n");
      } else {
        TempStringSize += StrLen (L"0xXX 0xXX 0xXX ")*2;
        gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
        UnicodeSPrint (
          TempString, 
          TempStringSize, 
          L"Event ID         : 0x%02x 0x%02x 0x%02x",
          EventLog[Index].GeneratorId
        );  
        StrCat(FinalString, TempString);
        gBS->FreePool(TempString);
        StrCat(FinalString, L"\n");
//        TempStringSize += StrLen (L" \n")*2;
      }      
      TotalStringSize += TempStringSize;
    } else {
      //
      // For storage driver that want to show Sensor Type, Sensor Num, Event Type.
      // Like BMC SEL =>  Sensor Type     : 0xXX
      //                  Sensor Num      : 0xXX
      //                  Event  Type     : 0xXX
      //
      TempStringSize =  StrLen (L"Sensor Type      : 0xXX ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"Sensor Type      : 0x%02x",
        EventLog[Index].EventID.SensorType
      );  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);
      TotalStringSize += TempStringSize;
      StrCat(FinalString, L"\n");
//      TotalStringSize += StrLen (L" \n")*2;

      TempStringSize = StrLen (L"Sensor Num       : 0xXX ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"Sensor Num       : 0x%02x",
        EventLog[Index].EventID.SensorNum
      );  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);
      TotalStringSize += TempStringSize;
      StrCat(FinalString, L"\n");
//      TotalStringSize += StrLen (L" \n")*2;

      TempStringSize = StrLen (L"Event  Type      : 0xXX ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"Event  Type      : 0x%02x",
        EventLog[Index].EventID.EventType
      );  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);
      TotalStringSize += TempStringSize;
      StrCat(FinalString, L"\n");
//      TotalStringSize += StrLen (L" \n")*2;
    }

    TempStringSize = StrLen (L"Event Data       : ")*2;
    if (EventLog[Index].Data != NULL) {
      TempStringSize += StrLen (L"0xXX 0xXX 0xXX ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"Event Data       : 0x%02x 0x%02x 0x%02x",
        EventLog[Index].Data[0],
        EventLog[Index].Data[1],
        EventLog[Index].Data[2]);  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);
      StrCat(FinalString, L"\n");
//      TempStringSize += StrLen (L" \n")*2;
    } else {
      TempStringSize += StrLen (L"NULL                \n")*2;
      StrCat(FinalString, L"Event Data       : ");        
      StrCat(FinalString, L"NULL                \n");        
    }    
    TotalStringSize += TempStringSize;
    
    TempStringSize = StrLen (L"Event Raw Data   : ")*2;
    StrCat(FinalString, L"Event Raw Data   : ");      
    TotalStringSize += TempStringSize;
    for (Index2=0; Index2 < (EventLog[Index].EventRawDataSize); Index2++) {   
      TempStringSize = StrLen (L"0xXX  ")*2;
      gBS->AllocatePool (EfiBootServicesData, TempStringSize, (VOID **)&TempString);
      UnicodeSPrint (
        TempString, 
        TempStringSize, 
        L"0x%02x ",
        EventLog[Index].EventRawData[Index2]);  
      StrCat(FinalString, TempString);
      gBS->FreePool(TempString);      
      TotalStringSize += (TempStringSize);
      HaveEvent=TRUE;
      if (!HaveEvent) {
        TempStringSize = StrLen (L"NULL")*2;
        StrCat(FinalString, L"NULL");      
        TotalStringSize += TempStringSize;
      }
    }
//    TempStringSize = StrLen (L"\n")*2;
    StrCat(FinalString, L"\n");      
    TotalStringSize += TempStringSize;

    TempStringSize = StrLen (L"Event String     : ")*2;    
    StrCat(FinalString, L"Event String     : ");
    if (!GetEventString ((VOID *)&EventLog[Index], &EventString, &StringSize)) {  
      TempStringSize += StringSize;
      StrCat(FinalString, EventString);
      gBS->FreePool(EventString);
    } else {
      TempStringSize += StrLen (L"Undefined")*2;
      StrCat(FinalString, L"Undefined");
    }
    TotalStringSize += TempStringSize;
    
    TempStringSize = StrLen (L"\n\n")*2;
    StrCat(FinalString, L"\n\n");
    TotalStringSize += TempStringSize;
    
    SaveEvent = TRUE;
  }

  if (SaveEvent) {
    
    //
    // 4. Transfer format from unicode to ascii and save event to file.
    //
    gBS->AllocatePool (EfiBootServicesData, StrLen(FinalString), (VOID **)&AsciiString);
    UnicodeStrToAsciiStr (FinalString, AsciiString);
    CreateSavingFile (Volume, FileName, AsciiString, StrLen(FinalString)+1);

    gBS->FreePool(FinalString);
    gBS->FreePool(AsciiString);
  }
  return EFI_SUCCESS;
}

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *
EFIAPI
SearchFileSystem (
  BOOLEAN Removable
  )
/*++

  Routine Description:
    Search first "Simple File System" protocol

  Arguments:
    Removable        - Is device of the "Simple File System" removable

  Returns:
    Return find out "Simple File System" protocol, Return NULL if not found

--*/
{
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *Volume;
  EFI_STATUS                          Status;
  EFI_HANDLE                         *HandleBuffer;
  UINTN                               NumberOfHandles;
  UINTN                               Index;
  EFI_BLOCK_IO_PROTOCOL              *BlkIo;


  Volume       = NULL;
  HandleBuffer = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {

    BlkIo  = NULL;
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiBlockIoProtocolGuid, (VOID **)&BlkIo);

    if (BlkIo != NULL) {
      if (BlkIo->Media->ReadOnly) {
        continue;
      }
      if (Removable && !BlkIo->Media->RemovableMedia) {
        continue;
      }
    }
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&Volume);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
  }

  return Volume;
}


EFI_STATUS
EFIAPI
CreateSavingFile (
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *Volume,  
  IN CHAR16                             *FileName,
  IN VOID                               *SavingDataBuffer,
  IN UINTN                              SavingDataSize
  )
{
  EFI_FILE                                *Root;
  EFI_FILE                                *File;
  EFI_STATUS                              Status;

  File = NULL;

  Status = Volume->OpenVolume (
                     Volume,
                     &Root
                     );
  if (EFI_ERROR (Status)) {
    return EFI_NO_MEDIA;
  }

  //
  // Confirm the FileName is existence or not.
  //
  Status = Root->Open (
                   Root,
                   &File,
                   FileName,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                   0
                   );
  if (EFI_ERROR (Status)) {
    //
    // No the same FileName be found. Create it and write event to it.
    //
    Status = Root->Open (
                 Root,
                 &File,
                 FileName,
                 EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                 0
                 );
    if (EFI_ERROR (Status)) {      
      Root->Close (Root);
      return EFI_NO_MEDIA;
    }
  }  
  //
  // The same FileName is existence. Overwrite event to the same file
  //
  Status = Root->Write (File, &SavingDataSize, SavingDataBuffer);
  Status = Root->Open (
                   Root,
                   &File,
                   FileName,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                   0
                   );
  Root->Close (Root);

  return Status;
}

EFI_STATUS
EFIAPI
ObtainSavingFileNameByTime (
  IN CHAR16                                 *StorageName,
  IN OUT CHAR16                             **FileName
  )
{
  EFI_STATUS                           Status;
  EFI_TIME                             EfiTime;
  CHAR16                               Buffer[5];
  UINTN                                FileNameSize=0;
  VOID                                 *TempString=NULL;

  //
  // File name will be defined as "YYYYMMDDHHMMSS"+"storage name"+".log"
  // ex: file name = 20091111171800BIOS.log
  //
  FileNameSize = StrLen(StorageName);
  FileNameSize += StrLen(L"20091111171800.log");

  Status = gBS->AllocatePool (EfiBootServicesData, FileNameSize, (VOID **)&TempString);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Status = gRT->GetTime (&EfiTime, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ValueToString (EfiTime.Year, 4, Buffer, VTS_LEAD_0);
  StrCpy (TempString, Buffer);
  
  ValueToString (EfiTime.Month, 2, Buffer, VTS_LEAD_0);
  StrCat (TempString, Buffer);

  ValueToString (EfiTime.Day, 2, Buffer, VTS_LEAD_0);
  StrCat (TempString, Buffer);

  ValueToString (EfiTime.Hour, 2, Buffer, VTS_LEAD_0);
  StrCat (TempString, Buffer);

  ValueToString (EfiTime.Minute, 2, Buffer, VTS_LEAD_0);
  StrCat (TempString, Buffer);

  ValueToString (EfiTime.Second, 2, Buffer, VTS_LEAD_0);
  StrCat (TempString, Buffer);

  StrCat (TempString, StorageName);

  StrCat (TempString, L".log");

  *FileName = TempString;

  return EFI_SUCCESS;
}

VOID
EFIAPI
ValueToString (
  UINTN     Value,
  UINTN     Digitals,
  CHAR16    *Buffer,
  UINTN     Flags
  )
{
  CHAR8   Str[30];
  UINTN   mod;
  CHAR8   *p1;
  CHAR16  *p2;
  CHAR16  StuffChar = 0;
  UINTN   count = 0;
  UINTN   Padding;

  p1  = Str;
  p2  = Buffer;

  if (Flags == VTS_RIGHT_ALIGN)
    StuffChar = L' ';

  if (Flags == VTS_LEAD_0)
    StuffChar = L'0';

  if (Value == 0) {

    // Stuff specify char to buffer
    if (StuffChar != 0) {
      Padding = Digitals - 1;
      
      while (Padding > 0) {
        *p2 = StuffChar;
        p2++;
        Padding--;      
      }
    }
    
    *p2++ = '0';
    *p2 = 0;
    return ;
  }
  
  while (Value) {
    mod = Value % 10;
    *p1 = (CHAR8)(mod + '0');
    Value = Value / 10;
    p1++;
    count++;
  }

  if ((count < Digitals) && (StuffChar != 0)) {
    Padding = Digitals - count;
    while (Padding) {
      *p2 = StuffChar;
      p2++;
      Padding--;
    }
  }

  while (count > 0) {
    p1--;
    *p2 = *p1;
    p2++;

    count--;
  }

  *p2 = 0;
}

