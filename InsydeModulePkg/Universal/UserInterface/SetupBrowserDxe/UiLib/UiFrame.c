/** @file
  UI Frame

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
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"

UI_CONTROL*
UiFrameGetParent (
  UI_CONTROL *Control
  )
{
  return NULL;
}

LRESULT
UiFrameProc(
  HWND Hwnd,
  UINT Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_FRAME    *This;
  RECT        PaintRc;
  HDC         PaintDc;
  HBITMAP     Bitmap;
  HRGN        Rgn;
  UI_MANAGER  *Manager;
  UI_CONTROL  *Control;
  LRESULT     Result;

  Manager = NULL;

  This    = (UI_FRAME *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *) This;

  if (This != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This          = (UI_FRAME *) AllocateZeroPool (sizeof (UI_FRAME));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);

    //
    // no need set manager
    //
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    //
    // UiManager
    //
    Manager = AllocateZeroPool (sizeof (UI_MANAGER));
    InitUiManager (Manager);
    Manager->Root         = Hwnd;
    Manager->MainWnd      = Hwnd;
    Manager->Instance     = (HINSTANCE) GetWindowLongPtr (Hwnd, GWLP_HINSTANCE);
    Manager->OffScreenDc  = NULL;
    Manager->PaintDc      = NULL;
    Control->Manager      = Manager;

    //
    // no need set manager
    //
    UiContainerProc (Hwnd, Msg, WParam, 0);
    Control->GetParent = UiFrameGetParent;
    break;

  case WM_WINDOWPOSCHANGED:
    if (Manager->DoOffScreen) {
      if (Manager->OffScreenDc != NULL) {
        Bitmap = SelectObject (Manager->OffScreenDc, Manager->OldBitmap);
        DeleteObject (Bitmap);
        DeleteDC (Manager->OffScreenDc);
        Manager->OffScreenDc = NULL;
      }
    }
    SendMessage (Manager->Root, UI_NOTIFY_SET_POSITION, 0, 0);
    break;


  case WM_PAINT:
    GetUpdateRect (Hwnd, &PaintRc, TRUE);
    ValidateRect (Hwnd, NULL);

    if (Manager->Root == NULL) {
      return 0;
    }

    PaintDc = GetDC (Hwnd);
    if (Manager->DoOffScreen) {
      if (Manager->OffScreenDc == NULL) {
        GetClientRect (Hwnd, &PaintRc);
        Manager->OffScreenDc = CreateCompatibleDC(NULL);
        Bitmap = CreateCompatibleBitmap(Manager->OffScreenDc, PaintRc.right, PaintRc.bottom);
        Manager->OldBitmap = SelectObject (Manager->OffScreenDc, Bitmap);
        Manager->PaintDc = Manager->OffScreenDc;
      }
    } else {
      Manager->PaintDc = PaintDc;
    }

    if (Manager->NeedUpdatePos) {
      SendMessage (Manager->Root, UI_NOTIFY_SET_POSITION, 0, 0);
      Manager->NeedUpdatePos = FALSE;
    }

    Rgn = CreateRectRgnIndirect (&PaintRc);

    //
    // SelectClipRgn will allocate new region for DC, so remove Rgn
    //
    SelectClipRgn (Manager->PaintDc, Rgn);
    DeleteObject (Rgn);
    SendMessage   (Manager->Root, UI_NOTIFY_PAINT, PAINT_ALL, (LPARAM)&PaintRc);

    if (Manager->DoOffScreen) {

      BitBlt (
        PaintDc,
        PaintRc.left, PaintRc.top, PaintRc.right - PaintRc.left, PaintRc.bottom - PaintRc.top,
        Manager->OffScreenDc,
        PaintRc.left, PaintRc.top,
        SRCCOPY
        );
    }
    ReleaseDC (Hwnd, PaintDc);
    break;

  case WM_DESTROY:
    if (Manager->OffScreenDc != NULL) {
      DeleteDC (Manager->OffScreenDc);

      Manager->OffScreenDc = NULL;
    }
    Result = UiContainerProc (Hwnd, Msg, WParam, LParam);
    DestroyUiManager (Manager);
    return Result;
    break;

  default:
    return UiContainerProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

