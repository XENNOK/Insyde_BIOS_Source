/** @file
 UI implement functions for SetupBrowserDxe driver

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
#include "Setup.h"
#include "SetupUtility.h"

#include <Library/DxeOemSvcKernelLib.h>

#include <Protocol/GifDecoder.h>

#define ANI_PER_COL 3
#define ANI_PAD_X   128
#define ANI_PAD_Y   100
LIST_ENTRY                   Menu;
LIST_ENTRY                   gMenuList = INITIALIZE_LIST_HEAD_VARIABLE (gMenuList);
MENU_REFRESH_ENTRY           *gMenuRefreshHead;
BOOLEAN                      gUpArrow   = FALSE;
BOOLEAN                      gDownArrow = FALSE;
UINTN                        gUpArrowX;
UINTN                        gUpArrowY;
UINTN                        gDownArrowX;
UINTN                        gDownArrowY;
LIST_ENTRY                   gAnimationList        = INITIALIZE_LIST_HEAD_VARIABLE (gAnimationList);
LIST_ENTRY                   gAnimationRefreshList = INITIALIZE_LIST_HEAD_VARIABLE (gAnimationRefreshList);
EFI_GIF_DECODER_PROTOCOL     *mGifDecoder;
EFI_GRAPHICS_OUTPUT_PROTOCOL *mGop      = NULL;
EFI_GRAPHICS_OUTPUT_PROTOCOL **mGopList = NULL;
UINTN                        mGopCount;
BOOLEAN                      gPopUpDialogBiggerThanPlatformDialog;
BOOLEAN                      mRepaintFormFlag;

//
// Search table for UiDisplayMenu()
//
SCAN_CODE_TO_SCREEN_OPERATION     gScanCodeToOperation[] = {
  { SCAN_UP,          CHAR_NULL,               UiUp            },
  { SCAN_DOWN,        CHAR_NULL,               UiDown          },
  { SCAN_PAGE_UP,     CHAR_NULL,               UiPageUp        },
  { SCAN_PAGE_DOWN,   CHAR_NULL,               UiPageDown      },
  { SCAN_ESC,         CHAR_NULL,               UiReset         },
  { SCAN_F1,          CHAR_NULL,               UiShowHelpScreen},
  { SCAN_LEFT,        CHAR_NULL,               UiLeft          },
  { SCAN_RIGHT,       CHAR_NULL,               UiRight         },
  { SCAN_F9,          CHAR_NULL,               UiDefault       },
  { SCAN_F10,         CHAR_NULL,               UiSaveAndExit   },
  { 0,                0,                       UiMaxOperation  }
};

SCREEN_OPERATION_T0_CONTROL_FLAG  gScreenOperationToControlFlag[] = {
  { UiNoOperation,    CfUiNoOperation    },
  { UiDefault,        CfUiDefault        },
  { UiSelect,         CfUiSelect         },
  { UiUp,             CfUiUp             },
  { UiDown,           CfUiDown           },
  { UiLeft,           CfUiLeft           },
  { UiRight,          CfUiRight          },
  { UiReset,          CfUiReset          },
  { UiSave,           CfUiSave           },
  { UiSaveCustom,     CfUiSaveCustom     },
  { UiDefaultCustom,  CfUiDefaultCustom  },
  { UiSaveAndExit,    CfUiSaveAndExit    },
  { UiSave,           CfUiSave           },
  { UiPageUp,         CfUiPageUp         },
  { UiPageDown,       CfUiPageDown       },
  { UiDiscard,        CfUiDiscard        },
  { UiJumpMenu,       CfUiJumpMenu       },
  { UiShowHelpScreen, CfUiShowHelpScreen }
};

/**
 Swap bytes in the buffer.

 @param[in, out] Buffer                 Binary buffer.
 @param[in]      BufferSize             Size of the buffer in bytes.
**/
VOID
SwapBuffer (
  IN OUT UINT8     *Buffer,
  IN UINTN         BufferSize
  )
{
  UINTN  Index;
  UINT8  Temp;
  UINTN  SwapCount;

  SwapCount = BufferSize / 2;
  for (Index = 0; Index < SwapCount; Index++) {
    Temp = Buffer[Index];
    Buffer[Index] = Buffer[BufferSize - 1 - Index];
    Buffer[BufferSize - 1 - Index] = Temp;
  }
}

/**
 Converts Hex String to binary buffer in reversed byte order from HexStringToBuf().

 @param[in, out] Buffer         Pointer to buffer that receives the data.
 @param[in, out] BufferSize     Length in bytes of the buffer to hold converted data.
                                If routine return with EFI_SUCCESS, containing length of converted data.
                                If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
 @param[in]      Str            String to be converted from.

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
HexStringToBuffer (
  IN OUT UINT8         *Buffer,
  IN OUT UINTN         *BufferSize,
  IN CHAR16            *Str
  )
{
  EFI_STATUS  Status;
  UINTN       ConvertedStrLen;

  ConvertedStrLen = 0;
  Status = HexStringToBuf (Buffer, BufferSize, Str, &ConvertedStrLen);
  if (!EFI_ERROR (Status)) {
    SwapBuffer (Buffer, (ConvertedStrLen + 1) / 2);
  }

  return Status;
}

/**
 Set Buffer to Value for Size bytes.

 @param[in] Buffer                 Memory to set.
 @param[in] Size                   Number of bytes to set
 @param[in] Value                  Value of the set operation.
**/
VOID
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  )
{
  CHAR16  *Ptr;

  Ptr = Buffer;
  while (Size--) {
    *(Ptr++) = Value;
  }
}

/**
 Initialize Menu option list.
**/
VOID
UiInitMenu (
  VOID
  )
{
  InitializeListHead (&Menu);
}

/**
 Free Menu option linked list.
**/
VOID
UiFreeMenu (
  VOID
  )
{
  UI_MENU_OPTION  *MenuOption;

  while (!IsListEmpty (&Menu)) {
    MenuOption = MENU_OPTION_FROM_LINK (Menu.ForwardLink);
    RemoveEntryList (&MenuOption->Link);

    //
    // We allocated space for this description when we did a GetToken, free it here
    //
    if (MenuOption->Skip != 0) {
      //
      // For date/time, MenuOption->Description is shared by three Menu Options
      // Data format :      [01/02/2004]      [11:22:33]
      // Line number :        0  0    1         0  0  1
      //
      gBS->FreePool (MenuOption->Description);
    }
    gBS->FreePool (MenuOption);
  }
}

/**
 Create a menu with specified formset GUID and form ID, and add it as a child
 of the given parent menu.

 @param[in, out] Parent                 The parent of menu to be added.
 @param[in]      FormSetGuid            The Formset Guid of menu to be added.
 @param[in]      FormId                 The Form ID of menu to be added.

 @return A pointer to the newly added menu or NULL if memory is insufficient.
**/
UI_MENU_LIST *
UiAddMenuList (
  IN OUT UI_MENU_LIST     *Parent,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  )
{
  UI_MENU_LIST  *MenuList;

  MenuList = AllocateZeroPool (sizeof (UI_MENU_LIST));
  if (MenuList == NULL) {
    return NULL;
  }

  MenuList->Signature = UI_MENU_LIST_SIGNATURE;
  InitializeListHead (&MenuList->ChildListHead);

  CopyMem (&MenuList->FormSetGuid, FormSetGuid, sizeof (EFI_GUID));
  MenuList->FormId = FormId;
  MenuList->Parent = Parent;

  if (Parent == NULL) {
    //
    // The parent is not specified, so this is the root Form of a Formset
    //
    InsertTailList (&gMenuList, &MenuList->Link);
  } else {
    InsertTailList (&Parent->ChildListHead, &MenuList->Link);
  }

  return MenuList;
}

/**
 Search Menu with given FormId in the parent menu and all its child menu.

 @param[in] Parent                 The parent of menu to search.
 @param[in] FormId                 The Form ID of menu to search.

 @return A pointer to menu found or NULL if not found.
**/
UI_MENU_LIST *
UiFindChildMenuList (
  IN UI_MENU_LIST         *Parent,
  IN UINT16               FormId
  )
{
  LIST_ENTRY      *Link;
  UI_MENU_LIST    *Child;
  UI_MENU_LIST    *MenuList;

  if (Parent->FormId == FormId) {
    return Parent;
  }

  Link = GetFirstNode (&Parent->ChildListHead);
  while (!IsNull (&Parent->ChildListHead, Link)) {
    Child = UI_MENU_LIST_FROM_LINK (Link);

    MenuList = UiFindChildMenuList (Child, FormId);
    if (MenuList != NULL) {
      return MenuList;
    }

    Link = GetNextNode (&Parent->ChildListHead, Link);
  }

  return NULL;
}


/**
 Search Menu with given FormSetGuid and FormId in all menu list.

 @param[in] FormSetGuid            The Formset GUID of the menu to search.
 @param[in] FormId                 The Form ID of menu to search.

 @return A pointer to menu found or NULL if not found.
**/
UI_MENU_LIST *
UiFindMenuList (
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  )
{
  LIST_ENTRY      *Link;
  UI_MENU_LIST    *MenuList;
  UI_MENU_LIST    *Child;

  Link = GetFirstNode (&gMenuList);
  while (!IsNull (&gMenuList, Link)) {
    MenuList = UI_MENU_LIST_FROM_LINK (Link);

    if (CompareGuid (FormSetGuid, &MenuList->FormSetGuid)) {
      //
      // This is the formset we are looking for, find the form in this formset
      //
      Child = UiFindChildMenuList (MenuList, FormId);
      if (Child != NULL) {
        return Child;
      }
    }

    Link = GetNextNode (&gMenuList, Link);
  }

  return NULL;
}

/**
 Free Menu option linked list.
**/
VOID
UiFreeRefreshList (
  VOID
  )
{
  MENU_REFRESH_ENTRY  *OldMenuRefreshEntry;

  while (gMenuRefreshHead != NULL) {
    OldMenuRefreshEntry = gMenuRefreshHead->Next;
    gBS->FreePool (gMenuRefreshHead);
    gMenuRefreshHead = OldMenuRefreshEntry;
  }

  gMenuRefreshHead = NULL;
}

/**
 Refresh screen.

 @param[in] RefereshDataTime    Indicate if it is to refresh time or date option

 @retval EFI_SUCCESS            Refresh successfully.
 @retval EFI_TIME_OUT           No refresh menu option is updated.
**/
EFI_STATUS
RefreshForm (
  IN BOOLEAN     RefereshDataTime
  )
{
  CHAR16                          *OptionString;
  MENU_REFRESH_ENTRY              *MenuRefreshEntry;
  UINTN                           Index;
  EFI_STATUS                      Status;
  UI_MENU_SELECTION               *Selection;
  FORM_BROWSER_STATEMENT          *Question;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_HII_VALUE                   *HiiValue;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  BOOLEAN                         SkipValueMatch;
  UINT16                          SkipValue;
  UINT16                          PromptSkipValue;
  CHAR16                          *String;
  UINT16                          Width;

  String = NULL;
  SkipValueMatch = TRUE;
  if (gMenuRefreshHead != NULL) {
    MenuRefreshEntry = gMenuRefreshHead;

    //
    // Reset FormPackage update flag
    //
    mHiiPackageListUpdated = FALSE;

    do {
      Selection = MenuRefreshEntry->Selection;
      Question = MenuRefreshEntry->MenuOption->ThisTag;

      if (!RefereshDataTime || Question->Operand == EFI_IFR_DATE_OP || Question->Operand == EFI_IFR_TIME_OP) {
        Status = GetQuestionValue (Selection->FormSet, Selection->Form, Question, FALSE);

        if (!EFI_ERROR (Status) && SkipValueMatch) {
          OptionString = NULL;
          ProcessOptions (Selection, MenuRefreshEntry->MenuOption, FALSE, &OptionString);
          GetLineByWidth (OptionString, (UINT16) gOptionBlockWidth, &SkipValue, &String);
          ScuSafeFreePool ((VOID **)&String);

          Width  = GetWidth (MenuRefreshEntry->MenuOption->ThisTag, Selection->Handle) - 1;
          PromptSkipValue = 0;
          GetLineByWidth (MenuRefreshEntry->MenuOption->Description, Width, &PromptSkipValue, &String);
          ScuSafeFreePool ((VOID **)&String);

          if ((Question->Operand != EFI_IFR_DATE_OP) &&
              (Question->Operand != EFI_IFR_TIME_OP) &&
              ((MenuRefreshEntry->MenuOption->Skip == PromptSkipValue &&
                (UINTN) SkipValue > MenuRefreshEntry->MenuOption->Skip) ||
               (MenuRefreshEntry->MenuOption->Skip != PromptSkipValue &&
                (UINTN) SkipValue != MenuRefreshEntry->MenuOption->Skip))) {
            SkipValueMatch   = FALSE;
            mRepaintFormFlag = TRUE;
          } else {
            if (OptionString != NULL) {
              //
              // If leading spaces on OptionString - remove the spaces
              //
              for (Index = 0; OptionString[Index] == L' '; Index++)
                ;

              //
              // If old Text is longer than new string, need to clean the old string before paint the newer.
              // This option is no need for time/date opcode, because time/data opcode has fixed string length.
              //
              if ((Question->Operand != EFI_IFR_DATE_OP) && (Question->Operand != EFI_IFR_TIME_OP)) {
                ClearLines (
                  MenuRefreshEntry->CurrentColumn,
                  MenuRefreshEntry->CurrentColumn + gOptionBlockWidth,
                  MenuRefreshEntry->CurrentRow,
                  MenuRefreshEntry->CurrentRow + MenuRefreshEntry->MenuOption->Skip - MenuRefreshEntry->SkipValue - 1,
                  MenuRefreshEntry->CurrentAttribute
                  );
              }
              gST->ConOut->SetAttribute (gST->ConOut, MenuRefreshEntry->CurrentAttribute);
              DisplayMenuOptionValue (
                MenuRefreshEntry->MenuOption,
                &OptionString[Index],
                MenuRefreshEntry->SkipValue,
                gScreenDimensions.TopRow + 3,
                gScreenDimensions.BottomRow - 4
                );
            }
          }
          ScuSafeFreePool ((VOID **)&OptionString);
        }

      //
      // Question value may be changed, need invoke its Callback()
      //
      ConfigAccess = Selection->FormSet->ConfigAccess;
      if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) && (ConfigAccess != NULL)) {
        ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

        HiiValue = &Question->HiiValue;
        if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
          //
          // Create String in HII database for Configuration Driver to retrieve
          //
          HiiValue->Value.string = NewString ((CHAR16 *) Question->BufferValue, Selection->FormSet->HiiHandle);
        }

        Status = ConfigAccess->Callback (
                                 ConfigAccess,
                                 EFI_BROWSER_ACTION_CHANGING,
                                 Question->QuestionId,
                                 HiiValue->Type,
                                 &HiiValue->Value,
                                 &ActionRequest
                                 );

        if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
          //
          // Clean the String in HII Database
          //
          DeleteString (HiiValue->Value.string, Selection->FormSet->HiiHandle);
        }

        if (!EFI_ERROR (Status)) {
          switch (ActionRequest) {
          case EFI_BROWSER_ACTION_REQUEST_RESET:
            gResetRequired = TRUE;
            break;

          case EFI_BROWSER_ACTION_REQUEST_SUBMIT:
            SubmitForm (Selection->FormSet, Selection->Form);
            break;

          case EFI_BROWSER_ACTION_REQUEST_EXIT:
            Selection->Action = UI_ACTION_EXIT;
            gNvUpdateRequired = FALSE;
            break;

          default:
            break;
            }
          }
        }
      }

      MenuRefreshEntry = MenuRefreshEntry->Next;

    } while (MenuRefreshEntry != NULL);

    if (mHiiPackageListUpdated) {
      //
      // Package list is updated, force to reparse IFR binary of target Formset
      //
      mHiiPackageListUpdated = FALSE;
      Selection->Action = UI_ACTION_REFRESH_FORMSET;
      return EFI_SUCCESS;
    }

    if (mRepaintFormFlag) {
      return EFI_SUCCESS;
    }
  }

  return EFI_TIMEOUT;
}

/**
 Wait for a given event to fire, or for an optional timeout to expire.

 @param[in] Event               The event to wait for
 @param[in] Timeout             An optional timeout value in 100 ns units.
 @param[in] RefreshInterval     Menu refresh interval (in seconds).

 @retval EFI_SUCCESS            Event fired before Timeout expired.
 @retval EFI_TIME_OUT           Timout expired before Event fired.
**/
EFI_STATUS
UiWaitForSingleEvent (
  IN EFI_EVENT                Event,
  IN UINT64                   Timeout, OPTIONAL
  IN UINT8                    RefreshInterval OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  EFI_EVENT   TimerEvent;
  EFI_EVENT   WaitList[2];

  //
  // Update screen every RefreshInterval second
  //
  if (!Timeout)
  {
    if (RefreshInterval == 0) {
      Timeout = ONE_SECOND;
    } else {
      Timeout = RefreshInterval * ONE_SECOND;
    }
  }

  do {
    Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);

    //
    // Set the timer event
    //
    gBS->SetTimer (
          TimerEvent,
          TimerRelative,
          Timeout
          );

    //
    // Wait for the original event or the timer
    //
    WaitList[0] = Event;
    WaitList[1] = TimerEvent;
    Status      = gBS->WaitForEvent (2, WaitList, &Index);

    //
    // If the timer expired, update anything that needs a refresh and keep waiting
    //
    if (!EFI_ERROR (Status) && Index == 1) {
      Status = EFI_TIMEOUT;
      if (RefreshInterval != 0) {
        Status = RefreshForm (FALSE);
      }
    }

    gBS->CloseEvent (TimerEvent);
  } while (Status == EFI_TIMEOUT);

  return Status;
}

