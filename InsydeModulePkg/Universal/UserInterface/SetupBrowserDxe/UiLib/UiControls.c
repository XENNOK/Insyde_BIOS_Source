/** @file
  UI Common Controls

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "UiControls.h"
#include "UiRender.h"
#include "UiManager.h"
#include "Ui.h"
#if defined(_MSC_VER)
#pragma warning (disable: 4366)
#endif


#define SCROLLBAR_TIMER_ID 1

VOID
UiApplyAttributeList (
  UI_CONTROL *Control,
  CHAR16     *StrList
  )
{
  CHAR16 Name[256];
  CHAR16 Value[256];
  UINTN  Index;
  CHAR16 QuoteChar;

  while(*StrList != L'\0') {
    Index = 0;

    //
    // skip space or tab
    //
    while ((*StrList == L' ') || (*StrList == L'\t')) {
      StrList++;
    }

    while( *StrList != L'\0' && *StrList != L'=') {
      Name[Index++] = *StrList++;
    }
    Name[Index] = '\0';

    ASSERT(*StrList == L'=');
    if(*StrList++ != L'=') {
      return ;
    }

    ASSERT(*StrList == L'\"' || *StrList == L'\'');
    if (*StrList != L'\"' && *StrList != L'\'') {
      return ;
    }

    QuoteChar = *StrList;
    StrList++;
    Index = 0;
    while(*StrList != L'\0' && *StrList != QuoteChar) {
      if (*StrList == '\\' && *(StrList + 1) == QuoteChar) {
        Value[Index++] = *StrList++;
      }
      Value[Index++] = *StrList++;
    }
    Value[Index] = '\0';

    ASSERT(*StrList == L'\"' || *StrList == L'\'');
    if (*StrList != L'\"' && *StrList != L'\'') {
      return ;
    }

    Control->SetAttribute (Control, Name, Value);

    StrList++;
    if(*StrList != L' ' && *StrList != '\t') {
      return ;
    }
    StrList++;
  }
}



BOOLEAN
UiSetAttribute (
  VOID       *This,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  return (((UI_CONTROL *)This)->SetAttribute ((UI_CONTROL *)This, Name, Value));
}

UI_CONTROL *
GetUiControl (
  HWND Hwnd
  )
{

  return (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);
}

UI_CONTROL *
CreateControl (
  CHAR16     *ControlName,
  CHAR16     *Text,
  UI_CONTROL *Parent,
  UI_MANAGER *Manager
  )
{

  HWND Wnd;

  ASSERT (Manager != NULL);

  Wnd = CreateWindowEx (
          WS_EX_NOACTIVATE,
          ControlName,
          Text,
          WS_CHILD | WS_VISIBLE,
          0, 0, 0, 0,
          (Parent != NULL) ? Parent->Wnd : NULL,
          NULL,
          Manager->Instance,
          Manager
          );

  ASSERT (Wnd != NULL);

  if (Wnd == NULL) {
    return NULL;
  }

  return GetUiControl (Wnd);
}

COLORREF
GetColorValue (
  CHAR16 *Value
  )
{
  EFI_STATUS Status;
  COLORREF   Color;

  while (*Value > L'\0' && *Value <= L' ') Value++;
  if (*Value == '#') Value++;

  Color = (COLORREF) StrToUInt (Value, 16, &Status);

  return Color;
}

VOID
UiControlSetPos (
  UI_CONTROL *Control,
  RECT       Rc
  )
{
  Control->FixedXY.cx = Rc.left >= 0 ? Rc.left : Rc.right;
  Control->FixedXY.cy = Rc.top  >= 0 ? Rc.top  : Rc.bottom;

  Control->FixedSize.cx = Rc.right - Rc.left;
  Control->FixedSize.cy = Rc.bottom - Rc.top;

  Control->Manager->NeedUpdatePos = TRUE;
  Control->Invalidate (Control);
}


BOOLEAN
UiControlSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  EFI_STATUS Status;
  CHAR16     *PtrValue;

  if (StrCmp (Name, L"name") == 0) {
    ASSERT (Control->Name);
    if (StrCmp(Control->Name, L"") != 0) {
      StringMapRemove (&Control->Manager->NameList, Value);
      gBS->FreePool (Control->Name);
    }
    Control->Name = AllocateCopyPool (StrSize(Value), Value);
    StringMapAdd (&Control->Manager->NameList, Value, Control);
  } else if (StrCmp (Name, L"fontsize") == 0) {
    Control->FontSize = (INT32)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"text") == 0) {
    SendMessage (Control->Wnd, WM_SETTEXT, 0, (LPARAM)Value);
  } else if (StrCmp (Name, L"width") == 0) {
      Control->FixedSize.cx  = (INT32)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"height") == 0) {
    Control->FixedSize.cy  = (INT32)StrToUInt (Value, 10, &Status);

  } else if (StrCmp (Name, L"bkimage") == 0) {
    Control->BkImage     = GetImageIdByName (Value);
  } else if (StrCmp (Name, L"bkcolor") == 0) {
    Control->BkColor     = GetColorValue (Value);
  } else if (StrCmp (Name, L"bkcolor2") == 0) {
    Control->BkColor2    = GetColorValue (Value);
  } else if (StrCmp (Name, L"bkcolor3") == 0) {
    Control->BkColor3    = GetColorValue (Value);
  } else if (StrCmp (Name, L"bordercolor") == 0) {
    Control->BorderColor = GetColorValue (Value);
  } else if (StrCmp (Name, L"visible") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      ShowWindow (Control->Wnd, SW_SHOW);
    } else {
      ShowWindow (Control->Wnd, SW_HIDE);
    }
  } else if (StrCmp (Name, L"padding") == 0) {
    Control->Padding.left   = wcstol (Value, &PtrValue, 10);
    Control->Padding.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    Control->Padding.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    Control->Padding.bottom = wcstol (PtrValue + 1, &PtrValue, 10);

    Control->Manager->NeedUpdatePos = TRUE;
  } else if (StrCmp (Name, L"borderround") == 0) {
    Control->BoardRound.cx = wcstol (Value, &PtrValue, 10);
    Control->BoardRound.cy = wcstol (PtrValue + 1, &PtrValue, 10);
  } else if (StrCmp (Name, L"pos") == 0) {
    RECT Rc;

    Rc.left   = wcstol (Value, &PtrValue, 10);
    Rc.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    Rc.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    Rc.bottom = wcstol (PtrValue + 1, &PtrValue, 10);
    UiControlSetPos (Control, Rc);
  } else if (StrCmp (Name, L"scale9grid") == 0) {
    Control->Scale9Grid.left   = wcstol (Value, &PtrValue, 10);
    Control->Scale9Grid.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    Control->Scale9Grid.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    Control->Scale9Grid.bottom = wcstol (PtrValue + 1, &PtrValue, 10);
  } else if (StrCmp (Name, L"float") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      Control->Float = TRUE;
    } else {
      Control->Float = FALSE;
    }
  } else if (StrCmp (Name, L"gradient_vertical") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      Control->GradientVertial = TRUE;
    } else {
      Control->GradientVertial = FALSE;
    }
  } else if (StrCmp (Name, L"user_data") == 0) {
    if (Control->UserData != NULL) {
      gBS->FreePool (Control->UserData);
    }
    Control->UserData = AllocateCopyPool (StrSize (Value), Value);
  } else if (StrCmp (Name, L"bkimagestyle") == 0) {
    if (StrCmp (Value, L"center") == 0) {
      Control->BkImageStyle |= DT_CENTER | DT_VCENTER;
    } else if (StrCmp (Value, L"") == 0) {
      Control->BkImageStyle = 0;
    }
  } else {
    ASSERT (0);
    return FALSE;
  }

  Control->Invalidate (Control);

  return TRUE;
}

BOOLEAN
UiControlActivate (
  UI_CONTROL *This
  )
{

  UINT32 Style;

  Style = (UINT32) GetWindowLongPtr (This->Wnd, GWL_STYLE);
  if ((Style & WS_DISABLED) != 0) {
    return FALSE;
  }

  if ((Style & WS_VISIBLE) == 0) {
    return FALSE;
  }

  return TRUE;
}


VOID
UiControlDestroy (
  UI_CONTROL *This
  )
{

  /*
  if (This->Wnd != This->Manager->MainWnd &&
      This->Wnd != This->Manager->Root) {
    Parent = ((UI_CONTAINER *)GetUiControl(GetParent (This->Wnd)));
    Parent->Remove (Parent, This);
  } */

  ASSERT (This);
  ASSERT (This->Name);

  if (StrCmp (This->Name, L"") != 0) {
    StringMapRemove (&This->Manager->NameList, This->Name);
    gBS->FreePool (This->Name);
  }

  if (This->UserData != NULL) {
    gBS->FreePool (This->UserData);
  }

  gBS->FreePool (This);
}

