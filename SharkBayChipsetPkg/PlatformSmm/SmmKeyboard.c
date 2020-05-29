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
#include <SmmKeyboard.h>
KEYBOARD_CONSOLE_IN_DEV mConIn;

//
// Global variables
//
//
// Scancode to Efi key mapping table
// Format:<efi scan code>, <unicode without shift>, <unicode with shift>
//
STATIC UINT8  ConvertKeyboardScanCodeToEfiKey[SCANCODE_MAX_MAKE][3] = {
  SCAN_NULL,
  0x00,
  0x00, // 0x00  Unknown
  SCAN_ESC,
  0x00,
  0x00, // 0x01  Escape
  SCAN_NULL,
  '1',
  '!',  // 0x02
  SCAN_NULL,
  '2',
  '@',  // 0x03
  SCAN_NULL,
  '3',
  '#',  // 0x04
  SCAN_NULL,
  '4',
  '$',  // 0x05
  SCAN_NULL,
  '5',
  '%',  // 0x06
  SCAN_NULL,
  '6',
  '^',  // 0x07
  SCAN_NULL,
  '7',
  '&',  // 0x08
  SCAN_NULL,
  '8',
  '*',  // 0x09
  SCAN_NULL,
  '9',
  '(',  // 0x0A
  SCAN_NULL,
  '0',
  ')',  // 0x0B
  SCAN_NULL,
  '-',
  '_',  // 0x0C
  SCAN_NULL,
  '=',
  '+',  // 0x0D
  SCAN_NULL,
  0x08,
  0x08, // 0x0E  BackSpace
  SCAN_NULL,
  0x09,
  0x09, // 0x0F  Tab
  SCAN_NULL,
  'q',
  'Q',  // 0x10
  SCAN_NULL,
  'w',
  'W',  // 0x11
  SCAN_NULL,
  'e',
  'E',  // 0x12
  SCAN_NULL,
  'r',
  'R',  // 0x13
  SCAN_NULL,
  't',
  'T',  // 0x14
  SCAN_NULL,
  'y',
  'Y',  // 0x15
  SCAN_NULL,
  'u',
  'U',  // 0x16
  SCAN_NULL,
  'i',
  'I',  // 0x17
  SCAN_NULL,
  'o',
  'O',  // 0x18
  SCAN_NULL,
  'p',
  'P',  // 0x19
  SCAN_NULL,
  '[',
  '{',  // 0x1a
  SCAN_NULL,
  ']',
  '}',  // 0x1b
  SCAN_NULL,
  0x0d,
  0x0d, // 0x1c  Enter
  SCAN_NULL,
  0x00,
  0x00, // 0x1d  Unknown
  SCAN_NULL,
  'a',
  'A',  // 0x1e
  SCAN_NULL,
  's',
  'S',  // 0x1f
  SCAN_NULL,
  'd',
  'D',  // 0x20
  SCAN_NULL,
  'f',
  'F',  // 0x21
  SCAN_NULL,
  'g',
  'G',  // 0x22
  SCAN_NULL,
  'h',
  'H',  // 0x23
  SCAN_NULL,
  'j',
  'J',  // 0x24
  SCAN_NULL,
  'k',
  'K',  // 0x25
  SCAN_NULL,
  'l',
  'L',  // 0x26
  SCAN_NULL,
  ';',
  ':',  // 0x27
  SCAN_NULL,
  '\'',
  '"',  // 0x28
  SCAN_NULL,
  '`',
  '~',  // 0x29
  SCAN_NULL,
  0x00,
  0x00, // 0x2A  Left Shift
  SCAN_NULL,
  '\\',
  '|',  // 0x2B
  SCAN_NULL,
  'z',
  'Z',  // 0x2c
  SCAN_NULL,
  'x',
  'X',  // 0x2d
  SCAN_NULL,
  'c',
  'C',  // 0x2e
  SCAN_NULL,
  'v',
  'V',  // 0x2f
  SCAN_NULL,
  'b',
  'B',  // 0x30
  SCAN_NULL,
  'n',
  'N',  // 0x31
  SCAN_NULL,
  'm',
  'M',  // 0x32
  SCAN_NULL,
  ',',
  '<',  // 0x33
  SCAN_NULL,
  '.',
  '>',  // 0x34
  SCAN_NULL,
  '/',
  '?',  // 0x35
  SCAN_NULL,
  0x00,
  0x00, // 0x36  Right Shift
  SCAN_NULL,
  '*',
  '*',  // 0x37  Numeric Keypad *
  SCAN_NULL,
  0x00,
  0x00, // 0x38  Left Alt/Extended Right Alt
  SCAN_NULL,
  ' ',
  ' ',  // 0x39
  SCAN_NULL,
  0x00,
  0x00, // 0x3A  CapsLock
  SCAN_F1,
  0x00,
  0x00, // 0x3B
  SCAN_F2,
  0x00,
  0x00, // 0x3C
  SCAN_F3,
  0x00,
  0x00, // 0x3D
  SCAN_F4,
  0x00,
  0x00, // 0x3E
  SCAN_F5,
  0x00,
  0x00, // 0x3F
  SCAN_F6,
  0x00,
  0x00, // 0x40
  SCAN_F7,
  0x00,
  0x00, // 0x41
  SCAN_F8,
  0x00,
  0x00, // 0x42
  SCAN_F9,
  0x00,
  0x00, // 0x43
  SCAN_F10,
  0x00,
  0x00, // 0x44
  SCAN_NULL,
  0x00,
  0x00, // 0x45  NumLock
  SCAN_NULL,
  0x00,
  0x00, // 0x46  ScrollLock
  SCAN_HOME,
  '7',
  '7',  // 0x47
  SCAN_UP,
  '8',
  '8',  // 0x48
  SCAN_PAGE_UP,
  '9',
  '9',  // 0x49
  SCAN_NULL,
  '-',
  '-',  // 0x4a
  SCAN_LEFT,
  '4',
  '4',  // 0x4b
  SCAN_NULL,
  '5',
  '5',  // 0x4c  Numeric Keypad 5
  SCAN_RIGHT,
  '6',
  '6',  // 0x4d
  SCAN_NULL,
  '+',
  '+',  // 0x4e
  SCAN_END,
  '1',
  '1',  // 0x4f
  SCAN_DOWN,
  '2',
  '2',  // 0x50
  SCAN_PAGE_DOWN,
  '3',
  '3',  // 0x51
  SCAN_INSERT,
  '0',
  '0',  // 0x52
  SCAN_DELETE,
  '.',
  '.'   // 0x53
};


