/** @file
  UI Common Controls

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
#include <Library/UefiLib.h>
#include "UiControls.h"

STATIC UI_SCROLLBAR_CLASS       *mScrollBarClass = NULL;
#define CURRENT_CLASS           mScrollBarClass

#define SCROLLBAR_TIMER_ID 1

UI_CONTROL *
CreateScrollbarChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyTexture1;
  UI_CONTROL                    *thumb;


  DummyTexture1 = CreateControl (L"Texture", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyTexture1);
  UiApplyAttributeList (DummyTexture1, L"bkimage='@ScrollbarBackground' float='true' height='-1' scale9grid='1,14,1,14' width='-1' name='DummyTexture1'");

  thumb = CreateControl (L"Texture", Root);
  CONTROL_CLASS(Root)->AddChild (Root, thumb);
  UiApplyAttributeList (thumb, L"bkimage='@ScrollbarThumb' float='true' height='25' name='thumb' pos='0,40,25,65' scale9grid='1,13,1,13' width='-1'");

  return DummyTexture1;
}

VOID
EFIAPI
UiScrollBarSetPosition (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  UI_SCROLLBAR                  *This;
  INT32                         Cy;
  INT32                         ThumbY;
  UI_CONTROL                    *Thumb;
  RECT                          Rc;
  INT32                         MinThumbY;
  CHAR16                        Str[100];


  This = (UI_SCROLLBAR *) Control;

  CopyRect (&Rc, Pos);
  OffsetRect (&Rc, -Rc.left, -Rc.top);

  Cy = Rc.bottom - Rc.top;
//ThumbY = Cy * (Rc.bottom - Rc.top) / (This->Range + Rc.bottom - Rc.top);
  MinThumbY = 20;
  if ((Cy - MinThumbY) <= (This->Range / This->LineSize)) {
    ThumbY = MinThumbY;
    This->LineSize = This->Range / (Cy - ThumbY) + 1;
  } else {
    ThumbY = Cy - (This->Range / This->LineSize);
  }

  if (This->Range == 0) {
    This->Thumb.left   = Rc.left;
    This->Thumb.right  = Rc.right;
    This->Thumb.top    = 0;
    This->Thumb.bottom = Cy;
  } else {
    This->Thumb.left   = Rc.left;
    This->Thumb.right  = Rc.right;
    This->Thumb.top    = This->ScrollPos * (Cy - ThumbY) / (This->Range / This->LineSize);
    This->Thumb.bottom = This->Thumb.top + ThumbY;
  }

  Thumb = UiFindChildByName (This, L"thumb");
  if (Thumb != NULL) {
    UnicodeSPrint (Str, sizeof (Str), L"%d,%d,%d,%d", This->Thumb.left, This->Thumb.top, This->Thumb.right, This->Thumb.bottom);
    UiSetAttribute (Thumb, L"pos", Str);
  }

  SetWindowPos (Control->Wnd, HWND_TOP, Pos->left, Pos->top, Pos->right - Pos->left, Pos->bottom - Pos->top, 0);
  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetPosition (Control,Pos);
}


VOID
EFIAPI
UiScrollBarSetScrollPos (
  UI_SCROLLBAR                  *This,
  INT32                         Pos
  )
{
  RECT                          Rc;
  INT32                         ScrollPos;

  if (This->ScrollPos == Pos) {
    return ;
  }

  if (Pos < 0) {
    if (This->ScrollPos == 0) {
      return ;
    }
    Pos = 0;
  }

  GetClientRect (This->Control.Wnd, &Rc);
  if ((Pos + (This->Thumb.bottom - This->Thumb.top)) > Rc.bottom) {
    ScrollPos = Rc.bottom - (This->Thumb.bottom - This->Thumb.top);
    if (ScrollPos == This->ScrollPos) {
      return ;
    }
    This->ScrollPos = ScrollPos;
    if (This->Owner != NULL) {
      This->Owner->ScrollPos.y = This->Range;
    }
  } else {
    This->ScrollPos = Pos;
    if (This->Owner != NULL) {
      This->Owner->ScrollPos.y = This->ScrollPos * This->LineSize;
    }
  }

  if (This->Owner != NULL) {
    UiNeedUpdatePos (This->Owner);
    This->Owner->Class->Invalidate (This->Owner);
  }
}

VOID
UiScrollBarCreate  (
  UI_SCROLLBAR *This
  )
{
  This->Owner      = NULL;
  This->Horizontal = FALSE;

  This->Range          = 19;
  This->LineSize       = 1;
  This->ScrollPos      = 0;
  This->LastScrollPos = 0;
  This->LastScrollOffset = 0;
  This->ScrollRepeatDelay  = 0;

  This->LastMouse.x    = 0;
  This->LastMouse.y    = 0;

  This->ShowThumb      = TRUE;
}

VOID
UiScrollBarSetHorizontal (
  UI_SCROLLBAR *This,
  BOOLEAN      IsHor
  )
{

  UI_CONTROL *Control;

  Control = (UI_CONTROL *) This;

  This->Horizontal    = IsHor;

  if (IsHor) {
    This->Horizontal    = IsHor;
    if (Control->FixedSize.cy == 0) {
      Control->FixedSize.cx = 0;
      Control->FixedSize.cy = 20;
    }
  } else {
    if (Control->FixedSize.cx == 0) {
      Control->FixedSize.cx = 25;
      Control->FixedSize.cy = 0;
    }
  }
}

BOOLEAN
EFIAPI
UiScrollBarSetAttribute (
  UI_CONTROL  *Control,
  CHAR16      *Name,
  CHAR16      *Value
  )
{


  UI_SCROLLBAR *This;

  This = (UI_SCROLLBAR *) Control;

  if (StrCmp (Name, L"hor") == 0) {
    UiScrollBarSetHorizontal (This, (StrCmp (Value, L"true") == 0));
  } else if (StrCmp (Name, L"displayscrollbar") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      UiScrollBarSetHorizontal (This, This->Horizontal);
    } else {
      Control->FixedSize.cx = 0;
      Control->FixedSize.cy = 0;
    }
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

VOID
UiScrollbarInvalidate (
  UI_CONTROL *Control
  )
{
  PARENT_CONTROL_CLASS (CURRENT_CLASS)->Invalidate (Control);
}


LRESULT
EFIAPI
UiScrollBarProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_SCROLLBAR *This;
  UI_CONTROL   *Control;
  POINT        Point;
  RECT         Rc;
  UI_MANAGER   *Manager;

  Control = (UI_CONTROL *) GetWindowLongPtr (Wnd, 0);
  if (Control == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  This    = (UI_SCROLLBAR *) Control;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_SCROLLBAR *) AllocateZeroPool (sizeof (UI_SCROLLBAR));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    UiScrollBarCreate (This);
    Manager = Control->Manager;
    CreateScrollbarChilds (Control);
    break;

//    UiScrollBarSetPosition (This);
//    return
//    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_ALL) {
      return PARENT_CLASS_WNDPROC(CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    }
    break;

  case WM_LBUTTONDOWN:
    POINTSTOPOINT(Point, LParam);
    if (This->Range == 0) {
      break;
    }
    if (PtInRect (&This->Thumb, Point)) {
      This->ThumbState |= UISTATE_CAPTURED | UISTATE_PRESSED;
      ClientToScreen (Wnd, &Point);
      This->LastMouse     = Point;
      This->LastScrollPos = This->ScrollPos;
    } else {
      if (Point.y < This->Thumb.top) {
        UiScrollBarSetScrollPos (This, This->ScrollPos - This->LineSize);
      } else if (Point.y > This->Thumb.bottom) {
        UiScrollBarSetScrollPos (This, This->ScrollPos + This->LineSize);
      }
    }
    if (GetCapture () != Wnd) {
      SetCapture (Wnd);
      This->ScrollRepeatDelay = 0;
      SetTimer (Wnd, SCROLLBAR_TIMER_ID, 50, NULL);
      Control->Class->Invalidate (Control);
    }
    break;

  case WM_LBUTTONUP:
  case WM_NCLBUTTONUP:
    if (GetCapture () == Wnd) {
      ReleaseCapture();
      KillTimer (Wnd, SCROLLBAR_TIMER_ID);
      if ((This->ThumbState & UISTATE_CAPTURED) != 0) {
        This->ThumbState &= ~( UISTATE_CAPTURED | UISTATE_PRESSED );
        Control->Class->Invalidate (Control);
      }
    }
    break;

  case WM_TIMER:
    ++This->ScrollRepeatDelay;
    if ((This->ThumbState & UISTATE_CAPTURED) != 0) {
      if (This->Horizontal) {
        UiScrollBarSetScrollPos (This, This->LastScrollPos + This->LastScrollOffset);
      } else {
        UiScrollBarSetScrollPos (This, This->LastScrollPos + This->LastScrollOffset);
      }
    } else {

      if (This->ScrollRepeatDelay < 5) {
        break;
      }
      GetCursorPos (&Point);
      ScreenToClient (Wnd, &Point);

      if (Point.y < This->Thumb.top) {
        UiScrollBarSetScrollPos (This, This->ScrollPos - This->LineSize);
      } else if (Point.y > This->Thumb.bottom) {
        UiScrollBarSetScrollPos (This, This->ScrollPos + This->LineSize);
      }

    }
    break;

  case WM_MOUSEMOVE:
  case WM_NCMOUSEMOVE:
    POINTSTOPOINT(Point, LParam);
    GetClientRect (Wnd, &Rc);
    if (Msg == WM_MOUSEMOVE) {
      ClientToScreen (Wnd, &Point);
    }

    if ((This->ThumbState & UISTATE_CAPTURED) != 0) {
      if (This->Horizontal) {
        This->LastScrollOffset = (Point.x - This->LastMouse.x) * This->Range / (Rc.right - Rc.left - This->Thumb.right + This->Thumb.left - 2 * Control->FixedSize.cy);
      } else {
        This->LastScrollOffset = (Point.y - This->LastMouse.y) * (This->Range / This->LineSize) / (Rc.bottom - Rc.top - This->Thumb.bottom + This->Thumb.top);
      }
    }
    break;


  case WM_MOUSEENTER:
    POINTSTOPOINT(Point, LParam);
    ScreenToClient (Wnd, &Point);
//    This->Button1State |= UISTATE_HOVER;
//    This->Button2State |= UISTATE_HOVER;
//  if(PtInRect(&This->Thumb, Point) ) {
    This->ThumbState |= UISTATE_HOVER;
//  }
    Control->Class->Invalidate (Control);
    break;


  case WM_MOUSELEAVE:
//    This->Button1State &= ~UISTATE_HOVER;
//    This->Button2State &= ~UISTATE_HOVER;
    This->ThumbState   &= ~UISTATE_HOVER;
    Control->Class->Invalidate (Control);
    break;

  case WM_DESTROY:
    KillTimer (Wnd, SCROLLBAR_TIMER_ID);
    return PARENT_CLASS_WNDPROC(CURRENT_CLASS, Wnd, Msg, WParam, LParam);

  default:
    return PARENT_CLASS_WNDPROC(CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_SCROLLBAR_CLASS *
EFIAPI
GetScrollBarClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }


  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"ScrollBar", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiScrollBarProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiScrollBarSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetPosition  = UiScrollBarSetPosition;

  CURRENT_CLASS->SetScrollPos                       = UiScrollBarSetScrollPos;

  return CURRENT_CLASS;
}

