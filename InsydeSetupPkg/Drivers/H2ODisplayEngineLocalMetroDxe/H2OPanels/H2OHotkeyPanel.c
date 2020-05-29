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

STATIC H2O_HOTKEY_PANEL_CLASS            *mH2OHotkeyPanelClass = NULL;
#define CURRENT_CLASS                    mH2OHotkeyPanelClass

UI_CONTROL *
CreateHotkeyPanelChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *Dummy;
  UI_CONTROL                    *HotkeyBackground;
  UI_CONTROL                    *HotkeyList;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *HotkeyShowHelp;
  UI_CONTROL                    *ShowHelpString;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *SelectItem;
  UI_CONTROL                    *HotkeySelectItemUp;
  UI_CONTROL                    *HotkeySelectItemDown;
  UI_CONTROL                    *SelectItemString;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyVerticalLayout3;
  UI_CONTROL                    *SelectMenu;
  UI_CONTROL                    *HotkeySelectMenuUp;
  UI_CONTROL                    *HotkeySelectMenuDown;
  UI_CONTROL                    *SelectMenuString;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DummyVerticalLayout4;
  UI_CONTROL                    *ModifyValue;
  UI_CONTROL                    *HotkeyModifyValueDown;
  UI_CONTROL                    *HotkeyModifyValueUp;
  UI_CONTROL                    *ModifyValueString;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *DummyVerticalLayout5;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DummyControl5;
  UI_CONTROL                    *HotkeyLoadDefault;
  UI_CONTROL                    *DummyControl6;
  UI_CONTROL                    *LoadDefaultString;
  UI_CONTROL                    *DummyControl7;
  UI_CONTROL                    *DummyVerticalLayout6;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyControl8;
  UI_CONTROL                    *HotkeySaveAndExit;
  UI_CONTROL                    *DummyControl9;
  UI_CONTROL                    *SaveAndExitString;
  UI_CONTROL                    *DummyControl10;
  UI_CONTROL                    *DummyVerticalLayout7;
  UI_CONTROL                    *DummyHorizontalLayout3;
  UI_CONTROL                    *DummyControl11;
  UI_CONTROL                    *HotkeyEnter;
  UI_CONTROL                    *DummyControl12;
  UI_CONTROL                    *EnterString;
  UI_CONTROL                    *DummyControl13;
  UI_CONTROL                    *DummyVerticalLayout8;
  UI_CONTROL                    *HotkeyDiscardExit;
  UI_CONTROL                    *DiscardExitString;


  Dummy = CreateControl (L"Control", Root);
  CONTROL_CLASS(Root)->AddChild (Root, Dummy);
  UiApplyAttributeList (Dummy, L"name='Dummy'");

  HotkeyBackground = CreateControl (L"Texture", Dummy);
  CONTROL_CLASS(Dummy)->AddChild (Dummy, HotkeyBackground);
  UiApplyAttributeList (HotkeyBackground, L"bkcolor='0xFF333333' bkimage='@OwnerDrawBkg' float='true' height='-1' name='HotkeyBackground' width='-1'");

  HotkeyList = CreateControl (L"HorizontalLayout", Dummy);
  CONTROL_CLASS(Dummy)->AddChild (Dummy, HotkeyList);
  UiApplyAttributeList (HotkeyList, L"name='HotkeyList' padding='15,35,5,35'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1' width='47'");

  HotkeyShowHelp = CreateControl (L"Button", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, HotkeyShowHelp);
  UiApplyAttributeList (HotkeyShowHelp, L"bkcolor='0x0' height='48' hoverimage='@HotkeyShowHelp' name='HotkeyShowHelp' normalimage='@HotkeyShowHelp' pushimage='@HotkeyShowHelp' width='47'");

  ShowHelpString = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, ShowHelpString);
  UiApplyAttributeList (ShowHelpString, L"align='center' bkcolor='0x0' fontsize='15' name='ShowHelpString' textcolor='0xFF999999'");

  DummyControl1 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"name='DummyControl1'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"name='DummyVerticalLayout2' width='102'");

  SelectItem = CreateControl (L"HorizontalLayout", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, SelectItem);
  UiApplyAttributeList (SelectItem, L"height='48' name='SelectItem'");

  HotkeySelectItemUp = CreateControl (L"Button", SelectItem);
  CONTROL_CLASS(SelectItem)->AddChild (SelectItem, HotkeySelectItemUp);
  UiApplyAttributeList (HotkeySelectItemUp, L"bkcolor='0x0' height='48' hoverimage='@HotkeySelectItemUp' name='HotkeySelectItemUp' normalimage='@HotkeySelectItemUp' pushimage='@HotkeySelectItemUp' width='47'");

  HotkeySelectItemDown = CreateControl (L"Button", SelectItem);
  CONTROL_CLASS(SelectItem)->AddChild (SelectItem, HotkeySelectItemDown);
  UiApplyAttributeList (HotkeySelectItemDown, L"bkcolor='0x0' height='48' hoverimage='@HotkeySelectItemDown' name='HotkeySelectItemDown' normalimage='@HotkeySelectItemDown' pushimage='@HotkeySelectItemDown' width='47'");

  SelectItemString = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, SelectItemString);
  UiApplyAttributeList (SelectItemString, L"align='center' bkcolor='0x0' fontsize='15' name='SelectItemString' textcolor='0xFF999999'");

  DummyControl2 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"name='DummyControl2'");

  DummyVerticalLayout3 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout3);
  UiApplyAttributeList (DummyVerticalLayout3, L"name='DummyVerticalLayout3' width='102'");

  SelectMenu = CreateControl (L"HorizontalLayout", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, SelectMenu);
  UiApplyAttributeList (SelectMenu, L"height='48' name='SelectMenu'");

  HotkeySelectMenuUp = CreateControl (L"Button", SelectMenu);
  CONTROL_CLASS(SelectMenu)->AddChild (SelectMenu, HotkeySelectMenuUp);
  UiApplyAttributeList (HotkeySelectMenuUp, L"bkcolor='0x0' height='48' hoverimage='@HotkeySelectMenuUp' name='HotkeySelectMenuUp' normalimage='@HotkeySelectMenuUp' pushimage='@HotkeySelectMenuUp' width='47'");

  HotkeySelectMenuDown = CreateControl (L"Button", SelectMenu);
  CONTROL_CLASS(SelectMenu)->AddChild (SelectMenu, HotkeySelectMenuDown);
  UiApplyAttributeList (HotkeySelectMenuDown, L"bkcolor='0x0' height='48' hoverimage='@HotkeySelectMenuDown' name='HotkeySelectMenuDown' normalimage='@HotkeySelectMenuDown' pushimage='@HotkeySelectMenuDown' width='47'");

  SelectMenuString = CreateControl (L"Label", DummyVerticalLayout3);
  CONTROL_CLASS(DummyVerticalLayout3)->AddChild (DummyVerticalLayout3, SelectMenuString);
  UiApplyAttributeList (SelectMenuString, L"align='center' bkcolor='0x0' fontsize='15' name='SelectMenuString' textcolor='0xFF999999'");

  DummyControl3 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"name='DummyControl3'");

  DummyVerticalLayout4 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout4);
  UiApplyAttributeList (DummyVerticalLayout4, L"name='DummyVerticalLayout4' width='102'");

  ModifyValue = CreateControl (L"HorizontalLayout", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, ModifyValue);
  UiApplyAttributeList (ModifyValue, L"height='48' name='ModifyValue'");

  HotkeyModifyValueDown = CreateControl (L"Button", ModifyValue);
  CONTROL_CLASS(ModifyValue)->AddChild (ModifyValue, HotkeyModifyValueDown);
  UiApplyAttributeList (HotkeyModifyValueDown, L"bkcolor='0x0' height='48' hoverimage='@HotKeyModifyValueDown' name='HotkeyModifyValueDown' normalimage='@HotKeyModifyValueDown' pushimage='@HotKeyModifyValueDown' width='47'");

  HotkeyModifyValueUp = CreateControl (L"Button", ModifyValue);
  CONTROL_CLASS(ModifyValue)->AddChild (ModifyValue, HotkeyModifyValueUp);
  UiApplyAttributeList (HotkeyModifyValueUp, L"bkcolor='0x0' height='48' hoverimage='@HotKeyModifyValueUp' name='HotkeyModifyValueUp' normalimage='@HotKeyModifyValueUp' pushimage='@HotKeyModifyValueUp' width='47'");

  ModifyValueString = CreateControl (L"Label", DummyVerticalLayout4);
  CONTROL_CLASS(DummyVerticalLayout4)->AddChild (DummyVerticalLayout4, ModifyValueString);
  UiApplyAttributeList (ModifyValueString, L"align='center' bkcolor='0x0' fontsize='15' name='ModifyValueString' textcolor='0xFF999999'");

  DummyControl4 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"name='DummyControl4'");

  DummyVerticalLayout5 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout5);
  UiApplyAttributeList (DummyVerticalLayout5, L"name='DummyVerticalLayout5' width='96'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", DummyVerticalLayout5);
  CONTROL_CLASS(DummyVerticalLayout5)->AddChild (DummyVerticalLayout5, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='48' name='DummyHorizontalLayout1'");

  DummyControl5 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl5);
  UiApplyAttributeList (DummyControl5, L"name='DummyControl5'");

  HotkeyLoadDefault = CreateControl (L"Button", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, HotkeyLoadDefault);
  UiApplyAttributeList (HotkeyLoadDefault, L"bkcolor='0x0' height='48' hoverimage='@HotKeyLoadDefault' name='HotkeyLoadDefault' normalimage='@HotKeyLoadDefault' pushimage='@HotKeyLoadDefault' width='47'");

  DummyControl6 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl6);
  UiApplyAttributeList (DummyControl6, L"name='DummyControl6'");

  LoadDefaultString = CreateControl (L"Label", DummyVerticalLayout5);
  CONTROL_CLASS(DummyVerticalLayout5)->AddChild (DummyVerticalLayout5, LoadDefaultString);
  UiApplyAttributeList (LoadDefaultString, L"align='center' bkcolor='0x0' fontsize='15' name='LoadDefaultString' textcolor='0xFF999999'");

  DummyControl7 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl7);
  UiApplyAttributeList (DummyControl7, L"name='DummyControl7'");

  DummyVerticalLayout6 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout6);
  UiApplyAttributeList (DummyVerticalLayout6, L"name='DummyVerticalLayout6' width='90'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyVerticalLayout6);
  CONTROL_CLASS(DummyVerticalLayout6)->AddChild (DummyVerticalLayout6, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"height='48' name='DummyHorizontalLayout2'");

  DummyControl8 = CreateControl (L"Control", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyControl8);
  UiApplyAttributeList (DummyControl8, L"name='DummyControl8'");

  HotkeySaveAndExit = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, HotkeySaveAndExit);
  UiApplyAttributeList (HotkeySaveAndExit, L"bkcolor='0x0' height='48' hoverimage='@HotKeySaveAndExit' name='HotkeySaveAndExit' normalimage='@HotKeySaveAndExit' pushimage='@HotKeySaveAndExit' width='47'");

  DummyControl9 = CreateControl (L"Control", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyControl9);
  UiApplyAttributeList (DummyControl9, L"name='DummyControl9'");

  SaveAndExitString = CreateControl (L"Label", DummyVerticalLayout6);
  CONTROL_CLASS(DummyVerticalLayout6)->AddChild (DummyVerticalLayout6, SaveAndExitString);
  UiApplyAttributeList (SaveAndExitString, L"align='center' bkcolor='0x0' fontsize='15' name='SaveAndExitString' textcolor='0xFF999999'");

  DummyControl10 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl10);
  UiApplyAttributeList (DummyControl10, L"name='DummyControl10'");

  DummyVerticalLayout7 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout7);
  UiApplyAttributeList (DummyVerticalLayout7, L"name='DummyVerticalLayout7' width='106'");

  DummyHorizontalLayout3 = CreateControl (L"HorizontalLayout", DummyVerticalLayout7);
  CONTROL_CLASS(DummyVerticalLayout7)->AddChild (DummyVerticalLayout7, DummyHorizontalLayout3);
  UiApplyAttributeList (DummyHorizontalLayout3, L"height='48' name='DummyHorizontalLayout3'");

  DummyControl11 = CreateControl (L"Control", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl11);
  UiApplyAttributeList (DummyControl11, L"name='DummyControl11'");

  HotkeyEnter = CreateControl (L"Button", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, HotkeyEnter);
  UiApplyAttributeList (HotkeyEnter, L"bkcolor='0x0' height='48' hoverimage='@HotkeyEnter' name='HotkeyEnter' normalimage='@HotkeyEnter' pushimage='@HotkeyEnter' width='47'");

  DummyControl12 = CreateControl (L"Control", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl12);
  UiApplyAttributeList (DummyControl12, L"name='DummyControl12'");

  EnterString = CreateControl (L"Label", DummyVerticalLayout7);
  CONTROL_CLASS(DummyVerticalLayout7)->AddChild (DummyVerticalLayout7, EnterString);
  UiApplyAttributeList (EnterString, L"align='center' bkcolor='0x0' fontsize='15' name='EnterString' textcolor='0xFF999999'");

  DummyControl13 = CreateControl (L"Control", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyControl13);
  UiApplyAttributeList (DummyControl13, L"name='DummyControl13'");

  DummyVerticalLayout8 = CreateControl (L"VerticalLayout", HotkeyList);
  CONTROL_CLASS(HotkeyList)->AddChild (HotkeyList, DummyVerticalLayout8);
  UiApplyAttributeList (DummyVerticalLayout8, L"name='DummyVerticalLayout8' width='47'");

  HotkeyDiscardExit = CreateControl (L"Button", DummyVerticalLayout8);
  CONTROL_CLASS(DummyVerticalLayout8)->AddChild (DummyVerticalLayout8, HotkeyDiscardExit);
  UiApplyAttributeList (HotkeyDiscardExit, L"bkcolor='0x0' height='48' hoverimage='@HotKeyDiscardExit' name='HotkeyDiscardExit' normalimage='@HotKeyDiscardExit' pushimage='@HotKeyDiscardExit' width='47'");

  DiscardExitString = CreateControl (L"Label", DummyVerticalLayout8);
  CONTROL_CLASS(DummyVerticalLayout8)->AddChild (DummyVerticalLayout8, DiscardExitString);
  UiApplyAttributeList (DiscardExitString, L"align='center' bkcolor='0x0' fontsize='15' name='DiscardExitString' textcolor='0xFF999999'");

  return Dummy;
}