UINT8
ReadKbcStatus(
  )
{
  return IoRead8(SMM_INPUT_KBC_STATUS_PORT);
}

UINT8
ReadKbcData(
  )
{
  return IoRead8(SMM_INPUT_KBC_DATA_PORT);
}

/**
 Read several bytes from the scancode buffer without removing them.
 This function is called to see if there are enough bytes of scancode
 representing a single key.

 @param        ConsoleIn
 @param [in]   Count            Number of bytes to be read
 @param [out]  Buf              Store the results

 @retval EFI_STATUS

**/
STATIC
EFI_STATUS
GetScancodeBufHead (
  KEYBOARD_CONSOLE_IN_DEV    *ConsoleIn,
  IN UINT32                  Count,
  OUT UINT8                  *Buf
  )
{
  UINT32  Index;
  UINT32  Pos;

  Index = 0;
  Pos   = 0;

  //
  // check the valid range of parameter 'Count'
  //
  if (Count <= 0 || ConsoleIn->ScancodeBufCount < Count) {
    return EFI_NOT_READY;
  }
  //
  // retrieve the values
  //
  for (Index = 0; Index < Count; Index++) {

    if (Index == 0) {

      Pos = ConsoleIn->ScancodeBufStartPos;
    } else {

      Pos = Pos + 1;
      if (Pos >= KEYBOARD_BUFFER_MAX_COUNT) {
        Pos = 0;
      }
    }

    Buf[Index] = ConsoleIn->ScancodeBuf[Pos];
  }

  return EFI_SUCCESS;
}

/**
 Read & remove several bytes from the scancode buffer.
 This function is usually called after GetScancodeBufHead()

 @param        ConsoleIn
 @param [in]   Count            Number of bytes to be read
 @param [out]  Buf              Store the results

 @retval EFI_STATUS

**/
STATIC
EFI_STATUS
PopScancodeBufHead (
  KEYBOARD_CONSOLE_IN_DEV   *ConsoleIn,
  IN UINT32                 Count,
  OUT UINT8                 *Buf
  )
{
  UINT32  Index;

  Index = 0;

  //
  // Check the valid range of parameter 'Count'
  //
  if (Count <= 0 || ConsoleIn->ScancodeBufCount < Count) {
    return EFI_NOT_READY;
  }
  //
  // Retrieve and remove the values
  //
  for (Index = 0; Index < Count; Index++) {

    if (Index != 0) {

      ConsoleIn->ScancodeBufStartPos++;
      if (ConsoleIn->ScancodeBufStartPos >= KEYBOARD_BUFFER_MAX_COUNT) {
        ConsoleIn->ScancodeBufStartPos = 0;
      }
    }

    Buf[Index] = ConsoleIn->ScancodeBuf[ConsoleIn->ScancodeBufStartPos];
    ConsoleIn->ScancodeBufCount--;
  }

  ConsoleIn->ScancodeBufStartPos++;
  if (ConsoleIn->ScancodeBufStartPos >= KEYBOARD_BUFFER_MAX_COUNT) {
    ConsoleIn->ScancodeBufStartPos = 0;
  }

  return EFI_SUCCESS;
}

