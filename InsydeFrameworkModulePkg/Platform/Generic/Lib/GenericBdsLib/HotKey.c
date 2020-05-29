//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "HotKey.h"
#ifdef SECURE_FLASH_SUPPORT
#include EFI_GUID_DEFINITION (SecureFlashInfo)
#endif

EFI_EVENT                          mGetHotKeyEvent;
EFI_EVENT                          mHotKeyDelayTimeEvent;
EFI_MONITOR_KEY_FILTER_PROTOCOL    *mMonitorKey = NULL;
OEM_SERVICES_PROTOCOL              *mOemServices = NULL;
BOOLEAN                            mGetFunctionKey = FALSE;
UINT16                             mFunctionKey;
BOOLEAN                            mDisableQuietBoot = FALSE;
#ifdef ENABLE_CONSOLE_EX
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *mSimpleTextInEx = NULL;
#endif


VOID
GetKeyFunction (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );


EFI_STATUS
BdsLibInstallHotKeys (
  IN HOT_KEY_CONTEXT      *NotifyContext
  )
/*++

Routine Description:

  HotKey services init

Arguments:

  NotifyContext  - A user defined context which is used in hot key service

Returns:

  EFI_SUCCESS            - Init Success
  EFI_INVALID_PARAMETER  - Input value is invalid

--*/
{
  UINT64                     Timeout;
  EFI_STATUS                 Status;
#ifdef SECURE_FLASH_SUPPORT
  UINTN                      Size;
  IMAGE_INFO                 ImageInfo;
#endif

  if (NotifyContext == NULL) {
    return EFI_INVALID_PARAMETER;
  }

#ifdef SECURE_FLASH_SUPPORT
  Size = sizeof (IMAGE_INFO);
  Status = gRT->GetVariable (
                  L"SecureFlashInfo",
                  &gSecureFlashInfoGuid,
                  NULL,
                  &Size,
                  &ImageInfo
                  );
  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
    mGetHotKeyEvent = NULL;
    return EFI_SUCCESS;
  }
#endif

  Timeout = SPECIFIED_TIME;

  Status = gBS->CreateEvent (
                  EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
                  EFI_TPL_NOTIFY,
                  GetKeyFunction,
                  (VOID *) NotifyContext,
                  &mGetHotKeyEvent
                  );

  if (!EFI_ERROR(Status)) {
    Status = gBS->SetTimer (mGetHotKeyEvent, TimerPeriodic, Timeout);
  }
  //
  // According to platform quiet boot policy to initialize disable quiet boot state
  //
  mDisableQuietBoot = NotifyContext->EnableQuietBootPolicy ? FALSE : TRUE;

  gBS->LocateProtocol(&gOemServicesProtocolGuid, NULL, &mOemServices);

  gBS->LocateProtocol(&gEfiMonitorKeyFilterProtocolGuid, NULL, &mMonitorKey);

#ifdef ENABLE_CONSOLE_EX
  gBS->HandleProtocol (
                 gST->ConsoleInHandle,
                 &gEfiSimpleTextInputExProtocolGuid,
                 &mSimpleTextInEx
                 );
#endif

  return EFI_SUCCESS;
}


EFI_STATUS
BdsLibSetKey (
  IN UINT16                    Key,
  IN HOT_KEY_CONTEXT           *HotKeyContext
  )
/*++

Routine Description:

  Set specific hot key

Arguments:

  Key         - Input hot key value

Returns:

  EFI_SUCCESS - Set specific key to hot key service successful

--*/
{
  mFunctionKey = Key;
  mGetFunctionKey = TRUE;
  return EFI_SUCCESS;
}



EFI_STATUS
GetQuietBootCondition (
  IN    HOT_KEY_CONTEXT    *HotKeyContext,
  IN    EFI_INPUT_KEY      *Key,
  OUT   BOOLEAN            *EnableQuietBoot
  )