SIZE
UiControlEstimateSize (
  UI_CONTROL *Control,
  SIZE       AvailableSize
  )
{

  return Control->FixedSize;
}

VOID
UiControlInvalidate (
  UI_CONTROL *Control
  )
{
  RECT Rc;
  RECT InvalidateRc;
  RECT TmpRc;
  RECT ParentRc;
  HWND Parent;

  if (Control->StateFlags & UISTATE_INVALIDATED) {
    //
    // check update region already in invalidate region
    //
    if (GetUpdateRect (Control->Manager->MainWnd, &InvalidateRc, FALSE)) {
      GetWindowRect (Control->Wnd, &Rc);
      MapWindowPoints(NULL, Control->Manager->MainWnd, (POINT *)&Rc, 2);
      if ((InvalidateRc.left <= Rc.left) && (InvalidateRc.right >= Rc.right) &&
          (InvalidateRc.top <= Rc.top)   && (InvalidateRc.bottom >= Rc.bottom)){
        return ;

      }
    }
  }

  Control->StateFlags |= UISTATE_INVALIDATED;
  GetWindowRect (Control->Wnd, &Rc);

  Parent = Control->Wnd;
  CopyRect (&InvalidateRc, &Rc);
  while ((Parent = Parent->parent) != NULL) {
    CopyRect (&TmpRc, &InvalidateRc);
    GetWindowRect (Parent, &ParentRc);

    if (!IntersectRect (&InvalidateRc, &TmpRc, &ParentRc)) {
      return ;
    }

    if (Parent == Control->Manager->Root) {
      break;
    }
  }

  MapWindowPoints(NULL, Control->Manager->MainWnd, (POINT *)&InvalidateRc, 2);
  InvalidateRect (Control->Manager->MainWnd, &InvalidateRc, FALSE);
}


