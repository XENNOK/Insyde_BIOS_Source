/** @file
  Graphics Dialog

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
#include "Ui.h"
#include "SetupUtility.h"
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"
#include <Protocol/H2ODialog.h>
#include "GraphicsUi.h"

CHAR16 *mUiDialogPopupXml = NULL;
CHAR16 *mUiDialogIconXml = NULL;
CHAR16 *mUiShowPageInfoXml = NULL;
CHAR16 *mSelectionDialogXml = NULL;
CHAR16 *mEnterOldPasswordXml = NULL;
CHAR16 *mEnterNewPasswordXml = NULL;
CHAR16 *mDatePickerDialogXml = NULL;
CHAR16 *mTimePickerDialogXml = NULL;

typedef struct {
  CHAR16 *ShowString;
  CHAR16 *Title;
  UINTN  DlgOpreator;
  UINTN  Select;

}  UI_DIALOG_POPUP_DATA;

typedef struct {
  UINTN  MaximumStringSize;
  CHAR16 *ShowString;
  CHAR16 *StringBuffer;
  UINTN  Select;
} UI_DIALOG_ICON_DATA;


typedef struct {
  CHAR16     *Title;
  UI_CONTROL *NewPasswordWnd;
  UI_CONTROL *ConfirmPasswordWnd;
  UI_CONTROL *OldPasswordWnd;

  CHAR16     *NewPasswordStr;
  CHAR16     *OldPasswordStr;

  UINTN      PasswordMaximum;

  EFI_STATUS Status;

} UI_DIALOG_READ_PASSWORD_DATA;

typedef struct {
  CHAR16       *Title;
  EFI_HII_DATE Date;
  EFI_STATUS   Status;

  UI_CONTROL   *MonthUp;
  UI_CONTROL   *DayUp;
  UI_CONTROL   *YearUp;

  UI_CONTROL   *MonthDown;
  UI_CONTROL   *DayDown;
  UI_CONTROL   *YearDown;

  UI_CONTROL   *Month;
  UI_CONTROL   *Day;
  UI_CONTROL   *Year;

  UI_CONTROL   *Ok;
  UI_CONTROL   *Cancel;

} UI_DIALOG_DATE_PICKER_DATA;

typedef struct {
  CHAR16       *Title;
  EFI_HII_TIME Time;
  EFI_STATUS   Status;

  UI_CONTROL   *HourUp;
  UI_CONTROL   *MinuteUp;
  UI_CONTROL   *SecondUp;

  UI_CONTROL   *HourDown;
  UI_CONTROL   *MinuteDown;
  UI_CONTROL   *SecondDown;

  UI_CONTROL   *Hour;
  UI_CONTROL   *Minute;
  UI_CONTROL   *Second;

  UI_CONTROL   *Ok;
  UI_CONTROL   *Cancel;

} UI_DIALOG_TIME_PICKER_DATA;


VOID
InitializeGraphicsDialogXML (
  VOID
  )
{
  mUiDialogPopupXml    = GetToken (STRING_TOKEN (DIALOG_POP_UP), gHiiHandle);
  mUiDialogIconXml     = GetToken (STRING_TOKEN (DIALOG_ICON), gHiiHandle);
  mUiShowPageInfoXml   = GetToken (STRING_TOKEN (SHOW_PAGE_INFO), gHiiHandle);
  mSelectionDialogXml  = GetToken (STRING_TOKEN (SELECTION_DIALOG), gHiiHandle);
  mEnterOldPasswordXml = GetToken (STRING_TOKEN (ENTER_OLD_PASSWORD_DIALOG), gHiiHandle);
  mEnterNewPasswordXml = GetToken (STRING_TOKEN (ENTER_NEW_PASSWORD_DIALOG), gHiiHandle);
  mDatePickerDialogXml = GetToken (STRING_TOKEN (DATE_PICK_DIALOG), gHiiHandle);
  mTimePickerDialogXml = GetToken (STRING_TOKEN (TIME_PICK_DIALOG), gHiiHandle);

  ASSERT (mUiDialogPopupXml != NULL);
  ASSERT (mUiDialogIconXml != NULL);
  ASSERT (mUiShowPageInfoXml != NULL);
  ASSERT (mSelectionDialogXml != NULL);
  ASSERT (mEnterOldPasswordXml != NULL);
  ASSERT (mEnterNewPasswordXml != NULL);
  ASSERT (mDatePickerDialogXml != NULL);
  ASSERT (mTimePickerDialogXml != NULL);
}

INTN
DialogPopupProc (
  HWND         Hwnd,
  UINT         message,
  WPARAM       WParam,
  LPARAM       LParam
  )
{
  UI_DIALOG_POPUP_DATA *Private;
  UI_DIALOG            *Dialog;
  CHAR16               *OkString, *CancelString;
  CHAR16               *YesString, *NoString;
  UI_CONTAINER         *Container;
  UI_MANAGER           *Manager;
  UI_CONTROL           *ShowString;
  UINTN                Index;
  UI_CONTROL           *Option;
  UI_CONTROL           *Control;
  UI_CONTROL           *Sender;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Hwnd, 0);

  Manager = NULL;
  Private = NULL;

  if (Dialog != NULL)
  {
      Manager = ((UI_CONTROL *)Dialog)->Manager;
      Private = (UI_DIALOG_POPUP_DATA *) Dialog->Param;
  }

  if (message == WM_HOTKEY) {
    if (HIWORD (LParam) == VK_ESCAPE) {
      Private->Select = 0;
      UiEndDialog (Manager->MainWnd, 0);
      return 1;
    }
  }

  if (message == UI_NOTIFY_WINDOWINIT) {

    Private->Select = 0;
    OkString        = GetToken (STRING_TOKEN(SCU_STR_OK_TEXT), gHiiHandle);
    CancelString    = GetToken (STRING_TOKEN(SCU_STR_CANCEL_TEXT), gHiiHandle);
    YesString       = GetToken (STRING_TOKEN(SCU_STR_YES_TEXT), gHiiHandle);
    NoString        = GetToken (STRING_TOKEN(SCU_STR_NO_TEXT), gHiiHandle);

    ShowString = Manager->FindControlByName (Manager, L"showstring");
    ShowString->SetAttribute (ShowString, L"text", Private->ShowString);

    Control = Manager->FindControlByName (Manager, L"title");
    if (Control != NULL && Private->Title != NULL) {
      Control->SetAttribute (Control, L"text", Private->Title);
    }

    Container = (UI_CONTAINER *) Manager->FindControlByName (Manager, L"content");

    switch (Private->DlgOpreator)
    {

    case DlgYesNo:
      ContainerCreateControl (Container, L"Control", NULL);
      Option = ContainerCreateControl (Container, L"Button", YesString);
      Option->SetAttribute (Option, L"width", L"60");
      Option->SetAttribute (Option, L"align", L"center");
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 1);

      Control = ContainerCreateControl (Container, L"Control", NULL);
      Control->SetAttribute (Control, L"width", L"20");
      Option = ContainerCreateControl (Container, L"Button", NoString);
      Option->SetAttribute (Option, L"width", L"60");
      Option->SetAttribute (Option, L"align", L"center");
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 2);
//      ContainerCreateControl (Container, L"Control", NULL);
      break;

    case DlgYesNoCancel:
      Option = ContainerCreateControl (Container, L"Button", YesString);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 1);
      Option = ContainerCreateControl (Container, L"Button", NoString);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 2);
      Option = ContainerCreateControl (Container, L"Button", CancelString);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 3);
      break;

    case DlgOk:
      Option = ContainerCreateControl (Container, L"Button", OkString);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 1);
      break;

    case DlgOkCancel:
      Option = ContainerCreateControl (Container, L"Button", OkString);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 1);
      Option = ContainerCreateControl (Container, L"Button", CancelString);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 2);
      break;

    default:
      break;
    }

    ScuSafeFreePool ((VOID **)&OkString);
    ScuSafeFreePool ((VOID **)&CancelString);
    ScuSafeFreePool ((VOID **)&YesString);
    ScuSafeFreePool ((VOID **)&NoString);

    return 1;
  }

  if (message == UI_NOTIFY_CLICK) {
    Sender = (UI_CONTROL *)WParam;
    Container = (UI_CONTAINER *) Manager->FindControlByName (Manager, L"content");
    for (Index = 0; Index < Container->ItemCount; Index++)
    {
      if (Sender == Container->Items[Index])
      {
        Private->Select = GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
        UiEndDialog (Manager->MainWnd, 0);
        break;
      }
    }
    return 1;
  }

  return 0;
}

EFI_STATUS
UiConfirmDialog (
  IN  UINT32                          DialogOperator,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  )
{
  UI_DIALOG_POPUP_DATA Private;
  EFI_STATUS           Status;

  Private.DlgOpreator = (UINTN)DialogOperator;
  Private.ShowString  = String;

  if (!mAlreadySendForm) {
    Status = InitializeGUI ();
    ASSERT_EFI_ERROR (Status);

    GdOpenMouse ();
    InitializeBrowserStrings ();
  }

  UiDialogBoxParam (
    NULL,
    mUiDialogPopupXml,
    NULL,
    DialogPopupProc,
    (LPARAM)&Private,
    0,
    GetSystemMetrics (SM_CYSCREEN) / 3,
    GetSystemMetrics (SM_CXSCREEN),
    GetSystemMetrics (SM_CYSCREEN) / 3,
    FALSE
  );

  if (Private.Select == 1)
  {
    KeyValue->UnicodeChar = CHAR_CARRIAGE_RETURN;
  }
  else
  {
    KeyValue->UnicodeChar = CHAR_NULL;
    KeyValue->ScanCode = SCAN_ESC;
  }

  if (!mAlreadySendForm) {
    GdCloseMouse ();
  }

  return EFI_SUCCESS;
}



INTN
DialogIconProc (
  HWND         Hwnd,
  UINT         message,
  WPARAM       WParam,
  LPARAM       LParam
  )
{
  UI_DIALOG_ICON_DATA  *Private;
  UI_DIALOG            *Dialog;
  CHAR16               *OkString, *CancelString;
  UI_CONTAINER         *Container;
  UI_MANAGER           *Manager;
  UI_CONTROL           *ShowString;
  UINTN                Index;
  UI_CONTROL           *Option;
  UI_CONTROL           *Input;
  UI_CONTROL           *Sender;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Hwnd, 0);

  Manager = NULL;
  Private = NULL;

  if (Dialog != NULL)
  {
      Manager = ((UI_CONTROL *)Dialog)->Manager;
      Private = (UI_DIALOG_ICON_DATA *) Dialog->Param;
  }

  if (message == WM_HOTKEY) {
    if (HIWORD (LParam) == VK_ESCAPE) {
      Private->Select = 0;
      UiEndDialog (Manager->MainWnd, 0);
      return 1;
    }
  }

  if (message ==  UI_NOTIFY_WINDOWINIT) {
    OkString        = GetToken (STRING_TOKEN(SCU_STR_OK_TEXT), gHiiHandle);
    CancelString    = GetToken (STRING_TOKEN(SCU_STR_CANCEL_TEXT), gHiiHandle);

    ShowString = Manager->FindControlByName (Manager, L"showstring");
    ShowString->SetAttribute (ShowString, L"text", Private->ShowString);

    Container = (UI_CONTAINER *) Manager->FindControlByName (Manager, L"content");
    Option = ContainerCreateControl (Container, L"Button", OkString);
    SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 1);
    Option = ContainerCreateControl (Container, L"Button", CancelString);
    SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, 2);

    ScuSafeFreePool ((VOID **)&OkString);
    ScuSafeFreePool ((VOID **)&CancelString);

    return 1;
  }

  if (message ==  UI_NOTIFY_CLICK) {
    Sender = (UI_CONTROL*) WParam;
    Container = (UI_CONTAINER *) Manager->FindControlByName (Manager, L"content");
    for (Index = 0; Index < Container->ItemCount; Index++)
    {
      if (Sender == Container->Items[Index])
      {
        Private->Select = GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
        Input = Manager->FindControlByName (Manager, L"input");
        GetWindowText (Input->Wnd, Private->StringBuffer, (INT32)Private->MaximumStringSize);
        UiEndDialog (Manager->MainWnd, 0);
        break;
      }
    }

    return 1;
  }

  return 0;
}


EFI_STATUS
UiPasswordDialog (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  )
{
  UI_DIALOG_ICON_DATA          Private;
  EFI_STATUS                   Status;

  if (!mAlreadySendForm) {
    Status = InitializeGUI ();
    ASSERT_EFI_ERROR (Status);

    GdOpenMouse ();
    InitializeBrowserStrings ();
  }


  Private.StringBuffer      = StringBuffer;
  Private.MaximumStringSize = MaximumStringSize;
  Private.ShowString        = String;

  UiDialogBoxParam (
    NULL,
    mUiDialogIconXml,
    NULL,
    DialogIconProc,
    (LPARAM)&Private,
    0,
    GetSystemMetrics (SM_CYSCREEN) / 3,
    GetSystemMetrics (SM_CXSCREEN),
    GetSystemMetrics (SM_CYSCREEN) / 3,
    FALSE
  );

  if (Private.Select == 1)
  {
    KeyValue->UnicodeChar = CHAR_CARRIAGE_RETURN;
  }
  else
  {
    ZeroMem (StringBuffer, MaximumStringSize * sizeof (CHAR16));
    KeyValue->UnicodeChar = CHAR_NULL;
    KeyValue->ScanCode = SCAN_ESC;
  }

  if (!mAlreadySendForm) {
    GdCloseMouse ();
  }
  return EFI_SUCCESS;
}

EFI_STATUS
UiCreateMsgPopUp (
  IN  UINT32                      RequestedWidth,
  IN  UINT32                      NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  )
{
  UI_DIALOG_POPUP_DATA Private;
  CHAR16               *StringBuf;
  CHAR16               **StringList;
  UINT32               Index;
  EFI_STATUS           Status;

  if (!mAlreadySendForm) {
    Status = InitializeGUI ();
    ASSERT_EFI_ERROR (Status);

    GdOpenMouse ();
    InitializeBrowserStrings ();
  }


  Private.DlgOpreator = DlgOkCancel + 1; // no operator button

  StringBuf = AllocateZeroPool (0x1000);
  StringList = &ArrayOfStrings;
  for (Index = 0; Index < NumberOfLines; Index++) {
    StrCat (StringBuf, StringList[Index]);
    StrCat (StringBuf, L"\n");
  }
  Private.ShowString = StringBuf;

  UiDialogBoxParam (
    NULL,
    mUiDialogPopupXml,
    NULL,
    DialogPopupProc,
    (LPARAM)&Private,
    0,
    GetSystemMetrics (SM_CYSCREEN) / 3,
    GetSystemMetrics (SM_CXSCREEN),
    GetSystemMetrics (SM_CYSCREEN) / 3,
    FALSE
  );

  gBS->FreePool (StringBuf);

  //
  // Bug Bug: Doesn't go here
  //
  ASSERT (FALSE);

  if (!mAlreadySendForm) {
    GdCloseMouse ();
  }
  return EFI_SUCCESS;
}

EFI_STATUS
UiShowPageInfo (
  IN   CHAR16          *TitleString,
  IN   CHAR16          *InfoStrings
  )
{
  UI_DIALOG_POPUP_DATA Private;
  EFI_STATUS           Status;

  Private.DlgOpreator = DlgOk; // no operator button

  Private.ShowString = InfoStrings;
  Private.Title      = TitleString;

  if (!mAlreadySendForm) {
    Status = InitializeGUI ();
    ASSERT_EFI_ERROR (Status);

    GdOpenMouse ();
    InitializeBrowserStrings ();
  }


  UiDialogBoxParam (
    NULL,
    mUiShowPageInfoXml,
    NULL,
    DialogPopupProc,
    (LPARAM)&Private,
    0,
    0,
    GetSystemMetrics (SM_CXSCREEN),
    GetSystemMetrics (SM_CYSCREEN),
    FALSE
  );

  if (!mAlreadySendForm) {
    GdCloseMouse ();
  }

  return EFI_SUCCESS;
}


LRESULT
SelectionInputProc (
  HWND    Hwnd,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam
  )
{
  UI_CONTROL              *Sender;
  UI_CONTROL              *Layout;
  UI_CONTROL              *Option;
  UI_DIALOG               *Dialog;
  UI_MANAGER              *Manager;
  COMBOBOX_INFO           *ComboBoxInfo;
  UI_CONTROL              *ComboBox;
  UI_CONTROL              *Control;
  RECT                    Rc;
  UINTN                   Height;
  UINTN                   Count;
  COMBOBOX_ITEM           *ComboBoxItem;
  LIST_ENTRY              *Link;
  RECT                    ParentRc;

  Dialog         = (UI_DIALOG *) GetWindowLongPtr (Hwnd, 0);
  Manager       = NULL;
  ComboBox      = NULL;
  Control       = NULL;
  ComboBoxInfo  = NULL;

  ASSERT (Dialog != NULL);
  ASSERT (Dialog->Param != 0);

  ComboBoxInfo  = (COMBOBOX_INFO *)  Dialog->Param;
  Manager       = (UI_MANAGER *)((UI_CONTROL *)Dialog)->Manager;
  ComboBox      = ComboBoxInfo->Control;

  switch (message) {

  case UI_NOTIFY_WINDOWINIT:
    Layout = Manager->FindControlByName (Manager, L"list");
    ASSERT (Layout != NULL);

    //
    // add item list
    //
    Count = 0;
    Link  = GetFirstNode (&ComboBoxInfo->ItemList);
    while (!IsNull (&ComboBoxInfo->ItemList, Link)) {
      ComboBoxItem = COMBOBOX_ITEM_FROM_LINK (Link);

      Option = CreateControl (L"Button", ComboBoxItem->Str, Layout, Manager);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, (INTN)ComboBoxItem);
      Option->SetAttribute (Option, L"height", L"30");
      Option->SetAttribute (Option, L"name",  L"menuoption");
      Option->SetAttribute (Option, L"bkcolor", L"0xFF123456");
      Option->SetAttribute (Option, L"textcolor", L"0xFF000000");
      ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Option);

      Link = GetNextNode (&ComboBoxInfo->ItemList, Link);
      Count++;
    }

    if (Count == 0) {
      Height = ((UI_CONTAINER *)Layout)->ChildPadding;
    } else {
      Height = Count * 30 + (Count - 1) * ((UI_CONTAINER *)Layout)->ChildPadding;
    }
    //
    // adjust pos
    //
    if (ComboBox != NULL) {
      //
      // Dropdown mode
      //
      GetWindowRect (ComboBox->Wnd, &Rc);
      GetWindowRect (Manager->Root->parent, &ParentRc);
      if ((UINTN)ParentRc.bottom >= ((UINTN)Rc.bottom + Height)) {
        MoveWindow (Manager->MainWnd, Rc.left, Rc.bottom, Rc.right - Rc.left, (INT32)Height, TRUE);
      } else {
        MoveWindow (Manager->MainWnd, Rc.left, (Rc.top - (MWCOORD)Height), Rc.right - Rc.left, (INT32)Height, TRUE);
      }
    } else {
      //
      // Dialog mode
      //

    }
    break;

  case UI_NOTIFY_CLICK:
    Sender = (UI_CONTROL *)wParam;
    ComboBoxInfo->SelectItem  = (COMBOBOX_ITEM *) GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
    UiEndDialog (Hwnd, 1);
    break;

  case WM_CLOSE:
    ComboBoxInfo->SelectItem = NULL;
    UiEndDialog (Hwnd, 0);
    break;

  case WM_HOTKEY:
    if (HIWORD (lParam) == VK_ESCAPE) {
      SendMessage (Hwnd, WM_CLOSE, 0, 0);
    }
    break;

  default:
    return 0;
  }

  return 1;
}




EFI_STATUS
UiGetSelectionInputPopUp (
  IN  UI_CONTROL                  *SpinnerControl,
  IN  HWND                        Parent,
  IN  UI_MENU_SELECTION           *Selection,
  FORM_BROWSER_STATEMENT          *Statement
  )
{
  COMBOBOX_INFO           Info;
  RECT                    Rc;
  INTN                    Result;
  QUESTION_OPTION         *OneOfOption;
  LIST_ENTRY              *Link;
  BOOLEAN                 IsOrderedList;
  COMBOBOX_ITEM           *ComboBoxItem;
  EFI_STATUS              Status;
  EFI_STATUS              ReturnStatus;

  ZeroMem (&Info, sizeof (COMBOBOX_INFO));
  Info.Control     = SpinnerControl;
  Info.NumOfItems  = 0;
  InitializeListHead (&Info.ItemList);

  IsOrderedList   = FALSE;

  //
  // List all options
  //
  Link = GetFirstNode (&Statement->OptionListHead);
  while (!IsNull (&Statement->OptionListHead, Link)) {
    OneOfOption = QUESTION_OPTION_FROM_LINK (Link);
    Status = CompareHiiValue (&Statement->HiiValue, &OneOfOption->Value, &Result, NULL);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (!IsOrderedList &&  Result == 0) {
      Link = GetNextNode (&Statement->OptionListHead, Link);
      continue;
    }
    if ((OneOfOption->SuppressExpression == NULL) || !OneOfOption->SuppressExpression->Result.Value.b) {
      ComboBoxItem = AllocateZeroPool (sizeof (COMBOBOX_ITEM));
      ComboBoxItem->Str  = GetToken (OneOfOption->Text, Selection->Handle);
      ComboBoxItem->Data = (VOID *)OneOfOption;
      InsertTailList (&Info.ItemList, &ComboBoxItem->Link);
      Info.NumOfItems++;
    }

    Link = GetNextNode (&Statement->OptionListHead, Link);
  }

  GetWindowRect (SpinnerControl->Wnd, &Rc);
  Result = UiDialogBoxParam (
             NULL,
             mSelectionDialogXml,
             Parent,
             SelectionInputProc,
             (LPARAM)&Info, Rc.left, Rc.bottom, Rc.right - Rc.left, 0,
             TRUE
             );

  ReturnStatus = EFI_NOT_READY;
  if (Result) {
    if (Info.SelectItem != NULL) {
      OneOfOption = (QUESTION_OPTION *)Info.SelectItem->Data;

      CopyMem (&Statement->HiiValue, &OneOfOption->Value, sizeof (EFI_HII_VALUE));
      Status = ValidateQuestion (Selection->FormSet, Selection->Form, Statement, EFI_HII_EXPRESSION_INCONSISTENT_IF);
      if (EFI_ERROR (Status)) {
        //
        // Input value is not valid, restore Question Value
        //
        GetQuestionValue (Selection->FormSet, Selection->Form, Statement, TRUE);
      } else {
        SetQuestionValue (Selection->FormSet, Selection->Form, Statement, TRUE);
        UpdateStatusBar (NV_UPDATE_REQUIRED, Statement->QuestionFlags, TRUE);
        ReturnStatus = EFI_SUCCESS;
      }
    }
  }

  //
  // free item list
  //
  while (!IsListEmpty (&Info.ItemList)) {
    ComboBoxItem = COMBOBOX_ITEM_FROM_LINK (Info.ItemList.ForwardLink);
    RemoveEntryList (&ComboBoxItem->Link);
    gBS->FreePool (ComboBoxItem->Str);
    gBS->FreePool (ComboBoxItem);
  }

  return ReturnStatus;
}


INTN
ReadPasswordProc (
  HWND         Hwnd,
  UINT         message,
  WPARAM       WParam,
  LPARAM       LParam
  )
{
  UI_DIALOG                    *Dialog;
  UI_DIALOG_READ_PASSWORD_DATA *Private;
  UI_MANAGER                   *Manager;
  UI_CONTROL                   *Sender;
  UI_CONTROL                   *Control;
  CHAR16                       ConfirmPassword[100];
  H2O_DIALOG_PROTOCOL          *H2ODialog;
  EFI_INPUT_KEY                  KeyValue;
  CHAR16                         *StrBuf;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Hwnd, 0);

  Manager = NULL;
  Private = NULL;

  if (Dialog != NULL)
  {
    Manager = ((UI_CONTROL *)Dialog)->Manager;
    Private = (UI_DIALOG_READ_PASSWORD_DATA *) Dialog->Param;
  }

  if (message == WM_HOTKEY) {
    if (HIWORD (LParam) == VK_ESCAPE) {
      UiEndDialog (Manager->MainWnd, 0);
      return 1;
    }
  }

  if (message == UI_NOTIFY_WINDOWINIT) {
    Control = Manager->FindControlByName (Manager, L"title");
    Control->SetAttribute (Control, L"text", Private->Title);

    Control = Manager->FindControlByName (Manager, L"oldpasswordtext");
    if (Control != NULL) {
      Control->SetAttribute (Control, L"text", gPromptForPassword);
    }

    Control = Manager->FindControlByName (Manager, L"newpasswordtext");
    Control->SetAttribute (Control, L"text", gPromptForNewPassword);


    Control = Manager->FindControlByName (Manager, L"confirmpasswordtext");
    Control->SetAttribute (Control, L"text", gConfirmPassword);

    return 1;
  }

  if (message == UI_NOTIFY_CLICK) {

    Sender = (UI_CONTROL*)WParam;

    if (StrCmp (Sender->Name, L"ok") == 0) {
      Control = Manager->FindControlByName (Manager, L"newpassword");
      GetWindowText (Control->Wnd, Private->NewPasswordStr, (INT32)Private->PasswordMaximum);

      Control = Manager->FindControlByName (Manager, L"confirmpassword");
      GetWindowText (Control->Wnd, ConfirmPassword, (INT32)Private->PasswordMaximum);

      if (StrCmp (Private->NewPasswordStr, ConfirmPassword) != 0) {

        gBS->LocateProtocol (
               &gH2ODialogProtocolGuid,
               NULL,
               (VOID **) &H2ODialog
               );

        StrBuf = AllocateZeroPool (0x1000);
        StrCat (StrBuf, gSetupWarning);
        StrCat (StrBuf, L"\n");
        StrCat (StrBuf, gConfirmError);
        StrCat (StrBuf, L"\n");

        H2ODialog->ConfirmDialog (
                           DlgOk,
                           FALSE,
                           0,
                           NULL,
                           &KeyValue,
                           StrBuf
                           );

        gBS->FreePool (StrBuf);
        return 1;
      }

      Control = Manager->FindControlByName (Manager, L"oldpassword");
      if (Control != NULL) {
        GetWindowText (Control->Wnd, Private->OldPasswordStr, (INT32)Private->PasswordMaximum);
      }

      Private->Status = EFI_SUCCESS;
      UiEndDialog (Manager->MainWnd, 0);


    } else if (StrCmp (Sender->Name, L"cancel") == 0) {


      UiEndDialog (Manager->MainWnd, 0);
    }

    return 1;
  }

  return 0;
}

/**
 Get password input from user.

 @param [in]   Selection
 @param [in]   Statement
 @param [in]   PassWordState
 @param [in]   CreatePopUp
 @param [out]  OldPassword
 @param [out]  NewPassword

 @retval EFI_SUCCESS            If string input is read successfully
 @retval EFI_DEVICE_ERROR       If operation fails

**/
EFI_STATUS
UiReadPassword (
  IN  UI_MENU_SELECTION           *Selection,
  IN  FORM_BROWSER_STATEMENT      *Statement,
  IN  UINTN                       PassWordState,
  IN  BOOLEAN                     CreatePopUp,
  OUT CHAR16                      *OldPassword,
  OUT CHAR16                      *NewPassword
  )
{
  UINTN                          Minimum;
  UINTN                          Maximum;
  FORM_BROWSER_STATEMENT         *Question;
  CHAR16                         *PasswordKind;
  UI_DIALOG_READ_PASSWORD_DATA   Private;


  ZeroMem (&Private, sizeof (Private));

  Question = Statement;
  Minimum  = (UINTN) Question->Minimum;
  Maximum  = (UINTN) Question->Maximum;
  PasswordKind = GetToken (Statement->Prompt, Selection->Handle);

  Private.Title          = PasswordKind;
  Private.OldPasswordStr = OldPassword;
  Private.NewPasswordStr = NewPassword;
  Private.Status         = EFI_NOT_READY;
  Private.PasswordMaximum = Maximum;
  switch (PassWordState) {

  case ENTER_NEW_PASSWORD:
    UiDialogBoxParam (
      NULL,
      mEnterNewPasswordXml,
      NULL,
      ReadPasswordProc,
      (LPARAM) &Private,
      0,
      GetSystemMetrics (SM_CYSCREEN) / 3,
      GetSystemMetrics (SM_CXSCREEN),
      GetSystemMetrics (SM_CYSCREEN) / 3,
      FALSE
      );
    break;

  case ENTER_OLD_PASSWORD:
    UiDialogBoxParam (
      NULL,
      mEnterOldPasswordXml,
      NULL,
      ReadPasswordProc,
      (LPARAM) &Private,
      0,
      GetSystemMetrics (SM_CYSCREEN) / 3,
      GetSystemMetrics (SM_CXSCREEN),
      GetSystemMetrics (SM_CYSCREEN) / 3,
      FALSE
      );
    break;

  default:
    ASSERT (FALSE);

  }

  ScuSafeFreePool ((VOID**)&PasswordKind);

  return Private.Status;

}

