//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;   The MonitorKeyFilter Protocol is used provide a standard interface to the
//;   monitor keys.
//;
#include "MonitorKey.h"

MONITOR_KEY_FILTER_PRIVATE           *mPrivate;
BOOLEAN                              mIsLegacySupport = FALSE;
KEY_ELEMENT                          mKeyList[] = PLATFORM_KEY_LIST;
#ifdef ENABLE_CONSOLE_EX
EFI_EVENT                            mHotkeyEvent;
VOID                                 *mHotkeyRegistration;
EFI_HANDLE                           mConInHandle = NULL;
EFI_GUID gSimpleTextInExNotifyGuid = { \
  0x856f2def, 0x4e93, 0x4d6b, 0x94, 0xce, 0x1c, 0xfe, 0x47, 0x1, 0x3e, 0xa5 \
};
#endif

#ifdef ENABLE_CONSOLE_EX
//
// Efi key to IBM Key Codemapping table
// Format:<efi scan code>, <unicode without shift>, <unicode with shift>
//
STATIC
UINT8 KeyCodeConvertionTable[][7] = {//       (========IBM PC Scan Code========)
//EFI Scancode <EFIUniCode> <EFIUniCode(Shift)> Base     Shift    Alt      Ctrl
    SCAN_NULL,      'a',      'A',              0x1E,    0x1E,    0x1E,    0x1E,// 0x04
    SCAN_NULL,      'b',      'B',              0x30,    0x30,    0x30,    0x30,// 0x05
    SCAN_NULL,      'c',      'C',              0x2E,    0x2E,    0x2E,    0x2E,// 0x06
    SCAN_NULL,      'd',      'D',              0x20,    0x20,    0x20,    0x20,// 0x07
    SCAN_NULL,      'e',      'E',              0x12,    0x12,    0x12,    0x12,// 0x08
    SCAN_NULL,      'f',      'F',              0x21,    0x21,    0x21,    0x21,// 0x09
    SCAN_NULL,      'g',      'G',              0x22,    0x22,    0x22,    0x22,// 0x0A
    SCAN_NULL,      'h',      'H',              0x23,    0x23,    0x23,    0x23,// 0x0B
    SCAN_NULL,      'i',      'I',              0x17,    0x17,    0x17,    0x17,// 0x0C
    SCAN_NULL,      'j',      'J',              0x24,    0x24,    0x24,    0x24,// 0x0D
    SCAN_NULL,      'k',      'K',              0x25,    0x25,    0x25,    0x25,// 0x0E
    SCAN_NULL,      'l',      'L',              0x26,    0x26,    0x26,    0x26,// 0x0F
    SCAN_NULL,      'm',      'M',              0x32,    0x32,    0x32,    0x32,// 0x10
    SCAN_NULL,      'n',      'N',              0x31,    0x31,    0x31,    0x31,// 0x11
    SCAN_NULL,      'o',      'O',              0x18,    0x18,    0x18,    0x18,// 0x12
    SCAN_NULL,      'p',      'P',              0x19,    0x19,    0x19,    0x19,// 0x13
    SCAN_NULL,      'q',      'Q',              0x10,    0x10,    0x10,    0x10,// 0x14
    SCAN_NULL,      'r',      'R',              0x13,    0x13,    0x13,    0x13,// 0x15
    SCAN_NULL,      's',      'S',              0x1F,    0x1F,    0x1F,    0x1F,// 0x16
    SCAN_NULL,      't',      'T',              0x14,    0x14,    0x14,    0x14,// 0x17
    SCAN_NULL,      'u',      'U',              0x16,    0x16,    0x16,    0x16,// 0x18
    SCAN_NULL,      'v',      'V',              0x2F,    0x2F,    0x2F,    0x2F,// 0x19
    SCAN_NULL,      'w',      'W',              0x11,    0x11,    0x11,    0x11,// 0x1A
    SCAN_NULL,      'x',      'X',              0x2D,    0x2D,    0x2D,    0x2D,// 0x1B
    SCAN_NULL,      'y',      'Y',              0x15,    0x15,    0x15,    0x15,// 0x1C
    SCAN_NULL,      'z',      'Z',              0x2C,    0x2C,    0x2C,    0x2C,// 0x1D
    SCAN_NULL,      '1',      '!',              0x02,    0x02,    0x78,    0x02,// 0x1E
    SCAN_NULL,      '2',      '@',              0x03,    0x03,    0x79,    0x03,// 0x1F
    SCAN_NULL,      '3',      '#',              0x04,    0x04,    0x7A,    0x04,// 0x20
    SCAN_NULL,      '4',      '$',              0x05,    0x05,    0x7B,    0x05,// 0x21
    SCAN_NULL,      '5',      '%',              0x06,    0x06,    0x7C,    0x06,// 0x22
    SCAN_NULL,      '6',      '^',              0x07,    0x07,    0x7D,    0x07,// 0x23
    SCAN_NULL,      '7',      '&',              0x08,    0x08,    0x7E,    0x08,// 0x24
    SCAN_NULL,      '8',      '*',              0x09,    0x09,    0x7F,    0x09,// 0x25
    SCAN_NULL,      '9',      '(',              0x0A,    0x0A,    0x81,    0x0A,// 0x26
    SCAN_NULL,      '0',      ')',              0x0B,    0x0B,    0x82,    0x0B,// 0x27
    SCAN_NULL,      0x0d,     0x0d,             0x1C,    0x1C,    0x1C,    0x1C,// 0x28   Enter
    SCAN_ESC,       0x00,     0x00,             0x01,    0x01,    0x01,    0x01,// 0x29   Esc
    SCAN_NULL,      0x08,     0x08,             0x0E,    0x0E,    0x0E,    0x0E,// 0x2A   Backspace
    SCAN_NULL,      0x09,     0x09,             0x0F,    0x0F,    0xA5,    0x94,// 0x2B   Tab
    SCAN_NULL,      ' ',      ' ',              0x39,    0x39,    0x39,    0x39,// 0x2C   Spacebar
    SCAN_NULL,      '-',      '_',              0x0C,    0x0C,    0x82,    0x0C,// 0x2D
    SCAN_NULL,      '=',      '+',              0x0D,    0x0D,    0x83,    0xFF,// 0x2E
    SCAN_NULL,      '[',      '{',              0x1A,    0x1A,    0x1A,    0x1A,// 0x2F
    SCAN_NULL,      ']',      '}',              0x1B,    0x1B,    0x1B,    0x1B,// 0x30
    SCAN_NULL,      '\\',     '|',              0x2B,    0x2B,    0x2B,    0x2B,// 0x31
    SCAN_NULL,      '\\',     '|',              0x56,    0x56,    0x56,    0x56,// 0x32  Keyboard US \ and |
    SCAN_NULL,      ';',      ':',              0x27,    0x27,    0x27,    0xFF,// 0x33
    SCAN_NULL,      '\'',     '"',              0x28,    0x28,    0x28,    0xFF,// 0x34
    SCAN_NULL,      '`',      '~',              0x29,    0x29,    0x29,    0xFF,// 0x35  Keyboard Grave Accent and Tlide
    SCAN_NULL,      ',',      '<',              0x33,    0x33,    0x33,    0xFF,// 0x36
    SCAN_NULL,      '.',      '>',              0x34,    0x34,    0x34,    0xFF,// 0x37
    SCAN_NULL,      '/',      '?',              0x35,    0x35,    0x35,    0xFF,// 0x38
    SCAN_NULL,      0x00,     0x00,             0x3A,    0x3A,    0x3A,    0x3A,// 0x39   CapsLock
    SCAN_F1,        0x00,     0x00,             0x3B,    0x54,    0x68,    0x5E,// 0x3A
    SCAN_F2,        0x00,     0x00,             0x3C,    0x55,    0x69,    0x5F,// 0x3B
    SCAN_F3,        0x00,     0x00,             0x3D,    0x56,    0x6A,    0x60,// 0x3C
    SCAN_F4,        0x00,     0x00,             0x3E,    0x57,    0x6B,    0x61,// 0x3D
    SCAN_F5,        0x00,     0x00,             0x3F,    0x58,    0x6C,    0x62,// 0x3E
    SCAN_F6,        0x00,     0x00,             0x40,    0x59,    0x6D,    0x63,// 0x3F
    SCAN_F7,        0x00,     0x00,             0x41,    0x5A,    0x6E,    0x64,// 0x40
    SCAN_F8,        0x00,     0x00,             0x42,    0x5B,    0x6F,    0x65,// 0x41
    SCAN_F9,        0x00,     0x00,             0x43,    0x5C,    0x70,    0x66,// 0x42
    SCAN_F10,       0x00,     0x00,             0x44,    0x5D,    0x71,    0x67,// 0x43
    SCAN_F11,       0x00,     0x00,             0x85,    0x87,    0x8B,    0x89,// 0x44   F11
    SCAN_F12,       0x00,     0x00,             0x86,    0x88,    0x8C,    0x8A,// 0x45   F12
    SCAN_NULL,      0x00,     0x00,             0x80,    0x80,    0x80,    0x72,// 0x46   PrintScreen
    SCAN_NULL,      0x00,     0x00,             0x46,    0x46,    0x46,    0x46,// 0x47   Scroll Lock
    SCAN_PAUSE,     0x00,     0x00,             0x81,    0x81,    0x81,    0x81,// 0x48   Pause
    SCAN_NULL,      0x00,     0x00,             0x45,    0x45,    0x45,    0x45,// 0x53   NumLock
    SCAN_NULL,      '/',      '/',              0xB5,    0xB5,    0xA4,    0x95,// 0x54
    SCAN_NULL,      '*',      '*',              0x37,    0x37,    0x37,    0x96,// 0x55
    SCAN_NULL,      '-',      '-',              0x4A,    0x4A,    0x4A,    0x8E,// 0x56
    SCAN_NULL,      '+',      '+',              0x4E,    0x4E,    0x4E,    0x90,// 0x57
    SCAN_NULL,      0x0d,     0x0d,             0x9C,    0x9C,    0xA6,    0x9C,// 0x58
    SCAN_END,       '1',      '1',              0x4F,    0x4F,    0x9F,    0x75,// 0x59
    SCAN_DOWN,      '2',      '2',              0x50,    0x50,    0xA0,    0x91,// 0x5A
    SCAN_PAGE_DOWN, '3',      '3',              0x51,    0x51,    0xA1,    0x76,// 0x5B
    SCAN_LEFT,      '4',      '4',              0x4B,    0x4B,    0x9B,    0x73,// 0x5C
    SCAN_NULL,      '5',      '5',              0x4C,    0x4C,    0xFF,    0x8F,// 0x5D
    SCAN_RIGHT,     '6',      '6',              0x4D,    0x4D,    0x9D,    0x74,// 0x5E
    SCAN_HOME,      '7',      '7',              0x47,    0x47,    0x97,    0x77,// 0x5F
    SCAN_UP,        '8',      '8',              0x48,    0x48,    0x98,    0x8D,// 0x60
    SCAN_PAGE_UP,   '9',      '9',              0x49,    0x49,    0x99,    0x84,// 0x61
    SCAN_INSERT,    '0',      '0',              0x52,    0x52,    0xA2,    0x92,// 0x62
    SCAN_DELETE,    '.',      '.',              0x53,    0x53,    0xA3,    0x93,// 0x63
    SCAN_NULL,      '\\',     '|',              0x2B,    0x2B,    0x2B,    0x2B,// 0x64 Keyboard Non-US \ and |
    SCAN_NULL,      0x00,     0x00,             0xDD,    0xDD,    0xDD,    0xDD,// 0x65 Keyboard Application
    0xFF,           0x00,     0x00,             0x00,    0x00,    0x00,    0x00 // End of table
};
//
// Shift    Alt      Ctrl
//
STATIC
UINT8 EfiKeyShiftStateTable[][3] = {
  0x01,    0x10,    0x04,
  0x01,    0x10,    0x08,
  0x01,    0x20,    0x08,
  0x01,    0x20,    0x04,
  0x02,    0x20,    0x08,
  0x02,    0x10,    0x04,
  0x02,    0x20,    0x04,
  0x02,    0x10,    0x08,
  0xFF,    0xFF,    0xFF
};

