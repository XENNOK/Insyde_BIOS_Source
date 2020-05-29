/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Platform.h>
#include <Print.h>
#include <SmmKeyboard.h>
#include <Library/BaseLib.h>

/**
 Convert hex string to uint

 @param        str      The string


**/
UINTN
Atoi (
  CHAR16  *str
  )
{
  UINTN   u;
  CHAR16  c;
  UINTN   m;
  UINTN   n;

  ASSERT (str != NULL);

  m = (UINTN) -1 / 10;
  n = (UINTN) -1 % 10;
  //
  // skip preceeding white space
  //
  while (*str && *str == ' ') {
    str += 1;
  }
  //
  // convert digits
  //
  u = 0;
  c = *(str++);
  while (c) {
    if (c >= '0' && c <= '9') {
      if (u > m || u == m && c - '0' > (INTN) n) {
        return (UINTN) -1;
      }

      u = (u * 10) + c - '0';
    } else {
      break;
    }

    c = *(str++);
  }

  return u;
}

/**
 AvSPrint worker function that prints a Value as a decimal number in Buffer

 @param [in, out] Buffer        Location to place ascii decimal number string of Value.
 @param [in]   Value            Decimal value to convert to a string in Buffer.
 @param [in]   Width            Width of hex value.

 @return Number of characters printed.

**/
UINTN
ValueToString (
  IN  OUT CHAR16  *Buffer,
  IN  INT64       Value,
  IN  UINTN       Width
  )
{
  CHAR16 TempBuffer[30];
  CHAR16 *TempStr;
  CHAR16 *BufferPtr;
  UINTN Count;
  UINT32 Remainder;

  TempStr   = TempBuffer;
  BufferPtr = Buffer;
  Count     = 0;

  if (Value < 0) {
    *(BufferPtr++)  = L'-';
    Value           = -Value;
    Count++;
  }

  do {
    Value         = (INT64) DivU64x32Remainder ((UINT64) Value, 10, &Remainder);
    *(TempStr++)  = (CHAR16) (Remainder + L'0');
    Count++;
  } while (Value != 0);

  //
  // Reverse temp string into Buffer.
  //
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
  }

  *BufferPtr = 0;
  return Count;
}

/**
 Clear display dialog.

 @param [in]   StartX           The dialog start coulmn address.
 @param [in]   StartY           The dialog start row address.
 @param [in]   EndX             The dialog end coulmn address.
 @param [in]   EndY             The dialog end rom address.

 @retval None.

**/
VOID
ClearString(
  IN UINTN StartX,
  IN UINTN StartY,
  IN UINTN EndX,
  IN UINTN EndY
  )
{
  UINTN Index;
  CHAR16 Buffer[100]; 

  if (StartX > EndX) {
    return;
  }


  if (StartY > EndY) {
    return;
  }

  for (Index = 0; Index < (EndX - StartX); Index++) {
     Buffer[Index] = L' ';
  }
  Buffer[Index] = 0;


  Index = StartY;
  do {
    ColorPrintAt(StartX, Index, Buffer ,PRINT_ATTRIBUTE_NORMAL);
    Index++;
  } while (Index <= EndY);
}

/**
 Display PaswordDialog then wait user input key.

 @param [in]   InputBufferLen   The input string buffer length.
 @param [in, out] StringBuffer  The input string buffer address.

 @retval None.

**/
VOID
PaswordDialog(
  IN UINTN InputBufferLen,
  IN OUT CHAR16 *StringBuffer
  )
{
  UINTN            Max_X;
  UINTN            Max_Y;
  UINTN            Set_X;
  UINTN            Set_Y;
  UINT16           KeyBuffer[2];
  CHAR16           *Char16Ptr;
  UINTN            Index;
  EFI_INPUT_KEY    OutPutKey;

  KeyBuffer[0] = 0x00;
  KeyBuffer[1] = 0x00;
  Char16Ptr = &KeyBuffer[0];
  Set_X = 0x00;
  Set_Y = 0x00;

  GetDisplayMode(&Max_X,&Max_Y);

  //
  // Compute the start poisition and print "Input Password:"
  //
  PrintAt((Max_X - (StrLen(L"Input Password:") + InputBufferLen)) / 2,
          Max_Y / 2,
          L"Input Password:"
         );
  SetCurser((Max_X - (StrLen(L"Input Password:") + InputBufferLen)) / 2
                   + StrLen(L"Input Password:"),
             Max_Y / 2);

  //
  // Clear string buffer
  //
  for (Index = 0; Index < InputBufferLen; Index++) {
     StringBuffer[Index] = 0x00;
  }

  //
  // Get user pass password
  //
  Index = 0;
  OutPutKey.UnicodeChar = 0;
  while ((Index < InputBufferLen) && (OutPutKey.UnicodeChar != CHAR_CARRIAGE_RETURN)) {
    while (EFI_ERROR(ReadKey(&OutPutKey))) {
    }

    switch (OutPutKey.UnicodeChar) {
      case CHAR_BACKSPACE:
          if (Index == 0) {
            break;
          }
          Index--;
          StringBuffer[Index] = 0;
          GetCurser(&Set_X,&Set_Y);
          SetCurser(--Set_X,Set_Y);
          *Char16Ptr = L' ';
          DisplayCharacter(L' ',0);
          SetCurser(Set_X,Set_Y);
        break;
      case CHAR_CARRIAGE_RETURN:

        break;
      default:
          DisplayCharacter(L'*',0);
          StringBuffer[Index] = OutPutKey.UnicodeChar;
          Index++;
        break;
    }
  }
  ClearString((Max_X - (StrLen(L"Input Password:") + InputBufferLen)) / 2,
               Max_Y / 2,
               (Max_X - (StrLen(L"Input Password:") + InputBufferLen)) / 2 +
                 (StrLen(L"Input Password:") + InputBufferLen),
               Max_Y / 2);
  return;
}