UI_CONTROL*
UiControlFindNextFocus (
  UI_CONTROL *Control,
  UI_CONTROL *Focused,
  UINTN      Direction
  )
{
  UI_CONTAINER *Root;
  UINTN        Index;
  UI_CONTROL   *Item;
  RECT         ItemRc;
  RECT         FocusRc;
  UI_CONTROL   *NextFocus;
  UINT32       Distance;
  UINT32       BestDistance;


  ASSERT (Direction == VK_LEFT || Direction == VK_RIGHT || Direction == VK_UP || Direction == VK_DOWN);

  if (!(GetWindowLongPtr (Control->Wnd, GWL_STYLE) & WS_CHILD)) {
    return NULL;
  }

  Root = (UI_CONTAINER *) GetUiControl (GetParent (Control->Wnd));
  if (Root == NULL) {
    return NULL;
  }

  GetWindowRect (Focused->Wnd, &FocusRc);

  //
  // search next focus
  //
  NextFocus = NULL;
  BestDistance = (UINT32)-1;
  for (Index = 0; Index < Root->ItemCount; Index++) {
    Item = Root->Items[Index];
    if (Item == Focused) {
      continue;
    }
    if (GetWindowLongPtr (Item->Wnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
      continue;
    }
    GetWindowRect (Item->Wnd, &ItemRc);

    if (Direction == VK_LEFT) {
      if (ItemRc.left >= FocusRc.left) {
        continue;
      }
    } else if (Direction == VK_UP) {
      if (ItemRc.top >= FocusRc.top) {
        continue;
      }
    } else if (Direction == VK_RIGHT) {
      if (ItemRc.left <= FocusRc.left) {
        continue;
      }
    } else if (Direction == VK_DOWN) {
      if (ItemRc.top <= FocusRc.top) {
        continue;
      }
    }
    Distance = (FocusRc.left - ItemRc.left) * (FocusRc.left - ItemRc.left) +
               (FocusRc.top - ItemRc.top) * (FocusRc.top - ItemRc.top);
    if (Distance < BestDistance) {
      NextFocus    = Item;
      BestDistance = Distance;
    }
  }

  return NextFocus;

}

UI_CONTROL*
UiControlGetParent (
  UI_CONTROL *Control
  )
{
  HWND       Hwnd;
  UI_CONTROL *Parent;

  Hwnd = GetParent (Control->Wnd);
  ASSERT (Hwnd != NULL);

  Parent = GetUiControl (Hwnd);
  ASSERT (Parent != NULL);

  return Parent;
}

BOOLEAN
UiControlShouldDelayChildPressedState (
  UI_CONTROL *Control
  )
{
  return FALSE;
}

VOID
UiControlCreate (
  UI_CONTROL *Control,
  HWND       Hwnd,
  UI_MANAGER *Manager
  )
{
  Control->SetAttribute   = UiControlSetAttribute;
  Control->EstimateSize   = UiControlEstimateSize;
  Control->Invalidate     = UiControlInvalidate;
  Control->FindNextFocus  = UiControlFindNextFocus;
  Control->GetParent                    = UiControlGetParent;
  Control->ShouldDelayChildPressedState = UiControlShouldDelayChildPressedState;

  Control->Wnd             = Hwnd;
  if (Manager != NULL) {
    Control->Manager       = Manager;
  }
  Control->UserData        = NULL;
  Control->Name            = L"";
  Control->BkColor         = 0;
  Control->BorderColor     = 0;
  Control->FixedSize.cx    = 0;
  Control->FixedSize.cy    = 0;
  Control->Float           = FALSE;
  Control->BoardRound.cx   = 0;
  Control->BoardRound.cy   = 0;
  Control->GradientVertial = TRUE;
  SetRectEmpty (&Control->Scale9Grid);

  //
  // no focus
  //
  SetWindowLongPtr (
    Control->Wnd,
    GWL_EXSTYLE,
    GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE
    );
}


BOOLEAN
UiControlIsInScrollContainer (
  UI_CONTROL *This
  )
{
  UI_CONTROL *Parent;

  Parent = This->GetParent (This);

  while (Parent != NULL) {
    if (Parent->ShouldDelayChildPressedState (Parent)) {
      return TRUE;
    }
    Parent = Parent->GetParent (Parent);
  }

  return FALSE;
}


LRESULT
UiControlProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_CONTROL   *Control;
  UI_CONTROL   *ParentControl;
  HWND         ParentWnd;

  HDC          Hdc;
  RECT         Rc;
  UI_MANAGER   *Manager;
  COLORREF     BorderColor;
  UI_CONTROL   *NextFocus;
  POINT        Point;
  POINT        LastPoint;

  Control = NULL;
  Manager = NULL;

  if (Msg != WM_CREATE) {
    Control = (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);
  }

  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    if (Control == NULL) {
      Control     = (UI_CONTROL *) AllocateZeroPool (sizeof (UI_CONTROL));
    }
    SetWindowLongPtr (Hwnd, 0, (INTN)Control);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    if (LParam == 0) {
      Manager = NULL;
    } else {
      Manager = (UI_MANAGER *)(((CREATESTRUCT *)(LParam))->lpCreateParams);
    }
    UiControlCreate (Control, Hwnd, Manager);
    break;

  case UI_NOTIFY_ACTIVATE:
    return UiControlActivate (Control);

  case WM_DESTROY:
    UiControlDestroy (Control);
    break;

  case WM_WINDOWPOSCHANGED:
    SendMessage (Hwnd, UI_NOTIFY_SET_POSITION, WParam, LParam);
    break;

  case UI_NOTIFY_SET_POSITION:
    Control->Invalidate (Control);
    break;

  case WM_PAINT:
//    ValidateRect (Hwnd, NULL);
    break;

  case UI_NOTIFY_PAINT:
    Hdc = Control->Manager->PaintDc;
    Manager->GetControlRect (Manager, Control, &Rc);

    switch (WParam) {

    case PAINT_ALL:
      Control->StateFlags &= ~UISTATE_INVALIDATED;
      if (Control->BoardRound.cx != 0 || Control->BoardRound.cx != 0) {

        HRGN OldRgn, NewRgn;
        RECT ClipRc;

        Manager->GetControlRect (Manager, Control, &Rc);
        GetClipBox (Manager->PaintDc, &ClipRc);
        OldRgn = CreateRectRgnIndirect (&ClipRc);
        NewRgn = CreateRoundRectRgn (Rc.left, Rc.top, Rc.right, Rc.bottom, Control->BoardRound.cx, Control->BoardRound.cy);
        ExtSelectClipRgn (Manager->PaintDc, NewRgn, RGN_AND);

        SendMessage (Hwnd, UI_NOTIFY_PAINT,     PAINT_BKCOLOR, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT,     PAINT_BKIMAGE, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT, PAINT_STATUSIMAGE, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT,  PAINT_TEXT, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT,      PAINT_BORDER, LParam);

        SelectClipRgn(Manager->PaintDc, OldRgn);
        DeleteObject(OldRgn);
        DeleteObject(NewRgn);

      } else {
        SendMessage (Hwnd, UI_NOTIFY_PAINT,     PAINT_BKCOLOR, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT,     PAINT_BKIMAGE, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT, PAINT_STATUSIMAGE, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT,  PAINT_TEXT, LParam);
        SendMessage (Hwnd, UI_NOTIFY_PAINT,      PAINT_BORDER, LParam);
      }
      break;

    case PAINT_BKIMAGE:
      if (Control->BkImage != 0) {

//      Rc.left   += Control->Padding.left;
//      Rc.top    += Control->Padding.top;
//      Rc.right  -= Control->Padding.right;
//      Rc.bottom -= Control->Padding.bottom;

        UiShowBitmap (
          Control->Wnd,
          Manager->PaintDc,
          &Rc,
          &Control->Scale9Grid,
          Control->BkImage,
          Control->BkImageStyle
          );
      }
      break;

    case PAINT_BKCOLOR:
      if (Control->BkColor != 0) {
        if (Control->BkColor2 != 0) {
          UiPaintGradient (Hdc, &Rc, Control->BkColor, Control->BkColor2, Control->GradientVertial, 64);
        } else {
          HDC      hdcMem;
          HBITMAP  hbmp, hbmpOrg;
          RECT     rc;
          COLORREF Color;

          Color = Control->BkColor;

          GetClientRect (Control->Wnd, &rc);
          /* redirect painting to offscreen dc*/
          hdcMem  = CreateCompatibleDC(Hdc);
          hbmp    = CreateCompatibleBitmap(hdcMem, rc.right, rc.bottom);
          hbmpOrg = SelectObject(hdcMem, hbmp);

          UiFastFillRect (hdcMem, &rc, Color);

          /* alpha blend blit offscreen map with physical screen*/
          BitBlt(Hdc, Rc.left, Rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, MWROP_SRC_OVER);  /* FIXME specify constant alpha somewhere!*/
          DeleteObject(SelectObject(hdcMem, hbmpOrg));
          DeleteDC(hdcMem);
        }
      }
      break;

    case PAINT_STATUSIMAGE:
      break;

    case PAINT_TEXT:
      break;

    case PAINT_BORDER:
      BorderColor = Control->BorderColor;
      if (Control->StateFlags & UISTATE_FOCUSED) {
        BorderColor = 0xFFFF0000;
      }
      if (BorderColor != 0) {
        SelectObject (Hdc, GetStockObject (PS_NULL));
        SelectObject (Hdc, GetStockObject (DC_PEN));
        SetDCPenColor (Hdc, BorderColor);
        Rectangle (Hdc, Rc.left, Rc.top, Rc.right, Rc.bottom);
      }
      if (Control->StateFlags & UISTATE_FOCUSED) {
        Rectangle (Hdc, Rc.left + 1, Rc.top + 1, Rc.right - 1, Rc.bottom - 1);
      }
      break;

    default:
      ASSERT (FALSE);
      break;

    }
    break;

  case WM_KEYDOWN:
    if (WParam == VK_RETURN) {
      if (!IsWindowVisible (Hwnd)) {
        break;
      }

      if (!(Control->StyleFlags & UISTYLE_CLICKABLE)) {
        break;
      }
      Control->StateFlags |= UISTATE_PRESSED;
      Control->Invalidate (Control);
    }

  //
  // we didn't send WM_KEYUP, so continue process WM_KEYUP
  //
  //
  case WM_KEYUP:
    if (WParam == VK_LEFT || WParam == VK_RIGHT
        || WParam == VK_UP || WParam == VK_DOWN) {

      NextFocus = Control->FindNextFocus (Control, Control, WParam);
      if (NextFocus != NULL) {
        SetFocus (NextFocus->Wnd);
      }
      break;
    }


    if (WParam == VK_RETURN) {
      if (!IsWindowVisible (Hwnd)) {
        break;
      }

      if (!(Control->StyleFlags & UISTYLE_CLICKABLE)) {
        break;
      }

      if (!(Control->StateFlags & UISTATE_PRESSED)) {
        break;
      }

      Control->StateFlags &= ~UISTATE_PRESSED;
      Control->Invalidate (Control);
      SendMessage (Hwnd, UI_NOTIFY_ACTIVATE, 0, 0);
    }
    break;

  case WM_LBUTTONDOWN:
    POINTSTOPOINT(Point, LParam);
    ClientToScreen (Hwnd, &Point);
    if ((GetWindowLongPtr (Hwnd, GWL_STYLE) & WS_DISABLED) == 0) {
      SetCapture (Hwnd);
      Control->StateFlags |= UISTATE_PRESSED | UISTATE_CAPTURED;
      Control->Invalidate (Control);
    }

    if (UiControlIsInScrollContainer (Control)) {
      Control->StateFlags |= UISTATE_PREPRESSED;
      Control->Invalidate (Control);
      Control->LastX = Point.x;
      Control->LastY = Point.y;
    }
    break;

  case WM_NCMOUSEMOVE:
    if (Control->StateFlags & UISTATE_PREPRESSED) {
      POINTSTOPOINT(Point, LParam);
      GetWindowRect (Hwnd, &Rc);
      if (!PtInRect (&Rc, Point) || abs(Control->LastY - Point.y) > 10) {
        //
        // change capture to parent
        //
        Control->StateFlags &= ~(UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
        Control->Invalidate (Control);
        ParentControl = Control->GetParent (Control);
        ASSERT (ParentControl != NULL);

        ParentWnd = ParentControl->Wnd;
        SetCapture (ParentWnd);
        LastPoint.x = Control->LastX;
        LastPoint.y = Control->LastY;
        ScreenToClient (ParentWnd, &LastPoint);
        SendMessage (ParentWnd, WM_LBUTTONDOWN, 0, MAKELONG(LastPoint.x, LastPoint.y));
        SendMessage (ParentWnd, WM_NCMOUSEMOVE, 0, MAKELONG(Point.x, Point.y));
      }
    }
    break;

  case WM_LBUTTONUP:
  case WM_NCLBUTTONUP:
    if (GetCapture () == Hwnd) {
      Control->StateFlags &= ~(UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
      Control->Invalidate (Control);
      ReleaseCapture ();
    }
    break;


  case WM_SETFOCUS:
    Control->StateFlags |= UISTATE_FOCUSED;
    Control->Invalidate (Control);
    break;

  case WM_KILLFOCUS:
    Control->StateFlags &= (~UISTATE_FOCUSED);
    Control->Invalidate (Control);
    break;

  case WM_ERASEBKGND:
    return 1;

  default:
    return DefWindowProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;

}



SIZE
UiLabelEstimateSize (
  UI_CONTROL *Control,
  SIZE       AvailableSize
  )
{

  RECT        TextRc;
  UI_LABEL    *This;
  SIZE        Size;

  This = (UI_LABEL *) Control;

  if (Control->FixedSize.cx != 0) {
   AvailableSize.cx = MIN (AvailableSize.cx, Control->FixedSize.cx );
  }

  if (Control->FontSize == 0) {
    Control->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
  }

  if (Control->Wnd->szTitle[0] == '\0') {
    Size    = Control->FixedSize;
    Size.cy = Control->FontSize;
    return Size;
  }

  SetRect (&TextRc, 0, 0, AvailableSize.cx, 9999);
  TextRc.left  += This->TextPadding.left;
  TextRc.right -= This->TextPadding.right;
  if (Control->FontSize == 0) {
    Control->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
  }
  if (Control->Manager->PaintDc) {
    Control->Manager->PaintDc->font->FontSize = Control->FontSize;
  }



  UiPaintText (
    Control->Manager->PaintDc,
    &TextRc,
    Control->Wnd->szTitle,
    This->TextColor,
    INVALID_COLOR,
    This->TextStyle | DT_WORDBREAK | DT_CALCRECT
    );

  Size.cx = TextRc.right - TextRc.left + This->TextPadding.left + This->TextPadding.right;
  Size.cy = TextRc.bottom - TextRc.top + This->TextPadding.top + This->TextPadding.bottom;

  if (Control->FixedSize.cy != 0) {
    Size.cy = Control->FixedSize.cy;
  }

  if (Control->FixedSize.cx != 0) {
    Size.cx = Control->FixedSize.cx;
  }

  return Size;
}


BOOLEAN
UiLabelSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_LABEL   *This;
  CHAR16     *PtrValue;

  This = (UI_LABEL *)Control;

  if (StrCmp (Name, L"textcolor") == 0) {
    This->TextColor = GetColorValue (Value);
  } else if (StrCmp (Name, L"align") == 0) {

    if (StrCmp (Value, L"bottom") == 0) {
      This->TextStyle &= ~(DT_TOP | DT_VCENTER | DT_LEFT | DT_RIGHT | DT_CENTER);
      This->TextStyle |= DT_BOTTOM | DT_SINGLELINE;
    }

    if (StrCmp (Value, L"center") == 0) {
      This->TextStyle &= ~(DT_LEFT | DT_RIGHT);
      This->TextStyle |= DT_CENTER | DT_SINGLELINE;
    }

  } else if (StrCmp (Name, L"textpadding") == 0) {
    This->TextPadding.left   = wcstol (Value, &PtrValue, 10);
    This->TextPadding.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    This->TextPadding.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    This->TextPadding.bottom = wcstol (PtrValue + 1, &PtrValue, 10);

    Control->Invalidate (Control);
  } else {
    return UiControlSetAttribute (Control, Name, Value);
  }

  return TRUE;
}


LRESULT
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
  This    = (UI_LABEL *) Control;
  Manager = NULL;

  if (This != NULL) {
    Manager = This->Control.Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_LABEL *) AllocateZeroPool (sizeof (UI_LABEL));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiControlProc (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    Control->SetAttribute = UiLabelSetAttribute;
    Control->EstimateSize = UiLabelEstimateSize;
    This->TextColor   = 0;
    This->TextStyle   = DT_VCENTER;
    SetRectEmpty (&This->TextPadding);
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_TEXT) {
      Hdc = Manager->PaintDc;
      Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rect);

      TextColor = This->TextColor;

      if (TextColor == 0) {
        TextColor = 0xFF000000;
      }

      Rect.left   += This->TextPadding.left;
      Rect.top    += This->TextPadding.top;
      Rect.right  -= This->TextPadding.right;
      Rect.bottom -= This->TextPadding.bottom;

      if (Control->FontSize == 0) {
        Control->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
      }
      Control->Manager->PaintDc->font->FontSize = Control->FontSize;

      UiPaintText (
        Hdc,
        &Rect,
        Hwnd->szTitle,
        TextColor,
        INVALID_COLOR,
        This->TextStyle | DT_WORDBREAK
        );

    } else {
      UiControlProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  default:
    return UiControlProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

BOOLEAN
UiButtonSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_BUTTON  *This;

  This = (UI_BUTTON *) Control;

  if (StrCmp (Name, L"normalimage") == 0) {
    This->NormalImage = GetImageIdByName (Value);
  } else if (StrCmp (Name, L"hotimage") == 0) {
    This->HotImage    = GetImageIdByName (Value);
  } else if (StrCmp (Name, L"pushimage") == 0) {
    This->PushImage   = GetImageIdByName (Value);
  } else {
    return UiLabelSetAttribute (Control, Name, Value);
  }

  return TRUE;
}

