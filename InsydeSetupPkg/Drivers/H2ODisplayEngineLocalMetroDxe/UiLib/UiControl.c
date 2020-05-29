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


VOID
UiControlOnUiCreate (
  UI_CONTROL                    *Control,
  HWND                          Wnd,
  UI_MANAGER                    *Manager
  )
{
  UI_CONTROL                    *This;

  This = (UI_CONTROL *) Control;

  This->Wnd             = Wnd;
  if (Manager != NULL) {
    This->Manager       = Manager;
  }

  This = (UI_CONTROL *) Control;

  //
  // original container
  //
  This->ItemCount = 0;
  This->Items     = NULL;
  This->ChildPadding = 0;
  This->HScrollBar = NULL;
  This->VScrollBar   = NULL;
  This->ScrollPos.x = 0;
  This->ScrollPos.y = 0;
  This->TouchSlop   = 4;
  This->IsBeginDraged   = FALSE;
  This->Dragged         = FALSE;
  This->BkImageStyle    = 0;

  This->UserData        = NULL;
  This->Name            = L"";
  This->BkColor         = 0x00000000;
  This->FocusBkColor    = 0x00000000;
  This->BorderColor     = 0;
  This->FixedSize.cx    = 0;
  This->FixedSize.cy    = 0;
  This->Float           = FALSE;
  This->GradientVertial = TRUE;
  This->NeedUpdateChildPos   = TRUE;
  SetRectEmpty (&This->Scale9Grid);

  //
  // no focus
  //
  SetWindowLongPtr (
    This->Wnd,
    GWL_EXSTYLE,
    GetWindowLongPtr (This->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE
    );
}


VOID
UiControlOnDestroy (
  UI_CONTROL                    *Control
  )
{
  UI_CONTROL                    *This;


  This = (UI_CONTROL *) Control;

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
    FreePool (This->Name);
  }

  if (This->UserData != NULL) {
    FreePool (This->UserData);
  }

  if (This->BkImage != NULL) {
    FreePool (This->BkImage);
  }

  if (This->HScrollBar != NULL) {
    DestroyWindow (((UI_CONTROL *)(This->HScrollBar))->Wnd);
  }

  if (This->VScrollBar != NULL) {
    DestroyWindow (((UI_CONTROL *)(This->VScrollBar))->Wnd);
  }

  CONTROL_CLASS (This)->RemoveAllChild (This);

  FreePool (This);
}


VOID
UiControlSetPosition (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  RECT                          Rc;
  UINTN                         Index;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;
  UI_CONTROL                    *This;
  SIZE                          Size;
  SIZE                          Available;
  POINT                         LeftTop;

  This = (UI_CONTROL *) Control;

  ASSERT (Pos->left <= Pos->right);
  ASSERT (Pos->top <= Pos->bottom);

  ShowSetPositionDbgInfo (Control, Pos);
  GetWindowRect(Control->Wnd, &Rc);
  MapWindowPoints(HWND_DESKTOP, GetParent(Control->Wnd), (LPPOINT) &Rc, 2);
  if (!EqualRect (&Rc, Pos)) {
    MoveWindow (Control->Wnd, Pos->left, Pos->top, Pos->right - Pos->left, Pos->bottom - Pos->top, TRUE);
    CONTROL_CLASS_INVALIDATE (This);
    This->NeedUpdateChildPos = TRUE;
  }

  if (This->ItemCount == 0) {
    return ;
  }

  if (!This->NeedUpdateChildPos) {
    return ;
  }
  This->NeedUpdateChildPos = FALSE;


  CopyRect (&Rc, Pos);
  OffsetRect (&Rc, -Rc.left, -Rc.top);

  Rc.left   += This->Padding.left;
  Rc.top    += This->Padding.top;
  Rc.right  -= This->Padding.right;
  Rc.bottom -= This->Padding.bottom;

  if (Rc.left > Rc.right) {
    Rc.left = Rc.right;
  }
  if (Rc.top > Rc.bottom) {
    Rc.top = Rc.bottom;
  }

  Available.cx = Rc.right - Rc.left;
  Available.cy = Rc.bottom - Rc.top;
  for (Index = 0; Index < This->ItemCount; Index++) {
    Item = This->Items[Index];

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    Size = CONTROL_CLASS (Item)->EstimateSize (Item, Available);

    if (Size.cx == 0 || Size.cx == MATCH_PARENT) {
      Size.cx = Rc.right - Rc.left;
    }

    if (Size.cy == 0 || Size.cy == MATCH_PARENT) {
      Size.cy = Rc.bottom - Rc.top;
    }

    LeftTop = Item->FixedXY;
    if (LeftTop.x == 0) {
      LeftTop.x = Rc.left;
    }
    if (LeftTop.y == 0) {
      LeftTop.y = Rc.top;
    }

    SetRect (
      &ItemRc,
      LeftTop.x,
      LeftTop.y,
      LeftTop.x + Size.cx,
      LeftTop.y + Size.cy
      );
    mSetPositionLevel++;
    CONTROL_CLASS (Item)->SetPosition (Item, &ItemRc);
    mSetPositionLevel--;
  }
}


