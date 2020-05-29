/** @file
 Setup utility related functions in SetupBrowserDxe driver

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

#include <Protocol/SetupMouse.h>
#include <Protocol/SetupUtilityBrowser.h>

BOOLEAN    mBootManager = FALSE;
BOOLEAN    mBootFromFile = FALSE;
EFI_SETUP_UTILITY_BROWSER_PROTOCOL *gDeviceManagerSetup;


UI_HOTKEY_INFO                     mHotKeyInfo[UI_HOTKEY_NUM];
EFI_SETUP_MOUSE_PROTOCOL           *gSetupMouse = NULL;
BOOLEAN                            mRefreshAnimation = FALSE;

VOID
ConvertToTextCoordinate (
  UINTN                         *X,
  UINTN                         *Y
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
  UINTN                         Columns;
  UINTN                         Rows;
  UINTN                         OffsetX;
  UINTN                         OffsetY;

  ASSERT (X != NULL && Y != NULL);

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &Columns,
                          &Rows
                          );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return ;
  }

  OffsetX = ((GraphicsOutput->Mode->Info->HorizontalResolution - Columns * EFI_GLYPH_WIDTH)) >> 1;
  OffsetY = ((GraphicsOutput->Mode->Info->VerticalResolution   - Rows * EFI_GLYPH_HEIGHT))   >> 1;

  *X = (*X - OffsetX) / EFI_GLYPH_WIDTH;
  *Y = (*Y - OffsetY) / EFI_GLYPH_HEIGHT;
}




/**
 Save hotkey information into mHotKeyInfo

 @param[in] Index                   Index of mHotKeyInfo
 @param[in] ScanCode                Corresponding scan code of hotkey
 @param[in] UnicodeChar             Corresponding unicode char of hotkey
 @param[in] Y                       Y axis value of hotkey in SCU
 @param[in] StartX                  Start X axis value of hotkey in SCU
 @param[in] EndX                    End X axis value of hotkey in SCU
**/
VOID
SaveHotkeyInfo (
  IN  UINTN                             Index,
  IN  UINT16                            ScanCode,
  IN  CHAR16                            UnicodeChar,
  IN  UINTN                             Y,
  IN  UINTN                             StartX,
  IN  UINTN                             EndX
  )
{
  mHotKeyInfo[Index].Key.ScanCode    = ScanCode;
  mHotKeyInfo[Index].Key.UnicodeChar = UnicodeChar;
  mHotKeyInfo[Index].Y               = Y;
  mHotKeyInfo[Index].StartX          = StartX;
  mHotKeyInfo[Index].EndX            = EndX;
}

/**
 Display title and core version string.

 @param[in] LocalScreen         Screen Descriptor for Setup Utility Form

 @retval EFI_SUCCESS            The function completed successfully.
**/
STATIC
EFI_STATUS
SetupDisplayTitle (
  IN EFI_SCREEN_DESCRIPTOR                       *LocalScreen
  )
{
  CHAR16                 *SetupUtilityTitle;
  CHAR16                 *CoreVersionString;

  SetupUtilityTitle    = GetToken(STRING_TOKEN(SCU_TITLE_STRING), gHiiHandle);
  CoreVersionString    = GetToken(STRING_TOKEN(SCU_CORE_VERSION_STRING), gHiiHandle);
  if (SetupUtilityTitle == NULL || CoreVersionString == NULL) {
    return EFI_NOT_FOUND;
  }

  ClearLines (LocalScreen->LeftColumn,
              LocalScreen->RightColumn,
              LocalScreen->TopRow,
              LocalScreen->TopRow,
              SU_TITLE_BACKGROUND
             );

  gST->ConOut->SetAttribute (gST->ConOut, SU_TITLE_TEXT | SU_TITLE_BACKGROUND);

  PrintStringAt (
                 (LocalScreen->RightColumn - GetStringWidth(SetupUtilityTitle)/2)/2,
                 LocalScreen->TopRow,
                 SetupUtilityTitle
                );

  PrintStringAt (
    LocalScreen->RightColumn - 10,
    LocalScreen->TopRow,
    CoreVersionString
    );

  ScuSafeFreePool ((VOID **)&SetupUtilityTitle);
  ScuSafeFreePool ((VOID **)&CoreVersionString);

  return EFI_SUCCESS;
}

/**
 Calculate the distance between two point

 @param[in] X1        X of first point
 @param[in] Y1        Y of first point
 @param[in] X2        X of second point
 @param[in] Y2        Y of second point


 @retval UINTN the square of distance between two point
**/
UINTN
CalcDistance (
  IN UINTN X1,
  IN UINTN Y1,
  IN UINTN X2,
  IN UINTN Y2
  )
/*++

Routine Description:

Arguments:
  X1         - X of first point
  Y1         - Y of first point
  X2         - X of second point
  Y2         - Y of second point

Returns:
  The square of distance between two point

--*/
{
  return (UINTN)((((INTN)X1 - (INTN)X2) * ((INTN)X1 - (INTN)X2)) + (((INTN)Y1 - (INTN)Y2) * ((INTN)Y1 - (INTN)Y2)));
}