VOID
UiButtonPaintStatusImage (
  UI_BUTTON *This,
  HDC       Hdc
  )
{

  RECT   Rc;
  COLORREF     Color1;
  COLORREF     Color2;
  COLORREF     Color3;
  UI_CONTROL   *Control;
  UI_MANAGER   *Manager;

  Control = (UI_CONTROL *)This;
  Manager = Control->Manager;

  Color1  = Control->BkColor;
  Color2  = Control->BkColor2;
  Color3  = Control->BkColor3;

  if (Color3 == 0) {
    return ;
  }

  Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rc);

  SetBkMode(Hdc, TRANSPARENT);


  if (Control->StateFlags & UISTATE_PRESSED) {

    if (This->PushImage != 0) {
      UiShowBitmap (Control->Wnd, Manager->PaintDc, &Rc, &Control->Scale9Grid, This->PushImage, 0);
    } else {
      UiPaintGradient (Hdc, &Rc, Color2, Color1, Control->GradientVertial, 32);
    }
  } else if (Control->StateFlags & UISTATE_HOT) {
    if (This->HotImage != 0) {
      UiShowBitmap (Control->Wnd, Manager->PaintDc, &Rc, &Control->Scale9Grid, This->HotImage, 0);
    } else {
      UiPaintGradient (Hdc, &Rc, Color1, Color2, Control->GradientVertial, 32);
    }
  } else {
    if (This->NormalImage != 0) {
      UiShowBitmap (Control->Wnd, Manager->PaintDc, &Rc, &Control->Scale9Grid, This->NormalImage, 0);
    } else {
      UiPaintGradient (Hdc, &Rc, Color1, Color2, Control->GradientVertial, 32);
    }
  }

}

SIZE
UiButtonEstimateSize (
  UI_CONTROL *Control,
  SIZE       AvailableSize
  )
{

  SIZE Size;

  if (Control->FixedSize.cx == 0 || Control->FixedSize.cy == 0) {
    Size = UiLabelEstimateSize (Control, AvailableSize);
  } else {
    Size = Control->FixedSize;
  }

  return Size;
}


LRESULT
UiButtonProc (
  HWND   Hwnd,
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

  This = (UI_BUTTON *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *)This;
  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_BUTTON *) AllocateZeroPool (sizeof (UI_BUTTON));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiLabelProc (Hwnd, Msg, WParam, LParam);
    Control->SetAttribute = UiButtonSetAttribute;
    Control->EstimateSize = UiButtonEstimateSize;
    ((UI_LABEL *)This)->TextStyle = DT_VCENTER | DT_CENTER;
    This->Padding.cx    = 0;
    This->Padding.cy    = 0;
    Control->BoardRound.cx = 0;
    Control->BoardRound.cy = 0;
    Control->BkColor  = RGB (215,215,227);
    Control->BkColor2 = RGB (250,250,252);
    Control->BkColor3 = RGB (150,150,150);
    Control->BkImageStyle = DT_CENTER | DT_VCENTER;
    Control->StyleFlags |= UISTYLE_CLICKABLE;
    SetWindowLongPtr (
      Control->Wnd,
      GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE
      );
    break;

  case WM_MOUSEENTER:
    Control->StateFlags |= UISTATE_HOT;
    Control->Invalidate (Control);
    break;

  case WM_MOUSELEAVE:
    Control->StateFlags &= ~UISTATE_HOT;
    Control->Invalidate (Control);
    break;

  case WM_NCLBUTTONUP:
    if(GetCapture() == Hwnd && ((Control->StateFlags & UISTATE_CAPTURED) != 0)) {
      GetWindowRect (Hwnd, &Rc);
      Point.x = LOWORD (LParam);
      Point.y = HIWORD (LParam);
      if (!PtInRect (&Rc, Point)) {
        ReleaseCapture();
        Control->StateFlags &= ~(UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
        Control->Invalidate (Control);
      }
    }
    break;

  case WM_LBUTTONUP:
    if (GetCapture () == Hwnd) {
      ReleaseCapture ();
    }

    if ((Control->StateFlags & UISTATE_CAPTURED) == 0) {
      break;
    }

    Control->StateFlags &= ~(UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);

    GetClientRect (Hwnd, &Rc);


    Point.x = LOWORD (LParam);
    Point.y = HIWORD (LParam);
    Control->Invalidate (Control);

    if (PtInRect (&Rc, Point)) {
      SendMessage (Hwnd, UI_NOTIFY_ACTIVATE, 0, 0);
    }
    break;


  case UI_NOTIFY_ACTIVATE:
    SendMessage (Manager->MainWnd, UI_NOTIFY_CLICK, (WPARAM)This, 0);
    return TRUE;

  case WM_SETTEXT:
    DefWindowProc (Hwnd, Msg, WParam, LParam);
    Control->Invalidate (Control);
    break;


  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_STATUSIMAGE) {
      UiButtonPaintStatusImage (This, Manager->PaintDc);
    } else  {
      UiLabelProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  default:
    return UiLabelProc (Hwnd, Msg, WParam, LParam);

  }
  return 0;

}

