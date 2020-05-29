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

#ifndef _UI_CONTROL_H
#define _UI_CONTROL_H

#include <Uefi.h>
#include <Protocol/H2OWindow.h>

typedef struct _UI_MANAGER   UI_MANAGER;
typedef struct _UI_CONTROL   UI_CONTROL;
typedef struct _UI_CONTAINER UI_CONTAINER;

typedef
BOOLEAN
(EFIAPI *UI_CONTROL_SET_ATTRIBUTE) (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  );

typedef
UI_CONTROL *
(EFIAPI *UI_CONTROL_GET_INTERFACE) (
  UI_CONTROL *Control,
  CHAR16     *ClassName
  );


typedef
BOOLEAN
(EFIAPI *UI_CONTAINER_ADD) (
  UI_CONTAINER  *Container,
  UI_CONTROL    *Control
  );

typedef
BOOLEAN
(EFIAPI *UI_CONTAINER_REMOVE) (
  UI_CONTAINER  *Container,
  UI_CONTROL    *Control
  );

typedef
BOOLEAN
(EFIAPI *UI_CONTAINER_REMOVE_ALL) (
  UI_CONTAINER  *Container
  );

typedef
SIZE
(EFIAPI *UI_CONTROL_ESTIMATE_SIZE) (
  UI_CONTROL *Control,
  SIZE       AvailableSize
  );

#pragma pack(1)


//
// Flags used for control state
//
typedef enum {
  UISTYLE_CLICKABLE     = 0x00010000,
  UISTYLE_LONGCLICKABLE = 0x00020000,
  UISTYLE_FOCUSABLE     = 0x00040000,
} UI_STYLE;


//
// Flags used for controlling the paint
//
typedef enum {
  UISTATE_FOCUSED     = 0x00000001,
  UISTATE_SELECTED    = 0x00000002,
  UISTATE_DISABLED    = 0x00000004,
  UISTATE_HOT         = 0x00000008,
  UISTATE_PRESSED     = 0x00000010,
  UISTATE_READONLY    = 0x00000020,
  UISTATE_CAPTURED    = 0x00000040,
  UISTATE_INVALIDATED = 0x00000080,
  UISTATE_PREPRESSED  = 0x00000100,
} UI_STATE;

struct _UI_CONTROL {
  HWND         Wnd;
  UI_CONTROL_SET_ATTRIBUTE SetAttribute;
  UI_CONTROL_ESTIMATE_SIZE EstimateSize;
  VOID                     (*Invalidate)(UI_CONTROL *Control);
  UI_CONTROL*              (*FindNextFocus)(UI_CONTROL *Control, UI_CONTROL *Focused, UINTN Direction);
  UI_CONTROL*              (*GetParent) (UI_CONTROL *Control);
  BOOLEAN                  (*ShouldDelayChildPressedState)(UI_CONTROL *Control);
  UI_STYLE                 StyleFlags;
  UI_STATE                 StateFlags;

  INT32        LastX;
  INT32        LastY;

  COLORREF     BkColor;
  COLORREF     BkColor2;
  COLORREF     BkColor3;
  COLORREF     BorderColor;
  SIZE         FixedXY;
  SIZE         FixedSize;
  SIZE         BoardRound;
  RECT         Padding;
  RECT         Scale9Grid;
  VOID         *UserData;
  CHAR16       *Name;
  UI_MANAGER   *Manager;
  UINT32       BkImage;
  UINT32       BkImageStyle;
  BOOLEAN      Float;
  BOOLEAN      GradientVertial;
  UINT32       FontSize;
  BOOLEAN      AlwaysOnTop;
};



typedef struct _UI_LABEL {
  UI_CONTROL Control;

  UINT32  TextColor;
  UINT32  TextStyle;
  RECT    TextPadding;
} UI_LABEL;


typedef struct _UI_BUTTON {
  UI_LABEL Label;


  BOOLEAN  CheckBoxValue;

  SIZE     Padding;
  UINTN    TextStyle;

  UINT32   NormalImage;
  UINT32   HotImage;
  UINT32   PushImage;
} UI_BUTTON;


typedef struct _UI_SWITCH {
  UI_BUTTON Button;
  BOOLEAN   CheckBoxValue;
  RECT      ThumbRc;
  INT32     ThumbPos;
  BOOLEAN   IsHitThumb;
  BOOLEAN   MoveThumb;
} UI_SWITCH;