/**
 Add one menu option by specified description and context.

 @param[in] String              String description for this option.
 @param[in] Handle              Hii handle for the package list.
 @param[in] Statement           Statement of this Menu Option.
 @param[in] NumberOfLines       Display lines for this Menu Option.
 @param[in] MenuItemCount       The index for this Option in the Menu.

 @retval Pointer                Pointer to the added Menu Option.
**/
UI_MENU_OPTION *
UiAddMenuOption (
  IN CHAR16                  *String,
  IN EFI_HII_HANDLE          Handle,
  IN FORM_BROWSER_STATEMENT  *Statement,
  IN UINT16                  NumberOfLines,
  IN UINT16                  MenuItemCount
  )
{
  UI_MENU_OPTION  *MenuOption;
  UINTN           Index;
  UINTN           Count;

  Count = 1;
  MenuOption = NULL;

  if (Statement->Operand == EFI_IFR_DATE_OP || Statement->Operand == EFI_IFR_TIME_OP) {
    //
    // Add three MenuOptions for Date/Time
    // Data format :      [01/02/2004]      [11:22:33]
    // Line number :        0  0    1         0  0  1
    //
    NumberOfLines = 0;
    Count = 3;

    if (Statement->Storage == NULL) {
      //
      // For RTC type of date/time, set default refresh interval to be 1 second
      //
      if (Statement->RefreshInterval == 0) {
        Statement->RefreshInterval = 1;
      }
    }
  }

  for (Index = 0; Index < Count; Index++) {
    MenuOption = AllocateZeroPool (sizeof (UI_MENU_OPTION));
    ASSERT (MenuOption);
    if (MenuOption == NULL) {
      return NULL;
    }

    MenuOption->Signature   = UI_MENU_OPTION_SIGNATURE;
    MenuOption->Description = String;
    MenuOption->Handle      = Handle;
    MenuOption->ThisTag     = Statement;
    MenuOption->EntryNumber = MenuItemCount;

    if (Index == 2) {
      //
      // Override LineNumber for the MenuOption in Date/Time sequence
      //
      MenuOption->Skip = 1;
    } else {
      MenuOption->Skip = NumberOfLines;
    }
    MenuOption->Sequence = Index;

    if (Statement->GrayOutExpression != NULL) {
      MenuOption->GrayOut = Statement->GrayOutExpression->Result.Value.b;
    }

    switch (Statement->Operand) {

    case EFI_IFR_ORDERED_LIST_OP:
    case EFI_IFR_ONE_OF_OP:
    case EFI_IFR_NUMERIC_OP:
    case EFI_IFR_TIME_OP:
    case EFI_IFR_DATE_OP:
    case EFI_IFR_CHECKBOX_OP:
    case EFI_IFR_PASSWORD_OP:
    case EFI_IFR_STRING_OP:
      //
      // User could change the value of these items
      //
      MenuOption->IsQuestion = TRUE;
      break;

    default:
      MenuOption->IsQuestion = FALSE;
      break;
    }

    if ((Statement->ValueExpression != NULL) ||
        (Statement->QuestionFlags & EFI_IFR_FLAG_READ_ONLY)) {
      MenuOption->ReadOnly = TRUE;
    }

    InsertTailList (&Menu, &MenuOption->Link);
  }

  return MenuOption;
}

/**
 Routine used to abstract a generic dialog interface and return the selected key or string

 @param[in]  NumberOfLines      The number of lines for the dialog box
 @param[in]  HotKey             Defines whether a single character is parsed
                                (TRUE) and returned in KeyValue or a string is
                                returned in StringBuffer.  Two special characters
                                are considered when entering a string, a SCAN_ESC
                                and an CHAR_CARRIAGE_RETURN.  SCAN_ESC terminates
                                string input and returns
 @param[in]  MaximumStringSize  The maximum size in bytes of a typed in string
                                (each character is a CHAR16) and the minimum
                                string returned is two bytes
 @param[out] StringBuffer       The passed in pointer to the buffer which will
                                hold the typed in string if HotKey is FALSE
 @param[out] KeyValue           The EFI_KEY value returned if HotKey is TRUE..
 @param[in]  ...                A series of (quantity == NumberOfLines) text
                                strings which will be used to construct the dialog box

 @retval EFI_SUCCESS            Displayed dialog and received user interaction
 @retval EFI_INVALID_PARAMETER  One of the parameters was invalid (e.g.
                                (StringBuffer == NULL) && (HotKey == FALSE))
 @retval EFI_DEVICE_ERROR       User typed in an ESC character to exit the routine
**/
EFI_STATUS
EFIAPI
CreateDialog (
  IN  UINTN                       NumberOfLines,
  IN  BOOLEAN                     HotKey,
  IN  UINTN                       MaximumStringSize,
  OUT CHAR16                      *StringBuffer,
  OUT EFI_INPUT_KEY               *KeyValue,
  ...
  )
{
  VA_LIST       Marker;
  UINTN         Count;
  EFI_INPUT_KEY Key;
  UINTN         LargestString;
  CHAR16        *TempString;
  CHAR16        *BufferedString;
  CHAR16        *StackString;
  CHAR16        KeyPad[2];
  UINTN         Start;
  UINTN         Top;
  UINTN         Index;
  EFI_STATUS    Status;
  BOOLEAN       SelectionComplete;
  UINTN         InputOffset;
  UINTN         CurrentAttribute;
  UINTN         DimensionsWidth;
  UINTN         DimensionsHeight;
  BOOLEAN       CursorVisible;

  DimensionsWidth   = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight  = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  SelectionComplete = FALSE;
  InputOffset       = 0;
  TempString        = AllocateZeroPool (MaximumStringSize * 2);
  BufferedString    = AllocateZeroPool (MaximumStringSize * 2);
  CurrentAttribute  = gST->ConOut->Mode->Attribute;

  ASSERT (TempString);
  ASSERT (BufferedString);
  if (TempString == NULL || BufferedString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Zero the outgoing buffer
  //
  ZeroMem (StringBuffer, MaximumStringSize);

  if (HotKey) {
    if (KeyValue == NULL) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    if (StringBuffer == NULL) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Disable cursor
  //
  CursorVisible = gST->ConOut->Mode->CursorVisible;
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  LargestString = 0;

  VA_START (Marker, KeyValue);

  //
  // Determine the largest string in the dialog box
  // Notice we are starting with 1 since String is the first string
  //
  for (Count = 0; Count < NumberOfLines; Count++) {
    StackString = VA_ARG (Marker, CHAR16 *);

    if (StackString[0] == L' ') {
      InputOffset = Count + 1;
    }

    if ((GetStringWidth (StackString) / 2) > LargestString) {
      //
      // Size of the string visually and subtract the width by one for the null-terminator
      //
      LargestString = (GetStringWidth (StackString) / 2);
    }
  }
  VA_END (Marker);

  Start = (DimensionsWidth - LargestString - 2) / 2 + gScreenDimensions.LeftColumn + 1;
  Top   = ((DimensionsHeight - NumberOfLines - 2) / 2) + gScreenDimensions.TopRow - 1;

  Count = 0;

  //
  // Display the Popup
  //
  VA_START (Marker, KeyValue);
  CreateSharedPopUp (LargestString, NumberOfLines, Marker);
  VA_END (Marker);

  //
  // Take the first key typed and report it back?
  //
  if (HotKey) {
    Status = WaitForKeyStroke (&Key);
    ASSERT_EFI_ERROR (Status);
    CopyMem (KeyValue, &Key, sizeof (EFI_INPUT_KEY));

  } else {
    do {
      Status = WaitForKeyStroke (&Key);

      switch (Key.UnicodeChar) {

      case CHAR_NULL:
        switch (Key.ScanCode) {
        case SCAN_ESC:
          gBS->FreePool (TempString);
          gBS->FreePool (BufferedString);
          gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
          gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
          return EFI_DEVICE_ERROR;

        default:
          break;
        }

        break;

      case CHAR_CARRIAGE_RETURN:
        SelectionComplete = TRUE;
        gBS->FreePool (TempString);
        gBS->FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_SUCCESS;
        break;

      case CHAR_BACKSPACE:
        if (StringBuffer[0] != CHAR_NULL) {
          for (Index = 0; StringBuffer[Index] != CHAR_NULL; Index++) {
            TempString[Index] = StringBuffer[Index];
          }
          //
          // Effectively truncate string by 1 character
          //
          TempString[Index - 1] = CHAR_NULL;
          StrCpy (StringBuffer, TempString);
        }

      default:
        //
        // If it is the beginning of the string, don't worry about checking maximum limits
        //
        if ((StringBuffer[0] == CHAR_NULL) && (Key.UnicodeChar != CHAR_BACKSPACE)) {
          StrnCpy (StringBuffer, &Key.UnicodeChar, 1);
          StrnCpy (TempString, &Key.UnicodeChar, 1);
        } else if ((GetStringWidth (StringBuffer) < MaximumStringSize) && (Key.UnicodeChar != CHAR_BACKSPACE)) {
          KeyPad[0] = Key.UnicodeChar;
          KeyPad[1] = CHAR_NULL;
          StrCat (StringBuffer, KeyPad);
          StrCat (TempString, KeyPad);
        }
        //
        // If the width of the input string is now larger than the screen, we nee to
        // adjust the index to start printing portions of the string
        //
        SetUnicodeMem (BufferedString, LargestString, L' ');

        PrintStringAt (Start + 1, Top + InputOffset, BufferedString);

        if ((GetStringWidth (StringBuffer) / 2) > (DimensionsWidth - 2)) {
          Index = (GetStringWidth (StringBuffer) / 2) - DimensionsWidth + 2;
        } else {
          Index = 0;
        }

        for (Count = 0; Index + 1 < GetStringWidth (StringBuffer) / 2; Index++, Count++) {
          BufferedString[Count] = StringBuffer[Index];
        }

        PrintStringAt (Start + 1, Top + InputOffset, BufferedString);
        break;
      }
    } while (!SelectionComplete);
  }

  gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
  gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);

  return EFI_SUCCESS;
}

/**
 Draw a pop up windows based on the dimension, number of lines and
 strings specified.

 @param[in] RequestedWidth  The width of the pop-up.
 @param[in] NumberOfLines   The number of lines.
 @param[in] Marker          The variable argument list for the list of string to be printed.
**/
VOID
CreateSharedPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       NumberOfLines,
  IN  VA_LIST                     Marker
  )
{
  UINTN   Index;
  UINTN   Count;
  CHAR16  Character;
  UINTN   Start;
  UINTN   End;
  UINTN   Top;
  UINTN   Bottom;
  CHAR16  *String;
  UINTN   DimensionsWidth;
  UINTN   DimensionsHeight;

  DimensionsWidth   = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight  = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);

  if ((RequestedWidth + 2) > DimensionsWidth) {
    RequestedWidth = DimensionsWidth - 2;
  }

  //
  // Subtract the PopUp width from total Columns, allow for one space extra on
  // each end plus a border.
  //
  Start     = (DimensionsWidth - RequestedWidth - 2) / 2 + gScreenDimensions.LeftColumn + 1;
  End       = Start + RequestedWidth + 1;
  Top       = ((DimensionsHeight - NumberOfLines + (NumberOfLines & 0x1) - 2) / 2) \
              + gScreenDimensions.TopRow - 1;
  Bottom    = Top + NumberOfLines + 2;

  Character = BOXDRAW_DOWN_RIGHT;
  PrintCharAt (Start, Top, Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++) {
    PrintChar (Character);
  }

  Character = BOXDRAW_DOWN_LEFT;
  PrintChar (Character);
  Character = BOXDRAW_VERTICAL;

  Count = 0;
  for (Index = Top; Index + 2 < Bottom; Index++, Count++) {
    String = VA_ARG (Marker, CHAR16*);

    //
    // This will clear the background of the line - we never know who might have been
    // here before us.  This differs from the next clear in that it used the non-reverse
    // video for normal printing.
    //
    if (GetStringWidth (String) / 2 > 1) {
      ClearLines (Start, End, Index + 1, Index + 1, POPUP_TEXT | POPUP_BACKGROUND);
    }

    //
    // Passing in a space results in the assumption that this is where typing will occur
    //
    if (String[0] == L' ') {
      ClearLines (Start + 1, End - 1, Index + 1, Index + 1, POPUP_INVERSE_TEXT | POPUP_INVERSE_BACKGROUND);
    }

    //
    // Passing in a NULL results in a blank space
    //
    if (String[0] == CHAR_NULL) {
      ClearLines (Start, End, Index + 1, Index + 1, POPUP_TEXT | POPUP_BACKGROUND);
    }

    PrintStringAt (
      ((DimensionsWidth - GetStringWidth (String) / 2) / 2) + gScreenDimensions.LeftColumn + 1,
      Index + 1,
      String
      );
    gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);
    PrintCharAt (Start, Index + 1, Character);
    PrintCharAt (End - 1, Index + 1, Character);
  }

  Character = BOXDRAW_UP_RIGHT;
  PrintCharAt (Start, Bottom - 1, Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++) {
    PrintChar (Character);
  }

  Character = BOXDRAW_UP_LEFT;
  PrintChar (Character);
}

/**
 Draw a pop up windows based on the dimension, number of lines and
 strings specified.

 @param[in] RequestedWidth  The width of the pop-up.
 @param[in] NumberOfLines   The number of lines.
 @param[in] ...             A series of text strings that displayed in the pop-up.
**/
VOID
EFIAPI
CreateMultiStringPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       NumberOfLines,
  ...
  )
{
  VA_LIST Marker;

  VA_START (Marker, NumberOfLines);

  CreateSharedPopUp (RequestedWidth, NumberOfLines, Marker);

  VA_END (Marker);
}

/**
 Draw a password pop up windows based on the dimension, number of lines and
 strings specified.

 @param[in] RequestedWidth          The width of the pop-up.
 @param[in] PasswordPopUpType       Type of password pop-up.
 @param[in] MaxInputChar
 @param[in] NumberOfLines           The number of lines.
 @param[in] ArrayOfStrings
**/
VOID
CreateSharedPasswordPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       PasswordPopUpType,
  IN  UINTN                       MaxInputChar,
  IN  UINTN                       NumberOfLines,
  IN  CHAR16                      **ArrayOfStrings
  )
{
  UINTN                           Index;
  UINTN                           Count;
  CHAR16                          Character;
  UINTN                           Start;
  UINTN                           End;
  UINTN                           Top;
  UINTN                           Bottom;
  CHAR16                          *String;

  UINTN                           DimensionsWidth;
  UINTN                           DimensionsHeight;

  DimensionsWidth = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  Count = 0;

  gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);

  if ((RequestedWidth + 2)> DimensionsWidth) {
    RequestedWidth = DimensionsWidth - 2;
  }

  //
  // Subtract the PopUp width from total Columns, allow for one space extra on
  // each end plus a border.
  //
  Start = (DimensionsWidth - RequestedWidth - 2) / 2 + gScreenDimensions.LeftColumn + 1;
  End = Start + RequestedWidth + 1;
  Top = ((DimensionsHeight - NumberOfLines + (NumberOfLines & 0x1) - 2) / 2) \
        + gScreenDimensions.TopRow - 1;
  Bottom = Top + NumberOfLines + 2;

  Character = BOXDRAW_DOWN_RIGHT;
  PrintAt (Start, Top, L"%c", Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++ ) {
    Print (L"%c", Character);
  }
  Character = BOXDRAW_DOWN_LEFT;
  Print (L"%c", Character);

  ClearLines(Start, End, Top + 1, Top + 1, POPUP_TEXT | POPUP_BACKGROUND);
  Index = Top;
  String = ArrayOfStrings[0];
  PrintAt (((DimensionsWidth - GetStringWidth (String) / 2) / 2) + gScreenDimensions.LeftColumn + 1, Index + 1, L"%s", String);

  Character = BOXDRAW_VERTICAL;
  gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);
  PrintAt (Start, Index + 1, L"%c", Character);
  PrintAt (End - 1, Index + 1, L"%c", Character);

  Character = BOXDRAW_VERTICAL_RIGHT;
  PrintAt (Start, Top + 2, L"%c", Character);

  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++ ) {
    Print (L"%c", Character);
  }
  Character = BOXDRAW_VERTICAL_LEFT;
  Print (L"%c", Character);

  Count = 1;
  for (Index = Top + 2; Index + 2 < Bottom; Index++ ) {
    String = ArrayOfStrings[Count];
    Count++;

    if (PasswordPopUpType == PASSWORD_INPUT_TYPE) {
      ClearLines(Start, End, Index + 1, Index + 1, POPUP_TEXT | POPUP_BACKGROUND);

      PrintAt (Start + 1, Index + 1, L"%s", String);
      if (String[0] != 0x0000) {
        ClearLines((Start + (RequestedWidth - MaxInputChar - 2)), End - 1, Index + 1, Index + 1, POPUP_INVERSE_TEXT | POPUP_INVERSE_BACKGROUND);
        gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);
        Character = LEFT_NUMERIC_DELIMITER;
        PrintAt ((Start + (RequestedWidth - MaxInputChar - 2)), Index + 1, L"%c", Character);
        Character = RIGHT_NUMERIC_DELIMITER;
        PrintAt (End - 2, Index + 1, L"%c", Character);
      }
    } else if (PasswordPopUpType == PASSWORD_NOTICE_TYPE){
      //
      // This will clear the background of the line - we never know who might have been
      // here before us.  This differs from the next clear in that it used the non-reverse
      // video for normal printing.
      //
      if (GetStringWidth(String)/2 > 1) {
        ClearLines(Start, End, Index + 1, Index + 1, POPUP_TEXT | POPUP_BACKGROUND);
      }

      //
      // Passing in a space results in the assumption that this is where typing will occur
      //
      if (String[0] == L' ') {
        ClearLines(Start + 1, End - 1, Index + 1, Index + 1, POPUP_INVERSE_TEXT | POPUP_INVERSE_BACKGROUND);
      }

      //
      // Passing in a NULL results in a blank space
      //
      if (String[0] == 0x0000) {
        ClearLines(Start, End, Index + 1, Index + 1, POPUP_TEXT | POPUP_BACKGROUND);
      }
      PrintAt (((DimensionsWidth - GetStringWidth(String)/2)/2) + gScreenDimensions.LeftColumn + 1, Index + 1, L"%s", String);
    }

    gST->ConOut->SetAttribute (gST->ConOut, POPUP_TEXT | POPUP_BACKGROUND);
    PrintAt (Start, Index + 1, L"%c", BOXDRAW_VERTICAL);
    PrintAt (End - 1, Index + 1, L"%c", BOXDRAW_VERTICAL);
  }
  Character = BOXDRAW_UP_RIGHT;
  PrintAt (Start, Bottom - 1, L"%c", Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++ ) {
    Print (L"%c", Character);
  }
  Character = BOXDRAW_UP_LEFT;
  Print (L"%c", Character);
}

/**
 Draw a password pop up windows based on the dimension, number of lines and
 strings specified.

 @param[in] RequestedWidth          The width of the pop-up.
 @param[in] PasswordPopUpType       Type of password pop-up.
 @param[in] MaxInputChar
 @param[in] NumberOfLines           The number of lines.
 @param[in] ArrayOfStrings
**/
VOID
CreatePasswordPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       PasswordPopUpType,
  IN  UINTN                       MaxInputChar,
  IN  UINTN                       NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  )
{
  CreateSharedPasswordPopUp (RequestedWidth, PasswordPopUpType, MaxInputChar, NumberOfLines, &ArrayOfStrings);
}