INT32
UiSwitchGetThumbScrollRange (
  UI_SWITCH *This
  )
{
  UI_CONTROL *Control;
  RECT       Rc;

  Control = (UI_CONTROL *) This;
  GetClientRect (Control->Wnd, &Rc);

  return (Rc.right - Rc.left) * 1 / 3;
}

BOOLEAN
UiSwitchSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_SWITCH  *This;
  INT32      ThumbScrollRange;

  This = (UI_SWITCH *) Control;

  if (StrCmp (Name, L"checkboxvalue") == 0) {
    ThumbScrollRange = UiSwitchGetThumbScrollRange (This);
    if (StrCmp (Value, L"true") == 0) {
      This->CheckBoxValue = TRUE;
      This->ThumbPos = ThumbScrollRange;

    }  else {
      This->CheckBoxValue = FALSE;
      This->ThumbPos = 0;
    }
    Control->Invalidate (Control);
  } else {
    return UiButtonSetAttribute (Control, Name, Value);
  }

  return TRUE;
}


VOID
UiSwitchPaintStatusImage (
  UI_SWITCH *This,
  HDC       Hdc
  )
{

  RECT   Rc;
  COLORREF     Color1;
  COLORREF     Color2;
  COLORREF     Color3;
  UI_CONTROL   *Control;
  UI_MANAGER   *Manager;
  UI_LABEL     *Label;
  EFI_STRING   Str;
  RECT         ThumbRc;
  BOOLEAN      IsOnStatus;

  Control = (UI_CONTROL *)This;
  Manager = Control->Manager;

  Color1  = Control->BkColor;
  Color2  = Control->BkColor2;
  Color3  = Control->BkColor3;


  if (Color3 == 0) {
    return ;
  }

  Manager->GetControlRect (Manager, (UI_CONTROL *)This, &Rc);

  SetBkMode(Hdc, TRANSPARENT);

  Color1 = RGB (0x7B,0x7B,0x7B);
  Color2 = RGB (0x4B,0x4B,0x4B);
  //
  // background
  //
  UiPaintGradient (Hdc, &Rc, Color1, Color2, TRUE, 16);

  if (This->CheckBoxValue || (Control->StateFlags & UISTATE_PRESSED)) {
    Color1 = 0xFFB97237;
    Color2 = 0xFFFBB680;
  } else {
    Color1 = 0xFF808080;
    Color2 = 0xFFC0C0C0;
  }

  ThumbRc = This->ThumbRc;
  OffsetRect (&ThumbRc, Rc.left, Rc.top);
  OffsetRect (&ThumbRc, This->ThumbPos, 0);

  // paint thumb
  UiPaintGradient (Hdc, &ThumbRc, Color1, Color2, TRUE, 16);

  //
  // paint text
  //
  Label = (UI_LABEL *)This;
  Label->TextStyle = DT_CENTER | DT_SINGLELINE;

  if (This->ThumbPos < UiSwitchGetThumbScrollRange (This)/ 2) {
    IsOnStatus = FALSE;
  } else {
    IsOnStatus = TRUE;
  }

  if (IsOnStatus) {
    Str = GetToken (STRING_TOKEN (ON_STRING), gHiiHandle);
  } else {
    Str = GetToken (STRING_TOKEN (OFF_STRING), gHiiHandle);
  }

  if (Control->FontSize == 0) {
    Control->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
  }
  Control->Manager->PaintDc->font->FontSize = Control->FontSize;

  UiPaintText (
    Hdc,
    &ThumbRc,
    Str,
    Label->TextColor,
    INVALID_COLOR,
    Label->TextStyle
    );

  gBS->FreePool (Str);
}


