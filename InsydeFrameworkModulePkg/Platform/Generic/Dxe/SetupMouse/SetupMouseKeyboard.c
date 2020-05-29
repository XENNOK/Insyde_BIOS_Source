//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   SetupMouseKeyboard.c
//;
//; Abstract:
//;
//;   Setup Mouse Keyboard implementation
//;
//;
#include "SetupMouse.h"
#include EFI_PROTOCOL_DEFINITION (SimpleTextIn)
#include EFI_PROTOCOL_DEFINITION (SimpleTextInputEx)
#include EFI_PROTOCOL_DEFINITION (HiiFont)
#include EFI_GUID_DEFINITION (HotPlugDevice)
#include EFI_PROTOCOL_DEFINITION (GifDecoder)
#include "TianoHii.h"
#include "KeyboardLayout.h"
#include "SetupMouseAniDefs.h"

#define INVALID_KEY_INDEX (UINTN)(-1)


EFI_GIF_DECODER_PROTOCOL             *mGifDecoder  = NULL;
EFI_HII_FONT_PROTOCOL                *mHiiFont;
EFI_HII_ANIMATION_PACKAGE_HDR        *mAnimationPackage;

extern unsigned char                 SetupMouseAnimations[];
extern KEYBOARD_LAYOUT               *mKeyboardLayoutList[];
extern UINTN                         mKeyboardLayoutCount;
extern KEY_MAP_STR                   mConvertWinKeyToStr[];
extern UINT8                         mKeyConvertionTable[][4];
extern UINT16                        mKeyConvertionTableNumber;


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


typedef struct _SETUP_MOUSE_KEYBOARD {
  EFI_HANDLE                          Handle;
  BOOLEAN                             UpdateNeeded;
  EFI_SIMPLE_TEXT_IN_PROTOCOL         SimpleInput;
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
} SETUP_MOUSE_KEYBOARD;

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


VOID
ConvertWinKeyToString (
  CHAR16  *Buffer,
  UINT8   WinKey,
  BOOLEAN Shift
  )
/*++

  Routine Description:

    Convert WinKey to string

  Arguments:

    Buffer                - String output buffer
    WinKey                - Key value
    Shift                 - Is Shift pressed

  Returns:

    N/A

--*/
{

  UINTN  Index;
  CHAR16 Key;

  Index = 0;
  while (mConvertWinKeyToStr[Index].WinKey != 0) {
    if (mConvertWinKeyToStr[Index].WinKey == WinKey) {
      EfiStrCpy (Buffer, mConvertWinKeyToStr[Index].Str);
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

EFI_STATUS
SetupMouseKeyboardReset (
  IN EFI_SIMPLE_TEXT_IN_PROTOCOL          *SimpleTextIn,
  IN BOOLEAN                              ExtendedVerification
  )
/*++

  Routine Description:
    Reset the input device and optionaly run diagnostics

  Arguments:
    KeyboardData          - Protocol instance pointer.
    ExtendedVerification  - Driver may perform diagnostics on reset.

  Returns:
    EFI_SUCCESS           - The device was reset.

--*/
{
  return EFI_SUCCESS;
}


EFI_STATUS
SetupMouseKeyboardResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN BOOLEAN                            ExtendedVerification
  )
/*++

  Routine Description:
    Reset the input device and optionaly run diagnostics

  Arguments:
    This                 - Protocol instance pointer.
    ExtendedVerification - Driver may perform diagnostics on reset.

  Returns:
    EFI_SUCCESS          - The device was reset.

--*/
{

  return EFI_SUCCESS;

}

EFI_STATUS
SetupMouseKeyboardSetStateEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
/*++

  Routine Description:
    Set certain state for the input device.

  Arguments:
    This                  - Protocol instance pointer.
    KeyToggleState        - A pointer to the EFI_KEY_TOGGLE_STATE to set the
                            state for the input device.

  Returns:
    EFI_UNSUPPORTED       - Doesn't support SetState

--*/
{

  EFI_TPL                               OldTpl;
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;

  OldTpl = gBS->RaiseTPL (EFI_TPL_NOTIFY);

  Private = mPrivate;
  Status = SetKeybaordToggleState (Private->KeyboardData, KeyToggleState);
  RenderImageForAllGop (Private);
  gBS->RestoreTPL (OldTpl);


  return Status;
}

EFI_STATUS
SetupMouseKeyboardRegisterKeyEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  )
/*++

  Routine Description:
    Register a notification function for a particular keystroke for the input device.

  Arguments:
    This                    - Protocol instance pointer.
    KeyData                 - A pointer to a buffer that is filled in with the keystroke
                              information data for the key that was pressed.
    KeyNotificationFunction - Points to the function to be called when the key
                              sequence is typed specified by KeyData.
    NotifyHandle            - Points to the unique handle assigned to the registered notification.

  Returns:
    EFI_UNSUPPORTED         - Doesn't support registerkey

--*/
{

  return EFI_UNSUPPORTED;

}

EFI_STATUS
SetupMouseKeyboardUnregisterKeyEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx,
  IN EFI_HANDLE                         NotificationHandle
  )
