/** @file
  Function prototype for USB Keyboard Driver

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

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

BOOLEAN
IsUsbKeyboard (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo
  );

EFI_STATUS
InitUsbKeyboard (
  IN USB_KB_DEV   *UsbKeyboardDevice
  );

EFI_STATUS
EFIAPI
KeyboardHandler (
  IN  VOID          *Data,
  IN  UINTN         DataLength,
  IN  VOID          *Context,
  IN  UINT32        Result
  );

VOID
EFIAPI
UsbKeyboardRecoveryHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  );

EFI_STATUS
UsbParseKey (
  IN OUT  USB_KB_DEV  *UsbKeyboardDevice,
  OUT     UINT8       *KeyChar
  );

EFI_STATUS
UsbKeyCodeToEFIScanCode (
  IN  USB_KB_DEV      *UsbKeyboardDevice,
  IN  UINT8           KeyChar,
  OUT EFI_INPUT_KEY   *Key
  );

EFI_STATUS
InitUsbKeyBuffer (
  IN OUT  USB_KB_BUFFER   *KeyboardBuffer
  );

BOOLEAN
IsUsbKeyboardBufferEmpty (
  IN  USB_KB_BUFFER   *KeyboardBuffer
  );

BOOLEAN
IsUsbKeyboardBufferFull (
  IN  USB_KB_BUFFER   *KeyboardBuffer
  );

EFI_STATUS
InsertKeyCode (
  IN      USB_KB_DEV    *UsbKeyboardDevice,
  IN      UINT8         Key,
  IN      UINT8         Down
  );

EFI_STATUS
RemoveKeyCode (
  IN OUT  USB_KB_BUFFER *KeyboardBuffer,
  OUT     USB_KEY       *UsbKey
  );

VOID
EFIAPI
UsbKeyboardRepeatHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  );

EFI_STATUS
SetKeyLED (
  IN  USB_KB_DEV    *UsbKeyboardDevice
  );

VOID
EFIAPI
UsbKeyboardCheckLEDHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  );
#endif