#define KEYTABLE_LENGTH (sizeof (KeyCodeConvertionTable) / (sizeof (UINT8) * 7))

EFI_DRIVER_ENTRY_POINT (MonitorKeyFilterInstall)

//
// Local function prototypes
//

UINT8
ConvertKBCScanCode (
  IN     EFI_KEY_DATA               *KeyData,
  IN OUT UINTN                      *Index
  )
/*++

Routine Description:
  Put the KBC scan code into EBDA offset 0x164.

Arguments:
  KeyData    - A pointer to a buffer that is filled in with the keystroke
               state data for the key that was pressed.

Returns:
  None

--*/
{
  UINTN                         ConvertIndex = 0;
  UINT8                         ConvertValue = 0;

  if (*Index != 0) {
    ConvertIndex = *Index;
  }

  if (ConvertIndex >= KEYTABLE_LENGTH) {
    ConvertIndex = 0;
  }

  do {
    if (KeyData->Key.ScanCode) {
      if (KeyData->Key.ScanCode == KeyCodeConvertionTable[ConvertIndex][0]) {
        if (KeyData->KeyState.KeyShiftState & (EFI_LEFT_SHIFT_PRESSED | EFI_RIGHT_SHIFT_PRESSED)) {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][4];
          break;
        } else if (KeyData->KeyState.KeyShiftState & (EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_CONTROL_PRESSED)) {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][6];
          break;
        } else if (KeyData->KeyState.KeyShiftState & (EFI_LEFT_ALT_PRESSED | EFI_RIGHT_ALT_PRESSED)) {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][5];
          break;
        } else {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][3];
          break;
        }
      }
    } else {
      if (KeyData->Key.UnicodeChar == KeyCodeConvertionTable[ConvertIndex][1]) {
        if (KeyData->KeyState.KeyShiftState & (EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_CONTROL_PRESSED)) {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][6];
          break;
        } else if (KeyData->KeyState.KeyShiftState & (EFI_LEFT_ALT_PRESSED | EFI_RIGHT_ALT_PRESSED)) {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][5];
          break;
        } else {
          ConvertValue = KeyCodeConvertionTable[ConvertIndex][3];
          break;
        }
      } else if (KeyData->Key.UnicodeChar == KeyCodeConvertionTable[ConvertIndex][2]) {
        ConvertValue = KeyCodeConvertionTable[ConvertIndex][4];
        break;
      }
    }
    ConvertIndex ++;
  } while ((KeyCodeConvertionTable[ConvertIndex][0] != 0xFF) && (ConvertIndex <= KEYTABLE_LENGTH));

  *Index = ConvertIndex;
  return ConvertValue;
}

