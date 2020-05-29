/** @file
  The H2O_DIALOG_PROTOCOL is the interface to the EFI Configuration Driver.

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

#ifndef _H2O_DIALOG_H_
#define _H2O_DIALOG_H_

#define H2O_DIALOG_PROTOCOL_GUID  \
  { \
    0x3634b8f9, 0xd032, 0x4f21, 0xa9, 0x89, 0x31, 0xad, 0xf0, 0x5f, 0x00, 0x12 \
  }

typedef struct _H2O_DIALOG_PROTOCOL H2O_DIALOG_PROTOCOL;

#ifndef MDE_CPU_ARM
typedef UINT16 STRING_REF;
#endif

typedef enum {
  DlgYesNo,
  DlgYesNoCancel,
  DlgOk,
  DlgOkCancel
} UI_DIALOG_OPERATION;


typedef
EFI_STATUS
(EFIAPI *H2O_CONFIRM_DIALOG) (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  );

typedef
EFI_STATUS
(EFIAPI *H2O_PASSWORD_DIALOG) (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *UserInputStringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *TitleString,
  ...
  );

typedef
EFI_STATUS
(EFIAPI *H2O_ONE_OF_OPTION_DIALOG) (
  IN  UINT32                          NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  CONST EFI_INPUT_KEY             *KeyList,
  OUT EFI_INPUT_KEY                   *EventKey,
  IN  UINT32                          MaximumStringSize,
  OUT CHAR16                          *TitleString,
  OUT UINT32                          *SelectIndex,
  IN  CHAR16                          **String,
  IN  UINT32                          Color
  );

typedef
EFI_STATUS
(EFIAPI *H2O_CREATE_MSG_POP_UP) (
  IN UINT32                           RequestedWidth,
  IN UINT32                           NumberOfLines,
  IN CHAR16                           *ArrayOfStrings,
  ...
  );

typedef
EFI_STATUS
(EFIAPI *H2O_SHOW_PAGE_INFORMATION) (
  IN CHAR16          *TitleString,
  IN CHAR16          *InfoStrings
  );

typedef
EFI_STATUS
(EFIAPI *H2O_NUMERIC_DIALOG) (
  IN  CHAR16                          *TitleString,
  IN  UINT64                          Minimum,
  IN  UINT64                          Maximum,
  IN  UINT64                          Step,
  IN  BOOLEAN                         IsHex,
  OUT UINT64                          *NumericValue
  );

struct _H2O_DIALOG_PROTOCOL {
  H2O_CONFIRM_DIALOG                 ConfirmDialog;
  H2O_PASSWORD_DIALOG                PasswordDialog;
  H2O_ONE_OF_OPTION_DIALOG           OneOfOptionDialog;
  H2O_CREATE_MSG_POP_UP              CreateMsgPopUp;
  H2O_SHOW_PAGE_INFORMATION          ShowPageInfo;
  H2O_NUMERIC_DIALOG                 NumericDialog;
  };

extern EFI_GUID gH2ODialogProtocolGuid;

#endif