/*++

  Routine Description:
    Remove a registered notification function from a particular keystroke.

  Arguments:
    This                    - Protocol instance pointer.
    NotificationHandle      - The handle of the notification function being unregistered.

  Returns:
    EFI_UNSUPPORTED         - Doesn't support unregisterkey

--*/
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
SetupMouseKeyboardReadKeyStrokeWorker (
  IN  SETUP_MOUSE_KEYBOARD              *KeyboardData,
  OUT EFI_KEY_DATA                      *KeyData
  )
/*++

  Routine Description:
    Reads the next keystroke from the input device. The WaitForKey Event can
    be used to test for existance of a keystroke via WaitForEvent () call.

  Arguments:
    KeyboardData          - SetupMouse keyboard data
    KeyData               - A pointer to a buffer that is filled in with the keystroke
                            state data for the key that was pressed.

  Returns:
    EFI_SUCCESS           - The keystroke information was returned.
    EFI_NOT_READY         - There was no keystroke data availiable.
    EFI_INVALID_PARAMETER - KeyData is NULL.

--*/
{

  EFI_TPL                               OldTpl;
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;


  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = EFI_SUCCESS;

  Private = mPrivate;

  OldTpl = gBS->RaiseTPL (EFI_TPL_NOTIFY);

  if (!Private->Keyboard.Visible) {
    gBS->RestoreTPL (OldTpl);
    return EFI_NOT_READY;
  }

  if (KeyboardData->Key.ScanCode == SCAN_NULL && KeyboardData->Key.UnicodeChar == 0x00) {
    Status = EFI_NOT_READY;
  } else {
    EfiCopyMem (&KeyData->Key, &KeyboardData->Key, sizeof (EFI_INPUT_KEY));
    EfiCopyMem (&KeyData->KeyState, &KeyboardData->KeyState, sizeof (EFI_KEY_STATE));
    KeyboardData->Key.ScanCode    = SCAN_NULL;
    KeyboardData->Key.UnicodeChar = 0x0000;
  }

  gBS->RestoreTPL (OldTpl);

  return Status;
}


EFI_STATUS
SetupMouseKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextIn,
  OUT EFI_KEY_DATA                      *Key
  )
