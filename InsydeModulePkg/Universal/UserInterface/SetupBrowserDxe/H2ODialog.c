/** @file
 The H2O_DIALOG_PROTOCOL is the interface to the EFI
 Configuration Driver.  This will allow the caller to direct the
 configuration driver to use either the HII database or use the passed
 :   in packet of data.  This will also allow the caller to post messages
 :   into the configuration drivers internal mailbox.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include <Uefi.h>
#include "Ui.h"
#include "Setup.h"
#include "SetupUtility.h"
#include <Protocol/H2ODialog.h>
#include <Protocol/SetupMouse.h>
#include <Guid/UsbEnumeration.h>

#define HEX_VALUE_PREFIX_STRING_LENGTH   2

/**
 Install USB enumeration protocol to make sure USB keyboard is initialized.
**/
VOID
NotifyUsbKbInitial (
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   Handle;
  UINT8                        *Instance;

  Status = gBS->LocateProtocol (&gEfiUsbEnumerationGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    Handle = 0;
    gBS->InstallProtocolInterface (
           &Handle,
           &gEfiUsbEnumerationGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }
}

/**
 Split input string to multiple lines to follow that the width of each line can not exceed BlockWidth.

 @param[in]  Str                     String for splitting to multi lines
 @param[in]  BlockWidth              Width of the multi line after splitting
 @param[out] FormattedStr            Pointer of string array after splitting
 @param[out] FormattedStrCount       Total line number of string array
 @param[out] FormattedStrOneLineLen  String length of each line of string array

 @retval EFI_SUCCESS             Process successfully.
 @retval EFI_INVALID_PARAMETER   Input parameter invalid.
**/
EFI_STATUS
ProcessStrToFormatted (
  IN  CHAR16             *Str,
  IN  UINTN              BlockWidth,
  OUT CHAR16             **FormattedStr,
  OUT UINTN              *FormattedStrCount,
  OUT UINTN              *FormattedStrOneLineLen
  )
{
  UINTN                    Size;
  UINTN                    *LineInfoArray;
  UINTN                    *OldLineInfoArray;
  UINTN                    FormattedStrLen;
  UINTN                    CurrIndex;
  UINTN                    LineStartIndex;
  UINTN                    LineLastIndex;
  UINTN                    GlyphOffset;
  UINTN                    GlyphWidth;
  UINTN                    LineStartGlyphWidth;
  UINTN                    LineLastGlyphWidth;
  UINTN                    LineCount;
  UINT32                   Width;

  if (Str == NULL || BlockWidth == 0 || FormattedStr == NULL || FormattedStrCount == NULL || FormattedStrOneLineLen == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Every three elements of LineInfoArray form a screen-line of string:
  // LineInfoArray[i*3], LineInfoArray[i*3+1] separately represent the start and end indexes of each line.
  // LineInfoArray[i*3+2] stores the glyph width character to represent the start glyph width of each line.
  //
  Size = 0x20;
  LineInfoArray = AllocatePool (Size * sizeof (UINTN) * 3);
  if (LineInfoArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  GlyphOffset = 0;
  if (Str[0] == NARROW_CHAR || Str[0] == WIDE_CHAR) {
    LineStartGlyphWidth = ((Str[0] == WIDE_CHAR) ? 2 : 1);
  } else {
    LineStartGlyphWidth = 1;
  }
  GlyphWidth         = LineStartGlyphWidth;
  LineLastGlyphWidth = LineStartGlyphWidth;

  for (CurrIndex = 0, LineStartIndex = 0, LineLastIndex = 0, LineCount = 0;
       Str[CurrIndex] != CHAR_NULL;
       CurrIndex++) {
    if (LineCount == Size) {
      Size += 0x10;
      OldLineInfoArray = LineInfoArray;
      LineInfoArray = AllocatePool (Size * sizeof (UINTN) * 3);
      ASSERT (LineInfoArray != NULL);
      if (LineInfoArray == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (LineInfoArray, OldLineInfoArray, LineCount * sizeof (UINTN) * 3);
      gBS->FreePool (OldLineInfoArray);
    }

    switch (Str[CurrIndex]) {

    case NARROW_CHAR:
    case WIDE_CHAR:
      GlyphWidth = (Str[CurrIndex] == WIDE_CHAR) ? 2 : 1;
      break;

    //
    // char is '\n' which means to go to next line.
    //
    case CHAR_LINEFEED:
      LineInfoArray[LineCount * 3]     = LineStartIndex;
      LineInfoArray[LineCount * 3 + 1] = CurrIndex;
      LineInfoArray[LineCount * 3 + 2] = LineStartGlyphWidth;
      LineCount++;

      GlyphOffset = 0;
      LineStartGlyphWidth = GlyphWidth;
      LineStartIndex = CurrIndex + 1;
      break;

    //
    // char is '\r' which means to go to the start of current line.
    // (Here also to handle "\r\n" which means to go to next line.)
    //
    case CHAR_CARRIAGE_RETURN:
      if (Str[CurrIndex + 1] == CHAR_LINEFEED) {
        LineInfoArray[LineCount * 3]     = LineStartIndex;
        LineInfoArray[LineCount * 3 + 1] = CurrIndex;
        LineInfoArray[LineCount * 3 + 2] = LineStartGlyphWidth;
        LineCount++;
        CurrIndex++;
      }

      GlyphOffset = 0;
      LineStartGlyphWidth = GlyphWidth;
      LineStartIndex = CurrIndex + 1;
      break;

    default:
      GlyphOffset += GlyphWidth;
      if (GlyphWidth == 1) {
        //
        // If character belongs to wide glyph, set the right glyph offset.
        //
        Width = ConsoleLibGetGlyphWidth (Str[CurrIndex]);
        if (Width == 2) {
          GlyphOffset++;
        }
      }
      //
      // LineLastIndex: remember position of last space
      //
      if (CheckUnicode (Str[CurrIndex])) {
        //
        // Word is separated by space char, so record info for the temp end index of this line.
        //
        LineLastIndex      = CurrIndex;
        LineLastGlyphWidth = GlyphWidth;
      }

      if (GlyphOffset >= BlockWidth) {
        if (LineLastIndex > LineStartIndex && !CheckUnicode (Str[CurrIndex]) && !CheckUnicode (Str[CurrIndex + 1])) {
          //
          // LineLastIndex points to the tail of last word in current screen-line, restore it to CurrIndex.
          //
          CurrIndex  = LineLastIndex;
          GlyphWidth = LineLastGlyphWidth;
        } else if (GlyphOffset > BlockWidth) {
          //
          // The word is too long to fit one screen-line and we don't get the chance
          // of GlyphOffset == BlockWidth because GlyphWidth = 2
          //
          CurrIndex--;
        }

        //
        // [LineStartIndex, CurrIndex + 1) represent the range of current line.
        //
        LineInfoArray[LineCount * 3]     = LineStartIndex;
        LineInfoArray[LineCount * 3 + 1] = CurrIndex + 1;
        LineInfoArray[LineCount * 3 + 2] = LineStartGlyphWidth;
        LineStartGlyphWidth = GlyphWidth;
        LineCount++;

        LineStartIndex = CurrIndex + 1;
        GlyphOffset = 0;
      }
      break;
    }
  }

  if (GlyphOffset > 0) {
    LineInfoArray[LineCount * 3]     = LineStartIndex;
    LineInfoArray[LineCount * 3 + 1] = CurrIndex;
    LineInfoArray[LineCount * 3 + 2] = LineStartGlyphWidth;
    LineCount++;
  }

  if (LineCount == 0) {
    LineInfoArray[0] = 0;
    LineInfoArray[1] = 1;
    LineInfoArray[2] = 1;
    LineCount++;
  }

  *FormattedStrCount = LineCount;
  *FormattedStrOneLineLen = (BlockWidth + 2) * 2;
  *FormattedStr = AllocateZeroPool (LineCount * (*FormattedStrOneLineLen) * sizeof (CHAR16));
  if (*FormattedStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Each line of formatted string sequentially contains glyth char, string and null char.
  //
  for (CurrIndex = 0; CurrIndex < LineCount; CurrIndex ++) {
    *(*FormattedStr + CurrIndex * (*FormattedStrOneLineLen)) = (LineInfoArray[CurrIndex * 3 + 2] == 2) ? WIDE_CHAR : NARROW_CHAR;

    StrnCpy (
      *FormattedStr + CurrIndex * (*FormattedStrOneLineLen) + 1,
      Str + LineInfoArray[CurrIndex * 3],
      LineInfoArray[CurrIndex * 3 + 1] - LineInfoArray[CurrIndex * 3]
      );

    FormattedStrLen = StrLen (&(*FormattedStr)[CurrIndex * (*FormattedStrOneLineLen)]);
    (*FormattedStr)[CurrIndex * (*FormattedStrOneLineLen) + FormattedStrLen] = CHAR_NULL;
  }

  gBS->FreePool (LineInfoArray);
  return EFI_SUCCESS;
}

/**
 Display Dialog for User Requirement

 @param[in] ShowString             Dialog question string
 @param[in] DialogOperator         Dialog type (Yes/No, Yes/No/Cancel, Ok or Ok/Cancel)
 @param[in] Color                  Dialog color

 @return Returns the value which stands for user selection
**/
UINTN
DisplayDialog (
  IN  CHAR16                    *ShowString,
  IN  UI_DIALOG_OPERATION       DialogOperator,
  IN  UINTN                     Color
  )
{
  UINTN                                       Columns;
  UINTN                                       Rows;
  UINTN                                       Index;
  UINTN                                       DlgNum;
  UINTN                                       RangeNum;
  UINTN                                       Select;
  CHAR16                                      *ResStr[10];
  BOOLEAN                                     LoopEnable;
  BOOLEAN                                     ExitEnable;
  EFI_KEY_DATA                                EfiKeyData;
  CHAR16                                      *CleanLine;
  BOOLEAN                                     ReDraw;
  UINTN                                       X;
  UINTN                                       Y;
  UINTN                                       StringLength;
  UINTN                                       StartX[3];
  CHAR16                                      *OkString;
  CHAR16                                      *CancelString;
  CHAR16                                      *YesString;
  CHAR16                                      *NoString;
  CHAR16                                      *String;
  CHAR16                                      *InfoStr;
  UINTN                                       InfoStrHeight;
  UINTN                                       InfoStrOneLineLen;
  UINTN                                       InputStrHeight;
  UINTN                                       OneLineStrWidth;
  UINTN                                       DialogWidth;
  UINTN                                       DialogHeight;
  UINTN                                       DialogTop;
  UINTN                                       DialogMiddle;
  UINTN                                       DialogBottom;
  UINTN                                       DialogLeft;
  UINTN                                       DialogRight;
  INT32                                       BackupAttribute;

  NotifyUsbKbInitial ();

  ReDraw = FALSE;
  DlgNum = 0;

  //
  // Disable cursor
  //
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);

  OkString     = GetToken (STRING_TOKEN(SCU_STR_OK_TEXT)    , gHiiHandle);
  CancelString = GetToken (STRING_TOKEN(SCU_STR_CANCEL_TEXT), gHiiHandle);
  YesString    = GetToken (STRING_TOKEN(SCU_STR_YES_TEXT)   , gHiiHandle);
  NoString     = GetToken (STRING_TOKEN(SCU_STR_NO_TEXT)    , gHiiHandle);

  switch (DialogOperator) {

  case DlgYesNo:
    ResStr[0] = YesString;
    ResStr[1] = NoString;
    DlgNum = 2;
    break;

  case DlgYesNoCancel:
    ResStr[0] = YesString;
    ResStr[1] = NoString;
    ResStr[2] = CancelString;
    DlgNum = 3;
    break;

  case DlgOk:
    ResStr[0] = OkString;
    DlgNum = 1;
    break;

  case DlgOkCancel:
    ResStr[0] = OkString;
    ResStr[1] = CancelString;
    DlgNum = 2;
    break;
  }

  if (DialogOperator != DlgOk) {
    String = AllocateCopyPool (StrSize(ShowString) + sizeof(CHAR16), ShowString);
    StrCat (String, L"?");
  } else {
    String = ShowString;
  }
  StringLength = GetStringWidth (String) / 2 - 1;

  //
  // Make sure the dialog width between 25% ~ 66% of screen width.
  //
  if (StringLength > Columns * 2 / 3) {
    OneLineStrWidth = Columns * 2 / 3;
  } else if (StringLength < Columns / 4) {
    OneLineStrWidth = Columns / 4;
  } else {
    OneLineStrWidth = StringLength;
  }

  //
  // Make sure the dialog width is bigger than the total string length of selections.
  //
  StringLength = 0;
  for (Index = 0; Index < DlgNum; Index++) {
    StringLength += (GetStringWidth(ResStr[Index])/2 - 1) + 2;
  }

  if (OneLineStrWidth < StringLength) {
    OneLineStrWidth = StringLength + DlgNum + 1;
    RangeNum = 1;
  } else {
    RangeNum = (OneLineStrWidth - StringLength) / (DlgNum + 1);
  }
  DialogWidth = OneLineStrWidth + 4;

  ProcessStrToFormatted (String, OneLineStrWidth, &InfoStr, &InfoStrHeight, &InfoStrOneLineLen);

  InputStrHeight = 1;
  DialogHeight = InfoStrHeight + InputStrHeight + 3;

  DialogLeft   = (Columns / 2) - (DialogWidth / 2);
  DialogRight  = DialogLeft + DialogWidth - 1;
  DialogTop    = (Rows / 2) - (DialogHeight / 2);
  DialogMiddle = DialogTop + InfoStrHeight + 1;
  DialogBottom = DialogMiddle + InputStrHeight + 1;

  gBS->AllocatePool (EfiBootServicesData, (DialogWidth + 1) * sizeof (CHAR16), (VOID **)&CleanLine);

  for (Index = 0; Index < DialogWidth; Index++) {
    CleanLine[Index] = CHAR_SPACE;
  }
  CleanLine[DialogWidth] = CHAR_NULL;

  BackupAttribute = gST->ConOut->Mode->Attribute;
  if (Color == 0) {
    Color = EFI_TEXT_ATTR (EFI_BLUE, EFI_MAGENTA);
  }
  gST->ConOut->SetAttribute (gST->ConOut, Color);


  for (Index = DialogTop; Index <= DialogBottom; Index++) {
    PrintAt (DialogLeft, Index, L"%s", CleanLine);
  }

  //
  // Print  border line
  // +-------------------------------------------+
  // |                                           |
  // |                                           |
  // |                                           |
  // |-------------------------------------------|
  // |                                           |
  // +-------------------------------------------+
  //
  PrintAt (DialogLeft, DialogTop   , L"%c", BOXDRAW_DOWN_RIGHT);
  PrintAt (DialogLeft, DialogBottom, L"%c", BOXDRAW_UP_RIGHT);

  for (Index = DialogLeft + 1; Index < DialogRight; Index++) {
    PrintAt (Index, DialogTop   , L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, DialogMiddle, L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, DialogBottom, L"%c", BOXDRAW_HORIZONTAL);
  }

  PrintAt (DialogRight, DialogTop   , L"%c", BOXDRAW_DOWN_LEFT);
  PrintAt (DialogRight, DialogBottom, L"%c", BOXDRAW_UP_LEFT);

  for (Index = DialogTop + 1; Index < DialogBottom; Index++) {
    PrintAt (DialogLeft , Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (DialogRight, Index,L"%c", BOXDRAW_VERTICAL);
  }

  PrintAt (DialogLeft , DialogMiddle, L"%c", BOXDRAW_VERTICAL_RIGHT);
  PrintAt (DialogRight, DialogMiddle, L"%c", BOXDRAW_VERTICAL_LEFT);

  for (Index = 0; Index < InfoStrHeight; Index++) {
    PrintAt (DialogLeft + 2, DialogTop + 1 + Index, L"%s", &InfoStr[Index * InfoStrOneLineLen]);
  }

  StartX[0] = DialogLeft + 2 + RangeNum + (((OneLineStrWidth - StringLength) % (DlgNum + 1)) / 2);

  for (Index = 1; Index < DlgNum; Index++) {
    StartX[Index] = StartX[Index - 1] + (GetStringWidth(ResStr[Index - 1])/2 - 1) + 2 + RangeNum;
  }

  LoopEnable = TRUE;
  ExitEnable = TRUE;
  Select = 1;
  ZeroMem (&EfiKeyData, sizeof(EFI_KEY_DATA));

  while (LoopEnable){
    gST->ConOut->SetAttribute (gST->ConOut, Color);
    for (Index = 1; Index <= DlgNum; Index++) {
      PrintAt (StartX[Index - 1], DialogMiddle + 1, L"[%s]", ResStr[Index - 1]);
    }

    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
    PrintAt (StartX[Select - 1], DialogMiddle + 1, L"[%s]", ResStr[Select - 1]);
    gST->ConOut->SetAttribute (gST->ConOut, Color);

    while (1) {
      if (ReDraw == TRUE) {
        ReDraw = FALSE;
        break;
      }

      if (!UiInputDevice(0, &EfiKeyData, &X, &Y)) {
        if (EfiKeyData.Key.ScanCode == SCAN_ESC)
          break;

        if (Y != DialogMiddle + 1)
          continue;

        switch (DlgNum) {

        case 1:
          StringLength = (GetStringWidth (ResStr[0]) / 2 - 1) + 2;
          if ((X < StartX[0]) || (X >= (StartX[0] + StringLength))) {
            continue;
          }
          break;

        case 2:
          StringLength = (GetStringWidth (ResStr[0]) / 2 - 1) + 2;
          if ((X >= StartX[0]) && (X < (StartX[0] + StringLength))) {
            Select = 1;
            break;
          }

          StringLength = (GetStringWidth (ResStr[1]) / 2 - 1) + 2;
          if ((X < StartX[1]) || (X >= (StartX[1] + StringLength))) {
            continue;
          }
          Select = 2;
          break;

        case 3:
          StringLength = (GetStringWidth (ResStr[0]) / 2 - 1) + 2;
          if ((X >= StartX[0]) && (X < (StartX[0] + StringLength))) {
            Select = 1;
            break;
          }

          StringLength = (GetStringWidth (ResStr[1]) / 2 - 1) + 2;
          if ((X >= StartX[1]) && (X < (StartX[1] + StringLength))) {
            Select = 2;
            break;
          }

          StringLength = (GetStringWidth (ResStr[2]) / 2 - 1) + 2;
          if ((X < StartX[2]) || (X >= (StartX[2] + StringLength))) {
              continue;
          }
          Select = 3;
          break;

        default:
          continue;
        }
        ReDraw = TRUE;
      }

      break;
    }

    if (ReDraw == TRUE) {
      continue;
    }

    switch (EfiKeyData.Key.UnicodeChar) {

    case CHAR_CARRIAGE_RETURN:
       LoopEnable = FALSE;
       break;

    case CHAR_NULL:
      if (EfiKeyData.Key.ScanCode == SCAN_LEFT) {
        if (Select > 1) {
          Select--;
        }
      } else if (EfiKeyData.Key.ScanCode == SCAN_RIGHT) {
        if (Select < DlgNum) {
          Select++;
        }
      } else if (EfiKeyData.Key.ScanCode == SCAN_ESC) {
        LoopEnable = FALSE;
        Select = 0;
      }
      break;
    }
  }

  //
  //Clean Dialog
  //
  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);

  for (Index = DialogTop; Index <= DialogBottom; Index++) {
    PrintAt (DialogLeft, Index, L"%s", CleanLine);
  }

  if (gDeviceManagerSetup != NULL) {
    for (Index = DialogTop; Index <= DialogBottom; Index++) {
      PrintAt (gPromptBlockWidth + gOptionBlockWidth + 1, Index, L"%c", BOXDRAW_VERTICAL);
    }
  }

  gBS->FreePool (OkString);
  gBS->FreePool (CancelString);
  gBS->FreePool (YesString);
  gBS->FreePool (NoString);
  gBS->FreePool (CleanLine);
  gBS->FreePool (InfoStr);
  if (String != ShowString) {
    gBS->FreePool (String);
  }

  return Select;
}

/**
 Display Dialog for User Requirement

 @param[in]  NumberOfLines          Dialog type (Yes/No, Yes/No/Cancel, Ok or Ok/Cancel)
 @param[in]  HotKey
 @param[in]  MaximumStringSize
 @param[out] StringBuffer
 @param[out] KeyValue               Output key value which stand for user selection
 @param[in]  String                 Dialog question string

 @retval EFI_SUCCESS                Process successfully.
**/
EFI_STATUS
EFIAPI
ConfirmDialog (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  )
{
  EFI_STATUS                          Status;
  BOOLEAN                             LocateMouseFlag;

  LocateMouseFlag = FALSE;
  //
  // We only focus on no locate SetupMouse state,
  // SCU or SecureManager will decide whether call Start()/Stop() by themself.
  //
  if (gSetupMouse == NULL) {
    Status = gBS->LocateProtocol (
                    &gSetupMouseProtocolGuid,
                    NULL,
                    (VOID **)&gSetupMouse
                    );
    if (!EFI_ERROR (Status)) {
      LocateMouseFlag = TRUE;
      gSetupMouse->Start (gSetupMouse);
    }
  }

  if (DisplayDialog (String, NumberOfLines, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE)) == 1) {
    KeyValue->UnicodeChar = CHAR_CARRIAGE_RETURN;
  } else {
    KeyValue->UnicodeChar = CHAR_NULL;
    KeyValue->ScanCode = SCAN_ESC;
  }

  if (LocateMouseFlag) {
    gSetupMouse->Close(gSetupMouse);
    gSetupMouse = NULL;
  }

  return EFI_SUCCESS;
}

/**
 Draw password dialog and wait user press password.

 @param[in]  NumberOfLines
 @param[in]  HotKey
 @param[in]  MaximumStringSize      Maximum string length.
 @param[out] UserInputStringBuffer  User input string buffer.
 @param[out] KeyValue               Report last user input key.
 @param[in]  TitleString            The password dialog title string.

 @retval EFI_SUCCESS                Alwasy return.
 @retval otherwise                  Display dialog fail because allocate memory fail.
**/
EFI_STATUS
EFIAPI
PasswordDialog (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *UserInputStringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *TitleString,
  ...
  )
{
  UINTN                                       Columns;
  UINTN                                       Rows;
  UINTN                                       Color;
  UINTN                                       MouX;
  UINTN                                       MouY;
  UINTN                                       CursorX;
  UINTN                                       CursorY;
  UINTN                                       Index;
  BOOLEAN                                     LoopEnable;
  CHAR16                                      *CleanLine;
  EFI_KEY_DATA                                EfiKeyData;
  BOOLEAN                                     LocateMouseFlag;
  EFI_SETUP_MOUSE_PROTOCOL                    *LocalSetupMouse ;
  EFI_STATUS                                  Status;
  CHAR16                                      *InfoStr;
  UINTN                                       InfoStrHeight;
  UINTN                                       InfoStrOneLineLen;
  UINTN                                       InputStrHeight;
  UINTN                                       OneLineStrWidth;
  UINTN                                       DialogWidth;
  UINTN                                       DialogHeight;
  UINTN                                       DialogTop;
  UINTN                                       DialogMiddle;
  UINTN                                       DialogBottom;
  UINTN                                       DialogLeft;
  UINTN                                       DialogRight;
  UINTN                                       StringLength;
  INT32                                       BackupAttribute;
  KEYBOARD_ATTRIBUTES                         KeyboardAttributes;

  LocalSetupMouse = NULL;
  MouX = 0;
  MouY = 0;
  Status = EFI_SUCCESS;
  LocateMouseFlag = FALSE;

  NotifyUsbKbInitial ();

  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);

  if ((MaximumStringSize == 0) || (UserInputStringBuffer == NULL)) {
    return EFI_SUCCESS;
  }

  if (gDeviceManagerSetup == NULL) {
    LocateMouseFlag = TRUE;
  } else if (!(gDeviceManagerSetup->SetupMouseFlag)) {
    LocateMouseFlag = TRUE;
  } else if (gDeviceManagerSetup->SetupMouse != NULL) {
    LocalSetupMouse = (EFI_SETUP_MOUSE_PROTOCOL *) gDeviceManagerSetup->SetupMouse;
    LocateMouseFlag = FALSE;
  }

  if (LocateMouseFlag) {
    Status = gBS->LocateProtocol (
                    &gSetupMouseProtocolGuid,
                    NULL,
                    (VOID **)&LocalSetupMouse
                    );
    if (!EFI_ERROR (Status)) {
      Status = LocalSetupMouse->Start (LocalSetupMouse);
      if (EFI_ERROR (Status)) {
        LocalSetupMouse = NULL;
        LocateMouseFlag = FALSE;
      }
    } else {
      LocateMouseFlag = FALSE;
    }
  }

  for (Index = 0; Index < MaximumStringSize; Index++) {
     UserInputStringBuffer[Index] = 0;
  }

  StringLength = MaximumStringSize > (GetStringWidth (TitleString) / 2 - 1)?MaximumStringSize:(GetStringWidth (TitleString) / 2 - 1);

  //
  // Make sure the dialog width between 25% ~ 66% of screen width.
  //
  if (StringLength > Columns * 2 / 3) {
    OneLineStrWidth = Columns * 2 / 3;
  } else if (StringLength < Columns / 4) {
    OneLineStrWidth = Columns / 4;
  } else {
    OneLineStrWidth = StringLength;
  }

  DialogWidth = OneLineStrWidth + 4;

  Status = ProcessStrToFormatted (TitleString, OneLineStrWidth, &InfoStr, &InfoStrHeight, &InfoStrOneLineLen);
  if (EFI_ERROR (Status)) {
    InfoStr = NULL;
    InfoStrHeight = 0;
  }

  InputStrHeight = 1;
  DialogHeight = InfoStrHeight + InputStrHeight + 3;

  Status = gBS->AllocatePool (EfiBootServicesData, (DialogWidth + 1) * sizeof (CHAR16), (VOID **)&CleanLine);
  if (EFI_ERROR (Status)) {
    if (InfoStr != NULL) {
      gBS->FreePool (InfoStr);
    }
    return Status;
  }

  for (Index = 0; Index < DialogWidth; Index++) {
    CleanLine[Index] = CHAR_SPACE;
  }
  CleanLine[DialogWidth] = 0;
  BackupAttribute = gST->ConOut->Mode->Attribute;
  Color = EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE);
  gST->ConOut->SetAttribute (gST->ConOut, Color);

  DialogLeft   = (Columns / 2) - (DialogWidth / 2);
  DialogRight  = DialogLeft + DialogWidth - 1;
  DialogTop    = (Rows / 2) - (DialogHeight / 2);
  DialogMiddle = DialogTop + InfoStrHeight + 1;
  DialogBottom = DialogMiddle + InputStrHeight + 1;

  for (Index = DialogTop; Index <= DialogBottom; Index++) {
    PrintAt (DialogLeft, Index, L"%s", CleanLine);
  }

  PrintAt (DialogLeft, DialogTop   , L"%c", BOXDRAW_DOWN_RIGHT);
  PrintAt (DialogLeft, DialogBottom, L"%c", BOXDRAW_UP_RIGHT);

  for (Index = DialogLeft + 1; Index < DialogRight; Index++) {
    PrintAt (Index, DialogTop   , L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, DialogMiddle, L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, DialogBottom, L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = DialogTop + 1; Index < DialogBottom; Index++) {
    PrintAt (DialogLeft , Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (DialogRight, Index,L"%c", BOXDRAW_VERTICAL);
  }

  PrintAt (DialogRight, DialogTop   , L"%c", BOXDRAW_DOWN_LEFT);
  PrintAt (DialogRight, DialogBottom, L"%c", BOXDRAW_UP_LEFT);
  PrintAt (DialogLeft , DialogMiddle, L"%c", BOXDRAW_VERTICAL_RIGHT);
  PrintAt (DialogRight, DialogMiddle, L"%c", BOXDRAW_VERTICAL_LEFT);

  for (Index = 0; Index < InfoStrHeight; Index++) {
    PrintAt (DialogLeft + 2, DialogTop + 1 + Index, L"%s", &InfoStr[Index * InfoStrOneLineLen]);
  }

  LoopEnable = TRUE;
  ZeroMem (&EfiKeyData, sizeof(EFI_KEY_DATA));
  CursorX = DialogLeft + 2;
  CursorY = DialogMiddle + 1;
  PrintAt (CursorX - 1, CursorY,L" ");

  Index = 0;
  do {
    if ((!InputDevice(LocalSetupMouse, &EfiKeyData, &MouX, &MouY) && LocateMouseFlag)) {
      LocalSetupMouse->GetKeyboardAttributes (LocalSetupMouse, &KeyboardAttributes);
      if (!KeyboardAttributes.IsStart) {
        if (EfiKeyData.Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
          continue;
        }
        LocalSetupMouse->StartKeyboard(LocalSetupMouse, 2, gScreenDimensions.BottomRow);
        gST->ConOut->SetAttribute (gST->ConOut, Color);
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorX, CursorY);
        continue;
      }
      ZeroMem (&EfiKeyData, sizeof(EFI_KEY_DATA));
      gST->ConOut->SetAttribute (gST->ConOut, Color);
      gST->ConOut->SetCursorPosition(gST->ConOut, CursorX, CursorY);
      if (Status != EFI_SUCCESS) {
        continue;
      }
    }

    OemSvcPasswordInputFilter (&EfiKeyData);
    if (EfiKeyData.Key.ScanCode == SCAN_NULL && EfiKeyData.Key.UnicodeChar == CHAR_NULL) {
      continue;
    }

    switch (EfiKeyData.Key.UnicodeChar) {

    case CHAR_CARRIAGE_RETURN:
      LoopEnable = FALSE;
      break;

    case CHAR_BACKSPACE:
      if (Index == 0) {
        PrintAt (CursorX - 1, CursorY, L" ");
        break;
      }

      if (Index > OneLineStrWidth) {
        PrintAt (CursorX + OneLineStrWidth - 1, CursorY, L" ");
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorX + OneLineStrWidth - 1, CursorY);
      } else {
        PrintAt (CursorX + Index - 1, CursorY, L" ");
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorX + Index - 1, CursorY);
      }

      Index--;
      UserInputStringBuffer[Index] = 0;
      break;

    case CHAR_NULL:
    case CHAR_TAB:
      continue;

    default:
      UserInputStringBuffer[Index] = EfiKeyData.Key.UnicodeChar;
      Index++;

      if (Index == MaximumStringSize) {
        Index--;
        UserInputStringBuffer[Index] = 0;
      } else {
        if (Index == MaximumStringSize - 1) {
          if (Index < OneLineStrWidth) {
            PrintAt (CursorX + Index - 1, CursorY, L"*");
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorX + Index - 1, CursorY);
          } else {
            PrintAt (CursorX + OneLineStrWidth - 1, CursorY, L"*");
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorX + OneLineStrWidth - 1, CursorY);
          }
        } else {
          if (Index < OneLineStrWidth) {
            PrintAt (CursorX + Index - 1, CursorY, L"*");
          } else {
            PrintAt (CursorX + OneLineStrWidth - 1, CursorY, L" ");
            gST->ConOut->SetCursorPosition (gST->ConOut, CursorX + OneLineStrWidth - 1, CursorY);
          }
        }
      }
      break;
    }
  } while (LoopEnable);

  //
  // Clear Screen
  //
  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  gST->ConOut->ClearScreen (gST->ConOut);
  if ((LocalSetupMouse != NULL) && LocateMouseFlag) {
    LocalSetupMouse->Close (LocalSetupMouse);
  }
  if (InfoStr != NULL) {
    gBS->FreePool (InfoStr);
  }
  gBS->FreePool (CleanLine);
  return EFI_SUCCESS;
}

