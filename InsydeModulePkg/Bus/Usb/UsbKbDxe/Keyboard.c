/** @file
  Helper functions for USB Keyboard Driver

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

#include "Efikey.h"
#include "Keyboard.h"
//
// USB Key Code to Efi key mapping table
// Format:<efi scan code>, <unicode without shift>, <unicode with shift>
//
STATIC
UINT8 KeyConvertionTable[USB_KEYCODE_MAX_MAKE][3] = {
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
    SCAN_NULL,      '\\',     '|',      // 0x32   Keyboard US \ and |
    SCAN_NULL,      ';',      ':',      // 0x33
    SCAN_NULL,      '\'',     '"',      // 0x34
    SCAN_NULL,      '`',      '~',      // 0x35   Keyboard Grave Accent and Tlide
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
    SCAN_NULL,      '\\',     '|',      // 0x64   Keyboard Non-US \ and |
    SCAN_NULL,      0x00,     0x00,     // 0x65   Keyboard Application
    SCAN_NULL,      0x00,     0x00,     // 0x66   Keyboard Power
    SCAN_NULL,      '=' ,     '=' ,     // 0x67   Keypad =
    SCAN_F13,       0x00,     0x00,     // 0x68
    SCAN_F14,       0x00,     0x00,     // 0x69
    SCAN_F15,       0x00,     0x00,     // 0x6A
    SCAN_F16,       0x00,     0x00,     // 0x6B
    SCAN_F17,       0x00,     0x00,     // 0x6C
    SCAN_F18,       0x00,     0x00,     // 0x6D
    SCAN_F19,       0x00,     0x00,     // 0x6E
    SCAN_F20,       0x00,     0x00,     // 0x6F
    SCAN_F21,       0x00,     0x00,     // 0x70
    SCAN_F22,       0x00,     0x00,     // 0x71
    SCAN_F23,       0x00,     0x00,     // 0x72
    SCAN_F24,       0x00,     0x00,     // 0x73
    SCAN_MUTE,      0x00,     0x00,     // 0x7F
    SCAN_VOLUME_UP, 0x00,     0x00,     // 0x80
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
UINT8 KbcModCodeTable[] = {
    0x1D,           // 0x00  'Left Ctrl'
    0x2A,           // 0x01  'Left Shift'
    0x38,           // 0x02  'Left Alt'
    0xDB,           // 0x03  'Left Windows'     E0,5B           E0,DB
    0x9D,           // 0x04  'Right Ctrl'       E0,1D           E0,9D
    0x36,           // 0x05  'Right Shift'
    0xB8,           // 0x06  'Right Alt'        E0,38           E0,B8
    0xDC            // 0x07  'Right Windows'    E0,5C           E0,DC
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

STATIC
UINT8 AltIgnoreKey[] = {
    0x4C,           // 0x4C  'Delete Forward'   E0,53   
    0x52,           // 0x52  'Up Arrow'         E0,48           E0,C8
    0x51,           // 0x51  'Down Arrow'       E0,50           E0,D0
    0x50,           // 0x50  'Left Arrow'       E0,4B           E0,CB
    0x4F,           // 0x4F  'Right Arrow'      E0,4D           E0,CD  
    0x62            // 0x62  'keypad 0 Insert'
};

STATIC
UINT8 KbcCodeTableJp109[] = {
    0x73,           // 0x87  '\  _'        Key Number #56
    0x70,           // 0x88  'KANA/HIRA'   Key Number #133
    0x7D,           // 0x89  '\  |'        Key Number #14
    0x79,           // 0x8A  'HENKAN'      Key Number #132
    0x7B            // 0x8B  'MU-HENKAN'   Key Number #131
};

/**

  Get Next Item

  @param  StartPos              Start Position
  @param  EndPos                End Position
  @param  HidItem               HidItem to return

  @retval Position

**/
STATIC
UINT8 *
GetNextItem (
  IN  UINT8    *StartPos,
  IN  UINT8    *EndPos,
  OUT HID_ITEM *HidItem
  )
{
  UINT8 Temp;

  if ((EndPos - StartPos) <= 0) {
    return NULL;
  }

  Temp = *StartPos;
  StartPos++;
  //
  // bit 2,3
  //
  HidItem->Type = (UINT8) ((Temp >> 2) & 0x03);
  //
  // bit 4-7
  //
  HidItem->Tag = (UINT8) ((Temp >> 4) & 0x0F);

  if (HidItem->Tag == HID_ITEM_TAG_LONG) {
    //
    // Long Items are not supported by HID rev1.0,
    // although we try to parse it.
    //
    HidItem->Format = HID_ITEM_FORMAT_LONG;

    if ((EndPos - StartPos) >= 2) {
      HidItem->Size = *StartPos++;
      HidItem->Tag  = *StartPos++;

      if ((EndPos - StartPos) >= HidItem->Size) {
        HidItem->Data.LongData = StartPos;
        StartPos += HidItem->Size;
        return StartPos;
      }
    }
  } else {
    HidItem->Format = HID_ITEM_FORMAT_SHORT;
    //
    // bit 0, 1
    //
    HidItem->Size   = (UINT8) (Temp & 0x03);
    switch (HidItem->Size) {

    case 0:
      //
      // No data
      //
      return StartPos;

    case 1:
      //
      // One byte data
      //
      if ((EndPos - StartPos) >= 1) {
        HidItem->Data.U8 = *StartPos++;
        return StartPos;
      }

    case 2:
      //
      // Two byte data
      //
      if ((EndPos - StartPos) >= 2) {
        HidItem->Data.U16 = *(UINT16*)StartPos;
        StartPos += 2;
        return StartPos;
      }

    case 3:
      //
      // 4 byte data, adjust size
      //
      HidItem->Size++;
      if ((EndPos - StartPos) >= 4) {
        HidItem->Data.U32 = *(UINT32*)StartPos;
        StartPos += 4;
        return StartPos;
      }
    }
  }

  return NULL;
}

/**

  Get Item Data

  @param  HidItem               HID_ITEM

  @retval HidItem Data
  
**/
STATIC
UINT32
GetItemData (
  IN  HID_ITEM *HidItem
  )
{
  //
  // Get Data from HID_ITEM structure
  //
  switch (HidItem->Size) {

  case 1:
    return HidItem->Data.U8;

  case 2:
    return HidItem->Data.U16;

  case 4:
    return HidItem->Data.U32;
  }

  return 0;
}

