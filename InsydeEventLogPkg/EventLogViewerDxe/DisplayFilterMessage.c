/** @file

  Event Log Viewer DXE implementation.

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

#include <DisplayFilterMessage.h>
#include <EventLogViewerSetting.h>
#include <EventLogViewerCommonFunc.h>

extern STORAGE_EVENT_LOG_INFO                           gFilteredEventLogData;

INT32                                                   SelectItemSize = 1;

EFI_STATUS
EFIAPI
WaitForSingleEvent (
  EFI_EVENT  Event,
  UINT64     Timeout OPTIONAL
  )
{
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_EVENT   TimerEvent;
  EFI_EVENT   WaitList[2];

  if (Timeout != 0) {
    //
    // Create a timer event
    //
    Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    if (!EFI_ERROR (Status)) {
      //
      // Set the timer event
      //
      gBS->SetTimer (
             TimerEvent,
             TimerRelative,
             Timeout
             );

      //
      // Wait for the original event or the timer
      //
      WaitList[0] = Event;
      WaitList[1] = TimerEvent;
      Status      = gBS->WaitForEvent (2, WaitList, &Index);
      gBS->CloseEvent (TimerEvent);

      //
      // If the timer expired, change the return to timed out
      //
      if (!EFI_ERROR (Status) && Index == 1) {
        Status = EFI_TIMEOUT;
      }
    }
  } else {
    //
    // No timeout... just wait on the event
    //
    Status = gBS->WaitForEvent (1, &Event, &Index);
  }

  return Status;
}


EFI_INPUT_KEY 
EFIAPI
WaitForAKey (
  IN UINT64                                             Timeout OPTIONAL
  )
{
  EFI_INPUT_KEY                              Key;
  
  WaitForSingleEvent (gST->ConIn->WaitForKey, Timeout);
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

  return Key;
}


EFI_STATUS
EFIAPI
CheckDecember (
  IN      CHAR16                                        *String,
  OUT     UINT32                                        *Number
  ) 
{
  CHAR16                                                *Temp = String;
  
  // skip preceeding white space
  while (*Temp && *Temp == L' ') {
    Temp += 1;
  }

  if (StrLen (Temp) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  // skip preceeding zeros
  while (*Temp && *Temp == L'0') {
    Temp += 1;
  }
  
  *Number = 0;
  while (*Temp) {
    if ( (*Temp >= L'0'  &&  *Temp <= L'9')) {
      *Number = *Number * 10 + *Temp -  L'0';
    } else {
      return EFI_INVALID_PARAMETER;
    }
    Temp += 1;
  }
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
CheckHex (
  IN      CHAR16                                        *String,
  OUT     UINT32                                        *Number
  ) 
{
  CHAR16                                                *Temp = String;
  CHAR16                                                Char16 = 0;
  
  // skip preceeding white space
  while (*Temp && *Temp == L' ') {
    Temp += 1;
  }

  if (StrLen (Temp) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  // skip preceeding zeros
  while (*Temp && *Temp == L'0') {
    Temp += 1;
  }
  
  *Number = 0;
  Char16 = * (String++);
  while (Char16) {
    if (Char16 >= L'a'  &&  Char16 <= L'f') {
      Char16 -= L'a' - L'A';
    }

    if ( (Char16 >= L'0'  &&  Char16 <= L'9')  || (Char16 >= L'A'  &&  Char16 <= L'F')) {
      *Number = *Number << 4  |  Char16 - (Char16 >= L'A' ? L'A' - 10 : L'0');
    } else {
      return EFI_INVALID_PARAMETER;
    }
   
    Char16 = * (String++);
  }
  return EFI_SUCCESS;
}


VOID
EFIAPI
RelItemList (
  IN      CHAR16                                        **ItemList,
  IN      UINT32                                        ItemCount
) 
{
  UINT32                                                Index = 0;

  if (ItemList == NULL) {
    return;
  }

  for (Index = 0; Index < ItemCount; Index++) {
    if (ItemList[Index] != NULL) {
      FreePool (ItemList[Index]);
      ItemList[Index] = NULL;
    }
  }

  FreePool (ItemList);
}


VOID
EFIAPI
PrintResult (
  IN      FILTER_EVENT_INFO                             *Data
  ) 
{
  switch (Data->FilterType) {

  case TypeDate:
    Print (L"Date: %d/%d/%d.\n", Data->Year, Data->Month, Data->Date);
    break;

  case TypeTime:
    Print (L"Time: %d:%d:%d.\n", Data->Hour, Data->Minute, Data->Second);
    break;

  case TypeGeneratorID:
    Print (L"GeneratorId: 0x%x.\n", Data->GeneratorId);
    break;

  case TypeEventID:
    Print (L"EventID: 0x%x 0x%x 0x%x.\n", Data->EventID.SensorType, Data->EventID.SensorNum, Data->EventID.EventType);
    break;

  case TypeSensorType:
    Print (L"SensorType: 0x%x.\n", Data->EventID.SensorType);
    break;

  case TypeSensorNum:
    Print (L"SensorNum: 0x%x.\n", Data->EventID.SensorNum);
    break;

  case TypeEventType:
    Print (L"Event Type: 0x%x.\n", Data->EventID.EventType);
    break;

  case TypeEventTypeID:
    Print (L"Event Type: 0x%x.\n", Data->EventTypeID);
    break;    

  default:
    break;
  }
  WaitForAKey (0);
}


VOID 
EFIAPI
DisplayString (
  IN      INT32                                         x,
  IN      INT32                                         y,
  IN      CHAR16                                        *String,
  IN      INT32                                         Len,
  IN      INT32                                         Color
  )
{
  static CHAR16 Text[WINDOW_LIMIT_WIDTH << 1];
	
//	  if (Len <= 0 || x < 1 || y < 1 || x > WINDOW_LIMIT_WIDTH || y > WINDOW_LIMIT_HIGH || String == NULL) {
//	    return;
//	  }
//		
//	  if (Len > WINDOW_LIMIT_WIDTH || (x+Len-1) > WINDOW_LIMIT_WIDTH) {
//	    Len = WINDOW_LIMIT_WIDTH - x + 1;
//	  }
	
  gBS->SetMem (Text, sizeof (Text), 0);
  CopyMem (Text, String, Len << 1);
    
  gST->ConOut->SetAttribute (gST->ConOut, Color);
	
  //
  // Set to narrow (not wide) characters.  Set attributes and strip it and print it
  //
  gST->ConOut->Mode->Attribute = gST->ConOut->Mode->Attribute & 0x7f;
  gST->ConOut->SetAttribute (gST->ConOut, gST->ConOut->Mode->Attribute);  
	
  gST->ConOut->SetCursorPosition (gST->ConOut, x-1, y-1);
  gST->ConOut->OutputString (gST->ConOut, Text);
}


VOID 
EFIAPI
UpdateSelectFrame (
  IN      SELECT_ITEM_INFO                              *ItemList,
  IN      UINT32                                        StartNo,
  IN      UINT32                                        ItemCount,
  IN      EFI_SCREEN_DESCRIPTOR                         FrameScreen
  ) 
{
  CHAR16                                                StrBuf[80] = {0};
  UINT32                                                NoIndex = 0;
  UINT32                                                Count = 0;

  ClearLines (FrameScreen.LeftColumn, FrameScreen.RightColumn, FrameScreen.TopRow - 1, FrameScreen.BottomRow - 1, VE_NORMAL_POPUP_TEXT);
  
  if ((ItemList == NULL) || (ItemCount == 0)) {
    //
    // clean file frame
    //
    return;
  }

  for (NoIndex = StartNo, Count = 0; (NoIndex < ItemCount) && (Count < (UINT32)SelectItemSize); NoIndex++, Count++) {

//	    if (ItemList[NoIndex] == NULL) {
//	      continue;
//	    }
    UnicodeSPrint (StrBuf, 80, L"%s", ItemList[NoIndex].String);
    DisplayString ((INT32)FrameScreen.LeftColumn, (INT32)FrameScreen.TopRow + Count, StrBuf, (INT32)(StrLen(StrBuf)), FILTER_UNSELECT_ITEM);
    
  }
}


VOID
EFIAPI
UpdateScrollBar (
  IN      INT32                                         NumberItems,
  IN      INT32                                         PageBaseOffset,
  IN      INT32                                         PageSize,
  IN      EFI_SCREEN_DESCRIPTOR                         FrameScreen
  ) 
{
  INT32                                                 NumberPages = 0;
  INT32                                                 Row = 0;
  UINTN                                                 Index = 0;  
  INT8                                                  BarIndex = 0;
  INT8                                                  BarSize = 0;
  CHAR16                                                StrBuf[80] = {0};
  
  NumberPages = (NumberItems / PageSize);
  if ((NumberItems % PageSize) != 0) {
    NumberPages++;
  }

//	  if (NumberPages <= 1) {
//	    //
//	    // 2.Draw two side
//	    //
//	
//	    //
//	    // 2.Draw two side
//	    //
//	    for (Index = (FrameScreen.TopRow); Index <= FrameScreen.BottomRow; Index++) {
//	      UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_VERTICAL);
//	      DisplayString ((INT32)(FrameScreen.RightColumn + 1), (INT32)Index, StrBuf, (INT32)1, VE_NORMAL_POPUP_TEXT);
//	    }
//	    DisplayString ((INT32)(FrameScreen.RightColumn), (INT32)(FrameScreen.TopRow - 1), L"    ", (INT32)4, VE_NORMAL_POPUP_TEXT);
//	    DisplayString ((INT32)(FrameScreen.RightColumn), (INT32)(FrameScreen.BottomRow + 1), L"    ", (INT32)4, VE_NORMAL_POPUP_TEXT);
//	    UnicodeSPrint (StrBuf, 80, L"%c%c", BOXDRAW_HORIZONTAL ,BOXDRAW_DOWN_LEFT);
//	    DisplayString ((INT32)(FrameScreen.RightColumn), (INT32)(FrameScreen.TopRow - 1), StrBuf, (INT32)2, VE_NORMAL_POPUP_TEXT);
//	    UnicodeSPrint (StrBuf, 80, L"%c%c",BOXDRAW_HORIZONTAL ,BOXDRAW_UP_LEFT);
//	    DisplayString ((INT32)(FrameScreen.RightColumn), (INT32)(FrameScreen.BottomRow + 1), StrBuf, (INT32)2, VE_NORMAL_POPUP_TEXT);
//	
//	    return;
//	  }

  //
  // calculate BarSize
  //
  BarSize = (INT8)(PageSize - ((NumberItems - PageSize + 1) * PageSize) / NumberItems);
  if ((PageBaseOffset + 1) == (NumberItems - PageSize + 1)) {
    Row = PageSize - BarSize;
  } else {
    Row = (PageBaseOffset * PageSize)/ NumberItems;
  }

  
  //
  // 2.Draw right side
  //
  DisplayString ((INT32)(FrameScreen.RightColumn), (INT32)(FrameScreen.TopRow - 1), L" ¡ô ", (INT32)(StrLen(L" ¡ô ")), FILTER_SCROLL_BAR_COLOR);
  DisplayString ((INT32)(FrameScreen.RightColumn), (INT32)(FrameScreen.BottomRow + 1), L" ¡õ ", (INT32)(StrLen(L" ¡õ ")), FILTER_SCROLL_BAR_COLOR);
  for (Index = FrameScreen.TopRow; Index <= FrameScreen.BottomRow;) {
    if ((UINTN)Row == (Index - FrameScreen.TopRow)) {
      for (BarIndex = 0; BarIndex < BarSize; BarIndex++) {
        DisplayString ((INT32)(FrameScreen.RightColumn + 1), (INT32)Index, L" ", (INT32)1, FILTER_SCROLL_BAR_COLOR);
        Index++;
      }
    } else {
      UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_VERTICAL);
      DisplayString ((INT32)(FrameScreen.RightColumn + 1), (INT32)Index, StrBuf, (INT32)1, VE_NORMAL_POPUP_TEXT);
      Index++;
    }
    
  }

}


VOID 
EFIAPI
PrintBorderLine (
  IN      EFI_SCREEN_DESCRIPTOR                         MessageScreen,
  IN      INT32                                         Color
  ) 
{
  UINTN                                                 Index = 0;
  CHAR16                                                StrBuf[80] = {0};
  
  //
  //Next three step will draw a dialog
  //1.draw two row line
  //2.draw two side
  //3.draw four corner
  //

  //
  // 1.This is draw two row line
  //
  for (Index = (MessageScreen.LeftColumn + 1); Index <= (MessageScreen.RightColumn - 1); Index++) {
    UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_HORIZONTAL);
    DisplayString ((INT32)Index, (INT32)MessageScreen.TopRow, StrBuf, (INT32)1, Color);
    DisplayString ((INT32)Index, (INT32)MessageScreen.TopRow + 2, StrBuf, (INT32)1, Color);
    DisplayString ((INT32)Index, (INT32)MessageScreen.BottomRow, StrBuf, (INT32)1, Color);
  }
	
  //
  // 2.Draw two side
  //
  for (Index = (MessageScreen.TopRow + 1); Index <= (MessageScreen.BottomRow - 1); Index++) {
    UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_VERTICAL);
    DisplayString ((INT32)MessageScreen.LeftColumn, (INT32)Index, StrBuf, (INT32)1, Color);
    DisplayString ((INT32)MessageScreen.RightColumn, (INT32)Index, StrBuf, (INT32)1, Color);
  }
	
  //
  // 3.This is draw the dialog four corner
  //
  UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_DOWN_RIGHT);
  DisplayString ((INT32)MessageScreen.LeftColumn, (INT32)MessageScreen.TopRow, StrBuf, (INT32)1, Color);
  UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_UP_RIGHT);
  DisplayString ((INT32)MessageScreen.LeftColumn, (INT32)MessageScreen.BottomRow, StrBuf, (INT32)1, Color);
  UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_DOWN_LEFT);
  DisplayString ((INT32)MessageScreen.RightColumn, (INT32)MessageScreen.TopRow, StrBuf, (INT32)1, Color);
  UnicodeSPrint (StrBuf, 80, L"%c", BOXDRAW_UP_LEFT);
  DisplayString ((INT32)MessageScreen.RightColumn, (INT32)MessageScreen.BottomRow, StrBuf, (INT32)1, Color);
}


EFI_STATUS
EFIAPI
InputKeyWord (
  IN      UINT32                                        StringLen,
  IN OUT  CHAR16                                        *KeyWord
  ) 
{
  EFI_INPUT_KEY                                         Key = {0};
  UINT8                                                 Index = 0;
  
  SetColor (INPUT_BUFFER_COLOR);
  
  while (TRUE) {
    
    //
    //wait for inputting a key
    //
    Key = WaitForAKey (0);

    if (Key.UnicodeChar == CHAR_BACKSPACE) {
      if (Index != 0){
        Print (L"\b \b");
        KeyWord[Index - 1] = '\0';
        Index--;
      }
      
    } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
//	      if (StrLen (KeyWord) == 0) {
//	        return EFI_ABORTED;
//	      }
      KeyWord [Index] = '\0';
      return EFI_SUCCESS;
     
    } else if (Key.ScanCode == SCAN_ESC) {
      KeyWord [Index] = '\0';
      return EFI_ABORTED;
      
    } else if ((Index < StringLen) && (Key.UnicodeChar != 0)) {
          
	    Print (L"%c", Key.UnicodeChar);
      KeyWord[Index] = Key.UnicodeChar;
      Index++;
      
    }

  }
 
}


EFI_STATUS 
EFIAPI
TransferStringToDate (
  IN      CHAR16                                        *String,
  OUT     FILTER_EVENT_INFO                             *Data
  ) 
{
  EFI_STATUS                                            Status = EFI_SUCCESS;
  CHAR16                                                *TempStr = NULL;
  UINT32                                                Index = 0;
  UINT32                                                NumberIndex = 0;
  UINT32                                                ArgIndex = 0;
  UINT32                                                Number = 0;
  
  if ((String == NULL) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempStr = AllocateZeroPool ((StrLen (String) + 2) * sizeof (CHAR16));
  if (TempStr == NULL) {
    return EFI_UNSUPPORTED;
  }
  StrCpy (TempStr, String);

  TempStr [StrLen(TempStr)] = L'/';
  TempStr [StrLen(TempStr) + 1] = L'\0';
  
  for (Index = 0, NumberIndex = 0, ArgIndex = 1; TempStr[Index]; Index++) {
    if (TempStr[Index] == L'/') {
      TempStr[Index] = L'\0';
      Status = CheckDecember (&TempStr[NumberIndex], &Number);
      if (Status != EFI_SUCCESS) {
        Status = EFI_INVALID_PARAMETER;
        goto TransferStringToDateExit;
      }
      
      switch (ArgIndex) {

      case 1: // year
        if (Number > 0xffff) {
          Status = EFI_INVALID_PARAMETER;
          goto TransferStringToDateExit;
        } else {
          Data->Year = (UINT16)Number;
        }
        break;

      case 2: // month
        if (Number > 12) {
          Status = EFI_INVALID_PARAMETER;
          goto TransferStringToDateExit;
        } else {
          Data->Month = (UINT8)Number;
        }
        break;

      case 3: // date
        if (Number > 31) {
          Status = EFI_INVALID_PARAMETER;
          goto TransferStringToDateExit;
        } else {
          Data->Date = (UINT8)Number;
        }
        break;

      default:
        Status = EFI_INVALID_PARAMETER;
        goto TransferStringToDateExit;
        break;
      }
      
      NumberIndex = Index + 1;
      ArgIndex++;
    }
  }

  if (ArgIndex != 4) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = EFI_SUCCESS;
  } 

TransferStringToDateExit:
  FreePool (TempStr);
  return Status;
}


EFI_STATUS 
EFIAPI
TransferStringToTime (
  IN      CHAR16                                        *String,
  OUT     FILTER_EVENT_INFO                             *Data
  ) 
{
  EFI_STATUS                                            Status = EFI_SUCCESS;
  CHAR16                                                *TempStr = NULL;
  UINT32                                                Index = 0;
  UINT32                                                NumberIndex = 0;
  UINT32                                                ArgIndex = 0;
  UINT32                                                Number = 0;
  
  if ((String == NULL) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempStr = AllocateZeroPool ((StrLen (String) + 2) * sizeof (CHAR16));
  if (TempStr == NULL) {
    return EFI_UNSUPPORTED;
  }

  StrCpy (TempStr, String);

  TempStr [StrLen(TempStr)] = L':';
  TempStr [StrLen(TempStr) + 1] = L'\0';
  
  for (Index = 0, NumberIndex = 0, ArgIndex = 1; TempStr[Index]; Index++) {
    if (TempStr[Index] == L':') {
      TempStr[Index] = L'\0';
      Status = CheckDecember (&TempStr[NumberIndex], &Number);
      if (Status != EFI_SUCCESS) {
        Status = EFI_INVALID_PARAMETER;
        goto TransferStringToTimeExit;
      }
      
      switch (ArgIndex) {

      case 1: // Hour
        if (Number > 24) {
          Status = EFI_INVALID_PARAMETER;
          goto TransferStringToTimeExit;
        } else {
          Data->Hour = (UINT8)Number;
        }
        break;

      case 2: // minute
        if (Number > 60) {
          Status = EFI_INVALID_PARAMETER;
          goto TransferStringToTimeExit;
        } else {
          Data->Minute = (UINT8)Number;
        }
        break;

      case 3: // second
        if (Number > 60) {
          Status = EFI_INVALID_PARAMETER;
          goto TransferStringToTimeExit;
        } else {
          Data->Second = (UINT8)Number;
        }
        break;

      default:
        Status = EFI_INVALID_PARAMETER;
        goto TransferStringToTimeExit;
        break;
      }
      
      NumberIndex = Index + 1;
      ArgIndex++;
    }
  }

  if (ArgIndex != 4) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = EFI_SUCCESS;
  } 

TransferStringToTimeExit:
  FreePool (TempStr);
  return Status;
}


EFI_STATUS 
EFIAPI
TransferStringToNumber (
  IN      CHAR16                                        *String,
  OUT     UINT32                                        *NumberOut
  ) 
{
  EFI_STATUS                                            Status = EFI_SUCCESS;
  UINT32                                                Number = 0;
  CHAR16                                                *Temp = NULL;
  
  if (String == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Temp = String;
  while (*Temp && *Temp == L' ') {
    Temp += 1;
  }
  
  if (StrLen (Temp) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((*Temp == L'0') && (*(Temp + 1) == L'x')) {
    Status = CheckHex ((Temp + 2), &Number);
  } else {
    Status = CheckDecember (Temp, &Number);
  }

  if (Status != EFI_SUCCESS) {
    return EFI_INVALID_PARAMETER;
  }

  *NumberOut = Number;

  return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI
TransferStringToEventID (
  IN      CHAR16                                        *String,
  OUT     FILTER_EVENT_INFO                             *Data
  ) 
{
  EFI_STATUS                                            Status = EFI_SUCCESS;
  CHAR16                                                *TempStr = NULL;
  UINT32                                                Index = 0;
  UINT32                                                NumberIndex = 0;
  UINT32                                                ArgIndex = 0;
  UINT32                                                Number = 0;
  
  if ((String == NULL) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TempStr = AllocateZeroPool ((StrLen (String) + 2) * sizeof (CHAR16));
  if (TempStr == NULL) {
    return EFI_UNSUPPORTED;
  }
  StrCpy (TempStr, String);

  TempStr [StrLen(TempStr)] = L' ';
  TempStr [StrLen(TempStr) + 1] = L'\0';
  
  for (Index = 0, NumberIndex = 0, ArgIndex = 1; TempStr[Index]; Index++) {

    //
    // skip preceeding white space, and Get Number start Index
    //
    while ((TempStr[Index]) && (TempStr[Index] == L' ')) {
      Index += 1;
    }
    NumberIndex = Index;

    //
    // Get Number End Index
    //
    while ((TempStr[Index]) && (TempStr[Index] != L' ')) {
      Index += 1;
    }

    if ((TempStr[Index] == L'\0') || (NumberIndex >= Index)) {
      break;
    }
    
    TempStr[Index] = L'\0';
    Status = TransferStringToNumber (&TempStr[NumberIndex], &Number);
    if ((Status != EFI_SUCCESS) || (Number > 0xff)) {
      Status = EFI_INVALID_PARAMETER;
      goto TransferStringToEventIDExit;
    }
    
    switch (ArgIndex) {

    case 1: // SensorType
      Data->EventID.SensorType = (UINT8)Number;
      break;

    case 2: // SensorNum
      Data->EventID.SensorNum = (UINT8)Number;
      break;

    case 3: // EventType
      Data->EventID.EventType = (UINT8)Number;
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      goto TransferStringToEventIDExit;
      break;
    }
    
    NumberIndex = Index + 1;
    ArgIndex++;
    
  }

  if (ArgIndex != 4) {
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = EFI_SUCCESS;
  } 

TransferStringToEventIDExit:
  FreePool (TempStr);
  return Status;
}


CHAR16* 
EFIAPI
DisplayMessageAndGetKeywordString (
  IN      CHAR16                                        *Title,
  IN      CHAR16                                        *Prompt,
  IN      UINT32                                        keywordSize,
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen
  ) 
{
  UINT32                                                WindowColumnSize = 0;
  EFI_SCREEN_DESCRIPTOR                                 MessageScreen = {0};
  CHAR16                                                StrBuf[80] = {0};
  UINT32                                                Index = 0;
  CHAR16                                                *KeyWord = NULL;
  

  //
  // Set Column Size
  //
  if (StrLen (Title) >= StrLen (Prompt)) {
    WindowColumnSize = (UINT32)StrLen (Title) + 2;
  } else if (StrLen (Prompt) >= keywordSize) {
    WindowColumnSize = (UINT32)StrLen (Prompt) + 2;
  } else {
    WindowColumnSize = keywordSize + 2;
  }

  if (WindowColumnSize >= 79) {
    WindowColumnSize = 79;
  }


  //
  // Set EFI_SCREEN_DESCRIPTOR
  //
  MessageScreen.TopRow      = LocalScreen.BottomRow/3 - 2;
  MessageScreen.BottomRow   = MessageScreen.TopRow + 5;
  MessageScreen.LeftColumn  = LocalScreen.RightColumn/2 - WindowColumnSize/2;
  MessageScreen.RightColumn = LocalScreen.RightColumn/2 + WindowColumnSize/2;
  
  //
  // Print border line
  // +-------------------------------------------+
  // |                                           |
  // |-------------------------------------------|
  // |                                           |
  // |                                           |
  // |                                           |
  // +-------------------------------------------+
  ClearLines (MessageScreen.LeftColumn - 1, MessageScreen.RightColumn, MessageScreen.TopRow - 1, MessageScreen.BottomRow - 1, VE_NORMAL_POPUP_TEXT);

  PrintBorderLine (MessageScreen, VE_NORMAL_POPUP_TEXT);
  


  //
  // Print Title
  //
  SetCurPos ((MessageScreen.LeftColumn + ((MessageScreen.RightColumn - MessageScreen.LeftColumn + 1 - StrLen (Title)) / 2)) - 1, MessageScreen.TopRow);
  Print (L"%s", Title);

  //
  // Print Prompt
  //
  SetCurPos ((MessageScreen.LeftColumn + 1) - 1, (MessageScreen.TopRow + 3) - 1);
  Print (L"%s", Prompt);

  //
  // Print Input Buffer
  //
  for (Index = 0; Index < (WindowColumnSize - 2); Index++) {
    StrBuf[Index] = L' ';
  }
  StrBuf[Index] = L'\0';
  SetColor (INPUT_BUFFER_COLOR);
  SetCurPos ((MessageScreen.LeftColumn + 1) - 1, (MessageScreen.TopRow + 4) - 1);
  Print (L"%s", StrBuf);

  KeyWord = AllocateZeroPool (sizeof (CHAR16) * (keywordSize + 1));
  SetCurPos ((MessageScreen.LeftColumn + 1) - 1, (MessageScreen.TopRow + 4) - 1);
  if (EFI_SUCCESS != InputKeyWord (keywordSize, KeyWord)) {
    FreePool (KeyWord);
    KeyWord = NULL;
  }

  //
  // clear message box
  //
  ClearLines (MessageScreen.LeftColumn - 1, MessageScreen.RightColumn, MessageScreen.TopRow - 1, MessageScreen.BottomRow - 1, VE_ITEM_TEXT);

  return KeyWord;
}


VOID 
EFIAPI
RecoveryItem (
  IN      SELECT_ITEM_INFO                              *ItemList,
  IN      CHAR16                                        *SpaceBuf,
  IN      INT32                                         PageStartNo,
  IN      INT32                                         ItemNo,
  IN      EFI_SCREEN_DESCRIPTOR                         FrameScreen  
  )  
{
  CHAR16                                                StrBuf[80] = {0};
  
  UnicodeSPrint (StrBuf, 80, L"%s", ItemList[ItemNo].String);
  DisplayString ((INT32)FrameScreen.LeftColumn, (INT32)FrameScreen.TopRow + (ItemNo - PageStartNo), SpaceBuf, 28, VE_NORMAL_POPUP_TEXT);
  DisplayString ((INT32)FrameScreen.LeftColumn, (INT32)FrameScreen.TopRow + (ItemNo - PageStartNo), StrBuf, (INT32)(StrLen(StrBuf)), VE_NORMAL_POPUP_TEXT);
}


EFI_STATUS
EFIAPI
SelectItem (
  IN      SELECT_ITEM_INFO                              *ItemList,
  IN      UINT32                                        ItemCount,
  IN      EFI_SCREEN_DESCRIPTOR                         FrameScreen,
  OUT     UINT32                                        *ItemNo
  ) 
{
  INT32                                                 SelectNo = 0;
  INT32                                                 LastSelectNo = 0xffffffff;
  INT32                                                 StartNo = 0;
  INT32                                                 EndNo = 0;
  INT32                                                 PageStartNo = 0;
  BOOLEAN                                               RefreshFileFrame = TRUE;
  BOOLEAN                                               OnePage = TRUE;
  INT32                                                 RelativeIndex = 0;
  EFI_INPUT_KEY                                         Key = {0};
  CHAR16                                                StrBuf[80] = {0};
  CHAR16                                                SpaceBuf[80] = {0};
  UINTN                                                 Index = 0;

  if ((ItemList == NULL) || (ItemCount == 0)) {
    //
    // clean file frame
    //
    return EFI_INVALID_PARAMETER;
  } 

  SelectNo         = 0;
  LastSelectNo     = 0xffffffff;
  StartNo          = 0;
  EndNo            = ItemCount - 1;
  PageStartNo      = 0;
  RefreshFileFrame = TRUE;
  OnePage          = (EndNo < (StartNo + SelectItemSize)) ? TRUE : FALSE;

  for (Index = 0; Index < (FrameScreen.RightColumn - FrameScreen.LeftColumn + 1); Index++) {
    SpaceBuf[Index] = L' ';
  }
  SpaceBuf[Index] = L'\0';
  
  while (TRUE) {
    
    if (RefreshFileFrame) {
      UpdateSelectFrame (ItemList, PageStartNo, ItemCount, FrameScreen);
      if (!OnePage) {
        UpdateScrollBar (ItemCount, PageStartNo, SelectItemSize, FrameScreen);
      }
    }

    if ((SelectNo != LastSelectNo) || RefreshFileFrame) {
      
      UnicodeSPrint (StrBuf, 80, L"%s", ItemList[SelectNo].String);
      DisplayString ((INT32)FrameScreen.LeftColumn, (INT32)FrameScreen.TopRow + (SelectNo - PageStartNo), SpaceBuf, (INT32)(StrLen(SpaceBuf)), VE_NORMAL_POPUP_TEXT);
      DisplayString ((INT32)FrameScreen.LeftColumn, (INT32)FrameScreen.TopRow + (SelectNo - PageStartNo), StrBuf, (INT32)(StrLen(StrBuf)), FILTER_SELECT_ITEM);
      LastSelectNo = SelectNo;
      
    }
    
    RefreshFileFrame = FALSE;

    Key = WaitForAKey (0);

    switch (Key.ScanCode) {

    case SCAN_ESC:
      return EFI_ABORTED;
      break;

    case SCAN_UP:
      if (SelectNo > PageStartNo) {
        RecoveryItem (ItemList, SpaceBuf, PageStartNo, SelectNo, FrameScreen); 
        SelectNo--;
      } else if (SelectNo > StartNo) {
        SelectNo--;
        PageStartNo = SelectNo;
        RefreshFileFrame = TRUE;
      }
      break;

    case SCAN_DOWN:
      if (!OnePage) {
        if (SelectNo < (PageStartNo + SelectItemSize - 1)) {
          RecoveryItem (ItemList, SpaceBuf, PageStartNo, SelectNo, FrameScreen); 
          SelectNo++;
        } else if (SelectNo < EndNo) {
          SelectNo++;
          PageStartNo++;
          RefreshFileFrame = TRUE;
        }
      }      else if (SelectNo < EndNo) { 
        RecoveryItem (ItemList, SpaceBuf, PageStartNo, SelectNo, FrameScreen);  
        SelectNo++;
      }
      break;

    case SCAN_PAGE_UP:
      if (PageStartNo == StartNo) {
        if (SelectNo != StartNo) {
          RecoveryItem (ItemList, SpaceBuf, PageStartNo, SelectNo, FrameScreen); 
          SelectNo = PageStartNo;
        } 
        
      } else if ((PageStartNo - SelectItemSize) >= StartNo) {
        RelativeIndex = SelectNo - PageStartNo;
        PageStartNo = PageStartNo - SelectItemSize;
        SelectNo = PageStartNo + RelativeIndex;
        RefreshFileFrame = TRUE;
      } else if ((PageStartNo - SelectItemSize) < StartNo) {
        RelativeIndex = SelectNo - PageStartNo;
        PageStartNo = StartNo;
        SelectNo = PageStartNo + RelativeIndex;
        RefreshFileFrame = TRUE;
      }
      break;

    case SCAN_PAGE_DOWN:
      if (!OnePage) {
        if ((PageStartNo + SelectItemSize - 1) == EndNo) {
          if (SelectNo != EndNo) {
            RecoveryItem (ItemList, SpaceBuf, PageStartNo, SelectNo, FrameScreen); 
            SelectNo = EndNo;
          } 
          
        } else if ((PageStartNo + SelectItemSize) <= (EndNo - SelectItemSize + 1)) {
          RelativeIndex = SelectNo - PageStartNo;
          PageStartNo = PageStartNo + SelectItemSize;
          SelectNo = PageStartNo + RelativeIndex;
          RefreshFileFrame = TRUE;
        } else if ((PageStartNo + SelectItemSize) > (EndNo - SelectItemSize + 1)) {
          RelativeIndex = SelectNo - PageStartNo;
          PageStartNo = (EndNo - SelectItemSize + 1);
          SelectNo = PageStartNo + RelativeIndex;
          RefreshFileFrame = TRUE;
        }
      } else {
        //
        // Only one page
        //
        if (SelectNo != EndNo) {
          RecoveryItem (ItemList, SpaceBuf, PageStartNo, SelectNo, FrameScreen); 
          SelectNo = EndNo;
        } 
      }
      break;

    default:

      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        *ItemNo = SelectNo;
        return EFI_SUCCESS;
      }
      break;
    }

  }
  
  
  return EFI_NOT_READY;
}


UINT32
EFIAPI
DisplayDialogueMessage (
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen,
  IN      SELECT_ITEM_INFO                              *LogTypeItem,
  ...
  ) 
{
  EFI_STATUS                                            Status;
  VA_LIST                                               Marker;
  UINT32                                                Index = 0;
  SELECT_ITEM_INFO                                      *Item = NULL;
  SELECT_ITEM_INFO                                      *ItemList = NULL;
  SELECT_ITEM_INFO                                      *ItemListTemp = NULL;
  UINT32                                                ItemCount = ITEM_TYPE_COUNT;
  UINT32                                                ItemNo = 0;
  EFI_SCREEN_DESCRIPTOR                                 MessageScreen = {0};
  EFI_SCREEN_DESCRIPTOR                                 FrameScreen = {0};
  UINTN                                                 StrLenth = 0;  
  UINT32                                                SelectedItemNum;

  //
  // Get ItemList
  //
  ItemList = (SELECT_ITEM_INFO *)AllocateZeroPool (sizeof (SELECT_ITEM_INFO) * ItemCount);
  if (ItemList == NULL) {
    return NO_DIALOGUE_RESPONSE;
  }
  
  VA_START (Marker, LogTypeItem);

  for (Index = 0, Item = LogTypeItem; Item->String != NULL; Index++) {

    if (Index >= ItemCount) {
      //
      // Reallocate larger Buffer
      //
      ItemListTemp = ItemList;
      ItemList = NULL;
      ItemList = (SELECT_ITEM_INFO *)AllocateZeroPool (sizeof (SELECT_ITEM_INFO) * (ItemCount + ITEM_TYPE_COUNT));
      if (ItemList == NULL) {
        return NO_DIALOGUE_RESPONSE;
      }
      CopyMem ((VOID *)ItemList, (VOID *)ItemListTemp, sizeof (SELECT_ITEM_INFO) * ItemCount);
      FreePool (ItemListTemp);
      ItemCount += ITEM_TYPE_COUNT;
    }
    
    CopyMem ((VOID *)&ItemList[Index], (VOID *)Item, sizeof (SELECT_ITEM_INFO));

    if (StrLen (Item->String) > StrLenth) {
      StrLenth = StrLen (Item->String);
    }
    
    Item = (SELECT_ITEM_INFO *)(VA_ARG (Marker, SELECT_ITEM_INFO *));
  }
  ItemCount = Index;
  VA_END (Marker);

  if (StrLenth < 14) {
    StrLenth = 15;
  }

  if (ItemCount > FILTER_SELECT_FRAME_ITEM_SIZE) {
    SelectItemSize = FILTER_SELECT_FRAME_ITEM_SIZE;
  } else {
    SelectItemSize = (INT32)ItemCount;
  } 

  //
  // Set Message box info
  //
  MessageScreen.TopRow      = LocalScreen.BottomRow/3 - 2;
  MessageScreen.BottomRow   = MessageScreen.TopRow + (UINTN)SelectItemSize + 3;
  MessageScreen.LeftColumn  = LocalScreen.RightColumn/2 - (StrLenth + 4)/2;
  MessageScreen.RightColumn = LocalScreen.RightColumn/2 + (StrLenth + 4)/2;

  //
  // Print border line
  // +-------------------------------------------+
  // |                                           |
  // |-------------------------------------------|
  // |                                           |
  // |                                           |
  // |                                           |
  // +-------------------------------------------+
  ClearLines (MessageScreen.LeftColumn - 1,  MessageScreen.RightColumn, MessageScreen.TopRow - 1, MessageScreen.BottomRow - 1, VE_NORMAL_POPUP_TEXT);

  PrintBorderLine (MessageScreen, VE_NORMAL_POPUP_TEXT);

  //
  //Print Title
  //
  SetCurPos ((MessageScreen.LeftColumn + ((MessageScreen.RightColumn - MessageScreen.LeftColumn + 1 - StrLen (L"Filter By")) / 2)) - 1, MessageScreen.TopRow);
  Print (L"Filter By");

  //
  // Select Item
  //
  FrameScreen.TopRow      = MessageScreen.TopRow + 3;
  FrameScreen.BottomRow   = MessageScreen.BottomRow - 1;
  FrameScreen.LeftColumn  = MessageScreen.LeftColumn + 2;
  FrameScreen.RightColumn = MessageScreen.RightColumn - 1;
  Status = SelectItem (ItemList, ItemCount, FrameScreen, (UINT32 *)&ItemNo);
  if (Status == EFI_SUCCESS) {    
    ClearLines (MessageScreen.LeftColumn - 1,  MessageScreen.RightColumn, MessageScreen.TopRow - 1, MessageScreen.BottomRow - 1, VE_ITEM_TEXT);
    SelectedItemNum = (UINT32)ItemList[ItemNo].Number;
    FreePool (ItemList);
    return SelectedItemNum;
  }

  
  //
  // clear message box
  //
  ClearLines (MessageScreen.LeftColumn - 1,  MessageScreen.RightColumn, MessageScreen.TopRow - 1, MessageScreen.BottomRow - 1, VE_ITEM_TEXT);

  FreePool (ItemList);
  return NO_DIALOGUE_RESPONSE;
}

VOID 
EFIAPI
ShowCurrentConditionMsg (
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen,
  IN      BOOLEAN                                       ClearMsg,
  IN      SELECT_ITEM_INFO                              *LogTypeItem,
  ...
  ) 
{
  VA_LIST                                               Marker;
  static UINT32                                         ItemCount = 0;
  static EFI_SCREEN_DESCRIPTOR                          MessageScreen = {0};
  UINT32                                                Index = 0;
  SELECT_ITEM_INFO                                      *Item = NULL;

  if (ClearMsg && (ItemCount != 0)) {
    ClearLines (MessageScreen.LeftColumn - 1,  MessageScreen.RightColumn, MessageScreen.TopRow - 1, MessageScreen.BottomRow - 1, VE_ITEM_TEXT);
    SetMem (&MessageScreen, sizeof (EFI_SCREEN_DESCRIPTOR), 0);
    ItemCount = 0;
  }

  //
  // Set Message box info
  //
  MessageScreen.TopRow      = LocalScreen.BottomRow/3 - 2;
  MessageScreen.LeftColumn  = LocalScreen.RightColumn - 3 - 28;
  MessageScreen.RightColumn = LocalScreen.RightColumn - 3;

  SetColor (VE_NORMAL_POPUP_TEXT);

  VA_START (Marker, LogTypeItem);

  for (Index = 0, ItemCount = 0, Item = LogTypeItem; Item->String != NULL; Index++) {

    SetCurPos ((MessageScreen.LeftColumn + 1) - 1, (MessageScreen.TopRow + 3 + ItemCount) - 1);
    Print (L"                           ");
    SetCurPos ((MessageScreen.LeftColumn + 1) - 1, (MessageScreen.TopRow + 3 + ItemCount) - 1);
    
    
    switch (Item->Number) {

    case TypeDate:
      if ((gFilteredEventLogData.Year == 0xFFFF) || (gFilteredEventLogData.Month == 0xFF) || (gFilteredEventLogData.Date == 0xFF)) {
        Print (L" Date: None");
      } else {
        Print (L" Date: %04d/%02d/%02d", gFilteredEventLogData.Year, gFilteredEventLogData.Month, gFilteredEventLogData.Date);
      }
      ItemCount++;
      break;

    case TypeTime:
      if ((gFilteredEventLogData.Hour == 0xFF) || (gFilteredEventLogData.Minute == 0xFF) || (gFilteredEventLogData.Second == 0xFF)) {
        Print (L" Time: None");
      } else {
        Print (L" Time: %02d:%02d:%02d", gFilteredEventLogData.Hour, gFilteredEventLogData.Minute, gFilteredEventLogData.Second);
      }
      ItemCount++;
      break;

    case TypeGeneratorID:
      if (gFilteredEventLogData.GeneratorId == 0xFFFF) {
        Print (L" Generator ID: None");
      } else {
        Print (L" Generator ID: 0x%x", gFilteredEventLogData.GeneratorId);
      }
      ItemCount++;
      break;

    case TypeEventID:
       if ((gFilteredEventLogData.EventID.SensorType == 0xFF) || (gFilteredEventLogData.EventID.SensorNum == 0xFF) || (gFilteredEventLogData.EventID.EventType == 0xFF)) {
        Print (L" Event ID: None");
      } else {
        Print (L" Event ID: 0x%02x 0x%02x 0x%02x", gFilteredEventLogData.EventID.SensorType, gFilteredEventLogData.EventID.SensorNum, gFilteredEventLogData.EventID.EventType);
      }
      ItemCount++;
      break;

    case TypeSensorType:
      if (gFilteredEventLogData.EventID.SensorType == 0xFF) {
        Print (L" Sensor Type: None");
      } else {
        Print (L" Sensor Type: 0x%02x", gFilteredEventLogData.EventID.SensorType);
      }
      ItemCount++;
      break;

    case TypeSensorNum:
      if (gFilteredEventLogData.EventID.SensorNum == 0xFF) {
        Print (L" Sensor Num: None");
      } else {
        Print (L" Sensor Num: 0x%02x", gFilteredEventLogData.EventID.SensorNum);
      }
      ItemCount++;
      break;

    case TypeEventType:
      if (gFilteredEventLogData.EventID.EventType == 0xFF) {
        Print (L" Event Type: None");
      } else {
        Print (L" Event Type: 0x%02x", gFilteredEventLogData.EventID.EventType);
      }
      ItemCount++;
      break;

    case TypeEventTypeID:
      if (gFilteredEventLogData.EventTypeID == 0xFF) {
        Print (L" Event Type ID: None");
      } else {
        Print (L" Event Type ID: 0x%02x", gFilteredEventLogData.EventTypeID);
      }
      ItemCount++;
      break;

    default:
      break;
    }
    
    Item = (SELECT_ITEM_INFO *)(VA_ARG (Marker, SELECT_ITEM_INFO *));
  }
  VA_END (Marker);

  if (ItemCount == 0) {
    return;
  }

  //
  // print title
  //
  SetCurPos ((MessageScreen.LeftColumn + 1) - 1, (MessageScreen.TopRow + 1) - 1);
  Print (L"    Filtered Condition     ");

  //
  // set BottomRow
  //
  MessageScreen.BottomRow = MessageScreen.TopRow + (UINTN)ItemCount + 3;
  
  PrintBorderLine (MessageScreen, VE_NORMAL_POPUP_TEXT);
  
  
}


CHAR16 *
EFIAPI
DisplayMsgAndGetKeyWordByFilterType (
  IN      UINT32                                        FilterType,
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen  
  ) 
{
  CHAR16                                                *KeyWord = NULL;
   
  switch (FilterType) {

  case TypeDate:
    KeyWord = DisplayMessageAndGetKeywordString (L"Date", L"Format:[Year]/[Month]/[Date] (Ex. 2013/07/31)", 10, LocalScreen);
    break;

  case TypeTime:
    KeyWord = DisplayMessageAndGetKeywordString (L"Time", L"Format:[Hour]:[Minute]:[Second] (Ex. 03:21:58)", 8, LocalScreen);
    break;

  case TypeGeneratorID:
    KeyWord = DisplayMessageAndGetKeywordString (L"Generator ID", L"Format:[Generator ID] (Ex. 0x0020)", 6, LocalScreen);
    break;

  case TypeEventID:
    KeyWord = DisplayMessageAndGetKeywordString (L"Event ID", L"Format:[Sensor Type] [Sensor Num] [Event Type](Ex. 0x0F 0x00 0x6F)", 14, LocalScreen);
    break;

  case TypeSensorType:
    KeyWord = DisplayMessageAndGetKeywordString (L"Sensor Type", L"Format:[Sensor Type] (Ex. 0x02)", 4, LocalScreen);
    break;

  case TypeSensorNum:
    KeyWord = DisplayMessageAndGetKeywordString (L"Sensor Num", L"Format:[Sensor Num] (Ex. 0xEC)", 4, LocalScreen);
    break;

  case TypeEventType:
    KeyWord = DisplayMessageAndGetKeywordString (L"Event Type", L"Format:[Event Type] (Ex. 0x02)", 4, LocalScreen);
    break;

  case TypeEventTypeID:
    KeyWord = DisplayMessageAndGetKeywordString (L"Event Type ID", L"Format:[Event Type ID] (Ex. 0x02)", 4, LocalScreen);
    break;

  default:
    break;
  }

  return KeyWord;
}


EFI_STATUS 
EFIAPI
TransferStringToData (
  IN      CHAR16                                        *String,
  IN OUT  FILTER_EVENT_INFO                             *Data
  ) 
{
  EFI_STATUS                                            Status = EFI_SUCCESS;
  UINT32                                                Number = 0;
  
  switch (Data->FilterType) {

  case TypeDate:
    Status = TransferStringToDate (String, Data);
    break;

  case TypeTime:
    Status = TransferStringToTime (String, Data);
    break;

  case TypeGeneratorID:
    Status = TransferStringToNumber (String, &Number);
    if ((Status == EFI_SUCCESS) && (Number <= 0xffff)) {
      Data->GeneratorId = (UINT16)Number;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
    break;

  case TypeEventID:
    Status = TransferStringToEventID (String, Data);
    break;

  case TypeSensorType:
    Status = TransferStringToNumber (String, &Number);
    if ((Status == EFI_SUCCESS) && (Number <= 0xff)) {
      Data->EventID.SensorType = (UINT8)Number;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
    break;

  case TypeSensorNum:
    Status = TransferStringToNumber (String, &Number);
    if ((Status == EFI_SUCCESS) && (Number <= 0xff)) {
      Data->EventID.SensorNum = (UINT8)Number;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
    break;

  case TypeEventType:
    Status = TransferStringToNumber (String, &Number);
    if ((Status == EFI_SUCCESS) && (Number <= 0xff)) {
      Data->EventID.EventType = (UINT8)Number;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
    break;

  case TypeEventTypeID:
    Status = TransferStringToNumber (String, &Number);
    if ((Status == EFI_SUCCESS) && (Number <= 0xff)) {
      Data->EventTypeID = (UINT8)Number;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
    break;    

  default:
    Status = EFI_INVALID_PARAMETER;
    break;
  }


  //
  // See data after transferring string
  //
//  if (Status == EFI_SUCCESS) {
//    PrintResult (Data);
//  }

  return Status;
}

