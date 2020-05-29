/** @file

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

#include <H2ODisplayEngineLib.h>
#include <DESpecificQuestionBehavior.h>
#include "DEControl.h"

STATIC
UINT32
SetStyleTypeByOpCode (
  IN     UINT32                               OpCode
  )
{
  UINT32                                      StyleType;

  switch (OpCode) {

  case EFI_IFR_REF_OP:
    StyleType = H2O_IFR_STYLE_TYPE_GOTO;
    break;

  case EFI_IFR_ACTION_OP:
    StyleType = H2O_IFR_STYLE_TYPE_ACTION;
    break;

  case EFI_IFR_RESET_BUTTON_OP:
    StyleType = H2O_IFR_STYLE_TYPE_RESETBUTTON;
    break;

  case EFI_IFR_CHECKBOX_OP:
    StyleType = H2O_IFR_STYLE_TYPE_CHECKBOX;
    break;

  case EFI_IFR_NUMERIC_OP:
    StyleType = H2O_IFR_STYLE_TYPE_NUMERIC;
    break;

  case EFI_IFR_ONE_OF_OP:
    StyleType = H2O_IFR_STYLE_TYPE_ONEOF;
    break;

  case EFI_IFR_ORDERED_LIST_OP:
    StyleType = H2O_IFR_STYLE_TYPE_ORDEREDLIST;
    break;

  case EFI_IFR_PASSWORD_OP:
    StyleType = H2O_IFR_STYLE_TYPE_PASSWORD;
    break;

  case EFI_IFR_STRING_OP:
    StyleType = H2O_IFR_STYLE_TYPE_STRING;
    break;

  case EFI_IFR_DATE_OP:
    StyleType = H2O_IFR_STYLE_TYPE_DATE;
    break;

  case EFI_IFR_TIME_OP:
    StyleType = H2O_IFR_STYLE_TYPE_TIME;
    break;

  case EFI_IFR_SUBTITLE_OP:
    StyleType = H2O_IFR_STYLE_TYPE_SUBTITLE;
    break;

  case EFI_IFR_TEXT_OP:
    StyleType = H2O_IFR_STYLE_TYPE_TEXT;
    break;

  default:
    StyleType = 0;
  }

  return StyleType;
}

BOOLEAN
IsRootForm (
  VOID
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PROTOCOL                   *FBProtocol;
  H2O_FORM_BROWSER_SM                         *SetupMenuData;
  BOOLEAN                                     Result;
  UINT32                                      Index;

  FBProtocol = mDEPrivate->FBProtocol;
  Result = FALSE;

  Status = FBProtocol->GetSMInfo (FBProtocol, &SetupMenuData);
  if (EFI_ERROR (Status)) {
    return Result;
  }

  if (SetupMenuData->NumberOfSetupMenus == 0) {
    FreeSetupMenuData (SetupMenuData);
    return Result;
  }

  Index = FBProtocol->CurrentP->PageId >> 16;
  if (FBProtocol->CurrentP->PageId == SetupMenuData->SetupMenuInfoList[Index].PageId) {
    Result = TRUE;
  }

  FreeSetupMenuData (SetupMenuData);

  return Result;
}

BOOLEAN
IsHotKey (
  IN     BOOLEAN                              Keyboard,
  IN     EFI_KEY_DATA                         *KeyData
  )
{
  EFI_STATUS                                     Status;
  H2O_FORM_BROWSER_PROTOCOL                      *FBProtocol;
  H2O_FORM_BROWSER_P                             *Page;

  HOT_KEY_INFO                                   *HotKey;
  UINT32                                         Index;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Check Form Set hot keys
  //
  Page = FBProtocol->CurrentP;
  HotKey = Page->HotKeyInfo;

  Index = 0;
  if (HotKey != NULL) {
    while (HotKey[Index].String != NULL) {
      if (
        (HotKey[Index].KeyData.Key.ScanCode == KeyData->Key.ScanCode) &&
        (HotKey[Index].KeyData.Key.UnicodeChar == KeyData->Key.UnicodeChar)) {
        return TRUE;
      }
      Index ++;
    }
  }

  return FALSE;
}

BOOLEAN
IsValidHighlightStatement (
  IN H2O_CONTROL_INFO                            *StatementControl
  )
{
  if (StatementControl == NULL) {
    return FALSE;
  }

  if (StatementControl->Selectable) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
SendEvtByHotKey (
  IN     BOOLEAN                              Keyboard,
  IN     EFI_KEY_DATA                         *KeyData
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PROTOCOL                   *FBProtocol;
  HOT_KEY_INFO                                *HotKey;
  UINT32                                      Index;
  BOOLEAN                                     IsIncrease = TRUE;
  EFI_INPUT_KEY                               NewKey;
  EFI_HII_VALUE                               HiiValue;
  H2O_CONTROL_INFO                            *MenuSelected;
  H2O_CONTROL_INFO                            *PageTagSelected;
  H2O_CONTROL_INFO                            *TempPageTagSelected;
  LIST_ENTRY                                  *PanelLink;
  H2O_PANEL_INFO                              *SetupMenuPanel;
  H2O_PANEL_INFO                              *SetupPagePanel;
  H2O_EVT_TYPE                                SendEvtType;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  SetupMenuPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_MENU);
  SetupPagePanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_PAGE);

  //
  // 1. Get event type
  // 2. Set increase or decrease for some event (first is decrease, second is increase)
  // Bugbug:
  //
  Index       = 0;
  SendEvtType = 0;
  HotKey      = (FBProtocol->CurrentP != NULL) ? FBProtocol->CurrentP->HotKeyInfo : NULL;
  if (HotKey != NULL) {
    while (HotKey[Index].String != NULL) {
      if ((HotKey[Index].KeyData.Key.ScanCode    == KeyData->Key.ScanCode) &&
          (HotKey[Index].KeyData.Key.UnicodeChar == KeyData->Key.UnicodeChar)) {
        SendEvtType = HotKey[Index].SendEvtType;
        IsIncrease = TRUE;
      } else if (SendEvtType == HotKey[Index].SendEvtType) {
        IsIncrease = FALSE;
      }
      Index++;
    }
  }
  if (SendEvtType == 0) {
    return EFI_NOT_FOUND;
  }

  switch (SendEvtType) {

    case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q:
      if (SetupMenuPanel == NULL || mDEPrivate->MenuSelected == NULL || SetupPagePanel == NULL) {
        break;
      }
      MenuSelected = FindNextSelectableControl (
                       SetupPagePanel->ControlList.ControlArray,
                       SetupPagePanel->ControlList.Count / 2,
                       mDEPrivate->MenuSelected,
                       IsIncrease,
                       FALSE
                       );
      if (MenuSelected != NULL && IsValidHighlightStatement (MenuSelected)) {
        mDEPrivate->MenuSelected = MenuSelected;
        SendSelectQNotify (
          MenuSelected->PageId,
          MenuSelected->QuestionId,
          MenuSelected->IfrOpCode
          );
      }
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P:
      if (SetupPagePanel == NULL || SetupMenuPanel == NULL) {
        break;
      }

      if (!IsRootForm ()) {
        //
        // Do nothing when it is not in root Form.
        //
        return EFI_SUCCESS;
      }

      NewKey.UnicodeChar  = CHAR_NULL;
      NewKey.ScanCode     = IsIncrease ? SCAN_RIGHT : SCAN_LEFT;
      TempPageTagSelected = mDEPrivate->PageTagSelected;

      while (TRUE) {
        PageTagSelected = TempPageTagSelected;
        CheckPressControls (Keyboard, &NewKey, 0, 0, SetupMenuPanel->ControlList.Count, SetupMenuPanel->ControlList.ControlArray, &PageTagSelected);
        if (PageTagSelected->Selectable) {
          mDEPrivate->PageTagSelected = PageTagSelected;
          SendSelectPNotify (PageTagSelected->PageId);
          break;
        }

        if ((UINTN)(UINTN *) PageTagSelected == (UINTN)(UINTN*) &SetupPagePanel->ControlList.ControlArray[0] ||
            (UINTN)(UINTN *) PageTagSelected == sizeof (H2O_CONTROL_INFO) * (SetupPagePanel->ControlList.Count - 1) + (UINTN)(UINTN *) &SetupPagePanel->ControlList.ControlArray[0]
            ) {
          //
          // If
          // 1.It is a "keyboard select" and select first page ||
          // 2.It is a "keyboard select" and select end page ||
          // Then, do not search the next selectable page.
          //
          break;
        }
        TempPageTagSelected = PageTagSelected;
      }
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q:
      Status = ChangeQuestionValueByStep (IsIncrease, FBProtocol->CurrentQ, &HiiValue);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      SendChangeQNotify (
        FBProtocol->CurrentQ->PageId,
        FBProtocol->CurrentQ->QuestionId,
        &HiiValue
        );
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT:
      SendDefaultNotify ();
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT:
      SendSubmitExitNotify ();
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT:
      SendDiscardExitNotify ();
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT:
      SendSubmitNotify ();
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD:
      SendDiscardNotify ();
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP:
      SendShowHelpNotify ();
      break;

    case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_Q:
      if (FBProtocol->CurrentQ != NULL) {
        SendOpenQNotify (
          FBProtocol->CurrentQ->PageId,
          FBProtocol->CurrentQ->QuestionId,
          FBProtocol->CurrentQ->IfrOpCode
          );
      }
      break;

    default:
      return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitLayout (
  VOID
  )
{
  EFI_STATUS                                 Status;


  Status = ProcessLayout ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitSetupMenu (
  VOID
  )
{
  EFI_STATUS                                     Status;
  H2O_FORM_BROWSER_PROTOCOL                      *FBProtocol;
  LIST_ENTRY                                     *PanelLink;
  UINT32                                         Index;
  H2O_FORM_BROWSER_SM                            *SetupMenuData;
  UINT32                                         SetupMenuCount;
  H2O_PANEL_INFO                                 *SetupMenuPanel;
  SETUP_MENU_INFO                                *SetupMenuInfoList;
  H2O_CONTROL_INFO                               *Control;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  SetupMenuPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_MENU);
  if (SetupMenuPanel == NULL) {
    return EFI_SUCCESS;
  }
  if (!IsVisibility (SetupMenuPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) {
    return EFI_SUCCESS;
  }
  //
  // Get setup menu info
  //
  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = FBProtocol->GetSMInfo (FBProtocol, &SetupMenuData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetupMenuCount    = SetupMenuData->NumberOfSetupMenus;
  SetupMenuInfoList = SetupMenuData->SetupMenuInfoList;
  if (SetupMenuCount == 0 || SetupMenuInfoList == NULL) {
    FreeSetupMenuData (SetupMenuData);
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init setup menu
  //
  FreeControlList (&SetupMenuPanel->ControlList);
  SetupMenuPanel->ControlList.Count        = SetupMenuCount;
  SetupMenuPanel->ControlList.ControlArray = (H2O_CONTROL_INFO *) AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * SetupMenuCount);
  if (SetupMenuPanel->ControlList.ControlArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < SetupMenuCount; Index++) {
    Control                           = &SetupMenuPanel->ControlList.ControlArray[Index];
    Control->Visible                  = IsRootForm ();
    Control->Text.String              = CatSPrint (NULL, L"%s%s%s", L" ", SetupMenuInfoList[Index].PageTitle, L" ");
    Control->PageId                   = SetupMenuInfoList[Index].PageId;
    Control->Selectable               = TRUE;
    Control->ParentPanel              = SetupMenuPanel;
    Control->ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_SELECTABLE;

    if (SetupMenuInfoList[Index].PageImage != NULL) {
      CopyMem (&Control->HelpImage, SetupMenuInfoList[Index].PageImage, sizeof (EFI_IMAGE_INPUT));
    }
  }

  FreeSetupMenuData (SetupMenuData);

  return EFI_SUCCESS;
}


/**
 Initialize hotkey bar data

 @param[in] HotKey               Pointer to hot key information

 @retval EFI_SUCCESS             Initialize hotkey bar data successfully
 @retval EFI_INVALID_PARAMETER   HotKey pointer is NULL or there is no hot key data
 @retval EFI_NOT_FOUND           There is no hot key panel information
 @retval EFI_OUT_OF_RESOURCES    Allocate pool fail
**/
EFI_STATUS
InitHotKeyBar (
  IN HOT_KEY_INFO                                *HotKey
  )
{
  LIST_ENTRY                                     *PanelLink;
  H2O_PANEL_INFO                                 *HotKeyPanel;
  UINT32                                         Index;
  UINT32                                         HotKeyCount;
  H2O_CONTROL_INFO                               *HotKeyControls;
  H2O_CONTROL_INFO                               *HotKeyDescriptionControls;

  if (HotKey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Count the number of hot keys
  //
  HotKeyCount = 0;
  while (HotKey[HotKeyCount].String != NULL) {
    HotKeyCount++;
  }
  if (HotKeyCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  HotKeyPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_HOTKEY);
  if (HotKeyPanel == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Init hot key bar data
  //
  FreeControlList (&HotKeyPanel->ControlList);
  HotKeyPanel->ControlList.Count        = HotKeyCount * 2;
  HotKeyPanel->ControlList.ControlArray = AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * HotKeyPanel->ControlList.Count);
  if (HotKeyPanel->ControlList.ControlArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HotKeyControls            = HotKeyPanel->ControlList.ControlArray;
  HotKeyDescriptionControls = HotKeyPanel->ControlList.ControlArray + HotKeyCount;

  for (Index = 0; Index < HotKeyCount; Index++) {
    HotKeyControls[Index].Text.String              = AllocateCopyPool (StrSize (HotKey[Index].Mark), HotKey[Index].Mark);
    HotKeyControls[Index].HiiValue.BufferLen       = sizeof (EFI_KEY_DATA);
    HotKeyControls[Index].HiiValue.Buffer          = (UINT8 *) &HotKey[Index].KeyData;
    HotKeyControls[Index].ParentPanel              = HotKeyPanel;
    HotKeyControls[Index].ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_HIGHLIGHT;

    HotKeyDescriptionControls[Index].Text.String              = AllocateCopyPool (StrSize (HotKey[Index].String), HotKey[Index].String);
    HotKeyDescriptionControls[Index].ParentPanel              = HotKeyPanel;
    HotKeyDescriptionControls[Index].ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_NORMAL;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitMenu (
  VOID
  )
{
  EFI_STATUS                                 Status;
  H2O_FORM_BROWSER_PROTOCOL                  *FBProtocol;
  H2O_FORM_BROWSER_S                         *Statement;
  UINT32                                     Index;
  LIST_ENTRY                                 *PanelListHead;
  UINT32                                     NumberOfStatementIds;
  UINT32                                     MenuCount;
  H2O_CONTROL_INFO                           *PromptArray;
  H2O_CONTROL_INFO                           *ValueArray;
  H2O_CONTROL_INFO                           *HelpMsgArray;
  H2O_PANEL_INFO                             *SetupPagePanel;
  H2O_PANEL_INFO                             *SetupPage2Panel;
  H2O_PANEL_INFO                             *HelpTextPanel;
  H2O_CONTROL_LIST                           *SetupPageControls;
  H2O_CONTROL_LIST                           *SetupPage2Controls;
  H2O_CONTROL_LIST                           *HelpControls;
  UINT32                                     PseudoClass;

  PanelListHead = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelListHead, PanelListHead->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumberOfStatementIds = FBProtocol->CurrentP->NumberOfStatementIds;
  PromptArray  = AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * NumberOfStatementIds);
  ValueArray   = AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * NumberOfStatementIds);
  HelpMsgArray = AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * NumberOfStatementIds);
  if (PromptArray == NULL || ValueArray == NULL || HelpMsgArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetupPagePanel  = GetPanelInfoByType (PanelListHead, H2O_PANEL_TYPE_SETUP_PAGE);
  SetupPage2Panel = GetPanelInfoByType (PanelListHead, H2O_PANEL_TYPE_SETUP_PAGE2);
  HelpTextPanel   = GetPanelInfoByType (PanelListHead, H2O_PANEL_TYPE_HELP_TEXT);

  MenuCount = 0;
  for (Index = 0; Index < NumberOfStatementIds; Index ++) {
    Status = FBProtocol->GetSInfo (FBProtocol, FBProtocol->CurrentP->PageId, FBProtocol->CurrentP->StatementIds[Index], &Statement);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if ((Statement->DisplayAttribute & (H2O_DISPLAY_GRAYOUT | H2O_DISPLAY_LOCK)) != 0) {
      PseudoClass = H2O_STYLE_PSEUDO_CLASS_GRAYOUT;
    } else if (Statement->Selectable) {
      PseudoClass = H2O_STYLE_PSEUDO_CLASS_SELECTABLE;
    } else {
      PseudoClass = H2O_STYLE_PSEUDO_CLASS_NORMAL;
    }

    //
    // Fill Prompt control info
    //
    CopyMem (&PromptArray[MenuCount].HiiValue, &Statement->HiiValue, sizeof (EFI_HII_VALUE));
    PromptArray[MenuCount].Text.String = (*Statement->Prompt == L'\0') ? AllocateCopyPool (sizeof(L" "), L" ") :
                                                                         AllocateCopyPool (StrSize (Statement->Prompt), Statement->Prompt);
    PromptArray[MenuCount].PageId      = Statement->PageId;
    PromptArray[MenuCount].StatementId = Statement->StatementId;
    PromptArray[MenuCount].QuestionId  = Statement->QuestionId;
    PromptArray[MenuCount].Operand     = Statement->Operand;
    PromptArray[MenuCount].IfrOpCode   = Statement->IfrOpCode;
    PromptArray[MenuCount].Visible     = TRUE;
    PromptArray[MenuCount].Selectable  = Statement->Selectable;
    PromptArray[MenuCount].Modifiable  = TRUE;
    PromptArray[MenuCount].ParentPanel = SetupPagePanel;
    if (Statement->Help != NULL) {
      PromptArray[MenuCount].HelpText.String = AllocateCopyPool (StrSize (Statement->Help), Statement->Help);
    }
    if (Statement->Image != NULL) {
      CopyMem (&PromptArray[MenuCount].HelpImage, Statement->Image, sizeof (EFI_IMAGE_INPUT));
    }
    PromptArray[MenuCount].ControlStyle.StyleType   = SetStyleTypeByOpCode (Statement->Operand);
    PromptArray[MenuCount].ControlStyle.PseudoClass = PseudoClass;


    //
    // Fill Value control info
    //
    CopyMem (&ValueArray[MenuCount].HiiValue, &Statement->HiiValue, sizeof (EFI_HII_VALUE));
    ValueArray[MenuCount].Text.String = GetQuestionValueStr (Statement);
    ValueArray[MenuCount].PageId      = Statement->PageId;
    ValueArray[MenuCount].StatementId = Statement->StatementId;
    ValueArray[MenuCount].QuestionId  = Statement->QuestionId;
    ValueArray[MenuCount].Operand     = Statement->Operand;
    ValueArray[MenuCount].IfrOpCode   = Statement->IfrOpCode;
    ValueArray[MenuCount].Visible     = TRUE;
    ValueArray[MenuCount].Selectable  = Statement->Selectable;
    ValueArray[MenuCount].Modifiable  = TRUE;
    ValueArray[MenuCount].ParentPanel = SetupPagePanel;
    ValueArray[MenuCount].ControlStyle.StyleType   = SetStyleTypeByOpCode (Statement->Operand);
    ValueArray[MenuCount].ControlStyle.PseudoClass = PseudoClass;

    //
    // Fill Help Message control info
    //
    CopyMem (&HelpMsgArray[MenuCount].HiiValue, &Statement->HiiValue, sizeof (EFI_HII_VALUE));
    HelpMsgArray[MenuCount].Text.String = (Statement->Help == NULL || *Statement->Help == L'\0') ? AllocateCopyPool (sizeof(L" "), L" ") :
                                                                                                   AllocateCopyPool (StrSize (Statement->Help), Statement->Help);
    HelpMsgArray[MenuCount].PageId      = Statement->PageId;
    HelpMsgArray[MenuCount].StatementId = Statement->StatementId;
    HelpMsgArray[MenuCount].QuestionId  = Statement->QuestionId;
    HelpMsgArray[MenuCount].Operand     = Statement->Operand;
    HelpMsgArray[MenuCount].IfrOpCode   = Statement->IfrOpCode;
    HelpMsgArray[MenuCount].Visible     = TRUE;
    HelpMsgArray[MenuCount].Selectable  = Statement->Selectable;
    HelpMsgArray[MenuCount].Modifiable  = TRUE;
    HelpMsgArray[MenuCount].ParentPanel = HelpTextPanel;
    HelpMsgArray[MenuCount].ControlStyle.StyleType   = SetStyleTypeByOpCode (Statement->Operand);
    HelpMsgArray[MenuCount].ControlStyle.PseudoClass = H2O_STYLE_PSEUDO_CLASS_NORMAL;

    MenuCount ++;
    SafeFreePool ((VOID **) &Statement);
  }

  if (SetupPagePanel != NULL) {
    //
    // Setup Page Panel
    //
    if ((PromptArray[0].PageId & 0x0ffff) == 0x1000 ||
        (PromptArray[0].PageId & 0x0ffff) == 0x0001 ||
        SetupPage2Panel == NULL) {
      SetupPageControls = &SetupPagePanel->ControlList;
      FreeControlList (SetupPageControls);
      SafeFreePool ((VOID **) &SetupPagePanel->ContentsImage.CurrentBlt);
      if (SetupPage2Panel != NULL) {
        SafeFreePool ((VOID **) &SetupPage2Panel->ContentsImage.CurrentBlt);
      }
      if (MenuCount != 0) {
        SetupPageControls->Count = MenuCount * 2;
        SetupPageControls->ControlArray = AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * MenuCount * 2);
        if (SetupPageControls->ControlArray == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (SetupPageControls->ControlArray, PromptArray, sizeof (H2O_CONTROL_INFO) * MenuCount);
        CopyMem ((SetupPageControls->ControlArray + MenuCount), ValueArray, sizeof (H2O_CONTROL_INFO) * MenuCount);
      } else {
        SetupPageControls->Count        = 0;
        SetupPageControls->ControlArray = NULL;
      }
    }
  }

  if (SetupPage2Panel != NULL) {
    //
    // Setup Page 2 Panel
    //
    if ((PromptArray[0].PageId & 0x0ffff) != 0x1000 &&
      (PromptArray[0].PageId & 0x0ffff) != 0x0001) {

      SetupPage2Controls = &SetupPage2Panel->ControlList;
      FreeControlList (SetupPage2Controls);
      SafeFreePool ((VOID **) &SetupPage2Panel->ContentsImage.CurrentBlt);
      if (MenuCount != 0) {
        SetupPage2Controls->Count = MenuCount * 2;
        SetupPage2Controls->ControlArray = AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * MenuCount * 2);
        if (SetupPage2Controls->ControlArray == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (SetupPage2Controls->ControlArray, PromptArray, sizeof (H2O_CONTROL_INFO) * MenuCount);
        CopyMem ((SetupPage2Controls->ControlArray + MenuCount), ValueArray, sizeof (H2O_CONTROL_INFO) * MenuCount);
      } else {
        SetupPage2Controls->Count        = 0;
        SetupPage2Controls->ControlArray = NULL;
      }
    }
  }

  if (HelpTextPanel != NULL) {
    //
    // Help Text Panel
    //
    HelpControls = &HelpTextPanel->ControlList;
    FreeControlList (HelpControls);
    SafeFreePool ((VOID **) &HelpTextPanel->ContentsImage.CurrentBlt);
    if (MenuCount != 0) {
      HelpControls->Count = MenuCount;
      HelpControls->ControlArray = (H2O_CONTROL_INFO*) AllocateCopyPool (sizeof (H2O_CONTROL_INFO) * MenuCount, HelpMsgArray);
    } else {
      HelpControls->Count = 0;
      HelpControls->ControlArray = NULL;
    }
  }

  SafeFreePool ((VOID **) &PromptArray);
  SafeFreePool ((VOID **) &ValueArray);
  SafeFreePool ((VOID **) &HelpMsgArray);

  return EFI_SUCCESS;
}

EFI_STATUS
DESelectQuestion (
  IN H2O_FORM_BROWSER_PROTOCOL                *FBProtocol,
  IN H2O_PAGE_ID                              PageId,
  IN EFI_QUESTION_ID                          QuestionId,
  IN EFI_IFR_OP_HEADER                        *IfrOpCode
  );

H2O_CONTROL_INFO *
GetControlInfo (
  IN H2O_CONTROL_INFO                         *ControlArray,
  IN UINT32                                   ControlCount,
  IN EFI_QUESTION_ID                          QuestionId,
  IN EFI_IFR_OP_HEADER                        *IfrOpCode
  )
{
  UINT32                                      Index;

  if (ControlArray == NULL || ControlCount == 0) {
    return NULL;
  }

  for (Index = 0; Index < ControlCount; Index++) {
    if ((QuestionId != 0    && ControlArray[Index].QuestionId == QuestionId) ||
        (IfrOpCode  != NULL && ControlArray[Index].IfrOpCode  == IfrOpCode)) {
      return &ControlArray[Index];
    }
  }

  return NULL;
}

EFI_STATUS
DERefresh (
  IN     H2O_FORM_BROWSER_PROTOCOL            *FBProtocol
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      Index;
  UINT32                                      MenuCount;
  H2O_PANEL_INFO                              *SetupPagePanel;
  H2O_PANEL_INFO                              *HelpTextPanel;
  LIST_ENTRY                                  *PanelLink;
  H2O_CONTROL_INFO                            *Control;

  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_AT_MENU;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  SetupPagePanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_PAGE);
  if (SetupPagePanel == NULL) {
    return EFI_SUCCESS;
  }

  //
  // Init attribute and location for each item of menu
  //
  Status = InitMenu ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DisplayMenu (FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MenuCount = SetupPagePanel->ControlList.Count / 2;

  //
  // Display Current Question (High-light)
  //
  if (mDEPrivate->MenuSelected != NULL &&
      FBProtocol->CurrentQ     != NULL &&
      FBProtocol->CurrentQ->PageId     == mDEPrivate->MenuSelected->PageId &&
      FBProtocol->CurrentQ->QuestionId == mDEPrivate->MenuSelected->QuestionId &&
      FBProtocol->CurrentQ->IfrOpCode  == mDEPrivate->MenuSelected->IfrOpCode) {
    Control = GetControlInfo (
                SetupPagePanel->ControlList.ControlArray,
                MenuCount,
                mDEPrivate->MenuSelected->QuestionId,
                mDEPrivate->MenuSelected->IfrOpCode
                );
    if (Control != NULL) {
      DESelectQuestion (FBProtocol, mDEPrivate->MenuSelected->PageId, mDEPrivate->MenuSelected->QuestionId, mDEPrivate->MenuSelected->IfrOpCode);
      return EFI_SUCCESS;
    }
  }

  //
  // mDEPrivate->MenuSelected is not ready.
  // First, check if current question is in current page. If not, select question on first valid highlight question.
  //
  mDEPrivate->MenuSelected = NULL;
  Control = NULL;
  if (FBProtocol->CurrentQ != NULL) {
    Control = GetControlInfo (SetupPagePanel->ControlList.ControlArray, MenuCount, FBProtocol->CurrentQ->QuestionId, FBProtocol->CurrentQ->IfrOpCode);
  }
  if (Control == NULL || !IsValidHighlightStatement (Control)) {
    for (Index = 0; Index < MenuCount; Index ++) {
      if (IsValidHighlightStatement (&SetupPagePanel->ControlList.ControlArray[Index])) {
        Control = &SetupPagePanel->ControlList.ControlArray[Index];
        break;
      }
    }
  }

  if (Control != NULL) {
    SendSelectQNotify (
      Control->PageId,
      Control->QuestionId,
      Control->IfrOpCode
      );
  } else {
    HelpTextPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_HELP_TEXT);
    if (HelpTextPanel != NULL) {
      Control = GetControlInfo (
                  HelpTextPanel->ControlList.ControlArray,
                  MenuCount,
                  SetupPagePanel->ControlList.ControlArray[0].QuestionId,
                  SetupPagePanel->ControlList.ControlArray[0].IfrOpCode
                  );
      if (Control != NULL) {
        DisplayHighLightControl (FALSE, Control);
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DERefreshQuestion (
  IN H2O_FORM_BROWSER_PROTOCOL                *FBProtocol,
  IN BOOLEAN                                  HightLight,
  IN H2O_PAGE_ID                              PageId,
  IN EFI_QUESTION_ID                          QuestionId,
  IN EFI_IFR_OP_HEADER                        *IfrOpCode
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_S                          *Statement;
  H2O_DISPLAY_ENGINE_EVT_SELECT_Q             SelectQ;
  UINT32                                      ControlCount;
  H2O_CONTROL_INFO                            *ControlArray;
  H2O_PANEL_INFO                              *SetupPagePanel;
  H2O_PANEL_INFO                              *HelpTextPanel;
  H2O_CONTROL_INFO                             *Control;


  SetupPagePanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_SETUP_PAGE);
  if (SetupPagePanel == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Prompt
  //
  ControlArray = SetupPagePanel->ControlList.ControlArray;
  ControlCount = SetupPagePanel->ControlList.Count / 2;

  Control = GetControlInfo (ControlArray, ControlCount, QuestionId, IfrOpCode);
  if (Control == NULL) {
    return EFI_NOT_FOUND;
  }
  Status = FBProtocol->GetSInfo (FBProtocol, PageId, Control->StatementId, &Statement);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SafeFreePool ((VOID **) &Control->Text.String);
  Control->Text.String = AllocateCopyPool (StrSize (Statement->Prompt), Statement->Prompt);

  if (HightLight) {
    DisplayHighLightControl (FALSE, Control);
  } else {
    DisplayNormalControls (1, Control);
  }

  //
  // Value
  //
  ControlArray = &SetupPagePanel->ControlList.ControlArray[ControlCount];
  Control      = GetControlInfo (ControlArray, ControlCount, QuestionId, IfrOpCode);
  if (Control != NULL) {
    SafeFreePool ((VOID **) &Control->Text.String);
    Control->Text.String = GetQuestionValueStr (Statement);

    if (mDEPrivate->MenuSelected != NULL && ((QuestionId != 0    && mDEPrivate->MenuSelected->QuestionId == QuestionId) ||
                                             (IfrOpCode  != NULL && mDEPrivate->MenuSelected->IfrOpCode  == IfrOpCode))) {
      //
      // Select question
      //
      mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_SELECT_Q;
      ZeroMem (&SelectQ, sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q));
      SelectQ.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q);
      SelectQ.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
      SelectQ.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q;
      SelectQ.PageId     = PageId;
      SelectQ.QuestionId = Statement->QuestionId;
      SelectQ.IfrOpCode  = IfrOpCode;
      Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, (H2O_DISPLAY_ENGINE_EVT *) &SelectQ, NULL, NULL, 0, 0);
      mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_AT_MENU;
      if (EFI_ERROR (Status)) {
        DisplayHighLightControl (FALSE, Control);
      }
    } else {
      if (HightLight) {
        DisplayHighLightControl (FALSE, Control);
      } else {
        DisplayNormalControls (1, Control);
      }
    }
  }

  if (mDEPrivate->MenuSelected != NULL && mDEPrivate->MenuSelected->IfrOpCode != IfrOpCode) {
    return EFI_SUCCESS;
  }

  //
  // Help
  //
  HelpTextPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_HELP_TEXT);
  if (HelpTextPanel == NULL || !IsVisibility (HelpTextPanel, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL)) {
    return EFI_SUCCESS;
  }

  ControlArray = HelpTextPanel->ControlList.ControlArray;
  Control      = GetControlInfo (ControlArray, ControlCount, QuestionId, IfrOpCode);
  if (Control != NULL) {
    SafeFreePool ((VOID **) &Control->Text.String);
    Control->Text.String = AllocateCopyPool (StrSize (Statement->Help), Statement->Help);

    DisplayHighLightControl (FALSE, Control);
  }

  SafeFreePool ((VOID **) &Statement);

  return EFI_SUCCESS;
}

EFI_STATUS
DEOpenLayout (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol
  )
{
  EFI_STATUS                                 Status;
  HOT_KEY_INFO                               *HotKey;

  //
  // Layout
  //
  Status = InitLayout ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = DisplayLayout ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Init and display setup menu (Form FB->GetSMInfo)
  //
  Status = InitSetupMenu ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = DisplaySetupMenu (FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Init and display hotkey (Form FB->GetFSInfo)
  //
  HotKey = FBProtocol->CurrentP->HotKeyInfo;
  Status = InitHotKeyBar (HotKey);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = DisplayHotkey ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

EFI_STATUS
DEShutLayout (
  VOID
  )
{
  LIST_ENTRY                                 *PanelLink;
  H2O_PANEL_INFO                             *Panel;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    PanelLink = PanelLink->ForwardLink;
    Panel = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);
    FreePanel (Panel);
  } while (!IsNodeAtEnd (&mDEPrivate->Layout->PanelListHead, PanelLink));

  return EFI_SUCCESS;
}

EFI_STATUS
DESelectPage (
  IN     H2O_PAGE_ID                          PageId
  )
{
  UINT32                                      Index;
  STATIC H2O_PAGE_ID                          OldPageId = 0;
  UINT32                                      SetupMenuCount;
  H2O_PANEL_INFO                              *SetupMenuPanel;
  LIST_ENTRY                                  *PanelLink;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  SetupMenuPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_MENU);
  if (SetupMenuPanel == NULL) {
    return EFI_SUCCESS;
  }
  SetupMenuCount = SetupMenuPanel->ControlList.Count;

  //
  // Set "PageTagSelected"
  //
  for (Index = 0; Index < SetupMenuCount; Index ++) {
    if ((SetupMenuPanel->ControlList.ControlArray[Index].PageId >> 16) == (PageId >> 16)) {
      //
      // Finding "PageTagSelected" is success
      //
      mDEPrivate->PageTagSelected = &SetupMenuPanel->ControlList.ControlArray[Index];
      //
      // Display new current page tag with high-light color
      //
      DisplayHighLightControl (FALSE, mDEPrivate->PageTagSelected);
    } else if ((SetupMenuPanel->ControlList.ControlArray[Index].PageId >> 16) == (OldPageId >> 16)) {
      //
      // Display old current page tag with original color
      //
      DisplayNormalControls (1, &SetupMenuPanel->ControlList.ControlArray[Index]);
    }
  }
  //
  // Update old page id
  //
  OldPageId = mDEPrivate->PageTagSelected->PageId;

  return EFI_SUCCESS;
}

