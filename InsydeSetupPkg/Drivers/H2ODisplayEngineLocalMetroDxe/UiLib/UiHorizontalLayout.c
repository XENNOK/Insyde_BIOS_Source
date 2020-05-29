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

STATIC UI_HORIZONTAL_LAYOUT_CLASS   *mHorizontalLayoutClass = NULL;
#define CURRENT_CLASS               mHorizontalLayoutClass

SIZE
UiHorizontalEstimateSize (
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
  INTN                          MaxHeight;
  INTN                          Width;


  if (Control->FixedSize.cx >= 0 && Control->FixedSize.cy >= 0) {
    return Control->FixedSize;
  }

  Count = Control->ItemCount;

  ItemAvailableSize.cx = AvailableSize.cx - Control->Padding.left - Control->Padding.right;
  ItemAvailableSize.cy = AvailableSize.cy - Control->Padding.top  - Control->Padding.bottom;

  MaxHeight = 0;
  Width = 0;
  for (Index = 0; Index < Count; ++Index) {

    Item = Control->Items[Index];
    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }
    if (Item->Float) {
      continue;
    }

    ItemSize = CONTROL_CLASS (Item)->EstimateSize (Item, ItemAvailableSize);

    MaxHeight = MAX (MaxHeight, ItemSize.cy);
    if (ItemSize.cx >= 0) {
      Width += ItemSize.cx;
    }
  }

  Width     += Control->Padding.left + Control->Padding.right;
  MaxHeight += Control->Padding.top  + Control->Padding.bottom;

  Width     = MAX (Width, Control->MinSize.cx);
  MaxHeight = MAX (MaxHeight, Control->MinSize.cy);

  Size = Control->FixedSize;
  if (Size.cy == WRAP_CONTENT) {
    Size.cy = (GDICOORD)MaxHeight;
  }

  if (Size.cx == WRAP_CONTENT) {
    Size.cx = (GDICOORD)Width;
  }

  return Size;
}


