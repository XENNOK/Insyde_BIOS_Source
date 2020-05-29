/** @file
 Implementation for handling user input from the User Interface

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "Ui.h"
#include "Setup.h"
#include "SetupUtility.h"

/**
 Get password kind description string.

 @param[in]  MenuOption    Pointer to the current input menu.
 @param[out] PasswordKind  A double pointer to save .

 @retval EFI_SUCCESS       If string input is read successfully
 @retval other             If operation fails
**/
EFI_STATUS
GetPasswordKind (
  IN  UI_MENU_OPTION        *MenuOption,
  OUT UINT16                **PasswordKind
  )
{

  if (PasswordKind == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *PasswordKind  = NULL;
  *PasswordKind = MenuOption->Description;

  if (*PasswordKind == NULL) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
 Get password input from user.

 @param[in]  MenuOption        Pointer to the current input menu.
 @param[in]  PassWordState
 @param[in]  LineNum
 @param[in]  CreatePopUp
 @param[out] StringPtr         Destination for use input string.

 @retval EFI_SUCCESS       If string input is read successfully
 @retval EFI_DEVICE_ERROR  If operation fails
**/
EFI_STATUS
ReadPassword (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  UINTN                       PassWordState,
  IN  UINTN                       LineNum,
  IN  BOOLEAN                     CreatePopUp,
  OUT CHAR16                      *StringPtr
  )
{
  EFI_STATUS              Status;
  EFI_KEY_DATA            EfiKeyData;
  CHAR16                  NullCharacter;
  UINTN                   ScreenSize;
  CHAR16                  KeyPad[2];
  CHAR16                  *TempString;
  UINTN                   Index;
  UINTN                   Start;
  UINTN                   Top;
  UINTN                   DimensionsWidth;
  UINTN                   DimensionsHeight;
  UINTN                   Minimum;
  UINTN                   Maximum;
  UINTN                   X, Y;
  FORM_BROWSER_STATEMENT  *Question;
  CHAR16                  *PasswordKind;
  KEYBOARD_ATTRIBUTES     KeyboardAttributes;

  DimensionsWidth  = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  NullCharacter    = CHAR_NULL;
  Question         = MenuOption->ThisTag;
  Minimum          = (UINTN) Question->Minimum;
  Maximum          = (UINTN) Question->Maximum;

  TempString = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
  ASSERT (TempString != NULL);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetPasswordKind (MenuOption, &PasswordKind);
  ScreenSize = MAX (GetStringWidth (gPromptForPassword), GetStringWidth (gPromptForNewPassword));
  ScreenSize = MAX (ScreenSize, GetStringWidth (PasswordKind));
  ScreenSize = (MAX (ScreenSize, GetStringWidth (gConfirmPassword)) / 2) + Maximum + 2;
  Start = (DimensionsWidth - ScreenSize - 4) / 2 + gScreenDimensions.LeftColumn + 1;
  Top   = ((DimensionsHeight - 6) / 2) + gScreenDimensions.TopRow - 1;

  if (CreatePopUp && !EFI_ERROR (Status)) {
    switch (PassWordState) {

    case UNLOCK_PASSWORD:
      CreatePasswordPopUp (
        ScreenSize,
        PASSWORD_INPUT_TYPE,
        Maximum,
        3,
        PasswordKind,
        gPromptForPassword
        );
        LineNum++;
      break;

    case ENTER_OLD_PASSWORD:
      CreatePasswordPopUp (
        ScreenSize,
        PASSWORD_INPUT_TYPE,
        Maximum,
        5,
        PasswordKind,
        gPromptForPassword,
        gPromptForNewPassword,
        gConfirmPassword
        );
      break;

    case ENTER_NEW_PASSWORD:
      CreatePasswordPopUp (
        ScreenSize,
        PASSWORD_INPUT_TYPE,
        Maximum,
        4,
        PasswordKind,
        gPromptForNewPassword,
        gConfirmPassword
        );
      break;
    }
  }
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));
  for (Index = 0; Index < Maximum; Index++) {
    PrintCharAt (Start + (ScreenSize - Maximum) + Index, Top + 2 + LineNum, L' ');
  }

  do {
    if (!UiInputDevice(0, &EfiKeyData, &X, &Y) && (EfiKeyData.Key.UnicodeChar == CHAR_CARRIAGE_RETURN)) {
      //
      //mouse input
      //
      gSetupMouse->GetKeyboardAttributes (gSetupMouse, &KeyboardAttributes);
      if (!KeyboardAttributes.IsStart) {
        if ((X < (Start - 1)) || (X > (Start + ScreenSize))) {
          continue;
        }

        if (Y != (Top + 2 + LineNum)) {
          continue;
        }

        //
        //Draw Keyboard
        //
        gSetupMouse->StartKeyboard(gSetupMouse, 2, gScreenDimensions.BottomRow);
      }

      continue;
    }
    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));

    OemSvcPasswordInputFilter (&EfiKeyData);
    if (EfiKeyData.Key.ScanCode == SCAN_NULL && EfiKeyData.Key.UnicodeChar == CHAR_NULL) {
      continue;
    }

    switch (EfiKeyData.Key.UnicodeChar) {

    case CHAR_TAB:
    case CHAR_NULL:
      switch (EfiKeyData.Key.ScanCode) {
      case SCAN_LEFT:
        break;

      case SCAN_RIGHT:
        break;

      case SCAN_ESC:
        Status = EFI_DEVICE_ERROR;
        goto Done;

      default:
        break;
      }

      break;

    case CHAR_CARRIAGE_RETURN:


      if ((GetStringWidth (StringPtr) >= ((Minimum + 1) * sizeof (CHAR16))) ||
          (GetStringWidth (StringPtr) == sizeof (CHAR16))) {
        Status = EFI_SUCCESS;
        goto Done;
      } else {
        //
        // Simply create a popup to tell the user that they had typed in too few characters.
        // To save code space, we can then treat this as an error and return back to the menu.
        //
        do {
          CreateDialog (4, TRUE, 0, NULL, &EfiKeyData.Key, &NullCharacter, gMiniString, gPressEnter, &NullCharacter);
        } while (EfiKeyData.Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

        Status = EFI_DEVICE_ERROR;
        goto Done;
      }

      break;

    case CHAR_BACKSPACE:
      if (StringPtr[0] != CHAR_NULL) {
        for (Index = 0; StringPtr[Index] != CHAR_NULL; Index++) {
          TempString[Index] = StringPtr[Index];
        }
        //
        // Effectively truncate string by 1 character
        //
        TempString[Index - 1] = CHAR_NULL;
        StrCpy (StringPtr, TempString);
      }

    default:
      //
      // If it is the beginning of the string, don't worry about checking maximum limits
      //
      if ((StringPtr[0] == CHAR_NULL) && (EfiKeyData.Key.UnicodeChar != CHAR_BACKSPACE)) {
        StrnCpy (StringPtr, &EfiKeyData.Key.UnicodeChar, 1);
        StrnCpy (TempString, &EfiKeyData.Key.UnicodeChar, 1);
      } else if ((GetStringWidth (StringPtr) < ((Maximum + 1) * sizeof (CHAR16))) && (EfiKeyData.Key.UnicodeChar != CHAR_BACKSPACE)) {
        KeyPad[0] = EfiKeyData.Key.UnicodeChar;
        KeyPad[1] = CHAR_NULL;
        StrCat (StringPtr, KeyPad);
        StrCat (TempString, KeyPad);
      }

      //
      // If the width of the input string is now larger than the screen, we nee to
      // adjust the index to start printing portions of the string
      //
      for (Index = 0; Index < Maximum; Index++) {
        PrintCharAt (Start + (ScreenSize - Maximum) + Index, Top + 2 + LineNum, L' ');
      }
      if ((GetStringWidth (StringPtr) / 2) > (DimensionsWidth - 2)) {
        Index = (GetStringWidth (StringPtr) / 2) - DimensionsWidth + 2;
      } else {
        Index = 0;
      }

      gST->ConOut->SetCursorPosition (gST->ConOut, Start + ScreenSize - Maximum / 2 - GetStringWidth (StringPtr) / 4, Top + 2 + LineNum);

      for (Index = 0; Index + 1 < GetStringWidth (StringPtr) / 2; Index++) {
        PrintChar (L'*');
      }
      break;
    }

    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
  } while (TRUE);

