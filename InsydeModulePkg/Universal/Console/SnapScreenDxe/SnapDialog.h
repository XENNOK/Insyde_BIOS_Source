/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _SNAP_DIALOG_H_
#define _SNAP_DIALOG_H_

#include <Uefi.h>
#include "SnapWin.h"

typedef struct _FS_DIALOG   FS_DIALOG;

typedef EFI_STATUS (*DIALOG_PROC) (FS_DIALOG *FsDialog);

typedef struct _FS_ITEM {
  UINT32            Signature;
  LIST_ENTRY        Link;
  UINT16            Index;
  EFI_HANDLE        ParentHandle;
  EFI_HANDLE        FsHandle;
  CHAR16            *Title;
} FS_ITEM;

typedef struct _FS_DIALOG {
  TWINDOW         *Win;
  DIALOG_PROC     DialogProc;
  UINTN           ItemCount;
  INT16           Index;
  LIST_ENTRY      *CurrLink;
  LIST_ENTRY      ItemHead;
} FS_DIALOG;

typedef struct _MSG_DIALOG {
  TWINDOW         *Win;
  BOOLEAN         IsShow;
  CHAR16          *Message;
} MSG_DIALOG;


EFI_STATUS
TFsDialogCreate (
  UINT16        X,
  UINT16        Y,
  CHAR16        *Title,
  DIALOG_PROC   DialogProc,
  FS_DIALOG     **FsDialog
  );

EFI_STATUS
TFsDialogDestroy (
  FS_DIALOG   *Dialog
  );

EFI_STATUS
TFsDialogExec (
  FS_DIALOG   *Dialog,
  EFI_HANDLE  *Result
  );

EFI_STATUS
TMsgDialogCreate (
  CHAR16        *MessageStr,
  MSG_DIALOG    **MsgDialog
  );

EFI_STATUS
TMsgDialogShow (
  MSG_DIALOG      *MsgDialog
  );

EFI_STATUS
TMsgDialogRun (
  MSG_DIALOG    *MsgDialog
  );

EFI_STATUS
TMsgDialogDestroy (
  MSG_DIALOG    *MsgDialog
  );
  
EFI_STATUS
PopUpMessage (
  CHAR16    *MessageStr
  );

#endif  //_SNAP_DIALOG_H_