/**
 Pop a password dialog interface based on input password status.

 @param[in] MenuOption          The password MenuOption.
 @param[in] Status              The password status.

 @retval EFI_SUCCESS            Displayed dialog successfully.
**/
EFI_STATUS
CreatePasswordDialog (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  EFI_STATUS                  Status
  )
{
  CHAR16                      NullCharacter;
  UINTN                       Maximum;
  UINTN                       ScreenSize;
  EFI_KEY_DATA                EfiKeyData;

  NullCharacter = CHAR_NULL;
  Maximum = (UINTN) MenuOption->ThisTag->Maximum;
  ScreenSize = MAX (GetStringWidth (gPromptForPassword), GetStringWidth (gPromptForNewPassword));
  ScreenSize = (MAX (ScreenSize, GetStringWidth (gConfirmPassword)) / 2) + Maximum + 2;
  switch (Status) {

  case EFI_SUCCESS:
    ScreenSize = MAX (ScreenSize, GetStringWidth (gPasswordChangesSaved) / 2);
    CreatePasswordPopUp (
      ScreenSize,
      PASSWORD_NOTICE_TYPE,
      Maximum,
      5,
      gSetupNotice,
      gPasswordChangesSaved,
      gContinue,
      &NullCharacter
    );
    break;

  case EFI_UNSUPPORTED:
    ScreenSize = (GetStringWidth (gSamePasswordError) / 2) + 12;
    CreatePasswordPopUp (
      ScreenSize,
      PASSWORD_NOTICE_TYPE,
      Maximum,
      5,
      gSetupWarning,
      gSamePasswordError,
      gPressEnter,
      &NullCharacter
      );
    break;

  case EFI_NOT_READY:
    CreatePasswordPopUp (
      ScreenSize,
      PASSWORD_NOTICE_TYPE,
      Maximum,
      5,
      gSetupWarning,
      gPassowordInvalid,
      gPressEnter,
      &NullCharacter
      );
    break;

  default:
    CreatePasswordPopUp (
      ScreenSize,
      PASSWORD_NOTICE_TYPE,
      Maximum,
      5,
      gSetupWarning,
      gConfirmError,
      gPressEnter,
      &NullCharacter
      );
    break;
  }

  do {
    UiInputDevice(0, &EfiKeyData, NULL, NULL);
  } while(EfiKeyData.Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

  return EFI_SUCCESS;
}

/**
 Update status bar on the bottom of menu.

 @param[in] MessageType            The type of message to be shown.
 @param[in] Flags                  The flags in Question header.
 @param[in] State                  Set or clear.
**/
VOID
UpdateStatusBar (
  IN  UINTN                       MessageType,
  IN  UINT8                       Flags,
  IN  BOOLEAN                     State
  )
{
  UINTN           Index;
  STATIC BOOLEAN  InputError;
  CHAR16          *NvUpdateMessage;
  CHAR16          *InputErrorMessage;

  if ((gDeviceManagerSetup != NULL) ||
      (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) ||
      gSecureBootMgr ||
       mBootManager ) {
    return;
  }
  NvUpdateMessage   = GetToken (STRING_TOKEN (NV_UPDATE_MESSAGE), gHiiHandle);
  InputErrorMessage = GetToken (STRING_TOKEN (INPUT_ERROR_MESSAGE), gHiiHandle);
  if (NvUpdateMessage == NULL || InputErrorMessage == NULL) {
    return;
  }

  switch (MessageType) {

  case INPUT_ERROR:
    if (State) {
      gST->ConOut->SetAttribute (gST->ConOut, ERROR_TEXT);
      PrintStringAt (
        gScreenDimensions.LeftColumn + gPromptBlockWidth,
        gScreenDimensions.BottomRow - 1,
        InputErrorMessage
        );
      InputError = TRUE;
    } else {
      gST->ConOut->SetAttribute (gST->ConOut, FIELD_TEXT_HIGHLIGHT);
      for (Index = 0; Index < (GetStringWidth (InputErrorMessage) - 2) / 2; Index++) {
        PrintAt (gScreenDimensions.LeftColumn + gPromptBlockWidth + Index, gScreenDimensions.BottomRow - 1, L"  ");
      }

      InputError = FALSE;
    }
    break;

  case NV_UPDATE_REQUIRED:
    if (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE) {
      if (State) {
        gST->ConOut->SetAttribute (gST->ConOut, INFO_TEXT);
        PrintStringAt (
          gScreenDimensions.LeftColumn + gPromptBlockWidth + gOptionBlockWidth,
          gScreenDimensions.BottomRow - 1,
          NvUpdateMessage
          );
        gResetRequired    = (BOOLEAN) (gResetRequired | ((Flags & EFI_IFR_FLAG_RESET_REQUIRED) == EFI_IFR_FLAG_RESET_REQUIRED));

        gNvUpdateRequired = TRUE;
      } else {
        gST->ConOut->SetAttribute (gST->ConOut, FIELD_TEXT_HIGHLIGHT);
        for (Index = 0; Index < (GetStringWidth (NvUpdateMessage) - 2) / 2; Index++) {
          PrintAt (
            (gScreenDimensions.LeftColumn + gPromptBlockWidth + gOptionBlockWidth + Index),
            gScreenDimensions.BottomRow - 1,
            L"  "
            );
        }

        gNvUpdateRequired = FALSE;
      }
    }
    break;

  case REFRESH_STATUS_BAR:
    if (InputError) {
      UpdateStatusBar (INPUT_ERROR, Flags, TRUE);
    }

    if (gNvUpdateRequired) {
      UpdateStatusBar (NV_UPDATE_REQUIRED, Flags, TRUE);
    }
    break;

  default:
    break;
  }

  gBS->FreePool (InputErrorMessage);
  gBS->FreePool (NvUpdateMessage);

  return ;
}

/**
 Get the supported width for a particular op-code

 @param[in] Statement              The FORM_BROWSER_STATEMENT structure passed in.
 @param[in] Handle                 The handle in the HII database being used

 @return Returns the number of CHAR16 characters that is support.
**/
UINT16
GetWidth (
  IN FORM_BROWSER_STATEMENT        *Statement,
  IN EFI_HII_HANDLE                 Handle
  )
{
  CHAR16  *String;
  UINTN   Size;
  UINT16  Width;

  Size = 0;

  //
  // See if the second text parameter is really NULL
  //
  if (((Statement->Operand == EFI_IFR_TEXT_OP) || (Statement->Operand == EFI_IFR_ACTION_OP)) && (Statement->TextTwo != 0)) {
    String = GetToken (Statement->TextTwo, Handle);
    if (String == NULL) {
      return 0;
    }
    Size   = StrLen (String);
    gBS->FreePool (String);
  }

  if ((Statement->Operand == EFI_IFR_SUBTITLE_OP) ||
      (Statement->Operand == EFI_IFR_PASSWORD_OP) ||
      (Statement->Operand == EFI_IFR_ACTION_OP) ||
      (Statement->Operand == EFI_IFR_RESET_BUTTON_OP) ||
      //
      // Allow a wide display if text op-code and no secondary text op-code
      //
      ((Statement->Operand == EFI_IFR_TEXT_OP) && (Size == 0))
      ) {
    Width = gClassOfVfr == EFI_GENERAL_APPLICATION_SUBCLASS ?
           (UINT16) (gPromptBlockWidth + gOptionBlockWidth + gHelpBlockWidth + 6) :
           (UINT16) (gPromptBlockWidth + gOptionBlockWidth);
  } else if (Statement->Operand == EFI_IFR_REF_OP) {
    Width = gClassOfVfr == EFI_GENERAL_APPLICATION_SUBCLASS ?
           (UINT16) (gPromptBlockWidth + gOptionBlockWidth + gHelpBlockWidth + 5) :
           (UINT16) (gPromptBlockWidth + gOptionBlockWidth - 1);
  } else {
    Width = (UINT16) gPromptBlockWidth;
  }

  if (Statement->InSubtitle) {
    Width -= SUBTITLE_INDENT;
  }

  return Width;
}

/**
 Will copy multiple number of lines and LineWidth amount of a string in the OutputString buffer

 @param[in]  InputString            String description for this option.
 @param[in]  LineWidth              Width of the desired string to extract in CHAR16 characters
 @param[out] LineNum                The number of lines in the output string
 @param[out] OutputString           Buffer to copy the string into

 @retval EFI_SUCCESS                According line width to separate the string to OutputString
 @return EFI_INVALId_PARAMETER      One of the parameters was invalid
**/
EFI_STATUS
GetLineByWidth (
  IN      CHAR16                      *InputString,
  IN      UINT16                      LineWidth,
  OUT     UINT16                      *LineNum,
  OUT     CHAR16                      **OutputString
  )
{
  BOOLEAN         IsWidth;
  BOOLEAN         Finished;
  BOOLEAN         AdjustStr;
  UINT16          AdjCount;
  UINT16          CharCount;
  UINT16          TempCharCount;
  UINTN           MaxLineNum;
  UINTN           BufferSize;
  CHAR16          *StringBuffer;
  UINT16          InputStrOffset;
  UINT16          OutputStrOffset;
  UINT16          StrWidth;
  UINT16          IncrementValue;
  UINT32          GlyphWidth;

  if (InputString == NULL || LineNum == NULL || OutputString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (LineWidth <= 1) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Try to calculate the maximun using memory size
  //
  MaxLineNum = StrSize (InputString) / (LineWidth - 1);
  BufferSize = MaxLineNum * 2 + StrSize (InputString);
  *OutputString = AllocateZeroPool (BufferSize);
  if (*OutputString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  IsWidth         = FALSE;
  Finished        = FALSE;
  InputStrOffset  = 0;
  OutputStrOffset = 0;
  *LineNum        = 0;
  IncrementValue  = 1;

  while (!Finished) {
    if (IsWidth) {
      (*OutputString)[OutputStrOffset] = WIDE_CHAR;
      OutputStrOffset++;
    }
    StrWidth = 0;
    CharCount = 0;
    AdjustStr = FALSE;
    do {
      //
      // Advance to the null-terminator or to the first width directive
      //
      for (;
           (InputString[InputStrOffset + CharCount] != CHAR_CARRIAGE_RETURN)&&
           (InputString[InputStrOffset + CharCount] != NARROW_CHAR) &&
           (InputString[InputStrOffset + CharCount] != WIDE_CHAR) && (InputString[InputStrOffset + CharCount] != 0) &&
           (StrWidth < LineWidth);
           CharCount++, StrWidth = StrWidth + IncrementValue) {
        if (IncrementValue == 1) {
          //
          // If character belongs to wide glyph, set the right glyph offset.
          //
          GlyphWidth = ConsoleLibGetGlyphWidth (InputString[InputStrOffset + CharCount]);
          if (GlyphWidth == 2) {
            StrWidth++;
          }
        }
      }

      if (StrWidth > LineWidth) {
        CharCount--;
        StrWidth = StrWidth - IncrementValue;
        AdjustStr = TRUE;
        break;
      } else if (StrWidth == LineWidth) {
        break;
      }
      //
      // We encountered a narrow directive - strip it from the size calculation since it doesn't get printed
      // and also set the flag that determines what we increment by.(if narrow, increment by 1, if wide increment by 2)
      //
      if (InputString[InputStrOffset + CharCount] == NARROW_CHAR) {
        //
        // Skip to the next character
        //
        CharCount++;
        IncrementValue = 1;
        IsWidth = FALSE;
      } else if (InputString[InputStrOffset + CharCount] == WIDE_CHAR) {
        //
        // Skip to the next character
        //
        CharCount++;
        IncrementValue = 2;
        IsWidth = TRUE;
      }
    } while (InputString[InputStrOffset + CharCount] != 0 && InputString[InputStrOffset + CharCount] != CHAR_CARRIAGE_RETURN);

    //
    // If space,narrow and wide character before end of the string, increase index to skip these characters
    //
    AdjCount      = 0;
    while (InputString[InputStrOffset + CharCount + AdjCount] != 0) {
      if ((InputString[InputStrOffset + CharCount + AdjCount] != WIDE_CHAR) &&
          (InputString[InputStrOffset + CharCount + AdjCount] != NARROW_CHAR) &&
          (InputString[InputStrOffset + CharCount + AdjCount] != CHAR_SPACE)
          ) {
        break;
      }
      AdjCount++;
    }

    if (InputString[InputStrOffset + CharCount + AdjCount] == 0) {
      Finished = TRUE;
    } else {
      if (StrWidth == LineWidth || AdjustStr) {
        //
        // Rewind the string from the maximum size until we see a space to break the line
        //
        if (!IsWidth) {
          TempCharCount = CharCount;

          for (; (InputString[InputStrOffset + CharCount] != CHAR_SPACE) && (CharCount != 0); CharCount--, StrWidth = StrWidth - IncrementValue)
          ;
          if (CharCount == 0) {
            CharCount = TempCharCount;
          }
        }
      }
    }
    if (BufferSize < (UINTN) (OutputStrOffset + (CharCount + 2) * 2)) {
       BufferSize *= 2;
       StringBuffer = AllocateZeroPool (BufferSize);
       if (StringBuffer == NULL) {
         return EFI_OUT_OF_RESOURCES;
       }
       CopyMem (StringBuffer, *OutputString, OutputStrOffset * 2);
       gBS->FreePool (*OutputString);
       *OutputString = StringBuffer;
    }
    CopyMem (&(*OutputString)[OutputStrOffset], &InputString[InputStrOffset], CharCount * 2);
    //
    // Increase offset to add null for end of one line
    //
    OutputStrOffset += (CharCount + 1);
    //
    // if not last string, we need increment the index to the first non-space character
    //
    if (!Finished) {
      while (InputString[InputStrOffset + CharCount] != 0) {
        if ((InputString[InputStrOffset + CharCount] != WIDE_CHAR) &&
            (InputString[InputStrOffset + CharCount] != NARROW_CHAR) &&
            (InputString[InputStrOffset + CharCount] != CHAR_SPACE) &&
            (InputString[InputStrOffset + CharCount] != CHAR_CARRIAGE_RETURN)) {
          break;
        }
        InputStrOffset++;
      }
    }
    InputStrOffset = InputStrOffset + CharCount;
    (*LineNum)++;
  }

  return EFI_SUCCESS;
}

/**
 Check whether this Menu Option could be highlighted.

 This is an internal function.

 @param[in] MenuOption          The MenuOption to be checked.

 @retval TRUE                   This Menu Option is selectable.
 @retval FALSE                  This Menu Option could not be selected.
**/
STATIC
BOOLEAN
IsSelectable (
  IN UI_MENU_OPTION   *MenuOption
  )
{
  if ((MenuOption->ThisTag->Operand == EFI_IFR_SUBTITLE_OP) ||
      MenuOption->GrayOut ||
      (gDeviceManagerSetup != NULL &&
        MenuOption->ThisTag->Operand == EFI_IFR_TEXT_OP &&
       !(MenuOption->ThisTag->QuestionFlags & EFI_IFR_FLAG_CALLBACK))) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
 Check have any selectale imte in the menu.

 This is an internal function.

 @retval TRUE                   At least have one selectable item.
 @retval FALSE                  Doesn't have any selectable item.
**/
STATIC
BOOLEAN
HaveAnySelectableItem (
  VOID
  )
{
  LIST_ENTRY            *Link;
  UI_MENU_OPTION        *NextMenuOption;

  if (IsListEmpty (&Menu)) {
    return FALSE;
  }

  for (Link = Menu.ForwardLink; Link != &Menu; Link = Link->ForwardLink) {
    NextMenuOption = MENU_OPTION_FROM_LINK (Link);
    if (IsSelectable (NextMenuOption)) {
      break;
    }
  }

  return (BOOLEAN) (Link != &Menu);
}

/**
 Determine if the menu is the last menu that can be selected.

 This is an internal function.

 @param[in] Direction      The scroll direction. False is down. True is up.
 @param[in] CurrentPos     The current focus.

 @return FALSE             the menu isn't the last menu that can be selected.
 @return TRUE              the menu is the last menu that can be selected.
**/
STATIC
BOOLEAN
ValueIsScroll (
  IN  BOOLEAN                     Direction,
  IN  LIST_ENTRY                  *CurrentPos
  )
{
  LIST_ENTRY      *Temp;

  Temp = Direction ? CurrentPos->BackLink : CurrentPos->ForwardLink;

  if (Temp == &Menu) {
    return TRUE;
  }

  return FALSE;
}

/**
 Move to next selectable statement.

 This is an internal function.

 @param[in]      GoUp                   The navigation direction. TRUE: up, FALSE: down.
 @param[in, out] CurrentPosition        Current position.

 @return The row distance from current MenuOption to next selectable MenuOption.
**/
STATIC
INTN
MoveToNextStatement (
  IN     BOOLEAN                   GoUp,
  IN OUT LIST_ENTRY                **CurrentPosition
  )
{
  INTN             Distance;
  LIST_ENTRY       *Pos;
  BOOLEAN          HitEnd;
  UI_MENU_OPTION   *NextMenuOption;
  UINT8            SavedOperand;

  Distance = 0;
  Pos      = *CurrentPosition;
  HitEnd   = FALSE;

  while (TRUE) {
    NextMenuOption = MENU_OPTION_FROM_LINK (Pos);
    if (IsSelectable (NextMenuOption)) {
      break;
    }
    if ((GoUp ? Pos->BackLink : Pos->ForwardLink) == &Menu) {
      HitEnd = TRUE;
      break;
    }
    Distance += NextMenuOption->Skip;
    Pos = (GoUp ? Pos->BackLink : Pos->ForwardLink);
  }

  if (HitEnd) {
    //
    // If we hit end there is still no statement can be focused,
    // we go backwards to find the statement can be focused.
    //
    Distance = 0;
    Pos = *CurrentPosition;

    while (TRUE) {
      NextMenuOption = MENU_OPTION_FROM_LINK (Pos);
      if (IsSelectable (NextMenuOption)) {
        break;
      }
      if ((!GoUp ? Pos->BackLink : Pos->ForwardLink) == &Menu) {
        break;
      }
      Distance -= NextMenuOption->Skip;
      Pos = (!GoUp ? Pos->BackLink : Pos->ForwardLink);
    }
    //
    // Adjust date and time: if hit the end, make sure the we select
    // hour in the time operand or select month in the date operand
    //
    if ((NextMenuOption->ThisTag->Operand == EFI_IFR_DATE_OP) ||
        (NextMenuOption->ThisTag->Operand == EFI_IFR_TIME_OP)
       ) {
       SavedOperand = NextMenuOption->ThisTag->Operand;
       while (SavedOperand == NextMenuOption->ThisTag->Operand) {
         Pos= Pos->BackLink;
         NextMenuOption = MENU_OPTION_FROM_LINK (Pos);
       }
       Pos= Pos->ForwardLink;
       NextMenuOption = MENU_OPTION_FROM_LINK (Pos);
    }
  }

  *CurrentPosition = &NextMenuOption->Link;
  return Distance;
}

/**
 Adjust Data and Time position accordingly.
 Data format :      [01/02/2004]      [11:22:33]
 Line number :        0  0    1         0  0  1

 This is an internal function.

 @param[in]      DirectionUp            the up or down direction. False is down. True is up.
 @param[in, out] CurrentPosition        Current position. On return: Point to the last
                                        Option (Year or Second) if up; Point to the first
                                        Option (Month or Hour) if down.

 @return Return line number to pad. It is possible that we stand on a zero-advance
 @return data or time opcode, so pad one line when we judge if we are going to scroll outside.
**/
STATIC
UINTN
AdjustDateAndTimePosition (
  IN     BOOLEAN                     DirectionUp,
  IN OUT LIST_ENTRY                  **CurrentPosition
  )
{
  UINTN           Count;
  LIST_ENTRY      *NewPosition;
  UI_MENU_OPTION  *MenuOption;
  UINTN           PadLineNumber;

  PadLineNumber = 0;
  NewPosition   = *CurrentPosition;
  MenuOption    = MENU_OPTION_FROM_LINK (NewPosition);

  if ((MenuOption->ThisTag->Operand == EFI_IFR_DATE_OP) ||
      (MenuOption->ThisTag->Operand == EFI_IFR_TIME_OP)) {
    //
    // Calculate the distance from current position to the last Date/Time MenuOption
    //
    Count = 0;
    while (MenuOption->Skip == 0) {
      Count++;
      NewPosition   = NewPosition->ForwardLink;
      MenuOption    = MENU_OPTION_FROM_LINK (NewPosition);
      PadLineNumber = 1;
    }

    NewPosition = *CurrentPosition;
    if (DirectionUp) {
      //
      // Since the behavior of hitting the up arrow on a Date/Time MenuOption is intended
      // to be one that back to the previous set of MenuOptions, we need to advance to the first
      // Date/Time MenuOption and leave the remaining logic in CfUiUp intact so the appropriate
      // checking can be done.
      //
      while (Count++ < 2) {
        NewPosition = NewPosition->BackLink;
      }
    } else {
      //
      // Since the behavior of hitting the down arrow on a Date/Time MenuOption is intended
      // to be one that progresses to the next set of MenuOptions, we need to advance to the last
      // Date/Time MenuOption and leave the remaining logic in CfUiDown intact so the appropriate
      // checking can be done.
      //
      while (Count-- > 0) {
        NewPosition = NewPosition->ForwardLink;
      }
    }

    *CurrentPosition = NewPosition;
  }

  return PadLineNumber;
}

/**
  Find HII Handle in the HII database associated with given Device Path.

  If DevicePath is NULL, then ASSERT.

  @param[in] DevicePath          Device Path associated with the HII package list handle.

  @retval Handle                 HII package list Handle associated with the Device Path.
  @retval NULL                   Hii Package list handle is not found.
**/
EFI_HII_HANDLE
EFIAPI
DevicePathToHiiHandle (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *TmpDevicePath;
  UINTN                       BufferSize;
  UINTN                       HandleCount;
  UINTN                       Index;
  EFI_HANDLE                  Handle;
  EFI_HANDLE                  DriverHandle;
  EFI_HII_HANDLE              *HiiHandles;
  EFI_HII_HANDLE              HiiHandle;

  ASSERT (DevicePath != NULL);

  TmpDevicePath = DevicePath;
  //
  // Locate Device Path Protocol handle buffer
  //
  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &TmpDevicePath,
                  &DriverHandle
                  );
  if (EFI_ERROR (Status) || !IsDevicePathEnd (TmpDevicePath)) {
    return NULL;
  }

  //
  // Retrieve all HII Handles from HII database
  //
  BufferSize = 0x1000;
  HiiHandles = AllocatePool (BufferSize);
  ASSERT (HiiHandles != NULL);
  if (HiiHandles == NULL) {
    return NULL;
  }
  Status = mHiiDatabase->ListPackageLists (
                           mHiiDatabase,
                           EFI_HII_PACKAGE_TYPE_ALL,
                           NULL,
                           &BufferSize,
                           HiiHandles
                           );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (HiiHandles);
    HiiHandles = AllocatePool (BufferSize);
    ASSERT (HiiHandles != NULL);
    if (HiiHandles == NULL) {
      return NULL;
    }

    Status = mHiiDatabase->ListPackageLists (
                             mHiiDatabase,
                             EFI_HII_PACKAGE_TYPE_ALL,
                             NULL,
                             &BufferSize,
                             HiiHandles
                             );
  }

  if (EFI_ERROR (Status)) {
    FreePool (HiiHandles);
    return NULL;
  }

  //
  // Search Hii Handle by Driver Handle
  //
  HiiHandle = NULL;
  HandleCount = BufferSize / sizeof (EFI_HII_HANDLE);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = mHiiDatabase->GetPackageListHandle (
                             mHiiDatabase,
                             HiiHandles[Index],
                             &Handle
                             );
    if (!EFI_ERROR (Status) && (Handle == DriverHandle)) {
      HiiHandle = HiiHandles[Index];
      break;
    }
  }

  FreePool (HiiHandles);
  return HiiHandle;
}

/**
 Display menu option string in specific position.

 @param[in] MenuOption             The displayed menu option
 @param[in] Text                   Display string
 @param[in] StartCol               Start display column
 @param[in] StartRow               Start display row
 @param[in] LineWidth              One line width
 @param[in] SkipRows               Skip value
 @param[in] TopRow                 Top row value
 @param[in] BottomRow              Bottom row value
**/
VOID
DisplayMenuOptionString (
  IN UI_MENU_OPTION *MenuOption,
  IN CHAR16         *Text,
  IN UINTN          StartCol,
  IN UINTN          StartRow,
  IN UINT16         LineWidth,
  IN UINTN          SkipRows,
  IN UINTN          TopRow,
  IN UINTN          BottomRow
  )
{
  CHAR16                          *OutputString;
  CHAR16                          *TempOutputString;
  UINT16                          Index;
  UINT16                          LineNum;
  UINTN                           Row;
  EFI_STATUS                      Status;

  OutputString = NULL;
  Row          = StartRow;

  Status = GetLineByWidth (Text, LineWidth, &LineNum, &OutputString);
  if (EFI_ERROR (Status)) {
    return;
  }
  if (OutputString == NULL) {
    return ;
  }

  TempOutputString = OutputString;
  for (Index = 0; Index < LineNum; Index++) {
    if ((SkipRows == 0) && (Row <= BottomRow) && (Row >= TopRow)) {
      if (gDeviceManagerSetup && MenuOption->ThisTag->Operand == EFI_IFR_REF_OP) {
        if(Row == StartRow) {
          PrintCharAt(StartCol, Row, GEOMETRICSHAPE_RIGHT_TRIANGLE);
        }
        PrintStringAt(StartCol + 1, Row, OutputString);
      } else {
        PrintStringAt (StartCol, Row, OutputString);
      }
      Row++;
    } else {
      SkipRows--;
    }
    OutputString += (StrSize (OutputString) / 2);
  }

  ScuSafeFreePool((VOID **)&TempOutputString);
}

/**
  Display prompt string of menu option.

  @param[in] MenuOption             The displayed menu option
  @param[in] SkipRows               Skip value
  @param[in] TopRow                 Top row value
  @param[in] BottomRow              Bottom row value
**/
VOID
DisplayMenuOptionPrompt (
  IN UI_MENU_OPTION *MenuOption,
  IN UINTN          SkipRows,
  IN UINTN          TopRow,
  IN UINTN          BottomRow
  )
{
  UINT16                          Width;

  Width       = GetWidth (MenuOption->ThisTag, MenuOption->Handle) - 1;
  DisplayMenuOptionString (
    MenuOption,
    MenuOption->Description,
    MenuOption->Col,
    MenuOption->Row,
    Width,
    SkipRows,
    TopRow,
    BottomRow
    );
}

/**
 Display option string of menu option.

 @param[in] MenuOption             The displayed menu option
 @param[in] OptionString           Display option string
 @param[in] SkipRows               Skip value
 @param[in] TopRow                 Top row value
 @param[in] BottomRow              Bottom row value
**/
VOID
DisplayMenuOptionValue(
  IN UI_MENU_OPTION *MenuOption,
  IN CHAR16         *OptionString,
  IN UINTN          SkipRows,
  IN UINTN          TopRow,
  IN UINTN          BottomRow
  )
{
  DisplayMenuOptionString (
    MenuOption,
    OptionString,
    MenuOption->OptCol,
    MenuOption->Row,
    (UINT16)gOptionBlockWidth,
    SkipRows,
    TopRow,
    BottomRow
    );

}

/**
 Initialize GOP list.
**/
VOID
InitGopList(
  VOID
  )
{
  EFI_STATUS                   Status;
  UINTN                        NoHandles;
  EFI_HANDLE                   *Handles;
  UINTN                        Index;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;

  //
  // doesn't use ConSplitter
  //
  Gop = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &NoHandles,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  mGopCount = 0;
  mGopList  = AllocatePool (sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL *) * NoHandles);
  if (mGopList == NULL) {
    return ;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **)&Gop
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }
    mGopList[mGopCount++] = Gop;
  }

  gBS->FreePool (Handles);
}