LRESULT
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
  INT32        ThumbScrollRange;
  RECT         ThumbRc;

  Manager = NULL;

  This = (UI_SWITCH *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *)This;
  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_SWITCH *) AllocateZeroPool (sizeof (UI_SWITCH));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiButtonProc (Hwnd, Msg, WParam, LParam);
    Control->SetAttribute = UiSwitchSetAttribute;
    break;

  case UI_NOTIFY_SET_POSITION:

    ThumbScrollRange    = UiSwitchGetThumbScrollRange (This);
    GetClientRect (Hwnd, &This->ThumbRc);

    This->ThumbRc.right -= ThumbScrollRange;

    if (This->CheckBoxValue) {
      This->ThumbPos = ThumbScrollRange;
    } else {
      This->ThumbPos = 0;
    }
    break;

  case UI_NOTIFY_ACTIVATE:
    This->CheckBoxValue = !This->CheckBoxValue;
    Control->Invalidate (Control);
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
      Control->Invalidate (Control);
    }
    return UiButtonProc (Hwnd, Msg, WParam, LParam);

  case WM_NCMOUSEMOVE:
    POINTSTOPOINT(Point, LParam);
    Dx = Point.x - Control->LastX;
    GetClientRect (Hwnd, &Rc);
    ThumbScrollRange = UiSwitchGetThumbScrollRange (This);
    if ((This->IsHitThumb && abs(Dx) > 3) || This->MoveThumb) {
      This->MoveThumb = TRUE;
      Pos = MAX (0, MIN (This->ThumbPos + Dx, ThumbScrollRange));
      if (Pos != This->ThumbPos) {
        This->ThumbPos = Pos;
        Control->LastX = Point.x;
        Control->Invalidate (Control);
      }
    } else {
      return UiButtonProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_MOUSELEAVE:
    This->IsHitThumb = FALSE;
    This->MoveThumb = FALSE;
    break;

  case WM_LBUTTONUP:
    if (!This->MoveThumb) {
      return UiButtonProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_NCLBUTTONUP:
    This->IsHitThumb = FALSE;
    if (This->MoveThumb) {
      This->MoveThumb = FALSE;
      if (GetCapture () == Hwnd) {
        Control->StateFlags &= ~(UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
        Control->Invalidate (Control);
        ReleaseCapture ();
      }
      GetClientRect (Hwnd, &Rc);
      ThumbScrollRange = UiSwitchGetThumbScrollRange (This);
      if (This->CheckBoxValue) {
        if (This->ThumbPos < ThumbScrollRange / 2) {
          SendMessage (Hwnd, UI_NOTIFY_ACTIVATE, 0, 0);
        } else {
          This->ThumbPos = ThumbScrollRange;
        }
      } else {
        if (This->ThumbPos > ThumbScrollRange / 2) {
          SendMessage (Hwnd, UI_NOTIFY_ACTIVATE, 0, 0);
        } else {
          This->ThumbPos = 0;
        }
      }
      Control->Invalidate (Control);
    } else {
      return UiButtonProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_STATUSIMAGE) {
      UiSwitchPaintStatusImage (This, Manager->PaintDc);
    } else  {
      if (WParam == PAINT_TEXT) {
        break;
      }
      UiButtonProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  default:
    return UiButtonProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

VOID
UiEditCreate (
  UI_EDIT *This
  )
{
  UI_CONTROL *Control;
  SIZE       Size;

  Control = (UI_CONTROL *) This;

  GetTextExtentPoint32 (Control->Manager->PaintDc, L"X", 1, &Size);
  This->CharHeight  = Size.cy;
  This->EditPos     = 0;
  This->MinValue    = 0;
  This->MaxValue    = 0;
  This->Step        = 0;
  This->IsDec       = FALSE;
  This->IsHex       = FALSE;
}

VOID
UiEditSetPasswordMode (
  UI_EDIT *This,
  BOOLEAN PasswordMode
  )
{
  if (This->IsPasswordMode != PasswordMode) {
    This->IsPasswordMode = PasswordMode;
    ((UI_CONTROL *)This)->Invalidate ((UI_CONTROL *)This);
  }
}

VOID
UiEditSetReadOnly (
  UI_EDIT *This,
  BOOLEAN ReadOnly
  )
{
  if (This->IsReadOnly != ReadOnly) {
    This->IsReadOnly = ReadOnly;
    ((UI_CONTROL *)This)->Invalidate ((UI_CONTROL *)This);
  }
}


SIZE
UiEditEstimateSize (
  UI_CONTROL *Control,
  SIZE       AvailableSize
  )
{

  RECT        TextRc;
  UI_EDIT     *This;
  SIZE        Size;

  This = (UI_EDIT *) Control;

  if (Control->FixedSize.cx != 0) {
   AvailableSize.cx = MIN (AvailableSize.cx, Control->FixedSize.cx );
  }

  if (Control->FontSize == 0) {
    Control->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
  }

  if (Control->Wnd->szTitle[0] == '\0') {
    Size    = Control->FixedSize;
    Size.cy = Control->FontSize;
    return Size;
  }

  SetRect (&TextRc, 0, 0, AvailableSize.cx, 9999);
//TextRc.left  += This->TextPadding.left;
//TextRc.right -= This->TextPadding.right;
  if (Control->FontSize == 0) {
    Control->FontSize = Control->Manager->GetDefaultFontSize (Control->Manager);
  }
  if (Control->Manager->PaintDc) {
    Control->Manager->PaintDc->font->FontSize = Control->FontSize;
  }

  UiPaintText (
    Control->Manager->PaintDc,
    &TextRc,
    Control->Wnd->szTitle,
    INVALID_COLOR,
    INVALID_COLOR,
    DT_WORDBREAK | DT_CALCRECT
    );

  Size.cx = TextRc.right - TextRc.left; // + This->TextPadding.left + This->TextPadding.right;
  Size.cy = TextRc.bottom - TextRc.top; // + This->TextPadding.top + This->TextPadding.bottom;

  if (Control->FixedSize.cy != 0) {
    Size.cy = Control->FixedSize.cy;
  }

  if (Control->FixedSize.cx != 0) {
    Size.cx = Control->FixedSize.cx;
  }

  return Size;
}



BOOLEAN
UiEditSetAttribute (
  UI_CONTROL  *Control,
  CHAR16      *Name,
  CHAR16      *Value
  )
{
  UI_EDIT    *This;

  This = (UI_EDIT *) Control;

  if (StrCmp (Name, L"hex") == 0) {
      This->IsHex = (StrCmp (Value, L"true") == 0) ? TRUE : FALSE;
  } else if (StrCmp (Name, L"dec") == 0) {
      This->IsDec = (StrCmp (Value, L"true") == 0) ? TRUE : FALSE;
  } else if (StrCmp (Name, L"password") == 0) {
    UiEditSetPasswordMode (This, (StrCmp (Value, L"true") == 0));
  } else if (StrCmp (Name, L"readonly") == 0) {
    UiEditSetReadOnly (This, (StrCmp (Value, L"true") == 0));
  } else {
    return UiLabelSetAttribute (Control, Name, Value);
  }

  return TRUE;
}

VOID
UiEditKeyDown (
  HWND Hwnd,
  WPARAM WParam,
  LPARAM LParam
  )
{

  UI_CONTROL *Control;
  UI_EDIT    *Edit;
  BOOLEAN    ReDraw;
  UINTN      LastEditPos;
  SIZE       Size;
  UI_MANAGER *Manager;

  Control = GetUiControl (Hwnd);
  Edit    = (UI_EDIT *) Control;
  LastEditPos = Edit->EditPos;
  Manager = Control->Manager;


  ReDraw  = FALSE;

  switch (WParam) {

  case VK_LEFT:
    if (Edit->EditPos > 0) {
      Edit->EditPos--;
    }

    break;

  case VK_RIGHT:
    if (Edit->EditPos < StrLen (Hwnd->szTitle)) {
      Edit->EditPos++;
    }
    break;

  case VK_HOME:
    Edit->EditPos = 0;
    break;

  case VK_END:
    Edit->EditPos = (UINT32) StrLen (Hwnd->szTitle);
    break;


  case VK_BACK:
    if (Edit->EditPos > 0) {
      Edit->EditPos--;
      memmove (Hwnd->szTitle + Edit->EditPos,
         Hwnd->szTitle + Edit->EditPos + 1,
         (StrLen (Hwnd->szTitle) - Edit->EditPos + 1) * sizeof (CHAR16));
      ReDraw = TRUE;
    }
    break;

  case VK_DELETE:
    if (Edit->EditPos < StrLen (Hwnd->szTitle)) {
      memmove (Hwnd->szTitle + Edit->EditPos,
         Hwnd->szTitle + Edit->EditPos + 1,
         (StrLen (Hwnd->szTitle) - Edit->EditPos + 1) * sizeof (CHAR16));
      ReDraw = TRUE;
    }
    break;
  }

  if (LastEditPos != Edit->EditPos || ReDraw) {
    if (Edit->EditPos != 0) {
      GetTextExtentPoint32 (Manager->PaintDc, Hwnd->szTitle, (UINT32)Edit->EditPos, &Size);
//    SetCaretPos (Size.cx, 1);
    } else {
//    SetCaretPos (0, 1);
    }

    Control->Invalidate (Control);
  }
}

VOID
UiEditCharPress (
  HWND     Hwnd,
  WPARAM   WParam,
  LPARAM   LParam
  )
{
  CHAR16   Char;
  CHAR16   *OldStr;
  CHAR16   *NewStr;
  UI_EDIT  *This;
  SIZE     Size;
  UINT8    Digital;
  UINT64   EditValue;
  EFI_STATUS Status;
  UI_CONTROL *Control;

  This    = (UI_EDIT *) GetUiControl (Hwnd);
  Control = (UI_CONTROL *)This;

  if (This->IsReadOnly) {
    return ;
  }

  Char = LOWORD (WParam);

  switch (Char) {
    case 0x00:  /* NULL */
    case 0x07:  /* BEL */
    case 0x08:  /* BS */
    case 0x09:  /* HT */
    case 0x0A:  /* LF */
    case 0x0B:  /* VT */
    case 0x0C:  /* FF */
    case 0x1B:  /* Escape */
      return ;
    case 0x0D:  /* enter */
      mSetupMouse->CloseKeyboard (mSetupMouse);
      return ;
  }

  if (This->IsDec) {
    if (Char > L'9' || Char < L'0') {
      return ;
    }
  }

  if (This->IsHex) {
    if (!IsHexDigit (&Digital, Char)) {
      return ;
    }
  }

  //
  // insert data
  //
  OldStr = Hwnd->szTitle;
  NewStr = AllocatePool (StrSize (Hwnd->szTitle) + sizeof (CHAR16));

  if (This->EditPos != 0) {
    CopyMem (NewStr, OldStr, This->EditPos * sizeof (CHAR16));
  }

  NewStr[This->EditPos] = Char;

  CopyMem (NewStr + This->EditPos + 1,
        OldStr + This->EditPos,
        (StrLen (OldStr) - This->EditPos + 1) * sizeof (CHAR16));



  if (This->IsDec || This->IsHex) {
    if (This->IsDec) {
      EditValue = StrToUInt (NewStr, 10, &Status);
    } else {
      EditValue = StrToUInt (NewStr, 16, &Status);
    }

    if (EditValue > This->MaxValue) {
      gBS->FreePool (NewStr);
      return;
    }
  }

  if (OldStr != NULL) {
    gBS->FreePool (OldStr);
  }
  Hwnd->szTitle = NewStr;
  This->EditPos++;

  GetTextExtentPoint32 (Control->Manager->PaintDc, Hwnd->szTitle, This->EditPos, &Size);
//SetCaretPos (Size.cx, 1);
  Control->Invalidate (Control);
  return ;
}



LRESULT
UiEditProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_EDIT      *This;
  UI_CONTROL   *Control;
  UI_MANAGER   *Manager;
//POINT        Point;
  RECT         Rc;
  SIZE         Size;
  LRESULT      Ret;
  CHAR16       *StrBuf;
  CHAR16       *TmpPtr;
  UINTN        Len;
  UINTN        Index;
  KEYBOARD_ATTRIBUTES  KeyboardAttributes;


  Control = (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);
  This    = (UI_EDIT *) Control;
  Manager = NULL;
  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_EDIT *) AllocateZeroPool (sizeof (UI_EDIT));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiLabelProc (Hwnd, Msg, WParam, LParam);
    UiEditCreate (This);
    Control->SetAttribute = UiEditSetAttribute;
    Control->EstimateSize = UiEditEstimateSize;
    Control->BkColor     = 0xFFD0D0D0;
    Control->BorderColor = 0xFF808080;
    SetWindowLongPtr (
      Control->Wnd,
      GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE
      );
    break;

  case WM_SETFOCUS:
    if ((Control->StateFlags & UISTATE_CAPTURED) != 0) {
      return 0;
    }


    Control->StateFlags |= UISTATE_CAPTURED;

    if (This->IsReadOnly) {
      return 0;
    }

//  CreateCaret(Hwnd, NULL, 1, This->CharHeight);
    //This->EditPos = 0;
    This->EditPos = (UINT32) StrLen (Hwnd->szTitle);
    GetTextExtentPoint32 (Control->Manager->PaintDc, Hwnd->szTitle, This->EditPos, &Size);
//  SetCaretPos (Size.cx, 1);
//  ShowCaret (Hwnd);
    SendMessage (Manager->MainWnd, UI_NOTIFY_EDITFOCUS, (WPARAM)Control, 0);
    Control->BkColor     = 0xFFFFFFFF;
    Control->BorderColor = 0xFF800000;
    Control->Invalidate (Control);
//  mSetupMouse->DrawKeyboard (mSetupMouse, 0, 0);
    break;

  case WM_KILLFOCUS:
    Control->StateFlags &= ~UISTATE_CAPTURED;
//  HideCaret(Hwnd);
//  DestroyCaret();
    SendMessage (Manager->MainWnd, UI_NOTIFY_BLUR, (WPARAM)Control, 0);
    Control->BkColor     = 0xFFD0D0D0;
    Control->BorderColor = 0xFF808080;
    Control->Invalidate  (Control);
    mSetupMouse->CloseKeyboard (mSetupMouse);
    break;


  case WM_CHAR:
    UiEditCharPress (Hwnd, WParam, LParam);
    break;

  case WM_KEYDOWN:
    if (WParam == VK_RETURN) {
      SendMessage (Manager->MainWnd, UI_NOTIFY_CARRIAGE_RETURN, (WPARAM) Control, 0);
    } else {
      UiEditKeyDown (Hwnd, WParam, LParam);
    }
    break;

  case WM_LBUTTONDOWN:
//  if (GetCapture () != Hwnd) {
//    SetCapture (Hwnd);
//    SetFocus (Hwnd);
//    return 0;
//  }
//
    UiLabelProc (Hwnd, Msg, WParam, LParam);
    GetWindowRect (Hwnd, &Rc);
    if ((GetWindowLongPtr (Hwnd, GWL_STYLE) & WS_DISABLED) == 0) {
      mSetupMouse->GetKeyboardAttributes (mSetupMouse, &KeyboardAttributes);
      if (!KeyboardAttributes.IsStart) {
        mSetupMouse->StartKeyboard (mSetupMouse, Rc.left, Rc.bottom + 20);
      }
    }
//  ReleaseCapture ();
//  if (
//
//  ReleaseCapture ();
//  This->StateFlags &= ~UISTATE_CAPTURED;
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_TEXT && This->IsPasswordMode) {
      Len = StrLen (Control->Wnd->szTitle);
      if (Len != 0) {
        TmpPtr = Control->Wnd->szTitle;
        StrBuf = AllocateZeroPool ((Len + 1) * sizeof (CHAR16));
        for (Index = 0; Index < Len; Index++) {
          StrBuf[Index] = L'*';
        }
        Control->Wnd->szTitle = StrBuf;
        Ret = UiLabelProc (Hwnd, Msg, WParam, LParam);
        Control->Wnd->szTitle = TmpPtr;
        gBS->FreePool (StrBuf);
        return Ret;
      }
    }
    return UiLabelProc (Hwnd, Msg, WParam, LParam);
    break;

  default:
    return UiLabelProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;
}


VOID
UiScrollBarSetPosition (
  UI_SCROLLBAR *This
  )
{

  RECT Rc;
  UI_CONTROL *Control;
  INT32 Cx;
  INT32  Cy;
  INT32 ThumbX;
  INT32 ThumbY;

  Control = (UI_CONTROL *) This;

  GetClientRect (Control->Wnd, &Rc);


  if (This->Horizontal) {

    Cx = Rc.right - Rc.left;
    Cx -= Control->FixedSize.cy * 2;

    This->Button1.left   = Rc.left;
    This->Button1.top    = Rc.top;
    This->Button1.right  = Rc.left + Control->FixedSize.cy;
    This->Button1.bottom = Rc.top  + Control->FixedSize.cy;

    This->Button2.right   = Rc.right;
    This->Button2.top     = Rc.top;
    This->Button2.left    = Rc.right - Control->FixedSize.cy;
    This->Button2.bottom  = Rc.top   + Control->FixedSize.cy;


    This->Thumb.top       = Rc.top;
    This->Thumb.bottom    = Rc.top + Control->FixedSize.cy;

    ThumbX = Cx * (Rc.right - Rc.left) / (This->Range + Rc.right - Rc.left);

    This->Thumb.left  = This->ScrollPos * (Cx - ThumbX) / This->Range + This->Button1.right;
    This->Thumb.right = This->Thumb.left + ThumbX;

  } else {

    Cy   = Rc.bottom - Rc.top;

    This->Button1.left   = Rc.left;
    This->Button1.top    = Rc.top;

    if (This->ShowButton1) {
      Cy -= Control->FixedSize.cx;
      This->Button1.right  = Rc.left + Control->FixedSize.cx;
      This->Button1.bottom = Rc.top  + Control->FixedSize.cx;
    } else {
      This->Button1.right  = This->Button1.left;
      This->Button1.bottom = This->Button1.top;
    }

    if (This->ShowButton2) {
      Cy -= Control->FixedSize.cx;
      This->Button2.left   = Rc.left;
      This->Button2.bottom = Rc.bottom;
      This->Button2.right  = Rc.left   + Control->FixedSize.cx;
      This->Button2.top    = Rc.bottom - Control->FixedSize.cx;
    }

    This->Thumb.left     = Rc.left;
    This->Thumb.right    = Rc.right;

    ThumbY = Cy * (Rc.bottom - Rc.top) / (This->Range + Rc.bottom - Rc.top);

    This->Thumb.top    = This->ScrollPos * (Cy - ThumbY) / This->Range + This->Button1.bottom;
    This->Thumb.bottom = This->Thumb.top + ThumbY;
  }


}


VOID
UiScrollBarSetScrollPos (
  UI_SCROLLBAR *This,
  INT32   Pos
  )
{

  if (This->ScrollPos == Pos) {
    return ;
  }

  This->ScrollPos = Pos;
  if (This->ScrollPos < 0) {
    This->ScrollPos = 0;
  }

  if (This->ScrollPos > This->Range) {
    This->ScrollPos = This->Range;
  }

  if (This->Owner != NULL) {
    This->Owner->ScrollPos.y = This->ScrollPos;
  }
//SendMessage (This->Owner->Control.Wnd, UI_NOTIFY_SET_POSITION, 0, 0);
  This->Owner->Control.Manager->NeedUpdatePos = TRUE;
  This->Owner->Control.Invalidate (&This->Owner->Control);

  UiScrollBarSetPosition (This);
  This->Control.Invalidate (&This->Control);
}

VOID
UiScrollBarCreate  (
  UI_SCROLLBAR *This
  )
{
  This->Owner      = NULL;
  This->Horizontal = FALSE;

  This->Range          = 2000;
  This->LineSize       = 25;
  This->ScrollPos      = 0;
  This->LastScrollPos = 0;
  This->LastScrollOffset = 0;
  This->ScrollRepeatDelay  = 0;

  This->LastMouse.x    = 0;
  This->LastMouse.y    = 0;

  This->ShowButton1    = FALSE;
  This->ShowButton2    = FALSE;
  This->ShowThumb      = TRUE;

  This->SetScrollPos   = UiScrollBarSetScrollPos;
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
      Control->FixedSize.cx = 20;
      Control->FixedSize.cy = 0;

    }
  }
}