BOOLEAN
ConvertEfiScanCode (
  IN KEY_ELEMENT                *KeyList,
  IN EFI_KEY_DATA               *KeyData
  )
/*++

Routine Description:
  Put the KBC scan code into EBDA offset 0x164.

Arguments:
  KeyData    - A pointer to a buffer that is filled in with the keystroke
               state data for the key that was pressed.

Returns:
  None

--*/
{
  UINTN                         Index;
  UINTN                         ScanCodeBase;

  ScanCodeBase = 3;
  Index        = 0;

  while(TRUE){
    if (KeyList->ScanCode == KeyCodeConvertionTable[Index][ScanCodeBase]) {
      KeyData->Key.ScanCode = KeyCodeConvertionTable[Index][0];
      KeyData->Key.UnicodeChar = KeyCodeConvertionTable[Index][1];
      if (KeyList->Keyattribute.ShiftKey) {
        KeyData->Key.UnicodeChar = KeyCodeConvertionTable[Index][2];
      }
      KeyData->KeyState.KeyShiftState = EFI_SHIFT_STATE_VALID;
      return TRUE;
    }
    if (KeyCodeConvertionTable[Index][ScanCodeBase] == 0 && ScanCodeBase == 6) break;
    if (KeyCodeConvertionTable[Index][ScanCodeBase] == 0) {
      ScanCodeBase++;
      Index = 0;
    } else {
      Index++;
    }
  }

  return FALSE;
}
#endif

