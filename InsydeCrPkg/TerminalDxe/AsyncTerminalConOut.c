/** @file
  Implementation for asynchronous EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL protocol.
    
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Terminal.h"

								 
UINTN                                       mTerminalAmount = 0;
LIST_ENTRY                                  mTermEventLists;
LIST_ENTRY                                  *mCurrentTermEvent = NULL;
BOOLEAN                                     mEscProcess = FALSE;

/**
  Output ESC sequence string without RefreshScreen event interrupt.

  @param This             Pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
  @param EscString      Pointer to output EscString buffer 

  @retval EFI_SUCCESS                           The string is output successfully. 
  @retval EFI_DEVICE_ERROR                  The serial port fails to send the string out. 
  @retval EFI_WARN_UNKNOWN_GLYPH    Indicates that some of the characters in the Unicode
                                                           string could not be rendered and are skipped.
**/
EFI_STATUS
OutputEscSquenceString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  CHAR16                           *EscString
  )
{
  TERMINAL_DEV  *TerminalDevice;
  EFI_STATUS    Status;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);
  
  //
  // Avoid send ESC command be interrupt
  //
  mEscProcess = TRUE;

  TerminalDevice->OutputEscChar = TRUE;
  Status                        = PrivateOutputString (This, EscString);
  TerminalDevice->OutputEscChar = FALSE;

  mEscProcess = FALSE;

  return Status;
}


/**
  Implements EFI_SIMPLE_TEXT_OUT_PROTOCOL.OutputString().
  The Unicode string will be converted to terminal expressible data stream
  and save into screen buffer.
  
  @param This         Indicates the calling context.
  @param WString    The Null-terminated Unicode string to be displayed on 
                             the terminal screen.
        
  @retval  EFI_SUCCESS                         The string is output successfully.   
  @retval  EFI_DEVICE_ERROR                The serial port fails to send the string out.
  @retval  EFI_WARN_UNKNOWN_GLYPH  Indicates that some of the characters in the Unicode string could not 
                                                            be rendered and are skipped.                    
**/
EFI_STATUS
EFIAPI
PrivateOutputString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  CHAR16                           *WString
  )
{
  TERMINAL_DEV                *TerminalDevice;
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;
  UINTN                       MaxColumn;
  UINTN                       MaxRow;
  UINTN                       Length;
  UTF8_CHAR                   Utf8Char;
  CHAR8                       GraphicChar;
  CHAR8                       AsciiChar;
  EFI_STATUS                  Status;
  UINT8                       ValidBytes;
  CHAR8                       LineDrawChar;

  //
  //  flag used to indicate whether condition happens which will cause
  //  return EFI_WARN_UNKNOWN_GLYPH
  //
  BOOLEAN                     Warning;

  ValidBytes   = 0;
  LineDrawChar = 0;
  Warning      = FALSE;
  
  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  //  Get current display mode
  //
  Mode = This->Mode;
  
  This->QueryMode (
          This,
          Mode->Mode,
          &MaxColumn,
          &MaxRow
          );

  for (; *WString != CHAR_NULL; WString++) {

    switch (TerminalDevice->TerminalType) {

    case PCANSITYPE:
    case VT100TYPE:
    case VT100PLUSTYPE:

      if (!TerminalIsValidTextGraphics (*WString, &GraphicChar, &AsciiChar, &LineDrawChar)) {
        //
        // If it's not a graphic character convert Unicode to ASCII.
        //
        GraphicChar = (CHAR8)*WString;

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

      //
      // VT100 / VT100+
      //
      if (TerminalDevice->TerminalType != PCANSITYPE &&
          TerminalDevice->OutputEscChar == FALSE) {
          
        if ((LineDrawChar != 0x00) && 
            ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_CHARSET_FLAG) == CR_TERMINAL_CHARSET_GRAPHIC)) {
          //
          // Use Line Draw Character Set
          //
          Status = Vt100SwitchCharSet (&TerminalDevice->SimpleTextOutput, VT100_GRAPH_CHAR_SET);          
          GraphicChar = LineDrawChar;
        } else {
          //
          // Use AscII Character Set
          //
          Status = Vt100SwitchCharSet (&TerminalDevice->SimpleTextOutput, VT100_ASCII_CHAR_SET);        
          GraphicChar = AsciiChar;
        }

        if (EFI_ERROR (Status)) {
          goto OutputError;
        }
        
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
      //
      // Update Terminal Current Cursor Position
      //
      TerminalDevice->TerCol = (TerminalDevice->TerCol == MaxColumn - 1)? 0 : (TerminalDevice->TerCol + 1);
      
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
      //
      // Update Terminal Current Cursor Position
      //
      TerminalDevice->TerCol = (TerminalDevice->TerCol == MaxColumn - 1)? 0 : (TerminalDevice->TerCol + 1);
      
      break;
    }

  }

  if (Warning) {
    return EFI_WARN_UNKNOWN_GLYPH;
  }

  return EFI_SUCCESS;

OutputError:
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_ERROR_CODE | EFI_ERROR_MINOR,
    (EFI_PERIPHERAL_REMOTE_CONSOLE | EFI_P_EC_CONTROLLER_ERROR),
    TerminalDevice->DevicePath
    );

  return EFI_DEVICE_ERROR;
}


