/** @file
  I2c Keyboard Driver

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Efikey.h"
#include "I2cKeyboard.h"

EFI_GUID gSimpleTextInExNotifyGuid = { \
  0x856f2def, 0x4e93, 0x4d6b, 0x94, 0xce, 0x1c, 0xfe, 0x47, 0x1, 0x3e, 0xa5 \
};

EFI_GUID gEfiLegacyBiosProtocolGuid = { \
  0xdb9a1e3d, 0x45cb, 0x4abb, 0x85, 0x3b, 0xe5, 0x38, 0x7f, 0xdb, 0x2e, 0x2d \
};

/**
  Get HID Descriptor

  @param [in]  I2cIo            Protocol instance pointer.
  @param [in]  HidDescReg       HID descriptor register
  @param [out] HidDescriptor    Point to HID Descriptor

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_DEVICE_ERROR      No ack for this command

**/
STATIC
EFI_STATUS
GetHidDescriptor (
  IN   EFI_I2C_IO_PROTOCOL      *I2cIo,
  IN   UINT32                   HidDescReg,
  OUT  HID_DESCRIPTOR           *HidDescriptor
  )
{
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         Buffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)Buffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof(UINT16);
  Operation[0].Buffer           = (UINT8*)&HidDescReg;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof(HID_DESCRIPTOR);
  Operation[1].Buffer           = (UINT8*)HidDescriptor;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    RequestPacket,
                    NULL
                    );
  return Status;
}

