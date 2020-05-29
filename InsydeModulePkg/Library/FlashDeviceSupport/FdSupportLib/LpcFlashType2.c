/** @file
  FdSupportLib implementation for LPC Flash Type 2 devices

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/FdSupportLib.h>
#include "LpcFlashSupport.h"


/**
  Flash device Recognize() fuction for LPC Flash Type 2 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported

**/
EFI_STATUS
EFIAPI
LpcFlashType2Recognize (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  UINT16                        CheckId;
  UINTN                         IdAddress;
  
  if (FlashDevice->DeviceType != LpcFlashType2) {
    return EFI_UNSUPPORTED;
  }
  
  IdAddress = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->IdAddress;

  SetMem ((VOID *)IdAddress, 1, LPC_FLASH_TYPE2_ID_CMD);
  CheckId = *((UINT16 *)IdAddress);
  SetMem ((VOID *)IdAddress, 1, LPC_FLASH_TYPE2_READ_ARRAY_CMD);

  if (CheckId == (UINT16)FlashDevice->DeviceInfo.Id) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}


/**
  Flash device Program() fuction for LPC Flash Type 2 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param Dest                   Target address to be programmed
  @param Src                    Souce buffer
  @param NumBytes               Total number of bytes to be programmed
  @param LbaWriteAddress        Flash device write address

  @retval EFI_SUCCESS           Data successfully written to flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to write the Flash ROM

**/
EFI_STATUS
EFIAPI
LpcFlashType2Program (
  IN FLASH_DEVICE               *FlashDevice,
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  )
{
  EFI_STATUS                    Status;
  UINTN                         Count;
  UINT8                         HubData;
  UINTN                         RetryTimes;

  Status = EFI_SUCCESS;

  for (Count = 0; Count < *NumBytes; Count++, Dest++, Src++) {

    SetMem ((VOID *)Dest, 1, LPC_FLASH_TYPE2_WRITE_SETUP_CMD);
    SetMem ((VOID *)Dest, 1, *Src);
    SetMem ((VOID *)Dest, 1, LPC_FLASH_TYPE2_READ_STATUS_CMD);

    //
    // Device error if time out occurs
    //
    RetryTimes = 0;
    while (RetryTimes < FVB_MAX_RETRY_TIMES) {
      HubData = *(UINT8*)Dest;
      if (HubData & LPC_FLASH_TYPE2_WRITE_STATE_STATUS) {
        break;
      }

      MicroSecondDelay (STALL_TIME);
      RetryTimes += STALL_TIME;
    }

    if (RetryTimes >= FVB_MAX_RETRY_TIMES) {
      *NumBytes = Count;
      Status    = EFI_DEVICE_ERROR;
      break;
    }
  }
  //
  // Clear status register
  //
  SetMem ((VOID *)LbaWriteAddress, 1, LPC_FLASH_TYPE2_CLEAR_STATUS_CMD);

  //
  // Issue read array command to return the FWH state machine to the
  // normal operational state
  //
  SetMem ((VOID *)LbaWriteAddress, 1, LPC_FLASH_TYPE2_READ_ARRAY_CMD);

  return Status;
}


/**
  Flash device EraseBlock() fuction for LPC Flash Type 1 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param LbaWriteAddress        Starting address to be erased

  @retval EFI_SUCCESS           Sector successfully erased
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Erase failed

**/
EFI_STATUS
EFIAPI
LpcFlashType2EraseBlock (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress
  )
{
  EFI_STATUS                    Status;
  UINT8                         HubData;
  UINTN                         TimeOut;

  Status = EFI_SUCCESS;

  //
  // Send erase commands to FWH
  //
  SetMem ((VOID *)(UINTN)LbaWriteAddress, 1, LPC_FLASH_TYPE2_BLOCK_ERASE_SETUP_CMD);

  SetMem ((VOID *)(UINTN)LbaWriteAddress, 1, LPC_FLASH_TYPE2_BLOCK_ERASE_CONFIRM_CMD);

  SetMem ((VOID *)(UINTN)LbaWriteAddress, 1, LPC_FLASH_TYPE2_READ_STATUS_CMD);

  //
  // Wait for completion. Indicated by LPC_FLASH_TYPE2_WRITE_STATE_STATUS bit becoming 0
  // Device error if time out occurs
  //
  TimeOut = 0;
  HubData = 0;
  while (TimeOut < FVB_MAX_RETRY_TIMES) {
    HubData = *(UINT8*)LbaWriteAddress;

    //
    // Commented out as EfiStall() in IA32 runtime environment
    // is not available yet
    //
    MicroSecondDelay (STALL_TIME);

    if (HubData & LPC_FLASH_TYPE2_WRITE_STATE_STATUS) {
      break;
    }
    TimeOut = TimeOut + STALL_TIME;
  }

  //
  // Commented out as it will have problems on Anacortes
  //
  if ( TimeOut >= FVB_MAX_RETRY_TIMES ) {
    Status = EFI_DEVICE_ERROR;
  }

  //
  // Clear status register
  //
  SetMem ((VOID *)(UINTN)LbaWriteAddress, 1, LPC_FLASH_TYPE2_CLEAR_STATUS_CMD);

  //
  // Issue read array command to return the FWH state machine to the normal op state
  //
  SetMem ((VOID *)(UINTN)((LbaWriteAddress & ~(0xFFFFF)) + 0xC0000), 1, LPC_FLASH_TYPE2_READ_ARRAY_CMD);

  return Status;
}


/**
  Flash device Erase() fuction for LPC Flash Type 2 devices

  @param FlashDevice             pointer to FLASH_DEVICE structure
  @param LbaWriteAddress         Starting address to be erased
  @param EraseBlockSize          Total size in bytes to be erased

  @retval EFI_SUCCESS            Flash ROM erased successfully
  @retval EFI_UNSUPPORTED        The flash device is not supported
  @retval EFI_INVALID_PARAMETER  Invalid function parameters

**/
EFI_STATUS
EFIAPI
LpcFlashType2Erase (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BlockDivQuotient  = 0;

  //
  // Check Erase Block size is supported
  //
  if ((EraseBlockSize % FLASH_BLOCK_SIZE) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  BlockDivQuotient = EraseBlockSize / FLASH_BLOCK_SIZE;
  //
  // erase loop
  //
  while (BlockDivQuotient > 0) {
    Status = LpcFlashType2EraseBlock (
               FlashDevice,
               LbaWriteAddress
               );
    ASSERT_EFI_ERROR (Status);
    LbaWriteAddress += FLASH_BLOCK_SIZE;
    BlockDivQuotient--;
  }

  return EFI_SUCCESS;
}