/**
  Implements EFI_SIMPLE_TEXT_OUT_PROTOCOL.SetAttribute().

  @param This             Pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
  @param Attribute      The attribute to set. Only bit0..6 are valid, all other bits
                                 are undefined and must be zero

  @retval EFI_SUCCESS            The requested attribute is set. 
  @retval EFI_DEVICE_ERROR   The requested attribute cannot be set due to serial port error. 
  @retval EFI_UNSUPPORTED    The attribute requested is not defined by EFI spec.
**/
EFI_STATUS
EFIAPI
PrivateSetAttribute (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            Attribute
  )
{
  UINT8         ForegroundControl;
  UINT8         BackgroundControl;
  UINT8         BrightControl;
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  // Speed up: 
  //   If current attribute is the same, dont't need to send command
  //
  if (TerminalDevice->TermCurAttr == Attribute) {
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

  case EFI_LIGHTGRAY:  
    ForegroundControl = 37;
    break;

  default:
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
    
  case EFI_LIGHTGRAY:
    BackgroundControl = 47;
    break;

  default:
    BackgroundControl = 47;
    break;
  }

    if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_VIDEO_FLAG) == CR_TERMINAL_VIDEO_MONO) {
      if (BackgroundControl == 40) {
        //Background = BLACK   ->  Background = WHITE   Foreground = BLACK
        BackgroundControl = 47;
        ForegroundControl = 30;
      }
      else {
        BackgroundControl = 40;
        ForegroundControl = 37;
      }    
    }
  
  //
  // terminal emulator's control sequence to set attributes
  //
  gSetAttributeString[BRIGHT_CONTROL_OFFSET]          = (CHAR16) ('0' + BrightControl);
  gSetAttributeString[FOREGROUND_CONTROL_OFFSET + 0]  = (CHAR16) ('0' + (ForegroundControl / 10));
  gSetAttributeString[FOREGROUND_CONTROL_OFFSET + 1]  = (CHAR16) ('0' + (ForegroundControl % 10));
  gSetAttributeString[BACKGROUND_CONTROL_OFFSET + 0]  = (CHAR16) ('0' + (BackgroundControl / 10));
  gSetAttributeString[BACKGROUND_CONTROL_OFFSET + 1]  = (CHAR16) ('0' + (BackgroundControl % 10));

  Status = OutputEscSquenceString (This, gSetAttributeString);

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  TerminalDevice->TermCurAttr = Attribute;

  return EFI_SUCCESS;

}


/**
  Send changing cursor ESC squence code to terminal.

  @param This             Pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
  @param Column        The column to set cursor 
  @param Row             The row to set cursor.

  @retval EFI_SUCCESS            The requested cursor position is set. 
  @retval EFI_DEVICE_ERROR   The requested cursor position cannot be set due to serial port error. 
  @retval EFI_UNSUPPORTED    The cursor position requested is not defined by EFI spec.
**/
EFI_STATUS
EFIAPI
PrivateSetCursorPosition (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            Column,
  IN  UINTN                            Row
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE   *Mode;
  UINTN                         MaxColumn;
  UINTN                         MaxRow;
  EFI_STATUS                    Status;
  TERMINAL_DEV                  *TerminalDevice;
  UINTN                         Value;

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
  // Speed up: 
  //   If current cursor is the same, dont't need to send command
  //
  if (TerminalDevice->TerRow == Row && \
      TerminalDevice->TerCol == Column) {
    return EFI_SUCCESS;
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

  Status = OutputEscSquenceString (This, gSetCursorPositionString);

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  
  TerminalDevice->TerRow = Row;
  TerminalDevice->TerCol = Column;

  return EFI_SUCCESS;
}


/**
  Switch terminal character set.

  @param This             Pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
  @param CharSet        The expected character set to switch

  @retval EFI_SUCCESS            The requested character set is update. 
  @retval EFI_DEVICE_ERROR   The requested character set cannot be set due to serial port error. 
  @retval EFI_UNSUPPORTED    The character set requested is not defined by EFI spec.

**/
EFI_STATUS
Vt100SwitchCharSet (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL      *This,
  IN  VT100_CHARACTER_SET                  CharSet
  )
{
  EFI_STATUS                        Status;
  TERMINAL_DEV                      *TerminalDevice;

  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  // Normal ASCii table
  //
  Status = EFI_SUCCESS;
  if (CharSet == VT100_ASCII_CHAR_SET) {
    if (TerminalDevice->Vt100CurrentCharSet != VT100_ASCII_CHAR_SET) {
      TerminalDevice->Vt100CurrentCharSet = VT100_ASCII_CHAR_SET;
      gSetCharSetString[2] = L'B';
      Status = OutputEscSquenceString (
                 This, 
                 gSetCharSetString
                 );
    }
  }
  
  //
  // Special drawing character set
  //  
  else if (CharSet == VT100_GRAPH_CHAR_SET) {
    if (TerminalDevice->Vt100CurrentCharSet != VT100_GRAPH_CHAR_SET) {
      TerminalDevice->Vt100CurrentCharSet = VT100_GRAPH_CHAR_SET;
      gSetCharSetString[2] = L'0';
      Status = OutputEscSquenceString (
                 This, 
                 gSetCharSetString
                 );
    }
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}


/**
  Scroll down screen buffer one line.

  @param TerminalDevice             Terminal device

**/
VOID
ScrollScreenBuffer (
  IN  TERMINAL_DEV                  *TerminalDevice
  )    
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;  
  CHAR16                      *ScrChar;
  CHAR16                      *TerChar;  
  UINT8                       *ScrAttribute;
  UINT8                       *TerAttribute;  
  UINTN                       Column;
  UINTN                       MaxColumn, MaxRow;

  //
  //  Get current display mode and variable in private data
  //
  Mode = TerminalDevice->SimpleTextOutput.Mode;

  TerminalDevice->SimpleTextOutput.QueryMode (
                    &(TerminalDevice->SimpleTextOutput),
                    Mode->Mode,
                    &MaxColumn,
                    &MaxRow
                    );
  ScrChar = TerminalDevice->PtrScrChar;
  ScrAttribute = TerminalDevice->PtrScrAttr;
  TerChar = TerminalDevice->PtrTerChar;
  TerAttribute = TerminalDevice->PtrTerAttr;

  //
  // Scroll Character and Character Buffer together
  //
  CopyMem (
    ScrChar,                                       
    &(ScrChar[1 * MaxColumn]),                     
    (MaxRow - 1) * MaxColumn * sizeof (CHAR16)     
    );
  CopyMem (
    TerChar,                                             
    &(TerChar[1 * MaxColumn]),                           
    (MaxRow - 1) * MaxColumn * sizeof (CHAR16)          
    );

  
  //
  // Scroll Attribute and Attribute Buffer togeter
  //    
  CopyMem (
    ScrAttribute,                                    
    &(ScrAttribute[1 * MaxColumn]),                  
    (MaxRow - 1) * MaxColumn * sizeof (UINT8)          
    );
  CopyMem (
    TerAttribute,                                          
    &(TerAttribute[1 * MaxColumn]),                        
    (MaxRow - 1) * MaxColumn * sizeof (UINT8)          
    );

  //
  // Clear last new line
  //
  for (Column =0; Column < MaxColumn; Column ++) {
    CopyMem (&ScrChar[(MaxRow - 1) * MaxColumn + Column], L" ", 2);
    CopyMem (&TerChar[(MaxRow - 1) * MaxColumn + Column], L" ", 2);    
  }
  CopyMem (
    &ScrAttribute[(MaxRow - 1) * MaxColumn], 
    &ScrAttribute[(MaxRow - 2) * MaxColumn],
    (MaxColumn * sizeof (UINT8))
    );
  CopyMem (
    &TerAttribute[(MaxRow - 1) * MaxColumn],
    &ScrAttribute[(MaxRow - 2) * MaxColumn],
    (MaxColumn * sizeof (UINT8))
    );

}


/**
  Scroll down screen buffer one line.

  @param TerminalDevice             Terminal device

**/
VOID
ScrollScreen (
  IN  TERMINAL_DEV                  *TerminalDevice
  )    
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;  
  UINTN                       MaxRow; 
  UINTN                       MaxColumn;
  UINT8                       *ScrAttribute;
  UINT8                       NewLinAttribute;

  //
  //  Get current display mode and variable in private data
  //
  Mode = TerminalDevice->SimpleTextOutput.Mode;

  TerminalDevice->SimpleTextOutput.QueryMode (
                                     &(TerminalDevice->SimpleTextOutput),
                                     Mode->Mode,
                                     &MaxColumn,
                                     &MaxRow
                                     );

  //
  // Make new line attribute
  //
  ScrAttribute = TerminalDevice->PtrScrAttr;
  NewLinAttribute = ScrAttribute[(MaxRow - 1) * MaxColumn + (MaxColumn - 1)];

  //
  // Send change line command to terminal and scroll buffer
  //
  OutputEscSquenceString (&(TerminalDevice->SimpleTextOutput), gSetCursorToLastRowStr);
  OutputEscSquenceString (&(TerminalDevice->SimpleTextOutput), gScrollDownStr);
  PrivateSetCursorPosition (&(TerminalDevice->SimpleTextOutput), 0, (MaxRow - 1));
  PrivateSetAttribute (&(TerminalDevice->SimpleTextOutput), NewLinAttribute);
  ScrollScreenBuffer (TerminalDevice);
  return;
}