VOID
UiControlOnUiPaint (
  UI_CONTROL                    *Control,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  COLORREF                      Color;
  RECT                          Rc;
  HDC                           Hdc;
  UI_MANAGER                    *Manager;
  HWND                          Wnd;
  RECT                          ClipRc;
  HRGN                          OldRgn;
  HRGN                          NewRgn;
  UINTN                         Index;
  UI_CONTROL                    *This;
  UI_CONTROL                    *Item;
  RECT                          TmpRc;
  RECT                          ItemRc;

  This = (UI_CONTROL *) Control;

  Manager = Control->Manager;
  Hdc = Control->Manager->PaintDC;
  Manager->GetControlRect (Manager, Control, &Rc);
  Wnd = Control->Wnd;

  switch (WParam) {

  case PAINT_ALL:
    CONTROL_CLASS_SET_STATE(Control, 0, UISTATE_INVALIDATED);

    Manager->GetControlRect (Manager, This, &Rc);
    if (!IntersectRect (&TmpRc, &Rc, (RECT *)LParam)) {
      return ;
    }

    SendMessage (Wnd, UI_NOTIFY_PAINT,  PAINT_BKCOLOR, LParam);
    SendMessage (Wnd, UI_NOTIFY_PAINT,  PAINT_BKIMAGE, LParam);
    SendMessage (Wnd, UI_NOTIFY_PAINT,  PAINT_STATUSIMAGE, LParam);
    SendMessage (Wnd, UI_NOTIFY_PAINT,  PAINT_TEXT, LParam);
    SendMessage (Wnd, UI_NOTIFY_PAINT,  PAINT_BORDER, LParam);

    GetClipBox (Manager->PaintDC, &ClipRc);
    OldRgn = CreateRectRgnIndirect (&ClipRc);
    NewRgn = CreateRectRgnIndirect (&TmpRc);
    ExtSelectClipRgn (Manager->PaintDC, NewRgn, RGN_AND);


    for (Index = 0; Index < This->ItemCount; Index++) {
      Item = This->Items[Index];

      if (!IsWindowVisible (Item->Wnd)) {
        continue;
      }


      Manager->GetControlRect (Manager, Item, &ItemRc);
      if (IntersectRect (&TmpRc, &ItemRc, (RECT *)LParam)) {
        SendMessage (Item->Wnd, UI_NOTIFY_PAINT, PAINT_ALL, (LPARAM)&TmpRc);
      }
    }

    if (This->HScrollBar != NULL) {
      Manager->GetControlRect (Manager, (UI_CONTROL *)This->HScrollBar, &ItemRc);
      if (IntersectRect (&TmpRc, &ItemRc, (RECT *)LParam)) {
        SendMessage (This->HScrollBar->Control.Wnd, UI_NOTIFY_PAINT, PAINT_ALL, (LPARAM)&TmpRc);
      }
    }

    if (This->VScrollBar != NULL) {
      Manager->GetControlRect (Manager, (UI_CONTROL *)This->VScrollBar, &ItemRc);
      if (IntersectRect (&TmpRc, &ItemRc, (RECT *)LParam)) {
        SendMessage (This->VScrollBar->Control.Wnd, UI_NOTIFY_PAINT, PAINT_ALL, (LPARAM)&TmpRc);
      }
    }

    SelectClipRgn(Manager->PaintDC, OldRgn);
    if (OldRgn != NULL) {
      DeleteObject(OldRgn);
    }
    if (NewRgn != NULL) {
      DeleteObject(NewRgn);
    }
    break;

  case PAINT_BKIMAGE:
    if (Control->BkImage != 0) {

      ASSERT (Rc.right > Rc.left && Rc.bottom > Rc.top);

      UiShowBitmap (
        Control->Wnd,
        Manager->PaintDC,
        &Rc,
        &Control->Scale9Grid,
        GetImageByString (Control->BkImage),
        Control->BkImageStyle,
        &Control->HsvDiff
        );
    }
    break;

  case PAINT_BKCOLOR:
    if ((CONTROL_CLASS_GET_STATE(Control) & UISTATE_FOCUSED) != 0 && Control->FocusBkColor != 0) {
      Color = Control->FocusBkColor;
    } else {
      Color = Control->BkColor;
    }

    if (Color != 0) {
      if (Color >= 0xFF000000) {
        UiFastFillRect (Hdc, &Rc, Color);
      } else {
        HDC      hdcMem;
        HBITMAP  hbmp, hbmpOrg, hbmpDel;
        RECT     rc;

        GetClientRect (Control->Wnd, &rc);
        /* redirect painting to offscreen dc*/
        hdcMem  = CreateCompatibleDC(Hdc);
        if (hdcMem != NULL) {
          hbmp    = CreateCompatibleBitmap(hdcMem, rc.right, rc.bottom);
          hbmpOrg = SelectObject(hdcMem, hbmp);

          UiFastFillRect (hdcMem, &rc, Color);

          /* alpha blend blit offscreen map with physical screen*/
          BitBlt(Hdc, Rc.left, Rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, MWROP_SRC_OVER);  /* FIXME specify constant alpha somewhere!*/
          hbmpDel = SelectObject(hdcMem, hbmpOrg);
          if (hbmpDel != NULL) {
            DeleteObject(hbmpDel);
          }
          DeleteDC(hdcMem);
        }

      }
    }
    break;

  case PAINT_STATUSIMAGE:
    break;

  case PAINT_TEXT:
    break;

  case PAINT_BORDER:
if (0) {
    Color = Control->BorderColor;
    if (CONTROL_CLASS_GET_STATE(Control) & UISTATE_FOCUSED) {
      Color = 0xFFFF0000;
    }
    if (Color != 0) {
      SelectObject (Hdc, GetStockObject (PS_NULL));
      SelectObject (Hdc, GetStockObject (DC_PEN));
      SetDCPenColor (Hdc, Color);
      Rectangle (Hdc, Rc.left, Rc.top, Rc.right, Rc.bottom);
    }
    if (CONTROL_CLASS_GET_STATE(Control) & UISTATE_FOCUSED) {
      Rectangle (Hdc, Rc.left + 1, Rc.top + 1, Rc.right - 1, Rc.bottom - 1);
    }
}
    break;

  default:
    ASSERT (FALSE);
    break;

  }
}

