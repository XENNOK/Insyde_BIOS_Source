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

STATIC UI_LIST_VIEW_CLASS           *mListViewClass = NULL;
#define CURRENT_CLASS               mListViewClass
#define LIST_VIEW_TIMER_ID 1

BOOLEAN
IsPressingSortIcon (
  IN UI_CONTROL                 *Item,
  IN POINT                      *Pt
  )
{
  UI_CONTROL                    *ChildControl;
  RECT                          Rc;

  ChildControl = UiFindChildByName (Item, L"SortIcon");
  if (ChildControl == NULL) {
    return FALSE;
  }

  GetWindowRect (ChildControl->Wnd, &Rc);
  MapWindowPoints (HWND_DESKTOP, GetParent(Item->Wnd), (LPPOINT) &Rc, 2);
  if (PtInRect (&Rc, *Pt)) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
IsDraggingItem (
  IN UI_LIST_VIEW               *This
  )
{
  if (This->MoveItemSupport && !IsRectEmpty (&This->MoveItemOrgRc)) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
NeedSwitchItem (
  IN UI_LIST_VIEW               *This
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;
  INT32                         Limit;
  INT32                         Offset;
  BOOLEAN                       Result;

  Control = (UI_CONTROL *) This;
  Result  = FALSE;
  Item    = Control->Items[This->CurSel];
  GetWindowRect (Item->Wnd, &ItemRc);

  switch (This->Layout) {

  case UiListViewVerticalLayout:
    Limit  = (ItemRc.bottom - ItemRc.top) / 2;
    Offset = ItemRc.top - This->MoveItemOrgRc.top;
    if (abs (Offset) > Limit) {
      Result  = TRUE;
    }
    break;

  case UiListViewHorizontalLayout:
    Limit  = (ItemRc.right - ItemRc.left) / 2;
    Offset = ItemRc.left - This->MoveItemOrgRc.left;
    if (abs (Offset) > Limit) {
      Result  = TRUE;
    }
    break;

  default:
    break;
  }

  return Result;
}

VOID
RestoreDraggingItemPos (
  IN UI_LIST_VIEW               *This
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;

  Control = (UI_CONTROL *) This;

  Item = Control->Items[This->CurSel];
  CopyRect (&ItemRc, &This->MoveItemOrgRc);
  MapWindowPoints (HWND_DESKTOP, GetParent(Item->Wnd), (LPPOINT) &ItemRc, 2);

  CONTROL_CLASS (Item)->SetPosition (Item, &ItemRc);
}

INT32
GetIntensityOfRepeatMoveToNext (
  IN UI_LIST_VIEW               *This,
  IN POINT                      Pt,
  IN BOOLEAN                    GoUp
  )
{
  UI_CONTROL                    *Control;
  RECT                          ListViewRc;
  INT32                         Difference;
  INT32                         Intensity;

  //
  // Intensity is the length between point and ListView region.
  //
  Control = (UI_CONTROL *) This;
  GetClientRect (GetParent (Control->Items[This->CurSel]->Wnd), &ListViewRc);

  Difference = GoUp ? (Pt.y - ListViewRc.top) : (Pt.y - ListViewRc.bottom + 1);

  Intensity  = 90 / Difference;
  if (Intensity == 0) {
    Intensity = GoUp ? -1 : 1;
  }

  return Intensity;
}

VOID
UiListViewOnUiCreate (
  UI_CONTROL                    *Control
  )
{
  UI_LIST_VIEW                  *This;

  This = (UI_LIST_VIEW *) Control;

  This->CurSel = -1;
  SetRectEmpty (&This->MoveItemOrgRc);
}

BOOLEAN
UiListViewSetAttribute (
  UI_CONTROL                    *Control,
  CHAR16                        *Name,
  CHAR16                        *Value
  )
{
  UI_LIST_VIEW                  *This;
  EFI_STATUS                    Status;

  This = (UI_LIST_VIEW *) Control;

  if (StrCmp (Name, L"layout") == 0) {
    if (StrCmp (Value, L"cellspnanning") == 0) {
      This->Layout = UiListViewCellSpanningLayout;
    } else {
      This->Layout = UiListViewVerticalLayout;
    }
  } else if (StrCmp (Name, L"cellwidth") == 0) {
    This->CellWidth  = (UINT32) StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"cellheight") == 0) {
    This->CellHeight = (UINT32) StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"moveitemsupport") == 0) {
    This->MoveItemSupport = (StrCmp (Value, L"true") == 0) ? TRUE : FALSE;
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

VOID
UiListViewSetPosition (
  UI_CONTROL                   *Control,
  CONST RECT                   *Pos
  )
{
  UI_LIST_VIEW                  *This;
  INT32                         X;
  INT32                         Y;
//INT32                         MaxX;
//INT32                         MaxY;
  RECT                          Rc;
  SIZE                          Available;
  SIZE                          ItemSize;
  UINT32                        Index;
  UI_CONTROL                    *Item;
  SIZE                          Size;
  RECT                          ItemRc;

  This = (UI_LIST_VIEW *) Control;
  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetPosition (Control, Pos);

  if (IsDraggingItem (This)) {
    GetWindowRect (Control->Items[This->CurSel]->Wnd, &Rc);
    CopyRect (&This->MoveItemOrgRc, &Rc);
  }

  if (This->Layout != UiListViewCellSpanningLayout) {
    return ;
  }

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

  if (Rc.left > Rc.right) {
    Rc.left = Rc.right;
  }
  if (Rc.top > Rc.bottom) {
    Rc.top = Rc.bottom;
  }

  Available.cx = Rc.right - Rc.left;
  Available.cy = Rc.bottom - Rc.top;

  X = Rc.left;
  Y = Rc.top;

  for (Index = 0; Index < Control->ItemCount; Index++) {
    Item = Control->Items[Index];
    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    if (Item->Float) {
      continue;
    }

    Size = Item->Class->EstimateSize (Item, Available);
    if (Size.cx == 0 || Size.cy == 0) {
      continue;
    }

    ItemSize.cx = 0;
    ItemSize.cy = 0;

    if (Size.cy > 0) {
      ItemSize.cy = Size.cy;
    }
    if (Size.cx > 0) {
      ItemSize.cx = Size.cx;
    }

    if (((X - Rc.left) + ItemSize.cx) > Available.cx) {
      Y += ItemSize.cy + This->CellHeight;
      X = Rc.left;
    }

    ItemRc.left   = X;
    ItemRc.top    = Y;
    ItemRc.right  = ItemRc.left + ItemSize.cx;
    ItemRc.bottom = ItemRc.top  + ItemSize.cy;

    mSetPositionLevel++;
    CONTROL_CLASS (Item)->SetPosition (Item, &ItemRc);
    mSetPositionLevel--;
    X = ItemRc.right + This->CellWidth;
  }
}

VOID
UiListViewEnsureChildVisible (
  UI_LIST_VIEW                  *This,
  UINT32                        Index
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Child;
  RECT                          ChildRc;
  RECT                          ListRc;

  Control = (UI_CONTROL *) This;
  ASSERT (Control != NULL);
  ASSERT (This->CurSel >= 0 && This->CurSel < (INT32) Control->ItemCount);


  Child = Control->Items[This->CurSel];
  Control->Manager->GetControlRect (Control->Manager, Child, &ChildRc);
  Control->Manager->GetControlRect (Control->Manager, Control, &ListRc);

  DEBUG ((EFI_D_INFO, "Select %d %d %d %d %d %d\n",
                        This->CurSel,
                        Control->ScrollPos.y,
                        ChildRc.left,
                        ChildRc.top,
                        ChildRc.right,
                        ChildRc.bottom
                        ));

  if (Control->VScrollBar != NULL) {
    INT32 dy;
    INT32 Pos;

    if (ChildRc.top >= ListRc.top && ChildRc.bottom < ListRc.bottom) {
      return ;
    }

    dy = 0;
    if (ChildRc.top < ListRc.top) {
      dy = ChildRc.top - ListRc.top;
    }

    if (ChildRc.bottom > ListRc.bottom) {
      dy = ChildRc.bottom - ListRc.bottom;
    }

    Pos = (Control->ScrollPos.y + dy) / Control->VScrollBar->LineSize;
    if (Pos != Control->ScrollPos.y) {
      SCROLLBAR_CLASS(Control->VScrollBar)->SetScrollPos (Control->VScrollBar, Pos);
    }
  }
}

BOOLEAN
UiListViewSetSelection (
  UI_LIST_VIEW                  *This,
  INT32                         Index,
  BOOLEAN                       Forward
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Child;
  INT32                         I;

  Control = (UI_CONTROL *) This;

  if (Control->ItemCount == 0) {
    This->CurSel = -1;
    CONTROL_CLASS_INVALIDATE (This);
    return TRUE;
  }

  if (Index >= (INT32) Control->ItemCount) {
    return TRUE;
  }

  if (Index < 0) {
    //
    // Earse current selection
    //
    if (This->CurSel >= 0 && This->CurSel < (INT32) Control->ItemCount) {
      Child = Control->Items[This->CurSel];
      CONTROL_CLASS_SET_STATE (Child, 0, UISTATE_SELECTED | UISTATE_FOCUSED);
    }
    This->CurSel = -1;
    CONTROL_CLASS_INVALIDATE (This);
    return TRUE;
  }

  //
  // check item is selectable
  //
  if (Forward) {
    for (I = Index; I < (INT32)Control->ItemCount; I++) {
      if (!(CONTROL_CLASS_GET_STATE (Control->Items[I]) & UISTATE_DISABLED)) {
        Index = I;
        break;
      }
    }
    if (I == (INT32)Control->ItemCount) {
      return TRUE;
    }
  } else {
    for (I = Index; I >= 0; --I) {
      if (!(CONTROL_CLASS_GET_STATE (Control->Items[I]) & UISTATE_DISABLED)) {
        Index = I;
        break;
      }
    }
    if (I < 0) {
      return TRUE;
    }
  }

  if (This->CurSel >= 0 && This->CurSel < (INT32) Control->ItemCount) {
    Child = Control->Items[This->CurSel];
    CONTROL_CLASS_SET_STATE (Child, 0, UISTATE_SELECTED | UISTATE_FOCUSED);
  }

  This->CurSel = Index;
  Child = Control->Items[Index];
  CONTROL_CLASS_SET_STATE (Child, UISTATE_SELECTED, 0);
  if (CONTROL_CLASS_GET_STATE (This) & UISTATE_FOCUSED) {
    CONTROL_CLASS_SET_STATE (Child, UISTATE_FOCUSED, 0);
  }

  if (This->OnItemSelected != NULL) {
    This->OnItemSelected (This, Child, (UINT32) Index);
  }

  //
  // EnsureChildVisible
  //
  UiListViewEnsureChildVisible (This, (UINT32) Index);


  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

INT32
UiListViewGetSelection (
  UI_LIST_VIEW                  *This
  )
{
  return This->CurSel;
}

VOID
EFIAPI
UiListViewSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  UI_LIST_VIEW                  *This;
  UI_CONTROL                    *Child;

  This = (UI_LIST_VIEW *) Control;

  if (This->CurSel >= 0 && This->CurSel < (INT32)Control->ItemCount) {
    Child = Control->Items[This->CurSel];
    if (SetState & UISTATE_FOCUSED) {
      CONTROL_CLASS_SET_STATE (Child, UISTATE_FOCUSED, 0);
    }

    if (ClearState & UISTATE_FOCUSED) {
      CONTROL_CLASS_SET_STATE (Child, 0, UISTATE_FOCUSED);
    }
  }

  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetState (Control, SetState, ClearState);
}



BOOLEAN
UiListViewPerformItemClick (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  )
{
  LIST_VIEW_CLASS (This)->SetSelection (This, Index, TRUE);
  if (This->OnItemClick != NULL) {
    This->OnItemClick (This, Item, Index);
  }

  return TRUE;
}

INT32
UiListViewFindNextSelection (
  UI_LIST_VIEW                  *This,
  INT32                         SelectedIndex,
  UINTN                         Direction
  )
{
  UI_CONTROL                    *Control;
  UINTN                         Index;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;
  RECT                          FocusRc;
  INT32                         NextFocus;
  UINT32                        Distance;
  UINT32                        BestDistance;
  UI_CONTROL                    *Selected;


  ASSERT (Direction == VK_LEFT || Direction == VK_RIGHT || Direction == VK_UP || Direction == VK_DOWN);

  Control = (UI_CONTROL *) This;

  if (SelectedIndex < 0 || SelectedIndex >= (INT32) Control->ItemCount) {
    return -1;
  }

  Selected = Control->Items[SelectedIndex];
  GetWindowRect (Selected->Wnd, &FocusRc);

  //
  // search next focus
  //
  NextFocus = -1;
  BestDistance = (UINT32)-1;
  for (Index = 0; Index < Control->ItemCount; Index++) {
    Item = Control->Items[Index];
    if (Item == Selected) {
      continue;
    }
    if (CONTROL_CLASS_GET_STATE (Item) & UISTATE_DISABLED) {
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
      NextFocus    = (INT32)Index;
      BestDistance = Distance;
    }
  }

  return NextFocus;
}

EFI_STATUS
UiListViewSwitchItems (
  UI_LIST_VIEW                  *This,
  INT32                         Index1,
  INT32                         Index2
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Item1;
  UI_CONTROL                    *Item2;
  RECT                          Rect1;
  RECT                          Rect2;

  if (This == NULL || Index1 == Index2) {
    return EFI_INVALID_PARAMETER;
  }

  Control = (UI_CONTROL *) This;

  if (Index1 < 0 || Index1 >= (INT32) Control->ItemCount ||
      Index2 < 0 || Index2 >= (INT32) Control->ItemCount) {
    return EFI_INVALID_PARAMETER;
  }

  Item1 = Control->Items[Index1];
  Item2 = Control->Items[Index2];

  GetWindowRect (Item1->Wnd, &Rect1);
  MapWindowPoints (HWND_DESKTOP, GetParent(Item1->Wnd), (LPPOINT) &Rect1, 2);
  GetWindowRect (Item2->Wnd, &Rect2);
  MapWindowPoints (HWND_DESKTOP, GetParent(Item2->Wnd), (LPPOINT) &Rect2, 2);

  CONTROL_CLASS (Item1)->SetPosition (Item1, &Rect2);
  CONTROL_CLASS (Item2)->SetPosition (Item2, &Rect1);

  Control->Items[Index1] = Item2;
  Control->Items[Index2] = Item1;

  return EFI_SUCCESS;
}

EFI_STATUS
UiListViewCurSelMoveToNext (
  IN UI_LIST_VIEW               *This,
  IN BOOLEAN                    GoUp
  )
{
  UI_CONTROL                    *Control;
  INT32                         SelectedIdx;
  EFI_STATUS                    Status;
  UI_CONTROL                    *Item;

  Control = (UI_CONTROL *) This;

  SelectedIdx = LIST_VIEW_CLASS (Control)->FindNextSelection (This, This->CurSel, GoUp ? VK_UP : VK_DOWN);

  if (SelectedIdx < 0 || SelectedIdx >= (INT32) Control->ItemCount) {
    return EFI_NOT_FOUND;
  }

  if (!IsRectEmpty (&This->MoveItemOrgRc)) {
    RestoreDraggingItemPos (This);
  }

  Status = LIST_VIEW_CLASS (Control)->SwitchItems (This, This->CurSel, SelectedIdx);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LIST_VIEW_CLASS (This)->SetSelection (This, SelectedIdx, TRUE);

  if (!IsRectEmpty (&This->MoveItemOrgRc)) {
    Item = Control->Items[This->CurSel];
    GetWindowRect (Item->Wnd, &This->MoveItemOrgRc);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return Status;
}

VOID
UiListViewDraggingItemProc (
  UI_CONTROL                    *Control,
  POINT                         Pt
  )
{
  UI_LIST_VIEW                  *This;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;
  RECT                          ListViewRc;
  RECT                          OffsetRc;
  INT32                         DifferenceY;
  BOOLEAN                       GoUp;

  This = (UI_LIST_VIEW *) Control;

  Item = Control->Items[This->CurSel];
  GetWindowRect (Item->Wnd, &ItemRc);
  MapWindowPoints (HWND_DESKTOP, GetParent(Item->Wnd), (LPPOINT) &ItemRc, 2);

  if (This->IntensityOfRepeatMoveToNext != 0) {
    //
    // In repeatly moving to next item mode.
    //
    GoUp = (This->IntensityOfRepeatMoveToNext < 0) ? TRUE : FALSE;
    if ((GoUp && Pt.y >= ItemRc.top) || (!GoUp && Pt.y <= ItemRc.bottom)) {
      This->IntensityOfRepeatMoveToNext = 0;
      KillTimer (Control->Wnd, LIST_VIEW_TIMER_ID);
      return;
    }

    This->IntensityOfRepeatMoveToNext = GetIntensityOfRepeatMoveToNext (This, Pt, GoUp);
    return;
  }

  if (!PtInRect (&ItemRc, This->MoveItemOrgPt)) {
    //
    // Just return when start dragging point is not on the selected item
    //
    CopyMem (&This->MoveItemOrgPt, &Pt, sizeof (POINT));
    return;
  }

  GetClientRect (GetParent (Item->Wnd), &ListViewRc);
  DifferenceY = Pt.y - This->MoveItemOrgPt.y;
  GoUp = (DifferenceY < 0) ? TRUE : FALSE;

  if (This->Layout == UiListViewVerticalLayout && DifferenceY != 0) {
    CopyRect (&OffsetRc, &ItemRc);
    OffsetRect (&OffsetRc, 0, DifferenceY);

    if ((GoUp && OffsetRc.top < ListViewRc.top) || (!GoUp && OffsetRc.bottom > ListViewRc.bottom)) {
      if (ItemRc.top > ListViewRc.top && ItemRc.bottom < ListViewRc.bottom) {
        DifferenceY = GoUp ? (ListViewRc.top - ItemRc.top) : (ListViewRc.bottom - ItemRc.bottom);
        CopyRect (&OffsetRc, &ItemRc);
        OffsetRect (&OffsetRc, 0, DifferenceY);
      }

      if (!PtInRect (&ListViewRc, Pt)) {
        This->IntensityOfRepeatMoveToNext = GetIntensityOfRepeatMoveToNext (This, Pt, GoUp);
        SetTimer (Control->Wnd, LIST_VIEW_TIMER_ID, 50, NULL);
        return;
      }
    }

    if (OffsetRc.top    >= ListViewRc.top &&
        OffsetRc.bottom <= ListViewRc.bottom) {
      CONTROL_CLASS (Item)->SetPosition (Item, &OffsetRc);

      if (NeedSwitchItem (This)) {
        UiListViewCurSelMoveToNext (This, (DifferenceY < 0));

        Item = Control->Items[This->CurSel];
        CONTROL_CLASS (Item)->SetPosition (Item, &OffsetRc);
      }
      CONTROL_CLASS_INVALIDATE (CONTROL_CLASS_GET_PARENT(This));
    }
  }

  CopyMem (&This->MoveItemOrgPt, &Pt, sizeof (POINT));
}

VOID
UiListViewMouseMove (
  UI_CONTROL                    *Control,
  UINTN                         Flags,
  POINT                         Pt
  )
{
  UI_LIST_VIEW                  *This;
  UINTN                         Index;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;

  This = (UI_LIST_VIEW *) Control;

  if (IsDraggingItem (This)) {
    UiListViewDraggingItemProc (Control, Pt);
  }

  if (Control->Dragged) {
    for (Index = 0; Index < Control->ItemCount; Index++) {
      Item = Control->Items[Index];
      CONTROL_CLASS_SET_STATE (Item, 0, UISTATE_PRESSED | UISTATE_CAPTURED);
    }
  }

  for (Index = 0; Index < Control->ItemCount; Index++) {

    Item = Control->Items[Index];
    GetWindowRect(Item->Wnd, &ItemRc);
    MapWindowPoints(HWND_DESKTOP, Control->Wnd, (LPPOINT) &ItemRc, 2);

    if (PtInRect (&ItemRc, Pt)) {
      CONTROL_CLASS_SET_STATE (Item, UISTATE_HOVER, 0);
    } else {
      if (CONTROL_CLASS_GET_STATE (Item) & UISTATE_HOVER) {
        CONTROL_CLASS_SET_STATE (Item, 0, UISTATE_HOVER);
      }
    }
  }

}

VOID
UiListViewLButtonDown (
  UI_CONTROL                    *Control,
  UINTN                         Flags,
  POINT                         Pt
  )
{
  UI_LIST_VIEW                  *This;
  UINTN                         Index;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;

  This = (UI_LIST_VIEW *) Control;

  for (Index = 0; Index < Control->ItemCount; Index++) {
    Item = Control->Items[Index];
    GetWindowRect(Item->Wnd, &ItemRc);
    MapWindowPoints(HWND_DESKTOP, Control->Wnd, (LPPOINT) &ItemRc, 2);

    if (PtInRect (&ItemRc, Pt)) {
      if (CONTROL_CLASS_GET_STATE (Control->Items[Index]) & UISTATE_DISABLED) {
        break;
      }

      CONTROL_CLASS_SET_STATE (Item, UISTATE_PRESSED | UISTATE_CAPTURED, 0);
      LIST_VIEW_CLASS (Control)->SetSelection ((UI_LIST_VIEW *)Control, (INT32)Index, TRUE);

      if (This->MoveItemSupport && IsPressingSortIcon (Item, &Pt)) {
        CopyRect (&This->MoveItemOrgRc, &Item->Wnd->winrect);
        CopyMem (&This->MoveItemOrgPt, &Pt, sizeof (POINT));
      }
      break;
    }
  }
}

VOID
UiListViewLButtonUp (
  UI_CONTROL                    *Control,
  UINTN                         Flags,
  POINT                         Pt
  )
{
  UI_LIST_VIEW                  *This;
  UINTN                         Index;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;

  This = (UI_LIST_VIEW *) Control;

  if (IsDraggingItem (This)) {
    RestoreDraggingItemPos (This);
    SetRectEmpty (&This->MoveItemOrgRc);
    if (This->IntensityOfRepeatMoveToNext != 0) {
      This->IntensityOfRepeatMoveToNext = 0;
      KillTimer (Control->Wnd, LIST_VIEW_TIMER_ID);
    }
    CONTROL_CLASS_INVALIDATE (CONTROL_CLASS_GET_PARENT(This));
  }

  if (!(CONTROL_CLASS_GET_STATE (Control) & UISTATE_CAPTURED)) {
    for (Index = 0; Index < Control->ItemCount; Index++) {
      Item = Control->Items[Index];
      CONTROL_CLASS_SET_STATE (Item, 0, UISTATE_PRESSED | UISTATE_CAPTURED);
    }
    return ;
  }

  for (Index = 0; Index < Control->ItemCount; Index++) {
    Item = Control->Items[Index];
    GetWindowRect(Item->Wnd, &ItemRc);
    MapWindowPoints(HWND_DESKTOP, Control->Wnd, (LPPOINT) &ItemRc, 2);

    if (CONTROL_CLASS_GET_STATE (Item) & (UISTATE_PRESSED | UISTATE_CAPTURED)) {
      if (PtInRect (&ItemRc, Pt)) {
        if (!(CONTROL_CLASS_GET_STATE (Control->Items[Index]) & UISTATE_DISABLED)) {
          LIST_VIEW_CLASS (Control)->PerformItemClick (This, Control->Items[Index], (UINT32)Index);
        }
      }
      CONTROL_CLASS_SET_STATE (Item, 0, UISTATE_PRESSED | UISTATE_CAPTURED);
    }
  }
}

LRESULT
UiListViewKeyDown (
  HWND                          Wnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_LIST_VIEW                  *This;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;
  INT32                         SelectedIdx;

  This    = (UI_LIST_VIEW *) GetWindowLongPtr (Wnd, 0);
  Control = (UI_CONTROL *)This;
  Manager = Control->Manager;

  switch (WParam) {

  case VK_RETURN:
    if (Control->ItemCount != 0 && This->CurSel >= 0 && This->CurSel < (INT32)Control->ItemCount) {
      LIST_VIEW_CLASS (This)->PerformItemClick (This, Control->Items[This->CurSel], (UINT32)This->CurSel);
    }
    if (This->MoveItemSupport) {
      LIST_VIEW_CLASS (This)->SetSelection (This, -1, TRUE);
      SendMessage (Manager->MainWnd, UI_NOTIFY_CARRIAGE_RETURN, (WPARAM) Control, 0);
      break;
    }
    break;

  case VK_UP:
  case VK_DOWN:
  case VK_LEFT:
  case VK_RIGHT:
    if (Control->ItemCount == 0) {
      break;
    }
    SelectedIdx = LIST_VIEW_CLASS (Control)->FindNextSelection (This, This->CurSel, WParam);
    if (SelectedIdx < 0) {
      return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    }
    LIST_VIEW_CLASS (This)->SetSelection (This, SelectedIdx, TRUE);
    CONTROL_CLASS_INVALIDATE (This);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;
}

LRESULT
EFIAPI
UiListViewProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_LIST_VIEW                  *This;
  UI_CONTROL                    *Control;
  POINT                         Pt;
  UI_CONTROL                    *Item;
  RECT                          ItemRc;
  STATIC INT32                  DelayConut;

  This = (UI_LIST_VIEW *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_LIST_VIEW *) AllocateZeroPool (sizeof (UI_LIST_VIEW));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    SetWindowLongPtr (
      Control->Wnd,
      GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE
      );
    This->CurSel = -1;
    break;

  case WM_MOUSEMOVE:
    POINTSTOPOINT (Pt, LParam);
    UiListViewMouseMove (Control, WParam, Pt);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    break;

  case WM_LBUTTONDOWN:
    POINTSTOPOINT (Pt, LParam);
    UiListViewLButtonDown (Control, WParam, Pt);
    if (IsRectEmpty (&This->MoveItemOrgRc)) {
      //
      // User doesn't press drag region, pass message to control to do scroll operation if need.
      //
      return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    }
    DelayConut = 0;
    SetCapture (Control->Wnd);
    break;

  case WM_LBUTTONUP:
    POINTSTOPOINT (Pt, LParam);
    UiListViewLButtonUp (Control, WParam, Pt);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    break;

  case WM_KEYDOWN:
    return UiListViewKeyDown (Wnd, Msg, WParam, LParam);

  case WM_KEYUP:
    break;

  case WM_NCHITTEST:
    return HTCLIENT;

  case UI_NOTIFY_PAINT:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    if (WParam == PAINT_ALL && IsDraggingItem (This)) {
      //
      // Repaint Current Selected Item
      //
      Item = Control->Items[This->CurSel];
      Item->Manager->GetControlRect (Item->Manager, Item, &ItemRc);
      SendMessage (Item->Wnd, UI_NOTIFY_PAINT, PAINT_ALL, (LPARAM)&ItemRc);
    }
    break;

  case WM_TIMER:
    if (This->IntensityOfRepeatMoveToNext != 0) {
      if (++DelayConut > abs (This->IntensityOfRepeatMoveToNext)) {
        DelayConut = 0;

        Item = Control->Items[This->CurSel];
        GetWindowRect (Item->Wnd, &ItemRc);
        MapWindowPoints (HWND_DESKTOP, GetParent(Item->Wnd), (LPPOINT) &ItemRc, 2);
        UiListViewCurSelMoveToNext (This, (This->IntensityOfRepeatMoveToNext < 0));

        Item = Control->Items[This->CurSel];
        CONTROL_CLASS (Item)->SetPosition (Item, &ItemRc);
      }
    }
    return PARENT_CLASS_WNDPROC(CURRENT_CLASS, Wnd, Msg, WParam, LParam);

  case WM_DESTROY:
    KillTimer (Wnd, LIST_VIEW_TIMER_ID);
    return PARENT_CLASS_WNDPROC(CURRENT_CLASS, Wnd, Msg, WParam, LParam);

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;

}

UI_LIST_VIEW_CLASS *
EFIAPI
GetListViewClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"ListView", (UI_CONTROL_CLASS *) GetVerticalLayoutClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc  = UiListViewProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetState = UiListViewSetState;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetPosition = UiListViewSetPosition;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiListViewSetAttribute;

  CURRENT_CLASS->PerformItemClick       = UiListViewPerformItemClick;
  CURRENT_CLASS->SetSelection           = UiListViewSetSelection;
  CURRENT_CLASS->GetSelection           = UiListViewGetSelection;
  CURRENT_CLASS->FindNextSelection      = UiListViewFindNextSelection;
  CURRENT_CLASS->SwitchItems            = UiListViewSwitchItems;

  return CURRENT_CLASS;
}

