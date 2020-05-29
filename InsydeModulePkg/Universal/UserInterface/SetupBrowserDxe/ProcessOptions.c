/** @file
 User input functions for UI.

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

#include "Ui.h"
#include "Setup.h"
#include "SetupUtility.h"

/**
 Process Question Config.

 @param[in] Selection           The UI menu selection.
 @param[in] Question            The Question to be peocessed.

 @retval EFI_SUCCESS            Question Config process success.
 @retval Other                  Question Config process fail.
**/
EFI_STATUS
ProcessQuestionConfig (
  IN  UI_MENU_SELECTION       *Selection,
  IN  FORM_BROWSER_STATEMENT  *Question
  )
{
  EFI_STATUS                      Status;
  CHAR16                          *ConfigResp;
  CHAR16                          *Progress;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;

  if (Question->QuestionConfig == 0) {
    return EFI_SUCCESS;
  }

  //
  // Get <ConfigResp>
  //
  ConfigResp = GetToken (Question->QuestionConfig, Selection->FormSet->HiiHandle);
  if (ConfigResp == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Send config to Configuration Driver
  //
  ConfigAccess = Selection->FormSet->ConfigAccess;
  if (ConfigAccess == NULL) {
    return EFI_UNSUPPORTED;
  }
  Status = ConfigAccess->RouteConfig (
                           ConfigAccess,
                           ConfigResp,
                           &Progress
                           );

  return Status;
}

/**
 Search an Option of a Question by its value.

 @param[in] Question            The Question
 @param[in] OptionValue         Value for Option to be searched.

 @retval Pointer                Pointer to the found Option.
 @retval NULL                   Option not found.
**/
QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  )
{
  LIST_ENTRY       *Link;
  QUESTION_OPTION  *Option;
  INTN             Result;

  Link = GetFirstNode (&Question->OptionListHead);
  while (!IsNull (&Question->OptionListHead, Link)) {
    Option = QUESTION_OPTION_FROM_LINK (Link);

    if ((CompareHiiValue (&Option->Value, OptionValue, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
      return Option;
    }

    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  return NULL;
}

/**
 Return data element in an Array by its Index.

 @param[in] Array               The data array.
 @param[in] Type                Type of the data in this array.
 @param[in] Index               Zero based index for data in this array.

 @retval Value                  The data to be returned
**/
UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index
  )
{
  UINT64 Data;

  ASSERT (Array != NULL);

  Data = 0;
  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    Data = (UINT64) *(((UINT8 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Data = (UINT64) *(((UINT16 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Data = (UINT64) *(((UINT32 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    Data = (UINT64) *(((UINT64 *) Array) + Index);
    break;

  default:
    break;
  }

  return Data;
}

/**
 Set value of a data element in an Array by its Index.

 @param[in] Array                  The data array.
 @param[in] Type                   Type of the data in this array.
 @param[in] Index                  Zero based index for data in this array.
 @param[in] Value                  The value to be set.
**/
VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index,
  IN UINT64                   Value
  )
{

  ASSERT (Array != NULL);

  switch (Type) {

  case EFI_IFR_TYPE_NUM_SIZE_8:
    *(((UINT8 *) Array) + Index) = (UINT8) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    *(((UINT16 *) Array) + Index) = (UINT16) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    *(((UINT32 *) Array) + Index) = (UINT32) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    *(((UINT64 *) Array) + Index) = (UINT64) Value;
    break;

  default:
    break;
  }
}

/**
 Print Question Value according to it's storage width and display attributes.

 @param[in]      Question               The Question to be printed.
 @param[in, out] FormattedNumber        Buffer for output string.
 @param[in]      BufferSize             The FormattedNumber buffer size in bytes.

 @retval EFI_SUCCESS            Print success.
 @retval EFI_BUFFER_TOO_SMALL   Buffer size is not enough for formatted number.
**/
EFI_STATUS
PrintFormattedNumber (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN OUT CHAR16               *FormattedNumber,
  IN UINTN                    BufferSize
  )
{
  INT64          Value;
  CHAR16         *Format;
  EFI_HII_VALUE  *QuestionValue;

  if (BufferSize < (21 * sizeof (CHAR16))) {
    return EFI_BUFFER_TOO_SMALL;
  }

  QuestionValue = &Question->HiiValue;

  Value = (INT64) QuestionValue->Value.u64;
  switch (Question->Flags & EFI_IFR_DISPLAY) {

  case EFI_IFR_DISPLAY_INT_DEC:
    switch (QuestionValue->Type) {

    case EFI_IFR_NUMERIC_SIZE_1:
      Value = (INT64) ((INT8) QuestionValue->Value.u8);
      break;

    case EFI_IFR_NUMERIC_SIZE_2:
      Value = (INT64) ((INT16) QuestionValue->Value.u16);
      break;

    case EFI_IFR_NUMERIC_SIZE_4:
      Value = (INT64) ((INT32) QuestionValue->Value.u32);
      break;

    case EFI_IFR_NUMERIC_SIZE_8:
    default:
      break;
    }

    if (Value < 0) {
      Value = -Value;
      Format = L"-%ld";
    } else {
      Format = L"%ld";
    }
    break;

  case EFI_IFR_DISPLAY_UINT_DEC:
    Format = L"%ld";
    break;

  case EFI_IFR_DISPLAY_UINT_HEX:
    Format = L"0x%lx";
    break;

  default:
    return EFI_UNSUPPORTED;
    break;
  }

  UnicodeSPrint (FormattedNumber, BufferSize, Format, Value);

  return EFI_SUCCESS;
}

/**
 Password may be stored as encrypted by Configuration Driver. When change a
 password, user will be challenged with old password. To validate user input old
 password, we will send the clear text to Configuration Driver via Callback().
 Configuration driver is responsible to check the passed in password and return
 the validation result. If validation pass, state machine in password Callback()
 will transit from BROWSER_STATE_VALIDATE_PASSWORD to BROWSER_STATE_SET_PASSWORD.
 After user type in new password twice, Callback() will be invoked to send the
 new password to Configuration Driver.

 @param[in] Selection           Pointer to UI_MENU_SELECTION.
 @param[in] MenuOption          The MenuOption for this password Question.
 @param[in] String              The clear text of password.

 @retval EFI_NOT_AVAILABLE_YET  Callback() request to terminate password input.
 @return In state of BROWSER_STATE_VALIDATE_PASSWORD:
 @retval EFI_SUCCESS            Password correct, Browser will prompt for new
                                password.
 @retval EFI_NOT_READY          Password incorrect, Browser will show error
                                message.
 @retval Other                  Browser will do nothing.
 @return In state of BROWSER_STATE_SET_PASSWORD:
 @retval EFI_SUCCESS            Set password success.
 @retval Other                  Set password failed.
**/
EFI_STATUS
PasswordCallback (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  CHAR16                      *String
  )
{
  EFI_STATUS                      Status;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  EFI_HII_VALUE                   *QuestionValue;

  QuestionValue = &MenuOption->ThisTag->HiiValue;
  ConfigAccess = Selection->FormSet->ConfigAccess;
  if (ConfigAccess == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Prepare password string in HII database
  //
  if (String != NULL) {
    if (QuestionValue->Value.string == 0) {
      QuestionValue->Value.string = NewString (String, Selection->FormSet->HiiHandle);
    } else {
      SetString (QuestionValue->Value.string, String, Selection->FormSet->HiiHandle);
    }
  } else {
    QuestionValue->Value.string = 0;
  }

  //
  // Send password to Configuration Driver for validation
  //
  Status = ConfigAccess->Callback (
                           ConfigAccess,
                           EFI_BROWSER_ACTION_CHANGING,
                           MenuOption->ThisTag->QuestionId,
                           QuestionValue->Type,
                           &QuestionValue->Value,
                           &ActionRequest
                           );

  //
  // Remove password string from HII database
  //
  if (String != NULL) {
    DeleteString (QuestionValue->Value.string, Selection->FormSet->HiiHandle);
  }

  return Status;
}

/**
 Display error message for invalid password.
**/
VOID
PasswordInvalid (
  VOID
  )
{
  EFI_INPUT_KEY  Key;

  //
  // Invalid password, prompt error message
  //
  do {
    CreateDialog (4, TRUE, 0, NULL, &Key, gEmptyString, gPassowordInvalid, gPressEnter, gEmptyString);
  } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
}

/**
 According OptionItemAction to change the selection of option item.

 @param[in] Selection           Pointer to UI_MENU_SELECTION.
 @param[in] MenuOption          The MenuOption for this Question.
 @param[in] OptionItemAction    Value to save option item action.

 @retval EFI_SUCCESS            Do option item action successful.
 @retval Other                  Do option item action fail.
**/
EFI_STATUS
ChangeItemSelection (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  UINTN                       OptionItemAction
  )
{
  FORM_BROWSER_STATEMENT          *Question;
  QUESTION_OPTION                 *OneOfOption;
  EFI_HII_VALUE                   *HiiValueArray;
  LIST_ENTRY                      *Link;
  EFI_STATUS                      Status;
  UINTN                           Index;
  UINTN                           OptionCount;
  UINTN                           PopUpMenuLines;
  UINTN                           HighlightOptionIndex;
  INTN                            Result;

  if (OptionItemAction == OPTION_ITEM_NO_ACTION) {
    return EFI_SUCCESS;
  }

  if (OptionItemAction != OPTION_ITEM_UP && OptionItemAction != OPTION_ITEM_DOWN) {
    return EFI_INVALID_PARAMETER;
  }

  Question = MenuOption->ThisTag;
  OptionCount = 0;
  Link = GetFirstNode (&Question->OptionListHead);
  while (!IsNull (&Question->OptionListHead, Link)) {
    OneOfOption = QUESTION_OPTION_FROM_LINK (Link);

    OptionCount++;

    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  //
  // Prepare HiiValue array
  //
  HiiValueArray = AllocateZeroPool (OptionCount * sizeof (EFI_HII_VALUE));
  ASSERT (HiiValueArray != NULL);
  Link = GetFirstNode (&Question->OptionListHead);
  for (Index = 0; Index < OptionCount; Index++) {
    OneOfOption = QUESTION_OPTION_FROM_LINK (Link);
    CopyMem (&HiiValueArray[Index], &OneOfOption->Value, sizeof (EFI_HII_VALUE));
    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  //
  // Move Suppressed Option to list tail
  //
  PopUpMenuLines = 0;
  for (Index = 0; Index < OptionCount; Index++) {
    OneOfOption = ValueToOption (Question, &HiiValueArray[OptionCount - Index - 1]);
    if (OneOfOption == NULL) {
      return EFI_NOT_FOUND;
    }

    RemoveEntryList (&OneOfOption->Link);

    if ((OneOfOption->SuppressExpression == NULL) ||
        !(OneOfOption->SuppressExpression->Result.Value.b)) {

      //
      // Insert to head
      //
      InsertHeadList (&Question->OptionListHead, &OneOfOption->Link);

      PopUpMenuLines++;
    }
  }

  //
  // Get the number of one of options present and its size
  //
  HighlightOptionIndex = 0;
  Link = GetFirstNode (&Question->OptionListHead);
  for (Index = 0; Index < PopUpMenuLines; Index++) {
    OneOfOption = QUESTION_OPTION_FROM_LINK (Link);

    if ((CompareHiiValue (&Question->HiiValue, &OneOfOption->Value, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
      //
      // Find current selected Option for OneOf
      //
      break;
    }
    HighlightOptionIndex++;
    Link = GetNextNode (&Question->OptionListHead, Link);
  }
  //
  // According OptionItemAction to change the item selection
  // OptionItemAction = OPTION_ITEM_UP ==> up the item selection
  // OptionItemAction = OPTION_ITEM_DOWN ==> down the item selection
  //
  if (OptionItemAction == OPTION_ITEM_UP) {
    if (HighlightOptionIndex == 0) {
      for (Index = 1; Index < PopUpMenuLines; Index++) {
        Link = GetNextNode (&Question->OptionListHead, Link);
      }
    } else {
      Link = Link->BackLink;
    }
  } else {
    if (HighlightOptionIndex == PopUpMenuLines - 1) {
      Link = GetFirstNode (&Question->OptionListHead);
    } else {
      Link = GetNextNode (&Question->OptionListHead, Link);
    }
  }

  OneOfOption = QUESTION_OPTION_FROM_LINK (Link);
  CopyMem (&Question->HiiValue, &OneOfOption->Value, sizeof (EFI_HII_VALUE));
  gBS->FreePool (HiiValueArray);
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

  return Status;
}

/**
 Process a Question's Option (whether selected or un-selected).

 @param[in]  Selection          Pointer to UI_MENU_SELECTION.
 @param[in]  MenuOption         The MenuOption for this Question.
 @param[in]  Selected           TRUE: if Question is selected.
 @param[out] OptionString       Pointer of the Option String to be displayed.

 @retval EFI_SUCCESS            Question Option process success.
 @retval Other                  Question Option process fail.
**/
EFI_STATUS
ProcessOptions (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected,
  OUT CHAR16                      **OptionString
  )
{
  EFI_STATUS                      Status;
  CHAR16                          *StringPtr;
  CHAR16                          *TempString;
  UINTN                           Index;
  FORM_BROWSER_STATEMENT          *Question;
  CHAR16                          FormattedNumber[21];
  UINT16                          Number;
  CHAR16                          Character[2];
  EFI_INPUT_KEY                   Key;
  UINTN                           BufferSize;
  QUESTION_OPTION                 *OneOfOption;
  LIST_ENTRY                      *Link;
  EFI_HII_VALUE                   HiiValue;
  EFI_HII_VALUE                   *QuestionValue;
  BOOLEAN                         Suppress;
  UINT16                          Minimum;
  UINT16                          Maximum;
  QUESTION_OPTION                 *Option;
  UINTN                           Index2;
  UINT8                           *ValueArray;
  UINT8                           ValueType;
  BOOLEAN                         CheckOldPassword;
  UINTN                           LineNum;
  BOOLEAN                         UnlockPassword;

  Status        = EFI_SUCCESS;
  StringPtr     = NULL;
  Character[1]  = L'\0';
  *OptionString = NULL;

  ZeroMem (FormattedNumber, 21 * sizeof (CHAR16));
  if (FeaturePcdGet (PcdGraphicsSetupSupported)) {
    BufferSize = 2000;
  } else {
    BufferSize = (gOptionBlockWidth + 1) * 2 * gScreenDimensions.BottomRow;
  }
  Question = MenuOption->ThisTag;
  QuestionValue = &Question->HiiValue;
  Minimum = (UINT16) Question->Minimum;
  Maximum = (UINT16) Question->Maximum;

  ValueArray = Question->BufferValue;
  ValueType = Question->ValueType;

  switch (Question->Operand) {

  case EFI_IFR_ORDERED_LIST_OP:
    //
    // Check whether there are Options of this OrderedList
    //
    if (IsListEmpty (&Question->OptionListHead)) {
      break;
    }

    //
    // Initialize Option value array
    //
    if (GetArrayData (ValueArray, ValueType, 0) == 0) {
      GetQuestionDefault (Selection->FormSet, Selection->Form, Question, 0);
    }

    if (Selected) {
      //
      // Go ask for input
      //
      Status = GetSelectionInputPopUp (Selection, MenuOption);
    } else {
      //
      // We now know how many strings we will have, so we can allocate the
      // space required for the array or strings.
      //
      *OptionString = AllocateZeroPool (Question->MaxContainers * BufferSize);
      ASSERT (*OptionString);

      HiiValue.Type = ValueType;
      HiiValue.Value.u64 = 0;
      for (Index = 0; Index < Question->MaxContainers; Index++) {
        HiiValue.Value.u64 = GetArrayData (ValueArray, ValueType, Index);
        if (HiiValue.Value.u64 == 0) {
          //
          // Values for the options in ordered lists should never be a 0
          //
          break;
        }

        OneOfOption = ValueToOption (Question, &HiiValue);
        if (OneOfOption == NULL) {
          //
          // Show error message
          //
          do {
            CreateDialog (4, TRUE, 0, NULL, &Key, gEmptyString, gOptionMismatch, gPressEnter, gEmptyString);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

          //
          // The initial value of the orderedlist is invalid, force to be valid value
          //
          Link = GetFirstNode (&Question->OptionListHead);
          Index2 = 0;
          while (!IsNull (&Question->OptionListHead, Link) && Index2 < Question->MaxContainers) {
            Option = QUESTION_OPTION_FROM_LINK (Link);
            SetArrayData (ValueArray, ValueType, Index2, Option->Value.Value.u64);
            Index2++;
            Link = GetNextNode (&Question->OptionListHead, Link);
          }
          SetArrayData (ValueArray, ValueType, Index2, 0);

          Status = SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
          UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);

          gBS->FreePool (*OptionString);
          *OptionString = NULL;
          return EFI_NOT_FOUND;
        }

        Suppress = FALSE;
        if ((OneOfOption->SuppressExpression != NULL) &&
            (OneOfOption->SuppressExpression->Result.Value.b)) {
          //
          // This option is suppressed
          //
          Suppress = TRUE;
        }

        if (!Suppress) {
          Character[0] = LEFT_ONEOF_DELIMITER;
          NewStrCat (OptionString[0], Character);
          StringPtr = GetToken (OneOfOption->Text, Selection->Handle);
          if (StringPtr == NULL) {
            return EFI_NOT_FOUND;
          }
          NewStrCat (OptionString[0], StringPtr);
          Character[0] = RIGHT_ONEOF_DELIMITER;
          NewStrCat (OptionString[0], Character);
          Character[0] = CHAR_CARRIAGE_RETURN;
          NewStrCat (OptionString[0], Character);

          gBS->FreePool (StringPtr);
        }
      }
    }
    break;

  case EFI_IFR_ONE_OF_OP:
    //
    // Check whether there are Options of this OneOf
    //
    if (IsListEmpty (&Question->OptionListHead)) {
      break;
    }

    if (Selected) {
      //
      // Go ask for input
      //
      Status = GetSelectionInputPopUp (Selection, MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      OneOfOption = ValueToOption (Question, QuestionValue);
      if (OneOfOption == NULL) {
        //
        // Show error message
        //
        do {
          CreateDialog (4, TRUE, 0, NULL, &Key, gEmptyString, gOptionMismatch, gPressEnter, gEmptyString);
        } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

        //
        // Force the Question value to be valid
        //
        Link = GetFirstNode (&Question->OptionListHead);
        while (!IsNull (&Question->OptionListHead, Link)) {
          Option = QUESTION_OPTION_FROM_LINK (Link);

          if ((Option->SuppressExpression == NULL) ||
              !Option->SuppressExpression->Result.Value.b) {
            CopyMem (QuestionValue, &Option->Value, sizeof (EFI_HII_VALUE));
            SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
            UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
            break;
          }

          Link = GetNextNode (&Question->OptionListHead, Link);
        }

        gBS->FreePool (*OptionString);
        *OptionString = NULL;
        return EFI_NOT_FOUND;
      }

      if ((OneOfOption->SuppressExpression != NULL) &&
          (OneOfOption->SuppressExpression->Result.Value.b)) {
        //
        // This option is suppressed
        //
        Suppress = TRUE;
      } else {
        Suppress = FALSE;
      }

      if (Suppress) {
        //
        // Current selected option happen to be suppressed,
        // enforce to select on a non-suppressed option
        //
        Link = GetFirstNode (&Question->OptionListHead);
        while (!IsNull (&Question->OptionListHead, Link)) {
          OneOfOption = QUESTION_OPTION_FROM_LINK (Link);

          if ((OneOfOption->SuppressExpression == NULL) ||
              !OneOfOption->SuppressExpression->Result.Value.b) {
            Suppress = FALSE;
            CopyMem (QuestionValue, &OneOfOption->Value, sizeof (EFI_HII_VALUE));
            SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
            UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
            gST->ConOut->SetAttribute (gST->ConOut, FIELD_TEXT | FIELD_BACKGROUND);
            break;
          }

          Link = GetNextNode (&Question->OptionListHead, Link);
        }
      }

      if (!Suppress) {
        Character[0] = LEFT_ONEOF_DELIMITER;
        NewStrCat (OptionString[0], Character);
        StringPtr = GetToken (OneOfOption->Text, Selection->Handle);
        if (StringPtr == NULL) {
          return EFI_NOT_FOUND;
        }
        NewStrCat (OptionString[0], StringPtr);
        Character[0] = RIGHT_ONEOF_DELIMITER;
        NewStrCat (OptionString[0], Character);

        gBS->FreePool (StringPtr);
      }
    }
    break;

  case EFI_IFR_CHECKBOX_OP:
    *OptionString = AllocateZeroPool (BufferSize);
    ASSERT (*OptionString);

    *OptionString[0] = LEFT_CHECKBOX_DELIMITER;

    if (Selected) {
      //
      // Since this is a BOOLEAN operation, flip it upon selection
      //
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
        gBS->FreePool (*OptionString);
        *OptionString = NULL;
        return Status;
      }

      //
      // Save Question value
      //
      Status = SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);
      UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
    }

    if (QuestionValue->Value.b) {
      *(OptionString[0] + 1) = CHECK_ON;
    } else {
      *(OptionString[0] + 1) = CHECK_OFF;
    }
    *(OptionString[0] + 2) = RIGHT_CHECKBOX_DELIMITER;
    break;

  case EFI_IFR_NUMERIC_OP:
    if (Selected) {
      //
      // Go ask for input
      //
      Status = GetNumericInput (Selection, MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      *OptionString[0] = LEFT_NUMERIC_DELIMITER;

      //
      // Formatted print
      //
      PrintFormattedNumber (Question, FormattedNumber, 21 * sizeof (CHAR16));
      Number = (UINT16) GetStringWidth (FormattedNumber);
      if (FeaturePcdGet (PcdGraphicsSetupSupported)) {
        CopyMem (*OptionString, FormattedNumber, Number);
      } else {
        *OptionString[0] = LEFT_NUMERIC_DELIMITER;
        CopyMem (OptionString[0] + 1, FormattedNumber, Number);
        *(OptionString[0] + Number / 2) = RIGHT_NUMERIC_DELIMITER;
      }
    }
    break;

  case EFI_IFR_DATE_OP:
    if (Selected) {
      //
      // This is similar to numerics
      //
      Status = GetNumericInput (Selection, MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize + 200);
      ASSERT (*OptionString);

      if (FeaturePcdGet (PcdGraphicsSetupSupported)) {
        UnicodeSPrint (*OptionString, 200, L"%02d/%02d/%4d",
          (UINTN) QuestionValue->Value.date.Month,
          (UINTN) QuestionValue->Value.date.Day,
          (UINTN) QuestionValue->Value.date.Year
          );
      } else {
      switch (MenuOption->Sequence) {
      case 0:
        *OptionString[0] = LEFT_NUMERIC_DELIMITER;
        UnicodeSPrint (OptionString[0] + 1, 21 * sizeof (CHAR16), L"%02d", (UINTN) QuestionValue->Value.date.Month);
        *(OptionString[0] + 3) = DATE_SEPARATOR;
        break;

      case 1:
        SetUnicodeMem (OptionString[0], 4, L' ');
        UnicodeSPrint (OptionString[0] + 4, 21 * sizeof (CHAR16), L"%02d", (UINTN) QuestionValue->Value.date.Day);
        *(OptionString[0] + 6) = DATE_SEPARATOR;
        break;

      case 2:
        SetUnicodeMem (OptionString[0], 7, L' ');
        UnicodeSPrint (OptionString[0] + 7, 21 * sizeof (CHAR16), L"%4d", (UINTN) QuestionValue->Value.date.Year);
        *(OptionString[0] + 11) = RIGHT_NUMERIC_DELIMITER;
        break;
      }
    }
    }
    break;

  case EFI_IFR_TIME_OP:
    if (Selected) {
      //
      // This is similar to numerics
      //
      Status = GetNumericInput (Selection, MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize + 200);
      ASSERT (*OptionString);

      if (FeaturePcdGet (PcdGraphicsSetupSupported)) {
        UnicodeSPrint (*OptionString, 200, L"%02d:%02d:%02d",
          (UINTN) QuestionValue->Value.time.Hour,
          (UINTN) QuestionValue->Value.time.Minute,
          (UINTN) QuestionValue->Value.time.Second
          );
      } else {
      switch (MenuOption->Sequence) {
      case 0:
        *OptionString[0] = LEFT_NUMERIC_DELIMITER;
        UnicodeSPrint (OptionString[0] + 1, 21 * sizeof (CHAR16), L"%02d", (UINTN) QuestionValue->Value.time.Hour);
        *(OptionString[0] + 3) = TIME_SEPARATOR;
        break;

      case 1:
        SetUnicodeMem (OptionString[0], 4, L' ');
        UnicodeSPrint (OptionString[0] + 4, 21 * sizeof (CHAR16), L"%02d", (UINTN) QuestionValue->Value.time.Minute);
        *(OptionString[0] + 6) = TIME_SEPARATOR;
        break;

      case 2:
        SetUnicodeMem (OptionString[0], 7, L' ');
        UnicodeSPrint (OptionString[0] + 7, 21 * sizeof (CHAR16), L"%02d", (UINTN) QuestionValue->Value.time.Second);
        *(OptionString[0] + 9) = RIGHT_NUMERIC_DELIMITER;
        break;
        }
      }
    }
    break;

  case EFI_IFR_STRING_OP:
    if (Selected) {
      StringPtr = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
      ASSERT (StringPtr);

      Status = ReadString (MenuOption, gPromptForData, StringPtr);
      if (!EFI_ERROR (Status)) {
        CopyMem (Question->BufferValue, StringPtr, Maximum * sizeof (CHAR16));
        SetQuestionValue (Selection->FormSet, Selection->Form, Question, TRUE);

        UpdateStatusBar (NV_UPDATE_REQUIRED, Question->QuestionFlags, TRUE);
      }

      gBS->FreePool (StringPtr);
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      if (((CHAR16 *) Question->BufferValue)[0] == 0x0000) {
        if (!FeaturePcdGet (PcdGraphicsSetupSupported)) {
        *(OptionString[0]) = '_';
        }
      } else {
        if ((Maximum * sizeof (CHAR16)) < BufferSize) {
          BufferSize = Maximum * sizeof (CHAR16);
        }
        CopyMem (OptionString[0], (CHAR16 *) Question->BufferValue, BufferSize);
      }
    }
    break;

  case EFI_IFR_PASSWORD_OP:
    if (Selected) {
      StringPtr = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
      ASSERT (StringPtr != NULL);
      if (StringPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // For interactive passwords, old password is validated by callback
      //
      LineNum = 1;
      CheckOldPassword = FALSE;
      if (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) {
        //
        // Use a NULL password to test whether old password is required
        //
        *StringPtr = 0;
        UnlockPassword = FALSE;
        Status = PasswordCallback (Selection, MenuOption, StringPtr);
        if (Status == EFI_NOT_AVAILABLE_YET) {
          //
          // Callback request to terminate password input
          //
          gBS->FreePool (StringPtr);
          return EFI_SUCCESS;
        }

        if (EFI_ERROR (Status)) {

          if (Status != EFI_CRC_ERROR) {
            //
            // Old password exist, ask user for the old password
            //
            Status = ReadPassword (MenuOption, ENTER_OLD_PASSWORD, LineNum++, TRUE, StringPtr);
          } else {
            //
            // Unlock password
            //
            UnlockPassword = TRUE;
            Status = ReadPassword (MenuOption, UNLOCK_PASSWORD, LineNum++, TRUE, StringPtr);
          }
          CheckOldPassword = TRUE;
          if (EFI_ERROR (Status)) {
            gBS->FreePool (StringPtr);
            return Status;
          }

          //
          // Check user input old password
          //
          Status = PasswordCallback (Selection, MenuOption, StringPtr);
          if (UnlockPassword) {
            gBS->FreePool (StringPtr);
            if (Status == EFI_NOT_READY) {
              CreatePasswordDialog (MenuOption, EFI_NOT_READY);
              if (PcdGetBool (PcdReturnDialogCycle)) {
                Selection->SelectAgain = TRUE;
              }
            }
            return EFI_SUCCESS;
          } else if (EFI_ERROR (Status)) {
            if (Status == EFI_NOT_READY) {
//              //
//              // Typed in old password incorrect
//              //
//              PasswordInvalid ();
              CreatePasswordDialog (MenuOption, EFI_NOT_READY);
              if (PcdGetBool (PcdReturnDialogCycle)) {
                Selection->SelectAgain = TRUE;
              }
            } else {
              Status = EFI_SUCCESS;
            }

            gBS->FreePool (StringPtr);
            return Status;
          }
        }
      } else {
        //
        // For non-interactive password, validate old password in local
        //
        if (*((CHAR16 *) Question->BufferValue) != 0) {
          //
          // There is something there!  Prompt for password
          //
          Status = ReadPassword (MenuOption, ENTER_OLD_PASSWORD, LineNum++, TRUE, StringPtr);
          CheckOldPassword = TRUE;
          if (EFI_ERROR (Status)) {
            gBS->FreePool (StringPtr);
            return Status;
          }

          TempString = AllocateCopyPool ((Maximum + 1) * sizeof (CHAR16), Question->BufferValue);
          TempString[Maximum] = L'\0';

          if (StrCmp (StringPtr, TempString) != 0) {

            gBS->FreePool (StringPtr);
            gBS->FreePool (TempString);
            return Status;
          }

          gBS->FreePool (TempString);
        }
      }

      //
      // Ask for new password
      //
      ZeroMem (StringPtr, (Maximum + 1) * sizeof (CHAR16));
      Status = ReadPassword (MenuOption, ENTER_NEW_PASSWORD, LineNum++, !CheckOldPassword, StringPtr);
      if (EFI_ERROR (Status)) {
        //
        // Reset state machine for interactive password
        //
        if (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) {
          PasswordCallback (Selection, MenuOption, NULL);
        }

        gBS->FreePool (StringPtr);
        return Status;
      }

      //
      // Confirm new password
      //
      TempString = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
      ASSERT (TempString != NULL);
      if (TempString == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      Status = ReadPassword (MenuOption, CONFIRM_NEW_PASSWORD, LineNum++, FALSE, TempString);
      if (EFI_ERROR (Status)) {
        //
        // Reset state machine for interactive password
        //
        if (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) {
          PasswordCallback (Selection, MenuOption, NULL);
        }

        gBS->FreePool (StringPtr);
        gBS->FreePool (TempString);
        return Status;
      }

      //
      // Compare two typed-in new passwords
      //
      if (StrCmp (StringPtr, TempString) == 0) {
        Status = EFI_SUCCESS;
        //
        // Two password match, send it to Configuration Driver
        //
        if (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) {
          Status = PasswordCallback (Selection, MenuOption, StringPtr);
        } else {
          CopyMem (Question->BufferValue, StringPtr, Maximum * sizeof (CHAR16));
          SetQuestionValue (Selection->FormSet, Selection->Form, Question, FALSE);
        }
        CreatePasswordDialog (MenuOption, Status);
      } else {
        //
        // Reset state machine for interactive password
        //
        if (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) {
          PasswordCallback (Selection, MenuOption, NULL);
        }
        CreatePasswordDialog (MenuOption, EFI_INVALID_PARAMETER);
        if (PcdGetBool (PcdReturnDialogCycle)) {
          Selection->SelectAgain = TRUE;
        }
      }

      gBS->FreePool (TempString);
      gBS->FreePool (StringPtr);
    }
    break;

  default:
    break;
  }

  return Status;
}

/**
 For parsing a sting, check this character if satisfy the condition of a actual word.

 @param[in] StringPtr           Input character

 @retval TRUE                   it satisfy the condition of a actual word.
 @retval FALSE                  it doesn't sataisfy the condition of a actual word.
**/
BOOLEAN
CheckUnicode(
  IN  CHAR16                  StringPtr
  )
{
  if (StringPtr >= 0x3001 ||
      StringPtr == 0x0009 ||
      StringPtr == 0x0020 ||
      StringPtr == 0x0028 ||
      StringPtr == 0x0029 ||
      StringPtr == 0x002F ||
      StringPtr == 0x005C ||
      StringPtr == 0x0000 ||
      StringPtr == 0x3000 ) {
    return TRUE;
  }

  return FALSE;
}

/**
 Process the help string: Split StringPtr to several lines of strings stored in
 FormattedString and the glyph width of each line cannot exceed gHelpBlockWidth.

 @param[in]  StringPtr              The entire help string.
 @param[out] FormattedString        The output formatted string.
 @param[in]  RowCount               TRUE: if Question is selected.
**/
VOID
ProcessHelpString (
  IN  CHAR16  *StringPtr,
  OUT CHAR16  **FormattedString,
  IN  UINTN   RowCount
  )
{
  CONST UINTN BlockWidth = (UINTN) gHelpBlockWidth - 1;
  UINTN AllocateSize;
  //
  // [PrevCurrIndex, CurrIndex) forms a range of a screen-line
  //
  UINTN CurrIndex;
  UINTN PrevCurrIndex;
  UINTN LineCount;
  UINTN VirtualLineCount;
  //
  // GlyphOffset stores glyph width of current screen-line
  //
  UINTN GlyphOffset;
  //
  // GlyphWidth equals to 2 if we meet width directive
  //
  UINTN GlyphWidth;
  //
  // during scanning, we remember the position of last space character
  // in case that if next word cannot put in current line, we could restore back to the position
  // of last space character
  // while we should also remmeber the glyph width of the last space character for restoring
  //
  UINTN LastSpaceIndex;
  UINTN LastSpaceGlyphWidth;
  //
  // every time we begin to form a new screen-line, we should remember glyph width of single character
  // of last line
  //
  UINTN LineStartGlyphWidth;
  UINTN *IndexArray;
  UINTN *OldIndexArray;
  UINT32                   Width;

  //
  // every three elements of IndexArray form a screen-line of string:[ IndexArray[i*3], IndexArray[i*3+1] )
  // IndexArray[i*3+2] stores the initial glyph width of single character. to save this is because we want
  // to bring the width directive of the last line to current screen-line.
  // e.g.: "\wideabcde ... fghi", if "fghi" also has width directive but is splitted to the next screen-line
  // different from that of "\wideabcde", we should remember the width directive.
  //
  AllocateSize  = 0x20;
  IndexArray    = AllocatePool (AllocateSize * sizeof (UINTN) * 3);
  ASSERT (IndexArray != NULL);
  if (IndexArray == NULL) {
    return ;
  }

  if (*FormattedString != NULL) {
    gBS->FreePool (*FormattedString);
    *FormattedString = NULL;
  }

  for (PrevCurrIndex = 0, CurrIndex  = 0, LineCount   = 0, LastSpaceIndex = 0,
       IndexArray[0] = 0, GlyphWidth = 1, GlyphOffset = 0, LastSpaceGlyphWidth = 1, LineStartGlyphWidth = 1;
       (StringPtr[CurrIndex] != CHAR_NULL);
       CurrIndex ++) {

    if (LineCount == AllocateSize) {
      AllocateSize += 0x10;
      OldIndexArray  = IndexArray;
      IndexArray = AllocatePool (AllocateSize * sizeof (UINTN) * 3);
      if (IndexArray == NULL) {
        return ;
      }
      CopyMem (IndexArray, OldIndexArray, LineCount * sizeof (UINTN) * 3);
      gBS->FreePool (OldIndexArray);
    }
    switch (StringPtr[CurrIndex]) {

    case NARROW_CHAR:
    case WIDE_CHAR:
      GlyphWidth = ((StringPtr[CurrIndex] == WIDE_CHAR) ? 2 : 1);
      if (CurrIndex == 0) {
        LineStartGlyphWidth = GlyphWidth;
      }
      break;

    //
    // char is '\n'
    // "\r\n" isn't handled here, handled by case CHAR_CARRIAGE_RETURN
    //
    case CHAR_LINEFEED:
      //
      // Store a range of string as a line
      //
      IndexArray[LineCount*3]   = PrevCurrIndex;
      IndexArray[LineCount*3+1] = CurrIndex;
      IndexArray[LineCount*3+2] = LineStartGlyphWidth;
      LineCount ++;
      //
      // Reset offset and save begin position of line
      //
      GlyphOffset = 0;
      LineStartGlyphWidth = GlyphWidth;
      PrevCurrIndex = CurrIndex + 1;
      break;

    //
    // char is '\r'
    // "\r\n" and "\r" both are handled here
    //
    case CHAR_CARRIAGE_RETURN:
      if (StringPtr[CurrIndex + 1] == CHAR_LINEFEED) {
        //
        // next char is '\n'
        //
        IndexArray[LineCount*3]   = PrevCurrIndex;
        IndexArray[LineCount*3+1] = CurrIndex;
        IndexArray[LineCount*3+2] = LineStartGlyphWidth;
        LineCount ++;
        CurrIndex ++;
      }
      GlyphOffset = 0;
      LineStartGlyphWidth = GlyphWidth;
      PrevCurrIndex = CurrIndex + 1;
      break;

    //
    // char is space or other char
    //
    default:
      GlyphOffset     += GlyphWidth;
      if (GlyphWidth == 1) {
        //
        // If character belongs to wide glyph, set the right glyph offset.
        //
        Width = ConsoleLibGetGlyphWidth (StringPtr[CurrIndex]);
        if (Width == 2) {
          GlyphOffset++;
        }
      }
      //
      // LastSpaceIndex: remember position of last space
      //
      if (CheckUnicode (StringPtr[CurrIndex])) {
        LastSpaceIndex      = CurrIndex;
        LastSpaceGlyphWidth = GlyphWidth;
      }
      if (GlyphOffset >= BlockWidth) {
        if (LastSpaceIndex > PrevCurrIndex && !CheckUnicode (StringPtr[CurrIndex]) && !CheckUnicode (StringPtr[CurrIndex + 1])) {
          //
          // LastSpaceIndex points to space inside current screen-line,
          // restore to LastSpaceIndex
          // (Otherwise the word is too long to fit one screen-line, just cut it)
          //
          CurrIndex  = LastSpaceIndex;
          GlyphWidth = LastSpaceGlyphWidth;
        } else if (GlyphOffset > BlockWidth) {
          //
          // the word is too long to fit one screen-line and we don't get the chance
          // of GlyphOffset == BlockWidth because GlyphWidth = 2
          //
          CurrIndex --;
        }

        IndexArray[LineCount*3]   = PrevCurrIndex;
        IndexArray[LineCount*3+1] = CurrIndex + 1;
        IndexArray[LineCount*3+2] = LineStartGlyphWidth;
        LineStartGlyphWidth = GlyphWidth;
        LineCount ++;
        //
        // Reset offset and save begin position of line
        //
        GlyphOffset                 = 0;
        PrevCurrIndex               = CurrIndex + 1;
      }
      break;
    }
  }

  if (GlyphOffset > 0) {
    IndexArray[LineCount*3]   = PrevCurrIndex;
    IndexArray[LineCount*3+1] = CurrIndex;
    IndexArray[LineCount*3+2] = LineStartGlyphWidth;
    LineCount ++;
  }

  if (LineCount == 0) {
    //
    // in case we meet null string
    //
    IndexArray[0] = 0;
    IndexArray[1] = 1;
    //
    // we assume null string's glyph width is 1
    //
    IndexArray[2] = 1;
    LineCount ++;
  }

  VirtualLineCount = RowCount * (LineCount / RowCount + (LineCount % RowCount > 0));
  *FormattedString = AllocateZeroPool (VirtualLineCount * (BlockWidth + 1) * sizeof (CHAR16) * 2);
  ASSERT (*FormattedString != NULL);
  if (*FormattedString == NULL) {
    return ;
  }

  for (CurrIndex = 0; CurrIndex < LineCount; CurrIndex ++) {
    *(*FormattedString + CurrIndex * 2 * (BlockWidth + 1)) = (IndexArray[CurrIndex*3+2] == 2) ? WIDE_CHAR : NARROW_CHAR;
    StrnCpy (
      *FormattedString + CurrIndex * 2 * (BlockWidth + 1) + 1,
      StringPtr + IndexArray[CurrIndex*3],
      IndexArray[CurrIndex*3+1]-IndexArray[CurrIndex*3]
      );
  }

  gBS->FreePool (IndexArray);
}