/**
  Get Report Descriptor

  @param [in]  I2cIo            Protocol instance pointer.
  @param [in]  HidDescriptor    Point to HID Descriptor
  @param [out] ReportDesc       Point to Report Descriptor

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_DEVICE_ERROR      No ack for this command
  @retval EFI_INVALID_PARAMETER ReportDesc == NULL
  @retval EFI_OUT_OF_RESOURCES  Memory Overflow
  
**/
STATIC
EFI_STATUS
GetReportDescriptor (
  IN   EFI_I2C_IO_PROTOCOL      *I2cIo,
  IN   HID_DESCRIPTOR           *HidDescriptor,  
  OUT  UINT8                    **ReportDesc
  )
{
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         Buffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  UINT16                        ReportDescReg;
  EFI_STATUS                    Status;
  
  if (ReportDesc == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  *ReportDesc = AllocateZeroPool (HidDescriptor->ReportDescLength);

  if (*ReportDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get the Report Descriptor
  //
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)Buffer;
  Operation                     = RequestPacket->Operation;
  ReportDescReg                 = HidDescriptor->ReportDescRegister;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof(UINT16);
  Operation[0].Buffer           = (UINT8*)&ReportDescReg;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = HidDescriptor->ReportDescLength;
  Operation[1].Buffer           = *ReportDesc;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Set the power state
  
  @param  KbDev                 Keyboard instance
  @param  PowerState            Power state

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
SetPower (
  IN  I2C_KB_DEV                *KbDev,
  IN  UINT16                    PowerState
  )
{
  UINT16                        Request[2];
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;

  if ((KbDev == NULL) ||
      ((I2cIo = KbDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Set the power state
  //
  Request[0] = KbDev->HidDescriptor.CommandRegister;
  Request[1] = SET_POWER | PowerState;
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = 0;
  RequestPacket.Operation[0].LengthInBytes = 4;
  RequestPacket.Operation[0].Buffer        = (UINT8*)Request;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Reset the device
  
  @param  KbDev                 KeyBoard instance

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
ResetDevice (
  IN  I2C_KB_DEV                *KbDev
  )
{
  UINT16                        Request[2];
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  
  if ((KbDev == NULL) ||
      ((I2cIo = KbDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Reset the device
  //
  Request[0] = KbDev->HidDescriptor.CommandRegister;
  Request[1] = RESET;
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = 0;
  RequestPacket.Operation[0].LengthInBytes = 4;
  RequestPacket.Operation[0].Buffer        = (UINT8*)Request;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Receive the status with flush the transmission buffer 
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = 2;
  RequestPacket.Operation[0].Buffer        = (UINT8*)Request;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Flush data buffer before operation
  
  @param  KeyboardDev           Keyboard instance

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
FlushDataBuffer (
  IN  I2C_KB_DEV                *KbDev
  )
{
  EFI_STATUS                    Status;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  
  if ((KbDev == NULL) ||
      ((I2cIo = KbDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Receive the status with flush the transmission buffer 
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = KbDev->HidDescriptor.MaxInputLength;
  RequestPacket.Operation[0].Buffer        = KbDev->ReportData;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    NULL,
                    &RequestPacket,
                    NULL
                    );
  return Status;
}

/**

  Sets Keyboard LED state.
  
  @param  KbDev                 The I2C_KB_DEV instance.

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
SetKeyLED (
  IN  I2C_KB_DEV                *KbDev
  )
{
  EFI_STATUS                    Status;
  LED_MAP                       Led;
  QUALIFIER_STAT                *QualifierStat;
  UINTN                         Index;
  REPORT_GROUP                  *ReportGroup;
  UINTN                         Length;
  UINT64                        Request;
  UINT8                         ReportData[18];
  UINT16                        Register;
  EFI_I2C_REQUEST_PACKET        RequestPacket;
  EFI_I2C_IO_PROTOCOL           *I2cIo;

  if ((KbDev == NULL) ||
      ((I2cIo = KbDev->I2cIo) == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Set each field in Led map.
  //
  Led.NumLock    = (UINT8) KbDev->NumLockOn;
  Led.CapsLock   = (UINT8) KbDev->CapsOn;
  Led.ScrollLock = (UINT8) KbDev->ScrollOn;
  Led.Resrvd     = 0;
  //
  // Make synchronize with BDA qualifier stat
  //
  if (KbDev->IsCsmEnabled) {
    QualifierStat = (QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
    QualifierStat->ScrollLock = Led.ScrollLock;
    QualifierStat->NumLock    = Led.NumLock;
    QualifierStat->CapsLock   = Led.CapsLock;
  }
  //
  // Only lighten LED if supported
  //
  if (KbDev->LedSupported) {
    //
    // Set the keyboard LED through SetReport
    //
    for (Index = 0, ReportGroup = NULL; Index < KbDev->ReportFieldInfo.Total; Index ++) {
      if (KbDev->ReportFieldInfo.ReportGroup[Index].DataValid && 
          KbDev->ReportFieldInfo.ReportGroup[Index].DataAttr == ATTR_KEYBOARD_LED) {
        ReportGroup = &KbDev->ReportFieldInfo.ReportGroup[Index];
        break;
      }
    }
    if (!ReportGroup) {
      return EFI_UNSUPPORTED;
    }
    if (ReportGroup->DataType == HID_MAIN_ITEM_TAG_FEATURE) {
      //
      // Use of SetReportFeature command to send out the LED data to device if report type is feature
      //
      if (ReportGroup->Id < 0x0f) {
        Length  = 4;
        Request = (KbDev->HidDescriptor.DataRegister << 16) | SET_REPORT_FEATURE | ReportGroup->Id;
      } else {
        Length  = 5;
        Request = (KbDev->HidDescriptor.DataRegister << 24) | (ReportGroup->Id << 16) | SET_REPORT_FEATURE | 0x0f;
      }
      CopyMem (ReportData + 2, &Request, Length);
      Register = KbDev->HidDescriptor.CommandRegister;
    } else {
      //
      // Output the LED data to device directly if report type is output
      //
      Length = 0;
      Register = KbDev->HidDescriptor.OutputRegister;
    }
    
    Length += 2;
    *(UINT16*)ReportData = Register;
    
    ReportData[Length + 0] = 0x04;            // Data length LSB
    ReportData[Length + 1] = 0x00;            // Data length MSB
    ReportData[Length + 2] = ReportGroup->Id;
    ReportData[Length + 3] = *(UINT8*)&Led;
    Length += 4;

    RequestPacket.OperationCount             = 1;
    RequestPacket.Operation[0].Flags         = 0;
    RequestPacket.Operation[0].LengthInBytes = (UINT32)Length;
    RequestPacket.Operation[0].Buffer        = ReportData;
  
    Status = I2cIo->QueueRequest (
                      I2cIo,
                      0,
                      NULL,
                      &RequestPacket,
                      NULL
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return EFI_SUCCESS;
}

/**

  LED sync routine for sync update LED
  
  @param  KbDev                 The I2C_KB_DEV instance.
  
**/
VOID
SyncKbdLed (
  IN  I2C_KB_DEV                *KbDev
  )
{
  EFI_STATUS                            Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *ConSplitterEx;
  EFI_KEY_TOGGLE_STATE                  KeyToggleState = EFI_TOGGLE_STATE_VALID;

  if (KbDev->ScrollOn == 1) {
    KeyToggleState |= EFI_SCROLL_LOCK_ACTIVE;
  }
  if (KbDev->NumLockOn == 1) {
    KeyToggleState |= EFI_NUM_LOCK_ACTIVE;
  }
  if (KbDev->CapsOn == 1) {
    KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
  }

  if (gST->ConsoleInHandle != NULL) {
    Status = gBS->HandleProtocol (
                    gST->ConsoleInHandle,
                    &gEfiSimpleTextInputExProtocolGuid,
                    (VOID **)&ConSplitterEx
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Update all of ConInEx device State.
      //      
      Status = ConSplitterEx->SetState (ConSplitterEx, &KeyToggleState);
      if (Status == EFI_SUCCESS) return;
    }
  }
  //
  // Update Keyboard State
  //
  KbDev->SimpleInputEx.SetState (&KbDev->SimpleInputEx, &KeyToggleState);
}

/**

  It is called whenever there is data received from async interrupt
  transfer.

  @param  Event                 Wait Event
  @param  Context               Passed parameter to event handler

**/
STATIC
VOID
EFIAPI
I2cKeyboardInterruptOnCompleted (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  I2C_KB_DEV                    *KbDev;
  UINTN                         DataLength;
  UINT8                         *ReportData;
  UINTN                         Index;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  EFI_I2C_REQUEST_PACKET        RequestPacket;

  KbDev      = (I2C_KB_DEV*)Context;
  I2cIo      = KbDev->I2cIo;
  DataLength = *(UINT16*)KbDev->ReportData;
  if (KbDev->TransferResult != EFI_SUCCESS || DataLength == 0) goto Exit;
  ReportData = KbDev->ReportData + sizeof (UINT16);
  DataLength -= sizeof (UINT16);
  if (KbDev->ReportId) {
    //
    // Searching for matched report field
    //
    for (Index = 0; Index < KbDev->ReportFieldInfo.Total; Index ++) {
      if (KbDev->ReportFieldInfo.ReportGroup[Index].Id == *ReportData && 
          KbDev->ReportFieldInfo.ReportGroup[Index].DataClass == KEYBOARD_CLASS &&
          KbDev->ReportFieldInfo.ReportGroup[Index].DataType == HID_MAIN_ITEM_TAG_INPUT) {
         ReportData ++;
         DataLength --;
         break;
      }
    }
    if (Index == KbDev->ReportFieldInfo.Total) goto Exit;
  }
  KeyboardHandler (ReportData, DataLength, KbDev, 0);
Exit:
  //
  // Starting next transmission
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = KbDev->HidDescriptor.MaxInputLength;
  RequestPacket.Operation[0].Buffer        = KbDev->ReportData;

  I2cIo->QueueRequest (
           I2cIo,
           0,
           KbDev->OnCompleteEvent,
           &RequestPacket,
           &KbDev->TransferResult
           );
}

/**

  Check whether there is key pending.

  @param  KbDev     The I2C_KB_DEV instance.
  
  @retval EFI_SUCCESS           Success

**/ 
STATIC
EFI_STATUS
I2cKeyboardCheckForKey (
  IN  I2C_KB_DEV                *KbDev
  )
{
  EFI_STATUS  Status;
  UINT8       KeyChar;
  //
  // Fetch raw data from the I2C keyboard input,
  // and translate it into ASCII data.
  //
  Status = ParseKey (KbDev, &KeyChar);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  KbDev->CurKeyChar = KeyChar;
  return EFI_SUCCESS;
}

/**

  Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.Reset() function.

  @param  This                  The EFI_SIMPLE_TEXT_INPUT_PROTOCOL instance.
  @param  ExtendedVerification  Indicates that the driver may perform a more exhaustive
                                verification operation of the device during reset.              
  
  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

**/ 
EFI_STATUS
EFIAPI
I2cKeyboardReset (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  IN  BOOLEAN                           ExtendedVerification
  )
{
  EFI_STATUS          Status;
  I2C_KB_DEV          *KbDev;

  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_PROTOCOL (This);
  //
  // Non Exhaustive reset:
  // only reset private data structures.
  //
  if (!ExtendedVerification) {
    //
    // Clear the key buffer of this I2C keyboard
    //
    InitKeyBuffer (&(KbDev->KeyboardBuffer));
    KbDev->CurKeyChar = 0;
    return EFI_SUCCESS;
  }
  //
  // Exhaustive reset
  //
  Status = ResetDevice (KbDev);
  KbDev->CurKeyChar = 0;
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**

  Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.ReadKeyStroke() function.

  @param  This                  The EFI_SIMPLE_TEXT_INPUT_PROTOCOL instance.
  @param  Key                   A pointer to a buffer that is filled in with the keystroke
                                information for the key that was pressed.
  
  @retval EFI_SUCCESS           Success

**/   
STATIC
EFI_STATUS
EFIAPI
I2cKeyboardReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  OUT EFI_INPUT_KEY                     *Key
  )
{
  I2C_KB_DEV  *KbDev;
  EFI_STATUS  Status;
  UINT8       KeyChar;

  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_PROTOCOL (This);

  //
  // Considering if the partial keystroke is enabled, there maybe a partial
  // keystroke in the queue, so here skip the partial keystroke and get the
  // next key from the queue
  //
  while (1) {
    //
    // if there is no saved ASCII byte, fetch it
    // by calling KeyboardCheckForKey().
    //
    if (KbDev->CurKeyChar == 0) {
      Status = I2cKeyboardCheckForKey (KbDev);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    Key->UnicodeChar              = CHAR_NULL;
    Key->ScanCode                 = SCAN_NULL;
    KeyChar                       = KbDev->CurKeyChar;
    KbDev->CurKeyChar          = 0;
    //
    // Translate saved ASCII byte into EFI_INPUT_KEY
    //
    Status = KeyCodeToEFIScanCode (KbDev, KeyChar, Key);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // SimpleTextIn Protocol doesn't support partial keystroke;
    //
    if (Key->ScanCode == SCAN_NULL && Key->UnicodeChar == CHAR_NULL) {
      continue;
    }
    return EFI_SUCCESS;
  }
}

/**

  Handler function for WaitForKey event.    

  @param  Event                 Event to be signaled when a key is pressed.
  @param  Context               Points to I2C_KB_DEV instance.
  
**/
STATIC
VOID
EFIAPI
I2cKeyboardWaitForKey (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  I2C_KB_DEV    *KbDev;
  EFI_INPUT_KEY Key;
  UINT8         KeyChar;
  EFI_STATUS    Status;

  KbDev = (I2C_KB_DEV *) Context;

  while (1) {
    if (KbDev->CurKeyChar == 0) {
      if (EFI_ERROR (I2cKeyboardCheckForKey (KbDev))) {
        return;
      }
    }
    Key.UnicodeChar               = CHAR_NULL;
    Key.ScanCode                  = SCAN_NULL;
    KeyChar                       = KbDev->CurKeyChar;
    //
    // Translate saved ASCII byte into EFI_INPUT_KEY
    //
    Status = KeyCodeToEFIScanCode (KbDev, KeyChar, &Key);
    //
    // SimpleTextIn Protocol doesn't support partial keystroke;
    //
    if (EFI_ERROR (Status) || (Key.ScanCode == SCAN_NULL && Key.UnicodeChar == CHAR_NULL)) {
      KbDev->CurKeyChar = 0;
      continue;
    }
    //
    // If has key pending, signal the event.
    //
    gBS->SignalEvent (Event);
    return;
  }
}

/**

  Free notify list which used for notification of console in

  @param  ListHead              The list head

  @retval EFI_SUCCESS           Free the notify list successfully
  @retval EFI_INVALID_PARAMETER ListHead is invalid.

**/
STATIC
EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY             *ListHead
  )
{
  KEYBOARD_CONSOLE_IN_EX_NOTIFY *NotifyNode;

  if (ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  while (!IsListEmpty (ListHead)) {
    NotifyNode = CR (
                   ListHead->ForwardLink, 
                   KEYBOARD_CONSOLE_IN_EX_NOTIFY, 
                   NotifyEntry, 
                   I2C_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                   );
    RemoveEntryList (ListHead->ForwardLink);
    gBS->FreePool (NotifyNode);
  }
  
  return EFI_SUCCESS;
}
/**

  Check is the key been registered

  @param  RegsiteredData        A pointer to a buffer that is filled in with the keystroke 
                                state data for the key that was registered.
  @param  InputData             A pointer to a buffer that is filled in with the keystroke 
                                state data for the key that was pressed.

  @retval TRUE                  Key be pressed matches a registered key.
  @retval FLASE                 Match failed. 
  
**/
STATIC
BOOLEAN
IsKeyRegistered (
  IN EFI_KEY_DATA  *RegsiteredData,
  IN EFI_KEY_DATA  *InputData
  )
{
  ASSERT (RegsiteredData != NULL && InputData != NULL);
  
  if ((RegsiteredData->Key.ScanCode    != InputData->Key.ScanCode) ||
      (RegsiteredData->Key.UnicodeChar != InputData->Key.UnicodeChar)) {
    return FALSE;  
  }      
  
  //
  // Assume KeyShiftState/KeyToggleState = 0 in Registered key data means these state could be ignored.
  //
  if (RegsiteredData->KeyState.KeyShiftState != 0 &&
      RegsiteredData->KeyState.KeyShiftState != InputData->KeyState.KeyShiftState) {
    return FALSE;    
  }   
  if (RegsiteredData->KeyState.KeyToggleState != 0 &&
      RegsiteredData->KeyState.KeyToggleState != InputData->KeyState.KeyToggleState) {
    return FALSE;    
  }     
  
  return TRUE;

}

/**

  Reset the input device and optionaly run diagnostics

  @param  This                  Protocol instance pointer.
  @param  ExtendedVerification  Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS           The device was reset.
  @retval EFI_DEVICE_ERROR      The device is not functioning properly and could 
                                not be reset.

**/
EFI_STATUS
EFIAPI
I2cKeyboardResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  EFI_STATUS                Status;
  I2C_KB_DEV                *KbDev;
  EFI_TPL                   OldTpl;
  

  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_EX_PROTOCOL (This);

  Status = KbDev->SimpleInput.Reset (&KbDev->SimpleInput, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  KbDev->KeyState.KeyShiftState  = EFI_SHIFT_STATE_VALID;
  KbDev->KeyState.KeyToggleState = EFI_TOGGLE_STATE_VALID;
  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;

}

/**

  Reads the next keystroke from the input device. The WaitForKey Event can 
  be used to test for existance of a keystroke via WaitForEvent () call.

  @param  This                  Protocol instance pointer.
  @param  KeyData               A pointer to a buffer that is filled in with the keystroke 
                                state data for the key that was pressed.

  @retval EFI_SUCCESS           The keystroke information was returned.
  @retval EFI_NOT_READY         There was no keystroke data availiable.
  @retval EFI_DEVICE_ERROR      The keystroke information was not returned due to 
                                hardware errors.
  @retval EFI_INVALID_PARAMETER KeyData is NULL.                        

**/
EFI_STATUS
EFIAPI
I2cKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  EFI_STATUS                        Status;
  UINT8                             KeyChar;  
  LIST_ENTRY                        *Link;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;  
  I2C_KB_DEV                        *KbDev;

  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_EX_PROTOCOL (This);

  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // if there is no saved ASCII byte, fetch it
  // by calling KeyboardCheckForKey().
  //
  if (KbDev->CurKeyChar == 0) {
    Status = I2cKeyboardCheckForKey (KbDev);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  KeyData->Key.UnicodeChar = 0;
  KeyData->Key.ScanCode    = SCAN_NULL;

  KeyChar = KbDev->CurKeyChar;

  KbDev->CurKeyChar = 0;

  //
  // Translate saved ASCII byte into EFI_INPUT_KEY
  //
  Status = KeyCodeToEFIScanCode (KbDev, KeyChar, &KeyData->Key);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (&KeyData->KeyState, &KbDev->KeyState, sizeof (KeyData->KeyState));
  //
  //Switch the control value to their original characters. In KeyCodeToEFIScanCode() the  CTRL-Alpha characters have been switched to 
  // their corresponding control value (ctrl-a = 0x0001 through ctrl-Z = 0x001A), here switch them back for notification function.
  //
  if (KbDev->CtrlOn) {
    if (KeyData->Key.UnicodeChar >= 0x01 && KeyData->Key.UnicodeChar <= 0x1A) {
      if (KbDev->CapsOn) {
        KeyData->Key.UnicodeChar = KeyData->Key.UnicodeChar + 'A' - 1;
      } else {
        KeyData->Key.UnicodeChar = KeyData->Key.UnicodeChar + 'a' - 1;
      } 
    } else if (KeyData->Key.ScanCode == SCAN_ESC &&
               KeyData->Key.UnicodeChar == CHAR_NULL) {
      KeyData->Key.UnicodeChar = KbDev->ShiftOn ? '{' : '[';
    } else if (KeyData->Key.UnicodeChar == 0x1C) {
      KeyData->Key.UnicodeChar = KbDev->ShiftOn ? '|' : '\\';
    } else if (KeyData->Key.UnicodeChar == 0x1D) {
      KeyData->Key.UnicodeChar = KbDev->ShiftOn ? '}' : ']';
    } else if (KeyData->Key.UnicodeChar == 0x1E) {
      KeyData->Key.UnicodeChar = KbDev->ShiftOn ? '^' : '6';
    } else if (KeyData->Key.UnicodeChar == 0x1F) {
      KeyData->Key.UnicodeChar = KbDev->ShiftOn ? '_' : '-';
    }
  }
  
  //
  // Invoke notification functions if exist
  //
  for (Link = KbDev->NotifyList.ForwardLink; Link != &KbDev->NotifyList; Link = Link->ForwardLink) {
    CurrentNotify = CR (
                      Link, 
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY, 
                      NotifyEntry, 
                      I2C_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (IsKeyRegistered (&CurrentNotify->KeyData, KeyData)) { 
      CurrentNotify->KeyNotificationFn (KeyData);
    }
  }
  return EFI_SUCCESS;
}

/**

  Set certain state for the input device.

  @param  This                  Protocol instance pointer.
  @param  KeyToggleState        A pointer to the EFI_KEY_TOGGLE_STATE to set the 
                                state for the input device.
                        
  @retval EFI_SUCCESS           The device state was set successfully.
  @retval EFI_DEVICE_ERROR      The device is not functioning correctly and could 
                                not have the setting adjusted.
  @retval EFI_UNSUPPORTED       The device does not have the ability to set its state.
  @retval EFI_INVALID_PARAMETER KeyToggleState is NULL.                       

**/  
EFI_STATUS
EFIAPI
I2cKeyboardSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{
  I2C_KB_DEV                        *KbDev;

  if (KeyToggleState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_EX_PROTOCOL (This);

  if (((KbDev->KeyState.KeyToggleState & EFI_TOGGLE_STATE_VALID) != EFI_TOGGLE_STATE_VALID) ||
      ((*KeyToggleState & EFI_TOGGLE_STATE_VALID) != EFI_TOGGLE_STATE_VALID) ||
      (((*KeyToggleState & EFI_KEY_STATE_EXPOSED) == EFI_KEY_STATE_EXPOSED) && KbDev->IsCsmEnabled)) {

    return EFI_UNSUPPORTED;
  }

  //
  // Update the status light
  //

  KbDev->ScrollOn   = 0;
  KbDev->NumLockOn  = 0;
  KbDev->CapsOn     = 0;
  KbDev->IsSupportPartialKey = 0;
 
  if ((*KeyToggleState & EFI_SCROLL_LOCK_ACTIVE) == EFI_SCROLL_LOCK_ACTIVE) {
    KbDev->ScrollOn = 1;
  }
  if ((*KeyToggleState & EFI_NUM_LOCK_ACTIVE) == EFI_NUM_LOCK_ACTIVE) {
    KbDev->NumLockOn = 1;
  }
  if ((*KeyToggleState & EFI_CAPS_LOCK_ACTIVE) == EFI_CAPS_LOCK_ACTIVE) {
    KbDev->CapsOn = 1;
  }
  if ((*KeyToggleState & EFI_KEY_STATE_EXPOSED) == EFI_KEY_STATE_EXPOSED) {
    KbDev->IsSupportPartialKey = 1;
  }

  SetKeyLED (KbDev);

  KbDev->KeyState.KeyToggleState = *KeyToggleState;

  return EFI_SUCCESS;
  
}

/**

  Register a notification function for a particular keystroke for the input device.

  @param  This                          Protocol instance pointer.
  @param  KeyData                       A pointer to a buffer that is filled in with the keystroke 
                                        information data for the key that was pressed.
  @param  KeyNotificationFunction       Points to the function to be called when the key 
                                        sequence is typed specified by KeyData.                        
  @param  NotifyHandle                  Points to the unique handle assigned to the registered notification.                          

  @retval EFI_SUCCESS                   The notification function was registered successfully.
  @retval EFI_OUT_OF_RESOURCES          Unable to allocate resources for necesssary data structures.
  @retval EFI_INVALID_PARAMETER         KeyData or NotifyHandle is NULL.                       
                            
**/  
EFI_STATUS
EFIAPI
I2cKeyboardRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  )
{
  I2C_KB_DEV                        *KbDev;
  EFI_STATUS                        Status;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *NewNotify;
  LIST_ENTRY                        *Link;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;  

  if (KeyData == NULL || NotifyHandle == NULL || KeyNotificationFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_EX_PROTOCOL (This);

  //
  // Return EFI_SUCCESS if the (KeyData, NotificationFunction) is already registered.
  //
  for (Link = KbDev->NotifyList.ForwardLink; Link != &KbDev->NotifyList; Link = Link->ForwardLink) {
    CurrentNotify = CR (
                      Link, 
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY, 
                      NotifyEntry, 
                      I2C_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (IsKeyRegistered (&CurrentNotify->KeyData, KeyData)) { 
      if (CurrentNotify->KeyNotificationFn == KeyNotificationFunction) {
        *NotifyHandle = CurrentNotify->NotifyHandle;        
        return EFI_SUCCESS;
      }
    }
  }
  
  //
  // Allocate resource to save the notification function
  //  
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (KEYBOARD_CONSOLE_IN_EX_NOTIFY),
                  (VOID **)&NewNotify
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (NewNotify, sizeof (KEYBOARD_CONSOLE_IN_EX_NOTIFY));
  NewNotify->Signature         = I2C_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE;     
  NewNotify->KeyNotificationFn = KeyNotificationFunction;
  CopyMem (&NewNotify->KeyData, KeyData, sizeof (EFI_KEY_DATA));
  InsertTailList (&KbDev->NotifyList, &NewNotify->NotifyEntry);

  //
  // Use gSimpleTextInExNotifyGuid to get a valid EFI_HANDLE
  //  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &NewNotify->NotifyHandle,
                  &gSimpleTextInExNotifyGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  
  *NotifyHandle = NewNotify->NotifyHandle;  
  
  return EFI_SUCCESS;
  
}

/**

  Remove a registered notification function from a particular keystroke.

  @param  This                  Protocol instance pointer.    
  @param  NotificationHandle    The handle of the notification function being unregistered.

  @retval EFI_SUCCESS           The notification function was unregistered successfully.
  @retval EFI_INVALID_PARAMETER The NotificationHandle is invalid.
  @retval EFI_NOT_FOUND         Can not find the matching entry in database.  
                            
**/  
EFI_STATUS
EFIAPI
I2cKeyboardUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle
  )
{
  I2C_KB_DEV                        *KbDev;
  EFI_STATUS                        Status;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;
  LIST_ENTRY                        *Link;

  if (NotificationHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }  
  
  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_EX_PROTOCOL (This);
  
  Status = gBS->OpenProtocol (
                  NotificationHandle,
                  &gSimpleTextInExNotifyGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Link = KbDev->NotifyList.ForwardLink; Link != &KbDev->NotifyList; Link = Link->ForwardLink) {
    CurrentNotify = CR (
                      Link, 
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY, 
                      NotifyEntry, 
                      I2C_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );       
    if (CurrentNotify->NotifyHandle == NotificationHandle) {
      //
      // Remove the notification function from NotifyList and free resources
      //
      RemoveEntryList (&CurrentNotify->NotifyEntry);      
      Status = gBS->UninstallMultipleProtocolInterfaces (
                      CurrentNotify->NotifyHandle,
                      &gSimpleTextInExNotifyGuid,
                      NULL,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);
      gBS->FreePool (CurrentNotify);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;  
}

/**

  Signal handlers for Legacy Bios installed event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
STATIC
VOID
EFIAPI
LegacyBiosNotifyFunction (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  QUALIFIER_STAT        *QualifierStat;
  I2C_KB_DEV            *KbDev;
  
  KbDev = (I2C_KB_DEV*)Context;
  //
  // Sync the initial state of LED with BDAs qualifier stat 
  //
  QualifierStat    = (QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
  //
  // Make it synchronization
  //
  KbDev->ScrollOn  = QualifierStat->ScrollLock;
  KbDev->NumLockOn = QualifierStat->NumLock;
  KbDev->CapsOn    = QualifierStat->CapsLock;
  SetKeyLED(KbDev);
}

/**

  Free the resource allocated by I2C Keyboard instance
  
  @param  KbDev                 I2C Keyboard instance
  @param  Controller            I2C I/O Controller Handle

**/
STATIC
VOID
FreeResource (
  IN I2C_KB_DEV                 *KbDev,
  IN EFI_HANDLE                 Controller
  )
{
  //
  // Always close protocol even the protocol has not being opened
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiI2cIoProtocolGuid,
         gI2cKeyboardDriverBinding.DriverBindingHandle,
         Controller
         );
  //
  // Free resources in the instance
  //
  if (KbDev) {
    if (KbDev->OnCompleteEvent) {
      gBS->CloseEvent (KbDev->OnCompleteEvent);
    }
    if (KbDev->SimpleInput.WaitForKey) {
      gBS->CloseEvent (KbDev->SimpleInput.WaitForKey);
    }
    if (KbDev->SimpleInputEx.WaitForKeyEx) {
      gBS->CloseEvent (KbDev->SimpleInputEx.WaitForKeyEx);
    }
    if (KbDev->NotifyList.ForwardLink) {
      KbdFreeNotifyList (&KbDev->NotifyList);
    }
    gBS->UninstallMultipleProtocolInterfaces (
           Controller,
           &gEfiSimpleTextInProtocolGuid,
           &KbDev->SimpleInput,
           &gEfiSimpleTextInputExProtocolGuid,
           &KbDev->SimpleInputEx,
           NULL
           );
    if (KbDev->ReportData) {
      FreePool (KbDev->ReportData);
    }
    if (KbDev->ControllerNameTable) {
      FreeUnicodeStringTable (KbDev->ControllerNameTable);
    }
    FreePool (KbDev);
  }
}
/**
  Verify the controller type

  This routine determines if an I2C Keyboard is available.

  This routine is called by the UEFI driver framework during connect
  processing.

  @param [in] DriverBinding             Protocol instance pointer.
  @param [in] Controller                Handle of device to test.
  @param [in] RemainingDevicePath       Not used.

  @retval EFI_SUCCESS                   This driver supports this device.
  @retval other                         This driver does not support this device.

**/
EFI_STATUS
EFIAPI
I2cKeyboardDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_I2C_IO_PROTOCOL                   *I2cIo;
  EFI_ADAPTER_INFORMATION_PROTOCOL      *Aip;
  H2O_I2C_HID_DEVICE                    *I2cHidDevice;
  UINTN                                 DataLength;
  //
  // Determine if the I2C I/O is available
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cIoProtocolGuid,
                  (VOID**) &I2cIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  //  The I2C I/O is available
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiI2cIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  //
  // Inspect the H2O I2C HID specific data in the controller
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiAdapterInformationProtocolGuid,
                  (VOID**) &Aip,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = Aip->GetInformation (
                  Aip,
                  &gI2cHidDeviceInfoGuid,
                  (VOID**) &I2cHidDevice,
                  &DataLength
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (I2cHidDevice->ClassType != KEYBOARD_CLASS) {
    Status = EFI_NOT_FOUND;
  }
  FreePool (I2cHidDevice);
  return Status;
}

/**
  Connect to the I2C Keyboard

  This routine initializes an instance of the I2C driver for this
  controller.

  This routine is called by the UEFI driver framework during connect
  processing if the controller passes the tests in I2cBusDriverSupported.

  @param [in] DriverBinding             Protocol instance pointer.
  @param [in] Controller                Handle of device to work with.
  @param [in] RemainingDevicePath       Not used, always produce all possible children.

  @retval EFI_SUCCESS                   This driver is added to Controller.
  @retval other                         This driver does not support this device.

**/
EFI_STATUS
EFIAPI
I2cKeyboardDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  I2C_KB_DEV                            *KbDev;
  EFI_I2C_IO_PROTOCOL                   *I2cIo;
  EFI_ADAPTER_INFORMATION_PROTOCOL      *Aip;
  H2O_I2C_HID_DEVICE                    *I2cHidDevice;
  UINTN                                 DataLength;
  UINT8                                 *ReportDescriptor;
  VOID                                  *LegacyBios;
  EFI_EVENT                             Event;
  VOID                                  *Registration;
  UINTN                                 Index;  
  REPORT_GROUP                          *ReportGroup;
  EFI_I2C_REQUEST_PACKET                RequestPacket;
  
  KbDev        = NULL;
  //
  // Occupy the I2C I/O protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cIoProtocolGuid,
                  (VOID**) &I2cIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the H2O I2C HID specific data in the controller
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiAdapterInformationProtocolGuid,
                  (VOID**) &Aip,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  Status = Aip->GetInformation (
                  Aip,
                  &gI2cHidDeviceInfoGuid,
                  (VOID**) &I2cHidDevice,
                  &DataLength
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  KbDev = AllocateZeroPool (sizeof (I2C_KB_DEV));
  if (KbDev == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  KbDev->Signature   = I2C_KB_DEV_SIGNATURE;
  KbDev->I2cIo       = I2cIo;
  CopyMem (&KbDev->I2cHid, I2cHidDevice, I2cHidDevice->Length);
  FreePool (I2cHidDevice);
  //
  // Get HID descriptor
  //
  Status = GetHidDescriptor (I2cIo, KbDev->I2cHid.DescReg, &KbDev->HidDescriptor);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  if (KbDev->HidDescriptor.HIDDescLength != sizeof(HID_DESCRIPTOR)) {
    Status = EFI_DEVICE_ERROR;
    goto Error;
  }
  KbDev->ReportData = AllocateZeroPool (KbDev->HidDescriptor.MaxInputLength);
  if (KbDev->ReportData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  //
  // Power on the device
  //
  SetPower (KbDev, POWER_ON);
  //
  // Reset the device
  //
  ResetDevice (KbDev);
  //
  // Flush the data buffer before operation
  //
  FlushDataBuffer (KbDev);
  //
  // Get report descriptor
  //
  Status = GetReportDescriptor ( I2cIo, &KbDev->HidDescriptor, &ReportDescriptor);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  Status = ParseReportDescriptor (
             ReportDescriptor,
             KbDev->HidDescriptor.ReportDescLength,
             &KbDev->ReportFieldInfo
             );
  FreePool (ReportDescriptor);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Searching for keyboard report
  //
  for (Index = 0, ReportGroup = NULL; Index < KbDev->ReportFieldInfo.Total; Index ++) {
    if (KbDev->ReportFieldInfo.ReportGroup[Index].DataValid &&
        KbDev->ReportFieldInfo.ReportGroup[Index].DataClass == KEYBOARD_CLASS &&
        KbDev->ReportFieldInfo.ReportGroup[Index].DataType == HID_MAIN_ITEM_TAG_INPUT &&
        KbDev->ReportFieldInfo.ReportGroup[Index].DataSize > 0) {
      ReportGroup =  &KbDev->ReportFieldInfo.ReportGroup[Index];
      break;
    }
  }
  if (ReportGroup == NULL) { 
    Status = EFI_UNSUPPORTED;
    goto Error;
  }
  KbDev->ReportId   = ReportGroup->Id;
  //
  // Check keyboard LED exist or not
  //
  for (Index = 0; Index < KbDev->ReportFieldInfo.Total; Index ++) {
    if (KbDev->ReportFieldInfo.ReportGroup[Index].DataValid && 
        KbDev->ReportFieldInfo.ReportGroup[Index].DataAttr == ATTR_KEYBOARD_LED) {
      KbDev->LedSupported = TRUE;
      break;
    }
  }
  //
  // Install SimpleInput/Ex protocol
  //
  KbDev->SimpleInput.Reset          = I2cKeyboardReset;
  KbDev->SimpleInput.ReadKeyStroke  = I2cKeyboardReadKeyStroke;
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  I2cKeyboardWaitForKey,
                  KbDev,
                  &KbDev->SimpleInput.WaitForKey
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  KbDev->SimpleInputEx.Reset               = I2cKeyboardResetEx;
  KbDev->SimpleInputEx.ReadKeyStrokeEx     = I2cKeyboardReadKeyStrokeEx;
  KbDev->SimpleInputEx.SetState            = I2cKeyboardSetState;
  KbDev->SimpleInputEx.RegisterKeyNotify   = I2cKeyboardRegisterKeyNotify;
  KbDev->SimpleInputEx.UnregisterKeyNotify = I2cKeyboardUnregisterKeyNotify; 
  InitializeListHead (&KbDev->NotifyList);
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  I2cKeyboardWaitForKey,
                  KbDev,
                  &KbDev->SimpleInputEx.WaitForKeyEx
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Install Simple Text Input Protocol and Simple Text Input Ex Protocol
  // for the I2C keyboard device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &KbDev->SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &KbDev->SimpleInputEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  if (KbDev->LedSupported) {
    //
    // Check is it legacy free system (no CSM installed)
    //
    Status = gBS->LocateProtocol (
                    &gEfiLegacyBiosProtocolGuid,
                    NULL,
                    &LegacyBios
                    );
    if (!EFI_ERROR (Status)) {
      //
      // CSM enabled system, notify the LegacyBios callback directly
      //
      LegacyBiosNotifyFunction (0, KbDev);
    } else {
      //
      // Register the notification for LegacyBios installed
      //
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      LegacyBiosNotifyFunction,
                      KbDev,
                      &Event
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = gBS->RegisterProtocolNotify (
                      &gEfiLegacyBiosProtocolGuid,
                      Event,
                      &Registration
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      SetKeyLED(KbDev);
    }
  }
  //
  // Create event for interrupt on completed handler
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  I2cKeyboardInterruptOnCompleted,
                  KbDev,
                  &KbDev->OnCompleteEvent
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Allocate and setup controller name table for ComponentName/2 protocol
  //
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_ISO639,
    gI2cKeyboardComponentName.SupportedLanguages,
    &KbDev->ControllerNameTable,
    CONTROLLER_DRIVER_NAME,
    TRUE
    );
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_RFC4646,
    gI2cKeyboardComponentName2.SupportedLanguages,
    &KbDev->ControllerNameTable,
    CONTROLLER_DRIVER_NAME,
    FALSE
    );
  //
  // Issue async I2C request for data transfer
  //
  RequestPacket.OperationCount             = 1;
  RequestPacket.Operation[0].Flags         = I2C_FLAG_READ;
  RequestPacket.Operation[0].LengthInBytes = KbDev->HidDescriptor.MaxInputLength;
  RequestPacket.Operation[0].Buffer        = KbDev->ReportData;

  Status = I2cIo->QueueRequest (
                    I2cIo,
                    0,
                    KbDev->OnCompleteEvent,
                    &RequestPacket,
                    &KbDev->TransferResult
                    );
Error:
  if (EFI_ERROR (Status)) {
    FreeResource (KbDev, Controller);
  }
  return Status;
}


/**
  Disconnect from the I2C host controller.

  This routine disconnects from the Keyboard

  This routine is called by DriverUnload when the Keyboard
  is being unloaded.

  @param [in] DriverBinding     Protocol instance pointer.
  @param [in] Controller        Handle of device to stop driver on.
  @param [in] NumberOfChildren  How many children need to be stopped.
  @param [in] ChildHandleBuffer Not used.

  @retval EFI_SUCCESS           This driver is removed Controller.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.
  @retval other                 This driver was not removed from this device.

**/
EFI_STATUS
EFIAPI
I2cKeyboardDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  )
{
  EFI_STATUS                            Status;
  I2C_KB_DEV                            *KbDev;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL        *SimpleInputProtocol;
  //
  //  Disconnect any connected drivers and locate the context
  //  structure
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID**) &SimpleInputProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  KbDev = I2C_KB_DEV_FROM_SIMPLE_INPUT_PROTOCOL (SimpleInputProtocol);
  FreeResource (KbDev, Controller);
  return Status;
}

/**

  Driver binding protocol support

**/
EFI_DRIVER_BINDING_PROTOCOL gI2cKeyboardDriverBinding = {
  I2cKeyboardDriverSupported,
  I2cKeyboardDriverStart,
  I2cKeyboardDriverStop,
  0x12,
  NULL,
  NULL
};

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS
  @retval others

**/
EFI_STATUS
EFIAPI
I2cKeyboardEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  //
  // Install binding protocols
  //
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gI2cKeyboardDriverBinding,
           ImageHandle,
           &gI2cKeyboardComponentName,
           &gI2cKeyboardComponentName2
           );
}