/*++

  Routine Description:
    Reads the next keystroke from the input device. The WaitForKey Event can
    be used to test for existance of a keystroke via WaitForEvent () call.

  Arguments:
    This       - Protocol instance pointer.
    KeyData    - A pointer to a buffer that is filled in with the keystroke
                 state data for the key that was pressed.

  Returns:
    EFI_SUCCESS           - The keystroke information was returned.
    EFI_NOT_READY         - There was no keystroke data availiable.
    EFI_DEVICE_ERROR      - The keystroke information was not returned due to
                            hardware errors.
    EFI_INVALID_PARAMETER - KeyData is NULL.

--*/
{
  EFI_STATUS              Status;
  EFI_KEY_DATA            KeyData;
  SETUP_MOUSE_KEYBOARD    *KeyboardData;
  PRIVATE_MOUSE_DATA      *Private;

  Private = mPrivate;

  if (!Private->Keyboard.Visible) {
    return EFI_NOT_READY;
  }

  KeyboardData =  Private->KeyboardData;
  Status = SetupMouseKeyboardReadKeyStrokeWorker (KeyboardData, &KeyData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EfiCopyMem (Key, &KeyData.Key, sizeof (EFI_INPUT_KEY));
  return EFI_SUCCESS;
}

EFI_STATUS
SetupMouseKeyboardReadKeyStroke (
  IN EFI_SIMPLE_TEXT_IN_PROTOCOL          *SimpleTextIn,
  OUT EFI_INPUT_KEY                       * Key
  )
/*++

  Routine Description:
    Implements EFI_SIMPLE_TEXT_IN_PROTOCOL.ReadKeyStroke() function.

  Arguments:
    This     The EFI_SIMPLE_TEXT_IN_PROTOCOL instance.
    Key      A pointer to a buffer that is filled in with the keystroke
             information for the key that was pressed.

  Returns:
    EFI_SUCCESS - Success

--*/
{
  EFI_STATUS              Status;
  SETUP_MOUSE_KEYBOARD     *KeyboardData;
  EFI_KEY_DATA            KeyData;
  PRIVATE_MOUSE_DATA      *Private;

  Private = mPrivate;

  if (!Private->Keyboard.Visible) {
    return EFI_NOT_READY;
  }

  KeyboardData =  Private->KeyboardData;
  Status = SetupMouseKeyboardReadKeyStrokeWorker (KeyboardData, &KeyData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EfiCopyMem (Key, &KeyData.Key, sizeof (EFI_INPUT_KEY));
  return EFI_SUCCESS;
}

VOID
EFIAPI
SetupMouseKeyboardWaitForKey (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
/*++

  Routine Description:
    Handler function for WaitForKey event.

  Arguments:
    Event        Event to be signaled when a key is pressed.
    Context      Points to USB_KB_DEV instance.

  Returns:
    VOID
--*/
{
  SETUP_MOUSE_KEYBOARD     *KeyboardData;

  KeyboardData = (SETUP_MOUSE_KEYBOARD *)Context;

  if (KeyboardData->Key.ScanCode != SCAN_NULL || KeyboardData->Key.UnicodeChar != 0x00) {
    gBS->SignalEvent (Event);
  }
}

VOID
SetupMouseKeyboardOpen (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
/*++

  Routine Description:
    Install SimpleTextIn / SimpleTextInEx

  Arguments:
    KeyboardData  - SetupMouse keyboard data

  Returns:
    VOID
--*/
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
                  EFI_EVENT_NOTIFY_WAIT,
                  EFI_TPL_NOTIFY,
                  SetupMouseKeyboardWaitForKey,
                  KeyboardData,
                  &KeyboardData->SimpleInput.WaitForKey
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_WAIT,
                  EFI_TPL_NOTIFY,
                  SetupMouseKeyboardWaitForKey,
                  KeyboardData,
                  &KeyboardData->SimpleInputEx.WaitForKeyEx
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &KeyboardData->Handle,
                  &gEfiDevicePathProtocolGuid,
                  &mVirtualKBDevicePath,
                  &gEfiSimpleTextInProtocolGuid,
                  &KeyboardData->SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &KeyboardData->SimpleInputEx,
                  &gEfiHotPlugDeviceGuid,
                  NULL,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

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


VOID
SetupMouseKeyboardClose (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
/*++

  Routine Description:
    Uninstall SimpleTextIn / SimpleTextInEx

  Arguments:
    KeyboardData  - SetupMouse keyboard data

  Returns:
    VOID
--*/
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
                  &gEfiHotPlugDeviceGuid,
                  NULL,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return ;
  }

}


VOID
SetupMouseKeyboardProcessKeyDown (
  SETUP_MOUSE_KEYBOARD *KeyboardData
  )
/*++

  Routine Description:
    Process vartual keyboard press key

  Arguments:
    KeyboardData  - SetupMouse keyboard data

  Returns:
    VOID
--*/
{
  UINT8       WinKey;
  UINTN       Index;
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
    CloseKeyboard (&mPrivate->SetupMouse);
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

EFI_STATUS
ProcessKeyboard (
  VOID
  )
/*++

  Routine Description:

    Filter virtual keyboard evnet accroding mouse / touch evnet position

  Arguments:

    N/A

  Returns:

    N/A

--*/
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
        NeedUpdateKeyboard = TRUE;
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



EFI_STATUS
DrawKeyboard (
  IN PRIVATE_MOUSE_DATA                    *Private
  )
/*++

  Routine Description:
    Draw keyboard image

  Arguments:

    Private                     - Setup mouse private data

  Returns:

    EFI_SUCCESS                 - Draw keyboard image success

--*/
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
  EfiZeroMem (&BkColor, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  for (Index = 0; Index < KeyboardLayout->KeyCount; Index++) {
    Style    = StyleList[KeyboardLayout->KeyList[Index].StyleId];
    KeyState = KeyPosMap[Index].State;
    SetupMouseShowBitmap (
      &KeyPosMap[Index].Pos,
      &Style[KeyState].ImageScaleGrid,
      Style[KeyState].ImageId
      );

    if (KeyList[Index].WinKey >= 'A' && KeyList[Index].WinKey <= 'Z') {
      ConvertWinKeyToString (KeyString, KeyList[Index].WinKey, KeyboardData->ShiftOn ^ KeyboardData->CapsOn);
    } else {
      ConvertWinKeyToString (KeyString, KeyList[Index].WinKey, KeyboardData->ShiftOn);
    }
    EfiCopyMem (&TextRc, &KeyPosMap[Index].Pos, sizeof (RECT));
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

EFI_STATUS
KeyboardGotoXY (
  IN  PRIVATE_MOUSE_DATA                   *Private,
  IN  INTN                                 X,
  IN  INTN                                 Y
  )
/*++

Routine Description:

  Move the keybaord to a particular point indicated by the X, Y axis.

Arguments:

  Private                     - Setup mouse private data
  X & Y - indicates location to move to.

Returns:

 None

--*/
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
  KeyboardData->KeyPosMap   = EfiLibAllocateZeroPool (sizeof (KEY_POS_MAP) * mKeyboardLayoutList[KeyboardData->KeyboardLayoutIndex]->KeyCount);


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

  Private->Keyboard.Image = EfiLibAllocateZeroPool (Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  SetRect (
    &Private->Keyboard.ImageRc,
    0,
    0,
    (INT32)Width,
    (INT32)Height
    );

  return 0;
}



EFI_STATUS
StartKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  IN  UINTN                             X,
  IN  UINTN                             Y
  )
/*++

Routine Description:

  KeyboardData function restores data from blt buffer to the screen.

Arguments:

  None

Returns:
  EFI_SUCCESS - Screen has been restored from blt buffer successfully

--*/
{
  PRIVATE_MOUSE_DATA                    *Private;
  SETUP_MOUSE_KEYBOARD                  *KeyboardData;
  EFI_TPL                               OriginalTPL;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);
  OriginalTPL = gBS->RaiseTPL (EFI_TPL_NOTIFY);

  KeyboardData  = Private->KeyboardData;

  if (Private->ScreenMode == EfiSetupMouseScreenText) {
    X = Private->MouseRange.StartX + X * GLYPH_WIDTH;
    Y = Private->MouseRange.StartY + Y * GLYPH_HEIGHT;
  }

  if (KeyboardData != NULL) {
    KeyboardGotoXY (Private, X, Y);
    gBS->RestoreTPL (OriginalTPL);
    return EFI_SUCCESS;
  }

  if (Private->GopCount == 0) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_NOT_READY;
  }

  gBS->LocateProtocol (&gEfiHiiFontProtocolGuid, NULL, &mHiiFont);
  gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, &mGifDecoder);
  ASSERT (mHiiFont != NULL && mGifDecoder != NULL);

  mAnimationPackage    = (EFI_HII_ANIMATION_PACKAGE_HDR *)(SetupMouseAnimations + 4);

  KeyboardData = EfiLibAllocateZeroPool (sizeof (SETUP_MOUSE_KEYBOARD));
  if (KeyboardData == NULL) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_OUT_OF_RESOURCES;
  }
  Private->KeyboardData = KeyboardData;

  InitializeKeyboard (Private, KeyboardData, 0);

  ShowImage (&Private->Keyboard);
  DrawKeyboard (Private);
  KeyboardGotoXY (Private, X, Y);
  RenderImageForAllGop (Private);
  SetupMouseKeyboardOpen (KeyboardData);

  gBS->RestoreTPL (OriginalTPL);

  return EFI_SUCCESS;
}

EFI_STATUS
CloseKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  )
/*++

Routine Description:

  KeyboardData function restores data from blt buffer to the screen.

Arguments:

  None

Returns:
  EFI_SUCCESS - Screen has been restored from blt buffer successfully

--*/
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;
  EFI_TPL                               OriginalTPL;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  OriginalTPL = gBS->RaiseTPL (EFI_TPL_NOTIFY);
  if (!Private->Keyboard.Visible) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_SUCCESS;
  }


  SetupMouseKeyboardClose (Private->KeyboardData);
  HideImage (&Private->Keyboard);
  RenderImageForAllGop (Private);
  DestroyImage (&Private->Keyboard);

  if (Private->KeyboardData != NULL) {
    if (Private->KeyboardData->KeyPosMap != NULL) {
      gBS->FreePool (Private->KeyboardData->KeyPosMap);
    }

    gBS->FreePool (Private->KeyboardData);
    Private->KeyboardData = NULL;
  }

  Status = EFI_SUCCESS;

  gBS->RestoreTPL (OriginalTPL);
  return Status;
}