/**
  
  Update screen character and attribute buffer.
  
  @param TerminalDevice    Terminal device
  @param Column               Coulumn to updated
  @param Row                    Row to updated

**/
VOID
UpdateScreenDataAttriBuffer (
  IN  TERMINAL_DEV                  *TerminalDevice,
  IN  UINTN                         Column,
  IN  UINTN                         Row
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;  
  CHAR16                      *ScrChar;
  CHAR16                      *TerChar;
  UINT8                       *ScrAttribute;
  UINT8                       *TerAttribute;  
  UINTN                       MaxColumn;
  UINTN                       MaxRow;
  
  //
  //  Get current display mode
  //
  Mode = TerminalDevice->SimpleTextOutput.Mode;

  TerminalDevice->SimpleTextOutput.QueryMode (
          &(TerminalDevice->SimpleTextOutput),
          Mode->Mode,
          &MaxColumn,
          &MaxRow
          );

  TerChar = TerminalDevice->PtrTerChar;
  ScrChar = TerminalDevice->PtrScrChar;
  ScrAttribute = TerminalDevice->PtrScrAttr;
  TerAttribute = TerminalDevice->PtrTerAttr;  

  //
  // Update screen character and attribute buffer
  //
  TerChar[Row * MaxColumn + Column] = ScrChar[Row * MaxColumn + Column];
  TerAttribute[Row * MaxColumn + Column] = ScrAttribute[Row * MaxColumn + Column];

}


