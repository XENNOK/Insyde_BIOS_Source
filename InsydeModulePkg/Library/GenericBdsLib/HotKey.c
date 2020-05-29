/** @file
  Hot key function

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

#include "HotKey.h"
#include <Protocol/DynamicHotKey.h>

EFI_EVENT                          mGetHotKeyEvent;
EFI_EVENT                          mHotKeyDelayTimeEvent;
EFI_MONITOR_KEY_FILTER_PROTOCOL    *mMonitorKey = NULL;
BOOLEAN                            mGetFunctionKey = FALSE;
UINT16                             mFunctionKey;
BOOLEAN                            mDisableQuietBoot = FALSE;
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *mSimpleTextInEx = NULL;


VOID
EFIAPI
GetKeyFunction (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );


/**
  HotKey services init

  @param  NotifyContext     A user defined context which is used in hot key service

  @retval EFI_SUCCESS             Init Success
  @retval EFI_INVALID_PARAMETER   Input value is invalid

**/
EFI_STATUS
BdsLibInstallHotKeys (
  IN HOT_KEY_CONTEXT      *NotifyContext
  )
{
  UINT64                     Timeout;
  EFI_STATUS                 Status;
  UINTN                      Size;
  IMAGE_INFO                 ImageInfo;

  if (NotifyContext == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (FeaturePcdGet(PcdSecureFlashSupported)) {
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
  }

  Timeout = SPECIFIED_TIME;

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
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

  gBS->LocateProtocol (&gEfiMonitorKeyFilterProtocolGuid, NULL, (VOID **) &mMonitorKey);

  gBS->HandleProtocol (
         gST->ConsoleInHandle,
         &gEfiSimpleTextInputExProtocolGuid,
         (VOID **) &mSimpleTextInEx
         );

  return EFI_SUCCESS;
}


/**
  Set specific hot key

  @param  Key           Input hot key value

  @retval EFI_SUCCESS   Set specific key to hot key service successful

**/
EFI_STATUS
BdsLibSetKey (
  IN UINT16                    Key,
  IN HOT_KEY_CONTEXT           *HotKeyContext
  )
{

  mFunctionKey    = Key;
  mGetFunctionKey = TRUE;

  return EFI_SUCCESS;
}


/**
  According input context and key value to determine to enable
  quiet boot or disable quiet boot

  @param  HotKeyContext      pointer to HOT_KEY_CONTEXT which save user quiet boot requirement
  @param  key                pointer to save key value
  @param  EnableQuietBoot    Aboolean value to save the enable quiet boot or disable quiet boot

  @retval EFI_SUCCESS            Get Quiet boot condition successful
  @retval EFI_INVALID_PARAMETER  Input value is invalid

**/
EFI_STATUS
GetQuietBootCondition (
  IN    HOT_KEY_CONTEXT    *HotKeyContext,
  IN    EFI_INPUT_KEY      *Key,
  OUT   BOOLEAN            *EnableQuietBoot
  )
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

/**
  This function uses to check gEfiConsoleControlProtocolGuid exists or not.

  @retval TRUE   System supports console control protocol.
  @retval FALSE  System doesn't support console control protocol.
**/
BOOLEAN
SupportConsoleControl (
  VOID
  )
{
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **) &ConsoleControl);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  return TRUE;
}

/**
  According to pressed hotkey to display relative badging string.

  @param[in]  HotkeyContext       Pointer to HOT_KEY_CONTEXT instance.

  @retval EFI_SUCCESS             Print badging string successful.
  @retval EFI_INVALID_PARAMETER   HotkeyContext is NULL.
  @retval EFI_UNSUPPORTED         Cannot find gEfiOEMBadgingSupportProtocolGuid or cannot show badging string.
  @retval EFI_NOT_READY           User doesn't press hotkey or hardware isn't ready.

**/
STATIC
EFI_STATUS
DisplayBadgingString (
  IN  HOT_KEY_CONTEXT    *HotkeyContext
  )
{
  UINT16                            FunctionKey;
  BOOLEAN                           KeyPressed;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  EFI_STATUS                        Status;

  if (HotkeyContext == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **) &Badging);
  if (EFI_ERROR (Status) || !HotkeyContext->CanShowString) {
    return EFI_UNSUPPORTED;
  }

  Status = BdsLibGetHotKey (&FunctionKey, &KeyPressed);
  if (EFI_ERROR (Status) || !KeyPressed) {
   return EFI_NOT_READY;
  }

  Status = EFI_SUCCESS;
  if (SupportConsoleControl ()) {
    //
    // print OEM string in text mode and GOP mode, if support console control protocol.
    //
    Status = ShowOemString (Badging, TRUE, (UINT8) FunctionKey);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY;
    }
    Status = BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);
  } else if (!mDisableQuietBoot) {
    //
    // Only print OEM string in GOP mode, if quiet boot is disabled and system doesn't
    // support console control protocol.
    //
    Status = ShowOemString (Badging, TRUE, (UINT8) FunctionKey);
  } else {
    //
    // Only print OEM string in text mode, if quiet boot is disabled and system doesn't
    // support console control protocol.
    //
    Status = BdsLibShowOemStringInTextMode (TRUE, (UINT8) FunctionKey);
  }

  return Status;
}