/**
 SetupDisplayTitleAndHotKeyBar

 @param[in] LocalScreen        Screen Descriptor for Setup Utility Form
 @param[in] MenuWidth

 @retval EFI_SUCCESS           The function completed successfully.
**/
EFI_STATUS
SetupDisplayTitleAndHotKeyBar (
  IN EFI_SCREEN_DESCRIPTOR                       *LocalScreen
  )
{
  CHAR16                                         *StringPtr;
  CHAR16                                         *StringPtr2;
  UINTN                                          X;
  UINTN                                          Y1,Y2;
  UINTN                                          Index;
  UINTN                                          AdjustColumn;

  ASSERT (gDeviceManagerSetup != NULL);
  Index = 0;

  ClearLines(
    LocalScreen->LeftColumn,
    LocalScreen->RightColumn - (!gDeviceManagerSetup->ScreenLastCharIsSet ? 0 : 1),
    LocalScreen->BottomRow - 1,
    LocalScreen->BottomRow - 1,
    SU_TITLE_BACKGROUND
    );
  gDeviceManagerSetup->ScreenLastCharIsSet = TRUE;
  ClearLines(
    LocalScreen->LeftColumn,
    LocalScreen->RightColumn,
    LocalScreen->BottomRow - 2,
    LocalScreen->BottomRow - 2,
    SU_TITLE_BACKGROUND
    );
  SetupDisplayTitle (LocalScreen);

  Y1 = LocalScreen->BottomRow - 1;
  Y2 = LocalScreen->BottomRow - 2;
  gST->ConOut->SetAttribute (gST->ConOut, SU_TEXT_HIGHLIGHT | SU_TITLE_BACKGROUND);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_HELP_KEY), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_ESC_KEY), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X  = LocalScreen->LeftColumn;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  SaveHotkeyInfo (Index++, SCAN_F1 , CHAR_NULL, Y2, X, X + StrLen (StringPtr) - 1);
  SaveHotkeyInfo (Index++, SCAN_ESC, CHAR_NULL, Y1, X, X + StrLen (StringPtr2) - 1);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  AdjustColumn = (LocalScreen->RightColumn - 80) / 4;
  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_SELECT_KEY), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_MENU_KEY), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 12 + AdjustColumn;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  SaveHotkeyInfo (Index++, SCAN_UP   , CHAR_NULL, Y2, X                          , X + StrLen (StringPtr) / 2 - 1);
  SaveHotkeyInfo (Index++, SCAN_DOWN , CHAR_NULL, Y2, X + StrLen (StringPtr) / 2 , X + StrLen (StringPtr) - 1);
  SaveHotkeyInfo (Index++, SCAN_LEFT , CHAR_NULL, Y1, X                          , X + StrLen (StringPtr2) / 2 - 1);
  SaveHotkeyInfo (Index++, SCAN_RIGHT, CHAR_NULL, Y1, X + StrLen (StringPtr2) / 2, X + StrLen (StringPtr2) - 1);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_CHANGE_KEY), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_ENTER_KEY), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 30 + AdjustColumn * 2;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  SaveHotkeyInfo (Index++, SCAN_F5, CHAR_NULL, Y2, X                                   , X + (StrLen (StringPtr) - 1) / 2 - 1);
  SaveHotkeyInfo (Index++, SCAN_F6, CHAR_NULL, Y2, X + (StrLen (StringPtr) - 1) / 2 + 1, X + StrLen (StringPtr) - 1);
  SaveHotkeyInfo (Index++, SCAN_NULL, CHAR_CARRIAGE_RETURN, Y1, X, X + StrLen (StringPtr2) - 1);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_DEFAULT_KEY), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_SAVE_KEY), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 58 + AdjustColumn * 3;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  SaveHotkeyInfo (Index++, SCAN_F9 , CHAR_NULL, Y2, X, X + StrLen (StringPtr) - 1);
  SaveHotkeyInfo (Index++, SCAN_F10, CHAR_NULL, Y1, X, X + StrLen (StringPtr2) - 1);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  gST->ConOut->SetAttribute (gST->ConOut, SU_TITLE_TEXT | SU_TITLE_BACKGROUND);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_HELP_DSC), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_ESC_DSC), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 4;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_SELECT_DSC), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_MENU_DSC), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 15 + AdjustColumn;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_CHANGE_DSC), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_ENTER_DSC), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 36 + AdjustColumn * 2;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);

  StringPtr = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_DEFAULT_DSC), gHiiHandle);
  StringPtr2 = GetToken(STRING_TOKEN(SCU_HOTKEY_BAR_SAVE_DSC), gHiiHandle);
  if (StringPtr == NULL || StringPtr2 == NULL) {
    return EFI_NOT_FOUND;
  }
  X = LocalScreen->LeftColumn + 62 + AdjustColumn * 3;
  PrintStringAt (X, Y2, StringPtr);
  PrintStringAt (X, Y1, StringPtr2);
  gBS->FreePool(StringPtr);
  gBS->FreePool(StringPtr2);
  return EFI_SUCCESS;
}

BOOLEAN
IsValidYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year < PcdGet16 (PcdRealTimeClockYearMin) || Time->Year > PcdGet16 (PcdRealTimeClockYearMax)) {
    return FALSE;
  }

  return TRUE;
}