EFI_STATUS
DEOpenPage (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol
  )
{
  EFI_STATUS                                 Status;
  H2O_FORM_BROWSER_SM                        *SetupMenuData;

  if (FBProtocol->CurrentP == NULL) {
    return EFI_ABORTED;
  }

  Status = FBProtocol->GetSMInfo (FBProtocol, &SetupMenuData);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = DisplayTitle ((CONST CHAR16*)SetupMenuData->TitleString, (CONST CHAR16*)SetupMenuData->CoreVersionString);
  FreeSetupMenuData (SetupMenuData);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // Display Current Page Menu
  //
  Status = DERefresh (FBProtocol);
  if (FBProtocol->CurrentP != NULL) {
    DESelectPage (FBProtocol->CurrentP->PageId);
  }

  return Status;
}

EFI_STATUS
DEShutPage (
  VOID
  )
{
  LIST_ENTRY                                     *PanelLink;
  H2O_PANEL_INFO                                 *Panel;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    PanelLink = PanelLink->ForwardLink;
    Panel = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);
    if (
      Panel->PanelType == H2O_PANEL_TYPE_HELP_TEXT ||
      Panel->PanelType == H2O_PANEL_TYPE_QUESTION
      ) {
      FreePanel (Panel);
    }
  } while (!IsNodeAtEnd (&mDEPrivate->Layout->PanelListHead, PanelLink));

  return EFI_SUCCESS;
}

