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
#include "UiControls.h"

STATIC UI_FRAME_CLASS        *mFrameClass = NULL;
#define CURRENT_CLASS        mFrameClass

UI_CONTROL*
EFIAPI
UiFrameGetParent (
  UI_CONTROL *Control
  )
{
  return NULL;
}

LRESULT
EFIAPI
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
  RECT        Rc;
  LRESULT     Result;

  Manager = NULL;

  This    = (UI_FRAME *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *) This;

  if (This != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {
  case WM_CREATE:
    This          = (UI_FRAME *) AllocateZeroPool (sizeof (UI_FRAME));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);

    //
    // change params, set manager to ui control
    //
    ((CREATESTRUCT *)(LParam))->lpCreateParams = Manager;
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    //
    // UiManager
    //
    Manager = AllocateZeroPool (sizeof (UI_MANAGER));
    if (Manager == 0) {
      return 0;
    }
    InitUiManager (Manager);
    Manager->Root         = Hwnd;
    Manager->MainWnd      = Hwnd;
    Manager->Instance     = (HINSTANCE) GetWindowLongPtr (Hwnd, GWLP_HINSTANCE);
    Manager->OffScreenDc  = NULL;
    Manager->PaintDC      = NULL;
    Control->Manager      = Manager;

    //
    // no need set manager
    //
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, 0);
    break;

  case WM_WINDOWPOSCHANGED:
    if (Manager->DoOffScreen) {
      if (Manager->OffScreenDc != NULL) {
        Bitmap = SelectObject (Manager->OffScreenDc, Manager->OldBitmap);
        if (Bitmap != NULL) {
          DeleteObject (Bitmap);
        }
        DeleteDC (Manager->OffScreenDc);
        Manager->OffScreenDc = NULL;

        //
        // create compatiable dc
        //
        GetClientRect (Hwnd, &PaintRc);
        Manager->OffScreenDc = CreateCompatibleDC(NULL);
        if (Manager->OffScreenDc != NULL) {
          Bitmap = CreateCompatibleBitmap(Manager->OffScreenDc, PaintRc.right, PaintRc.bottom);
        }
        if (Bitmap != NULL) {
          Manager->OldBitmap = SelectObject (Manager->OffScreenDc, Bitmap);
          Manager->PaintDC = Manager->OffScreenDc;
        } else {
          ASSERT (FALSE);
        }
      }
    }
    GetWindowRect (Manager->Root, &Rc);
    ShowSetPositionDbgInfo (Control, &Rc);
    mSetPositionLevel++;
    CONTROL_CLASS (Control)->SetPosition (Control, &Rc);
    mSetPositionLevel--;
    break;


  case WM_PAINT:
    GetUpdateRect (Hwnd, &PaintRc, TRUE);
    ValidateRect (Hwnd, NULL);

    ShowUpdateRegionDbgInfo (Control, &PaintRc);
    ASSERT (Manager->Root != NULL);

    PaintDc = GetDC (Hwnd);
    ASSERT (PaintDc != NULL);
    if (PaintDc == NULL) {
      return 0;
    }
    if (Manager->DoOffScreen) {
      if (Manager->OffScreenDc == NULL) {
        GetClientRect (Hwnd, &PaintRc);
        Manager->OffScreenDc = CreateCompatibleDC(NULL);
        if (Manager->OffScreenDc != NULL) {
          Bitmap = CreateCompatibleBitmap(Manager->OffScreenDc, PaintRc.right, PaintRc.bottom);
          Manager->OldBitmap = SelectObject (Manager->OffScreenDc, Bitmap);
          Manager->PaintDC = Manager->OffScreenDc;
        }
      }
    } else {
      Manager->PaintDC = PaintDc;
    }

    if (Manager->NeedUpdatePos || Control->NeedUpdateChildPos) {
      GetWindowRect (Manager->Root, &Rc);
      mSetPositionLevel++;
      CONTROL_CLASS (Control)->SetPosition (Control, &Rc);
      Manager->NeedUpdatePos = FALSE;
      Control->NeedUpdateChildPos = FALSE;
      mSetPositionLevel--;
    }

    Rgn = CreateRectRgnIndirect (&PaintRc);

    //
    // SelectClipRgn will allocate new region for DC, so remove Rgn
    //
    if (Rgn != NULL) {
      SelectClipRgn (Manager->PaintDC, Rgn);
      DeleteObject (Rgn);
    }
    SendMessage   (Manager->Root, UI_NOTIFY_PAINT, PAINT_ALL, (LPARAM)&PaintRc);

    if (Manager->DoOffScreen && Manager->OffScreenDc != NULL) {

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
    Result = PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    DestroyUiManager (Manager);
    return Result;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_FRAME_CLASS *
EFIAPI
GetFrameClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"Frame", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }

  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiFrameProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->GetParent    = UiFrameGetParent;

  return CURRENT_CLASS;
}