EFI_STATUS
SetupMouseGetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardAttributes
  )
{
  PRIVATE_MOUSE_DATA                    *Private;

  ASSERT (KeyboardAttributes != NULL);

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  KeyboardAttributes->Flags            = KBCF_X | KBCF_Y | KBCF_WIDTH_PERCENTAGE | KBCF_HEIGHT_PERCENTAGE | KBCF_FIXED_POSITION;
  KeyboardAttributes->IsStart          = Private->Keyboard.Visible;
  KeyboardAttributes->X                = (INT32)Private->Keyboard.ImageRc.left;
  KeyboardAttributes->Y                = (INT32)Private->Keyboard.ImageRc.top;
  KeyboardAttributes->WidthPercentage  = KeyboardAttributes->WidthPercentage;
  KeyboardAttributes->HeightPercentage = KeyboardAttributes->HeightPercentage;
  KeyboardAttributes->IsFixedPosition  = Private->SaveKeyboardAttributes.IsFixedPosition;

  return EFI_SUCCESS;
}

EFI_STATUS
SetupMouseSetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardAttributes
  )
{
  PRIVATE_MOUSE_DATA                    *Private;
  INTN                                  X;
  INTN                                  Y;
  EFI_TPL                               OriginalTPL;

  ASSERT (KeyboardAttributes != NULL);

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  OriginalTPL = gBS->RaiseTPL (EFI_TPL_NOTIFY);


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
  gBS->RestoreTPL (OriginalTPL);


  if (KeyboardAttributes->Flags & KBCF_WIDTH_PERCENTAGE) {
    Private->SaveKeyboardAttributes.WidthPercentage = KeyboardAttributes->WidthPercentage;
  }
  if (KeyboardAttributes->Flags & KBCF_HEIGHT_PERCENTAGE) {
    Private->SaveKeyboardAttributes.HeightPercentage = KeyboardAttributes->HeightPercentage;
  }

  if ((KeyboardAttributes->Flags & KBCF_WIDTH_PERCENTAGE) || (KeyboardAttributes->Flags & KBCF_HEIGHT_PERCENTAGE)) {
    if (Private->Keyboard.Visible) {
      CloseKeyboard (SetupMouse);
      StartKeyboard (SetupMouse, X, Y);
    }
  }

  if (KeyboardAttributes->Flags & KBCF_FIXED_POSITION) {
    Private->SaveKeyboardAttributes.IsFixedPosition = KeyboardAttributes->IsFixedPosition;
  }

  return EFI_SUCCESS;
}

