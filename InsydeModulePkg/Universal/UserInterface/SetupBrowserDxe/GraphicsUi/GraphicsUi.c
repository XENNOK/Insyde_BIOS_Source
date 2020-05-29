/** @file
  Graphics UI

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

#include "UiCellLayout.h"
#include "GraphicsUi.h"
#include "Setup.h"
#include <Protocol/SetupUtilityBrowser.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/OemGraphicsLib.h>

EFI_STATUS
EFIAPI
FormUpdateNotify (
  IN UINT8                              PackageType,
  IN CONST EFI_GUID                     *PackageGuid,
  IN CONST EFI_HII_PACKAGE_HEADER       *Package,
  IN EFI_HII_HANDLE                     Handle,
  IN EFI_HII_DATABASE_NOTIFY_TYPE       NotifyType
  );

extern H2O_WINDOW_PROTOCOL *gH2OWindow;

#define FB_NOTIFY_REFRESH          (WM_APP + 1)
#define FB_NOTIFY_REPAINT          (WM_APP + 2)
#define FB_NOTIFY_CHECK_SELECTION  (WM_APP + 3)
#define FB_NOTIFY_RESET            (WM_APP + 4)
#define FB_NOTIFY_LOAD_DEFAULT     (WM_APP + 5)
#define FB_NOTIFY_SAVE_AND_EXIT    (WM_APP + 6)


#define FB_REFRSH_QUESTION_TIMER_ID 1

extern LIST_ENTRY Menu;
extern STRING_REF mHelpString[];

EFI_GUID                         mInputFormSetGuid;

STATIC UI_HOTKEY_T0_QUESTION_ID gUiHotkeyToQuestionId[] = {
  { UI_HOTKEY_ESC, KEY_SCAN_ESC},
  { UI_HOTKEY_F9,  KEY_SCAN_F9 },
  { UI_HOTKEY_F10, KEY_SCAN_F10}
};


BOOLEAN mFrontPage = FALSE;
BOOLEAN mAlreadySendForm = FALSE;

//UI_MENU_SELECTION *Selection;

CHAR16 *mFrontPageXml    = NULL;
CHAR16 *mScuXml          = NULL;
CHAR16 *mSetupMenuSelectXml   = NULL;
CHAR16 *mSetupMenuUnSelectXml = NULL;
CHAR16 *mFrontPagePromptXml   = NULL;
CHAR16 *mFrontPagePromptWithAniXml = NULL;
EFI_EVENT mReadyToBootEvent        = NULL;

typedef struct _FORM_BROWSER_FRAME FORM_BROWSER_FRAME;

struct _FORM_BROWSER_FRAME {
  UI_FRAME     Frame;

  UI_CONTAINER *SetupMenuList;
  UI_CONTROL   *SetupMenuPanel;
  UI_CONTAINER *MenuOptionList;
  UI_CONTROL   *SubMenuTitle;
  UI_CONTROL   *Background;
  UI_CONTAINER *BootOrder;

  UI_CONTROL                  **RefreshMenuList;
  UINTN                       RefreshMenuCount;
  UI_MENU_SELECTION           *Selection;
  CELL_INFO                   *CellInfoList;
  UINT16                      CellCount;
  EFI_HANDLE                  PackageNotifyHandle;
};

#define FORM_BROWSER_CLASS_NAME  L"FormBrowser"


VOID
FBReadyToBootCallback (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;

  gBS->CloseEvent (Event);
  mReadyToBootEvent = NULL;
  GdCloseMouse ();

  //
  // clear screen
  //
  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  SetMem (&Color, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
  Status = GraphicsOutput->Blt (
                            GraphicsOutput,
                            &Color,
                            EfiBltVideoFill,
                            0,
                            0,
                            0,
                            0,
                            GraphicsOutput->Mode->Info->HorizontalResolution,
                            GraphicsOutput->Mode->Info->VerticalResolution,
                            0
                            );
}


EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  )
{
  UINTN        ModeNum;
  UINTN        Columns;
  UINTN        Rows;
  EFI_STATUS   Status;

  for (ModeNum = 0; ModeNum < (UINTN)(gST->ConOut->Mode->MaxMode); ModeNum++) {
    gST->ConOut->QueryMode (gST->ConOut, ModeNum, &Columns, &Rows);
    if ((RequestedColumns == Columns) && (RequestedRows == Rows)) {
      *TextModeNum = ModeNum;
      break;
    }
  }

  if (ModeNum == (UINTN)(gST->ConOut->Mode->MaxMode)) {
    *TextModeNum = ModeNum;
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS
CheckModeSupported (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                    HorizontalResolution,
  IN  UINT32                    VerticalResolution,
  OUT UINT32                    *CurrentModeNumber
  );

EFI_STATUS
InitializeGUI (
  )
{
  WNDCLASS           wndclass;
  STATIC BOOLEAN     FirstIn = TRUE;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
  EFI_STATUS                    Status;
  UINTN                         RequestedRows;
  UINTN                         RequestedColumns;
  UINTN                         TextModeNum;
  UINT32                                 Index;
  UINTN                                  SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
  OEM_LOGO_RESOLUTION_DEFINITION         *OemLogoResolutionPtr;
  UINT32                                 SizeOfX;
  UINT32                                 SizeOfY;

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gSetupMouseProtocolGuid,
                  NULL,
                  (VOID**) &mSetupMouse
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GdAddGopDevice (GraphicsOutput);

  OemLogoResolutionPtr = (OEM_LOGO_RESOLUTION_DEFINITION *) PcdGetPtr (PcdDefaultLogoResolution);
  Status = OemSvcLogoResolution (&OemLogoResolutionPtr);

  SizeOfX = OemLogoResolutionPtr->ScuResolutionX;
  SizeOfY = OemLogoResolutionPtr->ScuResolutionY;

  //
  // check current GOP mode
  //
  Status = EFI_SUCCESS;
  if ((GraphicsOutput->Mode->Info->HorizontalResolution != SizeOfX) ||
      (GraphicsOutput->Mode->Info->VerticalResolution   != SizeOfY)) {

    for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
      GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
      if (Info->HorizontalResolution == SizeOfX && Info->VerticalResolution == SizeOfY) {
        Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
        Status = CheckModeSupported (
                   GraphicsOutput,
                   SizeOfX,
                   SizeOfY,
                   &Index
                   );
        if (!EFI_ERROR(Status)) {
          RequestedColumns = SizeOfX / EFI_GLYPH_WIDTH;
          RequestedRows = SizeOfY / EFI_GLYPH_HEIGHT;
          Status = ChkTextModeNum (RequestedColumns, RequestedRows, &TextModeNum);
          if ((UINTN)(gST->ConOut->Mode->Mode) != TextModeNum) {
            Status = gST->ConOut->SetMode (gST->ConOut, TextModeNum);
          }
        }
        break;
      }
    }

    if ((EFI_ERROR (Status) || Index == GraphicsOutput->Mode->MaxMode)) {
      Status = EFI_NOT_FOUND;
    }
  }

  if (MwOpen() < 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (!FirstIn) {
    return EFI_SUCCESS;
  }

  FirstIn = FALSE;

  //
  // register class
  //
  RegisterUiControl  (NULL);
  RegisterUiLabel    (NULL);
  RegisterUiButton   (NULL);
  RegisterUiSwitch   (NULL);
  RegisterUiScrollBar (NULL);
  RegisterUiDialog   (NULL);
  RegisterUiEditBar  (NULL);


  RegisterUiContainer (NULL);
  RegisterUiVerticalLayout (NULL);
  RegisterUiHorizontalLayout (NULL);
  RegisterUiTileLayout (NULL);

  RegisterUiCellLayout (NULL);


  wndclass.lpszClassName  = FORM_BROWSER_CLASS_NAME;
  wndclass.style          = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc    = (WNDPROC) FormBrowserProc;
  wndclass.cbClsExtra     = 0;
  wndclass.cbWndExtra     = sizeof (UI_FRAME *);
  wndclass.hInstance      = 0;
  wndclass.hIcon          = 0;
  wndclass.hCursor        = 0;
  wndclass.hbrBackground  = (HBRUSH) GetStockObject (BS_NULL);
  wndclass.lpszMenuName   = NULL;
  RegisterClass(&wndclass);

  return EFI_SUCCESS;
}

struct {
  COLORREF   Color;
  CELL_INFO  CellInfo;
} mFrontPageList[] = {

  {MWRGB (0x05,0x8B,0xDE), {NULL, 0, 0, 1, 1}},
  {MWRGB (0x00,0xA6,0x91), {NULL, 1, 0, 1, 1}},
  {MWRGB (0x7F,0x4E,0xFF), {NULL, 2, 0, 2, 1}},

  {MWRGB (0x00,0xA6,0x91), {NULL, 0, 1, 1, 1}},
  {MWRGB (0xF5,0x24,0x7A), {NULL, 1, 1, 2, 1}},
  {MWRGB (0x00,0xA2,0xFF), {NULL, 3, 1, 1, 1}},

  {MWRGB (0x00,0xA2,0xFF), {NULL, 0, 2, 2, 1}},
  {MWRGB (0x7F,0x4E,0xFF), {NULL, 2, 2, 1, 1}},
  {MWRGB (0xF5,0x24,0x7A), {NULL, 3, 2, 1, 1}},
};

VOID
FBSetComponent (
  FORM_BROWSER_FRAME *This
  )
{
  UI_MANAGER  *Manager;
  UI_CONTROL  *Control;

  Manager = ((UI_CONTROL *) This)->Manager;

  This->SubMenuTitle   = Manager->FindControlByName (Manager, L"sub_title");
  This->Background     = Manager->FindControlByName (Manager, L"background");
  This->SetupMenuPanel = Manager->FindControlByName (Manager, L"setupmenu_panel");

  This->SetupMenuList  = (UI_CONTAINER *)Manager->FindControlByName (Manager, L"setupmenulist");
  This->MenuOptionList = (UI_CONTAINER *)Manager->FindControlByName (Manager, L"menuoptionlist");
  This->BootOrder      = (UI_CONTAINER *)Manager->FindControlByName (Manager, L"bootorder");

  if (This->BootOrder != NULL) {
    Control = Manager->FindControlByName (Manager, L"down");
    ASSERT (Control != NULL);
    SetWindowLongPtr (Control->Wnd, GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    Control = Manager->FindControlByName (Manager, L"up");
    ASSERT (Control != NULL);
    SetWindowLongPtr (Control->Wnd, GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
  }
}

VOID
FBRefreshSetupMenuList (
  FORM_BROWSER_FRAME *This
  )
{
  UI_MANAGER            *Manager;
  UI_CONTROL            *Option;
  UINTN                 Index;
  EFI_STRING            StrPtr;
  EFI_HII_HANDLE        HiiHandle;

  if (gDeviceManagerSetup == NULL) {
    if (This->SetupMenuPanel != NULL) {
      ShowWindow (This->SetupMenuPanel->Wnd, SW_HIDE);
    }
    return ;
  }

  if (This->SetupMenuList == NULL) {
    ASSERT (This->SetupMenuList != NULL);
    return ;
  }

  InitializeMenuStrings (gDeviceManagerSetup);
  if (This->SetupMenuList->ItemCount != gDeviceManagerSetup->MenuItemCount) {
    This->SetupMenuList->RemoveAll (This->SetupMenuList);
  }

  Manager = ((UI_CONTROL *) This)->Manager;
  for (Index = gDeviceManagerSetup->MenuItemCount; Index > 0 ; Index--) {
    if (This->SetupMenuList->ItemCount == gDeviceManagerSetup->MenuItemCount) {
      Option = This->SetupMenuList->Items[gDeviceManagerSetup->MenuItemCount - Index];
    } else {
      Option = CreateControl (L"Button", NULL, (UI_CONTROL *)This->SetupMenuList, Manager);
      SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, (INTN)(Index - 1));
      This->SetupMenuList->Add (This->SetupMenuList, Option);
      Option->SetAttribute (Option, L"name", L"page");
    }
    HiiHandle = gDeviceManagerSetup->MenuList[Index - 1].Page;
    StrPtr = GetToken (gDeviceManagerSetup->MenuList[Index - 1].MenuTitle, HiiHandle);
    Option->SetAttribute (Option, L"text", StrPtr);
    ScuSafeFreePool ((VOID **)&StrPtr);

    if (gDeviceManagerSetup->CurRoot == (Index - 1)) {
      UiApplyAttributeList (Option, mSetupMenuSelectXml);
    } else {
      UiApplyAttributeList (Option, mSetupMenuUnSelectXml);
    }
  }
}

EFI_STATUS
FBRefreshFormSet (
  UI_MENU_SELECTION     *Selection
  )
{
  EFI_STATUS                      Status;
  FORM_BROWSER_FORMSET            *FormSet;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST      DummyActionRequest;


  ASSERT (Selection != NULL);

  //
  // set hii handle
  //
  if (gDeviceManagerSetup != NULL &&
      gDeviceManagerSetup->Direction != NoChange) {
    Selection->Handle = gDeviceManagerSetup->MenuList[gDeviceManagerSetup->CurRoot].Page;
    CopyMem (&Selection->FormSetGuid, &mInputFormSetGuid, sizeof (EFI_GUID));
    Selection->FormId = 0;
    if (Selection->CurrentMenu != NULL) {
      Selection->CurrentMenu->QuestionId = 0;
    }
    gDeviceManagerSetup->Direction = NoChange;
  }

  ScuSafeFreePool ((VOID **)&gAnimationPackage);
  GetAnimationPackage (Selection->Handle, &gAnimationPackage);

  if (Selection->FormSet != NULL) {
    //
    // restore SCU local data
    //
    ConfigAccess = Selection->FormSet->ConfigAccess;
    if(gDeviceManagerSetup) {
      ConfigAccess->Callback(ConfigAccess, EFI_BROWSER_ACTION_CHANGING, GET_SETUP_CONFIG, 0, NULL, &DummyActionRequest);
    }
    DestroyFormSet (Selection->FormSet);
  }

  Selection->FormSet = AllocateZeroPool (sizeof (FORM_BROWSER_FORMSET));
  FormSet            = Selection->FormSet;

  //
  // Initialize internal data structures of FormSet
  //
  Status = InitializeFormSet (Selection->Handle, &Selection->FormSetGuid, FormSet);
  if (EFI_ERROR (Status) || IsListEmpty (&FormSet->FormListHead)) {
    DestroyFormSet (FormSet);
    Selection->FormSet = NULL;
    return EFI_LOAD_ERROR;
  }
  Selection->FormSet = FormSet;

  mFrontPage = FALSE;
  if (CompareGuid (&Selection->FormSetGuid, &gFrontPageFormsetGuid)) {
    mFrontPage = TRUE;
  }

  //
  // Initialize current settings of Questions in this FormSet
  //
  Status = InitializeCurrentSetting (FormSet);
  if (EFI_ERROR (Status)) {
    DestroyFormSet (FormSet);
    Selection->FormSet = NULL;
    return EFI_LOAD_ERROR;
  }

  //
  // set SCU local data
  //
  ConfigAccess = Selection->FormSet->ConfigAccess;
  if(gDeviceManagerSetup) {
    ConfigAccess->Callback(ConfigAccess, EFI_BROWSER_ACTION_CHANGING, SET_SETUP_CONFIG, 0, NULL, &DummyActionRequest);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
FBRefreshFormData (
  UI_MENU_SELECTION     *Selection
  )
{
  EFI_STATUS                      Status;
  LIST_ENTRY                      *FormLink;

  //
  // Initialize Selection->Form
  //
  if (Selection->FormId == 0) {
    //
    // Zero FormId indicates display the first Form in a FormSet
    //
    FormLink = GetFirstNode (&Selection->FormSet->FormListHead);

    Selection->Form = FORM_BROWSER_FORM_FROM_LINK (FormLink);
    Selection->FormId = Selection->Form->FormId;
  } else {
    Selection->Form = IdToForm (Selection->FormSet, Selection->FormId);
  }

  //
  // No Form to display
  //
  if (Selection->Form == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Load Questions' Value for display
  //
  Status = LoadFormSetConfig (Selection->FormSet);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Deal with root menu status
  //
  if (gDeviceManagerSetup) {
    if (Selection->FormId == 1) {
      gDeviceManagerSetup->AtRoot = TRUE;
    } else {
      gDeviceManagerSetup->AtRoot = FALSE;
    }
  }

  //
  // Evaluate all the Expressions in this Form
  //
  Status = EvaluateFormExpressions (Selection->FormSet, Selection->Form);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

BOOLEAN
FBIsSelectable (
  FORM_BROWSER_STATEMENT  *Statement
  )
{
  if ((Statement->Operand == EFI_IFR_SUBTITLE_OP) ||
      ((Statement->GrayOutExpression != NULL) && Statement->GrayOutExpression->Result.Value.b) ||
      (Statement->ValueExpression != NULL) ||
      (Statement->QuestionFlags & EFI_IFR_FLAG_READ_ONLY)) {
    return FALSE;
  }
  return TRUE;
}

EFI_STATUS
FBRefreshMenuOptionList (
  FORM_BROWSER_FRAME *This
  )
{
  UI_MANAGER             *Manager;
  UI_CONTAINER           *MenuOptionList;
  LIST_ENTRY             *Link;
  EFI_STRING             OptionString;
  CHAR16                 *CtrlName;
  UI_CONTROL             *Control;
  UI_CONTROL             *Layout;
  UI_CONTROL             *Prompt;
  CHAR16                 *PromptString;
  UI_CONTROL             *Help;
  UI_CONTROL             *Option;
  UI_CONTROL             *SelectControl;
  UI_EDIT                *Edit;
  CHAR16                 *TmpString;
  CHAR16                 *AttributesStr;
  CHAR16                 *PromptStr;
  CHAR16                 *HelpString;
  UI_CELL_LAYOUT         *CellLayout;
  BOOLEAN                HasBootOrder;
  UI_MENU_SELECTION      *Selection;
  UI_MENU_LIST           *CurrentMenu;
  FORM_BROWSER_FORM      *Form;
  FORM_BROWSER_STATEMENT *Statement;
  BOOLEAN                Suppress;
  HWND                   HWnd;
  UI_MENU_OPTION         FakeMenuOption;
  UINT8                  Operand;
  BOOLEAN                GrayOut;


  Control = (UI_CONTROL *) This;
  HWnd    = Control->Wnd;
  Manager = Control->Manager;

  MenuOptionList = This->MenuOptionList;
  Selection      = This->Selection;
  CurrentMenu    = Selection->CurrentMenu;

  TmpString = AllocatePool (sizeof (CHAR16) * 0x1000);
  AttributesStr = AllocatePool (sizeof (CHAR16) * 0x1000);
  ASSERT (TmpString != NULL);
  ASSERT (AttributesStr != NULL);

  //
  // set background, bugbug it need run after refrsh form set
  //
  if (This->Selection->FormSet->AnimationId != 0 && This->Background != NULL) {
    UnicodeValueToString (TmpString, 0, (INT64)This->Selection->FormSet->AnimationId, 0);
    This->Background->SetAttribute (This->Background, L"bkimage",  TmpString);
  }

  //
  // set scroll bar position to zero
  //
  if (CurrentMenu != NULL &&
      (!CompareGuid (&CurrentMenu->FormSetGuid, &Selection->FormSetGuid) ||
       CurrentMenu->FormId != Selection->FormId)) {
    ((UI_CONTROL *)MenuOptionList)->SetAttribute ((UI_CONTROL *)MenuOptionList, L"vscrollbarpos", L"0");
  }

  CurrentMenu = UiFindMenuList (&Selection->FormSetGuid, Selection->FormId);
  if (CurrentMenu == NULL) {
    //
    // Current menu not found, add it to the menu tree
    //
    CurrentMenu = UiAddMenuList (NULL, &Selection->FormSetGuid, Selection->FormId);
  }
  Selection->CurrentMenu = CurrentMenu;

  MenuOptionList->RemoveAll (MenuOptionList);

  //
  // init selection
  //

  Form = Selection->Form;


  This->CellCount = 0;
  This->CellInfoList = NULL;
  if (mFrontPage) {
    This->CellInfoList = AllocateZeroPool (Form->NumberOfStatement * sizeof (CELL_INFO));
  }

  This->RefreshMenuCount = 0;
  if (This->RefreshMenuList) {
    gBS->FreePool (This->RefreshMenuList);
  }
  This->RefreshMenuList  = AllocateZeroPool (Form->NumberOfStatement * sizeof (UI_CONTROL*));

  Selection->Statement = NULL;


  SelectControl = NULL;
  HasBootOrder = FALSE;
  Layout       = (UI_CONTROL *) MenuOptionList;

  Selection->NoMenuOption = TRUE;
  Selection->FormEditable = FALSE;
  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Statement = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    Operand = Statement->Operand;

    if (mFrontPage && Operand != EFI_IFR_REF_OP && Operand != EFI_IFR_ACTION_OP) {
      Link = GetNextNode (&Selection->Form->StatementListHead, Link);
      continue;
    }

    if (Statement->SuppressExpression != NULL) {
      Suppress = Statement->SuppressExpression->Result.Value.b;
    } else {
      Suppress = FALSE;
    }

    if (Statement->DisableExpression != NULL) {
      Suppress = Suppress || Statement->DisableExpression->Result.Value.b;
    }

    if (Suppress) {
      Link = GetNextNode (&Selection->Form->StatementListHead, Link);
      continue;
    }

    Selection->NoMenuOption = FALSE;
    Selection->FormEditable = TRUE;
    GrayOut = FALSE;
    if ((Statement->GrayOutExpression != NULL) && Statement->GrayOutExpression->Result.Value.b) {
      GrayOut = TRUE;
    }


    if (Statement->QuestionFlags == EFI_IFR_CHECKBOX_DEFAULT_MFG && Statement->Operand == EFI_IFR_ACTION_OP) {
      HasBootOrder = TRUE;
    }


    //
    // help string
    //
    if (Statement->Help == 0) {
      HelpString = AllocateZeroPool (sizeof (CHAR16));
    } else {
      HelpString = GetToken (Statement->Help, Selection->Handle);
    }

    //
    // prompt string
    //
    PromptStr = GetToken (Statement->Prompt, Selection->Handle);
    CtrlName = NULL;
    PromptString = PromptStr;
    AttributesStr[0] = '\0';

    Prompt = NULL;
    Option = NULL;

    //
    // option string
    //
    OptionString = NULL;
    if (((Operand == EFI_IFR_TEXT_OP) || (Operand == EFI_IFR_ACTION_OP)) && (Statement->TextTwo != 0)) {
      OptionString = GetToken (Statement->TextTwo, Selection->Handle);
    } else {
      FakeMenuOption.ThisTag = Statement;
      ProcessOptions (Selection, &FakeMenuOption, FALSE, &OptionString);
    }

    if (mFrontPage) {
      CtrlName = L"Button";
      if (Statement->AnimationId != 0) {
        UnicodeSPrint  (
          AttributesStr,
          0x2000,
          L"name='menu_prompt' bkcolor='0x%08x' bkcolor2='0x%08x' %s bkimage='%d'",
          mFrontPageList[This->CellCount].Color & 0xFFFFFFFF,
          mFrontPageList[This->CellCount].Color & 0xFFFFFFFF,
          mFrontPagePromptWithAniXml,
          Statement->AnimationId
          );
      } else {
        UnicodeSPrint  (
          AttributesStr,
          0x2000,
          L"name='menu_prompt' bkcolor='0x%08x' bkcolor2='0x%08x' %s",
          mFrontPageList[This->CellCount].Color & 0xFFFFFFFF,
          mFrontPageList[This->CellCount].Color & 0xFFFFFFFF,
          mFrontPagePromptXml
          );
      }
      Prompt = CreateControl (CtrlName, PromptString, Layout, Manager);
      UiApplyAttributeList (Prompt, AttributesStr);
      SetWindowLongPtr (Prompt->Wnd, GWLP_USERDATA, (INTN)Statement);
      ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Prompt);
      This->CellInfoList[This->CellCount] = mFrontPageList[This->CellCount].CellInfo;
      This->CellInfoList[This->CellCount].Control = (UI_CONTROL *) Prompt;
      This->CellCount++;
    } else {
      if (Operand == EFI_IFR_SUBTITLE_OP) {

        Prompt = CreateControl (L"Label", PromptString, Layout, Manager);
        UiApplyAttributeList (Prompt, L"textcolor='0x%0xFF008287' fontsize='40'");
        ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Prompt);

      } else if (GrayOut) {

        if (OptionString != NULL) {
          UnicodeSPrint (TmpString, 0x2000, L"<c 0xFF0000FF><f 19>%s</f></c>\n<c 0xFF0000FF>%s</c>", PromptStr, OptionString);
        } else {
          UnicodeSPrint (TmpString, 0x2000, L"<c 0xFF0000FF><f 19>%s</f></c>\n%s", PromptStr, HelpString);
        }
        Prompt = CreateControl (L"Label", TmpString, Layout, Manager);
        ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Prompt);

      } else if (Operand == EFI_IFR_REF_OP || Operand == EFI_IFR_ACTION_OP || Operand == EFI_IFR_PASSWORD_OP) {

        StrCpy (AttributesStr, L"name='menu_prompt' textpadding='5,2,5,2'");
        UnicodeSPrint (TmpString, 0x2000, L"<c 0xFF0000FF><f 23 >%s</f></c>\n%s", PromptStr, HelpString);
        PromptString = TmpString;
        Prompt = CreateControl (L"Button", PromptString, Layout, Manager);
        UiApplyAttributeList (Prompt, AttributesStr);
        SetWindowLongPtr (Prompt->Wnd, GWLP_USERDATA, (INTN)Statement);
        ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Prompt);

      } else {

        StrCpy (AttributesStr, L"textcolor='0xFF008287' fontsize='40'");
        Prompt = CreateControl (L"Label", PromptString, Layout, Manager);
        UiApplyAttributeList (Prompt, AttributesStr);
        SetWindowLongPtr (Prompt->Wnd, GWLP_USERDATA, (INTN)Statement);
        ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Prompt);

        Help = NULL;
        CtrlName = NULL;
        if (((Operand == EFI_IFR_TEXT_OP) || (Operand == EFI_IFR_ACTION_OP)) && (Statement->TextTwo != 0)) {
          CtrlName = L"Label";
        } else if (Operand == EFI_IFR_NUMERIC_OP || Operand == EFI_IFR_STRING_OP) {
          CtrlName = L"UiEdit";
        } else if (Operand == EFI_IFR_TIME_OP || Operand == EFI_IFR_DATE_OP) {
          CtrlName = L"Button";
        } else if (Operand == EFI_IFR_CHECKBOX_OP) {
          CtrlName = L"Switch";
        } else if (FBIsSelectable (Statement)) {
          CtrlName = L"Button";
        } else {
          CtrlName = L"Label";
        }

        if (CtrlName != NULL) {

          Option = CreateControl (CtrlName, OptionString, Layout, Manager);
          Option->SetAttribute (Option, L"name", L"menu_option");
          Option->SetAttribute (Option, L"textpadding", L"0,2,0,2");
          ((UI_CONTAINER *)Layout)->Add ((UI_CONTAINER *)Layout, Option);
          SetWindowLongPtr (Option->Wnd, GWLP_USERDATA, (INTN)Statement);

          if (Statement->Operand == EFI_IFR_TIME_OP || Statement->Operand == EFI_IFR_DATE_OP) {
            This->RefreshMenuList[This->RefreshMenuCount++] = Option;
          }

          if (Statement->Operand == EFI_IFR_NUMERIC_OP) {

            Edit = (UI_EDIT *) Option;
            Edit->MaxValue = Statement->Maximum;
            Edit->MinValue = Statement->Minimum;
            Edit->Step     = Statement->Step;

            if ((Statement->Flags & EFI_IFR_DISPLAY) == EFI_IFR_DISPLAY_UINT_HEX) {
              Option->SetAttribute (Option, L"hex", L"true");
            } else {
              Option->SetAttribute (Option, L"dec", L"true");
            }
          }

          if (Statement->Operand == EFI_IFR_CHECKBOX_OP) {
            Option->SetAttribute (Option, L"width", L"100");
            Option->SetAttribute (Option, L"float", L"true");
            Option->SetAttribute (Option, L"checkboxvalue", Statement->HiiValue.Value.b ? L"true" : L"false");
          }
        }
      }
    }
    ScuSafeFreePool ((VOID **)&PromptStr);
    ScuSafeFreePool ((VOID **)&HelpString);
    ScuSafeFreePool ((VOID **)&OptionString);

    //
    // Select Control
    //
    if (SelectControl == NULL ||
        (CurrentMenu->QuestionId != 0 && Statement->QuestionId == CurrentMenu->QuestionId)) {

      Selection->Statement = Statement;
      if (Prompt != NULL && !(GetWindowLongPtr (Prompt->Wnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        SelectControl = Prompt;
      }

      if (Option != NULL && !(GetWindowLongPtr (Option->Wnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        SelectControl = Option;
      }
    }
    Link = GetNextNode (&Selection->Form->StatementListHead, Link);
  }

  if (This->BootOrder != NULL) {
    ShowWindow (((UI_CONTROL*)This->BootOrder)->Wnd, HasBootOrder ? SW_SHOW : SW_HIDE);
  }

  if (This->RefreshMenuCount != 0) {
    SetTimer (HWnd, FB_REFRSH_QUESTION_TIMER_ID, 300, NULL);
  } else {
    KillTimer (HWnd, FB_REFRSH_QUESTION_TIMER_ID);
  }

  if (SelectControl != NULL) {
    SetFocus (SelectControl->Wnd);
  }

  if (mFrontPage) {
    CellLayout = (UI_CELL_LAYOUT *)MenuOptionList;
    CellLayout->SetCellInfo (CellLayout, (UINTN)This->CellCount, This->CellInfoList);
    gBS->FreePool (This->CellInfoList);
  }

  SetWindowPos (MenuOptionList->Control.Wnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
  SendMessage (MenuOptionList->Control.Wnd, UI_NOTIFY_SET_POSITION, 0, 0);
  MenuOptionList->Control.Invalidate (&MenuOptionList->Control);

  ScuSafeFreePool ((VOID **)&TmpString);
  ScuSafeFreePool ((VOID **)&AttributesStr);

  return EFI_SUCCESS;
}

EFI_STATUS
FBPasswordCallback (
  IN  UI_MENU_SELECTION           *Selection,
  IN  FORM_BROWSER_STATEMENT      *Statement,
  IN  CHAR16                      *String
  )
{
  UI_MENU_OPTION   MenuOption;

  MenuOption.ThisTag = Statement;
  return PasswordCallback (Selection, &MenuOption, String);
}

EFI_STATUS
FBProcessOptions (
  FORM_BROWSER_FRAME       *This,
  UI_MENU_SELECTION        *Selection,
  FORM_BROWSER_STATEMENT   *Question,
  UI_CONTROL               *Sender
  )
{
  EFI_STATUS             Status;
  UI_MENU_LIST           *CurrentMenu;
  EFI_HII_VALUE          *QuestionValue;
  CHAR16                 *PromptStr;
  UINTN                  StringLen;
  UINT16                 Maximum;
  CHAR16                 *OldPassword;
  CHAR16                 *NewPassword;
  UINTN                  PasswordStatus;


  Status      = EFI_UNSUPPORTED;
  CurrentMenu = Selection->CurrentMenu;
  QuestionValue = &Question->HiiValue;

  switch (Question->Operand) {

  case EFI_IFR_ORDERED_LIST_OP:
    break;

  case EFI_IFR_ONE_OF_OP:
    Status = UiGetSelectionInputPopUp (Sender, ((UI_CONTROL*)This)->Wnd, Selection, Question);
    break;

  case EFI_IFR_CHECKBOX_OP:
    QuestionValue->Value.b = QuestionValue->Value.b ? FALSE : TRUE;

    //
    // Perform inconsistent check
    //
    Status = ValidateQuestion (Selection->FormSet, Selection->Form, Question, EFI_HII_EXPRESSION_INCONSISTENT_IF);
    if (EFI_ERROR (Status)) {
      //
      // Inconsistent check fail, restore Question Value
      //
      QuestionValue->Value.b = QuestionValue->Value.b ? FALSE : TRUE;
      return Status;
    }

    Status = SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
    UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
    break;

  case EFI_IFR_NUMERIC_OP:
    //
    // bugbug, it need pop a dialog for user input.
    //
    if ((Question->Flags & EFI_IFR_DISPLAY) == EFI_IFR_DISPLAY_UINT_HEX) {
      QuestionValue->Value.u64 = StrToUInt (Sender->Wnd->szTitle, 16, &Status);
    } else {
      QuestionValue->Value.u64 = StrToUInt (Sender->Wnd->szTitle, 10, &Status);
    }
    //
    // Check to see if the Value is something reasonable against consistency limitations.
    // If not, let's kick the error specified.
    //
    Status = ValidateQuestion (Selection->FormSet, Selection->Form, Question, EFI_HII_EXPRESSION_INCONSISTENT_IF);
    if (EFI_ERROR (Status)) {
      //
      // Input value is not valid, restore Question Value
      //
      GetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
    } else {
      SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
      UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
    }
    break;

  case EFI_IFR_DATE_OP:
  case EFI_IFR_TIME_OP:
    PromptStr = GetToken (Question->Prompt, Selection->Handle);
    if (Question->Operand == EFI_IFR_DATE_OP) {
      Status = UiDatePickerDialog (PromptStr, &(QuestionValue->Value.date));
    } else {
      Status = UiTimePickerDialog (PromptStr, &(QuestionValue->Value.time));
    }
    ScuSafeFreePool ((VOID **)&PromptStr);
    if (!EFI_ERROR (Status)) {

      Status = ValidateQuestion (Selection->FormSet, Selection->Form, Question, EFI_HII_EXPRESSION_INCONSISTENT_IF);
      if (EFI_ERROR (Status)) {
        //
        // Input value is not valid, restore Question Value
        //
        GetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
      } else {
        SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
        UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
      }
    }
    break;

  case EFI_IFR_STRING_OP:
    Maximum   = (UINT16) Question->Maximum;
    StringLen = StrLen (Sender->Wnd->szTitle);
    if (StringLen > Maximum) {
      StringLen = Maximum;
    }
    CopyMem (Question->BufferValue, Sender->Wnd->szTitle, sizeof (CHAR16) * StringLen);
    SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
    UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
    break;

  case EFI_IFR_PASSWORD_OP:

    Question  = Question;
    Maximum   = (UINT16) Question->Maximum;
    OldPassword = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
    NewPassword = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));

    //
    // We doen't know what non interactive password.
    // So we don't implement it
    //
    ASSERT (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK);

    //
    // Use a NULL password to test whether old password is required
    //
    OldPassword[0] = L'\0';
    Status = FBPasswordCallback (Selection, Question, OldPassword);
    if (Status == EFI_NOT_AVAILABLE_YET) {
      //
      // Callback request to terminate password input
      //
      gBS->FreePool (OldPassword);
      gBS->FreePool (NewPassword);
      return Status;
    }

    //
    //  Default is no password
    //
    PasswordStatus = ENTER_NEW_PASSWORD;
    if (EFI_ERROR (Status)) {
      if (Status != EFI_CRC_ERROR) {
        PasswordStatus = ENTER_OLD_PASSWORD;
      } else {
        PasswordStatus = UNLOCK_PASSWORD;
      }
    }

    Status = UiReadPassword (Selection, Question, PasswordStatus, TRUE, OldPassword, NewPassword);
    if (EFI_ERROR (Status)) {
      gBS->FreePool (OldPassword);
      gBS->FreePool (NewPassword);
      return Status;
    }

    if (PasswordStatus == UNLOCK_PASSWORD) {
      Status = FBPasswordCallback (Selection, Question, OldPassword);
      gBS->FreePool (OldPassword);
      gBS->FreePool (NewPassword);
      return EFI_SUCCESS;
    }

    if (PasswordStatus == ENTER_OLD_PASSWORD) {
      Status = FBPasswordCallback (Selection, Question, OldPassword);
      if (EFI_ERROR (Status)) {
        if (Status == EFI_NOT_READY) {
          //
          // EFI_NOT_READY mean old password is incorrect
          //
        } else {
          Status = EFI_SUCCESS;
        }
        gBS->FreePool (OldPassword);
        gBS->FreePool (NewPassword);
        return Status;
      }
    }

    ASSERT (PasswordStatus == ENTER_OLD_PASSWORD || PasswordStatus == ENTER_NEW_PASSWORD);

    //
    // set new password
    //
    Status = FBPasswordCallback (Selection, Question, NewPassword);
    if (Status == EFI_SUCCESS) {
      H2O_DIALOG_PROTOCOL            *H2ODialog;
      EFI_INPUT_KEY                  KeyValue;
      CHAR16                         *StrBuf;


      gBS->LocateProtocol (
             &gH2ODialogProtocolGuid,
             NULL,
             (VOID**)&H2ODialog
             );

      StrBuf = AllocatePool (0x1000);
      StrCpy (StrBuf, gSetupNotice);
      StrCat (StrBuf, L"\n");
      StrCat (StrBuf, gChangesSaved);
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
    }
    break;
  }

  return Status;
}

EFI_STATUS
FBProcessGotoOpCode (
  IN OUT   FORM_BROWSER_STATEMENT      *Statement,
  IN OUT   UI_MENU_SELECTION           *Selection
  )
{
  UI_MENU_LIST                   *MenuList;

  if (Statement->RefFormId != 0) {
    //
    // Goto another form inside this formset,
    //
    Selection->Action = UI_ACTION_REFRESH_FORM;

    //
    // Link current form so that we can always go back when someone hits the ESC
    //
    MenuList = UiFindMenuList (&Selection->FormSetGuid, Statement->RefFormId);
    if (MenuList == NULL) {
      MenuList = UiAddMenuList (Selection->CurrentMenu, &Selection->FormSetGuid, Statement->RefFormId);
    }

    Selection->FormId = Statement->RefFormId;
    Selection->QuestionId = Statement->RefQuestionId;

    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}


VOID
FBProcessUiNotify (
  FORM_BROWSER_FRAME  *This,
  UINT                Msg,
  UI_CONTROL           *Sender
  )
{
  EFI_STATUS Status;
  FORM_BROWSER_STATEMENT *Statement;
  UI_MANAGER                 *Manager;
  UI_MENU_SELECTION *Selection;
  UI_MENU_LIST                    *CurrentMenu;
  HWND                            HWnd;
  HWND                            FocusWnd;

  HWnd    = ((UI_CONTROL *)This)->Wnd;
  Manager = NULL;
  if (Sender != NULL) {
    Manager = Sender->Manager;
  }
  Selection = This->Selection;
  CurrentMenu = Selection->CurrentMenu;

  if (Msg == UI_NOTIFY_CLICK || Msg == UI_NOTIFY_CARRIAGE_RETURN) {
    if ((StrCmp (Sender->Name, L"menu_option") == 0) ||
        (StrCmp (Sender->Name, L"menu_prompt") == 0)) {

      Statement = (FORM_BROWSER_STATEMENT *)GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
      ASSERT (Statement != NULL);

      Selection->Statement = Statement;
      Selection->QuestionId = Statement->QuestionId;
      CurrentMenu->QuestionId = Statement->QuestionId;

      Status = EFI_SUCCESS;
      FocusWnd = GetFocus ();

      switch (Statement->Operand) {

      case EFI_IFR_REF_OP:
        Status = FBProcessGotoOpCode (Statement, Selection);
        break;

      case EFI_IFR_ACTION_OP:
        Status = ProcessQuestionConfig (Selection, Statement);
        if (!EFI_ERROR (Status)) {
          Selection->Action = UI_ACTION_REFRESH_FORM;
        }
        break;

      case EFI_IFR_RESET_BUTTON_OP:
        //
        // not yet
        //
        break;

      default:
        Status = FBProcessOptions (This, Selection, Statement, Sender);
        if (!EFI_ERROR (Status)) {
          Selection->Action = UI_ACTION_REFRESH_FORM;
        }
      }
      SetFocus (FocusWnd);
      PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
      return ;
    }

    if (Msg == UI_NOTIFY_CLICK) {
      if (StrCmp (Sender->Name, L"exit") == 0) {
        PostMessage (HWnd, FB_NOTIFY_RESET, 0, 0);
      } else if (StrCmp (Sender->Name, L"page") == 0) {

        ASSERT (gDeviceManagerSetup != NULL);
        if (gDeviceManagerSetup->CurRoot != (UINT8)GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA)) {
          if (Selection->Statement != NULL) {
            Selection->Statement->QuestionFlags = 0;
          }

          gDeviceManagerSetup->CurRoot   = (UINT8)GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
          gDeviceManagerSetup->Direction = Jump;
          //
          // set scroll bar position to zero
          //
          ((UI_CONTROL *)This->MenuOptionList)->SetAttribute ((UI_CONTROL *)This->MenuOptionList, L"vscrollbarpos", L"0");

          Selection->Action = UI_ACTION_REFRESH_FORMSET;
          PostMessage (HWnd, FB_NOTIFY_REFRESH, 0, 0);
        }
      } else if (StrCmp (Sender->Name, L"down") == 0) {
        PostMessage (GetFocus(), WM_KEYDOWN, VK_F5, 0);
      } else if (StrCmp (Sender->Name, L"up") == 0) {
        PostMessage (GetFocus(), WM_KEYDOWN, VK_F6, 0);
      }
    }
  }
}


EFI_STATUS
ProcessCallBackFunction (
  IN OUT UI_MENU_SELECTION               *Selection,
  IN     FORM_BROWSER_STATEMENT          *Question,
  IN     EFI_BROWSER_ACTION              Action,
  IN     BOOLEAN                         SkipSaveOrDiscard
  )
{
  EFI_STATUS                      Status;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_HII_VALUE                   *HiiValue;
  EFI_IFR_TYPE_VALUE              *TypeValue;
  FORM_BROWSER_STATEMENT          *Statement;
  BOOLEAN                         SubmitFormIsRequired;
  BOOLEAN                         DiscardFormIsRequired;
  BOOLEAN                         NeedExit;
  LIST_ENTRY                      *Link;
  UINT8                           Index;
  BOOLEAN                         IsHotKey;
//INT32                           BackupAttribute;


  ConfigAccess = Selection->FormSet->ConfigAccess;
  SubmitFormIsRequired  = FALSE;
  DiscardFormIsRequired = FALSE;
  NeedExit              = FALSE;
  Status                = EFI_SUCCESS;
  ActionRequest         = EFI_BROWSER_ACTION_REQUEST_NONE;

  if (ConfigAccess == NULL) {
    return EFI_SUCCESS;
  }

  IsHotKey = FALSE;
  for (Index = 0; Index < sizeof (gUiHotkeyToQuestionId) / sizeof (gUiHotkeyToQuestionId[0]); Index++) {
    if (Selection->Action == gUiHotkeyToQuestionId[Index].Hotkey) {
      IsHotKey = TRUE;
      break;
    }
  }


  Link = GetFirstNode (&Selection->Form->StatementListHead);
  while (!IsNull (&Selection->Form->StatementListHead, Link)) {
    Statement = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    Link = GetNextNode (&Selection->Form->StatementListHead, Link);
    if (IsHotKey) {
      Statement = Question;
    }
    //
    // if Question != NULL, only process the question. Else, process all question in this form.
    //
    if ((Question != NULL) && (Statement != Question)) {
      continue;
    }
    if ((Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != EFI_IFR_FLAG_CALLBACK) {
      continue;
    }

    HiiValue = &Statement->HiiValue;
    TypeValue = &HiiValue->Value;

    if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
      //
      // Create String in HII database for Configuration Driver to retrieve
      //
      HiiValue->Value.string = NewString ((CHAR16 *) Statement->BufferValue, Selection->FormSet->HiiHandle);
    }
    //
    // Set attribute to subtitle attribute to prevent from using wrong attribute to clear popup screen.
    //
//  BackupAttribute = gST->ConOut->Mode->Attribute;
//  gST->ConOut->SetAttribute (gST->ConOut, SUBTITLE_TEXT | SUBTITLE_BACKGROUND);
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    Status = ConfigAccess->Callback (
                             ConfigAccess,
                             Action,
                             Statement->QuestionId,
                             HiiValue->Type,
                             TypeValue,
                             &ActionRequest
                             );
//  gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
    if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
      //
      // Clean the String in HII Database
      //
      DeleteString (HiiValue->Value.string, Selection->FormSet->HiiHandle);
    }
    if (!EFI_ERROR (Status)) {
      //
      // Only for EFI_BROWSER_ACTION_CHANGED need to handle this ActionRequest.
      //
//    if (Action == EFI_BROWSER_ACTION_CHANGED) {
        switch (ActionRequest) {
        case EFI_BROWSER_ACTION_REQUEST_RESET:
          gResetRequired = TRUE;
          Selection->Action = UI_ACTION_EXIT;
          gBS->RaiseTPL (TPL_NOTIFY);
          gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
          break;

        case EFI_BROWSER_ACTION_REQUEST_SUBMIT:
          SubmitFormIsRequired = TRUE;
//        Selection->Action = UI_ACTION_EXIT;
          break;

        case EFI_BROWSER_ACTION_REQUEST_EXIT:
          Selection->Action = UI_ACTION_EXIT;
          gExitRequired = TRUE;
          gNvUpdateRequired = FALSE;
          break;

        case EFI_BROWSER_ACTION_REQUEST_FORM_SUBMIT_EXIT:
          SubmitFormIsRequired  = TRUE;
          NeedExit              = TRUE;
          break;

        case EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT:
          DiscardFormIsRequired = TRUE;
          NeedExit              = TRUE;
          break;

        case EFI_BROWSER_ACTION_REQUEST_FORM_APPLY:
          SubmitFormIsRequired  = TRUE;
          break;

        case EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD:
          DiscardFormIsRequired = TRUE;
          break;

        default:
          break;
        }
    } else {
      //
      // According the spec, return fail from call back of "changing" and
      // "retrieve", should restore the question's value.
      //
      if (Status == EFI_UNSUPPORTED) {
        //
        // If return EFI_UNSUPPORTED, also consider Hii driver suceess deal with it.
        //
        Status = EFI_SUCCESS;
      }
    }

    if (IsHotKey) {
      break;
    }
  }
  if (SubmitFormIsRequired && !SkipSaveOrDiscard) {
    SubmitForm (Selection->FormSet, Selection->Form/*, SettingLevel*/);
  }

  return Status;
}