/**
 Wait for a given event to fire, or for an optional timeout to expire.

 @param[in]  RefreshInterval    Menu refresh interval (in seconds).
 @param[out] Key                The key which is pressed by user.
 @param[out] MouseX             The resolution of mouse pointer device on the x-axis
 @param[out] MouseY             The resolution of mouse pointer device on the y-axis

 @retval TRUE                   User pressed keyboard
 @retval FALSE                  User doesn't pressed keyboard
**/
BOOLEAN
UiInputDevice (
  IN   UINT8                             RefreshInterval,
  OUT  EFI_KEY_DATA                      *EfiKeyData,
  OUT  UINTN                             *MouseX,
  OUT  UINTN                             *MouseY
  )
{
  BOOLEAN                               LeftButton, RightButton;
  STATIC BOOLEAN                        SaveLeftButton = FALSE;
  STATIC BOOLEAN                        SaveRightButton = FALSE;
  BOOLEAN                               KeyBoard;
  UINTN                                 X, Y;
  STATIC UINTN                          OldX = 65536, OldY = 65536;
  EFI_STATUS                            Status;
  EFI_TIME                              OldTime;
  EFI_TIME                              NewTime;
  UINTN                                 TopRow;
  UINTN                                 BottomRow;
  EFI_INPUT_KEY                         *Key;
  BOOLEAN                               RefreshTimeOut;
  EFI_EVENT                             TimerEvent;
  UINTN                                 DownDistance;
  UINTN                                 UpDistance;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInEx;

  LeftButton  = FALSE;
  RightButton = FALSE;
  X = Y = 0;
  KeyBoard = TRUE;
  Status = EFI_NOT_READY;

  OldTime.Second = 60;
  TopRow = gScreenDimensions.TopRow + 3;
  BottomRow = gScreenDimensions.BottomRow - 4;
  RefreshTimeOut = FALSE;
  TimerEvent     = NULL;
  ZeroMem (EfiKeyData, sizeof(EFI_KEY_DATA));
  Key = &EfiKeyData->Key;
  SimpleTextInEx = GetimpleTextInExInstance ();
  do {
    if (RefreshInterval != 0) {
      //
      // Refresh all Questions with refresh interval, besides time and date
      //
      if (TimerEvent == NULL) {
        RefreshTimeOut = FALSE;
        Status = gBS->CreateEvent (
                        EVT_TIMER | EVT_NOTIFY_SIGNAL,
                        TPL_CALLBACK,
                        RefreshTimeOutNotify,
                        &RefreshTimeOut,
                        &TimerEvent
                        );
        if (!EFI_ERROR (Status)) {
          gBS->SetTimer (
                 TimerEvent,
                 TimerRelative,
                 RefreshInterval * ONE_SECOND
                 );
         }
      }

      if (RefreshTimeOut && TimerEvent != NULL) {
        gBS->CloseEvent (TimerEvent);
        TimerEvent = NULL;
        if (RefreshForm (FALSE) != EFI_TIMEOUT) {
          break;
        }
      }

      //
      // Refresh Time and Date
      //
      Status = gRT->GetTime (&NewTime, NULL);
      if (EFI_ERROR (Status) || !IsValidYear (&NewTime)) {
        NewTime.Second   = PcdGet8 (PcdRealTimeClockInitSecond);;
        NewTime.Minute   = PcdGet8 (PcdRealTimeClockInitMinute);
        NewTime.Hour     = PcdGet8 (PcdRealTimeClockInitHour);
        NewTime.Day      = PcdGet8 (PcdRealTimeClockInitDay);
        NewTime.Month    = PcdGet8 (PcdRealTimeClockInitMonth);
        NewTime.Year     = PcdGet16 (PcdRealTimeClockInitYear);
        NewTime.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
        NewTime.Daylight = 0;
        //
        // If Time Error occur, write defalut time into CMOS
        //
        Status = gRT->SetTime(&NewTime);
      }
      if (NewTime.Second != OldTime.Second)
      {
        OldTime.Second = NewTime.Second;
        Status = RefreshForm(TRUE);
      }
    }

    if (SimpleTextInEx != NULL) {
      Status = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, EfiKeyData);
    } else {
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
    }

    if (gAnimationPackage != NULL && mRefreshAnimation) {
      UiRefreshAnimationList ();
    }

    if (EFI_ERROR (Status)) {
      if (gSetupMouse == NULL)
        continue;

      Status = gSetupMouse->QueryState(gSetupMouse, &X, &Y, &LeftButton, &RightButton);
      if (EFI_ERROR (Status)) {
        continue;
      }

      Status = EFI_NOT_READY;

      if (SaveLeftButton != LeftButton || SaveRightButton != RightButton) {
        SaveLeftButton = LeftButton;
        SaveRightButton = RightButton;
      } else {
        LeftButton = FALSE;
        RightButton = FALSE;
      }
      if (gGraphicsEnabled) {

        if (LeftButton == TRUE) {
          Key->UnicodeChar = CHAR_CARRIAGE_RETURN;
          Status = EFI_SUCCESS;
          KeyBoard = FALSE;
          break;
        } else if (RightButton == TRUE) {
          continue;
        } else {
          if (X == OldX && Y == OldY) {
            continue;
          }
          OldX = X;
          OldY = Y;
          Key->ScanCode    = SCAN_NULL;
          Key->UnicodeChar = CHAR_NULL;
          Status           = EFI_SUCCESS;
          KeyBoard         = FALSE;
        }
        break;
      } else {
        ConvertToTextCoordinate (&X, &Y);
      }

      if (LeftButton == TRUE) {
        DownDistance = CalcDistance (X, Y, gDownArrowX, gDownArrowY);
        UpDistance   = CalcDistance (X, Y, gUpArrowX, gUpArrowY);
        if (gDownArrow && DownDistance  <= 4) {
          if (gDeviceManagerSetup != NULL && gDeviceManagerSetup->SetupMouseBehavior == SETUP_MOUSE_LINE_UP_DOWN) {
            Key->ScanCode = SCAN_DOWN;
          } else {
            Key->ScanCode = SCAN_PAGE_DOWN;
          }
          Key->UnicodeChar = CHAR_NULL;
          KeyBoard = TRUE;
          Status = EFI_SUCCESS;
        } else if (gUpArrow && UpDistance <= 4) {
          if (gDeviceManagerSetup != NULL && gDeviceManagerSetup->SetupMouseBehavior == SETUP_MOUSE_LINE_UP_DOWN) {
            Key->ScanCode = SCAN_UP;
          } else {
            Key->ScanCode = SCAN_PAGE_UP;
          }
          Key->UnicodeChar = CHAR_NULL;
          KeyBoard = TRUE;
          Status = EFI_SUCCESS;
        } else {
          Key->UnicodeChar = CHAR_CARRIAGE_RETURN;
          Status = EFI_SUCCESS;
          KeyBoard = FALSE;
        }

      } else if (RightButton == TRUE) {
        Key->UnicodeChar = CHAR_NULL;
        Key->ScanCode = SCAN_ESC;
        Status = EFI_SUCCESS;
        KeyBoard = FALSE;
      } else {
        continue;
      }
    }

  } while (Status != EFI_SUCCESS);
  if (TimerEvent != NULL) {
    gBS->CloseEvent (TimerEvent);
  }

  if (MouseX != NULL)
    *MouseX = X;
  if (MouseY != NULL)
    *MouseY = Y;

  return KeyBoard;
}

/**
 Wait for a given event to fire, or for an optional timeout to expire.

 @param[in] LocalSetupMouse    Pointer to setup mouse protocol instance.
 @param[in] Key                The key which is pressed by user.
 @param[in] MouseX             The resolution of mouse pointer device on the x-axis
 @param[in] MouseY             The resolution of mouse pointer device on the y-axis

 @retval TRUE                  User pressed by keyboard
 @retval FALSE                 User pressed by Mouse
**/
BOOLEAN
InputDevice (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *LocalSetupMouse,
  OUT EFI_KEY_DATA                      *EfiKeyData,
  IN  UINTN                             *MouseX,
  IN  UINTN                             *MouseY
  )
{
  BOOLEAN                               LeftButton, RightButton;
  STATIC BOOLEAN                        SaveLeftButton = FALSE;
  STATIC BOOLEAN                        SaveRightButton = FALSE;
  BOOLEAN                               KeyBoard;
  UINTN                                 X, Y;
  EFI_STATUS                            Status;
  EFI_INPUT_KEY                         *Key;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInEx;

  X = 0;
  Y = 0;
  KeyBoard = TRUE;
  Status = EFI_NOT_READY;
  LeftButton  = FALSE;
  RightButton = FALSE;
  ZeroMem (EfiKeyData, sizeof(EFI_KEY_DATA));
  Key = &EfiKeyData->Key;
  SimpleTextInEx = GetimpleTextInExInstance ();

  do {
    if (SimpleTextInEx != NULL) {
      Status = SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, EfiKeyData);
    } else {
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
    }
    if (EFI_ERROR (Status)) {
      if (LocalSetupMouse == NULL)
        continue;

      Status = LocalSetupMouse->QueryState(LocalSetupMouse, &X, &Y, &LeftButton, &RightButton);
      if (EFI_ERROR (Status)) {
        continue;
      }
      ConvertToTextCoordinate (&X, &Y);
      Status = EFI_NOT_READY;

      if (SaveLeftButton != LeftButton || SaveRightButton != RightButton) {
        SaveLeftButton = LeftButton;
        SaveRightButton = RightButton;
      } else {
        LeftButton = FALSE;
        RightButton = FALSE;
      }
      if (LeftButton == TRUE) {
        Key->UnicodeChar = CHAR_CARRIAGE_RETURN;
        Status = EFI_SUCCESS;
        KeyBoard = FALSE;
      } else if (RightButton == TRUE) {
        Key->UnicodeChar = CHAR_NULL;
        Key->ScanCode = SCAN_ESC;
        Status = EFI_SUCCESS;
        KeyBoard = FALSE;
      } else {
        continue;
      }
    }

  } while (Status != EFI_SUCCESS);

  if (MouseX != NULL)
    *MouseX = X;
  if (MouseY != NULL)
    *MouseY = Y;

  return KeyBoard;
}

