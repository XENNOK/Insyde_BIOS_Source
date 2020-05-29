/** @file
  Implementation for EFI_SIMPLE_TEXT_INPUT_PROTOCOL protocol.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Terminal.h"

#define ESC_SEQUENCE_KEY_PERIOD     (UINT64)10000000
#define ESC_RESET_KEY_PERIOD        (UINT64)30000000
#define CHECK_FIRST_MATCH           0
#define CHECK_ALL_STRINGS           1

typedef enum {
  ESC_CODE_INVALID,
  ESC_CODE_FINDED,
  ESC_CODE_UNFINISHED
} ESC_CODE_STATUS;

typedef enum {
  SPC_COMMAND_STRING,
  SPC_WAIT_CHAR,
  SPC_INVALID_STRING
} SPC_STATUS;


/**
  Reads the next keystroke from the input device. The WaitForKey Event can
  be used to test for existence of a keystroke via WaitForEvent () call.

  @param  TerminalDevice           Terminal driver private structure
  @param  KeyData                    A pointer to a buffer that is filled in with the
                                               keystroke state data for the key that was pressed.

  @retval EFI_SUCCESS                     The keystroke information was returned.
  @retval EFI_NOT_READY                 There was no keystroke data available.
  @retval EFI_INVALID_PARAMETER    KeyData is NULL.

**/
EFI_STATUS
ReadKeyStrokeWorker (
  IN  TERMINAL_DEV *TerminalDevice,
  OUT EFI_KEY_DATA *KeyData
  )
{
  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!EfiKeyFiFoRemoveOneKey (TerminalDevice, KeyData)) {
    return EFI_NOT_READY;
  }

  KeyData->KeyState.KeyShiftState  |= EFI_SHIFT_STATE_VALID;
  KeyData->KeyState.KeyToggleState = 0;


  return EFI_SUCCESS;

}


/**
  Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.Reset().
  This driver only perform dependent serial device reset regardless of
  the value of ExtendeVerification

  @param  This                           Indicates the calling context.
  @param  ExtendedVerification     Skip by this driver.

  @retval EFI_SUCCESS                  The reset operation succeeds.
  @retval EFI_DEVICE_ERROR         The dependent serial port reset fails.

**/
EFI_STATUS
EFIAPI
TerminalConInReset (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  IN  BOOLEAN                         ExtendedVerification
  )
{
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;

  TerminalDevice = TERMINAL_CON_IN_DEV_FROM_THIS (This);

  //
  // Report progress code here
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_PC_RESET),
    TerminalDevice->DevicePath
    );

  Status = TerminalDevice->SerialIo->Reset (TerminalDevice->SerialIo);

  //
  // Make all the internal buffer empty for keys
  //
  TerminalDevice->RawFiFo->Head     = TerminalDevice->RawFiFo->Tail;
  TerminalDevice->UnicodeFiFo->Head = TerminalDevice->UnicodeFiFo->Tail;
  TerminalDevice->EfiKeyFiFo->Head  = TerminalDevice->EfiKeyFiFo->Tail;

  if (EFI_ERROR (Status)) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_EC_CONTROLLER_ERROR),
      TerminalDevice->DevicePath
      );
  }

  return Status;
}


/**
  Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.ReadKeyStroke().

  @param  This       Indicates the calling context.
  @param  Key        A pointer to a buffer that is filled in with the keystroke information
                            for the key that was sent from terminal.

  @retval EFI_SUCCESS             The keystroke information is returned successfully.
  @retval EFI_NOT_READY         There is no keystroke data available.
  @retval EFI_DEVICE_ERROR    The dependent serial device encounters error.

**/
EFI_STATUS
EFIAPI
TerminalConInReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  OUT EFI_INPUT_KEY                   *Key
  )
{
  TERMINAL_DEV  *TerminalDevice;
  EFI_STATUS    Status;
  EFI_KEY_DATA  KeyData;

  //
  //  get TERMINAL_DEV from "This" parameter.
  //
  TerminalDevice  = TERMINAL_CON_IN_DEV_FROM_THIS (This);

  Status = ReadKeyStrokeWorker (TerminalDevice, &KeyData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Key, &KeyData.Key, sizeof (EFI_INPUT_KEY));

  return EFI_SUCCESS;

}

/**
  Check if the key already has been registered.

  If both RegsiteredData and InputData is NULL, then ASSERT().

  @param  RegsiteredData      A pointer to a buffer that is filled in with the
                                          keystroke state data for the key that was registered.
  @param  InputData             A pointer to a buffer that is filled in with the
                                          keystroke state data for the key that was pressed.

  @retval TRUE                     Key be pressed matches a registered key.
  @retval FLASE                    Match failed.

**/
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

  return TRUE;
}



/**

  Event notification function for EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.WaitForKeyEx event
  Signal the event if there is key available

  @param  Event       Indicates the event that invoke this function.
  @param  Context    Indicates the calling context.

**/
VOID
EFIAPI
TerminalConInWaitForKeyEx (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  TerminalConInWaitForKey (Event, Context);
}

//
// Simple Text Input Ex protocol functions
//

/**
  Reset the input device and optionally run diagnostics

  @param  This                           Protocol instance pointer.
  @param  ExtendedVerification     Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS                The device was reset.
  @retval EFI_DEVICE_ERROR       The device is not functioning properly and could not be reset.

**/
EFI_STATUS
EFIAPI
TerminalConInResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  EFI_STATUS      Status;
  TERMINAL_DEV    *TerminalDevice;

  TerminalDevice = TERMINAL_CON_IN_EX_DEV_FROM_THIS (This);

  Status = TerminalDevice->SimpleInput.Reset (&TerminalDevice->SimpleInput, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;

}


/**
  Reads the next keystroke from the input device. The WaitForKey Event can
  be used to test for existence of a keystroke via WaitForEvent () call.

  @param  This              Protocol instance pointer.
  @param  KeyData        A pointer to a buffer that is filled in with the
                                   keystroke state data for the key that was pressed.

  @retval EFI_SUCCESS                     The keystroke information was returned.
  @retval EFI_NOT_READY                 There was no keystroke data available.
  @retval EFI_DEVICE_ERROR            The keystroke information was not returned due
                                                     to hardware errors.
  @retval EFI_INVALID_PARAMETER    KeyData is NULL.

**/
EFI_STATUS
EFIAPI
TerminalConInReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  TERMINAL_DEV    *TerminalDevice;

  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TerminalDevice = TERMINAL_CON_IN_EX_DEV_FROM_THIS (This);

  return ReadKeyStrokeWorker (TerminalDevice, KeyData);

}