BOOLEAN
IsLeapYear (
  IN UINT16 Year
  );


INTN
DatePickerDialogProc (
  HWND         Hwnd,
  UINT         message,
  WPARAM       WParam,
  LPARAM       LParam
  )
{
  UI_DIALOG                    *Dialog;
  UI_DIALOG_DATE_PICKER_DATA   *Private;
  UI_MANAGER                   *Manager;
  UI_CONTROL                   *Sender;
  EFI_STATUS                   Status;
  CHAR16                       Str[10];
  UI_CONTROL                   *Control;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Hwnd, 0);

  Manager = NULL;
  Private = NULL;

  if (Dialog != NULL)
  {
    Manager = ((UI_CONTROL *)Dialog)->Manager;
    Private = (UI_DIALOG_DATE_PICKER_DATA *) Dialog->Param;
  }

  if (message == WM_HOTKEY) {
    if (HIWORD (LParam) == VK_ESCAPE) {
      UiEndDialog (Manager->MainWnd, 0);
      return 1;
    }
  }

  if (message == UI_NOTIFY_WINDOWINIT) {
    Control = Manager->FindControlByName (Manager, L"title");
    if (Control != NULL) {
      Control->SetAttribute (Control, L"text", Private->Title);
    }

    Private->MonthUp   = Manager->FindControlByName (Manager, L"month_up");
    Private->MonthDown = Manager->FindControlByName (Manager, L"month_down");
    Private->DayUp     = Manager->FindControlByName (Manager, L"day_up");
    Private->DayDown   = Manager->FindControlByName (Manager, L"day_down");
    Private->YearUp    = Manager->FindControlByName (Manager, L"year_up");
    Private->YearDown  = Manager->FindControlByName (Manager, L"year_down");

    Private->Month     = Manager->FindControlByName (Manager, L"month");
    Private->Day       = Manager->FindControlByName (Manager, L"day");
    Private->Year      = Manager->FindControlByName (Manager, L"year");

    Private->Ok        = Manager->FindControlByName (Manager, L"ok");
    Private->Cancel    = Manager->FindControlByName (Manager, L"cancel");

    UnicodeValueToString (Str, 0, (INT64)Private->Date.Month, 0);
    Private->Month->SetAttribute (Private->Month, L"text", Str);

    UnicodeValueToString (Str, 0, (INT64)Private->Date.Day, 0);
    Private->Day->SetAttribute (Private->Day, L"text", Str);

    UnicodeValueToString (Str, 0, (INT64)Private->Date.Year, 0);
    Private->Year->SetAttribute (Private->Year, L"text", Str);
    return 1;
  }


  if (message == UI_NOTIFY_CLICK) {

    INT64                           Minimum;
    INT64                           Maximum;
    INT64                           EditValue;
    INTN                            Step;
    INTN                            DayOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    UI_CONTROL                      *EditControl;

    Sender  = (UI_CONTROL*)WParam;
    Minimum = 0;
    Maximum = 0;
    Step    = 0;
    EditControl = NULL;

    if (Sender == Private->MonthUp || Sender == Private->MonthDown) {

      Minimum = 1;
      Maximum = 12;
      Step = (Sender == Private->MonthUp) ? 1 : -1;
      EditControl = Private->Month;

    } else if (Sender == Private->DayUp || Sender == Private->DayDown) {

      Minimum = 1;
      if ((Private->Date.Month==2) && IsLeapYear(Private->Date.Year)) {
        Maximum = 29;
      } else {
        Maximum = DayOfMonth[Private->Date.Month - 1];
      }
      Step = (Sender == Private->DayUp) ? 1 : -1;
      EditControl = Private->Day;

    } else if (Sender == Private->YearUp || Sender == Private->YearDown) {

      Maximum = 2099;
      Minimum = 2000;
      Step = (Sender == Private->YearUp) ? 1 : -1;
      EditControl = Private->Year;

    } else if (Sender == Private->Ok) {

      Private->Status = EFI_SUCCESS;
      UiEndDialog (Manager->MainWnd, 0);
      return 1;

    } else if (Sender == Private->Cancel) {

      Private->Status = EFI_NOT_READY;
      UiEndDialog (Manager->MainWnd, 0);
      return 1;

    } else {
      //
      // Don't run next code
      //
      return 0;
    }

    ASSERT (EditControl != NULL);
    EditValue = (INT64) StrToUInt (EditControl->Wnd->szTitle, 10, &Status);
    if ((EditValue + Step) > Maximum) {
      EditValue = Minimum;
    } else if ((EditValue + Step) < Minimum) {
      EditValue = Maximum;
    } else {
      EditValue += Step;
    }

    UnicodeValueToString (Str, 0, (INT64)EditValue, 0);
    EditControl->SetAttribute (EditControl, L"text", Str);

    //
    // write back date
    //
    Private->Date.Month = (UINT8) StrToUInt (Private->Month->Wnd->szTitle, 10, &Status);
    Private->Date.Day   = (UINT8) StrToUInt (Private->Day->Wnd->szTitle, 10, &Status);
    Private->Date.Year  = (UINT16) StrToUInt (Private->Year->Wnd->szTitle, 10, &Status);

    //
    // Fix month
    //
    EditValue = (INT64) StrToUInt (EditControl->Wnd->szTitle, 10, &Status);
    if ((Private->Date.Month==2) && IsLeapYear(Private->Date.Year)) {
      Maximum = 29;
    } else {
      Maximum = DayOfMonth[Private->Date.Month - 1];
    }
    if (Private->Date.Day > Maximum) {
      Private->Date.Day = (UINT8) Maximum;
    }
    UnicodeValueToString (Str, 0, (INT64)Private->Date.Day, 0);
    Private->Day->SetAttribute (Private->Day, L"text", Str);
  }
  return 0;

}


