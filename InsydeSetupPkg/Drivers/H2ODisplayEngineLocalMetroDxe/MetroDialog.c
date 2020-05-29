/** @file
  Entry point and initial functions for H2O local Metro display engine driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
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

extern H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA      *mMetroPrivate;
extern HWND                                       gWnd;
extern EFI_ABSOLUTE_POINTER_STATE                 mPreviousAbsPtrState;

HWND                            mDialogWnd = NULL;
HWND                            mOverlayWnd = NULL;
H2O_FORM_BROWSER_D              *mFbDialog;
BOOLEAN                         mTitleVisible;
BOOLEAN                         mIsSendForm;

UI_CONTROL *
CreateOneOfDialogChilds (
  UI_CONTROL *Control
  );

UI_CONTROL *
CreateOrderedListDialogChilds (
  UI_CONTROL *Control
  );

UI_CONTROL *
CreateOneOfDialogWithoutSendFormChilds (
  UI_CONTROL *Control
  );


UI_CONTROL *
CreateTimeDialogChilds (
  UI_CONTROL *Control
  );

UI_CONTROL *
CreateDateDialogChilds (
  UI_CONTROL *Control
  );

UI_CONTROL *
CreateCommonDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *PopUpDialog;
  UI_CONTROL                    *TitleLayout;
  UI_CONTROL                    *DialogTitle;
  UI_CONTROL                    *DialogText;
  UI_CONTROL                    *DialogSeparator;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DialogPasswordInput;
  UI_CONTROL                    *ConfirmNewPasswordLabel;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *ConfirmPasswordInput;
  UI_CONTROL                    *DialogButtonList;


  PopUpDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, PopUpDialog);
  UiApplyAttributeList (PopUpDialog, L"bkcolor='@menucolor' name='PopUpDialog' padding='40,30,40,30'");

  TitleLayout = CreateControl (L"VerticalLayout", PopUpDialog);
  CONTROL_CLASS(PopUpDialog)->AddChild (PopUpDialog, TitleLayout);
  UiApplyAttributeList (TitleLayout, L"name='TitleLayout'");

  DialogTitle = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DialogTitle);
  UiApplyAttributeList (DialogTitle, L"fontsize='29' height='110' name='DialogTitle' padding='0,0,30,0' textcolor='0xFFFFFFFF'");

  DialogText = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DialogText);
  UiApplyAttributeList (DialogText, L"fontsize='19' name='DialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DialogSeparator = CreateControl (L"Control", PopUpDialog);
  CONTROL_CLASS(PopUpDialog)->AddChild (PopUpDialog, DialogSeparator);
  UiApplyAttributeList (DialogSeparator, L"bkcolor='0x0' bkimage='@DialogSeparator' height='10' name='DialogSeparator'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", PopUpDialog);
  CONTROL_CLASS(PopUpDialog)->AddChild (PopUpDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DialogPasswordInput = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogPasswordInput);
  UiApplyAttributeList (DialogPasswordInput, L"height='51' name='DialogPasswordInput' padding='10,0,0,0' visible='false'");

  ConfirmNewPasswordLabel = CreateControl (L"Label", DialogPasswordInput);
  CONTROL_CLASS(DialogPasswordInput)->AddChild (DialogPasswordInput, ConfirmNewPasswordLabel);
  UiApplyAttributeList (ConfirmNewPasswordLabel, L"bkcolor='0xFFF2F2F2' fontsize='16' height='41' name='ConfirmNewPasswordLabel' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DialogPasswordInput);
  CONTROL_CLASS(DialogPasswordInput)->AddChild (DialogPasswordInput, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"bkcolor='0xFFF2F2F2' height='41' padding='6,8,6,8' name='DummyVerticalLayout2'");

  DummyControl1 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"bkcolor='0xFF999999' height='29' padding='1,1,1,1' name='DummyControl1'");

  ConfirmPasswordInput = CreateControl (L"UiEdit", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, ConfirmPasswordInput);
  UiApplyAttributeList (ConfirmPasswordInput, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='ConfirmPasswordInput' padding='7,3,0,3' password='true'");

  DialogButtonList = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogButtonList);
  UiApplyAttributeList (DialogButtonList, L"childpadding='2' name='DialogButtonList' padding='10,0,0,0'");

  return PopUpDialog;
}

UI_CONTROL *
CreateHelpDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *HelpDialog;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *HelpDialogText;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *HelpDialogButton;
  UI_CONTROL                    *DummyControl2;


  HelpDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, HelpDialog);
  UiApplyAttributeList (HelpDialog, L"bkcolor='@menucolor' name='HelpDialog' padding='20,20,0,20'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", HelpDialog);
  CONTROL_CLASS(HelpDialog)->AddChild (HelpDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"padding='0,0,20,0' vscrollbar='false' name='DummyVerticalLayout1'");

  HelpDialogText = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, HelpDialogText);
  UiApplyAttributeList (HelpDialogText, L"fontsize='19' name='HelpDialogText' textcolor='0xFFFFFFFF'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", HelpDialog);
  CONTROL_CLASS(HelpDialog)->AddChild (HelpDialog, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='50' name='DummyHorizontalLayout1'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"name='DummyControl1'");

  HelpDialogButton = CreateControl (L"Button", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, HelpDialogButton);
  UiApplyAttributeList (HelpDialogButton, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='27' height='30' name='HelpDialogButton' text='OK' textcolor='0xFFFFFFFF' width='55'");

  DummyControl2 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"name='DummyControl2'");

  return HelpDialog;
}

UI_CONTROL *
CreateNumericDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *NumericDialog;
  UI_CONTROL                    *TitleLayout;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *NumericDialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *NumericDialogImage;
  UI_CONTROL                    *NumericDialogText;
  UI_CONTROL                    *DialogSeparator;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *NumericLabel;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *InputNumericText;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *DummyControl6;
  UI_CONTROL                    *DummyHorizontalLayout3;
  UI_CONTROL                    *DummyControl7;
  UI_CONTROL                    *Ok;
  UI_CONTROL                    *Cancel;


  NumericDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, NumericDialog);
  UiApplyAttributeList (NumericDialog, L"bkcolor='@menucolor' name='NumericDialog' padding='40,30,40,30'");

  TitleLayout = CreateControl (L"VerticalLayout", NumericDialog);
  CONTROL_CLASS(NumericDialog)->AddChild (NumericDialog, TitleLayout);
  UiApplyAttributeList (TitleLayout, L"name='TitleLayout'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='110' padding='0,0,30,0' name='DummyHorizontalLayout1'");

  NumericDialogTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, NumericDialogTitle);
  UiApplyAttributeList (NumericDialogTitle, L"fontsize='29' name='NumericDialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"padding='0,25,30,25' name='DummyControl1'");

  NumericDialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, NumericDialogImage);
  UiApplyAttributeList (NumericDialogImage, L"bkimagestyle='stretch|light' name='NumericDialogImage'");

  NumericDialogText = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, NumericDialogText);
  UiApplyAttributeList (NumericDialogText, L"fontsize='19' name='NumericDialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DialogSeparator = CreateControl (L"Control", NumericDialog);
  CONTROL_CLASS(NumericDialog)->AddChild (NumericDialog, DialogSeparator);
  UiApplyAttributeList (DialogSeparator, L"bkcolor='0x0' bkimage='@DialogSeparator' height='10' name='DialogSeparator'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", NumericDialog);
  CONTROL_CLASS(NumericDialog)->AddChild (NumericDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"bkcolor='0xFFF2F2F2' height='41' width='300' name='DummyHorizontalLayout2'");

  NumericLabel = CreateControl (L"Label", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, NumericLabel);
  UiApplyAttributeList (NumericLabel, L"fontsize='16' height='41' name='NumericLabel' padding='12,0,0,0' text='input number:' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"height='41' padding='6,8,6,0' width='181' name='DummyVerticalLayout2'");

  DummyControl2 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"bkcolor='0xFF999999' height='29' padding='1,1,1,1' width='173' name='DummyControl3'");

  InputNumericText = CreateControl (L"UiEdit", DummyControl3);
  CONTROL_CLASS(DummyControl3)->AddChild (DummyControl3, InputNumericText);
  UiApplyAttributeList (InputNumericText, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='InputNumericText' valuetype='dec' width='171'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"name='DummyControl4'");

  DummyControl5 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"height='2' name='DummyControl5'");

  DummyControl6 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl6);
  UiApplyAttributeList (DummyControl6, L"height='10' name='DummyControl6'");

  DummyHorizontalLayout3 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout3);
  UiApplyAttributeList (DummyHorizontalLayout3, L"childpadding='2' height='55' padding='0,0,30,0' width='300' name='DummyHorizontalLayout3'");

  DummyControl7 = CreateControl (L"Control", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl7);
  UiApplyAttributeList (DummyControl7, L"name='DummyControl7'");

  Ok = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, Ok);
  UiApplyAttributeList (Ok, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Ok' text='Enter' textcolor='0xFFFFFFFF' width='55'");

  Cancel = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, Cancel);
  UiApplyAttributeList (Cancel, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Cancel' text='Cancel' textcolor='0xFFFFFFFF' width='55'");

  return NumericDialog;
}

UI_CONTROL *
CreateNumericDialogWithoutSendFormChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *NumericDialogWithoutSendForm;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *TitleLayout;
  UI_CONTROL                    *NumericDialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DialogPasswordInput;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *InputNumericText;
  UI_CONTROL                    *DialogButtonList;
  UI_CONTROL                    *FormHalo;


  NumericDialogWithoutSendForm = CreateControl (L"Control", Root);
  CONTROL_CLASS(Root)->AddChild (Root, NumericDialogWithoutSendForm);
  UiApplyAttributeList (NumericDialogWithoutSendForm, L"name='NumericDialogWithoutSendForm'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", NumericDialogWithoutSendForm);
  CONTROL_CLASS(NumericDialogWithoutSendForm)->AddChild (NumericDialogWithoutSendForm, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"bkcolor='@menucolor' name='DummyVerticalLayout1' padding='20,30,20,30'");

  TitleLayout = CreateControl (L"VerticalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, TitleLayout);
  UiApplyAttributeList (TitleLayout, L"name='TitleLayout'");

  NumericDialogTitle = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, NumericDialogTitle);
  UiApplyAttributeList (NumericDialogTitle, L"align='center' fontsize='19' height='40' name='NumericDialogTitle' textcolor='0xFFFFFFFF'");

  DummyControl1 = CreateControl (L"Control", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"height='15' name='DummyControl1'");

  DummyControl2 = CreateControl (L"Control", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' bkimagestyle='center' height='10' name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"height='15' name='DummyControl3'");

  DialogPasswordInput = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogPasswordInput);
  UiApplyAttributeList (DialogPasswordInput, L"height='51' name='DialogPasswordInput' padding='0,0,10,0'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DialogPasswordInput);
  CONTROL_CLASS(DialogPasswordInput)->AddChild (DialogPasswordInput, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"bkcolor='0xFFF2F2F2' height='41' name='DummyVerticalLayout2' padding='6,8,6,8'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"bkcolor='0xFF999999' height='29' name='DummyControl4' padding='1,1,1,1'");

  InputNumericText = CreateControl (L"UiEdit", DummyControl4);
  CONTROL_CLASS(DummyControl4)->AddChild (DummyControl4, InputNumericText);
  UiApplyAttributeList (InputNumericText, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='InputNumericText' padding='7,3,0,3'");

  DialogButtonList = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogButtonList);
  UiApplyAttributeList (DialogButtonList, L"childpadding='2' height='30' name='DialogButtonList' visible='false'");

  FormHalo = CreateControl (L"Texture", NumericDialogWithoutSendForm);
  CONTROL_CLASS(NumericDialogWithoutSendForm)->AddChild (NumericDialogWithoutSendForm, FormHalo);
  UiApplyAttributeList (FormHalo, L"bkimage='@FormHalo' float='true' height='-1' name='FormHalo' scale9grid='23,26,22,31' width='-1'");

  return NumericDialogWithoutSendForm;
}

UI_CONTROL *
CreatePasswordDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *PasswordDialog;
  UI_CONTROL                    *TitleLayout;
  UI_CONTROL                    *TitleLayout2;
  UI_CONTROL                    *PasswordDialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *PasswordDialogImage;
  UI_CONTROL                    *PasswordDialogText;
  UI_CONTROL                    *DialogSeparator;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *OldPassword;
  UI_CONTROL                    *OldPasswordLabel;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *OldPasswordText;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *OldPasswordSeperator;
  UI_CONTROL                    *NewPassword;
  UI_CONTROL                    *NewPasswordLabel;
  UI_CONTROL                    *DummyVerticalLayout3;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *DummyControl6;
  UI_CONTROL                    *NewPasswordText;
  UI_CONTROL                    *DummyControl7;
  UI_CONTROL                    *NewPasswordSeperator;
  UI_CONTROL                    *ConfirmNewPassword;
  UI_CONTROL                    *ConfirmNewPasswordLabel;
  UI_CONTROL                    *DummyVerticalLayout4;
  UI_CONTROL                    *DummyControl9;
  UI_CONTROL                    *DummyControl10;
  UI_CONTROL                    *ConfirmNewPasswordText;
  UI_CONTROL                    *DummyControl11;
  UI_CONTROL                    *ConfirmNewPasswordSeperator;
  UI_CONTROL                    *DummyControl13;
  UI_CONTROL                    *DummyHorizontalLayout3;
  UI_CONTROL                    *DummyControl14;
  UI_CONTROL                    *Ok;
  UI_CONTROL                    *Cancel;

  PasswordDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, PasswordDialog);
  UiApplyAttributeList (PasswordDialog, L"bkcolor='@menucolor' name='PasswordDialog' padding='40,30,40,30'");

  TitleLayout = CreateControl (L"VerticalLayout", PasswordDialog);
  CONTROL_CLASS(PasswordDialog)->AddChild (PasswordDialog, TitleLayout);
  UiApplyAttributeList (TitleLayout, L"name='TitleLayout'");

  TitleLayout2 = CreateControl (L"HorizontalLayout", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, TitleLayout2);
  UiApplyAttributeList (TitleLayout2, L"height='110' name='TitleLayout2' padding='0,0,30,0'");

  PasswordDialogTitle = CreateControl (L"Label", TitleLayout2);
  CONTROL_CLASS(TitleLayout2)->AddChild (TitleLayout2, PasswordDialogTitle);
  UiApplyAttributeList (PasswordDialogTitle, L"fontsize='29' name='PasswordDialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", TitleLayout2);
  CONTROL_CLASS(TitleLayout2)->AddChild (TitleLayout2, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"padding='0,25,30,25' name='DummyControl1'");

  PasswordDialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, PasswordDialogImage);
  UiApplyAttributeList (PasswordDialogImage, L"bkimagestyle='stretch|light' name='PasswordDialogImage'");

  PasswordDialogText = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, PasswordDialogText);
  UiApplyAttributeList (PasswordDialogText, L"fontsize='19' name='PasswordDialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DialogSeparator = CreateControl (L"Control", PasswordDialog);
  CONTROL_CLASS(PasswordDialog)->AddChild (PasswordDialog, DialogSeparator);
  UiApplyAttributeList (DialogSeparator, L"bkcolor='0x0' bkimage='@DialogSeparator' height='10' name='DialogSeparator'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", PasswordDialog);
  CONTROL_CLASS(PasswordDialog)->AddChild (PasswordDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  OldPassword = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OldPassword);
  UiApplyAttributeList (OldPassword, L"bkcolor='0xFFF2F2F2' height='41' name='OldPassword' width='300'");

  OldPasswordLabel = CreateControl (L"Label", OldPassword);
  CONTROL_CLASS(OldPassword)->AddChild (OldPassword, OldPasswordLabel);
  UiApplyAttributeList (OldPasswordLabel, L"fontsize='16' height='41' name='OldPasswordLabel' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", OldPassword);
  CONTROL_CLASS(OldPassword)->AddChild (OldPassword, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"height='41' padding='6,8,6,0' width='181' name='DummyVerticalLayout2'");

  DummyControl2 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"bkcolor='0xFF999999' height='29' padding='1,1,1,1' width='173' name='DummyControl3'");

  OldPasswordText = CreateControl (L"UiEdit", DummyControl3);
  CONTROL_CLASS(DummyControl3)->AddChild (DummyControl3, OldPasswordText);
  UiApplyAttributeList (OldPasswordText, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='OldPasswordText' padding='7,3,0,3' password='true' width='171'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"name='DummyControl4'");

  OldPasswordSeperator = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OldPasswordSeperator);
  UiApplyAttributeList (OldPasswordSeperator, L"height='2' name='OldPasswordSeperator'");

  NewPassword = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, NewPassword);
  UiApplyAttributeList (NewPassword, L"bkcolor='0xFFF2F2F2' height='41' name='NewPassword' width='300'");

  NewPasswordLabel = CreateControl (L"Label", NewPassword);
  CONTROL_CLASS(NewPassword)->AddChild (NewPassword, NewPasswordLabel);
  UiApplyAttributeList (NewPasswordLabel, L"fontsize='16' height='41' name='NewPasswordLabel' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout3 = CreateControl (L"VerticalLayout", NewPassword);
  CONTROL_CLASS(NewPassword)->AddChild (NewPassword, DummyVerticalLayout3);
  UiApplyAttributeList (DummyVerticalLayout3, L"height='41' padding='6,8,6,0' width='181' name='DummyVerticalLayout3'");

  DummyControl5 = CreateControl (L"Control", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"name='DummyControl5'");

  DummyControl6 = CreateControl (L"Control", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, DummyControl6);
  UiApplyAttributeList (DummyControl6, L"bkcolor='0xFF999999' height='29' padding='1,1,1,1' width='173' name='DummyControl6'");

  NewPasswordText = CreateControl (L"UiEdit", DummyControl6);
  CONTROL_CLASS(DummyControl6)->AddChild (DummyControl6, NewPasswordText);
  UiApplyAttributeList (NewPasswordText, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='NewPasswordText' padding='7,3,0,3' password='true' width='171'");

  DummyControl7 = CreateControl (L"Control", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, DummyControl7);
  UiApplyAttributeList (DummyControl7, L"name='DummyControl7'");

  NewPasswordSeperator = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, NewPasswordSeperator);
  UiApplyAttributeList (NewPasswordSeperator, L"height='2' name='NewPasswordSeperator'");

  ConfirmNewPassword = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, ConfirmNewPassword);
  UiApplyAttributeList (ConfirmNewPassword, L"bkcolor='0xFFF2F2F2' height='41' width='300' name='ConfirmNewPassword'");

  ConfirmNewPasswordLabel = CreateControl (L"Label", ConfirmNewPassword);
  CONTROL_CLASS(ConfirmNewPassword)->AddChild (ConfirmNewPassword, ConfirmNewPasswordLabel);
  UiApplyAttributeList (ConfirmNewPasswordLabel, L"fontsize='16' height='41' name='ConfirmNewPasswordLabel' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout4 = CreateControl (L"VerticalLayout", ConfirmNewPassword);
  CONTROL_CLASS(ConfirmNewPassword)->AddChild (ConfirmNewPassword, DummyVerticalLayout4);
  UiApplyAttributeList (DummyVerticalLayout4, L"height='41' padding='6,8,6,0' width='181' name='DummyVerticalLayout4'");

  DummyControl9 = CreateControl (L"Control", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, DummyControl9);
  UiApplyAttributeList (DummyControl9, L"name='DummyControl9'");

  DummyControl10 = CreateControl (L"Control", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, DummyControl10);
  UiApplyAttributeList (DummyControl10, L"bkcolor='0xFF999999' height='29' padding='1,1,1,1' width='173' name='DummyControl10'");

  ConfirmNewPasswordText = CreateControl (L"UiEdit", DummyControl10);
  CONTROL_CLASS(DummyControl10)->AddChild (DummyControl10, ConfirmNewPasswordText);
  UiApplyAttributeList (ConfirmNewPasswordText, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='ConfirmNewPasswordText' padding='7,3,0,3' password='true' width='171'");

  DummyControl11 = CreateControl (L"Control", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, DummyControl11);
  UiApplyAttributeList (DummyControl11, L"name='DummyControl11'");

  ConfirmNewPasswordSeperator = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, ConfirmNewPasswordSeperator);
  UiApplyAttributeList (ConfirmNewPasswordSeperator, L"height='2' name='ConfirmNewPasswordSeperator'");

  DummyControl13 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl13);
  UiApplyAttributeList (DummyControl13, L"height='10' name='DummyControl13'");

  DummyHorizontalLayout3 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout3);
  UiApplyAttributeList (DummyHorizontalLayout3, L"childpadding='2' height='30' width='300' name='DummyHorizontalLayout3'");

  DummyControl14 = CreateControl (L"Control", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl14);
  UiApplyAttributeList (DummyControl14, L"name='DummyControl14'");

  Ok = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, Ok);
  UiApplyAttributeList (Ok, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Ok' text='Enter' textcolor='0xFFFFFFFF' width='55'");

  Cancel = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, Cancel);
  UiApplyAttributeList (Cancel, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Cancel' text='Cancel' textcolor='0xFFFFFFFF' width='55'");

  return PasswordDialog;
}

UI_CONTROL *
CreateStringDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *StringDialog;
  UI_CONTROL                    *TitleLayout;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *StringDialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *StringDialogImage;
  UI_CONTROL                    *StringDialogText;
  UI_CONTROL                    *DialogSeparator;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *StringLabel;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *InputStringText;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *DummyControl6;
  UI_CONTROL                    *DummyHorizontalLayout3;
  UI_CONTROL                    *DummyControl7;
  UI_CONTROL                    *Ok;
  UI_CONTROL                    *Cancel;


  StringDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, StringDialog);
  UiApplyAttributeList (StringDialog, L"bkcolor='@menucolor' name='StringDialog' padding='40,30,40,30'");

  TitleLayout = CreateControl (L"VerticalLayout", StringDialog);
  CONTROL_CLASS(StringDialog)->AddChild (StringDialog, TitleLayout);
  UiApplyAttributeList (TitleLayout, L"name='TitleLayout'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='80' name='DummyHorizontalLayout1' padding='0,0,30,0'");

  StringDialogTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, StringDialogTitle);
  UiApplyAttributeList (StringDialogTitle, L"fontsize='29' name='StringDialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"name='DummyControl1' padding='0,25,0,25'");

  StringDialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, StringDialogImage);
  UiApplyAttributeList (StringDialogImage, L"bkimagestyle='stretch|light' name='StringDialogImage'");

  StringDialogText = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, StringDialogText);
  UiApplyAttributeList (StringDialogText, L"fontsize='19' name='StringDialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DialogSeparator = CreateControl (L"Control", StringDialog);
  CONTROL_CLASS(StringDialog)->AddChild (StringDialog, DialogSeparator);
  UiApplyAttributeList (DialogSeparator, L"bkcolor='0x0' bkimage='@DialogSeparator' height='10' name='DialogSeparator'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", StringDialog);
  CONTROL_CLASS(StringDialog)->AddChild (StringDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"bkcolor='0xFFF2F2F2' height='41' name='DummyHorizontalLayout2' width='300'");

  StringLabel = CreateControl (L"Label", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, StringLabel);
  UiApplyAttributeList (StringLabel, L"fontsize='16' height='41' name='StringLabel' padding='12,0,0,0' text='input string:' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"height='41' name='DummyVerticalLayout2' padding='6,8,6,0' width='181'");

  DummyControl2 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"bkcolor='0xFF999999' height='29' name='DummyControl3' padding='1,1,1,1' width='173'");

  InputStringText = CreateControl (L"UiEdit", DummyControl3);
  CONTROL_CLASS(DummyControl3)->AddChild (DummyControl3, InputStringText);
  UiApplyAttributeList (InputStringText, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='InputStringText' width='171'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"name='DummyControl4'");

  DummyControl5 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"height='2' name='DummyControl5'");

  DummyControl6 = CreateControl (L"Control", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyControl6);
  UiApplyAttributeList (DummyControl6, L"height='10' name='DummyControl6'");

  DummyHorizontalLayout3 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout3);
  UiApplyAttributeList (DummyHorizontalLayout3, L"childpadding='2' height='55' name='DummyHorizontalLayout3' padding='0,0,30,0' width='300'");

  DummyControl7 = CreateControl (L"Control", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl7);
  UiApplyAttributeList (DummyControl7, L"name='DummyControl7'");

  Ok = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, Ok);
  UiApplyAttributeList (Ok, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Ok' text='Enter' textcolor='0xFFFFFFFF' width='55'");

  Cancel = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, Cancel);
  UiApplyAttributeList (Cancel, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Cancel' text='Cancel' textcolor='0xFFFFFFFF' width='55'");

  return StringDialog;
}

UI_CONTROL *
CreateDialogWithoutSendFormChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DialogWithoutSendForm;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *TitleLayout;
  UI_CONTROL                    *DialogTitle;
  UI_CONTROL                    *DialogText;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DialogPasswordInput;
  UI_CONTROL                    *ConfirmNewPasswordLabel;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *ConfirmPasswordInput;
  UI_CONTROL                    *DialogButtonList;
  UI_CONTROL                    *FormHalo;


  DialogWithoutSendForm = CreateControl (L"Control", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DialogWithoutSendForm);
  UiApplyAttributeList (DialogWithoutSendForm, L"name='DialogWithoutSendForm'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", DialogWithoutSendForm);
  CONTROL_CLASS(DialogWithoutSendForm)->AddChild (DialogWithoutSendForm, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"bkcolor='@menucolor' padding='20,30,20,30' name='DummyVerticalLayout1'");

  TitleLayout = CreateControl (L"VerticalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, TitleLayout);
  UiApplyAttributeList (TitleLayout, L"name='TitleLayout'");

  DialogTitle = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DialogTitle);
  UiApplyAttributeList (DialogTitle, L"align='center' fontsize='19' height='40' name='DialogTitle' textcolor='0xFFFFFFFF' visible='false'");

  DialogText = CreateControl (L"Label", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DialogText);
  UiApplyAttributeList (DialogText, L"fontsize='19' height='40' name='DialogText' textcolor='0xFFFFFFFF'");

  DummyControl1 = CreateControl (L"Control", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"height='15' name='DummyControl1'");

  DummyControl2 = CreateControl (L"Control", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' bkimagestyle='center' height='10' name='DummyControl2'");

  DummyControl3 = CreateControl (L"Control", TitleLayout);
  CONTROL_CLASS(TitleLayout)->AddChild (TitleLayout, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"height='15' name='DummyControl3'");

  DialogPasswordInput = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogPasswordInput);
  UiApplyAttributeList (DialogPasswordInput, L"height='51' name='DialogPasswordInput' padding='0,0,10,0' visible='false'");

  ConfirmNewPasswordLabel = CreateControl (L"Label", DialogPasswordInput);
  CONTROL_CLASS(DialogPasswordInput)->AddChild (DialogPasswordInput, ConfirmNewPasswordLabel);
  UiApplyAttributeList (ConfirmNewPasswordLabel, L"bkcolor='0xFFF2F2F2' fontsize='16' height='41' name='ConfirmNewPasswordLabel' textcolor='0xFF4D4D4D' width='119'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DialogPasswordInput);
  CONTROL_CLASS(DialogPasswordInput)->AddChild (DialogPasswordInput, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"bkcolor='0xFFF2F2F2' height='41' padding='6,8,6,8' name='DummyVerticalLayout2'");

  DummyControl4 = CreateControl (L"Control", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"bkcolor='0xFF999999' height='29' padding='1,1,1,1' name='DummyControl4'");

  ConfirmPasswordInput = CreateControl (L"UiEdit", DummyControl4);
  CONTROL_CLASS(DummyControl4)->AddChild (DummyControl4, ConfirmPasswordInput);
  UiApplyAttributeList (ConfirmPasswordInput, L"bkcolor='0xFFF2F2F2' focusbkcolor='@menulightcolor' height='27' name='ConfirmPasswordInput' padding='7,3,0,3' password='true'");

  DialogButtonList = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DialogButtonList);
  UiApplyAttributeList (DialogButtonList, L"childpadding='2' height='30' name='DialogButtonList' visible='false'");

  FormHalo = CreateControl (L"Texture", DialogWithoutSendForm);
  CONTROL_CLASS(DialogWithoutSendForm)->AddChild (DialogWithoutSendForm, FormHalo);
  UiApplyAttributeList (FormHalo, L"bkimage='@FormHalo' float='true' height='-1' name='FormHalo' scale9grid='23,26,22,31' width='-1'");

  return DialogWithoutSendForm;
}


VOID
FreeDialogEvent (
  IN H2O_FORM_BROWSER_D                    **DialogEvt
  )
{
  H2O_FORM_BROWSER_D                       *Event;

  if (DialogEvt == NULL || *DialogEvt == NULL) {
    return;
  }

  Event = *DialogEvt;

  if (Event->TitleString != NULL) {
    FreePool (Event->TitleString);
  }

  if (Event->BodyStringArray != NULL && Event->BodyStringCount != 0) {
    FreeStringArray (Event->BodyStringArray, Event->BodyStringCount);
  }

  if (Event->BodyInputStringArray != NULL && Event->BodyInputCount != 0) {
    FreeStringArray (Event->BodyInputStringArray, Event->BodyInputCount);
  }

  if (Event->ButtonStringArray != NULL && Event->ButtonCount != 0) {
    FreeStringArray (Event->ButtonStringArray, Event->ButtonCount);
  }

  if (Event->BodyHiiValueArray != NULL && Event->BodyStringCount != 0) {
    FreeHiiValueArray (Event->BodyHiiValueArray, Event->BodyStringCount);
  }

  if (Event->ButtonHiiValueArray != NULL && Event->ButtonCount != 0) {
    FreeHiiValueArray (Event->ButtonHiiValueArray, Event->ButtonCount);
  }

  if (Event->ConfirmHiiValue.Type == EFI_IFR_TYPE_BUFFER && Event->ConfirmHiiValue.Buffer != NULL && Event->ConfirmHiiValue.BufferLen != 0) {
    FreePool (Event->ConfirmHiiValue.Buffer);
  }

  FreePool (Event);

  *DialogEvt = NULL;
}

H2O_FORM_BROWSER_D *
CopyDialogEvent (
  IN H2O_FORM_BROWSER_D                    *SrcDialogEvt
  )
{
  H2O_FORM_BROWSER_D                       *DialogEvt;

  if (SrcDialogEvt == NULL) {
    return NULL;
  }

  DialogEvt = AllocateCopyPool (sizeof (H2O_FORM_BROWSER_D), SrcDialogEvt);
  if (DialogEvt == NULL) {
    return NULL;
  }

  if (SrcDialogEvt->TitleString != NULL) {
    DialogEvt->TitleString = AllocateCopyPool (StrSize (SrcDialogEvt->TitleString), SrcDialogEvt->TitleString);
    if (DialogEvt->TitleString == NULL) {
      goto Error;
    }
  }

  if (SrcDialogEvt->BodyStringArray != NULL && SrcDialogEvt->BodyStringCount != 0) {
    DialogEvt->BodyStringArray = CopyStringArray (SrcDialogEvt->BodyStringArray, SrcDialogEvt->BodyStringCount);
    if (DialogEvt->BodyStringArray == NULL) {
      goto Error;
    }
  }

  if (SrcDialogEvt->BodyInputStringArray != NULL && SrcDialogEvt->BodyInputCount != 0) {
    DialogEvt->BodyInputStringArray = CopyStringArray (SrcDialogEvt->BodyInputStringArray, SrcDialogEvt->BodyInputCount);
    if (DialogEvt->BodyInputStringArray == NULL) {
      goto Error;
    }
  }

  if (SrcDialogEvt->ButtonStringArray != NULL && SrcDialogEvt->ButtonCount != 0) {
    DialogEvt->ButtonStringArray = CopyStringArray (SrcDialogEvt->ButtonStringArray, SrcDialogEvt->ButtonCount);
    if (DialogEvt->ButtonStringArray == NULL) {
      goto Error;
    }
  }

  if (SrcDialogEvt->BodyHiiValueArray != NULL && SrcDialogEvt->BodyStringCount != 0) {
    DialogEvt->BodyHiiValueArray = CopyHiiValueArray (SrcDialogEvt->BodyHiiValueArray, SrcDialogEvt->BodyStringCount);
    if (DialogEvt->BodyHiiValueArray == NULL) {
      goto Error;
    }
  }

  if (SrcDialogEvt->ButtonHiiValueArray != NULL && SrcDialogEvt->ButtonCount != 0) {
    DialogEvt->ButtonHiiValueArray = CopyHiiValueArray (SrcDialogEvt->ButtonHiiValueArray, SrcDialogEvt->ButtonCount);
    if (DialogEvt->ButtonHiiValueArray == NULL) {
      goto Error;
    }
  }

  if (SrcDialogEvt->ConfirmHiiValue.Type == EFI_IFR_TYPE_BUFFER && SrcDialogEvt->ConfirmHiiValue.Buffer != NULL && SrcDialogEvt->ConfirmHiiValue.BufferLen != 0) {
    DialogEvt->ConfirmHiiValue.Buffer = AllocateCopyPool (SrcDialogEvt->ConfirmHiiValue.BufferLen, SrcDialogEvt->ConfirmHiiValue.Buffer);
    if (DialogEvt->ConfirmHiiValue.Buffer == NULL) {
      goto Error;
    }
  }

  return DialogEvt;

Error:
  FreeDialogEvent (&DialogEvt);
  return NULL;
}

HWND
CreateModalDialog (
  HINSTANCE                     Instance,
  UI_DIALOG_CREATE_CHILDS_FUNC  CreateChildsFunc,
  HWND                          ParentWnd,
  WNDPROC                       DialogProc,
  LPARAM                        Param,
  INT32                         X,
  INT32                         Y,
  INT32                         Width,
  INT32                         Height,
  BOOLEAN                       CloseDlgWhenTouchOutside
  )
{
  UI_DIALOG                     *DialogData;
  HWND                          Dlg;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;

  DialogData = AllocateZeroPool (sizeof (UI_DIALOG));
  if (DialogData == NULL) {
    return NULL;
  }
  DialogData->Instance          = Instance;
  DialogData->ParentWnd         = ParentWnd;
  DialogData->Proc              = DialogProc;
  DialogData->Param             = Param;
  DialogData->CreateChildsFunc  = CreateChildsFunc;
  DialogData->Running           = TRUE;
  DialogData->CloseDlgWhenTouchOutside = CloseDlgWhenTouchOutside;

  Dlg = CreateWindowEx (
          WS_EX_NOACTIVATE, L"DIALOG", L"", WS_VISIBLE | WS_POPUP,
          X, Y, Width, Height, ParentWnd, NULL, Instance, DialogData
          );
  ASSERT (Dlg != NULL);
  if (Dlg == NULL) {
    return NULL;
  }

  Control = (UI_CONTROL *) (UINTN) GetWindowLongPtr (Dlg, 0);
  Manager = Control->Manager;

  SendMessage (Manager->MainWnd, UI_NOTIFY_WINDOWINIT, (WPARAM)Manager->Root, 0);

  return Dlg;
}

BOOLEAN
ClickOnOutsideOfDlgRegion (
  IN EFI_ABSOLUTE_POINTER_STATE               *AbsPtrState
  )
{
  RECT                                        DlgRect;
  POINT                                       Pt;

  ASSERT (mDialogWnd != NULL);
  ASSERT (AbsPtrState != NULL);

  if (GetCapture () != NULL) {
    return FALSE;
  }

  if (AbsPtrState->ActiveButtons == 0) {
    return FALSE;
  }

  Pt.x = (INT32) AbsPtrState->CurrentX;
  Pt.y = (INT32) AbsPtrState->CurrentY;

  GetWindowRect (mDialogWnd, &DlgRect);
  if (PtInRect (&DlgRect, Pt)) {
    return FALSE;
  }

  return TRUE;
}

UINT8
GetOpcodeByDialogType (
  IN UINT32                                   DialogType
  )
{
  UINT8                                       OpCode;

  switch ((DialogType & H2O_FORM_BROWSER_D_TYPE_QUESTIONS) >> 16) {

  case H2O_FORM_BROWSER_D_TYPE_ONE_OF:
    OpCode = EFI_IFR_ONE_OF_OP;
    break;

  case H2O_FORM_BROWSER_D_TYPE_ORDERED_LIST:
    OpCode = EFI_IFR_ORDERED_LIST_OP;
    break;

  case H2O_FORM_BROWSER_D_TYPE_NUMERIC:
    OpCode = EFI_IFR_NUMERIC_OP;
    break;

  case H2O_FORM_BROWSER_D_TYPE_STRING:
    OpCode = EFI_IFR_STRING_OP;
    break;

  case H2O_FORM_BROWSER_D_TYPE_DATE:
    OpCode = EFI_IFR_DATE_OP;
    break;

  case H2O_FORM_BROWSER_D_TYPE_TIME:
    OpCode = EFI_IFR_TIME_OP;
    break;

  case H2O_FORM_BROWSER_D_TYPE_PASSWORD:
    OpCode = EFI_IFR_PASSWORD_OP;
    break;

  default:
    OpCode = 0;
    break;
  }

  return OpCode;
}

UINT32
GetButtonWidthByStr (
  VOID
  )
{
  UINT32                                   Index;
  UINT32                                   ButtonWidth;

  ButtonWidth = 0;
  for (Index = 0; Index < mFbDialog->ButtonCount; Index++) {
    if (mFbDialog->ButtonStringArray[Index] != NULL) {
      ButtonWidth = (UINT32) MAX (GetStringWidth (mFbDialog->ButtonStringArray[Index]), ButtonWidth);
    }
  }

  return (ButtonWidth * 5 + 4);
}

EFI_STATUS
PopupDialogInit (
  IN UI_DIALOG                             *Dialog
  )
{
  EFI_STATUS                               Status;
  UI_CONTROL                               *Control;
  UI_CONTROL                               *DialogControl;
  UI_CONTROL                               *FocusControl;
  UINT32                                   Index;
  CHAR16                                   ButtonWidthStr[20];
  CHAR16                                   *BodyString;
  INTN                                     Result;

  if (mFbDialog->TitleString != NULL) {
    Control = UiFindChildByName (Dialog, L"DialogTitle");
    UiSetAttribute (Control, L"text", mFbDialog->TitleString);
  }

  if (mFbDialog->BodyStringArray != NULL) {
    Control = UiFindChildByName (Dialog, L"DialogText");
    BodyString = CatStringArray (mFbDialog->BodyStringCount, (CONST CHAR16 **) mFbDialog->BodyStringArray);
    if (BodyString != NULL) {
      UiSetAttribute (Control, L"text", BodyString);
      FreePool (BodyString);
    }
  }

  if (mFbDialog->ButtonCount == 0) {
    return EFI_SUCCESS;
  }

  DialogControl = UiFindChildByName (Dialog, L"DialogButtonList");

  Control = CreateControl (L"Control", DialogControl);
  CONTROL_CLASS(DialogControl)->AddChild (DialogControl, Control);

  UnicodeSPrint (ButtonWidthStr, sizeof (ButtonWidthStr), L"%d", GetButtonWidthByStr ());
  FocusControl = NULL;
  for (Index = 0; Index < mFbDialog->ButtonCount; Index++) {
    if (mFbDialog->ButtonStringArray[Index] == NULL) {
      continue;
    }

    Control = CreateControl (L"Button", DialogControl);
    UiSetAttribute (Control, L"text",  mFbDialog->ButtonStringArray[Index]);
    SetWindowLongPtr (Control->Wnd, GWLP_USERDATA, (INTN) Index);

    UiApplyAttributeList (Control, L"name='Button' height='30' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
    UiSetAttribute (Control, L"width", ButtonWidthStr);

    Status = CompareHiiValue (&mFbDialog->ButtonHiiValueArray[Index], &mFbDialog->ConfirmHiiValue, &Result);
    if (!EFI_ERROR(Status) && Result == 0) {
      FocusControl = Control;
    }
    CONTROL_CLASS(DialogControl)->AddChild (DialogControl, Control);
  }
  if (FocusControl != NULL) {
    SetFocus (FocusControl->Wnd);
  }

  Control = CreateControl (L"Control", DialogControl);
  CONTROL_CLASS(DialogControl)->AddChild (DialogControl, Control);
  return EFI_SUCCESS;
}

INTN
DialogPopupProc (
  HWND         Wnd,
  UINT         Msg,
  WPARAM       WParam,
  LPARAM       lParam
  )
{
  UI_DIALOG                                *Dialog;
  UI_CONTROL                               *Control;
  UINTN                                    Index;

  Dialog = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    PopupDialogInit (Dialog);
    break;

  case UI_NOTIFY_CLICK:
    Control = (UI_CONTROL *) WParam;
    Index = (UINTN) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);
    SendChangeQNotify (0, 0, &mFbDialog->ButtonHiiValueArray[Index]);
    break;

  case WM_HOTKEY:
    if (HIWORD(lParam) == VK_ESCAPE) {
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


LRESULT
HelpDialogProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  CHAR16                        *BodyString;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    Control = UiFindChildByName (Dialog, L"HelpDialogText");
    BodyString = CatStringArray (mFbDialog->BodyStringCount, (CONST CHAR16 **) mFbDialog->BodyStringArray);
    if (BodyString != NULL) {
      UiSetAttribute (Control, L"text", BodyString);
      FreePool (BodyString);
    }

    Control = UiFindChildByName (Dialog, L"HelpDialogButton");
    SetFocus (Control->Wnd);
    break;

  case UI_NOTIFY_CLICK:
    SendShutDNotify ();
    break;

  case WM_HOTKEY:
    if (HIWORD(LParam) == VK_ESCAPE) {
      SendShutDNotify ();
      return 0;
    }
    return 1;

  default:
    return 0;
  }
  return 1;
}


VOID
EnableTitle (
  HWND          Wnd,
  BOOLEAN       Enable
  )
{
  CHAR16           *Str;
  UI_DIALOG        *Dialog;
  UI_CONTROL       *Control;
  HWND             FocusedWnd;
  STATIC UINTN     CheckTimes = 0;

  //
  // Prevent from button location change to cause invalid button press, NOT do
  // enable/disable password during this period.
  //
  FocusedWnd = GetFocus ();
  Control  = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
  if (Control != NULL && (StrCmp (Control->Name, L"Ok") == 0 ||
      StrCmp (Control->Name, L"Cancel") == 0) && CheckTimes <= 10) {
    CheckTimes++;
    return;
  }
  CheckTimes = 0;
  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  Str = Enable ? L"true" : L"false";

  Control = UiFindChildByName (Dialog, L"TitleLayout");
  UiSetAttribute (Control, L"visible", Str);
  Control = UiFindChildByName (Dialog, L"DialogSeparator");
  UiSetAttribute (Control, L"visible", Str);
  mTitleVisible = Enable;
  CONTROL_CLASS_INVALIDATE (Dialog);
}


LRESULT
CALLBACK
DialogCallback (
  HWND Wnd,
  UINT Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  KEYBOARD_ATTRIBUTES  KeyboardAttributes;

  mSetupMouse->GetKeyboardAttributes (mSetupMouse, &KeyboardAttributes);
  if (KeyboardAttributes.IsStart && mTitleVisible) {
    EnableTitle  (Wnd, FALSE);
  } else if (!KeyboardAttributes.IsStart && !mTitleVisible) {
    EnableTitle  (Wnd, TRUE);
  }
  return 0;
}

EFI_STATUS
SendNumericChange (
  IN UI_DIALOG                  *Dialog
  )
{
  UI_CONTROL                    *Control;
  CHAR16                        *ValueStr;
  H2O_FORM_BROWSER_Q            *CurrentQ;
  EFI_HII_VALUE                 HiiValue;
  UINT64                        EditValue;
  EFI_STATUS                    Status;

  Control  = UiFindChildByName (Dialog, L"InputNumericText");
  ValueStr = ((UI_LABEL *) Control)->Text;
  CurrentQ = mFbDialog->H2OStatement;

  ZeroMem (&HiiValue, sizeof (HiiValue));
  if (((UI_EDIT *) Control)->ValueType == HEX_VALUE) {
    EditValue = StrToUInt (ValueStr, 16, &Status);
  } else {
    EditValue = StrToUInt (ValueStr, 10, &Status);
  }

  if (EditValue >= ((UI_EDIT *) Control)->MinValue) {
    CopyMem (&HiiValue, &CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
    HiiValue.Value.u64 = EditValue;
    SendChangeQNotify (0, 0, &HiiValue);
  }

  return EFI_SUCCESS;
}

LRESULT
NumericDialogProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{

  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  EFI_IMAGE_INPUT               *FormsetImage;
  CHAR16                        Str[20];
  UINTN                         Index;
  HWND                          FocusedWnd;
  H2O_FORM_BROWSER_Q            *CurrentQ;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  CurrentQ = mFbDialog->H2OStatement;
  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (CurrentQ == NULL) {
      break;
    }

    Control = UiFindChildByName (Dialog, L"NumericDialogTitle");
    if (mFbDialog->TitleString != NULL) {
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
    } else {
      UiSetAttribute (Control, L"text", CurrentQ->Prompt);
    }

    FormsetImage = GetCurrentFormSetImage ();
    if (FormsetImage != NULL) {
      Control = UiFindChildByName (Dialog, L"NumericDialogImage");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", FormsetImage);
      UiSetAttribute (Control, L"bkimage", Str);
      UiSetAttribute (Control, L"visible", L"true");
    }

    if (CurrentQ->Help != NULL) {
      Control = UiFindChildByName (Dialog, L"NumericDialogText");
      UiSetAttribute (Control, L"text", CurrentQ->Help);
    }

    //
    // set maximum value and display type
    //
    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"InputNumericText");
      UnicodeSPrint (Str, sizeof (Str), L"%d", CurrentQ->Maximum);
      UiSetAttribute (Control, L"maxvalue", Str);
      if ((CurrentQ->Flags & EFI_IFR_DISPLAY) == EFI_IFR_DISPLAY_UINT_HEX) {
        UiSetAttribute (Control, L"valuetype", L"hex");
      } else {
        UiSetAttribute (Control, L"valuetype", L"dec");
      }
    }

    if (mFbDialog->ButtonCount == 2) {
      Index = 0;
      Control = UiFindChildByName (Dialog, L"Ok");
      UiSetAttribute (Control, L"text", mFbDialog->ButtonStringArray[Index++]);
      Control = UiFindChildByName (Dialog, L"Cancel");
      UiSetAttribute (Control, L"text", mFbDialog->ButtonStringArray[Index++]);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"InputNumericText");
    } else {
      Control = UiFindChildByName (Dialog, L"Ok");
    }
    SetFocus (Control->Wnd);

    mTitleVisible = TRUE;
    SetTimer (Wnd, 0, 1, DialogCallback);
    break;

  case UI_NOTIFY_CLICK:
    FocusedWnd = GetFocus ();
    Control  = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    } else {
      SendNumericChange (Dialog);
    }
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
    FocusedWnd = GetFocus ();
    Control    = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"InputNumericText") == 0) {
      Control = UiFindChildByName (Dialog, L"Ok");
      if (Control == NULL) {
        SendNumericChange (Dialog);
      } else {
        SetFocus (Control->Wnd);
      }
    } else if (StrCmp (Control->Name, L"Ok") == 0) {
      SendNumericChange (Dialog);
    } else if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    }
    return 0;

  case WM_HOTKEY:
    if (HIWORD(LParam) == VK_ESCAPE) {
      SendShutDNotify ();
      return 0;
    }
    return 1;

  case WM_DESTROY:
    KillTimer (Wnd, 0);
    return 0;

  default:
    return 0;
  }
  return 1;
}

LRESULT
NumericDialogWithoutSendFormProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  CHAR16                        Str[20];
  HWND                          FocusedWnd;
  UI_CONTROL                    *ListControl;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (mFbDialog->TitleString != NULL) {
      Control = UiFindChildByName (Dialog, L"NumericDialogTitle");
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"InputNumericText");

      UnicodeSPrint (Str, sizeof (Str), L"%d", mFbDialog->H2OStatement->Minimum);
      UiSetAttribute (Control, L"minvalue", Str);
      UnicodeSPrint (Str, sizeof (Str), L"%d", mFbDialog->H2OStatement->Maximum);
      UiSetAttribute (Control, L"maxvalue", Str);

      if ((mFbDialog->H2OStatement->Flags & EFI_IFR_DISPLAY) == EFI_IFR_DISPLAY_UINT_HEX) {
        UiSetAttribute (Control, L"valuetype", L"hex");
      } else {
        UiSetAttribute (Control, L"valuetype", L"dec");
      }
    }

    if (mFbDialog->ButtonCount != 0) {
      ListControl = UiFindChildByName (Dialog, L"DialogButtonList");
      UiSetAttribute (ListControl, L"visible", L"true");

      Control = CreateControl (L"Control", ListControl);
      CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

      Control = CreateControl (L"Button", ListControl);
      UiApplyAttributeList (Control, L"name='Ok' text='Enter' height='30' width='75' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
      CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

      Control = CreateControl (L"Button", ListControl);
      UiApplyAttributeList (Control, L"name='Cancel' text='Cancel' height='30' width='75' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
      CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"InputNumericText");
    } else {
      Control = UiFindChildByName (Dialog, L"Ok");
    }
    SetFocus (Control->Wnd);
    break;

  case UI_NOTIFY_CLICK:
    FocusedWnd = GetFocus ();
    Control  = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    } else {
      SendNumericChange (Dialog);
    }
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
    FocusedWnd = GetFocus ();
    Control    = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"InputNumericText") == 0) {
      Control = UiFindChildByName (Dialog, L"Ok");
      if (Control == NULL) {
        SendNumericChange (Dialog);
      } else {
        SetFocus (Control->Wnd);
      }
    } else if (StrCmp (Control->Name, L"Ok") == 0) {
      SendNumericChange (Dialog);
    } else if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    }
    return 0;

  default:
    return 0;
  }

  return 1;
}

EFI_STATUS
SendPasswordChange (
  IN  HWND                       Wnd,
  IN  UINT32                     PasswordCount
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *OldPasswordControl;
  UI_CONTROL                    *NewPasswordControl;
  UI_CONTROL                    *ConfirmNewPasswordControl;
  CHAR16                        *OldPasswordStr;
  CHAR16                        *NewPasswordStr;
  CHAR16                        *ConfirmNewPasswordStr;
  UINT32                        OldPasswordStrSize;
  UINT32                        NewPasswordStrSize;
  UINT32                        ConfirmNewPasswordStrSize;
  EFI_HII_VALUE                 HiiValue;

  Dialog = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  OldPasswordControl        = UiFindChildByName (Dialog, L"OldPasswordText");
  OldPasswordStr            = ((UI_LABEL *) OldPasswordControl)->Text;
  NewPasswordControl        = UiFindChildByName (Dialog, L"NewPasswordText");
  NewPasswordStr            = ((UI_LABEL *) NewPasswordControl)->Text;
  ConfirmNewPasswordControl = UiFindChildByName (Dialog, L"ConfirmNewPasswordText");
  ConfirmNewPasswordStr     = ((UI_LABEL *) ConfirmNewPasswordControl)->Text;

  ZeroMem (&HiiValue, sizeof (HiiValue));
  HiiValue.Type = EFI_IFR_TYPE_STRING;

  switch (PasswordCount) {

  case 3:
    OldPasswordStrSize        = (UINT32) StrSize (OldPasswordStr);
    NewPasswordStrSize        = (UINT32) StrSize (NewPasswordStr);
    ConfirmNewPasswordStrSize = (UINT32) StrSize (ConfirmNewPasswordStr);

    HiiValue.BufferLen = (UINT16) (OldPasswordStrSize + NewPasswordStrSize + ConfirmNewPasswordStrSize);
    HiiValue.Buffer    = AllocatePool (HiiValue.BufferLen);
    ASSERT (HiiValue.Buffer != NULL);
    if (HiiValue.Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (&HiiValue.Buffer[0]                                      , OldPasswordStr       , OldPasswordStrSize);
    CopyMem (&HiiValue.Buffer[OldPasswordStrSize]                     , NewPasswordStr       , NewPasswordStrSize);
    CopyMem (&HiiValue.Buffer[OldPasswordStrSize + NewPasswordStrSize], ConfirmNewPasswordStr, ConfirmNewPasswordStrSize);
    break;

  case 2:
    NewPasswordStrSize        = (UINT32) StrSize (NewPasswordStr);
    ConfirmNewPasswordStrSize = (UINT32) StrSize (ConfirmNewPasswordStr);

    HiiValue.BufferLen = (UINT16) (NewPasswordStrSize + ConfirmNewPasswordStrSize);
    HiiValue.Buffer    = AllocatePool (HiiValue.BufferLen);
    ASSERT (HiiValue.Buffer != NULL);
    if (HiiValue.Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (&HiiValue.Buffer[0]                 , NewPasswordStr       , NewPasswordStrSize);
    CopyMem (&HiiValue.Buffer[NewPasswordStrSize], ConfirmNewPasswordStr, ConfirmNewPasswordStrSize);
    break;

  case 1:
    ConfirmNewPasswordStrSize = (UINT32) StrSize (ConfirmNewPasswordStr);

    HiiValue.BufferLen = (UINT16) (ConfirmNewPasswordStrSize);
    HiiValue.Buffer    = AllocatePool (HiiValue.BufferLen);
    ASSERT (HiiValue.Buffer != NULL);
    if (HiiValue.Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (&HiiValue.Buffer[0], ConfirmNewPasswordStr, ConfirmNewPasswordStrSize);
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  SendChangeQNotify (0, 0, &HiiValue);

  return EFI_SUCCESS;
}

LRESULT
PasswordDialogProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  EFI_IMAGE_INPUT               *FormsetImage;
  CHAR16                        Str[20];
  STATIC UINT32                 BodyInputCount;
  HWND                          FocusedWnd;


  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (gFB->CurrentQ != NULL && gFB->CurrentQ->Help != NULL) {
      Control = UiFindChildByName (Dialog, L"PasswordDialogText");
      UiSetAttribute (Control, L"text", gFB->CurrentQ->Help);
    }
    //
    // set maximum password length
    //
    UnicodeSPrint (Str, sizeof (Str), L"%d",  (mFbDialog->ConfirmHiiValue.BufferLen / sizeof (CHAR16) - 1));
    Control = UiFindChildByName (Dialog, L"OldPasswordText");
    UiSetAttribute (Control, L"maxlength", Str);
    Control = UiFindChildByName (Dialog, L"NewPasswordText");
    UiSetAttribute (Control, L"maxlength", Str);
    Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordText");
    UiSetAttribute (Control, L"maxlength", Str);

    //
    // Disable Enter old password if password doesn't exist.
    //
    BodyInputCount = mFbDialog->BodyInputCount;
    switch (BodyInputCount) {

    case 3:
      Control = UiFindChildByName (Dialog, L"OldPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[0]);
      Control = UiFindChildByName (Dialog, L"NewPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[1]);
      Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[2]);

      Control = UiFindChildByName (Dialog, L"OldPasswordText");
      SetFocus (Control->Wnd);
      break;

    case 2:
      Control = UiFindChildByName (Dialog, L"OldPassword");
      UiSetAttribute (Control, L"visible", L"false");
      Control = UiFindChildByName (Dialog, L"OldPasswordSeperator");
      UiSetAttribute (Control, L"visible", L"false");

      Control = UiFindChildByName (Dialog, L"NewPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[0]);
      Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[1]);

      Control = UiFindChildByName (Dialog, L"NewPasswordText");
      SetFocus (Control->Wnd);
      break;

    case 1:
      Control = UiFindChildByName (Dialog, L"OldPassword");
      UiSetAttribute (Control, L"visible", L"false");
      Control = UiFindChildByName (Dialog, L"OldPasswordSeperator");
      UiSetAttribute (Control, L"visible", L"false");

      Control = UiFindChildByName (Dialog, L"NewPassword");
      UiSetAttribute (Control, L"visible", L"false");
      Control = UiFindChildByName (Dialog, L"NewPasswordSeperator");
      UiSetAttribute (Control, L"visible", L"false");

      Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->BodyStringArray[0]);

      Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordText");
      SetFocus (Control->Wnd);
      break;

    default:
      ASSERT(FALSE);
      break;
    }

    Control = UiFindChildByName (Dialog, L"PasswordDialogTitle");
    if (mFbDialog->TitleString != NULL) {
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
    }
    FormsetImage = GetCurrentFormSetImage ();

    if (FormsetImage != NULL) {
      Control = UiFindChildByName (Dialog, L"PasswordDialogImage");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", FormsetImage);
      UiSetAttribute (Control, L"bkimage", Str);
      UiSetAttribute (Control, L"visible", L"true");
    }

    //
    // Add text for buttons
    //
    if (mFbDialog->ButtonCount == 2) {
      Control = UiFindChildByName (Dialog, L"Ok");
      UiSetAttribute (Control, L"text", mFbDialog->ButtonStringArray[0]);
      Control = UiFindChildByName (Dialog, L"Cancel");
      UiSetAttribute (Control, L"text", mFbDialog->ButtonStringArray[1]);
    }

    mTitleVisible = TRUE;
    SetTimer (Wnd, 0, 1, DialogCallback);
    break;

  case UI_NOTIFY_CLICK:
    FocusedWnd = GetFocus ();
    Control  = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    } else {
      SendPasswordChange (Wnd, BodyInputCount);
    }
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
    FocusedWnd = GetFocus ();
    Control    = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"OldPasswordText") == 0) {
      Control = UiFindChildByName (Dialog, L"NewPasswordText");
      SetFocus (Control->Wnd);
    } else if (StrCmp (Control->Name, L"NewPasswordText") == 0) {
      Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordText");
      SetFocus (Control->Wnd);
    } else if (StrCmp (Control->Name, L"ConfirmNewPasswordText") == 0) {
      Control = UiFindChildByName (Dialog, L"Ok");
      SetFocus (Control->Wnd);
    } else if (StrCmp (Control->Name, L"Ok") == 0) {
      SendPasswordChange (Wnd, BodyInputCount);
    } else if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    }
    return 0;

  case WM_HOTKEY:
    if (HIWORD(LParam) == VK_ESCAPE) {
      SendShutDNotify ();
      return 0;
    }
    return 1;

  case WM_DESTROY:
    KillTimer (Wnd, 0);
    return 0;

  default:
    return 0;
  }
  return 1;
}

EFI_STATUS
SendStringChange (
  IN UI_DIALOG                  *Dialog
  )
{
  UI_CONTROL                    *Control;
  CHAR16                        *String;
  UINTN                         StringSize;
  EFI_HII_VALUE                 HiiValue;

  Control    = UiFindChildByName (Dialog, L"InputStringText");
  String     = ((UI_LABEL *) Control)->Text;
  StringSize = StrSize (String);

  ZeroMem (&HiiValue, sizeof (HiiValue));
  HiiValue.Type      = EFI_IFR_TYPE_STRING;
  HiiValue.BufferLen = (UINT16) StringSize;
  HiiValue.Buffer    = (UINT8 *) AllocateCopyPool (StringSize, String);
  SendChangeQNotify (0, 0, &HiiValue);

  return EFI_SUCCESS;
}

LRESULT
StringDialogProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  EFI_IMAGE_INPUT               *FormsetImage;
  CHAR16                        Str[20];
  UINTN                         Index;
  HWND                          FocusedWnd;
  H2O_FORM_BROWSER_Q            *CurrentQ;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  CurrentQ = mFbDialog->H2OStatement;;
  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (CurrentQ == NULL) {
      break;
    }

    Control = UiFindChildByName (Dialog, L"StringDialogTitle");
    if (mFbDialog->TitleString != NULL) {
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
    } else {
      UiSetAttribute (Control, L"text", CurrentQ->Prompt);
    }

    FormsetImage = GetCurrentFormSetImage ();
    if (FormsetImage != NULL) {
      Control = UiFindChildByName (Dialog, L"StringDialogImage");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", FormsetImage);
      UiSetAttribute (Control, L"bkimage", Str);
      UiSetAttribute (Control, L"visible", L"true");
    }

    if (CurrentQ->Help != NULL) {
      Control = UiFindChildByName (Dialog, L"StringDialogText");
      UiSetAttribute (Control, L"text", CurrentQ->Help);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"InputStringText");
      UnicodeSPrint (Str, sizeof (Str), L"%d", CurrentQ->Maximum);
      UiSetAttribute (Control, L"maxlength", Str);
    }

    if (mFbDialog->ButtonCount == 2) {
      Index = 0;
      Control = UiFindChildByName (Dialog, L"Ok");
      UiSetAttribute (Control, L"text", mFbDialog->ButtonStringArray[Index++]);
      Control = UiFindChildByName (Dialog, L"Cancel");
      UiSetAttribute (Control, L"text", mFbDialog->ButtonStringArray[Index++]);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"InputStringText");
    } else {
      Control = UiFindChildByName (Dialog, L"Ok");
    }
    SetFocus (Control->Wnd);

    mTitleVisible = TRUE;
    SetTimer (Wnd, 0, 1, DialogCallback);
    break;

  case UI_NOTIFY_CLICK:
    FocusedWnd = GetFocus ();
    Control  = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    } else {
      SendStringChange (Dialog);
    }
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
    FocusedWnd = GetFocus ();
    Control    = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"InputStringText") == 0) {
      Control = UiFindChildByName (Dialog, L"Ok");
      if (Control == NULL) {
        SendStringChange (Dialog);
      } else {
        SetFocus (Control->Wnd);
      }
    } else if (StrCmp (Control->Name, L"Ok") == 0) {
      SendStringChange (Dialog);
    } else if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    }
    return 0;

  case WM_HOTKEY:
    if (HIWORD(LParam) == VK_ESCAPE) {
      SendShutDNotify ();
      return 0;
    }
    return 1;

  case WM_DESTROY:
    KillTimer (Wnd, 0);
    return 0;

  default:
    return 0;
  }
  return 1;
}

LRESULT
OneOfInputProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  );

LRESULT
OrderedListInputProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  );

EFI_STATUS
GetDialogRegion (
  IN  CHAR16                               *PanelName,
  OUT RECT                                 *DlgRect
  )
{
  EFI_STATUS                               Status;
  RECT                                     PanelRect;
  UINT32                                   PanelWidth;
  UINT32                                   PanelHeight;
  UINT32                                   DlgWidth;
  UINT32                                   DlgHeight;

  ASSERT (PanelName != NULL);
  ASSERT (DlgRect != NULL);

  Status = GetRectByName (gWnd, PanelName, &PanelRect);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  PanelWidth  = PanelRect.right  - PanelRect.left;
  PanelHeight = PanelRect.bottom - PanelRect.top;

  if ((mFbDialog->DialogType & H2O_FORM_BROWSER_D_TYPE_QUESTIONS) == 0) {
    if ((mFbDialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SELECTION) == H2O_FORM_BROWSER_D_TYPE_SELECTION) {
      DlgWidth  = 360;
      DlgHeight = 150;
    } else if ((mFbDialog->DialogType & H2O_FORM_BROWSER_D_TYPE_MSG) == H2O_FORM_BROWSER_D_TYPE_MSG) {
      DlgWidth  = 360;
      DlgHeight = 200;
    } else {
      return EFI_UNSUPPORTED;
    }
  } else {
    switch (GetOpcodeByDialogType (mFbDialog->DialogType)) {

    case EFI_IFR_ONE_OF_OP:
      DlgWidth = 360;
      if (mFbDialog->BodyStringCount <= 4) {
        DlgHeight = 130 + (mFbDialog->BodyStringCount * 40);
      } else {
        DlgHeight = 330;
      }
      break;

    case EFI_IFR_NUMERIC_OP:
      DlgWidth  = 360;
      DlgHeight = 200;
      break;

    case EFI_IFR_PASSWORD_OP:
      DlgWidth  = 400;
      DlgHeight = 120;
      break;

    default:
      return EFI_UNSUPPORTED;
      break;
    }
  }

  DlgRect->left   = PanelRect.left + ((PanelWidth - DlgWidth) / 2);
  DlgRect->top    = PanelRect.top  + ((PanelHeight - DlgHeight) / 2);
  DlgRect->right  = DlgRect->left + DlgWidth;
  DlgRect->bottom = DlgRect->top + DlgHeight;

  return EFI_SUCCESS;
}

EFI_STATUS
SetDialogText (
  IN UI_DIALOG                             *Dialog
  )
{
  EFI_STATUS                               Status;
  UINTN                                    Index;
  UI_CONTROL                               *Control;
  UI_CONTROL                               *ListControl;
  UI_CONTROL                               *FocusControl;
  CHAR16                                   *BodyString;
  INTN                                     Result;

  if ((mFbDialog->DialogType & H2O_FORM_BROWSER_D_TYPE_QUESTIONS) != 0) {
    return EFI_UNSUPPORTED;
  }

  FocusControl = NULL;

  if ((mFbDialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SELECTION) == H2O_FORM_BROWSER_D_TYPE_SELECTION) {
    if (mFbDialog->BodyStringArray != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogText");
      BodyString = CatStringArray (mFbDialog->BodyStringCount, (CONST CHAR16 **) mFbDialog->BodyStringArray);
      if (BodyString != NULL) {
        UiSetAttribute (Control, L"text", BodyString);
        FreePool (BodyString);
      }
    }

    ListControl = UiFindChildByName (Dialog, L"DialogButtonList");
    UiSetAttribute (ListControl, L"visible", L"true");

    Control = CreateControl (L"Control", ListControl);
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

    for (Index = 0; Index < mFbDialog->ButtonCount; Index++) {
      if (mFbDialog->ButtonStringArray[Index] == NULL) {
        continue;
      }

      Control = CreateControl (L"Button", ListControl);
      UiSetAttribute (Control, L"text",  mFbDialog->ButtonStringArray[Index]);
      SetWindowLongPtr (Control->Wnd, GWLP_USERDATA, (INTN) Index);
      UiApplyAttributeList (Control, L"name='Button' height='30' width='75' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
      CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

      Status = CompareHiiValue (&mFbDialog->ButtonHiiValueArray[Index], &mFbDialog->ConfirmHiiValue, &Result);
      if (!EFI_ERROR(Status) && Result == 0) {
        FocusControl = Control;
      }
    }

    Control = CreateControl (L"Control", ListControl);
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

    if (FocusControl != NULL) {
      SetFocus (FocusControl->Wnd);
    }
  }

  return EFI_SUCCESS;
}


LRESULT
DialogWithoutSendFormProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UINTN                         Index;
  UI_CONTROL                    *Control;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    SetDialogText (Dialog);
    break;

  case UI_NOTIFY_CLICK:
    Control = (UI_CONTROL *)WParam;

    if ((mFbDialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SELECTION) == H2O_FORM_BROWSER_D_TYPE_SELECTION) {
      Index = (UINTN) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);
      SendChangeQNotify (0, 0, &mFbDialog->ButtonHiiValueArray[Index]);
    }
    break;

  case WM_DESTROY:
    FreeDialogEvent (&mFbDialog);
    return 0;

  default:
    return 0;
  }

  return 1;
}

EFI_STATUS
SendConfirmPassword (
  IN  UI_DIALOG                 *Dialog
  )
{
  UI_CONTROL                    *Control;
  CHAR16                        *PasswordStr;
  EFI_HII_VALUE                 HiiValue;

  Control = UiFindChildByName (Dialog, L"ConfirmPasswordInput");
  PasswordStr = ((UI_LABEL *) Control)->Text;

  HiiValue.BufferLen = (UINT16) StrSize (PasswordStr);
  HiiValue.Buffer    = AllocatePool (HiiValue.BufferLen);
  ASSERT (HiiValue.Buffer != NULL);
  CopyMem (HiiValue.Buffer, PasswordStr, HiiValue.BufferLen);
  SendChangeQNotify (0, 0, &HiiValue);

  return EFI_SUCCESS;
}

LRESULT
ConfirmPasswordProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  CHAR16                        Str[20];
  HWND                          FocusedWnd;
  UI_CONTROL                    *ListControl;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (gFB->CurrentP == NULL) {
      Control = UiFindChildByName (Dialog, L"TitleLayout");
      UiSetAttribute (Control, L"visible", L"false");
    }

    if (mFbDialog->TitleString != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogText");
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
      Control = UiFindChildByName (Dialog, L"ConfirmNewPasswordLabel");
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"DialogPasswordInput");
      UiSetAttribute (Control, L"visible", L"true");

      UnicodeSPrint (Str, sizeof (Str), L"%d", (mFbDialog->ConfirmHiiValue.BufferLen / sizeof (CHAR16) - 1));
      Control = UiFindChildByName (Dialog, L"ConfirmPasswordInput");
      UiSetAttribute (Control, L"maxlength", Str);
    }

    ListControl = UiFindChildByName (Dialog, L"DialogButtonList");
    UiSetAttribute (ListControl, L"visible", L"true");

    Control = CreateControl (L"Control", ListControl);
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

    Control = CreateControl (L"Button", ListControl);
    UiApplyAttributeList (Control, L"name='Ok' text='Enter' height='30' width='75' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

    Control = CreateControl (L"Button", ListControl);
    UiApplyAttributeList (Control, L"name='Cancel' text='Cancel' height='30' width='75' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

    if (gFB->CurrentP != NULL) {
      mTitleVisible = TRUE;
      SetTimer (Wnd, 0, 1, DialogCallback);
    }

    if (mFbDialog->BodyInputCount != 0) {
      Control = UiFindChildByName (Dialog, L"ConfirmPasswordInput");
    } else {
      Control = UiFindChildByName (Dialog, L"Ok");
    }
    SetFocus (Control->Wnd);
    break;

  case UI_NOTIFY_CLICK:
    FocusedWnd = GetFocus ();
    Control  = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    } else {
      SendConfirmPassword (Dialog);
    }
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
    FocusedWnd = GetFocus ();
    Control    = (UI_CONTROL *) GetWindowLongPtr (FocusedWnd, 0);
    if (StrCmp (Control->Name, L"ConfirmPasswordInput") == 0) {
      Control = UiFindChildByName (Dialog, L"Ok");
      SetFocus (Control->Wnd);
    } else if (StrCmp (Control->Name, L"Ok") == 0) {
      SendConfirmPassword (Dialog);
    } else if (StrCmp (Control->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    }
    return 0;

  case WM_DESTROY:
    if (gFB->CurrentP != NULL) {
      KillTimer (Wnd, 0);
    }
    return 0;

  default:
    return 0;
  }

  return 1;
}


LRESULT
MsgPopUpProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  UI_CONTROL                    *ListControl;
  UI_CONTROL                    *FocusControl;
  CHAR16                        *BodyString;
  CHAR16                        ButtonWidthStr[20];
  UINTN                         Index;
  INTN                          Result;
  EFI_STATUS                    Status;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (mFbDialog->TitleString != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogTitle");
      UiSetAttribute (Control, L"text", mFbDialog->TitleString);
      UiSetAttribute (Control, L"visible", L"true");
    }

    if (mFbDialog->BodyStringArray != NULL) {
      Control = UiFindChildByName (Dialog, L"DialogText");
      BodyString = CatStringArray (mFbDialog->BodyStringCount, (CONST CHAR16 **) mFbDialog->BodyStringArray);
      if (BodyString != NULL) {
        UiSetAttribute (Control, L"text", BodyString);
        FreePool (BodyString);
      }
    }

    ListControl = UiFindChildByName (Dialog, L"DialogButtonList");
    UiSetAttribute (ListControl, L"visible", L"true");

    Control = CreateControl (L"Control", ListControl);
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);

    UnicodeSPrint (ButtonWidthStr, sizeof (ButtonWidthStr), L"%d", GetButtonWidthByStr ());
    FocusControl = NULL;
    for (Index = 0; Index < mFbDialog->ButtonCount; Index++) {
      if (mFbDialog->ButtonStringArray[Index] == NULL) {
        continue;
      }

      Control = CreateControl (L"Button", ListControl);
      SetWindowLongPtr (Control->Wnd, GWLP_USERDATA, (INTN) Index);

      UiApplyAttributeList (Control, L"name='Button' height='30' fontsize='19' textcolor='0xFFFFFFFF' align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor'");
      UiSetAttribute (Control, L"text",  mFbDialog->ButtonStringArray[Index]);
      UiSetAttribute (Control, L"width", ButtonWidthStr);

      Status = CompareHiiValue (&mFbDialog->ButtonHiiValueArray[Index], &mFbDialog->ConfirmHiiValue, &Result);
      if (!EFI_ERROR(Status) && Result == 0) {
        FocusControl = Control;
      }
      CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);
    }
    if (FocusControl != NULL) {
      SetFocus (FocusControl->Wnd);
    }

    Control = CreateControl (L"Control", ListControl);
    CONTROL_CLASS(ListControl)->AddChild (ListControl, Control);
    break;

  case UI_NOTIFY_CLICK:
    Control = (UI_CONTROL *) WParam;
    Index = (UINTN) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);
    SendChangeQNotify (0, 0, &mFbDialog->ButtonHiiValueArray[Index]);
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

INTN
DateTimeDialogProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  );

HWND
FindOverlayDialog (
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Overlay;
  UI_MANAGER                    *Manager;


  Control = GetUiControl (gWnd);
  Manager = Control->Manager;
  Overlay = Manager->FindControlByName (Manager, L"overlay");
  if (Overlay != NULL) {
    CONTROL_CLASS(Overlay)->SetAttribute (Overlay, L"visible", L"true");
    return Overlay->Wnd;
  }

  return NULL;
}


HWND
LocalMetroCreateDialog (
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  )
{
  HWND                          Wnd;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D *OpenDNotify;
  H2O_FORM_BROWSER_D            *Dialog;
  UI_DIALOG_CREATE_CHILDS_FUNC  CreateDialogFuncs;
  WNDPROC                       DialogProc;
  BOOLEAN                       CloseOnTouchOutside;
  UINT8                         OpCode;
  HWND                          ParentWnd;
  RECT                          DlgRect;
  EFI_STATUS                    Status;

  OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
  Dialog      = &OpenDNotify->Dialog;

  Wnd = NULL;
  CreateDialogFuncs = NULL;
  DialogProc = NULL;
  if (gFB->CurrentP != NULL) {
    CloseOnTouchOutside = TRUE;
    mIsSendForm         = TRUE;
  } else {
    CloseOnTouchOutside = FALSE;
    mIsSendForm         = FALSE;
    ZeroMem (&mPreviousAbsPtrState, sizeof (EFI_ABSOLUTE_POINTER_STATE));
  }
  SetRectEmpty (&DlgRect);
//ParentWnd = FindOverlayDialog();
  ParentWnd = GetFocus();
  mFbDialog = Dialog;



  if ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_QUESTIONS) == 0) {
    if ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SHOW_HELP) == H2O_FORM_BROWSER_D_TYPE_SHOW_HELP) {
      Status = GetInformationField (&DlgRect);
      ASSERT (!EFI_ERROR (Status));
      CreateDialogFuncs = CreateHelpDialogChilds;
      DialogProc = HelpDialogProc;
    } else if ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_SELECTION) == H2O_FORM_BROWSER_D_TYPE_SELECTION) {
      mFbDialog = CopyDialogEvent (Dialog);
      if (mFbDialog == NULL) {
        return NULL;
      }

      if (mIsSendForm) {
        Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
        ASSERT (!EFI_ERROR (Status));
        CreateDialogFuncs = CreateCommonDialogChilds;
        DialogProc = DialogPopupProc;
      } else {
        Status = GetDialogRegion (L"DialogPanel", &DlgRect);
        ASSERT_EFI_ERROR (Status);
        CreateDialogFuncs = CreateDialogWithoutSendFormChilds;
        DialogProc = DialogWithoutSendFormProc;
      }
    } else if ((Dialog->DialogType & H2O_FORM_BROWSER_D_TYPE_MSG) == H2O_FORM_BROWSER_D_TYPE_MSG) {
      if (mIsSendForm) {
        Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
        ASSERT_EFI_ERROR (Status);
        CreateDialogFuncs = CreateCommonDialogChilds;
      } else {
        Status = GetDialogRegion (L"DialogPanel", &DlgRect);
        ASSERT_EFI_ERROR (Status);
        OffsetRect (&DlgRect, 0, -30);
        CreateDialogFuncs = CreateDialogWithoutSendFormChilds;
      }

      DialogProc = MsgPopUpProc;
      mFbDialog = CopyDialogEvent (Dialog);
      if (mFbDialog == NULL) {
        return NULL;
      }
    } else {
      return NULL;
    }
  } else {
    OpCode = GetOpcodeByDialogType (Dialog->DialogType);

    switch (OpCode) {

    case EFI_IFR_ONE_OF_OP:
      mFbDialog = CopyDialogEvent (Dialog);
      if (mFbDialog == NULL) {
        return NULL;
      }

      if (mIsSendForm) {
        Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
        CreateDialogFuncs = CreateOneOfDialogChilds;
      } else {
        Status = GetDialogRegion (L"DialogPanel", &DlgRect);
        CreateDialogFuncs = CreateOneOfDialogWithoutSendFormChilds;
      }
      DialogProc = OneOfInputProc;
      ASSERT (!EFI_ERROR (Status));
      break;

    case EFI_IFR_NUMERIC_OP:
      //
      // TODO: Need receive and sending H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q to synchronize
      // with other display engine.
      //
      if (mIsSendForm) {
        Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
        ASSERT (!EFI_ERROR (Status));
        CreateDialogFuncs = CreateNumericDialogChilds;
        DialogProc = NumericDialogProc;
      } else {
        Status = GetDialogRegion (L"DialogPanel", &DlgRect);
        ASSERT_EFI_ERROR (Status);
        OffsetRect (&DlgRect, 0, -30);
        CreateDialogFuncs = CreateNumericDialogWithoutSendFormChilds;
        DialogProc = NumericDialogWithoutSendFormProc;
      }
      break;

    case EFI_IFR_ORDERED_LIST_OP:
      Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
      ASSERT (!EFI_ERROR (Status));
      CreateDialogFuncs = CreateOrderedListDialogChilds;
      DialogProc = OrderedListInputProc;
      break;

    case EFI_IFR_CHECKBOX_OP:
    case EFI_IFR_PASSWORD_OP:
      //
      // TODO: Need receive and sending H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q to synchronize
      // with other display engine.
      //
      if (mIsSendForm) {
        Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
        ASSERT (!EFI_ERROR (Status));
        CreateDialogFuncs = CreatePasswordDialogChilds;
        DialogProc = PasswordDialogProc;
      } else {
        Status = GetDialogRegion (L"DialogPanel", &DlgRect);
        ASSERT_EFI_ERROR (Status);
        OffsetRect (&DlgRect, 0, -30);
        CreateDialogFuncs = CreateDialogWithoutSendFormChilds;
        DialogProc = ConfirmPasswordProc;
      }
      break;

    case EFI_IFR_STRING_OP:
      //
      // TODO: Need receive and sending H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q to synchronize
      // with other display engine.
      //
      Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
      ASSERT (!EFI_ERROR (Status));
      CreateDialogFuncs = CreateStringDialogChilds;
      DialogProc = StringDialogProc;
      CloseOnTouchOutside = TRUE;
      break;

    case EFI_IFR_DATE_OP:
    case EFI_IFR_TIME_OP:
      Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &DlgRect);
      ASSERT (!EFI_ERROR (Status));
      CreateDialogFuncs  = OpCode == EFI_IFR_DATE_OP ? CreateDateDialogChilds : CreateTimeDialogChilds;
      DialogProc = DateTimeDialogProc;
      break;

    default:
      ASSERT (FALSE);
    }
  }

  Wnd = CreateModalDialog (
          NULL,
          CreateDialogFuncs,
          ParentWnd,
          DialogProc,
          0,
          DlgRect.left,
          DlgRect.top,
          DlgRect.right - DlgRect.left,
          DlgRect.bottom - DlgRect.top,
          CloseOnTouchOutside
          );

  UnregisterHotKey (Wnd, 1);
  MwRegisterHotKey (Wnd, 1,  0, VK_ESCAPE);

  return Wnd;
}

VOID
LocalMetroRegisterHotkey (
  HWND                          Wnd
  );

EFI_STATUS
EFIAPI
LocalMetroDialogNotify (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  )
{
  MSG                                 Msg;
  H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE *AbsPtr;
  H2O_DISPLAY_ENGINE_EVT_KEYPRESS     *KeyPress;
  UI_DIALOG                           *Dialog;

  switch (Notify->Type) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D:
    //
    // don't allow re-entry
    //
    ASSERT (mDialogWnd == NULL);
    if (mDialogWnd != NULL) {
      return EFI_ALREADY_STARTED;
    }
    EnableWindow (gWnd, FALSE);
    EnableWindow (GetDesktopWindow(), FALSE);
    mDialogWnd = LocalMetroCreateDialog (Notify);
    if (mDialogWnd == NULL) {
      SendShutDNotify ();
    }
    GrayOutBackground (gWnd, TRUE);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D:
    if (mDialogWnd != NULL) {
      DestroyWindow(mDialogWnd);
      mDialogWnd = NULL;
    }
    This->Notify = LocalMetroNotify;
    EnableWindow (gWnd, TRUE);
    GrayOutBackground (gWnd, FALSE);
    SetFocus (gWnd);
    LocalMetroRegisterHotkey (gWnd);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE:
    AbsPtr = (H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE *) Notify;
    Dialog = (UI_DIALOG *) GetWindowLongPtr (mDialogWnd, 0);

    if (Dialog->CloseDlgWhenTouchOutside && ClickOnOutsideOfDlgRegion (&AbsPtr->AbsPtrState) && mPreviousAbsPtrState.ActiveButtons == 0) {
      CopyMem (&mPreviousAbsPtrState, &AbsPtr->AbsPtrState, sizeof (EFI_ABSOLUTE_POINTER_STATE));
      SendShutDNotify ();
      break;
    }

    CopyMem (&mPreviousAbsPtrState, &AbsPtr->AbsPtrState, sizeof (EFI_ABSOLUTE_POINTER_STATE));
    GdAddPointerData (NULL, &AbsPtr->AbsPtrState);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS:
    KeyPress = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *) Notify;
    GdAddEfiKeyData (&KeyPress->KeyData);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER:
    if (((H2O_DISPLAY_ENGINE_EVT_TIMER *) Notify)->TimerId == H2O_METRO_DE_TIMER_ID_PERIODIC_TIMER) {
      RegisterTimerEvent (H2O_METRO_DE_TIMER_ID_PERIODIC_TIMER, H2O_METRO_DE_TIMER_PERIODIC_TIME);
    }
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q:
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q:
    break;

  default:
    ASSERT (FALSE);
  }

  while (PeekMessage (&Msg, NULL, 0, 0, PM_REMOVE)) {
    if (Msg.message == WM_RBUTTONUP) {
      PostMessage (mDialogWnd, WM_HOTKEY, 0, MAKELPARAM(0, VK_ESCAPE));
    }
    TranslateMessage (&Msg);
    DispatchMessage (&Msg);
  }

  return EFI_SUCCESS;
}