EFI_STATUS
InitMointorKeyBuffer(
  IN UINTN      Lineraddress
  )
/*++

Routine Description:

  Init MointorKey Buffer

Arguments:

  Lineraddress - MointorKey Buffer

Returns:

  EFI_SUCCESS

--*/
{
  UINT16                                    *BdaBuffer;
  UINT16                                    BdaBufferSegment;
  MONITOR_KEY_FILTER_BUFFER_HEAD_STRUCT     *EbdaBufferPtr;

  BdaBuffer = (UINT16 *)(UINTN)DEFINE_BDA_MEMORY_GET_EBDA_ADDRESS;
  BdaBufferSegment = *BdaBuffer;
  EbdaBufferPtr = (MONITOR_KEY_FILTER_BUFFER_HEAD_STRUCT *)(UINTN)(((UINT32)BdaBufferSegment << DEFINE_TRANSFER_SEGMENT) + DEFINE_EBDA_MONITOR_KEY_BUFFER_ADDRESS);
  EbdaBufferPtr->EnableFlag    = DEFINE_KEY_MOINTOR_ENABGLE_SIGNATURE;
  EbdaBufferPtr->BufferOffset  = (UINT16)(Lineraddress & DEFINE_MARK_OFFSET);
  EbdaBufferPtr->BufferSegment = (UINT16)(Lineraddress >> DEFINE_TRANSFER_SEGMENT);
  EbdaBufferPtr->StatusFlag    = 0x00;

  return EFI_SUCCESS;
}

EFI_STATUS
KeyListCopyMem (
  IN KEY_ELEMENT *KeyListSourcPtr,
  IN UINT8       *DestinationBufferPtr,
  IN UINTN       Buffersize
  )