BOOLEAN
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
  } else {
    return UiControlSetAttribute (Control, Name, Value);
  }

  return TRUE;
}

VOID
UiScrollBarPaint (
  UI_SCROLLBAR *This
  )
{

  HDC   Hdc;
  UI_MANAGER  *Manager;
  UI_CONTROL  *Control;
  RECT  Rc;
  RECT  TmpRc;
  POINT       Arrows[3];
  CHAR16      Str[20];

  Control  = (UI_CONTROL *) This;
  Manager  = Control->Manager;
  Hdc      = Manager->PaintDc;
  Manager->GetControlRect (Manager, Control, &Rc);

  SelectObject (Hdc, GetStockObject (DC_BRUSH));
  SetDCBrushColor (Hdc, 0xFF000000);

  if (This->ShowButton1) {
    //
    // left
    //
    TmpRc = This->Button1;
    OffsetRect (&TmpRc, Rc.left, Rc.top);
    Arrows[0].x = TmpRc.left + 2;
    Arrows[0].y = TmpRc.top  + Control->FixedSize.cy / 2;
    Arrows[1].x = TmpRc.left + Control->FixedSize.cy - 4 - 2;
    Arrows[1].y = TmpRc.top  + 2 + 2;
    Arrows[2].x = TmpRc.left + Control->FixedSize.cy - 4 - 2;
    Arrows[2].y = TmpRc.bottom - 4 - 2 + 1;
    Polygon(Hdc, Arrows, 3);
  }

  if (This->ShowButton2) {
    //
    // right
    //
    TmpRc = This->Button2;
    OffsetRect (&TmpRc, Rc.left, Rc.top);
    Arrows[0].x = TmpRc.right  - 2;
    Arrows[0].y = TmpRc.top    + Control->FixedSize.cy / 2;
    Arrows[1].x = TmpRc.right  - Control->FixedSize.cy + 4 + 2;
    Arrows[1].y = TmpRc.top    + 2 + 2;
    Arrows[2].x = TmpRc.right  - Control->FixedSize.cy + 4 + 2;
    Arrows[2].y = TmpRc.bottom - 4 - 2 + 1;
    Polygon(Hdc, Arrows, 3);
  }

  if (This->ShowThumb) {

    //
    // thumb
    //
    TmpRc = This->Thumb;
    OffsetRect (&TmpRc, Rc.left, Rc.top);

    SelectObject (Hdc, GetStockObject (DC_PEN));
    SelectObject (Hdc, GetStockObject (NULL_BRUSH));
    SetDCPenColor (Hdc, 0xFF000000);

    InflateRect (&TmpRc, -1, -2);
    if (TmpRc.top >= TmpRc.bottom || TmpRc.left >= TmpRc.right) {
      return ;
    }

    if ((This->ThumbState & (UISTATE_CAPTURED | UISTATE_HOT)) != 0) {
      Control->BkColor  = 0xFFCBCBCB;
      Control->BkColor2 = 0xFF989898;
      UiPaintGradient (Hdc, &TmpRc, Control->BkColor2, Control->BkColor, FALSE, 64);
    } else {
//      Control->BkColor  = 0xFFDCDCDC;
//      Control->BkColor2 = 0xFFCBCBCB;
      Control->BkColor  = 0xFFCBCBCB;
      Control->BkColor2 = 0xFF989898;
      UiPaintGradient (Hdc, &TmpRc, Control->BkColor2, Control->BkColor, FALSE, 64);
    }

    TmpRc.top = (TmpRc.bottom + TmpRc.top ) / 2;
    UnicodeSPrint (Str, sizeof (Str),  L"%d", This->ScrollPos);
  }

}

