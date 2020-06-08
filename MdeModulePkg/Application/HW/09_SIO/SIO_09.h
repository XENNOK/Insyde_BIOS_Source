/** @file
  SIO_09 H Source File

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

#ifndef _SIO_09_H_
#define _SIO_09_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/CpuIo2.h>

#define MAIN_PAGE_MODE    0
#define BRANCH_PAGE_MODE  1

#define DownEdgeInterval  15
#define RightEdgeInterval 50
#define LeftEdgeInterval  5

#define MaxIndex          256

#define AddressPort       0x2e
#define DataPort          0x2f
#define Unlock            0x87
#define Lock              0xaa
#define LdnReg            0x7

typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

typedef struct {
  UINT64    value;      /// write value
  POSITION  position;   /// cursor position
} WRITE_BUFFER;

VOID 
SetMainPageAppearance (
  VOID
  );

VOID 
SetRegisterPageAppearance (
  VOID
  );

VOID 
ShowMainPage (
  POSITION *CursorPosition
  );

VOID 
ShowRegisterPage (
  POSITION *CursorPosition
  );

VOID 
ShowOffset (
  POSITION *CursorPosition
  );

VOID 
ShowTitle (
  POSITION *CursorPosition,
  UINT8    *Ldn
  );

VOID 
ShowSio (
  POSITION *CursorPosition,
  UINT8    *Ldn
  );

UINT64 
InputValue (
  WRITE_BUFFER *WriteBuffer,
  UINT32       *InputAmount
  );

VOID 
InitializeArray(
  WRITE_BUFFER *WriteBuffer
  );

EFI_STATUS
ReadSio (
  POSITION *CursorPosition,
  UINT8    *Ldn
  );

EFI_STATUS
WriteSio (
  POSITION *CursorPosition,
  UINT8    *Ldn,
  UINT64   ModifyValue
  );

#endif