Done:
  if (gSetupMouse != NULL) {
    gSetupMouse->CloseKeyboard(gSetupMouse);
  }
  if (TempString != NULL) {
    gBS->FreePool (TempString);
  }
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
  return Status;
}

/**
 Get string or password input from user.

 @param[in]  MenuOption        Pointer to the current input menu.
 @param[in]  Prompt            The prompt string shown on popup window.
 @param[out] StringPtr         Old user input and destination for use input string.

 @retval EFI_SUCCESS       If string input is read successfully
 @retval EFI_DEVICE_ERROR  If operation fails
**/
EFI_STATUS
ReadString (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  CHAR16                      *Prompt,
  OUT CHAR16                      *StringPtr
  )
{
  EFI_STATUS              Status;
  EFI_INPUT_KEY           Key;
  CHAR16                  NullCharacter;
  UINTN                   ScreenSize;
  CHAR16                  Space[2];
  CHAR16                  KeyPad[2];
  CHAR16                  *TempString;
  CHAR16                  *BufferedString;
  UINTN                   Index;
  UINTN                   Count;
  UINTN                   Start;
  UINTN                   Top;
  UINTN                   DimensionsWidth;
  UINTN                   DimensionsHeight;
  BOOLEAN                 CursorVisible;
  UINTN                   Minimum;
  UINTN                   Maximum;
  FORM_BROWSER_STATEMENT  *Question;
  BOOLEAN                 IsPassword;
  UINTN                   Columns;
  UINTN                   Rows;

  DimensionsWidth  = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  NullCharacter    = CHAR_NULL;
  ScreenSize       = GetStringWidth (Prompt) / sizeof (CHAR16);
  Space[0]         = L' ';
  Space[1]         = CHAR_NULL;
  ZeroMem (&Key, sizeof(EFI_INPUT_KEY));

  Question         = MenuOption->ThisTag;
  Minimum          = (UINTN) Question->Minimum;
  Maximum          = (UINTN) Question->Maximum;

  if (Question->Operand == EFI_IFR_PASSWORD_OP) {
    IsPassword = TRUE;
  } else {
    IsPassword = FALSE;
  }

  TempString = AllocateZeroPool ((Maximum + 1)* sizeof (CHAR16));
  ASSERT (TempString);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (ScreenSize < (Maximum + 1)) {
    ScreenSize = Maximum + 1;
  }

  if ((ScreenSize + 2) > DimensionsWidth) {
    ScreenSize = DimensionsWidth - 2;
  }

  BufferedString = AllocateZeroPool (ScreenSize * 2);
  ASSERT (BufferedString);
  if (BufferedString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Start = (DimensionsWidth - ScreenSize - 2) / 2 + gScreenDimensions.LeftColumn + 1;
  Top   = ((DimensionsHeight - 6) / 2) + gScreenDimensions.TopRow - 1;

  //
  // Display prompt for string
  //
  CreateMultiStringPopUp (ScreenSize, 4, &NullCharacter, Prompt, Space, &NullCharacter);
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  if (ScreenSize > (Columns / 4)) {
    gPopUpDialogBiggerThanPlatformDialog = TRUE;
  }

  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));

  CursorVisible = gST->ConOut->Mode->CursorVisible;
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  do {
    Status = WaitForKeyStroke (&Key);

    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));
    switch (Key.UnicodeChar) {
    case CHAR_NULL:
      switch (Key.ScanCode) {
      case SCAN_LEFT:
        break;

      case SCAN_RIGHT:
        break;

      case SCAN_ESC:
        gBS->FreePool (TempString);
        gBS->FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_DEVICE_ERROR;

      default:
        break;
      }

      break;

    case CHAR_CARRIAGE_RETURN:

      if (GetStringWidth (StringPtr) >= ((Minimum + 1) * sizeof (CHAR16))) {
        gBS->FreePool (TempString);
        gBS->FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_SUCCESS;
      } else {
        //
        // Simply create a popup to tell the user that they had typed in too few characters.
        // To save code space, we can then treat this as an error and return back to the menu.
        //
        do {
          CreateDialog (4, TRUE, 0, NULL, &Key, &NullCharacter, gMiniString, gPressEnter, &NullCharacter);
        } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

        gBS->FreePool (TempString);
        gBS->FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_DEVICE_ERROR;
      }

      break;

    case CHAR_BACKSPACE:
      if (StringPtr[0] != CHAR_NULL) {
        for (Index = 0; StringPtr[Index] != CHAR_NULL; Index++) {
          TempString[Index] = StringPtr[Index];
        }
        //
        // Effectively truncate string by 1 character
        //
        TempString[Index - 1] = CHAR_NULL;
        StrCpy (StringPtr, TempString);
      }

    default:
      //
      // If it is the beginning of the string, don't worry about checking maximum limits
      //
      if ((StringPtr[0] == CHAR_NULL) && (Key.UnicodeChar != CHAR_BACKSPACE)) {
        StrnCpy (StringPtr, &Key.UnicodeChar, 1);
        StrnCpy (TempString, &Key.UnicodeChar, 1);
      } else if ((GetStringWidth (StringPtr) < ((Maximum + 1) * sizeof (CHAR16))) && (Key.UnicodeChar != CHAR_BACKSPACE)) {
        KeyPad[0] = Key.UnicodeChar;
        KeyPad[1] = CHAR_NULL;
        StrCat (StringPtr, KeyPad);
        StrCat (TempString, KeyPad);
      }

      //
      // If the width of the input string is now larger than the screen, we nee to
      // adjust the index to start printing portions of the string
      //
      SetUnicodeMem (BufferedString, ScreenSize - 1, L' ');
      PrintStringAt (Start + 1, Top + 3, BufferedString);

      if ((GetStringWidth (StringPtr) / 2) > (DimensionsWidth - 2)) {
        Index = (GetStringWidth (StringPtr) / 2) - DimensionsWidth + 2;
      } else {
        Index = 0;
      }

      if (IsPassword) {
        gST->ConOut->SetCursorPosition (gST->ConOut, Start + 1, Top + 3);
      }

      for (Count = 0; Index + 1 < GetStringWidth (StringPtr) / 2; Index++, Count++) {
        BufferedString[Count] = StringPtr[Index];

        if (IsPassword) {
          PrintChar (L'*');
        }
      }

      if (!IsPassword) {
        PrintStringAt (Start + 1, Top + 3, BufferedString);
      }
      break;
    }

    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
    gST->ConOut->SetCursorPosition (gST->ConOut, Start + GetStringWidth (StringPtr) / 2, Top + 3);
  } while (TRUE);

  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
  gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
  return Status;
}

