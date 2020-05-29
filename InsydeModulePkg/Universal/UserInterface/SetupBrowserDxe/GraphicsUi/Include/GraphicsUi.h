/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _GRAPHICS_UI_H_
#define _GRAPHICS_UI_H_

extern BOOLEAN                          mAlreadySendForm;


EFI_STATUS
UiConfirmDialog (
    IN  UINT32                          DialogOperator,
    IN  BOOLEAN                         HotKey,
    IN  UINT32                          MaximumStringSize,
    OUT CHAR16                          *StringBuffer,
    OUT EFI_INPUT_KEY                   *KeyValue,
    IN  CHAR16                          *String,
    ...
    );

EFI_STATUS
UiPasswordDialog (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  );

EFI_STATUS
UiCreateMsgPopUp (
  IN  UINT32                      RequestedWidth,
  IN  UINT32                      NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  );

EFI_STATUS
UiShowPageInfo (
  IN   CHAR16          *TitleString,
  IN   CHAR16          *InfoStrings
  );

typedef struct {
  LIST_ENTRY     Link;
  EFI_STRING     Str;
  VOID           *Data;
} COMBOBOX_ITEM;

#define COMBOBOX_ITEM_FROM_LINK(a)  BASE_CR (a, COMBOBOX_ITEM, Link)

typedef struct {
  UI_CONTROL            *Control;
  COMBOBOX_ITEM         *SelectItem;
  UINTN                 NumOfItems;
  LIST_ENTRY            ItemList;
} COMBOBOX_INFO;

LRESULT
SelectionInputProc (
  HWND    Hwnd,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam
  );

EFI_STATUS
UiGetSelectionInputPopUp (
  IN  UI_CONTROL                  *SpinnerControl,
  IN  HWND                        Parent,
  IN  UI_MENU_SELECTION           *Selection,
  FORM_BROWSER_STATEMENT          *Statement
  );

EFI_STATUS
UiReadPassword (
  IN  UI_MENU_SELECTION           *Selection,
  IN  FORM_BROWSER_STATEMENT      *Statement,
  IN  UINTN                       PassWordState,
  IN  BOOLEAN                     CreatePopUp,
  OUT CHAR16                      *OldPassword,
  OUT CHAR16                      *NewPassword
  );

EFI_STATUS
UiDatePickerDialog (
  CHAR16       *Title,
  EFI_HII_DATE *Date
  );

EFI_STATUS
UiTimePickerDialog (
  CHAR16       *Title,
  EFI_HII_TIME *Time
  );

UINT32
GetImageIdByName (
  CHAR16  *Name
  );


VOID
InitializeGraphicsDialogXML (
  VOID
  );

VOID
UpdateHelpString (
  UI_MENU_OPTION *MenuOption
  );


EFI_STATUS
RefreshFormData (
  UI_MENU_SELECTION     *Selection
  );

EFI_STATUS
EvaluateFormExpressions (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form
  );

LRESULT
CALLBACK
FormBrowserProc (
  HWND Hwnd,
  UINT message,
  WPARAM WParam,
  LPARAM LParam
  );

EFI_STATUS
PasswordCallback (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  CHAR16                      *String
  );

EFI_STATUS
InitializeGUI (
  );

#endif
