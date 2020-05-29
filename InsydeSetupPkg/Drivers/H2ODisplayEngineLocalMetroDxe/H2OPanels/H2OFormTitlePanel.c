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

STATIC H2O_FORM_TITLE_PANEL_CLASS        *mH2OFormTitlePanelClass = NULL;
#define CURRENT_CLASS                    mH2OFormTitlePanelClass

UI_CONTROL *
CreateSetupTitlePanelChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyTexture1;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *TitleImage;
  UI_CONTROL                    *TitleName;


  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"bkcolor='0xFF333333' height='-1' name='DummyHorizontalLayout1'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"float='true' height='-1' width='-1' name='DummyHorizontalLayout2'");

  DummyTexture1 = CreateControl (L"Texture", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyTexture1);
  UiApplyAttributeList (DummyTexture1, L"bkimage='@FormTitleBkg' bkimagestyle='stretch' name='DummyTexture1'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"height='65' padding='13,0,15,30' width='67' name='DummyControl1'");

  TitleImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, TitleImage);
  UiApplyAttributeList (TitleImage, L"bkimagestyle='stretch' name='TitleImage'");

  TitleName = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, TitleName);
  UiApplyAttributeList (TitleName, L"fontsize='30' name='TitleName' padding='15,0,15,8' textcolor='0xFFFFFFFF' width='1500'");

  return DummyHorizontalLayout1;
}

EFI_STATUS
RefreshFormTitleMenu (
  H2O_FORM_TITLE_PANEL          *This
  )
{

  EFI_IMAGE_INPUT               *PageImage;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;
  CHAR16                        *FormSetTitle;
  CHAR16                        *MenuTitle;
  CHAR16                        Str[20];

  Control   = (UI_CONTROL *) This;
  Manager   = Control->Manager;
  PageImage = GetCurrentFormSetImage ();
  Control = Manager->FindControlByName (Manager, L"TitleImage");
  if (PageImage != NULL) {
    UnicodeSPrint (Str, sizeof (Str), L"0x%x",  PageImage);
    UiSetAttribute (Control, L"bkimage", Str);
    UiSetAttribute (Control, L"visible", L"true");
  } else {
    UiSetAttribute (Control, L"visible", L"false");
  }

  Control = Manager->FindControlByName (Manager, L"TitleName");
  FormSetTitle = GetCurrentFormSetTitle ();
  if (gFB->CurrentQ == NULL) {
    UiSetAttribute (Control, L"text", L"");
  } else {
    if (!IsRootPage (gFB->CurrentQ->PageId)) {
      MenuTitle = CatSPrint (NULL, L"%s > %s", FormSetTitle, gFB->CurrentP->PageTitle);
      UiSetAttribute (Control, L"text", MenuTitle);
      FreePool (MenuTitle);
    } else {
      UiSetAttribute (Control, L"text", FormSetTitle);
    }
  }

  return EFI_SUCCESS;
}

LRESULT
EFIAPI
H2OFormTitlePanelProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_FORM_TITLE_PANEL          *This;
  UI_CONTROL                    *Control;

  This = (H2O_FORM_TITLE_PANEL *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *)This;

  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_FORM_TITLE_PANEL *) AllocateZeroPool (sizeof (H2O_FORM_TITLE_PANEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateSetupTitlePanelChilds (Control);
    break;

  case FB_NOTIFY_REPAINT:
    if (This != NULL) {
      RefreshFormTitleMenu (This);
    }
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  }
  return 0;

}


H2O_FORM_TITLE_PANEL_CLASS *
EFIAPI
GetH2OFormTitlePanelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"H2OFormTitlePanel", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }


  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc = H2OFormTitlePanelProc;

  return CURRENT_CLASS;
}