/**
  
  Refreash choosed row to update terminal.
  This function will calculate witch position need to update on terminal.
  
  @param TerminalDevice    Terminal device
  @param Row                    Row to updated

  @retval EFI_SUCCESS            Success for update this ro. 
  @retval EFI_DEVICE_ERROR   No anything need updated. 
  @retval EFI_UNSUPPORTED    Skip first/last row.
    
**/
EFI_STATUS
RefreshScreenRow (
  IN  TERMINAL_DEV                  *TerminalDevice,
  IN  UINTN                         Row,  
  IN EFI_EVENT                      ConOutTimeOutEvent
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;  
  CHAR16                      *ScrChar;
  CHAR16                      *TerChar;
  UINT8                       *ScrAttribute;
  UINT8                       *TerAttribute;    
  UINTN                       ColumnIndex;
  UINTN                       MaxColumn, MaxRow;
  CHAR16                      Outchar[2];
  UINTN                       ColUpdateTail; 
  UINTN                       ColUpdateHead;
  BOOLEAN                     SkipFirstRow;
  UINTN                       TerminalRow;
  BOOLEAN                     UpdateIsSpace;

  //
  //  Get current display mode and variable in private data
  //
  Mode = TerminalDevice->SimpleTextOutput.Mode;

  TerminalDevice->SimpleTextOutput.QueryMode (
          &(TerminalDevice->SimpleTextOutput),
          Mode->Mode,
          &MaxColumn,
          &MaxRow
          );

  if (Row >= MaxRow) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check 24 Row Policy
  //
  SkipFirstRow = FALSE;
  if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_ROW_FLAG) == CR_TERMINAL_ROW_24 && \
      (MaxColumn == 80) && (MaxRow == 25)) {
    if ((TerminalDevice->TerminalFeatureFlag & CR_24ROWS_POLICY_FLAG) == CR_24ROWS_POLICY_DEL_FIRST) {
      SkipFirstRow = TRUE;
      if (Row == 0) {
        //
        // Skip First Row
        //
        return EFI_ABORTED;
      }
    } else {
      if (Row == 24) {
        //
        // Skip Last Row
        //
        return EFI_ABORTED;
      }
    }
  }

  //
  // Calculate needed changed character from head
  //
  ScrAttribute = TerminalDevice->PtrScrAttr + Row * MaxColumn;
  TerAttribute = TerminalDevice->PtrTerAttr + Row * MaxColumn;
  ScrChar = TerminalDevice->PtrScrChar + Row * MaxColumn;
  TerChar = TerminalDevice->PtrTerChar + Row * MaxColumn;

  ColUpdateHead = 0;

  for (ColumnIndex = 0; ColumnIndex < MaxColumn; ColumnIndex ++) {
    if (*TerAttribute != *ScrAttribute || \
        *TerChar != *ScrChar) {
        ColUpdateHead = ColumnIndex;
        break;
    }
    ScrAttribute ++;
    TerAttribute ++;
    ScrChar ++;
    TerChar ++;
  }
  //
  // Check all the same?
  //
  if (ColumnIndex >= MaxColumn) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Calculate needed changed character from tail
  //
  ScrAttribute = TerminalDevice->PtrScrAttr + Row * MaxColumn + (MaxColumn - 1);
  TerAttribute = TerminalDevice->PtrTerAttr + Row * MaxColumn + (MaxColumn - 1);
  ScrChar = TerminalDevice->PtrScrChar + Row * MaxColumn + (MaxColumn - 1);
  TerChar = TerminalDevice->PtrTerChar + Row * MaxColumn + (MaxColumn - 1);

  ColUpdateTail = MaxColumn;

  for (ColumnIndex = MaxColumn; ColumnIndex >= 0; ColumnIndex --) {
    if (*TerAttribute != *ScrAttribute || \
        *TerChar  != *ScrChar) {
        ColUpdateTail = ColumnIndex;
        break;
    }
    ScrAttribute --;
    TerAttribute --;
    ScrChar --;
    TerChar --;
  }

  //
  // If need skip first row, offset row 2-24 up
  //
  if (SkipFirstRow) {
    TerminalRow = Row - 1;
  } else {
    TerminalRow = Row;
  }

  //
  // Check if all space word in updated region
  //
  ScrChar = TerminalDevice->PtrScrChar + Row * MaxColumn;
  UpdateIsSpace = TRUE;
  for (ColumnIndex = 0; ColumnIndex < MaxColumn; ColumnIndex ++) {      
    if (*ScrChar != 0x20) {
      UpdateIsSpace = FALSE;
      break;
    }
    ScrChar ++;
  }

  //
  // Move cursor to current row
  //
  PrivateSetCursorPosition (&(TerminalDevice->SimpleTextOutput), ColUpdateHead, TerminalRow);

  ScrAttribute = TerminalDevice->PtrScrAttr + Row * MaxColumn;
  TerAttribute = TerminalDevice->PtrTerAttr + Row * MaxColumn;
  ScrChar = TerminalDevice->PtrScrChar + Row * MaxColumn;
  TerChar = TerminalDevice->PtrTerChar + Row * MaxColumn;  
  
  ScrAttribute += ColUpdateHead;
  TerAttribute += ColUpdateHead;
  ScrChar += ColUpdateHead;
  TerChar += ColUpdateHead;
  
  if (UpdateIsSpace) {
    //
    // Speed up if only need update space line, use ESC command
    //    
    PrivateSetAttribute (&(TerminalDevice->SimpleTextOutput), (UINTN)*ScrAttribute);
    OutputEscSquenceString (&(TerminalDevice->SimpleTextOutput), gEraseLine);
  }
  
  ZeroMem (Outchar, 2 * sizeof(CHAR16));
  for (ColumnIndex = ColUpdateHead; ColumnIndex < ColUpdateTail; ColumnIndex ++) {      

    if (!UpdateIsSpace) {
      
      PrivateSetAttribute (&(TerminalDevice->SimpleTextOutput), (UINTN)*ScrAttribute);
             
      //
      // Send character      
      //
      Outchar[0] = *ScrChar;
      PrivateOutputString (&(TerminalDevice->SimpleTextOutput), Outchar);       
    }
    
    UpdateScreenDataAttriBuffer (TerminalDevice, ColumnIndex, Row); 

    if (*ScrAttribute & EFI_WIDE_ATTRIBUTE) {
      ScrAttribute += 2;
      TerAttribute += 2;
      ScrChar += 2;
      TerChar += 2;    
    } else {
      ScrAttribute += 1;
      TerAttribute += 1;
      ScrChar += 1;
      TerChar += 1;    
    }
  }

  return EFI_SUCCESS;
}


/**
  
  Refreash Screen on Terminal Event Handler.
  This routine will check terminal screen buffer to determine witch posiction
  need to update character to terminal.

  @param Event      Event
  @param Context   Event Context Pointer

**/
VOID
RefreshScreenRoutine (
  IN  EFI_EVENT          Event,
  IN  VOID               *Context
  )
{
  TERMINAL_DEV                *TerminalDevice;
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;  
  UINTN                       Row;
  UINTN                       MaxColumn, MaxRow;
  UINT32                      SerialControl;
  CHAR16                      *ScrChar;
  CHAR16                      *TerChar;
  UINT8                       *ScrAttribute;
  UINT8                       *TerAttribute;  
  UINTN                       LastRow;
#ifdef EFI_DEBUG
  UINT32                      OutputAttribute;
#endif
  UINTN                       FirstRow;
  EFI_STATUS                  RefreshStatus;
  EFI_STATUS                  Status;
  EFI_EVENT                   ConOutTimeOutEvent;
  EFI_STATUS                  TimerStatus;
  BOOLEAN                     NoAnyUpdate;
#ifndef EFI_DEBUG  
  UINTN                       TermCursorRow;
#endif

  TerminalDevice = (TERMINAL_DEV *)Context;
  ConOutTimeOutEvent = NULL;
  //
  // Avoid send ESC command be interrupt
  //
  if (mEscProcess) {
    goto FinishRoutine;
  }

  //
  // Check serial mode is not in loop back mode
  //
  TerminalDevice->SerialIo->GetControl (TerminalDevice->SerialIo, &SerialControl);
  if (SerialControl & (EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE | EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE)) {
    return;
  }

  //
  // Start Timeout Timer
  //
  Status = gBS->CreateEvent (EVT_TIMER, TPL_CALLBACK, NULL, NULL, &ConOutTimeOutEvent);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->SetTimer(ConOutTimeOutEvent, TimerRelative, REFRESH_ROUTING_TIMEOUT);
  
  TerChar = TerminalDevice->PtrTerChar;
  ScrChar = TerminalDevice->PtrScrChar;
  ScrAttribute = TerminalDevice->PtrScrAttr;
  TerAttribute = TerminalDevice->PtrTerAttr;  
  LastRow = TerminalDevice->LastRow;

  //
  //  Get current display mode and buffer point
  //
  Mode = TerminalDevice->SimpleTextOutput.Mode;

  TerminalDevice->SimpleTextOutput.QueryMode (
                                     &(TerminalDevice->SimpleTextOutput),
                                     Mode->Mode,
                                     &MaxColumn,
                                     &MaxRow
                                     );


  //
  // Check 24 Row Policy
  //
  FirstRow = 0;
  if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_ROW_FLAG) == CR_TERMINAL_ROW_24) {
    if ((TerminalDevice->TerminalFeatureFlag & CR_24ROWS_POLICY_FLAG) == CR_24ROWS_POLICY_DEL_FIRST) {
      FirstRow = 1;
    } else {
      FirstRow = 0;
      MaxRow = MaxRow - 1;
    }
  }

  NoAnyUpdate = TRUE;
  RefreshStatus = EFI_SUCCESS;
  for (Row =FirstRow; Row < MaxRow; Row ++) {
    RefreshStatus = RefreshScreenRow (TerminalDevice, LastRow, ConOutTimeOutEvent);

    //
    // Check if anything need update?
    //
    if (RefreshStatus == EFI_SUCCESS) {
      NoAnyUpdate = FALSE;
    }

    //
    // Check is it Timeout?
    //
    TimerStatus = gBS->CheckEvent (ConOutTimeOutEvent);
    if (!EFI_ERROR (TimerStatus)) {
      break;
    }

    LastRow ++;
    if (LastRow >= MaxRow) {
      LastRow = 0;
    }    
  }
  TerminalDevice->LastRow = LastRow;

  //
  // If there is debug message, always move cursor to left-top to avoid screen disturbance
  //
