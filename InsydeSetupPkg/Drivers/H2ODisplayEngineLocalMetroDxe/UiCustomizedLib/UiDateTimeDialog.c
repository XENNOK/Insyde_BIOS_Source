/** @file
  Date and time dialog function implementation

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
#include "UiCustomizedControls.h"
#include "MetroUi.h"
#include <Library/ConsoleLib.h>

EFI_TIME                        mEditTime;

UI_CONTROL *
CreateTimeDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DateTimeDialog;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DialogImage;
  UI_CONTROL                    *DialogText;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *Time;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *ok;
  UI_CONTROL                    *cancel;


  DateTimeDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DateTimeDialog);
  UiApplyAttributeList (DateTimeDialog, L"bkcolor='@menucolor' name='DateTimeDialog' padding='40,30,40,30'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='80' padding='0,0,30,0' name='DummyHorizontalLayout1'");

  DialogTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DialogTitle);
  UiApplyAttributeList (DialogTitle, L"fontsize='29' name='DialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"padding='0,25,0,25' name='DummyControl1'");

  DialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, DialogImage);
  UiApplyAttributeList (DialogImage, L"bkimagestyle='stretch|light' name='DialogImage'");

  DialogText = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogText);
  UiApplyAttributeList (DialogText, L"fontsize='19' name='DialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DummyControl2 = CreateControl (L"Control", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' height='2' name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"height='15' name='DummyControl3'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"name='DummyVerticalLayout2'");

  Time = CreateControl (L"TimeItem", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, Time);
  UiApplyAttributeList (Time, L"height='156' name='Time'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"height='12' name='DummyControl4'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"childpadding='2' height='30' width='280' name='DummyHorizontalLayout2'");

  DummyControl5 = CreateControl (L"Control", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"name='DummyControl5'");

  ok = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, ok);
  UiApplyAttributeList (ok, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='ok' text='Enter' textcolor='0xFFFFFFFF' width='55'");

  cancel = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, cancel);
  UiApplyAttributeList (cancel, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='cancel' text='Close' textcolor='0xFFFFFFFF' width='55'");

  return DateTimeDialog;
}

UI_CONTROL *
CreateDateDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DateTimeDialog;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DialogImage;
  UI_CONTROL                    *DialogText;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *Date;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *ok;
  UI_CONTROL                    *cancel;


  DateTimeDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DateTimeDialog);
  UiApplyAttributeList (DateTimeDialog, L"bkcolor='@menucolor' name='DateTimeDialog' padding='40,30,40,30'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='80' padding='0,0,30,0' name='DummyHorizontalLayout1'");

  DialogTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DialogTitle);
  UiApplyAttributeList (DialogTitle, L"fontsize='29' name='DialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"padding='0,25,0,25' name='DummyControl1'");

  DialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, DialogImage);
  UiApplyAttributeList (DialogImage, L"bkimagestyle='stretch|light' name='DialogImage'");

  DialogText = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogText);
  UiApplyAttributeList (DialogText, L"fontsize='19' name='DialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DummyControl2 = CreateControl (L"Control", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' height='2' name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"height='15' name='DummyControl3'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DateTimeDialog);
  CONTROL_CLASS(DateTimeDialog)->AddChild (DateTimeDialog, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"name='DummyVerticalLayout2'");

  Date = CreateControl (L"DateItem", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, Date);
  UiApplyAttributeList (Date, L"height='156' name='Date'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"height='12' name='DummyControl4'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"childpadding='2' height='30' width='286' name='DummyHorizontalLayout2'");

  DummyControl5 = CreateControl (L"Control", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"name='DummyControl5'");

  ok = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, ok);
  UiApplyAttributeList (ok, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='ok' text='Enter' textcolor='0xFFFFFFFF' width='55'");

  cancel = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, cancel);
  UiApplyAttributeList (cancel, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='cancel' text='Close' textcolor='0xFFFFFFFF' width='55'");

  return DateTimeDialog;
}

EFI_STATUS
SetHiiTimeToEfiTime (
  IN  EFI_HII_VALUE                        *HiiValue,
  OUT EFI_TIME                             *EfiTime
  )
{
  if (HiiValue == NULL || EfiTime == NULL || (HiiValue->Type != EFI_IFR_TYPE_TIME && HiiValue->Type != EFI_IFR_TYPE_DATE)) {
    return EFI_INVALID_PARAMETER;
  }

  if (HiiValue->Type == EFI_IFR_TYPE_TIME) {
    EfiTime->Hour   = HiiValue->Value.time.Hour;
    EfiTime->Minute = HiiValue->Value.time.Minute;
    EfiTime->Second = HiiValue->Value.time.Second;
  } else {
    EfiTime->Year  = HiiValue->Value.date.Year;
    EfiTime->Month = HiiValue->Value.date.Month;
    EfiTime->Day   = HiiValue->Value.date.Day;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SetEfiTimeToHiiTime (
  IN     EFI_TIME                          *EfiTime,
  IN OUT EFI_HII_VALUE                     *HiiValue
  )
{
  if (HiiValue == NULL || EfiTime == NULL || (HiiValue->Type != EFI_IFR_TYPE_TIME && HiiValue->Type != EFI_IFR_TYPE_DATE)) {
    return EFI_INVALID_PARAMETER;
  }

  if (HiiValue->Type == EFI_IFR_TYPE_TIME) {
    HiiValue->Value.time.Hour   = EfiTime->Hour;
    HiiValue->Value.time.Minute = EfiTime->Minute;
    HiiValue->Value.time.Second = EfiTime->Second;
  } else {
    HiiValue->Value.date.Year  = EfiTime->Year;
    HiiValue->Value.date.Month = EfiTime->Month;
    HiiValue->Value.date.Day   = EfiTime->Day;
  }

  return EFI_SUCCESS;
}

VOID
DateItemOnDateChange (
  UI_DATE_ITEM                  *This,
  EFI_TIME                      *EfiTime
  )
{
  CopyMem (&mEditTime, EfiTime, sizeof (EFI_TIME));
}

VOID
TimeItemOnTimeChange (
  UI_TIME_ITEM                  *This,
  EFI_TIME                      *EfiTime
  )
{
  CopyMem (&mEditTime, EfiTime, sizeof (EFI_TIME));
}

INTN
DateTimeDialogProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_MANAGER                    *Manager;
  H2O_FORM_BROWSER_Q            *CurrentQ;
  UI_CONTROL                    *Control;
  UI_CONTROL                    *ChildControl;
  CHAR16                        Str[20];
  EFI_IMAGE_INPUT               *FormsetImage;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  Manager = (UI_MANAGER *) ((UI_CONTROL *) Dialog)->Manager;

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    CurrentQ = gFB->CurrentQ;
    SetHiiTimeToEfiTime (&CurrentQ->HiiValue, &mEditTime);

    if (CurrentQ->Prompt != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogTitle");
      UiSetAttribute (Control, L"text", CurrentQ->Prompt);
    }
    if (CurrentQ->Help != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogText");
      UiSetAttribute (Control, L"text", CurrentQ->Help);
    }

    FormsetImage = GetCurrentFormSetImage ();
    if (FormsetImage != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogImage");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", FormsetImage);
      UiSetAttribute (Control, L"bkimage", Str);
      UiSetAttribute (Control, L"visible", L"true");
    }

    if (CurrentQ->HiiValue.Type == EFI_IFR_TYPE_DATE) {
      ChildControl = UiFindChildByName (Dialog, L"Date");
      ((UI_DATE_ITEM *)ChildControl)->OnDateChange = DateItemOnDateChange;
    } else {
      ChildControl = UiFindChildByName (Dialog, L"Time");
      ((UI_TIME_ITEM *)ChildControl)->OnTimeChange = TimeItemOnTimeChange;
    }
    UnicodeSPrint (Str, sizeof (Str), L"0x%x", &mEditTime);
    UiSetAttribute (ChildControl, L"efitime", Str);
    SetFocus (ChildControl->Wnd);
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
  case UI_NOTIFY_CLICK:
    CurrentQ = gFB->CurrentQ;
    Control = (UI_CONTROL *) WParam;
    if (StrCmp (Control->Name, L"Time") == 0 ||
        StrCmp (Control->Name, L"Date") == 0) {
      Control = UiFindChildByName (Dialog, L"ok");
      SetFocus (Control->Wnd);
    } else if (StrCmp (Control->Name, L"ok") == 0) {
      SetEfiTimeToHiiTime (&mEditTime, &CurrentQ->HiiValue);
      SendChangeQNotify (CurrentQ->PageId, CurrentQ->QuestionId, &CurrentQ->HiiValue);
    } else if (StrCmp (Control->Name, L"cancel") == 0) {
      SendShutDNotify ();
    }
    break;

  case WM_HOTKEY:
    if (HIWORD(LParam) == VK_ESCAPE) {
      SendShutDNotify ();
      return 1;
    }
    return 0;

  default:
    return 0;
  }

  return 1;
}

