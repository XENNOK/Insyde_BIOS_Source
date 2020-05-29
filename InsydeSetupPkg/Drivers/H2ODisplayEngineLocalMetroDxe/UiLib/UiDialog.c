/** @file
  UI Dialog Builder

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

STATIC UI_DIALOG_CLASS       *mDialogClass = NULL;
#define CURRENT_CLASS        mDialogClass

LRESULT
EFIAPI
UiDialogProc ( HWND Hwnd, UINT32 Msg, WPARAM WParam, LPARAM LParam )
{
  UI_DIALOG       *This;
  LRESULT         Ret;
  UI_MANAGER      *Manager;
  //RECT            Rc;
  UI_CONTROL      *Control;

  Manager = NULL;
  This    = (UI_DIALOG *) (UINTN) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *) This;

  if (This != NULL) {
    Manager = Control->Manager;

    Ret     = 0;
    if (This->Proc != NULL) {
      Ret = This->Proc (Hwnd, Msg, WParam, LParam);
    }

    if (Ret) {
      return (BOOLEAN)(Ret != 0);
    }
  }

  switch (Msg) {

    case WM_CREATE:
      This = ((CREATESTRUCT *)(LParam))->lpCreateParams;
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
      break;

    case UI_NOTIFY_CREATE:
      PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
      if (Control != NULL) {
        Manager = Control->Manager;
        if (This->CreateChildsFunc != NULL) {
          This->CreateChildsFunc (Control);
        }
      }
      break;

    default:
      return (BOOLEAN) PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }
  return 0;
}

UI_DIALOG_CLASS *
EFIAPI
GetDialogClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"DIALOG", (UI_CONTROL_CLASS *)GetFrameClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }

  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiDialogProc;

  return CURRENT_CLASS;
}
