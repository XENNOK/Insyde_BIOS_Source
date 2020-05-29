/** @file
 Presentation functions for UI

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

#include "Setup.h"
#include "Ui.h"
#include "SetupUtility.h"
#include <Library/DxeOemSvcKernelLib.h>


BOOLEAN            mHiiPackageListUpdated;
UI_MENU_SELECTION  *gCurrentSelection;

UI_HOTKEY_T0_QUESTION_ID gUiHotkeyToQuestionId[] = {
  { UI_HOTKEY_ESC, KEY_SCAN_ESC},
  { UI_HOTKEY_F9,  KEY_SCAN_F9 },
  { UI_HOTKEY_F10, KEY_SCAN_F10}
};

/**
 Draw retangle with specified character.

 @param[in] LeftColumn     Left column of retangle.
 @param[in] RightColumn    Right column of retangle.
 @param[in] TopRow         Start row of retangle.
 @param[in] BottomRow      End row of retangle.
 @param[in] Character      Character
**/
VOID
DrawLines (
  IN  UINTN                                   LeftColumn,
  IN  UINTN                                   RightColumn,
  IN  UINTN                                   TopRow,
  IN  UINTN                                   BottomRow,
  IN  CHAR16                                  Character
  )
{
  CHAR16  *Buffer;
  UINTN   Row;

  Buffer = AllocateZeroPool ((RightColumn - LeftColumn + 2) * sizeof (CHAR16));
  ASSERT (Buffer != NULL);

  //
  // Much faster to buffer the long string instead of print it a character at a time
  //
  SetUnicodeMem (Buffer, RightColumn - LeftColumn, Character);

  //
  // Clear the desired area with the appropriate foreground/background
  //
  for (Row = TopRow; Row <= BottomRow; Row++) {
    PrintStringAt (LeftColumn, Row, Buffer);
  }

  gST->ConOut->SetCursorPosition (gST->ConOut, LeftColumn, TopRow);

  gBS->FreePool (Buffer);
}

/**
 Clear retangle with specified text attribute.

 @param[in] LeftColumn     Left column of retangle.
 @param[in] RightColumn    Right column of retangle.
 @param[in] TopRow         Start row of retangle.
 @param[in] BottomRow      End row of retangle.
 @param[in] TextAttribute  The character foreground and background.
**/
VOID
ClearLines (
  IN UINTN                                       LeftColumn,
  IN UINTN                                       RightColumn,
  IN UINTN                                       TopRow,
  IN UINTN                                       BottomRow,
  IN UINTN                                       TextAttribute
  )
{
  //
  // Set foreground and background as defined
  //
  gST->ConOut->SetAttribute (gST->ConOut, TextAttribute);
  DrawLines (LeftColumn, RightColumn, TopRow, BottomRow, L' ');
}

/**
 Concatenate a narrow string to another string.

 @param[out] Destination  The destination string.
 @param[in]  Source       The source string. The string to be concatenated.
                          to the end of Destination.
**/
VOID
NewStrCat (
  OUT CHAR16                                      *Destination,
  IN  CHAR16                                      *Source
  )
{
  UINTN Length;

  for (Length = 0; Destination[Length] != 0; Length++)
    ;

  //
  // We now have the length of the original string
  // We can safely assume for now that we are concatenating a narrow value to this string.
  // For instance, the string is "XYZ" and cat'ing ">"
  // If this assumption changes, we need to make this routine a bit more complex
  //
  Destination[Length] = NARROW_CHAR;
  Length++;

  StrCpy (Destination + Length, Source);
}