/**
 Read keyboard controller. If OBF == true then transfer scan code.

 @param [out]  OutPutKey        report output key.

 @retval EFI_STATUS             have key output form keyboard controller.
 @retval EFI_NOT_READY          did not found key in output buffer of keyboard controller.

**/
EFI_STATUS
ReadKey(
  OUT   EFI_INPUT_KEY     *OutPutKey
  )
{
  UINT8                   Readed;
  UINT8                   ScanCode;
  EFI_STATUS              Status;
  BOOLEAN                 Extended;
  UINT8                   ScancodeArr[4];
  KEYBOARD_CONSOLE_IN_DEV *ConsoleIn;
  //
  // 4 bytes most
  //
  UINT32                  ScancodeArrPos;

  Readed          = 0;
  Extended        = FALSE;
  ScancodeArrPos  = 0;

  ConsoleIn = &mConIn;

  if ((ReadKbcStatus() & (KBC_OUT_BUFFER_FULL | KBC_AUX_DEV_OUTPUT)) != \
          KBC_OUT_BUFFER_FULL) {
    return EFI_NOT_FOUND;
  }

  Readed = ReadKbcData();

  ConsoleIn->ScancodeBufCount++;
  ConsoleIn->ScancodeBufEndPos++;
  if (ConsoleIn->ScancodeBufEndPos >= KEYBOARD_BUFFER_MAX_COUNT) {
    ConsoleIn->ScancodeBufEndPos = 0x00;
  }

  ConsoleIn->ScancodeBuf[mConIn.ScancodeBufEndPos] = Readed;
    //
    // Handle Alt+Ctrl+Del Key combination
    //
  switch (Readed) {

  case SCANCODE_CTRL_MAKE:
    ConsoleIn->Ctrled = TRUE;
    break;

  case SCANCODE_CTRL_BREAK:
    ConsoleIn->Ctrled = FALSE;
    break;

  case SCANCODE_ALT_MAKE:
    ConsoleIn->Alted = TRUE;
    break;

  case SCANCODE_ALT_BREAK:
    ConsoleIn->Alted = FALSE;
    break;
  }

  //
  // Check if there are enough bytes of scancode representing a single key
  // available in the buffer
  //
  while (1) {

    Status          = GetScancodeBufHead (ConsoleIn, 1, ScancodeArr);
    ScancodeArrPos  = 0;
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY;
    }

    if (ScancodeArr[ScancodeArrPos] == SCANCODE_EXTENDED) {
      Extended        = TRUE;
      Status          = GetScancodeBufHead (ConsoleIn, 2, ScancodeArr);
      ScancodeArrPos  = 1;
      if (EFI_ERROR (Status)) {
        return EFI_NOT_READY;
      }
    }
    //
    // Checks for key scancode for PAUSE:E1-1D/45-E1/9D-C5
    // if present, ignore them
    //
    if (ScancodeArr[ScancodeArrPos] == SCANCODE_EXTENDED1) {

      Status          = GetScancodeBufHead (ConsoleIn, 2, ScancodeArr);
      ScancodeArrPos  = 1;

      if (EFI_ERROR (Status)) {
        return EFI_NOT_READY;
      }

      Status          = GetScancodeBufHead (ConsoleIn, 3, ScancodeArr);
      ScancodeArrPos  = 2;

      if (EFI_ERROR (Status)) {
        return EFI_NOT_READY;
      }

      PopScancodeBufHead (ConsoleIn, 3, ScancodeArr);
      return EFI_NOT_READY;
    }
    //
    // if we reach this position, scancodes for a key is in buffer now,pop them
    //
    PopScancodeBufHead (ConsoleIn, ScancodeArrPos + 1, ScancodeArr);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY;
    }
    //
    // store the last available byte, this byte of scancode will be checked
    //
    ScanCode = ScancodeArr[ScancodeArrPos];

    //
    // Check for special keys and update the driver state.
    //
    switch (ScanCode) {

    case SCANCODE_CTRL_MAKE:
      ConsoleIn->Ctrl = TRUE;
      break;

    case SCANCODE_CTRL_BREAK:
      ConsoleIn->Ctrl = FALSE;
      break;

    case SCANCODE_ALT_MAKE:
      ConsoleIn->Alt = TRUE;
      break;

    case SCANCODE_ALT_BREAK:
      ConsoleIn->Alt = FALSE;
      break;

    case SCANCODE_LEFT_SHIFT_MAKE:
    case SCANCODE_RIGHT_SHIFT_MAKE:
      if (!Extended) {
        ConsoleIn->Shift = TRUE;
      }
      break;

    case SCANCODE_LEFT_SHIFT_BREAK:
    case SCANCODE_RIGHT_SHIFT_BREAK:
      if (!Extended) {
        ConsoleIn->Shift = FALSE;
      }
      break;

    case SCANCODE_CAPS_LOCK_MAKE:
      break;

    case SCANCODE_NUM_LOCK_MAKE:
      break;

    case SCANCODE_SCROLL_LOCK_MAKE:
      break;
    }
    //
    // If this is a BREAK Key or above the valid range, ignore it
    //
    if (ScanCode >= SCANCODE_MAX_MAKE) {
      continue;
    } else {
      break;
    }
  }
  //
  // If this is the SysRq, ignore it
  //
  if (Extended && ScanCode == 0x37) {
    return EFI_NOT_READY;
  }
  //
  // Treat Numeric Key Pad "/" specially
  //
  if (Extended && ScanCode == 0x35) {
    ConsoleIn->Key.ScanCode     = SCAN_NULL;
    ConsoleIn->Key.UnicodeChar  = '/';
    return EFI_SUCCESS;
  }
  //
  // Convert Keyboard ScanCode into an EFI Key
  //
  ConsoleIn->Key.ScanCode = ConvertKeyboardScanCodeToEfiKey[ScanCode][0];
  if (ConsoleIn->Shift) {
    ConsoleIn->Key.UnicodeChar = ConvertKeyboardScanCodeToEfiKey[ScanCode][2];
  } else {
    ConsoleIn->Key.UnicodeChar = ConvertKeyboardScanCodeToEfiKey[ScanCode][1];
  }
  //
  // alphabetic key is affected by CapsLock State
  //
  if (ConsoleIn->CapsLock) {

    if (ConsoleIn->Key.UnicodeChar >= 'a' && ConsoleIn->Key.UnicodeChar <= 'z') {

      ConsoleIn->Key.UnicodeChar = ConvertKeyboardScanCodeToEfiKey[ScanCode][2];

    } else if (ConsoleIn->Key.UnicodeChar >= 'A' && ConsoleIn->Key.UnicodeChar <= 'Z') {

      ConsoleIn->Key.UnicodeChar = ConvertKeyboardScanCodeToEfiKey[ScanCode][1];

    }
  }
  //
  // distinguish numeric key pad keys' 'up symbol' and 'down symbol'
  //
  if (ScanCode >= 0x47 && ScanCode <= 0x53) {

    if (ConsoleIn->NumLock && !ConsoleIn->Shift && !Extended) {
      ConsoleIn->Key.ScanCode = SCAN_NULL;
    } else if (ScanCode != 0x4a && ScanCode != 0x4e) {
      ConsoleIn->Key.UnicodeChar = 0x00;
    }
  }
  //
  // If the key can not be converted then just return.
  //
  if (ConsoleIn->Key.ScanCode == SCAN_NULL && ConsoleIn->Key.UnicodeChar == 0x00) {
    return EFI_NOT_READY;
  }

