/** @file
  Variable_15 H Source File

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

#ifndef _VARIABLE_15_H_
#define _VARIABLE_15_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#define WORD_INITIAL_SIZE    10
#define WORD_SIZE_INCREMENT  10

#define VALUE_INITIAL_SIZE   1
#define VALUE_SIZE_INCREMENT 1

EFI_STATUS
SearchAllVariable (
  VOID
  );

EFI_STATUS
SearchVariableByName (
  VOID
  );

EFI_STATUS
SearchVariableByGuid (
  VOID
  );

EFI_STATUS
SetCreateVariable (
  VOID
  );

EFI_STATUS
DeleteVariable (
  VOID
  );

VOID
ShowMainPage (
  VOID
  );

VOID
PressStop (
  VOID
  );

CHAR16* 
InputWord (
  VOID
  );
  
UINT64
InputValueD (
  VOID
  );

UINT64
InputValueH (
  VOID
  );

UINT64*
InputGuid (
  UINT64  *Buffer
  );

EFI_GUID 
ParseGuid (
  UINT64 *Buffer
  );

VOID
InitializeArray_8 (
  CHAR8 *Buffer,
  UINT64 BufferSize
  );

VOID
InitializeArray_16 (
  CHAR16 *Buffer,
  UINT64 BufferSize
  );

UINT64 
Power (
  UINT64 Base, 
  UINT64 Index
  );

VOID
ShowAttributeTable (
  VOID
  );

#endif