/**

  Parse Report Descriptor to Finding LED support

  @param  ReportDescriptor      Report descriptor to parse
  @param  ReportSize            Report descriptor size

**/
STATIC
BOOLEAN
ParseReportToFindingLed (
  IN  UINT8                     *ReportDescriptor,
  IN  UINTN                     ReportSize
  )
{
  UINT8     *DescriptorEnd;
  UINT8     *ptr;
  HID_ITEM  HidItem;
  UINT8     CurrentUsagePage = 0;
  UINT8     CurrentUsage = 0;
  
  DescriptorEnd = ReportDescriptor + ReportSize;
  ptr           = GetNextItem (ReportDescriptor, DescriptorEnd, &HidItem);

  while (ptr != NULL) {
    if (HidItem.Format != HID_ITEM_FORMAT_SHORT) {
      //
      // Long Format Item is not supported at current HID revision
      //
      return 0;
    }
    switch (HidItem.Type) {

    case HID_ITEM_TYPE_GLOBAL:
      //
      // For global Item, parse global item
      //
      if (HidItem.Tag == HID_GLOBAL_ITEM_TAG_USAGE_PAGE) {
        CurrentUsagePage = (UINT8) GetItemData (&HidItem);
      }
      break;

    case HID_ITEM_TYPE_LOCAL:
      //
      // For Local Item, parse local item
      //
      if (HidItem.Tag == HID_LOCAL_ITEM_TAG_USAGE) {
        CurrentUsage = (UINT8) GetItemData (&HidItem);
      }
      //
      // Check Touch device by Usage Page is DIGITIZERS and Usage is Touch Screen,
      // or Usage Page is DIGITIZERS and Usage is PEN
      //
      if (CurrentUsagePage == HID_UP_LED) {
        return TRUE;
      }
      break;

    }
    ptr = GetNextItem (ptr, DescriptorEnd, &HidItem);
  }
  return FALSE;
}

/**

  Check is it support LED in this interface

  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  
**/  
STATIC
BOOLEAN
CheckLedExist (
  IN USB_KB_DEV   *UsbKeyboardDevice
  )
{
  EFI_STATUS                    Status;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  EFI_USB_HID_DESCRIPTOR        KeyboardHidDesc;
  UINT8                         *ReportDesc;
  BOOLEAN                       LedSupported = FALSE;
  //
  // Assuming Interface 0 is major interface and support LED
  //
  if (UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber == 0) return TRUE;
  //
  // Examining the LED page for LED support
  //
  UsbCore = UsbKeyboardDevice->UsbCore;
  UsbIo   = UsbKeyboardDevice->UsbIo;
  Status = UsbCore->UsbGetHidDescriptor (
                      UsbIo,
                      UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
                      &KeyboardHidDesc
                      );
  if (EFI_ERROR (Status) || KeyboardHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
    return FALSE;
  }
  Status = UsbCore->AllocateBuffer (
                      KeyboardHidDesc.HidClassDesc[0].DescriptorLength,
                      ALIGNMENT_32,
                      (VOID **)&ReportDesc
                      );
  if (EFI_ERROR(Status)) {                    
    return FALSE;
  }
  Status = UsbCore->UsbGetReportDescriptor (
                      UsbIo,
                      UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
                      KeyboardHidDesc.HidClassDesc[0].DescriptorLength,
                      ReportDesc
                      );
  if (!EFI_ERROR (Status)) {
    LedSupported = ParseReportToFindingLed (
                     ReportDesc,
                     KeyboardHidDesc.HidClassDesc[0].DescriptorLength
                     );
  }
  UsbCore->FreeBuffer (
             KeyboardHidDesc.HidClassDesc[0].DescriptorLength,
             ReportDesc
             );
  return LedSupported;
}

/**

  Uses USB I/O to check whether the device is a USB Keyboard device.

  @param  UsbIo                 Points to a USB I/O protocol instance.

**/ 
BOOLEAN
IsUsbKeyboard (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;

  //
  // Get the Default interface descriptor, currently we
  // assume it is interface 1
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (
                    UsbIo,
                    &InterfaceDescriptor
                    );

  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (InterfaceDescriptor.InterfaceClass == CLASS_HID &&
      InterfaceDescriptor.InterfaceSubClass == SUBCLASS_BOOT &&
      InterfaceDescriptor.InterfaceProtocol == PROTOCOL_KEYBOARD
      ) {

    return TRUE;
  }

  return FALSE;
}

/*++

  Initialize USB Keyboard device and all private data structures.
  
  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  
  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

--*/
EFI_STATUS
InitUsbKeyboard (
  IN USB_KB_DEV   *UsbKeyboardDevice
  )
{
  UINT8                 Protocol;
  EFI_USB_IO_PROTOCOL   *UsbIo;
  EFI_USB_CORE_PROTOCOL *UsbCore;

  UsbIo   = UsbKeyboardDevice->UsbIo;
  UsbCore = UsbKeyboardDevice->UsbCore;

  KbdReportStatusCode (
    UsbKeyboardDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST)
    );

  InitUsbKeyBuffer (&(UsbKeyboardDevice->KeyboardBuffer));
  //
  // Get current protocol
  //
  UsbCore->UsbGetProtocolRequest (
             UsbKeyboardDevice->UsbIo,
             UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
             &Protocol
             );
  //
  // Sets boot protocol for the USB Keyboard.
  //
  if (Protocol != BOOT_PROTOCOL) {
    UsbCore->UsbSetProtocolRequest (
               UsbKeyboardDevice->UsbIo,
               UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
               BOOT_PROTOCOL
               );
  }
  //
  // Idle forever until there is a key pressed and released.
  //
  UsbCore->UsbSetIdleRequest (
             UsbKeyboardDevice->UsbIo,
             UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
             0,
             0
             );

  UsbKeyboardDevice->CtrlOn     = 0;
  UsbKeyboardDevice->AltOn      = 0;
  UsbKeyboardDevice->ShiftOn    = 0;
  if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
    UsbKeyboardDevice->NumLockOn  = 0;
    UsbKeyboardDevice->CapsOn     = 0;
    UsbKeyboardDevice->ScrollOn   = 0;
  }
  UsbKeyboardDevice->LeftCtrlOn   = 0;
  UsbKeyboardDevice->LeftAltOn    = 0;
  UsbKeyboardDevice->LeftShiftOn  = 0;  
  UsbKeyboardDevice->LeftLogoOn   = 0;  
  UsbKeyboardDevice->RightCtrlOn  = 0;
  UsbKeyboardDevice->RightAltOn   = 0;
  UsbKeyboardDevice->RightShiftOn = 0;  
  UsbKeyboardDevice->RightLogoOn  = 0;  
  UsbKeyboardDevice->MenuKeyOn    = 0;
  UsbKeyboardDevice->SysReqOn     = 0;  
  UsbKeyboardDevice->IsSupportPartialKey = 0;
  //
  // Check is it support LED
  //
  UsbKeyboardDevice->LedSupported = CheckLedExist (UsbKeyboardDevice);
  if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
    //
    // Sync the initial state of LED with BDAs qualifier stat 
    //
    UsbKeyboardCheckLEDHandler(1, UsbKeyboardDevice);
  } else {
    SetKeyLED(UsbKeyboardDevice);
  }
  ZeroMem (UsbKeyboardDevice->LastKeyCodeArray, sizeof (UINT8) * 8);

  return EFI_SUCCESS;
}