/**
 Draw select dialog and wait user select item.

 @param[in]  NumberOfLines          Number of display item.
 @param[in]  HotKey                 If TRUE need check assign key list.
                                    If FALSE use function default key event.
 @param[in]  KeyList                Assign key list.
 @param[out] EventKey
 @param[in]  MaximumStringSize      Maximum string length.
 @param[in]  TitleString            The dialog title string.
 @param[out] SelectIndex            The select index number.
 @param[in]  String                 The address array of string.
 @param[in]  Color                  Set display color.

 @retval EFI_SUCCESS                Alwasy return.
 @retval otherwise                  Display dialog fail because allocate memory fail.
**/
EFI_STATUS
EFIAPI
OneOfOptionDialog (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  CONST EFI_INPUT_KEY             *KeyList,
  OUT EFI_INPUT_KEY                   *EventKey,
  IN  UINT32                          MaximumStringSize,
  IN  CHAR16                          *TitleString,
  OUT UINT32                          *SelectIndex,
  IN  CHAR16                          **String,
  IN  UINT32                          Color
  )
{
  UINTN                                       Columns;
  UINTN                                       Rows;
  UINTN                                       Index;
  UINTN                                       StrLenth;
  UINTN                                       Item;
  UINTN                                       MouX;
  UINTN                                       MouY;
  UINTN                                       StartActX;
  UINTN                                       StartActY;
  UINTN                                       EndActX;
  UINTN                                       EndActY;
  BOOLEAN                                     AuxActive;
  BOOLEAN                                     LoopEnable;
  BOOLEAN                                     LocalHotkey;
  CHAR16                                      *CleanLine;
  CHAR16                                      *SelectIndexLin;
  EFI_KEY_DATA                                EfiKeyData;
  EFI_SETUP_MOUSE_PROTOCOL                    *LocalSetupMouse;
  EFI_STATUS                                  Status;
  BOOLEAN                                     LocateMouseFlag;
  INT32                                       BackupAttribute;

  AuxActive = FALSE;
  MouX = 0;
  MouY = 0;
  StartActX = 0;
  StartActY = 0;
  EndActX = 0;
  EndActY = 0;
  Status = EFI_SUCCESS;
  LocalSetupMouse = NULL;
  LocateMouseFlag = FALSE;

  ASSERT (EventKey != NULL);
  if (EventKey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NotifyUsbKbInitial ();

  if (gDeviceManagerSetup == NULL) {
    LocateMouseFlag = TRUE;
  } else if (!(gDeviceManagerSetup->SetupMouseFlag)) {
    LocateMouseFlag = TRUE;
  } else if (gDeviceManagerSetup->SetupMouse != NULL) {
    LocalSetupMouse = (EFI_SETUP_MOUSE_PROTOCOL *) gDeviceManagerSetup->SetupMouse;
    LocateMouseFlag = FALSE;
  }

  if (LocateMouseFlag) {
    Status = gBS->LocateProtocol (&gSetupMouseProtocolGuid,
                                  NULL,
                                  (VOID **)&LocalSetupMouse
                                  );
    if (!EFI_ERROR (Status)) {
      Status = LocalSetupMouse->Start (LocalSetupMouse);
      if (EFI_ERROR (Status)) {
        LocalSetupMouse = NULL;
        LocateMouseFlag = FALSE;
      }
    } else {
      LocateMouseFlag = FALSE;
    }
  }

  LocalHotkey = HotKey;
  if (HotKey) {
    if (KeyList == NULL) {
      LocalHotkey = FALSE;
    } else if ((KeyList[0].ScanCode == SCAN_NULL) &&
           (KeyList[0].UnicodeChar == CHAR_NULL)) {
      LocalHotkey = FALSE;
    }
  }
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);

  BackupAttribute = gST->ConOut->Mode->Attribute;
  if (Color == 0) {
    Color = EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE);
    gST->ConOut->SetAttribute (gST->ConOut, Color);
  }

  Item = NumberOfLines;

  StrLenth = MaximumStringSize;
  for (Index = 0; Index < NumberOfLines; Index++) {
    StrLenth = MAX (StrLenth, StrLen(String[Index]));
  }
  StrLenth = MAX (StrLenth, StrLen(TitleString));

  if (StrLenth < (Columns / 4)) {
    StrLenth = Columns / 4;
  }

  StrLenth = StrLenth / 2;
  Status = gBS->AllocatePool (EfiBootServicesData, ((StrLenth + 2) * 2 + 2) * sizeof (CHAR16), (VOID **)&CleanLine);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->AllocatePool (EfiBootServicesData, ((StrLenth + 2) * 2 + 2) * sizeof (CHAR16), (VOID **)&SelectIndexLin);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (CleanLine);
    return Status;
  }

  for (Index = 0; Index < (StrLenth + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
    SelectIndexLin[Index] = 0x20;
  }

  CleanLine[(StrLenth + 2) * 2 + 1] = 0x0000;
  SelectIndexLin[MaximumStringSize + 1] = 0x0000;

  Item = Item / 2;
  for (Index = Rows/2 - (Item + 3); Index <= Rows/2 - (Item + 3) + 3 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%s", CleanLine);
  }

  //
  // Print  border line
  // +-------------------------------------------+
  // |                                           |
  // +-------------------------------------------+
  // |                                           |
  // |                                           |
  // |                                           |
  // |                                           |
  // +-------------------------------------------+
  //

  for (Index = Columns/2 - (StrLenth + 1); Index < Columns/2 + (StrLenth + 2) ; Index++) {
    PrintAt (Index, Rows/2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 3) + 3 + NumberOfLines, L"%c", BOXDRAW_HORIZONTAL);
  }

  for (Index = Rows/2 - (Item + 2); Index < Rows/2 - (Item + 3) + 3 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (StrLenth + 2), Index,L"%c", BOXDRAW_VERTICAL);
  }

  PrintAt (Columns/2 + (StrLenth + 2) ,Rows/2 - (Item + 3) + 3 + NumberOfLines, L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns/2 + (StrLenth + 2) ,Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);

  PrintAt (Columns/2 - (StrLenth + 2), Rows/2 - (Item + 3) + 3 + NumberOfLines, L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns/2 - (StrLenth + 2), Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  //
  // Print Title
  //
  PrintAt (Columns/2 - (StrLen(TitleString)/2), Rows/2 - (Item + 2), L"%s", TitleString);

  if ((*SelectIndex) >= NumberOfLines) {
    (*SelectIndex) = 0;
  }

  LoopEnable = TRUE;
  do {
    for (Index = 0; Index < NumberOfLines; Index++) {
       CopyMem (SelectIndexLin, CleanLine, MaximumStringSize * sizeof(CHAR16));
       CopyMem (SelectIndexLin, String[Index], StrLen(String[Index]) * sizeof(CHAR16));

       if (Index == (*SelectIndex)) {
         gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR(EFI_BLACK, EFI_LIGHTGRAY));
         PrintAt (Columns/2 - StrLenth, Rows/2 - Item + Index, L"%s", SelectIndexLin);
         gST->ConOut->SetAttribute (gST->ConOut, Color);
         continue;
       }
       PrintAt (Columns/2 - StrLenth, Rows/2 - Item + Index, L"%s", SelectIndexLin);
    }

    StartActX = Columns/2 - StrLenth;
    EndActX = Columns/2 + StrLenth;
    StartActY = Rows/2 - Item;
    EndActY = Rows/2 - Item + (NumberOfLines - 1);

    while (!InputDevice(LocalSetupMouse, &EfiKeyData, &MouX, &MouY) && LocateMouseFlag) {
      if (EfiKeyData.Key.ScanCode == SCAN_ESC) {
        AuxActive = TRUE;
        break;
      } else {
        if (EfiKeyData.Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
          continue;
        }
        if ((MouX > StartActX) && (MouX < EndActX) &&
            (MouY >= StartActY) && (MouY <= EndActY)) {
           *SelectIndex = (UINT32) (MouY - StartActY);
           AuxActive = TRUE;
           LoopEnable = FALSE;
           break;
        }
      }
    }

    if ((LocalHotkey) && (AuxActive == FALSE)) {
      if (CheckKeyList(EfiKeyData.Key,KeyList)) {
        EventKey->ScanCode    = EfiKeyData.Key.ScanCode;
        EventKey->UnicodeChar = EfiKeyData.Key.UnicodeChar;
        LoopEnable = FALSE;
      }
    } else {
      if (EfiKeyData.Key.ScanCode == SCAN_NULL) {
        switch (EfiKeyData.Key.UnicodeChar) {

        case CHAR_CARRIAGE_RETURN:
          LoopEnable = FALSE;
          EventKey->ScanCode = EfiKeyData.Key.ScanCode;
          EventKey->UnicodeChar = EfiKeyData.Key.UnicodeChar;
          break;

        default:
          break;
        }
      } else {
        switch (EfiKeyData.Key.ScanCode) {

        case SCAN_UP:
          if ((*SelectIndex) == 0) {
            *SelectIndex = NumberOfLines - 1;
          } else {
            (*SelectIndex)--;
          }
          break;

        case SCAN_DOWN:
          if ((*SelectIndex) == (NumberOfLines - 1)) {
            *SelectIndex = 0;
          } else {
            (*SelectIndex)++;
          }
          break;

        case SCAN_ESC:
          LoopEnable = FALSE;
          break;

        default:
          break;
        }
        EventKey->ScanCode = EfiKeyData.Key.ScanCode;
        EventKey->UnicodeChar = EfiKeyData.Key.UnicodeChar;

      }
    }

  } while(LoopEnable);

  if ((LocalSetupMouse != NULL) && LocateMouseFlag){
    LocalSetupMouse->Close (LocalSetupMouse);
  }

  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  for (Index = Rows/2 - (Item + 3); Index <= Rows/2 - (Item + 3) + 3 + NumberOfLines; Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%s", CleanLine);
  }

  gBS->FreePool (CleanLine);
  gBS->FreePool (SelectIndexLin);

  return EFI_SUCCESS;
}