/*++

Routine Description:

  Copy KeyList

Arguments:

  KeyListSourcPtr      - Point to KeyListSourc
  DestinationBufferPtr - Point to DestinationBuffer
  Buffersize           - Buffer size of KeyList

Returns:

  EFI_STATUS

--*/
{
   UINT8   BufferCount;

   BufferCount = 0;
   if (Buffersize > (UINTN)(DEFINE_MAX_KEY_LIST * sizeof(KEY_ELEMENT))) {
     Buffersize = (UINTN)(DEFINE_MAX_KEY_LIST * sizeof(KEY_ELEMENT));
   }

   while (Buffersize != 0x00) {
     *(DestinationBufferPtr + BufferCount) = KeyListSourcPtr->ScanCode;
     BufferCount++;

     *(DestinationBufferPtr + BufferCount) = 0x00;
     *(DestinationBufferPtr + BufferCount + 1) = 0x00;

     if (KeyListSourcPtr->Keyattribute.ShiftKey == DEFINE_KEY_IS_SET) {
       *(DestinationBufferPtr + BufferCount) |= SHIFT_KEY_ENABLE_FLAG;
     }

     if (KeyListSourcPtr->Keyattribute.AltKey == DEFINE_KEY_IS_SET) {
       *(DestinationBufferPtr + BufferCount) |= ALT_KEY_ENABLE_FLAG;
     }

     if (KeyListSourcPtr->Keyattribute.CtrlKey == DEFINE_KEY_IS_SET) {
       *(DestinationBufferPtr + BufferCount) |= CTRL_KEY_ENABLE_FLAG;
     }
     //
     //KEY_ATTRIBUTE is UINT16 aligment
     //
     BufferCount++;
     BufferCount++;

     KeyListSourcPtr++;
     Buffersize -= sizeof(KEY_ELEMENT);
   }

   *(DestinationBufferPtr + BufferCount) = 0x00;
   BufferCount++;
   *(DestinationBufferPtr + BufferCount) = 0x00;
   BufferCount++;
   *(DestinationBufferPtr + BufferCount) = 0x00;

   return EFI_SUCCESS;
}

EFI_STATUS
GetMonitoredKeys (
  IN  EFI_MONITOR_KEY_FILTER_PROTOCOL   *This,
  OUT UINT32                            *KeyDetected
  )
/*++

  Routine Description:
    Returns a bit map of which, if any, of the monitored keys were seen.
    The internal equivalent of KeyDetected is set to zero after each invocation.

  Arguments:
    This - Indicates the calling context.
    KeyDetected - A bit map of the monitored keys found.
                  Bit N corresponds to KeyList[N] as provided by the
                  GetUsbPlatformOptions() API.


  Returns:
    EFI_SUCCESS   - Key detected status is returned successfully.
    EFI_TBD - No keys detected.

--*/