typedef struct _HOTKEY_STRING_LIST {
  CHAR16                        *ControlName;
  H2O_EVT_TYPE                  EventType;
} HOTKEY_STRING_LIST;

typedef struct _HOTKEY_BUTTON_LIST {
  CHAR16                        *ControlName;
  HOT_KEY_ACTION                HotkeyAction;
} HOTKEY_BUTTON_LIST;

STATIC HOTKEY_STRING_LIST mHotkeyStringList[] = {
  {L"ShowHelpString"   , H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP     },
  {L"SelectItemString" , H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q      },
  {L"SelectMenuString" , H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P      },
  {L"ModifyValueString", H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q      },
  {L"LoadDefaultString", H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT       },
  {L"SaveAndExitString", H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT   },
  {L"EnterString"      , H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_Q        },
  {L"DiscardExitString", H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT  },
  {NULL, 0},
};

STATIC HOTKEY_BUTTON_LIST mHotkeyButtonList[] = {
  {L"HotkeyShowHelp"        , HotkeyShowHelpMsg     },
  {L"HotkeyLoadDefault"     , HotKeyLoadDefault     },
  {L"HotkeySaveAndExit"     , HotKeySaveAndExit     },
  {L"HotkeyEnter"           , HotKeyEnter           },
  {L"HotkeyDiscardExit"     , HotKeyDiscardExit     },
  {L"HotkeySelectItemUp"    , HotKeySelectItemUp    },
  {L"HotkeySelectItemDown"  , HotKeySelectItemDown  },
  {L"HotkeySelectMenuUp"    , HotkeySelectMenuUp    },
  {L"HotkeySelectMenuDown"  , HotkeySelectMenuDown  },
  {L"HotkeyModifyValueUp"   , HotkeyModifyValueUp   },
  {L"HotkeyModifyValueDown" , HotkeyModifyValueDown },
  {NULL, 0},
};

