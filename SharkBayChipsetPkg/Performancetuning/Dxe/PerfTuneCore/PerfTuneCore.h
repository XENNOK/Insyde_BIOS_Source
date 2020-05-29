/** @file

  This file contains the headers for the core XTU table 
  generation functions.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _EFI_PERF_TUNE_CORE_H
#define _EFI_PERF_TUNE_CORE_H

#include <ChipsetSetupConfig.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/PerfTune.h>
  
#define DEF_STR_LEN         1000
#define BASE_VOLTAGE        12000

/**
  
  This routine returns a Discrete BIOS Setting DDD table
  
  @param  RawDiscreteBiosSettingList      Table defined raw data of discrete bios settings        
  @param  BiosSettingTable                     This is a pointer to the DDD table produced
  @param  TableSize                               This is the size of the DDD table produced

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_ABORTED

**/
EFI_STATUS
BuildDiscreteBiosSettingTable(
  IN  RAW_DDD_LBS_DATA            *RawDiscreteBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  );

/**
  
  This routine returns a Continuous BIOS Setting DDD table
  
  @param  RawDiscreteBiosSettingList      Table defined raw data of discrete bios settings        
  @param  BiosSettingTable                     This is a pointer to the DDD table produced
  @param  TableSize                               This is the size of the DDD table produced

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_ABORTED

**/
EFI_STATUS
BuildContinuousBiosSettingTable(
  IN  RAW_DDD_LBC_DATA            *RawContinueBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  );

/**
  
  This routine returns a High Precision Continuous BIOS Setting DDD table
  
  @param  RawDiscreteBiosSettingList      Table defined raw data of discrete bios settings        
  @param  BiosSettingTable                     This is a pointer to the DDD table produced
  @param  TableSize                               This is the size of the DDD table produced

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_ABORTED

**/
EFI_STATUS
BuildHpContinuousBiosSettingTable(
  IN  RAW_DDD_HLBC_DATA           *RawHpContinueBiosSettingList,
  OUT UINT8                       **BiosSettingTable,
  OUT UINTN                       *TableSize
  );

/**
  
  This routine reallocates the memory necessary for a specific buffer size if the current buffer is not large enough
  
  @param  DataBuffer      This is the address of a pointer to the data buffer.      
  @param  DataLen          This is the size of the requested buffer
  @param  BufferSize       On input this is the size of the current buffer, On output this is the size of the new buffer

  @retval  EFI_SUCCESS                          The function completed successfully.
  @retval  EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
ReallocIfBufferOverflow (
  IN UINT8                    **DataBuffer,
  IN UINTN                    DataLen,
  IN OUT UINTN                *BufferSize
  );

/**
  
  This routine is responsible for converting a string into a numeric value and an associated 
  precision.
  
  @param  Str      This is the string to be converted  
  @param  Val      This is the value the string was converted to
  @param  Prec    This is the number of digits of precision in the value returned 
                          (i.e. how many digits should be considered part of the decimal)

  @retval  None

**/
VOID
StringToSettingsValue(
  CHAR16 *Str,
  UINT16 *Val,
  UINT8  *Prec
  );


//#if defined(IDCC2_SUPPORTED) && (IDCC2_SUPPORTED != 0)
/**
  
  This routine is a math function which multiplies the Operand times ten to the power of the 
  exponent.
  
  @param  Operand      This is the operand to be acted upon
  @param  Exp             This is the exponent value

  @retval  This is the resulting value of Operand * (10 ^ Exp)

**/
UINT16
PowTen(
  IN  UINT16 Operand, 
  IN  UINT16 Exp
  );
//#endif
#endif
