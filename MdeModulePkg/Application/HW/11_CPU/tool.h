/** @file
  Tool H Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TOOL_H_
#define _TOOL_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

#define InputAmount 8

typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

typedef struct {
  UINT64    value;      /// write value
  POSITION  position;   /// cursor position
} WRITE_BUFFER;

VOID 
InitializeArray(
  WRITE_BUFFER *WriteBuffer
  );

UINT64 
Power (
  UINT64 Base, 
  UINT64 Index
  );

UINT32 
InputValue (
  WRITE_BUFFER *WriteBuffer, 
  POSITION *CursorPosition
  );

VOID
SetMainPageAppearance (
  VOID
  );

VOID
ShowMainPage (
  VOID
  );

VOID
SetBranchPageSupplementAppearance (
  VOID
  );

#endif