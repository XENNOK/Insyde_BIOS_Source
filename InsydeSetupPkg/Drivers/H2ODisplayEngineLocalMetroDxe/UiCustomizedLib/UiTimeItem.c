/** @file
  UI time item Controls

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

STATIC UI_TIME_ITEM_CLASS        *mTimeItemClass = NULL;
#define CURRENT_CLASS            mTimeItemClass

UI_CONTROL *
CreateTimeItemChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *Hour;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyLabel1;
  UI_CONTROL                    *Separation1;
  UI_CONTROL                    *DummyLabel2;
  UI_CONTROL                    *Minute;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyLabel3;
  UI_CONTROL                    *Separation2;
  UI_CONTROL                    *DummyLabel4;
  UI_CONTROL                    *Second;


  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='156' name='DummyHorizontalLayout1'");

  Hour = CreateControl (L"NumberPicker", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Hour);
  UiApplyAttributeList (Hour, L"bkcolor='@menucolor' name='Hour' width='92'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"bkcolor='0xFF333333' width='2' name='DummyVerticalLayout1'");

  DummyLabel1 = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyLabel1);
  UiApplyAttributeList (DummyLabel1, L"height='52' name='DummyLabel1'");

  Separation1 = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, Separation1);
  UiApplyAttributeList (Separation1, L"align='center' align='singleline' bkcolor='0xFF787878' fontsize='29' height='39' name='Separation1' text=':' textcolor='0xFF999999'");

  DummyLabel2 = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyLabel2);
  UiApplyAttributeList (DummyLabel2, L"height='65' name='DummyLabel2'");

  Minute = CreateControl (L"NumberPicker", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Minute);
  UiApplyAttributeList (Minute, L"bkcolor='@menucolor' name='Minute' width='92'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"bkcolor='0xFF333333' width='2' name='DummyVerticalLayout2'");

  DummyLabel3 = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyLabel3);
  UiApplyAttributeList (DummyLabel3, L"height='52' name='DummyLabel3'");

  Separation2 = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, Separation2);
  UiApplyAttributeList (Separation2, L"align='center' align='singleline' bkcolor='0xFF787878' fontsize='29' height='39' name='Separation2' text=':' textcolor='0xFF999999'");

  DummyLabel4 = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyLabel4);
  UiApplyAttributeList (DummyLabel4, L"height='65' name='DummyLabel4'");

  Second = CreateControl (L"NumberPicker", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Second);
  UiApplyAttributeList (Second, L"bkcolor='@menucolor' name='Second' width='92'");

  return DummyHorizontalLayout1;
}

EFI_STATUS
UiTimeItemNumberPickerUpdate (
  IN UI_TIME_ITEM               *This,
  IN EFI_TIME                   *EfiTime
  )
{
  UI_CONTROL                    *ChildControl;
  CHAR16                        Str[20];
  CHAR16                        **ListStr;
  UINT32                        MinValue;
  UINT32                        MaxValue;

  //
  // Hour
  //
  ChildControl = (UI_CONTROL *) This->HourNumberPicker;

  MinValue = 0;
  MaxValue = 23;
  ListStr = CreateStrArrayByNumRange (MinValue, MaxValue, 1, L"%02d");
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%02d", EfiTime->Hour);
  UiSetAttribute (ChildControl, L"currentindex", Str);
  FreeStringArray (ListStr, (MaxValue - MinValue + 1));

  //
  // Minute and second
  //
  MinValue = 0;
  MaxValue = 59;
  ListStr = CreateStrArrayByNumRange (MinValue, MaxValue, 1, L"%02d");

  ChildControl = (UI_CONTROL *) This->MinuteNumberPicker;
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%d", EfiTime->Minute);
  UiSetAttribute (ChildControl, L"currentindex", Str);

  ChildControl = (UI_CONTROL *) This->SecondNumberPicker;
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%02d", EfiTime->Second);
  UiSetAttribute (ChildControl, L"currentindex", Str);

  FreeStringArray (ListStr, (MaxValue - MinValue + 1));

  CONTROL_CLASS_INVALIDATE (This);
  return EFI_SUCCESS;
}

VOID
UiTimeItemSetSelectedControl (
  IN UI_TIME_ITEM               *This,
  IN UI_NUMBER_PICKER           *SelectedControl
  )
{
  if (This->SelectedControl != NULL) {
    CONTROL_CLASS_SET_STATE (This->SelectedControl, 0, UISTATE_SELECTED);
  }

  if (SelectedControl != NULL) {
    CONTROL_CLASS_SET_STATE (SelectedControl, UISTATE_SELECTED, 0);
  }

  This->SelectedControl = SelectedControl;
}

EFI_STATUS
UiTimeItemSetNextValue (
  IN UI_TIME_ITEM               *This,
  IN BOOLEAN                    Increasement
  )
{
  if (This->SelectedControl == This->HourNumberPicker) {
    if (Increasement) {
      This->EfiTime.Hour = (This->EfiTime.Hour== 23) ? 0 : (This->EfiTime.Hour + 1);
    } else {
      This->EfiTime.Hour = (This->EfiTime.Hour == 0) ? 23 : (This->EfiTime.Hour - 1);
    }
  } else if (This->SelectedControl == This->MinuteNumberPicker) {
    if (Increasement) {
      This->EfiTime.Minute = (This->EfiTime.Minute == 59) ? 0 : (This->EfiTime.Minute + 1);
    } else {
      This->EfiTime.Minute = (This->EfiTime.Minute == 0) ? 59 : (This->EfiTime.Minute - 1);
    }
  } else if (This->SelectedControl == This->SecondNumberPicker) {
    if (Increasement) {
      This->EfiTime.Second = (This->EfiTime.Second == 59) ? 0 : (This->EfiTime.Second + 1);
    } else {
      This->EfiTime.Second = (This->EfiTime.Second == 0) ? 59 : (This->EfiTime.Second - 1);
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  UiTimeItemNumberPickerUpdate (This, &This->EfiTime);
  if (This->OnTimeChange != NULL) {
    This->OnTimeChange (This, &This->EfiTime);
  }

  return EFI_SUCCESS;
}

VOID
UiTimeItemNumberPickerOnCurrentIndexChange (
  UI_NUMBER_PICKER              *This,
  INT32                         CurrentIndex
  )
{
  EFI_STATUS                    Status;
  UINT8                         Value;
  UI_CONTROL                    *Control;
  UI_TIME_ITEM                  *TimeItem;
  EFI_TIME                      EfiTime;

  Control  = (UI_CONTROL *) This;
  TimeItem = (UI_TIME_ITEM *) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);
  Value    = (UINT8) StrToUInt (This->List[CurrentIndex], 10, &Status);
  CopyMem (&EfiTime, &TimeItem->EfiTime, sizeof (EFI_TIME));

  if (StrCmp (Control->Name, L"Hour") == 0) {
    EfiTime.Hour    = Value;
  } else if (StrCmp (Control->Name, L"Minute") == 0) {
    EfiTime.Minute  = Value;
  } else if (StrCmp (Control->Name, L"Second") == 0) {
    EfiTime.Second  = Value;
  } else {
    return;
  }

  CopyMem (&TimeItem->EfiTime, &EfiTime, sizeof (EFI_TIME));

  if (TimeItem->OnTimeChange != NULL) {
    TimeItem->OnTimeChange (TimeItem, &TimeItem->EfiTime);
  }

  return;
}

VOID
UiTimeItemNumberPickerOnItemSelected (
  UI_NUMBER_PICKER              *This
  )
{
  UI_CONTROL                    *Control;
  UI_TIME_ITEM                  *TimeItem;

  Control  = (UI_CONTROL *) This;
  TimeItem = (UI_TIME_ITEM *) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);

  if (TimeItem->SelectedControl != NULL) {
    CONTROL_CLASS_SET_STATE (TimeItem->SelectedControl, 0, UISTATE_SELECTED);
  }

  CONTROL_CLASS_SET_STATE (This, UISTATE_SELECTED, 0);
  TimeItem->SelectedControl = This;
}

VOID
UiTimeItemCreate (
  IN OUT UI_TIME_ITEM           *This
  )
{
  UI_CONTROL                    *ChildControl;

  This->HourNumberPicker        = (UI_NUMBER_PICKER *) UiFindChildByName (This, L"Hour" );
  This->MinuteNumberPicker      = (UI_NUMBER_PICKER *) UiFindChildByName (This, L"Minute");
  This->SecondNumberPicker      = (UI_NUMBER_PICKER *) UiFindChildByName (This, L"Second");

  This->HourNumberPicker->OnItemSelected   = UiTimeItemNumberPickerOnItemSelected;
  This->MinuteNumberPicker->OnItemSelected = UiTimeItemNumberPickerOnItemSelected;
  This->SecondNumberPicker->OnItemSelected = UiTimeItemNumberPickerOnItemSelected;

  This->HourNumberPicker->OnCurrentIndexChange   = UiTimeItemNumberPickerOnCurrentIndexChange;
  This->MinuteNumberPicker->OnCurrentIndexChange = UiTimeItemNumberPickerOnCurrentIndexChange;
  This->SecondNumberPicker->OnCurrentIndexChange = UiTimeItemNumberPickerOnCurrentIndexChange;

  ChildControl = (UI_CONTROL *) This->HourNumberPicker;
  SetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA, (INTN) This);
  SetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE  , GetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

  ChildControl = (UI_CONTROL *) This->MinuteNumberPicker;
  SetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA, (INTN) This);
  SetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE  , GetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

  ChildControl = (UI_CONTROL *) This->SecondNumberPicker;
  SetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA, (INTN) This);
  SetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE  , GetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

  UiTimeItemSetSelectedControl (This, This->HourNumberPicker);
  ChildControl = UiFindChildByName (This, L"Separation1");
  UiSetAttribute (ChildControl, L"textcolor", L"0xFFFFFFFF");
  ChildControl = UiFindChildByName (This, L"Separation2");
  UiSetAttribute (ChildControl, L"textcolor", L"0xFFFFFFFF");
}

VOID
UiTimeItemKeyDown (
  HWND                          Wnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_TIME_ITEM                  *This;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;
  UI_CONTROL                    *ChildControl;

  This    = (UI_TIME_ITEM *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL) {
    ASSERT (FALSE);
    return;
  }
  Control = (UI_CONTROL *) This;

  switch (WParam) {

  case VK_RETURN:
    UiTimeItemSetSelectedControl (This, NULL);
    ChildControl = UiFindChildByName (This, L"Separation1");
    UiSetAttribute (ChildControl, L"textcolor", L"0xFF999999");
    ChildControl = UiFindChildByName (This, L"Separation2");
    UiSetAttribute (ChildControl, L"textcolor", L"0xFF999999");
    Manager = Control->Manager;
    SendMessage (Manager->MainWnd, UI_NOTIFY_CARRIAGE_RETURN, (WPARAM) Control, 0);
    break;

  case VK_RIGHT:
    if (This->SelectedControl == This->HourNumberPicker) {
      UiTimeItemSetSelectedControl (This, This->MinuteNumberPicker);
    } else if (This->SelectedControl == This->MinuteNumberPicker) {
      UiTimeItemSetSelectedControl (This, This->SecondNumberPicker);
    }
    break;

  case VK_LEFT:
    if (This->SelectedControl == This->SecondNumberPicker) {
      UiTimeItemSetSelectedControl (This, This->MinuteNumberPicker);
    } else if (This->SelectedControl == This->MinuteNumberPicker) {
      UiTimeItemSetSelectedControl (This, This->HourNumberPicker);
    }
    break;

  case VK_F5:
    UiTimeItemSetNextValue (This, FALSE);
    break;

  case VK_F6:
    UiTimeItemSetNextValue (This, TRUE);
    break;

  default:
    if (This->SelectedControl != NULL) {
      CONTROL_CLASS_WNDPROC (This->SelectedControl, ((UI_CONTROL *) This->SelectedControl)->Wnd, Msg, WParam, LParam);
    }
    break;
  }
}

BOOLEAN
EFIAPI
UiTimeItemSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_TIME_ITEM                  *This;
  EFI_STATUS                    Status;
  EFI_TIME                      *EfiTime;

  This = (UI_TIME_ITEM *) Control;

  if (StrCmp (Name, L"efitime") == 0) {
    EfiTime = (EFI_TIME *) (UINTN) StrToUInt (Value, 16, &Status);

    CopyMem (&This->EfiTime, EfiTime, sizeof(EFI_TIME));
    UiTimeItemNumberPickerUpdate (This, &This->EfiTime);
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

LRESULT
EFIAPI
UiTimeItemProc (
  HWND                          Wnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_TIME_ITEM                  *This;
  UI_CONTROL                    *Control;

  This    = (UI_TIME_ITEM *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *) This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_TIME_ITEM *) AllocateZeroPool (sizeof (UI_TIME_ITEM));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateTimeItemChilds (Control);
    SetWindowLongPtr (Control->Wnd, GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
    UiTimeItemCreate (This);
    break;

  case WM_KEYDOWN:
    UiTimeItemKeyDown (Wnd, Msg, WParam, LParam);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_TIME_ITEM_CLASS *
EFIAPI
GetTimeItemClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"TimeItem", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiTimeItemProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiTimeItemSetAttribute;

  return CURRENT_CLASS;
}

