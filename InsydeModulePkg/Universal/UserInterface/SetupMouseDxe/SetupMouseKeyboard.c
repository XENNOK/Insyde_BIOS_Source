/** @file
 Setup Mouse Keyboard implementation

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

#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/HiiFont.h>
#include <Protocol/DriverBinding.h>
#include <Library/UefiHiiServicesLib.h>
#include <Guid/ConsoleInDevice.h>
#include "SetupMouse.h"
#include "KeyboardLayout.h"

#define INVALID_KEY_INDEX (UINTN)(-1)



extern KEYBOARD_LAYOUT               *mKeyboardLayoutList[];
extern UINTN                         mKeyboardLayoutCount;
extern KEY_MAP_STR                   mConvertWinKeyToStr[];
extern UINT8                         mKeyConvertionTable[][4];
extern UINT16                        mKeyConvertionTableNumber;

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} VIRTUAL_KB_VENDOR_DEVICE_PATH;
#pragma pack()

VIRTUAL_KB_VENDOR_DEVICE_PATH  mVirtualKBDevicePath = {
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_SETUP_MOUSE_PROTOCOL_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (sizeof (EFI_DEVICE_PATH_PROTOCOL)),
      (UINT8) ((sizeof (EFI_DEVICE_PATH_PROTOCOL)) >> 8)
    }
  }
};

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Starts a device controller or a bus controller.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  EFI_STATUS                                Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL            SimpleInput;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL         SimpleInputEx;

  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (DevicePath != (EFI_DEVICE_PATH_PROTOCOL*)&mVirtualKBDevicePath) {
    return EFI_UNSUPPORTED;
  }

  //
  // Open the SimpleTextIn / SimpleTextInEx needed to perform the supported test.
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID*) &SimpleInput,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID*) &SimpleInputEx,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  return EFI_SUCCESS;

Exit:

  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiSimpleTextInProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiSimpleTextInputExProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  return Status;

}

/**
  Stops a device controller or a bus controller.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
  )
/*++

  Routine Description:
    Stop this driver on ControllerHandle by removing Disk IO protocol and closing
    the Block IO protocol on ControllerHandle.

  Arguments:
    This              - Protocol instance pointer.
    ControllerHandle  - Handle of device to stop driver on.
    NumberOfChildren  - Not used.
    ChildHandleBuffer - Not used.

  Returns:
    EFI_SUCCESS         - This driver is removed ControllerHandle.
    other               - This driver was not removed from this device.
    EFI_UNSUPPORTED

--*/
{

  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  EFI_STATUS                                Status;

  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (DevicePath != (EFI_DEVICE_PATH_PROTOCOL*)&mVirtualKBDevicePath) {
    return EFI_UNSUPPORTED;
  }

  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiSimpleTextInProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiSimpleTextInputExProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  return EFI_SUCCESS;
}

EFI_DRIVER_BINDING_PROTOCOL gSetupMouseKeyboardDriverBinding = {
  SetupMouseKeyboardDriverBindingSupported,
  SetupMouseKeyboardDriverBindingStart,
  SetupMouseKeyboardDriverBindingStop,
  0x10,
  NULL,
  NULL
};


EFI_STATUS
DrawKeyboard (
  IN PRIVATE_MOUSE_DATA                    *Private
  );

EFI_STATUS
KeyboardGotoXY (
  IN  PRIVATE_MOUSE_DATA                   *Private,
  IN  INTN                                 X,
  IN  INTN                                 Y
  );


struct _SETUP_MOUSE_KEYBOARD {
  EFI_HANDLE                          Handle;
  BOOLEAN                             UpdateNeeded;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL      SimpleInput;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   SimpleInputEx;
  EFI_EVENT                           WaitForKey;
  EFI_EVENT                           WaitForKeyEx;
  EFI_INPUT_KEY                       Key;
  EFI_KEY_STATE                       KeyState;
  KEY_POS_MAP                         *KeyPosMap;
  UINTN                               KeyboardLayoutIndex;
  BOOLEAN                             Focused;
  BOOLEAN                             UserFocused;
  BOOLEAN                             Draged;
  UINTN                               HoverIndex;
  UINTN                               PressIndex;
  BOOLEAN                             CapsOn;
  BOOLEAN                             ShiftOn;
  UINT8                               LeftShiftOn;
  UINT8                               RightShiftOn;
  POINT                               DragStartPoint;
  POINT                               DragOriginalPoint;
};