typedef struct _UI_EDIT {
  UI_LABEL   Label;

//  INT32    CaretX;

  UINT32      EditPos;
  COLORREF   EditTextColor;
  COLORREF   EditBkColor;


  UINTN      Flags;
  UINT32     CharHeight;

  BOOLEAN    IsDec;
  BOOLEAN    IsHex;

  UINT64     MaxValue;
  UINT64     MinValue;
  UINT64     Step;

  BOOLEAN    IsPasswordMode;
  BOOLEAN    IsReadOnly;

} UI_EDIT;


typedef struct _UI_SCROLLBAR UI_SCROLLBAR;

typedef
VOID
(EFIAPI *UI_SCROLLBAR_SET_POS) (
  IN UI_SCROLLBAR *This,
  IN INT32  Pos
  );

struct _UI_SCROLLBAR {
  UI_CONTROL Control;
  UI_CONTAINER *Owner;
  UI_SCROLLBAR_SET_POS SetScrollPos;

  BOOLEAN    Horizontal;
  INT32      Range;
  INT32      ScrollPos;
  INT32      LineSize;
  POINT      LastMouse;
  INT32      LastScrollPos;
  INT32      LastScrollOffset;
  INTN       ScrollRepeatDelay;

  UINTN      Button1State;
  UINTN      Button2State;
  UINTN      ThumbState;

  BOOLEAN    ShowButton1;
  BOOLEAN    ShowButton2;
  BOOLEAN    ShowThumb;


  RECT       Button1;
  RECT       Button2;
  RECT       Thumb;

};

struct _UI_CONTAINER {
  UI_CONTROL       Control;
  UI_CONTAINER_ADD Add;
  UI_CONTAINER_REMOVE Remove;
  UI_CONTAINER_REMOVE_ALL RemoveAll;
  UINTN      ItemCount;
  UI_CONTROL       **Items;
  RECT       Inset;
  UINT32     Inset2;
  UINT32     ChildPadding;

  UI_SCROLLBAR     *VScrollBar;
  UI_SCROLLBAR     *HScrollBar;

  INT32      LastMotionX;
  INT32      LastMotionY;

  BOOLEAN    IsBeginDraged;

  POINT      ScrollPos;

  INT32      TouchSlop;

};

typedef struct _UI_FRAME {
  UI_CONTAINER Container;
  BOOLEAN      DoOffScreen;
} UI_FRAME;

LRESULT
UiFrameProc(
  HWND Hwnd,
  UINT Msg,
  WPARAM WParam,
  LPARAM LParam
  );

typedef struct _UI_DRAG_CONTAINER {
  UI_CONTAINER Container;
  POINT  DragPoint;
  POINT  OrgPoint;
  SIZE   OrgSize;
  BOOLEAN      LButtonDown;
  BOOLEAN      DragMove;
  BOOLEAN      DragSize;
  BOOLEAN      DragStart;

} UI_DRAG_CONTAINER;

typedef struct _UI_VERTICAL_LAYOUT {
  UI_CONTAINER Container;

} UI_VERTICAL_LAYOUT;


typedef struct _UI_HORIZONTAL_LAYOUT {
  UI_CONTAINER Container;

} UI_HORIZONTAL_LAYOUT;


typedef struct _UI_TILE_LAYOUT {
  UI_CONTAINER Container;
  UINT32       Columns;
  UINT32       FixedHeight;

} UI_TILE_LAYOUT;

#pragma pack()


BOOLEAN
UiSetAttribute (
  VOID       *This,
  CHAR16     *Name,
  CHAR16     *Value
  );

VOID
UiControlSetPos (
  UI_CONTROL *This,
  RECT       Rc
  );


BOOLEAN
UiControlSetAttribute (
  UI_CONTROL *This,
  CHAR16     *Name,
  CHAR16     *Value
  );

LRESULT
UiControlProc (
  HWND   Hwnd,
  UINT32 Message,
  WPARAM WParam,
  LPARAM LParam
  );

LRESULT
UiContainerProc (
  HWND   Hwnd,
  UINT32 Message,
  WPARAM WParam,
  LPARAM LParam
  );

LRESULT
UiVerticalLayoutProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  );

UINT32
RegisterUiListView (
  HINSTANCE hInstance
  );