#ifdef EFI_DEBUG
  //
  // Restore default attribute (Foreground: White Background: Black)
  //
  OutputAttribute = 0;
  OutputAttribute = (EFI_BLACK << 4) | EFI_LIGHTGRAY;
  PrivateSetAttribute (&(TerminalDevice->SimpleTextOutput), OutputAttribute);
 
  //
  // Restore cursor to left top
  //
  PrivateSetCursorPosition (&(TerminalDevice->SimpleTextOutput), 0, 0);
#else
  //
  // If in 24 row without first line, adjust row number of cursor
  //
  TermCursorRow = Mode->CursorRow;
  if ((TerminalDevice->TerminalFeatureFlag & CR_TERMINAL_ROW_FLAG) == CR_TERMINAL_ROW_24 && \
      (MaxColumn == 80) && (MaxRow == 25)) {
    if ((TerminalDevice->TerminalFeatureFlag & CR_24ROWS_POLICY_FLAG) == CR_24ROWS_POLICY_DEL_FIRST) {
      TermCursorRow = Mode->CursorRow - 1;
    }
  }
  
  //
  // Move cursor to current position if nothong need update
  //
  if (NoAnyUpdate) {
    PrivateSetCursorPosition (
      &(TerminalDevice->SimpleTextOutput), 
      Mode->CursorColumn,     // Column, 
      TermCursorRow           // Row
      );    
  }
#endif

FinishRoutine:

  TriggerNextTermEvent (Context);

  gBS->CloseEvent (ConOutTimeOutEvent);

  return;
}


/**
  
  Free pool buffer safely.

  @param Buffer      Point to the specific buffer that want to free.

**/
VOID
SafeFreePool (
  IN  VOID             *Buffer
  )
{
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
  }
}


/**
  
  Initialize used screen buffer.

  @param  TerminalDevice  Terminal device

**/
VOID
InitializeScreenBuffer (
  IN TERMINAL_DEV   *TerminalDevice
  )
{
  UINTN                 MaxResolution, Resolution;
  UINTN                 Index;
  UINTN                 MaxMode;

  //
  // Find the maximum supported screen resolution
  //
  MaxResolution = TerminalDevice->TerminalConsoleModeData[0].Columns * TerminalDevice->TerminalConsoleModeData[0].Rows;  
  MaxMode = TerminalDevice->SimpleTextOutputMode.MaxMode;
  for (Index = 1; Index < MaxMode; Index ++) {
    Resolution = TerminalDevice->TerminalConsoleModeData[Index].Columns * TerminalDevice->TerminalConsoleModeData[Index].Rows; 
    if (MaxResolution < Resolution) {
      MaxResolution = Resolution;
    }
  }

  //
  // Allocate Buffers
  //
  TerminalDevice->PtrScrChar = AllocateZeroPool (MaxResolution * sizeof (CHAR16));
  TerminalDevice->PtrTerChar = AllocateZeroPool (MaxResolution * sizeof (CHAR16));
  TerminalDevice->PtrScrAttr = AllocateZeroPool (MaxResolution * sizeof (UINT8));
  TerminalDevice->PtrTerAttr = AllocateZeroPool (MaxResolution * sizeof (UINT8));

}


/**
  
  Remove Menu option list.
  
  @param  RefreshEvent  The specific event to removed from refresh even lsit.

**/
VOID
RemoveRefreshEventList (
  IN EFI_EVENT    RefreshEvent
  )
{
  TERMINAL_REFRESH_EVENT_LIST       *TermEventList;
  LIST_ENTRY                        *Link;

  for (Link = mTermEventLists.ForwardLink; Link != &mTermEventLists; Link = Link->ForwardLink) {
    TermEventList = CR (Link, TERMINAL_REFRESH_EVENT_LIST, Link, TERM_REFRESH_EVENT_LIST_SIGNATURE);
    if (TermEventList->RefreshEvent == RefreshEvent) {
      RemoveEntryList (&TermEventList->Link);
      gBS->FreePool (TermEventList);
      break;
    }
  }
}


/**
  
  The function to close refresh screen event.
  
  @param TerminalDevice    Terminal device

**/
VOID
CloseRefreshScreenRoutine (
  IN TERMINAL_DEV    *TerminalDevice
  ) 
{
  

  TERMINAL_REFRESH_EVENT_LIST       *TermEventList;
  VOID                              *Context = NULL;

  TermEventList = CR (mCurrentTermEvent, TERMINAL_REFRESH_EVENT_LIST, Link, TERM_REFRESH_EVENT_LIST_SIGNATURE);
  if (TermEventList->RefreshEvent == TerminalDevice->EventRefreshScreen) {
    TriggerNextTermEvent (Context);
  }


  //
  // Close Event then remove from Link List
  //
  RemoveRefreshEventList (TerminalDevice->EventRefreshScreen);

  if (TerminalDevice->EventRefreshScreen != NULL) {
    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "\nClose RefreshScreenRoutine !!!\n"));    
    gBS->CloseEvent (TerminalDevice->EventRefreshScreen);
    TerminalDevice->EventRefreshScreen = NULL;
  }


  //
  // Free Allocated Buffers
  //
  SafeFreePool(TerminalDevice->PtrScrChar);
  SafeFreePool(TerminalDevice->PtrTerChar);
  SafeFreePool(TerminalDevice->PtrScrAttr);
  SafeFreePool(TerminalDevice->PtrTerAttr);

  return;
}