/**
  Set certain state for the input device.

  @param  This                     Protocol instance pointer.
  @param  KeyToggleState     A pointer to the EFI_KEY_TOGGLE_STATE to set the
                                         state for the input device.

  @retval EFI_SUCCESS                     The device state was set successfully.
  @retval EFI_DEVICE_ERROR            The device is not functioning correctly and
                                                     could not have the setting adjusted.
  @retval EFI_UNSUPPORTED             The device does not have the ability to set its state.
  @retval EFI_INVALID_PARAMETER    KeyToggleState is NULL.

**/
EFI_STATUS
EFIAPI
TerminalConInSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{
  if (KeyToggleState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/**
  Register a notification function for a particular keystroke for the input device.

  @param  This                            Protocol instance pointer.
  @param  KeyData                       A pointer to a buffer that is filled in with the
                                                 keystroke information data for the key that was pressed.
  @param  KeyNotificationFunction  Points to the function to be called when the key
                                                 sequence is typed specified by KeyData.
  @param  NotifyHandle                Points to the unique handle assigned to the
                                                 registered notification.

  @retval EFI_SUCCESS                     The notification function was registered successfully.
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate resources for necessary data structures.
  @retval EFI_INVALID_PARAMETER    KeyData or NotifyHandle is NULL.

**/
EFI_STATUS
EFIAPI
TerminalConInRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT VOID                              **NotifyHandle
  )
{
  TERMINAL_DEV                    *TerminalDevice;
  TERMINAL_CONSOLE_IN_EX_NOTIFY   *NewNotify;
  LIST_ENTRY                      *Link;
  LIST_ENTRY                      *NotifyList;
  TERMINAL_CONSOLE_IN_EX_NOTIFY   *CurrentNotify;

  if (KeyData == NULL || NotifyHandle == NULL || KeyNotificationFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TerminalDevice = TERMINAL_CON_IN_EX_DEV_FROM_THIS (This);

  //
  // Return EFI_SUCCESS if the (KeyData, NotificationFunction) is already registered.
  //
  NotifyList = &TerminalDevice->NotifyList;
  for (Link = GetFirstNode (NotifyList); !IsNull (NotifyList,Link); Link = GetNextNode (NotifyList,Link)) {
    CurrentNotify = CR (
                      Link,
                      TERMINAL_CONSOLE_IN_EX_NOTIFY,
                      NotifyEntry,
                      TERMINAL_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (IsKeyRegistered (&CurrentNotify->KeyData, KeyData)) {
      if (CurrentNotify->KeyNotificationFn == KeyNotificationFunction) {
        *NotifyHandle = CurrentNotify;
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Allocate resource to save the notification function
  //
  NewNotify = (TERMINAL_CONSOLE_IN_EX_NOTIFY *) AllocateZeroPool (sizeof (TERMINAL_CONSOLE_IN_EX_NOTIFY));
  if (NewNotify == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewNotify->Signature         = TERMINAL_CONSOLE_IN_EX_NOTIFY_SIGNATURE;
  NewNotify->KeyNotificationFn = KeyNotificationFunction;
  CopyMem (&NewNotify->KeyData, KeyData, sizeof (EFI_KEY_DATA));
  InsertTailList (&TerminalDevice->NotifyList, &NewNotify->NotifyEntry);

  *NotifyHandle = NewNotify;

  return EFI_SUCCESS;
}


/**
  Remove a registered notification function from a particular keystroke.

  @param  This                      Protocol instance pointer.
  @param  NotificationHandle  The handle of the notification function being unregistered.

  @retval EFI_SUCCESS                     The notification function was unregistered successfully.
  @retval EFI_INVALID_PARAMETER    The NotificationHandle is invalid.

**/
EFI_STATUS
EFIAPI
TerminalConInUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN VOID                               *NotificationHandle
  )
{
  TERMINAL_DEV                    *TerminalDevice;
  LIST_ENTRY                      *Link;
  TERMINAL_CONSOLE_IN_EX_NOTIFY   *CurrentNotify;
  LIST_ENTRY                      *NotifyList;

  if (NotificationHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TerminalDevice = TERMINAL_CON_IN_EX_DEV_FROM_THIS (This);

  NotifyList = &TerminalDevice->NotifyList;
  for (Link = GetFirstNode (NotifyList); !IsNull (NotifyList,Link); Link = GetNextNode (NotifyList,Link)) {
    CurrentNotify = CR (
                      Link,
                      TERMINAL_CONSOLE_IN_EX_NOTIFY,
                      NotifyEntry,
                      TERMINAL_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (CurrentNotify == NotificationHandle) {
      //
      // Remove the notification function from NotifyList and free resources
      //
      RemoveEntryList (&CurrentNotify->NotifyEntry);

      gBS->FreePool (CurrentNotify);
      return EFI_SUCCESS;
    }
  }

  //
  // Can not find the matching entry in database.
  //
  return EFI_INVALID_PARAMETER;
}

/**
  Translate raw data into Unicode (according to different encode), and
  translate Unicode into key information. (according to different standard).

  @param  TerminalDevice       Terminal driver private structure.

**/
VOID
TranslateRawDataToEfiKey (
  IN  TERMINAL_DEV      *TerminalDevice
  )
{
  switch (TerminalDevice->TerminalType) {

  case PCANSITYPE:
  case VT100TYPE:
  case VT100PLUSTYPE:
    AnsiRawDataToUnicode (TerminalDevice);
    UnicodeToEfiKey (TerminalDevice);
    break;

  case VTUTF8TYPE:
    //
    // Process all the raw data in the RawFIFO,
    // put the processed key into UnicodeFIFO.
    //
    VTUTF8RawDataToUnicode (TerminalDevice);

    //
    // Translate all the Unicode data in the UnicodeFIFO to Efi key,
    // then put into EfiKeyFIFO.
    //
    UnicodeToEfiKey (TerminalDevice);

    break;
  }
}

/**
  Event notification function for EFI_SIMPLE_TEXT_INPUT_PROTOCOL.WaitForKey event
  Signal the event if there is key available

  @param  Event       Indicates the event that invoke this function.
  @param  Context    Indicates the calling context.

**/
VOID
EFIAPI
TerminalConInWaitForKey (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  //
  // Someone is waiting on the keystroke event, if there's
  // a key pending, signal the event
  //
  if (!IsEfiKeyFiFoEmpty ((TERMINAL_DEV *) Context)) {
    gBS->SignalEvent (Event);
  }
}

/**
  Timer handler to poll the key from serial.

  @param  Event       Indicates the event that invoke this function.
  @param  Context    Indicates the calling context.
  
**/
VOID
EFIAPI
TerminalConInTimerHandler (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  EFI_STATUS              Status;
  TERMINAL_DEV            *TerminalDevice;
  UINT32                  Control;
  UINT8                   Input;
  EFI_SERIAL_IO_MODE      *Mode;
  EFI_SERIAL_IO_PROTOCOL  *SerialIo;
  UINTN                   SerialInTimeOut;
  
  TerminalDevice  = (TERMINAL_DEV *) Context;

  SerialIo = TerminalDevice->SerialIo;
  if (SerialIo == NULL) {
    return ;
  }
  //
  //  if current timeout value for serial device is not identical with
  //  the value saved in TERMINAL_DEV structure, then recalculate the
  //  timeout value again and set serial attribute according to this value.
  //
  Mode = SerialIo->Mode;
  if (Mode->Timeout != TerminalDevice->SerialInTimeOut) {

    SerialInTimeOut = 0;
    if (Mode->BaudRate != 0) {
      //
      // According to BAUD rate to calculate the timeout value.
      //
      SerialInTimeOut = (1 + Mode->DataBits + Mode->StopBits) * 2 * 1000000 / (UINTN) Mode->BaudRate;
    }
    
    if (FeaturePcdGet(PcdCRSrvManagerSupported)) {
      SerialInTimeOut = 200000;
    }

    Status = SerialIo->SetAttributes (
                         SerialIo,
                         Mode->BaudRate,
                         Mode->ReceiveFifoDepth,
                         (UINT32) SerialInTimeOut,
                         (EFI_PARITY_TYPE) (Mode->Parity),
                         (UINT8) Mode->DataBits,
                         (EFI_STOP_BITS_TYPE) (Mode->StopBits)
                         );

    if (EFI_ERROR (Status)) {
      TerminalDevice->SerialInTimeOut = 0;
    } else {
      TerminalDevice->SerialInTimeOut = SerialInTimeOut;
    }
  }
  //
  // Check whether serial buffer is empty.
  //
  Status = SerialIo->GetControl (SerialIo, &Control);

  if ((Control & EFI_SERIAL_INPUT_BUFFER_EMPTY) == 0) {
    //
    // Fetch all the keys in the serial buffer,
    // and insert the byte stream into RawFIFO.
    //
    while (!IsRawFiFoFull (TerminalDevice)) {

      Status = GetOneKeyFromSerial (TerminalDevice->SerialIo, &Input);

      if (EFI_ERROR (Status)) {
        if (Status == EFI_DEVICE_ERROR) {
          REPORT_STATUS_CODE_WITH_DEVICE_PATH (
            EFI_ERROR_CODE | EFI_ERROR_MINOR,
            (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_EC_INPUT_ERROR),
            TerminalDevice->DevicePath
            );
        }
        break;
      }

      RawFiFoInsertOneKey (TerminalDevice, Input);
    }
  }

  //
  // Translate all the raw data in RawFIFO into EFI Key,
  // according to different terminal type supported.
  //
  TranslateRawDataToEfiKey (TerminalDevice);
}

/**
  Get one key out of serial buffer.

  @param  SerialIo   Serial I/O protocol attached to the serial device.
  @param  Output    The fetched key.

  @retval EFI_NOT_READY        If serial buffer is empty.
  @retval EFI_DEVICE_ERROR   If reading serial buffer encounter error.
  @retval EFI_SUCCESS            If reading serial buffer successfully, put
                                            the fetched key to the parameter output.

**/
EFI_STATUS
GetOneKeyFromSerial (
  EFI_SERIAL_IO_PROTOCOL  *SerialIo,
  UINT8                   *Output
  )
{
  EFI_STATUS    Status;
  UINTN         Size;

  Size    = 1;
  *Output = 0;

  //
  // Read one key from serial I/O device.
  //
  Status  = SerialIo->Read (SerialIo, &Size, Output);

  if (EFI_ERROR (Status)) {

    if (Status == EFI_TIMEOUT) {
      return EFI_NOT_READY;
    }

    return EFI_DEVICE_ERROR;

  }

  if (*Output == 0) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}


/**
  Insert one byte raw data into the Raw Data FIFO.

  @param  TerminalDevice       Terminal driver private structure.
  @param  Input                    The key will be input.

  @retval TRUE          If insert successfully.
  @retval FLASE         If Raw Data buffer is full before key insertion,
                               and the key is lost.

**/
BOOLEAN
RawFiFoInsertOneKey (
  TERMINAL_DEV      *TerminalDevice,
  UINT8             Input
  )
{
  UINT8    Tail;

  Tail = TerminalDevice->RawFiFo->Tail;

  if (IsRawFiFoFull (TerminalDevice)) {
    //
    // Raw FIFO is full
    //
    return FALSE;
  }

  TerminalDevice->RawFiFo->Data[Tail] = Input;

  TerminalDevice->RawFiFo->Tail = (UINT8) ((Tail + 1) % (RAW_FIFO_MAX_NUMBER + 1));

  return TRUE;
}


/**
  Remove one pre-fetched key out of the Raw Data FIFO.

  @param  TerminalDevice    Terminal driver private structure.
  @param  Output               The key will be removed.

  @retval TRUE        If insert successfully.
  @retval FLASE       If Raw Data FIFO buffer is empty before remove operation.

**/
BOOLEAN
RawFiFoRemoveOneKey (
  TERMINAL_DEV  *TerminalDevice,
  UINT8         *Output
  )
{
  UINT8    Head;

  Head = TerminalDevice->RawFiFo->Head;

  if (IsRawFiFoEmpty (TerminalDevice)) {
    //
    //  FIFO is empty
    //
    *Output = 0;
    return FALSE;
  }

  *Output = TerminalDevice->RawFiFo->Data[Head];

  TerminalDevice->RawFiFo->Head = (UINT8) ((Head + 1) % (RAW_FIFO_MAX_NUMBER + 1));

  return TRUE;
}


/**
  Clarify whether Raw Data FIFO buffer is empty.

  @param  TerminalDevice       Terminal driver private structure

  @retval TRUE                 If Raw Data FIFO buffer is empty.
  @retval FLASE                If Raw Data FIFO buffer is not empty.

**/
BOOLEAN
IsRawFiFoEmpty (
  TERMINAL_DEV  *TerminalDevice
  )
{
  if (TerminalDevice->RawFiFo->Head == TerminalDevice->RawFiFo->Tail) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Clarify whether Raw Data FIFO buffer is full.

  @param  TerminalDevice       Terminal driver private structure

  @retval TRUE                 If Raw Data FIFO buffer is full.
  @retval FLASE                If Raw Data FIFO buffer is not full.

**/
BOOLEAN
IsRawFiFoFull (
  TERMINAL_DEV  *TerminalDevice
  )
{
  UINT8 Tail;
  UINT8 Head;

  Tail  = TerminalDevice->RawFiFo->Tail;
  Head  = TerminalDevice->RawFiFo->Head;

  if (((Tail + 1) % (RAW_FIFO_MAX_NUMBER + 1)) == Head) {

    return TRUE;
  }

  return FALSE;
}

/**
  Insert one pre-fetched key into the FIFO buffer.

  @param  TerminalDevice       Terminal driver private structure.
  @param  Key                       The key will be input.

  @retval TRUE                 If insert successfully.
  @retval FLASE                If FIFO buffer is full before key insertion,
                                      and the key is lost.

**/
BOOLEAN
EfiKeyFiFoInsertOneKey (
  TERMINAL_DEV           *TerminalDevice,
  EFI_KEY_DATA           Key
  )
{
  UINT8                           Tail;
  LIST_ENTRY                      *Link;
  LIST_ENTRY                      *NotifyList;
  TERMINAL_CONSOLE_IN_EX_NOTIFY   *CurrentNotify;

  Tail = TerminalDevice->EfiKeyFiFo->Tail;

  //
  // Invoke notification functions if exist
  //
  NotifyList = &TerminalDevice->NotifyList;
  for (Link = GetFirstNode (NotifyList); !IsNull (NotifyList,Link); Link = GetNextNode (NotifyList,Link)) {
    CurrentNotify = CR (
                      Link,
                      TERMINAL_CONSOLE_IN_EX_NOTIFY,
                      NotifyEntry,
                      TERMINAL_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (IsKeyRegistered (&CurrentNotify->KeyData, &Key)) {

      //
	    // set KeyShiftState as Vaild for HotKey notify function verify
      //
      Key.KeyState.KeyShiftState  |= EFI_SHIFT_STATE_VALID;
      if (TerminalDevice->CrsEnable) {
        CurrentNotify->KeyNotificationFn (&Key);
	  }
    }
  }

  //
  // Check HotKey here for NonEscSeq hotkey.  
  //
  if (TerminalDevice->CrsEnable) {
    SetMonitorKey (Key);
  }
  
  if (IsEfiKeyFiFoFull (TerminalDevice)) {
    //
    // Efi Key FIFO is full
    //
    return FALSE;
  }

  CopyMem (&TerminalDevice->EfiKeyFiFo->Data[Tail], &Key, sizeof (EFI_KEY_DATA));

  TerminalDevice->EfiKeyFiFo->Tail = (UINT8) ((Tail + 1) % (FIFO_MAX_NUMBER + 1));

  return TRUE;
}

/**
  Remove one pre-fetched key out of the FIFO buffer.

  @param  TerminalDevice    Terminal driver private structure.
  @param  Output               The key will be removed.

  @retval TRUE                 If insert successfully.
  @retval FLASE                If FIFO buffer is empty before remove operation.

**/
BOOLEAN
EfiKeyFiFoRemoveOneKey (
  TERMINAL_DEV  *TerminalDevice,
  EFI_KEY_DATA  *Output
  )
{
  UINT8    Head;

  Head = TerminalDevice->EfiKeyFiFo->Head;
  ASSERT (Head < FIFO_MAX_NUMBER + 1);

  if (IsEfiKeyFiFoEmpty (TerminalDevice)) {
    //
    //  FIFO is empty
    //
    Output->Key.ScanCode = SCAN_NULL;
    Output->Key.UnicodeChar = 0;
    return FALSE;
  }

  *Output = TerminalDevice->EfiKeyFiFo->Data[Head];

  TerminalDevice->EfiKeyFiFo->Head = (UINT8) ((Head + 1) % (FIFO_MAX_NUMBER + 1));

  return TRUE;
}


/**
  Clarify whether FIFO buffer is empty.

  @param  TerminalDevice       Terminal driver private structure

  @retval TRUE                 If FIFO buffer is empty.
  @retval FLASE                If FIFO buffer is not empty.

**/
BOOLEAN
IsEfiKeyFiFoEmpty (
  TERMINAL_DEV  *TerminalDevice
  )
{
  if (TerminalDevice->EfiKeyFiFo->Head == TerminalDevice->EfiKeyFiFo->Tail) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Clarify whether FIFO buffer is full.

  @param  TerminalDevice       Terminal driver private structure

  @retval TRUE                 If FIFO buffer is full.
  @retval FLASE                If FIFO buffer is not full.

**/
BOOLEAN
IsEfiKeyFiFoFull (
  TERMINAL_DEV  *TerminalDevice
  )
{
  UINT8 Tail;
  UINT8 Head;

  Tail  = TerminalDevice->EfiKeyFiFo->Tail;
  Head  = TerminalDevice->EfiKeyFiFo->Head;

  if (((Tail + 1) % (FIFO_MAX_NUMBER + 1)) == Head) {

    return TRUE;
  }

  return FALSE;
}

/**
  Insert one pre-fetched key into the Unicode FIFO buffer.

  @param  TerminalDevice   Terminal driver private structure.
  @param  Input                The key will be input.

  @retval TRUE                 If insert successfully.
  @retval FLASE                If Unicode FIFO buffer is full before key insertion,
                                     and the key is lost.

**/
BOOLEAN
UnicodeFiFoInsertOneKey (
  TERMINAL_DEV      *TerminalDevice,
  UINT16            Input
  )
{
  UINT8    Tail;

  Tail = TerminalDevice->UnicodeFiFo->Tail;
  ASSERT (Tail < FIFO_MAX_NUMBER + 1);

  if (IsUnicodeFiFoFull (TerminalDevice)) {
    //
    // Unicode FIFO is full
    //
    return FALSE;
  }

  TerminalDevice->UnicodeFiFo->Data[Tail] = Input;

  TerminalDevice->UnicodeFiFo->Tail = (UINT8) ((Tail + 1) % (FIFO_MAX_NUMBER + 1));

  return TRUE;
}

/**
  Remove one pre-fetched key out of the Unicode FIFO buffer.

  @param  TerminalDevice    Terminal driver private structure.
  @param  Output               The key will be removed.

  @retval TRUE                 If insert successfully.
  @retval FLASE                If Unicode FIFO buffer is empty before remove operation.

**/
BOOLEAN
UnicodeFiFoRemoveOneKey (
  TERMINAL_DEV  *TerminalDevice,
  UINT16        *Output
  )
{
  UINT8    Head;

  Head = TerminalDevice->UnicodeFiFo->Head;
  ASSERT (Head < FIFO_MAX_NUMBER + 1);

  if (IsUnicodeFiFoEmpty (TerminalDevice)) {
    //
    //  FIFO is empty
    //
    Output = NULL;
    return FALSE;
  }

  *Output = TerminalDevice->UnicodeFiFo->Data[Head];

  TerminalDevice->UnicodeFiFo->Head = (UINT8) ((Head + 1) % (FIFO_MAX_NUMBER + 1));

  return TRUE;
}


/**
  Clarify whether Unicode FIFO buffer is empty.

  @param  TerminalDevice       Terminal driver private structure

  @retval TRUE                 If Unicode FIFO buffer is empty.
  @retval FLASE                If Unicode FIFO buffer is not empty.

**/
BOOLEAN
IsUnicodeFiFoEmpty (
  TERMINAL_DEV  *TerminalDevice
  )
{
  if (TerminalDevice->UnicodeFiFo->Head == TerminalDevice->UnicodeFiFo->Tail) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/**
  Clarify whether Unicode FIFO buffer is full.

  @param  TerminalDevice       Terminal driver private structure

  @retval TRUE                 If Unicode FIFO buffer is full.
  @retval FLASE                If Unicode FIFO buffer is not full.

**/
BOOLEAN
IsUnicodeFiFoFull (
  TERMINAL_DEV  *TerminalDevice
  )
{
  UINT8 Tail;
  UINT8 Head;

  Tail  = TerminalDevice->UnicodeFiFo->Tail;
  Head  = TerminalDevice->UnicodeFiFo->Head;

  if (((Tail + 1) % (FIFO_MAX_NUMBER + 1)) == Head) {
    return TRUE;
  }

  return FALSE;
}


/**
  Count Unicode FIFO buffer.

  @param  TerminalDevice       Terminal driver private structure

  @return The count in bytes of Unicode FIFO.

**/
UINT8
UnicodeFiFoGetKeyCount (
  TERMINAL_DEV    *TerminalDevice
  )
{
  UINT8    Tail;
  UINT8    Head;

  Tail = TerminalDevice->UnicodeFiFo->Tail;
  Head = TerminalDevice->UnicodeFiFo->Head;

  if (Tail >= Head) {
    return (UINT8) (Tail - Head);
  } else {
    return (UINT8) (Tail + FIFO_MAX_NUMBER + 1 - Head);
  }
}


/**
  Update the Unicode characters from a terminal input device into EFI Keys FIFO.

  @param TerminalDevice   The terminal device to use to translate raw input into EFI Keys

**/
VOID
UnicodeToEfiKeyFlushState (
  IN  TERMINAL_DEV    *TerminalDevice
  )
{
  EFI_KEY_DATA  Key;
  UINT32        InputState;

  InputState = TerminalDevice->InputState;

  if (IsEfiKeyFiFoFull (TerminalDevice)) {
    return;
  }

  if ((InputState & INPUT_STATE_ESC) != 0) {
    Key.Key.ScanCode = SCAN_ESC;
    Key.Key.UnicodeChar = 0;
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
  }

  if ((InputState & INPUT_STATE_CSI) != 0) {
    Key.Key.ScanCode    = SCAN_NULL;
    Key.Key.UnicodeChar = CSI;
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
  }

  if ((InputState & INPUT_STATE_LEFTOPENBRACKET) != 0) {
    Key.Key.ScanCode    = SCAN_NULL;
    Key.Key.UnicodeChar = LEFTOPENBRACKET;
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
  }

  if ((InputState & INPUT_STATE_O) != 0) {
    Key.Key.ScanCode    = SCAN_NULL;
    Key.Key.UnicodeChar = 'O';
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
  }

  if ((InputState & INPUT_STATE_2) != 0) {
    Key.Key.ScanCode    = SCAN_NULL;
    Key.Key.UnicodeChar = '2';
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
  }

  //
  // Cancel the timer.
  //
  gBS->SetTimer (
         TerminalDevice->TwoSecondTimeOut,
         TimerCancel,
         0
         );

  TerminalDevice->InputState = INPUT_STATE_DEFAULT;
}


/**
  Sets monitor keys for SCU (SETUP_HOT_KEY / BOOT_MANAGER_HOT_KEY...).

  @param ScanCode   Scan Code

**/
VOID
SetMonitorKey (
  IN      EFI_KEY_DATA    Key
  )
{
  UINT8   *FilterKeyList;
  UINTN   Index;
  UINTN   BitIndex;
  UINT16  ScanCode;
  
  //
  // Skip if no MonitorKey installed
  //
  if (EBDA(EBDA_MONITOR_KEY_TABLE) != 0x55) {
    return;
  }

  ScanCode = Key.Key.ScanCode;
  
  //
  // Check input key with EfiScanCode and 
  // convert to KBC converted code 
  //
  for (Index = 0; gEfiToKbScanCodeMap[Index].EfiScanCode != 0; Index ++) {
    if (gEfiToKbScanCodeMap[Index].EfiScanCode == ScanCode) {
      ScanCode = gEfiToKbScanCodeMap[Index].KbScanCode;
      //
      // F11: 0x57->0x85; F12: 0x58->0x86
      //
      if (ScanCode == 0x57 || ScanCode == 0x58) {
        ScanCode = 0x85 + (ScanCode - 0x57);
      }
    }
  }
  
  //
  // If input key doesn't have EfiScancode, we check it's UniCode
  //    
  for (Index = 0; gUniCodeToKbScanCodeMap[Index][0] != 0; Index ++) {
    if ((gUniCodeToKbScanCodeMap[Index][0] == Key.Key.UnicodeChar) ||
        (gUniCodeToKbScanCodeMap[Index][1] == Key.Key.UnicodeChar)) {
      ScanCode = gUniCodeToKbScanCodeMap[Index][2];
    }
  }
  
  //
  // Set function key flag
  //
  FilterKeyList = (UINT8*)(UINTN)((*(UINT16*)&(EBDA(EBDA_MONITOR_KEY_TABLE + 3)) << 4) + *(UINT16*)&(EBDA(EBDA_MONITOR_KEY_TABLE + 1)));
  for (Index = 0, BitIndex = 0; FilterKeyList[Index] != 0; Index += 3, BitIndex ++) {
    if (FilterKeyList[Index] == (UINT8) ScanCode) {
      *(UINT32*)&(EBDA(EBDA_MONITOR_KEY_TABLE + 5)) |= (1 << BitIndex);
      break;
    }
  }
}


/**
  Flush out terminal temporary fifo data.

  @param TerminalDevice   terminal device

**/
VOID 
TempFiFoFlush (
  IN TERMINAL_DEV    *TerminalDevice
  )
{
  UINTN           Index;
  UNICODE_FIFO    *pTempFiFo;
  EFI_KEY_DATA    Key;

  pTempFiFo = TerminalDevice->TempFiFo;

  if (TerminalDevice->InputState == INPUT_STATE_ESC) {

    // Insert ESC to EfiKey FIFO
    Key.Key.ScanCode = SCAN_ESC;
    Key.Key.UnicodeChar = 0;
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
    if (TerminalDevice->CrsEnable) {
      SetMonitorKey (Key);
    }
    // Reset Input state
    TerminalDevice->InputState = INPUT_STATE_DEFAULT;
  }
  
  for (Index = 0; Index < pTempFiFo->Tail; Index++) {

    if (pTempFiFo->Data[Index] == ESC) {
      Key.Key.ScanCode    = SCAN_ESC;
      Key.Key.UnicodeChar = 0;
    }
    else {
      Key.Key.ScanCode    = SCAN_NULL;
      Key.Key.UnicodeChar = pTempFiFo->Data[Index];
    }
    
    EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
    if (TerminalDevice->CrsEnable) {
      SetMonitorKey (Key);
    }
    pTempFiFo->Data[Index] = 0;
  }

  pTempFiFo->Data[Index] = 0;
  pTempFiFo->Tail = 0;

}


/**
  Insert input key to terminal temporary fifo.

  @param TerminalDevice   Terminal device
  @param Input                 The key for insert to temporary fifo
  
  @retval TRUE                  The key inserted.
  @retval FALSE                 The key insert fail due to temporary fifo full.  
  
**/
BOOLEAN
TempFiFoInsertOneKey (
  IN TERMINAL_DEV    *TerminalDevice,
  IN UINT16          Input
  )
{
  UNICODE_FIFO    *pTempFiFo;
  
  pTempFiFo = TerminalDevice->TempFiFo;

  if ( pTempFiFo->Tail >= FIFO_MAX_NUMBER ) {
    return FALSE;
  }
    
  pTempFiFo->Data[pTempFiFo->Tail] = Input;
  pTempFiFo->Tail++; 

  return TRUE;
}

/**
  Get one key from terminal temporary fifo.

  @param TerminalDevice   Terminal device
  @param Input                 The key get from temporary fifo
  
  @retval TRUE                  Get one key from temporary fifo success.
  @retval FALSE                 The temporary fifo is empty.  
  
**/
BOOLEAN
TempFiFoRemoveOneKey (
  IN  TERMINAL_DEV    *TerminalDevice,
  OUT UINT16          *pBuffer
  )
{
  UNICODE_FIFO    *pTempFiFo;
  
  pTempFiFo = TerminalDevice->TempFiFo;

  if (pTempFiFo->Tail == 0) {
    return FALSE;
  }

  *pBuffer = pTempFiFo->Data[pTempFiFo->Tail - 1];
  pTempFiFo->Data[pTempFiFo->Tail - 1] = 0;
  pTempFiFo->Tail--;

  return TRUE;
}


/**
  Clean up temporary fifo.

  @param TerminalDevice   Terminal device
    
**/
VOID
TempFiFoCleanUp (
  IN TERMINAL_DEV      *TerminalDevice
  )
{
  UNICODE_FIFO    *pTempFiFo;
  UINTN           Index;
  
  pTempFiFo = TerminalDevice->TempFiFo;

  for (Index = 0; Index <= pTempFiFo->Tail; Index++) {
    pTempFiFo->Data[Index] = 0;
  }

  pTempFiFo->Tail = 0;
}


/**
  Compare input EscSequenceCode with terminal EscSequenceCode table.

  @param TerminalDevice   Terminal device
  @param pEscSeqCode      Pointer to EscSequenceCode buffer

  @retval ESC_CODE_FINDED         The specific EscSequenceCode Found.
  @retval ESC_CODE_UNFINISHED  The specific EscSequenceCode not support.
  @retval ESC_CODE_INVALID        The invalid EscSequenceCode.
    
**/
ESC_CODE_STATUS
TempFiFoCompEscSeqCode (
  IN TERMINAL_DEV    *TerminalDevice,
  IN CHAR16          *pEscSeqCode
  )
{
  UNICODE_FIFO    *pTempFiFo;
  UINTN           Index;
  
  pTempFiFo = TerminalDevice->TempFiFo;

  Index = 0;
  while (*pEscSeqCode != 0 && pTempFiFo->Data[Index] != 0) {
    if(*pEscSeqCode != pTempFiFo->Data[Index]) {
      break;
    }
    Index++;
    pEscSeqCode++;
  }

  if (*pEscSeqCode == 0  && pTempFiFo->Data[Index] == 0) {
    return ESC_CODE_FINDED;
  }
    
  if (pTempFiFo->Data[Index] == 0  && *pEscSeqCode != 0) {
    return ESC_CODE_UNFINISHED;
  }

  return ESC_CODE_INVALID;
}


/**
  Insert input key to terminal special command fifo.

  @param TerminalDevice   Terminal device
  @param Unicode             The unicode for insert to special command fifo
  
  @retval TRUE                  The unicode inserted.
  @retval FALSE                 The unicode insert fail due to special command fifo full.  
  
**/
BOOLEAN
SpcFiFoInsertOneChar (
  TERMINAL_DEV    *TerminalDevice,
  UINT16          Unicode
  )
{
  UNICODE_FIFO    *pTempFiFo;
  
  pTempFiFo = TerminalDevice->SpcFiFo;

  if ( pTempFiFo->Tail >= FIFO_MAX_NUMBER ) {
    return FALSE;
  }
    
  pTempFiFo->Data[pTempFiFo->Tail] = Unicode;
  pTempFiFo->Tail++; 

  return TRUE;
}


/**
  Reset terminal special command fifo.

  @param TerminalDevice   Terminal device
    
**/
VOID
SpcFiFoReset (
  TERMINAL_DEV        *TerminalDevice
  )
{
  ZeroMem (TerminalDevice->SpcFiFo, sizeof(UNICODE_FIFO));
}


/**
  Compare input SpecialCommand with terminal SpecialCommand table.

  @param TerminalDevice    Terminal device
  @param pCommandStr      Pointer to SpecialCommand buffer

  @retval SPC_COMMAND_STRING   The specific SpecialCommand Found.
  @retval SPC_WAIT_CHAR            The specific SpecialCommand not support.
  @retval SPC_INVALID_STRING     The invalid SpecialCommand.
    
**/
SPC_STATUS
SpcFiFoCompString (
  IN  TERMINAL_DEV        *TerminalDevice,
  IN  CHAR16              *pCommandStr
  )
{
  UNICODE_FIFO    *pSpcFiFo;
  UINTN           Index = 0;
  
  pSpcFiFo = TerminalDevice->SpcFiFo;

  while (*pCommandStr != 0 && pSpcFiFo->Data[Index] != 0) {
    if(*pCommandStr != pSpcFiFo->Data[Index]) {
      break;
    }

    Index++;
    pCommandStr++;
  }

  if (*pCommandStr == 0  && pSpcFiFo->Data[Index] == 0) {
    return SPC_COMMAND_STRING;
  }
    
  if (pSpcFiFo->Data[Index] == 0  && *pCommandStr != 0) {
    return SPC_WAIT_CHAR;
  }

  return SPC_INVALID_STRING;

}



/**
  Search input SpecialCommand with terminal SpecialCommand table.

  @param TerminalDevice             Terminal device
  @param pCrSpecialCommand      Pointer to SpecialCommand buffer
  @param ResultIndex                  Index of SpecialCommand table    

  @retval SPC_COMMAND_STRING   The specific SpecialCommand Found.
  @retval SPC_WAIT_CHAR            The specific SpecialCommand not support.
  @retval SPC_INVALID_STRING     The invalid SpecialCommand.
    
**/
UINT8
LookUpSpcCommandCode (
  IN  TERMINAL_DEV        *TerminalDevice,
  IN  CR_SPECIAL_COMMAND  *pCrSpecialCommand,
  OUT UINT16              *ResultIndex
  )
{
  UINT8       Status;
  UINT16      Index;
  
  for (Index = 0; pCrSpecialCommand[Index].CommandStr != 0; Index++) {
  
    Status = SpcFiFoCompString (TerminalDevice, pCrSpecialCommand[Index].CommandStr);

    switch (Status) {
      case SPC_WAIT_CHAR :
        return Status;
        
      case SPC_COMMAND_STRING :
        *ResultIndex = Index;
        return Status;
    }
  }

  return SPC_INVALID_STRING;
}


/**
  Execute SpecialCommand.

  @param TerminalDevice             Terminal device
  @param pCrSpecialCommand      Pointer to SpecialCommand buffer
  @param Index                          Index of SpecialCommand table    
    
**/
VOID
ExecuteSpcCommand (
  IN CR_SPECIAL_COMMAND  *pCrSpecialCommand,
  IN UINT16              Index
  )
{
  switch (pCrSpecialCommand[Index].CommandType) {
    case TYPE_FUNCTION_ID :
      if (pCrSpecialCommand[Index].Command == FUNC_RESET) { 
        gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
      }
      break; 

    default :
      break;
  }
}


/**
  Function for process console redirection SpecialCommand.

  @param TerminalDevice             Terminal device  
  @param pCrSpecialCommand      Pointer to SpecialCommand buffer
  @param UnicodeChar                 Unicode to recognize as terminal SpecialCommand    
    
**/
VOID
ProcessCrSpecialCommand (
  IN TERMINAL_DEV        *TerminalDevice,
  IN CR_SPECIAL_COMMAND  *pCrSpecialCommand,
  IN UINT16              UnicodeChar
  )
{
  BOOLEAN     BStatus;
  UINT8       SpcStatus;
  UINT16      TableIndex;

  if (!TerminalDevice->CrsEnable) {
    return;
  }
  
  BStatus = SpcFiFoInsertOneChar (TerminalDevice, UnicodeChar);
  if (BStatus == FALSE) {
    SpcFiFoReset (TerminalDevice);
    return;
  }

  SpcStatus = LookUpSpcCommandCode (TerminalDevice, pCrSpecialCommand, &TableIndex);

  switch (SpcStatus) {
    case SPC_COMMAND_STRING : 
      ExecuteSpcCommand (pCrSpecialCommand, TableIndex);
      break;

    case SPC_WAIT_CHAR :
      break;

    case SPC_INVALID_STRING :
      SpcFiFoReset (TerminalDevice);

    default:
      break;
  }

}


/**
  Search input EscSequenceCode with terminal EscSequenceCode table.

  @param TerminalDevice             Terminal device
  @param Method                        Search method  
  @param pEscSeqCodeTable         Pointer to EscSequenceCode buffer
  @param ResultIndex                  Index of EscSequenceCode table    

  @retval ESC_CODE_FINDED          The EscSequenceCode found.
  @retval ESC_CODE_UNFINISHED   The EscSequenceCode was not complete.
  @retval ESC_CODE_INVALID         The invalid EscSequenceCode.
    
**/
ESC_CODE_STATUS
LookUpEscSequenceCodeTable (
  IN TERMINAL_DEV        *TerminalDevice,
  IN UINT8               Method,
  IN ESC_SEQUENCE_CODE   *pEscSeqCodeTable,
  OUT UINTN              *ResultIndex
  )
{
  UINTN             Index;
  ESC_CODE_STATUS   Status;
  ESC_CODE_STATUS   EscCodeStatus;

  EscCodeStatus = ESC_CODE_INVALID; 
  for (Index = 0; pEscSeqCodeTable[Index].TerminalType != 0; Index++) {

    if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_KEY_FLAG) == CR_TERMINAL_KEY_SENSITIVE) {
      if ((pEscSeqCodeTable[Index].TerminalType & TerminalDevice->TerminalType) == 0) {
        continue;
      }
    }

    Status = TempFiFoCompEscSeqCode (TerminalDevice, pEscSeqCodeTable[Index].EscSequenceCode);
   
    switch (Status) {
      case ESC_CODE_UNFINISHED :
        if (Method == CHECK_FIRST_MATCH) {
          break;
        }
        return ESC_CODE_UNFINISHED;          
        
      case ESC_CODE_FINDED :
        *ResultIndex = Index;
        EscCodeStatus = ESC_CODE_FINDED;

        if (Method == CHECK_FIRST_MATCH) {
          return ESC_CODE_FINDED;
        }
        
      default:
        break;
    }
  }
  
  return EscCodeStatus;
}


/**
  Search input EscSequenceCode with terminal EscSequenceCode table.

  @param TerminalDevice             Terminal device
  @param Flag                            The flag setting that update to terminaldevice  
    
**/
VOID
TerminalToggleFlags (
  IN TERMINAL_DEV        *TerminalDevice,
  IN UINT16              Flag
  )
{
  UINT16  Data;
  
  Data = TerminalDevice->TerminalFeatureFlag & Flag;
  Data ^= Flag;
  TerminalDevice->TerminalFeatureFlag = (TerminalDevice->TerminalFeatureFlag & ~Flag) | Data;
}


/**
  Function for process console redirection EscSequenceCode.

  @param TerminalDevice           Terminal device  
  @param pEscSeqCodeTable       Pointer to EscSequenceCode buffer
  @param TableIndex                 Index of EscSequenceCode table    
    
**/
VOID 
ProcessEscSequenceCode (
  IN TERMINAL_DEV        *TerminalDevice,
  IN ESC_SEQUENCE_CODE   *pEscSeqCodeTable,
  OUT UINTN              TableIndex
  )
{
  UINT8                      DataType;
  EFI_KEY_DATA               Key;
  EFI_STATUS                 Status;
  CR_SRV_MANAGER_PROTOCOL    *CrSrvManager;
  
  DataType = pEscSeqCodeTable[TableIndex].DataType;

  switch (DataType) {
    case ESC_CODE_CONTROL :
      Key.Key.ScanCode    = 0;
      Key.Key.UnicodeChar = pEscSeqCodeTable[TableIndex].Data;
      EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
      break;

    case ESC_CODE_ALTCODE :
    case ESC_CODE_EXTENTION :
    case ESC_CODE_SCANCODE :
      Key.Key.ScanCode    = pEscSeqCodeTable[TableIndex].Data;
      Key.Key.UnicodeChar = 0;
      EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
      if (TerminalDevice->CrsEnable) {      
        SetMonitorKey (Key);
      }
      break;
      
    case ESC_CODE_FUNC :
      switch (pEscSeqCodeTable[TableIndex].Data) {
        case FUNC_RESET :
          gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
          break;

        case FUNC_VIDEO :
          TerminalToggleFlags (TerminalDevice, CR_TERMINAL_VIDEO_FLAG);
          break;

        case FUNC_SENSE :
          TerminalToggleFlags (TerminalDevice, CR_TERMINAL_KEY_FLAG);
          break;

        case FUNC_AUTO_REFRESH :  
          //
          // ConIn detect "\x1b[0n" esc sequence code.
          //
          if ((TerminalDevice->TerminalFeatureFlag & CR_AUTO_REFRESH_FLAG) == CR_AUTO_REFRESH_ENABLE) {
            //
            // Only refresh screen in last remote terminal device was not exist situation.
            //
            if (!TerminalDevice->LastRemoteTermStatus) {
              //
              // We need send disable AutoWrapMode Escstring after reconnect to remote terminal
              //
              OutputEscSquenceString (&TerminalDevice->SimpleTextOutput, gDiableAutoWrapString);
              ClearTermCharAndAttriBuffer (TerminalDevice);
            }
            TerminalDevice->RemoteTermExist = TRUE;
          } else {
            TempFiFoFlush (TerminalDevice);
          }
          break;

        case FUNC_MANUAL_REFRESH :
          //
          // ConIn detect "\x1bR" or "\x1br" esc sequence code.
          //
          if ((TerminalDevice->TerminalFeatureFlag & CR_MANUAL_REFRESH_FLAG) == CR_MANUAL_REFRESH_ENABLE) {
            OutputEscSquenceString (&TerminalDevice->SimpleTextOutput, gDiableAutoWrapString);
            //
            // Execute a manual screen refresh
            //
            ClearTermCharAndAttriBuffer (TerminalDevice);
          } else {
            TempFiFoFlush (TerminalDevice);
          }
          break;

        case FUNC_CHARSET :
          TerminalToggleFlags (TerminalDevice, CR_TERMINAL_CHARSET_FLAG); 
          break;

        case FUNC_NONVT100_ALTKEY :
          TerminalDevice->NonVt100AltKey = TRUE;
          gBS->SetTimer(TerminalDevice->TwoSecondTimeOut, TimerRelative, ESC_SEQUENCE_KEY_PERIOD);
          break;

        case FUNC_CR_SRV_MANAGER :
          Status = gBS->LocateProtocol (
                          &gCrSrvManagerProtocolGuid,
                          NULL,
                          (VOID **)&CrSrvManager
                          );
  
          if (Status == EFI_SUCCESS) { 
            CrSrvManager->CrSrvManagerEntry (TerminalDevice->Handle);
          }
          break;
          
        default :
          break;
      }
      
      break;
      
    default :
      break;
      
  } 
}


/**
  Function for process reset special command.

  @param TerminalDevice           Terminal device  
  @param UnicodeChar               Unicode to recognize as terminal SpecialCommand     
    
**/
VOID
ProcessEscResetCode (
  IN TERMINAL_DEV    *TerminalDevice,
  IN CHAR16          UnicodeChar
  )
{
  switch (TerminalDevice->ResetState) {
    case 1 :  // < ESC> 
    case 5 :  // < ESC> R <ESC> r <ESC>
      if (UnicodeChar == 'R') {
        if (TerminalDevice->ResetState == 5) {   
          gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
        }  
        TerminalDevice->ResetState++;  // 2  
      }
      else {
        TerminalDevice->ResetState = INPUT_STATE_DEFAULT;
      }

      break;
        
    case 2 :  // < ESC> R
    case 4 :  // < ESC> R <ESC> r
      if (UnicodeChar == ESC) {          
        TerminalDevice->ResetState++;    // 3 or 5
      } else { 
        TerminalDevice->ResetState = INPUT_STATE_DEFAULT;
      }
      break;

    case 3 : //< ESC> R <ESC>
      if (UnicodeChar == 'r') {        
        TerminalDevice->ResetState++;    // 4
      } else { 
        TerminalDevice->ResetState = INPUT_STATE_DEFAULT;
      }
      break;
      
    default:
      break;
  }
}


/**
  Function for process EscSequeceCode after lookup table fail.

  @param TerminalDevice           Terminal device  
    
**/
VOID
DoFinalEscCodeProcess (
  IN TERMINAL_DEV    *TerminalDevice
  )
{
  ESC_CODE_STATUS     LookUpStatus;
  UINTN               TableIndex;

  TableIndex = 0;
  LookUpStatus = LookUpEscSequenceCodeTable (TerminalDevice, CHECK_FIRST_MATCH, gEscSequenceCode, &TableIndex);

  switch (LookUpStatus) {
    case ESC_CODE_INVALID :
      TempFiFoFlush (TerminalDevice);
      break;

    case ESC_CODE_FINDED :
      ProcessEscSequenceCode (TerminalDevice, gEscSequenceCode, TableIndex);
      TempFiFoCleanUp(TerminalDevice);
      TerminalDevice->InputState = INPUT_STATE_DEFAULT;
      break;

    default :
      break;
  }

}


/**
  Confirm input code is alphabet or not.

  @param Code                  The input code for confirm
  
  @retval TRUE                  The input code is alphabet.
  @retval FALSE                 The input code is not alphabet.  
  
**/
BOOLEAN
IsAlphabet (
  IN UINT8   Code
  )
{
  if (Code >= '0' && Code <= '9') {
    return TRUE;
  }

  if (Code >= 'a' && Code <= 'z') {
    return TRUE;
  }

  if (Code >= 'A' && Code <= 'Z') {
    return TRUE;
  }
  
  return FALSE;
}

/**
  Converts a stream of Unicode characters from a terminal input device into EFI Keys that
  can be read through the Simple Input Protocol.

  The table below shows the keyboard input mappings that this function supports.
  If the ESC sequence listed in one of the columns is presented, then it is translated
  into the corresponding EFI Scan Code.  If a matching sequence is not found, then the raw
  key strokes are converted into EFI Keys.

  2 seconds are allowed for an ESC sequence to be completed.  If the ESC sequence is not
  completed in 2 seconds, then the raw key strokes of the partial ESC sequence are
  converted into EFI Keys.
  There is one special input sequence that will force the system to reset.
  This is ESC R ESC r ESC R.

  Note: current implementation support terminal types include: PC ANSI, VT100+/VTUTF8, VT100. 
        The table below is not same with UEFI Spec 2.3 Appendix B Table 201(not support ANSI X3.64 /
        DEC VT200-500 and extra support PC ANSI, VT100)since UEFI Table 201 is just an example.
        
  Symbols used in table below
  ===========================
    ESC = 0x1B
    CSI = 0x9B
    DEL = 0x7f
    ^   = CTRL

  +=========+======+===========+==========+==========+
  |         | EFI  | UEFI 2.0  |          |          |
  |         | Scan |           |  VT100+  |          |
  |   KEY   | Code |  PC ANSI  |  VTUTF8  |   VT100  |
  +=========+======+===========+==========+==========+
  | NULL    | 0x00 |           |          |          |
  | UP      | 0x01 | ESC [ A   | ESC [ A  | ESC [ A  |
  | DOWN    | 0x02 | ESC [ B   | ESC [ B  | ESC [ B  |
  | RIGHT   | 0x03 | ESC [ C   | ESC [ C  | ESC [ C  |
  | LEFT    | 0x04 | ESC [ D   | ESC [ D  | ESC [ D  |
  | HOME    | 0x05 | ESC [ H   | ESC h    | ESC [ H  |
  | END     | 0x06 | ESC [ F   | ESC k    | ESC [ K  |
  | INSERT  | 0x07 | ESC [ @   | ESC +    | ESC [ @  |
  |         |      | ESC [ L   |          | ESC [ L  |
  | DELETE  | 0x08 | ESC [ X   | ESC -    | ESC [ P  |
  | PG UP   | 0x09 | ESC [ I   | ESC ?    | ESC [ V  |
  |         |      |           |          | ESC [ ?  |
  | PG DOWN | 0x0A | ESC [ G   | ESC /    | ESC [ U  |
  |         |      |           |          | ESC [ /  |
  | F1      | 0x0B | ESC [ M   | ESC 1    | ESC O P  |
  | F2      | 0x0C | ESC [ N   | ESC 2    | ESC O Q  |
  | F3      | 0x0D | ESC [ O   | ESC 3    | ESC O w  |
  | F4      | 0x0E | ESC [ P   | ESC 4    | ESC O x  |
  | F5      | 0x0F | ESC [ Q   | ESC 5    | ESC O t  |
  | F6      | 0x10 | ESC [ R   | ESC 6    | ESC O u  |
  | F7      | 0x11 | ESC [ S   | ESC 7    | ESC O q  |
  | F8      | 0x12 | ESC [ T   | ESC 8    | ESC O r  |
  | F9      | 0x13 | ESC [ U   | ESC 9    | ESC O p  |
  | F10     | 0x14 | ESC [ V   | ESC 0    | ESC O M  |
  | Escape  | 0x17 | ESC       | ESC      | ESC      |
  | F11     | 0x15 |           | ESC !    |          |
  | F12     | 0x16 |           | ESC @    |          |
  +=========+======+===========+==========+==========+

  Special Mappings
  ================
  ESC R ESC r ESC R = Reset System

  @param TerminalDevice   The terminal device to use to translate raw input into EFI Keys

**/
VOID
UnicodeToEfiKey (
  IN  TERMINAL_DEV    *TerminalDevice
  )
  {
    EFI_STATUS          Status;
    EFI_STATUS          TimerStatus;
    UINT16              UnicodeChar;
    EFI_KEY_DATA        Key;
    ESC_CODE_STATUS     LookUpStatus;
    UINTN               TableIndex;

    TableIndex = 0;
    if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_COMBO_KEY_DISABLE) == CR_TERMINAL_COMBO_KEY_DISABLE) {
      while (!IsUnicodeFiFoEmpty(TerminalDevice)) {
        UnicodeFiFoRemoveOneKey (TerminalDevice, &UnicodeChar);
        if (UnicodeChar == ESC) {
          Key.Key.ScanCode    = SCAN_ESC;
          Key.Key.UnicodeChar = 0;
        } else {
          Key.Key.ScanCode    = 0;
          Key.Key.UnicodeChar = UnicodeChar;
        }
        EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
        return;
      }
    }
    
    TimerStatus = gBS->CheckEvent (TerminalDevice->TwoSecondTimeOut);
  
    if (!EFI_ERROR (TimerStatus)) {
      DoFinalEscCodeProcess (TerminalDevice);
    }
  
    while (!IsUnicodeFiFoEmpty(TerminalDevice)) {
      //
      // In ESC sequence state that must check 2 second.
      //
      if (TerminalDevice->InputState != INPUT_STATE_DEFAULT) {
        TimerStatus = gBS->CheckEvent (TerminalDevice->TwoSecondTimeOut);
        if (!EFI_ERROR (TimerStatus)) {
          DoFinalEscCodeProcess (TerminalDevice);
        }
      }
    
      UnicodeFiFoRemoveOneKey (TerminalDevice, &UnicodeChar);
  
      //2 Process Reset command   <ESC> R <ESC> r <ESC> R 
      if (TerminalDevice->ResetState != INPUT_STATE_DEFAULT) {
        ProcessEscResetCode(TerminalDevice, UnicodeChar);
      }

      //
      // if input key is ESC then check if we has a new reset ESC sequence start.
      //
      if (UnicodeChar == ESC) {
        if (TerminalDevice->ResetState == INPUT_STATE_DEFAULT) {
          TerminalDevice->ResetState = INPUT_STATE_ESC;
          gBS->SetTimer (TerminalDevice->ResetTimerEvent, TimerRelative, ESC_RESET_KEY_PERIOD);
        }
      }
      //2 Process normal input key
      if (TerminalDevice->InputState == INPUT_STATE_DEFAULT) {
        //
        // Process leading ESC code, set Timer and change input state.
        //
        if (UnicodeChar == ESC) {
          //
          // Restart a new 2s time period
          //
          Status = gBS->SetTimer(TerminalDevice->TwoSecondTimeOut, TimerRelative, ESC_SEQUENCE_KEY_PERIOD);
        
          TerminalDevice->InputState = INPUT_STATE_ESC;
        } else if (TerminalDevice->NonVt100AltKey) {
          //
          // received "ESC ^ A" sequence code and decode it as Alt Key
          //
          if (IsAlphabet((UINT8)UnicodeChar)) {
            //
            // Process Alt + key
            //
            Key.Key.ScanCode = SCAN_NULL;
            Key.Key.UnicodeChar = UnicodeChar;
            Key.KeyState.KeyShiftState |= (EFI_LEFT_ALT_PRESSED | EFI_RIGHT_ALT_PRESSED);
            EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
            TempFiFoCleanUp(TerminalDevice);
          } else {
	          TempFiFoFlush (TerminalDevice);
		      }
          TerminalDevice->NonVt100AltKey = FALSE;
        } else {
          //
          // Normal code, just insert to EfiKeyFiFo
          //
          ProcessCrSpecialCommand (TerminalDevice, gCrSpecialCommand, UnicodeChar);
          Key.Key.ScanCode    = SCAN_NULL;
          Key.Key.UnicodeChar = UnicodeChar;
          EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
        }
      }else {
        //2 Process ESC sequence code         
        TempFiFoInsertOneKey (TerminalDevice, UnicodeChar);
        
        LookUpStatus = LookUpEscSequenceCodeTable (TerminalDevice, CHECK_ALL_STRINGS, gEscSequenceCode, &TableIndex);
        
        switch (LookUpStatus) {
          case ESC_CODE_INVALID :
            if (IsAlphabet((UINT8)UnicodeChar)) {
		          // Process Alt + key
              Key.Key.ScanCode = SCAN_NULL;
              Key.Key.UnicodeChar = UnicodeChar;
              Key.KeyState.KeyShiftState |= (EFI_LEFT_ALT_PRESSED | EFI_RIGHT_ALT_PRESSED);
              EfiKeyFiFoInsertOneKey (TerminalDevice, Key);
              TempFiFoCleanUp(TerminalDevice);
              TerminalDevice->InputState = INPUT_STATE_DEFAULT;
            } else {
              if (UnicodeChar == ESC) {
                TempFiFoRemoveOneKey(TerminalDevice, &UnicodeChar);
              }
              TempFiFoFlush (TerminalDevice);

              if (UnicodeChar == ESC) {
                TempFiFoInsertOneKey (TerminalDevice, UnicodeChar);
                TerminalDevice->InputState = INPUT_STATE_ESC;
                Status = gBS->SetTimer(TerminalDevice->TwoSecondTimeOut, TimerRelative, ESC_SEQUENCE_KEY_PERIOD);
              }
           }
          break;
            
          case ESC_CODE_FINDED :
            ProcessEscSequenceCode (TerminalDevice, gEscSequenceCode, TableIndex);
            TempFiFoCleanUp(TerminalDevice);
            TerminalDevice->InputState = INPUT_STATE_DEFAULT;
            break;
            
          case ESC_CODE_UNFINISHED :
          default :
            break;
        }    
      }
    }
  }