/**
 Get main menu selection from mouse input

 @param[in] Key                The input key
 @param[in] X                  The resolution of mouse pointer device on the x-axis
 @param[in] Y                  The resolution of mouse pointer device on the y-axis

 @retval EFI_SUCCESS           A main menu was successfully selected
 @retval Other                 No main menu was selected
**/
EFI_STATUS
ParentMenu (
  IN  EFI_INPUT_KEY                     *Key,
  IN  UINTN                             X,
  IN  UINTN                             Y
  )
{
  UINTN                                 StartX;
  UINTN                                 StringLength;
  UI_MENU_OPTION                        *MenuOption;
  EFI_STATUS                            Status;
  UINTN                                 Index;

  MenuOption = NULL;
  StringLength = 0;
  StartX = 2;

  if (Key->ScanCode == SCAN_ESC) {
    return EFI_NOT_FOUND;
  }

  if (Key->UnicodeChar != CHAR_CARRIAGE_RETURN) {
    return EFI_NOT_FOUND;
  }

  if (Y != 1) {
    return EFI_NOT_FOUND;
  }
  if (!gDeviceManagerSetup->AtRoot) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  for (Index = gDeviceManagerSetup->MenuItemCount; Index > 0 ; Index--) {
    StringLength = (GetStringWidth(gDeviceManagerSetup->MenuList[Index - 1].String) / 2);
    if ((X >= StartX) && (X < (StartX + StringLength - 1))) {
      gDeviceManagerSetup->CurRoot   = (UINT8) (Index - 1);
      gDeviceManagerSetup->Direction = Jump;
      Status = EFI_SUCCESS;
      break;
    }

    StartX += (StringLength + 1);
  }

  if (EFI_ERROR(Status)) {
    Key->ScanCode = 0;
    Key->UnicodeChar = 0;
  }
  return Status;
}

/**
 Get menu option selection from mouse input

 @param[in] Key                     The input key
 @param[in] X                       The resolution of mouse pointer device on the x-axis
 @param[in] Y                       The resolution of mouse pointer device on the y-axis
 @param[in] TopOfScreen             The top menu option in current meny

 @return A pointer to selected menu option or NULL if not found.
**/
LIST_ENTRY *
ChildMenu (
  IN     EFI_INPUT_KEY                      *Key,
  IN     UINTN                              X,
  IN     UINTN                              Y,
  IN     LIST_ENTRY                         *TopOfScreen
  )
{
  UINTN                                 StringLength, Start;
  LIST_ENTRY                            *NewPos, *Temp;
  UI_MENU_OPTION                        *MenuOption;
  CHAR16                                *OptionString;
  UINTN                                 SpaceCharCount;
  KEYBOARD_ATTRIBUTES                   KeyboardAttributes;

  MenuOption = NULL;
  StringLength = 0;
  NewPos = NULL;
  OptionString = NULL;

  if (Key->ScanCode == SCAN_ESC) {
    return NULL;
  }

  if (Key->UnicodeChar != CHAR_CARRIAGE_RETURN) {
    return NULL;
  }

  if (Y < 3) {
    Key->UnicodeChar = 0;
    return NULL;
  }

  for (Temp = TopOfScreen; Temp != &Menu; Temp = Temp->ForwardLink) {
    MenuOption = CR(Temp, UI_MENU_OPTION, Link, UI_MENU_OPTION_SIGNATURE);

    if ((MenuOption->GrayOut) ||
        (MenuOption->ThisTag->Operand == EFI_IFR_SUBTITLE_OP)) {
      MenuOption = NULL;
      continue;
    }

    StringLength = gPromptBlockWidth + gOptionBlockWidth;
    Start = MenuOption->Col;

    if (MenuOption->Skip > 0) {
      if (Y < MenuOption->Row || Y > MenuOption->Row + MenuOption->Skip - 1) {
        MenuOption = NULL;
        continue;
      }
    } else {
      if (Y != MenuOption->Row) {
        MenuOption = NULL;
        continue;
      }
    }

    if ((X >= Start) && (X < (Start + StringLength))) {
      NewPos = Temp;

      if ((MenuOption->ThisTag->Operand == EFI_IFR_TIME_OP) ||
          (MenuOption->ThisTag->Operand == EFI_IFR_DATE_OP) ||
          (MenuOption->ThisTag->Operand == EFI_IFR_NUMERIC_OP)) {

        ProcessOptions(gCurrentSelection, MenuOption, FALSE, &OptionString);

        if (OptionString != NULL) {
          SpaceCharCount = 0;
          while (OptionString[SpaceCharCount] == L' ') {
            SpaceCharCount++;
          }
          StringLength = StrLen(OptionString + SpaceCharCount);
          ScuSafeFreePool((VOID **)&OptionString);

          //
          //time-hour, date-month and S5 wakeup number of days
          //
          if ((X >= MenuOption->OptCol ) && (X <= (MenuOption->OptCol + StringLength - 1))) {
            if (gSetupMouse != NULL) {
              gSetupMouse->GetKeyboardAttributes (gSetupMouse, &KeyboardAttributes);
              if (!KeyboardAttributes.IsStart) {
                //
                // Draw Keyboard
                //
                gSetupMouse->StartKeyboard(gSetupMouse, 2, gScreenDimensions.BottomRow);
              }
            }
            break;
          }
        }
        NewPos = NULL;
        MenuOption = NULL;
        continue;
      }
      break;
    } else {
      MenuOption = NULL;
      NewPos = NULL;
    }
  }

  if (MenuOption == NULL) {
    ZeroMem(Key, sizeof (EFI_INPUT_KEY));
  }
  if (NewPos==NULL) {
    Key->UnicodeChar = 0;
  }
  return NewPos;
}

/**
 Get hotkey selection from mouse input and output corresponding key value

 @param[out] Key               Output corresponding key value
 @param[in]  X                 The resolution of mouse pointer device on the x-axis
 @param[in]  Y                 The resolution of mouse pointer device on the y-axis

 @retval EFI_SUCCESS           One hotkey was successfully selected
 @retval EFI_NOT_FOUND         No hotkey was selected
**/
EFI_STATUS
HotkeyMenu (
  OUT  EFI_INPUT_KEY                     *Key,
  IN   UINTN                             X,
  IN   UINTN                             Y
  )
{
  UINTN                                 Index;

  if (Key->ScanCode == SCAN_ESC ||
      Key->UnicodeChar != CHAR_CARRIAGE_RETURN ||
      (Y != gScreenDimensions.BottomRow - 2 && Y != gScreenDimensions.BottomRow - 1)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0;
       Index < sizeof (mHotKeyInfo) / sizeof (mHotKeyInfo[0]);
       Index++) {
    if (Y == mHotKeyInfo[Index].Y &&
        X >= mHotKeyInfo[Index].StartX &&
        X <= mHotKeyInfo[Index].EndX) {
      Key->ScanCode    = mHotKeyInfo[Index].Key.ScanCode;
      Key->UnicodeChar = mHotKeyInfo[Index].Key.UnicodeChar;
      return EFI_SUCCESS;
    }
  }

  Key->ScanCode    = 0;
  Key->UnicodeChar = 0;

  return EFI_NOT_FOUND;
}