EFI_STATUS
SetKeybaordToggleState (
  IN SETUP_MOUSE_KEYBOARD               *KeyboardData,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{
  UINTN                                 Index;
  EFI_KEY_STATE                         *KeyState;

  //
  // update toggle state
  //
  if ((*KeyToggleState & EFI_TOGGLE_STATE_VALID) != EFI_TOGGLE_STATE_VALID) {
    return EFI_UNSUPPORTED;
  }

  KeyState = &KeyboardData->KeyState;
  KeyState->KeyToggleState = EFI_TOGGLE_STATE_VALID;
  KeyboardData->CapsOn     = 0;
  if ((*KeyToggleState & EFI_CAPS_LOCK_ACTIVE) == EFI_CAPS_LOCK_ACTIVE) {
    KeyState->KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
    KeyboardData->CapsOn = 1;
  }

  for (Index = 0; Index < mKeyConvertionTableNumber; Index++) {
    switch (mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[Index].WinKey) {

    case VK_CAPITAL:
      if (KeyboardData->CapsOn == 1) {
        KeyboardData->KeyPosMap[Index].State = KEY_PRESS;
      } else {
        KeyboardData->KeyPosMap[Index].State = KEY_NORMAL;
      }
      break;
    }
  }

  DrawKeyboard (mPrivate);
  return EFI_SUCCESS;
}

/**
 Sync the keyboard toggle state by BDA in legacy mode.


 @retval EFI_SUCCESS            Update the keybaord state success.
 @retval EFI_NOT_READY          Virtual Keybaord not yet display.

**/
EFI_STATUS
UpdateKeyboardStateByBDA (
  VOID
  )
{
  PRIVATE_MOUSE_DATA   *Private;
  SETUP_MOUSE_KEYBOARD *KeyboardData;
  QUALIFIER_STAT       *QualifierStat;
  EFI_KEY_TOGGLE_STATE KeyToggleState;

  Private = mPrivate;

  if (!Private->Keyboard.Visible) {
    return EFI_NOT_READY;
  }

  KeyboardData = Private->KeyboardData;
  //
  // Sync LEDs state, currently only support CapsLock
  //
  if (Private->IsCsmEnabled) {
    QualifierStat = (QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
    if (KeyboardData->CapsOn != QualifierStat->CapsLock) {
      KeyToggleState = KeyboardData->KeyState.KeyToggleState | EFI_TOGGLE_STATE_VALID;
      if (QualifierStat->CapsLock) {
        KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
      } else {
        KeyToggleState &= (~EFI_CAPS_LOCK_ACTIVE);
      }
      SetKeybaordToggleState (KeyboardData, &KeyToggleState);
      RenderImageForAllGop (Private);
    }
  }




  return EFI_SUCCESS;
}


VOID
SyncKeyboardState (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
{
  EFI_STATUS                            Status;
  EFI_KEY_STATE                         *KeyState;
  EFI_KEY_TOGGLE_STATE                  KeyToggleState;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInEx;
  UINTN                                 Index;

  //
  // update shift state
  //
  KeyState = &KeyboardData->KeyState;
  KeyState->KeyShiftState = EFI_SHIFT_STATE_VALID;
  KeyboardData->ShiftOn = 0;
  if (KeyboardData->LeftShiftOn == 1) {
    KeyState->KeyShiftState |= EFI_LEFT_SHIFT_PRESSED;
    KeyboardData->ShiftOn = 1;
  }
  if (KeyboardData->RightShiftOn == 1) {
    KeyState->KeyShiftState |= EFI_RIGHT_SHIFT_PRESSED;
    KeyboardData->ShiftOn = 1;
  }

  for (Index = 0; Index < mKeyConvertionTableNumber; Index++) {
    switch (mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[Index].WinKey) {

    case VK_LSHIFT:
      if (KeyboardData->LeftShiftOn == 1) {
        KeyboardData->KeyPosMap[Index].State = KEY_PRESS;
      } else {
        KeyboardData->KeyPosMap[Index].State = KEY_NORMAL;
      }
      break;
    case VK_RSHIFT:
      if (KeyboardData->RightShiftOn == 1) {
        KeyboardData->KeyPosMap[Index].State = KEY_PRESS;
      } else {
        KeyboardData->KeyPosMap[Index].State = KEY_NORMAL;
      }
      break;
    }
  }

  //
  // sync toggle state
  //
  KeyToggleState = EFI_TOGGLE_STATE_VALID;
  if (KeyboardData->CapsOn == 1) {
    KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
  }

  //
  // SetKeyboardState
  //
  if (gST->ConsoleInHandle != NULL) {
    Status = gBS->HandleProtocol (
                    gST->ConsoleInHandle,
                    &gEfiSimpleTextInputExProtocolGuid,
                    (VOID **)&SimpleTextInEx
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Update all of ConInEx device State.
      //
      Status = SimpleTextInEx->SetState (SimpleTextInEx, &KeyToggleState);
      if (Status == EFI_SUCCESS) return;
    }
  }
  //
  // Update Keyboard State
  //
  KeyboardData->SimpleInputEx.SetState (&KeyboardData->SimpleInputEx, &KeyToggleState);
}

/**
 Convert WinKey to string

 @param        Buffer           String output buffer
 @param        WinKey           Key value
 @param        Shift            Is Shift pressed

 @retval N/A

**/
VOID
ConvertWinKeyToString (
  CHAR16  *Buffer,
  UINT8   WinKey,
  BOOLEAN Shift
  )
{

  UINTN  Index;
  CHAR16 Key;

  Index = 0;
  while (mConvertWinKeyToStr[Index].WinKey != 0) {
    if (mConvertWinKeyToStr[Index].WinKey == WinKey) {
      StrCpy (Buffer, mConvertWinKeyToStr[Index].Str);
      return;
    }
    Index++;
  }

  Key = ' ';
  for (Index = 0; Index < mKeyConvertionTableNumber; Index++) {
    if (WinKey == mKeyConvertionTable[Index][0]) {
      if (Shift) {
        Key = mKeyConvertionTable[Index][3];
      } else {
        Key = mKeyConvertionTable[Index][2];
      }
      break;
    }
  }

  Buffer[0] = Key;
  Buffer[1] = '\0';
}

/**
 Reset the input device and optionaly run diagnostics

 @param [in]   SimpleTextIn
 @param [in]   ExtendedVerification  Driver may perform diagnostics on reset.

 @retval EFI_SUCCESS            The device was reset.

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardReset (
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL       *SimpleTextIn,
  IN BOOLEAN                              ExtendedVerification
  )
{
  return EFI_SUCCESS;
}


/**
 Reset the input device and optionaly run diagnostics

 @param [in]   SimpleTextInEx
 @param [in]   ExtendedVerification  Driver may perform diagnostics on reset.

 @retval EFI_SUCCESS            The device was reset.

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN BOOLEAN                            ExtendedVerification
  )
{

  return EFI_SUCCESS;

}

/**
 Set certain state for the input device.

 @param [in]   SimpleTextInEx
 @param [in]   KeyToggleState   A pointer to the EFI_KEY_TOGGLE_STATE to set the
                                state for the input device.

 @retval EFI_UNSUPPORTED        Doesn't support SetState

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardSetStateEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{

  EFI_TPL                               OldTpl;
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  Private = mPrivate;
  Status = SetKeybaordToggleState (Private->KeyboardData, KeyToggleState);
  RenderImageForAllGop (Private);
  gBS->RestoreTPL (OldTpl);


  return Status;
}

/**
 Register a notification function for a particular keystroke for the input device.

 @param [in]   SimpleTextInEx
 @param [in]   KeyData          A pointer to a buffer that is filled in with the keystroke
                                information data for the key that was pressed.
 @param [in]   KeyNotificationFunction  Points to the function to be called when the key
                                sequence is typed specified by KeyData.
 @param [out]  NotifyHandle     Points to the unique handle assigned to the registered notification.

 @retval EFI_UNSUPPORTED        Doesn't support registerkey

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardRegisterKeyEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  )
{

  return EFI_UNSUPPORTED;

}

/**
 Remove a registered notification function from a particular keystroke.

 @param [in]   SimpleTextInEx
 @param [in]   NotificationHandle  The handle of the notification function being unregistered.

 @retval EFI_UNSUPPORTED        Doesn't support unregisterkey

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardUnregisterKeyEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN EFI_HANDLE                         NotificationHandle
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Reads the next keystroke from the input device. The WaitForKey Event can
 be used to test for existance of a keystroke via WaitForEvent () call.


 @retval EFI_SUCCESS            The keystroke information was returned.
 @retval EFI_NOT_READY          There was no keystroke data availiable.
 @retval EFI_INVALID_PARAMETER  KeyData is NULL.

**/
EFI_STATUS
SetupMouseKeyboardReadKeyStrokeWorker (
  IN  PRIVATE_MOUSE_DATA        *Private,
  OUT EFI_KEY_DATA              *KeyData
  )
{
  EFI_STATUS                    Status;
  SETUP_MOUSE_KEYBOARD          *KeyboardData;

  ASSERT (Private);
  ASSERT (KeyData);
  ASSERT (Private->KeyboardData);

  ASSERT_LOCKED (&Private->SetupMouseLock);

  Status = EFI_SUCCESS;
  if (!Private->Keyboard.Visible) {
     return EFI_NOT_READY;
  }

  KeyboardData = Private->KeyboardData;
  if (KeyboardData->Key.ScanCode == SCAN_NULL && KeyboardData->Key.UnicodeChar == 0x00) {
    Status = EFI_NOT_READY;
  } else {
    CopyMem (&KeyData->Key, &KeyboardData->Key, sizeof (EFI_INPUT_KEY));
    CopyMem (&KeyData->KeyState, &KeyboardData->KeyState, sizeof (EFI_KEY_STATE));
    KeyboardData->Key.ScanCode    = SCAN_NULL;
    KeyboardData->Key.UnicodeChar = 0x0000;
  }

  return Status;
}


/**
 Reads the next keystroke from the input device. The WaitForKey Event can
 be used to test for existance of a keystroke via WaitForEvent () call.


 @retval EFI_SUCCESS            The keystroke information was returned.
 @retval EFI_NOT_READY          There was no keystroke data availiable.
 @retval EFI_DEVICE_ERROR       The keystroke information was not returned due to
                                hardware errors.
 @retval EFI_INVALID_PARAMETER  KeyData is NULL.

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextIn,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  EFI_STATUS                    Status;
  PRIVATE_MOUSE_DATA            *Private;

  Private = mPrivate;
  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AcquireSetupMouseLock (Private);
  Status = SetupMouseKeyboardReadKeyStrokeWorker (Private, KeyData);
  ReleaseSetupMouseLock (Private);

  return Status;
}

/**
 Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.ReadKeyStroke() function.


 @retval EFI_SUCCESS            Success

**/
EFI_STATUS
EFIAPI
SetupMouseKeyboardReadKeyStroke (
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL      *SimpleTextIn,
  OUT EFI_INPUT_KEY                      *Key
  )
{
  EFI_STATUS                    Status;
  EFI_KEY_DATA                  KeyData;
  PRIVATE_MOUSE_DATA            *Private;

  Private = mPrivate;

  AcquireSetupMouseLock (Private);
  Status = SetupMouseKeyboardReadKeyStrokeWorker (Private, &KeyData);
  ReleaseSetupMouseLock (Private);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Key, &KeyData.Key, sizeof (EFI_INPUT_KEY));
  return EFI_SUCCESS;
}

/**
 Handler function for WaitForKey event.

 @param [in]   Event            Event to be signaled when a key is pressed.
 @param [in]   Context          Points to USB_KB_DEV instance.

 @retval VOID

**/
VOID
EFIAPI
SetupMouseKeyboardWaitForKey (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  SETUP_MOUSE_KEYBOARD     *KeyboardData;

  KeyboardData = (SETUP_MOUSE_KEYBOARD *)Context;

  if (KeyboardData->Key.ScanCode != SCAN_NULL || KeyboardData->Key.UnicodeChar != 0x00) {
    gBS->SignalEvent (Event);
  }
}

/**
 Install SimpleTextIn / SimpleTextInEx

 @param        KeyboardData     SetupMouse keyboard data

 @retval VOID

**/
VOID
SetupMouseKeyboardOpen (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
{
  EFI_STATUS                  Status;

  ASSERT (KeyboardData != NULL);

  KeyboardData->SimpleInput.Reset                 = SetupMouseKeyboardReset;
  KeyboardData->SimpleInput.ReadKeyStroke         = SetupMouseKeyboardReadKeyStroke;

  KeyboardData->SimpleInputEx.Reset               = SetupMouseKeyboardResetEx;
  KeyboardData->SimpleInputEx.ReadKeyStrokeEx     = SetupMouseKeyboardReadKeyStrokeEx;
  KeyboardData->SimpleInputEx.SetState            = SetupMouseKeyboardSetStateEx;
  KeyboardData->SimpleInputEx.RegisterKeyNotify   = SetupMouseKeyboardRegisterKeyEx;
  KeyboardData->SimpleInputEx.UnregisterKeyNotify = SetupMouseKeyboardUnregisterKeyEx;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  SetupMouseKeyboardWaitForKey,
                  KeyboardData,
                  &KeyboardData->SimpleInput.WaitForKey
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  SetupMouseKeyboardWaitForKey,
                  KeyboardData,
                  &KeyboardData->SimpleInputEx.WaitForKeyEx
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  KeyboardData->Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &KeyboardData->Handle,
                  &gEfiDevicePathProtocolGuid,
                  &mVirtualKBDevicePath,
                  &gEfiSimpleTextInProtocolGuid,
                  &KeyboardData->SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &KeyboardData->SimpleInputEx,
                  &gEfiConsoleInDeviceGuid,
                  NULL,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  gSetupMouseKeyboardDriverBinding.DriverBindingHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gSetupMouseKeyboardDriverBinding.DriverBindingHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &gSetupMouseKeyboardDriverBinding,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = gSetupMouseKeyboardDriverBinding.Start (
                                              &gSetupMouseKeyboardDriverBinding,
                                              KeyboardData->Handle,
                                              NULL
                                              );


  Status = gBS->ConnectController (
                  KeyboardData->Handle,
                  NULL,
                  NULL,
                  FALSE
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }
}


/**
 Uninstall SimpleTextIn / SimpleTextInEx

 @param        KeyboardData     SetupMouse keyboard data

 @retval VOID

**/
VOID
SetupMouseKeyboardClose (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
{
  EFI_STATUS Status;
  PRIVATE_MOUSE_DATA     *Private;

  Private = mPrivate;

  if (!Private->Keyboard.Visible) {
    return ;
  }

  gBS->CloseEvent (KeyboardData->SimpleInput.WaitForKey);
  gBS->CloseEvent (KeyboardData->SimpleInputEx.WaitForKeyEx);

  //
  // Disconnect the virtual keyboard device for ConPlatform and ConSplitter
  //
  Status = gBS->DisconnectController (
                  KeyboardData->Handle,
                  NULL,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  //
  // Uninstall the fake SimpleTextIn/Ex protocols
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  KeyboardData->Handle,
                  &gEfiDevicePathProtocolGuid,
                  &mVirtualKBDevicePath,
                  &gEfiSimpleTextInProtocolGuid,
                  &KeyboardData->SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &KeyboardData->SimpleInputEx,
                  &gEfiConsoleInDeviceGuid,
                  NULL,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  &gSetupMouseKeyboardDriverBinding.DriverBindingHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &gSetupMouseKeyboardDriverBinding,
                  NULL
                  );


}


/**
 Process vartual keyboard press key

 @param        KeyboardData     SetupMouse keyboard data

 @retval VOID

**/
VOID
SetupMouseKeyboardProcessKeyDown (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
{
  UINT8                   WinKey;
  UINTN                   Index;
  EFI_INPUT_KEY           *Key;

  //
  // Is this key existent ?
  //
  WinKey = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[KeyboardData->PressIndex].WinKey;
  for (Index = 0; Index < mKeyConvertionTableNumber; Index++) {
    if (mKeyConvertionTable[Index][0] == WinKey) {
      break;
    }
  }

  if (WinKey == 0xC3) {
    InternalCloseKeyboard (&mPrivate->SetupMouse);
    return ;
  }

  ASSERT (Index != mKeyConvertionTableNumber);
  if (Index == mKeyConvertionTableNumber) {
    return;
  }

  //
  // check have ScanCode / Unicode
  //
  Key = &KeyboardData->Key;
  Key->ScanCode = SCAN_NULL;
  Key->UnicodeChar = CHAR_NULL;
  if (mKeyConvertionTable[Index][1] != 0 || mKeyConvertionTable[Index][2] != 0) {
    //
    // send key
    //
    Key->ScanCode = mKeyConvertionTable[Index][1];

    if (KeyboardData->ShiftOn) {
      Key->UnicodeChar = mKeyConvertionTable[Index][3];
      //
      // Need not return associated shift state if a class of printable characters that
      // are normally adjusted by shift modifiers. e.g. Shift Key + 'f' key = 'F'
      //
      if (Key->UnicodeChar != CHAR_NULL || Key->ScanCode != SCAN_NULL) {
        KeyboardData->LeftShiftOn = 0;
        KeyboardData->RightShiftOn = 0;
        SyncKeyboardState (KeyboardData);
      }
    } else {
      Key->UnicodeChar = mKeyConvertionTable[Index][2];
    }

    if (KeyboardData->CapsOn) {
      if (Key->UnicodeChar >= 'a' && Key->UnicodeChar <= 'z') {
        Key->UnicodeChar = mKeyConvertionTable[Index][3];
      } else if (Key->UnicodeChar >= 'A' && Key->UnicodeChar <= 'Z') {
        Key->UnicodeChar = mKeyConvertionTable[Index][2];
      }
    }

    ASSERT (KeyboardData->PressIndex < mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyCount);
    KeyboardData->KeyPosMap[KeyboardData->PressIndex].State = KEY_PRESS;
    return ;
  }

  switch (WinKey) {

  case VK_LSHIFT:
    KeyboardData->LeftShiftOn ^= 1;
    break;

  case VK_RSHIFT:
    KeyboardData->RightShiftOn ^= 1;
    break;

  case VK_CAPITAL:
    KeyboardData->CapsOn ^= 1;
    break;

  default:
    return ;
  }

  SyncKeyboardState (KeyboardData);
  DrawKeyboard (mPrivate);
}

/**
 Filter virtual keyboard evnet accroding mouse / touch evnet position

 @param None

 @retval N/A

**/
EFI_STATUS
ProcessKeyboard (
  VOID
  )
{

  SETUP_MOUSE_KEYBOARD *KeyboardData;
  POINT                Pt;
  UINTN                Index;
  BOOLEAN              NeedUpdateKeyboard;
  BOOLEAN              FoundHover;
  INTN                 OffsetX;
  INTN                 OffsetY;
  PRIVATE_MOUSE_DATA   *Private;
  KEY_POS_MAP          *KeyPosMap;
  KEYBOARD_LAYOUT      *KeyboardLayout;

  Private = mPrivate;

  if (!Private->Keyboard.Visible) {
    return EFI_NOT_READY;
  }

  KeyboardData = Private->KeyboardData;

  Pt.x = (INT32)Private->Cursor.ImageRc.left;
  Pt.y = (INT32)Private->Cursor.ImageRc.top;

  if (KeyboardData->UserFocused) {
    if (!Private->LButton) {
      KeyboardData->UserFocused = FALSE;
    }
    return EFI_NOT_READY;
  }

  if (!PtInRect (&Private->Keyboard.ImageRc, Pt) && (!KeyboardData->Focused)) {
    if (Private->LButton) {
      //
      // avoid user drag mouse in outside
      //
      KeyboardData->UserFocused = TRUE;
    }
    return EFI_NOT_READY;
  }

  //
  // convert to keyboard relative coordinate
  //
  Pt.x -= (INT32)Private->Keyboard.ImageRc.left;
  Pt.y -= (INT32)Private->Keyboard.ImageRc.top;

  NeedUpdateKeyboard = FALSE;
  KeyPosMap = KeyboardData->KeyPosMap;
  KeyboardLayout = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex];

  //
  // Test hover
  //
  FoundHover = FALSE;
  if (KeyboardData->HoverIndex < KeyboardLayout->KeyCount) {
    //
    // Doesn't change hover target
    //
    if (PtInRect (&KeyPosMap[KeyboardData->HoverIndex].Pos, Pt)) {
      FoundHover = TRUE;
    }
  }

  if (!FoundHover) {
    for (Index = 0; Index < KeyboardLayout->KeyCount; Index++) {
      if (PtInRect (&KeyPosMap[Index].Pos, Pt)) {
        //
        // not yet implement hover state
        //
        //NeedUpdateKeyboard = TRUE;
        FoundHover = TRUE;
        break;
      }
    }
    KeyboardData->HoverIndex = Index;
  }

  //
  // LButton Down
  //
  if (Private->LButton && !KeyboardData->Focused) {
    KeyboardData->Focused = TRUE;
    KeyboardData->PressIndex = KeyboardData->HoverIndex;
    if (KeyboardData->PressIndex < KeyboardLayout->KeyCount) {
      //
      // key down
      //
      NeedUpdateKeyboard = TRUE;
      SetupMouseKeyboardProcessKeyDown (KeyboardData);
      if (!Private->Keyboard.Visible) {
        return EFI_SUCCESS;
      }
    } else {
      if (Private->SaveKeyboardAttributes.IsFixedPosition) {
        return EFI_SUCCESS;
      }
      KeyboardData->Draged = TRUE;
      KeyboardData->DragStartPoint.x    = (INT32)Private->Cursor.ImageRc.left;
      KeyboardData->DragStartPoint.y    = (INT32)Private->Cursor.ImageRc.top;
      KeyboardData->DragOriginalPoint.x = (INT32)Private->Keyboard.ImageRc.left;
      KeyboardData->DragOriginalPoint.y = (INT32)Private->Keyboard.ImageRc.top;
      NeedUpdateKeyboard = TRUE;
    }
  }

  //
  // LButton Up
  //
  if (!Private->LButton && KeyboardData->Focused) {
    KeyboardData->Focused = FALSE;
    if (KeyboardData->Draged) {
      KeyboardData->Draged = FALSE;
      NeedUpdateKeyboard = TRUE;
    }
    if (KeyboardData->PressIndex < mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyCount) {
      if (mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[KeyboardData->PressIndex].WinKey != VK_LSHIFT &&
          mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[KeyboardData->PressIndex].WinKey != VK_RSHIFT &&
          mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[KeyboardData->PressIndex].WinKey != VK_CAPITAL) {
        KeyboardData->KeyPosMap[KeyboardData->PressIndex].State = KEY_NORMAL;
      }
      NeedUpdateKeyboard = TRUE;
      KeyboardData->PressIndex = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyCount;
    }
  }

  //
  // Drag keyboard
  //
  if (KeyboardData->Draged) {
    OffsetX = Private->Cursor.ImageRc.left - KeyboardData->DragStartPoint.x + KeyboardData->DragOriginalPoint.x;
    OffsetY = Private->Cursor.ImageRc.top  - KeyboardData->DragStartPoint.y + KeyboardData->DragOriginalPoint.y;
    KeyboardGotoXY (Private, OffsetX, OffsetY);
    return EFI_SUCCESS;
  }

  if (NeedUpdateKeyboard && Private->Keyboard.Visible) {
    DrawKeyboard (Private);
  }

  if (KeyboardData->Focused) {
    return EFI_SUCCESS;
  }

  return EFI_NOT_READY;
}



/**
 Draw keyboard image

 @param [in]   Private          Setup mouse private data

 @retval EFI_SUCCESS            Draw keyboard image success

**/
EFI_STATUS
DrawKeyboard (
  IN PRIVATE_MOUSE_DATA                    *Private
  )
{

  RECT                          ImageRc;
  SETUP_MOUSE_KEYBOARD          *KeyboardData;
  UINTN                         Index;
  CHAR16                        KeyString[100];
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BkColor;
  KEYBOARD_LAYOUT               *KeyboardLayout;
  UINTN                         Width;
  UINTN                         Height;
  KEY_STYLE_ARRAY               *StyleList;
  KEY_STYLE_ENTRY               *Style;
  KEY_POS_MAP                   *KeyPosMap;
  KEY_STATE                     KeyState;
  KEY_MAP_ENTRY                 *KeyList;
  RECT                          TextRc;

  ASSERT (Private->Keyboard.Visible);

  KeyboardData   = Private->KeyboardData;
  KeyboardLayout = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex];
  StyleList      = KeyboardLayout->StyleList;
  KeyPosMap      = KeyboardData->KeyPosMap;
  KeyList        = KeyboardLayout->KeyList;

  Width  = (UINTN)(Private->Keyboard.ImageRc.right  - Private->Keyboard.ImageRc.left);
  Height = (UINTN)(Private->Keyboard.ImageRc.bottom - Private->Keyboard.ImageRc.top);

  //
  // draw keyboard background
  //
  SetRect (&ImageRc, 0, 0, (INT32)Width, (INT32)Height);
  SetupMouseShowBitmap (&ImageRc, &KeyboardLayout->BackgroundScaleGrid, KeyboardLayout->BackgroundImageId);

  //
  // draw key and text
  //
  ZeroMem (&BkColor, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  for (Index = 0; Index < KeyboardLayout->KeyCount; Index++) {
    Style    = StyleList[KeyboardLayout->KeyList[Index].StyleId];
    KeyState = KeyPosMap[Index].State;
    SetupMouseShowBitmap (
      &KeyPosMap[Index].Pos,
      &Style[KeyState].ImageScaleGrid,
      Style[KeyState].ImageId
      );

//    if (KeyboardLayout->KeyList[Index].StyleId != STYLE_NORMAL) {
//      continue;
//    }

    if (KeyList[Index].WinKey >= 'A' && KeyList[Index].WinKey <= 'Z') {
      ConvertWinKeyToString (KeyString, KeyList[Index].WinKey, KeyboardData->ShiftOn ^ KeyboardData->CapsOn);
    } else {
      ConvertWinKeyToString (KeyString, KeyList[Index].WinKey, KeyboardData->ShiftOn);
    }
    CopyMem (&TextRc, &KeyPosMap[Index].Pos, sizeof (RECT));
    SetupMouseDrawText (
      &Private->Keyboard,
      &Style[KeyState].TextColor,
      &BkColor,
      KeyString,
      &TextRc
      );
  }

  InvalidateImage (Private, &Private->Keyboard);

  return EFI_SUCCESS;
}

/**
 Move the keybaord to a particular point indicated by the X, Y axis.

 @param [in]   Private          Setup mouse private data
 @param [in]   X                & Y  indicates location to move to.
 @param [in]   Y

 @retval None

**/
EFI_STATUS
KeyboardGotoXY (
  IN  PRIVATE_MOUSE_DATA                   *Private,
  IN  INTN                                 X,
  IN  INTN                                 Y
  )
{
  UINTN Width;
  UINTN Height;

  Width  = Private->Keyboard.ImageRc.right - Private->Keyboard.ImageRc.left;
  Height = Private->Keyboard.ImageRc.bottom - Private->Keyboard.ImageRc.top;

  if (X <= (INTN) Private->MouseRange.StartX) {
    X = (INTN) Private->MouseRange.StartX;
  }

  if ((INTN)(X + Width) > (INTN)Private->MouseRange.EndX) {
    X = (INTN)(Private->MouseRange.EndX - Width);
  }

  if (Y <= (INTN) Private->MouseRange.StartY) {
    Y = (INTN) Private->MouseRange.StartY;
  }

  if ((INTN)(Y + Height) > (INTN) Private->MouseRange.EndY) {
    Y = Private->MouseRange.EndY - Height;
  }

  MoveImage (&Private->Keyboard, X, Y);

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeKeyboard (
  IN  PRIVATE_MOUSE_DATA                *Private,
  IN  SETUP_MOUSE_KEYBOARD              *KeyboardData,
  IN  UINTN                             KeyMapIndex
  )
{
  UINTN                                 Width;
  UINTN                                 Height;
  UINT32                                XScale;
  UINT32                                YScale;
  UINTN                                 Index;

  ASSERT (KeyMapIndex < mKeyboardLayoutCount);

  if (KeyMapIndex >= mKeyboardLayoutCount) {
    return EFI_INVALID_PARAMETER;
  }


  KeyboardData->KeyboardLayoutIndex = KeyMapIndex;
  if (KeyboardData->KeyPosMap != NULL) {
    gBS->FreePool (KeyboardData->KeyPosMap);
  }

  KeyboardData->HoverIndex  = INVALID_KEY_INDEX;
  KeyboardData->PressIndex  = INVALID_KEY_INDEX;
  KeyboardData->KeyPosMap   = AllocateZeroPool (sizeof (KEY_POS_MAP) * mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyCount);
  if (KeyboardData->KeyPosMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Width  = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->Width;
  Height = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->Height;

  if (Private->SaveKeyboardAttributes.WidthPercentage == 0) {
    Private->SaveKeyboardAttributes.WidthPercentage = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->WidthPercentage;
  }
  if (Private->SaveKeyboardAttributes.WidthPercentage != 0) {
    Width = ((Private->MouseRange.EndX - Private->MouseRange.StartX) * Private->SaveKeyboardAttributes.WidthPercentage) / 100;
  }

  if (Private->SaveKeyboardAttributes.HeightPercentage == 0) {
    Private->SaveKeyboardAttributes.HeightPercentage = mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->HeightPercentage;
  }
  if (Private->SaveKeyboardAttributes.HeightPercentage != 0) {
    Height = ((Private->MouseRange.EndY - Private->MouseRange.StartY) * Private->SaveKeyboardAttributes.HeightPercentage) / 100;
  }

  if (Width >= (Private->MouseRange.EndX - Private->MouseRange.StartX)) {
    Width = Private->MouseRange.EndX - Private->MouseRange.StartX;
  }

  if (Height >= (Private->MouseRange.EndY - Private->MouseRange.StartY)) {
    Height = Private->MouseRange.EndY - Private->MouseRange.StartY;
  }

  //
  // calculate all key size
  //
  XScale = (UINT32)((Width << 16)  / mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->Width);
  YScale = (UINT32)((Height << 16) / mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->Height);

  for (Index = 0; Index < mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyCount; Index++) {
    KeyboardData->KeyPosMap[Index].Pos.left   = ((XScale * mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[Index].Pos.left )   >> 16);
    KeyboardData->KeyPosMap[Index].Pos.right  = ((XScale * mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[Index].Pos.right )  >> 16);
    KeyboardData->KeyPosMap[Index].Pos.top    = ((YScale * mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[Index].Pos.top )    >> 16);
    KeyboardData->KeyPosMap[Index].Pos.bottom = ((YScale * mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyList[Index].Pos.bottom ) >> 16);
  }

  if (Private->Keyboard.Image != NULL) {
    gBS->FreePool (Private->Keyboard.Image);
  }

  Private->Keyboard.Image = AllocateZeroPool (Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (Private->Keyboard.Image == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetRect (
    &Private->Keyboard.ImageRc,
    0,
    0,
    (INT32)Width,
    (INT32)Height
    );

  return EFI_SUCCESS;
}



/**
 KeyboardData function restores data from blt buffer to the screen.

 @param [in]   SetupMouse
 @param [in]   X
 @param [in]   Y

 @retval EFI_SUCCESS            Screen has been restored from blt buffer successfully

**/
EFI_STATUS
InternalStartKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  IN  UINTN                             X,
  IN  UINTN                             Y
  )
{
  PRIVATE_MOUSE_DATA                    *Private;
  SETUP_MOUSE_KEYBOARD                  *KeyboardData;
  EFI_STATUS                            Status;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);
  ASSERT_LOCKED (&Private->SetupMouseLock);

  KeyboardData  = Private->KeyboardData;

  if (KeyboardData != NULL) {
    KeyboardGotoXY (Private, X, Y);
    return EFI_SUCCESS;
  }

  if (IsListEmpty (&Private->GopList)) {
    return EFI_NOT_READY;
  }

  KeyboardData = AllocateZeroPool (sizeof (SETUP_MOUSE_KEYBOARD));
  if (KeyboardData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Private->KeyboardData = KeyboardData;

  Status = InitializeKeyboard (Private, KeyboardData, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ShowImage (&Private->Keyboard);
  DrawKeyboard (Private);
  KeyboardGotoXY (Private, X, Y);
  RenderImageForAllGop (Private);
  SetupMouseKeyboardOpen (KeyboardData);

  return EFI_SUCCESS;
}




/**
 KeyboardData function restores data from blt buffer to the screen.

 @param [in]   SetupMouse

 @retval EFI_SUCCESS            Screen has been restored from blt buffer successfully

**/
EFI_STATUS
InternalCloseKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  )
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);
  ASSERT_LOCKED (&Private->SetupMouseLock);

  SetupMouseKeyboardClose (Private->KeyboardData);
  HideImage (&Private->Keyboard);
  RenderImageForAllGop (Private);
  DestroyImage (&Private->Keyboard);

  if (Private->KeyboardData != NULL) {
    if (Private->KeyboardData->KeyPosMap != NULL) {
      FreePool (Private->KeyboardData->KeyPosMap);
    }

    FreePool (Private->KeyboardData);
    Private->KeyboardData = NULL;
  }

  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
InternalSetupMouseSetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardAttributes
  )
{
  PRIVATE_MOUSE_DATA                    *Private;
  INTN                                  X;
  INTN                                  Y;

  ASSERT (KeyboardAttributes != NULL);

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  ASSERT_LOCKED (&Private->SetupMouseLock);


  X = (UINTN)Private->Keyboard.ImageRc.left;
  Y = (UINTN)Private->Keyboard.ImageRc.top;

  if (KeyboardAttributes->Flags & KBCF_X) {
    X = (INTN) KeyboardAttributes->X;
  }
  if (KeyboardAttributes->Flags & KBCF_Y) {
    Y = (INTN) KeyboardAttributes->Y;
  }
  if ((KeyboardAttributes->Flags & KBCF_X) || (KeyboardAttributes->Flags & KBCF_Y)) {
    if (Private->Keyboard.Visible) {
      KeyboardGotoXY (Private, X, Y);
    }
  }

  if (KeyboardAttributes->Flags & KBCF_WIDTH_PERCENTAGE) {
    Private->SaveKeyboardAttributes.WidthPercentage = KeyboardAttributes->WidthPercentage;
  }
  if (KeyboardAttributes->Flags & KBCF_HEIGHT_PERCENTAGE) {
    Private->SaveKeyboardAttributes.HeightPercentage = KeyboardAttributes->HeightPercentage;
  }

  if ((KeyboardAttributes->Flags & KBCF_WIDTH_PERCENTAGE) || (KeyboardAttributes->Flags & KBCF_HEIGHT_PERCENTAGE)) {
    if (Private->Keyboard.Visible) {
      InternalCloseKeyboard (SetupMouse);
      InternalStartKeyboard (SetupMouse, X, Y);
    }
  }

  if (KeyboardAttributes->Flags & KBCF_FIXED_POSITION) {
    Private->SaveKeyboardAttributes.IsFixedPosition = KeyboardAttributes->IsFixedPosition;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InternalSetupMouseGetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardAttributes
  )
{
  PRIVATE_MOUSE_DATA                    *Private;

  ASSERT (KeyboardAttributes != NULL);

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  ASSERT_LOCKED (&Private->SetupMouseLock);


  KeyboardAttributes->Flags            = KBCF_X | KBCF_Y | KBCF_WIDTH_PERCENTAGE | KBCF_HEIGHT_PERCENTAGE | KBCF_FIXED_POSITION;
  KeyboardAttributes->IsStart          = Private->Keyboard.Visible;
  KeyboardAttributes->X                = (INT32)Private->Keyboard.ImageRc.left;
  KeyboardAttributes->Y                = (INT32)Private->Keyboard.ImageRc.top;
  KeyboardAttributes->WidthPercentage  = KeyboardAttributes->WidthPercentage;
  KeyboardAttributes->HeightPercentage = KeyboardAttributes->HeightPercentage;
  KeyboardAttributes->IsFixedPosition  = Private->SaveKeyboardAttributes.IsFixedPosition;

  return EFI_SUCCESS;
}