/**
 Blt Operations for setup browser.

 @param[in] This         Protocol instance pointer.
 @param[in] BltBuffer    Buffer containing data to blit into video buffer. This
                         buffer has a size of Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
 @param[in] BltOperation Operation to perform on BlitBuffer and video memory
 @param[in] SourceX      X coordinate of source for the BltBuffer.
 @param[in] SourceY      Y coordinate of source for the BltBuffer.
 @param[in] DestinationX X coordinate of destination for the BltBuffer.
 @param[in] DestinationY Y coordinate of destination for the BltBuffer.
 @param[in] Width        Width of rectangle in BltBuffer in pixels.
 @param[in] Height       Hight of rectangle in BltBuffer in pixels.
 @param[in] Delta        OPTIONAL

 @retval EFI_SUCCESS           The Blt operation completed.
 @retval EFI_INVALID_PARAMETER BltOperation is not valid.
 @retval EFI_DEVICE_ERROR      A hardware error occured writting to the video buffer.
**/
EFI_STATUS
EFIAPI
SetupBrowserBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta         OPTIONAL
  )
{
  EFI_STATUS Status;
  UINT32     GopIndex;

  Status = EFI_UNSUPPORTED;

  ASSERT (mGopCount != 0);

  switch (BltOperation) {

  case EfiBltVideoFill:
  case EfiBltBufferToVideo:
  case EfiBltVideoToVideo:
    for (GopIndex = 0; GopIndex < mGopCount; GopIndex++) {
      Status = mGopList[GopIndex]->Blt (
                 mGopList[GopIndex],
                 BltBuffer,
                 BltOperation,
                 SourceX,
                 SourceY,
                 DestinationX,
                 DestinationY,
                 Width,
                 Height,
                 Delta
                 );
    }
    break;


  case EfiBltVideoToBltBuffer:
   Status = mGopList[0]->Blt (
              mGopList[0],
              BltBuffer,
              BltOperation,
              SourceX,
              SourceY,
              DestinationX,
              DestinationY,
              Width,
              Height,
              Delta
              );
    break;

  default:
    return EFI_UNSUPPORTED;

  }
  return Status;
}

UINT8 *
GetAnimation (
  IN UINT8             *AnimationBlock,
  IN EFI_ANIMATION_ID  AnimationId
  )
{
  EFI_ANIMATION_ID              CurrentAnimationId;


  CurrentAnimationId = 1;
  while (*AnimationBlock != EFI_HII_AIBT_END) {
    if (CurrentAnimationId == AnimationId) {
        return AnimationBlock;
    }

    if (*AnimationBlock == EFI_HII_AIBT_EXT4) {
      AnimationBlock += ((EFI_HII_AIBT_EXT4_BLOCK *)AnimationBlock)->Length;
      CurrentAnimationId++;
    } else {
      //
      // not yet support type, so return not fonud
      //
      return NULL;
    }
  }

  return NULL;
}