/**
 This routine reads a numeric value from the user input.

 @param[in] Selection      Pointer to current selection.
 @param[in] MenuOption     Pointer to the current input menu.

 @retval EFI_SUCCESS       If numerical input is read successfully
 @retval EFI_DEVICE_ERROR  If operation fails
**/
EFI_STATUS
GetNumericInput (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption
  )
{
  EFI_STATUS              Status;
  UINTN                   Column;
  UINTN                   Row;
  CHAR16                  InputText[25];
  CHAR16                  FormattedNumber[22];
  UINT64                  PreviousNumber[21];
  UINTN                   Count;
  UINTN                   Loop;
  BOOLEAN                 ManualInput;
  BOOLEAN                 HexInput;
  BOOLEAN                 DateOrTime;
  UINTN                   InputWidth;
  UINT64                  EditValue;
  UINT64                  Step;
  UINT64                  Minimum;
  UINT64                  Maximum;
  UINTN                   EraseLen;
  UINT8                   Digital;
  EFI_INPUT_KEY           Key;
  EFI_HII_VALUE           *QuestionValue;
  FORM_BROWSER_FORM       *Form;
  FORM_BROWSER_FORMSET    *FormSet;
  FORM_BROWSER_STATEMENT  *Question;
  INTN                    DayOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  UINTN                   TextStartPtr;


  Column            = MenuOption->OptCol;
  Row               = MenuOption->Row;
  PreviousNumber[0] = 0;
  Count             = 0;
  InputWidth        = 0;
  Digital           = 0;

  FormSet       = Selection->FormSet;
  Form          = Selection->Form;
  Question      = MenuOption->ThisTag;
  QuestionValue = &Question->HiiValue;
  Step          = Question->Step;
  Minimum       = Question->Minimum;
  Maximum       = Question->Maximum;

  if ((Question->Operand == EFI_IFR_DATE_OP) || (Question->Operand == EFI_IFR_TIME_OP)) {
    DateOrTime = TRUE;
  } else {
    DateOrTime = FALSE;
  }

  //
  // Prepare Value to be edit
  //
  EraseLen = 0;
  EditValue = 0;
  if (Question->Operand == EFI_IFR_DATE_OP) {
    Step = 1;
    Minimum = 1;

    switch (MenuOption->Sequence) {
    case 0:
      Maximum = 12;
      EraseLen = 4;
      EditValue = QuestionValue->Value.date.Month;
      break;

    case 1:
      if ((QuestionValue->Value.date.Month==2) && IsLeapYear(QuestionValue->Value.date.Year)) {
        Maximum = 29;
      } else {
        Maximum = DayOfMonth[QuestionValue->Value.date.Month - 1];
      }
      EraseLen = 3;
      EditValue = QuestionValue->Value.date.Day;
      break;

    case 2:
      Maximum = 2099;
      Minimum = 2000;
      EraseLen = 5;
      EditValue = QuestionValue->Value.date.Year;
      break;

    default:
      break;
    }
  } else if (Question->Operand == EFI_IFR_TIME_OP) {
    Step = 1;
    Minimum = 0;

    switch (MenuOption->Sequence) {
    case 0:
      Maximum = 23;
      EraseLen = 4;
      EditValue = QuestionValue->Value.time.Hour;
      break;

    case 1:
      Maximum = 59;
      EraseLen = 3;
      EditValue = QuestionValue->Value.time.Minute;
      break;

    case 2:
      Maximum = 59;
      EraseLen = 3;
      EditValue = QuestionValue->Value.time.Second;
      break;

    default:
      break;
    }
  } else {
    //
    // Numeric
    //
    EraseLen = gOptionBlockWidth;
    EditValue = QuestionValue->Value.u64;
    if (Maximum == 0) {
      Maximum = (UINT64) -1;
    }
  }

  if (gDirection == 0) {
    ManualInput = TRUE;
  } else {
    ManualInput = FALSE;
  }

  if ((Question->Operand == EFI_IFR_NUMERIC_OP) &&
      ((Question->Flags & EFI_IFR_DISPLAY) == EFI_IFR_DISPLAY_UINT_HEX)) {
    HexInput = TRUE;
  } else {
    HexInput = FALSE;
  }

  if (ManualInput) {
    if (HexInput) {
      InputWidth = Question->StorageWidth * 2;
      if (InputWidth > 16) {
        InputWidth = 16;
      }
    } else {
      switch (Question->StorageWidth) {
      case 1:
        InputWidth = 3;
        break;

      case 2:
        InputWidth = 5;
        break;

      case 4:
        InputWidth = 10;
        break;

      case 8:
        InputWidth = 20;
        break;

      default:
        InputWidth = 0;
        break;
      }
    }

    InputText[0] = LEFT_NUMERIC_DELIMITER;
    if (HexInput) {
      InputText[1] = L'0';
      InputText[2] = L'x';
      TextStartPtr = 3;
    } else {
      TextStartPtr = 1;
    }
    SetUnicodeMem (InputText + TextStartPtr, InputWidth, L' ');
    InputText[TextStartPtr + InputWidth] = RIGHT_NUMERIC_DELIMITER;
    InputText[TextStartPtr + InputWidth + 1] = L'\0';

    PrintAt (Column, Row, InputText);
    Column += TextStartPtr;
  }

  //
  // First time we enter this handler, we need to check to see if
  // we were passed an increment or decrement directive
  //
  do {
    Key.UnicodeChar = CHAR_NULL;
    if (gDirection != 0) {
      Key.ScanCode  = gDirection;
      gDirection    = 0;
      goto TheKey2;
    }

    Status = WaitForKeyStroke (&Key);

TheKey2:
    switch (Key.UnicodeChar) {

    case '+':
    case '-':
      if (Key.UnicodeChar == '+') {
        Key.ScanCode = SCAN_RIGHT;
      } else {
        Key.ScanCode = SCAN_LEFT;
      }
      Key.UnicodeChar = CHAR_NULL;
      goto TheKey2;

    case CHAR_NULL:
      switch (Key.ScanCode) {
      case SCAN_LEFT:
      case SCAN_RIGHT:
        if (DateOrTime) {
          //
          // By setting this value, we will return back to the caller.
          // We need to do this since an auto-refresh will destroy the adjustment
          // based on what the real-time-clock is showing.  So we always commit
          // upon changing the value.
          //
          gDirection = SCAN_DOWN;
        }

        if ((Step != 0) && !ManualInput) {
          if (Key.ScanCode == SCAN_LEFT) {
            if (EditValue >= Minimum + Step) {
              EditValue = EditValue - Step;
            } else if (EditValue > Minimum){
              EditValue = Minimum;
            } else {
              EditValue = Maximum;
            }
          } else if (Key.ScanCode == SCAN_RIGHT) {
            if (EditValue + Step <= Maximum) {
              EditValue = EditValue + Step;
            } else if (EditValue < Maximum) {
              EditValue = Maximum;
            } else {
              EditValue = Minimum;
            }
          }

          ZeroMem (FormattedNumber, 21 * sizeof (CHAR16));
          if (Question->Operand == EFI_IFR_DATE_OP) {
            if (MenuOption->Sequence == 2) {
              //
              // Year
              //
              UnicodeSPrint (FormattedNumber, 21 * sizeof (CHAR16), L"%04d", (UINTN) EditValue);
            } else {
              //
              // Month/Day
              //
              UnicodeSPrint (FormattedNumber, 21 * sizeof (CHAR16), L"%02d", (UINTN) EditValue);
            }

            if (MenuOption->Sequence == 0) {
              FormattedNumber[EraseLen - 2] = DATE_SEPARATOR;
            } else if (MenuOption->Sequence == 1) {
              FormattedNumber[EraseLen - 1] = DATE_SEPARATOR;
            }
          } else if (Question->Operand == EFI_IFR_TIME_OP) {
            UnicodeSPrint (FormattedNumber, 21 * sizeof (CHAR16), L"%02d", (UINTN) EditValue);

            if (MenuOption->Sequence == 0) {
              FormattedNumber[EraseLen - 2] = TIME_SEPARATOR;
            } else if (MenuOption->Sequence == 1) {
              FormattedNumber[EraseLen - 1] = TIME_SEPARATOR;
            }
          } else {
            QuestionValue->Value.u64 = EditValue;
            PrintFormattedNumber (Question, FormattedNumber, 21 * sizeof (CHAR16));
          }

          if (DateOrTime && gDeviceManagerSetup != NULL) {
            gST->ConOut->SetAttribute (gST->ConOut, SU_TEXT_HIGHLIGHT | SU_BACKGROUND_HIGHLIGHT);
          } else {
            gST->ConOut->SetAttribute (gST->ConOut, PcdGet8 (PcdBrowserFieldTextColor) | FIELD_BACKGROUND);
            for (Loop = 0; Loop < EraseLen; Loop++) {
              PrintAt (MenuOption->OptCol + Loop, MenuOption->Row, L" ");
            }
            gST->ConOut->SetAttribute (gST->ConOut, PcdGet8 (PcdBrowserFieldTextHighlightColor) | PcdGet8 (PcdBrowserFieldBackgroundHighlightColor));
          }

          if (MenuOption->Sequence == 0) {
            PrintCharAt (MenuOption->OptCol, Row, LEFT_NUMERIC_DELIMITER);
            Column = MenuOption->OptCol + 1;
          }

          PrintStringAt (Column, Row, FormattedNumber);

          if (!DateOrTime || MenuOption->Sequence == 2) {
            PrintChar (RIGHT_NUMERIC_DELIMITER);
          }

          goto EnterCarriageReturn;
        }
        break;

      case SCAN_UP:
      case SCAN_DOWN:
        goto EnterCarriageReturn;

      case SCAN_ESC:
        return EFI_DEVICE_ERROR;

      default:
        break;
      }

      break;

EnterCarriageReturn:

    case CHAR_CARRIAGE_RETURN:
      //
      // Store Edit value back to Question
      //
      if (Question->Operand == EFI_IFR_DATE_OP) {
        switch (MenuOption->Sequence) {
        case 0:
          QuestionValue->Value.date.Month = (UINT8) EditValue;
          break;

        case 1:
          QuestionValue->Value.date.Day = (UINT8) EditValue;
          break;

        case 2:
          QuestionValue->Value.date.Year = (UINT16) EditValue;
          break;

        default:
          break;
        }
      } else if (Question->Operand == EFI_IFR_TIME_OP) {
        switch (MenuOption->Sequence) {
        case 0:
          QuestionValue->Value.time.Hour = (UINT8) EditValue;
          break;

        case 1:
          QuestionValue->Value.time.Minute = (UINT8) EditValue;
          break;

        case 2:
          QuestionValue->Value.time.Second = (UINT8) EditValue;
          break;

        default:
          break;
        }
      } else {
        //
        // Numeric
        //
        if (EditValue < Minimum || EditValue > Maximum) {
          return EFI_ABORTED;
        }
        QuestionValue->Value.u64 = EditValue;
      }

      //
      // Check to see if the Value is something reasonable against consistency limitations.
      // If not, let's kick the error specified.
      //
      Status = ValidateQuestion (FormSet, Form, Question, EFI_HII_EXPRESSION_INCONSISTENT_IF);
      if (EFI_ERROR (Status)) {
        //
        // Input value is not valid, restore Question Value
        //
        GetQuestionValue (FormSet, Form, Question, TRUE);
      } else {
        SetQuestionValue (FormSet, Form, Question, TRUE);
        if (!DateOrTime || (Question->Storage != NULL)) {
          //
          // NV flag is unnecessary for RTC type of Date/Time
          //
          UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
        }
      }

      return Status;
      break;

    case CHAR_BACKSPACE:
      if (ManualInput) {
        if (Count == 0) {
          break;
        }
        //
        // Remove a character
        //
        EditValue = PreviousNumber[Count - 1];
        UpdateStatusBar (INPUT_ERROR, Question->QuestionFlags, FALSE);
        Count--;
        Column--;
        PrintAt (Column, Row, L" ");
      }
      break;

    default:
      if (ManualInput) {
        if (HexInput) {
          if (!IsHexDigit (&Digital, Key.UnicodeChar)) {
            UpdateStatusBar (INPUT_ERROR, Question->QuestionFlags, TRUE);
            break;
          }
        } else {
          if (Key.UnicodeChar > L'9' || Key.UnicodeChar < L'0') {
            UpdateStatusBar (INPUT_ERROR, Question->QuestionFlags, TRUE);
            break;
          }
        }

        //
        // If Count exceed input width, there is no way more is valid
        //
        if (Count >= InputWidth) {
          break;
        }
        //
        // Someone typed something valid!
        //
        if (Count != 0) {
          if (HexInput) {
            EditValue = LShiftU64 (EditValue, 4) + Digital;
          } else {
            //
            // EditValue = EditValue * 10 + (Key.UnicodeChar - L'0');
            //
            EditValue = LShiftU64 (EditValue, 3) + LShiftU64 (EditValue, 1) + (Key.UnicodeChar - L'0');
          }
        } else {
          if (HexInput) {
            EditValue = Digital;
          } else {
            EditValue = Key.UnicodeChar - L'0';
          }
        }

        if (EditValue > Maximum) {
          UpdateStatusBar (INPUT_ERROR, Question->QuestionFlags, TRUE);
          EditValue = PreviousNumber[Count];
          break;
        } else {
          UpdateStatusBar (INPUT_ERROR, Question->QuestionFlags, FALSE);
        }

        Count++;
        PreviousNumber[Count] = EditValue;

        PrintCharAt (Column, Row, Key.UnicodeChar);
        Column++;
      }
      break;
    }
  } while (TRUE);

  return EFI_SUCCESS;
}

