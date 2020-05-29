/** @file
  Implementation for synchronous EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL protocol.

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


//
// Body of the ConOut functions
//

/**
  Scroll down screen one line.

  @param  TerminalDevice         Terminal Device.
  @param  MaxColumn              The maximum column of terminal screen 
  @param  MaxRow                  The maximum row of terminal screen

**/
VOID
SyncTerminalScrollScreen (
  IN  TERMINAL_DEV    *TerminalDevice,
  IN  UINTN           MaxColumn,
  IN  UINTN           MaxRow
  ) 
{
  EFI_STATUS    Status;
  UINTN         StrLength;
  UINTN         Value;
  UINTN         Index;

  
  //
  // Move cursor to last row
  //
  Index = 0;
  while (gSetCursorToLastRowStr[Index] != CHAR_NULL)  {
    StrLength = 1;
    Status = TerminalDevice->SerialIo->Write (
                                         TerminalDevice->SerialIo,
                                         &StrLength,
                                         &gSetCursorToLastRowStr[Index]
                                         );
    Index++;
  }

  //
  // Scroll screen
  //
  Index = 0;
  while (gScrollDownStr[Index] != CHAR_NULL)  {
    StrLength = 1;
    Status = TerminalDevice->SerialIo->Write (
                                         TerminalDevice->SerialIo,
                                         &StrLength,
                                         &gScrollDownStr[Index]
                                         );
    Index++;
  }

  //
  // control sequence to move the cursor
  //
  Value = MaxRow;
  gSetCursorPositionString[ROW_OFFSET + 0]    = (CHAR16) ('0' + (Value / 100));
  Value = (Value % 100);
  gSetCursorPositionString[ROW_OFFSET + 1]    = (CHAR16) ('0' + (Value / 10));
  Value = (Value % 10);
  gSetCursorPositionString[ROW_OFFSET + 2]    = (CHAR16) ('0' + Value);

  Value = 0;
  gSetCursorPositionString[COLUMN_OFFSET + 0] = (CHAR16) ('0' + (Value / 100));
  Value = (Value % 100);
  gSetCursorPositionString[COLUMN_OFFSET + 1] = (CHAR16) ('0' + (Value / 10));
  Value = (Value % 10);
  gSetCursorPositionString[COLUMN_OFFSET + 2] = (CHAR16) ('0' + Value);

  //
  // Restore cursor
  //
  Index = 0;
  while (gSetCursorPositionString[Index] != CHAR_NULL)  {
    StrLength = 1;
    Status = TerminalDevice->SerialIo->Write (
                                         TerminalDevice->SerialIo,
                                         &StrLength,
                                         &gSetCursorPositionString[Index]
                                         );
    Index++;
  }
}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.Reset().

  If ExtendeVerification is TRUE, then perform dependent serial device reset,
  and set display mode to mode 0.
  If ExtendedVerification is FALSE, only set display mode to mode 0.

  @param  This                         Indicates the calling context.
  @param  ExtendedVerification  Indicates that the driver may perform a more
                                              exhaustive verification operation of the device
                                              during reset.

  @retval EFI_SUCCESS               The reset operation succeeds.
  @retval EFI_DEVICE_ERROR      The terminal is not functioning correctly or the serial port reset fails.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutReset (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  BOOLEAN                          ExtendedVerification
  )
{
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;
  UINTN         StringLen;

  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  // Reset Terminal, Send out "<ESC>c"
  //
  StringLen = 2;
  Status = TerminalDevice->SerialIo->Write (TerminalDevice->SerialIo, &StringLen, gResetDeviceString);
  //
  // Perform a more exhaustive reset by resetting the serial port.
  //
  if (ExtendedVerification) {
    //
    // Report progress code here
    //
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_PROGRESS_CODE,
      (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_PC_RESET),
      TerminalDevice->DevicePath
      );

    Status = TerminalDevice->SerialIo->Reset (TerminalDevice->SerialIo);
    if (EFI_ERROR (Status)) {
      //
      // Report error code here
      //
      REPORT_STATUS_CODE_WITH_DEVICE_PATH (
        EFI_ERROR_CODE | EFI_ERROR_MINOR,
        (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_EC_CONTROLLER_ERROR),
        TerminalDevice->DevicePath
        );

      return Status;
    }
  }

  This->SetAttribute (This, EFI_TEXT_ATTR (This->Mode->Attribute & 0x0F, EFI_BLACK));

  Status = This->SetMode (This, 0);

  return Status;
}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.OutputString().

  The Unicode string will be converted to terminal expressible data stream
  and send to terminal via serial port.

  @param  This             Indicates the calling context.
  @param  WString        The Null-terminated Unicode string to be displayed
                                  on the terminal screen.

  @retval EFI_SUCCESS                         The string is output successfully.
  @retval EFI_DEVICE_ERROR                The serial port fails to send the string out.
  @retval EFI_WARN_UNKNOWN_GLYPH  Indicates that some of the characters in the Unicode string could not
                                                         be rendered and are skipped.
  @retval EFI_UNSUPPORTED                 If current display mode is out of range.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutOutputString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  CHAR16                           *WString
  )
{
  TERMINAL_DEV                 *TerminalDevice;
  EFI_SIMPLE_TEXT_OUTPUT_MODE  *Mode;
  UINTN                        MaxColumn;
  UINTN                        MaxRow;
  UINTN                        Length;
  UTF8_CHAR                    Utf8Char;
  CHAR8                        GraphicChar;
  CHAR8                        AsciiChar;
  EFI_STATUS                   Status;
  UINT8                        ValidBytes;
  //
  //  flag used to indicate whether condition happens which will cause
  //  return EFI_WARN_UNKNOWN_GLYPH
  //
  BOOLEAN                     Warning;

  ValidBytes  = 0;
  Warning     = FALSE;
  AsciiChar   = 0;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  //  Get current display mode
  //
  Mode = This->Mode;

  if (Mode->Mode >= TerminalDevice->SimpleTextOutputMode.MaxMode) {
    return EFI_UNSUPPORTED;
  }

  This->QueryMode (
          This,
          Mode->Mode,
          &MaxColumn,
          &MaxRow
          );

  for (; *WString != CHAR_NULL; WString++) {

    if (!(Mode->CursorRow >= (INT32) (MaxRow - 1) && (*WString == CHAR_LINEFEED))) {
    
	  switch (TerminalDevice->TerminalType) {

        case PCANSITYPE:
        case VT100TYPE:
        case VT100PLUSTYPE:

          if (!TerminalIsValidTextGraphics (*WString, &GraphicChar, &AsciiChar, NULL)) {
            //
            // If it's not a graphic character convert Unicode to ASCII.
            //
            GraphicChar = (CHAR8) *WString;

            if (!(TerminalIsValidAscii (GraphicChar) || TerminalIsValidEfiCntlChar (GraphicChar))) {
              //
              // when this driver use the OutputString to output control string,
              // TerminalDevice->OutputEscChar is set to let the Esc char
              // to be output to the terminal emulation software.
              //
              if ((GraphicChar == 27) && TerminalDevice->OutputEscChar) {
                GraphicChar = 27;
              } else {
                GraphicChar = '?';
                Warning     = TRUE;
              }
            }

            AsciiChar = GraphicChar;

          }

          if (TerminalDevice->TerminalType != PCANSITYPE) {
            GraphicChar = AsciiChar;
          }

          Length = 1;

          Status = TerminalDevice->SerialIo->Write (
                                               TerminalDevice->SerialIo,
                                               &Length,
                                               &GraphicChar
                                               );
 
          if (EFI_ERROR (Status)) {
            goto OutputError;
          }

        break;

        case VTUTF8TYPE:
          UnicodeToUtf8 (*WString, &Utf8Char, &ValidBytes);
          Length = ValidBytes;
          Status = TerminalDevice->SerialIo->Write (
                                               TerminalDevice->SerialIo,
                                               &Length,
                                               (UINT8 *) &Utf8Char
      	                                       );
      	  if (EFI_ERROR (Status)) {
        	  goto OutputError;
      	  }
        break;
      }
    }
    //
    //  Update cursor position.
    //
    switch (*WString) {

    case CHAR_BACKSPACE:
      if (Mode->CursorColumn > 0) {
        Mode->CursorColumn--;
      }
      break;

    case CHAR_LINEFEED:
      if (Mode->CursorRow < (INT32) (MaxRow - 1)) {
        Mode->CursorRow++;
      } else {
        SyncTerminalScrollScreen (TerminalDevice, MaxColumn, MaxRow);
      }
      break;

    case CHAR_CARRIAGE_RETURN:
      Mode->CursorColumn = 0;
      break;

    default:
      if (Mode->CursorColumn < (INT32) (MaxColumn - 1)) {

        Mode->CursorColumn++;

      } else {

        Mode->CursorColumn = 0;
        if (Mode->CursorRow < (INT32) (MaxRow - 1)) {
          Mode->CursorRow++;
        } else {
          SyncTerminalScrollScreen (TerminalDevice, MaxColumn, MaxRow);
        }

      }
      break;

    };

  }

  if (Warning) {
    return EFI_WARN_UNKNOWN_GLYPH;
  }

  return EFI_SUCCESS;

OutputError:
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_ERROR_CODE | EFI_ERROR_MINOR,
    (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_EC_OUTPUT_ERROR),
    TerminalDevice->DevicePath
    );

  return EFI_DEVICE_ERROR;
}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.TestString().

  If one of the characters in the *Wstring is neither valid Unicode drawing characters,
  not ASCII code, then this function will return EFI_UNSUPPORTED.

  @param  This                       Indicates the calling context.
  @param  WString                  The Null-terminated Unicode string to be tested.

  @retval EFI_SUCCESS            The terminal is capable of rendering the output string.
  @retval EFI_UNSUPPORTED    Some of the characters in the Unicode string cannot be rendered.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutTestString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  CHAR16                           *WString
  )
{
  TERMINAL_DEV  *TerminalDevice;
  EFI_STATUS    Status;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  switch (TerminalDevice->TerminalType) {

  case PCANSITYPE:
  case VT100TYPE:
  case VT100PLUSTYPE:
    Status = AnsiTestString (TerminalDevice, WString);
    break;

  case VTUTF8TYPE:
    Status = VTUTF8TestString (TerminalDevice, WString);
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.QueryMode().

  It returns information for an available text mode
  that the terminal supports.

  @param This              Indicates the calling context.
  @param ModeNumber  The mode number to return information on.
  @param Columns        The returned columns of the requested mode.
  @param Rows            The returned rows of the requested mode.

  @retval EFI_SUCCESS           The requested mode information is returned.
  @retval EFI_UNSUPPORTED   The mode number is not valid.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutQueryMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            ModeNumber,
  OUT UINTN                            *Columns,
  OUT UINTN                            *Rows
  )
{
  TERMINAL_DEV  *TerminalDevice;

  //
  //  Get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);
  if (ModeNumber >= (UINTN) TerminalDevice->SimpleTextOutputMode.MaxMode) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get Terminal device data structure pointer.
  //  
  *Columns = TerminalDevice->TerminalConsoleModeData[ModeNumber].Columns;
  *Rows    = TerminalDevice->TerminalConsoleModeData[ModeNumber].Rows;

  return EFI_SUCCESS;
}


/**
  Implements EFI_SIMPLE_TEXT_OUT.SetMode().

  Set the terminal to a specified display mode. In this driver, we only support mode 0.

  @param This                Indicates the calling context.
  @param ModeNumber    The text mode to set.

  @retval EFI_SUCCESS            The requested text mode is set.
  @retval EFI_DEVICE_ERROR   The requested text mode cannot be set 
                                            because of serial device error.
  @retval EFI_UNSUPPORTED    The text mode number is not valid.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutSetMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            ModeNumber
  )
{
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  if (ModeNumber >= (UINTN) (TerminalDevice->SimpleTextOutputMode.MaxMode)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Set the current mode
  //
  This->Mode->Mode = (INT32) ModeNumber;

  This->ClearScreen (This);

  TerminalDevice->OutputEscChar = TRUE;
  Status                        = This->OutputString (This, gSetModeString);
  TerminalDevice->OutputEscChar = FALSE;

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  This->Mode->Mode  = (INT32) ModeNumber;

  Status            = This->ClearScreen (This);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;

}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.SetAttribute().

  @param This          Indicates the calling context.
  @param Attribute   The attribute to set. Only bit0..6 are valid, all other bits
                              are undefined and must be zero.

  @retval EFI_SUCCESS            The requested attribute is set.
  @retval EFI_DEVICE_ERROR   The requested attribute cannot be set due to serial port error.
  @retval EFI_UNSUPPORTED    The attribute requested is not defined by EFI spec.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutSetAttribute (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            Attribute
  )
{
  UINT8         ForegroundControl;
  UINT8         BackgroundControl;
  UINT8         BrightControl;
  INT32         SavedColumn;
  INT32         SavedRow;
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;

  SavedColumn = 0;
  SavedRow    = 0;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  //  only the bit0..6 of the Attribute is valid
  //
  if ((Attribute | 0x7f) != 0x7f) {
    return EFI_UNSUPPORTED;
  }

  //
  // Skip outputting the command string for the same attribute
  // It improves the terminal performance significantly
  //
  if (This->Mode->Attribute == (INT32) Attribute) {
    return EFI_SUCCESS;
  }

  //
  //  convert Attribute value to terminal emulator
  //  understandable foreground color
  //
  switch (Attribute & 0x07) {

  case EFI_BLACK:
    ForegroundControl = 30;
    break;

  case EFI_BLUE:
    ForegroundControl = 34;
    break;

  case EFI_GREEN:
    ForegroundControl = 32;
    break;

  case EFI_CYAN:
    ForegroundControl = 36;
    break;

  case EFI_RED:
    ForegroundControl = 31;
    break;

  case EFI_MAGENTA:
    ForegroundControl = 35;
    break;

  case EFI_BROWN:
    ForegroundControl = 33;
    break;

  default:

  case EFI_LIGHTGRAY:
    ForegroundControl = 37;
    break;

  }
  //
  //  bit4 of the Attribute indicates bright control
  //  of terminal emulator.
  //
  BrightControl = (UINT8) ((Attribute >> 3) & 1);

  //
  //  convert Attribute value to terminal emulator
  //  understandable background color.
  //
  switch ((Attribute >> 4) & 0x07) {

  case EFI_BLACK:
    BackgroundControl = 40;
    break;

  case EFI_BLUE:
    BackgroundControl = 44;
    break;

  case EFI_GREEN:
    BackgroundControl = 42;
    break;

  case EFI_CYAN:
    BackgroundControl = 46;
    break;

  case EFI_RED:
    BackgroundControl = 41;
    break;

  case EFI_MAGENTA:
    BackgroundControl = 45;
    break;

  case EFI_BROWN:
    BackgroundControl = 43;
    break;

  default:

  case EFI_LIGHTGRAY:
    BackgroundControl = 47;
    break;
  }
  if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_VIDEO_FLAG) == CR_TERMINAL_VIDEO_MONO) {
    BackgroundControl = 40;
    ForegroundControl = 37;
  }
  //
  // terminal emulator's control sequence to set attributes
  //
  gSetAttributeString[BRIGHT_CONTROL_OFFSET]          = (CHAR16) ('0' + BrightControl);
  gSetAttributeString[FOREGROUND_CONTROL_OFFSET + 0]  = (CHAR16) ('0' + (ForegroundControl / 10));
  gSetAttributeString[FOREGROUND_CONTROL_OFFSET + 1]  = (CHAR16) ('0' + (ForegroundControl % 10));
  gSetAttributeString[BACKGROUND_CONTROL_OFFSET + 0]  = (CHAR16) ('0' + (BackgroundControl / 10));
  gSetAttributeString[BACKGROUND_CONTROL_OFFSET + 1]  = (CHAR16) ('0' + (BackgroundControl % 10));

  //
  // save current column and row
  // for future scrolling back use.
  //
  SavedColumn                   = This->Mode->CursorColumn;
  SavedRow                      = This->Mode->CursorRow;

  TerminalDevice->OutputEscChar = TRUE;
  Status                        = This->OutputString (This, gSetAttributeString);
  TerminalDevice->OutputEscChar = FALSE;

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  //  scroll back to saved cursor position.
  //
  This->Mode->CursorColumn  = SavedColumn;
  This->Mode->CursorRow     = SavedRow;
  This->Mode->Attribute     = (INT32) Attribute;

  return EFI_SUCCESS;

}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.ClearScreen().
  It clears the ANSI terminal's display to the currently selected background color.

  @param This     Indicates the calling context.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_DEVICE_ERROR  The terminal screen cannot be cleared due to serial port error.
  @retval EFI_UNSUPPORTED   The terminal is not in a valid display mode.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutClearScreen (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This
  )
{
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;

  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  //  control sequence for clear screen request
  //
  TerminalDevice->OutputEscChar = TRUE;
  Status                        = This->OutputString (This, gClearScreenString);
  TerminalDevice->OutputEscChar = FALSE;

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Status = This->SetCursorPosition (This, 0, 0);

  return Status;
}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.SetCursorPosition().

  @param This         Indicates the calling context.
  @param Column    The row to set cursor to.
  @param Row         The column to set cursor to.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_DEVICE_ERROR  The request fails due to serial port error.
  @retval EFI_UNSUPPORTED   The terminal is not in a valid text mode, or the cursor position
                                           is invalid for current mode.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutSetCursorPosition (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            Column,
  IN  UINTN                            Row
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE     *Mode;
  UINTN                           MaxColumn;
  UINTN                           MaxRow;
  EFI_STATUS                      Status;
  TERMINAL_DEV                    *TerminalDevice;
  UINTN                           Value;

  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  //  get current mode
  //
  Mode = This->Mode;

  //
  //  get geometry of current mode
  //
  Status = This->QueryMode (
                   This,
                   Mode->Mode,
                   &MaxColumn,
                   &MaxRow
                   );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (Column >= MaxColumn || Row >= MaxRow) {
    return EFI_UNSUPPORTED;
  }
  //
  // control sequence to move the cursor
  //
  Value = Row + 1;
  gSetCursorPositionString[ROW_OFFSET + 0]    = (CHAR16) ('0' + (Value / 100));
  Value = (Value % 100);
  gSetCursorPositionString[ROW_OFFSET + 1]    = (CHAR16) ('0' + (Value / 10));
  Value = (Value % 10);
  gSetCursorPositionString[ROW_OFFSET + 2]    = (CHAR16) ('0' + Value);

  Value = Column + 1;
  gSetCursorPositionString[COLUMN_OFFSET + 0] = (CHAR16) ('0' + (Value / 100));
  Value = (Value % 100);
  gSetCursorPositionString[COLUMN_OFFSET + 1] = (CHAR16) ('0' + (Value / 10));
  Value = (Value % 10);
  gSetCursorPositionString[COLUMN_OFFSET + 2] = (CHAR16) ('0' + Value);

  TerminalDevice->OutputEscChar = TRUE;
  Status = This->OutputString (This, gSetCursorPositionString);
  TerminalDevice->OutputEscChar = FALSE;

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  //  update current cursor position
  //  in the Mode data structure.
  //
  Mode->CursorColumn  = (INT32) Column;
  Mode->CursorRow     = (INT32) Row;

  return EFI_SUCCESS;
}


/**
  Implements SIMPLE_TEXT_OUTPUT.EnableCursor().

  In this driver, the cursor cannot be hidden.

  @param This      Indicates the calling context.
  @param Visible   If TRUE, the cursor is set to be visible,
                          If FALSE, the cursor is set to be invisible.

  @retval EFI_SUCCESS          The request is valid.
  @retval EFI_UNSUPPORTED  The terminal does not support cursor hidden.

**/
EFI_STATUS
EFIAPI
SyncTerminalConOutEnableCursor (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  BOOLEAN                          Visible
  )
{
  if (!Visible) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
