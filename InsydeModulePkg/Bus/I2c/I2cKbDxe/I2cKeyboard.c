/** @file
  Helper functions for I2C Keyboard Driver

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

#include "Efikey.h"
#include "I2cKeyboard.h"
//
// I2C Key Code to Efi key mapping table
// Format:<efi scan code>, <unicode without shift>, <unicode with shift>
//
STATIC
UINT8 KeyConvertionTable[KEYCODE_MAX_MAKE][3] = {
    SCAN_NULL,      'a',      'A',      // 0x04
    SCAN_NULL,      'b',      'B',      // 0x05
    SCAN_NULL,      'c',      'C',      // 0x06
    SCAN_NULL,      'd',      'D',      // 0x07
    SCAN_NULL,      'e',      'E',      // 0x08
    SCAN_NULL,      'f',      'F',      // 0x09
    SCAN_NULL,      'g',      'G',      // 0x0A
    SCAN_NULL,      'h',      'H',      // 0x0B
    SCAN_NULL,      'i',      'I',      // 0x0C
    SCAN_NULL,      'j',      'J',      // 0x0D
    SCAN_NULL,      'k',      'K',      // 0x0E
    SCAN_NULL,      'l',      'L',      // 0x0F
    SCAN_NULL,      'm',      'M',      // 0x10
    SCAN_NULL,      'n',      'N',      // 0x11
    SCAN_NULL,      'o',      'O',      // 0x12
    SCAN_NULL,      'p',      'P',      // 0x13
    SCAN_NULL,      'q',      'Q',      // 0x14
    SCAN_NULL,      'r',      'R',      // 0x15
    SCAN_NULL,      's',      'S',      // 0x16
    SCAN_NULL,      't',      'T',      // 0x17
    SCAN_NULL,      'u',      'U',      // 0x18
    SCAN_NULL,      'v',      'V',      // 0x19
    SCAN_NULL,      'w',      'W',      // 0x1A
    SCAN_NULL,      'x',      'X',      // 0x1B
    SCAN_NULL,      'y',      'Y',      // 0x1C
    SCAN_NULL,      'z',      'Z',      // 0x1D
    SCAN_NULL,      '1',      '!',      // 0x1E
    SCAN_NULL,      '2',      '@',      // 0x1F
    SCAN_NULL,      '3',      '#',      // 0x20
    SCAN_NULL,      '4',      '$',      // 0x21
    SCAN_NULL,      '5',      '%',      // 0x22
    SCAN_NULL,      '6',      '^',      // 0x23
    SCAN_NULL,      '7',      '&',      // 0x24
    SCAN_NULL,      '8',      '*',      // 0x25
    SCAN_NULL,      '9',      '(',      // 0x26
    SCAN_NULL,      '0',      ')',      // 0x27
    SCAN_NULL,      0x0d,     0x0d,     // 0x28   Enter
    SCAN_ESC,       0x00,     0x00,     // 0x29   Esc
    SCAN_NULL,      0x08,     0x08,     // 0x2A   Backspace
    SCAN_NULL,      0x09,     0x09,     // 0x2B   Tab
    SCAN_NULL,      ' ',      ' ',      // 0x2C   Spacebar
    SCAN_NULL,      '-',      '_',      // 0x2D
    SCAN_NULL,      '=',      '+',      // 0x2E
    SCAN_NULL,      '[',      '{',      // 0x2F
    SCAN_NULL,      ']',      '}',      // 0x30
    SCAN_NULL,      '\\',     '|',      // 0x31
    SCAN_NULL,      '\\',     '|',      // 0x32  Keyboard US \ and |
    SCAN_NULL,      ';',      ':',      // 0x33
    SCAN_NULL,      '\'',     '"',      // 0x34
    SCAN_NULL,      '`',      '~',      // 0x35  Keyboard Grave Accent and Tlide
    SCAN_NULL,      ',',      '<',      // 0x36
    SCAN_NULL,      '.',      '>',      // 0x37
    SCAN_NULL,      '/',      '?',      // 0x38
    SCAN_NULL,      0x00,     0x00,     // 0x39   CapsLock
    SCAN_F1,        0x00,     0x00,     // 0x3A
    SCAN_F2,        0x00,     0x00,     // 0x3B
    SCAN_F3,        0x00,     0x00,     // 0x3C  
    SCAN_F4,        0x00,     0x00,     // 0x3D  
    SCAN_F5,        0x00,     0x00,     // 0x3E
    SCAN_F6,        0x00,     0x00,     // 0x3F
    SCAN_F7,        0x00,     0x00,     // 0x40
    SCAN_F8,        0x00,     0x00,     // 0x41
    SCAN_F9,        0x00,     0x00,     // 0x42
    SCAN_F10,       0x00,     0x00,     // 0x43
    SCAN_F11,       0x00,     0x00,     // 0x44   F11
    SCAN_F12,       0x00,     0x00,     // 0x45   F12
    SCAN_NULL,      0x00,     0x00,     // 0x46   PrintScreen
    SCAN_NULL,      0x00,     0x00,     // 0x47   Scroll Lock
    SCAN_PAUSE,     0x00,     0x00,     // 0x48   Pause
    SCAN_INSERT,    0x00,     0x00,     // 0x49
    SCAN_HOME,      0x00,     0x00,     // 0x4A
    SCAN_PAGE_UP,   0x00,     0x00,     // 0x4B
    SCAN_DELETE,    0x00,     0x00,     // 0x4C
    SCAN_END,       0x00,     0x00,     // 0x4D
    SCAN_PAGE_DOWN, 0x00,     0x00,     // 0x4E
    SCAN_RIGHT,     0x00,     0x00,     // 0x4F
    SCAN_LEFT,      0x00,     0x00,     // 0x50
    SCAN_DOWN,      0x00,     0x00,     // 0x51
    SCAN_UP,        0x00,     0x00,     // 0x52
    SCAN_NULL,      0x00,     0x00,     // 0x53   NumLock
    SCAN_NULL,      '/',      '/',      // 0x54
    SCAN_NULL,      '*',      '*',      // 0x55
    SCAN_NULL,      '-',      '-',      // 0x56
    SCAN_NULL,      '+',      '+',      // 0x57
    SCAN_NULL,      0x0d,     0x0d,     // 0x58
    SCAN_END,       '1',      '1',      // 0x59
    SCAN_DOWN,      '2',      '2',      // 0x5A
    SCAN_PAGE_DOWN, '3',      '3',      // 0x5B
    SCAN_LEFT,      '4',      '4',      // 0x5C
    SCAN_NULL,      '5',      '5',      // 0x5D
    SCAN_RIGHT,     '6',      '6',      // 0x5E
    SCAN_HOME,      '7',      '7',      // 0x5F
    SCAN_UP,        '8',      '8',      // 0x60
    SCAN_PAGE_UP,   '9',      '9',      // 0x61
    SCAN_INSERT,    '0',      '0',      // 0x62
    SCAN_DELETE,    '.',      '.',      // 0x63
    SCAN_NULL,      '\\',     '|',      // 0x64 Keyboard Non-US \ and |
    SCAN_NULL,      0x00,     0x00,     // 0x65 Keyboard Application
    SCAN_NULL,      0x00,     0x00,     // 0x66 Keyboard Power
    SCAN_NULL,      '=' ,     '=' ,     // 0x67 Keypad =
    SCAN_F13,         0x00,   0x00,     // 0x68
    SCAN_F14,         0x00,   0x00,     // 0x69
    SCAN_F15,         0x00,   0x00,     // 0x6A  
    SCAN_F16,         0x00,   0x00,     // 0x6B  
    SCAN_F17,         0x00,   0x00,     // 0x6C
    SCAN_F18,         0x00,   0x00,     // 0x6D
    SCAN_F19,         0x00,   0x00,     // 0x6E
    SCAN_F20,         0x00,   0x00,     // 0x6F
    SCAN_F21,         0x00,   0x00,     // 0x70
    SCAN_F22,         0x00,   0x00,     // 0x71
    SCAN_F23,         0x00,   0x00,     // 0x72
    SCAN_F24,         0x00,   0x00,     // 0x73
    SCAN_MUTE,        0x00,   0x00,     // 0x7F
    SCAN_VOLUME_UP,   0x00,   0x00,     // 0x80
    SCAN_VOLUME_DOWN, 0x00,   0x00      // 0x81
};

STATIC KB_MODIFIER  KB_Mod[8] = {
  { MOD_CONTROL_L,  0xe0 }, // 11100000 
  { MOD_CONTROL_R,  0xe4 }, // 11100100 
  { MOD_SHIFT_L,    0xe1 }, // 11100001 
  { MOD_SHIFT_R,    0xe5 }, // 11100101 
  { MOD_ALT_L,      0xe2 }, // 11100010 
  { MOD_ALT_R,      0xe6 }, // 11100110 
  { MOD_WIN_L,      0xe3 }, // 11100011 
  { MOD_WIN_R,      0xe7 }, // 11100111 
};

STATIC
UINT8 KbcCodeTable[][4] = {
//  Original Shift    Alt      Ctrl
    0x1E,    0x1E,    0x1E,    0x1E,           // 0x04  'A'
    0x30,    0x30,    0x30,    0x30,           // 0x05  'B'
    0x2E,    0x2E,    0x2E,    0x2E,           // 0x06  'C'
    0x20,    0x20,    0x20,    0x20,           // 0x07  'D'
    0x12,    0x12,    0x12,    0x12,           // 0x08  'E'
    0x21,    0x21,    0x21,    0x21,           // 0x09  'F'
    0x22,    0x22,    0x22,    0x22,           // 0x0A  'G'
    0x23,    0x23,    0x23,    0x23,           // 0x0B  'H'
    0x17,    0x17,    0x17,    0x17,           // 0x0C  'I'
    0x24,    0x24,    0x24,    0x24,           // 0x0D  'J'
    0x25,    0x25,    0x25,    0x25,           // 0x0E  'K'
    0x26,    0x26,    0x26,    0x26,           // 0x0F  'L'
    0x32,    0x32,    0x32,    0x32,           // 0x10  'M'
    0x31,    0x31,    0x31,    0x31,           // 0x11  'N'
    0x18,    0x18,    0x18,    0x18,           // 0x12  'O'
    0x19,    0x19,    0x19,    0x19,           // 0x13  'P'
    0x10,    0x10,    0x10,    0x10,           // 0x14  'Q'
    0x13,    0x13,    0x13,    0x13,           // 0x15  'R'
    0x1F,    0x1F,    0x1F,    0x1F,           // 0x16  'S'
    0x14,    0x14,    0x14,    0x14,           // 0x17  'T'
    0x16,    0x16,    0x16,    0x16,           // 0x18  'U'
    0x2F,    0x2F,    0x2F,    0x2F,           // 0x19  'V'
    0x11,    0x11,    0x11,    0x11,           // 0x1A  'W'
    0x2D,    0x2D,    0x2D,    0x2D,           // 0x1B  'X'
    0x15,    0x15,    0x15,    0x15,           // 0x1C  'Y'
    0x2C,    0x2C,    0x2C,    0x2C,           // 0x1D  'Z'
    0x02,    0x02,    0x78,    0x00,           // 0x1E  '1 !'
    0x03,    0x03,    0x79,    0x03,           // 0x1F  '2 @'
    0x04,    0x04,    0x7A,    0x00,           // 0x20  '3 #'
    0x05,    0x05,    0x7B,    0x00,           // 0x21  '4 $'
    0x06,    0x06,    0x7C,    0x00,           // 0x22  '5 %'
    0x07,    0x07,    0x7D,    0x07,           // 0x23  '6 ^'
    0x08,    0x08,    0x7E,    0x00,           // 0x24  '7 &'
    0x09,    0x09,    0x7F,    0x00,           // 0x25  '8 *'
    0x0A,    0x0A,    0x81,    0x00,           // 0x26  '9 ('
    0x0B,    0x0B,    0x82,    0x00,           // 0x27  '0 )'
    0x1C,    0x1C,    0x1C,    0x1C,           // 0x28  'Enter'
    0x01,    0x01,    0x01,    0x01,           // 0x29  'Esc'
    0x0E,    0x0E,    0x0E,    0x0E,           // 0x2A  'Backspace'
    0x0F,    0x0F,    0xA5,    0x94,           // 0x2B  'Tab'
    0x39,    0x39,    0x39,    0x39,           // 0x2C  'Space bar'
    0x0C,    0x0C,    0x82,    0x0C,           // 0x2D  '- _'
    0x0D,    0x0D,    0x83,    0x00,           // 0x2E  '= +'
    0x1A,    0x1A,    0x1A,    0x1A,           // 0x2F  '[ {'
    0x1B,    0x1B,    0x1B,    0x1B,           // 0x30  '] }'
    0x2B,    0x2B,    0x2B,    0x2B,           // 0x31  '\ |'
    0x56,    0x56,    0x56,    0x56,           // 0x32  Non-US '# ~' ******
    0x27,    0x27,    0x27,    0x00,           // 0x33  '; :'
    0x28,    0x28,    0x28,    0x00,           // 0x34  '' "'
    0x29,    0x29,    0x29,    0x00,           // 0x35  ;` ~'
    0x33,    0x33,    0x33,    0x00,           // 0x36  ', <'
    0x34,    0x34,    0x34,    0x00,           // 0x37  '. >'
    0x35,    0x35,    0x35,    0x00,           // 0x38  '/ ?'
    0x3A,    0x3A,    0x3A,    0x3A,           // 0x39  'CapsLock'
    0x3B,    0x54,    0x68,    0x5E,           // 0x3A  'F1'
    0x3C,    0x55,    0x69,    0x5F,           // 0x3B  'F2'
    0x3D,    0x56,    0x6A,    0x60,           // 0x3C  'F3'
    0x3E,    0x57,    0x6B,    0x61,           // 0x3D  'F4'
    0x3F,    0x58,    0x6C,    0x62,           // 0x3E  'F5'
    0x40,    0x59,    0x6D,    0x63,           // 0x3F  'F6'
    0x41,    0x5A,    0x6E,    0x64,           // 0x40  'F7'
    0x42,    0x5B,    0x6F,    0x65,           // 0x41  'F8'
    0x43,    0x5C,    0x70,    0x66,           // 0x42  'F9'
    0x44,    0x5D,    0x71,    0x67,           // 0x43  'F10'
    0x57,    0x87,    0x8B,    0x89,           // 0x44  'F11'
    0x58,    0x88,    0x8C,    0x8A,           // 0x45  'F12'
    0x80,    0x80,    0x80,    0x72,           // 0x46  'PrntScrn'         E0,2A,E0,37     E0,B7,E0,AA
    0x46,    0x46,    0x46,    0x46,           // 0x47  'ScrollLock'
    0x81,    0x81,    0x81,    0x81,           // 0x48  'Pause'            E1,1D,45        E1,9D,C5
    0xD2,    0xD2,    0xA2,    0x92,           // 0x49  'Insert'           E0,52           E0,D2
    0xC7,    0xC7,    0x97,    0x77,           // 0x4A  'Home'             E0,47           E0,C7
    0xC9,    0xC9,    0x99,    0x84,           // 0x4B  'PageUp'           E0,49           E0,C9
    0xD3,    0xD3,    0xA3,    0x93,           // 0x4C  'Delete Forward'   E0,53           E0,D3
    0xCF,    0xCF,    0x9F,    0x75,           // 0x4D  'End'              E0,4F           E0,CF
    0xD1,    0xD1,    0xA1,    0x76,           // 0x4E  'PageDown'         E0,51           E0,D1
    0xCD,    0xCD,    0x9D,    0x74,           // 0x4F  'Right Arrow'      E0,4D           E0,CD
    0xCB,    0xCB,    0x9B,    0x73,           // 0x50  'Left Arrow'       E0,4B           E0,CB
    0xD0,    0xD0,    0xA0,    0x91,           // 0x51  'Down Arrow'       E0,50           E0,D0
    0xC8,    0xC8,    0x98,    0x8D,           // 0x52  'Up Arrow'         E0,48           E0,C8
    0x45,    0x45,    0x45,    0x45,           // 0x53  'NumLock'
    0xB5,    0xB5,    0xA4,    0x95,           // 0x54  'keypad /'         E0,35           E0,B5
    0x37,    0x37,    0x37,    0x96,           // 0x55  'keypad *'
    0x4A,    0x4A,    0x4A,    0x8E,           // 0x56  'keypad -'
    0x4E,    0x4E,    0x4E,    0x90,           // 0x57  'keypad +'
    0x9C,    0x9C,    0xA6,    0x9C,           // 0x58  'keypad Enter'     E0,1C           E0,9C
    0x4F,    0x4F,    0x9F,    0x75,           // 0x59  'keypad 1 End'
    0x50,    0x50,    0xA0,    0x91,           // 0x5A  'keypad 2 DownArrow'
    0x51,    0x51,    0xA1,    0x76,           // 0x5B  'keypad 3 PageDn'
    0x4B,    0x4B,    0x9B,    0x73,           // 0x5C  'keypad 4 LeftArrow'
    0x4C,    0x4C,    0x00,    0x8F,           // 0x5D  'keypad 5'
    0x4D,    0x4D,    0x9D,    0x74,           // 0x5E  'keypad 6 RightArrow'
    0x47,    0x47,    0x97,    0x77,           // 0x5F  'keypad 7 Home'
    0x48,    0x48,    0x98,    0x8D,           // 0x60  'keypad 8 UpArrow'
    0x49,    0x49,    0x99,    0x84,           // 0x61  'keypad 9 PageUp'
    0x52,    0x52,    0xA2,    0x92,           // 0x62  'keypad 0 Insert'
    0x53,    0x53,    0xA3,    0x93,           // 0x63  'keypad . Del'
    0x2B,    0x2B,    0x2B,    0x2B,           // 0x64  '\ |'
    0xDD,    0xDD,    0xDD,    0xDD            // 0x65  'Application'      E0,5D           E0,DD
};

STATIC
UINT8 CtrlIgnoreKey[] = {
    0x1E,           // '1','!'
    0x20,           // '3','#'
    0x21,           // '4','$'
    0x22,           // '5','%'
    0x24,           // '7','&'
    0x25,           // '8','*'
    0x26,           // '9','('
    0x27,           // '0',')'
    0x2E,           // '=','+'
    0x33,           // ';',':'
    0x34,           // '\'','"'
    0x35,           // '`','~'
    0x36,           // ',','<'
    0x37,           // '.','>'
    0x38            // '/','?'
};

/**

  Handler function for I2C Keyboard's interrupt transfer.
  
  @param  Data                  A pointer to a buffer that is filled with key data which is
                                retrieved via asynchronous interrupt transfer.
  @param  DataLength            Indicates the size of the data buffer.
  @param  Context               Pointing to I2C_KB_DEV instance.
  @param  Result                Indicates the result of the asynchronous interrupt transfer.
  
  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

**/  
EFI_STATUS
EFIAPI
KeyboardHandler (
  IN  VOID              *Data,
  IN  UINTN             DataLength,
  IN  VOID              *Context,
  IN  UINT32            Result
  )
{
  I2C_KB_DEV            *KbDev;
  UINT8                 *CurKeyCodeBuffer;
  UINT8                 *OldKeyCodeBuffer;
  UINT8                 CurModifierMap;
  UINT8                 OldModifierMap;
  UINT8                 Index;
  UINT8                 Index2;
  BOOLEAN               Down;
  EFI_STATUS            Status;
  BOOLEAN               KeyRelease;
  BOOLEAN               KeyPress;
  UINT8                 SavedTail;
  KEY                   Key;
  UINT8                 NewRepeatKey;

  if (DataLength == 0 || Data == NULL) {
    return EFI_SUCCESS;
  }
  KbDev = (I2C_KB_DEV *) Context;
  NewRepeatKey      = 0;
  CurKeyCodeBuffer  = (UINT8 *) Data;
  OldKeyCodeBuffer  = KbDev->LastKeyCodeArray;
  //
  // checks for new key stroke.
  // if no new key got, return immediately.
  //
  for (Index = 0; Index < 8; Index++) {
    if (OldKeyCodeBuffer[Index] != CurKeyCodeBuffer[Index]) {
      break;
    }
  }
  if (Index == 8) {
    return EFI_SUCCESS;
  }
  //
  // Parse the modifier key
  //
  CurModifierMap  = CurKeyCodeBuffer[0];
  OldModifierMap  = OldKeyCodeBuffer[0];
  //
  // handle modifier key's pressing or releasing situation.
  //
  for (Index = 0; Index < 8; Index++) {

    if ((CurModifierMap & KB_Mod[Index].Mask) != (OldModifierMap & KB_Mod[Index].Mask)) {
      //
      // if current modifier key is up, then
      // CurModifierMap & KB_Mod[Index].Mask = 0;
      // otherwize it is a non-zero value.
      // Inserts the pressed modifier key into key buffer.
      //
      Down = (UINT8) (CurModifierMap & KB_Mod[Index].Mask);
      InsertKeyCode (KbDev, KB_Mod[Index].Key, Down);
    }
  }
  //
  // handle normal key's releasing situation
  //
  KeyRelease = FALSE;
  for (Index = 2; Index < 8; Index++) {

    if (!KBD_VALID_KEYCODE (OldKeyCodeBuffer[Index])) {
      continue;
    }

    KeyRelease = TRUE;
    for (Index2 = 2; Index2 < 8; Index2++) {

      if (!KBD_VALID_KEYCODE (CurKeyCodeBuffer[Index2])) {
        continue;
      }

      if (OldKeyCodeBuffer[Index] == CurKeyCodeBuffer[Index2]) {
        KeyRelease = FALSE;
        break;
      }
    }

    if (KeyRelease) {
      InsertKeyCode (
        KbDev,
        OldKeyCodeBuffer[Index],
        0
        );
      //
      // the original reapeat key is released.
      //
      if (OldKeyCodeBuffer[Index] == KbDev->RepeatKey) {
        KbDev->RepeatKey = 0;
      }
    }
  }
    
  //
  // original repeat key is released, cancel the repeat timer
  //
  if (KbDev->RepeatKey == 0 && KbDev->RepeatPollingHandle != NULL) {
    gBS->CloseEvent (KbDev->RepeatPollingHandle);
    KbDev->RepeatPollingHandle = NULL;
  }

  //
  // handle normal key's pressing situation
  //
  KeyPress = FALSE;
  for (Index = 2; Index < 8; Index++) {

    if (!KBD_VALID_KEYCODE (CurKeyCodeBuffer[Index])) {
      continue;
    }

    KeyPress = TRUE;
    for (Index2 = 2; Index2 < 8; Index2++) {

      if (!KBD_VALID_KEYCODE (OldKeyCodeBuffer[Index2])) {
        continue;
      }

      if (CurKeyCodeBuffer[Index] == OldKeyCodeBuffer[Index2]) {
        KeyPress = FALSE;
        break;
      }
    }

    if (KeyPress) {
      InsertKeyCode (KbDev, CurKeyCodeBuffer[Index], 1);
      //
      // NumLock/CapsLock/ScrollLock pressed
      //
      if (CurKeyCodeBuffer[Index] != 0x53 && CurKeyCodeBuffer[Index] != 0x39 && CurKeyCodeBuffer[Index] != 0x47) {
        NewRepeatKey = CurKeyCodeBuffer[Index];
      }
      //
      // do not repeat the original repeated key
      //
      KbDev->RepeatKey = 0;
    }
  }
  
  //
  // Update LastKeycodeArray[] buffer in the
  // Usb Keyboard Device data structure.
  //
  for (Index = 0; Index < 8; Index++) {
    KbDev->LastKeyCodeArray[Index] = CurKeyCodeBuffer[Index];
  }
  //
  // pre-process KeyboardBuffer, pop out the ctrl,alt,del key in sequence
  // and judge whether it will invoke reset event.
  //
  SavedTail = KbDev->KeyboardBuffer.bTail;
  Index     = KbDev->KeyboardBuffer.bHead;
  while (Index != SavedTail) {
    RemoveKeyCode (&(KbDev->KeyboardBuffer), &Key);
  
    switch (Key.KeyCode) {
    case 0xe1:
    case 0xe5:
      if (Key.Down) {
        KbDev->ShiftOn = 1;
      } else {
        KbDev->ShiftOn = 0;
      }
      break;
    
    case 0xe0:
    case 0xe4:
      if (Key.Down) {
        KbDev->CtrlOn = 1;
      } else {
        KbDev->CtrlOn = 0;
      }
      break;
  
    case 0xe2:
    case 0xe6:
      if (Key.Down) {
        KbDev->AltOn = 1;
      } else {
        KbDev->AltOn = 0;
      }
      break;
  
    //
    // Del Key Code
    //
    case 0x4c:
    case 0x63:
      if (Key.Down) {
        if (KbDev->CtrlOn && KbDev->AltOn) {
          gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, KEYBOARD_RESET_SIGNATURE);
        }
      }
      break;
  
    default:
      break;
    }
    
    //
    // insert the key back to the buffer.
    // so the key sequence will not be destroyed.
    //
    InsertKeyCode (
      KbDev,
      Key.KeyCode,
      Key.Down
      );
    Index = KbDev->KeyboardBuffer.bHead;
  }
  //
  // If have new key pressed, update the RepeatKey value, and set the
  // timer to repeate delay timer
  //
  if (NewRepeatKey != 0) {
    if (KbDev->RepeatPollingHandle) {
      gBS->CloseEvent (KbDev->RepeatPollingHandle);
      KbDev->RepeatPollingHandle = NULL;
    }
    //
    // sets trigger time to "Repeat Delay Time",
    // to trigger the repeat timer when the key is hold long
    // enough time.
    //
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    KeyboardRepeatHandler,
                    KbDev,
                    &KbDev->RepeatPollingHandle
                    );
    if (EFI_ERROR(Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = gBS->SetTimer (
                    KbDev->RepeatPollingHandle,
                    TimerRelative,
                    KBD_REPEAT_DELAY
                    );
    if (EFI_ERROR(Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    KbDev->RepeatKey = NewRepeatKey;
  }
  return EFI_SUCCESS;
}

/**

  Convert key code to KBC scan code
  
  @param  KbDev                 The I2C_KB_DEV instance.
  @param  Key                   Key code
  
  @retval KBC scan code

**/  
STATIC
UINT16
ConvertKbcScanCode (        
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             Key
  )
{
  UINT16 KbcScanCode;

  if (KbDev->AltOn) {
    KbcScanCode = KbcCodeTable[Key - 4][2];
  } else if (KbDev->CtrlOn) {
    KbcScanCode = KbcCodeTable[Key - 4][3];
  } else if (KbDev->ShiftOn) {
    KbcScanCode = KbcCodeTable[Key - 4][1];
  } else {
    KbcScanCode = KbcCodeTable[Key - 4][0];
    if (KbcScanCode == 0x57 || KbcScanCode == 0x58) {
      //
      // Convert F11/F12 to KBC converted code 
      //
      KbcScanCode = 0x85 + (KbcScanCode - 0x57);
    } else if (KbcScanCode >= 0x82) {
      //
      // Double code key
      //
      KbcScanCode &= ~0x80;
      KbcScanCode <<= 8;
      KbcScanCode |= 0xE0;
    }
  }
  return KbcScanCode;
}  

/**

  Retrieves a key character after parsing the raw data in keyboard buffer.
  
  @param  KbDev                 The I2C_KB_DEV instance.
  @param  KeyChar               Points to the Key character after key parsing.
  
  @retval EFI_SUCCESS           Success
  @retval EFI_NOT_READY         Device is not ready

**/  
EFI_STATUS
ParseKey (
  IN  I2C_KB_DEV        *KbDev,
  OUT  UINT8            *KeyChar
  )
{
  KEY Key;

  *KeyChar = 0;

  while (!IsKeyboardBufferEmpty (&KbDev->KeyboardBuffer)) {
    //
    // pops one raw data off.
    //
    RemoveKeyCode (&(KbDev->KeyboardBuffer), &Key);

    if (!Key.Down) {
      switch (Key.KeyCode) {

      case 0xe0:
        KbDev->LeftCtrlOn = 0;
        KbDev->CtrlOn = 0;
        break;
      case 0xe4:
        KbDev->RightCtrlOn = 0;
        KbDev->CtrlOn = 0;
        break;
      case 0xe1:
        KbDev->LeftShiftOn = 0;
        KbDev->ShiftOn = 0;
        break;
      case 0xe5:
        KbDev->RightShiftOn = 0;
        KbDev->ShiftOn = 0;
        break;
      case 0xe2:
        KbDev->LeftAltOn = 0;
        KbDev->AltOn = 0;
        break;
      case 0xe6:
        KbDev->RightAltOn = 0;
        KbDev->AltOn = 0;
        break;
      //
      // Logo release
      //
      case 0xe3:
        KbDev->LeftLogoOn = 0;
        break;
      case 0xe7:
        KbDev->RightLogoOn = 0;
        break;
      //
      // Menu key (App/Apps) release
      //
      case 0x65:
        KbDev->MenuKeyOn = 0;
        break;
      //
      // SysReq release
      //
      case 0x46:
        KbDev->SysReqOn = 0;
        break;
      default:
        break;
      }
      continue;
    }
    
    //
    // Analyzes key pressing situation
    //
    switch (Key.KeyCode) {

    case 0xe0:
      KbDev->LeftCtrlOn = 1;
      KbDev->CtrlOn = 1;
      break;
    case 0xe4:
      KbDev->RightCtrlOn = 1;
      KbDev->CtrlOn = 1;
      break;
    case 0xe1:
      KbDev->LeftShiftOn = 1;
      KbDev->ShiftOn = 1;
      break;
    case 0xe5:
      KbDev->RightShiftOn = 1;      
      KbDev->ShiftOn = 1;
      break;
    case 0xe2:
      KbDev->LeftAltOn = 1;
      KbDev->AltOn = 1;
      break;
    case 0xe6:
      KbDev->RightAltOn = 1;      
      KbDev->AltOn = 1;
      break;
    case 0xe3:
      KbDev->LeftLogoOn = 1;
      continue;
      break;
    case 0xe7:
      KbDev->RightLogoOn = 1;
      continue;
      break;

    case 0x53:
      if (KbDev->IsCsmEnabled) {
        if (!PS2_LED_UPDATING) {
          USB_LED_UPDATE;
          KbDev->NumLockOn ^= 1;
          SyncKbdLed (KbDev);
          USB_LED_UPDATED;
        }
      } else {
        KbDev->NumLockOn ^= 1;
        SyncKbdLed (KbDev);
      }
      continue;
      break;

    case 0x39:
      if (KbDev->IsCsmEnabled) {
        if (!PS2_LED_UPDATING) {
      	  USB_LED_UPDATE;
          KbDev->CapsOn ^= 1;
          SyncKbdLed (KbDev);
          USB_LED_UPDATED;
        }
      } else {
        KbDev->CapsOn ^= 1;
        SyncKbdLed (KbDev);
      }
      continue;
      break;

    case 0x47:
      if (KbDev->IsCsmEnabled) {
        if (!PS2_LED_UPDATING) {
      	  USB_LED_UPDATE;
          KbDev->ScrollOn ^= 1;
          SyncKbdLed (KbDev);
          USB_LED_UPDATED;
        }
      } else {
        KbDev->ScrollOn ^= 1;
        SyncKbdLed (KbDev);
      }
      continue;
      break;
    //
    // PrintScreen,Pause,Application,Power
    // keys are not valid EFI key
    //
    case 0x46:
      KbDev->SysReqOn = 1;
      continue;
      break;
    case 0x65:
      KbDev->MenuKeyOn = 1;
      continue;
      break;
    case 0x48:
      break;
    case 0x66:
      continue;
      break;

    default:
      break;
    }
    
    //
    // When encountered Del Key...
    //
    if (Key.KeyCode == 0x4c || Key.KeyCode == 0x63) {
      if (KbDev->CtrlOn && KbDev->AltOn) {
        gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, KEYBOARD_RESET_SIGNATURE);
      }
    }
    *KeyChar = Key.KeyCode;
    if ((KbDev->IsCsmEnabled) && (Key.Down) && (Key.KeyCode < 0xe0) && (Key.KeyCode != 0x46) && (Key.KeyCode != 0x48)) {
      //
      // Put the KBC scan code into EBDA+0x164. The PrntScrn and Pause will be filter out to sync with PS2 path
      //
      *(UINT16*)&(EBDA(EBDA_KEYBORD_SCAN_CODE)) = ConvertKbcScanCode (KbDev, Key.KeyCode);
    }
    return EFI_SUCCESS;
  }
  return EFI_NOT_READY;

}

/**

  Converts Keyboard code to EFI Scan Code.
  
  @param  KbDev                 The I2C_KB_DEV instance.
  @param  KeyChar               Indicates the key code that will be interpreted.    
  @param  Key                   A pointer to a buffer that is filled in with 
                                the keystroke information for the key that 
                                was pressed.

  @retval EFI_NOT_READY         Device is not ready
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
KeyCodeToEFIScanCode (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             KeyChar,
  OUT EFI_INPUT_KEY     *Key
  )
{
  UINT8 Index;

  if (!KBD_VALID_KEYCODE (KeyChar)) {
    return EFI_NOT_READY;
  }
  if ((KeyChar >= 0xe0) && (KeyChar <= 0xe7)) {
    //
    // For Partial Keystroke support
    //
    Key->ScanCode    = SCAN_NULL;
    Key->UnicodeChar = CHAR_NULL;
  } else {
    //
    // valid Key Code starts from 4
    //
    Index = (UINT8) (KeyChar - 4);
  
    if (Index >= KEYCODE_MAX_MAKE) {
      return EFI_NOT_READY;
    }
  
    //
    // Undefined entries from 0x74 to 0x7E
    //
    if (KeyChar > KEYCODE_MAX_MAKE) {
      Index = Index - 11;
    }
  
    Key->ScanCode = KeyConvertionTable[Index][0];
  
    if (KbDev->ShiftOn) {
  
      Key->UnicodeChar = KeyConvertionTable[Index][2];
      //
      // Need not return associated shift state if a class of printable characters that
      // are normally adjusted by shift modifiers. e.g. Shift Key + 'f' key = 'F'
      //
      if (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'Z') {
        KbDev->LeftShiftOn = 0;
        KbDev->RightShiftOn = 0;
      }
  
    } else {
  
      Key->UnicodeChar = KeyConvertionTable[Index][1];
    }
  
    if (KbDev->CapsOn) {
  
      if (Key->UnicodeChar >= 'a' && Key->UnicodeChar <= 'z') {
  
        Key->UnicodeChar = KeyConvertionTable[Index][2];
  
      } else if (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'Z') {
  
        Key->UnicodeChar = KeyConvertionTable[Index][1];
  
      }
    }
  
    //
    // Translate the CTRL-Alpha characters to their corresponding control value  
    // (ctrl-a = 0x0001 through ctrl-Z = 0x001A, ctrl-[, ctrl-\, ctrl-], ctrl-^ and ctrl-_)
    //
    if (KbDev->CtrlOn) {
      for (Index = 0; Index < sizeof (CtrlIgnoreKey); Index++) {
        if (KeyChar == CtrlIgnoreKey[Index]) {
          return EFI_NOT_READY;
        }
      }
      
      if (Key->UnicodeChar >= 'a' && Key->UnicodeChar <= 'z') {
        Key->UnicodeChar = Key->UnicodeChar - 'a' + 1;
      } else if (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'Z') {
        Key->UnicodeChar = Key->UnicodeChar - 'A' + 1;
      } else if (Key->UnicodeChar == '[' || Key->UnicodeChar == '{') {
        Key->ScanCode    = SCAN_ESC;
        Key->UnicodeChar = CHAR_NULL;
      } else if (Key->UnicodeChar == '\\' || Key->UnicodeChar == '|') {
        Key->UnicodeChar = 0x1C;
      } else if (Key->UnicodeChar == ']' || Key->UnicodeChar == '}') {
        Key->UnicodeChar = 0x1D;
      } else if (Key->UnicodeChar == '^' || Key->UnicodeChar == '6') {
        Key->UnicodeChar = 0x1E;
      } else if (Key->UnicodeChar == '_' || Key->UnicodeChar == '-') {
        Key->UnicodeChar = 0x1F;
      }
    }
  
    if (KeyChar >= 0x59 && KeyChar <= 0x63) {
  
      if (KbDev->NumLockOn && !KbDev->ShiftOn) {
  
        Key->ScanCode = SCAN_NULL;
  
      } else {
  
        Key->UnicodeChar = 0x00;
      }
    }
  }

  if (Key->UnicodeChar == 0 && Key->ScanCode == SCAN_NULL) {
    if (!KbDev->IsSupportPartialKey) {
      return EFI_NOT_READY;
    }
  }
  //
  // Save Shift/Toggle state
  //
  KbDev->KeyState.KeyShiftState  = EFI_SHIFT_STATE_VALID;
  KbDev->KeyState.KeyToggleState = EFI_TOGGLE_STATE_VALID;
  if (KbDev->LeftCtrlOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_LEFT_CONTROL_PRESSED;
  }
  if (KbDev->RightCtrlOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_RIGHT_CONTROL_PRESSED;
  }
  if (KbDev->LeftAltOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_LEFT_ALT_PRESSED;
  }
  if (KbDev->RightAltOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_RIGHT_ALT_PRESSED;
  }
  if (KbDev->LeftShiftOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_LEFT_SHIFT_PRESSED;
  }
  if (KbDev->RightShiftOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_RIGHT_SHIFT_PRESSED;
  }
  if (KbDev->LeftLogoOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_LEFT_LOGO_PRESSED;
  }
  if (KbDev->RightLogoOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_RIGHT_LOGO_PRESSED;
  }
  if (KbDev->MenuKeyOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_MENU_KEY_PRESSED;
  }
  if (KbDev->SysReqOn == 1) {
    KbDev->KeyState.KeyShiftState |= EFI_SYS_REQ_PRESSED;
  }  

  if (KbDev->ScrollOn == 1) {
    KbDev->KeyState.KeyToggleState |= EFI_SCROLL_LOCK_ACTIVE;
  }
  if (KbDev->NumLockOn == 1) {
    KbDev->KeyState.KeyToggleState |= EFI_NUM_LOCK_ACTIVE;
  }
  if (KbDev->CapsOn == 1) {
    KbDev->KeyState.KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
  }

  if (KbDev->IsSupportPartialKey) {
    KbDev->KeyState.KeyToggleState |= EFI_KEY_STATE_EXPOSED;
  }
  return EFI_SUCCESS;
}

/**

  Resets Keyboard Buffer.
  
  @param  KeyboardBuffer        Points to the Keyboard Buffer.
  
  @retval EFI_SUCCESS           Success

**/  
EFI_STATUS
InitKeyBuffer (
  OUT KB_BUFFER         *KeyboardBuffer
  )
{
  ZeroMem (KeyboardBuffer, sizeof (KB_BUFFER));

  KeyboardBuffer->bHead = KeyboardBuffer->bTail;

  return EFI_SUCCESS;
}

/**

  Check whether Keyboard buffer is empty.
  
  @param  KeyboardBuffer        Keyboard Buffer.
  
**/
BOOLEAN
IsKeyboardBufferEmpty (
  IN  KB_BUFFER         *KeyboardBuffer
  )
{
  //
  // meet FIFO empty condition
  //
  return (BOOLEAN) (KeyboardBuffer->bHead == KeyboardBuffer->bTail);
}

/**

  Check whether Keyboard buffer is full.
  
  @param  KeyboardBuffer        Keyboard Buffer.

**/
BOOLEAN
IsKeyboardBufferFull (
  IN  KB_BUFFER         *KeyboardBuffer
  )
{
  return (BOOLEAN)(((KeyboardBuffer->bTail + 1) % (MAX_KEY_ALLOWED + 1)) == 
                                                        KeyboardBuffer->bHead);
}

/**

  Sets monitor keys for SCU (SETUP_HOT_KEY / BOOT_MANAGER_HOT_KEY...)
  
  @param  KeyboardBuffer        Points to the Keyboard Buffer.
  @param  Key                   Key code
  
**/
VOID
SetMonitorKey (
  IN      I2C_KB_DEV    *KbDev,
  IN      UINT8         Key
  )
{
  UINT8  *FilterKeyList;
  UINT8  ModKey;
  UINTN  Index;
  UINTN  BitIndex;
  UINT16 KbcScanCode;
  //
  // Skip if no MonitorKey installed
  //
  if (EBDA(EBDA_MONITOR_KEY_TABLE) != 0x55 || Key >= 0xe0) return;
  //
  // Convert key code to KBC key code
  //
  KbcScanCode = ConvertKbcScanCode (KbDev, Key);
  if ((KbcScanCode & 0xff) == 0xe0) {
    //
    // Convert double code back to single code
    //
    KbcScanCode >>= 8;
    KbcScanCode |= 0x80;
  }
  //
  // Get FilterKeyList pointer
  //
  FilterKeyList = (UINT8*)(UINTN)((*(UINT16*)&(EBDA(EBDA_MONITOR_KEY_TABLE + 3)) << 4) + *(UINT16*)&(EBDA(EBDA_MONITOR_KEY_TABLE + 1)));
  for (Index = 0, BitIndex = 0; FilterKeyList[Index] != 0; Index += 3, BitIndex ++) {
    if (FilterKeyList[Index] == (UINT8)KbcScanCode) {
      //
      // Also checks Shift/Alt/Ctrl keys
      //
      if ((ModKey = FilterKeyList[Index + 1]) != 0) {
        if (((ModKey & 0x01) && !KbDev->ShiftOn) ||
            ((ModKey & 0x02) && !KbDev->AltOn) ||
            ((ModKey & 0x04) && !KbDev->CtrlOn)) {
          continue;
        }
      }
      *(UINT32*)&(EBDA(EBDA_MONITOR_KEY_TABLE + 5)) |= (1 << BitIndex);
      break;
    }
  }
}

/**

  Sync the control key status to CSM environment

  @param  KeyboardBuffer        Points to the Keyboard Buffer.
  @param  Key                   Key code
  @param  Down                  Special key

  @retval EFI_SUCCESS           Success  

**/
EFI_STATUS
SyncEfiKeyToCsmkey (
  IN      I2C_KB_DEV    *KbDev,
  IN      UINT8         KeyCode,
  IN      UINT8         Down
  )
{
  QUALIFIER_STAT *QualifierStat;
  
  QualifierStat = (QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
  
  switch (KeyCode) {
  case 0xe1:
    if (Down) {
      QualifierStat->LeftShift = 1;
    } else {
      QualifierStat->LeftShift = 0;
    }
    break;
  case 0xe5:
    if (Down) {
      QualifierStat->RightShift = 1;
    } else {
      QualifierStat->RightShift = 0;
    }
    break;
  
  case 0xe0:
  case 0xe4:
    if (Down) {
      QualifierStat->Ctrl = 1;
    } else {
      QualifierStat->Ctrl = 0;
    }
    break;

  case 0xe2:
  case 0xe6:
    if (Down) {
      QualifierStat->Alt = 1;
    } else {
      QualifierStat->Alt = 0;
    }
    break;
    
  default:
        break;
  }
      
  return  EFI_SUCCESS;
}

/**

  Sync the control key status to EFI environment

  @param  KeyboardBuffer        Points to the Keyboard Buffer.
  @param  Key                   Key code
  @param  Down                  Special key

  @retval EFI_SUCCESS           Success  

**/
EFI_STATUS
SyncCsmKeyToEfikey (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             KeyCode,
  IN  UINT8             Down
  )
{
  KB_BUFFER             *KeyboardBuffer;
  KEY                   Key;
  
  KeyboardBuffer = &KbDev->KeyboardBuffer;
  //
  // if keyboard buffer is full, throw the
  // first key out of the keyboard buffer.
  //
  if (IsKeyboardBufferFull (KeyboardBuffer)) {
    RemoveKeyCode (&KbDev->KeyboardBuffer, &Key);
  }
  KeyboardBuffer->buffer[KeyboardBuffer->bTail].KeyCode = KeyCode;
  KeyboardBuffer->buffer[KeyboardBuffer->bTail].Down    = Down;

  //
  // adjust the tail pointer of the FIFO keyboard buffer.
  //
  KeyboardBuffer->bTail = (UINT8) ((KeyboardBuffer->bTail + 1) % (MAX_KEY_ALLOWED + 1));
  return  EFI_SUCCESS;
}

/**

  Inserts a key code into keyboard buffer.
  
  @param  KeyboardBuffer        Points to the Keyboard Buffer.
  @param  Key                   Key code
  @param  Down                  Special key

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
InsertKeyCode (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             KeyCode,
  IN  UINT8             Down
  )
{
  KB_BUFFER             *KeyboardBuffer;
  KEY                   Key;
  //
  // Ignore LED update if it is updating by Int9
  //
  if (KbDev->IsCsmEnabled && PS2_LED_UPDATING && (KeyCode == 0x53 || KeyCode == 0x39 || KeyCode == 0x47)) {
    return EFI_SUCCESS;
  }
  KeyboardBuffer = &KbDev->KeyboardBuffer;
  //
  // if keyboard buffer is full, throw the
  // first key out of the keyboard buffer.
  //
  if (IsKeyboardBufferFull (KeyboardBuffer)) {
    RemoveKeyCode (&KbDev->KeyboardBuffer, &Key);
  }

  KeyboardBuffer->buffer[KeyboardBuffer->bTail].KeyCode = KeyCode;
  KeyboardBuffer->buffer[KeyboardBuffer->bTail].Down    = Down;

  //
  // adjust the tail pointer of the FIFO keyboard buffer.
  //
  KeyboardBuffer->bTail = (UINT8) ((KeyboardBuffer->bTail + 1) % (MAX_KEY_ALLOWED + 1));
  //
  // Sets monitor keys for SCU (SETUP_HOT_KEY / BOOT_MANAGER_HOT_KEY...)
  //
  if (KbDev->IsCsmEnabled) {
    SetMonitorKey(KbDev, KeyCode);
    if (KeyCode >= 0xe0) {
      SyncEfiKeyToCsmkey (KbDev, KeyCode, Down);
    }
  }
  return EFI_SUCCESS;
}

/**

  Pops a key code off from keyboard buffer.
  
  @param  KeyboardBuffer        Points to the Keyboard Buffer.
  @param  Key                   Points to the buffer that contains a key code.

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

**/  
EFI_STATUS
RemoveKeyCode (
  OUT  KB_BUFFER        *KeyboardBuffer,
  OUT  KEY              *Key
  )
{
  if (IsKeyboardBufferEmpty (KeyboardBuffer)) {
    return EFI_DEVICE_ERROR;
  }

  Key->KeyCode = KeyboardBuffer->buffer[KeyboardBuffer->bHead].KeyCode;
  Key->Down    = KeyboardBuffer->buffer[KeyboardBuffer->bHead].Down;

  //
  // adjust the head pointer of the FIFO keyboard buffer.
  //
  KeyboardBuffer->bHead = (UINT8) ((KeyboardBuffer->bHead + 1) % (MAX_KEY_ALLOWED + 1));

  return EFI_SUCCESS;
}

/**

  Timer handler for Repeat Key timer.
  
  @param  Context               Points to the I2C_KB_DEV instance.
  
**/  
VOID
KeyboardRepeatHandler (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context
  )
{
  EFI_STATUS  Status;
  I2C_KB_DEV  *KbDev;
  

  KbDev = (I2C_KB_DEV *) Context;
  //
  // Do nothing when there is no repeat key.
  //
  if (KbDev->RepeatKey != 0) {
    //
    // Inserts one Repeat key into keyboard buffer,
    //
    InsertKeyCode (
      KbDev,
      KbDev->RepeatKey,
      1
      );
     Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    KeyboardRepeatHandler,
                    KbDev,
                    &KbDev->RepeatPollingHandle
                    );
    if (EFI_ERROR(Status)) {
      return;
    }
    Status = gBS->SetTimer (
                    KbDev->RepeatPollingHandle,
                    TimerRelative,
                    KBD_REPEAT_RATE
                    );
    if (EFI_ERROR(Status)) {
      return;
    }
  }
}