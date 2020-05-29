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

#ifndef  _SMM_PLATFORM_KEYBOARD_H_
#define  _SMM_PLATFORM_KEYBOARD_H_

#include <Uefi.h>

#define CHAR_BACKSPACE        0x0008
#define CHAR_TAB              0x0009
#define CHAR_LINEFEED         0x000A
#define CHAR_CARRIAGE_RETURN  0x000D

#define      SMM_INPUT_KBC_CMD_PORT       0x64
#define      SMM_INPUT_KBC_STATUS_PORT    0x64
#define      SMM_INPUT_KBC_DATA_PORT      0x60


#define KBC_OUT_BUFFER_FULL   0x01
#define KBC_INPUT_BUFFER_FULL 0x02
#define KBC_SYSTEM_FLAG       0x04
#define KBC_COMMAND_FLAG      0x08
#define KBC_AUX_DEV_OUTPUT    0x20
#define KBC_PARITY_ERROR      0x80

#define KEYBOARD_WAITFORVALUE_TIMEOUT   1000000 // 1s
#define KEYBOARD_BAT_TIMEOUT            4000000 // 4s
#define KEYBOARD_TIMER_INTERVAL         200000  // 0.02s
#define SCANCODE_EXTENDED               0xE0
#define SCANCODE_EXTENDED1              0xE1
#define SCANCODE_CTRL_MAKE              0x1D
#define SCANCODE_CTRL_BREAK             0x9D
#define SCANCODE_ALT_MAKE               0x38
#define SCANCODE_ALT_BREAK              0xB8
#define SCANCODE_LEFT_SHIFT_MAKE        0x2A
#define SCANCODE_LEFT_SHIFT_BREAK       0xAA
#define SCANCODE_RIGHT_SHIFT_MAKE       0x36
#define SCANCODE_RIGHT_SHIFT_BREAK      0xB6
#define SCANCODE_CAPS_LOCK_MAKE         0x3A
#define SCANCODE_NUM_LOCK_MAKE          0x45
#define SCANCODE_SCROLL_LOCK_MAKE       0x46
#define SCANCODE_MAX_MAKE               0x54

#define KEYBOARD_BUFFER_MAX_COUNT         32

typedef struct {
  EFI_INPUT_KEY               Key;
  BOOLEAN                     Ctrl;
  BOOLEAN                     Alt;
  BOOLEAN                     Shift;
  BOOLEAN                     CapsLock;
  BOOLEAN                     NumLock;
  BOOLEAN                     ScrollLock;
  //
  // Buffer storing key scancodes
  //
  UINT8                       ScancodeBuf[KEYBOARD_BUFFER_MAX_COUNT];
  UINT32                      ScancodeBufStartPos;
  UINT32                      ScancodeBufEndPos;
  UINT32                      ScancodeBufCount;

  //
  // Indicators of the key pressing state, used in detecting Alt+Ctrl+Del
  //
  BOOLEAN                     Ctrled;
  BOOLEAN                     Alted;

  //
  // Error state
  //
  BOOLEAN                     KeyboardErr;
} KEYBOARD_CONSOLE_IN_DEV;

/**
 Read keyboard controller. If OBF == true then transfer scan code.

 @param [out]  OutPutKey        report output key.

 @retval EFI_STATUS             have key output form keyboard controller.
 @retval EFI_NOT_READY          did not found key in output buffer of keyboard controller.

**/
EFI_STATUS
ReadKey(
  OUT   EFI_INPUT_KEY     *OutPutKey
  );

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
  );
#endif