/**
 Display informations by page

 @param[in] TitleString             Dialog Title string
 @param[in] InfoStrings             Information strings structure pointer
 @param[in] LocalScreen             Screen dimension

 @retval EFI_SUCCESS
**/
EFI_STATUS
SetupUtilityShowInformations (
  IN   CHAR16                *TitleString,
  IN   CHAR16                *InfoStrings,
  IN   EFI_SCREEN_DESCRIPTOR *LocalScreen
  )
{

  DisplayEmptyPage (LocalScreen);
  DisplayInfoPage (
    TitleString,
    InfoStrings,
    LocalScreen
    );

  return EFI_SUCCESS;
}

EFI_STATUS
ProcessStrToFormatted (
  IN  CHAR16             *Str,
  IN  UINTN              BlockWidth,
  OUT CHAR16             **FormattedStr,
  OUT UINTN              *FormattedStrCount,
  OUT UINTN              *FormattedStrOneLineLen
  );

/**
 Display informations by page

 @param[in] TitleString             Dialog Title string
 @param[in] InfoString              Information strings structure pointer
 @param[in] LocalScreen             Screen dimension

 @retval EFI_SUCCESS
**/
EFI_STATUS
DisplayInfoPage (
  IN CHAR16                                   *TitleString,
  IN CHAR16                                   *InfoString,
  IN EFI_SCREEN_DESCRIPTOR                    *LocalScreen
  )
{
  UINTN                                       StartX, StartY;
  UINTN                                       EndX, EndY;
  UINTN                                       StringIndex;
  UINTN                                       CurrentPage;
  UINTN                                       NumOfShowInfoLines;
  EFI_KEY_DATA                                EfiKeyData;
  UINTN                                       X, Y;
  UINTN                                       LastStrings,StringsCnt;
  CHAR16                                      *InfoStr;
  UINTN                                       InfoStrOneLineLen;
  UINTN                                       NumOfInfoStrings;
  UINTN                                       PageCount;

  StartX = LocalScreen->LeftColumn;
  EndX = LocalScreen->RightColumn - 1;

  StartY = LocalScreen->TopRow + EVENT_LOG_FRAME_SPACE;
  EndY = LocalScreen->BottomRow - EVENT_LOG_FRAME_SPACE;

  StringIndex = 0;
  CurrentPage = 0;

  //
  //Show Titile String
  //
  PrintAt ((EndX - StartX - StrLen (TitleString))/2, StartY + 1, TitleString);


  //
  // show content
  //

  StartX += 1;
  StartY += 3;
  gDownArrowX = EndX;
  gDownArrowY = EndY - 5;
  gUpArrowX = EndX;
  gUpArrowY = StartY - 1;

  NumOfShowInfoLines = LocalScreen->BottomRow - 12;
  ProcessStrToFormatted (InfoString, EndX - StartX, &InfoStr, &NumOfInfoStrings, &InfoStrOneLineLen);

  if ((NumOfInfoStrings > (NumOfShowInfoLines)) && (NumOfInfoStrings % NumOfShowInfoLines) != 0) {
    PageCount = (NumOfInfoStrings / NumOfShowInfoLines) + 1;
  } else {
    PageCount = (NumOfInfoStrings / NumOfShowInfoLines);
  }

  LastStrings        = NumOfInfoStrings % NumOfShowInfoLines;


  while (TRUE) {
    if (((CurrentPage == (PageCount - 1)) && (LastStrings != 0)) || (PageCount == 0)) {
      StringsCnt = LastStrings;
    } else {
      StringsCnt = NumOfShowInfoLines;
    }
    for (StringIndex = 0 ; StringIndex < StringsCnt ; StringIndex++) {

      PrintAt (
        StartX,
        StartY + StringIndex,
        L"%s",
        &InfoStr[((CurrentPage * NumOfShowInfoLines) + StringIndex) * InfoStrOneLineLen]
        );
    }

    gDownArrow = TRUE;
    gUpArrow   = TRUE;
    gST->ConOut->SetAttribute (gST->ConOut, SU_FRAME_LINE | EFI_BACKGROUND_LIGHTGRAY);
    PrintCharAt (gDownArrowX, gDownArrowY, BOXDRAW_VERTICAL_LEFT);
    PrintCharAt (gUpArrowX,   gUpArrowY,   BOXDRAW_VERTICAL_LEFT);

    if (PageCount != 0) {
      if (CurrentPage != (PageCount - 1)) {
        gST->ConOut->SetAttribute (gST->ConOut, ARROW_TEXT | ARROW_BACKGROUND);
        PrintCharAt (gDownArrowX, gDownArrowY, ARROW_DOWN);
      }
      if (CurrentPage != 0) {
        gST->ConOut->SetAttribute (gST->ConOut, ARROW_TEXT | ARROW_BACKGROUND);
        PrintCharAt (gUpArrowX,   gUpArrowY,   ARROW_UP);
      }
      gST->ConOut->SetAttribute (gST->ConOut, SU_FRAME_LINE | EFI_BACKGROUND_LIGHTGRAY);
    }

    UiInputDevice (0, &EfiKeyData, &X, &Y);

    if (EfiKeyData.Key.ScanCode == SCAN_ESC || EfiKeyData.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      break;
    }

    if (EfiKeyData.Key.ScanCode == SCAN_PAGE_UP) {
      if ((PageCount != 0) && (CurrentPage != 0)){
        CurrentPage --;
        ClearLines (StartX, EndX, StartY, EndY - 6, SU_FRAME_LINE | EFI_BACKGROUND_LIGHTGRAY);

      }
      continue;
    }
    if (EfiKeyData.Key.ScanCode == SCAN_PAGE_DOWN) {
      if ((PageCount != 0) && (CurrentPage < (PageCount - 1))) {
        CurrentPage ++;
        ClearLines (StartX, EndX, StartY, EndY - 6, SU_FRAME_LINE | EFI_BACKGROUND_LIGHTGRAY);
      }
      continue;
    }

  }

  ScuSafeFreePool ((VOID **)&InfoStr);

  return EFI_SUCCESS;
}

