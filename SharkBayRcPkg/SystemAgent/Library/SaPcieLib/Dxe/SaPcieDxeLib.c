/**
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/

/**

Copyright (c) 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  SaPcieDxeLib.c

@brief:

  This is DXE library code used by Intel System Agent PCIe library

**/

#include <Uefi.h>
#include <PiDxe.h>
//#include <EfiScriptLib.h>
#include <Library/S3BootScriptLib.h>
//#include <Protocol/BootScriptSave.h>

EFI_STATUS
SaScriptMemWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  UINTN                             Count,
  IN  VOID                              *Buffer
)
/**

@brief

  Wrapper for boot script with opcode EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE

  @param[in] Width   - The width of the memory operations.
  @param[in] Address - The base address of the memory operations.
  @param[in] Count   - The number of memory operations to perform.
  @param[in] Buffer  - The source buffer from which to write the data. 

  @retval EFI_SUCCESS - Always returns EFI_SUCCESS


**/
{
  S3BootScriptSaveMemWrite (Width, Address, Count, Buffer);
  return EFI_SUCCESS;
}

EFI_STATUS
SaScriptMemReadWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  VOID                              *Data,
  IN  VOID                              *DataMask
)
/**

@brief

  Wrapper for boot script with opcode EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE

  @param[in] TableName - Desired boot script table
  @param[in] Width   - The width of the memory operations.
  @param[in] Address - The base address of the memory operations.
  @param[in] Data    - A pointer to the data to be OR-ed.
  @param[in] DataMask  - A pointer to the data mask to be AND-ed with the data read from the register.

  @retval EFI_SUCCESS - Always returns EFI_SUCCESS


**/
{
  S3BootScriptSaveMemReadWrite (Width, Address, Data, DataMask);
  return EFI_SUCCESS;
}

EFI_STATUS
SaScriptMemPoll (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH          Width,
  IN  UINT64                            Address,
  IN  VOID                              *BitMask,
  IN  VOID                              *BitValue,
  IN  UINTN                             Duration,
  IN  UINTN                             LoopTimes
  )
/**

@brief

  Wrapper for boot script for Polling one memory mapping register

  @param[in] Width     - The width of the memory operations.
  @param[in] Address   - The base address of the memory operations.
  @param[in] BitMask   - A pointer to the bit mask to be AND-ed with the data read from the register.
  @param[in] BitValue  - A pointer to the data value after to be Masked.
  @param[in] Duration  - Duration in microseconds of the stall.
  @param[in] LoopTimes - The times of the register polling.

  @retval EFI_SUCCESS - Always returns EFI_SUCCESS

**/
{
  S3BootScriptSaveMemPoll (Width, Address, BitMask, BitValue, Duration, LoopTimes);
  return EFI_SUCCESS;
}