LRESULT
CALLBACK
FormBrowserProc (
  HWND HWnd,
  UINT message,
  WPARAM WParam,
  LPARAM LParam
  )
{
  FORM_BROWSER_FRAME              *This;
  UI_MANAGER                      *Manager;
  UI_CONTROL                      *Control;
  EFI_STATUS                      Status;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  UI_MENU_SELECTION               *Selection;
  UI_MENU_LIST                    *CurrentMenu;
  FORM_BROWSER_STATEMENT          *Statement;
  FORM_BROWSER_STATEMENT          HotkeyStatement;
  UINTN                           Index;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;

  This    = (FORM_BROWSER_FRAME *) GetWindowLongPtr (HWnd, 0);
  Control = (UI_CONTROL *) This;

  Selection = NULL;
  Manager = NULL;
  CurrentMenu = NULL;
  if (Control != NULL) {
    Manager = Control->Manager;
    Selection = This->Selection;
    CurrentMenu = Selection->CurrentMenu;
  }

  switch ( message )
  {

  case WM_CREATE:
    This = (FORM_BROWSER_FRAME *) AllocateZeroPool (sizeof (FORM_BROWSER_FRAME));
    This->Selection = ((CREATESTRUCT *)(LParam))->lpCreateParams;
    SetWindowLongPtr (HWnd, 0, (INTN)This);
    SendMessage (HWnd, UI_NOTIFY_CREATE, WParam, LParam);
    break;

  case UI_NOTIFY_CREATE:
    UiFrameProc (HWnd, UI_NOTIFY_CREATE, WParam, LParam);

    //
    // create UiContainer as root
    //
    Manager = Control->Manager;
    if (CompareGuid (&Selection->FormSetGuid, &gFrontPageFormsetGuid)) {
      UiDlgBuilderCreate (mFrontPageXml, Manager, Manager->Root);
    } else {
      UiDlgBuilderCreate (mScuXml, Manager, Manager->Root);
    }

    //
    // process
    //
    FBSetComponent (This);
    Selection->Action = UI_ACTION_REFRESH_FORMSET;
    PostMessage (HWnd, FB_NOTIFY_REFRESH, 0, 0);
    MoveWindow (HWnd, 0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN), TRUE);
    break;

  case FB_NOTIFY_REFRESH:

    if (This->PackageNotifyHandle == NULL) {
      Status = gHiiDatabase->RegisterPackageNotify (
                               gHiiDatabase,
                               EFI_HII_PACKAGE_FORMS,
                               NULL,
                               FormUpdateNotify,
                               EFI_HII_DATABASE_NOTIFY_REMOVE_PACK,
                               &This->PackageNotifyHandle
                               );
      if (EFI_ERROR (Status)) {
        This->PackageNotifyHandle = NULL;
        break;
      }
    }

    if (mHiiPackageListUpdated) {
      Selection->Action = UI_ACTION_REFRESH_FORMSET;
      mHiiPackageListUpdated = FALSE;
    }

    if (Selection->Action == UI_ACTION_EXIT) {
      SendMessage (HWnd, WM_CLOSE, 0, 0);
      PostQuitMessage (0);
    } else {
      if (Selection->Action == UI_ACTION_REFRESH_FORMSET) {
        Status = FBRefreshFormSet (Selection);
        if (EFI_ERROR (Status)) {
          SendMessage (HWnd, WM_CLOSE, 0, 0);
          PostQuitMessage (0);
          break;
        }
      }

      Status = FBRefreshFormData   (Selection);
      if (EFI_ERROR (Status)) {
        SendMessage (HWnd, WM_CLOSE, 0, 0);
        PostQuitMessage (0);
        break;
      }

      PostMessage (HWnd, FB_NOTIFY_REPAINT, 0, 0);
    }
    break;

  case FB_NOTIFY_REPAINT:
    Selection->Action = UI_ACTION_NONE;
    FBRefreshSetupMenuList (This);
    FBRefreshMenuOptionList (This);
    if (gDeviceManagerSetup != NULL) {
      gDeviceManagerSetup->Firstin = FALSE;
    }
    break;

  case FB_NOTIFY_CHECK_SELECTION:
    ASSERT (Selection->FormSet != NULL);

    if (Selection->Action == UI_ACTION_NONE) {
      PostMessage (HWnd, FB_NOTIFY_REPAINT, 0, 0);
      break;
    }

    //
    // Stop refresh timer, it maybe move to ui notify click
    //
    KillTimer (HWnd, FB_REFRSH_QUESTION_TIMER_ID);

    //
    // Process callback function
    //
    Statement = Selection->Statement;
    ConfigAccess = Selection->FormSet->ConfigAccess;
    if (ConfigAccess && (Statement != NULL || (Selection->NoMenuOption))) {
      for (Index = 0; Index < sizeof (gUiHotkeyToQuestionId) / sizeof (gUiHotkeyToQuestionId[0]); Index++) {
         if (Selection->Action == gUiHotkeyToQuestionId[Index].Hotkey) {
           ZeroMem (&HotkeyStatement, sizeof (FORM_BROWSER_STATEMENT));
           HotkeyStatement.QuestionFlags = EFI_IFR_FLAG_CALLBACK;
           HotkeyStatement.QuestionId    = gUiHotkeyToQuestionId[Index].QuestionId;
           Statement = &HotkeyStatement;
           break;
         }
       }
       if ((Statement != NULL) &&
             (Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK) &&
             (Statement->Operand != EFI_IFR_PASSWORD_OP)) {
           ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

         EfiCreateEventReadyToBootEx (TPL_CALLBACK, FBReadyToBootCallback, NULL, &mReadyToBootEvent);
         Status = ProcessCallBackFunction (Selection, Statement, EFI_BROWSER_ACTION_CHANGING, FALSE);
         if (mReadyToBootEvent == NULL) {
           GdOpenMouse ();
         } else {
           gBS->CloseEvent (mReadyToBootEvent);
           mReadyToBootEvent = NULL;
         }

         if (Status != EFI_SUCCESS && Status != EFI_UNSUPPORTED && Selection->Action != UI_ACTION_EXIT) {
           //
           // Callback return error status other than EFI_UNSUPPORTED
           //
           if (Statement->Operand == EFI_IFR_REF_OP) {
             //
             // Cross reference will not be taken
             //
             Selection->FormId = Selection->Form->FormId;
           }
         }
       }

       //
       // Check whether Form Package has been updated during Callback
       //
       if (mHiiPackageListUpdated && (Selection->Action == UI_ACTION_REFRESH_FORM)) {
         //
         // Force to reparse IFR binary of target Formset
         //
         Selection->Action = UI_ACTION_REFRESH_FORMSET;
         mHiiPackageListUpdated = FALSE;
       }
    }
    if (Selection->Action != UI_ACTION_EXIT && Statement == &HotkeyStatement) {
      Selection->Action = UI_ACTION_REFRESH_FORM;
    }
    PostMessage (HWnd, FB_NOTIFY_REFRESH, 0, 0);
    break;

  case UI_NOTIFY_CLICK:
  case UI_NOTIFY_CARRIAGE_RETURN:
    FBProcessUiNotify (This, message, (UI_CONTROL *)WParam);
    break;

  case FB_NOTIFY_SAVE_AND_EXIT:
    if (gDeviceManagerSetup != NULL || gSecureBootMgr) {
      Selection->Action = UI_HOTKEY_F10;
      PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
    }
    break;

  case FB_NOTIFY_LOAD_DEFAULT:
    if (gDeviceManagerSetup) {
      Selection->Action = UI_HOTKEY_F9;
    } else {
      if (!Selection->FormEditable) {
        //
        // This Form is not editable, ignore the F9 (reset to default)
        //
        break;
      }

      Status = ExtractFormDefault (Selection->FormSet, Selection->Form, EFI_HII_DEFAULT_CLASS_STANDARD);

      if (!EFI_ERROR (Status)) {
        Selection->Action = UI_ACTION_REFRESH_FORM;
        Selection->Statement = NULL;

        //
        // Show NV update flag on status bar
        //
        gNvUpdateRequired = TRUE;
      }
    }
    PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
    break;

  case FB_NOTIFY_RESET:

    if (mFrontPage) {
      Selection->Action = UI_ACTION_NONE;
      break;
    }

    if (gDeviceManagerSetup == NULL && !gSecureBootMgr) {
      if (gNvUpdateRequired) {

        //
        // Bugbug: add a check for "Are you exit without save ?"
        //
        Status = SubmitForm (Selection->FormSet, Selection->Form);
      }
    }

    if (CurrentMenu->Parent != NULL) {
       //
       // we have a parent, so go to the parent menu
       //
       if (CompareGuid (&CurrentMenu->FormSetGuid, &CurrentMenu->Parent->FormSetGuid)) {
         //
         // The parent menu and current menu are in the same formset
         //
         Selection->Action = UI_ACTION_REFRESH_FORM;
       } else {
         Selection->Action = UI_ACTION_REFRESH_FORMSET;
       }
       Selection->Statement = NULL;

       Selection->FormId = CurrentMenu->Parent->FormId;
       Selection->QuestionId = CurrentMenu->Parent->QuestionId;
       Selection->CurrentRow = 0;

       //
       // Clear highlight record for this menu
       //
       CurrentMenu->QuestionId = 0;
       PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
       break;
    }

    if (gDeviceManagerSetup != NULL) {
      Selection->Action = UI_HOTKEY_ESC;
      PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
      break;
    }

    Selection->Action = UI_ACTION_EXIT;
    if (gDeviceManagerSetup == NULL && !gSecureBootMgr) {
      Selection->Statement = NULL;
      CurrentMenu->QuestionId = 0;
      PostMessage (HWnd, FB_NOTIFY_REFRESH, 0, 0);
      break;
    }

    Selection->Action = UI_HOTKEY_ESC;
    PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
    break;



  case WM_TIMER:
    if (WParam == FB_REFRSH_QUESTION_TIMER_ID) {
      UINTN                 Index;
      EFI_STATUS            Status;
      EFI_STRING            OptionString;
      CHAR16                Str[100];
      UI_MENU_OPTION        FakeMenuOption;

      for (Index = 0; Index < This->RefreshMenuCount; Index++) {
        Statement = (FORM_BROWSER_STATEMENT *)GetWindowLongPtr (This->RefreshMenuList[Index]->Wnd, GWLP_USERDATA);
        Status = GetQuestionValue (Selection->FormSet, Selection->Form, Statement, FALSE);
        if (!EFI_ERROR (Status)) {
          OptionString = NULL;
          FakeMenuOption.ThisTag = Statement;
          ProcessOptions (Selection, &FakeMenuOption, FALSE, &OptionString);
          if (OptionString != NULL) {
            GetWindowText (This->RefreshMenuList[Index]->Wnd, Str, sizeof (Str)/sizeof (CHAR16));
            if (StrCmp (Str, OptionString) != 0) {
              This->RefreshMenuList[Index]->SetAttribute (This->RefreshMenuList[Index], L"text", OptionString);
            }
          }
          ScuSafeFreePool ((VOID **)&OptionString);
        }
      }
    }
    break;

  case WM_SETFOCUS:
    break;

  case WM_DESTROY:
    if (This->PackageNotifyHandle != NULL) {
      Status = gHiiDatabase->UnregisterPackageNotify (
                               gHiiDatabase,
                               This->PackageNotifyHandle
                               );
    }
    ScuSafeFreePool ((VOID**)&This->RefreshMenuList);
    UiFrameProc (HWnd,message, WParam,LParam);
    break;

  case WM_CLOSE:
    DestroyWindow(HWnd);
    gExitRequired = TRUE;
    break;

  default:
    return UiFrameProc (HWnd,message, WParam,LParam);
  }
  return 0;
}


