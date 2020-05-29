/** @file
  Setup Mouse Protocol definition

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_MOUSE_PROTOCOL_H_
#define _SETUP_MOUSE_PROTOCOL_H_

#include <Uefi.h>

#define EFI_SETUP_MOUSE_PROTOCOL_GUID \
  { 0xec002edf, 0x15e9, 0x499e, 0xad, 0xb1, 0x3d, 0x7b, 0xb1, 0x29, 0x47, 0xf5 }

typedef struct _EFI_SETUP_MOUSE_PROTOCOL EFI_SETUP_MOUSE_PROTOCOL;

//
// Data structures
//
typedef struct {
  UINTN                                 StartX;
  UINTN                                 StartY;
  UINTN                                 EndX;
  UINTN                                 EndY;
} MOUSE_CURSOR_RANGE;

typedef enum {
  EfiSetupMouseScreenText,
  EfiSetupMouseScreenGraphics,
  EfiSetupMouseScreenMaxValue
} EFI_SETUP_MOUSE_SCREEN_MODE;


enum {
  KBCF_X                  = 0x00000001,
  KBCF_Y                  = 0x00000002,
  KBCF_WIDTH_PERCENTAGE   = 0x00000004,
  KBCF_HEIGHT_PERCENTAGE  = 0x00000008,
  KBCF_FIXED_POSITION     = 0x00000010,
} KEYBOARD_CONTROL_FLAG;

typedef struct {
  UINT32    Length;
  UINT32    Flags;
  BOOLEAN   IsStart;
  BOOLEAN   IsFixedPosition;
  INT32     X;
  INT32     Y;
  UINT32    WidthPercentage;
  UINT32    HeightPercentage;
} KEYBOARD_ATTRIBUTES;



typedef
EFI_STATUS
(EFIAPI *EFI_START_MOUSE) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL  *This
);

typedef
EFI_STATUS
(EFIAPI *EFI_CLOSE_MOUSE) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL  *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MOUSE_STATE) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL  *This,
  OUT UINTN                             *X,
  OUT UINTN                             *Y,
  OUT BOOLEAN                           *LeftButton,
  OUT BOOLEAN                           *RightButton
);

typedef
EFI_STATUS
(EFIAPI *EFI_START_KEYBOARD) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL  *This,
  IN  UINTN                             X,
  IN  UINTN                             Y
);

typedef
EFI_STATUS
(EFIAPI *EFI_CLOSE_KEYBOARD) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL  *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SETUP_MOUSE_SET_MODE) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL   *This,
  IN  EFI_SETUP_MOUSE_SCREEN_MODE        SceenMode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SETUP_MOUSE_SET_KEYBOARD_ATTRIBUTES) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL   *This,
  IN  KEYBOARD_ATTRIBUTES                *KeyboardAttributes
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SETUP_MOUSE_GET_KEYBOARD_ATTRIBUTES) (
  IN  struct _EFI_SETUP_MOUSE_PROTOCOL   *This,
  IN  KEYBOARD_ATTRIBUTES                *KeyboardAttributes
  );

struct _EFI_SETUP_MOUSE_PROTOCOL {
  EFI_EVENT                               Event;
  EFI_START_MOUSE                         Start;
  EFI_CLOSE_MOUSE                         Close;
  EFI_MOUSE_STATE                         QueryState;
  EFI_START_KEYBOARD                      StartKeyboard;
  EFI_CLOSE_KEYBOARD                      CloseKeyboard;
  EFI_SETUP_MOUSE_SET_MODE                SetMode;
  EFI_SETUP_MOUSE_SET_KEYBOARD_ATTRIBUTES SetKeyboardAttributes;
  EFI_SETUP_MOUSE_GET_KEYBOARD_ATTRIBUTES GetKeyboardAttributes;
};


extern EFI_GUID gSetupMouseProtocolGuid;

#endif
