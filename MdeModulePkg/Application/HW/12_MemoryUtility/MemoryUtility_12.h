/** @file
  MemoryUtylity_12 H Source File

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

#ifndef _MEMORY_UTILITY_12_H_
#define _MEMORY_UTILITY_12_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#define MAIN_PAGE_MODE   0
#define BRANCH_PAGE_MODE 1
#define GET_SIZE_MODE    2
#define GET_ADDR_MODE    3
#define WRITE_MODE       4
#define SHOW_MODE        5
#define ERROR_MODE       6

typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

typedef struct {
  UINT64    value;      /// write value
  POSITION  position;   /// cursor position
} WRITE_BUFFER;

typedef enum {
  POOL,
  PAGE
} ALLOCATE_TYPE;

typedef enum {
  MAIN_PAGE_NUM     = 2,
  ALLOCATE_TYPE_NUM = 3,
  MEMORY_TYPE_NUM   = 14
} BRANCH_PAGE_NUM;

VOID 
SetMainPageAppearance (
  VOID
  );

VOID 
SetAllocateTypePageAppearance (
  VOID
  );

VOID 
SetMemoryTypePageAppearance (
  VOID
  );

VOID 
SetGetAddressPageAppearance (
  VOID
  );  

VOID 
SetGetSizePageAppearance (
  VOID
  ); 

VOID 
SetMemorySettingPageAppearance (
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

UINT64 
Power (
  UINT64 Base, 
  UINT64 Index
  );

#endif