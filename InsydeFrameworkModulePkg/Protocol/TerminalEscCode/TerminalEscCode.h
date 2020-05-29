//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _TERMINAL_ESC_CODE_H_
#define _TERMINAL_ESC_CODE_H_

#define TERMINAL_ESC_CODE_PROTOCOL_GUID \
  { \
    0xC40BC698, 0x0F16, 0x443c, 0xB2, 0xF0, 0xBA, 0x6B, 0x5B, 0x16, 0x37, 0x99\
  }


// CAUTION : The TP_XXXX definition must sync with VFR's TerminalType order
#define TP_VT100                1
#define TP_VT100P               2
#define TP_VTUTF8               4
#define TP_PCANSI               8

typedef struct {
  UINT16  EfiScanCode;
  UINT16  KbScanCode;
} EFI_TO_KB_SCANCODE_MAP;

typedef enum {
  ESC_CODE_SCANCODE,
  ESC_CODE_EXTENTION,
  ESC_CODE_CONTROL,
  ESC_CODE_ALTCODE,
  ESC_CODE_FUNC
} ESC_CODE_TYPE;

typedef struct {
  UINT8   TerminalType : 4;
  UINT8   Reserve : 1;
  UINT8   DataType : 3;
  UINT16  Data;
  CHAR16  *EscSequenceCode;
} ESC_SEQUENCE_CODE;

typedef enum {
  TYPE_FUNCTION_ID,      // Special Command Type   
  TYPE_FUNCTION_POINTER
} SPECIAL_COMMAND_TYPE;

typedef struct _CR_SPECIAL_COMMAND {
  CHAR16                    *CommandStr;
  SPECIAL_COMMAND_TYPE      CommandType;
  UINT16                    Command;
} CR_SPECIAL_COMMAND;

typedef struct {
  UINT16                  EscCodeCount;
  ESC_SEQUENCE_CODE       *EscSequenceCode;
  EFI_TO_KB_SCANCODE_MAP  *EfiToKbScanCode;
  UINT16                  CrSpecialCommandCount;
  CR_SPECIAL_COMMAND      *CrSpecialCommand;
} EFI_TERMINAL_ESC_CODE_PROTOCOL;

extern EFI_GUID gTerminalEscCodeProtocolGuid;

#endif