BOOLEAN
FBProcessKey (
  HWND          HWnd,
  MSG           *Msg
  )
{
  FORM_BROWSER_FRAME              *This;
  UINTN                           Index;
  HWND                            FocusWnd;
  UI_MENU_SELECTION               *Selection;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  FORM_BROWSER_STATEMENT          *Statement;
  EFI_STATUS                      Status;

  if (gExitRequired) {
    return FALSE;
  }

  This = (FORM_BROWSER_FRAME *) GetWindowLongPtr (HWnd, 0);
  Selection = This->Selection;

  if (Msg->message == WM_RBUTTONDOWN) {
    PostMessage (HWnd, WM_KEYDOWN, VK_ESCAPE, 0);
    return TRUE;
  }

  if (Msg->message != WM_KEYDOWN) {
    return FALSE;
  }

  switch (Msg->wParam) {

  case VK_LEFT:
    if (gDeviceManagerSetup != NULL) {
      FocusWnd = GetFocus();
      if (FocusWnd != NULL && StrCmp (GetUiControl (FocusWnd)->Name, L"page") != 0 && This->SetupMenuList != NULL) {
       for (Index = 0; Index < This->SetupMenuList->ItemCount; Index++) {
          if (gDeviceManagerSetup->CurRoot ==
              (UINT8)GetWindowLongPtr (This->SetupMenuList->Items[Index]->Wnd, GWLP_USERDATA)) {
            SetFocus (This->SetupMenuList->Items[Index]->Wnd);
            return TRUE;
          }
        }
      }
    }
    return FALSE;

  case VK_RIGHT:
    if (gDeviceManagerSetup != NULL) {
      FocusWnd = GetFocus ();
      if (FocusWnd != NULL && StrCmp (GetUiControl (FocusWnd)->Name, L"page") == 0 && This->MenuOptionList != NULL) {
        for (Index = 0; Index < This->MenuOptionList->ItemCount; Index++) {
          if (!(GetWindowLongPtr (This->MenuOptionList->Items[Index]->Wnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
            SetFocus (This->MenuOptionList->Items[Index]->Wnd);
            return TRUE;
          }
        }
      }
    }
    return FALSE;

  case VK_F5:
  case VK_F6:
    if (gDeviceManagerSetup != NULL) {
      FocusWnd = GetFocus ();
      if (FocusWnd != NULL && StrCmp (GetUiControl (FocusWnd)->Name, L"menu_prompt") != 0) {
        return FALSE;
      }

      Statement = (FORM_BROWSER_STATEMENT *) GetWindowLongPtr (FocusWnd, GWLP_USERDATA);
      if ((Statement->Operand == EFI_IFR_ACTION_OP) &&
          (Statement->QuestionFlags == EFI_IFR_CHECKBOX_DEFAULT_MFG)) {

        EFI_IFR_TYPE_VALUE         Value;
        EFI_BROWSER_ACTION_REQUEST DummyActionRequest;

        ConfigAccess = Selection->FormSet->ConfigAccess;

        Value.u32 = Statement->QuestionId;
        Status = ConfigAccess->Callback(
                                  ConfigAccess,
                                  EFI_BROWSER_ACTION_CHANGING,
                                  (Msg->wParam == VK_F5) ? KEY_DOWN_SHIFT : KEY_UP_SHIFT,
                                  EFI_IFR_TYPE_NUM_SIZE_32,
                                  &Value,
                                  &DummyActionRequest
                                  );
        if (!EFI_ERROR (Status)) {
          if (Msg->wParam == VK_F5) {
            Statement = FORM_BROWSER_STATEMENT_FROM_LINK (Statement->Link.ForwardLink);
            Selection->CurrentMenu->QuestionId = Statement->QuestionId;
          } else {
            Statement = FORM_BROWSER_STATEMENT_FROM_LINK (Statement->Link.BackLink);
            Selection->CurrentMenu->QuestionId = Statement->QuestionId;
          }
          Status = ProcessQuestionConfig (Selection, Statement);
          if (!EFI_ERROR (Status)) {
            Selection->Action = UI_ACTION_REFRESH_FORM;
          }
          PostMessage (HWnd, FB_NOTIFY_CHECK_SELECTION, 0, 0);
        }
      }
    }
    return FALSE;

  case VK_ESCAPE:
    PostMessage (HWnd, FB_NOTIFY_RESET, 0, 0);
    break;

  case VK_F9:
    PostMessage (HWnd, FB_NOTIFY_LOAD_DEFAULT, 0, 0);
    break;

  case VK_F10:
    PostMessage (HWnd, FB_NOTIFY_SAVE_AND_EXIT, 0, 0);
    break;

  default:
    return FALSE;
  }

  return TRUE;

}



EFI_STATUS
FBDisplayMenu (
  IN OUT UI_MENU_SELECTION    *Selection
  )
{
  HWND          HWnd;
  MSG           Msg;
  EFI_STATUS    Status;

  Status = InitializeGUI ();
  ASSERT_EFI_ERROR (Status);

  GdOpenMouse ();
  InitializeBrowserStrings ();

  HWnd = CreateWindowEx (
           WS_EX_NOACTIVATE,
           FORM_BROWSER_CLASS_NAME,
           L"Insyde H2O",
           WS_OVERLAPPED | WS_POPUP | WS_VISIBLE,
           0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN),
           NULL,
           NULL,
           NULL,
           Selection
           );
  //
  // Process Message
  //
  while (GetMessage(&Msg,NULL,0,0)) {
    if (!FBProcessKey (HWnd, &Msg)) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
  }

  //
  // Remove all message (desktop WM_PAINT)
  //
  while (PeekMessage (&Msg, NULL, 0, 0, PM_REMOVE)) {
  }

  FreeBrowserStrings ();
  GdCloseMouse ();

  return EFI_SUCCESS;
}


EFI_STATUS
FBSendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid, OPTIONAL
  IN  UINT16                           FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  )
{
  UI_MENU_SELECTION               *Selection;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST      DummyActionRequest;
  EFI_STATUS                      Status;

  mAlreadySendForm = TRUE;
  //
  // There should be only one instance in the system
  // of the DeviceManagerSetup protocol, installed by
  // the BDS before sending the SetupUtilutyBrowser form
  //
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID**)&gDeviceManagerSetup
                  );

  if (EFI_ERROR (Status)) {
    gDeviceManagerSetup = NULL;
  }

  Status = gBS->LocateProtocol (
                  &gSetupMouseProtocolGuid,
                  NULL,
                  (VOID**) &mSetupMouse
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // BugBug, gScreenDimensions for ProcessOptions allocate buffer
  // but we will ignore gScreenDimensions
  //
  ZeroMem (&gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  Selection = AllocateZeroPool (sizeof (UI_MENU_SELECTION));

  if (gDeviceManagerSetup != NULL) {
    gDeviceManagerSetup->Firstin = FALSE;
  }

  if (gDeviceManagerSetup != NULL) {
    Selection->Handle = gDeviceManagerSetup->MenuList[gDeviceManagerSetup->CurRoot].Page;
  } else {
    Selection->Handle = Handles[0];
  }

  if (FormSetGuid != NULL) {
    CopyMem (&mInputFormSetGuid     , FormSetGuid, sizeof (EFI_GUID));
    CopyMem (&Selection->FormSetGuid, FormSetGuid, sizeof (EFI_GUID));
    Selection->FormId = FormId;
  }

  gExitRequired     = FALSE;
  gNvUpdateRequired = FALSE;
  gCurrentSelection = Selection;

  FBDisplayMenu (Selection);

  ScuSafeFreePool ((VOID **)&gAnimationPackage);

  ConfigAccess = Selection->FormSet->ConfigAccess;
  if(gDeviceManagerSetup) {
    ConfigAccess->Callback(ConfigAccess, EFI_BROWSER_ACTION_CHANGING, GET_SETUP_CONFIG, 0, NULL, &DummyActionRequest);
  }

  if (Selection->FormSet != NULL) {
    DestroyFormSet (Selection->FormSet);
  }

  ScuSafeFreePool ((VOID **)&Selection);

  if (ActionRequest != NULL) {
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    if (gResetRequired) {
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
    } else if (gExitRequired) {
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;

    }
  }

  mAlreadySendForm = FALSE;
  return EFI_SUCCESS;
}


EFI_STATUS
InitializeGraphicsUi (
  IN EFI_HANDLE           ImageHandle
  )
{
  EFI_FORM_BROWSER2_PROTOCOL      *FormBrowser2;
  H2O_DIALOG_PROTOCOL             *H2ODialog;
  EFI_STATUS                      Status;

  Status = gBS->LocateProtocol (
                  &gEfiFormBrowser2ProtocolGuid,
                  NULL,
                  (VOID**)&FormBrowser2
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID**)&H2ODialog
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gH2OWindowProtocolGuid,
                  NULL,
                  (VOID **) &gH2OWindow
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FormBrowser2->SendForm     = FBSendForm;
  H2ODialog->ConfirmDialog   = UiConfirmDialog;
  H2ODialog->PasswordDialog  = UiPasswordDialog;
  H2ODialog->CreateMsgPopUp  = UiCreateMsgPopUp;
  H2ODialog->ShowPageInfo    = UiShowPageInfo;

  //
  // initial XML setting
  //
  mFrontPageXml         = GetToken (STRING_TOKEN (FRONT_PAGE_XML), gHiiHandle);
  mScuXml               = GetToken (STRING_TOKEN (SCU_XML), gHiiHandle);
  mSetupMenuSelectXml   = GetToken (STRING_TOKEN (SETUP_MENU_SELECT_XML),   gHiiHandle);
  mSetupMenuUnSelectXml = GetToken (STRING_TOKEN (SETUP_MENU_UNSELECT_XML), gHiiHandle);
  mFrontPagePromptXml   = GetToken (STRING_TOKEN (FRONT_PAGE_PROMPT_XML), gHiiHandle);
  mFrontPagePromptWithAniXml = GetToken (STRING_TOKEN (FRONT_PAGE_PROMPT_WITH_ANI_XML), gHiiHandle);

  ASSERT (mFrontPageXml != NULL);
  ASSERT (mScuXml != NULL);
  ASSERT (mSetupMenuSelectXml != NULL);
  ASSERT (mSetupMenuUnSelectXml != NULL);
  ASSERT (mFrontPagePromptXml != NULL);
  ASSERT (mFrontPagePromptWithAniXml != NULL);

  InitializeGraphicsDialogXML ();

  return EFI_SUCCESS;
}