EFI_STATUS
DESelectQuestion (
  IN H2O_FORM_BROWSER_PROTOCOL                *FBProtocol,
  IN H2O_PAGE_ID                              PageId,
  IN EFI_QUESTION_ID                          QuestionId,
  IN EFI_IFR_OP_HEADER                        *IfrOpCode
  )
{
  EFI_STATUS                                  Status;
  STATIC H2O_FORM_BROWSER_Q                   *OldCurrentQ = NULL;
  H2O_DISPLAY_ENGINE_EVT_SELECT_Q             SelectQ;
  H2O_DISPLAY_ENGINE_EVT_SHUT_Q               ShutQ;
  H2O_PANEL_INFO                              *SetupPagePanel;
  LIST_ENTRY                                  *PanelLink;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }


  SetupPagePanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_PAGE);
  if (SetupPagePanel == NULL || SetupPagePanel->ControlList.Count == 0) {
    OldCurrentQ = FBProtocol->CurrentQ;
    return EFI_SUCCESS;
  }

  mDEPrivate->MenuSelected = GetControlInfo (
                               SetupPagePanel->ControlList.ControlArray,
                               SetupPagePanel->ControlList.Count / 2,
                               QuestionId,
                               IfrOpCode
                               );
  //
  // Display old current question control with original color
  // and display new current question control with high-light color
  //
  if (OldCurrentQ != NULL) {
    if (OldCurrentQ->IfrOpCode != IfrOpCode) {
      //
      // Check Specific Question (SHUT_Q)
      //
      mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_SHUT_Q;
      ZeroMem (&ShutQ, sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_Q));
      ShutQ.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_Q);
      ShutQ.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
      ShutQ.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_Q;
      ShutQ.PageId     = OldCurrentQ->PageId;
      ShutQ.QuestionId = OldCurrentQ->QuestionId;
      ShutQ.IfrOpCode  = OldCurrentQ->IfrOpCode;
      Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, (H2O_DISPLAY_ENGINE_EVT *) &ShutQ, NULL, NULL, 0, 0);
    }
    if (OldCurrentQ->PageId != PageId) {
      //
      // Changed page
      //
      OldCurrentQ = NULL;
    } else if (OldCurrentQ->IfrOpCode != IfrOpCode) {
      //
      // Refresh Question
      //
      DERefreshQuestion (FBProtocol, FALSE, OldCurrentQ->PageId, OldCurrentQ->QuestionId, OldCurrentQ->IfrOpCode);
    }
  }

  //
  // Check Specific Question (SELECT_Q)
  //
  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_SELECT_Q;
  ZeroMem (&SelectQ, sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q));
  SelectQ.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q);
  SelectQ.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  SelectQ.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q;
  SelectQ.PageId     = PageId;
  SelectQ.QuestionId = QuestionId;
  SelectQ.IfrOpCode  = IfrOpCode;
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, (H2O_DISPLAY_ENGINE_EVT *) &SelectQ, NULL, NULL, 0, 0);
  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_AT_MENU;

  if (EFI_ERROR (Status)) {
    //
    // Ensure selected question is on this page now
    //
    if (EnsureControlInPanel (mDEPrivate->MenuSelected) == EFI_SUCCESS) {
      DERefresh (FBProtocol);
    }
    //
    // High light question
    //
    DERefreshQuestion (FBProtocol, TRUE, PageId, QuestionId, IfrOpCode);
  }
  //
  // Update old current question
  //
  OldCurrentQ = FBProtocol->CurrentQ;

  return EFI_SUCCESS;
}

