/** @file
  Provide support functions for FVB services which can be executed both in
  protected mode and SMM mode.

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

#include "CommonFunctions.h"
#include "SmmFunctions.h"

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
  )
{
  VOID           *Buffer;
  EFI_STATUS     Status;

  if (mSmst == NULL) {
    Status = gBS->AllocatePool (
                    Runtime ? EfiRuntimeServicesData : EfiBootServicesData,
                    Size,
                    &Buffer
                    );
  } else {
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Size,
                      &Buffer
                      );
  }

  if (!EFI_ERROR (Status)) {
    ZeroMem (Buffer, Size);
  } else {
    Buffer = NULL;
  }
  return Buffer;
}


/**
  Check access FVB through SMI or not

  @retval TRUE    Accessing FVB through SMI.
  @retval FALSE   Accessing FVB in protected mode.
**/
BOOLEAN
IsFvbAccessThroughSmi (
  VOID
  )
{
  if (mFvbAccessThroughSmi || mSmst != NULL) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  if (IsFvbAccessThroughSmi ()) {
    return ReadFdThroughSmi (ReadAddress, ReadLength, ReadBuffer);
  } else {
    return FlashRead ((UINT8 *) ReadBuffer, (UINT8 *) ReadAddress, *ReadLength);
  }
}

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
  OUT      VOID       *WriteBuffer
  )
{
  if (IsFvbAccessThroughSmi ()) {
    return WriteFdThroughSmi (WriteAddress, WriteLength, WriteBuffer);
  } else {
    return FlashProgram ((VOID *) (UINTN) WriteAddress, (VOID *) WriteBuffer, WriteLength, WriteAddress);
  }
}

/**
  Common function to do flash erase.

  This function can auto detect system supports accessing FVB through SMI.
  If supporting this feature, it will erase flash blocks through SMI.
  Otherwise, it will erase flash blocks in protected mode.

  @param[in] EraseAddress   Target address to erase.
  @param[in] EraseLength    Erase size in bytes.

  @retval EFI_SUCCESS       Erase flash block successful.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to erase blocks.
**/
EFI_STATUS
CommonFlashErase (
  IN  UINTN      EraseAddress,
  IN  UINTN      EraseLength
  )
{
  if (IsFvbAccessThroughSmi ()) {
    return EraseFdThroughSmi (EraseAddress, EraseLength);
  } else {
    return FlashErase (EraseAddress, EraseLength);
  }
}
