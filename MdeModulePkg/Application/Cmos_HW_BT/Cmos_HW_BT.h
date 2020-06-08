/** @file
  Cmos_HW_BT H Source File

;******************************************************************************
;* Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CMOS_HW_BT_H_
#define _CMOS_HW_BT_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/CpuIo2.h>

#define MAIN_PAGE_MODE   0
#define BRANCH_PAGE_MODE 1

#define Count            256

#define BdaSegment       0x400

#define CmosWritePort    0x70
#define CmosReadPort     0x71
#define CmosMaxIndex     0x7f

typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

const POSITION  MainPage_ResetPosition  = {24, 5};
const POSITION  ResetPosition           = {5, 4};

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
ResetCursorPosition (
  POSITION *CursorPosition
  );

VOID 
JudgeFontColor (
  UINT8 Buffer
  );

VOID 
ReadBda (
  POSITION *CursorPosition, 
  UINT8 RecordBuffer[], 
  UINT8 *RecordBufferTime
  );

VOID 
ReadEbda (
  POSITION *CursorPosition, 
  UINT8 RecordBuffer[], 
  UINT8 *RecordBufferTime
  );

VOID 
ReadCmos (
  POSITION *CursorPosition, 
  UINT8 RecordBuffer[], 
  UINT8 *RecordBufferTime
  );

#endif