{
  MONITOR_KEY_FILTER_PRIVATE                *Private;
  UINT16                                    *BdaBuffer;
  UINT16                                    BdaBufferSegment;
  MONITOR_KEY_FILTER_BUFFER_HEAD_STRUCT     *EbdaBufferPtr;
  EFI_STATUS                                Status;
  STATIC EFI_PRE_POST_HOTKEY                SavedPrePostHotkey = {0};
  EFI_PRE_POST_HOTKEY                       PrePostHotkey;
  UINTN                                     DataSize;
  STATIC BOOLEAN                            PrePOSTHotKeyExist = TRUE;

  Private = NULL;
  BdaBuffer = (UINT16 *)(UINTN)DEFINE_BDA_MEMORY_GET_EBDA_ADDRESS;
  BdaBufferSegment = *BdaBuffer;
  EbdaBufferPtr = (MONITOR_KEY_FILTER_BUFFER_HEAD_STRUCT *)(UINTN)(((UINT32)BdaBufferSegment << DEFINE_TRANSFER_SEGMENT) + DEFINE_EBDA_MONITOR_KEY_BUFFER_ADDRESS);


  if (SavedPrePostHotkey.KeyBit != PRE_POST_HOTKEY_NOT_EXIST) {
    //
    // get pre-post key from static SavedPrePostHotkey variable
    //
    if (SavedPrePostHotkey.KeyBit == PRE_POST_HOTKEY_OTHER_KEY) {
      *KeyDetected = PRE_POST_HOTKEY_NOT_EXIST;
    } else {
      *KeyDetected = SavedPrePostHotkey.KeyBit;
    }

  } else if (PrePOSTHotKeyExist) {

    DataSize = sizeof (EFI_PRE_POST_HOTKEY);
    Status = gRT->GetVariable (
                    EFI_PRE_POST_HOTKEY_NAME,
                    &gEfiGenericVariableGuid,
                    NULL,
                    &DataSize,
                    &PrePostHotkey
                    );
    if (EFI_ERROR (Status) || (PrePostHotkey.KeyBit == PRE_POST_HOTKEY_NOT_EXIST)) {

      PrePOSTHotKeyExist = FALSE;

    } else {

      EfiCopyMem (&SavedPrePostHotkey, &PrePostHotkey, sizeof (EFI_PRE_POST_HOTKEY));
      //
      // Get pre-post key from EFI_PRE_POST_HOTKEY_NAME variable
      //
      if (PrePostHotkey.KeyBit == PRE_POST_HOTKEY_OTHER_KEY) {
        *KeyDetected = PRE_POST_HOTKEY_NOT_EXIST;
      } else {
        *KeyDetected = PrePostHotkey.KeyBit;
      }
      //
      // Clear pre-post key to PRE_POST_HOTKEY_NOT_EXIST
      //
      PrePostHotkey.KeyBit = PRE_POST_HOTKEY_NOT_EXIST;
      DataSize = sizeof (EFI_PRE_POST_HOTKEY);
      Status = gRT->SetVariable(
                      EFI_PRE_POST_HOTKEY_NAME,
                      &gEfiGenericVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      DataSize,
                      &PrePostHotkey
                      );
    }
  }
  if (!PrePOSTHotKeyExist) {
    //
    // Check Enagle Sigunature;
    //
    if (EbdaBufferPtr->EnableFlag == DEFINE_KEY_MOINTOR_ENABGLE_SIGNATURE && mIsLegacySupport) {
      *KeyDetected = EbdaBufferPtr->StatusFlag;
    } else {
      *KeyDetected = mPrivate->KeyDetected;
    }
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
InstallMointorKey (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  EFI_STATUS                           Status;
  EFI_USB_LEGACY_PLATFORM_PROTOCOL     *UsbLegacyPlatform;
  KEY_ELEMENT                          *KeyList;
  UINTN                                KeyListSize;
  UINT8                                *DestinationAddress;

  KeyList = NULL;
  KeyListSize = 0;
  DestinationAddress = (UINT8*)(UINTN)DEFINE_DESTINATION_MEMORY_ARRESS;
  //
  //Locate Usb Legacy Platform Protocol instance
  //
  Status = gBS->LocateProtocol (&gEfiUsbLegacyPlatformProtocolGuid,
                                NULL,
                                &UsbLegacyPlatform
                               );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = UsbLegacyPlatform->GetPlatformMonitorKeyOptions (
                                UsbLegacyPlatform,
                                &KeyList,
                                &KeyListSize
                                );

  Status = KeyListCopyMem(
             KeyList,
             DestinationAddress,
             KeyListSize
             );

  //
  // Initialize EBDA Mointor Key Head Buffer
  //
  InitMointorKeyBuffer((UINTN)DestinationAddress);

  mPrivate->UsbLegacyPlatform = UsbLegacyPlatform;
  mIsLegacySupport = TRUE;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  return;
}
#ifdef ENABLE_CONSOLE_EX
EFI_STATUS
EfiHotkeyCallback (
  IN EFI_KEY_DATA     *KeyData
  )
/*++

Routine Description:

  This is the common notification function for HotKeys, it will be registered
  with SimpleTextInEx protocol interface - RegisterKeyNotify() of ConIn handle.

Arguments:

  KeyData               - A pointer to a buffer that is filled in with the keystroke
                          information for the key that was pressed.

Returns:

  EFI_SUCCESS           - KeyData is successfully processed.

--*/
{
  UINT8                              ScanCode;
  UINTN                              Index;
  BOOLEAN                            ThreeKeys;
  BOOLEAN                            Found;
  UINTN                              ConvertIndex;

  ThreeKeys    = FALSE;
  Found        = FALSE;
  ConvertIndex = 0;
  ScanCode     = ConvertKBCScanCode(KeyData, &ConvertIndex);
  //
  // Check scancode on HotKeyList
  //
  do {
    for(Index = 0; Index < mPrivate->HotKeyCount; Index++) {
      if (mPrivate->HotKeyList[Index].ScanCode == ScanCode) {
        Found = TRUE;
        break;
      }
    }
    if (!Found) {
      ConvertIndex++;
      ScanCode = ConvertKBCScanCode(KeyData, &ConvertIndex);
    }
  } while (!Found);

  for(Index = 0; Index < mPrivate->HotKeyCount; Index++) {
    if ((mPrivate->HotKeyList[Index].Keyattribute.AltKey == 1 && mPrivate->HotKeyList[Index].Keyattribute.CtrlKey == 1) ||
        (mPrivate->HotKeyList[Index].Keyattribute.AltKey == 1 && mPrivate->HotKeyList[Index].Keyattribute.ShiftKey == 1) ||
        (mPrivate->HotKeyList[Index].Keyattribute.CtrlKey == 1 && mPrivate->HotKeyList[Index].Keyattribute.ShiftKey == 1)) {
      ThreeKeys = TRUE;
    }
    if (mPrivate->HotKeyList[Index].ScanCode == ScanCode || ThreeKeys) {
      if(KeyData->KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) {
        mPrivate->KeyDetected = (UINT32) (1 << Index);
        break;
      }
    }
    ThreeKeys = FALSE;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EfiHotkeyRegisterNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx
  )
/*++

Routine Description:

  Register the common HotKey notify function to given SimpleTextInEx protocol instance.

Arguments:

  SimpleTextInEx        - Simple Text Input Ex protocol instance

Returns:

  EFI_SUCCESS           - Register hotkey notification function successfully.
  EFI_OUT_OF_RESOURCES  - Unable to allocate necessary data structures.

--*/
{
  EFI_STATUS                         Status;
  EFI_HANDLE                         Handle;
  EFI_KEY_DATA                       KeyData;
  EFI_KEY_DATA                       KeyPad;
  UINT32                             PreShiftState;
  UINTN                              Index;
  UINTN                              Count;
  UINT8                              ScanCode;

  Index  = 0;
  Count  = 0;
  Status = EFI_SUCCESS;

  do {
    Handle = NULL;
    EfiZeroMem(&KeyData, sizeof(EFI_KEY_DATA));
    EfiZeroMem(&KeyPad, sizeof(EFI_KEY_DATA));
    ScanCode = mPrivate->HotKeyList[Index].ScanCode;
    if (ConvertEfiScanCode (&mPrivate->HotKeyList[Index], &KeyData)) {
      if (mPrivate->HotKeyList[Index].Keyattribute.ShiftKey ||
          mPrivate->HotKeyList[Index].Keyattribute.AltKey   ||
          mPrivate->HotKeyList[Index].Keyattribute.CtrlKey) {
        //
        // Register Combination Key
        //
        PreShiftState = 0;
        for (Count = 0; EfiKeyShiftStateTable[Count][0] != 0xFF; Count++) {
          Handle = NULL;
          KeyData.KeyState.KeyShiftState = EFI_SHIFT_STATE_VALID;
          if (mPrivate->HotKeyList[Index].Keyattribute.ShiftKey) {
            KeyData.KeyState.KeyShiftState |= EfiKeyShiftStateTable[Count][0];
          }
          if (mPrivate->HotKeyList[Index].Keyattribute.AltKey) {
            KeyData.KeyState.KeyShiftState |= EfiKeyShiftStateTable[Count][1];
          }
          if (mPrivate->HotKeyList[Index].Keyattribute.CtrlKey) {
            KeyData.KeyState.KeyShiftState |= EfiKeyShiftStateTable[Count][2];
          }
          if (PreShiftState == KeyData.KeyState.KeyShiftState) {
            continue;
          }
          //
          // Register numeric key pad keys
          //
          if (ScanCode >= 0x47 && ScanCode <= 0x53) {
            EfiCopyMem(&KeyPad, &KeyData, sizeof(EFI_KEY_DATA));
            KeyPad.Key.ScanCode = SCAN_NULL;
            if (ScanCode != 0x4a && ScanCode != 0x4e) {
              KeyData.Key.UnicodeChar = 0x00;
            }
            Status = SimpleTextInEx->RegisterKeyNotify (
                                       SimpleTextInEx,
                                       &KeyPad,
                                       EfiHotkeyCallback,
                                       &Handle
                                       );
          }

          Status = SimpleTextInEx->RegisterKeyNotify (
                                     SimpleTextInEx,
                                     &KeyData,
                                     EfiHotkeyCallback,
                                     &Handle
                                     );
          if (EFI_ERROR (Status) || Handle == NULL) {
            return EFI_NOT_READY;
          }
          PreShiftState = KeyData.KeyState.KeyShiftState;
        }
      } else {
        //
        // Register numeric key pad keys
        //
        if (ScanCode >= 0x47 && ScanCode <= 0x53) {
          EfiCopyMem(&KeyPad, &KeyData, sizeof(EFI_KEY_DATA));
          KeyPad.Key.ScanCode = SCAN_NULL;
          if (ScanCode != 0x4a && ScanCode != 0x4e) {
            KeyData.Key.UnicodeChar = 0x00;
          }
          Status = SimpleTextInEx->RegisterKeyNotify (
                                     SimpleTextInEx,
                                     &KeyPad,
                                     EfiHotkeyCallback,
                                     &Handle
                                     );
        }
        //
        // Register single Hot Key
        //
        Status = SimpleTextInEx->RegisterKeyNotify (
                                   SimpleTextInEx,
                                   &KeyData,
                                   EfiHotkeyCallback,
                                   &Handle
                                   );
        if (EFI_ERROR (Status) || Handle == NULL) {
          return EFI_NOT_READY;
        }
      }
    }
    Index++;
  } while (Index < mPrivate->HotKeyCount);
  return EFI_SUCCESS;
}

EFI_STATUS
EfiHotkeyUnregisterNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx
  )
/*++

Routine Description:

  Unregister the common HotKey notify function to given SimpleTextInEx protocol instance.

Arguments:

  SimpleTextInEx        - Simple Text Input Ex protocol instance

Returns:

  EFI_SUCCESS           - Unregister hotkey notification function successfully.
  EFI_OUT_OF_RESOURCES  - Unable to allocate necessary data structures.

--*/
{
  EFI_STATUS                Status;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     Index;


  HandleCount = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gSimpleTextInExNotifyGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    //
    // If no more notification events exist
    //
    return Status;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->OpenProtocol (
                    HandleBuffer[Index],
                    &gSimpleTextInExNotifyGuid,
                    NULL,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // Register single Hot Key
    //
    Status = SimpleTextInEx->UnregisterKeyNotify (
                               SimpleTextInEx,
                               HandleBuffer[Index]
                               );
  }

  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }
  return EFI_SUCCESS;
}

VOID
EFIAPI
EfiHotkeyEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/*++

Routine Description:
  Callback function for SimpleTextInEx protocol install events

Arguments:

  Standard event notification function arguments:
  Event         - the event that is signaled.
  Context       - not used here.

Returns:

--*/
{
  EFI_STATUS                         Status;
  UINTN                              BufferSize;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx;

  SimpleTextInEx = NULL;

  if(mIsLegacySupport) {
    if (mHotkeyEvent != NULL) {
      if (mConInHandle != NULL) {
        Status = gBS->HandleProtocol (
                        mConInHandle,
                        &gEfiSimpleTextInputExProtocolGuid,
                        &SimpleTextInEx
                        );
        if (!EFI_ERROR (Status)) {
          Status = EfiHotkeyUnregisterNotify (SimpleTextInEx);
        }
      }
      gBS->CloseEvent (mHotkeyEvent);
    }
    return;
  }

  while (TRUE) {
    BufferSize = sizeof (EFI_HANDLE);
    Status = gBS->LocateHandle (
                    ByRegisterNotify,
                    NULL,
                    mHotkeyRegistration,
                    &BufferSize,
                    &mConInHandle
                    );
    if (EFI_ERROR (Status)) {
      //
      // If no more notification events exist
      //
      return;
    }

    Status = gBS->HandleProtocol (
                    mConInHandle,
                    &gEfiSimpleTextInputExProtocolGuid,
                    &SimpleTextInEx
                    );
    if (!EFI_ERROR (Status)) {
      Status = EfiHotkeyRegisterNotify (SimpleTextInEx);
    }
  }
}
#endif

EFI_STATUS
MonitorKeyFilterInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:
  Install Driver to produce MonitorKeyFilter protocol.

Arguments:
  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns:

  EFI_SUCCESS - MonitorKeyFilter protocol installed

  Other       - No protocol installed, unload driver.

--*/
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           MonitorKeyHandle;
  EFI_EVENT                            Event;
  VOID                                 *Registration;


  EfiInitializeDriverLib (ImageHandle, SystemTable);

  MonitorKeyHandle = NULL;

  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  EFI_TPL_CALLBACK,
                  InstallMointorKey,
                  NULL,
                  &Event
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gEfiLegacyBiosProtocolGuid,
                  Event,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);
