/** @file
  Header file for USB Keyboard Driver's Data Structures

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

#ifndef _USB_KB_H
#define _USB_KB_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Ps2Policy.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/UsbCore.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/EventGroup.h>

#define MAX_KEY_ALLOWED                 32

#define USBKBD_REPEAT_DELAY             500
#define USBKBD_REPEAT_RATE              32

#define CLASS_HID                       3
#define SUBCLASS_BOOT                   1
#define PROTOCOL_KEYBOARD               1

#define BOOT_PROTOCOL                   0
#define REPORT_PROTOCOL                 1

#define BDA_QUALIFIER_STAT              0x417
#define PAUSE_IN_PROGRESS               ((*(UINT8*)(UINTN)0x418) & 0x08)

#define KEYBOARD_RESET_SIGNATURE        L"CAD"

#define IMAGE_DRIVER_NAME               L"Usb Keyboard Driver"
#define CONTROLLER_DRIVER_NAME          L"Generic Usb Keyboard"

typedef struct {
  UINT8 Down;
  UINT8 KeyCode;
} USB_KEY;

typedef struct {
  USB_KEY buffer[MAX_KEY_ALLOWED + 1];
  UINT8   bHead;
  UINT8   bTail;
} USB_KB_BUFFER;

#define USB_KB_DEV_SIGNATURE            SIGNATURE_32 ('u', 'k', 'b', 'd')
#define USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE SIGNATURE_32 ('u', 'k', 'b', 'x')

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            NotifyHandle;
  EFI_KEY_DATA                          KeyData;
  EFI_KEY_NOTIFY_FUNCTION               KeyNotificationFn;
  LIST_ENTRY                            NotifyEntry;
} KEYBOARD_CONSOLE_IN_EX_NOTIFY;

typedef struct {
  UINTN                                 Signature;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL        SimpleInput;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  EFI_USB_CORE_PROTOCOL                 *UsbCore;
  EFI_USB_INTERFACE_DESCRIPTOR          InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR           IntEndpointDescriptor;
  USB_KB_BUFFER                         KeyboardBuffer;
  UINT8                                 CtrlOn;
  UINT8                                 AltOn;
  UINT8                                 ShiftOn;
  UINT8                                 NumLockOn;
  UINT8                                 CapsOn;
  UINT8                                 ScrollOn;
  UINT8                                 CurKeyChar;
  UINT8                                 RepeatKey;
  UINT8                                 LastKeyCodeArray[8];
  EFI_UNICODE_STRING_TABLE              *ControllerNameTable;
  EFI_HANDLE                            RepeatPollingHandle;
  EFI_HANDLE                            RecoveryPollingHandle;
  BOOLEAN                               LedSupported;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     SimpleInputEx;
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
  BOOLEAN                               IsSupportPartialKey;

  EFI_KEY_STATE                         KeyState;
  LIST_ENTRY                            NotifyList;
} USB_KB_DEV;

typedef struct {
  EFI_HANDLE                            Handle;
  UINT8                                 UsbHidDevicePath[sizeof(USB_DEVICE_PATH) + sizeof(USB_CLASS_DEVICE_PATH) + END_DEVICE_PATH_LENGTH];
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL        SimpleInput;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     SimpleInputEx;
} FAKE_DEV;


//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL      gUsbKeyboardDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gUsbKeyboardComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gUsbKeyboardComponentName2;

VOID
KbdReportStatusCode (
  IN USB_KB_DEV                *UsbKeyboardDevice,
  IN EFI_STATUS_CODE_TYPE      CodeType,
  IN EFI_STATUS_CODE_VALUE     Value
  );

#define USB_KB_DEV_FROM_THIS(a) \
    CR(a, USB_KB_DEV, SimpleInput, USB_KB_DEV_SIGNATURE)

#define TEXT_INPUT_EX_USB_KB_DEV_FROM_THIS(a) \
    CR(a, USB_KB_DEV, SimpleInputEx, USB_KB_DEV_SIGNATURE)

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

#define USB_KEYCODE_MAX_MAKE      0x7E
#define USBKBD_VALID_KEYCODE(key) ((UINT8) (key) > 3)

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
//
// HID usage tables
//
#define HID_UP_LED        0x08
//
// HID Item general structure
//
typedef struct _hid_item {
  UINT16  Format;
  UINT8   Size;
  UINT8   Type;
  UINT8   Tag;
  union {
    UINT8   U8;
    UINT16  U16;
    UINT32  U32;
    INT8    I8;
    INT16   I16;
    INT32   I32;
    UINT8   *LongData;
  } Data;
} HID_ITEM;

#endif
