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

STATIC UI_LABEL_CLASS            *mLabelClass = NULL;
#define CURRENT_CLASS            mLabelClass

SIZE
EFIAPI
UiLabelEstimateSize (
  UI_CONTROL *Control,
  SIZE       AvailableSize
  )
{

  RECT                          TextRc;
  UI_LABEL                      *This;
  SIZE                          Size;
  INT32                         TextWidth;

  This = (UI_LABEL *) Control;

  if (Control->FixedSize.cy == WRAP_CONTENT) {
    if (This->Text[0] == '\0') {
      Size.cx = Control->FixedSize.cx;
      Size.cy = This->FontSize;
      return Size;
    }

    TextWidth = AvailableSize.cx;
    if (Control->FixedSize.cx == MATCH_PARENT) {
      TextWidth -= (Control->Padding.left + Control->Padding.right);
    } else if (Control->FixedSize.cx == WRAP_CONTENT) {
      //
      // BUGBUG: not yet implement
      //
      ASSERT (FALSE);
    } else if (Control->FixedSize.cx == 0) {
      Size.cx = Control->FixedSize.cx;
      Size.cy = This->FontSize;
      return Size;
    } else {
      TextWidth = Control->FixedSize.cx - (Control->Padding.left + Control->Padding.right);
    }
    ASSERT (TextWidth >= 0);
    SetRect (&TextRc, 0, 0, TextWidth, 9999);
    if (Control->Manager->PaintDC) {
      Control->Manager->PaintDC->font->FontSize = This->FontSize;
    }

    UiPaintText (
      Control->Manager->PaintDC,
      &TextRc,
      This->Text,
      This->TextColor,
      INVALID_COLOR,
      This->TextStyle | DT_WORDBREAK | DT_CALCRECT
      );
    Size.cx = Control->FixedSize.cx;
    Size.cy = MAX (TextRc.bottom - TextRc.top + Control->Padding.top  + Control->Padding.bottom, Control->MinSize.cy);
    Size.cy = MAX (Size.cy, 0);
    return Size;
  } else if (1) {

    Size = Control->FixedSize;
    if (Size.cx == MATCH_PARENT) {
      Size.cx = AvailableSize.cx;
    }
    if (Size.cy == MATCH_PARENT) {
      Size.cy = AvailableSize.cy;
    }
    return Size;
  }


  if (Control->FixedSize.cx != 0) {
    AvailableSize.cx = MIN (AvailableSize.cx, Control->FixedSize.cx);
  }

  if (This->FontSize == 0) {
    This->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
  }

  if (This->Text[0] == '\0') {
    Size    = Control->FixedSize;
    Size.cy = This->FontSize;
    return Size;
  }

  SetRect (&TextRc, 0, 0, AvailableSize.cx, 9999);
  if (Control->Manager->PaintDC) {
    Control->Manager->PaintDC->font->FontSize = This->FontSize;
  }

  UiPaintText (
    Control->Manager->PaintDC,
    &TextRc,
    This->Text,
    This->TextColor,
    INVALID_COLOR,
    This->TextStyle | DT_WORDBREAK | DT_CALCRECT
    );

  Size.cx = TextRc.right - TextRc.left + Control->Padding.left + Control->Padding.right;
  Size.cy = TextRc.bottom - TextRc.top + Control->Padding.top  + Control->Padding.bottom;

  if (Control->FixedSize.cy != 0) {
    Size.cy = Control->FixedSize.cy;
  }

  if (Control->FixedSize.cx != 0) {
    Size.cx = Control->FixedSize.cx;
  }

  return Size;
}


BOOLEAN
EFIAPI
UiLabelSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_LABEL                      *This;
  EFI_STATUS                    Status;

  This = (UI_LABEL *) Control;

  if (StrCmp (Name, L"text") == 0) {
    FreePool (This->Text);
    This->Text = StrDuplicate (Value);
  } else if (StrCmp (Name, L"fontsize") == 0) {
    This->FontSize = (INT32)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"textcolor") == 0) {
    This->TextColor = GetColorValue (Value);
  } else if (StrCmp (Name, L"align") == 0) {

    if (StrCmp (Value, L"bottom") == 0) {
      This->TextStyle &= ~(DT_TOP | DT_VCENTER | DT_LEFT | DT_RIGHT | DT_CENTER);
      This->TextStyle |= DT_CENTER | DT_BOTTOM | DT_SINGLELINE;
    } else if (StrCmp (Value, L"center") == 0) {
      This->TextStyle &= ~(DT_LEFT | DT_RIGHT);
      This->TextStyle |= DT_CENTER;
    } else if (StrCmp (Value, L"singleline") == 0) {
      This->TextStyle |= DT_SINGLELINE;
    }
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }
  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}


LRESULT
EFIAPI
UiLabelProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_CONTROL   *Control;
  UI_LABEL     *This;
  UINT32       TextColor;
  RECT   Rect;
  HDC    Hdc;
  UI_MANAGER   *Manager;

  Control = (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);
  if (Control == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  This    = (UI_LABEL *) Control;

  Manager = NULL;
  if (This != NULL) {
    Manager = This->Control.Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_LABEL *) AllocateZeroPool (sizeof (UI_LABEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);

    This->TextColor   = 0;
    This->TextStyle   = DT_VCENTER;
    This->Text        = StrDuplicate (L"");
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_TEXT) {
      Hdc = Manager->PaintDC;
      Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rect);

      TextColor = This->TextColor;

      if (TextColor == 0) {
        TextColor = 0xFF000000;
      }

      Rect.left   += Control->Padding.left;
      Rect.top    += Control->Padding.top;
      Rect.right  -= Control->Padding.right;
      Rect.bottom -= Control->Padding.bottom;

      if (This->FontSize == 0) {
        This->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
      }
      Control->Manager->PaintDC->font->FontSize = This->FontSize;

      UiPaintText (
        Hdc,
        &Rect,
        This->Text,
        TextColor,
        INVALID_COLOR,
        This->TextStyle | DT_WORDBREAK
        );
    } else {
      PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_NCHITTEST:
    return HTTRANSPARENT;

  case WM_DESTROY:
    if (This->Text != NULL) {
      FreePool (This->Text);
    }
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_LABEL_CLASS *
EFIAPI
GetLabelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"Label", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiLabelProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiLabelSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->EstimateSize = UiLabelEstimateSize;


  return CURRENT_CLASS;
}

