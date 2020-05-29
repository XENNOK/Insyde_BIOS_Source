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
#include "H2ODisplayEngineLocalMetro.h"
#include "UiControls.h"
#include "UiCustomizedControls.h"
#include "H2OPanels.h"
#include "MetroUi.h"

STATIC H2O_SETUP_PAGE_PANEL_CLASS        *mH2OSetupPagePanelClass = NULL;
#define CURRENT_CLASS                    mH2OSetupPagePanelClass

extern HWND                     gLastFocus;

//
// Record old page id, when change page need set scrollpos to zero
//
H2O_PAGE_ID                     mOldPageId = 0;

VOID
FreeStatements (
  H2O_SETUP_PAGE_PANEL          *This
  )
{
  UINTN                         Index;

  if (This->StatementCount == 0 && This->Statement == NULL) {
    return;
  }

  for (Index = 0; Index < This->StatementCount; Index++) {
    FreePool (This->Statement[Index]);
  }
  This->StatementCount = 0;
  FreePool (This->Statement);
  This->Statement = NULL;
}


VOID
SetupPageItemSelected (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  )
{
  H2O_FORM_BROWSER_S            *Statement;
  H2O_STATEMENT_ID              StatementId;
  EFI_STATUS                    Status;

  StatementId = (H2O_STATEMENT_ID) GetWindowLongPtr (Item->Wnd, GWLP_USERDATA);
  Status = gFB->GetSInfo (gFB, gFB->CurrentP->PageId, StatementId, &Statement);
  if (EFI_ERROR (Status)) {
    return ;
  }

  if (Statement->IfrOpCode != gFB->CurrentQ->IfrOpCode) {
    SendSelectQNotify (Statement->PageId, Statement->QuestionId, Statement->IfrOpCode);
  }

  FreePool (Statement);
}


VOID
SetupPageItemClick (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  )
{
  H2O_FORM_BROWSER_S            *Statement;
  H2O_STATEMENT_ID              StatementId;
  EFI_STATUS                    Status;
  EFI_HII_VALUE                 HiiValue;

  StatementId = (H2O_STATEMENT_ID) GetWindowLongPtr (Item->Wnd, GWLP_USERDATA);
  Status = gFB->GetSInfo (gFB, gFB->CurrentP->PageId, StatementId, &Statement);
  if (EFI_ERROR (Status)) {
    return ;
  }

  if (Statement->Selectable) {
    SendSelectQNotify (Statement->PageId, Statement->QuestionId, Statement->IfrOpCode);

    if (Statement->Operand == EFI_IFR_CHECKBOX_OP) {
      CopyMem (&HiiValue, &Statement->HiiValue, sizeof (EFI_HII_VALUE));
      HiiValue.Value.b = !HiiValue.Value.b;
      SendChangeQNotify (Statement->PageId, Statement->QuestionId, &HiiValue);
    } else {
      SendOpenQNotify (Statement->PageId, Statement->QuestionId);
    }
  }

  FreePool (Statement);
}

VOID
FrontPageItemOnSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  H2O_FORM_BROWSER_S            *Statement;
  H2O_STATEMENT_ID              StatementId;
  EFI_STATUS                    Status;

  if (SetState & UISTATE_HOVER) {

    StatementId = (H2O_STATEMENT_ID) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);
    Status = gFB->GetSInfo (gFB, gFB->CurrentP->PageId, StatementId, &Statement);
    if (EFI_ERROR (Status)) {
      return ;
    }

    if (Statement->Selectable) {
      SendSelectQNotify (Statement->PageId, Statement->QuestionId, Statement->IfrOpCode);
    }
    FreePool (Statement);
  }
}

