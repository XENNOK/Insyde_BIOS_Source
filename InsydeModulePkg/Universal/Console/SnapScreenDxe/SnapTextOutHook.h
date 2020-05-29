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


#ifndef _SNAP_TEXT_OUT_HOOK_H_
#define _SNAP_TEXT_OUT_HOOK_H_



VOID
TextGetBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

VOID
TextPutBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );
  
EFI_STATUS
InitializeTextOutHook (
  VOID
  );

EFI_STATUS
FinalizeTextOutHook (
  VOID
  );
  
#endif // _SNAP_TEXT_OUT_HOOK_H_