/*++

Routine Description:

  According input context and key value to determine to enable
  quiet boot or disable quiet boot

Arguments:

  HotKeyContext       - pointer to HOT_KEY_CONTEXT which save user quiet boot requirement
  key                 - pointer to save key value
  EnableQuietBoot     - Aboolean value to save the enable quiet boot or disable quiet boot

Returns:

  EFI_SUCCESS            - Get Quiet boot condition successful
  EFI_INVALID_PARAMETER  - Input value is invalid

--*/
{
  UINTN    Index;

  if (HotKeyContext == NULL || Key == NULL || EnableQuietBoot == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // According context policy to set EableQuietBoot
  //
  if (HotKeyContext->EnableQuietBootPolicy) {
    *EnableQuietBoot = TRUE;
  } else {
    *EnableQuietBoot = FALSE;
  }

  //
  // check key value is disable quiet boot hot key
  //
  if (*EnableQuietBoot) {
    for (Index = 0; Index < HotKeyContext->DisableQueitBootHotKeyCnt; Index++) {
      if ((HotKeyContext->HotKeyList[Index].ScanCode == Key->ScanCode) &&
          (HotKeyContext->HotKeyList[Index].UnicodeChar == Key->UnicodeChar)) {
         *EnableQuietBoot = FALSE;
      }
    }
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
DisplayBadgingString (
  IN  HOT_KEY_CONTEXT    *HotkeyContext
  )
/*++

Routine Description:

  According to pressed hotkey to display relative badging string.

Arguments:

  HotkeyContext            - Pointer to HOT_KEY_CONTEXT instance.

Returns:

  EFI_SUCCESS              - Print badging string successful.
  EFI_INVALID_PARAMETER    - HotkeyContext is NULL.
  EFI_UNSUPPORTED          - Cannot find gEfiOEMBadgingSupportProtocolGuid or cannot show badging string.
  EFI_NOT_READY            - User doesn't press hotkey or hardware isn't ready.

--*/
{
  UINT16                            FunctionKey;
  BOOLEAN                           KeyPressed;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  EFI_STATUS                        Status;

  if (HotkeyContext == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, &Badging);
  if (EFI_ERROR (Status) || !HotkeyContext->CanShowString) {
    return EFI_UNSUPPORTED;
  }

  Status = BdsLibGetHotKey (&FunctionKey, &KeyPressed);
  if (EFI_ERROR (Status) || !KeyPressed) {
   return EFI_NOT_READY;
  }

  Status = ShowOemString (Badging, TRUE, (UINT8) FunctionKey);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }
  BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);

  return EFI_SUCCESS;
}

VOID
GetKeyFunction (
  IN  EFI_EVENT          Event,
  IN  VOID               *Context
  )
/*++

Routine Description:

  HotKey Check Event Handler

Arguments:

  Event     - Event
  Context   - Event Context Pointer

Returns:

  EFI_SUCCESS

--*/
{
  EFI_STATUS                         Status;
  UINT32                             KeyDetected = 0;
  UINT16                             GetKey;
  UINTN                              KeyValue;
  EFI_STATUS                         KeyStatus;
  EFI_INPUT_KEY                      Key;
  BOOLEAN                            EnableQuietBoot;
  HOT_KEY_CONTEXT                    *HotKeyContext;
#ifdef ENABLE_CONSOLE_EX
  EFI_KEY_DATA                       KeyData;
#endif
  BOOLEAN                            CheckPlatformHook;
  STATIC BOOLEAN                     BadgingStrPrinted = FALSE;

  HotKeyContext = (HOT_KEY_CONTEXT *) Context;
#ifdef ENABLE_CONSOLE_EX
  if (mSimpleTextInEx != NULL) {
    KeyStatus = mSimpleTextInEx->ReadKeyStrokeEx (mSimpleTextInEx, &KeyData);
    Key = KeyData.Key;
  } else {
#endif
    KeyStatus = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
#ifdef ENABLE_CONSOLE_EX
  }
#endif
  if (!EFI_ERROR (KeyStatus)) {
    EnableQuietBoot = TRUE;
    GetQuietBootCondition (HotKeyContext, &Key, &EnableQuietBoot);
    //
    //  1. Check the Quiet boot requirement from input context
    //  2. Check the mDisableQuiete to determine is whether already disable quiet boot
    //
    if (!EnableQuietBoot && !mDisableQuietBoot) {
      DisableQuietBoot ();
      mDisableQuietBoot = TRUE;
    }
  }


  if (!mGetFunctionKey) {
    if (mOemServices == NULL || mMonitorKey == NULL) {
      return;
    }

    mMonitorKey->GetMonitoredKeys (mMonitorKey, &KeyDetected);
    KeyValue = 0;
    Status = mOemServices->Funcs[COMMON_POST_KEY_TABLE](
                             mOemServices,
                             COMMON_POST_KEY_TABLE_ARG_COUNT,
                             KeyDetected,
                             SCAN_NULL,
                             &KeyValue
                             );
    if (EFI_ERROR(Status) && (Status != EFI_UNSUPPORTED)) {
      return;
    }
    CheckPlatformHook = TRUE;
    GetKey = (UINT16) KeyValue;
    if (HotKeyContext != NULL && HotKeyContext->PlatformGetKeyFunction != NULL) {
      CheckPlatformHook = HotKeyContext->PlatformGetKeyFunction (GetKey);
    }
    if (CheckPlatformHook) {
      BdsLibSetKey (GetKey, HotKeyContext);
    }
  }
  if (!BadgingStrPrinted) {
    //
    // Display badging string again if hot key is pressed but system doesn't print bading string before.
    //
    Status = DisplayBadgingString (HotKeyContext);
    if (!EFI_ERROR (Status)) {
      BadgingStrPrinted = TRUE;
    }
  }
  return;
}


EFI_STATUS
BdsLibStopHotKeyEvent (
  VOID
  )
/*++

Routine Description:

  Stop HotKey event

Arguments:

  VOID

Returns:

  EFI_SUCCESS  - Stop HotKeyEvent successful
  Other        - Cannot stop HotKey event.

--*/
{
  EFI_STATUS      Status;
  EFI_STATUS      KeyStatus;
  UINT16          FunctionKey;
  BOOLEAN         HotKeyPressed;


  while (mHotKeyDelayTimeEvent != NULL) {
    Status = gBS->CheckEvent (mHotKeyDelayTimeEvent);

    if (mGetFunctionKey || Status != EFI_NOT_READY) {
      //
      // If user inputs hotkey or delay time has expired, close events.
      //
      gBS->CloseEvent (mHotKeyDelayTimeEvent);
      mHotKeyDelayTimeEvent = NULL;
    } else {
      gBS->Stall (500);
    }
  }

  Status = EFI_ABORTED;

  if (mGetHotKeyEvent != NULL) {
    Status = gBS->CloseEvent (mGetHotKeyEvent);
    mGetHotKeyEvent = NULL;
  }

  if (BdsLibIsBootOrderHookEnabled ()) {
    KeyStatus = BdsLibGetHotKey (&FunctionKey, &HotKeyPressed);
    if (!EFI_ERROR (KeyStatus) && HotKeyPressed) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
  }


  return Status;
}

EFI_STATUS
BdsLibGetHotKey (
  OUT UINT16  *FunctionKey,
  OUT BOOLEAN *HotKeyPressed
  )
/*++

Routine Description:

  Get the state of user is whether pressed hotkey and the pressed hotkey value

Arguments:

  FunctionKey   - pointer to user pressed hotkey value
  HotKeyPressed - TRUE:  user has pressed hotkey
                  FALSE: user hasn't pressed hotkey yet
Returns:

  EFI_SUCCESS           - Get hotkey value and state successful
  EFI_INVALID_PARAMETER - Input value is invalid

--*/
{
  if (FunctionKey == NULL || HotKeyPressed == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HotKeyPressed = mGetFunctionKey;
  //
  // Only need output hotkey after hot key pressed
  //
  if (mGetFunctionKey) {
    *FunctionKey = mFunctionKey;
  }
  return EFI_SUCCESS;
}


EFI_STATUS
BdsLibGetQuietBootState (
  OUT BOOLEAN        *QuietBootState
  )
/*++

Routine Description:

  Get current quiet boot state

Arguments:

  QuietBootState  - TRUE:  indicate current quiet boot state is enable quiet boot
                    FALSE: indicate current quiet boot state is disable quiet boot
Returns:

  EFI_SUCCESS           - Get quiet boot state successful
  EFI_INVALID_PARAMETER - Input value is invalid

--*/
{
  if (QuietBootState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *QuietBootState = mDisableQuietBoot ? FALSE : TRUE;
  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibSetHotKeyDelayTime (
  VOID
  )
/*++

Routine Description:

  Based on BootDelayTime of "Setup" variable, create hotkey delay timer event.

Arguments:

  None

Returns:

  EFI_SUCCESS            - Create event success
  EFI_UNSUPPORTED        - Hot key is not installed. Not support hot key delay.
  EFI_NOT_FOUND          - Locate EfiSetupUtility protocol fail
  otherwise              - Create event or set timer fail

--*/
{
  UINT64                                Timeout;
  EFI_STATUS                            Status;
  UINT16                                TimeoutVar;
  UINTN                                 Size;

  //
  // If hot key is not initialized or delay event is already set, return unsupported.
  //
  if (mGetHotKeyEvent == NULL || mHotKeyDelayTimeEvent != NULL) {
    return EFI_UNSUPPORTED;
  }

  Timeout = 0;
  Size = sizeof (TimeoutVar);
  Status = gRT->GetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &TimeoutVar
                  );
  if (!EFI_ERROR (Status) && TimeoutVar != 0) {
    Timeout = TIMER_EVENT_ONE_SEC * TimeoutVar;
    Status = gBS->CreateEvent (
                    EFI_EVENT_TIMER,
                    EFI_TPL_NOTIFY,
                    NULL,
                    NULL,
                    &mHotKeyDelayTimeEvent
                    );
    if (!EFI_ERROR(Status)) {
      Status = gBS->SetTimer (mHotKeyDelayTimeEvent, TimerRelative, Timeout);
    }
  }

  return Status;
}
