//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   KeyboardLayout.c
//;
//; Abstract:
//;
//;   Keyboard layout
//;
//;
#include "Tiano.h"
#include "SetupMouse.h"
#include "KeyboardLayout.h"
#include "SetupMouseAniDefs.h"

#pragma warning (disable:4047)

//
// {BGRA}, {Animation Id}, {ScaleGrid}
//
KEY_STYLE_ARRAY mEnglishStyleList[] = {
  {
    {{0xFF, 0xFF, 0xFF, 0xFF}, ANIMATION_TOKEN (ANI_KB_NORMAL), {5,5,5,5}},
    {{0xFF, 0xFF, 0xFF, 0xFF}, ANIMATION_TOKEN (ANI_KB_HOVER),  {5,5,5,5}},
    {{0x00, 0x00, 0x1, 0xFF},  ANIMATION_TOKEN (ANI_KB_PRESS),  {5,5,5,5}}
  },
};

KEY_MAP_ENTRY mEnglishEntryList[] = {
  {0,  0x8, {562, 33,628, 60}},
  {0,  0x9, {  6, 61, 74, 88}},
  {0,  0xd, {519, 89,628,116}},
  {0, 0x14, {  6, 89, 89,116}},
  {0, 0x1b, {  6, 33, 54, 60}},
  {0, 0x20, {107,145,457,172}},
  {0, 0x25, {458,145,496,172}}, // VK_LEFT
  {0, 0x26, {497,117,535,144}}, // VK_UP
  {0, 0x27, {536,145,574,172}}, // VK_RIGHT
  {0, 0x28, {497,145,535,172}}, // VK_DOWN
  {0, 0x2e, {582, 61,628, 88}}, // VK_DELETE
  {0, 0x30, {445, 33,483, 60}}, // 0
  {0, 0x31, { 94, 33,132, 60}}, // 1
  {0, 0x32, {133, 33,171, 60}}, // 2
  {0, 0x33, {172, 33,210, 60}}, // 3
  {0, 0x34, {211, 33,249, 60}}, // 4
  {0, 0x35, {250, 33,288, 60}}, // 5
  {0, 0x36, {289, 33,327, 60}}, // 6
  {0, 0x37, {328, 33,366, 60}}, // 7
  {0, 0x38, {367, 33,405, 60}}, // 8
  {0, 0x39, {406, 33,444, 60}}, // 9
  {0, 0x41, { 90, 89,128,116}}, // A
  {0, 0x42, {263,117,301,144}}, // B
  {0, 0x43, {185,117,223,144}}, // C
  {0, 0x44, {168, 89,206,116}}, // D
  {0, 0x45, {153, 61,191, 88}}, // E
  {0, 0x46, {207, 89,245,116}}, // F
  {0, 0x47, {246, 89,284,116}}, // G
  {0, 0x48, {285, 89,323,116}}, // H
  {0, 0x49, {348, 61,386, 88}}, // I
  {0, 0x4a, {324, 89,362,116}}, // J
  {0, 0x4b, {363, 89,401,116}}, // K
  {0, 0x4c, {402, 89,440,116}}, // L
  {0, 0x4d, {341,117,379,144}}, // M
  {0, 0x4e, {302,117,340,144}}, // N
  {0, 0x4f, {387, 61,425, 88}}, // O
  {0, 0x50, {426, 61,464, 88}}, // P
  {0, 0x51, { 75, 61,113, 88}}, // Q
  {0, 0x52, {192, 61,230, 88}}, // R
  {0, 0x53, {129, 89,167,116}}, // S
  {0, 0x54, {231, 61,269, 88}}, // T
  {0, 0x55, {309, 61,347, 88}}, // U
  {0, 0x56, {224,117,262,144}}, // V
  {0, 0x57, {114, 61,152, 88}}, // W
  {0, 0x58, {146,117,184,144}}, // X
  {0, 0x59, {270, 61,308, 88}}, // Y
  {0, 0x5a, {107,117,145,144}}, // Z
  {0, 0x70, { 74,  5,112, 32}}, // VK_F1
  {0, 0x71, {113,  5,151, 32}}, // VK_F2
  {0, 0x72, {152,  5,190, 32}}, // VK_F3
  {0, 0x73, {191,  5,229, 32}}, // VK_F4
  {0, 0x74, {230,  5,268, 32}}, // VK_F5
  {0, 0x75, {269,  5,307, 32}}, // VK_F6
  {0, 0x76, {308,  5,346, 32}}, // VK_F7
  {0, 0x77, {347,  5,385, 32}}, // VK_F8
  {0, 0x78, {386,  5,424, 32}}, // VK_F9
  {0, 0x79, {425,  5,463, 32}}, // VK_F10
  {0, 0x7a, {464,  5,502, 32}}, // VK_F11
  {0, 0x7b, {503,  5,541, 32}}, // VK_F12
  {0, 0xa0, {  6,117,106,144}},
  {0, 0xa1, {536,117,628,144}},
  {0, 0xba, {441, 89,479,116}},
  {0, 0xbb, {523, 33,561, 60}},
  {0, 0xbc, {380,117,418,144}},
  {0, 0xbd, {484, 33,522, 60}},
  {0, 0xbe, {419,117,457,144}},
  {0, 0xbf, {458,117,496,144}},
  {0, 0xc0, { 55, 33, 93, 60}},
  {0, 0xc3, {590,  5,628, 32}},
  {0, 0xdb, {465, 61,503, 88}},
  {0, 0xdc, {543, 61,581, 88}},
  {0, 0xdd, {504, 61,542, 88}},
  {0, 0xde, {480, 89,518,116}},
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
  {VK_TAB,       L"Tab"},
  {VK_CAPITAL,   L"Caps"},
  {VK_LSHIFT,    L"Shift"},
  {VK_RSHIFT,    L"Shift"},
  {VK_LCONTROL,  L"Ctrl" },
  {VK_RCONTROL,  L"Ctrl" },
  {VK_LMENU,     L"Alt" },
  {VK_RMENU,     L"Alt" },
  {VK_RETURN,    L"Enter"},
  {VK_DELETE,    L"Del" },
  {VK_BACK,      L"Bksp"},
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
  {0xC3,         L"X"},
  {0, NULL},
};


KEYBOARD_LAYOUT mEnglishKeyboardLayout = {
  633,
  175,
  80,
  30,
  ANIMATION_TOKEN (ANI_KB_BACKGROUND),
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


