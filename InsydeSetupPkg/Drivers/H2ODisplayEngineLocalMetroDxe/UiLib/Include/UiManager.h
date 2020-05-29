/** @file

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

#ifndef _UI_MANAGER_H
#define _UI_MANAGER_H

#include <Protocol/PerformanceMeasure.h>
extern EFI_PERFORMANCE_MEASURE_PROTOCOL  *mPerf;
extern EFI_SETUP_MOUSE_PROTOCOL *mSetupMouse;

#include "UiControls.h"

typedef struct _UI_CONTROL UI_CONTROL;
typedef struct _UI_MANAGER UI_MANAGER;

typedef struct {
  LIST_ENTRY Link;
  CHAR16     *Key;
  VOID       *Data;
} STRING_MAP;

typedef
UI_CONTROL *
(EFIAPI *FIND_CONTROL_BY_NAME) (
  IN UI_MANAGER     *This,
  IN CHAR16   *Name
  );

typedef
VOID
(EFIAPI *GET_CONTROL_RECT) (
  IN UI_MANAGER *This,
  IN UI_CONTROL *Control,
  OUT RECT      *Rect
  );

typedef
VOID
(EFIAPI *SET_CONTROL_RECT) (
  IN UI_MANAGER *This,
  IN UI_CONTROL *Control,
  IN RECT       *Rect
  );


typedef
UINTN
(EFIAPI *SET_DEFAULT_FONT_SIZE) (
  IN UI_MANAGER *This,
  IN UINT32      FontSize
  );

typedef
UINT32
(EFIAPI *GET_DEFAULT_FONT_SIZE) (
  IN UI_MANAGER *This
  );

struct _UI_MANAGER {
  HWND           Root;
  HINSTANCE      Instance;
  HDC            PaintDC;
  HDC            OffScreenDc;
  BOOLEAN        DoOffScreen;
  HBITMAP        OldBitmap;
  HWND           MainWnd;
  RECT           InvalidateRect;
  UINT32         DefaultFontSize;
  BOOLEAN        NeedUpdatePos;
  STRING_MAP     OptionGroup;
  STRING_MAP     NameList;

  FIND_CONTROL_BY_NAME          FindControlByName;
  GET_CONTROL_RECT              GetControlRect;
  SET_CONTROL_RECT              SetControlRect;
  SET_DEFAULT_FONT_SIZE         SetDefaultFontSize;
  GET_DEFAULT_FONT_SIZE         GetDefaultFontSize;
};

VOID
EFIAPI
InitUiManager (
  IN UI_MANAGER  *This
  );

VOID
EFIAPI
DestroyUiManager (
  IN UI_MANAGER  *This
  );

UINT64
EFIAPI
StrToUInt (
  IN     CHAR16       *Str,
  IN     UINTN  Base,
  OUT    EFI_STATUS   *Status
  );

BOOLEAN
EFIAPI
StringMapAdd (
  STRING_MAP *Head,
  CHAR16     *Key,
  VOID       *Data
  );

VOID *
EFIAPI
StringMapFind (
  STRING_MAP *Head,
  CHAR16     *Key
  );


VOID *
EFIAPI
StringMapSet (
  STRING_MAP *Head,
  CHAR16     *Key,
  VOID       *Data
  );


BOOLEAN
EFIAPI
StringMapRemove (
  STRING_MAP *Head,
  CHAR16     *Key
  );

VOID
EFIAPI
StringMapRemoveAll (
  STRING_MAP *Head
  );

STRING_MAP *
EFIAPI
StringMapGetAtItem (
  STRING_MAP *Head,
  UINTN      Index
  );

UINTN
EFIAPI
StringMapGetSize (
  STRING_MAP *Head
  );

VOID
EFIAPI
ScuSafeFreePool (
  IN VOID **Buffer
  );

#endif
