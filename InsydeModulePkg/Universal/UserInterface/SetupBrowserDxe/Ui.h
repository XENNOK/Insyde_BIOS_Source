/** @file
 Definitions of UI implement functions for SetupBrowserDxe driver

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _UI_H
#define _UI_H


#include <PiDxe.h>
#include <Framework/FrameworkInternalFormRepresentation.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include "Setup.h"
//#include "OemGraphicsLib.h"
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/ConsoleLib.h>

//
// Globals
//
#define REGULAR_NUMERIC 0
#define TIME_NUMERIC    1
#define DATE_NUMERIC    2

#define SUBTITLE_INDENT  2


typedef enum {
  CfInitialization,
  CfCheckSelection,
  CfRepaint,
  CfRefreshHighLight,
  CfUpdateHelpString,
  CfPrepareToReadKey,
  CfReadKey,
  CfScreenOperation,
  CfUiSelect,
  CfUiReset,
  CfUiLeft,
  CfUiRight,
  CfUiUp,
  CfUiPageUp,
  CfUiPageDown,
  CfUiDown,
  CfUiSave,
  CfUiSaveCustom,
  CfUiDefaultCustom,
  CfUiSaveAndExit,
  CfUiDefault,
  CfUiDiscard,
  CfUiNoOperation,
  CfExit,
  CfUiTab,
  CfUiTabBack,
  CfUiJumpMenu,
  CfUiShowHelpScreen,
  CfMaxControlFlag
} UI_CONTROL_FLAG;

#define UI_ACTION_NONE               0
#define UI_ACTION_REFRESH_FORM       1
#define UI_ACTION_REFRESH_FORMSET    2
#define UI_ACTION_EXIT               3
#define UI_HOTKEY_ESC                4
#define UI_HOTKEY_F9                 5
#define UI_HOTKEY_F10                6

typedef struct _UI_MENU_LIST UI_MENU_LIST;

typedef struct {
  EFI_HII_HANDLE  Handle;

  //
  // Target formset/form/Question information
  //
  EFI_GUID        FormSetGuid;
  UINT16          FormId;
  UINT16          QuestionId;

  UINTN           TopRow;
  UINTN           BottomRow;
  UINTN           PromptCol;
  UINTN           OptionCol;
  UINTN           CurrentRow;

  //
  // Ation for Browser to taken:
  //   UI_ACTION_NONE            - navigation inside a form
  //   UI_ACTION_REFRESH_FORM    - re-evaluate expressions and repaint form
  //   UI_ACTION_REFRESH_FORMSET - re-parse formset IFR binary
  //
  UINTN           Action;

  //
  // Current selected fomset/form/Question
  //
  FORM_BROWSER_FORMSET    *FormSet;
  FORM_BROWSER_FORM       *Form;
  FORM_BROWSER_STATEMENT  *Statement;

  //
  // Whether the Form is editable
  //
  BOOLEAN                 FormEditable;

  //
  // Whether the Form has menu option or not
  //
  BOOLEAN                 NoMenuOption;

  //
  //  After showing errer message, let password dialog pop up again
  //
  BOOLEAN                 SelectAgain;

  UI_MENU_LIST            *CurrentMenu;
} UI_MENU_SELECTION;

#define UI_MENU_OPTION_SIGNATURE  SIGNATURE_32 ('u', 'i', 'm', 'm')
#define UI_MENU_LIST_SIGNATURE    SIGNATURE_32 ('u', 'i', 'm', 'l')

typedef struct {
  UINTN                   Signature;
  LIST_ENTRY              Link;

  EFI_HII_HANDLE          Handle;
  FORM_BROWSER_STATEMENT  *ThisTag;
  UINT16                  EntryNumber;

  UINTN                   Row;
  UINTN                   Col;
  UINTN                   OptCol;
  CHAR16                  *Description;
  UINTN                   Skip;           // Number of lines

  //
  // Display item sequence for date/time
  //  Date:      Month/Day/Year
  //  Sequence:  0     1   2
  //
  //  Time:      Hour : Minute : Second
  //  Sequence:  0      1        2
  //
  //
  UINTN                   Sequence;

  BOOLEAN                 GrayOut;
  BOOLEAN                 ReadOnly;

  //
  // Whether user could change value of this item
  //
  BOOLEAN                 IsQuestion;

} UI_MENU_OPTION;

#define MENU_OPTION_FROM_LINK(a)  CR (a, UI_MENU_OPTION, Link, UI_MENU_OPTION_SIGNATURE)

struct _UI_MENU_LIST {
  UINTN           Signature;
  LIST_ENTRY      Link;

  EFI_GUID        FormSetGuid;
  UINT16          FormId;
  UINT16          QuestionId;

  UI_MENU_LIST    *Parent;
  LIST_ENTRY      ChildListHead;
};

#define UI_MENU_LIST_FROM_LINK(a)  CR (a, UI_MENU_LIST, Link, UI_MENU_LIST_SIGNATURE)

typedef struct _MENU_REFRESH_ENTRY {
  struct _MENU_REFRESH_ENTRY  *Next;
  UI_MENU_OPTION              *MenuOption;  // Describes the entry needing an update
  UI_MENU_SELECTION           *Selection;
  UINTN                       CurrentColumn;
  UINTN                       CurrentRow;
  UINTN                       CurrentAttribute;
  UINTN                       SkipValue;
} MENU_REFRESH_ENTRY;

typedef struct {
  UI_SCREEN_OPERATION ScreenOperation;
  UI_CONTROL_FLAG     ControlFlag;
} SCREEN_OPERATION_T0_CONTROL_FLAG;

typedef struct {
  UINTN               Hotkey;
  EFI_QUESTION_ID     QuestionId;
} UI_HOTKEY_T0_QUESTION_ID;

#define UI_HOTKEY_NUM         11

typedef struct {
  EFI_INPUT_KEY       Key;
  UINTN               Y;
  UINTN               StartX;
  UINTN               EndX;
} UI_HOTKEY_INFO;

extern LIST_ENTRY          Menu;
extern LIST_ENTRY          gMenuList;
extern MENU_REFRESH_ENTRY  *gMenuRefreshHead;
extern UI_MENU_SELECTION   *gCurrentSelection;
extern BOOLEAN             mHiiPackageListUpdated;
extern BOOLEAN             gPopUpDialogBiggerThanPlatformDialog;

//
// Password pop up relative definitions
//
#define PASSWORD_INPUT_TYPE    0x01
#define PASSWORD_NOTICE_TYPE   0x02
//
// Global Functions
//
VOID
UiInitMenu (
  VOID
  );

VOID
UiFreeMenu (
  VOID
  );

UI_MENU_LIST *
UiAddMenuList (
  IN OUT UI_MENU_LIST     *Parent,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  );

UI_MENU_LIST *
UiFindChildMenuList (
  IN UI_MENU_LIST         *Parent,
  IN UINT16               FormId
  );

UI_MENU_LIST *
UiFindMenuList (
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  );

UI_MENU_OPTION *
UiAddMenuOption (
  IN CHAR16                  *String,
  IN EFI_HII_HANDLE          Handle,
  IN FORM_BROWSER_STATEMENT  *Statement,
  IN UINT16                  NumberOfLines,
  IN UINT16                  MenuItemCount
  );

EFI_STATUS
UiDisplayMenu (
  IN OUT UI_MENU_SELECTION           *Selection
  );

VOID
FreeBrowserStrings (
  VOID
  );

EFI_STATUS
SetupBrowser (
  IN OUT UI_MENU_SELECTION    *Selection
  );

EFI_STATUS
UiIntToString (
  IN UINTN      num,
  IN OUT CHAR16 *str,
  IN UINT16     size
  );

VOID
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  );

EFI_STATUS
UiWaitForSingleEvent (
  IN EFI_EVENT                Event,
  IN UINT64                   Timeout, OPTIONAL
  IN UINT8                    RefreshInterval OPTIONAL
  );

VOID
CreatePasswordPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       PasswordPopUpType,
  IN  UINTN                       MaxInputChar,
  IN  UINTN                       NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  );

EFI_STATUS
CreatePasswordDialog (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  EFI_STATUS                  Status
  );

VOID
EFIAPI
CreateMultiStringPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       NumberOfLines,
  ...
  );

EFI_STATUS
ReadString (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  CHAR16                      *Prompt,
  OUT CHAR16                      *StringPtr
  );

EFI_STATUS
ReadPassword (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  UINTN                       PassWordState,
  IN  UINTN                       LineNum,
  IN  BOOLEAN                     CreatePopUp,
  OUT CHAR16                      *StringPtr
  );

EFI_STATUS
GetSelectionInputPopUp (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption
  );

EFI_STATUS
GetNumericInput (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption
  );

VOID
UpdateStatusBar (
  IN  UINTN                       MessageType,
  IN  UINT8                       Flags,
  IN  BOOLEAN                     State
  );

EFI_STATUS
ProcessQuestionConfig (
  IN  UI_MENU_SELECTION       *Selection,
  IN  FORM_BROWSER_STATEMENT  *Question
  );

EFI_STATUS
PrintFormattedNumber (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN OUT CHAR16               *FormattedNumber,
  IN UINTN                    BufferSize
  );

QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  );

UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index
  );

VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index,
  IN UINT64                   Value
  );

EFI_STATUS
ChangeItemSelection (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  UINTN                       OptionItemAction
  );

EFI_STATUS
ProcessOptions (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected,
  OUT CHAR16                      **OptionString
  );

VOID
ProcessHelpString (
  IN  CHAR16                      *StringPtr,
  OUT CHAR16                      **FormattedString,
  IN  UINTN                       RowCount
  );

VOID
UpdateKeyHelp (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected
  );

VOID
DrawLines (
  IN  UINTN                       LeftColumn,
  IN  UINTN                       RightColumn,
  IN  UINTN                       TopRow,
  IN  UINTN                       BottomRow,
  IN  CHAR16                      Character
  );

VOID
ClearLines (
  IN  UINTN                        LeftColumn,
  IN  UINTN                        RightColumn,
  IN  UINTN                        TopRow,
  IN  UINTN                        BottomRow,
  IN  UINTN                        TextAttribute
  );

UINTN
GetStringWidth (
  CHAR16                                      *String
  );

EFI_STATUS
GetLineByWidth (
  IN      CHAR16                      *InputString,
  IN      UINT16                      LineWidth,
  OUT     UINT16                      *LineNum,
  OUT     CHAR16                      **OutputString
  );

UINT16
GetWidth (
  IN FORM_BROWSER_STATEMENT        *Statement,
  IN EFI_HII_HANDLE                 Handle
  );

VOID
NewStrCat (
  CHAR16                                      *Destination,
  CHAR16                                      *Source
  );

EFI_STATUS
WaitForKeyStroke (
  OUT  EFI_INPUT_KEY           *Key
  );

VOID
ResetScopeStack (
  VOID
  );

EFI_STATUS
PushScope (
  IN UINT8   Operand
  );

EFI_STATUS
PopScope (
  OUT UINT8     *Operand
  );

FORM_BROWSER_FORM *
IdToForm (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN UINT16                FormId
  );

FORM_BROWSER_STATEMENT *
IdToQuestion (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN UINT16                QuestionId
  );

FORM_EXPRESSION *
IdToExpression (
  IN FORM_BROWSER_FORM  *Form,
  IN UINT8              RuleId
  );

VOID
ExtendValueToU64 (
  IN  EFI_HII_VALUE   *Value
  );

EFI_STATUS
CompareHiiValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  OUT INTN            *Result,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  );

EFI_STATUS
EvaluateExpression (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN OUT FORM_EXPRESSION   *Expression
  );

EFI_STATUS
RefreshForm (
  IN BOOLEAN     RefereshDataTime
  );


VOID
UiRefreshAnimationList (
  );

LIST_ENTRY     *
AnimationMenu (
  OUT  EFI_INPUT_KEY                     *Key,
  IN   UINTN                             X,
  IN   UINTN                             Y
  );

VOID
DisplayMenuOptionValue(
  UI_MENU_OPTION *MenuOption,
  CHAR16         *OptionString,
  UINTN          SkipRows,
  UINTN          TopRow,
  UINTN          BottomRow
  );

BOOLEAN
CheckUnicode(
  IN  CHAR16                  StringPtr
  );

UINT8 *
GetAnimation (
  IN UINT8             *AnimationBlock,
  IN EFI_ANIMATION_ID  AnimationId
  );


#endif // _UI_H