VOID
UiControlSetPos (
  UI_CONTROL                    *Control,
  RECT                          Rc
  )
{
  UI_CONTROL                    *This;

  This = (UI_CONTROL *) Control;

  Control->FixedXY.x = Rc.left >= 0 ? Rc.left : Rc.right;
  Control->FixedXY.y = Rc.top  >= 0 ? Rc.top  : Rc.bottom;

  Control->FixedSize.cx = Rc.right - Rc.left;
  Control->FixedSize.cy = Rc.bottom - Rc.top;

  UiNeedUpdatePos (Control);
}


UI_STATE
EFIAPI
UiControlGetState (
  UI_CONTROL                    *Control
  )
{
  return Control->StateFlags;
}

VOID
EFIAPI
UiControlSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  UI_STATE                      OldState;

  if (Control->OnSetState) {
    Control->OnSetState (Control, SetState, ClearState);
  }
  OldState = Control->StateFlags;

  Control->StateFlags |= SetState;
  Control->StateFlags &= ~(ClearState);

  if ((Control->StateFlags != OldState) &&
      !(ClearState & UISTATE_INVALIDATED) &&
      !(Control->StateFlags & UISTATE_INVALIDATED)) {
    CONTROL_CLASS_INVALIDATE (Control);
  }
}

BOOLEAN
UiControlSetAttribute (
  UI_CONTROL                    *Control,
  CHAR16                        *Name,
  CHAR16                        *Value
  )
{
  EFI_STATUS                    Status;
  CHAR16                        *PtrValue;
  UI_CONTROL                    *ScrollBar;
  UI_CONTROL                    *This;

  This = (UI_CONTROL *) Control;

  if (StrCmp (Name, L"name") == 0) {
    ASSERT (This->Name);
    if (StrCmp(This->Name, L"") != 0) {
      StringMapRemove (&This->Manager->NameList, Value);
      FreePool (This->Name);
    }
    This->Name = AllocateCopyPool (StrSize(Value), Value);
    StringMapAdd (&This->Manager->NameList, Value, This);
  } else if (StrCmp (Name, L"width") == 0) {
    if (isalpha (Value[0])) {
      if (StrCmp (Value, L"match_parent") == 0) {
        This->FixedSize.cx = MATCH_PARENT;
      } else if (StrCmp (Value, L"wrap_content") == 0) {
        This->FixedSize.cx = WRAP_CONTENT;
      } else {
        DEBUG ((EFI_D_ERROR, "width can only use match_parent, wrap_content or const value"));
        ASSERT (FALSE);
      }
    } else {
      This->FixedSize.cx  = wcstol (Value, &PtrValue, 10);
    }
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"height") == 0) {
    if (isalpha (Value[0])) {
      if (StrCmp (Value, L"match_parent") == 0) {
        This->FixedSize.cy = MATCH_PARENT;
      } else if (StrCmp (Value, L"wrap_content") == 0) {
        This->FixedSize.cy = WRAP_CONTENT;
      } else {
        DEBUG ((EFI_D_ERROR, "width can only use match_parent, wrap_content or const value"));
        ASSERT (FALSE);
      }
    } else {
      This->FixedSize.cy  =  wcstol (Value, &PtrValue, 10);
    }
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"bkimage") == 0) {
    if (This->BkImage != NULL) {
      FreePool (This->BkImage);
    }
    This->BkImage     = AllocateCopyPool (StrSize(Value), Value);
  } else if (StrCmp (Name, L"bkcolor") == 0) {
    This->BkColor     = GetColorValue (Value);
  } else if (StrCmp (Name, L"focusbkcolor") == 0) {
    This->FocusBkColor = GetColorValue (Value);
  } else if (StrCmp (Name, L"bordercolor") == 0) {
    This->BorderColor = GetColorValue (Value);
  } else if (StrCmp (Name, L"visible") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      ShowWindow (This->Wnd, SW_SHOW);
    } else {
      ShowWindow (This->Wnd, SW_HIDE);
    }
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"padding") == 0) {
    This->Padding.top    = wcstol (Value, &PtrValue, 10);
    This->Padding.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    This->Padding.bottom = wcstol (PtrValue + 1, &PtrValue, 10);
    This->Padding.left   =  wcstol (PtrValue + 1, &PtrValue, 10);
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"pos") == 0) {
    RECT Rc;

    Rc.left   = wcstol (Value, &PtrValue, 10);
    Rc.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    Rc.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    Rc.bottom = wcstol (PtrValue + 1, &PtrValue, 10);
    UiControlSetPos (This, Rc);
  } else if (StrCmp (Name, L"scale9grid") == 0) {
    This->Scale9Grid.left   = wcstol (Value, &PtrValue, 10);
    This->Scale9Grid.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    This->Scale9Grid.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    This->Scale9Grid.bottom = wcstol (PtrValue + 1, &PtrValue, 10);
  } else if (StrCmp (Name, L"float") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      This->Float = TRUE;
    } else {
      This->Float = FALSE;
    }
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"gradient_vertical") == 0) {
    if (StrCmp (Value, L"true") == 0) {
      This->GradientVertial = TRUE;
    } else {
      This->GradientVertial = FALSE;
    }
  } else if (StrCmp (Name, L"user_data") == 0) {
    if (This->UserData != NULL) {
      FreePool (This->UserData);
    }
    This->UserData = AllocateCopyPool (StrSize (Value), Value);
  } else if (StrCmp (Name, L"bkimagestyle") == 0) {

    This->BkImageStyle = 0;
    if (StrStr (Value, L"center") != NULL) {
      This->BkImageStyle &= ~ (DT_STRETCH);
      This->BkImageStyle |= DT_CENTER | DT_VCENTER;
    }
    if (StrStr (Value, L"stretch") != NULL) {
      SetRectEmpty (&Control->Scale9Grid);
      Control->BkImageStyle &= ~ (DT_CENTER | DT_VCENTER);
      Control->BkImageStyle |= DT_STRETCH;
    }
    if (StrStr (Value, L"gray") != NULL) {
      Control->BkImageStyle |= DT_GRAY;
    }
    if (StrStr (Value, L"light") != NULL) {
      Control->BkImageStyle |= DT_LIGHT;
    }
    if (StrCmp (Value, L"") == 0) {
      This->BkImageStyle = 0;
    }
  } else if (StrCmp (Name, L"childpadding") == 0) {
    This->ChildPadding = (UINT32)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"vscrollbarpos") == 0) {
    if (This->VScrollBar != NULL) {
       SCROLLBAR_CLASS(This->VScrollBar)->SetScrollPos (This->VScrollBar, (INT32)StrToUInt (Value, 10, &Status));
    }
  } else if (StrCmp (Name, L"vscrollbar") == 0) {

    if (StrCmp (Value, L"true") == 0 &&
      This->VScrollBar == NULL) {
      ScrollBar = CreateControl (L"SCROLLBAR", This);
      ScrollBar->Class->SetAttribute (ScrollBar, L"hor", L"false");
      This->VScrollBar = (UI_SCROLLBAR *) ScrollBar;
      ((UI_SCROLLBAR *)ScrollBar)->Owner = This;
    }
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"hscrollbar") == 0) {

    if ((StrCmp (Value, L"true") == 0) && (This->HScrollBar == NULL)) {

      ScrollBar = CreateControl (L"SCROLLBAR", This);
      ScrollBar->Class->SetAttribute (ScrollBar, L"hor", L"true");
      This->HScrollBar = (UI_SCROLLBAR *) ScrollBar;
    }
    UiNeedUpdatePos (Control);
  } else if (StrCmp (Name, L"hsvadjust") == 0) {
    This->HsvDiff.HueDiff         = (INT16) wcstol (Value, &PtrValue, 10);
    This->HsvDiff.SaturationDiff  = (INT8) wcstol (PtrValue + 1, &PtrValue, 10);
    This->HsvDiff.ValueDiff       = (INT8) wcstol (PtrValue + 1, &PtrValue, 10);
  } else if (StrCmp (Name, L"minheight") == 0) {
    This->MinSize.cy  =  wcstol (Value, &PtrValue, 10);
    UiNeedUpdatePos (Control);
  } else {
    DEBUG ((EFI_D_ERROR, "Unsupported attribute name/name: %s %s", Name, Value));
    ASSERT (0);
    return FALSE;
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

BOOLEAN
EFIAPI
UiControlAddChild (
  UI_CONTROL                    *Control,
  UI_CONTROL                    *Child
  )
{
  UI_CONTROL                    *This;

  ASSERT (Control != NULL);

  This = (UI_CONTROL *) Control;
  This->ItemCount++;
  This->Items = realloc (This->Items, This->ItemCount * sizeof (UI_CONTROL *));
  if (This->Items == NULL) {
    return FALSE;
  }
  This->Items[This->ItemCount - 1] = Child;

  UiNeedUpdatePos (Child);

  return TRUE;
}

BOOLEAN
EFIAPI
UiControlRemoveChild (
  UI_CONTROL                    *Control,
  UI_CONTROL                    *Child
  )
{

  UINTN                         Index;
  UINTN                         Index2;
  UI_CONTROL                    *This;

  ASSERT (Control != NULL);
  This = (UI_CONTROL *) Control;

  for (Index = 0;  Index < This->ItemCount; Index++) {
    if (This->Items[Index] == Child) {
      for (Index2 = Index; Index < This->ItemCount - 1; Index2++) {
        This->Items[Index2] = This->Items[Index2 + 1];
      }
      This->ItemCount--;
      DestroyWindow (Child->Wnd);
      break;
    }
  }

  UiNeedUpdatePos (This);

  return TRUE;
}



BOOLEAN
EFIAPI
UiControlRemoveAllChild (
  UI_CONTROL                    *Control
  )
{
  UINTN Index;

  if (Control->ItemCount == 0) {
    return TRUE;
  }

  for (Index = 0; Index < Control->ItemCount; Index++) {
    DestroyWindow (Control->Items[Index]->Wnd);
  }

  FreePool (Control->Items);
  Control->Items     = NULL;
  Control->ItemCount = 0;

  UiNeedUpdatePos (Control);
  return TRUE;
}

UI_CONTROL *
EFIAPI
UiControlFindChildByName (
  UI_CONTROL                    *Control,
  CHAR16                        *Name
  )
{
  UINTN                         Index;
  UI_CONTROL                    *This;
  UI_CONTROL                    *Child;

  This = (UI_CONTROL *) Control;

  if (StrCmp (This->Name, Name) == 0) {
    return This;
  }
  for (Index = 0; Index < This->ItemCount; Index++) {
    Child = CONTROL_CLASS (This->Items[Index])->FindChildByName (This->Items[Index], Name);
    if (Child != NULL) {
      return Child;
    }
  }
  return NULL;
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

SIZE
UiControlEstimateSize (
  UI_CONTROL                    *Control,
  SIZE                          AvailableSize
  )
{
  SIZE                          Size;
  SIZE                          ItemSize;
  SIZE                          ItemAvailableSize;
  UINTN                         Index;
  UI_CONTROL                    *Item;
  UINTN                         Count;
  INTN                          MaxWidth;
  INTN                          MaxHeight;


  if (Control->FixedSize.cx >= 0 && Control->FixedSize.cy >= 0) {
    return Control->FixedSize;
  }

  Count = Control->ItemCount;

  ItemAvailableSize.cx = AvailableSize.cx - Control->Padding.left - Control->Padding.right;
  ItemAvailableSize.cy = AvailableSize.cy - Control->Padding.top  - Control->Padding.bottom;

  MaxWidth = 0;
  MaxHeight = 0;
  for (Index = 0; Index < Count; ++Index) {

    Item = Control->Items[Index];
    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }
    if (Item->Float) {
      continue;
    }

    ItemSize = CONTROL_CLASS (Item)->EstimateSize (Item, ItemAvailableSize);

    MaxWidth = MAX (MaxWidth, ItemSize.cx);
    MaxHeight = MAX (MaxHeight, ItemSize.cy);
  }

  MaxWidth = MAX (MaxWidth, Control->MinSize.cx);
  MaxHeight = MAX (MaxHeight, Control->MinSize.cy);

  Size = Control->FixedSize;
  if (Size.cx == WRAP_CONTENT) {
    Size.cx = (GDICOORD)MaxWidth;
  }

  if (Size.cy == WRAP_CONTENT) {
    Size.cy = (GDICOORD)MaxHeight;
  }

  return Size;
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

  if (CONTROL_CLASS_GET_STATE(Control) & UISTATE_INVALIDATED) {
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

  CONTROL_CLASS_SET_STATE(Control, UISTATE_INVALIDATED, 0);
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
  UI_CONTROL   *Root;
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

  Root = GetUiControl (GetParent (Control->Wnd));
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
    if (!IsWindowVisible (Item->Wnd)) {
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
      if (ItemRc.top >= FocusRc.top || ItemRc.right >= FocusRc.left || ItemRc.left >= FocusRc.right) {
        continue;
      }
    } else if (Direction == VK_RIGHT) {
      if (ItemRc.left <= FocusRc.left) {
        continue;
      }
    } else if (Direction == VK_DOWN) {
      if (ItemRc.top <= FocusRc.top || ItemRc.right >= FocusRc.left || ItemRc.left >= FocusRc.right) {
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
  UI_CONTROL                    *Control
  )
{
  if (Control->VScrollBar != NULL || Control->HScrollBar != NULL) {
    return TRUE;
  }
  return FALSE;
}


BOOLEAN
UiControlIsInScrollControl (
  UI_CONTROL *This
  )
{
  UI_CONTROL *Parent;

  Parent = CONTROL_CLASS (This)->GetParent (This);

  while (Parent != NULL) {
    if (CONTROL_CLASS (Parent)->ShouldDelayChildPressedState (Parent)) {
      return TRUE;
    }
    Parent = CONTROL_CLASS (Parent)->GetParent (Parent);
  }

  return FALSE;
}


LRESULT
UiControlProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_CONTROL                    *This;
  UI_CONTROL                    *ParentControl;
  UI_CONTROL                    *NextFocus;
  HWND                           ParentWnd;

  RECT                          Rc;
  UI_MANAGER                    *Manager;

  POINT                         Point;
  POINT                         LastPoint;
  INT32                         DeltaX;
  INT32                         DeltaY;
  INT32                         ScrollPos;

  This = NULL;
  Manager = NULL;

  if (Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    This = (UI_CONTROL *) GetWindowLongPtr (Wnd, 0);
    if (This == NULL) {
      ASSERT (FALSE);
      return 0;
    }
  }

  if (This != NULL) {
    Manager = This->Manager;
  }

  switch (Msg) {

  case WM_CREATE:
    if (This == NULL) {
      This     = (UI_CONTROL *) AllocateZeroPool (sizeof (UI_CONTROL));
    } else {
      ASSERT (FALSE);
    }
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    if (LParam == 0) {
      Manager = NULL;
    } else {
      Manager = (UI_MANAGER *)(((CREATESTRUCT *)(LParam))->lpCreateParams);
    }
    UiControlOnUiCreate (This, Wnd, Manager);
    break;

  case UI_NOTIFY_ACTIVATE:
    return UiControlActivate (This);

  case WM_DESTROY:
    UiControlOnDestroy (This);
    break;

  case WM_WINDOWPOSCHANGED:
    break;

  case WM_PAINT:
//    ValidateRect (Wnd, NULL);
    break;

  case WM_NCHITTEST:
    return HTCLIENT;
//  return HTTRANSPARENT;
    break;

  case UI_NOTIFY_PAINT:
    UiControlOnUiPaint (This, WParam, LParam);
    break;

  case WM_KEYDOWN:
    if (WParam == VK_RETURN) {
      if (!IsWindowVisible (Wnd)) {
        break;
      }

      if (!(This->StyleFlags & UISTYLE_CLICKABLE)) {
        break;
      }

      CONTROL_CLASS_SET_STATE(This, UISTATE_PRESSED, 0);
      This->Class->Invalidate (This);
    }

  //
  // we didn't send WM_KEYUP, so continue process WM_KEYUP
  //
  //
  case WM_KEYUP:
    if (WParam == VK_LEFT || WParam == VK_RIGHT
        || WParam == VK_UP || WParam == VK_DOWN) {

      NextFocus = This->Class->FindNextFocus (This, This, WParam);
      if (NextFocus != NULL) {
        SetFocus (NextFocus->Wnd);
      }
      break;
    }


    if (WParam == VK_RETURN) {
      if (!IsWindowVisible (Wnd)) {
        break;
      }

      if (!(This->StyleFlags & UISTYLE_CLICKABLE)) {
        break;
      }

      if (!(CONTROL_CLASS_GET_STATE (This) & UISTATE_PRESSED)) {
        break;
      }

      CONTROL_CLASS_SET_STATE(This, 0, UISTATE_PRESSED);
      This->Class->Invalidate (This);
      SendMessage (Wnd, UI_NOTIFY_ACTIVATE, 0, 0);
    }
    break;

  case WM_LBUTTONDOWN:
    if ((This->HScrollBar != NULL) || (This->VScrollBar != NULL)) {
      //
      // orignal Container
      //
      This->IsBeginDraged = TRUE;
      POINTSTOPOINT(Point, LParam);
      ClientToScreen (Wnd, &Point);
      This->LastMotionX = Point.x;
      This->LastMotionY = Point.y;
      SetCapture (Wnd);
      if ((GetWindowLongPtr (Wnd, GWL_STYLE) & WS_DISABLED) == 0) {
        CONTROL_CLASS_SET_STATE(This, UISTATE_PRESSED | UISTATE_CAPTURED, 0);
        This->Class->Invalidate (This);
      }

    } else {
      POINTSTOPOINT(Point, LParam);
      ClientToScreen (Wnd, &Point);
      if ((GetWindowLongPtr (Wnd, GWL_STYLE) & WS_DISABLED) == 0) {
        SetCapture (Wnd);
        CONTROL_CLASS_SET_STATE(This, UISTATE_PRESSED | UISTATE_CAPTURED, 0);
        This->Class->Invalidate (This);
      }

      if (UiControlIsInScrollControl (This)) {
        CONTROL_CLASS_SET_STATE(This, UISTATE_PREPRESSED, 0);
        This->Class->Invalidate (This);
        This->LastX = Point.x;
        This->LastY = Point.y;
      }
    }
    break;

  case WM_LBUTTONDBLCLK:
    return SendMessage (Wnd, WM_LBUTTONDOWN, WParam, LParam);

  case WM_NCMOUSEMOVE:

    if (This->IsBeginDraged) {
      POINTSTOPOINT(Point, LParam);
      DeltaX = This->LastMotionX - Point.x;
      DeltaY = This->LastMotionY - Point.y;

      if (abs(DeltaY) < This->TouchSlop) {
        break;
      }

      This->LastMotionX = Point.x;
      This->LastMotionY = Point.y;
      //
      // now only process y
      //


      if (DeltaY < 0) {
        if (This->ScrollPos.y <= 0) {
          break;
        }
      } else if (DeltaY > 0) {
  //     DeltaY =
      }

      if (This->VScrollBar != NULL) {
        ScrollPos = (This->ScrollPos.y + DeltaY) / This->VScrollBar->LineSize;
        SCROLLBAR_CLASS(This->VScrollBar)->SetScrollPos (This->VScrollBar, ScrollPos);
        This->Dragged = TRUE;
        //SCROLLBAR_CLASS(This->VScrollBar)->SetScrollPos (This->VScrollBar, This->ScrollPos.y + DeltaY);
      }
    } else if (CONTROL_CLASS_GET_STATE (This) & UISTATE_PREPRESSED) {
      POINTSTOPOINT(Point, LParam);
      GetWindowRect (Wnd, &Rc);
      if (!PtInRect (&Rc, Point) || abs(This->LastY - Point.y) > 10) {
        //
        // change capture to parent
        //
        CONTROL_CLASS_SET_STATE (This, 0, UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
        This->Class->Invalidate (This);
        ParentControl = This->Class->GetParent (This);
        ASSERT (ParentControl != NULL);

        ParentWnd = ParentControl->Wnd;
        SetCapture (ParentWnd);
        LastPoint.x = This->LastX;
        LastPoint.y = This->LastY;
        ScreenToClient (ParentWnd, &LastPoint);
        SendMessage (ParentWnd, WM_LBUTTONDOWN, 0, MAKELONG(LastPoint.x, LastPoint.y));
        SendMessage (ParentWnd, WM_NCMOUSEMOVE, 0, MAKELONG(Point.x, Point.y));
      }
    }
    break;

  case WM_NCLBUTTONUP:
    break;

  case WM_MOUSELEAVE:
  case WM_LBUTTONUP:
    This->Dragged = FALSE;
    if (This->IsBeginDraged) {
      ReleaseCapture ();
      This->IsBeginDraged = FALSE;
    } else if (GetCapture () == Wnd) {
      CONTROL_CLASS_SET_STATE (This, 0, UISTATE_PRESSED | UISTATE_CAPTURED | UISTATE_PREPRESSED);
      This->Class->Invalidate (This);
      ReleaseCapture ();
    }
    break;

  case WM_SETFOCUS:
    DEBUG ((EFI_D_INFO, "focuse from (%s) to (%s)\n", WParam ? ((HWND)WParam)->pClass->szClassName : L"NULL", Wnd->pClass->szClassName));
    CONTROL_CLASS_SET_STATE (This, UISTATE_FOCUSED, 0);
    This->Class->Invalidate (This);
    break;

  case WM_KILLFOCUS:
    CONTROL_CLASS_SET_STATE (This, 0, UISTATE_FOCUSED);
    This->Class->Invalidate (This);
    break;

  case WM_ERASEBKGND:
    return 1;

  default:
    return DefWindowProc (Wnd, Msg, WParam, LParam);
  }

  return 0;

}


UI_CONTROL_CLASS *
GetControlClass (
  VOID
  )
{
  STATIC UI_CONTROL_CLASS       *Class = NULL;

  if (Class != NULL) {
    return Class;
  }

  Class                  = AllocateZeroPool (sizeof (UI_CONTROL_CLASS));
  if (Class == NULL) {
    return NULL;
  }

  Class->Parent          = NULL;
  Class->ClassSize       = sizeof (*Class);
  Class->ClassName       = StrDuplicate (L"Control");

  Class->FindChildByName = UiControlFindChildByName;
  Class->AddChild        = UiControlAddChild;
  Class->RemoveChild     = UiControlRemoveChild;
  Class->RemoveAllChild  = UiControlRemoveAllChild;

  Class->WndProc         = UiControlProc;
  Class->SetState        = UiControlSetState;
  Class->GetState        = UiControlGetState;
  Class->SetAttribute    = UiControlSetAttribute;
  Class->SetPosition     = UiControlSetPosition;
  Class->EstimateSize    = UiControlEstimateSize;
  Class->Invalidate      = UiControlInvalidate;
  Class->FindNextFocus   = UiControlFindNextFocus;
  Class->GetParent       = UiControlGetParent;
  Class->ShouldDelayChildPressedState = UiControlShouldDelayChildPressedState;


  return Class;
}