/**
  
  The function to close refresh screen event.
  
  @param TerminalDevice    Terminal device

**/
VOID
AddRefreshEventList (
  IN EFI_EVENT    RefreshEvent,
  IN UINTN        TerminalCount
  )
{
  TERMINAL_REFRESH_EVENT_LIST     *TermEventListEntry;
  
  TermEventListEntry = AllocateZeroPool (sizeof (TERMINAL_REFRESH_EVENT_LIST));
  TermEventListEntry->Signature = TERM_REFRESH_EVENT_LIST_SIGNATURE;
  TermEventListEntry->RefreshEvent = RefreshEvent;

  InsertTailList (&mTermEventLists, &TermEventListEntry->Link);

  if (TerminalCount == 1) {
    mCurrentTermEvent = &TermEventListEntry->Link;
  }
}

/**
  
  Initialize terminal refresh screen event list.
  
**/
VOID
InitRefreshEventList (
  VOID
  )
{
  InitializeListHead (&mTermEventLists);
}


/**
  
  The function to trigger next terminal refresh screen event.
  
  @param Context    the context of event 

**/
VOID
TriggerNextTermEvent (
  IN  VOID    *Context
  )
{
  TERMINAL_REFRESH_EVENT_LIST       *TermEventList;

  if (!IsListEmpty (&mTermEventLists)) {    
    mCurrentTermEvent = mCurrentTermEvent->ForwardLink;
    if (mCurrentTermEvent == &mTermEventLists) {
      mCurrentTermEvent = mTermEventLists.ForwardLink;
    }

    if (mCurrentTermEvent != NULL) {
      TermEventList = CR (mCurrentTermEvent, TERMINAL_REFRESH_EVENT_LIST, Link, TERM_REFRESH_EVENT_LIST_SIGNATURE);
      if (TermEventList->RefreshEvent != NULL) {
        gBS->SetTimer (TermEventList->RefreshEvent, TimerRelative, REFRESH_ROUTING_INTERVAL);         
      }
    }
  }
}


/**
  
  Create Timer event to refresh terminal screen.
  
  @param TerminalDevice    Terminal device 
  @param TerminalCount     The number of terminal device in refresh screen list

  @retval EFI_SUCCESS            Refresh screen event initial success. 
  @retval Others                     event intial fail.

**/
EFI_STATUS
InitialRefreshScreenRoutine (
  IN TERMINAL_DEV    *TerminalDevice,
  IN UINTN           TerminalCount
  )
{
  EFI_STATUS                      Status;  

  TerminalDevice->LastRow = 0; 
  InitializeScreenBuffer (TerminalDevice);

  Status = gBS->CreateEvent (
               EVT_TIMER | EVT_NOTIFY_SIGNAL, 
               TPL_CALLBACK,
               RefreshScreenRoutine, 
               TerminalDevice, 
               &(TerminalDevice->EventRefreshScreen)
               );

  if (!EFI_ERROR(Status)) {
    AddRefreshEventList (TerminalDevice->EventRefreshScreen, TerminalCount);
    if (TerminalCount == 1) {
      Status = gBS->SetTimer (TerminalDevice->EventRefreshScreen, TimerRelative, REFRESH_ROUTING_INTERVAL); 
    }
  }

  return Status;
}


/**
  
  Monitor remote terminal was exist or not event handler.
  
  @param Event        The event callback of monitor remote terminal routine 
  @param Context     The event context 

**/
VOID
MonitorRemoteTermRoutine (
  IN  EFI_EVENT    Event,
  IN  VOID         *Context
  )
{
  EFI_SERIAL_IO_PROTOCOL    *SerialIo;
  TERMINAL_DEV              *TerminalDevice;
  EFI_STATUS                Status;
  
  TerminalDevice = (TERMINAL_DEV *)Context;
  SerialIo       = TerminalDevice->SerialIo;

  //
  // Depend on remote terminal status to set next time monitor event.
  //
  if (TerminalDevice->RemoteTermExist) {

    //
    //If romote terminal exist, set next event occur in next 30 secs.
    //
    Status = gBS->SetTimer (TerminalDevice->EventAutoRefresh, TimerRelative, (30000 * TICKS_PER_MS));
    TerminalDevice->LastRemoteTermStatus = TRUE;
  } else {
    //
    //If romote terminal exist, set next event occur in next 5 secs.
    //
    Status = gBS->SetTimer (TerminalDevice->EventAutoRefresh, TimerRelative, (5000 * TICKS_PER_MS));
    TerminalDevice->LastRemoteTermStatus = FALSE;
  }

  //
  // Write get device status esc sequence code
  //
  OutputEscSquenceString (&TerminalDevice->SimpleTextOutput, gGetTermStatusString);

  //
  // Set remote Terminal status as not exist as default.  
  //
  TerminalDevice->RemoteTermExist = FALSE;


}


/**
  
  Create Timer event to monitor remote termial.
  
  @param TerminalDevice        Terminal Device 

  @retval EFI_SUCCESS            monitor remote terminal event initial success. 
  @retval Others                     event intial fail.

**/
EFI_STATUS
InitialAutoRefreshRoutine (
  IN  TERMINAL_DEV  *TerminalDevice
  )
{
  EFI_STATUS    Status;

  //
  // Create Timer event to monitor remote termial
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL, 
                  TPL_CALLBACK,
                  MonitorRemoteTermRoutine, 
                  TerminalDevice,
                  &(TerminalDevice->EventAutoRefresh)
                  );

  if (!EFI_ERROR(Status)) {
    //
    // Set next event occur in next 5 secs.
    //
    Status = gBS->SetTimer (TerminalDevice->EventAutoRefresh, TimerRelative, (5000 * TICKS_PER_MS)); 
  }

  TerminalDevice->RemoteTermExist      = FALSE;
  TerminalDevice->LastRemoteTermStatus = FALSE;
  
  return Status;
}