/**

  Handler function for USB Keyboard's asynchronous interrupt transfer.
  
  @param  Data                  A pointer to a buffer that is filled with key data which is
                                retrieved via asynchronous interrupt transfer.
  @param  DataLength            Indicates the size of the data buffer.
  @param  Context               Pointing to USB_KB_DEV instance.
  @param  Result                Indicates the result of the asynchronous interrupt transfer.
  
  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

**/  
EFI_STATUS
EFIAPI
KeyboardHandler (
  IN  VOID          *Data,
  IN  UINTN         DataLength,
  IN  VOID          *Context,
  IN  UINT32        Result
  )
{
  USB_KB_DEV            *UsbKeyboardDevice;
  EFI_USB_IO_PROTOCOL   *UsbIo;
  EFI_USB_CORE_PROTOCOL *UsbCore;
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
  USB_KEY               UsbKey;
  UINT8                 NewRepeatKey;
  UINT32                UsbStatus;
  UINTN                 Mode;

  ASSERT (Context);

  NewRepeatKey      = 0;
  UsbKeyboardDevice = (USB_KB_DEV *) Context;
  UsbIo             = UsbKeyboardDevice->UsbIo;
  UsbCore           = UsbKeyboardDevice->UsbCore;
  //
  // Get current mode
  //
  UsbCore->GetMode(&Mode);
  //
  // Analyzes the Result and performs corresponding action.
  //
  if (Result != EFI_USB_NOERROR) {
    //
    // Some errors happen during the process
    //
    KbdReportStatusCode (
      UsbKeyboardDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_INPUT_ERROR)
      );
    //
    // stop the repeat key generation if any
    //
    UsbKeyboardDevice->RepeatKey = 0;
    
    if (UsbKeyboardDevice->RepeatPollingHandle) {
      UsbCore->RemovePeriodicTimer (
                 UsbKeyboardDevice->RepeatPollingHandle
                 );
      UsbKeyboardDevice->RepeatPollingHandle = NULL;
    }
    if ((Result & EFI_USB_ERR_STALL) == EFI_USB_ERR_STALL) {
      UsbCore->UsbClearEndpointHalt (
                 UsbIo,
                 UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress,
                 &UsbStatus
                 );
    }
    
    //
    // Delete & Submit this interrupt again
    //
    
    Status = UsbIo->UsbAsyncInterruptTransfer (
                      UsbIo,
                      UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress,
                      FALSE,
                      0,
                      0,
                      NULL,
                      NULL
                      );
    //
    // Check is the device be detached
    //
    if (UsbCore->CheckDeviceDetached(UsbIo) == EFI_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }
    UsbCore->InsertPeriodicTimer(
               USB_CORE_ONCE_TIMER,
               UsbKeyboardRecoveryHandler,
               UsbKeyboardDevice,
               EFI_USB_INTERRUPT_DELAY / 10000,
               &UsbKeyboardDevice->RecoveryPollingHandle
               );

    return EFI_DEVICE_ERROR;
  }

  if (DataLength == 0 || Data == NULL) {
    return EFI_SUCCESS;
  }

  CurKeyCodeBuffer  = (UINT8 *) Data;
  OldKeyCodeBuffer  = UsbKeyboardDevice->LastKeyCodeArray;

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
      InsertKeyCode (UsbKeyboardDevice, KB_Mod[Index].Key, Down);
    }
  }
  
  //
  // handle normal key's releasing situation
  //
  KeyRelease = FALSE;
  for (Index = 2; Index < 8; Index++) {

    if (!USBKBD_VALID_KEYCODE (OldKeyCodeBuffer[Index])) {
      continue;
    }

    KeyRelease = TRUE;
    for (Index2 = 2; Index2 < 8; Index2++) {

      if (!USBKBD_VALID_KEYCODE (CurKeyCodeBuffer[Index2])) {
        continue;
      }

      if (OldKeyCodeBuffer[Index] == CurKeyCodeBuffer[Index2]) {
        KeyRelease = FALSE;
        break;
      }
    }

    if (KeyRelease) {
      InsertKeyCode (
        UsbKeyboardDevice,
        OldKeyCodeBuffer[Index],
        0
        );
      //
      // the original reapeat key is released.
      //
      if (OldKeyCodeBuffer[Index] == UsbKeyboardDevice->RepeatKey) {
        UsbKeyboardDevice->RepeatKey = 0;
      }
    }
  }
    
  //
  // original repeat key is released, cancel the repeat timer
  //
  if (UsbKeyboardDevice->RepeatKey == 0 && UsbKeyboardDevice->RepeatPollingHandle != NULL) {
    UsbCore->RemovePeriodicTimer (
               UsbKeyboardDevice->RepeatPollingHandle
               );
    UsbKeyboardDevice->RepeatPollingHandle = NULL;
  }

  //
  // handle normal key's pressing situation
  //
  KeyPress = FALSE;
  for (Index = 2; Index < 8; Index++) {

    if (!USBKBD_VALID_KEYCODE (CurKeyCodeBuffer[Index])) {
      continue;
    }

    KeyPress = TRUE;
    for (Index2 = 2; Index2 < 8; Index2++) {

      if (!USBKBD_VALID_KEYCODE (OldKeyCodeBuffer[Index2])) {
        continue;
      }

      if (CurKeyCodeBuffer[Index] == OldKeyCodeBuffer[Index2]) {
        KeyPress = FALSE;
        break;
      }
    }

    if (KeyPress) {
      InsertKeyCode (UsbKeyboardDevice, CurKeyCodeBuffer[Index], 1);
      //
      // NumLock/CapsLock/ScrollLock pressed
      //
      if (CurKeyCodeBuffer[Index] != 0x53 && CurKeyCodeBuffer[Index] != 0x39 && CurKeyCodeBuffer[Index] != 0x47) {
        NewRepeatKey = CurKeyCodeBuffer[Index];
      }
      //
      // do not repeat the original repeated key
      //
      UsbKeyboardDevice->RepeatKey = 0;
    }
  }
  
  //
  // Update LastKeycodeArray[] buffer in the
  // Usb Keyboard Device data structure.
  //
  for (Index = 0; Index < 8; Index++) {
    UsbKeyboardDevice->LastKeyCodeArray[Index] = CurKeyCodeBuffer[Index];
  }
  
  UsbCore->GetMode(&Mode);
  if (Mode != USB_CORE_RUNTIME_MODE) {
    //
    // pre-process KeyboardBuffer, pop out the ctrl,alt,del key in sequence
    // and judge whether it will invoke reset event.
    //
    SavedTail = UsbKeyboardDevice->KeyboardBuffer.bTail;
    Index     = UsbKeyboardDevice->KeyboardBuffer.bHead;
    while (Index != SavedTail) {
      RemoveKeyCode (&(UsbKeyboardDevice->KeyboardBuffer), &UsbKey);
    
      switch (UsbKey.KeyCode) {
      case 0xe1:
      case 0xe5:
        if (UsbKey.Down) {
          UsbKeyboardDevice->ShiftOn = 1;
        } else {
          UsbKeyboardDevice->ShiftOn = 0;
        }
        break;
      
      case 0xe0:
      case 0xe4:
        if (UsbKey.Down) {
          UsbKeyboardDevice->CtrlOn = 1;
        } else {
          UsbKeyboardDevice->CtrlOn = 0;
        }
        break;
    
      case 0xe2:
      case 0xe6:
        if (UsbKey.Down) {
          UsbKeyboardDevice->AltOn = 1;
        } else {
          UsbKeyboardDevice->AltOn = 0;
        }
        break;
    
      //
      // Del Key Code
      //
      case 0x4c:
      case 0x63:
        if (UsbKey.Down) {
          if (UsbKeyboardDevice->CtrlOn && UsbKeyboardDevice->AltOn) {
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
        UsbKeyboardDevice,
        UsbKey.KeyCode,
        UsbKey.Down
        );
      Index = UsbKeyboardDevice->KeyboardBuffer.bHead;
    
    }
  }
  //
  // If have new key pressed, update the RepeatKey value, and set the
  // timer to repeate delay timer
  //
  if (NewRepeatKey != 0) {
    if (UsbKeyboardDevice->RepeatPollingHandle) {
      UsbCore->RemovePeriodicTimer(
                 UsbKeyboardDevice->RepeatPollingHandle
                 );
      UsbKeyboardDevice->RepeatPollingHandle = NULL;
    }
    //
    // sets trigger time to "Repeat Delay Time",
    // to trigger the repeat timer when the key is hold long
    // enough time.
    //
    UsbCore->InsertPeriodicTimer(
               USB_CORE_ONCE_TIMER,
               UsbKeyboardRepeatHandler,
               UsbKeyboardDevice,
               USBKBD_REPEAT_DELAY,
               &UsbKeyboardDevice->RepeatPollingHandle
               );
    UsbKeyboardDevice->RepeatKey = NewRepeatKey;
  }

  return EFI_SUCCESS;
}

/**

  Convert USB key code to KBC scan code
  
  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  @param  Key                   USB key code
  
  @retval KBC scan code

**/  
STATIC
UINT16
ConvertKbcScanCode (        
  IN USB_KB_DEV         *UsbKeyboardDevice,
  IN UINT8              Key
  )
{
  UINT16 KbcScanCode;

  if (Key < 4 || Key >= (sizeof (KbcCodeTable) / 4) + 4) return 0;
  if (UsbKeyboardDevice->AltOn) {
    KbcScanCode = KbcCodeTable[Key - 4][2];
  } else if (UsbKeyboardDevice->CtrlOn) {
    KbcScanCode = KbcCodeTable[Key - 4][3];
  } else if (UsbKeyboardDevice->ShiftOn) {
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
  
  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  @param  KeyChar               Points to the Key character after key parsing.
  
  @retval EFI_SUCCESS           Success
  @retval EFI_NOT_READY         Device is not ready

**/  
EFI_STATUS
UsbParseKey (
  IN OUT  USB_KB_DEV  *UsbKeyboardDevice,
  OUT     UINT8       *KeyChar
  )
{
  USB_KEY UsbKey;

  *KeyChar = 0;

  while (!IsUsbKeyboardBufferEmpty (&UsbKeyboardDevice->KeyboardBuffer)) {
    //
    // pops one raw data off.
    //
    RemoveKeyCode (&(UsbKeyboardDevice->KeyboardBuffer), &UsbKey);

    if (!UsbKey.Down) {
      switch (UsbKey.KeyCode) {

      case 0xe0:
        UsbKeyboardDevice->LeftCtrlOn = 0;
        UsbKeyboardDevice->CtrlOn = 0;
        break;
      case 0xe4:
        UsbKeyboardDevice->RightCtrlOn = 0;
        UsbKeyboardDevice->CtrlOn = 0;
        break;
      case 0xe1:
        UsbKeyboardDevice->LeftShiftOn = 0;
        UsbKeyboardDevice->ShiftOn = 0;
        break;
      case 0xe5:
        UsbKeyboardDevice->RightShiftOn = 0;
        UsbKeyboardDevice->ShiftOn = 0;
        break;
      case 0xe2:
        UsbKeyboardDevice->LeftAltOn = 0;
        UsbKeyboardDevice->AltOn = 0;
        break;
      case 0xe6:
        UsbKeyboardDevice->RightAltOn = 0;
        UsbKeyboardDevice->AltOn = 0;
        break;
      //
      // Logo release
      //
      case 0xe3:
        UsbKeyboardDevice->LeftLogoOn = 0;
        break;
      case 0xe7:
        UsbKeyboardDevice->RightLogoOn = 0;
        break;
      //
      // Menu key (App/Apps) release
      //
      case 0x65:
        UsbKeyboardDevice->MenuKeyOn = 0;
        break;
      //
      // SysReq release
      //
      case 0x46:
        UsbKeyboardDevice->SysReqOn = 0;
        break;
      default:
        break;
      }
      continue;
    }
    
    //
    // Analyzes key pressing situation
    //
    switch (UsbKey.KeyCode) {

    case 0xe0:
      UsbKeyboardDevice->LeftCtrlOn = 1;
      UsbKeyboardDevice->CtrlOn = 1;
      break;
    case 0xe4:
      UsbKeyboardDevice->RightCtrlOn = 1;
      UsbKeyboardDevice->CtrlOn = 1;
      break;
    case 0xe1:
      UsbKeyboardDevice->LeftShiftOn = 1;
      UsbKeyboardDevice->ShiftOn = 1;
      break;
    case 0xe5:
      UsbKeyboardDevice->RightShiftOn = 1;      
      UsbKeyboardDevice->ShiftOn = 1;
      break;
    case 0xe2:
      UsbKeyboardDevice->LeftAltOn = 1;
      UsbKeyboardDevice->AltOn = 1;
      break;
    case 0xe6:
      UsbKeyboardDevice->RightAltOn = 1;      
      UsbKeyboardDevice->AltOn = 1;
      break;
    case 0xe3:
      UsbKeyboardDevice->LeftLogoOn = 1;
      continue;
      break;
    case 0xe7:
      UsbKeyboardDevice->RightLogoOn = 1;
      continue;
      break;

    case 0x53:
      if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
        if (!PS2_LED_UPDATING) {
          USB_LED_UPDATE;
          UsbKeyboardDevice->NumLockOn ^= 1;
          SetKeyLED (UsbKeyboardDevice);
          if (UsbKeyboardDevice->UsbCore->IsKbcExist() == EFI_NOT_FOUND) {
            UsbKeyboardDevice->UsbCore->SyncKbdLed();
          }
          USB_LED_UPDATED;
        }
      } else {
        UsbKeyboardDevice->NumLockOn ^= 1;
        UsbKeyboardCheckLEDHandler(0, UsbKeyboardDevice);
      }
      continue;
      break;

    case 0x39:
      if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
        if (!PS2_LED_UPDATING) {
      	  USB_LED_UPDATE;
          UsbKeyboardDevice->CapsOn ^= 1;
          SetKeyLED (UsbKeyboardDevice);
          if (UsbKeyboardDevice->UsbCore->IsKbcExist() == EFI_NOT_FOUND) {
            UsbKeyboardDevice->UsbCore->SyncKbdLed();
          }
          USB_LED_UPDATED;
        }
      } else {
        UsbKeyboardDevice->CapsOn ^= 1;
        UsbKeyboardCheckLEDHandler(0, UsbKeyboardDevice);
      }
      continue;
      break;

    case 0x47:
      if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
        if (!PS2_LED_UPDATING) {
      	  USB_LED_UPDATE;
          UsbKeyboardDevice->ScrollOn ^= 1;
          SetKeyLED (UsbKeyboardDevice);
          if (UsbKeyboardDevice->UsbCore->IsKbcExist() == EFI_NOT_FOUND) {
            UsbKeyboardDevice->UsbCore->SyncKbdLed();
          }
          USB_LED_UPDATED;
        }
      } else {
        UsbKeyboardDevice->ScrollOn ^= 1;
        UsbKeyboardCheckLEDHandler(0, UsbKeyboardDevice);
      }
      continue;
      break;
    //
    // PrintScreen,Pause,Application,Power
    // keys are not valid EFI key
    //
    case 0x46:
      UsbKeyboardDevice->SysReqOn = 1;
      continue;
      break;
    case 0x65:
      UsbKeyboardDevice->MenuKeyOn = 1;
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
    if (UsbKey.KeyCode == 0x4c || UsbKey.KeyCode == 0x63) {
      if (UsbKeyboardDevice->CtrlOn && UsbKeyboardDevice->AltOn) {
        gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, KEYBOARD_RESET_SIGNATURE);
        //
        // Return EFI_NOT_READY in case some OEM feature ignore the reset in purposely through hook the ResetSystem function
        //
        return EFI_NOT_READY;
      }
    }

    *KeyChar = UsbKey.KeyCode;
    if ((UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) && (UsbKey.Down) && (UsbKey.KeyCode < 0xe0) && (UsbKey.KeyCode != 0x46) && (UsbKey.KeyCode != 0x48)) {
      //
      // Put the KBC scan code into EBDA+0x164. The PrntScrn and Pause will be filter out to sync with PS2 path
      //
      *(UINT16*)&(EBDA(EBDA_KEYBORD_SCAN_CODE)) = ConvertKbcScanCode (UsbKeyboardDevice, UsbKey.KeyCode);
    }
    return EFI_SUCCESS;
  }

  return EFI_NOT_READY;

}

