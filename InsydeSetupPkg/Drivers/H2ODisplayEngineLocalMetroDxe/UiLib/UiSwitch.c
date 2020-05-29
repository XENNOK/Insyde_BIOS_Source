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
#include "H2ODisplayEngineLocalMetro.h"
#include "MetroUi.h"

STATIC UI_SWITCH_CLASS           *mSwitchClass = NULL;
#define CURRENT_CLASS            mSwitchClass

UI_CONTROL *
CreateSwitchChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *CheckboxBorder;
  UI_CONTROL                    *CheckboxBkg;
  UI_CONTROL                    *CheckboxText;
  UI_CONTROL                    *CheckboxButton;
  UI_CONTROL                    *ButtonHorizontal;


  CheckboxBorder = CreateControl (L"Label", Root);
  CONTROL_CLASS(Root)->AddChild (Root, CheckboxBorder);
  UiApplyAttributeList (CheckboxBorder, L"bkimage='@CheckBoxBorder' float='true' name='CheckboxBorder' scale9grid='14,1,14,1'");

  CheckboxBkg = CreateControl (L"Label", Root);
  CONTROL_CLASS(Root)->AddChild (Root, CheckboxBkg);
  UiApplyAttributeList (CheckboxBkg, L"bkimage='@CheckBoxBkg' float='true' name='CheckboxBkg' padding='3,5,4,5' scale9grid='14,1,14,1'");

  CheckboxText = CreateControl (L"Label", Root);
  CONTROL_CLASS(Root)->AddChild (Root, CheckboxText);
  UiApplyAttributeList (CheckboxText, L"float='true' fontsize='17' name='CheckboxText' textcolor='0xFFFFFFFF'");

  ButtonHorizontal = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, ButtonHorizontal);
  UiApplyAttributeList (ButtonHorizontal, L"float='true' name='CheckboxButton'");

  CheckboxButton = CreateControl (L"Label", ButtonHorizontal);
  CONTROL_CLASS(Root)->AddChild (ButtonHorizontal, CheckboxButton);
  UiApplyAttributeList (CheckboxButton, L"bkimage='@CheckBoxButton' scale9grid='11,1,11,1'");

  return CheckboxBorder;
}

#define SWITCH_TEXT_LENGTH       60
#define SWITCH_BORDER_WIDTH      4


VOID
UpdateSwitch (
  UI_CONTROL                    *Control,
  CONST RECT                    *Rc
  )
{
  UI_SWITCH                     *This;
  UI_CONTROL                    *Child;
  CHAR16                        Str[20];
  INT32                         Width;
  INT32                         Height;
  INT32                         ThumbWidth;
  INT32                         ThumbHeight;
  INT32                         FreeWidth;

  This = (UI_SWITCH *) Control;
  Width  = Rc->right   -  Rc->left;
  Height =  Rc->bottom -  Rc->top;
  ASSERT (Width >= 98 && Height >= 20);
  ThumbWidth  = (Width - SWITCH_BORDER_WIDTH * 2) / 3;
  ThumbHeight = Height - 7;

  UnicodeSPrint (Str, sizeof (Str), L"3,%d,4,%d", Width - ThumbWidth - 4 -  This->ThumbPos, This->ThumbPos + 4);
  Child = UiFindChildByName (Control, L"CheckboxButton");
  UiSetAttribute (Child, L"padding", Str);
  if (This->ThumbPos < This->ThumbRange / 2) {
    Child = UiFindChildByName (Control, L"CheckboxText");
    UiSetAttribute (Child, L"text", L"Enabled");
    FreeWidth = Width - SWITCH_TEXT_LENGTH - ThumbWidth - SWITCH_BORDER_WIDTH * 2;
    UnicodeSPrint (Str, sizeof (Str), L"%d,4,4,%d", (Height - 20) / 2, ThumbWidth + SWITCH_BORDER_WIDTH + FreeWidth / 2);
    UiSetAttribute (Child, L"padding", Str);
  } else {
    Child = UiFindChildByName (Control, L"CheckboxText");
    UiSetAttribute (Child, L"text", L"Disabled");
    FreeWidth = Width - SWITCH_TEXT_LENGTH - ThumbWidth - SWITCH_BORDER_WIDTH * 2;
    UnicodeSPrint (Str, sizeof (Str), L"%d,4,4,%d", (Height - 20) / 2, SWITCH_BORDER_WIDTH + FreeWidth / 2);
    UiSetAttribute (Child, L"padding", Str);
  }
}