EFI_STATUS
DEShutQuestion (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT            *Notify
  )
{
  EFI_STATUS                                 Status;


  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_SHUT_Q;
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, (H2O_DISPLAY_ENGINE_EVT *) Notify, NULL, NULL, 0, 0);
  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_AT_MENU;

  return Status;
}

EFI_STATUS
DEChangingQuestion (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT            *Notify
  )
{
  EFI_STATUS                                 Status;

  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_CHANGING_Q;
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, Notify, NULL, NULL, 0, 0);
  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG;
  if (Status == EFI_NOT_FOUND) {
    Status = EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS
DEOpenDialog (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT            *Notify
  )
{
  EFI_STATUS                                 Status;

  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_OPEN_D;
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, Notify, NULL, NULL, 0, 0);
  mDEPrivate->DEStatus = (!EFI_ERROR (Status)) ? DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG : DISPLAY_ENGINE_STATUS_AT_MENU;
  if (Status == EFI_NOT_FOUND) {
    Status = EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS
DEShutDialog (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT            *Notify
  )
{
  EFI_STATUS                                 Status;

  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_SHUT_D;
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, Notify, NULL, NULL, 0, 0);
  mDEPrivate->DEStatus = DISPLAY_ENGINE_STATUS_AT_MENU;
  if (Status == EFI_NOT_FOUND) {
    Status = EFI_SUCCESS;
  }

  if (FBProtocol->CurrentP != NULL) {
    DEOpenLayout (FBProtocol);
    DEOpenPage (FBProtocol);
  }
  DEConOutSetAttribute (mDEPrivate, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);

  return Status;
}

EFI_STATUS
DEExit (
  VOID
  )
{
  LIST_ENTRY                                     *PanelLink;
  H2O_PANEL_INFO                                 *Panel;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  //
  // Clear layout info
  //
  do {
    PanelLink = PanelLink->ForwardLink;
    Panel = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);
    FreePanel (Panel);
  } while (!IsNodeAtEnd (&mDEPrivate->Layout->PanelListHead, PanelLink));

  mDEPrivate->PageTagSelected = NULL;
  mDEPrivate->MenuSelected = NULL;
  mDEPrivate->PopUpSelected = NULL;

  return EFI_SUCCESS;
}

