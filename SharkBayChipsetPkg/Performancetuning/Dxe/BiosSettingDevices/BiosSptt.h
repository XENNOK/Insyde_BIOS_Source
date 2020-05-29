/** @file

  Header file for building the Memory $DDD tables necessary to add to the SPTT
  table used for Performance Tuning in XTU

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
#ifndef _EFI_MEMORY_PERF_TUNE_H
#define _EFI_MEMORY_PERF_TUNE_H

#include <ChipsetSetupConfig.h>
#include <CpuDataStruct.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/XTUInfoHob.h>
#include <Protocol/PerfTune.h>

// UQI Strings
#define StrMemRefFreqPromptUqi               0x01E8  // STR_FSB_LATCH_PROMPT
#define StrMemFreqPromptUqi                  0x0428  // STR_MEMORY_SPEED_PROMPT

#define Str_tCL_PromptUqi                    0x04B6  // STR_NUMERIC_MEMORY_TCL_PROMPT
#define Str_tRCD_PromptUqi                   0x04B0  // STR_NUMERIC_MEMORY_TRCD_PROMPT
#define Str_tRP_PromptUqi                    0x04B3  // STR_NUMERIC_MEMORY_TRP_PROMPT
#define Str_tRASmin_PromptUqi                0x04B4  // STR_NUMERIC_MEMORY_TRAS_PROMPT
#define Str_Uclk_MultiplierUqi               0x045D  // STR_UCLK_RATIO_PROMPT
#define Str_Mem_MultiplierUqi                0x0430  // STR_MEMORY_RATIO_PROMPT
#define Str_tRFC_PromptUqi                   0x04AE  // STR_NUMERIC_MEMORY_TRFC_PROMPT
#define Str_tRRD_PromptUqi                   0x04AF  // STR_NUMERIC_MEMORY_TRRD_PROMPT
#define Str_tWR_PromptUqi                    0x04B2  // STR_NUMERIC_MEMORY_TWR_PROMPT
#define Str_tWTR_PromptUqi                   0x04B1  // STR_NUMERIC_MEMORY_TWTR_PROMPT
#define Str_tRTP_PromptUqi                   0x04B5  // STR_NUMERIC_MEMORY_TRTP_PROMPT
#define Str_tCK_PromptUqi                    0x0401  // STR_MEMORY_COMMAND_RATE_PROMPT

/**
  
  Entry point and initialization function for the Voltage Override component.  This installs the SPTT Data
  protocol for each independently controllable voltage in the system.

  @param  ImageHandle           Not Used
  @param  SystemTable            Not Used

  @retval EFI_SUCCESS            Initialization completed successfully
  @retval All others                   Failure to initialize the Tuning Core correctly

**/
EFI_STATUS
BiosSpttInit (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  );

EFI_STATUS
GetBiosSettingData(
  OUT UINT8                       **DataBuffer,
  OUT UINTN                       *DataLen
  );

BOOLEAN
ValidMemMultEntry(
  UINT16 XeModeCap,
  UINT16 TurboModeCap,
  UINT8  *DataBuffer,
  UINTN  Idx
  );

#endif
