/** @file
  UI Container

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
#if defined(_MSC_VER)
#pragma warning (disable: 4366)
#endif

UI_CONTROL *
ContainerCreateControl (
  UI_CONTAINER *Container,
  CHAR16     *ControlName,
  CHAR16     *Text
  )
{
  UI_CONTROL *Control;

  Control = CreateControl (ControlName, Text, (UI_CONTROL *) Container, Container->Control.Manager);
  Container->Add (Container, Control);

  return Control;
}



VOID
UiContainerCreate (
  UI_CONTAINER *This
  )
{
  This->ItemCount = 0;
  This->Items     = NULL;
  This->ChildPadding = 2;
  This->HScrollBar = NULL;
  This->VScrollBar   = NULL;
  SetRectEmpty (&This->Inset);
  This->Inset.left = 0;
  This->Inset.right = 0;
  This->ScrollPos.x = 0;
  This->ScrollPos.y = 0;
  This->TouchSlop   = 4;
}



BOOLEAN
UiContainerSetAttribute (
  UI_CONTROL   *Control,
  CHAR16       *Name,
  CHAR16       *Value
  )
{
  EFI_STATUS   Status;
  UI_CONTAINER *This;
  UI_CONTROL   *ScrollBar;
  CHAR16       *PtrValue;

  This = (UI_CONTAINER *) Control;

  if (StrCmp (Name, L"childpadding") == 0) {
    This->ChildPadding = (UINT32)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"vscrollbarpos") == 0) {
    if (This->VScrollBar != NULL) {
      This->VScrollBar->SetScrollPos (This->VScrollBar, (INT32)StrToUInt (Value, 10, &Status));
    }
  } else if (StrCmp (Name, L"vscrollbar") == 0) {

    if (StrCmp (Value, L"true") == 0 &&
      This->VScrollBar == NULL) {
      ScrollBar = CreateControl (L"SCROLLBAR", NULL, Control, Control->Manager);
      ScrollBar->SetAttribute (ScrollBar, L"hor", L"false");
      This->VScrollBar = (UI_SCROLLBAR *) ScrollBar;
      ((UI_SCROLLBAR *)ScrollBar)->Owner = This;
    }
    //    EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);

  } else if (StrCmp (Name, L"hscrollbar") == 0) {

    if ((StrCmp (Value, L"true") == 0) && (This->HScrollBar == NULL)) {

      ScrollBar = CreateControl (L"SCROLLBAR", NULL, Control, Control->Manager);
      ScrollBar->SetAttribute (ScrollBar, L"hor", L"true");
      This->HScrollBar = (UI_SCROLLBAR *) ScrollBar;
    }
    //    EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);

  } else if (StrCmp (Name, L"inset") == 0) {
    This->Inset.left   = wcstol (Value, &PtrValue, 10);
    This->Inset.top    = wcstol (PtrValue + 1, &PtrValue, 10);
    This->Inset.right  = wcstol (PtrValue + 1, &PtrValue, 10);
    This->Inset.bottom = wcstol (PtrValue + 1, &PtrValue, 10);

    Control->Manager->NeedUpdatePos = TRUE;
    Control->Invalidate (Control);

 } else if (StrCmp (Name, L"inset2") == 0) {

    This->Inset2 = (UINT32)StrToUInt (Value, 10, &Status);

    Control->Manager->NeedUpdatePos = TRUE;
    Control->Invalidate (Control);
  } else {
    return UiControlSetAttribute (Control, Name, Value);
  }
  return TRUE;
}


BOOLEAN
UiContainerAdd (
  UI_CONTAINER *This,
  UI_CONTROL   *Control
  )
{

  ASSERT (Control != NULL);

  This->ItemCount++;
  This->Items = realloc (This->Items, This->ItemCount * sizeof (UI_CONTROL *));
  This->Items[This->ItemCount - 1] = Control;

  Control->Manager->NeedUpdatePos = TRUE;
  Control->Invalidate (Control);
  return TRUE;
}

BOOLEAN
UiContainerRemove (
  UI_CONTAINER *This,
  UI_CONTROL   *Control
  )
{

  UINTN Index;
  UINTN Index2;
  ASSERT (Control != NULL);

  for (Index = 0;  Index < This->ItemCount; Index++) {
    if (This->Items[Index] == Control) {
      for (Index2 = Index; Index < This->ItemCount - 1; Index2++) {
        This->Items[Index2] = This->Items[Index2 + 1];
      }
      This->ItemCount--;
      DestroyWindow (Control->Wnd);
      break;
    }
  }

  This->Control.Manager->NeedUpdatePos = TRUE;
  This->Control.Invalidate (&This->Control);
  //Control->Invalidate (Control);
  return TRUE;
}



BOOLEAN
UiContainerRemoveAll (
  UI_CONTAINER *This
  )
{
  UINTN Index;

  if (This->ItemCount == 0) {
    return TRUE;
  }

  for (Index = 0; Index < This->ItemCount; Index++) {
    DestroyWindow (This->Items[Index]->Wnd);
  }

  gBS->FreePool (This->Items);
  This->Items     = NULL;
  This->ItemCount = 0;

  This->Control.Manager->NeedUpdatePos = TRUE;
  This->Control.Invalidate (&This->Control);
  return TRUE;
}

VOID
UiContainerSetPos (
  UI_CONTAINER *This
  )
{

  RECT   Rc;
  UINTN  Index;
  UI_CONTROL   *Item;
  RECT   ItemRc;
  if (This->ItemCount == 0) {
    return ;
  }

  GetClientRect (This->Control.Wnd, &Rc);

  Rc.left   += This->Inset.left;
  Rc.top    += This->Inset.top;
  Rc.right  -= This->Inset.right;
  Rc.bottom -= This->Inset.bottom;

  for (Index = 0; Index < This->ItemCount; Index++) {
    Item = This->Items[Index];

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    ItemRc = Rc;

    if (Item->Float) {
      SetRectEmpty (&ItemRc);

      if( Item->FixedXY.cx >= 0 ) {
        ItemRc.left  = Rc.left + Item->FixedXY.cx;
        ItemRc.right = Rc.left + Item->FixedXY.cx + Item->FixedSize.cx;
      }
      else {
        ItemRc.left = Rc.right + Item->FixedXY.cx - Item->FixedSize.cx;
        ItemRc.right = Rc.right + Item->FixedSize.cx;
      }
      if( Item->FixedXY.cy >= 0 ) {
        ItemRc.top = Rc.top + Item->FixedXY.cy;
        ItemRc.bottom = Rc.top + Item->FixedXY.cy + Item->FixedSize.cy;
      }
      else {
        ItemRc.top = Rc.bottom + Item->FixedXY.cy - Item->FixedSize.cy;
        ItemRc.bottom = Rc.bottom + Item->FixedSize.cy;
      }
    }


    MoveWindow (
                  Item->Wnd,
                  ItemRc.left, ItemRc.top,
                  ItemRc.right - ItemRc.left, ItemRc.bottom - ItemRc.top,
                  TRUE
                  );

    SendMessage (Item->Wnd, UI_NOTIFY_SET_POSITION, 0, 0);


  }
}

BOOLEAN
UiContainerShouldDelayChildPressedState (
  UI_CONTROL *Control
  )
{
  UI_CONTAINER *Container;

  Container = (UI_CONTAINER *) Control;

  if (Container->VScrollBar != NULL || Container->HScrollBar != NULL) {
    return TRUE;
  }
  return FALSE;
}

LRESULT
UiContainerProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_CONTAINER *This;
  UI_CONTROL   *Item;
  UINTN  Index;
  UI_MANAGER   *Manager;
  RECT   ItemRc;
  RECT   TmpRc;
  RECT   Rc;
  UI_CONTROL   *Control;
  RECT   ClipRc;
  HRGN   OldRgn;
  HRGN   NewRgn;
  POINT  Point;
  INT32  DeltaX;
  INT32  DeltaY;

  Control = (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);

  This = (UI_CONTAINER *) Control;

  Manager = NULL;


  if (This != NULL) {
    Manager = This->Control.Manager;
  }


  switch (Msg) {

  case WM_CREATE:
    This = (UI_CONTAINER *) AllocateZeroPool (sizeof (UI_CONTAINER));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;


  case UI_NOTIFY_CREATE:
    UiControlProc (Hwnd, Msg, WParam, LParam);
    UiContainerCreate (This);
    Control->SetAttribute = UiContainerSetAttribute;
    Control->ShouldDelayChildPressedState = UiContainerShouldDelayChildPressedState;
    This->Add             = UiContainerAdd;
    This->RemoveAll       = UiContainerRemoveAll;
    This->Remove          = UiContainerRemove;
    This->IsBeginDraged   = FALSE;
    break;

  case UI_NOTIFY_SET_POSITION:
    UiContainerSetPos (This);
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_ALL) {
      Manager->GetControlRect (Manager, &This->Control, &Rc);
      if (!IntersectRect (&TmpRc, &Rc, (RECT *)LParam)) {
        return 0;
      }

      UiControlProc (Hwnd, Msg, WParam, LParam);
      GetClipBox (Manager->PaintDc, &ClipRc);
      OldRgn = CreateRectRgnIndirect (&ClipRc);
      NewRgn = CreateRectRgnIndirect (&TmpRc);
      ExtSelectClipRgn (Manager->PaintDc, NewRgn, RGN_AND);

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

      SelectClipRgn(Manager->PaintDc, OldRgn);
      DeleteObject(OldRgn);
      DeleteObject(NewRgn);

    } else {
      UiControlProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_DESTROY:
    if (This->HScrollBar != NULL) {
      DestroyWindow (((UI_CONTROL *)(This->HScrollBar))->Wnd);
    }

    if (This->VScrollBar != NULL) {
      DestroyWindow (((UI_CONTROL *)(This->VScrollBar))->Wnd);
    }

    This->RemoveAll (This);

    UiControlProc (Hwnd, Msg, WParam, LParam);
    break;

  case WM_LBUTTONDOWN:
    if ((This->HScrollBar != NULL) || (This->VScrollBar != NULL)) {
      This->IsBeginDraged = TRUE;
      POINTSTOPOINT(Point, LParam);
      ClientToScreen (Hwnd, &Point);
      This->LastMotionX = Point.x;
      This->LastMotionY = Point.y;
      SetCapture (Hwnd);
    } else {
      UiControlProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_NCMOUSEMOVE:
    if (This->IsBeginDraged) {
      POINTSTOPOINT(Point, LParam);
      DeltaX = This->LastMotionX - Point.x;
      DeltaY = This->LastMotionY - Point.y;

      DEBUG ((EFI_D_ERROR, "deltax:%d\n", DeltaY));

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
          DeltaY = 0;
        }
      } else if (DeltaY > 0) {
  //     DeltaY =
      }

      if (This->VScrollBar != NULL) {
        This->VScrollBar->SetScrollPos (This->VScrollBar, This->ScrollPos.y + DeltaY);
      }
    } else {
      UiControlProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  case WM_LBUTTONUP:
  case WM_NCLBUTTONUP:
    if (This->IsBeginDraged) {
      ReleaseCapture ();
      This->IsBeginDraged = FALSE;
    } else {
      UiControlProc (Hwnd, Msg, WParam, LParam);
    }
    break;

  default:
    return UiControlProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

VOID
UiVerticalLayoutSetPos (
  UI_VERTICAL_LAYOUT *This
)
{
  UI_CONTAINER *Container;
  UI_CONTROL   *Control;
  UI_CONTROL   *Item;
  UINTN  Index;
  RECT   Rc;
  RECT   ItemRc;
  SIZE   Size;
  UINT32       AjustableCount;
  UINT32       Fixed;
  UINT32       EstimateNum;
  UINT32       Remain;
  UINT32       Pos;
  UINT32       RemainFixed;
  UINT32       Expand;
  SIZE   Available;
  SIZE   ItemSize;


  Size.cx  = 0;
  Size.cy  = 0;
  Expand   = 0;

  Container = (UI_CONTAINER *)This;
  Control   = (UI_CONTROL *) This;
  if (Container->ItemCount == 0) {
    return ;
  }

  GetClientRect (This->Container.Control.Wnd, &Rc);
  Rc.left   += Container->Inset.left;
  Rc.top    += Container->Inset.top;
  Rc.right  -= Container->Inset.right;
  Rc.bottom -= Container->Inset.bottom;

  if (Container->VScrollBar != NULL ) {
    Rc.right -= Container->VScrollBar->Control.FixedSize.cx;
    Rc.right -= Container->Inset2;
  }

  if (Container->HScrollBar != NULL) {
    Rc.bottom -= Container->HScrollBar->Control.FixedSize.cy;
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
  Fixed    = 0;
  EstimateNum    = 0;

  if (StrCmp (Control->Name, L"menuoptionlist") == 0) {
    Index = 0;
  }


  for (Index = 0; Index < Container->ItemCount; Index++) {
    Item = Container->Items[Index];

    Size = Item->EstimateSize (Item, Available);

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    if (Size.cy == 0) {
      AjustableCount++;
    }

    Fixed += Size.cy;
    Fixed += Item->Padding.top + Item->Padding.bottom;
    EstimateNum ++;
  }
  Fixed += (EstimateNum - 1) * Container->ChildPadding;

  if (AjustableCount > 0) {
    Expand = MAX (0, (Available.cy - Fixed) / AjustableCount);
  }

  Pos = Rc.top;
  Pos -= Container->ScrollPos.y;
  Remain = 0;
  RemainFixed = Fixed;
  for (Index = 0; Index < Container->ItemCount; Index++) {

    Item = Container->Items[Index];

//    Size = Item->FixedXY;
    Size = Item->EstimateSize (Item, Available);

//    if (StrCmp (Item->Name, L"menu_prompt") == 0) {
      //DEBUG ((EFI_D_ERROR, "%s %d %d\n", Item->Name, Size.cx, Size.cy));
  //  }

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    if (Size.cy == 0) {
      ItemSize.cy = Expand;
    } else {
      ItemSize.cy = Size.cy;
      RemainFixed -= Size.cy;
    }

    if (Size.cx != 0 && Item->Float) {
      ItemSize.cx = Size.cx;
    } else {
      ItemSize.cx = Rc.right - Rc.left;
    }


    ItemRc.left   = Rc.left;
    ItemRc.top    = Pos + Item->Padding.top;
    ItemRc.right  = ItemRc.left + ItemSize.cx;
    ItemRc.bottom = ItemRc.top  + ItemSize.cy;

    MoveWindow (
      Item->Wnd,
      ItemRc.left, ItemRc.top,
      ItemSize.cx, ItemSize.cy,
      TRUE
      );

    SendMessage (Item->Wnd, UI_NOTIFY_SET_POSITION, 0, 0);

    Pos = ItemRc.bottom + Container->ChildPadding + Item->Padding.top + Item->Padding.bottom;

  }


  //
  // process scroll bar
  //
  if (Container->HScrollBar != NULL) {
    SetWindowPos (
      Container->HScrollBar->Control.Wnd,
      HWND_TOP,
      0, Rc.bottom, Rc.right - Rc.left, Container->HScrollBar->Control.FixedSize.cy, 0);
  }

  if (Container->VScrollBar != NULL) {
    SetWindowPos (
      Container->VScrollBar->Control.Wnd,
      HWND_TOP,
      Rc.right + Container->Inset2, 0, Container->VScrollBar->Control.FixedSize.cx, Rc.bottom, 0);
  }
}


LRESULT
UiVerticalLayoutProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_VERTICAL_LAYOUT *This;

  This = (UI_VERTICAL_LAYOUT *) GetWindowLongPtr (Hwnd, 0);

  switch (Msg) {

  case WM_CREATE:
    This = (UI_VERTICAL_LAYOUT *) AllocateZeroPool (sizeof (UI_VERTICAL_LAYOUT));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiContainerProc (Hwnd, Msg, WParam, LParam);
    break;

  case UI_NOTIFY_SET_POSITION:
    if (StrCmp (This->Container.Control.Name, L"list") == 0) {
      This = This;
    }

    UiVerticalLayoutSetPos (This);
    break;

  default:
    return UiContainerProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;

}




VOID
UiHorizontalLayoutSetPos (
  UI_HORIZONTAL_LAYOUT *This
)
{
  UI_CONTAINER *Container;
  UI_CONTROL   *Item;
  UINTN  Index;
  RECT   Rc;
  RECT   ItemRc;
  SIZE   Size;
  UINT32       AjustableCount;
  UINT32       Fixed;
  UINT32       EstimateNum;
  UINT32       Remain;
  UINT32       Pos;
  UINT32       RemainFixed;
  UINT32       Expand;
  SIZE   Available;
  SIZE   ItemSize;


  Size.cx  = 0;
  Size.cy  = 0;
  Expand   = 0;

  Container = (UI_CONTAINER *)This;

  if (Container->ItemCount == 0) {
    return ;
  }

  GetClientRect (This->Container.Control.Wnd, &Rc);

  Rc.left   += Container->Inset.left;
  Rc.top    += Container->Inset.top;
  Rc.right  -= Container->Inset.right;
  Rc.bottom -= Container->Inset.bottom;

  if (Container->VScrollBar != NULL) {
    Rc.right -= Container->VScrollBar->Control.FixedSize.cx;
  }

  if (Container->HScrollBar != NULL) {
    Rc.bottom -= Container->HScrollBar->Control.FixedSize.cy;
    Rc.right -= Container->Inset2;
  }

  Available.cx = Rc.right - Rc.left;
  Available.cy = Rc.bottom - Rc.top;

  AjustableCount = 0;
  Fixed          = 0;
  EstimateNum    = 0;

  for (Index = 0; Index < Container->ItemCount; Index++) {
    Item = Container->Items[Index];
//  Size = Item->FixedXY;
    Size = Item->EstimateSize (Item, Available);


    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    if (Size.cx == 0) {
      AjustableCount++;
    }
    Fixed += Size.cx;
    Fixed += Item->Padding.left + Item->Padding.right;
    EstimateNum ++;
  }
  Fixed += (EstimateNum - 1) * Container->ChildPadding;
  if (AjustableCount > 0) {
    Expand = MAX (0, (Available.cx - Fixed) / AjustableCount);
  }

  Pos = Rc.left;
  Remain = 0;
  RemainFixed = Fixed;
  for (Index = 0; Index < Container->ItemCount; Index++) {
    Item = Container->Items[Index];
//  Size = Item->FixedXY;
    Size = Item->EstimateSize (Item, Available);

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    //DEBUG ((EFI_D_ERROR, "%s %d %d: %s\n", Item->Name, Size.cx, Size.cy, Item->Wnd->szTitle));

    if (Size.cx == 0) {
      ItemSize.cx = Expand;
    } else {
      ItemSize.cx = Size.cx;
      RemainFixed -= Size.cx;
    }

    if (Size.cy == 0) {
      ItemSize.cy = Rc.bottom - Rc.top;
    } else {
      ItemSize.cy = Size.cy;
    }

    ItemRc.left   = Pos + Item->Padding.left;
    ItemRc.top    = Rc.top;
    ItemRc.right  = ItemRc.left + ItemSize.cx;
    ItemRc.bottom = ItemRc.top  + ItemSize.cy;

    MoveWindow (
      Item->Wnd,
      ItemRc.left, ItemRc.top,
      ItemSize.cx, ItemSize.cy,
      TRUE
      );

    SendMessage (Item->Wnd, UI_NOTIFY_SET_POSITION, 0, 0);

    Pos = ItemRc.right + Container->ChildPadding + Item->Padding.left + Item->Padding.right;

  }
}

LRESULT
UiHorizontalLayoutProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_HORIZONTAL_LAYOUT *This;

  This = (UI_HORIZONTAL_LAYOUT *) GetWindowLongPtr (Hwnd, 0);

  switch (Msg) {

  case WM_CREATE:
    This = (UI_HORIZONTAL_LAYOUT *) AllocateZeroPool (sizeof (UI_HORIZONTAL_LAYOUT));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiContainerProc (Hwnd, Msg, WParam, LParam);
    break;

  case UI_NOTIFY_SET_POSITION:
    UiHorizontalLayoutSetPos (This);
    break;

  default:
    return UiContainerProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;

}

typedef struct {
  UI_CONTROL *Item;
  RECT       Rc;
} TABLE_INFO;

VOID
UiTileLayoutSetPos (
  UI_TILE_LAYOUT *This
)
{
  UI_CONTAINER *Container;
  UI_CONTROL   *Item;
  UINTN  Index;
  UINTN  Index2;
  RECT   Rc;
  SIZE   Size;
  UINT32       AjustableCount;
  UINT32       Fixed;
  UINT32       EstimateNum;
  UINT32       Expand;
  SIZE   Available;
  UINT32       FixedWidth;
  INT32  FixedHeight;
  RECT   TileRc;
  POINT  TilePos;
  UINTN  Count;
  RECT   WinRc;
  TABLE_INFO *TableInfo;
  RECT   ParentRc;
  RECT   TmpRc;
  Size.cx  = 0;
  Size.cy  = 0;
  Expand   = 0;

  Container = (UI_CONTAINER *)This;

  if (Container->ItemCount == 0) {
    return ;
  }

  GetClientRect (This->Container.Control.Wnd, &Rc);
  GetWindowRect (This->Container.Control.Wnd, &ParentRc);

  Rc.left   += Container->Inset.left;
  Rc.top    += Container->Inset.top;
  Rc.right  -= Container->Inset.right;
  Rc.bottom -= Container->Inset.bottom;

  if (Container->VScrollBar != NULL) {
    Rc.right -= Container->VScrollBar->Control.FixedSize.cx;
  }

  if (Container->HScrollBar != NULL) {
    Rc.bottom -= Container->HScrollBar->Control.FixedSize.cy;
  }

  Available.cx = 0;

  TilePos.x  = Rc.left;
  TilePos.y  = Rc.top;

  if (Rc.right < Rc.left) {
    FixedWidth = 100;
  } else {
    FixedWidth = (Rc.right - Rc.left - Container->ChildPadding * (This->Columns - 1)) / This->Columns;
  }

  AjustableCount = 0;
  Fixed    = 0;
  EstimateNum    = 0;

  Count    = 0;

  if (This->FixedHeight != 0) {
    FixedHeight = This->FixedHeight;
  } else {
    FixedHeight = 100;
  }

  //
  // create table info
  //
  TableInfo = AllocateZeroPool (sizeof (TABLE_INFO) * 20);
  for (Index = 0; Index < 20; Index++) {
    TableInfo[Index].Rc.left   = TilePos.x;
    TableInfo[Index].Rc.top    = TilePos.y;
    TableInfo[Index].Rc.right  = TableInfo[Index].Rc.left + FixedWidth;
    TableInfo[Index].Rc.bottom = TableInfo[Index].Rc.top  + FixedHeight;

    if (((Index + 1) % This->Columns) == 0) {
      TilePos.x =  Rc.left;
      TilePos.y += FixedHeight + Container->ChildPadding;
    } else {
      TilePos.x += FixedWidth + Container->ChildPadding;;
    }
  }


  SetRectEmpty (&TileRc);
  for (Index = 0; Index < Container->ItemCount; Index++) {
    Item = Container->Items[Index];

    if ((GetWindowLongPtr (Item->Wnd, GWL_STYLE) & WS_VISIBLE) == 0) {
      continue;
    }

    GetWindowRect (Item->Wnd, &WinRc);
    OffsetRect (&WinRc, -ParentRc.left, -ParentRc.top);


    //
    // find nearest empty position
    //
    for (Index2 = 0; Index2 < 20; Index2++) {
      if (TableInfo[Index2].Item == NULL) {
        TmpRc = TableInfo[Index2].Rc;
        InflateRect (&TmpRc, 1, 1);
        TmpRc.left -= Container->ChildPadding;
        TmpRc.top  -= Container->ChildPadding;
        if (PtInRect (&TmpRc, *(POINT *)&WinRc)) {
          TileRc = TableInfo[Index2].Rc;
          TableInfo[Index2].Item = Item;
          break;
        }
      }
    }

    if (Index2 == 20) {
      for (Index2 = 0; Index2 < 20; Index2++) {
        if (TableInfo[Index2].Item == NULL) {
          TableInfo[Index2].Item = Item;
          TileRc = TableInfo[Index2].Rc;
          break;
        }
      }
    }

    MoveWindow (
      Item->Wnd,
      (TileRc.left),
      (TileRc.top),
      FixedWidth,
      FixedHeight,
      TRUE
      );

   SendMessage (Item->Wnd, UI_NOTIFY_SET_POSITION, 0, 0);


  }
}




BOOLEAN
UiTileLayoutSetAttribute (
  UI_CONTROL   *Control,
  CHAR16       *Name,
  CHAR16       *Value
  )
{

  EFI_STATUS     Status;
  UI_TILE_LAYOUT *This;

  This = (UI_TILE_LAYOUT *) Control;

  if (StrCmp (Name, L"fixedheight") == 0) {
    This->FixedHeight = (UINT32)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"columns") == 0) {
    This->Columns =  (UINT32)StrToUInt (Value, 10, &Status);
  } else {
    return UiContainerSetAttribute (Control, Name, Value);
  }
  return TRUE;

}




LRESULT
UiTileLayoutProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_TILE_LAYOUT *This;
  UI_CONTROL     *Control;


  This = (UI_TILE_LAYOUT *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *) This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_TILE_LAYOUT *) AllocateZeroPool (sizeof (UI_TILE_LAYOUT));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiContainerProc (Hwnd, Msg, WParam, LParam);
    Control->SetAttribute = UiTileLayoutSetAttribute;
    This->Columns = 2;
    //This->FixedHeight = 128;
    break;

  case UI_NOTIFY_SET_POSITION:
    UiTileLayoutSetPos (This);
    break;

  default:
    return UiContainerProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;

}


UINT32
RegisterUiContainer(HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiContainerProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon   = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"Container";

  return RegisterClass(&wc);
}

UINT32
WINAPI
RegisterUiVerticalLayout (HINSTANCE hInstance)
{
  WNDCLASS  wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiVerticalLayoutProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon   = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"VerticalLayout";

  return RegisterClass(&wc);
}

UINT32
WINAPI
RegisterUiHorizontalLayout (HINSTANCE hInstance)
{
  WNDCLASS  wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiHorizontalLayoutProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon   = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"HorizontalLayout";

  return RegisterClass(&wc);
}

UINT32
WINAPI
RegisterUiTileLayout (HINSTANCE hInstance)
{
  WNDCLASS  wc;

  wc.style   = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiTileLayoutProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon   = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"TileLayout";

  return RegisterClass(&wc);
}