//  *OutPutKey = ConsoleIn->Key.UnicodeChar;
  OutPutKey->UnicodeChar = ConsoleIn->Key.UnicodeChar;
  OutPutKey->ScanCode = ConsoleIn->Key.ScanCode;
  return EFI_SUCCESS;
}

/**
 Read key and fill in stringbufer address.

 @param [out]  StringBuffer     String buffer address.
 @param [in, out] StringLength  String buffer length.

 @retval None.

**/
VOID
InputString(
     OUT CHAR16 *StringBuffer,
  IN OUT UINTN  *StringLength
  )
{
  UINTN          MaxCount;
  UINTN          Index;
  EFI_INPUT_KEY  OutPutKey;

  Index = 0;
  MaxCount = (*StringLength) - 1;

  while (Index < MaxCount) {
    while (EFI_ERROR(ReadKey(&OutPutKey))) {
    }

    switch (OutPutKey.UnicodeChar) {
      case CHAR_BACKSPACE:
          if (Index == 0) {
            break;
          }
          Index--;
          StringBuffer[Index] = 0;
        break;
      case CHAR_CARRIAGE_RETURN:
          *StringLength = Index;
          StringBuffer[MaxCount] = 0x00;
          return;
        break;
      default:
          StringBuffer[Index] = OutPutKey.UnicodeChar;
          Index++;
        break;
    }
  }
  *StringLength = Index;
  StringBuffer[MaxCount] = 0x00;
  return;
}