/**

  Converts USB Keyboard code to EFI Scan Code.
  
  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  @param  KeyChar               Indicates the key code that will be interpreted.    
  @param  Key                   A pointer to a buffer that is filled in with 
                                the keystroke information for the key that 
                                was pressed.

  @retval EFI_NOT_READY         Device is not ready
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
UsbKeyCodeToEFIScanCode (
  IN  USB_KB_DEV      *UsbKeyboardDevice,
  IN  UINT8           KeyChar,
  OUT EFI_INPUT_KEY   *Key
  )
{
  UINT8 Index;

  if (!USBKBD_VALID_KEYCODE (KeyChar)) {
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
    // valid USB Key Code starts from 4
    //
    Index = (UINT8) (KeyChar - 4);
  
    if (Index >= USB_KEYCODE_MAX_MAKE) {
      return EFI_NOT_READY;
    }
  
    //
    // Undefined entries from 0x74 to 0x7E
    //
    if (KeyChar > USB_KEYCODE_MAX_MAKE) {
      Index = Index - 11;
    }
  
    Key->ScanCode = KeyConvertionTable[Index][0];
  
    if (UsbKeyboardDevice->ShiftOn) {
  
      Key->UnicodeChar = KeyConvertionTable[Index][2];
      //
      // Need not return associated shift state if a class of printable characters that
      // are normally adjusted by shift modifiers. e.g. Shift Key + 'f' key = 'F'
      //
      if (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'Z') {
        UsbKeyboardDevice->LeftShiftOn = 0;
        UsbKeyboardDevice->RightShiftOn = 0;
      }
  
    } else {
  
      Key->UnicodeChar = KeyConvertionTable[Index][1];
    }
  
    if (UsbKeyboardDevice->CapsOn) {
  
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
    if (UsbKeyboardDevice->CtrlOn) {
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
  
      if (UsbKeyboardDevice->NumLockOn && !UsbKeyboardDevice->ShiftOn) {
  
        Key->ScanCode = SCAN_NULL;
  
      } else {
  
        Key->UnicodeChar = 0x00;
      }
    }

    if (UsbKeyboardDevice->AltOn) {
      for (Index = 0; Index < sizeof (AltIgnoreKey); Index++) {
        if (KeyChar == AltIgnoreKey[Index]) {
          return EFI_NOT_READY;
        }
      }

      //
      // Alt + numpad 1~9
      //
      if (KeyChar >= 0x59 && KeyChar <= 0x61) {
        Index = (UINT8) (KeyChar - 4);
        Key->UnicodeChar = KeyConvertionTable[Index][1];

        if (Key->UnicodeChar >= '1' && Key->UnicodeChar <= '9') {
          Key->UnicodeChar = Key->UnicodeChar - '1' + 1;
        }
      }  

      //
      // Alt + (Up, Down, Left, Right)
      //
      if (Key->ScanCode >= SCAN_UP && Key->ScanCode <= SCAN_PAGE_DOWN) {
        Key->ScanCode = SCAN_NULL;       
      }
    }

  }

  if (Key->UnicodeChar == 0 && Key->ScanCode == SCAN_NULL) {
    if (!UsbKeyboardDevice->IsSupportPartialKey) {
      return EFI_NOT_READY;
    }
  }
  //
  // Save Shift/Toggle state
  //
  UsbKeyboardDevice->KeyState.KeyShiftState  = EFI_SHIFT_STATE_VALID;
  UsbKeyboardDevice->KeyState.KeyToggleState = EFI_TOGGLE_STATE_VALID;
  if (UsbKeyboardDevice->LeftCtrlOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_LEFT_CONTROL_PRESSED;
  }
  if (UsbKeyboardDevice->RightCtrlOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_RIGHT_CONTROL_PRESSED;
  }
  if (UsbKeyboardDevice->LeftAltOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_LEFT_ALT_PRESSED;
  }
  if (UsbKeyboardDevice->RightAltOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_RIGHT_ALT_PRESSED;
  }
  if (UsbKeyboardDevice->LeftShiftOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_LEFT_SHIFT_PRESSED;
  }
  if (UsbKeyboardDevice->RightShiftOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_RIGHT_SHIFT_PRESSED;
  }
  if (UsbKeyboardDevice->LeftLogoOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_LEFT_LOGO_PRESSED;
  }
  if (UsbKeyboardDevice->RightLogoOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_RIGHT_LOGO_PRESSED;
  }
  if (UsbKeyboardDevice->MenuKeyOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_MENU_KEY_PRESSED;
  }
  if (UsbKeyboardDevice->SysReqOn == 1) {
    UsbKeyboardDevice->KeyState.KeyShiftState |= EFI_SYS_REQ_PRESSED;
  }  

  if (UsbKeyboardDevice->ScrollOn == 1) {
    UsbKeyboardDevice->KeyState.KeyToggleState |= EFI_SCROLL_LOCK_ACTIVE;
  }
  if (UsbKeyboardDevice->NumLockOn == 1) {
    UsbKeyboardDevice->KeyState.KeyToggleState |= EFI_NUM_LOCK_ACTIVE;
  }
  if (UsbKeyboardDevice->CapsOn == 1) {
    UsbKeyboardDevice->KeyState.KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
  }

  if (UsbKeyboardDevice->IsSupportPartialKey) {
    UsbKeyboardDevice->KeyState.KeyToggleState |= EFI_KEY_STATE_EXPOSED;
  }
  return EFI_SUCCESS;
}

/**

  Resets USB Keyboard Buffer.
  
  @param  KeyboardBuffer        Points to the USB Keyboard Buffer.
  
  @retval EFI_SUCCESS           Success

**/  
EFI_STATUS
InitUsbKeyBuffer (
  IN OUT  USB_KB_BUFFER   *KeyboardBuffer
  )
{
  ZeroMem (KeyboardBuffer, sizeof (USB_KB_BUFFER));

  KeyboardBuffer->bHead = KeyboardBuffer->bTail;

  return EFI_SUCCESS;
}