/**
 Get selection for OneOf and OrderedList (Left/Right will be ignored).

 @param[in] Selection      Pointer to current selection.
 @param[in] MenuOption     Pointer to the current input menu.

 @retval EFI_SUCCESS       If Option input is processed successfully
 @retval EFI_DEVICE_ERROR  If operation fails
**/
EFI_STATUS
GetSelectionInputPopUp (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption
  )
{
  EFI_STATUS              Status;
  EFI_KEY_DATA            EfiKeyData;
  UINTN                   Index;
  CHAR16                  *StringPtr;
  CHAR16                  *TempStringPtr;
  UINTN                   Index2;
  UINTN                   TopOptionIndex;
  UINTN                   HighlightOptionIndex;
  UINTN                   Start;
  UINTN                   End;
  UINTN                   Top;
  UINTN                   Bottom;
  UINTN                   PopUpMenuLines;
  UINTN                   MenuLinesInView;
  UINTN                   PopUpWidth;
  CHAR16                  Character;
  INT32                   SavedAttribute;
  BOOLEAN                 ShowDownArrow;
  BOOLEAN                 ShowUpArrow;
  UINTN                   DimensionsWidth;
  UINTN                   DimensionsHeight;
  LIST_ENTRY              *Link;
  BOOLEAN                 OrderedList;
  UINT8                   *ValueArray;
  UINT8                   ValueType;
  EFI_HII_VALUE           HiiValue;
  EFI_HII_VALUE           *HiiValueArray;
  UINTN                   OptionCount;
  QUESTION_OPTION         *OneOfOption;
  QUESTION_OPTION         *CurrentOption;
  FORM_BROWSER_STATEMENT  *Question;
  INTN                    Result;
  UINTN                   X, Y;

  DimensionsWidth   = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight  = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  ValueArray        = NULL;
  ValueType         = 0;
  CurrentOption     = NULL;
  ShowDownArrow     = FALSE;
  ShowUpArrow       = FALSE;
  ZeroMem (&EfiKeyData, sizeof(EFI_KEY_DATA));

  StringPtr = NULL;

  Question = MenuOption->ThisTag;
  if (Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
    ValueArray = Question->BufferValue;
    ValueType = Question->ValueType;
    OrderedList = TRUE;
  } else {
    OrderedList = FALSE;
  }

  //
  // Calculate Option count
  //
  if (OrderedList) {
    for (Index = 0; Index < Question->MaxContainers; Index++) {
      if (GetArrayData (ValueArray, ValueType, Index) == 0) {
        break;
      }
    }

    OptionCount = Index;
  } else {
    OptionCount = 0;
    Link = GetFirstNode (&Question->OptionListHead);
    while (!IsNull (&Question->OptionListHead, Link)) {
      OneOfOption = QUESTION_OPTION_FROM_LINK (Link);

      OptionCount++;

      Link = GetNextNode (&Question->OptionListHead, Link);
    }
  }

  //
  // Prepare HiiValue array
  //
  HiiValueArray = AllocateZeroPool (OptionCount * sizeof (EFI_HII_VALUE));
  ASSERT (HiiValueArray != NULL);
  if (HiiValueArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Link = GetFirstNode (&Question->OptionListHead);
  for (Index = 0; Index < OptionCount; Index++) {
    if (OrderedList) {
      HiiValueArray[Index].Type = ValueType;
      HiiValueArray[Index].Value.u64 = GetArrayData (ValueArray, ValueType, Index);
    } else {
      OneOfOption = QUESTION_OPTION_FROM_LINK (Link);
      CopyMem (&HiiValueArray[Index], &OneOfOption->Value, sizeof (EFI_HII_VALUE));
      Link = GetNextNode (&Question->OptionListHead, Link);
    }
  }

  //
  // Move Suppressed Option to list tail
  //
  PopUpMenuLines = 0;
  for (Index = 0; Index < OptionCount; Index++) {
    OneOfOption = ValueToOption (Question, &HiiValueArray[OptionCount - Index - 1]);
    if (OneOfOption == NULL) {
      return EFI_NOT_FOUND;
    }

    RemoveEntryList (&OneOfOption->Link);

    if ((OneOfOption->SuppressExpression != NULL) &&
        (OneOfOption->SuppressExpression->Result.Value.b)) {
      //
      // This option is suppressed, insert to tail
      //
      InsertTailList (&Question->OptionListHead, &OneOfOption->Link);
    } else {
      //
      // Insert to head
      //
      InsertHeadList (&Question->OptionListHead, &OneOfOption->Link);

      PopUpMenuLines++;
    }
  }

  //
  // Get the number of one of options present and its size
  //
  PopUpWidth = 0;
  HighlightOptionIndex = 0;
  Link = GetFirstNode (&Question->OptionListHead);
  for (Index = 0; Index < PopUpMenuLines; Index++) {
    OneOfOption = QUESTION_OPTION_FROM_LINK (Link);

    StringPtr = GetToken (OneOfOption->Text, MenuOption->Handle);
    if (StringPtr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if (StrLen (StringPtr) > PopUpWidth) {
      PopUpWidth = StrLen (StringPtr);
    }
    gBS->FreePool (StringPtr);

    if (!OrderedList && (CompareHiiValue (&Question->HiiValue, &OneOfOption->Value, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
      //
      // Find current selected Option for OneOf
      //
      HighlightOptionIndex = Index;
    }

    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  //
  // Perform popup menu initialization.
  //
  PopUpWidth = PopUpWidth + POPUP_PAD_SPACE_COUNT;

  SavedAttribute = gST->ConOut->Mode->Attribute;
  gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);

  if ((PopUpWidth + POPUP_FRAME_WIDTH) > DimensionsWidth) {
    PopUpWidth = DimensionsWidth - POPUP_FRAME_WIDTH;
  }

  Start  = (DimensionsWidth - PopUpWidth - POPUP_FRAME_WIDTH) / 2 + gScreenDimensions.LeftColumn;
  End    = Start + PopUpWidth + POPUP_FRAME_WIDTH;
  Top    = gScreenDimensions.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT;
  Bottom = gScreenDimensions.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT - 1;

  MenuLinesInView = Bottom - Top - 1;
  if (MenuLinesInView >= PopUpMenuLines) {
    Top     = Top + (MenuLinesInView - PopUpMenuLines) / 2;
    Bottom  = Top + PopUpMenuLines + 1;
  } else {
    ShowDownArrow = TRUE;
  }

  if (HighlightOptionIndex > (MenuLinesInView - 1)) {
    TopOptionIndex = HighlightOptionIndex - MenuLinesInView + 1;
  } else {
    TopOptionIndex = 0;
  }

  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Y, &X);
  if ((End - Start + 1) > (Y / 4) ||
      (Bottom - Top + 1) > 5 ) {
    gPopUpDialogBiggerThanPlatformDialog = TRUE;
  }

  do {
    //
    // Clear that portion of the screen
    //
    ClearLines (Start, End, Top, Bottom, POPUP_TEXT | POPUP_BACKGROUND);

    //
    // Draw "One of" pop-up menu
    //
    Character = BOXDRAW_DOWN_RIGHT;
    PrintCharAt (Start, Top, Character);
    for (Index = Start; Index + 2 < End; Index++) {
      if ((ShowUpArrow) && ((Index + 1) == (Start + End) / 2)) {
        Character = GEOMETRICSHAPE_UP_TRIANGLE;
      } else {
        Character = BOXDRAW_HORIZONTAL;
      }

      PrintChar (Character);
    }

    Character = BOXDRAW_DOWN_LEFT;
    PrintChar (Character);
    Character = BOXDRAW_VERTICAL;
    for (Index = Top + 1; Index < Bottom; Index++) {
      PrintCharAt (Start, Index, Character);
      PrintCharAt (End - 1, Index, Character);
    }

    //
    // Move to top Option
    //
    Link = GetFirstNode (&Question->OptionListHead);
    for (Index = 0; Index < TopOptionIndex; Index++) {
      Link = GetNextNode (&Question->OptionListHead, Link);
    }

    //
    // Display the One of options
    //
    Index2 = Top + 1;
    for (Index = TopOptionIndex; (Index < PopUpMenuLines) && (Index2 < Bottom); Index++) {
      OneOfOption = QUESTION_OPTION_FROM_LINK (Link);
      Link = GetNextNode (&Question->OptionListHead, Link);

      StringPtr = GetToken (OneOfOption->Text, MenuOption->Handle);
      if (StringPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // If the string occupies multiple lines, truncate it to fit in one line,
      // and append a "..." for indication.
      //
      if (StrLen (StringPtr) > (PopUpWidth - 1)) {
        TempStringPtr = AllocateZeroPool (sizeof (CHAR16) * (PopUpWidth - 1));
        ASSERT ( TempStringPtr != NULL );
        if (TempStringPtr == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (TempStringPtr, StringPtr, (sizeof (CHAR16) * (PopUpWidth - 5)));
        gBS->FreePool (StringPtr);
        StringPtr = TempStringPtr;
        StrCat (StringPtr, L"...");
      }

      if (Index == HighlightOptionIndex) {
          //
          // Highlight the selected one
          //
          CurrentOption = OneOfOption;

          gST->ConOut->SetAttribute (gST->ConOut, PICKLIST_HIGHLIGHT_TEXT | PICKLIST_HIGHLIGHT_BACKGROUND);
          PrintStringAt (Start + 2, Index2, StringPtr);
          gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);
        } else {
          gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);
          PrintStringAt (Start + 2, Index2, StringPtr);
        }

      Index2++;
      gBS->FreePool (StringPtr);
    }

    Character = BOXDRAW_UP_RIGHT;
    PrintCharAt (Start, Bottom, Character);
    for (Index = Start; Index + 2 < End; Index++) {
      if ((ShowDownArrow) && ((Index + 1) == (Start + End) / 2)) {
        Character = GEOMETRICSHAPE_DOWN_TRIANGLE;
      } else {
        Character = BOXDRAW_HORIZONTAL;
      }

      PrintChar (Character);
    }

    Character = BOXDRAW_UP_LEFT;
    PrintChar (Character);

    //
    // Get User selection
    //
    EfiKeyData.Key.UnicodeChar = CHAR_NULL;
    if ((gDirection == SCAN_UP) || (gDirection == SCAN_DOWN)) {
      EfiKeyData.Key.ScanCode  = gDirection;
      gDirection    = 0;
      goto TheKey;
    }


    if (!InputDevice (gSetupMouse, &EfiKeyData, &X, &Y)) {
      if (EfiKeyData.Key.ScanCode != SCAN_ESC) {
        //
        // Left mouse button clicked
        //
        if ((X < (Start + 2)) || (X > (End - 2)) || (Y < (Top + 1)) || (Y > (Bottom - 1))) {
          continue;
        } else {
          if (HighlightOptionIndex == TopOptionIndex + (Y - Top - 1)) {
            EfiKeyData.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
          } else {
            HighlightOptionIndex = TopOptionIndex + (Y - Top - 1);
            continue;
          }
        }
      }
    }



TheKey:
    switch (EfiKeyData.Key.UnicodeChar) {
    case '+':
      if (OrderedList) {
        if ((TopOptionIndex > 0) && (TopOptionIndex == HighlightOptionIndex)) {
          //
          // Highlight reaches the top of the popup window, scroll one menu item.
          //
          TopOptionIndex--;
          ShowDownArrow = TRUE;
        }

        if (TopOptionIndex == 0) {
          ShowUpArrow = FALSE;
        }

        if (HighlightOptionIndex > 0) {
          HighlightOptionIndex--;
          ASSERT (CurrentOption == NULL);
          if (CurrentOption == NULL) {
            return EFI_LOAD_ERROR;
          }
          SwapListEntries (CurrentOption->Link.BackLink, &CurrentOption->Link);
        }
      }
      break;

    case '-':
      //
      // If an ordered list op-code, we will allow for a popup of +/- keys
      // to create an ordered list of items
      //
      if (OrderedList) {
        if (((TopOptionIndex + MenuLinesInView) < PopUpMenuLines) &&
            (HighlightOptionIndex == (TopOptionIndex + MenuLinesInView - 1))) {
          //
          // Highlight reaches the bottom of the popup window, scroll one menu item.
          //
          TopOptionIndex++;
          ShowUpArrow = TRUE;
        }

        if ((TopOptionIndex + MenuLinesInView) == PopUpMenuLines) {
          ShowDownArrow = FALSE;
        }

        if (HighlightOptionIndex < (PopUpMenuLines - 1)) {
          HighlightOptionIndex++;
          ASSERT (CurrentOption == NULL);
          if (CurrentOption == NULL) {
            return EFI_LOAD_ERROR;
          }
          SwapListEntries (&CurrentOption->Link, CurrentOption->Link.ForwardLink);
        }
      }
      break;

    case CHAR_NULL:
      switch (EfiKeyData.Key.ScanCode) {
      case SCAN_UP:
      case SCAN_DOWN:
        if (EfiKeyData.Key.ScanCode == SCAN_UP) {
          if ((TopOptionIndex > 0) && (TopOptionIndex == HighlightOptionIndex)) {
            //
            // Highlight reaches the top of the popup window, scroll one menu item.
            //
            TopOptionIndex--;
            ShowDownArrow = TRUE;
          }

          if (TopOptionIndex == 0) {
            ShowUpArrow = FALSE;
          }

          if (HighlightOptionIndex > 0) {
            HighlightOptionIndex--;
          }
        } else {
          if (((TopOptionIndex + MenuLinesInView) < PopUpMenuLines) &&
              (HighlightOptionIndex == (TopOptionIndex + MenuLinesInView - 1))) {
            //
            // Highlight reaches the bottom of the popup window, scroll one menu item.
            //
            TopOptionIndex++;
            ShowUpArrow = TRUE;
          }

          if ((TopOptionIndex + MenuLinesInView) == PopUpMenuLines) {
            ShowDownArrow = FALSE;
          }

          if (HighlightOptionIndex < (PopUpMenuLines - 1)) {
            HighlightOptionIndex++;
          }
        }
        break;

      case SCAN_ESC:
        gST->ConOut->SetAttribute (gST->ConOut, SavedAttribute);

        //
        // Restore link list order for orderedlist
        //
        if (OrderedList) {
          HiiValue.Type = ValueType;
          HiiValue.Value.u64 = 0;
          for (Index = 0; Index < Question->MaxContainers; Index++) {
            HiiValue.Value.u64 = GetArrayData (ValueArray, ValueType, Index);
            if (HiiValue.Value.u64 == 0) {
              break;
            }

            OneOfOption = ValueToOption (Question, &HiiValue);
            if (OneOfOption == NULL) {
              return EFI_NOT_FOUND;
            }

            RemoveEntryList (&OneOfOption->Link);
            InsertTailList (&Question->OptionListHead, &OneOfOption->Link);
          }
        }

        gBS->FreePool (HiiValueArray);
        return EFI_DEVICE_ERROR;

      default:
        break;
      }

      break;

    case CHAR_CARRIAGE_RETURN:
      //
      // return the current selection
      //
      if (OrderedList) {
        Index = 0;
        Link = GetFirstNode (&Question->OptionListHead);
        while (!IsNull (&Question->OptionListHead, Link)) {
          OneOfOption = QUESTION_OPTION_FROM_LINK (Link);

          SetArrayData (ValueArray, ValueType, Index, OneOfOption->Value.Value.u64);

          Index++;
          if (Index > Question->MaxContainers) {
            break;
          }

          Link = GetNextNode (&Question->OptionListHead, Link);
        }
      } else {
        CopyMem (&Question->HiiValue, &CurrentOption->Value, sizeof (EFI_HII_VALUE));
      }

      gST->ConOut->SetAttribute (gST->ConOut, SavedAttribute);
      gBS->FreePool (HiiValueArray);

      Status = ValidateQuestion (Selection->FormSet, Selection->Form, Question, EFI_HII_EXPRESSION_INCONSISTENT_IF);
      if (EFI_ERROR (Status)) {
        //
        // Input value is not valid, restore Question Value
        //
        GetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
      } else {
        SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
        UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
      }

      return Status;

    default:
      break;
    }
  } while (TRUE);

  //
  // Code will not reach here
  //
  return EFI_SUCCESS;
}

/**
 Wait for a key to be pressed by user.

 @param[out] Key         The key which is pressed by user.

 @retval EFI_SUCCESS     The function always completed successfully.
**/
EFI_STATUS
WaitForKeyStroke (
  OUT  EFI_INPUT_KEY           *Key
  )
{
  EFI_STATUS                            Status;
  EFI_KEY_DATA                          KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInEx;

  SimpleTextInEx = GetimpleTextInExInstance ();
  do {
    if (SimpleTextInEx != NULL) {
      UiWaitForSingleEvent (SimpleTextInEx->WaitForKeyEx, 0, 0);
      Status = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
      *Key = KeyData.Key;
    } else {
      UiWaitForSingleEvent (gST->ConIn->WaitForKey, 0, 0);
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
    }
  } while (EFI_ERROR(Status));

  return Status;
}