/**
 Display empty page

 @param[in] LocalScreen        Screen Descriptor for Setup Utility Form

 @retval EFI_SUCCESS           Show empty page successfully.
**/
EFI_STATUS
DisplayEmptyPage (
  IN EFI_SCREEN_DESCRIPTOR                    *LocalScreen
  )
{
  UINTN                                       Index;
  CHAR16                                      Character;
  CHAR16                                      *Buffer;

  UINTN                                       StartX, StartY;
  UINTN                                       EndX, EndY;
  CHAR16                                      *NewString;

  NewString = NULL;

  StartX = LocalScreen->LeftColumn;
  EndX = LocalScreen->RightColumn - 1;

  StartY = LocalScreen->TopRow + EVENT_LOG_FRAME_SPACE;
  EndY = LocalScreen->BottomRow - EVENT_LOG_FRAME_SPACE - 1;

  Buffer = AllocateZeroPool (0x200);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // clean screen
  //
  ClearLines (StartX, EndX, StartY,EndY, SU_MENU_TEXT | EFI_BACKGROUND_LIGHTGRAY);

  //
  // set frame line and help string color
  //
  gST->ConOut->SetAttribute (gST->ConOut, SU_FRAME_LINE | EFI_BACKGROUND_LIGHTGRAY);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = 0; Index <= (EndX - StartX - 2); Index++) {
    Buffer[Index] = Character;
  }

  //
  // Print Top & Buttom Line:
  // +------------------------------------------------------------------------------+
  //
  Character = BOXDRAW_DOWN_RIGHT;
  PrintAt (StartX, StartY, L"%c", Character);
  Print (L"%s", Buffer);

  Character = BOXDRAW_DOWN_LEFT;
  Print (L"%c", Character);

  Character = BOXDRAW_UP_RIGHT;
  PrintAt (StartX, EndY, L"%c", Character);
  Print (L"%s", Buffer);

  Character = BOXDRAW_UP_LEFT;
  Print (L"%c", Character);

  //
  // Print Veritcal Line:
  // |                                                                              |
  // |                                                                              |
  // |                                                                              |
  //

  Character = BOXDRAW_VERTICAL;
  for (Index = 0; Index <= (EndY - StartY - 2); Index++) {
    PrintAt (StartX, StartY + 1 + Index, L"%c", Character);
    PrintAt (EndX, StartY + 1 + Index, L"%c", Character);
  }

  //
  // Print button information & Top Information:
  // +------------------------------------------------------------------------------+
  // |  Push Enter/ESC  - Leave                                                       |
  // |  Push PageUp     - previous Page                                               |
  // |  Push PageDown   - Next Page                                                   |
  //
  Character = BOXDRAW_HORIZONTAL;

  for (Index = 0; Index <= (EndX - StartX - 2); Index++) {
    Buffer[Index] = Character;
  }

  Character = BOXDRAW_VERTICAL_RIGHT;
  PrintAt (StartX, StartY + 2, L"%c", Character);
  Print (L"%s", Buffer);

  Character = BOXDRAW_VERTICAL_LEFT;
  Print (L"%c", Character);

  Character = BOXDRAW_VERTICAL_RIGHT;
  PrintAt (StartX, EndY - 4, L"%c", Character);
  Print (L"%s", Buffer);
  gBS->FreePool(Buffer);

  Character = BOXDRAW_VERTICAL_LEFT;
  Print (L"%c", Character);

  NewString = GetToken(
                STRING_TOKEN (STR_EVENT_LOG_PUSH),
                gHiiHandle
                );
  if (NewString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PrintAt (StartX + 1, EndY - 3, L"  %s", NewString);
  gBS->FreePool(NewString);

  NewString = GetToken(
                STRING_TOKEN (STR_EVENT_LOG_PUSH_PAGEUP),
                gHiiHandle
                );
  if (NewString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PrintAt (StartX + 1, EndY - 2, L"  %s", NewString);
  gBS->FreePool(NewString);

  NewString = GetToken(
                STRING_TOKEN (STR_EVENT_LOG_PUSH_PAGEDOWN),
                gHiiHandle
                );
  if (NewString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PrintAt (StartX + 1, EndY - 1, L"  %s", NewString);
  gBS->FreePool (NewString);

  return EFI_SUCCESS;
}

/**
 Print border line

 @param[in] LocalScreen        Screen Descriptor for Setup Utility Form
 @param[in] MenuWidth

 @retval EFI_SUCCESS           Print border line successfully.
**/
EFI_STATUS
SetupUtilityPrintBorderLine (
  IN EFI_SCREEN_DESCRIPTOR                       *LocalScreen,
  IN UINTN                                       MenuWidth
  )
{
  UINTN                                     Index;

  //
  // Print  border line
  // +-------------------------------+-----------+
  // |                               |           |
  // +-------------------------------+-----------+
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLUE, EFI_LIGHTGRAY));

  PrintCharAt (LocalScreen->LeftColumn,      LocalScreen->TopRow + 2, BOXDRAW_DOWN_RIGHT);
  PrintCharAt (MenuWidth,                    LocalScreen->TopRow + 2, BOXDRAW_DOWN_HORIZONTAL);
  PrintCharAt (LocalScreen->RightColumn - 1, LocalScreen->TopRow + 2, BOXDRAW_DOWN_LEFT);

  PrintCharAt (LocalScreen->LeftColumn,      LocalScreen->BottomRow - 3, BOXDRAW_UP_RIGHT);
  PrintCharAt (MenuWidth,                    LocalScreen->BottomRow - 3, BOXDRAW_UP_HORIZONTAL);
  PrintCharAt (LocalScreen->RightColumn - 1, LocalScreen->BottomRow - 3, BOXDRAW_UP_LEFT);

  DrawLines (1, MenuWidth, LocalScreen->TopRow + 2,    LocalScreen->TopRow + 2,    BOXDRAW_HORIZONTAL);
  DrawLines (1, MenuWidth, LocalScreen->BottomRow - 3, LocalScreen->BottomRow - 3, BOXDRAW_HORIZONTAL);

  DrawLines (MenuWidth + 1, LocalScreen->RightColumn - 1, LocalScreen->TopRow + 2, LocalScreen->TopRow + 2, BOXDRAW_HORIZONTAL);
  DrawLines (MenuWidth + 1, LocalScreen->RightColumn - 1, LocalScreen->BottomRow - 3, LocalScreen->BottomRow - 3, BOXDRAW_HORIZONTAL);

  for (Index = LocalScreen->TopRow + 3; Index < LocalScreen->BottomRow - 3; Index++) {
    PrintCharAt (LocalScreen->LeftColumn,      Index, BOXDRAW_VERTICAL);
    PrintCharAt (MenuWidth,                    Index, BOXDRAW_VERTICAL);
    PrintCharAt (LocalScreen->RightColumn - 1, Index, BOXDRAW_VERTICAL);
  }

  return EFI_SUCCESS;
}


VOID
ScuSafeFreePool (
  IN VOID **Buffer
  )
{
  ASSERT (Buffer != NULL);
  if (Buffer == NULL) {
    return ;
  }

  if (*Buffer != NULL) {
    gBS->FreePool (*Buffer);
    *Buffer = NULL;
  }
}

/**
 Check the instance of gEfiSetupUtilityBrowserProtocolGuid protocol

 @retval  EFI_SUCCESS           Protocol instance exist.
 @retval  Other                 Protocol instance does not exist.
**/
EFI_STATUS
CheckDeviceManagerSetup (
  VOID
  )
{
  EFI_STATUS    Status;

  //
  // There should be only one instance in the system
  // of the DeviceManagerSetup protocol, installed by
  // the BDS before sending the SetupUtilutyBrowser form
  //
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID **) &gDeviceManagerSetup
                  );

  if (EFI_ERROR (Status)) {
    gDeviceManagerSetup = NULL;
  }

  return Status;
}

