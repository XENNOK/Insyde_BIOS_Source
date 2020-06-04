/** @file
  PciUtility_06 H Source File

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

#ifndef _PCIUTILITY_06_H_
#define _PCIUTILITY_06_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

#define MAIN_PAGE_MODE   0
#define BRANCH_PAGE_MODE 1
#define WRITE_MODE       2
 
#define BYTE             1
#define WORD             2
#define DWORD            4

typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

typedef struct {
  UINT32    value;      /// write value
  POSITION  position;   /// cursor position
} WRITE_BUFFER;

typedef struct {
  UINT16   Bus;         
  UINT16   Device;      
  UINT16   Func;
  UINT16   Offset;
} ADDRESS;

UINT8
ShowMainPage (
  POSITION *CursorPosition
  );

VOID 
ShowRegistPage (
  ADDRESS *Address, 
  POSITION *CursorPosition
  );

VOID 
ReadRegist (
  ADDRESS *Address,
  POSITION *CursorPosition, 
  UINT8 ReadMode
  );

VOID 
WriteRegist (
  ADDRESS *Address, 
  UINT32 WriteBufferConvert, 
  UINT8 ReadMode
  );

VOID 
DisplayPosition (
  UINT8 ReadMode, 
  POSITION *CursorPosition
  );

const POSITION  MainPage_ResetPosition  = {16, 5};
const POSITION  ReadMode1_ResetPosition = {5, 4};
const POSITION  ReadMode2_ResetPosition = {6, 4};                  
const POSITION  ReadMode4_ResetPosition = {8, 4};

#endif