/**
 Get password input from user.

 @param        Title
 @param        Date

 @retval EFI_SUCCESS            If string input is read successfully
 @retval EFI_DEVICE_ERROR       If operation fails

**/
EFI_STATUS
UiDatePickerDialog (
  CHAR16       *Title,
  EFI_HII_DATE *Date
  )
{

  UI_DIALOG_DATE_PICKER_DATA Private;

  Private.Title   = Title;
  Private.Date    = *Date;
  Private.Status  = EFI_NOT_READY;

  UiDialogBoxParam (
      NULL,
      mDatePickerDialogXml,
      NULL,
      DatePickerDialogProc,
      (LPARAM) &Private,
      0,
      GetSystemMetrics (SM_CYSCREEN) / 3,
      GetSystemMetrics (SM_CXSCREEN),
      GetSystemMetrics (SM_CYSCREEN) / 3,
      FALSE
      );

  if (!EFI_ERROR (Private.Status)) {
    *Date = Private.Date;
  }

  return Private.Status;
}

INTN
TimePickerDialogProc (
  HWND         Hwnd,
  UINT         message,
  WPARAM       WParam,
  LPARAM       LParam
  )
{
  UI_DIALOG                    *Dialog;
  UI_DIALOG_TIME_PICKER_DATA   *Private;
  UI_MANAGER                   *Manager;
//UI_NOTIFY                    *Notify;
  UI_CONTROL                   *Sender;
  EFI_STATUS                   Status;
  CHAR16                       Str[10];
  UI_CONTROL                   *Control;

  Dialog   = (UI_DIALOG *) GetWindowLongPtr (Hwnd, 0);

  Manager = NULL;
  Private = NULL;

  if (Dialog != NULL)
  {
    Manager = ((UI_CONTROL *)Dialog)->Manager;
    Private = (UI_DIALOG_TIME_PICKER_DATA *) Dialog->Param;
  }

  if (message == WM_HOTKEY) {
    if (HIWORD (LParam) == VK_ESCAPE) {
      UiEndDialog (Manager->MainWnd, 0);
      return 1;
    }
  }

  if (message == UI_NOTIFY_WINDOWINIT) {
    Control = Manager->FindControlByName (Manager, L"title");
    if (Control != NULL) {
      Control->SetAttribute (Control, L"text", Private->Title);
    }

    Private->HourUp     = Manager->FindControlByName (Manager, L"hour_up");
    Private->HourDown   = Manager->FindControlByName (Manager, L"hour_down");
    Private->MinuteUp   = Manager->FindControlByName (Manager, L"minute_up");
    Private->MinuteDown = Manager->FindControlByName (Manager, L"minute_down");
    Private->SecondUp   = Manager->FindControlByName (Manager, L"second_up");
    Private->SecondDown = Manager->FindControlByName (Manager, L"second_down");

    Private->Hour       = Manager->FindControlByName (Manager, L"hour");
    Private->Minute     = Manager->FindControlByName (Manager, L"minute");
    Private->Second     = Manager->FindControlByName (Manager, L"second");

    Private->Ok         = Manager->FindControlByName (Manager, L"ok");
    Private->Cancel     = Manager->FindControlByName (Manager, L"cancel");

    UnicodeValueToString (Str, 0, (INT64)Private->Time.Hour, 0);
    Private->Hour->SetAttribute (Private->Hour, L"text", Str);

    UnicodeValueToString (Str, 0, (INT64)Private->Time.Minute, 0);
    Private->Minute->SetAttribute (Private->Minute, L"text", Str);

    UnicodeValueToString (Str, 0, (INT64) Private->Time.Second, 0);
    Private->Second->SetAttribute (Private->Second, L"text", Str);

    return 1;
  }

  if (message == UI_NOTIFY_CLICK) {

    INT64                           Minimum;
    INT64                           Maximum;
    INT64                           EditValue;
    INTN                            Step;
    UI_CONTROL                      *EditControl;

    Minimum = 0;
    Maximum = 0;
    Step    = 0;
    EditControl = NULL;
    Sender  = (UI_CONTROL*)WParam;

    if (Sender == Private->HourUp || Sender == Private->HourDown) {

      Minimum = 0;
      Maximum = 23;
      Step = (Sender == Private->HourUp) ? 1 : -1;
      EditControl = Private->Hour;

    } else if (Sender == Private->MinuteUp ||Sender == Private->MinuteDown) {

      Minimum = 0;
      Maximum = 59;
      Step = (Sender == Private->MinuteUp) ? 1 : -1;
      EditControl = Private->Minute;

    } else if (Sender == Private->SecondUp || Sender == Private->SecondDown) {

      Minimum = 0;
      Maximum = 59;
      Step = (Sender == Private->SecondUp) ? 1 : -1;
      EditControl = Private->Second;

    } else if (Sender == Private->Ok) {

      Private->Status = EFI_SUCCESS;
      UiEndDialog (Manager->MainWnd, 0);
      return 1;

    } else if (Sender == Private->Cancel) {

      Private->Status = EFI_NOT_READY;
      UiEndDialog (Manager->MainWnd, 0);
      return 1;

    } else {
      //
      // Don't run next code
      //
      return 0;
    }

    ASSERT (EditControl != NULL);
    EditValue = (INT64) StrToUInt (EditControl->Wnd->szTitle, 10, &Status);
    if ((EditValue + Step) > Maximum) {
      EditValue = Minimum;
    } else if ((EditValue + Step) < Minimum) {
      EditValue = Maximum;
    } else {
      EditValue += Step;
    }

    UnicodeValueToString (Str, 0, (INT64)EditValue, 0);
    EditControl->SetAttribute (EditControl, L"text", Str);

    //
    // write back date
    //
    Private->Time.Hour    = (UINT8) StrToUInt (Private->Hour->Wnd->szTitle, 10, &Status);
    Private->Time.Minute  = (UINT8) StrToUInt (Private->Minute->Wnd->szTitle, 10, &Status);
    Private->Time.Second  = (UINT8) StrToUInt (Private->Second->Wnd->szTitle, 10, &Status);
  }
  return 0;

}


/**
 Get password input from user.

 @param        Title
 @param        Time

 @retval EFI_SUCCESS            If string input is read successfully
 @retval EFI_DEVICE_ERROR       If operation fails

**/
EFI_STATUS
UiTimePickerDialog (
  CHAR16       *Title,
  EFI_HII_TIME *Time
  )
{

  UI_DIALOG_TIME_PICKER_DATA Private;

  Private.Title   = Title;
  Private.Time    = *Time;
  Private.Status  = EFI_NOT_READY;

  UiDialogBoxParam (
      NULL,
      mTimePickerDialogXml,
      NULL,
      TimePickerDialogProc,
      (LPARAM) &Private,
      0,
      GetSystemMetrics (SM_CYSCREEN) / 3,
      GetSystemMetrics (SM_CXSCREEN),
      GetSystemMetrics (SM_CYSCREEN) / 3,
      FALSE
      );

  if (!EFI_ERROR (Private.Status)) {
    *Time = Private.Time;
  }

  return Private.Status;
}