EFI_STATUS
AtMenu (
  IN     H2O_FORM_BROWSER_PROTOCOL            *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN     BOOLEAN                              Keyboard,
  IN     EFI_KEY_DATA                         *KeyData,
  IN     UINT32                               MouseX,
  IN     UINT32                               MouseY
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *MenuSelected;
  H2O_CONTROL_INFO                            *TempMenuSelected;
  H2O_CONTROL_INFO                            *PageTagSelected;
  H2O_CONTROL_INFO                            *TempPageTagSelected;
  H2O_CONTROL_INFO                            *HotKeySelected;
  H2O_CONTROL_INFO                            NullControl;
  UINT32                                      SetupPageMenuCount;
  H2O_PANEL_INFO                              *SetupPagePanel;
  UINT32                                      SetupMenuCount;
  H2O_PANEL_INFO                              *SetupMenuPanel;
  H2O_PANEL_INFO                              *HotkeyPanel;
  LIST_ENTRY                                  *PanelLink;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  //
  // Specific Question At Menu Behavior
  //
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, Notify, &Keyboard, &KeyData->Key, MouseX, MouseY);
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // Keyboard Hot Key
  //
  if (IsHotKey (Keyboard, KeyData)) {
    Status = SendEvtByHotKey (Keyboard, KeyData);
    goto EXIT;
  }

  SetupPagePanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_PAGE);
  SetupMenuPanel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_MENU);
  if (SetupPagePanel == NULL || SetupMenuPanel == NULL) {
    goto EXIT;
  }
  SetupPageMenuCount = SetupPagePanel->ControlList.Count / 2;
  SetupMenuCount     = SetupMenuPanel->ControlList.Count;

  //
  // Standard At Menu Behavior
  //
  TempMenuSelected = mDEPrivate->MenuSelected;
  TempPageTagSelected = mDEPrivate->PageTagSelected;
  do {
    MenuSelected = TempMenuSelected;
    PageTagSelected = TempPageTagSelected;
    if (
      CheckPressControls (Keyboard, &KeyData->Key, MouseX, MouseY, SetupPageMenuCount * 2, SetupPagePanel->ControlList.ControlArray, &MenuSelected) ||
      CheckPressControls (Keyboard, &KeyData->Key, MouseX, MouseY, SetupMenuCount, SetupMenuPanel->ControlList.ControlArray, &PageTagSelected)
      ) {
      //
      // One of Menu been press
      //
      if (MenuSelected == NULL) {
        SendDiscardExitNotify ();
        break;
      } else if (
        PageTagSelected->PageId == TempPageTagSelected->PageId &&
        MenuSelected->PageId == mDEPrivate->MenuSelected->PageId &&
        MenuSelected->QuestionId == mDEPrivate->MenuSelected->QuestionId &&
        MenuSelected->IfrOpCode == mDEPrivate->MenuSelected->IfrOpCode &&
        KeyData->Key.UnicodeChar == CHAR_CARRIAGE_RETURN
        ) {
        if (!Keyboard && CheckPressControls (Keyboard, &KeyData->Key, MouseX, MouseY, SetupMenuCount, SetupMenuPanel->ControlList.ControlArray, &PageTagSelected)) {
          //
          // Do nothing when user click current Setup Menu
          //
          break;
        }

        SendOpenQNotify (
          FBProtocol->CurrentQ->PageId,
          FBProtocol->CurrentQ->QuestionId,
          FBProtocol->CurrentQ->IfrOpCode
          );
        break;
      } else if (
        KeyData->Key.ScanCode == SCAN_UP ||
        KeyData->Key.ScanCode == SCAN_DOWN ||
        (PageTagSelected->PageId == TempPageTagSelected->PageId &&
        MenuSelected->PageId == mDEPrivate->MenuSelected->PageId &&
        !Keyboard && KeyData->Key.UnicodeChar == CHAR_CARRIAGE_RETURN)
        ){
        //
        // If it is a "keyboard select" or a "mouse select one of question in this page"
        // Then, select one of another question, notify Select Question (SELECT_Q)
        //
        if (MenuSelected->Selectable) {
          mDEPrivate->MenuSelected = MenuSelected;
          SendSelectQNotify (
            MenuSelected->PageId,
            MenuSelected->QuestionId,
            MenuSelected->IfrOpCode
            );
          break;
        }
        if (
          (UINTN)(UINTN *) MenuSelected == (UINTN)(UINTN *) &SetupPagePanel->ControlList.ControlArray[0] ||
          (UINTN)(UINTN *) MenuSelected == sizeof (H2O_CONTROL_INFO) * (SetupPageMenuCount - 1) + (UINTN)(UINTN *) &SetupPagePanel->ControlList.ControlArray[0] ||
          !Keyboard
          ) {
          //
          // If
          // 1.It is a "keyboard select" and select first option of this page ||
          // 2.It is a "keyboard select" and select end option of this page ||
          // 3.It is a "Mouse select"
          // Then, do not search the next selectable question.
          //
          break;
        }
        //
        // If it is a keyboard event, and the selected question is not selectable
        // Then, keep search the next selectable question
        //
        TempMenuSelected = MenuSelected;
        continue;
      } else if (
        KeyData->Key.ScanCode == SCAN_LEFT ||
        KeyData->Key.ScanCode == SCAN_RIGHT ||
        (PageTagSelected->PageId != mDEPrivate->PageTagSelected->PageId && !Keyboard && KeyData->Key.UnicodeChar == CHAR_CARRIAGE_RETURN)
        ){
        if (!IsRootForm ()) {
          //
          // Do nothing when it is not in root Form.
          //
          goto EXIT;
        }

        if (KeyData->Key.ScanCode == SCAN_LEFT || KeyData->Key.ScanCode == SCAN_RIGHT) {
          //
          // Confirm MenuSelected is one of Page Tag Controls (not Prompt or Value Control)
          //
          PageTagSelected = mDEPrivate->PageTagSelected;
          CheckPressControls (Keyboard, &KeyData->Key, MouseX, MouseY, SetupPageMenuCount, SetupPagePanel->ControlList.ControlArray, &PageTagSelected);
        }

        //
        // If it is a "keyboard select Left and Right" or a "mouse select one of another page"
        //
        if (PageTagSelected->Selectable) {
          //
          // Select another page
          //
          mDEPrivate->PageTagSelected = PageTagSelected;
          SendSelectPNotify (PageTagSelected->PageId);
          break;
        }

        if (
          (UINTN)(UINTN *) PageTagSelected == (UINTN)(UINTN*) &SetupMenuPanel->ControlList.ControlArray[0] ||
          (UINTN)(UINTN *) PageTagSelected == sizeof (H2O_CONTROL_INFO) * (SetupMenuCount - 1) + (UINTN)(UINTN *) &SetupMenuPanel->ControlList.ControlArray[0] ||
          !Keyboard
          ) {
          //
          // If
          // 1.It is a "keyboard select" and select first page ||
          // 2.It is a "keyboard select" and select end page ||
          // 3.It is a "Mouse select"
          // Then, do not search the next selectable page.
          //
          break;
        }
      }
      TempPageTagSelected = PageTagSelected;
      continue;
    } else {
      //
      // Hot Key
      //
      ZeroMem (&NullControl, sizeof (H2O_CONTROL_INFO));
      HotKeySelected = &NullControl;
      HotkeyPanel  = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_HOTKEY);
      if (HotkeyPanel != NULL) {
        if (
          CheckPressControls (Keyboard, &KeyData->Key, MouseX, MouseY, HotkeyPanel->ControlList.Count, HotkeyPanel->ControlList.ControlArray, &HotKeySelected) ||
          IsHotKey (Keyboard, KeyData)
          ) {
          if (!Keyboard) {
            if (HotKeySelected->HiiValue.Buffer == NULL) {
              break;
            }

            KeyData = (EFI_KEY_DATA *)(UINTN *)HotKeySelected->HiiValue.Buffer;
            Keyboard = TRUE;
          }
          //
          // Notify Hot-Key
          //
          Status = SendEvtByHotKey (Keyboard, KeyData);
          break;
        }
      }

      //
      // Page Up and Page Down
      //
      if (KeyData->Key.ScanCode == SCAN_PAGE_UP || KeyData->Key.ScanCode == SCAN_PAGE_DOWN) {
        if (KeyData->Key.ScanCode == SCAN_PAGE_UP) {
          PanelPageUp (SetupPagePanel);
        } else if (KeyData->Key.ScanCode == SCAN_PAGE_DOWN) {
          PanelPageDown (SetupPagePanel);
        }
        //
        // Notify refresh for display page with PAGE_UP or PAGE_DOWN (REFRESH)
        //
        SendRefreshNotify ();
        break;
      }

      //
      // Not press on any Control
      //
      break;
    }
  } while (TRUE);