EFI_STATUS
RefreshMenuOptionList (
  H2O_SETUP_PAGE_PANEL          *This
  )
{
  UINTN                         Index;
  EFI_STATUS                    Status;
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Item;
  HWND                          Wnd;

  H2O_FORM_BROWSER_P            *CurrentP;
  H2O_PAGE_ID                   PageId;
  H2O_FORM_BROWSER_S            *Statement;
  CHAR16                        Str[20];
  BOOLEAN                       InFrontPage;
  H2O_STATEMENT_ID              StatementId;

  Control = (UI_CONTROL *) This;
  Wnd     = Control->Wnd;

  CurrentP   = gFB->CurrentP;
  PageId     = CurrentP->PageId;

  //
  // remove old items
  //
  CONTROL_CLASS(This)->RemoveAllChild ((UI_CONTROL *)This);

  //
  // set scroll pos to zero when change page
  //
  if (mOldPageId != PageId) {
    mOldPageId = PageId;
    if (Control->VScrollBar != NULL) {
      SCROLLBAR_CLASS(Control->VScrollBar)->SetScrollPos (Control->VScrollBar, 0);
    }
  }

  FreeStatements (This);
  This->Statement      = AllocateZeroPool (CurrentP->NumberOfStatementIds * sizeof (H2O_FORM_BROWSER_S *));
  if (This->Statement == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InFrontPage = IsFrontPage ();
  This->StatementCount = CurrentP->NumberOfStatementIds;
  for (Index = 0; Index < CurrentP->NumberOfStatementIds; Index++) {
    Status = gFB->GetSInfo (gFB, PageId, CurrentP->StatementIds[Index], &Statement);
    This->Statement[Index] = Statement;
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (InFrontPage) {
      if (Statement->Operand != EFI_IFR_REF_OP && Statement->Operand != EFI_IFR_ACTION_OP) {
        continue;
      }
      Item = CreateControl (L"FrontPageItem", (UI_CONTROL*)This);
      UiApplyAttributeList (Item, L"height='175' width='240'");
      Item->OnSetState = FrontPageItemOnSetState;
    } else {
      Item = CreateControl (L"SetupPageItem", (UI_CONTROL*)This);
    }
    UnicodeSPrint (Str, sizeof (Str), L"0x%x", Statement);
    UiSetAttribute (Item, L"statement", Str);
    SetWindowLongPtr (Item->Wnd, GWLP_USERDATA, (INTN)CurrentP->StatementIds[Index]);
    CONTROL_CLASS(This)->AddChild ((UI_CONTROL *)This, Item);
  }

  if (gFB->CurrentQ != NULL) {
    for (Index = 0; Index < Control->ItemCount; Index++) {
      StatementId = (H2O_STATEMENT_ID) GetWindowLongPtr (Control->Items[Index]->Wnd, GWLP_USERDATA);
      if (StatementId == gFB->CurrentQ->StatementId) {
        LIST_VIEW_CLASS (This)->SetSelection ((UI_LIST_VIEW *)This, (INT32)Index, TRUE);
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

VOID
H2OSetupPagePanelSelectQ (
  H2O_SETUP_PAGE_PANEL          *This
  )
{
  UINTN                         Index;
  UI_CONTROL                    *Control;
  H2O_STATEMENT_ID              StatementId;

  Control = (UI_CONTROL *) This;

  for (Index = 0; Index < Control->ItemCount; Index++) {
    StatementId = (H2O_STATEMENT_ID) GetWindowLongPtr (Control->Items[Index]->Wnd, GWLP_USERDATA);
    if (StatementId == gFB->CurrentQ->StatementId) {
      LIST_VIEW_CLASS (This)->SetSelection ((UI_LIST_VIEW *)This, (INT32)Index, TRUE);
      break;
    }
  }
}


LRESULT
EFIAPI
H2OSetupPagePanelProc (
  HWND   Wnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_SETUP_PAGE_PANEL          *This;
  UI_CONTROL                    *Control;
  EFI_KEY_DATA                  KeyData;

  This = (H2O_SETUP_PAGE_PANEL *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_SETUP_PAGE_PANEL *) AllocateZeroPool (sizeof (H2O_SETUP_PAGE_PANEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    ((UI_LIST_VIEW *) This)->OnItemClick = SetupPageItemClick;
    ((UI_LIST_VIEW *) This)->OnItemSelected = SetupPageItemSelected;

    if (IsFrontPage ()) {
      UiApplyAttributeList (
        (UI_CONTROL *)This,
        L"cellwidth='76' cellheight='65' layout='cellspnanning' bkimage='@FrontPageBkg' bkimagestyle='stretch' padding='70,76,70,76' "
        );
    } else {
      UiSetAttribute (This, L"vscrollbar", L"true");
    }
    break;

  case WM_KEYDOWN:
    if (!IsFrontPage ()) {
      switch (WParam) {

      case VK_RIGHT:
        return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, VK_RETURN, LParam);

      case VK_LEFT:
        if (!IsRootPage (gFB->CurrentP->PageId)) {
          ZeroMem (&KeyData, sizeof (EFI_KEY_DATA));
          KeyData.Key.ScanCode = SCAN_ESC;
          GdAddEfiKeyData (&KeyData);
          return 0;
        }
        break;

      case VK_F5:
        HotKeyFunc (HotkeyModifyValueDown);
        return 0;

      case VK_F6:
        HotKeyFunc (HotkeyModifyValueUp);
        return 0;

      default:
        break;
      }
    }
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
    break;


  case WM_SETFOCUS:
    gLastFocus = Wnd;
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);


  case FB_NOTIFY_SELECT_Q:
    H2OSetupPagePanelSelectQ (This);
    break;

  case FB_NOTIFY_REPAINT:
    RefreshMenuOptionList (This);
    break;

  case WM_DESTROY:
    FreeStatements (This);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);

  }
  return 0;

}


H2O_SETUP_PAGE_PANEL_CLASS *
EFIAPI
GetH2OSetupPagePanelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"H2OSetupPagePanel", (UI_CONTROL_CLASS *) GetListViewClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc = H2OSetupPagePanelProc;

  return CURRENT_CLASS;
}

