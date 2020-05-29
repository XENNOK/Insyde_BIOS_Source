/** @file
  Layout

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
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
#include "UiCellLayout.h"
#if defined(_MSC_VER)
#pragma warning (disable: 4366)
#endif



BOOLEAN
UiCellLayoutAdd (
  UI_CONTAINER *This,
  UI_CONTROL   *Control
  )
{

  ASSERT (Control != NULL);

  This->ItemCount++;
  This->Items = realloc (This->Items, This->ItemCount * sizeof (UI_CONTROL *));
  This->Items[This->ItemCount - 1] = Control;

  This->Control.Invalidate (&This->Control);
  Control->Manager->NeedUpdatePos = TRUE;
  return TRUE;
}


VOID
UiCellLayoutSetPos (
  UI_CELL_LAYOUT *This
  )
{
  UI_CONTAINER  *Container;
  CELL_INFO     *CellInfo;
  UINTN         Index;
  UINTN         ItemIndex;
  RECT          Rc;

  INT32         X;
  INT32         Y;
  INT32         Width;
  INT32         Height;

  Container = (UI_CONTAINER *) This;

  if (Container->ItemCount == 0) {
    return ;
  }

  GetClientRect (Container->Control.Wnd, &Rc);

  Rc.left   += Container->Inset.left;
  Rc.top    += Container->Inset.top;
  Rc.right  -= Container->Inset.right;
  Rc.bottom -= Container->Inset.bottom;

  if (This->CellInfo == NULL) {
    return ;
  }

  for (Index = 0; Index < This->CellCount; Index++) {
    CellInfo = &This->CellInfo[Index];
    for (ItemIndex = 0; ItemIndex < Container->ItemCount; ItemIndex++) {
      if (CellInfo->Control != Container->Items[Index]) {
        continue;
      }
      X      = (INT32)(CellInfo->CellX * (This->CellWidth + This->WidthGap) + Rc.left);
      Y      = (INT32)(CellInfo->CellY * (This->CellHeight + This->HeightGap) + Rc.top);
      Width  = (INT32)(CellInfo->SpanX * This->CellWidth + (CellInfo->SpanX - 1) * This->WidthGap);
      Height = (INT32)(CellInfo->SpanY * This->CellHeight + (CellInfo->SpanY - 1) * This->HeightGap);

      MoveWindow (Container->Items[Index]->Wnd, X, Y, Width, Height, TRUE);
      SendMessage (Container->Items[Index]->Wnd, UI_NOTIFY_SET_POSITION, 0, 0);
    }
  }
}


BOOLEAN
UiCellLayoutSetAttribute (
  UI_CONTROL   *Control,
  CHAR16       *Name,
  CHAR16       *Value
  )
{

  EFI_STATUS     Status;
  UI_CELL_LAYOUT *This;

  This = (UI_CELL_LAYOUT *) Control;

  if (StrCmp (Name, L"cellwidth") == 0) {
    This->CellWidth  = (INTN)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"cellheight") == 0) {
    This->CellHeight = (INTN)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"countx") == 0) {
    This->CountX     = (INTN)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"county") == 0) {
    This->CountY     = (INTN)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"widthgap") == 0) {
    This->WidthGap   = (INTN)StrToUInt (Value, 10, &Status);
  } else if (StrCmp (Name, L"heightgap") == 0) {
    This->HeightGap  = (INTN)StrToUInt (Value, 10, &Status);
  } else {
    return UiContainerSetAttribute (Control, Name, Value);
  }
  return TRUE;

}

VOID
UiCellLayoutSetCellInfo (
  UI_CELL_LAYOUT *This,
  UINTN          CellCount,
  CELL_INFO      *CellInfo
  )
{
  if (This->CellInfo != NULL) {
    gBS->FreePool (This->CellInfo);
  }

  This->CellCount = CellCount;
  This->CellInfo  = AllocateCopyPool (
                      CellCount * sizeof (CELL_INFO),
                      CellInfo
                      );



  SendMessage (This->Container.Control.Wnd, UI_NOTIFY_SET_POSITION, 0, 0);
}

LRESULT
UiCellLayoutProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_CELL_LAYOUT *This;
  UI_CONTROL     *Control;


  This    = (UI_CELL_LAYOUT *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *) This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_CELL_LAYOUT *) AllocateZeroPool (sizeof (UI_CELL_LAYOUT));
    SetWindowLongPtr (Hwnd, 0, (INTN)This);
    SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiContainerProc (Hwnd, Msg, WParam, LParam);
    Control->SetAttribute = UiCellLayoutSetAttribute;
    This->SetCellInfo     = UiCellLayoutSetCellInfo;
    break;

  case UI_NOTIFY_SET_POSITION:
    UiCellLayoutSetPos (This);
    break;

  case WM_DESTROY:
    if (This->CellInfo != NULL) {
      ScuSafeFreePool ((VOID**)&This->CellInfo);
    }
    UiContainerProc (Hwnd, Msg, WParam, LParam);
    break;

  default:
    return UiContainerProc (Hwnd, Msg, WParam, LParam);
  }

  return 0;

}

UINT32
WINAPI
RegisterUiCellLayout (
  HINSTANCE hInstance
  )
{
  WNDCLASS  wc;

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiCellLayoutProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_CONTROL *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"CellLayout";

  return RegisterClass(&wc);
}



