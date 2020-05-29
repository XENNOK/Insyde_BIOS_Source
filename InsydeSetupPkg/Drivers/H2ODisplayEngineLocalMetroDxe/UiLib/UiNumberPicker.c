/** @file
  UI number picker control

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

STATIC UI_NUMBER_PICKER_CLASS       *mNumberPickerClass = NULL;
#define CURRENT_CLASS               mNumberPickerClass
#define CURRENT_VALUE_TEXT_COLOR L"0xFF999999"

UI_CONTROL *
CreateNumberPickerChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *UpArrow;
  UI_CONTROL                    *UpArrowImage;
  UI_CONTROL                    *PreviousValueLayout;
  UI_CONTROL                    *PreviousValue;
  UI_CONTROL                    *CurrentValue;
  UI_CONTROL                    *NextValueLayout;
  UI_CONTROL                    *NextValue;
  UI_CONTROL                    *DownArrow;
  UI_CONTROL                    *DownArrowImage;


  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  UpArrow = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, UpArrow);
  UiApplyAttributeList (UpArrow, L"height='13' name='UpArrow' visible='false'");

  UpArrowImage = CreateControl (L"Texture", UpArrow);
  CONTROL_CLASS(UpArrow)->AddChild (UpArrow, UpArrowImage);
  UiApplyAttributeList (UpArrowImage, L"bkcolor='0xFF333333' bkimage='@DialogButtonUp' bkimagestyle='center' float='true' name='UpArrowImage'");

  PreviousValueLayout = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, PreviousValueLayout);
  UiApplyAttributeList (PreviousValueLayout, L"height='52' name='PreviousValueLayout'");

  PreviousValue = CreateControl (L"Label", PreviousValueLayout);
  CONTROL_CLASS(PreviousValueLayout)->AddChild (PreviousValueLayout, PreviousValue);
  UiApplyAttributeList (PreviousValue, L"align='center' align='singleline' bkcolor='0xFF333333' fontsize='29' name='PreviousValue' textcolor='0xFF808080'");

  CurrentValue = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, CurrentValue);
  UiApplyAttributeList (CurrentValue, L"align='center' align='singleline' bkcolor='0xFF787878' fontsize='29' height='39' name='CurrentValue'");

  NextValueLayout = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, NextValueLayout);
  UiApplyAttributeList (NextValueLayout, L"height='52' name='NextValueLayout'");

  NextValue = CreateControl (L"Label", NextValueLayout);
  CONTROL_CLASS(NextValueLayout)->AddChild (NextValueLayout, NextValue);
  UiApplyAttributeList (NextValue, L"align='center' align='singleline' bkcolor='0xFF333333' fontsize='29' name='NextValue' textcolor='0xFF808080'");

  DownArrow = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DownArrow);
  UiApplyAttributeList (DownArrow, L"height='13' name='DownArrow' visible='true'");

  DownArrowImage = CreateControl (L"Texture", DownArrow);
  CONTROL_CLASS(DownArrow)->AddChild (DownArrow, DownArrowImage);
  UiApplyAttributeList (DownArrowImage, L"bkcolor='0xFF333333' bkimage='@DialogButtonDown' bkimagestyle='center' float='true' name='DownArrowImage'");

  return DummyVerticalLayout1;
}

UI_CONTROL *
GetControlByPoint (
  IN UI_NUMBER_PICKER           *This,
  IN CHAR16                     **List,
  IN UINT32                     ListCount,
  IN POINT                      *Pt
  )
{
  UINT32                        Index;
  UI_CONTROL                    *ChildControl;
  RECT                          Rc;

  ChildControl = NULL;

  for (Index = 0; Index < ListCount; Index++) {
    ChildControl = UiFindChildByName (This, List[Index]);
    if (ChildControl == NULL) {
      continue;
    }

    GetWindowRect (ChildControl->Wnd, &Rc);
    if (PtInRect (&Rc, *Pt)) {
      return ChildControl;
    }
  }

  return NULL;
}

CHAR16 *
GetPreviousValueStr (
  IN UI_NUMBER_PICKER           *This
  )
{
  if (This->ListCount <= 1 || This->CurrentIndex == -1) {
    return NULL;
  }

  if (This->ListCount == 2) {
    if (This->CurrentIndex == 1) {
      return This->List[0];
    } else {
      return NULL;
    }
  }

  if (This->CurrentIndex == 0) {
    if (This->IsLoop) {
      return This->List[This->ListCount - 1];
    } else {
      return NULL;
    }
  } else {
    return This->List[This->CurrentIndex - 1];
  }
}

CHAR16 *
GetNextValueStr (
  IN UI_NUMBER_PICKER           *This
  )
{
  if (This->ListCount <= 1 || This->CurrentIndex == -1) {
    return NULL;
  }

  if (This->ListCount == 2) {
    if (This->CurrentIndex == 0) {
      return This->List[1];
    } else {
      return NULL;
    }
  }

  if (This->CurrentIndex == (INT32) (This->ListCount - 1)) {
    if (This->IsLoop) {
      return This->List[0];
    } else {
      return NULL;
    }
  } else {
    return This->List[This->CurrentIndex + 1];
  }
}

EFI_STATUS
UpdateValueStr (
  IN UI_NUMBER_PICKER           *This
  )
{
  CHAR16                        *Str;

  if (This->CurrentIndex == -1 || This->List == NULL || This->CurrentIndex >= (INT32) This->ListCount) {
    return EFI_NOT_READY;
  }

  Str = This->List[This->CurrentIndex];
  UiSetAttribute (This->CurrentValueControl , L"text", Str);
  Str = GetPreviousValueStr (This);
  UiSetAttribute (This->PreviousValueControl, L"text", ((Str == NULL) ? L"" : Str));
  Str = GetNextValueStr (This);
  UiSetAttribute (This->NextValueControl    , L"text", ((Str == NULL) ? L"" : Str));

  CONTROL_CLASS_INVALIDATE (This);
  return EFI_SUCCESS;
}

VOID
ShowPreviousNextValue (
  IN UI_NUMBER_PICKER           *This
  )
{
  UiSetAttribute (This->PreviousValueControl , L"visible", L"true");
  UiSetAttribute (This->NextValueControl     , L"visible", L"true");
  UiSetAttribute (This->UpArrowImageControl  , L"visible", L"true");
  UiSetAttribute (This->DownArrowImageControl, L"visible", L"true");

  CONTROL_CLASS_INVALIDATE (This);
}

VOID
HidePreviousNextValue (
  UI_NUMBER_PICKER              *This
  )
{
  UiSetAttribute (This->PreviousValueControl , L"visible", L"false");
  UiSetAttribute (This->NextValueControl     , L"visible", L"false");
  UiSetAttribute (This->UpArrowImageControl  , L"visible", L"false");
  UiSetAttribute (This->DownArrowImageControl, L"visible", L"false");

  CONTROL_CLASS_INVALIDATE (This);
}

INT32
GetNextIndex (
  IN UI_NUMBER_PICKER           *This,
  IN BOOLEAN                    Increasement
  )
{
  INT32                         NextIndex;

  NextIndex = This->CurrentIndex;

  if (Increasement) {
    if (This->CurrentIndex < (INT32) (This->ListCount - 1)) {
      NextIndex = This->CurrentIndex + 1;
    } else {
      NextIndex = (This->IsLoop) ? 0 : This->CurrentIndex;
    }
  } else {
    if (This->CurrentIndex > 0) {
      NextIndex = This->CurrentIndex - 1;
    } else {
      NextIndex = (This->IsLoop) ? (This->ListCount - 1) : This->CurrentIndex;
    }
  }

  return NextIndex;
}

EFI_STATUS
SetCurrentIndex (
  IN UI_NUMBER_PICKER           *This,
  IN INT32                      CurrentIndex
  )
{
  if (CurrentIndex == -1 || CurrentIndex >= (INT32) This->ListCount) {
    return EFI_INVALID_PARAMETER;
  }

  if (CurrentIndex == This->CurrentIndex) {
    return EFI_SUCCESS;
  }

  This->CurrentIndex = CurrentIndex;
  UpdateValueStr (This);

  return EFI_SUCCESS;
}

VOID
SetCurrentIndexByMouseClick (
  IN UI_NUMBER_PICKER           *This,
  IN POINT                      *Pt
  )
{
  INT32                         CurrentIndex;
  UI_CONTROL                    *ChildControl;
  CHAR16                        *ClickControlList[] = {L"UpArrow",
                                                       L"PreviousValue",
                                                       L"NextValue",
                                                       L"DownArrow"
                                                       };

  ChildControl = GetControlByPoint (
                   This,
                   ClickControlList,
                   sizeof (ClickControlList) / sizeof (CHAR16 *),
                   Pt
                   );
  if (ChildControl == NULL) {
    return;
  }

  if (ChildControl != This->ButtonDownControl) {
    //
    // Button up and button down controls are not the same
    //
    return;
  }

  if ((ChildControl == This->UpArrowControl) ||
      (ChildControl == This->NextValueControl)) {
    CurrentIndex = GetNextIndex (This, TRUE);
  } else {
    CurrentIndex = GetNextIndex (This, FALSE);
  }

  SetCurrentIndex (This, CurrentIndex);
}

VOID
UiNumberPickerButtonDown (
  IN UI_NUMBER_PICKER           *This,
  IN POINT                      *Pt
  )
{
  UI_CONTROL                    *ChildControl;
  CHAR16                        *List[] = {L"UpArrow",
                                           L"PreviousValue",
                                           L"CurrentValue",
                                           L"NextValue",
                                           L"DownArrow"
                                           };
  This->ButtonDownListIndex = -1;

  ChildControl = GetControlByPoint (
                   This,
                   List,
                   sizeof (List) / sizeof (CHAR16 *),
                   Pt
                   );
  if (ChildControl == NULL) {
    return;
  }

  if (This->DisplayMode == DISPLAY_SELECTION_WHEN_HOVER && !This->Hover && StrCmp (ChildControl->Name, L"CurrentValue") != 0) {
    return;
  }

  if (This->DisplayMode == DISPLAY_SELECTION_WHEN_PRESS) {
    if ((StrCmp (ChildControl->Name, L"CurrentValue") != 0)) {
      return;
    }

    ShowPreviousNextValue (This);
  }

  CopyMem (&This->ButtonDownPoint, Pt, sizeof (POINT));
  This->ButtonDownListIndex = This->CurrentIndex;
  This->ButtonDownControl   = ChildControl;

  if (This->OnItemSelected != NULL) {
    This->OnItemSelected (This);
  }
}

VOID
UiNumberPickerMouseMove (
  IN UI_NUMBER_PICKER           *This,
  IN POINT                      *Pt
  )
{
  INT32                         CurrentIndex;
  INT32                         Difference;
  INT32                         DifferenceIndex;
  INT32                         ListCount;
  BOOLEAN                       Increasment;

  if (This->ButtonDownListIndex == -1) {
    //
    // Return when user button down control is not found
    //
    return;
  }
  if ((This->ButtonDownControl == This->UpArrowControl) ||
      (This->ButtonDownControl == This->DownArrowControl)) {
    //
    // Return when user button down control is UpArrow or DownArrow
    //
    return;
  }

  Difference = Pt->y - This->ButtonDownPoint.y;
  if (abs (Difference) < This->PixelPerStep) {
    return;
  }

  CurrentIndex    = This->ButtonDownListIndex;
  ListCount       = (INT32) This->ListCount;
  Increasment     = (Difference < 0) ? TRUE : FALSE;
  DifferenceIndex = (abs (Difference) / This->PixelPerStep) % ListCount;

  if (Increasment) {
    if (CurrentIndex + DifferenceIndex >= ListCount) {
      if (This->IsLoop) {
        DifferenceIndex -= (ListCount - CurrentIndex);
        CurrentIndex = DifferenceIndex;
      } else {
        CurrentIndex = (ListCount > 1) ? (ListCount - 1) : 0;
      }
    } else {
      CurrentIndex += DifferenceIndex;
    }
  } else {
    if (DifferenceIndex > CurrentIndex) {
      if (This->IsLoop) {
        DifferenceIndex -= (CurrentIndex + 1);
        CurrentIndex = (ListCount > 1) ? (ListCount - 1 - DifferenceIndex) : 0;
      } else {
        CurrentIndex = 0;
      }
    } else {
      CurrentIndex -= DifferenceIndex;
    }
  }

  SetCurrentIndex (This, CurrentIndex);
}

VOID
UiNumberPickerButtonUp (
  IN UI_NUMBER_PICKER           *This,
  IN POINT                      Pt
  )
{
  if (This->CurrentIndex == This->ButtonDownListIndex) {
    SetCurrentIndexByMouseClick (This, &Pt);
  }

  if (This->CurrentIndex != This->ButtonDownListIndex &&
      This->OnCurrentIndexChange != NULL) {
    This->OnCurrentIndexChange (This, This->CurrentIndex);
  }

  if (This->DisplayMode == DISPLAY_SELECTION_WHEN_PRESS && This->ButtonDownListIndex != -1) {
    HidePreviousNextValue (This);
  }

  This->ButtonDownListIndex = -1;
  This->ButtonDownControl   = NULL;
}

LRESULT
UiNumberPickerKeyDown (
  HWND                          Hwnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_NUMBER_PICKER              *This;
  INT32                         CurrentIndex;

  This = (UI_NUMBER_PICKER *) GetWindowLongPtr (Hwnd, 0);

  switch (WParam) {

  case VK_UP:
  case VK_OEM_PLUS:
    CurrentIndex = GetNextIndex (This, TRUE);
    SetCurrentIndex (This, CurrentIndex);
    if (This->OnCurrentIndexChange != NULL) {
      This->OnCurrentIndexChange (This, CurrentIndex);
    }
    break;

  case VK_DOWN:
  case VK_OEM_MINUS:
    CurrentIndex = GetNextIndex (This, FALSE);
    SetCurrentIndex (This, CurrentIndex);
    if (This->OnCurrentIndexChange != NULL) {
      This->OnCurrentIndexChange (This, CurrentIndex);
    }
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

LRESULT
CALLBACK
UiNumberPickerCallback (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_NUMBER_PICKER              *This;
  POINT                         Pt;
  RECT                          Rc;
  UI_CONTROL                    *ChildControl;
  CHAR16                        *List[] = {L"UpArrow",
                                           L"PreviousValue",
                                           L"CurrentValue",
                                           L"NextValue",
                                           L"DownArrow"
                                           };

  This  = (UI_NUMBER_PICKER *) GetWindowLongPtr (Wnd, 0);
  if (This->ButtonDownListIndex != -1) {
    //
    // Return when user is dragging by mouse
    //
    return 0;
  }

  GetCursorPos (&Pt);

  if (This->Hover) {
    ChildControl = GetControlByPoint (
                     This,
                     List,
                     sizeof (List) / sizeof (CHAR16 *),
                     &Pt
                     );
    if (ChildControl == NULL) {
      This->Hover = FALSE;
      HidePreviousNextValue (This);
    }
  } else {
    GetWindowRect (This->CurrentValueControl->Wnd, &Rc);
    if (PtInRect (&Rc, Pt)) {
      This->Hover = TRUE;
      ShowPreviousNextValue (This);
    }
  }

  return 0;
}

EFI_STATUS
UiNumberPickerCreate (
  IN OUT UI_NUMBER_PICKER       *This
  )
{
  This->CurrentIndex          = -1;
  This->ButtonDownListIndex   = -1;
  This->PixelPerStep          = 5;
  This->IsLoop                = TRUE;
  This->DisplayMode           = ALWAYS_DISPLAY_SELECTION;

  This->UpArrowControl        = UiFindChildByName (This, L"UpArrow");
  This->UpArrowImageControl   = UiFindChildByName (This, L"UpArrowImage");
  This->PreviousValueControl  = UiFindChildByName (This, L"PreviousValue");
  This->CurrentValueControl   = UiFindChildByName (This, L"CurrentValue");
  This->NextValueControl      = UiFindChildByName (This, L"NextValue");
  This->DownArrowControl      = UiFindChildByName (This, L"DownArrow");
  This->DownArrowImageControl = UiFindChildByName (This, L"DownArrowImage");

  UiSetAttribute (This->CurrentValueControl, L"textcolor", CURRENT_VALUE_TEXT_COLOR);

  switch (This->DisplayMode) {

  case DISPLAY_SELECTION_WHEN_HOVER:
    //
    // BUGBUG:
    // Should use WM_MOUSEENTER and WM_MOUSELEAVE msg to implement hover.
    // But this two message cannot work properly, so use timer event to implement this feature temporarily.
    //
    SetTimer (((UI_CONTROL *) This)->Wnd, 0, 1, UiNumberPickerCallback);
    HidePreviousNextValue (This);
    break;

  case DISPLAY_SELECTION_WHEN_PRESS:
    HidePreviousNextValue (This);
    break;

  default:
    break;
  }

  return EFI_SUCCESS;
}

BOOLEAN
EFIAPI
UiNumberPickerSetAttribute (
  UI_CONTROL                    *Control,
  CHAR16                        *Name,
  CHAR16                        *Value
  )
{
  UI_NUMBER_PICKER              *This;
  EFI_STATUS                    Status;
  CHAR16                        **List;

  This = (UI_NUMBER_PICKER *) Control;

  if (StrCmp (Name, L"list") == 0) {
    //
    // Memory address of string array. It contains the string pointer of each item of number picker
    //
    FreeStringArray (This->List, This->ListCount);
    List            = (CHAR16 **) (UINTN) StrToUInt (Value, 16, &Status);
    This->ListCount = GetStringArrayCount (List);
    This->List      = CopyStringArray (List, This->ListCount);
    UpdateValueStr (This);
  } else if (StrCmp (Name, L"currentindex") == 0) {
    //
    // Index value of string array for current value
    //
    This->CurrentIndex = wcstol (Value, NULL, 10);
    UpdateValueStr (This);
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);
  return TRUE;
}

VOID
EFIAPI
UiNumberPickerSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  UI_NUMBER_PICKER              *This;

  This = (UI_NUMBER_PICKER *) Control;

  if ((SetState & UISTATE_SELECTED) == UISTATE_SELECTED) {
    UiSetAttribute (This->CurrentValueControl  , L"textcolor"   , L"0xFFFFFFFF");
    UiSetAttribute (This->UpArrowImageControl  , L"bkimagestyle", L"center|light");
    UiSetAttribute (This->DownArrowImageControl, L"bkimagestyle", L"center|light");
  }

  if ((ClearState & UISTATE_SELECTED) == UISTATE_SELECTED) {
    UiSetAttribute (This->CurrentValueControl  , L"textcolor"   , CURRENT_VALUE_TEXT_COLOR);
    UiSetAttribute (This->UpArrowImageControl  , L"bkimagestyle", L"center");
    UiSetAttribute (This->DownArrowImageControl, L"bkimagestyle", L"center");
  }

  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetState (Control, SetState, ClearState);
}

LRESULT
EFIAPI
UiNumberPickerProc (
  HWND                          Hwnd,
  UINT32                        Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_NUMBER_PICKER              *This;
  UI_CONTROL                    *Control;
  POINT                         CurrentPoint;

  This    = (UI_NUMBER_PICKER *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *) This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_NUMBER_PICKER *) AllocateZeroPool (sizeof (UI_NUMBER_PICKER));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    CreateNumberPickerChilds (Control);
    SetWindowLongPtr (Control->Wnd, GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
    UiNumberPickerCreate (This);
    break;

  case WM_DESTROY:
    if (This->DisplayMode == DISPLAY_SELECTION_WHEN_HOVER) {
      KillTimer (Hwnd, 0);
    }
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  case WM_NCLBUTTONDOWN:
    POINTSTOPOINT (CurrentPoint, LParam);
    UiNumberPickerButtonDown (This, &CurrentPoint);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  case WM_NCMOUSEMOVE:
    POINTSTOPOINT (CurrentPoint, LParam);
    UiNumberPickerMouseMove (This, &CurrentPoint);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  case WM_NCLBUTTONUP:
    POINTSTOPOINT (CurrentPoint, LParam);
    UiNumberPickerButtonUp (This, CurrentPoint);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  case WM_KEYDOWN:
    UiNumberPickerKeyDown (Hwnd, Msg, WParam, LParam);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_NUMBER_PICKER_CLASS *
EFIAPI
GetNumberPickerClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"NumberPicker", (UI_CONTROL_CLASS *) GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiNumberPickerProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetState     = UiNumberPickerSetState;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiNumberPickerSetAttribute;

  return CURRENT_CLASS;
}