/**
 To draw dialog function.

 @param[in] StrLenth               Dialog Width.
 @param[in] Item                   Number of item.
 @param[in] MsgString              Message strings.
**/
VOID
DrawDialogBlock (
  IN  UINTN             StrLenth,
  IN  UINTN             Item,
  IN  CHAR16            **MsgString,
  ...
  )
{
  UINTN                 Index;
  UINTN                 Color;
  CHAR16                CleanLine[80];
  CHAR16                SelectIndexLin[80];
  UINTN                 Columns;
  UINTN                 Rows;
  CHAR16                *String;
  UINTN                 InputItemNum;
  INT32                 BackupAttribute;

  InputItemNum = Item;
  Color        = 0;
  //
  //Set the background
  //
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  BackupAttribute = gST->ConOut->Mode->Attribute;
  Color = EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE);
  gST->ConOut->SetAttribute (gST->ConOut, Color);
  StrLenth = StrLenth / 2;
  for (Index = 0; Index < (StrLenth + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
    SelectIndexLin[Index] = 0x20;
  }
  CleanLine[(StrLenth + 2) * 2 + 1] = 0x0000;
  SelectIndexLin[1 + 1] = 0x0000;
  if (Item % 2 != 0) {
    Item = (Item / 2) + 1;
  } else {
    Item = Item / 2;
  }
  for (Index = Rows/2 - (Item + 3); Index <= Rows/2 + (Item - 1); Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%s", CleanLine);
  }

  //
  //Next three step will draw a dialog
  //1.draw three row line
  //2.draw two side
  //3.draw four corner
  //

  //
  //This is draw three row line
  //
  for (Index = Columns/2 - (StrLenth + 1); Index < Columns/2 + (StrLenth + 2) ; Index++) {
    PrintAt (Index, Rows/2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 - (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows/2 + (Item - 1), L"%c", BOXDRAW_HORIZONTAL);
  }

  //
  //Draw two side
  //
  for (Index = Rows/2 - (Item + 2); Index < Rows/2 + (Item - 1); Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (StrLenth + 2), Index,L"%c", BOXDRAW_VERTICAL);
  }

  //
  //This is draw the dialog four corner
  //
  PrintAt (Columns/2 + (StrLenth + 2) ,Rows/2 + (Item - 1), L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns/2 + (StrLenth + 2) ,Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);
  PrintAt (Columns/2 - (StrLenth + 2), Rows/2 + (Item - 1), L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns/2 - (StrLenth + 2), Rows/2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  //
  //Print the title and message
  //
  String = MsgString[0];
  PrintAt (Columns/2 - (StrLen(String)/2), Rows/2 - (Item + 2), L"%s", String);
  for (Index = 1; Index < InputItemNum; Index++) {
    String = MsgString[Index];
    PrintAt (Columns/2 - (StrLen(String)/2) , Rows/2 - Item + Index - 1, L"%s", String);
  }
  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
}

/**
 To show message dialog with title.

 @param[in] RequestedWidth      Dialog Width.
 @param[in] NumberOfLines       Number of item.
 @param[in] ArrayOfStrings      Message strings.

 @retval  EFI_SUCCESS           The function completed successfully.
**/
EFI_STATUS
EFIAPI
CreateMsgPopUp (
  IN  UINT32                      RequestedWidth,
  IN  UINT32                      NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  )
{
  DrawDialogBlock (RequestedWidth, NumberOfLines, &ArrayOfStrings);

  return EFI_SUCCESS;
}

/**
 Display page information

 @param[in] TitleString         The dialog title string
 @param[in] InfoStrings         Information strings

 @return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
ShowPageInfo (
  IN   CHAR16          *TitleString,
  IN   CHAR16          *InfoStrings
  )
{

  EFI_SCREEN_DESCRIPTOR            LocalScreen;
  EFI_STATUS                       Status;

  ZeroMem (&LocalScreen, sizeof (EFI_SCREEN_DESCRIPTOR));

  gST->ConOut->QueryMode (
                 gST->ConOut,
                 gST->ConOut->Mode->Mode,
                 &LocalScreen.RightColumn,
                 &LocalScreen.BottomRow
                 );

  Status = SetupUtilityShowInformations (TitleString, InfoStrings, &LocalScreen);

  return Status;
}

/**
 Display numeric dialog for user input numeric value

 @param[in]  TitleString         The dialog title string
 @param[in]  Minimum             Minimum value of output numeric value
 @param[in]  Maximum             Maximum value of output numeric value
 @param[in]  Step                Step value of numeric value
 @param[in]  IsHex               Flag to determine if the value is hexadecimal or not
 @param[out] Step                Output numeric value

 @retval EFI_SUCCESS             Get user input value successfully
 @retval EFI_OUT_OF_RESOURCES    Allocate memory fail
**/
EFI_STATUS
EFIAPI
NumericDialog (
  IN  CHAR16                          *TitleString,
  IN  UINT64                          Minimum,
  IN  UINT64                          Maximum,
  IN  UINT64                          Step,
  IN  BOOLEAN                         IsHex,
  OUT UINT64                          *NumericValue
  )
{
  UINTN                               Columns;
  UINTN                               Rows;
  UINTN                               MouseX;
  UINTN                               MouseY;
  UINTN                               CursorX;
  UINTN                               CursorY;
  UINTN                               Index;
  BOOLEAN                             Loop;
  CHAR16                              *CleanLineStr;
  EFI_KEY_DATA                        EfiKeyData;
  BOOLEAN                             SetupMouseStartByThisFunc;
  EFI_SETUP_MOUSE_PROTOCOL            *SetupMouse;
  EFI_STATUS                          Status;
  CHAR16                              *InfoStr;
  UINTN                               InfoStrHeight;
  UINTN                               InfoStrOneLineLen;
  UINTN                               InputStrHeight;
  UINTN                               OneLineStrWidth;
  UINTN                               DialogWidth;
  UINTN                               DialogHeight;
  UINTN                               DialogTop;
  UINTN                               DialogMiddle;
  UINTN                               DialogBottom;
  UINTN                               DialogLeft;
  UINTN                               DialogRight;
  UINTN                               StringLength;
  UINTN                               Attribute;
  INT32                               BackupAttribute;
  KEYBOARD_ATTRIBUTES                 KeyboardAttributes;
  UINT32                              ValueStrLen;
  CHAR16                              *ValueStr;
  UINT32                              Base;
  UINT64                              Value;
  UINT8                               Digit;

  NotifyUsbKbInitial ();

  //
  // Initialize Setup mouse
  //
  SetupMouse = NULL;
  SetupMouseStartByThisFunc = FALSE;
  if (gDeviceManagerSetup != NULL && gDeviceManagerSetup->SetupMouse != NULL && gDeviceManagerSetup->SetupMouseFlag) {
    SetupMouse = (EFI_SETUP_MOUSE_PROTOCOL *) gDeviceManagerSetup->SetupMouse;
  } else {
    Status = gBS->LocateProtocol (&gSetupMouseProtocolGuid, NULL, (VOID **) &SetupMouse);
    if (!EFI_ERROR (Status)) {
      Status = SetupMouse->Start (SetupMouse);
      if (!EFI_ERROR (Status)) {
        SetupMouseStartByThisFunc = TRUE;
      } else {
        SetupMouse = NULL;
      }
    }
  }

  //
  // Get value string length
  //
  ValueStrLen = 1;
  Base  = IsHex ? 16 : 10;
  Value = Maximum;
  while (DivU64x32 (Value, Base) != 0) {
    ValueStrLen++;
    Value = DivU64x32 (Value, Base);
  }
  ValueStrLen += (IsHex ? HEX_VALUE_PREFIX_STRING_LENGTH : 0);

  ValueStr = AllocateZeroPool ((ValueStrLen + 1) * sizeof (CHAR16));
  if (ValueStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  if (IsHex) {
    ValueStr[0] = '0';
    ValueStr[1] = 'x';
  }

  StringLength = ValueStrLen > (GetStringWidth (TitleString) / 2 - 1) ? ValueStrLen : (GetStringWidth (TitleString) / 2 - 1);

  //
  // Make sure the dialog width between 25% ~ 66% of screen width.
  //
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  if (StringLength > Columns * 2 / 3) {
    OneLineStrWidth = Columns * 2 / 3;
  } else if (StringLength < Columns / 4) {
    OneLineStrWidth = Columns / 4;
  } else {
    OneLineStrWidth = StringLength;
  }

  DialogWidth = OneLineStrWidth + 4;

  //
  // Display dialog
  //
  Status = ProcessStrToFormatted (TitleString, OneLineStrWidth, &InfoStr, &InfoStrHeight, &InfoStrOneLineLen);
  if (EFI_ERROR (Status)) {
    InfoStr = NULL;
    InfoStrHeight = 0;
  }

  InputStrHeight = 1;
  DialogHeight = InfoStrHeight + InputStrHeight + 3;

  CleanLineStr = AllocatePool ((DialogWidth + 1) * sizeof (CHAR16));
  if (CleanLineStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < DialogWidth; Index++) {
    CleanLineStr[Index] = CHAR_SPACE;
  }
  CleanLineStr[DialogWidth] = 0;

  BackupAttribute = gST->ConOut->Mode->Attribute;
  Attribute       = EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLUE);
  gST->ConOut->SetAttribute (gST->ConOut, Attribute);

  DialogLeft   = (Columns / 2) - (DialogWidth / 2);
  DialogRight  = DialogLeft + DialogWidth - 1;
  DialogTop    = (Rows / 2) - (DialogHeight / 2);
  DialogMiddle = DialogTop + InfoStrHeight + 1;
  DialogBottom = DialogMiddle + InputStrHeight + 1;

  for (Index = DialogTop; Index <= DialogBottom; Index++) {
    PrintAt (DialogLeft, Index, L"%s", CleanLineStr);
  }
  PrintAt (DialogLeft, DialogTop   , L"%c", BOXDRAW_DOWN_RIGHT);
  PrintAt (DialogLeft, DialogBottom, L"%c", BOXDRAW_UP_RIGHT);
  for (Index = DialogLeft + 1; Index < DialogRight; Index++) {
    PrintAt (Index, DialogTop   , L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, DialogMiddle, L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, DialogBottom, L"%c", BOXDRAW_HORIZONTAL);
  }
  for (Index = DialogTop + 1; Index < DialogBottom; Index++) {
    PrintAt (DialogLeft , Index,L"%c", BOXDRAW_VERTICAL);
    PrintAt (DialogRight, Index,L"%c", BOXDRAW_VERTICAL);
  }
  PrintAt (DialogRight, DialogTop   , L"%c", BOXDRAW_DOWN_LEFT);
  PrintAt (DialogRight, DialogBottom, L"%c", BOXDRAW_UP_LEFT);
  PrintAt (DialogLeft , DialogMiddle, L"%c", BOXDRAW_VERTICAL_RIGHT);
  PrintAt (DialogRight, DialogMiddle, L"%c", BOXDRAW_VERTICAL_LEFT);

  for (Index = 0; Index < InfoStrHeight; Index++) {
    PrintAt (DialogLeft + 2, DialogTop + 1 + Index, L"%s", &InfoStr[Index * InfoStrOneLineLen]);
  }

  //
  // Process user input
  //
  ZeroMem (&EfiKeyData, sizeof(EFI_KEY_DATA));
  CursorX = DialogLeft + 2;
  CursorY = DialogMiddle + 1;
  PrintAt (CursorX - 1, CursorY, L" ");

  if (IsHex) {
    PrintAt (CursorX, CursorY, ValueStr);
    Index = HEX_VALUE_PREFIX_STRING_LENGTH;
  } else {
    Index = 0;
  }

  Loop = TRUE;
  do {
    if (!InputDevice(SetupMouse, &EfiKeyData, &MouseX, &MouseY) && SetupMouse != NULL) {
      SetupMouse->GetKeyboardAttributes (SetupMouse, &KeyboardAttributes);
      if (!KeyboardAttributes.IsStart) {
        if (EfiKeyData.Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
          continue;
        }
        if (MouseY != CursorY || MouseX <= DialogLeft || MouseX >= DialogRight) {
          continue;
        }
        SetupMouse->StartKeyboard (SetupMouse, 2, gScreenDimensions.BottomRow);
        gST->ConOut->SetAttribute (gST->ConOut, Attribute);
        gST->ConOut->SetCursorPosition (gST->ConOut, CursorX, CursorY);
        continue;
      }
      ZeroMem (&EfiKeyData, sizeof(EFI_KEY_DATA));
      gST->ConOut->SetAttribute (gST->ConOut, Attribute);
      gST->ConOut->SetCursorPosition(gST->ConOut, CursorX, CursorY);
      if (Status != EFI_SUCCESS) {
        continue;
      }
    }

    switch (EfiKeyData.Key.UnicodeChar) {

    case CHAR_CARRIAGE_RETURN:
      Value = IsHex ? StrHexToUint64 (ValueStr) : StrDecimalToUint64 (ValueStr);
      if (Value < Minimum || Value > Maximum) {
        continue;
      }

      *NumericValue = Value;
      Loop = FALSE;
      break;

    case CHAR_BACKSPACE:
      if (!IsHex && Index == 0) {
        PrintAt (CursorX - 1, CursorY, L" ");
        break;
      }
      if (IsHex && Index == HEX_VALUE_PREFIX_STRING_LENGTH) {
        PrintAt (CursorX + Index - 1, CursorY, L"x");
        break;
      }

      if (Index > OneLineStrWidth) {
        PrintAt (CursorX + OneLineStrWidth - 1, CursorY, L" ");
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorX + OneLineStrWidth - 1, CursorY);
      } else {
        PrintAt (CursorX + Index - 1, CursorY, L" ");
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorX + Index - 1, CursorY);
      }

      Index--;
      ValueStr[Index] = 0;
      break;

    default:
      if ((IsHex && !IsHexDigit (&Digit, EfiKeyData.Key.UnicodeChar)) ||
          (!IsHex && (EfiKeyData.Key.UnicodeChar > L'9' || EfiKeyData.Key.UnicodeChar < L'0'))) {
        continue;
      }

      ValueStr[Index] = EfiKeyData.Key.UnicodeChar;
      Index++;

      Value = IsHex ? StrHexToUint64 (ValueStr) : StrDecimalToUint64 (ValueStr);
      if (Value > Maximum) {
        Index--;
        ValueStr[Index] = 0;
      } else {
        PrintAt (CursorX + Index - 1, CursorY, L"%c", EfiKeyData.Key.UnicodeChar);
      }
      break;
    }
  } while (Loop);

  //
  // Clear Dialog
  //
  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
  for (Index = DialogTop; Index <= DialogBottom; Index++) {
    PrintAt (DialogLeft, Index, L"%s", CleanLineStr);
  }
  if (gDeviceManagerSetup != NULL) {
    for (Index = DialogTop; Index <= DialogBottom; Index++) {
      PrintAt (gPromptBlockWidth + gOptionBlockWidth + 1, Index, L"%c", BOXDRAW_VERTICAL);
    }
  }
  if ((SetupMouse != NULL) && SetupMouseStartByThisFunc) {
    SetupMouse->Close (SetupMouse);
  }

  if (InfoStr != NULL) {
    FreePool ((VOID *) InfoStr);
  }
  FreePool ((VOID *) CleanLineStr);
  FreePool ((VOID *) ValueStr);

  return EFI_SUCCESS;
}

/**
 This is the routine which install the OEM form browser protocol.

 @retval  EFI_SUCCESS            The function completed successfully.
 @retval  otherwise              Install protocol interface fail
**/
EFI_STATUS
InstallH2ODialogProtocol (
  IN EFI_HANDLE                        Handle
  )
{
  EFI_STATUS                           Status;
  H2O_DIALOG_PROTOCOL                  *H2ODialogData;

  H2ODialogData = AllocatePool (sizeof (H2O_DIALOG_PROTOCOL));
  ASSERT (H2ODialogData != NULL);
  if (H2ODialogData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  H2ODialogData->ConfirmDialog     = ConfirmDialog;
  H2ODialogData->PasswordDialog    = PasswordDialog;
  H2ODialogData->OneOfOptionDialog = OneOfOptionDialog;
  H2ODialogData->CreateMsgPopUp    = CreateMsgPopUp;
  H2ODialogData->ShowPageInfo      = ShowPageInfo;
  H2ODialogData->NumericDialog     = NumericDialog;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gH2ODialogProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  H2ODialogData
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

