/** @file
  Header file for I2C Keyboard driver function prototype and data structures

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

#ifndef _I2C_KB_H
#define _I2C_KB_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HidDescriptorLib.h>
#include <Protocol/I2cIo.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AdapterInformation.h>
#include <I2cHidDeviceInfo.h>

#define KEYBOARD_CLASS                  0x0106
#define MAX_KEY_ALLOWED                 32

#define KBD_REPEAT_DELAY                ((1000 * 1000 * 10) / 2)
#define KBD_REPEAT_RATE                 ((1000 * 1000 * 10) / 32)

#define BDA_QUALIFIER_STAT              0x417
#ifndef EBDA
  #define EBDA(a)                       (*(UINT8*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))
#endif
#ifndef EBDA_MONITOR_KEY_TABLE
  #define EBDA_MONITOR_KEY_TABLE        0x03b
#endif
#ifndef EBDA_KEYBORD_SCAN_CODE
  #define EBDA_KEYBORD_SCAN_CODE        0x164
#endif
#ifndef PAUSE_IN_PROGRESS
  #define PAUSE_IN_PROGRESS             ((*(UINT8*)(UINTN)0x418) & 0x08)
#endif
#ifndef PS2_LED_UPDATING
  #define PS2_LED_UPDATING              ((*(UINT8*)(UINTN)0x497) & 0x40)
#endif
#ifndef USB_LED_UPDATE
  #define USB_LED_UPDATE                ((*(UINT8*)(UINTN)0x497) |= 0x08)
#endif
#ifndef USB_LED_UPDATED
  #define USB_LED_UPDATED               ((*(UINT8*)(UINTN)0x497) &= ~0x08)
#endif

#define KEYBOARD_RESET_SIGNATURE        L"CAD"

typedef struct {
  UINT8   Down;
  UINT8   KeyCode;
} KEY;

typedef struct {
  KEY     buffer[MAX_KEY_ALLOWED + 1];
  UINT8   bHead;
  UINT8   bTail;
} KB_BUFFER;

#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"
#define IMAGE_DRIVER_NAME               L"I2C Keyboard Driver"
#define CONTROLLER_DRIVER_NAME          L"Generic I2C Keyboard"
#define I2C_KB_DEV_SIGNATURE            SIGNATURE_32 ('I', '2', 'C', 'K')
#define I2C_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE SIGNATURE_32 ('I', 'K', 'B', 'X')

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            NotifyHandle;
  EFI_KEY_DATA                          KeyData;
  EFI_KEY_NOTIFY_FUNCTION               KeyNotificationFn;
  LIST_ENTRY                            NotifyEntry;
} KEYBOARD_CONSOLE_IN_EX_NOTIFY;

typedef struct {
  UINTN                                 Signature;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL        SimpleInput;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     SimpleInputEx;
  EFI_UNICODE_STRING_TABLE              *ControllerNameTable;
  EFI_I2C_IO_PROTOCOL                   *I2cIo;
  EFI_EVENT                             OnCompleteEvent;
  EFI_STATUS                            TransferResult;
  H2O_I2C_HID_DEVICE                    I2cHid;
  HID_DESCRIPTOR                        HidDescriptor;
  REPORT_FIELD_INFO                     ReportFieldInfo;
  UINT8                                 ReportId;
  UINT8                                 *ReportData;
  KB_BUFFER                             KeyboardBuffer;
  UINT8                                 CtrlOn;
  UINT8                                 AltOn;
  UINT8                                 ShiftOn;
  UINT8                                 NumLockOn;
  UINT8                                 CapsOn;
  UINT8                                 ScrollOn;
  UINT8                                 CurKeyChar;
  UINT8                                 RepeatKey;
  UINT8                                 LastKeyCodeArray[8];
  EFI_HANDLE                            RepeatPollingHandle;
  BOOLEAN                               IsCsmEnabled;
  BOOLEAN                               LedSupported;
  BOOLEAN                               IsSupportPartialKey;
  UINT8                                 LeftCtrlOn;
  UINT8                                 LeftAltOn;
  UINT8                                 LeftShiftOn;
  UINT8                                 LeftLogoOn;
  UINT8                                 RightCtrlOn;
  UINT8                                 RightAltOn;
  UINT8                                 RightShiftOn;
  UINT8                                 RightLogoOn;  
  UINT8                                 MenuKeyOn;
  UINT8                                 SysReqOn;
  EFI_KEY_STATE                         KeyState;
  LIST_ENTRY                            NotifyList;
} I2C_KB_DEV;

#define I2C_KB_DEV_FROM_SIMPLE_INPUT_PROTOCOL(a) \
    CR(a, I2C_KB_DEV, SimpleInput, I2C_KB_DEV_SIGNATURE)

#define I2C_KB_DEV_FROM_SIMPLE_INPUT_EX_PROTOCOL(a) \
    CR(a, I2C_KB_DEV, SimpleInputEx, I2C_KB_DEV_SIGNATURE)

#define MOD_CONTROL_L           0x01
#define MOD_CONTROL_R           0x10
#define MOD_SHIFT_L             0x02
#define MOD_SHIFT_R             0x20
#define MOD_ALT_L               0x04
#define MOD_ALT_R               0x40
#define MOD_WIN_L               0x08
#define MOD_WIN_R               0x80

typedef struct {
  UINT8 Mask;
  UINT8 Key;
} KB_MODIFIER;

#define KEYCODE_MAX_MAKE        0x7E
#define KBD_VALID_KEYCODE(key)  ((UINT8) (key) > 3)

typedef struct {
  UINT8 NumLock : 1;
  UINT8 CapsLock : 1;
  UINT8 ScrollLock : 1;
  UINT8 Resrvd : 5;
} LED_MAP;

typedef struct {
  UINT8 RightShift : 1;
  UINT8 LeftShift : 1;
  UINT8 Ctrl : 1;
  UINT8 Alt : 1;
  UINT8 ScrollLock : 1;
  UINT8 NumLock : 1;
  UINT8 CapsLock : 1;
  UINT8 Insert : 1;
} QUALIFIER_STAT;

extern EFI_DRIVER_BINDING_PROTOCOL      gI2cKeyboardDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gI2cKeyboardComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gI2cKeyboardComponentName2;

#endif