/**

  Check whether USB Keyboard buffer is empty.
  
  @param  KeyboardBuffer        USB Keyboard Buffer.
  
**/
BOOLEAN
IsUsbKeyboardBufferEmpty (
  IN  USB_KB_BUFFER   *KeyboardBuffer
  )
{
  //
  // meet FIFO empty condition
  //
  return (BOOLEAN) (KeyboardBuffer->bHead == KeyboardBuffer->bTail);
}

/**

  Check whether USB Keyboard buffer is full.
  
  @param  KeyboardBuffer        USB Keyboard Buffer.

**/
BOOLEAN
IsUsbKeyboardBufferFull (
  IN  USB_KB_BUFFER   *KeyboardBuffer
  )
{
  return (BOOLEAN)(((KeyboardBuffer->bTail + 1) % (MAX_KEY_ALLOWED + 1)) == 
                                                        KeyboardBuffer->bHead);
}

/**

  Sets monitor keys for SCU (SETUP_HOT_KEY / BOOT_MANAGER_HOT_KEY...)
  
  @param  KeyboardBuffer        Points to the USB Keyboard Buffer.
  @param  Key                   Key code
  
**/
VOID
SetMonitorKey (
  IN      USB_KB_DEV    *UsbKeyboardDevice,
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
  // Convert USB key code to KBC key code
  //
  KbcScanCode = ConvertKbcScanCode (UsbKeyboardDevice, Key);
  if ((KbcScanCode & 0xff) == 0xe0) {
    //
    // To synchronize with CSM16, covert the scan code of "Enter KP" (0xe0, 0x1c) or "/ KP" (0xe0, 0x35)
    // to invalid code (0xe0) and needn't covert other double code to single code.
    //
    KbcScanCode >>= 8;
    if ((UINT8) KbcScanCode == 0x1c || (UINT8) KbcScanCode == 0x35) {
      KbcScanCode = 0xe0;
    }
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
        if (((ModKey & 0x01) && !UsbKeyboardDevice->ShiftOn) ||
            ((ModKey & 0x02) && !UsbKeyboardDevice->AltOn) ||
            ((ModKey & 0x04) && !UsbKeyboardDevice->CtrlOn)) {
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

  @param  KeyboardBuffer        Points to the USB Keyboard Buffer.
  @param  Key                   Key code
  @param  Down                  Special key

  @retval EFI_SUCCESS           Success  

**/
EFI_STATUS
SyncEfiKeyToCsmkey (
  IN      USB_KB_DEV    *UsbKeyboardDevice,
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

  @param  KeyboardBuffer        Points to the USB Keyboard Buffer.
  @param  Key                   Key code
  @param  Down                  Special key

  @retval EFI_SUCCESS           Success  

**/
EFI_STATUS
SyncCsmKeyToEfikey (
  IN      USB_KB_DEV    *UsbKeyboardDevice,
  IN      UINT8         KeyCode,
  IN      UINT8         Down
  )
{
  USB_KB_BUFFER         *KeyboardBuffer;
  USB_KEY               UsbKey;
  
  KeyboardBuffer = &UsbKeyboardDevice->KeyboardBuffer;
  //
  // if keyboard buffer is full, throw the
  // first key out of the keyboard buffer.
  //
  if (IsUsbKeyboardBufferFull (KeyboardBuffer)) {
    RemoveKeyCode (&UsbKeyboardDevice->KeyboardBuffer, &UsbKey);
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
  
  @param  KeyboardBuffer        Points to the USB Keyboard Buffer.
  @param  Key                   Key code
  @param  Down                  Special key

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
InsertKeyCode (
  IN      USB_KB_DEV    *UsbKeyboardDevice,
  IN      UINT8         Key,
  IN      UINT8         Down
  )
{
  USB_KB_BUFFER         *KeyboardBuffer;
  USB_KEY               UsbKey;
  UINT8                 KbcKeyCode[4];
  EFI_USB_CORE_PROTOCOL *UsbCore;
  UINTN                 Mode;
  EFI_STATUS            IsCsmEnabled;
  
  if (!USBKBD_VALID_KEYCODE (Key)) return EFI_NOT_FOUND;
  UsbCore = UsbKeyboardDevice->UsbCore;
  UsbCore->GetMode(&Mode);
  IsCsmEnabled = UsbCore->IsCsmEnabled();
  //
  // Ignore LED update if it is updating by Int9
  //
  if (IsCsmEnabled == EFI_SUCCESS && PS2_LED_UPDATING && (Key == 0x53 || Key == 0x39 || Key == 0x47)) {
    return EFI_SUCCESS;
  }
  if ((IsCsmEnabled != EFI_SUCCESS) || (Mode != USB_CORE_RUNTIME_MODE && EBDA(EBDA_OPROM_PROCESSING_FLAG) == 0 && !PAUSE_IN_PROGRESS && (UsbCore->IsKbcExist() == EFI_NOT_FOUND || Key != 0x48))) {
    KeyboardBuffer = &UsbKeyboardDevice->KeyboardBuffer;
    //
    // if keyboard buffer is full, throw the
    // first key out of the keyboard buffer.
    //
    if (IsUsbKeyboardBufferFull (KeyboardBuffer)) {
      RemoveKeyCode (&UsbKeyboardDevice->KeyboardBuffer, &UsbKey);
    }

    KeyboardBuffer->buffer[KeyboardBuffer->bTail].KeyCode = Key;
    KeyboardBuffer->buffer[KeyboardBuffer->bTail].Down    = Down;

    //
    // adjust the tail pointer of the FIFO keyboard buffer.
    //
    KeyboardBuffer->bTail = (UINT8) ((KeyboardBuffer->bTail + 1) % (MAX_KEY_ALLOWED + 1));
    //
    // Sets monitor keys for SCU (SETUP_HOT_KEY / BOOT_MANAGER_HOT_KEY...)
    //
    if (UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
      SetMonitorKey(UsbKeyboardDevice, Key);
      if (Key >= 0xe0) {
        SyncEfiKeyToCsmkey (UsbKeyboardDevice, Key, Down);
      }
    }
  } else {
    //
    // Insert KBC code for legacy mode
    //
    if (Key >= 0xe0) {
      if (Mode != USB_CORE_RUNTIME_MODE) {
        SyncCsmKeyToEfikey (UsbKeyboardDevice, Key, Down);
      }
      KbcKeyCode[0] = KbcModCodeTable[Key - 0xe0];
    } else if (Key >= 4 && Key < (sizeof (KbcCodeTable) / 4) + 4) {
      KbcKeyCode[0] = KbcCodeTable[Key - 4][0];
    } else if (Key >= 0x87 && Key < 0x8C) {
      KbcKeyCode[0] = KbcCodeTableJp109[Key - 0x87];
    }
    if (!(KbcKeyCode[0] & 0x80)) {
      //
      // Single code key
      //
      if (!Down) KbcKeyCode[0] |= 0x80;
      UsbCore->InsertKbcKeyCode(USB_CORE_KEYBOARD_CODE, KbcKeyCode, 1);
    } else if (KbcKeyCode[0] == 0x80) {
      //
      // PrntScrn
      //
      *(UINT32*)KbcKeyCode = (Down) ? 0x37E02AE0 : 0xAAE0B7E0;
      UsbCore->InsertKbcKeyCode(USB_CORE_KEYBOARD_CODE, KbcKeyCode, 4);
    } else if (KbcKeyCode[0] == 0x81) {
      //
      // Pause
      //
      *(UINT32*)KbcKeyCode = (Down) ? 0x00451DE1 : 0x00C59DE1;
      UsbCore->InsertKbcKeyCode(USB_CORE_KEYBOARD_CODE, KbcKeyCode, 3);
    } else {
      //
      // Double code key
      //
      if (Down) KbcKeyCode[0] &= ~0x80;
      KbcKeyCode[1] = KbcKeyCode[0];
      KbcKeyCode[0] = 0xE0;
      UsbCore->InsertKbcKeyCode(USB_CORE_KEYBOARD_CODE, KbcKeyCode, 2);
    }
  }
  return EFI_SUCCESS;
}

/**

  Pops a key code off from keyboard buffer.
  
  @param  KeyboardBuffer        Points to the USB Keyboard Buffer.
  @param  UsbKey                Points to the buffer that contains a usb key code.

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

**/  
EFI_STATUS
RemoveKeyCode (
  IN OUT  USB_KB_BUFFER *KeyboardBuffer,
  OUT     USB_KEY       *UsbKey
  )
{
  if (IsUsbKeyboardBufferEmpty (KeyboardBuffer)) {
    return EFI_DEVICE_ERROR;
  }

  UsbKey->KeyCode = KeyboardBuffer->buffer[KeyboardBuffer->bHead].KeyCode;
  UsbKey->Down    = KeyboardBuffer->buffer[KeyboardBuffer->bHead].Down;

  //
  // adjust the head pointer of the FIFO keyboard buffer.
  //
  KeyboardBuffer->bHead = (UINT8) ((KeyboardBuffer->bHead + 1) % (MAX_KEY_ALLOWED + 1));

  return EFI_SUCCESS;
}

/**

  Sets USB Keyboard LED state.
  
  @param  UsbKeyboardDevice     The USB_KB_DEV instance.

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
SetKeyLED (
  IN  USB_KB_DEV    *UsbKeyboardDevice
  )
{
  LED_MAP        Led;
  QUALIFIER_STAT *QualifierStat;

  //
  // Set each field in Led map.
  //
  Led.NumLock    = (UINT8) UsbKeyboardDevice->NumLockOn;
  Led.CapsLock   = (UINT8) UsbKeyboardDevice->CapsOn;
  Led.ScrollLock = (UINT8) UsbKeyboardDevice->ScrollOn;
  Led.Resrvd     = 0;
  //
  // Make synchronize with BDA qualifier stat
  //
  if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
    QualifierStat = (QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
    QualifierStat->ScrollLock = Led.ScrollLock;
    QualifierStat->NumLock    = Led.NumLock;
    QualifierStat->CapsLock   = Led.CapsLock;
  }
  //
  // Only lighten LED if supported
  //
  if (UsbKeyboardDevice->LedSupported) {
    //
    // call Set Report Request to lighten the LED.
    //
    UsbKeyboardDevice->UsbCore->UsbSetReportRequest (
                                  UsbKeyboardDevice->UsbIo,
                                  UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
                                  0,
                                  HID_OUTPUT_REPORT,
                                  1,
                                  (UINT8 *) &Led
                                  );
  }
  return EFI_SUCCESS;
}

/**

  Timer handler for Repeat Key timer.
  
  @param  Context               Points to the USB_KB_DEV instance.
  
**/  
VOID
EFIAPI
UsbKeyboardRepeatHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  )
{
  USB_KB_DEV  *UsbKeyboardDevice;

  UsbKeyboardDevice = (USB_KB_DEV *) Context;
  //
  // Do nothing when there is no repeat key.
  //
  if (UsbKeyboardDevice->RepeatKey != 0) {
    //
    // Inserts one Repeat key into keyboard buffer,
    //
    InsertKeyCode (
      UsbKeyboardDevice,
      UsbKeyboardDevice->RepeatKey,
      1
      );
    //
    // set repeate rate for repeat key generation.
    //
    UsbKeyboardDevice->UsbCore->InsertPeriodicTimer(
                         USB_CORE_ONCE_TIMER,
                         UsbKeyboardRepeatHandler,
                         UsbKeyboardDevice,
                         USBKBD_REPEAT_RATE,
                         &UsbKeyboardDevice->RepeatPollingHandle
                         );
  }
}

/**

  Timer handler for Delayed Recovery timer.
  
  @param  Context               Points to the USB_KB_DEV instance.
  
**/
VOID
EFIAPI
UsbKeyboardRecoveryHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  )
{

  USB_KB_DEV          *UsbKeyboardDevice;
  EFI_USB_IO_PROTOCOL *UsbIo;
  UINT8               PacketSize;

  UsbKeyboardDevice = (USB_KB_DEV *) Context;

  UsbIo             = UsbKeyboardDevice->UsbIo;

  PacketSize        = (UINT8) (UsbKeyboardDevice->IntEndpointDescriptor.MaxPacketSize);

  UsbIo->UsbAsyncInterruptTransfer (
           UsbIo,
           UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress,
           TRUE,
           UsbKeyboardDevice->IntEndpointDescriptor.Interval,
           PacketSize,
           KeyboardHandler,
           UsbKeyboardDevice
           );
}

/**

  Timer handler for Repeat Key timer.
  
  @param  Context               Points to the USB_KB_DEV instance.
  
**/
VOID
EFIAPI
UsbKeyboardCheckLEDHandler (
  IN    UINTN        ForceSet,
  IN    VOID         *Context
  )
{
  USB_KB_DEV     *UsbKeyboardDevice;
  QUALIFIER_STAT QualifierStat;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *ConSplitterEx;
  EFI_KEY_TOGGLE_STATE                KeyToggleState = EFI_TOGGLE_STATE_VALID;
  EFI_STATUS                          Status;

  UsbKeyboardDevice = (USB_KB_DEV *) Context;
  if (UsbKeyboardDevice->UsbCore->IsCsmEnabled() == EFI_SUCCESS) {
    //
    // Get Qualifier Stat in BDA
    //
    QualifierStat = *(QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
    //
    // Make it synchronization
    //
    if (ForceSet || 
        QualifierStat.ScrollLock != UsbKeyboardDevice->ScrollOn ||
        QualifierStat.NumLock != UsbKeyboardDevice->NumLockOn ||
        QualifierStat.CapsLock != UsbKeyboardDevice->CapsOn) {
      UsbKeyboardDevice->ScrollOn = QualifierStat.ScrollLock;
      UsbKeyboardDevice->NumLockOn = QualifierStat.NumLock;
      UsbKeyboardDevice->CapsOn = QualifierStat.CapsLock;
      SetKeyLED(UsbKeyboardDevice);
    }
  } else {
    if (UsbKeyboardDevice->ScrollOn == 1) {
      KeyToggleState |= EFI_SCROLL_LOCK_ACTIVE;
    }
    if (UsbKeyboardDevice->NumLockOn == 1) {
      KeyToggleState |= EFI_NUM_LOCK_ACTIVE;
    }
    if (UsbKeyboardDevice->CapsOn == 1) {
      KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
    }

    if (gST->ConsoleInHandle != NULL) {
      Status = gBS->HandleProtocol (
                      gST->ConsoleInHandle,
                      &gEfiSimpleTextInputExProtocolGuid,
                      (VOID **)&ConSplitterEx
                      );
      if (!EFI_ERROR (Status)) {
        //
        // Update all of ConInEx device State.
        //      
        Status = ConSplitterEx->SetState (ConSplitterEx, &KeyToggleState);
        if (Status == EFI_SUCCESS) return;
      }
    }
    //
    // Update USB Keyboard State
    //
    UsbKeyboardDevice->SimpleInputEx.SetState (&UsbKeyboardDevice->SimpleInputEx, &KeyToggleState);
  }
}