EXIT:
  return Status;
}

EFI_STATUS
AtPopUpDialog (
  IN     H2O_FORM_BROWSER_PROTOCOL            *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN     BOOLEAN                              Keyboard,
  IN     EFI_KEY_DATA                         *KeyData,
  IN     UINT32                               MouseX,
  IN     UINT32                               MouseY
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *MenuSelected;
  H2O_CONTROL_LIST                            *PopUpControls;
  LIST_ENTRY                                  *PanelLink;
  H2O_PANEL_INFO                              *Panel;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  //
  // Specific Question At Pop-Up Dialog Behavior
  //
  Status = CheckSpecificQuestion (FBProtocol, mDEPrivate->DEStatus, Notify, &Keyboard, &KeyData->Key, MouseX, MouseY);
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Panel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_QUESTION);
  if (Panel == NULL) {
    return EFI_SUCCESS;
  }
  PopUpControls = &Panel->ControlList;
  //
  // Standard At Pop-Up Dialog Behavior
  //
  MenuSelected = mDEPrivate->PopUpSelected;
  if (CheckPressControls (Keyboard, &KeyData->Key, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &MenuSelected)) {
    if (MenuSelected == NULL) {
      SendShutDNotify (FBProtocol);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CheckDisplayEngineStatus (
  IN     H2O_FORM_BROWSER_PROTOCOL            *FBProtocol,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN     EFI_KEY_DATA                         *KeyData,
  IN     EFI_SIMPLE_POINTER_PROTOCOL          *SimplePointer,
  IN     EFI_SIMPLE_POINTER_STATE             *RelPtrState,
  IN     EFI_ABSOLUTE_POINTER_PROTOCOL        *AbsolutePointer,
  IN     EFI_ABSOLUTE_POINTER_STATE           *AbsPtrState
  )
{
  EFI_STATUS                                  Status;

  BOOLEAN                                     Keyboard;
  EFI_KEY_DATA                                SendKeyData;
  STATIC UINT32                               PreviousActiveButtons = 0;
  STATIC INT32                                RealMouseX = 0;
  STATIC INT32                                RealMouseY = 0;


  Keyboard = FALSE;
  ZeroMem (&SendKeyData, sizeof (EFI_KEY_DATA));
  if (KeyData != NULL) {
    //
    // Keyboard
    //
    Keyboard = TRUE;
    CopyMem (&SendKeyData, KeyData, sizeof (EFI_KEY_DATA));
  } else if (RelPtrState != NULL) {
    //
    // Rel Mouse
    //
    if (RelPtrState->LeftButton) {
      SendKeyData.Key.ScanCode = SCAN_NULL;
      SendKeyData.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
    }
    if (RelPtrState->RightButton) {
      SendKeyData.Key.ScanCode = SCAN_ESC;
      SendKeyData.Key.UnicodeChar = CHAR_NULL;
    }
    RealMouseX += RelPtrState->RelativeMovementX;
    RealMouseY += RelPtrState->RelativeMovementY;
  } else if (AbsPtrState != NULL) {
    //
    // Abs Mouse
    //
    if ((AbsPtrState->ActiveButtons & EFI_ABSP_TouchActive) == EFI_ABSP_TouchActive &&
        (AbsPtrState->ActiveButtons & EFI_ABSP_TouchActive) != (PreviousActiveButtons & EFI_ABSP_TouchActive)) {
      //
      // Left Button
      //
      SendKeyData.Key.ScanCode = SCAN_NULL;
      SendKeyData.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
    }
    if ((AbsPtrState->ActiveButtons & EFI_ABS_AltActive) == EFI_ABS_AltActive &&
        (AbsPtrState->ActiveButtons & EFI_ABS_AltActive) != (PreviousActiveButtons & EFI_ABS_AltActive)) {
      //
      // Right Button
      //
      SendKeyData.Key.ScanCode = SCAN_ESC;
      SendKeyData.Key.UnicodeChar = CHAR_NULL;
    }
    PreviousActiveButtons = AbsPtrState->ActiveButtons;
    RealMouseX = (INT32)AbsPtrState->CurrentX;
    RealMouseY = (INT32)AbsPtrState->CurrentY;
  }

  Status = EFI_SUCCESS;
  switch (mDEPrivate->DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_MENU:
    Status = AtMenu (FBProtocol, Notify, Keyboard, &SendKeyData, (UINT32) RealMouseX, (UINT32) RealMouseY);
    break;

  case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
    Status = AtPopUpDialog (FBProtocol, Notify, Keyboard, &SendKeyData, (UINT32) RealMouseX, (UINT32) RealMouseY);
    break;

  default:
    break;
  }

  return Status;
}

EFI_STATUS
DEEventCallback (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL        *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PROTOCOL                   *FBProtocol;
  H2O_DISPLAY_ENGINE_EVT_KEYPRESS             *KeyPressNotify;
  H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE         *RelPtrMoveNotify;
  H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE         *AbsPtrMoveNotify;
  H2O_DISPLAY_ENGINE_EVT_REFRESH_Q            *RefreshQNotify;
  H2O_DISPLAY_ENGINE_EVT_SELECT_P             *SelectPNotify;
  H2O_DISPLAY_ENGINE_EVT_SELECT_Q             *SelectQNotify;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (IsNull (&mDEPrivate->ConsoleDevListHead, mDEPrivate->ConsoleDevListHead.ForwardLink)) {
    //
    // Do nothing
    //
    return EFI_SUCCESS;
  }

  switch (Notify->Type) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS:
    KeyPressNotify = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *) Notify;
    Status = CheckDisplayEngineStatus(FBProtocol, Notify, &KeyPressNotify->KeyData, NULL, NULL, NULL, NULL);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE:
    RelPtrMoveNotify = (H2O_DISPLAY_ENGINE_EVT_REL_PTR_MOVE *) Notify;
    Status = CheckDisplayEngineStatus(FBProtocol, Notify, NULL, NULL, &RelPtrMoveNotify->State, NULL, NULL);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE:
    AbsPtrMoveNotify = (H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE *) Notify;
    Status = CheckDisplayEngineStatus(FBProtocol, Notify, NULL, NULL, NULL, NULL, &AbsPtrMoveNotify->AbsPtrState);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_L:
    Status = DEOpenLayout (FBProtocol);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_L:
    Status = DEShutLayout ();
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_P:
    Status = DEShutPage ();
    Status = DEOpenPage (FBProtocol);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_P:
    Status = DEShutPage ();
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D:
    Status = DEOpenDialog (FBProtocol, Notify);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D:
    Status = DEShutDialog (FBProtocol, Notify);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH:
    Status = DERefresh (FBProtocol);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q:
    if (!IsDialogStatus (mDEPrivate->DEStatus)) {
      RefreshQNotify = (H2O_DISPLAY_ENGINE_EVT_REFRESH_Q *) Notify;
      DERefreshQuestion (
        FBProtocol,
        FALSE,
        RefreshQNotify->PageId,
        RefreshQNotify->QuestionId,
        RefreshQNotify->IfrOpCode
        );
    }
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q:
    SelectQNotify = (H2O_DISPLAY_ENGINE_EVT_SELECT_Q *) Notify;
    Status = DESelectQuestion (
               FBProtocol,
               SelectQNotify->PageId,
               SelectQNotify->QuestionId,
               SelectQNotify->IfrOpCode
               );
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_Q:
    Status = DEShutQuestion (FBProtocol, Notify);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P:
    SelectPNotify = (H2O_DISPLAY_ENGINE_EVT_SELECT_P *) Notify;
    Status = DESelectPage (SelectPNotify->PageId);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q:
    Status = DEChangingQuestion (FBProtocol, Notify);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT:
    Status = DEExit ();
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return Status;
}

/**
 Initialize display engine private data

 @retval EFI_SUCCESS                Initialize display engine private data successfully
 @retval EFI_OUT_OF_RESOURCES       Allocate pool fail
 @retval Other                      Locate H2O form browser protocol fail

**/
EFI_STATUS
DEInit (
  VOID
  )
{
  EFI_STATUS                               Status;

  mDEPrivate = (H2O_DISPLAY_ENGINE_PRIVATE_DATA *) AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_PRIVATE_DATA));
  if (mDEPrivate == NULL) {
    DEBUG ((EFI_D_ERROR, "[H2ODisplayEngineLib] Allocate pool fail in DEInit function\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  mDEPrivate->Signature          = H2O_DISPLAY_ENGINE_SIGNATURE;
  mDEPrivate->ImageHandle        = gImageHandle;
  mDEPrivate->DEStatus           = DISPLAY_ENGINE_STATUS_INIT;
  mDEPrivate->DisplayEngine.Size = (UINT32) sizeof (H2O_DISPLAY_ENGINE_PROTOCOL);

  InitializeListHead (&mDEPrivate->ConsoleDevListHead);

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &mDEPrivate->FBProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[H2ODisplayEngineLib] Locate H2O_FORM_BROWSER_PROTOCOL fail in DEInit function, Status : %r\n", Status));
    SafeFreePool ((VOID **) &mDEPrivate);
    mDEPrivate = NULL;
    return Status;
  }

  return Status;
}