LIST_ENTRY *
AnimationMenu (
  OUT  EFI_INPUT_KEY                     *Key,
  IN   UINTN                             X,
  IN   UINTN                             Y
  )
{
  ANIMATION_REFRESH_ENTRY      *Entry;
  for (
    Entry = (ANIMATION_REFRESH_ENTRY *)GetFirstNode (&gAnimationRefreshList);
    !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
    Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {
    if (((X >= Entry->X) && (X <= Entry->X + Entry->Animation->Width)) &&
        ((Y >= Entry->Y) && (Y <= Entry->Y + Entry->Animation->Height))) {
      return &(((UI_MENU_OPTION *)Entry->Data)->Link);
    }
  }
  return NULL;
}

EFI_STATUS
UiPlayAnimation (
  IN UI_MENU_OPTION *MenuOption
  )
{

  ANIMATION_REFRESH_ENTRY *Entry;

  Entry = NULL;

  for (
    Entry = (ANIMATION_REFRESH_ENTRY *)GetFirstNode (&gAnimationRefreshList);
    !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
    Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {
    if (Entry->Data == MenuOption) {
      break;
    }
  }

  if (Entry != NULL) {
    Entry->Status = ANIM_STATUS_PLAY;
  }

  ASSERT (Entry != NULL);

  return EFI_SUCCESS;
}

EFI_STATUS
UiStopAllAnimation (
  )
{

  ANIMATION_REFRESH_ENTRY *Entry;

  Entry = NULL;
  for (
    Entry = (ANIMATION_REFRESH_ENTRY *) GetFirstNode (&gAnimationRefreshList);
    !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
    Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {

    //
    // draw first image
    //
    if (Entry->Current != Entry->Animation->Frames) {
      Entry->Status  = ANIM_STATUS_PLAY;
      Entry->Current = NULL;
      mGifDecoder->NextAnimationFrame (mGifDecoder, Entry, mGop);
    }
    Entry->Status = ANIM_STATUS_STOP;

  }

  return EFI_SUCCESS;
}

EFI_STATUS
UiAddAnimationList (
  IN EFI_ANIMATION_ID           AnimationId,
  IN UI_MENU_OPTION             *MenuOption,
  IN UINTN                      X,
  IN UINTN                      Y,
  IN UINTN                      BltWidth,
  IN UINTN                      BltHeight,
  IN UINT32                     AnimationStatus
  )
{
  EFI_STATUS                    Status;
  LIST_ENTRY                    *Link;
  ANIMATION                     *Animation;
  ANIMATION_REFRESH_ENTRY       *AnimationRefreshEntry;
  EFI_HII_AIBT_EXT4_BLOCK       *Ext4Block;

  Animation            = NULL;
  AnimationRefreshEntry = NULL;

  if (mGop == NULL) {
    return EFI_NOT_FOUND;
  }

  if (gAnimationPackage == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // search animaton list by animation id
  // if animation not found, then create animation from memory
  // add to list
  //
  Link = GetFirstNode (&gAnimationList);
  while (!IsNull (&gAnimationList, Link)) {
    if ((UINT16)(UINTN)(((ANIMATION *)Link)->Data) == AnimationId) {
      Animation = (ANIMATION *)Link;
      break;
    }
    Link = GetNextNode (&gAnimationList, Link);
  }

  //
  // not found exist animation, create aniamtion from package
  //
  if (Animation == NULL) {
    Ext4Block = (EFI_HII_AIBT_EXT4_BLOCK *) GetAnimation (
                  (UINT8 *)gAnimationPackage + gAnimationPackage->AnimationInfoOffset,
                  AnimationId
                  );

    if (Ext4Block == NULL) {
      return EFI_NOT_FOUND;
    }

    if (*(UINT8 *)Ext4Block != EFI_HII_AIBT_EXT4) {
      return EFI_NOT_FOUND;
    }

    Status = mGifDecoder->CreateAnimationFromMem (
                            mGifDecoder,
                            (UINT8 *)(Ext4Block + 1),
                            Ext4Block->Length,
                            (VOID *)(UINTN)AnimationId,
                            &Animation
                            );

    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    InsertTailList (&gAnimationList, (LIST_ENTRY *)Animation);
  }
  if (Animation == NULL) {
    return EFI_NOT_FOUND;
  }

  Animation->BkColor.Blue  = 0;
  Animation->BkColor.Green = 0;
  Animation->BkColor.Red    = 1;
  Animation->BkColor.Reserved = 1;

  AnimationRefreshEntry = AllocateZeroPool (sizeof (ANIMATION_REFRESH_ENTRY));
  if (AnimationRefreshEntry == NULL) {
    goto Error;
  }

  AnimationRefreshEntry->Animation = Animation;
  AnimationRefreshEntry->Data      = MenuOption;
  AnimationRefreshEntry->X         = X;
  AnimationRefreshEntry->Y         = Y;
  AnimationRefreshEntry->BltWidth   = BltWidth;
  AnimationRefreshEntry->BltHeight  = BltHeight;
  AnimationRefreshEntry->Current    = NULL;
  AnimationRefreshEntry->RecordTick = 0;
  AnimationRefreshEntry->AutoLoop   = TRUE;
  AnimationRefreshEntry->Status     = AnimationStatus;

  InsertTailList (&gAnimationRefreshList, (LIST_ENTRY *) AnimationRefreshEntry);
  return EFI_SUCCESS;

Error:
  if (AnimationRefreshEntry != NULL) {
    gBS->FreePool (AnimationRefreshEntry);
  }

  if (Animation != NULL) {
    mGifDecoder->DestroyAnimation (mGifDecoder, Animation);
  }

  return EFI_NOT_FOUND;
}

VOID
UiFreeAnimationRefreshList (
  )
{
  ANIMATION_REFRESH_ENTRY  *AniamtionRefresh;
  ANIMATION                *Animation;

  if (mGifDecoder == NULL) return ;

  while (!IsListEmpty (&gAnimationList)) {
    Animation = (ANIMATION *) GetFirstNode (&gAnimationList);
    RemoveEntryList ((LIST_ENTRY *) Animation);
    mGifDecoder->DestroyAnimation (mGifDecoder, Animation);
  }

  while (!IsListEmpty (&gAnimationRefreshList)) {
    AniamtionRefresh = (ANIMATION_REFRESH_ENTRY *) GetFirstNode (&gAnimationRefreshList);
    RemoveEntryList ((LIST_ENTRY *) AniamtionRefresh);
    gBS->FreePool (AniamtionRefresh);
  }

}

VOID
UiRefreshAnimationList (
  )
{
  ANIMATION_REFRESH_ENTRY      *Entry;

  if (mGop == NULL) {
    return ;
  }

  if (IsListEmpty (&gAnimationRefreshList)) {
    return ;
  }

  for (
    Entry = (ANIMATION_REFRESH_ENTRY *)GetFirstNode (&gAnimationRefreshList);
    !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
    Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {
    mGifDecoder->RefreshAnimation (mGifDecoder, Entry, mGop, 0);
  }
}

EFI_STATUS
GetStartOffset (
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput,
  OUT     UINTN                          *OffsetX,
  OUT     UINTN                          *OffsetY
  )
{
  EFI_STATUS              Status;
  UINTN                   UsedHorizontalPixles;
  UINTN                   UsedVerticalPixles;
  UINTN                   Rows;
  UINTN                   Columns;
  UINTN                   DeltaY;

  if (GraphicsOutput == NULL || OffsetX == NULL || OffsetY == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &Columns,
                          &Rows
                          );
  ASSERT_EFI_ERROR (Status);
  UsedHorizontalPixles = Columns * EFI_GLYPH_WIDTH;
  UsedVerticalPixles = GraphicsOutput->Mode->Info->VerticalResolution;
  switch (Rows) {

  case 25:
    UsedVerticalPixles = 480;

    break;

  case 31:
    UsedVerticalPixles = 600;
    break;

  case 40:
    UsedVerticalPixles = 768;
    break;

  case 50:
    UsedVerticalPixles = 960;
    break;

  default:
    //
    // Try to use full screen text mode to calculate delta Y
    //
    UsedVerticalPixles = GraphicsOutput->Mode->Info->VerticalResolution;
    if (UsedVerticalPixles - (Rows * EFI_GLYPH_HEIGHT) >= EFI_GLYPH_HEIGHT) {
      //
      // Unsupported resolution and isn't a full screen text mode,
      // we can not calculate delta Y, so assert it.
      //
      ASSERT_EFI_ERROR (TRUE);
    }
    break;

  }
  DeltaY = (UsedVerticalPixles % EFI_GLYPH_HEIGHT) >> 1;
  ASSERT (GraphicsOutput->Mode->Info->HorizontalResolution >= UsedHorizontalPixles);
  ASSERT (GraphicsOutput->Mode->Info->VerticalResolution >= UsedVerticalPixles);
  *OffsetX = (GraphicsOutput->Mode->Info->HorizontalResolution - UsedHorizontalPixles) / 2;
  *OffsetY = (GraphicsOutput->Mode->Info->VerticalResolution - UsedVerticalPixles) / 2 + DeltaY;
  return EFI_SUCCESS;
}

/**
 Get number of column in animation layout

 @param[in] ItemCount    Animation item count

 @retval number of column in animation layout.
**/
UINTN
GetAnimationLayoutNumColumn (
  IN UINTN ItemCount
  )
{
  if (ItemCount >= 10 && mGop->Mode->Info->VerticalResolution <= 600) {
    return 4;
  }
  return ANI_PER_COL;
}

/**
 Get number of row in animation layout

 @param[in] ItemCount    Animation item count

 @retval number of row in animation layout.
**/
UINTN
GetAnimationLayoutNumRow (
  IN UINTN ItemCount
  )
{
  UINTN NumColumn;
  NumColumn = GetAnimationLayoutNumColumn (ItemCount);
  return (ItemCount + (NumColumn - 1)) / NumColumn;
}

/**
 Update every animation' position, we assume every animation has the same size
**/
VOID
UpdateAnimationLayout (
  VOID
  )
{
  UINTN X0;
  UINTN Y0;
  UINTN TotalWidth;
  UINTN TotalHeight;
  UINTN MaxWidth;
  UINTN MaxHeight;
  UINTN ItemCount;
  UINTN Index;
  UINTN NumColumn;
  UINTN NumRow;
  UINTN HorizontalSpacing;
  UINTN VerticalSpacing;
  ANIMATION_REFRESH_ENTRY      *Entry;

  if (IsListEmpty (&gAnimationRefreshList)) {
    return ;
  }

  //
  // calculate total width and total height
  //
  ItemCount = 0;
  MaxWidth  = 0;
  MaxHeight = 0;
  for (
    Entry = (ANIMATION_REFRESH_ENTRY *)GetFirstNode (&gAnimationRefreshList);
    !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
    Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {
    if (Entry->Animation->Width > MaxWidth) MaxWidth  = Entry->Animation->Width;
    if (Entry->Animation->Height > MaxHeight)  MaxHeight = Entry->Animation->Height;
    ItemCount++;
  }
  NumColumn = GetAnimationLayoutNumColumn (ItemCount);
  NumRow    = GetAnimationLayoutNumRow (ItemCount);

  //
  // get spacing
  //
  HorizontalSpacing = ANI_PAD_X;
  TotalWidth        = MaxWidth * NumColumn + (NumColumn - 1) * HorizontalSpacing;
  if (TotalWidth > mGop->Mode->Info->HorizontalResolution) {
    TotalWidth        = mGop->Mode->Info->HorizontalResolution;
    HorizontalSpacing = (TotalWidth - MaxWidth * NumColumn) / (NumColumn - 1);
  }

  VerticalSpacing = ANI_PAD_Y;
  TotalHeight     = MaxHeight * NumRow + (NumRow - 1) * VerticalSpacing;
  if (TotalHeight > mGop->Mode->Info->VerticalResolution) {
    TotalHeight     = mGop->Mode->Info->VerticalResolution;
    VerticalSpacing = (TotalHeight - MaxHeight * NumRow) / (NumRow - 1);
  }

  //
  // calculate x0 and y0, then set position for every animation
  //
  X0 = (mGop->Mode->Info->HorizontalResolution - TotalWidth)  / 2;
  Y0 = (mGop->Mode->Info->VerticalResolution   - TotalHeight) / 2;

  Index = 0;
  for (
    Entry = (ANIMATION_REFRESH_ENTRY *)GetFirstNode (&gAnimationRefreshList);
    !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
    Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {

    Entry->X = X0 + (Index % NumColumn) * (MaxWidth + HorizontalSpacing);
    Entry->Y = Y0 + (Index / NumColumn) * (MaxHeight + VerticalSpacing);
    Index++;
  }
}

/**
 Display menu and wait for user to select one menu option, then return it.
 If AutoBoot is enabled, then if user doesn't select any option,
 after period of time, it will automatically return the first menu option.

 @param[in, out] Selection      Menu selection.

 @retval EFI_SUCESSS            This function always return successfully for now.
**/
EFI_STATUS
UiDisplayMenu (
  IN OUT UI_MENU_SELECTION           *Selection
  )
{
  INTN                            SkipValue;
  INTN                            Difference;
  INTN                            OldSkipValue;
  UINTN                           DistanceValue;
  UINTN                           Row;
  UINTN                           Col;
  UINTN                           Temp;
  UINTN                           Temp2;
  UINTN                           TopRow;
  UINTN                           BottomRow;
  UINTN                           Index;
  UINT32                          Count;
  CHAR16                          *StringPtr;
  CHAR16                          *OptionString;
  CHAR16                          *OutputString;
  CHAR16                          *FormattedString;
  CHAR16                          YesResponse;
  CHAR16                          NoResponse;
  BOOLEAN                         NewLine;
  BOOLEAN                         Repaint;
  BOOLEAN                         SavedValue;
  EFI_STATUS                      Status;
  EFI_INPUT_KEY                   Key;
  LIST_ENTRY                      *Link;
  LIST_ENTRY                      *NewPos;
  LIST_ENTRY                      *TopOfScreen;
  LIST_ENTRY                      *SavedListEntry;
  UI_MENU_OPTION                  *MenuOption;
  UI_MENU_OPTION                  *NextMenuOption;
  UI_MENU_OPTION                  *SavedMenuOption;
  UI_MENU_OPTION                  *PreviousMenuOption;
  UI_CONTROL_FLAG                 ControlFlag;
  EFI_SCREEN_DESCRIPTOR           LocalScreen;
  MENU_REFRESH_ENTRY              *MenuRefreshEntry;
  UI_SCREEN_OPERATION             ScreenOperation;
  UINT8                           MinRefreshInterval;
  UINTN                           BufferSize;
  UINT16                          DefaultId;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  FORM_BROWSER_STATEMENT          *Statement;
  UI_MENU_LIST                    *CurrentMenu;
  UI_MENU_LIST                    *MenuList;
  UINTN                           NormalTextAttr;
  UINTN                           HighLightTextAttr;
  UINTN                           SubTitleTextAttr;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  UINTN                           OptionItemAction;
  BOOLEAN                         DoRepaintScreen;
  UI_CONTROL_FLAG                 SavedControlFlag;
  EFI_INPUT_KEY                   *MouseKey = NULL;
  SCAN_CODE_TO_SCREEN_OPERATION   *CodeToOperation;
  UINTN                           ScanCodeToOperationCount;
  UINTN                           FormattedStrLen;
  UINTN                           StartX;
  UINTN                           StartY;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    Gop;
  UINTN                           NumColumn;
  UINTN                           ItemCount;
  ANIMATION_REFRESH_ENTRY         *Entry;
  CHAR16                          *TitleString;

  if (gDeviceManagerSetup != NULL) {
    NormalTextAttr    = SU_FIELD_TEXT        | SU_BACKGROUND;
    HighLightTextAttr = SU_TEXT_HIGHLIGHT    | SU_BACKGROUND_HIGHLIGHT;
    SubTitleTextAttr  = SU_SUBTITLE_TEXT     | SU_BACKGROUND;
    gSetupMouse = gDeviceManagerSetup->SetupMouseFlag ?
                 (EFI_SETUP_MOUSE_PROTOCOL *)gDeviceManagerSetup->SetupMouse : NULL;
  } else {
    NormalTextAttr    = FIELD_TEXT           | FIELD_BACKGROUND;
    HighLightTextAttr = FIELD_TEXT_HIGHLIGHT | FIELD_BACKGROUND_HIGHLIGHT;
    SubTitleTextAttr  = SUBTITLE_TEXT        | SUBTITLE_BACKGROUND;
    gSetupMouse = NULL;
  }
  //
  // Always locate SetupMouse for dialog detect
  //
  gBS->LocateProtocol (&gSetupMouseProtocolGuid, NULL, (VOID **)&gSetupMouse);
  CopyMem (&LocalScreen, &gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  Status              = EFI_SUCCESS;
  FormattedString     = NULL;
  OptionString        = NULL;
  ScreenOperation     = UiNoOperation;
  NewLine             = TRUE;
  DefaultId           = 0;

  //
  // default refresh interval is UINT8 maximum
  //
  MinRefreshInterval  = 255;

  OutputString        = NULL;
  gUpArrow            = FALSE;
  gDownArrow          = FALSE;
  SkipValue           = 0;
  OldSkipValue        = 0;
  MenuRefreshEntry    = gMenuRefreshHead;

  NextMenuOption      = NULL;
  PreviousMenuOption  = NULL;
  SavedMenuOption     = NULL;
  ConfigAccess        = Selection->FormSet->ConfigAccess;
  OptionItemAction    = OPTION_ITEM_NO_ACTION;

  DoRepaintScreen     = FALSE;
  SavedControlFlag    = CfInitialization;
  ZeroMem (&Key, sizeof (EFI_INPUT_KEY));
  if (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) {
    TopRow  = LocalScreen.TopRow + FRONT_PAGE_HEADER_HEIGHT + SCROLL_ARROW_HEIGHT;
    Row     = LocalScreen.TopRow + FRONT_PAGE_HEADER_HEIGHT + SCROLL_ARROW_HEIGHT;
    BottomRow   = LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT - SCROLL_ARROW_HEIGHT - 1;
  } else  if (gDeviceManagerSetup != NULL) {
    TopRow  = LocalScreen.TopRow + 3;
    Row     = TopRow;
    //
    //re-draw UI for remove help msg on right under area on SCU
    //
    BottomRow = LocalScreen.BottomRow - 4;
  }  else if (mBootManager) {
    TopRow = LocalScreen.TopRow + 4;
    Row    = TopRow;
    BottomRow   = LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT - SCROLL_ARROW_HEIGHT - 1;
  } else {
    TopRow  = LocalScreen.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT + SCROLL_ARROW_HEIGHT;
    Row     = LocalScreen.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT + SCROLL_ARROW_HEIGHT;
    BottomRow = LocalScreen.BottomRow - STATUS_BAR_HEIGHT - FOOTER_HEIGHT - SCROLL_ARROW_HEIGHT - 1;
  }

  Col = LocalScreen.LeftColumn + 1;

  Selection->TopRow = TopRow;
  Selection->BottomRow = BottomRow;
  Selection->PromptCol = Col;
  Selection->OptionCol = gPromptBlockWidth + 1 + LocalScreen.LeftColumn;
  Selection->Statement = NULL;

  TopOfScreen = Menu.ForwardLink;
  Repaint     = TRUE;
  MenuOption  = NULL;

  //
  // Find current Menu
  //
  CurrentMenu = UiFindMenuList (&Selection->FormSetGuid, Selection->FormId);
  if (CurrentMenu == NULL) {
    //
    // Current menu not found, add it to the menu tree
    //
    CurrentMenu = UiAddMenuList (NULL, &Selection->FormSetGuid, Selection->FormId);
  }
  ASSERT (CurrentMenu != NULL);

  if (Selection->QuestionId == 0) {
    //
    // Highlight not specified, fetch it from cached menu
    //
    Selection->QuestionId = CurrentMenu->QuestionId;
  }

  //
  // Get user's selection
  //
  NewPos = Menu.ForwardLink;

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  UpdateStatusBar (REFRESH_STATUS_BAR, (UINT8) 0, TRUE);

  //
  // create a SetupBroser GOP protocol,
  // it only contain blt function and mode information.
  //
  InitGopList ();

  if (mGopCount != 0) {
    mGop = &Gop;
    mGop->Blt  = SetupBrowserBlt;
    mGop->Mode = mGopList[0]->Mode;
  } else {
    mGop = NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiGifDecoderProtocolGuid,
                  NULL,
                  (VOID **)&mGifDecoder
                  );
  if (EFI_ERROR (Status)) {
    mGifDecoder = NULL;
  }

  ControlFlag = CfInitialization;
  Selection->Action = UI_ACTION_NONE;
  while (TRUE) {
    switch (ControlFlag) {

    case CfInitialization:
      ControlFlag = CfCheckSelection;
      break;

    case CfCheckSelection:
      if (Selection->Action != UI_ACTION_NONE) {
        ControlFlag = CfExit;
        if (Selection->Action == UI_HOTKEY_F10 ||
            Selection->Action == UI_HOTKEY_F9  ||
            Selection->Action == UI_HOTKEY_ESC) {
          //
          // Clear highlight record for this menu
          //
          Selection->QuestionId = CurrentMenu->QuestionId;
          CurrentMenu->QuestionId = 0;
          if (MenuOption != NULL && IsSelectable (MenuOption)) {
            Selection->CurrentRow = MenuOption->Row;
          }
        }
      } else {
        ControlFlag = CfRepaint;
      }
      break;

    case CfRepaint:
      ControlFlag = CfRefreshHighLight;

      if (Repaint) {
        //
        // Display menu
        //
        gDownArrow      = FALSE;
        gUpArrow        = FALSE;
        Row             = TopRow;

        Temp            = SkipValue;
        Temp2           = SkipValue;

        if (gDeviceManagerSetup != NULL) {
          ClearLines (
            LocalScreen.LeftColumn,
            LocalScreen.RightColumn,
            TopRow - 1,
            BottomRow + 1,
            NormalTextAttr
            );
          SetupUtilityPrintBorderLine (&LocalScreen, gPromptBlockWidth + gOptionBlockWidth + 1);
        } else {

          ClearLines (
            LocalScreen.LeftColumn,
            LocalScreen.RightColumn,
            TopRow - SCROLL_ARROW_HEIGHT,
            BottomRow + SCROLL_ARROW_HEIGHT,
            NormalTextAttr
            );
        }

        UiFreeRefreshList ();

        //
        // default refresh interval is UINT8 maximum
        //
        MinRefreshInterval = 255;

        if (IsListEmpty (&Menu)) {
          ControlFlag = CfReadKey;
          Selection->NoMenuOption = TRUE;
          break;
        }
        Selection->NoMenuOption = FALSE;

        for (Link = TopOfScreen; Link != &Menu; Link = Link->ForwardLink) {
          MenuOption          = MENU_OPTION_FROM_LINK (Link);
          MenuOption->Row     = Row;
          MenuOption->Col     = Col;
          MenuOption->OptCol  = gPromptBlockWidth + 1 + LocalScreen.LeftColumn;

          Statement = MenuOption->ThisTag;
          if (Statement->InSubtitle) {
            MenuOption->Col += SUBTITLE_INDENT;
          }

          if (MenuOption->GrayOut) {
            gST->ConOut->SetAttribute (gST->ConOut, FIELD_TEXT_GRAYED | FIELD_BACKGROUND);
          } else if (Statement->Operand == EFI_IFR_SUBTITLE_OP ||
                     (gDeviceManagerSetup && Statement->Operand == EFI_IFR_TEXT_OP && !(Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK))) {
              gST->ConOut->SetAttribute (gST->ConOut, SubTitleTextAttr);
          } else {
              gST->ConOut->SetAttribute (gST->ConOut, NormalTextAttr);
          }

          DisplayMenuOptionPrompt (MenuOption, Temp, TopRow, BottomRow);
          Temp = 0;

          Status = ProcessOptions (Selection, MenuOption, FALSE, &OptionString);
          if (EFI_ERROR (Status)) {
            //
            // Repaint to clear possible error prompt pop-up
            //
            Repaint = TRUE;
            NewLine = TRUE;
            ControlFlag = CfRepaint;
            break;
          }

          if (OptionString != NULL) {
            if (Statement->Operand == EFI_IFR_DATE_OP || Statement->Operand == EFI_IFR_TIME_OP) {
              //
              // If leading spaces on OptionString - remove the spaces
              //
              for (Index = 0; OptionString[Index] == L' '; Index++) {
                MenuOption->OptCol++;
              }

              for (Count = 0; OptionString[Index] != CHAR_NULL; Index++) {
                OptionString[Count] = OptionString[Index];
                Count++;
              }

              OptionString[Count] = CHAR_NULL;
            }

            DisplayMenuOptionValue(MenuOption, OptionString, Temp2, TopRow, BottomRow);
            Temp2 = 0;
            ScuSafeFreePool ((VOID **)&OptionString);
          }

          //
          // If Question request refresh, register the op-code
          //
          if (Statement->RefreshInterval != 0) {
            //
            // Menu will be refreshed at minimal interval of all Questions
            // which have refresh request, the RTC data and time refresh interval
            // are used any other method to refresh and always are 1, so we needn't
            // include them to clacluate minimal refresh interval.
            //
            if ((Statement->RefreshInterval < MinRefreshInterval)
               && (Statement->Operand != EFI_IFR_DATE_OP && Statement->Operand != EFI_IFR_TIME_OP)) {
              MinRefreshInterval = Statement->RefreshInterval;
            }

            if (gMenuRefreshHead == NULL) {
              MenuRefreshEntry = AllocateZeroPool (sizeof (MENU_REFRESH_ENTRY));
              ASSERT (MenuRefreshEntry != NULL);
              MenuRefreshEntry->MenuOption        = MenuOption;
              MenuRefreshEntry->Selection         = Selection;
              MenuRefreshEntry->CurrentColumn     = MenuOption->OptCol;
              MenuRefreshEntry->CurrentRow        = MenuOption->Row;
              if (MenuOption->GrayOut) {
                MenuRefreshEntry->CurrentAttribute  = EFI_DARKGRAY | EFI_BACKGROUND_LIGHTGRAY;
              } else {
                MenuRefreshEntry->CurrentAttribute  = NormalTextAttr;
              }
              MenuRefreshEntry->SkipValue         = Link == TopOfScreen ? (UINTN) SkipValue : 0;
              gMenuRefreshHead                    = MenuRefreshEntry;
            } else {
              //
              // Advance to the last entry
              //
              for (MenuRefreshEntry = gMenuRefreshHead;
                   MenuRefreshEntry->Next != NULL;
                   MenuRefreshEntry = MenuRefreshEntry->Next
                  )
                ;
              MenuRefreshEntry->Next = AllocateZeroPool (sizeof (MENU_REFRESH_ENTRY));
              ASSERT (MenuRefreshEntry->Next != NULL);
              MenuRefreshEntry                    = MenuRefreshEntry->Next;
              MenuRefreshEntry->MenuOption        = MenuOption;
              MenuRefreshEntry->Selection         = Selection;
              MenuRefreshEntry->CurrentColumn     = MenuOption->OptCol;
              MenuRefreshEntry->CurrentRow        = MenuOption->Row;
              if (MenuOption->GrayOut) {
                MenuRefreshEntry->CurrentAttribute  = EFI_DARKGRAY | EFI_BACKGROUND_LIGHTGRAY;
              } else {
                MenuRefreshEntry->CurrentAttribute  = NormalTextAttr;
              }
            }
          }

          //
          // If this is a text op with secondary text information
          //
          if (((Statement->Operand == EFI_IFR_TEXT_OP) ||
               (Statement->Operand == EFI_IFR_ACTION_OP) ||
               (Statement->Operand == EFI_IFR_PASSWORD_OP)) &&
               (Statement->TextTwo != 0)) {
            if (Statement->Operand == EFI_IFR_PASSWORD_OP) {
              gST->ConOut->SetAttribute (gST->ConOut, FIELD_TEXT_GRAYED | FIELD_BACKGROUND);
            }
            StringPtr   = GetToken (Statement->TextTwo, MenuOption->Handle);
            DisplayMenuOptionValue (MenuOption, StringPtr, Temp2, TopRow, BottomRow);
            Temp2 = 0;
            gBS->FreePool (StringPtr);
          }

          //
          // In front page, there are many animation in the same time.
          //

          if (gGraphicsEnabled &&
              CompareGuid (&Selection->FormSetGuid, &gFrontPageFormsetGuid) &&
              gAnimationPackage != NULL) {

              //ASSERT (MenuOption->ThisTag->AnimationId != 0);
              UiAddAnimationList (
                MenuOption->ThisTag->AnimationId,
                MenuOption, 0, 0,
                mGop->Mode->Info->HorizontalResolution,
                mGop->Mode->Info->VerticalResolution,
                ANIM_STATUS_STOP
                );
          }
          //
          // Need to handle the bottom of the display
          //
          if (MenuOption->Skip > 1) {
            Row += MenuOption->Skip - SkipValue;
            SkipValue = 0;
          } else {
            Row += MenuOption->Skip;
          }

          if (Row > BottomRow) {
            if (!ValueIsScroll (FALSE, Link)) {
              gDownArrow = TRUE;
            }

            Row = BottomRow + 1;
            break;
          }
        }

        if (!ValueIsScroll (TRUE, TopOfScreen)) {
          gUpArrow = TRUE;
        }

        if (gUpArrow) {
          gST->ConOut->SetAttribute (gST->ConOut, ARROW_TEXT | ARROW_BACKGROUND);
          gUpArrowX = LocalScreen.LeftColumn + gPromptBlockWidth + gOptionBlockWidth + 1;
          gUpArrowY = TopRow - SCROLL_ARROW_HEIGHT;
          PrintCharAt (gUpArrowX, gUpArrowY, ARROW_UP);
        }
        if (gDownArrow) {
          gST->ConOut->SetAttribute (gST->ConOut, ARROW_TEXT | ARROW_BACKGROUND);
          gDownArrowX = LocalScreen.LeftColumn + gPromptBlockWidth + gOptionBlockWidth + 1;
          gDownArrowY = BottomRow + SCROLL_ARROW_HEIGHT;
          PrintCharAt (gDownArrowX, gDownArrowY, ARROW_DOWN);
        }
        gST->ConOut->SetAttribute (gST->ConOut, NormalTextAttr);


        if (gGraphicsEnabled &&
            CompareGuid (&Selection->FormSetGuid, &gFrontPageFormsetGuid)) {
          UpdateAnimationLayout ();
        }

        MenuOption = NULL;
      }
      break;

    case CfRefreshHighLight:
      //
      // MenuOption: Last menu option that need to remove hilight
      //             MenuOption is set to NULL in Repaint
      // NewPos:     Current menu option that need to hilight
      //
      ControlFlag = CfUpdateHelpString;

      //
      // Repaint flag is normally reset when finish processing CfUpdateHelpString. Temporarily
      // reset Repaint flag because we may break halfway and skip CfUpdateHelpString processing.
      //
      SavedValue  = Repaint;
      Repaint     = FALSE;

      if (Selection->QuestionId != 0) {
        Index = TopRow;
        NewPos = TopOfScreen;
        SavedMenuOption = MENU_OPTION_FROM_LINK (NewPos);

        while (SavedMenuOption->ThisTag->QuestionId != Selection->QuestionId && NewPos->ForwardLink != &Menu) {
          Index += SavedMenuOption->Skip;
          NewPos = NewPos->ForwardLink;
          SavedMenuOption = MENU_OPTION_FROM_LINK (NewPos);
        }

        if (SavedMenuOption->ThisTag->QuestionId == Selection->QuestionId) {
          if (Selection->CurrentRow == 0 && Index > BottomRow) {
            //
            // No current row data and highlight option exceed page size, re-calculate TopOfScreen.
            //
            Repaint = TRUE;
            NewLine = TRUE;
            ControlFlag = CfRepaint;

            while (Index > BottomRow) {
              SavedMenuOption = MENU_OPTION_FROM_LINK (TopOfScreen);
              Index -= SavedMenuOption->Skip;
              TopOfScreen = TopOfScreen->ForwardLink;
            }

            AdjustDateAndTimePosition (TRUE, &TopOfScreen);
            break;
          }
          //
          // Based on current row of highlight option, find out TopOfScreen.
          //
          if (Selection->CurrentRow != 0) {
            Link = NewPos;
            Index = Selection->CurrentRow;
            while (Index > TopRow && Link->BackLink != &Menu) {
              Link = Link->BackLink;
              SavedMenuOption = MENU_OPTION_FROM_LINK (Link);
              Index -= SavedMenuOption->Skip;
            }

            SkipValue = TopRow > Index ? TopRow - Index : 0;
            if ((TopOfScreen != Link) || (SkipValue != OldSkipValue)) {
              TopOfScreen = Link;
              OldSkipValue = SkipValue;
              Repaint = TRUE;
              NewLine = TRUE;
              ControlFlag = CfRepaint;
              Selection->CurrentRow = 0;
              AdjustDateAndTimePosition (TRUE, &TopOfScreen);
              break;
            }
          }
        } else {
          //
          // Target Question not found, highlight the default menu option
          //
          NewPos = TopOfScreen;
        }

        Selection->QuestionId = 0;
      }

      if (NewPos != NULL && (MenuOption == NULL || NewPos != &MenuOption->Link)) {
        if (MenuOption != NULL && IsSelectable (MenuOption)) {
          //
          // Remove highlight on last Menu Option
          //
          gST->ConOut->SetCursorPosition (gST->ConOut, MenuOption->Col, MenuOption->Row);
          ProcessOptions (Selection, MenuOption, FALSE, &OptionString);
          gST->ConOut->SetAttribute (gST->ConOut, NormalTextAttr);
          if ((OptionString != NULL) && (!MenuOption->ReadOnly)) {

            DisplayMenuOptionPrompt (MenuOption, 0, TopRow, BottomRow);
            if ((MenuOption->ThisTag->Operand == EFI_IFR_DATE_OP) ||
                (MenuOption->ThisTag->Operand == EFI_IFR_TIME_OP)
               ) {
              //
              // If leading spaces on OptionString - remove the spaces
              //
              for (Index = 0; OptionString[Index] == L' '; Index++)
                ;

              for (Count = 0; OptionString[Index] != CHAR_NULL; Index++) {
                OptionString[Count] = OptionString[Index];
                Count++;
              }

              OptionString[Count] = CHAR_NULL;
            }
            DisplayMenuOptionValue (MenuOption, OptionString, 0, TopRow, BottomRow);
            ScuSafeFreePool ((VOID **)&OptionString);
          } else {
            if (NewLine) {
              if (MenuOption->GrayOut) {
                gST->ConOut->SetAttribute (gST->ConOut, FIELD_TEXT_GRAYED | FIELD_BACKGROUND);
              } else if (MenuOption->ThisTag->Operand == EFI_IFR_SUBTITLE_OP) {
                gST->ConOut->SetAttribute (gST->ConOut, SubTitleTextAttr);
              } else {
                gST->ConOut->SetAttribute (gST->ConOut, NormalTextAttr);
              }

              DisplayMenuOptionPrompt (MenuOption, 0, TopRow, BottomRow);
              gST->ConOut->SetAttribute (gST->ConOut, NormalTextAttr);
            }
          }
          if (((MenuOption->ThisTag->Operand == EFI_IFR_TEXT_OP) || (MenuOption->ThisTag->Operand == EFI_IFR_ACTION_OP)) && (MenuOption->ThisTag->TextTwo != 0)) {
            StringPtr   = GetToken (MenuOption->ThisTag->TextTwo, MenuOption->Handle);
            DisplayMenuOptionValue (MenuOption, StringPtr, 0, TopRow, BottomRow);
            gBS->FreePool (StringPtr);
          }
        }
        //
        // This is only possible if we entered this page and the first menu option is
        // a "non-menu" item.  In that case, force it UiDown
        //
        MenuOption = MENU_OPTION_FROM_LINK (NewPos);
        if (!IsSelectable (MenuOption)) {
          if (HaveAnySelectableItem ()) {
            ScreenOperation = UiDown;
            ControlFlag     = CfScreenOperation;
          } else {
            ControlFlag = CfUpdateHelpString;
            Statement = MenuOption->ThisTag;
            Selection->Statement = Statement;
          }
          break;
        }

        //
        // This is the current selected statement
        //
        Statement = MenuOption->ThisTag;
        Selection->Statement = Statement;
        //
        // Record highlight for current menu
        //
        CurrentMenu->QuestionId = Statement->QuestionId;

        //
        // Set reverse attribute
        //
        gST->ConOut->SetAttribute (gST->ConOut, HighLightTextAttr);
        gST->ConOut->SetCursorPosition (gST->ConOut, MenuOption->Col, MenuOption->Row);

        //
        // Assuming that we have a refresh linked-list created, lets annotate the
        // appropriate entry that we are highlighting with its new attribute.  Just prior to this
        // lets reset all of the entries' attribute so we do not get multiple highlights in he refresh
        //
        if (gMenuRefreshHead != NULL) {
          for (MenuRefreshEntry = gMenuRefreshHead; MenuRefreshEntry != NULL; MenuRefreshEntry = MenuRefreshEntry->Next) {
            if (MenuRefreshEntry->MenuOption->GrayOut) {
              MenuRefreshEntry->CurrentAttribute  = EFI_DARKGRAY | EFI_BACKGROUND_LIGHTGRAY;
            } else {
              MenuRefreshEntry->CurrentAttribute  = NormalTextAttr;
            }

            if (MenuRefreshEntry->MenuOption == MenuOption) {
              MenuRefreshEntry->CurrentAttribute = HighLightTextAttr;
            }
          }
        }

        ProcessOptions (Selection, MenuOption, FALSE, &OptionString);

        if ((OptionString != NULL) && (!MenuOption->ReadOnly)) {
          if (Statement->Operand == EFI_IFR_DATE_OP || Statement->Operand == EFI_IFR_TIME_OP) {
            //
            // If leading spaces on OptionString - remove the spaces
            //
            for (Index = 0; OptionString[Index] == L' '; Index++)
              ;

            for (Count = 0; OptionString[Index] != CHAR_NULL; Index++) {
              OptionString[Count] = OptionString[Index];
              Count++;
            }

            OptionString[Count] = CHAR_NULL;
          }

          DisplayMenuOptionPrompt (MenuOption, 0, TopRow, BottomRow);
          DisplayMenuOptionValue (MenuOption, OptionString, 0, TopRow, BottomRow);
          ScuSafeFreePool ((VOID **)&OptionString);
        } else {
          if (NewLine) {
            DisplayMenuOptionPrompt(MenuOption, 0, TopRow, BottomRow);
          }
        }
        if (((Statement->Operand == EFI_IFR_TEXT_OP) || (Statement->Operand == EFI_IFR_ACTION_OP)) && (Statement->TextTwo != 0)) {
          StringPtr   = GetToken (Statement->TextTwo, MenuOption->Handle);
          DisplayMenuOptionValue (MenuOption, StringPtr, 0, TopRow, BottomRow);
          gBS->FreePool (StringPtr);
        }
        UpdateKeyHelp (Selection, MenuOption, FALSE);
        //
        // Clear reverse attribute
        //
        gST->ConOut->SetAttribute (gST->ConOut, NormalTextAttr);
      }
      //
      // Repaint flag will be used when process CfUpdateHelpString, so restore its value
      // if we didn't break halfway when process CfRefreshHighLight.
      //
      Repaint = SavedValue;
      break;

    case CfUpdateHelpString:
      ControlFlag = CfPrepareToReadKey;
      StringPtr = NULL;
      if ((Repaint || NewLine) && gClassOfVfr != EFI_GENERAL_APPLICATION_SUBCLASS) {
        //
        // Don't print anything if it is a NULL help token
        //
        if (MenuOption->ThisTag->Help == 0) {
          StringPtr = AllocateZeroPool (sizeof (CHAR16));;
        } else {
          StringPtr = GetToken (MenuOption->ThisTag->Help, MenuOption->Handle);
        }

        if (gGraphicsEnabled &&
            CompareGuid (&Selection->FormSetGuid, &gFrontPageFormsetGuid)) {
          UiStopAllAnimation ();
          UiPlayAnimation (MenuOption);
        }

        ProcessHelpString (StringPtr, &FormattedString, (BottomRow - TopRow + 1));
        gST->ConOut->SetAttribute (gST->ConOut, HELP_TEXT | FIELD_BACKGROUND);

        for (Index = 0; Index < (BottomRow - TopRow + 1); Index++) {
          //
          // Pad String with spaces to simulate a clearing of the previous line
          //
          for (; GetStringWidth (&FormattedString[Index * gHelpBlockWidth * 2]) / 2 < gHelpBlockWidth;) {
            //
            // make sure all of the pad space character is narrow character.
            //
            FormattedStrLen = StrLen (&FormattedString[Index * gHelpBlockWidth * 2]);
            FormattedString[Index * gHelpBlockWidth * 2 + FormattedStrLen] = NARROW_CHAR;
            FormattedString[Index * gHelpBlockWidth * 2 + FormattedStrLen + 1] = CHAR_SPACE;
            FormattedString[Index * gHelpBlockWidth * 2 + FormattedStrLen + 2] = CHAR_NULL;
          }

          PrintStringAt (
            LocalScreen.RightColumn - gHelpBlockWidth,
            Index + TopRow,
            &FormattedString[Index * gHelpBlockWidth * 2]
            );
        }
        if (gDeviceManagerSetup) {
          UiFreeAnimationRefreshList ();
          if (gAnimationPackage != NULL && mGop != NULL &&
              MenuOption->ThisTag->AnimationId != 0 && mGifDecoder != NULL) {
            GetStartOffset (mGop, &StartX, &StartY);
            UiAddAnimationList (
              MenuOption->ThisTag->AnimationId,
              MenuOption,
              StartX + (gPromptBlockWidth + gOptionBlockWidth + 3)  * EFI_GLYPH_WIDTH,
              StartY + (LocalScreen.BottomRow / 2)* EFI_GLYPH_HEIGHT,
              mGop->Mode->Info->HorizontalResolution,
              mGop->Mode->Info->VerticalResolution,
              ANIM_STATUS_PLAY
              );
            UiPlayAnimation (MenuOption);
          }
        }
      }
      //
      // Reset this flag every time we finish using it.
      //
      Repaint = FALSE;
      NewLine = FALSE;
      if (DoRepaintScreen) {
        DoRepaintScreen = FALSE;
        ControlFlag = SavedControlFlag;
        Selection->QuestionId = MenuOption->ThisTag->QuestionId;
        if (IsSelectable (MenuOption)) {
          Selection->CurrentRow = MenuOption->Row;
        }
      }
      ScuSafeFreePool ((VOID **)&StringPtr);
      ScuSafeFreePool ((VOID **)&FormattedString);
      break;

    case CfPrepareToReadKey:
      ControlFlag = CfReadKey;
      ScreenOperation = UiNoOperation;
      break;

    case CfReadKey:
      ControlFlag = CfScreenOperation;

      ScreenOperation = UiNoOperation;
      Link = NULL;
      if (PcdGetBool (PcdReturnDialogCycle)) {
        if (Selection->SelectAgain) {
          Selection->SelectAgain = FALSE;
          Key.ScanCode = SCAN_NULL;
          Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
        } else {
  	      Status = SetupUtilityReadKey(&MouseKey, &Key, TopOfScreen, &Link, MinRefreshInterval);
        }
      } else {
        SetupUtilityReadKey(&MouseKey, &Key, TopOfScreen, &Link, MinRefreshInterval);
      }
      if (mRepaintFormFlag) {
        Selection->Action = UI_ACTION_REFRESH_FORM;
        Selection->Statement = NULL;
        Selection->QuestionId = MenuOption->ThisTag->QuestionId;
        if (MenuOption != NULL && IsSelectable (MenuOption)) {
          Selection->CurrentRow = MenuOption->Row;
        }
        mRepaintFormFlag = FALSE;
        break;
      }

      if (IsListEmpty (&Menu)) {
        ControlFlag = CfReadKey;

        if (((Key.ScanCode == SCAN_F1) && ((gFunctionKeySetting & FUNCTION_ONE) != FUNCTION_ONE)) ||
            ((Key.ScanCode == SCAN_F9) && ((gFunctionKeySetting & FUNCTION_NINE) != FUNCTION_NINE)) ||
            ((Key.ScanCode == SCAN_F10) && ((gFunctionKeySetting & FUNCTION_TEN) != FUNCTION_TEN))
            ) {
          //
          // If the function key has been disabled, just ignore the key.
          //
        } else {
          switch (Key.ScanCode) {

          case SCAN_F1:
            if (gDeviceManagerSetup != NULL) {
              ControlFlag = CfUiShowHelpScreen;
            }
            break;

          case SCAN_F9:
            if (gDeviceManagerSetup != NULL) {
              DefaultId = EFI_HII_DEFAULT_CLASS_STANDARD;
            }
            ControlFlag = CfUiDefault;
            break;

          case SCAN_F10:
            if (gDeviceManagerSetup != NULL) {
              ControlFlag = CfUiSaveAndExit;
            } else {
              ControlFlag = CfUiSave;
            }
            break;

          case SCAN_ESC:
            ControlFlag = CfUiReset;
            break;

          default:
            break;
          }
        }

        break;
      }
      if (gGraphicsEnabled &&
          CompareGuid (&Selection->FormSetGuid, &gFrontPageFormsetGuid) &&
          gAnimationPackage != NULL) {
        for (
          ItemCount = 0, Entry = (ANIMATION_REFRESH_ENTRY *)GetFirstNode (&gAnimationRefreshList);
          !IsNull (&gAnimationRefreshList, (LIST_ENTRY *)Entry);
          Entry = (ANIMATION_REFRESH_ENTRY *) GetNextNode (&gAnimationRefreshList, (LIST_ENTRY *)Entry)) {
          ItemCount++;
        }
        NumColumn = GetAnimationLayoutNumColumn (ItemCount);
        if (Key.ScanCode == SCAN_LEFT || Key.ScanCode == SCAN_RIGHT) {
          Key.ScanCode = (Key.ScanCode == SCAN_LEFT) ? SCAN_UP : SCAN_DOWN;
        } else if (Key.ScanCode == SCAN_UP) {
          for (Link = NewPos, Count = 0; (Link != &Menu) && (Count < NumColumn);
            Link = Link->BackLink, Count++) {}
          if (Count != NumColumn || Link == &Menu) {
            ControlFlag = CfPrepareToReadKey;
            break;
          }
        } else if (Key.ScanCode == SCAN_DOWN) {
          for (Link = NewPos, Count = 0; (Link != &Menu) && (Count < NumColumn);
            Link = Link->ForwardLink, Count++) {}
          if (Count != NumColumn || Link == &Menu) {
            ControlFlag = CfPrepareToReadKey;
            break;
          }
        }
      }

      if(Link != NULL) {
        if (Link != NewPos) {
          NewPos = Link;
          NewLine = TRUE;
          ControlFlag = CfRepaint;
          break;
        }
      }
      else if (gDeviceManagerSetup != NULL) {
        if (gDeviceManagerSetup->Direction == Jump) {
          ScreenOperation = UiJumpMenu;
          break;
        }
      }

      switch (Key.UnicodeChar) {
      case CHAR_CARRIAGE_RETURN:
        ScreenOperation = UiSelect;
        gDirection      = 0;
        break;

      //
      // We will push the adjustment of these numeric values directly to the input handler
      //  NOTE: we won't handle manual input numeric
      //
      case '+':
      case '-':
        Statement = MenuOption->ThisTag;
        if ((Statement->Operand == EFI_IFR_DATE_OP)
          || (Statement->Operand == EFI_IFR_TIME_OP)
          || ((Statement->Operand == EFI_IFR_NUMERIC_OP) && (Statement->Step != 0))
        ){
          if (Key.UnicodeChar == '+') {
            gDirection = SCAN_RIGHT;
          } else {
            gDirection = SCAN_LEFT;
          }
          Status = ProcessOptions (Selection, MenuOption, TRUE, &OptionString);
          if (EFI_ERROR (Status)) {
            //
            // Repaint to clear possible error prompt pop-up
            //
            Repaint = TRUE;
            NewLine = TRUE;
          }
          ScuSafeFreePool ((VOID **)&OptionString);
        } else if ( (Statement->Operand == EFI_IFR_ACTION_OP) &&
                    (Statement->QuestionFlags == EFI_IFR_CHECKBOX_DEFAULT_MFG)) {

          EFI_IFR_TYPE_VALUE         Value;
          EFI_BROWSER_ACTION_REQUEST DummyActionRequest;

          Value.u32 = Statement->QuestionId;
          Status = ConfigAccess->Callback(
                                    ConfigAccess,
                                    EFI_BROWSER_ACTION_CHANGING,
                                    Key.UnicodeChar == '+' ? KEY_UP_SHIFT : KEY_DOWN_SHIFT,
                                    EFI_IFR_TYPE_NUM_SIZE_32,
                                    &Value,
                                    &DummyActionRequest
                                    );
          if (!EFI_ERROR (Status)) {
            gDirection = 0;
            ScreenOperation = UiSelect;
            NewPos = Key.UnicodeChar == '+' ? NewPos->BackLink : NewPos->ForwardLink;
            MenuOption = MENU_OPTION_FROM_LINK (NewPos);
          }

        }
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (gDeviceManagerSetup != NULL) {
          Statement = MenuOption->ThisTag;
          if ((Statement->Operand == EFI_IFR_DATE_OP)
            || (Statement->Operand == EFI_IFR_TIME_OP)) {
            Status = DateTimeInputOnTheFly(Selection, MenuOption, Key);
            if(EFI_ERROR(Status)) {
              Repaint = TRUE;
              NewLine = TRUE;
            }
          }
        }
        break;

      case '^':
        ScreenOperation = UiUp;
        break;

      case 'V':
      case 'v':
        ScreenOperation = UiDown;
        break;

      case ' ':
        if (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE) {
          if (MenuOption->ThisTag->Operand == EFI_IFR_CHECKBOX_OP && !MenuOption->GrayOut) {
            ScreenOperation = UiSelect;
          }
        }
        break;

      case CHAR_NULL:

        if (((Key.ScanCode == SCAN_F1) && ((gFunctionKeySetting & FUNCTION_ONE) != FUNCTION_ONE)) ||
            ((Key.ScanCode == SCAN_F9) && ((gFunctionKeySetting & FUNCTION_NINE) != FUNCTION_NINE)) ||
            ((Key.ScanCode == SCAN_F10) && ((gFunctionKeySetting & FUNCTION_TEN) != FUNCTION_TEN))
            ) {
          //
          // If the function key has been disabled, just ignore the key.
          //
        } else if (Key.ScanCode == SCAN_F5 || Key.ScanCode == SCAN_F6) {
          Statement = MenuOption->ThisTag;
          if (Statement->Operand == EFI_IFR_ONE_OF_OP) {
            ScreenOperation = UiSelect;
            gDirection = 0;
            OptionItemAction = Key.ScanCode == SCAN_F6 ? OPTION_ITEM_UP : OPTION_ITEM_DOWN;
          } else {
            Key.UnicodeChar = Key.ScanCode == SCAN_F6 ? '+' : '-';
            Key.ScanCode = 0;
          }
          if ((Statement->Operand == EFI_IFR_DATE_OP)
            || (Statement->Operand == EFI_IFR_TIME_OP)
            || ((Statement->Operand == EFI_IFR_NUMERIC_OP) && (Statement->Step != 0))
          ){
            if (Key.UnicodeChar == '+') {
              gDirection = SCAN_RIGHT;
            } else {
              gDirection = SCAN_LEFT;
            }
            Status = ProcessOptions (Selection, MenuOption, TRUE, &OptionString);
            if (EFI_ERROR (Status)) {
              //
              // Repaint to clear possible error prompt pop-up
              //
              Repaint = TRUE;
              NewLine = TRUE;
            }
            ScuSafeFreePool ((VOID **)&OptionString);
          } else if ( (Statement->Operand == EFI_IFR_ACTION_OP) &&
                      (Statement->QuestionFlags == EFI_IFR_CHECKBOX_DEFAULT_MFG)) {

            EFI_IFR_TYPE_VALUE         Value;
            EFI_BROWSER_ACTION_REQUEST DummyActionRequest;

            Value.u32 = Statement->QuestionId;
            Status = ConfigAccess->Callback(
                                      ConfigAccess,
                                      EFI_BROWSER_ACTION_CHANGING,
                                      Key.UnicodeChar == '+' ? KEY_UP_SHIFT : KEY_DOWN_SHIFT,
                                      EFI_IFR_TYPE_NUM_SIZE_32,
                                      &Value,
                                      &DummyActionRequest
                                      );
            if (!EFI_ERROR (Status)) {
              gDirection = 0;
              ScreenOperation = UiSelect;
              NewPos = Key.UnicodeChar == '+' ? NewPos->BackLink : NewPos->ForwardLink;
              MenuOption = MENU_OPTION_FROM_LINK (NewPos);
            }

          }
        } else {
          for (Index = 0; Index < sizeof (gScanCodeToOperation) / sizeof (gScanCodeToOperation[0]); Index++) {
            if (Key.ScanCode == gScanCodeToOperation[Index].ScanCode) {

              if (Key.ScanCode == SCAN_F9) {
                //
                // Reset to standard default
                //
                if (gDeviceManagerSetup != NULL) {
                  DefaultId = EFI_HII_DEFAULT_CLASS_STANDARD;
                }
              }


              if ((Key.ScanCode == SCAN_F10) || (Key.ScanCode == SCAN_F1)) {
                if(gDeviceManagerSetup != NULL || (Key.ScanCode == SCAN_F10 && gSecureBootMgr)) {
                  ScreenOperation = gScanCodeToOperation[Index].ScreenOperation;
                } else {
                  if (Key.ScanCode == SCAN_F10) {
                    ScreenOperation = UiSave;
                  } else if (Key.ScanCode == SCAN_F1) {
                    ScreenOperation = UiNoOperation;
                  }
                }
              } else {
                if (gScanCodeToOperation[Index].ScreenOperation != UiMaxOperation) {
                  ScreenOperation = gScanCodeToOperation[Index].ScreenOperation;
                }
              }
              break;
            }
          }
        }
        break;
      default:
        if (mBootManager == TRUE) {
          CodeToOperation = PcdGetPtr(PcdScanCodeToOperation);
          ScanCodeToOperationCount = PcdGet8(PcdScanCodeToOperationCount);

	  //
	  // OemServices
	  //
          Status = OemSvcInstallBootMangerKeyTable (
                     &ScanCodeToOperationCount,
                     &CodeToOperation
                     );

          if (ScanCodeToOperationCount != 0) {
            for (Index = 0; CodeToOperation[Index].ScreenOperation != UiMaxOperation && Index < ScanCodeToOperationCount; Index++) {
              if (Key.ScanCode == CodeToOperation[Index].ScanCode &&
                  Key.UnicodeChar == CodeToOperation[Index].UnicodeChar) {
                ScreenOperation = CodeToOperation[Index].ScreenOperation;
                break;
              }
            }
          }
        }
        break;
      }
      break;

    case CfScreenOperation:
      for (Index = 0;
           Index < sizeof (gScreenOperationToControlFlag) / sizeof (gScreenOperationToControlFlag[0]);
           Index++
          ) {
        if (ScreenOperation == gScreenOperationToControlFlag[Index].ScreenOperation) {
          ControlFlag = gScreenOperationToControlFlag[Index].ControlFlag;
          break;
        }
      }
      break;

    case CfUiSelect:
      ControlFlag = CfCheckSelection;

      Statement = MenuOption->ThisTag;
      if ((Statement->Operand == EFI_IFR_DATE_OP) || (Statement->Operand == EFI_IFR_TIME_OP)) {
        if (MenuOption->Sequence != 2) {
          //
          // In the middle or tail of the Date/Time op-code set, go left.
          //
          NewPos = NewPos->ForwardLink;
        } else {
          NewPos = NewPos->BackLink->BackLink;
        }
        break;
      } else if ((Statement->Operand == EFI_IFR_TEXT_OP) ||
                 (Statement->Operand == EFI_IFR_DATE_OP) ||
                 (Statement->Operand == EFI_IFR_TIME_OP) ||
                 MenuOption->ReadOnly) {
        break;
      }

      //
      // Keep highlight on current MenuOption
      //
      Selection->QuestionId = Statement->QuestionId;

      switch (Statement->Operand) {
      case EFI_IFR_REF_OP:
        if (Statement->RefDevicePath != 0) {
          //
          // Goto another Hii Package list
          //
          ControlFlag = CfCheckSelection;
          Selection->Action = UI_ACTION_REFRESH_FORMSET;

          StringPtr = GetToken (Statement->RefDevicePath, Selection->FormSet->HiiHandle);
          if (StringPtr == NULL) {
            //
            // No device path string not found, exit
            //
            Selection->Action = UI_ACTION_EXIT;
            Selection->Statement = NULL;
            break;
          }
          BufferSize = StrLen (StringPtr) / 2;
          DevicePath = AllocatePool (BufferSize);

          HexStringToBuffer ((UINT8 *) DevicePath, &BufferSize, StringPtr);
          Selection->Handle = DevicePathToHiiHandle (DevicePath);
          if (Selection->Handle == NULL) {
            //
            // If target Hii Handle not found, exit
            //
            Selection->Action = UI_ACTION_EXIT;
            Selection->Statement = NULL;
            break;
          }

          gBS->FreePool (StringPtr);
          gBS->FreePool (DevicePath);

          CopyMem (&Selection->FormSetGuid, &Statement->RefFormSetId, sizeof (EFI_GUID));
          Selection->FormId = Statement->RefFormId;
          Selection->QuestionId = Statement->RefQuestionId;
        } else if (!CompareGuid (&Statement->RefFormSetId, &gZeroGuid)) {
          //
          // Goto another Formset, check for uncommitted data
          //
          ControlFlag = CfCheckSelection;
          Selection->Action = UI_ACTION_REFRESH_FORMSET;

          CopyMem (&Selection->FormSetGuid, &Statement->RefFormSetId, sizeof (EFI_GUID));
          Selection->FormId = Statement->RefFormId;
          Selection->QuestionId = Statement->RefQuestionId;
        } else if (Statement->RefFormId != 0) {
          //
          // Goto another form inside this formset,
          //
          Selection->Action = UI_ACTION_REFRESH_FORM;

          //
          // Link current form so that we can always go back when someone hits the ESC
          //
          MenuList = UiFindMenuList (&Selection->FormSetGuid, Statement->RefFormId);
          if (MenuList == NULL) {
            MenuList = UiAddMenuList (CurrentMenu, &Selection->FormSetGuid, Statement->RefFormId);
          }

          Selection->FormId = Statement->RefFormId;
          Selection->QuestionId = Statement->RefQuestionId;
        } else if (Statement->RefQuestionId != 0) {
          //
          // Goto another Question
          //
          Selection->QuestionId = Statement->RefQuestionId;

          if ((Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK)) {
            Selection->Action = UI_ACTION_REFRESH_FORM;
          } else {
            Repaint = TRUE;
            NewLine = TRUE;
            break;
          }
        }
        break;

      case EFI_IFR_ACTION_OP:
        //
        // Process the Config string <ConfigResp>
        //
        Status = ProcessQuestionConfig (Selection, Statement);

        if (EFI_ERROR (Status)) {
          break;
        }

        //
        // The action button may change some Question value, so refresh the form
        //
        Selection->Action = UI_ACTION_REFRESH_FORM;
        Selection->CurrentRow = MenuOption->Row;
        break;

      case EFI_IFR_RESET_BUTTON_OP:
        //
        // Reset Question to default value specified by DefaultId
        //
        ControlFlag = CfUiDefault;
        DefaultId = Statement->DefaultId;
        Selection->CurrentRow = MenuOption->Row;
        break;

      default:
        //
        // Editable Questions: oneof, ordered list, checkbox, numeric, string, password
        //
        UpdateKeyHelp (Selection, MenuOption, TRUE);
        if (OptionItemAction != OPTION_ITEM_NO_ACTION) {
          Status = ChangeItemSelection (Selection, MenuOption, OptionItemAction);
          OptionItemAction = OPTION_ITEM_NO_ACTION;
        } else {
          Status = ProcessOptions (Selection, MenuOption, TRUE, &OptionString);
          ScuSafeFreePool ((VOID **)&OptionString);
        }

        Selection->CurrentRow = 0;
        if (EFI_ERROR (Status)) {
          Repaint = TRUE;
          NewLine = TRUE;
          UpdateKeyHelp (Selection, MenuOption, FALSE);
        } else {
          Selection->Action = UI_ACTION_REFRESH_FORM;
          //
          // ProcessOptions() may pop dialog but don't clean it. So before exit UiDisplayMenu, repaint the page.
          //
          if (Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK &&
              (Statement->Operand == EFI_IFR_ORDERED_LIST_OP ||
               Statement->Operand == EFI_IFR_ONE_OF_OP ||
               Statement->Operand == EFI_IFR_STRING_OP) &&
              gPopUpDialogBiggerThanPlatformDialog) {
            Selection->QuestionId = 0;
            DoRepaintScreen = TRUE;
            Repaint = TRUE;
            SavedControlFlag = ControlFlag;
            ControlFlag = CfRepaint;
          } else {
            if (IsSelectable (MenuOption)) {
              Selection->CurrentRow = MenuOption->Row;
            }
          }
        }

        gPopUpDialogBiggerThanPlatformDialog = FALSE;
        SkipValue = OldSkipValue;
        break;
      }
      break;

    case CfUiReset:
      //
      // We come here when someone press ESC
      //
      ControlFlag = CfCheckSelection;

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
        break;
      }
      if (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) {
        //
        // We never exit FrontPage, so skip the ESC
        //
        Selection->Action = UI_ACTION_NONE;
        break;
      }
      if (gDeviceManagerSetup == NULL && !gSecureBootMgr) {
        //
        // We are going to leave current FormSet, so check uncommited data in this FormSet
        //
        if (gNvUpdateRequired) {
          Status      = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

          YesResponse = gYesResponse[0];
          NoResponse  = gNoResponse[0];

          //
          // If NV flag is up, prompt user
          //
          do {
            CreateDialog (4, TRUE, 0, NULL, &Key, gEmptyString, gSaveChanges, gAreYouSure, gEmptyString);
          } while
          (
            (Key.ScanCode != SCAN_ESC) &&
            ((Key.UnicodeChar | UPPER_LOWER_CASE_OFFSET) != (NoResponse | UPPER_LOWER_CASE_OFFSET)) &&
            ((Key.UnicodeChar | UPPER_LOWER_CASE_OFFSET) != (YesResponse | UPPER_LOWER_CASE_OFFSET))
          );

          if (Key.ScanCode == SCAN_ESC) {
            //
            // User hits the ESC key
            //
            Repaint = TRUE;
            NewLine = TRUE;

            Selection->Action = UI_ACTION_NONE;
            break;
          }

          //
          // If the user hits the YesResponse key
          //
          if ((Key.UnicodeChar | UPPER_LOWER_CASE_OFFSET) == (YesResponse | UPPER_LOWER_CASE_OFFSET)) {
            Status = SubmitForm (Selection->FormSet, Selection->Form);
          }
        }
      }


      Selection->Action = UI_ACTION_EXIT;

      if (gDeviceManagerSetup == NULL && !gSecureBootMgr) {
        Selection->Statement = NULL;
        CurrentMenu->QuestionId = 0;
        UiFreeAnimationRefreshList ();
        if (mGopList != NULL) {
          gBS->FreePool (mGopList);
          mGopList = NULL;
        }
        return EFI_SUCCESS;
      }
      Selection->Action = UI_HOTKEY_ESC;
      break;

    case CfUiLeft:
    case CfUiRight:

          if (gDeviceManagerSetup) {
            if (gDeviceManagerSetup->AtRoot) {
              Selection->Action = UI_ACTION_EXIT;
              Selection->Statement->QuestionFlags = 0;
              CurrentMenu->QuestionId = 0;
              //
              // Handle menu change so set flags and exit
              //
              gDeviceManagerSetup->Direction = (ControlFlag == CfUiLeft) ? Left : Right;
              gDeviceManagerSetup->PreviousMenuEntry = 0;
              //
              // Deal with NvRam map change
              //
              if (gNvUpdateRequired) {
                //
                // Make sure to set the changed flag for later use
                //
                gDeviceManagerSetup->Changed = TRUE;
              }
            }
          }

          ControlFlag = CfCheckSelection;
      break;

    case CfUiUp:
      ControlFlag = CfCheckSelection;
      SavedListEntry = TopOfScreen;

      if (NewPos->BackLink != &Menu) {
        NewLine = TRUE;
        //
        // Adjust Date/Time position before we advance forward.
        //
        AdjustDateAndTimePosition (TRUE, &NewPos);

        //
        // Caution that we have already rewind to the top, don't go backward in this situation.
        //
        if (NewPos->BackLink != &Menu) {
          NewPos = NewPos->BackLink;
        }

        PreviousMenuOption = MENU_OPTION_FROM_LINK (NewPos);
        DistanceValue = PreviousMenuOption->Skip;

        //
        // Since the behavior of hitting the up arrow on a Date/Time op-code is intended
        // to be one that back to the previous set of op-codes, we need to advance to the sencond
        // Date/Time op-code and leave the remaining logic in UiDown intact so the appropriate
        // checking can be done.
        //
        DistanceValue += AdjustDateAndTimePosition (TRUE, &NewPos);

        //
        // Check the previous menu entry to see if it was a zero-length advance.  If it was,
        // don't worry about a redraw.
        //
        if ((INTN) MenuOption->Row - (INTN) DistanceValue < (INTN) TopRow) {
          Repaint     = TRUE;
          TopOfScreen = NewPos;
        }

        Difference = MoveToNextStatement (TRUE, &NewPos);
        if (Difference >= 0) {
          PreviousMenuOption = MENU_OPTION_FROM_LINK (NewPos);
          Difference += PreviousMenuOption->Skip;
        }
        if ((INTN) MenuOption->Row - (INTN) Difference < (INTN) TopRow) {
          if (Difference > 0) {
            //
            // Previous focus MenuOption is above the TopOfScreen, so we need to scroll
            //
            TopOfScreen = NewPos;
            Repaint     = TRUE;
            SkipValue = 0;
            OldSkipValue = 0;
          }
        }
        //
        // If we encounter a Date/Time op-code set, rewind to the first op-code of the set.
        //
        AdjustDateAndTimePosition (TRUE, &TopOfScreen);

        UpdateStatusBar (INPUT_ERROR, MenuOption->ThisTag->QuestionFlags, FALSE);
        if (Difference < 0) {
          //
          // We want to goto previous MenuOption, but finally we go down.
          // it means that we hit the begining MenuOption that can be focused
          // so we simply scroll to the top
          //
          if (SavedListEntry != Menu.ForwardLink) {
            NewPos = Menu.ForwardLink;
            TopOfScreen = Menu.ForwardLink;
            //
            // clear OldSkipValue to make sure the TopOfScreeen can be calculated correctly.
            //
            OldSkipValue = 0;
            Repaint     = TRUE;
          }
        }
      } else {
        SavedMenuOption = MenuOption;
        MenuOption      = MENU_OPTION_FROM_LINK (NewPos);
        if (!IsSelectable (MenuOption)) {
          //
          // If we are at the end of the list and sitting on a text op, we need to more forward
          //
          if (HaveAnySelectableItem ()) {
            ScreenOperation = UiDown;
            ControlFlag     = CfScreenOperation;
          } else {
            ControlFlag = CfPrepareToReadKey;
          }
          break;
        }

        MenuOption = SavedMenuOption;
      }
      AdjustDateAndTimePosition (TRUE, &TopOfScreen);
      AdjustDateAndTimePosition (TRUE, &NewPos);
      break;

    case CfUiPageUp:
      ControlFlag     = CfCheckSelection;

      if (TopOfScreen->BackLink == &Menu) {
        NewLine = FALSE;
        Repaint = FALSE;
        break;
      }

      NewLine   = TRUE;
      Repaint   = TRUE;
      SavedListEntry = TopOfScreen->BackLink;
      Link           = TopOfScreen->BackLink;
      Index = BottomRow;
      while ((Index >= TopRow) && (Link != &Menu)) {
        PreviousMenuOption = MENU_OPTION_FROM_LINK (Link);
        Index = Index - PreviousMenuOption->Skip;
        Link = Link->BackLink;
      }

      //
      // Find out TopOfScreen, then set highlight option (i.e. NewPos)
      //
      TopOfScreen = Link->ForwardLink;

      Link = SavedListEntry;
      Difference = MoveToNextStatement (TRUE, &Link);
      NewPos = Link;
      if (Difference > (INTN) (BottomRow - TopRow + 1)) {
        TopOfScreen = Link;
      } else if (Difference >= 0) {
        SkipValue = (Index >= TopRow) ? 0 : (TopRow - Index - 1);
      } else if (Difference < 0) {
        NextMenuOption = MENU_OPTION_FROM_LINK (Link);
        if (IsSelectable (NextMenuOption)) {
          NewPos = Menu.ForwardLink;
          OldSkipValue = 0;
          TopOfScreen = Menu.ForwardLink;
        } else {
          NewPos = TopOfScreen;
        }
      }

      MenuOption = NULL;

      //
      // If we encounter a Date/Time op-code set, rewind to the first op-code of the set.
      // Don't do this when we are already in the first page.
      //
      AdjustDateAndTimePosition (TRUE, &TopOfScreen);
      AdjustDateAndTimePosition (TRUE, &NewPos);
      break;

    case CfUiPageDown:
      ControlFlag     = CfCheckSelection;

      if (NewPos->ForwardLink == &Menu) {
        NewLine = FALSE;
        Repaint = FALSE;
        break;
      }

      NewLine = TRUE;
      Repaint = TRUE;
      Link    = TopOfScreen;
      NextMenuOption = MENU_OPTION_FROM_LINK (Link);
      Index = TopRow;
      while ((Index <= BottomRow) && (Link->ForwardLink != &Menu)) {
        Index = Index + NextMenuOption->Skip;
        Link           = Link->ForwardLink;
        NextMenuOption = MENU_OPTION_FROM_LINK (Link);
      }

      if (Index <= BottomRow) {
        NewLine = FALSE;
        Repaint = FALSE;
        break;
      }
      //
      // Find out TopOfScreen, then set highlight option (i.e. NewPos)
      //
      TopOfScreen = Link;
      Difference = MoveToNextStatement (FALSE, &Link);
      NewPos     = Link;
      if (Difference > (INTN) (BottomRow - TopRow + 1)) {
        Index = TopRow + (UINTN) Difference;
        while (Index > BottomRow) {
          NextMenuOption = MENU_OPTION_FROM_LINK (TopOfScreen);
          Index -= NextMenuOption->Skip;
          TopOfScreen = TopOfScreen->ForwardLink;
        }
      } else if (Difference < 0) {
        NextMenuOption = MENU_OPTION_FROM_LINK (Link);
        if (IsSelectable (NextMenuOption)) {
          TopOfScreen = Link;
        } else {
          NewPos = TopOfScreen;
        }
      }

      MenuOption = NULL;

      //
      // If we encounter a Date/Time op-code set, rewind to the first op-code of the set.
      // Don't do this when we are already in the last page.
      //
      AdjustDateAndTimePosition (TRUE, &TopOfScreen);
      AdjustDateAndTimePosition (TRUE, &NewPos);
      break;

    case CfUiDown:
      ControlFlag = CfCheckSelection;
      //
      // Since the behavior of hitting the down arrow on a Date/Time op-code is intended
      // to be one that progresses to the next set of op-codes, we need to advance to the last
      // Date/Time op-code and leave the remaining logic in UiDown intact so the appropriate
      // checking can be done.  The only other logic we need to introduce is that if a Date/Time
      // op-code is the last entry in the menu, we need to rewind back to the first op-code of
      // the Date/Time op-code.
      //
      SavedListEntry = NewPos;
      DistanceValue = 0;
      AdjustDateAndTimePosition (FALSE, &NewPos);

      if (NewPos->ForwardLink != &Menu) {
        MenuOption      = MENU_OPTION_FROM_LINK (NewPos);
        NewLine         = TRUE;
        NewPos          = NewPos->ForwardLink;
        NextMenuOption  = MENU_OPTION_FROM_LINK (NewPos);
        DistanceValue  += MoveToNextStatement (FALSE, &NewPos);
        //
        // Add the skip of this selected item
        //
        NextMenuOption  = MENU_OPTION_FROM_LINK (NewPos);
        DistanceValue += NextMenuOption->Skip;
        Temp = MenuOption->Row + MenuOption->Skip + DistanceValue - 1;

        if ((NextMenuOption->ThisTag->Operand == EFI_IFR_DATE_OP ||
             NextMenuOption->ThisTag->Operand == EFI_IFR_TIME_OP)
            ) {
          Temp ++;
          DistanceValue++;
        }

        //
        // If we are going to scroll, update TopOfScreen
        //
        if (Temp > BottomRow) {
          do {
            //
            // Is the current top of screen a zero-advance op-code?
            // If so, keep moving forward till we hit a >0 advance op-code
            //
            SavedMenuOption = MENU_OPTION_FROM_LINK (TopOfScreen);

            //
            // If bottom op-code is more than one line or top op-code is more than one line
            //
            if ((DistanceValue > 1) || (MenuOption->Skip > 1)) {
              //
              // Is the bottom op-code greater than or equal in size to the top op-code?
              //
              if ((INTN) (Temp - BottomRow) >= (INTN) (SavedMenuOption->Skip - OldSkipValue)) {
                Difference      = (Temp - BottomRow) - (SavedMenuOption->Skip - OldSkipValue);
                OldSkipValue    = Difference;
                //
                // Skip the top op-code
                //
                TopOfScreen     = TopOfScreen->ForwardLink;
                SavedMenuOption = MENU_OPTION_FROM_LINK (TopOfScreen);
                while (TRUE) {
                  if (Difference == (INTN) SavedMenuOption->Skip) {
                      TopOfScreen     = TopOfScreen->ForwardLink;
                      SavedMenuOption = MENU_OPTION_FROM_LINK (TopOfScreen);
                      Difference      = 0;
                  }
                  if (Difference < (INTN) SavedMenuOption->Skip) {
                    break;
                  }
                  Difference -= (INTN) SavedMenuOption->Skip;
                  TopOfScreen     = TopOfScreen->ForwardLink;
                  SavedMenuOption = MENU_OPTION_FROM_LINK (TopOfScreen);
                }
                //
                // Since we will act on this op-code in the next routine, and increment the
                // SkipValue, set the skips to one less than what is required.
                //
                SkipValue = Difference - 1;

              } else {
                //
                // Since we will act on this op-code in the next routine, and increment the
                // SkipValue, set the skips to one less than what is required.
                //
                SkipValue = OldSkipValue + (Temp - BottomRow) - 1;
              }
            } else {
              if ((OldSkipValue + 1) >= (INTN) SavedMenuOption->Skip) {
                TopOfScreen = TopOfScreen->ForwardLink;
                break;
              } else {
                SkipValue = OldSkipValue;
              }
            }
            //
            // If the op-code at the top of the screen is more than one line, let's not skip it yet
            // Let's set a skip flag to smoothly scroll the top of the screen.
            //
            if (SavedMenuOption->Skip > 1) {
              if (SavedMenuOption == NextMenuOption) {
                SkipValue = 0;
              } else {
                SkipValue++;
              }
            } else {
              SkipValue   = 0;
            }
          } while (SavedMenuOption->Skip == 0);

          Repaint       = TRUE;
          OldSkipValue  = SkipValue;
        }

        MenuOption = MENU_OPTION_FROM_LINK (SavedListEntry);

        UpdateStatusBar (INPUT_ERROR, MenuOption->ThisTag->QuestionFlags, FALSE);

      } else {
        SavedMenuOption = MenuOption;
        MenuOption      = MENU_OPTION_FROM_LINK (NewPos);
        if (!IsSelectable (MenuOption)) {
          //
          // If we are at the end of the list and sitting on a text op, we need to more forward
          //
          if (HaveAnySelectableItem ()) {
            ScreenOperation = UiUp;
            ControlFlag     = CfScreenOperation;
          } else {
            ControlFlag = CfPrepareToReadKey;
          }
          break;
        }

        MenuOption = SavedMenuOption;
      }
      AdjustDateAndTimePosition (TRUE, &TopOfScreen);
      AdjustDateAndTimePosition (TRUE, &NewPos);
      break;

    case CfUiSave:
      ControlFlag = CfCheckSelection;

      //
      // Submit the form
      //
      Status = SubmitForm (Selection->FormSet, Selection->Form);

      if (!EFI_ERROR (Status)) {
        UpdateStatusBar (INPUT_ERROR, MenuOption->ThisTag->QuestionFlags, FALSE);
        UpdateStatusBar (NV_UPDATE_REQUIRED, MenuOption->ThisTag->QuestionFlags, FALSE);
        if (!((gDeviceManagerSetup != NULL) ||
             (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) ||
             gSecureBootMgr ||
             mBootManager)) {
          Selection->Action = UI_ACTION_REFRESH_FORMSET;
          Selection->Statement = NULL;
        }
      } else {
        do {
          CreateDialog (4, TRUE, 0, NULL, &Key, gEmptyString, gSaveFailed, gPressEnter, gEmptyString);
        } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

        Repaint = TRUE;
        NewLine = TRUE;
      }
      break;

    case CfUiSaveAndExit:
        ControlFlag = CfCheckSelection;
        Selection->Action                   = UI_HOTKEY_F10;
        break;

    case CfUiDefault:
      ControlFlag = CfCheckSelection;
      if (gDeviceManagerSetup) {
        Selection->Action = UI_HOTKEY_F9;
      } else {
        if (!Selection->FormEditable) {
          //
          // This Form is not editable, ignore the F9 (reset to default)
          //
          break;
        }

        Status = ExtractFormDefault (Selection->FormSet, Selection->Form, DefaultId);

        if (!EFI_ERROR (Status)) {
          Selection->Action = UI_ACTION_REFRESH_FORM;
          Selection->Statement = NULL;

          //
          // Show NV update flag on status bar
          //
          gNvUpdateRequired = TRUE;
        }
      }
      break;

    case CfUiNoOperation:
      ControlFlag = CfCheckSelection;
      break;

    case CfExit:
      UiFreeRefreshList ();
      UiFreeAnimationRefreshList ();

      gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
      if (mGopList != NULL) {
        gBS->FreePool (mGopList);
        mGopList = NULL;
      }
      return EFI_SUCCESS;

    case CfUiJumpMenu:
      ControlFlag = CfCheckSelection;
      Selection->Action = UI_ACTION_EXIT;
      Selection->Statement->QuestionFlags = 0;
      CurrentMenu->QuestionId = 0;
      gDeviceManagerSetup->Direction = Jump;
      break;

    case CfUiShowHelpScreen:
      ControlFlag = CfCheckSelection;
      TitleString = GetToken (STRING_TOKEN (STR_GENERAL_HELP_STRING), gHiiHandle);
      if (TitleString == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      StringPtr   = GetToken (STRING_TOKEN(STR_SETUP_DESCRIPTION_STRING), gHiiHandle);
      if (StringPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      SetupUtilityShowInformations (
        TitleString,
        StringPtr,
        &LocalScreen
        );
      Repaint = TRUE;
      NewLine = TRUE;
      SkipValue = OldSkipValue;
      ScuSafeFreePool ((VOID **)&TitleString);
      ScuSafeFreePool ((VOID **)&StringPtr);
    break;

    default:
      break;
    }
  }
}

