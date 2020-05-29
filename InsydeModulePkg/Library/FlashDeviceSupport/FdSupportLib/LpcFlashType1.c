/** @file
  FdSupportLib implementation for LPC Flash Type 1 devices

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
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/FdSupportLib.h>
#include "LpcFlashSupport.h"

#ifdef MDE_CPU_ARM
#define MemoryFence()
#else
#include <Library/BaseLib.h>
#endif


/**
  Flash device Recognize() fuction for LPC Flash Type 1 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported

**/
EFI_STATUS
EFIAPI
LpcFlashType1Recognize (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  UINT16                                CheckId;
  UINTN                                 CmdPort55;
  UINTN                                 CmdPort2A;

  if (FlashDevice->DeviceType != LpcFlashType1) {
    return EFI_UNSUPPORTED;
  }
  
  CmdPort55 = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort55;
  CmdPort2A = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort2A;

  SetMem ((VOID *)CmdPort55, 1, 0xAA);
  MemoryFence ();
  SetMem ((VOID *)CmdPort2A, 1, 0x55);
  MemoryFence ();
  SetMem ((VOID *)CmdPort55, 1, 0x90);
  MemoryFence ();

  CheckId = *(UINT16 *)(((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->IdAddress);

  SetMem ((VOID *)CmdPort55, 1, LPC_FLASH_TYPE1_SOFT_ID_EXIT);
  MemoryFence ();

  if (CheckId == (UINT16)FlashDevice->DeviceInfo.Id) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}


/**
  Flash device Program() fuction for LPC Flash Type 1 devices

  @param FlashDevice            Pointer to FLASH_DEVICE structure
  @param Dest                   Target address to be programmed
  @param Src                    Souce buffer
  @param NumBytes               Total number of bytes to be programmed
  @param LbaWriteAddress        not used

  @retval EFI_SUCCESS           Data successfully written to flash device
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Failed to write the Flash ROM

**/
EFI_STATUS
EFIAPI
LpcFlashType1Program (
  IN FLASH_DEVICE               *FlashDevice,
  IN UINT8                      *Dest,
  IN UINT8                      *Src,
  IN UINTN                      *NumBytes,
  IN UINTN                      LbaWriteAddress
  )
{
  EFI_STATUS                    Status;
  UINTN                         CmdPort55;
  UINTN                         CmdPort2A;
  UINTN                         Count;
  UINT8                         Toggle1;
  UINT8                         Toggle2;
  UINTN                         TimeOut;

  if (FlashDevice->DeviceType != LpcFlashType1) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_SUCCESS;
  CmdPort55 = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort55;
  CmdPort2A = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort2A;

  for (Count = 0; Count < *NumBytes; Count++, Dest++, Src++) {
    if (CompareMem ((VOID *)(UINTN)Dest, (VOID *)(UINTN)Src, 1) != 0) {

      //
      // Program command
      //
      SetMem ((VOID *)CmdPort55, 1, 0xAA);
      MemoryFence ();

      SetMem ((VOID *)CmdPort2A, 1, 0x55);
      MemoryFence ();

      SetMem ((VOID *)CmdPort55, 1, 0xA0);
      MemoryFence ();

      //
      // Write Data to flash parts.
      //
      SetMem ((VOID *)Dest, 1, *Src);
      MemoryFence ();

      MicroSecondDelay (STALL_TIME);

      for (TimeOut = 0; TimeOut < LPC_FLASH_TYPE1_TIME_OUT; TimeOut++) {
        Toggle1 = (*(UINT8*)(UINTN)(Dest));
        MicroSecondDelay (STALL_TIME);
        Toggle2=(*(UINT8*)(UINTN)(Dest));

        if (Toggle1 == Toggle2) {
          break;
        }
        if (TimeOut >= (LPC_FLASH_TYPE1_TIME_OUT - 1)) {
          *NumBytes = Count;
          Status = EFI_DEVICE_ERROR;
          break;
        }
      }

      Toggle1 = (*(UINT8*)(UINTN)(Dest));
      Toggle2 = (*(UINT8*)(UINTN)(Src));
      if (Toggle1 != Toggle2) {
        *NumBytes = Count;
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
  }

  return Status;
}


/**
  Flash device EraseBlock() fuction for LPC Flash Type 1 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param LbaWriteAddress        Starting address to be erased
  @param EraseCmd               Erase command code

  @retval EFI_SUCCESS           Sector successfully erased
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Erase failed

**/
EFI_STATUS
EFIAPI
LpcFlashType1EraseBlock (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINT8                     EraseCmd
  )
{
  EFI_STATUS                    Status;
  UINTN                         CmdPort55;
  UINTN                         CmdPort2A;
  UINT8                         Toggle1;
  UINT8                         Toggle2;
  UINTN                         TimeOut;

  if (FlashDevice->DeviceType != LpcFlashType1) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_SUCCESS;
  CmdPort55 = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort55;
  CmdPort2A = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort2A;

  //
  // erase commands
  //
  SetMem ((VOID *)CmdPort55, 1, 0xAA);
  MemoryFence ();
  SetMem ((VOID *)CmdPort2A, 1, 0x55);
  MemoryFence ();
  SetMem ((VOID *)CmdPort55, 1, 0x80);
  MemoryFence ();
  SetMem ((VOID *)CmdPort55, 1, 0xAA);
  MemoryFence ();
  SetMem ((VOID *)CmdPort2A, 1, 0x55);
  MemoryFence ();
  SetMem ((VOID *)LbaWriteAddress, 1, EraseCmd);
  MemoryFence ();

  //
  // Wait for completion. Indicated by status register FVB_WSM_STATUS bit becoming 1
  //
  TimeOut = 0;
  do {
    Toggle1 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle1 = (UINT8)(Toggle1 & LPC_FLASH_TYPE1_DQ6);
      MicroSecondDelay (STALL_TIME);
    Toggle2 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle2 = (UINT8)(Toggle2 & LPC_FLASH_TYPE1_DQ6);
      if ((Toggle1 == Toggle2) && ((*(UINTN *)(UINTN)(LbaWriteAddress)) == ~(UINTN)1)) {
        break;
      }
      TimeOut++;
  } while (TimeOut <= LPC_FLASH_TYPE1_TIME_OUT);

  //
  // Commented out as it will have problems on Anacortes
  //
  if ( TimeOut >= FVB_MAX_RETRY_TIMES ) {
    Status = EFI_DEVICE_ERROR;
  }

  MicroSecondDelay (STALL_TIME);

  //
  //  Lock Block
  //
  return Status;
}


/**
  Flash device EraseSector() fuction for LPC Flash Type 1 devices

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param LbaWriteAddress        Starting address to be erased
  @param EraseCmd               Erase command code

  @retval EFI_SUCCESS           Sector successfully erased
  @retval EFI_UNSUPPORTED       The flash device is not supported
  @retval EFI_DEVICE_ERROR      Erase failed

**/
EFI_STATUS
EFIAPI
LpcFlashType1EraseSector (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINT8                     EraseCmd
  )
{
  EFI_STATUS                    Status;
  UINTN                         CmdPort55;
  UINTN                         CmdPort2A;
  UINT8                         Toggle1;
  UINT8                         Toggle2;
  UINTN                         TimeOut;

  if (FlashDevice->DeviceType != LpcFlashType1) {
    return EFI_UNSUPPORTED;
  }
  
  Status = EFI_SUCCESS;
  CmdPort55 = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort55;
  CmdPort2A = ((LPC_FLASH_DEVICE_INFO *)(FlashDevice->TypeSpecificInfo))->CmdPort2A;

  MemoryFence ();

  //
  // Erase commands
  //
  SetMem ((VOID *)CmdPort55, 1, 0xAA);
  MemoryFence ();
  SetMem ((VOID *)CmdPort2A, 1, 0x55);
  MemoryFence ();
  SetMem ((VOID *)CmdPort55, 1, 0x80);
  MemoryFence ();
  SetMem ((VOID *)CmdPort55, 1, 0xAA);
  MemoryFence ();
  SetMem ((VOID *)CmdPort2A, 1, 0x55);
  MemoryFence ();
  SetMem ((VOID *)LbaWriteAddress, 1, EraseCmd);
  MemoryFence ();

  //
  // Wait for completion. Indicated by status register FVB_WSM_STATUS bit becoming 1
  //
  TimeOut = 0;
  do {
      Toggle1 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle1 = (UINT8)(Toggle1 & LPC_FLASH_TYPE1_DQ6);
      MicroSecondDelay (STALL_TIME);
      Toggle2 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle2 = (UINT8)(Toggle2 & LPC_FLASH_TYPE1_DQ6);
      if ( (Toggle1 == Toggle2) && ((*(UINT32*)(LbaWriteAddress)) == 0xFFFFFFFF) ) {
        break;
      }
      TimeOut++;
  } while (TimeOut <= LPC_FLASH_TYPE1_TIME_OUT);

  //
  // Commented out as it will have problems on Anacortes
  //
  if ( TimeOut >= FVB_MAX_RETRY_TIMES ) {
    Status = EFI_DEVICE_ERROR;
  }

  MicroSecondDelay (STALL_TIME);
  MemoryFence ();

  return Status;
}

/**
  Flash device Erase() fuction for LPC Flash Type 1 devices

  @param FlashDevice             pointer to FLASH_DEVICE structure
  @param LbaWriteAddress         Starting address to be erased
  @param EraseBlockSize          Total size in bytes to be erased

  @retval EFI_SUCCESS            Flash ROM erased successfully
  @retval EFI_UNSUPPORTED        The flash device is not supported
  @retval EFI_INVALID_PARAMETER  Invalid function parameters

**/
EFI_STATUS
EFIAPI
LpcFlashType1Erase (
  IN FLASH_DEVICE               *FlashDevice,
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  )
{
  UINTN                         EraseSize;
  UINTN                         EraseCount;
  UINT8                         EraseCmd;
  UINTN                         BlockDivQuotient  = 0;
  UINTN                         BlockDivRemainder = 0;
  EFI_STATUS                    Status;

  if (FlashDevice->DeviceType != LpcFlashType1) {
    return EFI_UNSUPPORTED;
  }

  // Check Erase Block size is supported
  //
  if (((EraseBlockSize % FLASH_SECTOR_SIZE) != 0) || (EraseBlockSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  BlockDivQuotient  = EraseBlockSize / FLASH_BLOCK_SIZE;
  BlockDivRemainder = EraseBlockSize % FLASH_BLOCK_SIZE;
  if ((BlockDivQuotient > 0) && (BlockDivRemainder == 0)) {
    EraseSize  = FLASH_BLOCK_SIZE;
    EraseCount = BlockDivQuotient;
    EraseCmd   = LPC_FLASH_TYPE1_BLOCK_CMD;
  } else {
    EraseSize  = FLASH_SECTOR_SIZE;
    EraseCount = EraseBlockSize / FLASH_SECTOR_SIZE ;
    EraseCmd   = LPC_FLASH_TYPE1_SECTOR_CMD;
  }

  //
  // Erase loop
  //
  while (EraseCount > 0) {
    if (EraseSize == FLASH_BLOCK_SIZE) {
      Status = LpcFlashType1EraseBlock (
                 FlashDevice,
                 LbaWriteAddress,
                 EraseCmd
                 );
    } else {
      Status = LpcFlashType1EraseSector (
                 FlashDevice,
                 LbaWriteAddress,
                 EraseCmd
                 );
    }
    ASSERT_EFI_ERROR (Status);
    LbaWriteAddress += EraseSize;
    EraseCount--;
  }

  return EFI_SUCCESS;
}