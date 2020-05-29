/** @file
  Functions for H2O display engine driver.

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
#include <DEDialog.h>
#include <DEControl.h>

EFI_STATUS
InitLayout (
  VOID
  );

STATIC
BOOLEAN
IsHexStr (
  IN CHAR16   *Str
  )
{
  //
  // skip preceeding white space
  //
  while ((*Str != 0) && *Str == L' ') {
    Str++;
  }
  //
  // skip preceeding zeros
  //
  while ((*Str != 0) && *Str == L'0') {
    Str++;
  }

  return (BOOLEAN) (*Str == L'x' || *Str == L'X');
}

UINT64
GetBufferValue (
  IN     UINT8                                *Buffer,
  IN     UINT8                                Type,
  IN     UINT32                               Index
  )
{
  UINT8                                       *Ptr8;
  UINT16                                      *Ptr16;
  UINT32                                      *Ptr32;
  UINT64                                      *Ptr64;
  UINT64                                      Value;

  Value = 0;
  if (Type == EFI_IFR_TYPE_NUM_SIZE_8) {
    Ptr8 = (UINT8 *) Buffer;
    Value = (UINT64) Ptr8[Index];
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_16) {
    Ptr16 = (UINT16 *) Buffer;
    Value = (UINT64) Ptr16[Index];
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_32) {
    Ptr32 = (UINT32 *) Buffer;
    Value = (UINT64) Ptr32[Index];
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_64) {
    Ptr64 = (UINT64 *) Buffer;
    Value = (UINT64) Ptr64[Index];
  }

  return Value;
}

EFI_STATUS
SetBufferValue (
  IN OUT UINT8                                *Buffer,
  IN     UINT8                                Type,
  IN     UINT32                               Index,
  IN     UINT64                               Value
  )
{
  UINT8                                       *Ptr8;
  UINT16                                      *Ptr16;
  UINT32                                      *Ptr32;
  UINT64                                      *Ptr64;

  if (Type == EFI_IFR_TYPE_NUM_SIZE_8) {
    Ptr8 = (UINT8 *) Buffer;
    Ptr8[Index] = (UINT8) Value;
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_16) {
    Ptr16 = (UINT16 *) Buffer;
    Ptr16[Index] = (UINT16) Value;
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_32) {
    Ptr32 = (UINT32 *) Buffer;
    Ptr32[Index] = (UINT32) Value;
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_64) {
    Ptr64 = (UINT64 *) Buffer;
    Ptr64[Index] = (UINT64) Value;
  } else {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

CHAR16 *
HiiValueToStr (
  IN     EFI_HII_VALUE                   *HiiValue,
  IN     BOOLEAN                         IsHex
  )
{
  CHAR16                                 *ValueString;

  switch (HiiValue->Type) {
    case EFI_IFR_TYPE_NUM_SIZE_8:
      if (IsHex) {
        ValueString = CatSPrint (NULL, L"0x%x", HiiValue->Value.u8);
      } else {
        ValueString = CatSPrint (NULL, L"%d", HiiValue->Value.u8);
      }
      break;

    case EFI_IFR_TYPE_NUM_SIZE_16:
      if (IsHex) {
        ValueString = CatSPrint (NULL, L"0x%x", HiiValue->Value.u16);
      } else {
        ValueString = CatSPrint (NULL, L"%d", HiiValue->Value.u16);
      }
      break;

    case EFI_IFR_TYPE_NUM_SIZE_32:
      if (IsHex) {
        ValueString = CatSPrint (NULL, L"0x%x", HiiValue->Value.u32);
      } else {
        ValueString = CatSPrint (NULL, L"%d", HiiValue->Value.u32);
      }
      break;

    case EFI_IFR_TYPE_NUM_SIZE_64:
      if (IsHex) {
        ValueString = CatSPrint (NULL, L"0x%x", HiiValue->Value.u64);
      } else {
        ValueString = CatSPrint (NULL, L"%d", HiiValue->Value.u64);
      }
      break;

    case EFI_IFR_TYPE_BOOLEAN:
      if (HiiValue->Value.b) {
        ValueString = CatSPrint (NULL, L"TRUE");
      } else {
        ValueString = CatSPrint (NULL, L"FALSE");
      }
      break;

    case EFI_IFR_TYPE_TIME:
      ValueString = CatSPrint (
                      NULL,
                      L"%02d:%02d:%02d",
                      HiiValue->Value.time.Hour,
                      HiiValue->Value.time.Minute,
                      HiiValue->Value.time.Second
                      );
      break;

    case EFI_IFR_TYPE_DATE:
      ValueString = CatSPrint (
                      NULL,
                      L"%04d/%02d/%02d",
                      HiiValue->Value.date.Year,
                      HiiValue->Value.date.Month,
                      HiiValue->Value.date.Day
                      );
      break;

    case EFI_IFR_TYPE_STRING:
      if (HiiValue->Buffer != NULL && ((UINTN) HiiValue->Buffer & BIT0) == 0) {
        ValueString = CatSPrint (NULL, (CHAR16*)HiiValue->Buffer);
      } else {
        ValueString = CatSPrint (NULL, L"");
      }
      break;

    case EFI_IFR_TYPE_ACTION:
    case EFI_IFR_TYPE_BUFFER:
    case EFI_IFR_TYPE_REF:
    default:
      ValueString = CatSPrint (NULL, L"");
      break;
  }

  return ValueString;
}

CHAR16 *
GetQuestionValueStr (
  IN     H2O_FORM_BROWSER_Q                   *Question
  )
{
  CHAR16                                      *QuestionValueString = NULL;

  H2O_FORM_BROWSER_O                          *Option;
  UINT32                                      Index;
  UINT32                                      ContainerIndex;
  UINT32                                      OptionIndex;
  UINT64                                      Value;
  UINT32                                      ValuePanelWidth;
  CHAR16                                      *OptionStr;
  CHAR16                                      *TempStr;
  LIST_ENTRY                                  *PanelLink;
  H2O_PANEL_INFO                              *Panel;


  switch (Question->Operand) {

    case EFI_IFR_SUBTITLE_OP:
    case EFI_IFR_REF_OP:
    case EFI_IFR_RESET_BUTTON_OP:
      QuestionValueString = CatSPrint (NULL, L" ");
      break;

    case EFI_IFR_TEXT_OP:
    case EFI_IFR_ACTION_OP:
      if (Question->TextTwo != NULL && ((UINTN) Question->TextTwo & BIT0) == 0) {
        QuestionValueString = CatSPrint (NULL, L"%s", Question->TextTwo);
      } else {
        QuestionValueString = CatSPrint (NULL, L" ");
      }
      break;

    case EFI_IFR_ONE_OF_OP:
      Option = NULL;
      for (Index = 0; Index < Question->NumberOfOptions; Index++) {
        Option = &(Question->Options[Index]);
        if ((Option->HiiValue.Type == Question->HiiValue.Type) &&
            (Option->HiiValue.Value.u64 == Question->HiiValue.Value.u64)) {
          break;
        }
      }
      if (Index != Question->NumberOfOptions && Option != NULL) {
        QuestionValueString = AllocateCopyPool (StrSize (Option->Text), Option->Text);
      } else {
        QuestionValueString = AllocateZeroPool (sizeof (CHAR16));
      }
      break;

    case EFI_IFR_ORDERED_LIST_OP:
      PanelLink = &mDEPrivate->Layout->PanelListHead;
      if (IsNull (PanelLink, PanelLink->ForwardLink)) {
        QuestionValueString = CatSPrint (NULL, L" ");
        break;
      }

      ValuePanelWidth = 10;
      do {
        PanelLink = PanelLink->ForwardLink;
        Panel = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);
        if (Panel->PanelType == H2O_PANEL_TYPE_SETUP_PAGE) {
          ValuePanelWidth = (UINT32)(Panel->PanelField.right - Panel->PanelField.left) / 2;
          break;
        }
      } while (!IsNodeAtEnd (&mDEPrivate->Layout->PanelListHead, PanelLink));

      OptionStr = AllocateZeroPool (sizeof (CHAR16) * ValuePanelWidth);
      if (OptionStr == NULL) {
        return NULL;
      }
      QuestionValueString = CatSPrint (NULL, L"");
      for (ContainerIndex = 0; ContainerIndex < Question->ContainerCount; ContainerIndex ++) {
        for (OptionIndex = 0; OptionIndex < Question->NumberOfOptions; OptionIndex++) {
          Option = &(Question->Options[OptionIndex]);
          Value = GetBufferValue (Question->HiiValue.Buffer, Option->HiiValue.Type, ContainerIndex);
          if (Option->Visibility && Option->HiiValue.Value.u64 == Value) {
            for (Index = 0; Index < (ValuePanelWidth - 1); Index ++) {
              OptionStr [Index] = ' ';
            }
            if (StrLen (Option->Text) < (ValuePanelWidth - 5)) {
              CopyMem (OptionStr, Option->Text, sizeof (CHAR16) * StrLen (Option->Text));
            } else {
              CopyMem (OptionStr, Option->Text, sizeof (CHAR16) * (ValuePanelWidth - 7));
              TempStr = OptionStr;
              OptionStr = CatSPrint (OptionStr, L"..");
              FreePool (TempStr);
            }

            TempStr = QuestionValueString;
            QuestionValueString = CatSPrint (QuestionValueString, L"%s", OptionStr);
            FreePool (TempStr);
            break;
          }
        }
      }
      FreePool (OptionStr);
      break;

    case EFI_IFR_NUMERIC_OP:
      if ((Question->Flags & EFI_IFR_DISPLAY_UINT_HEX) == EFI_IFR_DISPLAY_UINT_HEX) {
        QuestionValueString = HiiValueToStr (&Question->HiiValue, TRUE);
      } else {
        QuestionValueString = HiiValueToStr (&Question->HiiValue, FALSE);
      }
      break;

    default:
      QuestionValueString = HiiValueToStr (&Question->HiiValue, FALSE);
  }

  if (QuestionValueString == NULL) {
    QuestionValueString = HiiValueToStr (&Question->HiiValue, FALSE);
  }

  return QuestionValueString;
}

EFI_STATUS
ExchangeContainerValue (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN OUT UINT8                                *Buffer,
  IN     UINT8                                Type,
  IN     EFI_HII_VALUE                        *HiiValue1,
  IN     EFI_HII_VALUE                        *HiiValue2
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *Controls = NULL;
  UINT32                                      Index1;
  UINT32                                      Index2;

  H2O_CONTROL_INFO                            *OrgControl1;
  H2O_CONTROL_INFO                            *OrgControl2;


  if (HiiValue1 == NULL || HiiValue2 == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = GetPopUpSelectionByValue (PopUpControls, HiiValue1, &Controls, &Index1);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  Status = GetPopUpSelectionByValue (PopUpControls, HiiValue2, &Controls, &Index2);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (Index1 >= PopUpControls->Count || Index2 >= PopUpControls->Count) {
    return EFI_NOT_FOUND;
  }

  //
  // Exchange buffer
  //
  Status = SetBufferValue (Buffer, Type, Index1, HiiValue2->Value.u64);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  Status = SetBufferValue (Buffer, Type, Index2, HiiValue1->Value.u64);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Exchange controls
  //
  OrgControl1 = AllocateCopyPool (sizeof (H2O_CONTROL_INFO), &PopUpControls->ControlArray[Index1]);
  if (OrgControl1 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  OrgControl2 = AllocateCopyPool (sizeof (H2O_CONTROL_INFO), &PopUpControls->ControlArray[Index2]);
  if (OrgControl2 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (&PopUpControls->ControlArray[Index1], OrgControl2, sizeof (H2O_CONTROL_INFO));
  CopyMem (&PopUpControls->ControlArray[Index2], OrgControl1, sizeof (H2O_CONTROL_INFO));

  PopUpControls->ControlArray[Index1].ControlField.top = OrgControl1->ControlField.top;
  PopUpControls->ControlArray[Index1].ControlField.bottom   = OrgControl1->ControlField.top + (OrgControl2->ControlField.bottom - OrgControl2->ControlField.top);

  PopUpControls->ControlArray[Index2].ControlField.top = OrgControl2->ControlField.top;
  PopUpControls->ControlArray[Index2].ControlField.bottom   = OrgControl2->ControlField.top + (OrgControl1->ControlField.bottom - OrgControl1->ControlField.top);

  FreePool (OrgControl1);
  FreePool (OrgControl2);

  return EFI_SUCCESS;
}

BOOLEAN
CheckInputDec (
  IN     EFI_INPUT_KEY                       *Key,
  IN OUT UINT64                              *Value
  )
{
  if (Key->UnicodeChar >= '0' && Key->UnicodeChar <= '9') {
    *Value = (UINT64) (Key->UnicodeChar - '0');
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
CheckInputHex (
  IN     EFI_INPUT_KEY                       *Key,
  IN OUT UINT64                              *Value
  )
{
  if (Key->UnicodeChar >= '0' && Key->UnicodeChar <= '9') {
    *Value = (UINT64) (Key->UnicodeChar - '0');
    return TRUE;
  }
  if (Key->UnicodeChar >= 'a' && Key->UnicodeChar <= 'f') {
    *Value = (UINT64) (Key->UnicodeChar - 'a');
    return TRUE;
  }
  if (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'F') {
    *Value = (UINT64) (Key->UnicodeChar - 'A');
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
CheckInputDecChar (
  IN     EFI_INPUT_KEY                       *Key,
  IN OUT CHAR16                              *Char
  )
{
  if (Key->UnicodeChar >= '0' && Key->UnicodeChar <= '9') {
    //
    // Basic visiable unicode
    //
    *Char = Key->UnicodeChar;
    return TRUE;
  }

  *Char = 0;

  return FALSE;
}

BOOLEAN
CheckInputHexChar (
  IN     EFI_INPUT_KEY                       *Key,
  IN OUT CHAR16                              *Char
  )
{
  if ((Key->UnicodeChar >= '0' && Key->UnicodeChar <= '9') ||
     (Key->UnicodeChar >= 'a' && Key->UnicodeChar <= 'f') ||
     (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'F') ){
    //
    // Basic visiable unicode
    //
    *Char = Key->UnicodeChar;
    return TRUE;
  }

  *Char = 0;

  return FALSE;
}

BOOLEAN
IsVisibleInputKey (
  IN EFI_INPUT_KEY                           *Key
  )
{
  if (Key->UnicodeChar >= ' ' && Key->UnicodeChar <= '~') {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
InitializeVirtualKb (
  VOID
  )
{
  EFI_STATUS                                  Status;
  EFI_SETUP_MOUSE_PROTOCOL                    *SetupMouse;
  LIST_ENTRY                                  *Link;
  KEYBOARD_ATTRIBUTES                         KeyboardAttributes;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE           *ConsoleDevNode;
  UINTN                                       X;
  UINTN                                       Y;

  Status = gBS->LocateProtocol (&gSetupMouseProtocolGuid, NULL, (VOID **) &SetupMouse);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SetupMouse->GetKeyboardAttributes (SetupMouse, &KeyboardAttributes);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (KeyboardAttributes.IsStart) {
    return EFI_SUCCESS;
  }

  KeyboardAttributes.WidthPercentage  = 100;
  KeyboardAttributes.HeightPercentage = 40;
  SetupMouse->SetKeyboardAttributes (SetupMouse, &KeyboardAttributes);

  Link = mDEPrivate->ConsoleDevListHead.ForwardLink;
  ConsoleDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
  Status = ConsoleDevNode->SimpleTextOut->QueryMode (
                                            ConsoleDevNode->SimpleTextOut,
                                            ConsoleDevNode->SimpleTextOut->Mode->Mode,
                                            &X,
                                            &Y
                                            );
  if (!EFI_ERROR (Status)) {
    Y = (Y * EFI_GLYPH_HEIGHT * (100 - KeyboardAttributes.HeightPercentage)) / 100;
  } else {
    Y = 0;
  }

  return SetupMouse->StartKeyboard (SetupMouse, 0, Y);
}

EFI_STATUS
ShutdownVirtualKb (
  VOID
  )
{
  EFI_STATUS                                  Status;
  EFI_SETUP_MOUSE_PROTOCOL                    *SetupMouse;
  KEYBOARD_ATTRIBUTES                         KeyboardAttributes;

  Status = gBS->LocateProtocol (&gSetupMouseProtocolGuid, NULL, (VOID **) &SetupMouse);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SetupMouse->GetKeyboardAttributes (SetupMouse, &KeyboardAttributes);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!KeyboardAttributes.IsStart) {
    return EFI_SUCCESS;
  }

  return SetupMouse->CloseKeyboard (SetupMouse);
}

EFI_STATUS
ProcessNumberInput (
  IN     H2O_CONTROL_LIST                    *PopUpControls,
  IN     BOOLEAN                             IsHex,
  IN     EFI_INPUT_KEY                       *Key,
  IN OUT UINT32                              *TabIndex,
  OUT    BOOLEAN                             *IsEnterVisibleChar,
  IN OUT CHAR16                              **ResultStr
  )
{
  CHAR16                                     InputChar;
  BOOLEAN                                    InputNumber;
  UINT32                                     TempTabIndex;
  BOOLEAN                                    TempIsEnterVisibleChar;
  CHAR16                                     *TempStr = NULL;
  CHAR16                                     *Ptr;
  UINT64                                     TempValue;


  SafeFreePool ((VOID **) ResultStr);

  TempTabIndex           = (TabIndex != NULL) ? *TabIndex : 0;
  TempIsEnterVisibleChar = TRUE;
  GetInputWithTab (PopUpControls, TempTabIndex, &TempStr);
  InputNumber            = IsHex ? CheckInputHexChar (Key, &InputChar) : CheckInputDecChar (Key, &InputChar);

  if (InputNumber) {
    //
    // Press visible char
    //
    Ptr = TempStr;
    TempStr = CatSPrint (TempStr, L"%c", InputChar);
    FreePool (Ptr);
  } else if (Key->UnicodeChar == CHAR_BACKSPACE) {
    //
    // Press backspace
    //
    TempStr[StrLen(TempStr) - 1] = '\0';
  } else if (TabIndex != NULL && (Key->UnicodeChar == CHAR_TAB || Key->UnicodeChar == CHAR_CARRIAGE_RETURN)) {
    //
    // Press tab
    //
    if (TempTabIndex < GetInputMaxTabNumber (PopUpControls)) {
      TempTabIndex ++;
    } else {
      TempTabIndex = 0;
    }
    GetInputWithTab (PopUpControls, TempTabIndex, &TempStr);
  } else if (Key->UnicodeChar == CHAR_SUB || Key->UnicodeChar == CHAR_ADD) {
    //
    // Press '+' or '-'
    //
    if (IsHex) {
      TempValue = StrHexToUint64 (TempStr);
    } else {
      TempValue = StrDecimalToUint64 (TempStr);
    }
    FreePool (TempStr);
    if (Key->UnicodeChar == CHAR_SUB) {
      TempValue --;
    } else {
      TempValue ++;
    }
    if (IsHex) {
      TempStr = CatSPrint (NULL, L"%X", TempValue);
    } else {
      TempStr = CatSPrint (NULL, L"%d", TempValue);
    }
  } else {
    //
    // Not press visible char or backspace or tab
    //
    TempIsEnterVisibleChar = FALSE;
  }

  if (TabIndex != NULL) {
    *TabIndex = TempTabIndex;
  }
  *IsEnterVisibleChar = TempIsEnterVisibleChar;
  *ResultStr = TempStr;

  return EFI_SUCCESS;
}

EFI_STATUS
ProcessCharInput (
  IN     H2O_CONTROL_LIST                    *PopUpControls,
  IN     EFI_INPUT_KEY                       *Key,
  IN OUT UINT32                              *TabIndex,
  OUT    BOOLEAN                             *IsEnterVisibleChar,
  IN OUT CHAR16                              **ResultStr
  )
{
  UINT32                                     TempTabIndex;
  BOOLEAN                                    TempIsEnterVisibleChar;
  CHAR16                                     *TempStr = NULL;

  SafeFreePool ((VOID **) ResultStr);

  TempTabIndex = (TabIndex != NULL) ? *TabIndex : 0;

  TempIsEnterVisibleChar = TRUE;
  GetInputWithTab (PopUpControls, TempTabIndex, &TempStr);

  if (IsVisibleInputKey (Key)) {
    TempStr = CatSPrint (TempStr, L"%c", Key->UnicodeChar);
  } else if (Key->UnicodeChar == CHAR_BACKSPACE) {
    //
    // Press backspace
    //
    TempStr[StrLen(TempStr) - 1] = '\0';
  } else if (TabIndex != NULL && (Key->UnicodeChar == CHAR_TAB || Key->UnicodeChar == CHAR_CARRIAGE_RETURN)) {
    //
    // Press tab
    //
    if (TempTabIndex < GetInputMaxTabNumber (PopUpControls)) {
      TempTabIndex++;
    } else if (Key->UnicodeChar == CHAR_TAB) {
      TempTabIndex = 0;
    } else {
      //
      // End of tab index and press CHAR_CARRIAGE_RETURN
      //
      TempTabIndex = 0;
      TempIsEnterVisibleChar = FALSE;
    }
    GetInputWithTab (PopUpControls, TempTabIndex, &TempStr);
  } else {
    //
    // Not press visible char or backspace or tab
    //
    TempIsEnterVisibleChar = FALSE;
  }

  if (TabIndex != NULL) {
    *TabIndex = TempTabIndex;
  }

  *IsEnterVisibleChar = TempIsEnterVisibleChar;
  *ResultStr          = TempStr;

  return EFI_SUCCESS;
}

EFI_STATUS
ChangeQuestionValueByStep (
  IN  BOOLEAN                                 Increase,
  IN  H2O_FORM_BROWSER_Q                      *Question,
  OUT EFI_HII_VALUE                           *HiiValue
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_O                          *Option;
  UINT32                                      Index;
  UINT64                                      EditValue;
  UINT64                                      Step;
  UINT64                                      Maximum;
  UINT64                                      Minimum;
  BOOLEAN                                     IsLoop;

  Status = EFI_SUCCESS;

  switch (Question->Operand) {

  case EFI_IFR_NUMERIC_OP:
    if (Question->Step == 0) {
      return EFI_UNSUPPORTED;
    }

    EditValue = Question->HiiValue.Value.u64;
    Maximum   = Question->Maximum;
    Minimum   = Question->Minimum;
    Step      = Question->Step;
    IsLoop    = FALSE;

    if (Increase) {
      if (EditValue + Step <= Maximum) {
         EditValue = EditValue + Step;
       } else if (EditValue < Maximum) {
         EditValue = Maximum;
       } else {
         if (!IsLoop) {
           return EFI_UNSUPPORTED;
         }

         EditValue = Minimum;
       }
    } else {
      if (EditValue >= Minimum + Step) {
        EditValue = EditValue - Step;
      } else if (EditValue > Minimum){
        EditValue = Minimum;
      } else {
        if (!IsLoop) {
          return EFI_UNSUPPORTED;
        }

        EditValue = Maximum;
      }
    }

    CopyMem (HiiValue, &Question->HiiValue, sizeof (EFI_HII_VALUE));
    HiiValue->Value.u64 = EditValue;
    break;

  case EFI_IFR_ONE_OF_OP:
    Option = NULL;
    for (Index = 0; Index < Question->NumberOfOptions; Index++) {
      Option = &(Question->Options[Index]);
      if ((Option->HiiValue.Type == Question->HiiValue.Type) &&
          (Option->HiiValue.Value.u64 == Question->HiiValue.Value.u64)) {
        break;
      }
    }
    if (Index == Question->NumberOfOptions) {
      Status = EFI_NOT_FOUND;
    }

    if (Increase) {
      if (Index < (Question->NumberOfOptions - 1)) {
        CopyMem (HiiValue, &Question->Options[Index + 1].HiiValue, sizeof (EFI_HII_VALUE));
      } else {
        CopyMem (HiiValue, &Question->Options[0].HiiValue, sizeof (EFI_HII_VALUE));
      }
    } else {
      if (Index > 0) {
        CopyMem (HiiValue, &Question->Options[Index - 1].HiiValue, sizeof (EFI_HII_VALUE));
      } else {
        CopyMem (HiiValue, &Question->Options[Question->NumberOfOptions - 1].HiiValue, sizeof (EFI_HII_VALUE));
      }
    }
    break;

  default:
    Status = EFI_NOT_FOUND;
    break;
  }

  return Status;
}

VOID
LetDateRegular (
  IN OUT EFI_HII_VALUE                     *HiiValue
  )
{
  //
  // Year
  //
  if (HiiValue->Value.date.Year < 1) {
    HiiValue->Value.date.Year = 3000;
  }

  //
  // Month
  //
  if (HiiValue->Value.date.Month < 1) {
    HiiValue->Value.date.Month = 12;
  } else if (HiiValue->Value.date.Month > 12) {
    HiiValue->Value.date.Month = 1;
  }

  //
  // Day
  //
  switch (HiiValue->Value.date.Month) {
    case 2:
      if (HiiValue->Value.date.Year / 4 == 0) {
        if (HiiValue->Value.date.Day < 1) {
          HiiValue->Value.date.Day = 29;
        } else if (HiiValue->Value.date.Day > 29) {
          HiiValue->Value.date.Day = 1;
        }
      } else {
        if (HiiValue->Value.date.Day < 1) {
          HiiValue->Value.date.Day = 28;
        } else if (HiiValue->Value.date.Day > 28) {
          HiiValue->Value.date.Day = 1;
        }
      }
      break;

    case 4:
    case 6:
    case 9:
    case 11:
      if (HiiValue->Value.date.Day < 1) {
        HiiValue->Value.date.Day = 30;
      } else if (HiiValue->Value.date.Day > 30) {
        HiiValue->Value.date.Day = 1;
      }
      break;

    default:
      if (HiiValue->Value.date.Day < 1) {
        HiiValue->Value.date.Day = 31;
      } else if (HiiValue->Value.date.Day > 31) {
        HiiValue->Value.date.Day = 1;
      }
      break;
  }
}

VOID
LetTimeRegular (
  IN OUT EFI_HII_VALUE                       *HiiValue
  )
{
  //
  // Hour
  //
  if (HiiValue->Value.time.Hour == (UINT8) (-1)) {
    HiiValue->Value.time.Hour = 23;
  } else if (HiiValue->Value.time.Hour > 23) {
    HiiValue->Value.time.Hour = 0;
  }

  //
  // Minute
  //
  if (HiiValue->Value.time.Minute == (UINT8) (-1)) {
    HiiValue->Value.time.Minute = 59;
  } else if (HiiValue->Value.time.Minute > 59) {
    HiiValue->Value.time.Minute = 0;
  }

  //
  // Second
  //
  if (HiiValue->Value.time.Second == (UINT8) (-1)) {
    HiiValue->Value.time.Second = 59;
  } else if (HiiValue->Value.time.Second > 59) {
    HiiValue->Value.time.Second = 0;
  }
}

EFI_STATUS
CreateValueControls (
  IN H2O_FORM_BROWSER_Q                       *Question,
  IN H2O_CONTROL_LIST                         *PromptControls,
  IN H2O_CONTROL_LIST                         *ValueControls,
  IN H2O_CONTROL_LIST                         *PopUpControls
  )
{
  UINT32                                      Index;
  RECT                                        ValueField;
  UINT32                                      ContainerIndex;
  UINT32                                      OptionIndex;
  H2O_FORM_BROWSER_O                          *Option;
  UINT64                                      ContainerIndexValue;
  CHAR16                                      *ControlStr;
  UINT32                                      ControlCount;
  H2O_CONTROL_INFO                            *ControlArray;
  EFI_HII_VALUE                               HiiValue;
  H2O_PANEL_INFO                              *ValuePanel;
  UINT32                                      ValuePanelWidth;
  CHAR16                                      *TempStr;
  CHAR16                                      *OptionStr;
  LIST_ENTRY                                  *PanelLink;
  UINT32                                      PseudoClass;

  PanelLink = &mDEPrivate->Layout->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  ValuePanel      = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_SETUP_PAGE);
  ValuePanelWidth = (ValuePanel != NULL) ? (UINT32) ((ValuePanel->PanelField.right - ValuePanel->PanelField.left) / 2) : 10;

  //
  // Get question value field
  //
  for (Index = 0; Index < PromptControls->Count; Index ++) {
    if (ValueControls->ControlArray[Index].PageId     == Question->PageId &&
        ValueControls->ControlArray[Index].QuestionId == Question->QuestionId) {
      break;
    }
  }
  if (Index == PromptControls->Count) {
    return EFI_NOT_FOUND;
  }
  CopyMem (&ValueField, &ValueControls->ControlArray[Index].ControlField, sizeof(RECT));
  PseudoClass = ValueControls->ControlArray[Index].ControlStyle.PseudoClass;

  ControlArray = NULL;
  ControlStr   = NULL;
  ControlCount = 0;
  ZeroMem (&HiiValue, sizeof (EFI_HII_VALUE));

  switch (Question->Operand) {

  case EFI_IFR_ORDERED_LIST_OP:
    if (Question->ContainerCount == 0) {
      return EFI_NOT_FOUND;
    }

    ControlCount = Question->ContainerCount;
    OptionStr    = AllocateZeroPool (sizeof (CHAR16) * ValuePanelWidth);
    ControlArray = (H2O_CONTROL_INFO *) AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * ControlCount);
    if (OptionStr == NULL || ControlArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (ContainerIndex = 0; ContainerIndex < ControlCount; ContainerIndex ++) {
      for (OptionIndex = 0; OptionIndex < Question->NumberOfOptions; OptionIndex++) {
        Option = &(Question->Options[OptionIndex]);
        ContainerIndexValue = GetBufferValue (Question->HiiValue.Buffer, Option->HiiValue.Type, ContainerIndex);
        if (Option->Visibility && Option->HiiValue.Value.u64 == ContainerIndexValue) {
          for (Index = 0; Index < (ValuePanelWidth - 1); Index ++) {
            OptionStr [Index] = ' ';
          }
          if (StrLen (Option->Text) < (ValuePanelWidth - 5)) {
            CopyMem (OptionStr, Option->Text, sizeof (CHAR16) * StrLen (Option->Text));
          } else {
            CopyMem (OptionStr, Option->Text, sizeof (CHAR16) * (ValuePanelWidth - 7));
            TempStr = OptionStr;
            OptionStr = CatSPrint (OptionStr, L"..");
            FreePool (TempStr);
          }
          ControlStr = CatSPrint (NULL, L"%s", OptionStr);
          CopyMem (&HiiValue, &Option->HiiValue, sizeof (EFI_HII_VALUE));
          break;
        }
      }

      if (OptionIndex >= Question->NumberOfOptions) {
        FreePool (ControlArray);
        FreePool (OptionStr);
        return EFI_NOT_FOUND;
      }

      ControlArray[ContainerIndex].Selectable               = TRUE;
      ControlArray[ContainerIndex].Modifiable               = FALSE;
      ControlArray[ContainerIndex].Editable                 = FALSE;
      ControlArray[ContainerIndex].Text.String              = ControlStr;
      ControlArray[ContainerIndex].ControlField.left        = ValueField.left;
      ControlArray[ContainerIndex].ControlField.right       = ValueField.right;
      ControlArray[ContainerIndex].ControlField.top         = ValueField.top + ContainerIndex;
      ControlArray[ContainerIndex].ControlField.bottom      = ControlArray[ContainerIndex].ControlField.top;
      ControlArray[ContainerIndex].ParentPanel              = ValuePanel;
      ControlArray[ContainerIndex].ControlStyle.PseudoClass = PseudoClass;
      CopyMem (&ControlArray[ContainerIndex].HiiValue, &HiiValue, sizeof (EFI_HII_VALUE));
    }
    FreePool (OptionStr);
    break;

  case EFI_IFR_DATE_OP:
    ControlCount = 3;
    ControlArray = (H2O_CONTROL_INFO *) AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * ControlCount);
    if (ControlArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < ControlCount; Index ++) {
      if (Index == 0) {
        ControlStr = CatSPrint (NULL, L"%04d", Question->HiiValue.Value.date.Year);
      } else if (Index == 1) {
        ControlStr = CatSPrint (NULL, L"%02d", Question->HiiValue.Value.date.Month);
      } else {
        ControlStr = CatSPrint (NULL, L"%02d", Question->HiiValue.Value.date.Day);
      }

      ControlArray[Index].Selectable               = TRUE;
      ControlArray[Index].Modifiable               = TRUE;
      ControlArray[Index].Editable                 = TRUE;
      ControlArray[Index].Text.String              = ControlStr;
      ControlArray[Index].HiiValue.Type            = EFI_IFR_TYPE_STRING;
      ControlArray[Index].HiiValue.BufferLen       = (UINT16) StrSize (ControlStr);
      ControlArray[Index].HiiValue.Buffer          = (UINT8 *) ControlStr;
      if (Index == 0) {
        ControlArray[Index].ControlField.left      = ValueField.left;
        ControlArray[Index].ControlField.right     = ControlArray[Index].ControlField.left + 3;
      } else {
        ControlArray[Index].ControlField.left      = ControlArray[Index - 1].ControlField.right + 2;
        ControlArray[Index].ControlField.right     = ControlArray[Index].ControlField.left + 1;
      }
      ControlArray[Index].ControlField.top         = ValueField.top;
      ControlArray[Index].ControlField.bottom      = ValueField.bottom;
      ControlArray[Index].ParentPanel              = ValuePanel;
      ControlArray[Index].ControlStyle.PseudoClass = PseudoClass;
    }
    break;

  case EFI_IFR_TIME_OP:
    ControlCount = 3;
    ControlArray = (H2O_CONTROL_INFO *) AllocateZeroPool (sizeof (H2O_CONTROL_INFO) * ControlCount);
    if (ControlArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < ControlCount; Index ++) {
      if (Index == 0) {
        ControlStr = CatSPrint (NULL, L"%02d", Question->HiiValue.Value.time.Hour);
      } else if (Index == 1) {
        ControlStr = CatSPrint (NULL, L"%02d", Question->HiiValue.Value.time.Minute);
      } else {
        ControlStr = CatSPrint (NULL, L"%02d", Question->HiiValue.Value.time.Second);
      }

      ControlArray[Index].Selectable               = TRUE;
      ControlArray[Index].Modifiable               = TRUE;
      ControlArray[Index].Editable                 = TRUE;
      ControlArray[Index].Text.String              = ControlStr;
      ControlArray[Index].HiiValue.Type            = EFI_IFR_TYPE_STRING;
      ControlArray[Index].HiiValue.BufferLen       = (UINT16) StrSize (ControlStr);
      ControlArray[Index].HiiValue.Buffer          = (UINT8 *) ControlStr;
      if (Index == 0) {
        ControlArray[Index].ControlField.left      = ValueField.left;
        ControlArray[Index].ControlField.right     = ControlArray[Index].ControlField.left + 1;
      } else {
        ControlArray[Index].ControlField.left      = ControlArray[Index - 1].ControlField.right + 2;
        ControlArray[Index].ControlField.right     = ControlArray[Index].ControlField.left + 1;
      }
      ControlArray[Index].ControlField.top         = ValueField.top;
      ControlArray[Index].ControlField.bottom      = ValueField.bottom;
      ControlArray[Index].ParentPanel              = ValuePanel;
      ControlArray[Index].ControlStyle.PseudoClass = PseudoClass;
    }
    break;

  default:
    return EFI_NOT_FOUND;
  }

  //
  // Save
  //
  if (PopUpControls != NULL && ControlArray != NULL) {
    FreeControlList (PopUpControls);
    PopUpControls->Count        = ControlCount;
    PopUpControls->ControlArray = AllocateCopyPool (sizeof (H2O_CONTROL_INFO) * ControlCount, ControlArray);
    if (PopUpControls->ControlArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    FreePool (ControlArray);
    //
    // Display
    //
    DisplayNormalControls (PopUpControls->Count, PopUpControls->ControlArray);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SendChangeQNotify (
  IN H2O_PAGE_ID                              PageId,
  IN EFI_QUESTION_ID                          QuestionId,
  IN EFI_HII_VALUE                            *HiiValue
  )
{
  EFI_STATUS                                  Status;
  H2O_DISPLAY_ENGINE_EVT_CHANGE_Q             ChangeQNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&ChangeQNotify, sizeof (ChangeQNotify));

  ChangeQNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_CHANGE_Q);
  ChangeQNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q;
  ChangeQNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  ChangeQNotify.PageId     = PageId;
  ChangeQNotify.QuestionId = QuestionId;
  CopyMem (&ChangeQNotify.HiiValue, HiiValue, sizeof (EFI_HII_VALUE));

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &ChangeQNotify.Hdr);

  return Status;
}

EFI_STATUS
SendChangingQNotify (
  IN H2O_FORM_BROWSER_PROTOCOL                *FBProtocol,
  IN EFI_HII_VALUE                            *BodyHiiValue,
  IN EFI_HII_VALUE                            *ButtonHiiValue
  )
{
  EFI_STATUS                                  Status;
  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;

  //
  // Notify Changing Question (CHANGING_Q)
  //
  ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q));
  if (ChangingQNotify == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ChangingQNotify->Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q);
  ChangingQNotify->Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  ChangingQNotify->Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q;
  //
  // FBDialog will store which question is current question, so PageId and QuestionId can be 0.
  //
  ChangingQNotify->PageId = 0;
  ChangingQNotify->QuestionId = 0;
  //
  // Save Old Value and New Value
  //
  if (BodyHiiValue != NULL) {
    CopyMem (&ChangingQNotify->BodyHiiValue, BodyHiiValue, sizeof (EFI_HII_VALUE));
  }
  if (ButtonHiiValue != NULL) {
    CopyMem (&ChangingQNotify->ButtonHiiValue, ButtonHiiValue, sizeof (EFI_HII_VALUE));
  }
  Status = FBProtocol->Notify (FBProtocol, &ChangingQNotify->Hdr);
  FreePool (ChangingQNotify);

  return Status;
}

EFI_STATUS
SendShutDNotify (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol
  )
{
  EFI_STATUS                                  Status;
  H2O_DISPLAY_ENGINE_EVT_SHUT_D               *ShutDNotify;

  //
  // Notify ShutD
  //
  ShutDNotify = (H2O_DISPLAY_ENGINE_EVT_SHUT_D *) AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D));
  if (ShutDNotify == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ShutDNotify->Hdr.Size = sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D);
  ShutDNotify->Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_BROADCAST;
  ShutDNotify->Hdr.Type = H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D;
  Status = FBProtocol->Notify (FBProtocol, &ShutDNotify->Hdr);
  FreePool (ShutDNotify);

  return Status;
}

EFI_STATUS
SendSelectQNotify (
  IN H2O_PAGE_ID                     PageId,
  IN EFI_QUESTION_ID                 QuestionId,
  IN EFI_IFR_OP_HEADER               *IfrOpCode
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_SELECT_Q    SelectQNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&SelectQNotify, sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q));

  SelectQNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q);
  SelectQNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q;
  SelectQNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  SelectQNotify.PageId     = PageId;
  SelectQNotify.QuestionId = QuestionId;
  SelectQNotify.IfrOpCode  = IfrOpCode;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &SelectQNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendOpenQNotify (
  IN H2O_PAGE_ID                     PageId,
  IN EFI_QUESTION_ID                 QuestionId,
  IN EFI_IFR_OP_HEADER               *IfrOpCode
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_OPEN_Q      OpenQNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&OpenQNotify, sizeof (H2O_DISPLAY_ENGINE_EVT_OPEN_Q));

  OpenQNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_OPEN_Q);
  OpenQNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_Q;
  OpenQNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  OpenQNotify.PageId     = PageId;
  OpenQNotify.QuestionId = QuestionId;
  OpenQNotify.IfrOpCode  = IfrOpCode;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &OpenQNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendSelectPNotify (
  IN H2O_PAGE_ID                     PageId
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_SELECT_P    SelectPNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&SelectPNotify, sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_P));

  SelectPNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_P);
  SelectPNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_P;
  SelectPNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  SelectPNotify.PageId     = PageId;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &SelectPNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendDefaultNotify (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_DEFAULT     DefaultNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&DefaultNotify, sizeof (DefaultNotify));

  DefaultNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_DEFAULT);
  DefaultNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT;
  DefaultNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &DefaultNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendSubmitExitNotify (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT SubmitExitNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&SubmitExitNotify, sizeof (SubmitExitNotify));

  SubmitExitNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT);
  SubmitExitNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT;
  SubmitExitNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &SubmitExitNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendDiscardExitNotify (
  VOID
  )
{
  EFI_STATUS                           Status;
  H2O_DISPLAY_ENGINE_EVT_DISCARD_EXIT  DiscardExitNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&DiscardExitNotify, sizeof (DiscardExitNotify));

  DiscardExitNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_DISCARD_EXIT);
  DiscardExitNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT;
  DiscardExitNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &DiscardExitNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendSubmitNotify (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_SUBMIT      SubmitNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&SubmitNotify, sizeof (SubmitNotify));

  SubmitNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SUBMIT);
  SubmitNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT;
  SubmitNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &SubmitNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendDiscardNotify (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_DISCARD     DiscardNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&DiscardNotify, sizeof (DiscardNotify));

  DiscardNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_DISCARD);
  DiscardNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD;
  DiscardNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &DiscardNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendRefreshNotify (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_REFRESH     RefreshNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&RefreshNotify, sizeof (RefreshNotify));

  RefreshNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_REFRESH);
  RefreshNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH;
  RefreshNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &RefreshNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
SendShowHelpNotify (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_SHOW_HELP   ShowHelpNotify;

  if (mDEPrivate == NULL || mDEPrivate->FBProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&ShowHelpNotify, sizeof (ShowHelpNotify));

  ShowHelpNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SHOW_HELP);
  ShowHelpNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP;
  ShowHelpNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  Status = mDEPrivate->FBProtocol->Notify (mDEPrivate->FBProtocol, &ShowHelpNotify.Hdr);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

UINT8
GetOperandByDialogType (
  IN UINT32                                   DialogType
  )
{
  switch ((DialogType & H2O_FORM_BROWSER_D_TYPE_QUESTIONS) >> 16) {

  case H2O_FORM_BROWSER_D_TYPE_ONE_OF:
    return EFI_IFR_ONE_OF_OP;

  case H2O_FORM_BROWSER_D_TYPE_ORDERED_LIST:
    return EFI_IFR_ORDERED_LIST_OP;

  case H2O_FORM_BROWSER_D_TYPE_NUMERIC:
    return EFI_IFR_NUMERIC_OP;

  case H2O_FORM_BROWSER_D_TYPE_STRING:
    return EFI_IFR_STRING_OP;

  case H2O_FORM_BROWSER_D_TYPE_DATE:
    return EFI_IFR_DATE_OP;

  case H2O_FORM_BROWSER_D_TYPE_TIME:
    return EFI_IFR_TIME_OP;

  case H2O_FORM_BROWSER_D_TYPE_PASSWORD:
    return EFI_IFR_PASSWORD_OP;

  default:
    break;
  }

  return 0;
}

BOOLEAN
IsDialogStatus (
  IN UINT8                                    DEStatus
  )
{
  if (DEStatus == DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG ||
      DEStatus == DISPLAY_ENGINE_STATUS_OPEN_D ||
      DEStatus == DISPLAY_ENGINE_STATUS_SHUT_D ||
      DEStatus == DISPLAY_ENGINE_STATUS_CHANGING_Q) {
    return TRUE;
  }

  return FALSE;
}

H2O_CONTROL_INFO *
FindControlByControlId (
  IN H2O_CONTROL_INFO                         *ControlArray,
  IN UINT32                                   ControlArrayCount,
  IN UINT32                                   ControlId,
  IN UINT32                                   SequenceIndex
  )
{
  UINT32                                      Index;
  UINT32                                      Count;

  if (ControlArray == NULL) {
    return NULL;
  }

  Count = 0;

  for (Index = 0; Index < ControlArrayCount; Index++) {
    if (ControlArray[Index].ControlId != ControlId) {
      continue;
    }

    if (Count == SequenceIndex) {
      return &ControlArray[Index];
    }

    Count++;
  }

  return NULL;
}

H2O_CONTROL_INFO *
FindNextSelectableControl (
  IN H2O_CONTROL_INFO                         *ControlArray,
  IN UINT32                                   ControlArrayCount,
  IN H2O_CONTROL_INFO                         *CurrentControl,
  IN BOOLEAN                                  GoDown,
  IN BOOLEAN                                  IsLoop
  )
{
  UINT32                                      Index;
  H2O_CONTROL_INFO                            *FirstControl;
  H2O_CONTROL_INFO                            *LastControl;
  H2O_CONTROL_INFO                            *PreviousControl;
  H2O_CONTROL_INFO                            *NextControl;
  BOOLEAN                                     CurrentControlFound;


  if (ControlArray == NULL) {
    return NULL;
  }

  FirstControl        = NULL;
  PreviousControl     = NULL;
  NextControl         = NULL;
  LastControl         = NULL;
  CurrentControlFound = (CurrentControl == NULL) ? TRUE : FALSE;

  for (Index = 0; Index < ControlArrayCount; Index++) {
    if (!IsValidHighlightStatement(&ControlArray[Index])) {
      continue;
    }

    if (FirstControl == NULL) {
      FirstControl = &ControlArray[Index];
    }

    if (CurrentControlFound && NextControl == NULL) {
      NextControl = &ControlArray[Index];
    }

    if (&ControlArray[Index] == CurrentControl) {
      CurrentControlFound = TRUE;
    }

    if (!CurrentControlFound) {
      PreviousControl = &ControlArray[Index];
    }

    LastControl = &ControlArray[Index];
  }

  if (GoDown) {
    if (NextControl != NULL) {
      return NextControl;
    }

    if (IsLoop && FirstControl != NULL && FirstControl != CurrentControl) {
      return FirstControl;
    }
  } else {
    if (PreviousControl != NULL) {
      return PreviousControl;
    }

    if (IsLoop && LastControl != NULL && LastControl != CurrentControl) {
      return LastControl;
    }
  }

  return NULL;
}

EFI_STATUS
DlgProcessUserInput (
  IN  BOOLEAN                               IsKeyboard,
  IN  EFI_INPUT_KEY                         *InputKey,
  IN  UINT32                                MouseX,
  IN  UINT32                                MouseY,
  IN  H2O_CONTROL_LIST                      *PopUpControls,
  OUT H2O_CONTROL_INFO                      **SelectedControl,
  OUT CHAR16                                **InputString,
  OUT BOOLEAN                               *IsShutdownDialog
  )
{
  BOOLEAN                                   CurrentIsBody;
  BOOLEAN                                   CurrentIsBodyInput;
  BOOLEAN                                   CurrentIsButton;
  UINTN                                     StringLength;
  UINTN                                     MinStringLength;
  H2O_CONTROL_INFO                          *NextControl;
  EFI_STATUS                                Status;

  if (InputKey == NULL || PopUpControls == NULL || SelectedControl == NULL || InputString == NULL || IsShutdownDialog == NULL) {
    return EFI_UNSUPPORTED;
  }

  *SelectedControl   = NULL;
  *InputString       = NULL;
  *IsShutdownDialog  = FALSE;
  CurrentIsBody      = (mDEPrivate->PopUpSelected->ControlId == H2O_CONTROL_ID_DIALOG_BODY)       ? TRUE : FALSE;
  CurrentIsBodyInput = (mDEPrivate->PopUpSelected->ControlId == H2O_CONTROL_ID_DIALOG_BODY_INPUT) ? TRUE : FALSE;
  CurrentIsButton    = (mDEPrivate->PopUpSelected->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON)     ? TRUE : FALSE;
  Status             = EFI_SUCCESS;

  if (!IsKeyboard) {
    if (CheckPressControls (IsKeyboard, InputKey, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &NextControl)) {
      if (NextControl == NULL) {
        *IsShutdownDialog = TRUE;
      } else {
        *SelectedControl = NextControl;
      }
    }

    return Status;
  }

  //
  // Process keyboard event
  //
  switch (InputKey->UnicodeChar) {

  case CHAR_TAB:
    if (CurrentIsBody) {
      *SelectedControl = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BUTTON, 0);
      break;
    }

    NextControl = FindNextSelectableControl (PopUpControls->ControlArray, PopUpControls->Count, mDEPrivate->PopUpSelected, TRUE, TRUE);
    if (NextControl != NULL && NextControl != mDEPrivate->PopUpSelected) {
      *SelectedControl = NextControl;
    }
    break;

  case CHAR_CARRIAGE_RETURN:
    if (CurrentIsBodyInput) {
      *SelectedControl = FindNextSelectableControl (PopUpControls->ControlArray, PopUpControls->Count, mDEPrivate->PopUpSelected, TRUE, FALSE);
      if (*SelectedControl == NULL) {
        *SelectedControl = mDEPrivate->PopUpSelected;
      }
      break;
    }

    if (CurrentIsBody || CurrentIsButton) {
      *SelectedControl = mDEPrivate->PopUpSelected;
      break;
    }
    break;

  case CHAR_BACKSPACE:
    if (CurrentIsBodyInput) {
      if (mDEPrivate->PopUpSelected->Operand == EFI_IFR_NUMERIC_OP && IsHexStr (mDEPrivate->PopUpSelected->Text.String)) {
        MinStringLength = sizeof (L"0x") / sizeof(CHAR16) - 1;
      } else {
        MinStringLength = 0;
      }

      StringLength = StrLen (mDEPrivate->PopUpSelected->Text.String);
      if (StringLength > MinStringLength) {
        *InputString = AllocateZeroPool (StringLength * sizeof (CHAR16));
        CopyMem (*InputString, mDEPrivate->PopUpSelected->Text.String, (StringLength - 1) * sizeof (CHAR16));
      }
    }
    break;

  case CHAR_NULL:
    switch (InputKey->ScanCode) {

    case SCAN_RIGHT:
    case SCAN_LEFT:
      if (CurrentIsButton) {
        NextControl = FindNextSelectableControl (
                        PopUpControls->ControlArray,
                        PopUpControls->Count,
                        mDEPrivate->PopUpSelected,
                        (InputKey->ScanCode == SCAN_RIGHT) ? TRUE : FALSE,
                        FALSE
                        );
        if (NextControl != NULL && NextControl->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON) {
          *SelectedControl = NextControl;
        }
      }
      break;

    case SCAN_UP:
    case SCAN_DOWN:
      if (CurrentIsBody) {
        NextControl = FindNextSelectableControl (
                        PopUpControls->ControlArray,
                        PopUpControls->Count,
                        mDEPrivate->PopUpSelected,
                        (InputKey->ScanCode == SCAN_DOWN) ? TRUE : FALSE,
                        FALSE
                        );
        if (NextControl != NULL && NextControl->ControlId == H2O_CONTROL_ID_DIALOG_BODY) {
          *SelectedControl = NextControl;
        }
      }
      break;

    case SCAN_ESC:
      *IsShutdownDialog = TRUE;
      break;
    }
    break;

  default:
    if (CurrentIsBodyInput && IsVisibleInputKey (InputKey)) {
      *InputString = CatSPrint (NULL, L"%s%c", mDEPrivate->PopUpSelected->Text.String, InputKey->UnicodeChar);
    }
    break;
  }

  return Status;
}

EFI_STATUS
GetPasswordHiiValue (
  IN  H2O_CONTROL_LIST                        *PopUpControls,
  OUT EFI_HII_VALUE                           *HiiValue
  )
{
  UINTN                                       Index;
  UINTN                                       TotalStrSize;
  UINTN                                       StringSize;
  UINTN                                       PasswordCount;
  UINT8                                       *StringBuffer;

  if (PopUpControls == NULL || HiiValue == NULL || PopUpControls->ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TotalStrSize  = 0;
  PasswordCount = 0;

  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (PopUpControls->ControlArray[Index].ControlId == H2O_CONTROL_ID_DIALOG_BODY_INPUT) {
      TotalStrSize += StrSize (PopUpControls->ControlArray[Index].Text.String);
      PasswordCount++;
    }
  }

  if (PasswordCount == 0 || TotalStrSize == 0) {
    return EFI_NOT_FOUND;
  }

  StringBuffer = AllocateZeroPool (TotalStrSize);
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HiiValue->Type      = EFI_IFR_TYPE_STRING;
  HiiValue->BufferLen = (UINT16) TotalStrSize;
  HiiValue->Buffer    = StringBuffer;

  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (PopUpControls->ControlArray[Index].ControlId == H2O_CONTROL_ID_DIALOG_BODY_INPUT) {
      StringSize = StrSize (PopUpControls->ControlArray[Index].Text.String);

      CopyMem (StringBuffer, PopUpControls->ControlArray[Index].Text.String, StringSize);
      StringBuffer += StringSize;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
AtSimpleDialog (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  STATIC H2O_FORM_BROWSER_D                   *Dialog = NULL;
  H2O_PANEL_INFO                              *QuestionPanel;
  H2O_CONTROL_INFO                            *SelectedControl;
  CHAR16                                      *UpdatedString;
  BOOLEAN                                     IsShutdownDialog;

  Status = EFI_NOT_FOUND;

  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
    if (mDEPrivate->PopUpSelected == NULL) {
      if ((Keyboard != NULL && *Keyboard) && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
        Status = SendShutDNotify (FBProtocol);
      } else if ((Keyboard != NULL && *Keyboard) && (Key->ScanCode == SCAN_PAGE_UP || Key->ScanCode == SCAN_PAGE_DOWN)) {
        QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
        if (QuestionPanel == NULL) {
          break;
        }

        if (Key->ScanCode == SCAN_PAGE_UP) {
          PanelPageUp (QuestionPanel);
        } else {
          PanelPageDown (QuestionPanel);
        }
        DisplayDialog (TRUE, Dialog, NULL);
      }
      break;
    }

    Status = DlgProcessUserInput (
               (Keyboard == NULL ? FALSE : *Keyboard),
               Key,
               MouseX,
               MouseY,
               PopUpControls,
               &SelectedControl,
               &UpdatedString,
               &IsShutdownDialog
               );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (SelectedControl != NULL) {
      if (SelectedControl == mDEPrivate->PopUpSelected) {
        Status = SendChangeQNotify (0, 0, &mDEPrivate->PopUpSelected->HiiValue);
      } else {
        DisplayControls (FALSE, FALSE, 1, mDEPrivate->PopUpSelected);
        mDEPrivate->PopUpSelected = SelectedControl;
        DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
      }
    }

    if (IsShutdownDialog) {
      Status = SendShutDNotify (FBProtocol);
    }
    break;

  case DISPLAY_ENGINE_STATUS_OPEN_D:
    Dialog = &((H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify)->Dialog;
    Status = DisplayDialog (FALSE, Dialog, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
    if (QuestionPanel == NULL) {
      break;
    }

    mDEPrivate->PopUpSelected = FindNextSelectableControl (
                                  QuestionPanel->ControlList.ControlArray,
                                  QuestionPanel->ControlList.Count,
                                  NULL,
                                  TRUE,
                                  TRUE
                                  );
    if (mDEPrivate->PopUpSelected != NULL) {
      DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
    }
    break;

  case DISPLAY_ENGINE_STATUS_SHUT_D:
    Status = ShutDialog ();
    break;
  }

  return Status;
}

EFI_STATUS
AtSimpleInputDialog (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;

  STATIC H2O_PANEL_INFO                       *OldDialogPanel = NULL;
  H2O_CONTROL_INFO                            *Select;

  BOOLEAN                                     IsEnterVisibleChar;
  EFI_HII_VALUE                               HiiValue;

  CHAR16                                      *TempStr = NULL;
  STATIC BOOLEAN                              HasButton = FALSE;
  EFI_HII_VALUE                               SelectHiiValue;
  STATIC UINT32                               StrWidth = 0;

  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D               *OpenDNotify;


  Status = EFI_NOT_FOUND;
  switch (DEStatus) {

    case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
      if (PopUpControls == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      //
      // Check Input
      //
      IsEnterVisibleChar = FALSE;
      if (Keyboard != NULL && *Keyboard) {
        ProcessCharInput (PopUpControls, Key, NULL, &IsEnterVisibleChar, &TempStr);
      } else {
        GetInputWithTab (PopUpControls, 0, &TempStr);
      }

      if (IsEnterVisibleChar) {
        //
        // Prepare HIIValue
        //
        HiiValue.Type = EFI_IFR_TYPE_STRING;
        HiiValue.BufferLen = (UINT16) StrSize (TempStr);
        HiiValue.Buffer = (UINT8 *) TempStr;
        //
        // Press visible char or backspace or tab, notify Changing Question (CHANGING_Q)
        // (In SimpleInput: ChangingQNotify->BodyHiiValue store "Current String")
        // (In SimpleInput: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
        //
        if (HasButton) {
          CopyMem (&SelectHiiValue, &mDEPrivate->PopUpSelected->HiiValue, sizeof (EFI_HII_VALUE));
        } else {
          SelectHiiValue.Type = EFI_IFR_TYPE_BOOLEAN;
          SelectHiiValue.Value.b = TRUE;
        }
        Status = SendChangingQNotify (FBProtocol, &HiiValue, &SelectHiiValue);
        break;
      }
      //
      // Change Select
      //
      if (!HasButton) {
        if (Keyboard != NULL && *Keyboard && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
          HiiValue.Type      = EFI_IFR_TYPE_STRING;
          HiiValue.BufferLen = (UINT16) StrSize (TempStr);
          HiiValue.Buffer    = (UINT8 *) TempStr;

          Status = SendChangeQNotify (0, 0, &HiiValue);
        }
        break;
      }
      Select = mDEPrivate->PopUpSelected;
      if (CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &Select)) {
        if (Select == NULL) {
          //
          // Same as Standard
          //
          return EFI_NOT_FOUND;
        } else if (Select == mDEPrivate->PopUpSelected && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
          if (mDEPrivate->PopUpSelected->HiiValue.Value.b) {
            HiiValue.Type      = EFI_IFR_TYPE_STRING;
            HiiValue.BufferLen = (UINT16) StrSize (TempStr);
            HiiValue.Buffer    = (UINT8 *) TempStr;

            Status = SendChangeQNotify (0, 0, &HiiValue);
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
          break;
        } else {
          if (Select->Selectable && !Select->Modifiable) {
            //
            // Prepare HIIValue
            //
            HiiValue.Type = EFI_IFR_TYPE_STRING;
            HiiValue.BufferLen = (UINT16) StrSize (TempStr);
            HiiValue.Buffer = (UINT8 *) TempStr;
            //
            // Notify Changing Question (CHANGING_Q)
            // (In SimpleInput: ChangingQNotify->BodyHiiValue store "Current String")
            // (In SimpleInput: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
            //
            Status = SendChangingQNotify (FBProtocol, &HiiValue, &Select->HiiValue);
            break;
            }
        }
      }
      break;

    case DISPLAY_ENGINE_STATUS_OPEN_D:
      if (PopUpControls == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      //
      // Pop-Up Input Dialog
      //
      OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
      if (OpenDNotify->Dialog.ButtonCount != 0) {
        HasButton = TRUE;
      } else {
        HasButton = FALSE;
      }
      Status = DisplayDialog (FALSE, &OpenDNotify->Dialog, &OldDialogPanel);
      if (!EFI_ERROR (Status)) {
        //
        // Set PopUpSelected
        //
        if (HasButton) {
          if (PopUpControls->ControlArray != NULL && PopUpControls->Count >= OpenDNotify->Dialog.ButtonCount) {
            mDEPrivate->PopUpSelected = &PopUpControls->ControlArray[PopUpControls->Count - OpenDNotify->Dialog.ButtonCount];
          }
        } else {
          mDEPrivate->PopUpSelected = NULL;
        }
        //
        // Init strings
        //
        StrWidth = OpenDNotify->Dialog.BodyHiiValueArray[0].BufferLen;
        TempStr = CatSPrint (NULL, L"%s", OpenDNotify->Dialog.BodyHiiValueArray[0].Buffer);
        Status = RefreshPasswordInput (PopUpControls->ControlArray, PopUpControls->Count, TempStr);
        FreePool (TempStr);
        //
        // High-Light PopUpSelected
        //
        if (HasButton) {
          DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
        }
      }
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_D:
      Status = ShutDialog ();
      break;

    case DISPLAY_ENGINE_STATUS_CHANGING_Q:
      if (PopUpControls == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify;
      //
      // (In SimpleInput: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
      //
      if (HasButton) {
        Status = RefreshPopUpSelectionByValue (PopUpControls, FALSE, FALSE, &mDEPrivate->PopUpSelected->HiiValue);
        Status = RefreshPopUpSelectionByValue (PopUpControls, TRUE, FALSE, &ChangingQNotify->ButtonHiiValue);
      }
      //
      // (In SimpleInput: ChangingQNotify->BodyHiiValue store "Current String")
      //
      TempStr = CatSPrint (NULL, (CHAR16 *) ChangingQNotify->BodyHiiValue.Buffer);
      Status = RefreshPasswordInput (PopUpControls->ControlArray, PopUpControls->Count, TempStr);
      FreePool (TempStr);
      //
      // Set PopUpSelected
      //
      if (HasButton) {
        GetPopUpSelectionByValue (PopUpControls, &ChangingQNotify->ButtonHiiValue, &mDEPrivate->PopUpSelected, NULL);
      }
      break;
  }
  return Status;
}

EFI_STATUS
AtCheckBox (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *Select;

  Status = EFI_NOT_FOUND;

  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_MENU:
    Select = mDEPrivate->MenuSelected;
    if (CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, PromptControls->Count, PromptControls->ControlArray, &Select) ||
        CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, ValueControls->Count , ValueControls->ControlArray , &Select)) {
       if (Select != NULL &&
           Select->PageId     == mDEPrivate->MenuSelected->PageId &&
           Select->QuestionId == mDEPrivate->MenuSelected->QuestionId &&
           Key->UnicodeChar   == CHAR_CARRIAGE_RETURN) {
        mDEPrivate->MenuSelected                   = Select;
        mDEPrivate->MenuSelected->HiiValue.Type    = EFI_IFR_TYPE_BOOLEAN;
        mDEPrivate->MenuSelected->HiiValue.Value.b = (BOOLEAN) (mDEPrivate->MenuSelected->HiiValue.Value.b ? FALSE : TRUE);

        Status = SendChangeQNotify (
                   mDEPrivate->MenuSelected->PageId,
                   mDEPrivate->MenuSelected->QuestionId,
                   &mDEPrivate->MenuSelected->HiiValue
                   );
      }
    }
    break;
  }

  return Status;
}

EFI_STATUS
AtNumeric (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  EFI_HII_VALUE                               HiiValue;
  H2O_PANEL_INFO                              *QuestionPanel;
  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;
  H2O_CONTROL_INFO                            *Control;
  H2O_CONTROL_INFO                            *SelectedControl;
  CHAR16                                      *UpdatedString;
  BOOLEAN                                     IsShutdownDialog;
  BOOLEAN                                     IsChangeQ;
  H2O_FORM_BROWSER_D                          *Dialog;

  Status = EFI_NOT_FOUND;

  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_MENU:
    if (Key->UnicodeChar == CHAR_SUB || Key->UnicodeChar == CHAR_ADD) {
      Status = ChangeQuestionValueByStep ((Key->UnicodeChar == CHAR_ADD), FBProtocol->CurrentQ, &HiiValue);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Status = SendChangeQNotify (0, 0, &HiiValue);
    }
    break;

  case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    Status = DlgProcessUserInput (
               (Keyboard == NULL ? FALSE : *Keyboard),
               Key,
               MouseX,
               MouseY,
               PopUpControls,
               &SelectedControl,
               &UpdatedString,
               &IsShutdownDialog
               );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (SelectedControl != NULL) {
      if (SelectedControl == mDEPrivate->PopUpSelected) {
        IsChangeQ = FALSE;
        if (SelectedControl->ControlId == H2O_CONTROL_ID_DIALOG_BODY_INPUT) {
          if (Keyboard != NULL && *Keyboard) {
            IsChangeQ = TRUE;
          } else {
            InitializeVirtualKb ();
          }
        }

        if (SelectedControl->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON) {
          if (SelectedControl->HiiValue.Value.b) {
            IsChangeQ = TRUE;
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
        }

        if (IsChangeQ) {
          Control = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BODY_INPUT, 0);
          if (Control == NULL) {
            break;
          }

          CopyMem (&HiiValue, &Control->HiiValue, sizeof (EFI_HII_VALUE));
          HiiValue.Value.u64 = IsHexStr (Control->Text.String) ? StrHexToUint64 (Control->Text.String) : StrDecimalToUint64 (Control->Text.String);
          if (HiiValue.Value.u64 < Control->Minimum || HiiValue.Value.u64 > Control->Maximum) {
            break;
          }
          Status = SendChangeQNotify (0, 0, &HiiValue);
        }
      } else {
        DisplayControls (FALSE, FALSE, 1, mDEPrivate->PopUpSelected);
        mDEPrivate->PopUpSelected = SelectedControl;
        DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
      }
    }

    if (UpdatedString != NULL) {
      Control = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BODY_INPUT, 0);
      if (Control == NULL) {
        break;
      }
      CopyMem (&HiiValue, &Control->HiiValue, sizeof (EFI_HII_VALUE));
      HiiValue.Value.u64 = IsHexStr (UpdatedString) ? StrHexToUint64 (UpdatedString) : StrDecimalToUint64 (UpdatedString);
      if (HiiValue.Value.u64 > Control->Maximum) {
        break;
      }
      Status = SendChangingQNotify (FBProtocol, &HiiValue, NULL);
    }

    if (IsShutdownDialog) {
      Status = SendShutDNotify (FBProtocol);
    }
    break;

  case DISPLAY_ENGINE_STATUS_OPEN_D:
    Dialog = &((H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify)->Dialog;
    Status = DisplayDialog (FALSE, Dialog, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    mDEPrivate->PopUpSelected = FindNextSelectableControl (
                                  PopUpControls->ControlArray,
                                  PopUpControls->Count,
                                  NULL,
                                  TRUE,
                                  FALSE
                                  );

    if ((Dialog->H2OStatement->Flags & EFI_IFR_DISPLAY_UINT_HEX) == EFI_IFR_DISPLAY_UINT_HEX) {
      Control = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BODY_INPUT, 0);
      if (Control == NULL) {
        break;
      }
      SafeFreePool ((VOID **) &Control->Text.String);
      Control->Text.String = AllocateCopyPool (sizeof(L"0x"), L"0x");
      DisplayControls (FALSE, FALSE, 1, Control);
    }

    DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
    break;

  case DISPLAY_ENGINE_STATUS_SHUT_D:
    QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
    if (QuestionPanel == NULL) {
      break;
    }
    Status = ShutDialog ();
    ShutdownVirtualKb ();
    break;

  case DISPLAY_ENGINE_STATUS_CHANGING_Q:
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify;

    Control = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BODY_INPUT, 0);
    if (Control == NULL) {
      break;
    }

    if (IsHexStr (Control->Text.String)) {
      UpdatedString = CatSPrint (NULL, L"0x%x", ChangingQNotify->BodyHiiValue.Value.u64);
    } else {
      UpdatedString = CatSPrint (NULL, L"%d", ChangingQNotify->BodyHiiValue.Value.u64);
    }
    SafeFreePool ((VOID **) &Control->Text.String);
    Control->Text.String = UpdatedString;
    if (mDEPrivate->PopUpSelected == Control) {
      DisplayHighLightControl (FALSE, Control);
    } else {
      DisplayControls (FALSE, FALSE, 1, Control);
    }
    break;
  }

  return Status;
}

EFI_STATUS
AtOneOf (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *Select;
  EFI_HII_VALUE                               CurrentQHiiValue;
  EFI_HII_VALUE                               HiiValue;
  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D               *OpenDNotify;
  H2O_CONTROL_INFO                            *SelectedControl;
  CHAR16                                      *UpdatedString;
  BOOLEAN                                     IsShutdownDialog;

  Status = EFI_NOT_FOUND;
  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
    Status = DlgProcessUserInput (
               (Keyboard == NULL ? FALSE : *Keyboard),
               Key,
               MouseX,
               MouseY,
               PopUpControls,
               &SelectedControl,
               &UpdatedString,
               &IsShutdownDialog
               );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (SelectedControl != NULL) {
      if (SelectedControl == mDEPrivate->PopUpSelected) {
        CopyMem (&CurrentQHiiValue, &mDEPrivate->PopUpSelected->HiiValue, sizeof (EFI_HII_VALUE));
        if (FBProtocol->CurrentQ != NULL && FBProtocol->CurrentQ->NumberOfOptions != 0) {
          CopyMem (&CurrentQHiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
        }
        CopyMem (&HiiValue.Type, &CurrentQHiiValue, sizeof (EFI_HII_VALUE));
        HiiValue.Value.u64 = mDEPrivate->PopUpSelected->HiiValue.Value.u64;

        Status = SendChangeQNotify (0, 0, &HiiValue);
      } else {
        if (SelectedControl->Selectable) {
          Status = SendChangingQNotify (FBProtocol, &SelectedControl->HiiValue, NULL);
        }
      }
    }

    if (IsShutdownDialog) {
      Status = SendShutDNotify (FBProtocol);
    }
    break;

  case DISPLAY_ENGINE_STATUS_OPEN_D:
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
    if (OpenDNotify->Dialog.BodyStringCount == 0 || OpenDNotify->Dialog.BodyHiiValueArray == NULL) {
      break;
    }

    Status = DisplayDialog (FALSE, &OpenDNotify->Dialog, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = GetPopUpSelectionByValue (PopUpControls, &OpenDNotify->Dialog.ConfirmHiiValue , &Select, NULL);
    if (EFI_ERROR (Status)) {
      Select = &PopUpControls->ControlArray[0];
    }

    mDEPrivate->PopUpSelected = Select;
    DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
    break;

  case DISPLAY_ENGINE_STATUS_SHUT_D:
    Status = ShutDialog ();
    break;

  case DISPLAY_ENGINE_STATUS_CHANGING_Q:
    ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify;

    Status = GetPopUpSelectionByValue (PopUpControls, &ChangingQNotify->BodyHiiValue, &Select, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    DisplayControls (FALSE, FALSE, 1, mDEPrivate->PopUpSelected);
    mDEPrivate->PopUpSelected = Select;
    DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
    break;
  }

  return Status;
}

EFI_STATUS
AtOrderList (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  STATIC H2O_PANEL_INFO                       *OldDialogPanel = NULL;
  H2O_CONTROL_INFO                            *Select;
  UINT32                                      Index;
  STATIC UINT32                               TabIndex = 0;
  STATIC BOOLEAN                              Exchanged = FALSE;
  STATIC UINT8                                *Buffer = NULL;
  STATIC EFI_HII_VALUE                        OldHiiValue;
  EFI_HII_VALUE                               HiiValue;
  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D               *OpenDNotify;

  Status = EFI_NOT_FOUND;
  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_MENU:
    if ((Key->ScanCode == SCAN_UP && TabIndex == 0) ||
        (Key->ScanCode == SCAN_DOWN && TabIndex == (UINT32)(FBProtocol->CurrentQ->ContainerCount - 1))) {
      return EFI_NOT_FOUND;
    }
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    Select = mDEPrivate->PopUpSelected;
    if (CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &Select)) {
      if (Select == NULL) {
        return EFI_NOT_FOUND;
      } else if (Select == mDEPrivate->PopUpSelected && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
        return EFI_NOT_FOUND;
      } else {
        GetPopUpSelectionByValue (PopUpControls, &Select->HiiValue, &mDEPrivate->PopUpSelected, &TabIndex);

        CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
        CopyMem (HiiValue.Buffer, Buffer, HiiValue.BufferLen);

        Status = SendChangeQNotify (FBProtocol->CurrentQ->PageId, FBProtocol->CurrentQ->QuestionId, &HiiValue);
        break;
      }
    }
    if (Key->ScanCode == SCAN_F5 || Key->ScanCode == SCAN_F6 || Key->UnicodeChar == CHAR_SUB || Key->UnicodeChar == CHAR_ADD) {
      for (Index = 0; Index < FBProtocol->CurrentQ->ContainerCount; Index ++) {
        if (mDEPrivate->PopUpSelected->HiiValue.Value.u64 == GetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index)) {
          break;
        }
      }
      if (Index >= FBProtocol->CurrentQ->ContainerCount) {
        //
        // Not Found
        //
        break;
      }
      //
      // Exchanged
      //
      CopyMem (&HiiValue, &mDEPrivate->PopUpSelected->HiiValue, sizeof (EFI_HII_VALUE));
      if (Key->UnicodeChar == CHAR_ADD || Key->ScanCode == SCAN_F6) {
        if (Index <= 0) {
          //
          // Can't move up
          //
          break;
        }
        HiiValue.Value.u64 = GetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index - 1);
        SetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index - 1, mDEPrivate->PopUpSelected->HiiValue.Value.u64);
        SetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index, HiiValue.Value.u64);
      } else {
        if (Index >= FBProtocol->CurrentQ->ContainerCount) {
          //
          // Can't move down
          //
          break;
        }
        HiiValue.Value.u64 = GetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index + 1);
        SetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index + 1, mDEPrivate->PopUpSelected->HiiValue.Value.u64);
        SetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index, HiiValue.Value.u64);
      }
      //
      // Get Select
      //
      GetPopUpSelectionByValue (PopUpControls, &HiiValue, &mDEPrivate->PopUpSelected, &TabIndex);
      //
      // Prepare HIIValue
      //
      CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
      CopyMem (HiiValue.Buffer, Buffer, HiiValue.BufferLen);
      //
      // Notify Change Question (CHANGE_Q)
      //
      Status = SendChangeQNotify (FBProtocol->CurrentQ->PageId, FBProtocol->CurrentQ->QuestionId, &HiiValue);
      CopyMem (&OldHiiValue, &HiiValue, sizeof (EFI_HII_VALUE));
      break;
    }
    break;

    case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
      if (PopUpControls == NULL) {
        return EFI_INVALID_PARAMETER;
      }

      Select = mDEPrivate->PopUpSelected;
      if (CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &Select)) {
        if (Select == NULL) {
          //
          // Same as Standard
          //
          return EFI_NOT_FOUND;
        } else if (Select == mDEPrivate->PopUpSelected && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
          if (mDEPrivate->PopUpSelected->HiiValue.Value.b) {
            CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
            CopyMem (HiiValue.Buffer, Buffer, HiiValue.BufferLen);

            Status = SendChangeQNotify (0, 0, &HiiValue);
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
          break;
        } else {
          //
          // Notify Changing Question (CHANGING_Q)
          //
          Status = SendChangingQNotify (FBProtocol, &Select->HiiValue, NULL);
          CopyMem (&OldHiiValue, &mDEPrivate->PopUpSelected->HiiValue, sizeof (EFI_HII_VALUE));
          break;
        }
      }
      if (Key->ScanCode == SCAN_F5 || Key->ScanCode == SCAN_F6 || Key->UnicodeChar == CHAR_SUB || Key->UnicodeChar == CHAR_ADD) {
        for (Index = 0; Index < FBProtocol->CurrentQ->ContainerCount; Index ++) {
          if (mDEPrivate->PopUpSelected->HiiValue.Value.u64 == GetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index)) {
            break;
          }
        }
        if (Index >= FBProtocol->CurrentQ->ContainerCount) {
          //
          // Not Found
          //
          break;
        }
        //
        // Exchanged
        //
        Exchanged = TRUE;
        //
        // Prepare HIIValue
        //
        CopyMem (&HiiValue.Type, &mDEPrivate->PopUpSelected->HiiValue, sizeof (EFI_HII_VALUE));
        if (Key->UnicodeChar == CHAR_ADD || Key->ScanCode == SCAN_F6) {
          if (Index <= 0) {
            //
            // Can't move up
            //
            break;
          }
          HiiValue.Value.u64 = GetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index - 1);
        } else {
          if (Index >= FBProtocol->CurrentQ->ContainerCount) {
            //
            // Can't move down
            //
            break;
          }
          HiiValue.Value.u64 = GetBufferValue (Buffer, mDEPrivate->PopUpSelected->HiiValue.Type, Index + 1);
        }
        //
        // Notify Changing Question (CHANGING_Q)
        //
        Status = SendChangingQNotify (FBProtocol, &mDEPrivate->PopUpSelected->HiiValue, NULL);
        CopyMem (&OldHiiValue, &HiiValue, sizeof (EFI_HII_VALUE));
        break;
      }
      break;

    case DISPLAY_ENGINE_STATUS_OPEN_D:
      if (PopUpControls == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      if (FBProtocol->CurrentQ->NumberOfOptions == 0) {
        break;
      }
      //
      // Create temp value controls
      //
      Status = FreeControlList (PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }

      OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
      Status = DisplayDialog (FALSE, &OpenDNotify->Dialog, &OldDialogPanel);
      if (EFI_ERROR (Status)) {
        break;
      }
      TabIndex = 0;
      Status = RefreshPopUpSelectionByValue (PopUpControls, TRUE, FALSE, &PopUpControls->ControlArray[TabIndex].HiiValue);

      mDEPrivate->PopUpSelected = &PopUpControls->ControlArray[TabIndex];

      SafeFreePool ((VOID **) &Buffer);
      Buffer = AllocateCopyPool (FBProtocol->CurrentQ->HiiValue.BufferLen, FBProtocol->CurrentQ->HiiValue.Buffer);
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_D:
      Status = ShutDialog ();
      break;

    case DISPLAY_ENGINE_STATUS_CHANGING_Q:
      ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify;
      if (Exchanged) {
        ExchangeContainerValue (PopUpControls, Buffer, OldHiiValue.Type, &OldHiiValue, &ChangingQNotify->BodyHiiValue);
        Exchanged = FALSE;
      }
      //
      // Redraw old select
      //
      Status = RefreshPopUpSelectionByValue (PopUpControls, FALSE, FALSE, &OldHiiValue);
      //
      // Select Option
      //
      Status = RefreshPopUpSelectionByValue (PopUpControls, TRUE, FALSE, &ChangingQNotify->BodyHiiValue);
      //
      // Set PopUpSelected
      //
      GetPopUpSelectionByValue (PopUpControls, &ChangingQNotify->BodyHiiValue, &mDEPrivate->PopUpSelected, &TabIndex);
      break;

    case DISPLAY_ENGINE_STATUS_SELECT_Q:
      if (PopUpControls == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      if (FBProtocol->CurrentQ->NumberOfOptions == 0) {
        break;
      }

      Status = FreeControlList (PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      Status = CreateValueControls (FBProtocol->CurrentQ, PromptControls, ValueControls, PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Hight Light TabIndex Control
      //
      Status = RefreshPopUpSelectionByValue (PopUpControls, TRUE, FALSE, &PopUpControls->ControlArray[TabIndex].HiiValue);
      //
      // Init PopUpSelected
      //
      mDEPrivate->PopUpSelected = &PopUpControls->ControlArray[TabIndex];

      SafeFreePool ((VOID **) &Buffer);
      Buffer = AllocateCopyPool (FBProtocol->CurrentQ->HiiValue.BufferLen, FBProtocol->CurrentQ->HiiValue.Buffer);
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_Q:
      TabIndex = 0;
      SafeFreePool ((VOID **) &Buffer);
      break;
  }

  return Status;
}

EFI_STATUS
AtString (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *Control;
  EFI_HII_VALUE                               HiiValue;
  H2O_CONTROL_INFO                            *SelectedControl;
  CHAR16                                      *UpdatedString;
  BOOLEAN                                     IsShutdownDialog;
  BOOLEAN                                     IsChangeQ;
  UINTN                                       StringSize;

  Status = EFI_NOT_FOUND;

  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    Status = DlgProcessUserInput (
               (Keyboard == NULL ? FALSE : *Keyboard),
               Key,
               MouseX,
               MouseY,
               PopUpControls,
               &SelectedControl,
               &UpdatedString,
               &IsShutdownDialog
               );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (SelectedControl != NULL) {
      if (SelectedControl == mDEPrivate->PopUpSelected) {
        IsChangeQ = FALSE;

        if (SelectedControl->ControlId == H2O_CONTROL_ID_DIALOG_BODY_INPUT) {
          if (Keyboard != NULL && *Keyboard) {
            IsChangeQ = TRUE;
          } else {
            InitializeVirtualKb ();
          }
        }

        if (SelectedControl->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON) {
          if (SelectedControl->HiiValue.Value.b) {
            IsChangeQ = TRUE;
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
        }

        if (IsChangeQ) {
          Control = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BODY_INPUT, 0);
          if (Control == NULL) {
            break;
          }

          StringSize = StrSize (Control->Text.String);

          HiiValue.Type      = EFI_IFR_TYPE_STRING;
          HiiValue.BufferLen = (UINT16) StringSize;
          HiiValue.Buffer    = (UINT8 *) AllocateCopyPool (StringSize, Control->Text.String);
          if (HiiValue.Buffer == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }

          Status = SendChangeQNotify (0, 0, &HiiValue);
        }
      } else {
        DisplayControls (FALSE, FALSE, 1, mDEPrivate->PopUpSelected);
        mDEPrivate->PopUpSelected = SelectedControl;
        DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
      }
    }

    if (UpdatedString != NULL) {
      if (StrSize (UpdatedString) > mDEPrivate->PopUpSelected->HiiValue.BufferLen) {
        FreePool ((VOID *) UpdatedString);
        break;
      }

      HiiValue.Type      = EFI_IFR_TYPE_STRING;
      HiiValue.BufferLen = (UINT16) StrSize (UpdatedString);
      HiiValue.Buffer    = (UINT8 *) UpdatedString;

      Status = SendChangingQNotify (FBProtocol, &HiiValue, NULL);
    }

    if (IsShutdownDialog) {
      Status = SendShutDNotify (FBProtocol);
    }
    break;

  case DISPLAY_ENGINE_STATUS_OPEN_D:
    Status = DisplayDialog (FALSE, &((H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify)->Dialog, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    mDEPrivate->PopUpSelected = FindNextSelectableControl (
                                  PopUpControls->ControlArray,
                                  PopUpControls->Count,
                                  NULL,
                                  TRUE,
                                  FALSE
                                  );
    DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
    break;

  case DISPLAY_ENGINE_STATUS_SHUT_D:
    ShutdownVirtualKb ();
    Status = ShutDialog ();
    break;

  case DISPLAY_ENGINE_STATUS_CHANGING_Q:
    if (PopUpControls == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    Control = FindControlByControlId (PopUpControls->ControlArray, PopUpControls->Count, H2O_CONTROL_ID_DIALOG_BODY_INPUT, 0);
    if (Control == NULL) {
      break;
    }

    SafeFreePool ((VOID **) &Control->Text.String);
    Control->Text.String = (CHAR16 *) (((H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify)->BodyHiiValue.Buffer);

    if (mDEPrivate->PopUpSelected == Control) {
      DisplayHighLightControl (FALSE, Control);
    } else {
      DisplayControls (FALSE, FALSE, 1, Control);
    }
    break;
  }
  return Status;
}

EFI_STATUS
AtPassword (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  EFI_HII_VALUE                               HiiValue;
  H2O_PANEL_INFO                              *QuestionPanel;
  H2O_CONTROL_INFO                            *SelectedControl;
  CHAR16                                      *UpdatedString;
  BOOLEAN                                     IsShutdownDialog;
  BOOLEAN                                     IsChangeQ;

  Status = EFI_NOT_FOUND;

  switch (DEStatus) {

  case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
    Status = DlgProcessUserInput (
               (Keyboard == NULL ? FALSE : *Keyboard),
               Key,
               MouseX,
               MouseY,
               PopUpControls,
               &SelectedControl,
               &UpdatedString,
               &IsShutdownDialog
               );
    if (EFI_ERROR (Status)) {
      break;
    }

    if (SelectedControl != NULL) {
      if (SelectedControl == mDEPrivate->PopUpSelected) {
        IsChangeQ = FALSE;
        if (SelectedControl->ControlId == H2O_CONTROL_ID_DIALOG_BODY_INPUT) {
          if (Keyboard != NULL && *Keyboard) {
            IsChangeQ = TRUE;
          } else {
            InitializeVirtualKb ();
          }
        }

        if (SelectedControl->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON) {
          if (SelectedControl->HiiValue.Value.b) {
            IsChangeQ = TRUE;
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
        }

        if (IsChangeQ) {
          Status = GetPasswordHiiValue (PopUpControls, &HiiValue);
          if (EFI_ERROR (Status)) {
            break;
          }
          Status = SendChangeQNotify (0, 0, &HiiValue);
        }
      } else {
        if (mDEPrivate->PopUpSelected->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON) {
          DisplayControls (FALSE, FALSE, 1, mDEPrivate->PopUpSelected);
        }
        mDEPrivate->PopUpSelected = SelectedControl;
        RefreshPassword (PopUpControls, mDEPrivate->PopUpSelected, NULL);
        if (mDEPrivate->PopUpSelected->ControlId == H2O_CONTROL_ID_DIALOG_BUTTON) {
          DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
        }
      }
    }

    if (UpdatedString != NULL) {
      if (StrSize (UpdatedString) > mDEPrivate->PopUpSelected->HiiValue.BufferLen) {
        FreePool ((VOID *) UpdatedString);
        break;
      }

      HiiValue.Type      = EFI_IFR_TYPE_STRING;
      HiiValue.BufferLen = (UINT16) StrSize (UpdatedString);
      HiiValue.Buffer    = (UINT8 *) UpdatedString;
      Status = SendChangingQNotify (FBProtocol, &HiiValue, NULL);
    }

    if (IsShutdownDialog) {
      Status = SendShutDNotify (FBProtocol);
    }
    break;

  case DISPLAY_ENGINE_STATUS_OPEN_D:
    QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
    if (QuestionPanel == NULL) {
      break;
    }

    Status = DisplayDialog (FALSE, &((H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify)->Dialog, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    mDEPrivate->PopUpSelected = FindNextSelectableControl (
                                  QuestionPanel->ControlList.ControlArray,
                                  QuestionPanel->ControlList.Count,
                                  NULL,
                                  TRUE,
                                  TRUE
                                  );
    DisplayHighLightControl (FALSE, mDEPrivate->PopUpSelected);
    break;

  case DISPLAY_ENGINE_STATUS_SHUT_D:
    QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
    if (QuestionPanel == NULL) {
      break;
    }
    ShutdownVirtualKb ();
    Status = ShutDialog ();
    break;

  case DISPLAY_ENGINE_STATUS_CHANGING_Q:
    RefreshPassword (PopUpControls, mDEPrivate->PopUpSelected, (CHAR16 *) ((H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify)->BodyHiiValue.Buffer);
    break;
  }

  return Status;
}

EFI_STATUS
AtDate (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;
  STATIC H2O_PANEL_INFO                       *OldDialogPanel = NULL;
  H2O_CONTROL_INFO                            *Select;

  BOOLEAN                                     IsEnterVisibleChar;
  EFI_HII_VALUE                               HiiValue;

  STATIC UINT32                               TabIndex = 0;
  STATIC CHAR16                               *NotifyStr = NULL;
  CHAR16                                      *TempStr = NULL;

  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D               *OpenDNotify;


  Status = EFI_NOT_FOUND;
  switch (DEStatus) {

    case DISPLAY_ENGINE_STATUS_AT_MENU:
      //
      // Check Input
      //
      IsEnterVisibleChar = FALSE;
      if (Keyboard != NULL && *Keyboard) {
        ProcessNumberInput (PopUpControls, FALSE, Key, &TabIndex, &IsEnterVisibleChar, &TempStr);
      } else {
        GetInputWithTab (PopUpControls, TabIndex, &TempStr);
      }

      //
      // Regular Date HIIValue
      //
      CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
      if (!IsEnterVisibleChar || TempStr == NULL) {
        if (TabIndex == 0) {
          TempStr = CatSPrint (NULL, L"%04d", HiiValue.Value.date.Year);
        } else if (TabIndex == 1) {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.date.Month);
        } else {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.date.Day);
        }
      }
      if (TabIndex == 0) {
        HiiValue.Value.date.Year = (UINT16) StrDecimalToUint64 (TempStr);
      } else if (TabIndex == 1) {
        HiiValue.Value.date.Month = (UINT8) StrDecimalToUint64 (TempStr);
      } else {
        HiiValue.Value.date.Day = (UINT8) StrDecimalToUint64 (TempStr);
      }
      FreePool (TempStr);
      LetDateRegular (&HiiValue);

      if (IsEnterVisibleChar) {
        //
        // Notify Change Question (CHANGE_Q)
        //
        Status = SendChangeQNotify (FBProtocol->CurrentQ->PageId, FBProtocol->CurrentQ->QuestionId, &HiiValue);
        break;
      } else {
        //
        // Same as Standard
        //
        return EFI_NOT_FOUND;
      }
      break;

    case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
      if (NotifyStr != NULL) {
        FreePool (NotifyStr);
        NotifyStr = NULL;
      }
      //
      // Check Input
      //
      IsEnterVisibleChar = FALSE;
      if (Keyboard != NULL && *Keyboard) {
        ProcessNumberInput (PopUpControls, FALSE, Key, &TabIndex, &IsEnterVisibleChar, &TempStr);
      } else {
        GetInputWithTab (PopUpControls, TabIndex, &TempStr);
      }

      //
      // Regular Date HIIValue and TempStr
      //
      CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
      if (!IsEnterVisibleChar || TempStr == NULL) {
        if (TabIndex == 0) {
          TempStr = CatSPrint (NULL, L"%04d", HiiValue.Value.date.Year);
        } else if (TabIndex == 1) {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.date.Month);
        } else {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.date.Day);
        }
      }
      if (TabIndex == 0) {
        HiiValue.Value.date.Year = (UINT16) StrDecimalToUint64 (TempStr);
        FreePool (TempStr);
        TempStr = CatSPrint (NULL, L"%04d", HiiValue.Value.date.Year);
      } else if (TabIndex == 1) {
        HiiValue.Value.date.Month = (UINT8) StrDecimalToUint64 (TempStr);
        FreePool (TempStr);
        TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.date.Month);
      } else {
        HiiValue.Value.date.Day = (UINT8) StrDecimalToUint64 (TempStr);
        FreePool (TempStr);
        TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.date.Day);
      }
      LetDateRegular (&HiiValue);

      if (IsEnterVisibleChar) {
        //
        // Prepare HIIValue
        //
        HiiValue.Type = EFI_IFR_TYPE_STRING;
        HiiValue.BufferLen = (UINT16) StrSize (TempStr);
        HiiValue.Buffer = (UINT8 *) TempStr;
        NotifyStr = TempStr;
        //
        // Press visible char or backspace or tab, notify Changing Question (CHANGING_Q)
        // (In Date: ChangingQNotify->BodyHiiValue store "Current Date Index Str")
        // (In Date: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
        //
        Status = SendChangingQNotify (FBProtocol, &HiiValue, &mDEPrivate->PopUpSelected->HiiValue);
        break;
      }
      //
      // Change Select
      //
      Select = mDEPrivate->PopUpSelected;
      if (CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &Select)) {
        if (Select == NULL) {
          //
          // Same as Standard
          //
          FreePool (TempStr);
          return EFI_NOT_FOUND;
        } else if (Select == mDEPrivate->PopUpSelected && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
          //
          // Reset TabIndex
          //
          TabIndex = 0;
          FreePool (TempStr);

          if (mDEPrivate->PopUpSelected->HiiValue.Value.b) {
            Status = SendChangeQNotify (0, 0, &HiiValue);
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
          break;
        } else {
          if (Select->Selectable && !Select->Modifiable) {
            //
            // Prepare HIIValue
            //
            HiiValue.Type = EFI_IFR_TYPE_STRING;
            HiiValue.BufferLen = (UINT16) StrSize (TempStr);
            HiiValue.Buffer = (UINT8 *) TempStr;
            NotifyStr = TempStr;
            //
            // Press visible char or backspace or tab, notify Changing Question (CHANGING_Q)
            // (In Date: ChangingQNotify->BodyHiiValue store "Current Date Index Str")
            // (In Date: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
            //
            Status = SendChangingQNotify (FBProtocol, &HiiValue, &Select->HiiValue);
            break;
          }
        }
      }
      break;

    case DISPLAY_ENGINE_STATUS_OPEN_D:
      TabIndex = 0;
      //
      // Create temp value controls
      //
      Status = FreeControlList (PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Pop-Up Input Dialog
      //
      OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
      Status = DisplayDialog (FALSE, &OpenDNotify->Dialog, &OldDialogPanel);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Set PopUpSelected
      //
      if (PopUpControls->ControlArray != NULL && OpenDNotify->Dialog.ButtonCount > 0 && PopUpControls->Count >= OpenDNotify->Dialog.ButtonCount) {
        mDEPrivate->PopUpSelected = &PopUpControls->ControlArray[PopUpControls->Count - OpenDNotify->Dialog.ButtonCount];
      }
      //
      // Init popup Month, Day, Year string
      //
      TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.date.Month);
      Status = RefreshInputWithTab (PopUpControls, 1, TempStr);
      FreePool (TempStr);
      TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.date.Day);
      Status = RefreshInputWithTab (PopUpControls, 2, TempStr);
      FreePool (TempStr);
      TempStr = CatSPrint (NULL, L"%04d", FBProtocol->CurrentQ->HiiValue.Value.date.Year);
      Status = RefreshInputWithTab (PopUpControls, 0, TempStr);
      FreePool (TempStr);
      //
      // High-Light PopUpSelected
      //
      DisplayHighLightControl (TRUE, mDEPrivate->PopUpSelected);
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_D:
      Status = ShutDialog ();
      if (NotifyStr != NULL) {
        FreePool (NotifyStr);
        NotifyStr = NULL;
      }
      break;

    case DISPLAY_ENGINE_STATUS_CHANGING_Q:
      ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify;
      //
      // (In Date: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
      //
      Status = RefreshPopUpSelectionByValue (PopUpControls, FALSE, TRUE, &mDEPrivate->PopUpSelected->HiiValue);
      Status = RefreshPopUpSelectionByValue (PopUpControls, TRUE, TRUE, &ChangingQNotify->ButtonHiiValue);
      //
      // (In Date: ChangingQNotify->BodyHiiValue store "Current Input String")
      //
      TempStr = CatSPrint (NULL, (CHAR16 *) ChangingQNotify->BodyHiiValue.Buffer);
      Status = RefreshInputWithTab (PopUpControls, TabIndex, TempStr);
      FreePool (TempStr);
      //
      // Set PopUpSelected
      //
      GetPopUpSelectionByValue (PopUpControls, &ChangingQNotify->ButtonHiiValue, &mDEPrivate->PopUpSelected, NULL);
      break;

    case DISPLAY_ENGINE_STATUS_SELECT_Q:
      //
      // Create temp value controls
      //
      Status = FreeControlList (PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      Status = CreateValueControls (FBProtocol->CurrentQ, PromptControls, ValueControls, PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Init Strings
      //
      if (TabIndex == 0) {
        TempStr = CatSPrint (NULL, L"%04d", FBProtocol->CurrentQ->HiiValue.Value.date.Year);
      } else if (TabIndex == 1) {
        TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.date.Month);
      } else {
        TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.date.Day);
      }
      Status = RefreshInputWithTab (PopUpControls, TabIndex, TempStr);
      FreePool (TempStr);
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_Q:
      TabIndex = 0;
      break;
  }
  return Status;
}

EFI_STATUS
AtTime (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY,
  IN       H2O_CONTROL_LIST                   *PromptControls,
  IN       H2O_CONTROL_LIST                   *ValueControls,
  IN       H2O_CONTROL_LIST                   *PopUpControls
  )
{
  EFI_STATUS                                  Status;

  STATIC H2O_PANEL_INFO                       *OldDialogPanel = NULL;
  H2O_CONTROL_INFO                            *Select;

  BOOLEAN                                     IsEnterVisibleChar;
  EFI_HII_VALUE                               HiiValue;

  STATIC UINT32                               TabIndex = 0;
  STATIC CHAR16                               *NotifyStr = NULL;
  CHAR16                                      *TempStr = NULL;

  H2O_DISPLAY_ENGINE_EVT_CHANGING_Q           *ChangingQNotify;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D               *OpenDNotify;
  UINT8                                       InputValue;


  Status = EFI_NOT_FOUND;
  switch (DEStatus) {

    case DISPLAY_ENGINE_STATUS_AT_MENU:
      //
      // Check Input
      //
      IsEnterVisibleChar = FALSE;
      if (Keyboard != NULL && *Keyboard) {
        ProcessNumberInput (PopUpControls, FALSE, Key, &TabIndex, &IsEnterVisibleChar, &TempStr);
      } else {
        GetInputWithTab (PopUpControls, TabIndex, &TempStr);
      }

      //
      // Regular Time HIIValue
      //
      CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
      if (!IsEnterVisibleChar || TempStr == NULL) {
        if (TabIndex == 0) {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Hour);
        } else if (TabIndex == 1) {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Minute);
        } else {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Second);
        }
      }

      if (TempStr[0] == '-') {
        InputValue = (UINT8) (-1);
      } else {
        InputValue = (UINT8) StrDecimalToUint64 (TempStr);
      }

      if (TabIndex == 0) {
        HiiValue.Value.time.Hour = InputValue;
      } else if (TabIndex == 1) {
        HiiValue.Value.time.Minute = InputValue;
      } else {
        HiiValue.Value.time.Second = InputValue;
      }
      FreePool (TempStr);
      LetTimeRegular (&HiiValue);

      if (IsEnterVisibleChar) {
        //
        // Notify Change Question (CHANGE_Q)
        //
        Status = SendChangeQNotify (FBProtocol->CurrentQ->PageId, FBProtocol->CurrentQ->QuestionId, &HiiValue);
        break;
      } else {
        //
        // Same as Standard
        //
        return EFI_NOT_FOUND;
      }
      break;

    case DISPLAY_ENGINE_STATUS_AT_POPUP_DIALOG:
      if (NotifyStr != NULL) {
        FreePool (NotifyStr);
        NotifyStr = NULL;
      }
      //
      // Check Input
      //
      IsEnterVisibleChar = FALSE;
      if (Keyboard != NULL && *Keyboard) {
        ProcessNumberInput (PopUpControls, FALSE, Key, &TabIndex, &IsEnterVisibleChar, &TempStr);
      } else {
        GetInputWithTab (PopUpControls, TabIndex, &TempStr);
      }

      //
      // Regular Time HIIValue and TempStr
      //
      CopyMem (&HiiValue, &FBProtocol->CurrentQ->HiiValue, sizeof (EFI_HII_VALUE));
      if (!IsEnterVisibleChar || TempStr == NULL) {
        if (TabIndex == 0) {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Hour);
        } else if (TabIndex == 1) {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Minute);
        } else {
          TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Second);
        }
      }
      if (TabIndex == 0) {
        HiiValue.Value.time.Hour = (UINT8) StrDecimalToUint64 (TempStr);
        FreePool (TempStr);
        TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Hour);
      } else if (TabIndex == 1) {
        HiiValue.Value.time.Minute = (UINT8) StrDecimalToUint64 (TempStr);
        FreePool (TempStr);
        TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Minute);
      } else {
        HiiValue.Value.time.Second = (UINT8) StrDecimalToUint64 (TempStr);
        FreePool (TempStr);
        TempStr = CatSPrint (NULL, L"%02d", HiiValue.Value.time.Second);
      }
      LetTimeRegular (&HiiValue);

      if (IsEnterVisibleChar) {
        //
        // Prepare HIIValue
        //
        HiiValue.Type = EFI_IFR_TYPE_STRING;
        HiiValue.BufferLen = (UINT16) StrSize (TempStr);
        HiiValue.Buffer = (UINT8 *) TempStr;
        NotifyStr = TempStr;
        //
        // Press visible char or backspace or tab, notify Changing Question (CHANGING_Q)
        // (In Time: ChangingQNotify->BodyHiiValue store "Current Time Index Str")
        // (In Time: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
        //
        Status = SendChangingQNotify (FBProtocol, &HiiValue, &mDEPrivate->PopUpSelected->HiiValue);
        break;
      }
      //
      // Change Select
      //
      Select = mDEPrivate->PopUpSelected;
      if (CheckPressControls (Keyboard == NULL ? FALSE : *Keyboard, Key, MouseX, MouseY, PopUpControls->Count, PopUpControls->ControlArray, &Select)) {
        if (Select == NULL) {
          //
          // Same as Standard
          //
          FreePool (TempStr);
          return EFI_NOT_FOUND;
        } else if (Select == mDEPrivate->PopUpSelected && Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
          //
          // Reset TabIndex
          //
          TabIndex = 0;
          FreePool (TempStr);

          if (mDEPrivate->PopUpSelected->HiiValue.Value.b) {
            Status = SendChangeQNotify (0, 0, &HiiValue);
          } else {
            Status = SendShutDNotify (FBProtocol);
          }
          break;
        } else {
          if (Select->Selectable && !Select->Modifiable) {
            //
            // Prepare HIIValue
            //
            HiiValue.Type = EFI_IFR_TYPE_STRING;
            HiiValue.BufferLen = (UINT16) StrSize (TempStr);
            HiiValue.Buffer = (UINT8 *) TempStr;
            NotifyStr = TempStr;
            //
            // Press visible char or backspace or tab, notify Changing Question (CHANGING_Q)
            // (In Time: ChangingQNotify->BodyHiiValue store "Current Time Index Str")
            // (In Time: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
            //
            Status = SendChangingQNotify (FBProtocol, &HiiValue, &Select->HiiValue);
            break;
          }
        }
      }
      break;

    case DISPLAY_ENGINE_STATUS_OPEN_D:
      TabIndex = 0;
      //
      // Create temp value controls
      //
      Status = FreeControlList (PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Pop-Up Input Dialog
      //
      OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
      Status = DisplayDialog (FALSE, &OpenDNotify->Dialog, &OldDialogPanel);
      if (EFI_ERROR (Status)) {
        break;
      }

      //
      // Set PopUpSelected
      //
      if (PopUpControls->ControlArray != NULL && OpenDNotify->Dialog.ButtonCount > 0 && PopUpControls->Count >= OpenDNotify->Dialog.ButtonCount) {
        mDEPrivate->PopUpSelected = &PopUpControls->ControlArray[PopUpControls->Count - OpenDNotify->Dialog.ButtonCount];
      }
      //
      // Init popup Minute, Second, Hour string
      //
      TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.time.Minute);
      Status = RefreshInputWithTab (PopUpControls, 1, TempStr);
      FreePool (TempStr);
      TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.time.Second);
      Status = RefreshInputWithTab (PopUpControls, 2, TempStr);
      FreePool (TempStr);
      TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.time.Hour);
      Status = RefreshInputWithTab (PopUpControls, 0, TempStr);
      FreePool (TempStr);
      //
      // High-Light PopUpSelected
      //
      DisplayHighLightControl (TRUE, mDEPrivate->PopUpSelected);
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_D:
      Status = ShutDialog ();
      if (NotifyStr != NULL) {
        FreePool (NotifyStr);
        NotifyStr = NULL;
      }
      break;

    case DISPLAY_ENGINE_STATUS_CHANGING_Q:
      ChangingQNotify = (H2O_DISPLAY_ENGINE_EVT_CHANGING_Q *) Notify;
      //
      // (In Time: ChangingQNotify->ButtonHiiValue store "PopUpSelected")
      //
      Status = RefreshPopUpSelectionByValue (PopUpControls, FALSE, TRUE, &mDEPrivate->PopUpSelected->HiiValue);
      Status = RefreshPopUpSelectionByValue (PopUpControls, TRUE, TRUE, &ChangingQNotify->ButtonHiiValue);
      //
      // (In Time: ChangingQNotify->BodyHiiValue store "Current Input String")
      //
      TempStr = CatSPrint (NULL, (CHAR16 *) ChangingQNotify->BodyHiiValue.Buffer);
      Status = RefreshInputWithTab (PopUpControls, TabIndex, TempStr);
      FreePool (TempStr);
      //
      // Set PopUpSelected
      //
      GetPopUpSelectionByValue (PopUpControls, &ChangingQNotify->ButtonHiiValue, &mDEPrivate->PopUpSelected, NULL);
      break;

    case DISPLAY_ENGINE_STATUS_SELECT_Q:
      //
      // Create temp value controls
      //
      Status = FreeControlList (PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      Status = CreateValueControls (FBProtocol->CurrentQ, PromptControls, ValueControls, PopUpControls);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Init Strings
      //
      if (TabIndex == 0) {
        TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.time.Hour);
      } else if (TabIndex == 1) {
        TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.time.Minute);
      } else {
        TempStr = CatSPrint (NULL, L"%02d", FBProtocol->CurrentQ->HiiValue.Value.time.Second);
      }
      Status = RefreshInputWithTab (PopUpControls, TabIndex, TempStr);
      FreePool (TempStr);
      break;

    case DISPLAY_ENGINE_STATUS_SHUT_Q:
      TabIndex = 0;
      break;
  }
  return Status;
}

EFI_STATUS
CheckSpecificQuestion (
  IN       H2O_FORM_BROWSER_PROTOCOL          *FBProtocol,
  IN       UINT8                              DEStatus,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify,
  IN OUT   BOOLEAN                            *Keyboard,
  IN OUT   EFI_INPUT_KEY                      *Key,
  IN       UINT32                             MouseX,
  IN       UINT32                             MouseY
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_Q                          *Question;
  H2O_DISPLAY_ENGINE_EVT_SHUT_Q               *ShutQNotify;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D               *OpenDNotify;
  STATIC BOOLEAN                              IsQuestion = TRUE;
  STATIC BOOLEAN                              HasInput = FALSE;
  H2O_CONTROL_LIST                            PromptControls;
  H2O_CONTROL_LIST                            ValueControls;
  H2O_CONTROL_LIST                            *PopUpControls;

  STATIC UINT8                                OpCode = 0;
  LIST_ENTRY                                  *PanelLink;
  H2O_PANEL_INFO                              *Panel;

  ZeroMem (&PromptControls, sizeof (H2O_CONTROL_LIST));
  ZeroMem (&ValueControls, sizeof (H2O_CONTROL_LIST));
  PopUpControls = NULL;

  if (mDEPrivate->Layout != NULL) {
    PanelLink = &mDEPrivate->Layout->PanelListHead;
    if (IsNull (PanelLink, PanelLink->ForwardLink)) {
      return EFI_NOT_FOUND;
    }

    Panel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_SETUP_PAGE);
    if (Panel != NULL) {
      PromptControls.Count        = Panel->ControlList.Count / 2;
      PromptControls.ControlArray = Panel->ControlList.ControlArray;
      ValueControls.Count         = Panel->ControlList.Count / 2;
      ValueControls.ControlArray  = Panel->ControlList.ControlArray + PromptControls.Count;
    }

    Panel = GetPanelInfoByType (PanelLink, H2O_PANEL_TYPE_QUESTION);
    if (Panel != NULL) {
      PopUpControls = &Panel->ControlList;
    }
  }

  Question = NULL;
  if (mDEPrivate->MenuSelected != NULL && !IsDialogStatus (DEStatus)) {
    Status = FBProtocol->GetQInfo (FBProtocol, mDEPrivate->MenuSelected->PageId, mDEPrivate->MenuSelected->QuestionId, &Question);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (DEStatus == DISPLAY_ENGINE_STATUS_SHUT_Q) {
      SafeFreePool ((VOID **) &Question);

      ShutQNotify = (H2O_DISPLAY_ENGINE_EVT_SHUT_Q *) Notify;
      //
      // SHUT_Q status must get another specific question
      //
      Status = FBProtocol->GetQInfo (FBProtocol, ShutQNotify->PageId, ShutQNotify->QuestionId, &Question);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    OpCode = Question->Operand;
  }

  if (DEStatus == DISPLAY_ENGINE_STATUS_OPEN_D) {
    if (PopUpControls == NULL) {
      //
      // BugBug: InitLayout maybe need Page information
      //
      Status = InitLayout ();
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Panel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
      if (Panel == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      PopUpControls = &Panel->ControlList;
    }

    OpenDNotify = (H2O_DISPLAY_ENGINE_EVT_OPEN_D *) Notify;
    IsQuestion  = ((OpenDNotify->Dialog.DialogType & H2O_FORM_BROWSER_D_TYPE_QUESTIONS) == 0) ? FALSE : TRUE;
    OpCode      = IsQuestion ? GetOperandByDialogType (OpenDNotify->Dialog.DialogType) : 0;
    HasInput    = (OpenDNotify->Dialog.BodyInputCount != 0) ? TRUE : FALSE;
  }

  Status = EFI_NOT_FOUND;

  if (!IsQuestion) {
    if (DEStatus == DISPLAY_ENGINE_STATUS_SHUT_D) {
      IsQuestion = TRUE;
    }

    if (HasInput) {
      Status = AtSimpleInputDialog (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    } else {
      Status = AtSimpleDialog (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    }

    SafeFreePool ((VOID **) &Question);
    return Status;
  }

  if (PopUpControls == NULL) {
    if (Question != NULL) {
      FreePool (Question);
    }
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;

  switch (OpCode) {

  case EFI_IFR_REF_OP:
  case EFI_IFR_ACTION_OP:
  case EFI_IFR_RESET_BUTTON_OP:
    break;

  case EFI_IFR_CHECKBOX_OP:
    Status = AtCheckBox (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;

  case EFI_IFR_NUMERIC_OP:
    Status = AtNumeric (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;

  case EFI_IFR_ONE_OF_OP:
    Status = AtOneOf (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;

  case EFI_IFR_ORDERED_LIST_OP:
    Status = AtOrderList (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;

  case EFI_IFR_PASSWORD_OP:
    Status = AtPassword (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, PopUpControls);
    break;

  case EFI_IFR_STRING_OP:
    Status = AtString (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;

  case EFI_IFR_DATE_OP:
    Status = AtDate (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;

  case EFI_IFR_TIME_OP:
    Status = AtTime (FBProtocol, DEStatus, Notify, Keyboard, Key, MouseX, MouseY, &PromptControls, &ValueControls, PopUpControls);
    break;
  }

  SafeFreePool ((VOID **) &Question);

  return Status;
}

