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
#include "UiControls.h"

STATIC UI_BUTTON_CLASS           *mButtonClass = NULL;
#define CURRENT_CLASS            mButtonClass

BOOLEAN
EFIAPI
UiButtonSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_BUTTON                     *This;

  This = (UI_BUTTON *) Control;

  if (StrCmp (Name, L"normalimage") == 0) {
    if (This->NormalImage != NULL) {
      FreePool (This->NormalImage);
    }
    This->NormalImage = StrDuplicate (Value);
  } else if (StrCmp (Name, L"hoverimage") == 0) {
    if (This->HoverImage != NULL) {
      FreePool (This->HoverImage);
    }
    This->HoverImage = StrDuplicate (Value);
  } else if (StrCmp (Name, L"pushimage") == 0) {
    if (This->PushImage != NULL) {
      FreePool (This->PushImage);
    }
    This->PushImage = StrDuplicate (Value);
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

VOID
UiButtonPaintStatusImage (
  UI_BUTTON                     *This,
  HDC                           Hdc
  )
{

  RECT   Rc;
  UI_CONTROL   *Control;
  UI_MANAGER   *Manager;

  Control = (UI_CONTROL *)This;
  Manager = Control->Manager;

  Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rc);

  SetBkMode(Hdc, TRANSPARENT);


  if (CONTROL_CLASS_GET_STATE(Control) & UISTATE_PRESSED) {

    if (This->PushImage != 0) {
      UiShowBitmap (Control->Wnd, Manager->PaintDC, &Rc, &Control->Scale9Grid, GetImageByString (This->PushImage), Control->BkImageStyle, &Control->HsvDiff);
    }
  } else if (CONTROL_CLASS_GET_STATE(Control) & UISTATE_HOVER) {
    if (This->HoverImage != 0) {
      UiShowBitmap (Control->Wnd, Manager->PaintDC, &Rc, &Control->Scale9Grid, GetImageByString (This->HoverImage), Control->BkImageStyle, &Control->HsvDiff);
    }
  } else {
    if (This->NormalImage != 0) {
      UiShowBitmap (Control->Wnd, Manager->PaintDC, &Rc, &Control->Scale9Grid, GetImageByString (This->NormalImage), Control->BkImageStyle, &Control->HsvDiff);
    }
  }

}

SIZE
EFIAPI
UiButtonEstimateSize (
  UI_CONTROL                    *Control,
  SIZE                          AvailableSize
  )
{

  SIZE Size;

  if (Control->FixedSize.cx == 0 || Control->FixedSize.cy == 0 ||
     (Control->FixedSize.cy == -2 && Control->MinSize.cy != 0)) {
    Size = PARENT_CONTROL_CLASS(CURRENT_CLASS)->EstimateSize (Control, AvailableSize);
  } else {
    Size = Control->FixedSize;
  }

  return Size;
}


LRESULT
EFIAPI
UiButtonProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_BUTTON    *This;
  UI_CONTROL   *Control;
  UI_MANAGER   *Manager;
  RECT   Rc;
  POINT  Point;

  Manager = NULL;

  This = (UI_BUTTON *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;
  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_BUTTON *) AllocateZeroPool (sizeof (UI_BUTTON));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    ((UI_LABEL *)This)->TextStyle = DT_VCENTER;
    Control->BkImageStyle = DT_CENTER | DT_VCENTER;
    Control->StyleFlags |= UISTYLE_CLICKABLE;
    SetWindowLongPtr (
      Control->Wnd,
      GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE
      );
    break;

  case WM_MOUSEENTER:
    CONTROL_CLASS_SET_STATE(Control, UISTATE_HOVER, 0);
    CONTROL_CLASS(Control)->Invalidate (Control);
    break;

  case WM_MOUSELEAVE:
    CONTROL_CLASS_SET_STATE(Control, 0, UISTATE_HOVER);
    Control->Class->Invalidate (Control);
    break;

  case WM_NCLBUTTONUP:
    if(GetCapture() == Wnd && ((CONTROL_CLASS_GET_STATE(Control) & UISTATE_CAPTURED) != 0)) {
      GetWindowRect (Wnd, &Rc);
      Point.x = LOWORD (LParam);
      Point.y = HIWORD (LParam);
      if (!PtInRect (&Rc, Point)) {
        ReleaseCapture();
        CONTROL_CLASS_SET_STATE(Control, 0, UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
        Control->Class->Invalidate (Control);
      }
    }
    break;

  case WM_LBUTTONUP:
    if (GetCapture () == Wnd) {
      ReleaseCapture ();
    }

    if ((CONTROL_CLASS_GET_STATE(Control) & UISTATE_CAPTURED) == 0) {
      break;
    }

    CONTROL_CLASS_SET_STATE(Control, 0, UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
    GetClientRect (Wnd, &Rc);


    Point.x = LOWORD (LParam);
    Point.y = HIWORD (LParam);
    Control->Class->Invalidate (Control);

    if (PtInRect (&Rc, Point)) {
      SendMessage (Wnd, UI_NOTIFY_ACTIVATE, 0, 0);
    }
    break;


  case UI_NOTIFY_ACTIVATE:
    SendMessage (Manager->MainWnd, UI_NOTIFY_CLICK, (WPARAM)This, 0);
    return TRUE;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_STATUSIMAGE) {
      UiButtonPaintStatusImage (This, Manager->PaintDC);
    } else  {
      if (WParam == PAINT_BKCOLOR) {
        if ((CONTROL_CLASS_GET_STATE(Control) & UISTATE_PRESSED && (This->PushImage != 0)) ||
            (CONTROL_CLASS_GET_STATE(Control) & UISTATE_HOVER &&  (This->HoverImage != 0)) ||
            (This->NormalImage != 0)) {
            return 0;
        }
      }

      PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    }
    break;

  case WM_NCHITTEST:
    return HTCLIENT;

  case WM_DESTROY:
    if (This->NormalImage != NULL) {
      FreePool (This->NormalImage);
    }
    if (This->HoverImage != NULL) {
      FreePool (This->HoverImage);
    }
    if (This->PushImage != NULL) {
      FreePool (This->PushImage);
    }
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);

  }
  return 0;

}


UI_BUTTON_CLASS *
EFIAPI
GetButtonClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"Button", (UI_CONTROL_CLASS *)GetLabelClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiButtonProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiButtonSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->EstimateSize = UiButtonEstimateSize;

  return CURRENT_CLASS;
}