VOID
EFIAPI
UiSwitchSetPosition (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  UI_SWITCH                     *This;
  INT32                         Width;
  INT32                         ThumbWidth;

  This = (UI_SWITCH *) Control;
  if (!This->InitThumb) {
    Width = Pos->right - Pos->left;
    ThumbWidth  = (Width - SWITCH_BORDER_WIDTH * 2) / 3;
    This->ThumbRc.left   = SWITCH_BORDER_WIDTH;
    This->ThumbRc.top    = 3;
    This->ThumbRc.right  = This->ThumbRc.left + ThumbWidth;
    This->ThumbRc.bottom = Pos->bottom - Pos->top - 4;
    This->ThumbRange = (Width - SWITCH_BORDER_WIDTH * 2) - ThumbWidth;
    if (This->CheckBoxValue) {
      This->ThumbPos = 0;
    } else {
      This->ThumbPos = This->ThumbRange;
    }
    This->InitThumb = TRUE;
  }

  UpdateSwitch (Control, Pos);
  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetPosition (Control,Pos);
  SetWindowPos (Control->Wnd, HWND_TOP, Pos->left, Pos->top, Pos->right - Pos->left, Pos->bottom - Pos->top, 0);
}

VOID
GetCurrentCheckBoxBkgHsv (
  OUT HSV_VALUE      *Hsv
  )
{
  EFI_IMAGE_INPUT         *Image;
  UINTN                   Index;
  UINTN                   BufferLen;

  Image = GetImageByString (L"@CheckBoxBkg");
  ASSERT (Image != NULL);
  if (Image == NULL) {
    return;
  }
  BufferLen = Image->Width * Image->Height;
  for (Index = 0; Index < BufferLen; Index++) {
    if (Image->Bitmap[Index].Reserved != 0) {
      RGB2HSV (&Image->Bitmap[Index], Hsv);
      return;
    }
  }
  ASSERT (FALSE);
}

STATIC
VOID
UpdateCheckBoxColor (
  IN UI_SWITCH              *This
  )
{
  UI_CONTROL                    *Control;
  HSV_VALUE                     MenuHsv;
  HSV_VALUE                     HaloHsv;
  INT16                         HueDiff;
  INT8                          SaturationDiff;
  INT8                          ValueDiff;
  CHAR16                        Str[20];

  ASSERT (This != NULL);
  GetCurrentCheckBoxBkgHsv (&HaloHsv);
  GetCurrentMenuHsv (&MenuHsv);
  if (MenuHsv.Hue == 0 && MenuHsv.Saturation == 0 && MenuHsv.Value == 0) {
    return;
  }
  HueDiff        = (INT16) (MenuHsv.Hue - HaloHsv.Hue);
  SaturationDiff = (INT8)  (MenuHsv.Saturation - HaloHsv.Saturation);
  ValueDiff      = (INT8)  (MenuHsv.Value - HaloHsv.Value);
  UnicodeSPrint (Str, sizeof (Str), L"%d,%d,%d",  HueDiff, SaturationDiff, ValueDiff);
  Control = UiFindChildByName (This, L"CheckboxBkg");
  PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, L"hsvadjust", Str);
}


