/** @file
  UI ordered list control

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

UI_CONTROL *
CreateOrderedListDialogChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *OrderedListDialog;
  UI_CONTROL                    *DummyVerticalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *OrderedListDialogTitle;
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *OrderedListDialogImage;
  UI_CONTROL                    *OrderedListDialogText;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *OrderedListList;
  UI_CONTROL                    *DummyControl3;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyControl4;
  UI_CONTROL                    *Ok;
  UI_CONTROL                    *Cancel;


  OrderedListDialog = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, OrderedListDialog);
  UiApplyAttributeList (OrderedListDialog, L"bkcolor='@menucolor' name='OrderedListDialog' padding='40,30,40,30'");

  DummyVerticalLayout1 = CreateControl (L"VerticalLayout", OrderedListDialog);
  CONTROL_CLASS(OrderedListDialog)->AddChild (OrderedListDialog, DummyVerticalLayout1);
  UiApplyAttributeList (DummyVerticalLayout1, L"name='DummyVerticalLayout1'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"height='80' name='DummyHorizontalLayout1' padding='0,0,30,0'");

  OrderedListDialogTitle = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OrderedListDialogTitle);
  UiApplyAttributeList (OrderedListDialogTitle, L"fontsize='29' name='OrderedListDialogTitle' textcolor='0xFFFFFFFF' width='200'");

  DummyControl1 = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"name='DummyControl1' padding='0,25,0,25'");

  OrderedListDialogImage = CreateControl (L"Texture", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, OrderedListDialogImage);
  UiApplyAttributeList (OrderedListDialogImage, L"bkimagestyle='light|stretch' name='OrderedListDialogImage'");

  OrderedListDialogText = CreateControl (L"Label", DummyVerticalLayout1);
  CONTROL_CLASS(DummyVerticalLayout1)->AddChild (DummyVerticalLayout1, OrderedListDialogText);
  UiApplyAttributeList (OrderedListDialogText, L"fontsize='19' name='OrderedListDialogText' padding='0,0,45,0' textcolor='0xFFA8D2DF'");

  DummyControl2 = CreateControl (L"Control", OrderedListDialog);
  CONTROL_CLASS(OrderedListDialog)->AddChild (OrderedListDialog, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"bkcolor='0x0' bkimage='@DialogSeparator' height='10' name='DummyControl2'");

  OrderedListList = CreateControl (L"ListView", OrderedListDialog);
  CONTROL_CLASS(OrderedListDialog)->AddChild (OrderedListDialog, OrderedListList);
  UiApplyAttributeList (OrderedListList, L"childpadding='2' moveitemsupport='true' name='OrderedListList' vscrollbar='true'");

  DummyControl3 = CreateControl (L"Control", OrderedListDialog);
  CONTROL_CLASS(OrderedListDialog)->AddChild (OrderedListDialog, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"height='12' name='DummyControl3'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", OrderedListDialog);
  CONTROL_CLASS(OrderedListDialog)->AddChild (OrderedListDialog, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"childpadding='2' height='55' name='DummyHorizontalLayout2' padding='0,0,30,0' width='300'");

  DummyControl4 = CreateControl (L"Control", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyControl4);
  UiApplyAttributeList (DummyControl4, L"name='DummyControl4'");

  Ok = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, Ok);
  UiApplyAttributeList (Ok, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Ok' text='YES' textcolor='0xFFFFFFFF' width='55'");

  Cancel = CreateControl (L"Button", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, Cancel);
  UiApplyAttributeList (Cancel, L"align='center' align='singleline' bkcolor='0xFFCCCCCC' focusbkcolor='@menulightcolor' fontsize='19' height='30' name='Cancel' text='NO' textcolor='0xFFFFFFFF' width='55'");

  return OrderedListDialog;
}

UI_CONTROL *
CreateOrderedListOptionChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *Option;
  UI_CONTROL                    *Icon;
  UI_CONTROL                    *Words;
  UI_CONTROL                    *SortIcon;

  Option = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, Option);
  UiApplyAttributeList (Option, L"bkcolor='0xFFE6E6E6' height='40' name='Option' width='-1'");

  Icon = CreateControl (L"Texture", Option);
  CONTROL_CLASS(Option)->AddChild (Option, Icon);
  UiApplyAttributeList (Icon, L"height='40' name='Icon' width='40'");

  Words = CreateControl (L"Label", Option);
  CONTROL_CLASS(Option)->AddChild (Option, Words);
  UiApplyAttributeList (Words, L"align='center' fontsize='14' name='Words' padding='7,0,7,0' textcolor='0xFF404040'");

  SortIcon = CreateControl (L"Texture", Option);
  CONTROL_CLASS(Option)->AddChild (Option, SortIcon);
  UiApplyAttributeList (SortIcon, L"bkimage='@DialogSortIcon' bkimagestyle='stretch|gray' height='40' name='SortIcon' width='40'");

  return Option;
}

UINT16
GetSizeByHiiValueType (
  IN UINT8                      Type
  )
{
  UINT8                         Width;

  //
  // Size of HII value type is referenced from IfrParse of form browser
  //
  Width = 1;
  switch (Type) {

  case EFI_IFR_TYPE_NUM_SIZE_8:
    Width = 1;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Width = 2;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Width = 4;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    Width = 8;
    break;

  default:
    break;
  }

  return Width;
}

VOID
UiOrderedListOptionOnSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  UI_CONTROL                    *ChildControl;

  if (SetState & UISTATE_SELECTED) {
    ChildControl = UiFindChildByName (Control, L"Option");
    UiApplyAttributeList (ChildControl, L"bkcolor='@menulightcolor' ");
  }

  if (ClearState & UISTATE_SELECTED) {
    ChildControl = UiFindChildByName (Control, L"Option");
    UiApplyAttributeList (ChildControl, L"bkcolor='0xFFE6E6E6' ");
  }
}

EFI_STATUS
UiOrderedListCreateOptions (
  IN UI_CONTROL                 *Layout
  )
{
  UINT8                         *ValueBuffer;
  UINT8                         ContainerCount;
  UINTN                         NumberOfOptions;
  H2O_FORM_BROWSER_O            *OptionArray;
  UINT64                        ContainerHiiValue;
  UINT32                        ContainerIndex;
  UINT32                        OptionIndex;
  H2O_FORM_BROWSER_O            *Option;
  UI_CONTROL                    *CreatedControl;
  UI_CONTROL                    *ChildControl;
  CHAR16                        Str[20];
  BOOLEAN                       Found;

  Found           = FALSE;
  ValueBuffer     = gFB->CurrentQ->HiiValue.Buffer;
  ContainerCount  = gFB->CurrentQ->ContainerCount;
  NumberOfOptions = gFB->CurrentQ->NumberOfOptions;
  OptionArray     = gFB->CurrentQ->Options;

  if (ContainerCount == 0 || NumberOfOptions == 0 || ValueBuffer == NULL || OptionArray == NULL) {
    return EFI_NOT_FOUND;
  }

  for (ContainerIndex = 0; ContainerIndex < ContainerCount; ContainerIndex ++) {
    ContainerHiiValue = GetArrayData (ValueBuffer, OptionArray[0].HiiValue.Type, ContainerIndex);

    for (OptionIndex = 0; OptionIndex < NumberOfOptions; OptionIndex ++) {
      Option = &OptionArray[OptionIndex];
      if (Option->HiiValue.Value.u64 != ContainerHiiValue) {
        continue;
      }

      CreatedControl             = CreateOrderedListOptionChilds (Layout);
      CreatedControl->OnSetState = UiOrderedListOptionOnSetState;
      SetWindowLongPtr (CreatedControl->Wnd, GWLP_USERDATA, (INTN) Option->HiiValue.Value.u64);

      //
      // Get Option button and set its text and bkimage
      //
      ChildControl = UiFindChildByName (CreatedControl, L"Words");
      UiSetAttribute (ChildControl, L"text", Option->Text);

      if (Option->Image != NULL) {
        UnicodeSPrint (Str, sizeof (Str), L"0x%x", Option->Image);
        ChildControl = UiFindChildByName (CreatedControl, L"Icon");
        UiSetAttribute (ChildControl, L"bkimage", Str);
      }

      Found = TRUE;
      break;
    }
  }

  if (!Found) {
    return EFI_NOT_FOUND;
  }

  LIST_VIEW_CLASS (Layout)->SetSelection ((UI_LIST_VIEW *) Layout, (INT32)0, TRUE);
  return EFI_SUCCESS;
}

EFI_STATUS
UiOrderedListGetHiiValue (
  IN  UI_CONTROL                *Layout,
  OUT EFI_HII_VALUE             *HiiValue
  )
{
  UINT8                         HiiValueType;
  UINT32                        Index;
  UI_CONTROL                    *ChildControl;

  if (Layout == NULL || Layout->ItemCount == 0 || HiiValue == NULL) {
    return EFI_ABORTED;
  }

  HiiValueType = gFB->CurrentQ->Options[0].HiiValue.Type;

  ZeroMem (HiiValue, sizeof (EFI_HII_VALUE));
  HiiValue->Type      = EFI_IFR_TYPE_BUFFER;
  HiiValue->BufferLen = (UINT16) (GetSizeByHiiValueType (HiiValueType) * Layout->ItemCount);
  HiiValue->Buffer    = AllocateZeroPool (HiiValue->BufferLen);
  if (HiiValue->Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < Layout->ItemCount; Index++) {
    ChildControl = Layout->Items[Index];
    SetArrayData (HiiValue->Buffer, HiiValueType, Index, (UINT64) GetWindowLongPtr (ChildControl->Wnd, GWLP_USERDATA));
  }

  return EFI_SUCCESS;
}

LRESULT
OrderedListInputProc (
  HWND                          Wnd,
  UINT                          Msg,
  WPARAM                        WParam,
  LPARAM                        LParam
  )
{
  UI_DIALOG                     *Dialog;
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Layout;
  UI_CONTROL                    *ChildControl;
  CHAR16                        Str[20];
  EFI_IMAGE_INPUT               *FormsetImage;
  EFI_HII_VALUE                 HiiValue;
  EFI_STATUS                    Status;
  INT32                         SelectedIndex;
  INT32                         NextIndex;

  Dialog  = (UI_DIALOG *) GetWindowLongPtr (Wnd, 0);
  Control = (UI_CONTROL *) Dialog;

  switch (Msg) {

  case UI_NOTIFY_WINDOWINIT:
    if (mFbDialog->TitleString != NULL) {
      ChildControl = UiFindChildByName (Dialog, L"OrderedListDialogTitle");
      UiSetAttribute (ChildControl, L"text", mFbDialog->TitleString);
    }

    if (gFB->CurrentQ != NULL && gFB->CurrentQ->Help != NULL) {
      ChildControl = UiFindChildByName (Dialog, L"OrderedListDialogText");
      UiSetAttribute (ChildControl, L"text", gFB->CurrentQ->Help);
    }

    FormsetImage = GetCurrentFormSetImage ();
    if (FormsetImage != NULL) {
      ChildControl = UiFindChildByName (Dialog, L"OrderedListDialogImage");
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", FormsetImage);
      UiSetAttribute (ChildControl, L"bkimage", Str);
      UiSetAttribute (ChildControl, L"visible", L"true");
    }

    Layout = UiFindChildByName (Dialog, L"OrderedListList");
    if (Layout->VScrollBar != NULL) {
      UiSetAttribute (Layout->VScrollBar, L"displayscrollbar", L"false");
      CONTROL_CLASS_INVALIDATE (Layout);
    }

    Status = UiOrderedListCreateOptions (Layout);
    if (!EFI_ERROR (Status)) {
      ChildControl = Layout;
    } else {
      ChildControl = UiFindChildByName (Dialog, L"Ok");
    }
    SetFocus (ChildControl->Wnd);

    UnregisterHotKey (Wnd, 4);
    UnregisterHotKey (Wnd, 5);
    UnregisterHotKey (Wnd, 8);
    UnregisterHotKey (Wnd, 9);
    MwRegisterHotKey (Wnd, 4,  0, VK_F5);
    MwRegisterHotKey (Wnd, 5,  0, VK_F6);
    MwRegisterHotKey (Wnd, 8,  0, VK_OEM_PLUS);
    MwRegisterHotKey (Wnd, 9,  0, VK_OEM_MINUS);
    break;

  case UI_NOTIFY_CARRIAGE_RETURN:
  case UI_NOTIFY_CLICK:
    ChildControl = (UI_CONTROL *) WParam;
    if (StrCmp (ChildControl->Name, L"OrderedListList") == 0) {
      ChildControl = UiFindChildByName (Dialog, L"Ok");
      SetFocus (ChildControl->Wnd);
    } else if (StrCmp (ChildControl->Name, L"Cancel") == 0) {
      SendShutDNotify ();
    } else if (StrCmp (ChildControl->Name, L"Ok") == 0) {
      Layout = UiFindChildByName (Dialog, L"OrderedListList");
      UiOrderedListGetHiiValue (Layout, &HiiValue);
      SendChangeQNotify (0, 0, &HiiValue);
    }
    break;

  case WM_HOTKEY:
    switch (HIWORD(LParam)) {

    case VK_ESCAPE:
      SendShutDNotify ();
      return 0;

    case VK_F5:
    case VK_F6:
    case VK_OEM_PLUS:
    case VK_OEM_MINUS:
      Layout        = UiFindChildByName (Dialog, L"OrderedListList");
      SelectedIndex = LIST_VIEW_CLASS (Layout)->GetSelection ((UI_LIST_VIEW *) Layout);
      NextIndex     = LIST_VIEW_CLASS (Layout)->FindNextSelection (
                                                  (UI_LIST_VIEW *) Layout,
                                                  SelectedIndex,
                                                  (HIWORD(LParam) == VK_F6 || HIWORD(LParam) == VK_OEM_PLUS) ? VK_UP : VK_DOWN
                                                  );
      if (SelectedIndex != -1 && NextIndex != -1) {
        LIST_VIEW_CLASS (Layout)->SwitchItems ((UI_LIST_VIEW *) Layout, SelectedIndex, NextIndex);
        LIST_VIEW_CLASS (Layout)->SetSelection ((UI_LIST_VIEW *) Layout, NextIndex, TRUE);
      }
      return 0;

    default:
      break;
    }
    return 1;

  case WM_DESTROY:
    UnregisterHotKey (Wnd, 4);
    UnregisterHotKey (Wnd, 5);
    UnregisterHotKey (Wnd, 8);
    UnregisterHotKey (Wnd, 9);
    return 0;

  default:
    return 0;
  }

  return 1;
}