#ifdef ENABLE_CONSOLE_EX
  //
  // Register Protocol notify for Hotkey service
  //
  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  EFI_TPL_CALLBACK,
                  EfiHotkeyEvent,
                  NULL,
                  &mHotkeyEvent
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  //
  // Register for protocol notifications on this event
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSimpleTextInputExProtocolGuid,
                  mHotkeyEvent,
                  &mHotkeyRegistration
                  );
  ASSERT_EFI_ERROR (Status);
#endif
  //
  // Initialize MonitorKeyFilter instance
  //
  Status = gBS->AllocatePool(
                  EfiBootServicesData,
                  sizeof(MONITOR_KEY_FILTER_PRIVATE),
                  (VOID **)&mPrivate
                  );

  if (EFI_ERROR (Status)) {
     return Status;
  }

  EfiZeroMem(mPrivate, sizeof(MONITOR_KEY_FILTER_PRIVATE));

  mPrivate->Signature = MONITOR_KEY_FILTER_PRIVATE_SIGNATURE;
  mPrivate->MonitorKeyInstance.GetMonitoredKeys  = GetMonitoredKeys;
  mPrivate->HotKeyList = &mKeyList[0];
  mPrivate->HotKeyCount = sizeof(mKeyList) / sizeof(KEY_ELEMENT);

  //
  // Make a new handle and install the protocol
  //

  Status = gBS->InstallProtocolInterface (
                  &MonitorKeyHandle,
                  &gEfiMonitorKeyFilterProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivate->MonitorKeyInstance
                  );

  return Status;
}