BOOLEAN
EFIAPI
UiSwitchSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_SWITCH  *This;

  This = (UI_SWITCH *) Control;

  if (StrCmp (Name, L"checkboxvalue") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      This->CheckBoxValue = TRUE;
    }  else {
      This->CheckBoxValue = FALSE;
    }
    CONTROL_CLASS (Control)->Invalidate (Control);
  } else if (StrCmp (Name, L"switchcolor") == 0) {
    if (StrCmp (Value, L"@menucolor") == 0) {
      UpdateCheckBoxColor (This);
    }
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}


LRESULT
EFIAPI
UiSwitchProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_SWITCH    *This;
  UI_CONTROL   *Control;
  UI_MANAGER   *Manager;
  POINT        Point;
  RECT         Rc;
  INT32        Pos;
  INT32        Dx;
  RECT         ThumbRc;



  This = (UI_SWITCH *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;
  Manager = NULL;
  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_SWITCH *) AllocateZeroPool (sizeof (UI_SWITCH));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateSwitchChilds (Control);
    break;

  case UI_NOTIFY_ACTIVATE:
    This->CheckBoxValue = !This->CheckBoxValue;
    This->ThumbPos = This->CheckBoxValue ? 0 : This->ThumbRange;
    Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rc);
    UpdateSwitch (Control, &Rc);
    SendMessage (Manager->MainWnd, UI_NOTIFY_CLICK, (WPARAM)This, 0);
    return TRUE;

  case WM_LBUTTONDOWN:
    POINTSTOPOINT(Point, LParam);
    ThumbRc = This->ThumbRc;
    OffsetRect (&ThumbRc, This->ThumbPos, 0);
    This->MoveThumb = FALSE;
    This->IsHitThumb = FALSE;
    if (PtInRect (&ThumbRc, Point)) {
      This->IsHitThumb = TRUE;
      CONTROL_CLASS(Control)->Invalidate (Control);
    }
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  case WM_NCMOUSEMOVE:
    POINTSTOPOINT(Point, LParam);
    Dx = Point.x - Control->LastX;
    GetClientRect (Hwnd, &Rc);
    if ((This->IsHitThumb && abs(Dx) > 3) || This->MoveThumb) {
      This->MoveThumb = TRUE;
      Pos = MAX (0, MIN (This->ThumbPos + Dx, This->ThumbRange));
      if (Pos != This->ThumbPos) {
        This->ThumbPos = Pos;
        Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rc);
        UpdateSwitch (Control, &Rc);
        Control->LastX = Point.x;
      }
    } else {
      return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_MOUSELEAVE:
    This->IsHitThumb = FALSE;
    This->MoveThumb = FALSE;
    break;

  case WM_LBUTTONUP:
    if (!This->MoveThumb) {
      return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_NCLBUTTONUP:
    This->IsHitThumb = FALSE;
    if (This->MoveThumb) {
      This->MoveThumb = FALSE;
      if (GetCapture () == Hwnd) {
        CONTROL_CLASS_SET_STATE (Control, 0, UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
        Control->Class->Invalidate (Control);
        ReleaseCapture ();
      }
      GetClientRect (Hwnd, &Rc);
      if (This->CheckBoxValue) {
        if (This->ThumbPos >= This->ThumbRange / 2) {
          SendMessage (Hwnd, UI_NOTIFY_ACTIVATE, 0, 0);
        } else {
          This->ThumbPos = 0;
          UpdateSwitch (Control, &Rc);
        }
      } else {
        if (This->ThumbPos < This->ThumbRange / 2) {
          SendMessage (Hwnd, UI_NOTIFY_ACTIVATE, 0, 0);
        } else {
          This->ThumbPos = This->ThumbRange;
          UpdateSwitch (Control, &Rc);
        }
      }
    } else {
      return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    }
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_TEXT || WParam == PAINT_BKCOLOR || WParam == PAINT_STATUSIMAGE) {
      break;
    }
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    break;


  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_SWITCH_CLASS *
EFIAPI
GetSwitchClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"Switch", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiSwitchProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiSwitchSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetPosition  = UiSwitchSetPosition;

  return CURRENT_CLASS;
}