/**
 Free allocated menu strings from memory

 @param[in, out] DevMgrInfo     Pointer to the Setup Utility Browser Protocol

 @retval EFI_SUCCESS            Strings successully freed
 @retval Other                  Memory free failed
**/
EFI_STATUS
FreeMenuStrings (
  IN OUT  EFI_SETUP_UTILITY_BROWSER_PROTOCOL *DevMgrInfo
  )
{
  // Local Variables
  UINTN     index;

  // Check the input information
  if (DevMgrInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (DevMgrInfo->MenuItemCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  // Free each string
  for (index = 0; index < DevMgrInfo->MenuItemCount; index++) {
    if (DevMgrInfo->MenuList[index].String == NULL) {
      continue;
    }

    gBS->FreePool (DevMgrInfo->MenuList[index].String);
    DevMgrInfo->MenuList[index].String = NULL;
  }

  return EFI_SUCCESS;
}

/**
 Initial menu strings by allocating memory

 @param[in, out] DevMgrInfo     Pointer to the Setup Utility Browser Protocol

 @retval  EFI_SUCCESS           Strings successully freed
 @retval  EFI_INVALID_PARAMETER Invalid input parameter
**/
EFI_STATUS
InitializeMenuStrings (
  IN OUT  EFI_SETUP_UTILITY_BROWSER_PROTOCOL *DevMgrInfo
  )
{
  // Local variables
  UINTN     index;

  // Check input information
  if (DevMgrInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (DevMgrInfo->MenuItemCount == 0) {
    return EFI_INVALID_PARAMETER;
  }
  FreeMenuStrings (DevMgrInfo);

  // loop through all the strings and allocate string buffers
  for (index = 0; index < DevMgrInfo->MenuItemCount; index++) {
    if (DevMgrInfo->MenuList[index].String != NULL) {
      continue;
    }

    DevMgrInfo->MenuList[index].String = GetToken(
                                           DevMgrInfo->MenuList[index].MenuTitle,
                                           DevMgrInfo->MenuList[index].Page
                                           );
  }

  return EFI_SUCCESS;
}

/**
 Check key event and application assign key list whether is match.

 @param[in] Key                Input key.
 @param[in] KeyList            Assign key list.

 @retval TRUE                  Found match key in key list.
 @retval FALSE                 Could not found match key in key list.
**/
BOOLEAN
CheckKeyList (
   IN  EFI_INPUT_KEY                   Key,
   IN  CONST EFI_INPUT_KEY             *KeyList
  )
{
  UINTN Index;
  Index = 0;
  while ((KeyList[Index].ScanCode != SCAN_NULL) ||
         (KeyList[Index].UnicodeChar != CHAR_NULL)) {
    if (Key.ScanCode != SCAN_NULL) {
      if (KeyList[Index].ScanCode == Key.ScanCode) {
        return TRUE;
      }
    } else {
      if (KeyList[Index].UnicodeChar == Key.UnicodeChar) {
        return TRUE;
      }

    }
    //
    // Not match, check next key
    //
    Index++;
  }

  return FALSE;
}

/**
  Function uses to get simple text input extention instance from system table.

  @return EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *  Pointer to EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL instance.
  @retval NULL                                 Cannot find EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL instance.
**/
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *
GetimpleTextInExInstance (
  VOID
  )
{
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx;
  EFI_STATUS                         Status;

  Status = gBS->HandleProtocol (
                  gST->ConsoleInHandle,
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID **) &SimpleTextInEx
                  );

  return Status == EFI_SUCCESS ? SimpleTextInEx : NULL;
}

/**
 A timer event notify function. When menu refresh interval time expires,
 the firmware signal the event.

 @param[in] Event               Event whose notification function is being invoked.
 @param[in] Context             Pointer to the notification function!|s context, which is
                                implementation-dependent.  Context corresponds to
                                NotifyContext in CreateEventEx().
**/
VOID
EFIAPI
RefreshTimeOutNotify (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  *((BOOLEAN *) Context) = TRUE;
  return;
}

/**
 ReadKey function for Setup Utility

 @param[in]  MouseKey           Key input from mouse
 @param[out] KeyBoardKey        Key input from keyboard
 @param[in]  TopOfScreen        First entry on top of screen
 @param[out] NewPos             New selected entry
 @param[in]  RefreshInterval    Menu refresh interval (in seconds).

 @retval EFI_SUCCESS            Key successfully read
 @retval Other                  No vailid key inputed
**/
EFI_STATUS
SetupUtilityReadKey (
  IN  EFI_INPUT_KEY                **MouseKey,
  OUT EFI_INPUT_KEY                *KeyBoardKey,
  IN  LIST_ENTRY                   *TopOfScreen,
  OUT LIST_ENTRY                   **NewPos,
  IN  UINT8                        RefreshInterval
  )
{
  UINTN                                 X, Y;
  EFI_STATUS                            Status;
  LIST_ENTRY                            *TempNewPos;
  EFI_KEY_DATA                          EfiKeyData;

  Status = EFI_SUCCESS;

  ZeroMem(KeyBoardKey, sizeof (EFI_INPUT_KEY));
  if (*MouseKey != NULL) {
    CopyMem (KeyBoardKey, *MouseKey, sizeof (EFI_INPUT_KEY));
  }

  if (*MouseKey == NULL) {
    mRefreshAnimation = TRUE;
    if (!UiInputDevice(RefreshInterval, &EfiKeyData, &X, &Y)) {
      CopyMem (KeyBoardKey, &EfiKeyData.Key, sizeof(EFI_INPUT_KEY));

      mRefreshAnimation = FALSE;
      if (KeyBoardKey->ScanCode != SCAN_ESC) {
        if (gGraphicsEnabled) {
          TempNewPos = AnimationMenu (KeyBoardKey, X, Y);
          if (TempNewPos == NULL) {
            KeyBoardKey->ScanCode    = SCAN_NULL;
            KeyBoardKey->UnicodeChar = CHAR_NULL;
            return EFI_NOT_FOUND;
          }
          *NewPos = TempNewPos;
          return EFI_SUCCESS;
        }
        //If select parent menu
        if (Y == 1) {
          return ParentMenu(KeyBoardKey, X, Y);
        } else if (Y == gScreenDimensions.BottomRow - 2 || Y == gScreenDimensions.BottomRow - 1) {
          return HotkeyMenu (KeyBoardKey, X, Y);
        } else {
          //if select sub-menu
          TempNewPos = ChildMenu(KeyBoardKey, X, Y, TopOfScreen);
          if (TempNewPos == NULL) {
            return EFI_NOT_FOUND;
          }

          *NewPos    = TempNewPos;
          return EFI_SUCCESS;;
        }
      }
    } else {
      CopyMem (KeyBoardKey, &EfiKeyData.Key, sizeof(EFI_INPUT_KEY));
    }
    mRefreshAnimation = FALSE;
  }

  if (*MouseKey != NULL) {
    gBS->FreePool (*MouseKey);
    *MouseKey = NULL;
  }

  return Status;
}

/**
 To check whether a year is leap year or not

 @param[in] Year                Year to test whether it is leap year or not

 @retval TRUE                   The input Year is leap year
 @retval FALSE                  The input Year is not leap year
**/
BOOLEAN
IsLeapYear (
  IN UINT16   Year
  )
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

/**
 Convert a unicode string to a UINTN

 @param[in] String                 Unicode string.

 @return UINTN of the number represented by String.
**/
UINTN
Atoi (
  IN CHAR16  *String
  )
{
  UINTN   Number;
  CHAR16  *Str;
  UINTN   m;
  UINTN   n;

  m = (UINTN) -1 / 10;
  n = (UINTN) -1 % 10;
  //
  // Skip preceeding white space
  //
  Str = String;
  while ((*Str) && (*Str == ' ')) {
    Str++;
  }
  //
  // Convert ot a Number
  //
  Number = 0;
  while (*Str != '\0') {
    if ((*Str >= '0') && (*Str <= '9')) {
      if ( (Number > m || Number == m) && *Str - '0' > (INTN) n) {
        return (UINTN) -1;
      }

      Number = (Number * 10) +*Str - '0';
    } else {
      break;
    }

    Str++;
  }

  return Number;
}

/**
 On-line editing date/time field

 @param[in]  Selection          Pointer to UI_MENU_SELECTION
 @param[in]  MenuOption         MenuOption of the selection Date/Time field
 @param[in]  FirstKey           The first inputed number key

 @retval EFI_SUCCESS            Date/Time sucessfully modified
 @retval EFI_ABORTED            Modification aborted
**/
EFI_STATUS
DateTimeInputOnTheFly (
  IN UI_MENU_SELECTION *Selection,
  IN UI_MENU_OPTION    *MenuOption,
  IN EFI_INPUT_KEY     FirstKey
  )
{
  FORM_BROWSER_STATEMENT *Question;
  EFI_HII_VALUE          *QuestionValue;
  EFI_INPUT_KEY          Key;
  UINTN                  EditValue;
  UINTN                  FieldWidth;
  UINTN                  Maximum;
  UINTN                  Minimum;
  UINTN                  StartCol;
  EFI_STATUS             Status;
  CHAR16                 InputText[20];
  UINTN                  Index;
  INTN                   DayOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


  Question = MenuOption->ThisTag;
  QuestionValue = &Question->HiiValue;
  Key       = FirstKey;
  Status = EFI_SUCCESS;
  FieldWidth = 2;
  Minimum = 0;
  Maximum = 9999;
  if (Question->Operand == EFI_IFR_DATE_OP) {
    Minimum = 1;
    switch (MenuOption->Sequence) {
    case 0:
      Maximum = 12;
      EditValue = QuestionValue->Value.date.Month;
      break;

    case 1:
      if ((QuestionValue->Value.date.Month==2) && IsLeapYear(QuestionValue->Value.date.Year)) {
        Maximum = 29;
      } else {
        Maximum = DayOfMonth[QuestionValue->Value.date.Month - 1];
      }
      EditValue = QuestionValue->Value.date.Day;
      break;

    case 2:
      Minimum = 2000;
      Maximum = 2099;
      FieldWidth = 4;
      EditValue = QuestionValue->Value.date.Year;
      break;

    default:
      break;
    }
  } else if (Question->Operand == EFI_IFR_TIME_OP) {
    Minimum = 0;

    switch (MenuOption->Sequence) {
    case 0:
      Maximum = 23;
      EditValue = QuestionValue->Value.time.Hour;
      break;

    case 1:
      Maximum = 59;
      EditValue = QuestionValue->Value.time.Minute;
      break;

    case 2:
      Maximum = 59;
      EditValue = QuestionValue->Value.time.Second;
      break;

    default:
      break;
    }
  } else {
    return EFI_ABORTED;
  }

  StartCol = (MenuOption->Sequence == 0) ? MenuOption->OptCol + 1 : MenuOption->OptCol;

  gST->ConOut->SetAttribute (gST->ConOut, SU_TEXT_HIGHLIGHT | SU_BACKGROUND_HIGHLIGHT);

  gST->ConOut->SetCursorPosition (gST->ConOut, StartCol, MenuOption->Row);
  if ((Question->Operand == EFI_IFR_DATE_OP) && (MenuOption->Sequence == 2)) {
    Print (L"    ");
  } else {
    Print (L"  ");
  }
  gST->ConOut->SetAttribute (gST->ConOut, SU_TEXT_HIGHLIGHT | SU_BACKGROUND_HIGHLIGHT);
  gST->ConOut->SetCursorPosition (gST->ConOut, StartCol, MenuOption->Row);

  Index = 0;
  Status = EFI_SUCCESS;
  while (Key.ScanCode != SCAN_ESC) {
    if((Key.UnicodeChar >= '0') && (Key.UnicodeChar <= '9')) {
      InputText[Index++] = Key.UnicodeChar;
      Print (L"%c",Key.UnicodeChar);
    } else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      InputText[Index] = 0;
      break;
    } else {
      Status = EFI_ABORTED;
      break;
    }
    if (Index == FieldWidth) {
      InputText[Index] = 0;
      break;
    }
    Status = WaitForKeyStroke (&Key);
    if (EFI_ERROR(Status)) {
      break;
    }
  }

  if (EFI_ERROR(Status) || (Key.ScanCode == SCAN_ESC)) {
    return EFI_ABORTED;
  }

  EditValue = Atoi (InputText);
  if ((EditValue < Minimum) || (EditValue > Maximum)) {
    return EFI_ABORTED;
  }

  if (Question->Operand == EFI_IFR_DATE_OP) {
    switch (MenuOption->Sequence) {
    case 0:
      QuestionValue->Value.date.Month = (UINT8)EditValue;
      break;

    case 1:
      QuestionValue->Value.date.Day = (UINT8)EditValue;
      break;

    case 2:
      QuestionValue->Value.date.Year = (UINT16)EditValue;
      break;

    default:
      break;
    }
  } else if (Question->Operand == EFI_IFR_TIME_OP) {
    switch (MenuOption->Sequence) {
    case 0:
      QuestionValue->Value.time.Hour = (UINT8)EditValue;
      break;

    case 1:
      QuestionValue->Value.time.Minute = (UINT8)EditValue;
      break;

    case 2:
      QuestionValue->Value.time.Second = (UINT8)EditValue;
      break;

    default:
      break;
    }
  }

  SetQuestionValue(Selection->FormSet, Selection->Form, Question, TRUE);

  return EFI_SUCCESS;
}