VOID
UiHorizontalLayoutSetPosition (
  UI_CONTROL                   *Control,
  CONST RECT                   *Pos
  )
{
  UI_HORIZONTAL_LAYOUT *This;
  UI_CONTROL   *Item;
  UINTN  Index;
  RECT   Rc;
  RECT   ItemRc;
  SIZE   Size;
  UINT32       AjustableCount;
  UINT32       Fixed;
  UINT32       EstimateNum;
  UINT32       Remain;
  UINT32       Left;
  UINT32       RemainFixed;
  UINT32       Expand;
  SIZE   Available;
  SIZE   ItemSize;


  Size.cx  = 0;
  Size.cy  = 0;
  Expand   = 0;

  This = (UI_HORIZONTAL_LAYOUT *) Control;

  ShowSetPositionDbgInfo (Control, Pos);
  GetWindowRect(Control->Wnd, &Rc);
  MapWindowPoints(HWND_DESKTOP, GetParent(Control->Wnd), (LPPOINT) &Rc, 2);
  if (!EqualRect (&Rc, Pos)) {
    ShowSetPositionDbgInfo (Control, Pos);
    MoveWindow (Control->Wnd, Pos->left, Pos->top, Pos->right - Pos->left, Pos->bottom - Pos->top, TRUE);
    CONTROL_CLASS_INVALIDATE (This);
    Control->NeedUpdateChildPos = TRUE;
  }
  if (Control->ItemCount == 0) {
    return ;
  }

  if (!Control->NeedUpdateChildPos) {
    return ;
  }
  Control->NeedUpdateChildPos = FALSE;


  CopyRect (&Rc, Pos);
  OffsetRect (&Rc, -Rc.left, -Rc.top);

  Rc.left   += Control->Padding.left;
  Rc.top    += Control->Padding.top;
  Rc.right  -= Control->Padding.right;
  Rc.bottom -= Control->Padding.bottom;

  if (Control->VScrollBar != NULL ) {
    Rc.right -= Control->VScrollBar->Control.FixedSize.cx;
  }

  if (Control->HScrollBar != NULL) {
    Rc.bottom -= Control->HScrollBar->Control.FixedSize.cy;
  }

  if (Rc.left > Rc.right) {
    Rc.left = Rc.right;
  }
  if (Rc.top > Rc.bottom) {
    Rc.top = Rc.bottom;
  }

  Available.cx = Rc.right - Rc.left;
  Available.cy = Rc.bottom - Rc.top;

  AjustableCount = 0;
  Fixed          = 0;
  EstimateNum    = 0;

  for (Index = 0; Index < Control->ItemCount; Index++) {

    Item = Control->Items[Index];
    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    if (Item->Float) {
      continue;
    }

    Size = CONTROL_CLASS (Item)->EstimateSize (Item, Available);
    if (Size.cx == 0) {
      AjustableCount++;
    }

    if (Size.cx == MATCH_PARENT) {
      continue;
    }

    Fixed += Size.cx;
    EstimateNum ++;
  }
  Fixed += (EstimateNum - 1) * Control->ChildPadding;
  if (AjustableCount > 0) {
    Expand = MAX (0, (INT32)(Available.cx - Fixed) / (INT32)AjustableCount);
  }

  Left = Rc.left;
  Left -= Control->ScrollPos.y;
  Remain = 0;
  RemainFixed = Fixed;
  for (Index = 0; Index < Control->ItemCount; Index++) {

    Item = Control->Items[Index];

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    if (Item->Float) {
      Size = CONTROL_CLASS (Item)->EstimateSize (Item, Available);

      if (Size.cx == 0 || Size.cx == MATCH_PARENT) {
        Size.cx = Available.cx;
      }

      if (Size.cy == 0 || Size.cy == MATCH_PARENT) {
        Size.cy = Available.cy;
      }

      SetRect (
        &ItemRc,
        Item->FixedXY.x,
        Item->FixedXY.y,
        Item->FixedXY.x + Size.cx,
        Item->FixedXY.y + Size.cy
        );
      mSetPositionLevel++;
      CONTROL_CLASS (Item)->SetPosition (Item, &ItemRc);
      mSetPositionLevel--;
      continue;
    }

    Size = CONTROL_CLASS (Item)->EstimateSize (Item, Available);

    if (Size.cx == 0) {
      ItemSize.cx = Expand;
    } else if (Size.cx == MATCH_PARENT) {
      ItemSize.cx = RemainFixed;
      RemainFixed = 0;
    } else {
      ItemSize.cx = Size.cx;
      RemainFixed -= Size.cx;
    }


    if ((Size.cy == 0) || (Size.cy == MATCH_PARENT)) {
      ItemSize.cy = Available.cy;
    } else {
      ItemSize.cy = Size.cy;
    }

    ItemRc.left   = Left;
    ItemRc.top    = Rc.top;
    ItemRc.right  = ItemRc.left + ItemSize.cx;
    ItemRc.bottom = ItemRc.top  + ItemSize.cy;

    mSetPositionLevel++;
    CONTROL_CLASS (Item)->SetPosition (Item, &ItemRc);
    mSetPositionLevel--;
    Left = ItemRc.right + Control->ChildPadding;

  }


  //
  // process scroll bar
  //
  if (Control->HScrollBar != NULL) {
    SetWindowPos (
      Control->HScrollBar->Control.Wnd,
      HWND_TOP,
      0, Rc.bottom, Rc.right - Rc.left, Control->HScrollBar->Control.FixedSize.cy, 0);
  }

  if (Control->VScrollBar != NULL) {
    SetWindowPos (
      Control->VScrollBar->Control.Wnd,
      HWND_TOP,
      Rc.right, 0, Control->VScrollBar->Control.FixedSize.cx, Rc.bottom, 0);
  }
}

LRESULT
EFIAPI
UiHorizontalLayoutProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_HORIZONTAL_LAYOUT          *This;
  UI_CONTROL                    *Control;

  This = (UI_HORIZONTAL_LAYOUT *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_HORIZONTAL_LAYOUT *) AllocateZeroPool (sizeof (UI_HORIZONTAL_LAYOUT));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);

    break;

  case WM_NCHITTEST:
    if (Control->VScrollBar != NULL) {
      return HTCLIENT;
    }
    return HTTRANSPARENT;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;

}


UI_HORIZONTAL_LAYOUT_CLASS *
EFIAPI
GetHorizontalLayoutClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"HorizontalLayout", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiHorizontalLayoutProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetPosition  = UiHorizontalLayoutSetPosition;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->EstimateSize = UiHorizontalEstimateSize;
  return CURRENT_CLASS;
}

