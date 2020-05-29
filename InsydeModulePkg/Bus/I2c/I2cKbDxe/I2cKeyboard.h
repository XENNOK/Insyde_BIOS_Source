/** @file
  Function prototype for I2C Keyboard Driver

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

#ifndef _I2C_KEYBOARD_H
#define _I2C_KEYBOARD_H

EFI_STATUS
EFIAPI
KeyboardHandler (
  IN  VOID              *Data,
  IN  UINTN             DataLength,
  IN  VOID              *Context,
  IN  UINT32            Result
  );

VOID
KeyboardRepeatHandler (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context
  );

EFI_STATUS
ParseKey (
  IN  I2C_KB_DEV        *KbDev,
  OUT UINT8             *KeyChar
  );

EFI_STATUS
KeyCodeToEFIScanCode (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             KeyChar,
  OUT EFI_INPUT_KEY     *Key
  );

EFI_STATUS
InitKeyBuffer (
  OUT KB_BUFFER         *KeyboardBuffer
  );

BOOLEAN
IsKeyboardBufferEmpty (
  IN  KB_BUFFER         *KeyboardBuffer
  );

BOOLEAN
IsKeyboardBufferFull (
  IN  KB_BUFFER         *KeyboardBuffer
  );

EFI_STATUS
InsertKeyCode (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             Key,
  IN  UINT8             Down
  );

EFI_STATUS
RemoveKeyCode (
  OUT KB_BUFFER         *KeyboardBuffer,
  OUT KEY               *Key
  );

EFI_STATUS
SetKeyLED (
  IN  I2C_KB_DEV        *KbDev
  );

VOID
SyncKbdLed (
  IN  I2C_KB_DEV        *KbDev
  );
#endif