/**
  HotKey Check Event Handler

  @param  Event         Event
  @param  Context       Event Context Pointer

  @retval EFI_SUCCESS

**/
VOID
EFIAPI
GetKeyFunction (
  IN  EFI_EVENT          Event,
  IN  VOID               *Context
  )
{
  EFI_STATUS                            Status;
  UINT32                                KeyDetected = 0;
  UINT16                                GetKey;
  UINTN                                 KeyValue;
  EFI_STATUS                            KeyStatus;
  EFI_INPUT_KEY                         Key;
  BOOLEAN                               EnableQuietBoot;
  HOT_KEY_CONTEXT                       *HotKeyContext;
  EFI_KEY_DATA                          KeyData;
  BOOLEAN                               CheckPlatformHook;
  STATIC BOOLEAN                        BadgingStrPrinted = FALSE;

  HotKeyContext = (HOT_KEY_CONTEXT *) Context;
  if (mSimpleTextInEx != NULL) {
    KeyStatus = mSimpleTextInEx->ReadKeyStrokeEx (mSimpleTextInEx, &KeyData);
    Key = KeyData.Key;
  } else {
    KeyStatus = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  }

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
    if (mMonitorKey == NULL) {
      return;
    }

    mMonitorKey->GetMonitoredKeys (mMonitorKey, &KeyDetected);
    if (FeaturePcdGet (PcdDynamicHotKeySupported)) {
      STATIC DYNAMIC_HOTKEY_PROTOCOL        *DynamicHotKey;
      //
      // Get dynamic key first so that OEM can use dynamic hot key to override built-in hot key
      //
      if (DynamicHotKey == NULL) {
        Status = gBS->LocateProtocol (&gDynamicHotKeyProtocolGuid, NULL, (VOID **)&DynamicHotKey);
        if (EFI_ERROR (Status)) {
          DynamicHotKey = NULL;
        }
      }
      if (DynamicHotKey != NULL) {
        Status = DynamicHotKey->GetDynamicHotKeyOperation (DynamicHotKey, KeyDetected, &KeyValue);
        if (!EFI_ERROR (Status)) {
          mFunctionKey = (UINT16)KeyValue;
          mGetFunctionKey = TRUE;
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
      }
    }
    
    //
    // OemServices
    //
    KeyValue = 0;
    Status = OemSvcInstallPostKeyTable (
               KeyDetected,
               SCAN_NULL,
               &KeyValue
	       );
    if (!EFI_ERROR(Status) || KeyValue == 0) {
      return;
    }
    GetKey = (UINT16) KeyValue;
    CheckPlatformHook = TRUE;
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


/**
  Stop HotKey event

  @retval EFI_SUCCESS   Stop HotKeyEvent successful
  @retval Other         Cannot stop HotKey event.

**/
EFI_STATUS
BdsLibStopHotKeyEvent (
  VOID
  )
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
  BdsLibGetHotKey (&FunctionKey, &HotKeyPressed);
  if ((!HotKeyPressed) && (mGetHotKeyEvent != NULL)) {
    //
    // Last try, see if any hotkey is pressed and 
    // it guarantees mGetHotKeyEvent's callback function can be invoked at least once.
    //
    gBS->SignalEvent (mGetHotKeyEvent);
  }
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


/**
  Get the state of user is whether pressed hotkey and the pressed hotkey value

  @param  FunctionKey       pointer to user pressed hotkey value
  @param  HotKeyPressed     TRUE:  user has pressed hotkey
                            FALSE: user hasn't pressed hotkey yet

  @retval EFI_SUCCESS              Get hotkey value and state successful
  @retval EFI_INVALID_PARAMETER    Input value is invalid

**/
EFI_STATUS
BdsLibGetHotKey (
  OUT UINT16  *FunctionKey,
  OUT BOOLEAN *HotKeyPressed
  )
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


/**
  Get current quiet boot state

  @param  QuietBootState     TRUE:  indicate current quiet boot state is enable quiet boot
                             FALSE: indicate current quiet boot state is disable quiet boot

  @retval EFI_SUCCESS             Get quiet boot state successful
  @retval EFI_INVALID_PARAMETER   Input value is invalid

**/
EFI_STATUS
BdsLibGetQuietBootState (
  OUT BOOLEAN        *QuietBootState
  )
{
  if (QuietBootState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *QuietBootState = mDisableQuietBoot ? FALSE : TRUE;

  return EFI_SUCCESS;
}


/**
  Based on BootDelayTime of "Setup" variable, create hotkey delay timer event.

  @param  Level         The memory test intensive level.

  @retval EFI_SUCCESS       Create event success
  @retval EFI_UNSUPPORTED   Hot key is not installed. Not support hot key delay.
  @retval EFI_NOT_FOUND     Locate EfiSetupUtility protocol fail
  @retval Other             Create event or set timer fail

**/
EFI_STATUS
BdsLibSetHotKeyDelayTime (
  VOID
  )
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
                    EVT_TIMER,
                    TPL_NOTIFY,
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

