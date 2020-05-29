/** @file
  UI One Of Input Dialog

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

#include "H2ODisplayEngineLocalMetro.h"
#include "UiControls.h"
#include "MetroUi.h"
#include <Library/ConsoleLib.h>

extern H2O_FORM_BROWSER_D       *mFbDialog;
extern BOOLEAN                  mIsSendForm;

VOID
FreeDialogEvent (
  IN H2O_FORM_BROWSER_D                    **DialogEvt
  );

UI_CONTROL *
CreateOneOfDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *OneOfDialog;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *OneOfDialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *OneOfDialogImage;
  UI_CONTROL                    *OneOfDialogText;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *OneOfList;


  OneOfDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, OneOfDialog);
  UiApplyAttributeList (OneOfDialog, L"bkcolor='@menucolor' name='OneOfDialog' padding='40,30,40,30'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", OneOfDialog);
  CONTROL_CLASS(OneOfDialog)->AddChild (OneOfDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='80' padding='0,0,30,0' name='DummyHorizontalLayout1'");

  OneOfDialogTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OneOfDialogTitle);
  UiApplyAttributeList (OneOfDialogTitle, L"fontsize='29' name='OneOfDialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"padding='0,25,0,25' name='DummyControl1'");

  OneOfDialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, OneOfDialogImage);
  UiApplyAttributeList (OneOfDialogImage, L"bkimagestyle='stretch|light' name='OneOfDialogImage'");

  OneOfDialogText = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OneOfDialogText);
  UiApplyAttributeList (OneOfDialogText, L"fontsize='19' name='OneOfDialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DummyControl2 = CreateControl (L"Control", OneOfDialog);
  CONTROL_CLASS(OneOfDialog)->AddChild (OneOfDialog, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' height='10' name='DummyControl2'");

  OneOfList = CreateControl (L"ListView", OneOfDialog);
  CONTROL_CLASS(OneOfDialog)->AddChild (OneOfDialog, OneOfList);
  UiApplyAttributeList (OneOfList, L"childpadding='2' name='OneOfList' padding='15,0,0,0' vscrollbar='true'");

  return OneOfDialog;
}

UI_CONTROL *
CreateOneOfDialogWithoutSendFormChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *OneOfDialogTitle;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *OneOfList;
  UI_CONTROL                    *FormHalo;


  DummyControl1 = CreateControl (L"Control", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"name='DummyControl1'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"bkcolor='@menucolor' childpadding='15' padding='20,30,20,30' name='DummyVerticalLayout1'");

  OneOfDialogTitle = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OneOfDialogTitle);
  UiApplyAttributeList (OneOfDialogTitle, L"fontsize='19' height='40' name='OneOfDialogTitle' textcolor='0xFFFFFFFF'");

  DummyControl2 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' bkimagestyle='center' height='10' name='DummyControl2'");

  OneOfList = CreateControl (L"ListView", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OneOfList);
  UiApplyAttributeList (OneOfList, L"childpadding='2' name='OneOfList' padding='0,0,10,0' vscrollbar='true'");

  FormHalo = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, FormHalo);
  UiApplyAttributeList (FormHalo, L"bkimage='@FormHalo' float='true' height='-1' name='FormHalo' scale9grid='23,26,22,31' width='-1'");

  return DummyControl1;
}

UI_CONTROL *
CreateOneOfOptionChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *Option;
  UI_CONTROL                    *OptionSelectedIcon;


  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='40' name='DummyHorizontalLayout1'");

  Option = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Option);
  UiApplyAttributeList (Option, L"align='center' align='singleline' bkcolor='0xFFE6E6E6' fontsize='19' height='40' name='Option' textcolor='0xFF404040'");

  OptionSelectedIcon = CreateControl (L"Texture", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OptionSelectedIcon);
  UiApplyAttributeList (OptionSelectedIcon, L"bkcolor='0x0' bkimage='@DialogSelectedIcon' float='true' name='OptionSelectedIcon' pos='260,12,281,29' visible='false'");

  return DummyHorizontalLayout1;
}


VOID
AdjustImage (
  UI_CONTROL                    *Control,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Src,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Dst
  )
{
  INT16                         HueDiff;
  INT8                          SaturationDiff;
  INT8                          ValueDiff;
  CHAR16                        Str[20];
  HSV_VALUE                     SrcHsv;
  HSV_VALUE                     DstHsv;

  RGB2HSV(Src, &SrcHsv);
  RGB2HSV(Dst, &DstHsv);

  if (DstHsv.Hue == 0 && DstHsv.Saturation == 0 && DstHsv.Value == 0) {
    return ;
  }
  HueDiff        = (INT16) (DstHsv.Hue - SrcHsv.Hue);
  SaturationDiff = (INT8)  (DstHsv.Saturation - SrcHsv.Saturation);
  ValueDiff      = (INT8)  (DstHsv.Value - SrcHsv.Value);
  UnicodeSPrint (Str, sizeof (Str), L"%d,%d,%d",  HueDiff, SaturationDiff, ValueDiff);
  UiSetAttribute (Control, L"hsvadjust", Str);
}



VOID
UiOneOfOptionOnSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  UI_CONTROL                    *Child;
  COLORREF                      Color;
  EFI_IMAGE_INPUT               *Image;

  if (!((SetState & UISTATE_SELECTED) || (ClearState & UISTATE_SELECTED))) {
    return ;
  }

  if (SetState & UISTATE_SELECTED) {
    Child = UiFindChildByName (Control, L"Option");
    UiApplyAttributeList (Child, L"textcolor='0xFFFFFFFF' bkcolor='@menulightcolor' ");
    Child = UiFindChildByName (Control, L"OptionSelectedIcon");
    ASSERT (Child != NULL);
    if (Child == NULL) {
      return;
    }
    if (mIsSendForm) {
      if (IsWindowVisible (Child->Wnd)) {
        Color = GetColorValue (L"0xFFFFFFFF");
        Image = GetImageByString (Child->BkImage);
        if (Image != NULL) {
          AdjustImage (Child, Image->Bitmap, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)&Color);
        }
      }
    } else {
      UiSetAttribute (Child, L"visible", L"true");
    }

  } else {
    Child = UiFindChildByName (Control, L"Option");
    UiApplyAttributeList (Child, L"textcolor='0xFF404040' bkcolor='0xFFE6E6E6' ");
    Child = UiFindChildByName (Control, L"OptionSelectedIcon");
    if (mIsSendForm) {
      if (IsWindowVisible (Child->Wnd)) {
        Color = GetColorValue (L"@menulightcolor");
        Image = GetImageByString (Child->BkImage);
        if (Image != NULL) {
          AdjustImage (Child, Image->Bitmap, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)&Color);
        }
      }
    } else {
      UiSetAttribute (Child, L"visible", L"false");
    }
  }
}

VOID
UiOneOfOptionOnItemClick (
  UI_LIST_VIEW                  *This,
  UI_CONTROL                    *Item,
  UINT32                        Index
  )
{
  Index = (UINT32) GetWindowLongPtr (Item->Wnd, GWLP_USERDATA);
  SendChangeQNotify (0, 0, &mFbDialog->BodyHiiValueArray[Index]);
}

LRESULT
OneOfInputProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_CONTROL                    *Layout;
  UI_DIALOG                     *Dialog;
  UI_MANAGER                    *Manager;
  UINTN                         Index;
  UI_CONTROL                    *Control;
  CHAR16                        Str[20];
  EFI_IMAGE_INPUT               *FormsetImage;
  UI_CONTROL                    *CreatedControl;
  INTN                          Result;
  EFI_STATUS                    Status;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  Manager = (UI_MANAGER *)((UI_CONTROL *)Dialog)->Manager;

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (mFbDialog->TitleString != NULL) {
      Control = UiFindChildByName (Dialog, L"OneOfDialogTitle");
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
    }

    if (gFB->CurrentQ != NULL && gFB->CurrentQ->Help != NULL) {
      Control = UiFindChildByName (Dialog, L"OneOfDialogText");
      UiSetAttribute (Control, L"text", gFB->CurrentQ->Help);
    }

    FormsetImage = GetCurrentFormSetImage ();
    if (FormsetImage != NULL) {
      Control = UiFindChildByName (Dialog, L"OneOfDialogImage");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", FormsetImage);
      UiSetAttribute (Control, L"bkimage", Str);
      UiSetAttribute (Control, L"visible", L"true");
    }

    Layout = UiFindChildByName (Dialog, L"OneOfList");
    if (Layout->VScrollBar != NULL) {
      UiSetAttribute (Layout->VScrollBar, L"displayscrollbar", L"false");
      CONTROL_CLASS_INVALIDATE (Layout);
    }

    ((UI_LIST_VIEW *)Layout)->OnItemClick = UiOneOfOptionOnItemClick;
    for (Index = 0, Result = 1; Index < mFbDialog->BodyStringCount; Index++) {
      CreatedControl = CreateOneOfOptionChilds (Layout);

      SetWindowLongPtr (CreatedControl->Wnd, GWLP_USERDATA, (INTN) Index);
      CreatedControl->OnSetState = UiOneOfOptionOnSetState;

      Control = UiFindChildByName (CreatedControl, L"Option");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[Index]);

      Status = CompareHiiValue (&mFbDialog->BodyHiiValueArray[Index], &mFbDialog->ConfirmHiiValue, &Result);
      if (!EFI_ERROR (Status) && Result == 0) {
        Control = UiFindChildByName (CreatedControl, L"OptionSelectedIcon");
        UiSetAttribute (Control, L"visible", L"true");

        LIST_VIEW_CLASS (Layout)->SetSelection (
                                  (UI_LIST_VIEW *)Layout,
                                  (INT32)Index,
                                  TRUE
                                  );
      }
    }
    SetFocus (Layout->Wnd);
    break;

  case WM_HOTKEY:
    if (HIWORD(LParam) == VK_ESCAPE) {
      SendShutDNotify ();
      return 0;
    }
    return 1;

  case WM_DESTROY:
    FreeDialogEvent (&mFbDialog);
    return 0;

  default:
    return 0;
  }


  return 1;
}
