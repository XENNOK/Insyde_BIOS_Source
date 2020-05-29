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
#include "H2OPanels.h"
#include "MetroUi.h"

STATIC H2O_HELP_TEXT_PANEL_CLASS         *mH2OHelpTextPanelClass = NULL;
#define CURRENT_CLASS                    mH2OHelpTextPanelClass

VOID
CreateHelpTextPanelChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *HelpBkgImage;
  UI_CONTROL                    *HelpMenu;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *HelpTitle;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *HelpImage;
  UI_CONTROL                    *HelpText;


  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"float='true' height='match_parent' name='DummyVerticalLayout1' width='match_parent'");

  DummyControl1 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"name='DummyControl1'");

  HelpBkgImage = CreateControl (L"Texture", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, HelpBkgImage);
  UiApplyAttributeList (HelpBkgImage, L"bkimagestyle='stretch|gray' height='300' name='HelpBkgImage' width='300'");

  HelpMenu = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, HelpMenu);
  UiApplyAttributeList (HelpMenu, L"float='true' height='-1' name='HelpMenu' padding='40,30,40,30' width='-1'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", HelpMenu);
  CONTROL_CLASS(HelpMenu)->AddChild (HelpMenu, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='wrap_content' minheight='50' name='DummyHorizontalLayout1' padding='0,0,10,0'");

  HelpTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, HelpTitle);
  UiApplyAttributeList (HelpTitle, L"fontsize='29' height='wrap_content' name='HelpTitle' textcolor='0xFF268FB0' width='200'");

  DummyControl2 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"height='50' name='DummyControl2' padding='0,25,0,25'");

  HelpImage = CreateControl (L"Texture", DummyControl2);
  CONTROL_CLASS(DummyControl2)->AddChild (DummyControl2, HelpImage);
  UiApplyAttributeList (HelpImage, L"bkimagestyle='stretch' height='50' name='HelpImage' width='50'");

  HelpText = CreateControl (L"Label", HelpMenu);
  CONTROL_CLASS(HelpMenu)->AddChild (HelpMenu, HelpText);
  UiApplyAttributeList (HelpText, L"fontsize='19' name='HelpText' textcolor='0xFF666666' width='-1'");

}

EFI_STATUS
RefreshHelpMenu (
  H2O_HELP_TEXT_PANEL          *This
  )
{
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;
  H2O_FORM_BROWSER_Q            *CurrentQ;
  CHAR16                        Str[20];
  EFI_IMAGE_INPUT               *PageImage;

  Control   = (UI_CONTROL *) This;
  Manager   = Control->Manager;
  CurrentQ  = gFB->CurrentQ;

  if (CurrentQ != NULL && CurrentQ->Prompt != NULL) {
    Control = UiFindChildByName (This, L"HelpTitle");
    UiSetAttribute (Control, L"text", CurrentQ->Prompt);
  }

  PageImage = GetCurrentFormSetImage ();
  if (PageImage != NULL) {
    Control = UiFindChildByName (This, L"HelpImage");
    UnicodeSPrint (Str, sizeof (Str), L"0x%x",  PageImage);
    UiSetAttribute (Control, L"bkimage", Str);
    UiSetAttribute (Control, L"visible", L"true");
    Control = UiFindChildByName (This, L"HelpBkgImage");
    UiSetAttribute (Control, L"bkimage", Str);
    UiSetAttribute (Control, L"visible", L"true");
  } else {
    Control = UiFindChildByName (This, L"HelpImage");
    UiSetAttribute (Control, L"visible", L"false");
    Control = UiFindChildByName (This, L"HelpBkgImage");
    UiSetAttribute (Control, L"visible", L"false");
  }
  if (CurrentQ != NULL && CurrentQ->Help != NULL) {
    Control = UiFindChildByName (This, L"HelpText");
    UiSetAttribute (Control, L"text", CurrentQ->Help);
  }
  return EFI_SUCCESS;
}



LRESULT
EFIAPI
H2OHelpTextPanelProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_HELP_TEXT_PANEL           *This;
  UI_CONTROL                    *Control;


  This = (H2O_HELP_TEXT_PANEL *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *)This;
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_HELP_TEXT_PANEL *) AllocateZeroPool (sizeof (H2O_HELP_TEXT_PANEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateHelpTextPanelChilds (Control);
    break;

  case FB_NOTIFY_SELECT_Q:
  case FB_NOTIFY_REPAINT:
    RefreshHelpMenu (This);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  }
  return 0;

}


H2O_HELP_TEXT_PANEL_CLASS *
EFIAPI
GetH2OHelpTextPanelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"H2OHelpTextPanel", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }

  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc = H2OHelpTextPanelProc;

  return CURRENT_CLASS;
}