/**
 Draw the Administer Secure Boot menu

 @param[in] LocalScreen         Screen Descriptor for administer secure boot Form

 @retval EFI_SUCCESS            print the footer successful.
 @return EFI_INVALID_PARAMETER  input parameter is invalid.
**/
EFI_STATUS
DrawSecureBootFooter (
  IN EFI_SCREEN_DESCRIPTOR             *LocalScreen
  )
{
  UINTN               BottomRowOfHelp;
  UINTN               PrintCol;
  CHAR16              *StringPtr;

  if (LocalScreen == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BottomRowOfHelp = LocalScreen->BottomRow - 3;
  PrintCol = LocalScreen->LeftColumn + 1;
  PrintAt (PrintCol, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
  PrintCol = LocalScreen->LeftColumn + (LocalScreen->RightColumn - LocalScreen->LeftColumn) / 4;
  PrintStringAt (PrintCol, BottomRowOfHelp, gEnterString);
  PrintCol = LocalScreen->LeftColumn + (LocalScreen->RightColumn - LocalScreen->LeftColumn) * 2 / 4;
  StringPtr = GetToken(STRING_TOKEN (SECURE_MENU_ESCAPE_STRING), gHiiHandle);
  PrintStringAt (PrintCol, BottomRowOfHelp, StringPtr);
  ScuSafeFreePool ((VOID **)&StringPtr);
  PrintCol = LocalScreen->LeftColumn + (LocalScreen->RightColumn - LocalScreen->LeftColumn) * 3 / 4;
  StringPtr = GetToken(STRING_TOKEN (SECURE_SAVE_STRING), gHiiHandle);
  PrintStringAt (PrintCol, BottomRowOfHelp, StringPtr);
  ScuSafeFreePool ((VOID **)&StringPtr);

  return EFI_SUCCESS;
}

/**
 This function displays the page frame.
**/
VOID
DisplayPageFrame (
  VOID
  )
{
  UINTN                  Index;
  UINT8                  Line;
  UINT8                  Alignment;
  CHAR16                 Character;
  CHAR16                 *Buffer;
  CHAR16                 *StrFrontPageBanner;
  EFI_SCREEN_DESCRIPTOR  LocalScreen;
  UINTN                  Row;


  CopyMem (&LocalScreen, &gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  if (gDeviceManagerSetup != NULL) {
    if (gDeviceManagerSetup->Firstin == TRUE) {
      //
      // Maybe change language, so we re-init browser strings
      //
      InitializeBrowserStrings ();

      SetupDisplayTitleAndHotKeyBar (&LocalScreen);
      gDeviceManagerSetup->Firstin = FALSE;
    }
    return;
  }
  if ((gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) ||
      mBootManager) {
    ClearLines (
      LocalScreen.LeftColumn,
      LocalScreen.RightColumn,
      LocalScreen.TopRow,
      LocalScreen.BottomRow,
      BANNER_TEXT | BANNER_BACKGROUND
      );
  } else {
    ClearLines (
      LocalScreen.LeftColumn,
      LocalScreen.RightColumn,
      LocalScreen.TopRow,
      LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT - 1,
      BANNER_TEXT | BANNER_BACKGROUND
      );
    ClearLines (
      LocalScreen.LeftColumn,
      LocalScreen.RightColumn,
      LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT,
      LocalScreen.BottomRow,
      KEYHELP_TEXT | KEYHELP_BACKGROUND
      );
  }
  if (gSecureBootMgr) {
    DrawSecureBootFooter (&LocalScreen);
  }
  //
  // For now, allocate an arbitrarily long buffer
  //
  Buffer = AllocateZeroPool (0x10000);
  ASSERT (Buffer != NULL);
  if (Buffer == NULL) {
    return ;
  }

  Character = BOXDRAW_HORIZONTAL;

  for (Index = 0; Index + 2 < (LocalScreen.RightColumn - LocalScreen.LeftColumn); Index++) {
    Buffer[Index] = Character;
  }

  if (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) {
    for (Line = (UINT8) LocalScreen.TopRow; Line < BANNER_HEIGHT + (UINT8) LocalScreen.TopRow; Line++) {
      for (Alignment = 0; Alignment < BANNER_COLUMNS; Alignment++) {
        if (gBannerData->Banner[Line - (UINT8) LocalScreen.TopRow][Alignment] != 0x0000) {
          StrFrontPageBanner = GetToken (
                                 gBannerData->Banner[Line - (UINT8) LocalScreen.TopRow][Alignment],
                                 gFrontPageHandle
                                 );
          if (StrFrontPageBanner == NULL) {
            continue;
          }
        } else {
          continue;
        }

        switch (Alignment) {

        case 0:
          //
          // Handle left column
          //
          PrintStringAt (LocalScreen.LeftColumn, Line, StrFrontPageBanner);
          break;

        case 1:
            //
            // Handle center column
            //
            PrintStringAt (
              (LocalScreen.RightColumn - GetStringWidth(StrFrontPageBanner)/2)/2,
              Line,
              StrFrontPageBanner
              );
            break;

        case 2:
          //
          // Handle right column
          //
          PrintStringAt (
            LocalScreen.RightColumn - (GetStringWidth(StrFrontPageBanner)/2)-1,
            Line,
            StrFrontPageBanner
            );
          break;
        }

        gBS->FreePool (StrFrontPageBanner);
      }
    }
  } else {
    ClearLines (
      LocalScreen.LeftColumn,
      LocalScreen.RightColumn,
      LocalScreen.TopRow,
      LocalScreen.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT - 1,
      TITLE_TEXT | TITLE_BACKGROUND
      );
    //
    // Print Top border line
    // +------------------------------------------------------------------------------+
    // ?                                                                             ?
    // +------------------------------------------------------------------------------+
    //
    Character = BOXDRAW_DOWN_RIGHT;

    PrintChar (Character);
    PrintString (Buffer);

    Character = BOXDRAW_DOWN_LEFT;
    PrintChar (Character);

    Character = BOXDRAW_VERTICAL;
    for (Row = LocalScreen.TopRow + 1; Row <= LocalScreen.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT - 2; Row++) {
      PrintCharAt (LocalScreen.LeftColumn, Row, Character);
      PrintCharAt (LocalScreen.RightColumn - 1, Row, Character);
    }

    Character = BOXDRAW_UP_RIGHT;
    PrintCharAt (LocalScreen.LeftColumn, LocalScreen.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT - 1, Character);
    PrintString (Buffer);

    Character = BOXDRAW_UP_LEFT;
    PrintChar (Character);

    if (!mBootManager) {
      //
      // Print Bottom border line
      // +------------------------------------------------------------------------------+
      // ?                                                                             ?
      // +------------------------------------------------------------------------------+
      //
      Character = BOXDRAW_DOWN_RIGHT;
      PrintCharAt (LocalScreen.LeftColumn, LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT, Character);

      PrintString (Buffer);

      Character = BOXDRAW_DOWN_LEFT;
      PrintChar (Character);
      Character = BOXDRAW_VERTICAL;
      for (Row = LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT + 1;
           Row <= LocalScreen.BottomRow - STATUS_BAR_HEIGHT - 2;
           Row++
          ) {
        PrintCharAt (LocalScreen.LeftColumn, Row, Character);
        PrintCharAt (LocalScreen.RightColumn - 1, Row, Character);
      }

      Character = BOXDRAW_UP_RIGHT;
      PrintCharAt (LocalScreen.LeftColumn, LocalScreen.BottomRow - STATUS_BAR_HEIGHT - 1, Character);

      PrintString (Buffer);

      Character = BOXDRAW_UP_LEFT;
      PrintChar (Character);
    }
  }

  gBS->FreePool (Buffer);
}

/**
 Evaluate all expressions in a Form.

 @param[in] FormSet     FormSet this Form belongs to.
 @param[in] Form        The Form.

 @retval EFI_SUCCESS    The expression evaluated successfuly
**/
EFI_STATUS
EvaluateFormExpressions (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form
  )
{
  EFI_STATUS       Status;
  LIST_ENTRY       *Link;
  FORM_EXPRESSION  *Expression;

  Link = GetFirstNode (&Form->ExpressionListHead);
  while (!IsNull (&Form->ExpressionListHead, Link)) {
    Expression = FORM_EXPRESSION_FROM_LINK (Link);
    Link = GetNextNode (&Form->ExpressionListHead, Link);

    if (Expression->Type == EFI_HII_EXPRESSION_INCONSISTENT_IF ||
        Expression->Type == EFI_HII_EXPRESSION_NO_SUBMIT_IF) {
      //
      // Postpone Form validation to Question editing or Form submiting
      //
      continue;
    }

    Status = EvaluateExpression (FormSet, Form, Expression);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/*
+------------------------------------------------------------------------------+
?                                 Setup Page                                  ?
+------------------------------------------------------------------------------+

















+------------------------------------------------------------------------------+
|                            F9=Reset to Defaults          F10=Save and Exit   |
| ^"=Move Highlight          <Spacebar> Toggles Checkbox   Esc=Discard Changes |
+------------------------------------------------------------------------------+
*/

/**
 Display form and wait for user to select one menu option, then return it.

 @param[in, out] Selection      On input, Selection tell setup browser the information
                                about the Selection, form and formset to be displayed.
                                On output, Selection return the screen item that is selected by user.

 @retval EFI_SUCESSS            This function always return successfully for now.
**/
EFI_STATUS
DisplayForm (
  IN OUT UI_MENU_SELECTION           *Selection
  )
{
  CHAR16                 *StringPtr;
  UINT16                 MenuItemCount;
  EFI_HII_HANDLE         Handle;
  UINT16                 FormId;
  BOOLEAN                Suppress;
  EFI_SCREEN_DESCRIPTOR  LocalScreen;
  UINT16                 Width;
  CHAR16                 *OutputString;
  LIST_ENTRY             *Link;
  FORM_BROWSER_STATEMENT *Statement;
  UINT16                 NumberOfLines;
  UINT16                 NumberOfTexts;
  CHAR16                 *TextStringPtr;
  CHAR16                 *OptionString;
  EFI_STATUS             Status;
  UI_MENU_OPTION         *MenuOption;
  UINTN                  Index;
  UINTN                  CursorPos;
  CHAR16                 BackupPromptBlockWidth;
  CHAR16                 BackupOptionBlockWidth;
  CHAR16                 BackupHelpBlockWidth;

  Handle        = Selection->Handle;
  FormId        = 0;
  MenuItemCount = 0;
  OutputString  = NULL;

  UiInitMenu ();

  CopyMem (&LocalScreen, &gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  StringPtr = GetToken (Selection->Form->FormTitle, Handle);
  if ((gDeviceManagerSetup == NULL) && (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE) && StringPtr != NULL) {
    gST->ConOut->SetAttribute (gST->ConOut, TITLE_TEXT | TITLE_BACKGROUND);
    PrintStringAt (
        (LocalScreen.RightColumn + LocalScreen.LeftColumn - GetStringWidth (StringPtr) / 2) / 2,
        LocalScreen.TopRow + 1,
        StringPtr
        );
  }

  //
  // Display menu here if in the device manager
  //
  if (gDeviceManagerSetup) {
      BackupPromptBlockWidth = gPromptBlockWidth;
      BackupOptionBlockWidth = gOptionBlockWidth;
      BackupHelpBlockWidth   = gHelpBlockWidth;
    //
    // OemServices
    //
    OemSvcUpdateFormLen (
      NULL,
      gDeviceManagerSetup->CurRoot,
      (CHAR8 *)&gPromptBlockWidth,
      (CHAR8 *)&gOptionBlockWidth,
      (CHAR8 *)&gHelpBlockWidth
      );
      if (BackupPromptBlockWidth + BackupOptionBlockWidth + BackupHelpBlockWidth !=
          gPromptBlockWidth + gOptionBlockWidth + gHelpBlockWidth) {
        //
        // if the block width information is incorrect, use the default setting
        //
        DEBUG ((EFI_D_WARN, "The prompt and option block width is incorrect, so using default setting"));
        gPromptBlockWidth = BackupPromptBlockWidth;
        gOptionBlockWidth = BackupOptionBlockWidth;
        gHelpBlockWidth   = BackupHelpBlockWidth;
      }
    if (gDeviceManagerSetup->UseMenus) {
      //
      // Display full menu
      //
      CursorPos = 2;
      for (Index = gDeviceManagerSetup->MenuItemCount; Index > 0 ; Index--) {
        //
        // Clear previous two space
        //
        gST->ConOut->SetAttribute (gST->ConOut, MENU_TEXT_UNSEL | MENU_BACKGROUND_UNSEL);
        PrintCharAt (CursorPos - 2, LocalScreen.TopRow + 1, L' ');
        PrintCharAt (CursorPos - 1, LocalScreen.TopRow + 1, L' ');
        //
        // Check to see if the item is selected
        //
        if (gDeviceManagerSetup->CurRoot == (Index - 1)) {
          gST->ConOut->SetAttribute (gST->ConOut, MENU_TEXT_SEL | MENU_BACKGROUND_SEL);
        } else {
          if (gDeviceManagerSetup->AtRoot) {
            gST->ConOut->SetAttribute (gST->ConOut, MENU_TEXT_UNSEL | MENU_BACKGROUND_UNSEL);
          } else {
            gST->ConOut->SetAttribute (gST->ConOut, MENU_TEXT_SEL | MENU_BACKGROUND_UNSEL);
          }
        }

        //
        // Display string
        //
        PrintStringAt (
          CursorPos,
          LocalScreen.TopRow + 1,
          gDeviceManagerSetup->MenuList[Index - 1].String
          );
        gST->ConOut->SetAttribute (gST->ConOut, MENU_TEXT_UNSEL | MENU_BACKGROUND_UNSEL);


        //
        // Get next cursor position
        //
        CursorPos += ((GetStringWidth (gDeviceManagerSetup->MenuList[Index - 1].String) / 2) + 1);
      }
      //
      // Clear last space
      //
      ClearLines (CursorPos - 2, LocalScreen.RightColumn, LocalScreen.TopRow + 1, LocalScreen.TopRow + 1, MENU_TEXT_UNSEL | MENU_BACKGROUND_UNSEL);
    }
  }

  if ( (gDeviceManagerSetup==NULL) && (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE)) {
    gST->ConOut->SetAttribute (gST->ConOut, HELP_TEXT | FIELD_BACKGROUND);
  }

  //
  // Remove Buffer allocated for StringPtr after it has been used.
  //
  if (StringPtr != NULL) {
    gBS->FreePool (StringPtr);
  }

  //
  // Evaluate all the Expressions in this Form
  //
  Status = EvaluateFormExpressions (Selection->FormSet, Selection->Form);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Selection->FormEditable = FALSE;
  Link = GetFirstNode (&Selection->Form->StatementListHead);
  while (!IsNull (&Selection->Form->StatementListHead, Link)) {
    Statement = FORM_BROWSER_STATEMENT_FROM_LINK (Link);

    if (Statement->SuppressExpression != NULL) {
      Suppress = Statement->SuppressExpression->Result.Value.b;
    } else {
      Suppress = FALSE;
    }

    if (Statement->DisableExpression != NULL) {
      Suppress = Suppress || Statement->DisableExpression->Result.Value.b;
    }

    if (!Suppress) {
      StringPtr = GetToken (Statement->Prompt, Handle);

      Width     = GetWidth (Statement, Handle) - 1;
      NumberOfLines = 0;
      GetLineByWidth (StringPtr, Width, &NumberOfLines, &OutputString);
      ScuSafeFreePool((VOID **)&OutputString);

      NumberOfTexts = 0;
      if (Statement->TextTwo != 0) {
        TextStringPtr = GetToken (Statement->TextTwo, Handle);
        NumberOfTexts = 0;
        GetLineByWidth (TextStringPtr, (UINT16) gOptionBlockWidth, &NumberOfTexts, &OutputString);
        ScuSafeFreePool((VOID **)&OutputString);
        ScuSafeFreePool((VOID **)&TextStringPtr);
      }
      NumberOfLines = MAX (NumberOfTexts, NumberOfLines);

      //
      // We are NOT!! removing this StringPtr buffer via FreePool since it is being used in the menuoptions, we will do
      // it in UiFreeMenu.
      //
      MenuOption = UiAddMenuOption (StringPtr, Selection->Handle, Statement, NumberOfLines, MenuItemCount);
      if (MenuOption == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      ProcessOptions (Selection, MenuOption, FALSE, &OptionString);
      //
      // Doesn't need adjust MenuOption->Skip == 0, it is adjusted for date and time
      //
      if (OptionString != NULL && MenuOption->Skip != 0) {
        NumberOfLines = 0;
        GetLineByWidth (OptionString, (UINT16) gOptionBlockWidth, &NumberOfLines, &OutputString);
        MenuOption->Skip = MAX (MenuOption->Skip, NumberOfLines);
        ScuSafeFreePool ((VOID **)&OutputString);
        ScuSafeFreePool ((VOID **)&OptionString);
      }
      MenuItemCount++;

      if (MenuOption->IsQuestion && !MenuOption->ReadOnly) {
        //
        // At least one item is not readonly, this Form is considered as editable
        //
        Selection->FormEditable = TRUE;
      }
    }

    Link = GetNextNode (&Selection->Form->StatementListHead, Link);
  }

  Status = UiDisplayMenu (Selection);

  UiFreeMenu ();

  return Status;
}

/**
 Initialize the HII String Token to the correct values.
**/
VOID
InitializeBrowserStrings (
  VOID
  )
{
  FreeBrowserStrings ();

  gFunctionNineString   = GetToken (STRING_TOKEN (FUNCTION_NINE_STRING), gHiiHandle);
  gFunctionTenString    = GetToken (STRING_TOKEN (FUNCTION_TEN_STRING), gHiiHandle);
  gEnterString          = GetToken (STRING_TOKEN (ENTER_STRING), gHiiHandle);
  gEnterCommitString    = GetToken (STRING_TOKEN (ENTER_COMMIT_STRING), gHiiHandle);
  gEnterEscapeString    = GetToken (STRING_TOKEN (ENTER_ESCAPE_STRING), gHiiHandle);
  gEscapeString         = GetToken (STRING_TOKEN (ESCAPE_STRING), gHiiHandle);
  gSaveFailed           = GetToken (STRING_TOKEN (SAVE_FAILED), gHiiHandle);
  gMoveHighlight        = GetToken (STRING_TOKEN (MOVE_HIGHLIGHT), gHiiHandle);
  gMakeSelection        = GetToken (STRING_TOKEN (MAKE_SELECTION), gHiiHandle);
  gDecNumericInput      = GetToken (STRING_TOKEN (DEC_NUMERIC_INPUT), gHiiHandle);
  gHexNumericInput      = GetToken (STRING_TOKEN (HEX_NUMERIC_INPUT), gHiiHandle);
  gToggleCheckBox       = GetToken (STRING_TOKEN (TOGGLE_CHECK_BOX), gHiiHandle);
  gPromptForData        = GetToken (STRING_TOKEN (PROMPT_FOR_DATA), gHiiHandle);
  gPromptForPassword    = GetToken (STRING_TOKEN (PROMPT_FOR_PASSWORD), gHiiHandle);
  gPromptForNewPassword = GetToken (STRING_TOKEN (PROMPT_FOR_NEW_PASSWORD), gHiiHandle);
  gConfirmPassword      = GetToken (STRING_TOKEN (CONFIRM_PASSWORD), gHiiHandle);
  gConfirmError         = GetToken (STRING_TOKEN (CONFIRM_ERROR), gHiiHandle);
  gPassowordInvalid     = GetToken (STRING_TOKEN (PASSWORD_INVALID), gHiiHandle);
  gSamePasswordError    = GetToken (STRING_TOKEN (PASSWORD_SAME_ERROR), gHiiHandle);
  gPressEnter           = GetToken (STRING_TOKEN (PRESS_ENTER), gHiiHandle);
  gEmptyString          = GetToken (STRING_TOKEN (EMPTY_STRING), gHiiHandle);
  gAreYouSure           = GetToken (STRING_TOKEN (ARE_YOU_SURE), gHiiHandle);
  gYesResponse          = GetToken (STRING_TOKEN (ARE_YOU_SURE_YES), gHiiHandle);
  gNoResponse           = GetToken (STRING_TOKEN (ARE_YOU_SURE_NO), gHiiHandle);
  gMiniString           = GetToken (STRING_TOKEN (MINI_STRING), gHiiHandle);
  gPlusString           = GetToken (STRING_TOKEN (PLUS_STRING), gHiiHandle);
  gMinusString          = GetToken (STRING_TOKEN (MINUS_STRING), gHiiHandle);
  gAdjustNumber         = GetToken (STRING_TOKEN (ADJUST_NUMBER), gHiiHandle);
  gSaveChanges          = GetToken (STRING_TOKEN (SAVE_CHANGES), gHiiHandle);
  gOptionMismatch       = GetToken (STRING_TOKEN (OPTION_MISMATCH), gHiiHandle);
  gSetupWarning         = GetToken (STRING_TOKEN (SETUP_WARNING_STRING), gHiiHandle);
  gSetupNotice          = GetToken (STRING_TOKEN (SETUP_NOTICE_STRING), gHiiHandle);
  gChangesSaved         = GetToken (STRING_TOKEN (CHANGES_SAVED_STRING), gHiiHandle);
  gContinue             = GetToken (STRING_TOKEN (CONTINUE_STRING), gHiiHandle);
  gPasswordChangesSaved = GetToken (STRING_TOKEN (PASSWORD_CHANGES_SAVED_STRING), gHiiHandle);

  if (gDeviceManagerSetup != NULL) {
    InitializeMenuStrings (gDeviceManagerSetup);
  }

  return ;
}

/**
  Free up the resource allocated for all strings required by Setup Browser.
**/
VOID
FreeBrowserStrings (
  VOID
  )
{
  ScuSafeFreePool ((VOID **)&gFunctionNineString);
  ScuSafeFreePool ((VOID **)&gFunctionTenString);
  ScuSafeFreePool ((VOID **)&gEnterString);
  ScuSafeFreePool ((VOID **)&gEnterCommitString);
  ScuSafeFreePool ((VOID **)&gEnterEscapeString);
  ScuSafeFreePool ((VOID **)&gSaveFailed);
  ScuSafeFreePool ((VOID **)&gEscapeString);
  ScuSafeFreePool ((VOID **)&gMoveHighlight);
  ScuSafeFreePool ((VOID **)&gMakeSelection);
  ScuSafeFreePool ((VOID **)&gDecNumericInput);
  ScuSafeFreePool ((VOID **)&gHexNumericInput);
  ScuSafeFreePool ((VOID **)&gToggleCheckBox);
  ScuSafeFreePool ((VOID **)&gPromptForData);
  ScuSafeFreePool ((VOID **)&gPromptForPassword);
  ScuSafeFreePool ((VOID **)&gPromptForNewPassword);
  ScuSafeFreePool ((VOID **)&gConfirmPassword);
  ScuSafeFreePool ((VOID **)&gPassowordInvalid);
  ScuSafeFreePool ((VOID **)&gSamePasswordError);
  ScuSafeFreePool ((VOID **)&gConfirmError);
  ScuSafeFreePool ((VOID **)&gPressEnter);
  ScuSafeFreePool ((VOID **)&gEmptyString);
  ScuSafeFreePool ((VOID **)&gAreYouSure);
  ScuSafeFreePool ((VOID **)&gYesResponse);
  ScuSafeFreePool ((VOID **)&gNoResponse);
  ScuSafeFreePool ((VOID **)&gMiniString);
  ScuSafeFreePool ((VOID **)&gPlusString);
  ScuSafeFreePool ((VOID **)&gMinusString);
  ScuSafeFreePool ((VOID **)&gAdjustNumber);
  ScuSafeFreePool ((VOID **)&gSaveChanges);
  ScuSafeFreePool ((VOID **)&gOptionMismatch);
  ScuSafeFreePool ((VOID **)&gSetupWarning);
  ScuSafeFreePool ((VOID **)&gSetupNotice);
  ScuSafeFreePool ((VOID **)&gChangesSaved);
  ScuSafeFreePool ((VOID **)&gContinue);
  ScuSafeFreePool ((VOID **)&gPasswordChangesSaved);

  if (gDeviceManagerSetup != NULL) {
    FreeMenuStrings (gDeviceManagerSetup);
  }

  return ;
}

/**
 Update key's help imformation.

 @param[in] Selection      Tell setup browser the information about the Selection
 @param[in] MenuOption     The Menu option
 @param[in] Selected       Whether or not a tag be selected
**/
VOID
UpdateKeyHelp (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected
  )
{
  UINTN                  SecCol;
  UINTN                  ThdCol;
  UINTN                  LeftColumnOfHelp;
  UINTN                  RightColumnOfHelp;
  UINTN                  TopRowOfHelp;
  UINTN                  BottomRowOfHelp;
  UINTN                  StartColumnOfHelp;
  EFI_SCREEN_DESCRIPTOR  LocalScreen;
  FORM_BROWSER_STATEMENT *Statement;

  if ((gDeviceManagerSetup != NULL) ||
      (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) ||
      gSecureBootMgr ||
       mBootManager ) {
    return;
  }

  CopyMem (&LocalScreen, &gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  SecCol            = LocalScreen.LeftColumn + (LocalScreen.RightColumn - LocalScreen.LeftColumn) / 3;
  ThdCol            = LocalScreen.LeftColumn + (LocalScreen.RightColumn - LocalScreen.LeftColumn) * 2 / 3;

  StartColumnOfHelp = LocalScreen.LeftColumn + 2;
  LeftColumnOfHelp  = LocalScreen.LeftColumn + 1;
  RightColumnOfHelp = LocalScreen.RightColumn - 2;
  TopRowOfHelp      = LocalScreen.BottomRow - 4;
  BottomRowOfHelp   = LocalScreen.BottomRow - 3;

  gST->ConOut->SetAttribute (gST->ConOut, KEYHELP_TEXT | KEYHELP_BACKGROUND);

  Statement = MenuOption->ThisTag;
  switch (Statement->Operand) {
  case EFI_IFR_ORDERED_LIST_OP:
  case EFI_IFR_ONE_OF_OP:
  case EFI_IFR_NUMERIC_OP:
  case EFI_IFR_TIME_OP:
  case EFI_IFR_DATE_OP:
    ClearLines (LeftColumnOfHelp, RightColumnOfHelp, TopRowOfHelp, BottomRowOfHelp, KEYHELP_TEXT | KEYHELP_BACKGROUND);

    if (!Selected) {
      if ((gClassOfVfr & FORMSET_CLASS_PLATFORM_SETUP) != 0) {
        if (Selection->FormEditable) {
          PrintStringAt (SecCol, TopRowOfHelp, gFunctionNineString);
          PrintStringAt (ThdCol, TopRowOfHelp, gFunctionTenString);
        }
        PrintStringAt (ThdCol, BottomRowOfHelp, gEscapeString);
      }

      if ((Statement->Operand == EFI_IFR_DATE_OP) ||
          (Statement->Operand == EFI_IFR_TIME_OP)) {
        PrintAt (
          StartColumnOfHelp,
          BottomRowOfHelp,
          L"%c%c%c%c%s",
          ARROW_UP,
          ARROW_DOWN,
          ARROW_RIGHT,
          ARROW_LEFT,
          gMoveHighlight
          );
        PrintStringAt (SecCol, BottomRowOfHelp, gAdjustNumber);
      } else {
        PrintAt (StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
        if (Statement->Operand == EFI_IFR_NUMERIC_OP && Statement->Step != 0) {
          PrintStringAt (SecCol, BottomRowOfHelp, gAdjustNumber);
        } else if (!MenuOption->ReadOnly) {
          PrintStringAt (SecCol, BottomRowOfHelp, gEnterString);
        }
      }
    } else {
      PrintStringAt (SecCol, BottomRowOfHelp, gEnterCommitString);

      //
      // If it is a selected numeric with manual input, display different message
      //
      if ((Statement->Operand == EFI_IFR_NUMERIC_OP) && (Statement->Step == 0)) {
        PrintStringAt (
          SecCol,
          TopRowOfHelp,
          (Statement->Flags & EFI_IFR_DISPLAY_UINT_HEX) ? gHexNumericInput : gDecNumericInput
          );
      } else if (Statement->Operand != EFI_IFR_ORDERED_LIST_OP) {
        PrintAt (StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
      }

      if (Statement->Operand == EFI_IFR_ORDERED_LIST_OP) {
        PrintStringAt (StartColumnOfHelp, TopRowOfHelp, gPlusString);
        PrintStringAt (ThdCol, TopRowOfHelp, gMinusString);
      }

      PrintStringAt (ThdCol, BottomRowOfHelp, gEnterEscapeString);
    }
    break;

  case EFI_IFR_CHECKBOX_OP:
    ClearLines (LeftColumnOfHelp, RightColumnOfHelp, TopRowOfHelp, BottomRowOfHelp, KEYHELP_TEXT | KEYHELP_BACKGROUND);

    if ((gClassOfVfr & FORMSET_CLASS_PLATFORM_SETUP) != 0) {
      if (Selection->FormEditable) {
        PrintStringAt (SecCol, TopRowOfHelp, gFunctionNineString);
        PrintStringAt (ThdCol, TopRowOfHelp, gFunctionTenString);
      }
      PrintStringAt (ThdCol, BottomRowOfHelp, gEscapeString);
    }

    PrintAt (StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
    PrintStringAt (SecCol, BottomRowOfHelp, gToggleCheckBox);
    break;

  case EFI_IFR_REF_OP:
  case EFI_IFR_PASSWORD_OP:
  case EFI_IFR_STRING_OP:
  case EFI_IFR_TEXT_OP:
  case EFI_IFR_ACTION_OP:
  case EFI_IFR_RESET_BUTTON_OP:
    ClearLines (LeftColumnOfHelp, RightColumnOfHelp, TopRowOfHelp, BottomRowOfHelp, KEYHELP_TEXT | KEYHELP_BACKGROUND);

    if (!Selected) {
      if ((gClassOfVfr & FORMSET_CLASS_PLATFORM_SETUP) != 0) {
        if (Selection->FormEditable) {
          PrintStringAt (SecCol, TopRowOfHelp, gFunctionNineString);
          PrintStringAt (ThdCol, TopRowOfHelp, gFunctionTenString);
        }
        PrintStringAt (ThdCol, BottomRowOfHelp, gEscapeString);
      }

      PrintAt (StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
      if (Statement->Operand != EFI_IFR_TEXT_OP) {
        PrintStringAt (SecCol, BottomRowOfHelp, gEnterString);
      }
    } else {
      if (Statement->Operand != EFI_IFR_REF_OP) {
        PrintStringAt (
          (LocalScreen.RightColumn - GetStringWidth (gEnterCommitString) / 2) / 2,
          BottomRowOfHelp,
          gEnterCommitString
          );
        PrintStringAt (ThdCol, BottomRowOfHelp, gEnterEscapeString);
      }
    }
    break;

  default:
    break;
  }
}

/**
 Functions which are registered to receive notification of
 database events have this prototype. The actual event is encoded
 in NotifyType. The following table describes how PackageType,
 PackageGuid, Handle, and Package are used for each of the notification types.

 @param[in] PackageType  Package type of the notification.
 @param[in] PackageGuid  If PackageType is EFI_HII_PACKAGE_TYPE_GUID, then this is
                         the pointer to the GUID from the Guid field of EFI_HII_PACKAGE_GUID_HEADER.
                         Otherwise, it must be NULL.
 @param[in] Package      Points to the package referred to by the
                         notification Handle The handle of the package list which contains the specified package.

 @param[in] Handle       The HII handle.
 @param[in] NotifyType   The type of change concerning the database. See EFI_HII_DATABASE_NOTIFY_TYPE.
**/
EFI_STATUS
EFIAPI
FormUpdateNotify (
  IN UINT8                              PackageType,
  IN CONST EFI_GUID                     *PackageGuid,
  IN CONST EFI_HII_PACKAGE_HEADER       *Package,
  IN EFI_HII_HANDLE                     Handle,
  IN EFI_HII_DATABASE_NOTIFY_TYPE       NotifyType
  )
{
  mHiiPackageListUpdated = TRUE;

  return EFI_SUCCESS;
}

/**
 check whether the formset need to update the NV.

 @param[in] FormSet             FormSet data structure.

 @retval TRUE                   Need to update the NV.
 @retval FALSE                  No need to update the NV.
**/
BOOLEAN
IsNvUpdateRequired (
  IN FORM_BROWSER_FORMSET  *FormSet
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_FORM       *Form;

  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    if (Form->NvUpdateRequired ) {
      return TRUE;
    }

    Link = GetNextNode (&FormSet->FormListHead, Link);
  }

  return FALSE;
}

/**
 The worker function that send the displays to the screen. On output,
 the selection made by user is returned.

 @param[in, out] Selection    On input, Selection tell setup browser the information
                              about the Selection, form and formset to be displayed.
                              On output, Selection return the screen item that is selected by user.

 @retval EFI_SUCCESS    The page is displayed successfully.
 @return Other value if the page failed to be diplayed.
**/
EFI_STATUS
SetupBrowser (
  IN OUT UI_MENU_SELECTION    *Selection
  )
{
  EFI_STATUS                      Status;
  LIST_ENTRY                      *Link;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  EFI_HANDLE                      NotifyHandle;
  EFI_HII_VALUE                   *HiiValue;
  FORM_BROWSER_STATEMENT          *Statement;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  UI_MENU_LIST                    *CurrentMenu;
  BOOLEAN                         HotkeyRequest;
  UINT8                           Index;
  EFI_QUESTION_ID                 HotkeyQuestionId;
  INT32                           BackupAttribute;

  gMenuRefreshHead = NULL;
  gExitRequired = FALSE;
  HiiValue = NULL;

  //
  // Register notify for Form package update
  //
  Status = mHiiDatabase->RegisterPackageNotify (
                           mHiiDatabase,
                           EFI_HII_PACKAGE_FORMS,
                           NULL,
                           FormUpdateNotify,
                           EFI_HII_DATABASE_NOTIFY_REMOVE_PACK,
                           &NotifyHandle
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  do {
    //
    // Initialize Selection->Form
    //
    if (Selection->FormId == 0) {
      //
      // Zero FormId indicates display the first Form in a FormSet
      //
      Link = GetFirstNode (&Selection->FormSet->FormListHead);

      Selection->Form = FORM_BROWSER_FORM_FROM_LINK (Link);
      Selection->FormId = Selection->Form->FormId;
    } else {
      Selection->Form = IdToForm (Selection->FormSet, Selection->FormId);
    }

    if (Selection->Form == NULL) {
      //
      // No Form to display
      //
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

      if (gDeviceManagerSetup->Firstin == TRUE) {
        if (gDeviceManagerSetup->JumpToFirstOption == TRUE) {
          Selection->QuestionId = 0;
          Selection->CurrentRow = 0;
          CurrentMenu = UiFindMenuList (&Selection->FormSetGuid, Selection->FormId);
          if (CurrentMenu != NULL) {
            CurrentMenu->QuestionId = 0;
          }
        }
      }
    }
    HotkeyRequest = FALSE;
    HotkeyQuestionId = 0;

    //
    // Displays the Header and Footer borders
    //
    DisplayPageFrame ();

    if (gDeviceManagerSetup != NULL) {
      gDeviceManagerSetup->JumpToFirstOption = TRUE;
    }

    //
    // Display form
    //
    Status = DisplayForm (Selection);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Check Selected Statement (if press ESC, Selection->Statement will be NULL)
    //
    Statement = Selection->Statement;
    if (Statement != NULL || Selection->NoMenuOption) {
      //
      // Reset FormPackage update flag
      //
      mHiiPackageListUpdated = FALSE;

    for (Index = 0;
         Index < sizeof (gUiHotkeyToQuestionId) / sizeof (gUiHotkeyToQuestionId[0]);
         Index++) {
      if (Selection->Action == gUiHotkeyToQuestionId[Index].Hotkey) {
        HotkeyQuestionId = gUiHotkeyToQuestionId[Index].QuestionId;
        HotkeyRequest = TRUE;
        break;
      }
    }

      if ((Statement != NULL &&
           Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK &&
           Statement->Operand != EFI_IFR_PASSWORD_OP) ||
          HotkeyRequest) {
        ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

        if (!HotkeyRequest) {
          HiiValue = &Statement->HiiValue;
          if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
            //
            // Create String in HII database for Configuration Driver to retrieve
            //
            HiiValue->Value.string = NewString ((CHAR16 *) Statement->BufferValue, Selection->FormSet->HiiHandle);
          }
        }

        ConfigAccess = Selection->FormSet->ConfigAccess;
        if (ConfigAccess == NULL) {
          return EFI_UNSUPPORTED;
        }
        //
        // Set attribute to subtitle attribute to prevent from using wrong attribute to clear popup screen.
        //
        BackupAttribute = gST->ConOut->Mode->Attribute;
        gST->ConOut->SetAttribute (gST->ConOut, SUBTITLE_TEXT | SUBTITLE_BACKGROUND);
        Status = ConfigAccess->Callback (
                                 ConfigAccess,
                                 EFI_BROWSER_ACTION_CHANGING,
                                 HotkeyRequest == TRUE ? HotkeyQuestionId : Statement->QuestionId,
                                 HotkeyRequest ? 0 : HiiValue->Type,
                                 HotkeyRequest ? NULL : &HiiValue->Value,
                                 &ActionRequest
                                 );
        gST->ConOut->SetAttribute (gST->ConOut, BackupAttribute);
        if (!HotkeyRequest &&
            HiiValue->Type == EFI_IFR_TYPE_STRING) {
          //
          // Clean the String in HII Database
          //
          DeleteString (HiiValue->Value.string, Selection->FormSet->HiiHandle);
        }

        if (!EFI_ERROR (Status)) {
          switch (ActionRequest) {
          case EFI_BROWSER_ACTION_REQUEST_RESET:
            Selection->Action = UI_ACTION_EXIT;
            gResetRequired = TRUE;

            gBS->RaiseTPL (TPL_NOTIFY);
            gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

            break;

          case EFI_BROWSER_ACTION_REQUEST_SUBMIT:
            SubmitForm (Selection->FormSet, Selection->Form);
            break;

          case EFI_BROWSER_ACTION_REQUEST_EXIT:
            Selection->Action = UI_ACTION_EXIT;
            gExitRequired = TRUE;
            gNvUpdateRequired = FALSE;
            break;

          default:
            break;
          }
        } else if (Status != EFI_UNSUPPORTED && Statement != NULL) {
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
      }
    }
    if (Selection->Action != UI_ACTION_EXIT && HotkeyRequest) {
      Selection->Action = UI_ACTION_REFRESH_FORM;
    }
  } while (Selection->Action == UI_ACTION_REFRESH_FORM);

  //
  // Unregister notify for Form package update
  //
  Status = mHiiDatabase->UnregisterPackageNotify (
                           mHiiDatabase,
                           NotifyHandle
                           );

  return Status;
}