/**
  
  Close monitor remote termial timer event.
  
  @param TerminalDevice        Terminal Device 

**/
VOID
CloseAutoRefreshRoutine (
  IN  TERMINAL_DEV  *TerminalDevice
  ) 
{
  
  if (TerminalDevice->EventAutoRefresh != NULL) {
    gBS->CloseEvent (TerminalDevice->EventAutoRefresh);
  }

  return;
}


/**
  
  This function will clean up terminal's Char and Attribute buffer.
  
  @param TerminalDevice        Terminal Device 

**/ 
VOID
ClearTermCharAndAttriBuffer (
  IN  TERMINAL_DEV    *TerminalDevice
  )
{
  CHAR16    *TerChar;
  UINT8     *TerAttr;
  UINTN     MaxResolution;
  UINTN     Resolution;
  UINTN     Index;
  UINTN     MaxMode;
  
  TerChar = TerminalDevice->PtrTerChar;
  TerAttr = TerminalDevice->PtrTerAttr;
  
  //
  // Find the maximum supported screen resolution
  //
  MaxResolution = TerminalDevice->TerminalConsoleModeData[0].Columns * TerminalDevice->TerminalConsoleModeData[0].Rows;  
  MaxMode = TerminalDevice->SimpleTextOutputMode.MaxMode;
  for (Index = 1; Index < MaxMode; Index ++) {
    Resolution = TerminalDevice->TerminalConsoleModeData[Index].Columns * TerminalDevice->TerminalConsoleModeData[Index].Rows; 
    if (MaxResolution < Resolution) {
      MaxResolution = Resolution;
    }
  }

  //
  // Clear terminal screen and attribute buffer 
  //
  SetMem (TerChar, (MaxResolution * sizeof (CHAR16)), 0);
  SetMem (TerAttr, (MaxResolution * sizeof (UINT8)), 0);

}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.Reset().

  If ExtendeVerification is TRUE, then perform dependent serial device reset,
  and set display mode to mode 0.
  If ExtendedVerification is FALSE, only set display mode to mode 0.

  @param  This                          Indicates the calling context.
  @param  ExtendedVerification   Indicates that the driver may perform a more
                                              exhaustive verification operation of the device
                                              during reset.

  @retval EFI_SUCCESS               The reset operation succeeds.
  @retval EFI_DEVICE_ERROR      The terminal is not functioning correctly or the serial port reset fails.

**/
EFI_STATUS
EFIAPI
AsyncTerminalConOutReset (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  BOOLEAN                          ExtendedVerification
  )
{
  EFI_STATUS    Status;
  TERMINAL_DEV  *TerminalDevice;

  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  // Reset Terminal, Send out "<ESC>c"
  //
  OutputEscSquenceString (This, gResetDeviceString);

  //
  // Disable Auto Wrap mode, Send out "ESC [ ? 7 l"
  //
  OutputEscSquenceString (This, gDiableAutoWrapString);

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
  Vt100SwitchCharSet (This, VT100_ASCII_CHAR_SET);

  Status = This->SetMode (This, 0);

  return Status;
}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.OutputString().

  The Unicode string will be converted to terminal expressible data stream
  and send to terminal via serial port.

  @param  This            Indicates the calling context.
  @param  WString       The Null-terminated Unicode string to be displayed
                                 on the terminal screen.

  @retval EFI_SUCCESS                          The string is output successfully.
  @retval EFI_DEVICE_ERROR                 The serial port fails to send the string out.
  @retval EFI_WARN_UNKNOWN_GLYPH  Indicates that some of the characters in the Unicode string could not
                                                          be rendered and are skipped.
  @retval EFI_UNSUPPORTED                  If current display mode is out of range.

**/
EFI_STATUS
EFIAPI
AsyncTerminalConOutOutputString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  CHAR16                           *WString
  )
{
  TERMINAL_DEV                 *TerminalDevice;
  EFI_SIMPLE_TEXT_OUTPUT_MODE  *Mode;
  UINTN                        MaxColumn;
  UINTN                        MaxRow;
  CHAR16                       *ScrChar;
  UINT8                        *AttrBuffer;
  CHAR16                       Unicode;
  UINT32                       GlyphWidth;
  INT32                        AtributeSetting;

  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  //
  //  Get current display mode
  //
  Mode = This->Mode;
  
  This->QueryMode (
          This,
          Mode->Mode,
          &MaxColumn,
          &MaxRow
          );
  
  ScrChar = TerminalDevice->PtrScrChar;
  AttrBuffer = TerminalDevice->PtrScrAttr;
  GlyphWidth = 0;
  
  for (; *WString != CHAR_NULL; WString++) {
    AtributeSetting = Mode->Attribute;

    //
    // Update screen RAW data buffer
    //
    switch (*WString) {

    case CHAR_BACKSPACE:
      if (Mode->CursorColumn > 0) {
        Mode->CursorColumn--;
      }
      else if ( Mode->CursorColumn == 0 && Mode->CursorRow > 0) {
        Mode->CursorRow--;
        Mode->CursorColumn = (INT32) MaxColumn - 1;
      }
      break;

    case CHAR_LINEFEED:
      if (Mode->CursorRow < (INT32) (MaxRow - 1)) {
        Mode->CursorRow++;
      }
      else {
        ScrollScreen (TerminalDevice);
      }
      break;

    case CHAR_CARRIAGE_RETURN:
      Mode->CursorColumn = 0;
      break;

    case WIDE_CHAR:
      AtributeSetting |= EFI_WIDE_ATTRIBUTE;
      break;

    case NARROW_CHAR:
      AtributeSetting &= (~ (UINT32) EFI_WIDE_ATTRIBUTE);
      break;

    default:
      Unicode = *WString;
      GlyphWidth = 1;

      //
      // Only UTF8 support Wide Character,
      // Translate Wide Character Unicode and calculate glyph width
      //
      if (TerminalDevice->TerminalType == VTUTF8TYPE) {
        if (AtributeSetting & EFI_WIDE_ATTRIBUTE) {
          WideUnicode (Unicode, &Unicode);
          GlyphWidth = 2;
        } else {
          //
          // If current attribute is narrow, still check if this chatacter belongs to wide glyph or not.
          //        
          GlyphWidth = ConsoleLibGetGlyphWidth (*WString);
          if (GlyphWidth >= 2) {
            AtributeSetting |= EFI_WIDE_ATTRIBUTE;
          }
        }
      }
      
      ScrChar[(Mode->CursorRow * MaxColumn) + Mode->CursorColumn] = Unicode;
      //
      // Update screen attribute buffer
      //
      AttrBuffer[(Mode->CursorRow * MaxColumn) + Mode->CursorColumn] = (UINT8) AtributeSetting;
      
      if (Mode->CursorColumn < (INT32) (MaxColumn - 1)) {
        //
        // move to next column
        //
        Mode->CursorColumn += GlyphWidth;

        if (Mode->CursorColumn > (INT32)(MaxColumn - 1)) {
          //
          // For avoid over column
          //
          Mode->CursorColumn = (INT32) MaxColumn - 1;
        }
      } else {
        //
        // move to next row
        //
        Mode->CursorColumn = 0;
        if (Mode->CursorRow < (INT32) (MaxRow - 1)) {
          Mode->CursorRow++;
        }
        else {
          ScrollScreen (TerminalDevice);
        }
      }
      break;

    };    
  }

  return EFI_SUCCESS;

}


