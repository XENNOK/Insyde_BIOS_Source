/** @file

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

#include "SetupMouse.h"
#include "KeyboardLayout.h"

typedef enum {
  STYLE_NORMAL,
  STYLE_LEFTSHIFT,
  STYLE_RIGHTSHIFT,  
  STYLE_CLOSE, 
  STYLE_TAB,
  STYLE_CAPS,
  STYLE_ENTER,
  STYLE_BACKSPACE
} KEY_STYLE;


//
// {font color:BGRA}, {Image Id}, {ScaleGrid}
//
KEY_STYLE_ARRAY mEnglishStyleList[] = {
  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_NORMAL), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_NORMAL), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_PRESS),  {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_LEFTSHIFT), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_LEFTSHIFT), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_LEFTSHIFT_PRESS), {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_RIGHTSHIFT), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_RIGHTSHIFT), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_RIGHTSHIFT_PRESS), {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_CLOSE), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_CLOSE), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_CLOSE_PRESS), {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_TAB), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_TAB), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_TAB_PRESS), {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_CAPS), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_CAPS), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_CAPS_PRESS), {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_ENTER), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_ENTER), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_ENTER_PRESS), {5,5,5,5}}
  },

  {
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_BACKSPACE), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_BACKSPACE), {5,5,5,5}},
    {{0x00, 0x00, 0x00, 0xFF}, IMAGE_TOKEN (IMAGE_KB_BACKSPACE_PRESS), {5,5,5,5}}
  },

};

KEY_MAP_ENTRY mEnglishEntryList[] = {
{STYLE_BACKSPACE,   0X8   , { 800 ,   64  , 885 , 115 }},
{STYLE_TAB,         0X9   , { 9   ,   123 , 94  , 175 }},
{STYLE_ENTER,       0XD   , { 783 ,   183 , 884 , 235 }},
{STYLE_CAPS,        0X14  , { 8   ,   183 , 110 , 235 }},
{STYLE_NORMAL,      0X1B  , { 9   ,   22  , 64  , 57  }},
{STYLE_NORMAL,      0X20  , { 265 ,   302 , 625 , 364 }},
{STYLE_NORMAL,      0X30  , { 619 ,   64  , 674 , 115 }},
{STYLE_NORMAL,      0X31  , { 68  ,   64  , 123 , 115 }},
{STYLE_NORMAL,      0X32  , { 130 ,   64  , 185 , 115 }},
{STYLE_NORMAL,      0X33  , { 193 ,   64  , 247 , 115 }},
{STYLE_NORMAL,      0X34  , { 253 ,   64  , 307 , 115 }},
{STYLE_NORMAL,      0X35  , { 314 ,   64  , 369 , 115 }},
{STYLE_NORMAL,      0X36  , { 376 ,   64  , 431 , 115 }},
{STYLE_NORMAL,      0X37  , { 436 ,   64  , 491 , 115 }},
{STYLE_NORMAL,      0X38  , { 497 ,   64  , 552 , 115 }},
{STYLE_NORMAL,      0X39  , { 558 ,   64  , 613 , 115 }},
{STYLE_NORMAL,      0X41  , { 114 ,   183 , 169 , 235 }},
{STYLE_NORMAL,      0X42  , { 386 ,   243 , 441 , 295 }},
{STYLE_NORMAL,      0X43  , { 264 ,   243 , 319 , 295 }},
{STYLE_NORMAL,      0X44  , { 236 ,   183 , 291 , 235 }},
{STYLE_NORMAL,      0X45  , { 220 ,   123 , 275 , 175 }},
{STYLE_NORMAL,      0X46  , { 297 ,   183 , 352 , 235 }},
{STYLE_NORMAL,      0X47  , { 358 ,   183 , 413 , 235 }},
{STYLE_NORMAL,      0X48  , { 419 ,   183 , 474 , 235 }},
{STYLE_NORMAL,      0X49  , { 526 ,   123 , 581 , 175 }},
{STYLE_NORMAL,      0X4A  , { 481 ,   183 , 536 , 235 }},
{STYLE_NORMAL,      0X4B  , { 542 ,   183 , 597 , 235 }},
{STYLE_NORMAL,      0X4C  , { 602 ,   183 , 657 , 235 }},
{STYLE_NORMAL,      0X4D  , { 509 ,   243 , 564 , 295 }},
{STYLE_NORMAL,      0X4E  , { 449 ,   243 , 504 , 295 }},
{STYLE_NORMAL,      0X4F  , { 588 ,   123 , 643 , 175 }},
{STYLE_NORMAL,      0X50  , { 648 ,   123 , 703 , 175 }},
{STYLE_NORMAL,      0X51  , { 98  ,   123 , 153 , 175 }},
{STYLE_NORMAL,      0X52  , { 281 ,   123 , 336 , 175 }},
{STYLE_NORMAL,      0X53  , { 174 ,   183 , 229 , 235 }},
{STYLE_NORMAL,      0X54  , { 342 ,   123 , 397 , 175 }},
{STYLE_NORMAL,      0X55  , { 465 ,   123 , 520 , 175 }},
{STYLE_NORMAL,      0X56  , { 323 ,   243 , 378 , 295 }},
{STYLE_NORMAL,      0X57  , { 159 ,   123 , 214 , 175 }},
{STYLE_NORMAL,      0X58  , { 203 ,   243 , 258 , 295 }},
{STYLE_NORMAL,      0X59  , { 403 ,   123 , 458 , 175 }},
{STYLE_NORMAL,      0X5A  , { 142 ,   243 , 197 , 295 }},
{STYLE_NORMAL,      0X70  , { 71  ,   22  , 126 , 57  }},
{STYLE_NORMAL,      0X71  , { 134 ,   22  , 189 , 57  }},
{STYLE_NORMAL,      0X72  , { 198 ,   22  , 253 , 57  }},
{STYLE_NORMAL,      0X73  , { 261 ,   22  , 316 , 57  }},
{STYLE_NORMAL,      0X74  , { 324 ,   22  , 379 , 57  }},
{STYLE_NORMAL,      0X75  , { 387 ,   22  , 442 , 57  }},
{STYLE_NORMAL,      0X76  , { 450 ,   22  , 505 , 57  }},
{STYLE_NORMAL,      0X77  , { 515 ,   22  , 570 , 57  }},
{STYLE_NORMAL,      0X78  , { 577 ,   22  , 632 , 57  }},
{STYLE_NORMAL,      0X79  , { 641 ,   22  , 696 , 57  }},
{STYLE_NORMAL,      0X7A  , { 703 ,   22  , 758 , 57  }},
{STYLE_NORMAL,      0X7B  , { 767 ,   22  , 822 , 57  }},
{STYLE_LEFTSHIFT,   0XA0  , { 8   ,   243 , 139 , 295 }},
{STYLE_RIGHTSHIFT,  0XA1  , { 754 ,   243 , 885 , 295 }},
{STYLE_NORMAL,      0XA2  , { 7   ,   302 , 92  , 364 }},
{STYLE_NORMAL,      0XA3  , { 631 ,   302 , 716 , 364 }},
{STYLE_NORMAL,      0XA4  , { 99  ,   304 , 169 , 364 }},
{STYLE_NORMAL,      0XA5  , { 721 ,   302 , 792 , 364 }},
{STYLE_NORMAL,      0XBA  , { 664 ,   183 , 719 , 235 }},
{STYLE_NORMAL,      0XBB  , { 743 ,   63  , 798 , 115 }},
{STYLE_NORMAL,      0XBC  , { 570 ,   243 , 625 , 295 }},
{STYLE_NORMAL,      0XBD  , { 681 ,   64  , 736 , 115 }},
{STYLE_NORMAL,      0XBE  , { 631 ,   243 , 686 , 295 }},
{STYLE_NORMAL,      0XBF  , { 693 ,   243 , 748 , 295 }},
{STYLE_NORMAL,      0XC0  , { 8   ,   64  , 63  , 115 }},
{STYLE_CLOSE,       0XC3  , { 800 ,   302 , 885 , 364 }},
{STYLE_NORMAL,      0XDB  , { 709 ,   123 , 764 , 175 }},
{STYLE_NORMAL,      0XDC  , { 831 ,   123 , 886 , 175 }},
{STYLE_NORMAL,      0XDD  , { 771 ,   123 , 826 , 175 }},
{STYLE_NORMAL,      0XDE  , { 725 ,   183 , 780 , 235 }},
};

UINT8 mKeyConvertionTable[][4] = {
  //
  // WinKey, ScanCode, Unicode, Shift,
  //
  {0x08,   SCAN_NULL, 0x08, 0x08}, // VK_BACK
  {0x09,   SCAN_NULL, 0x09, 0x09}, // VK_TAB
  {0x0d,   SCAN_NULL, 0x0d, 0x0d}, // VK_RETURN
  {0x14,   SCAN_NULL, 0x00, 0x00}, // VK_CAPITAL
  {0x1b,    SCAN_ESC, 0x00, 0x00}, // VK_ESC
  {0x20,   SCAN_NULL,  ' ',  ' '}, // VK_SPACE
  {0x25,   SCAN_LEFT, 0x00, 0x00}, // VK_LEFT
  {0x26,     SCAN_UP, 0x00, 0x00}, // VK_UP
  {0x27,  SCAN_RIGHT, 0x00, 0x00}, // VK_RIGHTT
  {0x28,   SCAN_DOWN, 0x00, 0x00}, // VK_DOWN
  {0x2e, SCAN_DELETE, 0x00, 0x00}, // VK_DELETETE
  {0x30,   SCAN_NULL,  '0',  ')'}, // 0
  {0x31,   SCAN_NULL,  '1',  '!'}, // 1
  {0x32,   SCAN_NULL,  '2',  '@'}, // 2
  {0x33,   SCAN_NULL,  '3',  '#'}, // 3
  {0x34,   SCAN_NULL,  '4',  '$'}, // 4
  {0x35,   SCAN_NULL,  '5',  '%'}, // 5
  {0x36,   SCAN_NULL,  '6',  '^'}, // 6
  {0x37,   SCAN_NULL,  '7',  '&'}, // 7
  {0x38,   SCAN_NULL,  '8',  '*'}, // 8
  {0x39,   SCAN_NULL,  '9',  '('}, // 9
  {0x41,   SCAN_NULL,  'a',  'A'}, // A
  {0x42,   SCAN_NULL,  'b',  'B'}, // B
  {0x43,   SCAN_NULL,  'c',  'C'}, // C
  {0x44,   SCAN_NULL,  'd',  'D'}, // D
  {0x45,   SCAN_NULL,  'e',  'E'}, // E
  {0x46,   SCAN_NULL,  'f',  'F'}, // F
  {0x47,   SCAN_NULL,  'g',  'G'}, // G
  {0x48,   SCAN_NULL,  'h',  'H'}, // H
  {0x49,   SCAN_NULL,  'i',  'I'}, // I
  {0x4a,   SCAN_NULL,  'j',  'J'}, // J
  {0x4b,   SCAN_NULL,  'k',  'K'}, // K
  {0x4c,   SCAN_NULL,  'l',  'L'}, // L
  {0x4d,   SCAN_NULL,  'm',  'M'}, // M
  {0x4e,   SCAN_NULL,  'n',  'N'}, // N
  {0x4f,   SCAN_NULL,  'o',  'O'}, // O
  {0x50,   SCAN_NULL,  'p',  'P'}, // P
  {0x51,   SCAN_NULL,  'q',  'Q'}, // Q
  {0x52,   SCAN_NULL,  'r',  'R'}, // R
  {0x53,   SCAN_NULL,  's',  'S'}, // S
  {0x54,   SCAN_NULL,  't',  'T'}, // T
  {0x55,   SCAN_NULL,  'u',  'U'}, // U
  {0x56,   SCAN_NULL,  'v',  'V'}, // V
  {0x57,   SCAN_NULL,  'w',  'W'}, // W
  {0x58,   SCAN_NULL,  'x',  'X'}, // X
  {0x59,   SCAN_NULL,  'y',  'Y'}, // Y
  {0x5a,   SCAN_NULL,  'z',  'Z'}, // Z
  {0x5b,   SCAN_NULL, 0x00, 0x00}, // VK_LWIN
  {0x5d,   SCAN_NULL, 0x00, 0x00}, // VK_APP
  {0xa0,   SCAN_NULL, 0x00, 0x00}, // VK_LSHIFT
  {0xa1,   SCAN_NULL, 0x00, 0x00}, // VK_RSHIFT
  {0xa2,   SCAN_NULL, 0x00, 0x00}, // VK_LCONTROL
  {0xa3,   SCAN_NULL, 0x00, 0x00}, // VK_DstRcONTROL
  {0xa4,   SCAN_NULL, 0x00, 0x00}, // VK_LMENU
  {0xa5,   SCAN_NULL, 0x00, 0x00}, // VK_RMENU
  {0xba,   SCAN_NULL,  ';',  ':'}, // VK_OEM_1      ';:' for US
  {0xbb,   SCAN_NULL,  '=',  '+'}, // VK_OEM_PLUS   '+' any country
  {0xbc,   SCAN_NULL,  ',',  '<'}, // VK_OEM_COMMA  ',' any country
  {0xbd,   SCAN_NULL,  '-',  '_'}, // VK_OEM_MINUS  '-' any country
  {0xbe,   SCAN_NULL,  '.',  '>'}, // VK_OEM_PERIOD '.' any country
  {0xbf,   SCAN_NULL,  '/',  '?'}, // VK_OEM_2      '/?' for US
  {0xc0,   SCAN_NULL,  '`',  '~'}, // VK_OEM_3      '`~' for US
  {0xc3,   SCAN_NULL, 0x00, 0x00}, // close
  {0xdb,   SCAN_NULL,  '[',  '{'}, // VK_OEM_4 '[{' for US
  {0xdc,   SCAN_NULL, '\\',  '|'}, // VK_OEM_5 '\|' for US
  {0xdd,   SCAN_NULL,  ']',  '}'}, // VK_OEM_6 ']}' for US
  {0xde,   SCAN_NULL, '\'',  '"'}, // VK_OEM_7 ''"' for US
  {0xe8,   SCAN_NULL, 0x00, 0x00}, // Fn
  {0x70,   SCAN_F1,   CHAR_NULL, 0x00}, // VK_F1
  {0x71,   SCAN_F2,   CHAR_NULL, 0x00}, // VK_F2
  {0x72,   SCAN_F3,   CHAR_NULL, 0x00}, // VK_F3
  {0x73,   SCAN_F4,   CHAR_NULL, 0x00}, // VK_F4
  {0x74,   SCAN_F5,   CHAR_NULL, 0x00}, // VK_F5
  {0x75,   SCAN_F6,   CHAR_NULL, 0x00}, // VK_F6
  {0x76,   SCAN_F7,   CHAR_NULL, 0x00}, // VK_F7
  {0x77,   SCAN_F8,   CHAR_NULL, 0x00}, // VK_F8
  {0x78,   SCAN_F9,   CHAR_NULL, 0x00}, // VK_F9
  {0x79,   SCAN_F10,  CHAR_NULL, 0x00}, // VK_F10
  {0x7a,   SCAN_F11,  CHAR_NULL, 0x00}, // VK_F11
  {0x7b,   SCAN_F12,  CHAR_NULL, 0x00}, // VK_F12
};

KEY_MAP_STR mConvertWinKeyToStr[] = {
  {VK_ESCAPE,    L"Esc"},
  {VK_LCONTROL,  L"Ctrl" },
  {VK_RCONTROL,  L"Ctrl" },
  {VK_LMENU,     L"Alt" },
  {VK_RMENU,     L"Alt" },
  {VK_DELETE,    L"Del" },
  {VK_UP,        L"^"},
  {VK_DOWN,      L"v"},
  {VK_LEFT,      L"<"},
  {VK_RIGHT,     L">"},
  {VK_LWIN,      L"Win"},
  {VK_RWIN,      L"Win"},
  {VK_APPS,      L"App"},
  {VK_F1,        L"F1"},
  {VK_F2,        L"F2"},
  {VK_F3,        L"F3"},
  {VK_F4,        L"F4"},
  {VK_F5,        L"F5"},
  {VK_F6,        L"F6"},
  {VK_F7,        L"F7"},
  {VK_F8,        L"F8"},
  {VK_F9,        L"F9"},
  {VK_F10,       L"F10"},
  {VK_F11,       L"F11"},
  {VK_F12,       L"F12"},
  {0xE8,         L"Fn"},
  {0, NULL},
};


KEYBOARD_LAYOUT mEnglishKeyboardLayout = {
  900,
  378,
  0,
  0,
  IMAGE_TOKEN (IMAGE_KB_BACKGROUND),
  {5, 5, 5, 5},
  sizeof (mEnglishEntryList) / sizeof (mEnglishEntryList[0]),
  mEnglishEntryList,
  sizeof (mEnglishStyleList) / sizeof (mEnglishStyleList[0]),
  mEnglishStyleList
};

KEYBOARD_LAYOUT *mKeyboardLayoutList[] = {
  &mEnglishKeyboardLayout,
  NULL,
};

UINTN  mKeyboardLayoutCount      = sizeof (mKeyboardLayoutList) / sizeof (mKeyboardLayoutList[0]);
UINT16 mKeyConvertionTableNumber = sizeof (mKeyConvertionTable) / sizeof (mKeyConvertionTable[0]);


