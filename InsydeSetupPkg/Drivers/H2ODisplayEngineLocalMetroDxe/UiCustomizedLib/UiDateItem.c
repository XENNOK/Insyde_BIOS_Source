/** @file
  UI date item Controls

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

STATIC UI_DATE_ITEM_CLASS        *mDateItemClass = NULL;
#define CURRENT_CLASS            mDateItemClass

#define YEAR_MIN      2000
#define YEAR_MAX      2099

UI_CONTROL *
CreateDateItemChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *Year;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyLabel1;
  UI_CONTROL                    *Separation1;
  UI_CONTROL                    *DummyLabel2;
  UI_CONTROL                    *Month;
  UI_CONTROL                    *DummyVerticalLayout2;
  UI_CONTROL                    *DummyLabel3;
  UI_CONTROL                    *Separation2;
  UI_CONTROL                    *DummyLabel4;
  UI_CONTROL                    *Day;


  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='156' name='DummyHorizontalLayout1'");

  Year = CreateControl (L"NumberPicker", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Year);
  UiApplyAttributeList (Year, L"bkcolor='@menucolor' name='Year' width='105'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"bkcolor='0xFF333333' width='5' name='DummyVerticalLayout1'");

  DummyLabel1 = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyLabel1);
  UiApplyAttributeList (DummyLabel1, L"height='52' name='DummyLabel1'");

  Separation1 = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, Separation1);
  UiApplyAttributeList (Separation1, L"align='center' align='singleline' bkcolor='0xFF787878' fontsize='29' height='39' name='Separation1' text=':' textcolor='0xFF999999'");

  DummyLabel2 = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyLabel2);
  UiApplyAttributeList (DummyLabel2, L"height='65' name='DummyLabel2'");

  Month = CreateControl (L"NumberPicker", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Month);
  UiApplyAttributeList (Month, L"bkcolor='@menucolor' name='Month' width='86'");

  DummyVerticalLayout2 = CreateControl (L"VerticalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyVerticalLayout2);
  UiApplyAttributeList (DummyVerticalLayout2, L"bkcolor='0xFF333333' width='5' name='DummyVerticalLayout2'");

  DummyLabel3 = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyLabel3);
  UiApplyAttributeList (DummyLabel3, L"height='52' name='DummyLabel3'");

  Separation2 = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, Separation2);
  UiApplyAttributeList (Separation2, L"align='center' align='singleline' bkcolor='0xFF787878' fontsize='29' height='39' name='Separation2' text=':' textcolor='0xFF999999'");

  DummyLabel4 = CreateControl (L"Label", DummyVerticalLayout2);
  CONTROL_CLASS(DummyVerticalLayout2)->AddChild (DummyVerticalLayout2, DummyLabel4);
  UiApplyAttributeList (DummyLabel4, L"height='65' name='DummyLabel4'");

  Day = CreateControl (L"NumberPicker", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, Day);
  UiApplyAttributeList (Day, L"bkcolor='@menucolor' name='Day' width='85'");

  return DummyHorizontalLayout1;
}

EFI_STATUS
UiDateItemNumberPickerUpdate (
  IN UI_DATE_ITEM               *This,
  IN EFI_TIME                   *EfiTime
  )
{
  UI_CONTROL                    *ChildControl;
  CHAR16                        Str[20];
  CHAR16                        **ListStr;
  UINT32                        MinValue;
  UINT32                        MaxValue;
  UINT8                         DayOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  //
  // Year
  //
  ChildControl = (UI_CONTROL *) This->YearNumberPicker;

  MinValue = YEAR_MIN;
  MaxValue = YEAR_MAX;
  ListStr = CreateStrArrayByNumRange (MinValue, MaxValue, 1, L"%04d");
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%02d", EfiTime->Year - YEAR_MIN);
  UiSetAttribute (ChildControl, L"currentindex", Str);
  FreeStringArray (ListStr, (MaxValue - MinValue + 1));

  //
  // Month
  //
  ChildControl = (UI_CONTROL *) This->MonthNumberPicker;

  MinValue = 1;
  MaxValue = 12;
  ListStr = CreateStrArrayByNumRange (MinValue, MaxValue, 1, L"%02d");
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%d", EfiTime->Month - 1);
  UiSetAttribute (ChildControl, L"currentindex", Str);
  FreeStringArray (ListStr, (MaxValue - MinValue + 1));

  //
  // Day
  //
  ChildControl = (UI_CONTROL *) This->DayNumberPicker;

  MinValue = 1;
  if (EfiTime->Month == 2 && IsLeapYear (EfiTime->Year)) {
    MaxValue = 29;
  } else {
    MaxValue = (UINT32) DayOfMonth[EfiTime->Month - 1];
  }
  ListStr = CreateStrArrayByNumRange (MinValue, MaxValue, 1, L"%02d");
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%02d", EfiTime->Day - 1);
  UiSetAttribute (ChildControl, L"currentindex", Str);
  FreeStringArray (ListStr, (MaxValue - MinValue + 1));

  CONTROL_CLASS_INVALIDATE (This);
  return EFI_SUCCESS;
}

VOID
UiDateItemSetSelectedControl (
  IN UI_DATE_ITEM               *This,
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
UiDateItemSetNextValue (
  IN UI_DATE_ITEM               *This,
  IN BOOLEAN                    Increasement
  )
{
  UINT8                         DayOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  UINT8                         MonthIndex;

  if (This->SelectedControl == This->YearNumberPicker) {
    if (Increasement) {
      This->EfiTime.Year = (This->EfiTime.Year== YEAR_MAX) ? YEAR_MIN : (This->EfiTime.Year + 1);
    } else {
      This->EfiTime.Year = (This->EfiTime.Year == YEAR_MIN) ? YEAR_MAX : (This->EfiTime.Year - 1);
    }
  } else if (This->SelectedControl == This->MonthNumberPicker) {
    if (Increasement) {
      This->EfiTime.Month = (This->EfiTime.Month == 12) ? 1 : (This->EfiTime.Month + 1);
    } else {
      This->EfiTime.Month = (This->EfiTime.Month == 1) ? 12 : (This->EfiTime.Month - 1);
    }
  } else if (This->SelectedControl == This->DayNumberPicker) {
    MonthIndex = This->EfiTime.Month - 1;

    if (Increasement) {
      if (This->EfiTime.Month == 2 && IsLeapYear (This->EfiTime.Year)) {
        This->EfiTime.Day = (This->EfiTime.Day == 29) ? 1 : (This->EfiTime.Day + 1);
      }  else {
        This->EfiTime.Day = (This->EfiTime.Day == DayOfMonth[MonthIndex]) ? 1 : (This->EfiTime.Day + 1);
      }
    } else {
      if (This->EfiTime.Month == 2 && IsLeapYear (This->EfiTime.Year)) {
        This->EfiTime.Day = (This->EfiTime.Day == 1) ? 29 : (This->EfiTime.Day - 1);
      } else {
        This->EfiTime.Day = (This->EfiTime.Day == 1) ? DayOfMonth[MonthIndex] : (This->EfiTime.Day - 1);
      }
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  while (!IsDayValid (&This->EfiTime)) {
    This->EfiTime.Day--;
    if (This->EfiTime.Day == 0) {
      ASSERT(FALSE);
      return EFI_ABORTED;
    }
  }

  UiDateItemNumberPickerUpdate (This, &This->EfiTime);
  if (This->OnDateChange != NULL) {
    This->OnDateChange (This, &This->EfiTime);
  }

  return EFI_SUCCESS;
}

VOID
UiDateItemNumberPickerOnCurrentIndexChange (
  UI_NUMBER_PICKER              *This,
  INT32                         CurrentIndex
  )
{
  BOOLEAN                       IsDay;
  BOOLEAN                       IsYear;
  EFI_STATUS                    Status;
  UINT16                        Value;
  UINT8                         DayOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  CHAR16                        Str[20];
  UINT16                        MinValue;
  UINT16                        MaxValue;
  UI_CONTROL                    *Control;
  UI_CONTROL                    *ChildControl;
  UI_DATE_ITEM                  *DateItem;
  EFI_TIME                      EfiTime;
  CHAR16                        **ListStr;

  IsDay    = FALSE;
  IsYear   = FALSE;
  Control  = (UI_CONTROL *) This;
  DateItem = (UI_DATE_ITEM *) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);
  Value    = (UINT16) StrToUInt (This->List[CurrentIndex], 10, &Status);
  CopyMem (&EfiTime, &DateItem->EfiTime, sizeof (EFI_TIME));

  if (StrCmp (Control->Name, L"Day") == 0) {
    IsDay = TRUE;
    EfiTime.Day   = (UINT8) Value;
  } else if (StrCmp (Control->Name, L"Month") == 0) {
    EfiTime.Month = (UINT8) Value;
  } else if (StrCmp (Control->Name, L"Year") == 0) {
    IsYear = TRUE;
    EfiTime.Year  = Value;
  } else {
    return;
  }

  while (!IsDayValid (&EfiTime)) {
    EfiTime.Day--;
    if (EfiTime.Day == 0) {
      ASSERT(FALSE);
      return;
    }
  }

  CopyMem (&DateItem->EfiTime, &EfiTime, sizeof (EFI_TIME));

  if (DateItem->OnDateChange != NULL) {
    DateItem->OnDateChange (DateItem, &DateItem->EfiTime);
  }

  if (IsDay || (IsYear && EfiTime.Month != 2)) {
    return;
  }

  //
  // Update string list of day
  //
  MinValue     = 1;
  if (EfiTime.Month == 2 && IsLeapYear (EfiTime.Year)) {
    MaxValue = 29;
  } else {
    MaxValue = DayOfMonth[EfiTime.Month - 1];
  }

  ListStr = CreateStrArrayByNumRange (MinValue, MaxValue, 1, L"%02d");
  ChildControl = (UI_CONTROL *) DateItem->DayNumberPicker;
  UnicodeSPrint (Str, sizeof (Str), L"0x%x", ListStr);
  UiSetAttribute (ChildControl, L"list", Str);
  UnicodeSPrint (Str, sizeof (Str), L"%02d", EfiTime.Day - 1);
  UiSetAttribute (ChildControl, L"currentindex", Str);
  FreeStringArray (ListStr, (MaxValue - MinValue + 1));

  return;
}

VOID
UiDateItemNumberPickerOnItemSelected (
  UI_NUMBER_PICKER              *This
  )
{
  UI_CONTROL                    *Control;
  UI_DATE_ITEM                  *DateItem;

  Control  = (UI_CONTROL *) This;
  DateItem = (UI_DATE_ITEM *) GetWindowLongPtr (Control->Wnd, GWLP_USERDATA);

  if (DateItem->SelectedControl != NULL) {
    CONTROL_CLASS_SET_STATE (DateItem->SelectedControl, 0, UISTATE_SELECTED);
  }

  CONTROL_CLASS_SET_STATE (This, UISTATE_SELECTED, 0);
  DateItem->SelectedControl = This;
}

VOID
UiDateItemCreate (
  IN OUT UI_DATE_ITEM           *This
  )
{
  UI_CONTROL                    *ChildControl;

  This->YearNumberPicker        = (UI_NUMBER_PICKER *) UiFindChildByName (This, L"Year");
  This->MonthNumberPicker       = (UI_NUMBER_PICKER *) UiFindChildByName (This, L"Month");
  This->DayNumberPicker         = (UI_NUMBER_PICKER *) UiFindChildByName (This, L"Day");

  This->YearNumberPicker->OnCurrentIndexChange  = UiDateItemNumberPickerOnCurrentIndexChange;
  This->MonthNumberPicker->OnCurrentIndexChange = UiDateItemNumberPickerOnCurrentIndexChange;
  This->DayNumberPicker->OnCurrentIndexChange   = UiDateItemNumberPickerOnCurrentIndexChange;

  This->YearNumberPicker->OnItemSelected  = UiDateItemNumberPickerOnItemSelected;
  This->MonthNumberPicker->OnItemSelected = UiDateItemNumberPickerOnItemSelected;
  This->DayNumberPicker->OnItemSelected   = UiDateItemNumberPickerOnItemSelected;

  ChildControl = (UI_CONTROL *) This->YearNumberPicker;
  SetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA, (INTN) This);
  SetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE  , GetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

  ChildControl = (UI_CONTROL *) This->MonthNumberPicker;
  SetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA, (INTN) This);
  SetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE  , GetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

  ChildControl = (UI_CONTROL *) This->DayNumberPicker;
  SetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA, (INTN) This);
  SetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE  , GetWindowLongPtr (ChildControl->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

  UiDateItemSetSelectedControl (This, This->YearNumberPicker);
  ChildControl = UiFindChildByName (This, L"Separation1");
  UiSetAttribute (ChildControl, L"textcolor", L"0xFFFFFFFF");
  ChildControl = UiFindChildByName (This, L"Separation2");
  UiSetAttribute (ChildControl, L"textcolor", L"0xFFFFFFFF");
}

VOID
UiDateItemKeyDown (
  HWND                          Wnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DATE_ITEM                  *This;
  UI_CONTROL                    *Control;
  UI_MANAGER                    *Manager;
  UI_CONTROL                    *ChildControl;

  This    = (UI_DATE_ITEM *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL) {
    ASSERT (FALSE);
    return;
  }
  Control = (UI_CONTROL *) This;

  switch (WParam) {

  case VK_RETURN:
    UiDateItemSetSelectedControl (This, NULL);
    ChildControl = UiFindChildByName (This, L"Separation1");
    UiSetAttribute (ChildControl, L"textcolor", L"0xFF999999");
    ChildControl = UiFindChildByName (This, L"Separation2");
    UiSetAttribute (ChildControl, L"textcolor", L"0xFF999999");
    Manager = Control->Manager;
    SendMessage (Manager->MainWnd, UI_NOTIFY_CARRIAGE_RETURN, (WPARAM) Control, 0);
    break;

  case VK_RIGHT:
    if (This->SelectedControl == This->YearNumberPicker) {
      UiDateItemSetSelectedControl (This, This->MonthNumberPicker);
    } else if (This->SelectedControl == This->MonthNumberPicker) {
      UiDateItemSetSelectedControl (This, This->DayNumberPicker);
    }
    break;

  case VK_LEFT:
    if (This->SelectedControl == This->DayNumberPicker) {
      UiDateItemSetSelectedControl (This, This->MonthNumberPicker);
    } else if (This->SelectedControl == This->MonthNumberPicker) {
      UiDateItemSetSelectedControl (This, This->YearNumberPicker);
    }
    break;

  case VK_F5:
    UiDateItemSetNextValue (This, FALSE);
    break;

  case VK_F6:
    UiDateItemSetNextValue (This, TRUE);
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
UiDateItemSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_DATE_ITEM                  *This;
  EFI_STATUS                    Status;
  EFI_TIME                      *EfiTime;

  This = (UI_DATE_ITEM *) Control;

  if (StrCmp (Name, L"efitime") == 0) {
    EfiTime = (EFI_TIME *) (UINTN) StrToUInt (Value, 16, &Status);
    ASSERT(IsDayValid (EfiTime));

    CopyMem (&This->EfiTime, EfiTime, sizeof(EFI_TIME));
    UiDateItemNumberPickerUpdate (This, &This->EfiTime);
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}

LRESULT
EFIAPI
UiDateItemProc (
  HWND                          Wnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DATE_ITEM                  *This;
  UI_CONTROL                    *Control;

  This    = (UI_DATE_ITEM *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *) This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_DATE_ITEM *) AllocateZeroPool (sizeof (UI_DATE_ITEM));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateDateItemChilds (Control);
    SetWindowLongPtr (Control->Wnd, GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
    UiDateItemCreate (This);
    break;

  case WM_KEYDOWN:
    UiDateItemKeyDown (Wnd, Msg, WParam, LParam);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_DATE_ITEM_CLASS *
EFIAPI
GetDateItemClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"DateItem", (UI_CONTROL_CLASS *)GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiDateItemProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiDateItemSetAttribute;

  return CURRENT_CLASS;
}