/**
  Implements EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.TestString().

  If one of the characters in the *Wstring is neither valid Unicode drawing characters,
  not ASCII code, then this function will return EFI_UNSUPPORTED.

  @param  This                      Indicates the calling context.
  @param  WString                 The Null-terminated Unicode string to be tested.

  @retval EFI_SUCCESS           The terminal is capable of rendering the output string.
  @retval EFI_UNSUPPORTED   Some of the characters in the Unicode string cannot be rendered.

**/
EFI_STATUS
EFIAPI
AsyncTerminalConOutTestString (
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

  @param This               Indicates the calling context.
  @param ModeNumber  The mode number to return information on.
  @param Columns        The returned columns of the requested mode.
  @param Rows            The returned rows of the requested mode.

  @retval EFI_SUCCESS           The requested mode information is returned.
  @retval EFI_UNSUPPORTED   The mode number is not valid.

**/
EFI_STATUS
EFIAPI
AsyncTerminalConOutQueryMode (
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

  @retval EFI_SUCCESS           The requested text mode is set.
  @retval EFI_DEVICE_ERROR  The requested text mode cannot be set 
                                           because of serial device error.
  @retval EFI_UNSUPPORTED   The text mode number is not valid.

**/
EFI_STATUS
EFIAPI
AsyncTerminalConOutSetMode (
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

  Status = OutputEscSquenceString (This, gSetModeString);

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
AsyncTerminalConOutSetAttribute (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            Attribute
  )
{
  TERMINAL_DEV  *TerminalDevice;
 
  //
  //  get Terminal device data structure pointer.
  //
  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);

  if ((Attribute | 0xFF) != 0xFF) {
    return EFI_UNSUPPORTED;
  }

  This->Mode->Attribute = (INT32) Attribute;

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
AsyncTerminalConOutClearScreen (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This
  )
{
  EFI_STATUS                          Status;
  TERMINAL_DEV                        *TerminalDevice;
  EFI_SIMPLE_TEXT_OUTPUT_MODE         *Mode;
  UINTN                               MaxColumn;
  UINTN                               MaxRow;
  UINTN                               BufferSize;
  CHAR16                              *ScrChar;
  CHAR16                              *TerChar;
  UINTN                               Row, Column;  
  UINTN                               MaxResolution;
  UINTN                               Resolution;
  UINTN                               Index;
  UINTN                               MaxMode;
  UINT32                              OutputAttribute;

  TerminalDevice = TERMINAL_CON_OUT_DEV_FROM_THIS (This);
  Mode = This->Mode;
  This->QueryMode (
          This,
          Mode->Mode,
          &MaxColumn,
          &MaxRow
          );
  
  ScrChar = TerminalDevice->PtrScrChar;
  TerChar = TerminalDevice->PtrTerChar;
  OutputAttribute = 0;
  
  //
  // Restore default attribute (Foreground: White Background: Black)
  //
  OutputAttribute = (EFI_BLACK << 4) | EFI_LIGHTGRAY;
  PrivateSetAttribute (&(TerminalDevice->SimpleTextOutput), OutputAttribute);

  //
  // Erases the screen with the background colour and moves the cursor to home. 
  //
  Status = OutputEscSquenceString (This, gClearScreenString);
  
  //
  // Find the maximum supported screen resolution
  //
  MaxResolution = TerminalDevice->TerminalConsoleModeData[0].Columns * TerminalDevice->TerminalConsoleModeData[0].Rows;  
  MaxMode = TerminalDevice->SimpleTextOutputMode.MaxMode;
  for (Index = 1; Index < MaxMode; Index ++) {
    Resolution = TerminalDevice->TerminalConsoleModeData[Index].Columns * TerminalDevice->TerminalConsoleModeData[Index].Rows; 
    if (MaxResolution < Resolution) {
      MaxResolution = Resolution;
    }
  }

  //
  // Clear screen buffer and terminal buffer
  //
  for (Row = 0; Row < MaxRow; Row ++) {
    for (Column = 0; Column < MaxColumn; Column ++) {
      CopyMem (&(ScrChar[Row * MaxColumn + Column]), L" ", 2);
      CopyMem (&(TerChar[Row * MaxColumn + Column]), L" ", 2);
    }
  }
  BufferSize = MaxResolution * sizeof (UINT8);
  SetMem (TerminalDevice->PtrScrAttr, BufferSize, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  SetMem (TerminalDevice->PtrTerAttr, BufferSize, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);  

  //
  //  Status = This->SetCursorPosition (This, 0, 0);
  //
  Mode->CursorRow = 0;
  Mode->CursorColumn = 0;

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
AsyncTerminalConOutSetCursorPosition (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  UINTN                            Column,
  IN  UINTN                            Row
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE  *Mode;
  UINTN                        MaxColumn;
  UINTN                        MaxRow;
  EFI_STATUS                   Status;
  TERMINAL_DEV                 *TerminalDevice;

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
  //  update current cursor position
  //  in the Mode data structure.
  //
  Mode->CursorColumn = (INT32) Column;
  Mode->CursorRow    = (INT32) Row;

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
AsyncTerminalConOutEnableCursor (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *This,
  IN  BOOLEAN                          Visible
  )
{
  if (!Visible) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