LRESULT
UiScrollBarProc (
  HWND   Hwnd,
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


  Control = (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);
  This    = (UI_SCROLLBAR *) Control;
  Manager = NULL;

  if (Control != NULL) {
    Manager = Control->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (UI_SCROLLBAR *) AllocateZeroPool (sizeof (UI_SCROLLBAR));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiControlProc (Hwnd, Msg, WParam, LParam);
    UiScrollBarCreate (This);
    Control->SetAttribute = UiScrollBarSetAttribute;

    break;

  case UI_NOTIFY_SET_POSITION:
    UiScrollBarSetPosition (This);
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_ALL) {
      Control->StateFlags &= ~UISTATE_INVALIDATED;
      UiScrollBarPaint (This);
    }
    break;

  case WM_LBUTTONDOWN:
    POINTSTOPOINT(Point, LParam);
    if (PtInRect (&This->Button1, Point)) {
      This->Button1State |= UISTATE_PRESSED;
      UiScrollBarSetScrollPos (This, This->ScrollPos - This->LineSize);
    } else if (PtInRect (&This->Button2, Point)) {
      This->Button2State |= UISTATE_PRESSED;
      UiScrollBarSetScrollPos (This, This->ScrollPos + This->LineSize);
    } else if (PtInRect (&This->Thumb, Point)) {
      This->ThumbState |= UISTATE_CAPTURED | UISTATE_PRESSED;
      ClientToScreen (Hwnd, &Point);
      This->LastMouse     = Point;
      This->LastScrollPos = This->ScrollPos;
    } else {
      if (Point.y < This->Thumb.top) {
        UiScrollBarSetScrollPos (This, This->ScrollPos - This->LineSize);
      } else if (Point.y > This->Thumb.bottom) {
        UiScrollBarSetScrollPos (This, This->ScrollPos + This->LineSize);
      }
    }
    SetCapture (Hwnd);
    This->ScrollRepeatDelay = 0;
    SetTimer (Hwnd, SCROLLBAR_TIMER_ID, 50, NULL);
    Control->Invalidate (Control);
    break;

  case WM_LBUTTONUP:
  case WM_NCLBUTTONUP:
    if (GetCapture () == Hwnd) {
      ReleaseCapture();
      KillTimer (Hwnd, SCROLLBAR_TIMER_ID);
      if ((This->ThumbState & UISTATE_CAPTURED) != 0) {
        This->ThumbState &= ~( UISTATE_CAPTURED | UISTATE_PRESSED );
        Control->Invalidate (Control);
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
      Control->Invalidate (Control);
    } else {

      if (This->ScrollRepeatDelay < 5) {
        break;
      }
      GetCursorPos (&Point);
      ScreenToClient (Hwnd, &Point);

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
    GetClientRect (Hwnd, &Rc);
    if (Msg == WM_MOUSEMOVE) {
      ClientToScreen (Hwnd, &Point);
    }

    if (This->Horizontal) {
      This->LastScrollOffset = (Point.x - This->LastMouse.x) * This->Range / (Rc.right - Rc.left - This->Thumb.right + This->Thumb.left - 2 * Control->FixedSize.cy);
    } else {
      This->LastScrollOffset = (Point.y - This->LastMouse.y) * This->Range / (Rc.bottom - Rc.top - This->Thumb.bottom + This->Thumb.top - 2 * Control->FixedSize.cx);
    }
    break;


  case WM_MOUSEENTER:
    POINTSTOPOINT(Point, LParam);
    ScreenToClient (Hwnd, &Point);
    This->Button1State |= UISTATE_HOT;
    This->Button2State |= UISTATE_HOT;
//  if(PtInRect(&This->Thumb, Point) ) {
      This->ThumbState |= UISTATE_HOT;
//  }
    Control->Invalidate (Control);
    break;


  case WM_MOUSELEAVE:
    This->Button1State &= ~UISTATE_HOT;
    This->Button2State &= ~UISTATE_HOT;
    This->ThumbState   &= ~UISTATE_HOT;
    Control->Invalidate (Control);
    break;


  case WM_PAINT:
    break;

  case WM_DESTROY:
    KillTimer (Hwnd, SCROLLBAR_TIMER_ID);
    break;

  default:
    return UiControlProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;
}


UINT32
RegisterUiControl (
  HINSTANCE hInstance
  )
{
  WNDCLASS  wc;

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiControlProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (VOID *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"Control";

  return RegisterClass(&wc);
}


UINT32
RegisterUiLabel (HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiLabelProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"Label";

  return RegisterClass(&wc);
}

UINT32
RegisterUiButton (HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiButtonProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"Button";

  return RegisterClass(&wc);
}

UINT32
RegisterUiSwitch (HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiSwitchProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"Switch";

  return RegisterClass(&wc);
}




UINT32
RegisterUiScrollBar (HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiScrollBarProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"ScrollBar";

  return RegisterClass(&wc);
}

UINT32
RegisterUiEditBar (HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiEditProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon   = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"UiEdit";

  return RegisterClass(&wc);
}

