/** @file
  UI Common Controls

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UiCustomizedControls.h"
#include "H2ODisplayEngineLocalMetro.h"
#include "MetroUi.h"

STATIC UI_SETUP_MENU_ITEM_CLASS  *mSetupMenuItemClass = NULL;
#define CURRENT_CLASS            mSetupMenuItemClass

VOID
CreateSetupMenuItemChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *bkimage;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *image;
  UI_CONTROL                    *DummyVerticalLayout3;
  UI_CONTROL                    *text;


  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"bkcolor='0xFFFF0000' name='DummyVerticalLayout1'");

  bkimage = CreateControl (L"Texture", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, bkimage);
  UiApplyAttributeList (bkimage, L"bkimage='@MenuBackgroundNormal' bkimagestyle='stretch' float='true' height='match_parent' name='bkimage' width='match_parent'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"float='true' height='match_parent' name='DummyVerticalLayout2' padding='30, 30, 30, 30' width='match_parent'");

  image = CreateControl (L"Texture", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, image);
  UiApplyAttributeList (image, L"bkimagestyle='stretch|light' name='image'");

  DummyVerticalLayout3 = CreateControl (L"VerticalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyVerticalLayout3);
  UiApplyAttributeList (DummyVerticalLayout3, L"name='DummyVerticalLayout3'");

  text = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, text);
  UiApplyAttributeList (text, L"align='center' fontsize='20' height='33' name='text' padding='4,0,8,0' textcolor='0xFFDDDDDD'");

}

VOID
EFIAPI
UiSetupMenuItemSetPosition (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetPosition (Control,Pos);
}

VOID
EFIAPI
UiSetupMenuItemSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  UI_SETUP_MENU_ITEM            *SetupMenuItem;
  UI_CONTROL                    *Child;

  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetState (Control, SetState, ClearState);

  SetupMenuItem = (UI_SETUP_MENU_ITEM *) Control;
  Child = UiFindChildByName (Control, L"image");
}

BOOLEAN
EFIAPI
UiSetupMenuItemSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_SETUP_MENU_ITEM            *This;
  EFI_STATUS                    Status;
  UI_CONTROL                    *Child;
  SETUP_MENU_INFO               *SetupMenuInfo;
  CHAR16                        Str[20];

  This = (UI_SETUP_MENU_ITEM *) Control;

  if (StrCmp (Name, L"setupmenuinfo") == 0) {
    SetupMenuInfo = (SETUP_MENU_INFO *)(UINTN) StrToUInt (Value, 16, &Status);
    This->SetupMenuInfo = SetupMenuInfo;

    if (SetupMenuInfo->PageImage != NULL) {
      Child = UiFindChildByName (This, L"image");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x",  SetupMenuInfo->PageImage);
      UiSetAttribute (Child, L"bkimage", Str);
      UiSetAttribute (Child, L"bkimagestyle", L"stretch");
      if ((SetupMenuInfo->PageId >> 16) != (gFB->CurrentP->PageId >> 16)) {
        UiSetAttribute (Child, L"bkimagestyle", L"stretch|light");
      }
    }

    Child = UiFindChildByName (This, L"text");
    UiSetAttribute (Child, L"text", SetupMenuInfo->PageTitle);

  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}


LRESULT
EFIAPI
UiSetupMenuItemProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_SETUP_MENU_ITEM            *This;
  UI_CONTROL                    *Control;

  This = (UI_SETUP_MENU_ITEM *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_SETUP_MENU_ITEM *) AllocateZeroPool (sizeof (UI_SETUP_MENU_ITEM));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateSetupMenuItemChilds (Control);
    break;

  case UI_NOTIFY_PAINT:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);

    if (WParam == PAINT_ALL &&
        (CONTROL_CLASS_GET_STATE (This) & (UISTATE_SELECTED | UISTATE_FOCUSED)) == (UISTATE_SELECTED | UISTATE_FOCUSED)) {
      COLORREF                  Color;
      UI_MANAGER                *Manager;
      RECT                      Rc;
      HDC                       Hdc;

      Color = 0xFFFF0000;
      Manager = Control->Manager;
      Hdc     = Manager->PaintDC;

      Manager->GetControlRect (Manager, Control, &Rc);

      SelectObject (Hdc, GetStockObject (PS_NULL));
      SelectObject (Hdc, GetStockObject (DC_PEN));
      SetDCPenColor (Hdc, Color);
      Rectangle (Hdc, Rc.left, Rc.top, Rc.right, Rc.bottom);
   }
   break;

  case WM_NCHITTEST:
    return HTTRANSPARENT;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_SETUP_MENU_ITEM_CLASS *
EFIAPI
GetSetupMenuItemClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"SetupMenuItem", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiSetupMenuItemProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiSetupMenuItemSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetPosition  = UiSetupMenuItemSetPosition;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetState     = UiSetupMenuItemSetState;


  return CURRENT_CLASS;
}