LRESULT
UiDragContainerProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  );

BOOLEAN
UiContainerSetAttribute (
  UI_CONTROL   *Control,
  CHAR16       *Name,
  CHAR16       *Value
  );


enum {
  PAINT_BKCOLOR,
  PAINT_BKIMAGE,
  PAINT_STATUSIMAGE,
  PAINT_TEXT,
  PAINT_BORDER,
  PAINT_ALL
} PAINT_TYPE;

#define UI_COMMON_NOTIFY   (WM_USER + 0x100)

#define UI_NOTIFY_FIRST         (UI_COMMON_NOTIFY)

#define UI_NOTIFY_PAINT           (UI_COMMON_NOTIFY)
#define UI_NOTIFY_ACTIVATE        (UI_COMMON_NOTIFY + 1)
#define UI_NOTIFY_SET_POSITION    (UI_COMMON_NOTIFY + 2)
#define UI_NOTIFY_CREATE          (UI_COMMON_NOTIFY + 3)
#define UI_NOTIFY_WINDOWINIT      (UI_COMMON_NOTIFY + 4)
#define UI_NOTIFY_CARRIAGE_RETURN (UI_COMMON_NOTIFY + 5)
#define UI_NOTIFY_CLICK           (UI_COMMON_NOTIFY + 6)
#define UI_NOTIFY_BLUR            (UI_COMMON_NOTIFY + 7)
#define UI_NOTIFY_EDITFOCUS       (UI_COMMON_NOTIFY + 8)
#define UI_NOTIFY_LAST            (UI_COMMON_NOTIFY + 9)

UINT32
RegisterUiControl (
  HINSTANCE hInstance
  );

UINT32
RegisterUiLabel (
  HINSTANCE hInstance
  );

UINT32
RegisterUiButton (
  HINSTANCE hInstance
  );

UINT32
RegisterUiLayout (
  HINSTANCE hInstance
  );

UINT32
RegisterUiContainer (
  HINSTANCE hInstance
  );

UINT32
RegisterUiDragContainer (
  HINSTANCE hInstance
  );

UINT32
RegisterUiVerticalLayout (
  HINSTANCE hInstance
  );

UINT32
RegisterUiScrollBar (
  HINSTANCE hInstance
  );

UINT32
RegisterUiSwitch (
  HINSTANCE hInstance
  );


UINT32
RegisterUiEditBar (
  HINSTANCE hInstance
  );

UINT32
RegisterUiHorizontalLayout (
  HINSTANCE hInstance
  );
UINT32
RegisterUiTileLayout (
  HINSTANCE hInstance
  );

UINT32
RegisterUiCheckbox (
  HINSTANCE hInstance
  );


UI_CONTROL *
GetUiControl (
  HWND Hwnd
  );


UI_CONTROL *
CreateControl (
  CHAR16     *ControlName,
  CHAR16     *Text,
  UI_CONTROL *Parent,
  UI_MANAGER *Manager
  );


UI_CONTROL *
UiDlgBuilderCreate (
  CHAR16     *XMLBuffer,
  UI_MANAGER *Manager,
  HWND       Parent
  ) ;

typedef struct _UI_DIALOG {
  UI_FRAME    Frame;
  HINSTANCE   Instance;
  HWND        ParentWnd;
  CHAR16      *XmlBuffer;
  WNDPROC     Proc;
  LPARAM      Param;
  BOOLEAN     Running;
  INTN        Ret;
} UI_DIALOG;

UINT32
RegisterUiDialog (HINSTANCE hInstance);

INTN
UiDialogBoxParam (
  HINSTANCE  Instance,
  CHAR16     *XmlBuffer,
  HWND       ParentWnd,
  WNDPROC    DialogProc,
  LPARAM     Param,
  INT32      X,
  INT32      Y,
  INT32      Width,
  INT32      Height,
  BOOLEAN    CloseOnTouchOutside
  );


UI_CONTROL *
ContainerCreateControl (
  UI_CONTAINER *Container,
  CHAR16     *ControlName,
  CHAR16     *Text
  );

BOOL WINAPI
UiEndDialog(HWND hDlg, int nResult);


VOID
UiApplyAttributeList (
  UI_CONTROL *Control,
  CHAR16     *StrList
  );

UINT32
GetImageIdByName (
  CHAR16  *Name
  );


#endif