EFI_STATUS
H2OHotkeyPanelRepaint (
  IN H2O_HOTKEY_PANEL           *This
  )
{
  UI_CONTROL                    *Child;
  HOTKEY_STRING_LIST            *StringList;
  HOTKEY_BUTTON_LIST            *ButtonList;
  CHAR16                        *Description;

  //
  // update hotkey string
  //
  for (StringList = mHotkeyStringList; StringList->ControlName != NULL; StringList++) {
    Description = GetHotkeyDescription (StringList->EventType);
    if (Description != NULL) {
      Child = UiFindChildByName (This, StringList->ControlName);
      if (Child != NULL) {
        UiSetAttribute (Child, L"text", Description);
      }
    }
  }

  //
  // update hotkey button userdata
  //
  for (ButtonList = mHotkeyButtonList; ButtonList->ControlName != NULL; ButtonList++) {
    Child = UiFindChildByName (This, ButtonList->ControlName);
    //
    // disable button keyboard focus
    //
    if (Child != NULL) {
      SetWindowLongPtr (Child->Wnd, GWL_EXSTYLE, GetWindowLongPtr (Child->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
      SetWindowLongPtr (Child->Wnd, GWLP_USERDATA, (INTN) ButtonList->HotkeyAction);
    }
  }

  return EFI_SUCCESS;
}

LRESULT
EFIAPI
H2OHotkeyPanelProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  H2O_HOTKEY_PANEL              *This;
  UI_CONTROL                    *Control;

  This = (H2O_HOTKEY_PANEL *) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *)This;
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }

  switch (Msg) {

  case WM_CREATE:
    This = (H2O_HOTKEY_PANEL *) AllocateZeroPool (sizeof (H2O_HOTKEY_PANEL));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateHotkeyPanelChilds (Control);
    This->HotkeyList = UiFindChildByName (This, L"HotkeyList");
    break;

  case FB_NOTIFY_REPAINT:
    H2OHotkeyPanelRepaint (This);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  }
  return 0;

}


H2O_HOTKEY_PANEL_CLASS *
EFIAPI
GetH2OHotkeyPanelClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"H2OHotkeyPanel", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc = H2OHotkeyPanelProc;

  return CURRENT_CLASS;
}

