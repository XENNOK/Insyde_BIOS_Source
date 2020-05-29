/** @file
  Header file for support functions can work both in protected mode and SMM mode.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _COMMON_FUNCTIONS_H_
#define _COMMON_FUNCTIONS_H_
#include "FvbServicesRuntimeDxe.h"


#define FVB_FREE_POOL(Addr) \
{  mSmst ? mSmst->SmmFreePool((VOID*) (Addr)) : gBS->FreePool ((VOID *) (Addr)); \
   Addr = NULL;}

/**
  According to system mode to allocate pool.
  Allocate BootServicesData pool in protect mode if Runtime is FALSE.
  Allocate EfiRuntimeServicesData pool in protect mode if Runtime is TRUE.
  Allocate memory from SMM ram if system in SMM mode.

  @param[in] Size    The size of buffer to allocate
  @param[in] Runtime Runtime Data or not.

  @return NULL       Buffer unsuccessfully allocated.
  @return Other      Buffer successfully allocated.
**/
VOID *
FvbAllocateZeroBuffer (
  IN UINTN     Size,
  IN BOOLEAN   Runtime
  );


/**
  Check access FVB through SMI or not

  @retval TRUE    Accessing FVB through SMI.
  @retval FALSE   Accessing FVB in protected mode.
**/
BOOLEAN
IsFvbAccessThroughSmi (
  VOID
  );

/**
  Common function to do flash read.

  This function can auto detect system supports accessing FVB through SMI.
  If supporting this feature, it will read flash data through SMI.
  Otherwise, it will read data in protected mode.

  @param[in]      ReadAddress  Target address to be read.
  @param[in, out] ReadLength   In:  Input buffer size in bytes.
                               Out: Total read data size in bytes.
  @param[out]     ReadBuffer   Output buffer to contains read data.

  @retval EFI_SUCCESS          Data successfully read from flash device.
  @retval EFI_UNSUPPORTED      The flash device is not supported.
  @retval EFI_DEVICE_ERROR     Failed to read the blocks.
**/
EFI_STATUS
CommonFlashRead (
  IN       UINTN      ReadAddress,
  IN OUT   UINTN      *ReadLength,
  OUT      VOID       *ReadBuffer
  );

/**
  Common function to do flash write.

  This function can auto detect system supports accessing FVB through SMI.
  If supporting this feature, it will write data to flash through SMI.
  Otherwise, it will write data in protected mode.

  @param[in]      WriteAddress  Target address to write.
  @param[in, out] WriteLength   In:  Input buffer size in bytes.
                                Out: Total write data size in bytes.
  @param[out]     WriteBuffer   input buffer to write.

  @retval EFI_SUCCESS           Data successfully write to flash device.
  @retval EFI_UNSUPPORTED       The flash device is not supported.
  @retval EFI_DEVICE_ERROR      Failed to write the blocks.
**/
EFI_STATUS
CommonFlashWrite (
  IN       UINTN      WriteAddress,
  IN OUT   UINTN      *WriteLength,
  IN       VOID       *WriteBuffer
  );

/**
  Common function to do flash erase.

  This function can auto detect system supports accessing FVB through SMI.
  If supporting this feature, it will erase flash blocks through SMI.
  Otherwise, it will erase flash blocks in protected mode.

  @retval EFI_SUCCESS           Erase flash block successful.
  @retval EFI_UNSUPPORTED       The flash device is not supported.
  @retval EFI_DEVICE_ERROR      Failed to erase blocks.
**/
EFI_STATUS
CommonFlashErase (
  IN  UINTN      EraseAddress,
  IN  UINTN      EraseLength
  );

#endif