/**
 Display SelectDialog then wait user select item.

 @param [in]        StringArrey         The item buffer address array of string item.
 @param [in]        NumOfItem           Number of item.
 @param [in]        MaxStringLen        The maximum string length of item.
 @param [in]        TitilString         The dialog title string.
 @param [in, out]   SelectIndex         User or application select item number.
 @param [out]       EventKey            Report input key

 @retval None.
*/
VOID
SelectDialog (
  IN     CHAR16           **StringArrey,
  IN     UINTN            NumOfItem,
  IN     UINTN            MaxStringLen,
  IN     CHAR16           *TitilString,
  IN OUT UINTN            *SelectIndex,
     OUT EFI_INPUT_KEY    *EventKey
  )
{
  UINTN            Max_X;
  UINTN            Max_Y;
  UINTN            Set_X;
  UINTN            Set_Y;
  UINTN            Index;
  EFI_INPUT_KEY    Key;
  UINTN            RecIndex;
  BOOLEAN          LoopEnable;

  Set_X = 0x00;
  Set_Y = 0x00;

  GetDisplayMode(&Max_X,&Max_Y);

  //
  // Compute the window's start poisition
  //
  Set_Y = (Max_Y - (NumOfItem + 1)) / 2;
  Set_X = (Max_X - MaxStringLen) / 2;

  if (TitilString != NULL) {
    //
    // Print title string
    //
    PrintAt((Max_X - StrLen(TitilString)) / 2,Set_Y,TitilString);
  }

  //
  // Print Information on window's buttom
  //
  PrintAt(Set_X,Set_Y + NumOfItem + 1,L"Press [Esc] abort. Please Select Item:" );
  SetCurser(Set_X,Set_Y + NumOfItem + 1);


  Key.UnicodeChar = 0x00;
  Key.ScanCode = 0x00;
  RecIndex = *SelectIndex;

  //
  // Print select item
  //
  LoopEnable = TRUE;
  while (LoopEnable) {

    if ((*SelectIndex) >= NumOfItem) {
      (*SelectIndex) = 0x00;
    }

    for (Index = 0; Index < NumOfItem; Index++) {

      if (Index == (*SelectIndex)) {
        //
        // item == select, set high light
        //
        ColorPrintAt(Set_X,Set_Y + Index + 1,StringArrey[Index],PRINT_ATTRIBUTE_BLOCK); 
        continue;
      }
      ColorPrintAt(Set_X,Set_Y + Index + 1,StringArrey[Index],PRINT_ATTRIBUTE_NORMAL); 
    }

    //
    // Wait Key
    //
    while (EFI_ERROR(ReadKey(&Key))) {
    }

    if (Key.ScanCode == SCAN_NULL) {
      switch (Key.UnicodeChar) {
      case CHAR_CARRIAGE_RETURN: //Enter
        //
        // Press 'Enter'break while loop
        //
        LoopEnable = FALSE;
//[-start-130110-IB10870066-modify]//
        if (EventKey != NULL) {
          EventKey->ScanCode = Key.ScanCode;
          EventKey->UnicodeChar = Key.UnicodeChar;
        }
//[-end-130110-IB10870066-modify]//
        break;
      default:
        break;
      }
    } else {
      switch (Key.ScanCode) {
      case SCAN_UP: //Enter
        if ((*SelectIndex) == 0) {
          *SelectIndex = NumOfItem - 1;
        } else {
          (*SelectIndex)--;
        }
        break;
      case SCAN_DOWN:
        if ((*SelectIndex) == (NumOfItem - 1)) {
          *SelectIndex = 0;
        } else {
          (*SelectIndex)++;
        }
        break;
      case SCAN_ESC:
        //
        // Press 'ESC' break while loop
        //
        LoopEnable = FALSE;
        break;
      default:
        break;
      }

      if (EventKey != NULL) {
        EventKey->ScanCode = Key.ScanCode;
        EventKey->UnicodeChar = Key.UnicodeChar;
      }
    }//if (Key.ScanCode == SCAN_NULL)
  }

  ClearString (Set_X,Set_Y,Set_X + MaxStringLen, Set_Y + NumOfItem + 2);
  return;
}

