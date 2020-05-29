/** @file
 Setup utility related definitions in SetupBrowserDxe driver

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

#ifndef _SETUP_UTILITY_H
#define _SETUP_UTILITY_H

#include "Setup.h"

#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SetupUtilityBrowser.h>
#include <Protocol/SetupMouse.h>
#include <KernelSetupConfig.h>

#define EVENT_LOG_FRAME_SPACE   2
#define MAX_HELP_STR_LINE       13
#define LINE_CONUT_PER_PAGE     80

#define KEY_UP_SHIFT              0x103a
#define KEY_DOWN_SHIFT            0x103b

#define UNLOCK_PASSWORD          0x00
#define ENTER_OLD_PASSWORD       0x01
#define ENTER_NEW_PASSWORD       0x02
#define CONFIRM_NEW_PASSWORD     0x03

extern BOOLEAN    mBootManager;
extern BOOLEAN    mBootFromFile;
extern EFI_SETUP_UTILITY_BROWSER_PROTOCOL *gDeviceManagerSetup;
extern EFI_SETUP_MOUSE_PROTOCOL           *gSetupMouse;
extern BOOLEAN                         gUpArrow;
extern BOOLEAN                         gDownArrow;
extern UINTN                           gUpArrowX;
extern UINTN                           gUpArrowY;
extern UINTN                           gDownArrowX;
extern UINTN                           gDownArrowY;


VOID
ScuSafeFreePool (
  IN VOID **Buffer
  );

EFI_STATUS
SetupUtilityPrintBorderLine (
  IN EFI_SCREEN_DESCRIPTOR                       *LocalScreen,
  IN UINTN                                       MenuWidth
  );

EFI_STATUS
SetupDisplayTitleAndHotKeyBar (
  IN EFI_SCREEN_DESCRIPTOR                       *LocalScreen
  );

VOID
UiFreeMenuList (
  VOID
  );

EFI_STATUS
CheckDeviceManagerSetup (
  VOID
  );

EFI_STATUS
InitializeMenuStrings (
  IN OUT  EFI_SETUP_UTILITY_BROWSER_PROTOCOL *DevMgrInfo
  );

  EFI_STATUS
FreeMenuStrings (
  IN OUT  EFI_SETUP_UTILITY_BROWSER_PROTOCOL *DevMgrInfo
  );

EFI_STATUS
SetupUtilityShowInformations (
  IN   CHAR16                *TitleString,
  IN   CHAR16                *InfoStrings,
  IN   EFI_SCREEN_DESCRIPTOR *LocalScreen
  );

EFI_STATUS
DisplayInfoPage (
  IN CHAR16                                   *TitleString,
  IN CHAR16                                   *InfoString,
  IN EFI_SCREEN_DESCRIPTOR                    *LocalScreen
  );

EFI_STATUS
DisplayEmptyPage (
  IN EFI_SCREEN_DESCRIPTOR                    *LocalScreen
  );

BOOLEAN
UiInputDevice (
  IN   UINT8                             RefreshInterval,
  OUT  EFI_KEY_DATA                      *EfiKeyData,
  OUT  UINTN                             *MouseX,
  OUT  UINTN                             *MouseY
  );

BOOLEAN
InputDevice (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *LocalSetupMouse,
  OUT EFI_KEY_DATA                      *EfiKeyData,
  IN  UINTN                             *MouseX,
  IN  UINTN                             *MouseY
  );

BOOLEAN
CheckKeyList (
  IN  EFI_INPUT_KEY                   Key,
  IN  CONST EFI_INPUT_KEY             *KeyList
  );

EFI_STATUS
InstallH2ODialogProtocol (
  IN EFI_HANDLE                        Handle
  );

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *
GetimpleTextInExInstance (
  VOID
  );

VOID
EFIAPI
RefreshTimeOutNotify (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

EFI_STATUS
SetupUtilityReadKey (
  EFI_INPUT_KEY                    **MouseKey,
  EFI_INPUT_KEY                    *KeyBoardKey,
  LIST_ENTRY                   *TopOfScreen,
  LIST_ENTRY                   **NewPos,
  UINT8                            RefreshInterval
  );

BOOLEAN
IsLeapYear (
  IN UINT16   Year
  );

EFI_STATUS
DateTimeInputOnTheFly (
  UI_MENU_SELECTION *Selection,
  UI_MENU_OPTION    *MenuOption,
  EFI_INPUT_KEY     FirstKey
  );

#endif

