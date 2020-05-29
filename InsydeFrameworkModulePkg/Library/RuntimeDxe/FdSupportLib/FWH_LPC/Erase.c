//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "FdSupportLib.h"
#include "FWH_LPC.h"

EFI_STATUS
EraseBlockMethodOne (
  IN  UINTN                     LbaWriteAddress,
  IN  UINT8                     EraseCmd
  )
/*++

Routine Description:

  Erase the flash device from LbaWriteAddress

Arguments:

  LbaWriteAddress               Erase Address

Returns:

  EFI status

--*/
{
  EFI_STATUS                    Status;
  UINT8                         Toggle1;
  UINT8                         Toggle2;
  UINTN                         TimeOut;

  Status = EFI_SUCCESS;

  //
  // erase commands
  //
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xAA);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort2A, 1, 0x55);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0x80);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xAA);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort2A, 1, 0x55);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)LbaWriteAddress, 1, EraseCmd);
  MEMORY_FENCE ();

  //
  // Wait for completion. Indicated by status register FVB_WSM_STATUS bit becoming 1
  //
  TimeOut = 0;
  do {
    Toggle1 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle1 = (UINT8)(Toggle1 & EFI_METHOD_ONE_DQ6);
      Stall (STALL_TIME);
    Toggle2 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle2 = (UINT8)(Toggle2 & EFI_METHOD_ONE_DQ6);
      if ((Toggle1 == Toggle2) && ((*(UINTN *)(UINTN)(LbaWriteAddress)) == ~(UINTN)1)) {
        break;
      }
      TimeOut++;
  } while (TimeOut <= EFI_METHOD_ONE_TIME_OUT);

  //
  // Commented out as it will have problems on Anacortes
  //
  if ( TimeOut >= FVB_MAX_RETRY_TIMES ) {
    Status = EFI_DEVICE_ERROR;
  }

  Stall (STALL_TIME);

  //
  //  Lock Block
  //
  return Status;
}

EFI_STATUS
EraseSectorMethodOne (
  IN  UINTN                     LbaWriteAddress,
  IN  UINT8                     EraseCmd
  )
/*++

Routine Description:

  Erase the flash device from LbaWriteAddress

Arguments:

  LbaWriteAddress               Erase Address

Returns:

  EFI status

--*/
{
  EFI_STATUS                    Status;
  UINT8                         Toggle1;
  UINT8                         Toggle2;
  UINTN                         TimeOut;

  Status = EFI_SUCCESS;

  MEMORY_FENCE ();

  //
  // Erase commands
  //
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xAA);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort2A, 1, 0x55);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0x80);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort55, 1, 0xAA);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)mMethodOne->CmdPort2A, 1, 0x55);
  MEMORY_FENCE ();
  EfiSetMem ((VOID *)LbaWriteAddress, 1, EraseCmd);
  MEMORY_FENCE ();

  //
  // Wait for completion. Indicated by status register FVB_WSM_STATUS bit becoming 1
  //
  TimeOut = 0;
  do {
      Toggle1 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle1 = (UINT8)(Toggle1 & EFI_METHOD_ONE_DQ6);
      Stall (STALL_TIME);
      Toggle2 = (*(UINT8*)(UINTN)(LbaWriteAddress));
      Toggle2 = (UINT8)(Toggle2 & EFI_METHOD_ONE_DQ6);
      if ( (Toggle1 == Toggle2) && ((*(UINT32*)(LbaWriteAddress)) == 0xFFFFFFFF) ) {
        break;
      }
      TimeOut++;
  } while (TimeOut <= EFI_METHOD_ONE_TIME_OUT);

  //
  // Commented out as it will have problems on Anacortes
  //
  if ( TimeOut >= FVB_MAX_RETRY_TIMES ) {
    Status = EFI_DEVICE_ERROR;
  }

  Stall (STALL_TIME);
  MEMORY_FENCE ();

  return Status;
}


EFI_STATUS
EraseBlockMethodTwo (
  IN  UINTN                     LbaWriteAddress
  )
/*++

Routine Description:

  Erase the flash device from LbaWriteAddress

Arguments:

  LbaWriteAddress -             Erase Address

Returns:

  EFI status

--*/
{
  EFI_STATUS                    Status;
  UINT8                         HubData;
  UINTN                         TimeOut;

  Status = EFI_SUCCESS;

  //
  // Send erase commands to FWH
  //
  EfiSetMem ((VOID *)(UINTN)LbaWriteAddress, 1, FWH_BLOCK_ERASE_SETUP_COMMAND);

  EfiSetMem ((VOID *)(UINTN)LbaWriteAddress, 1, FWH_BLOCK_ERASE_CONFIRM_COMMAND);

  EfiSetMem ((VOID *)(UINTN)LbaWriteAddress, 1, FWH_READ_STATUS_COMMAND);

  //
  // Wait for completion. Indicated by FWH_WRITE_STATE_STATUS bit becoming 0
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
    Stall (STALL_TIME);

    if (HubData & FWH_WRITE_STATE_STATUS) {
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
  EfiSetMem ((VOID *)(UINTN)LbaWriteAddress, 1, FWH_CLEAR_STATUS_COMMAND);

  //
  // Issue read array command to return the FWH state machine to the normal op state
  //
  EfiSetMem ((VOID *)(UINTN)((LbaWriteAddress & ~(0xFFFFF)) + 0xC0000), 1, FWH_READ_ARRAY_COMMAND);

  return Status;
}

EFI_STATUS
EraseMethodOne (
  IN  UINTN                                 LbaWriteAddress,
  IN  UINTN                                 EraseBlockSize
)
{
  UINTN                                     EraseSize;
  UINTN                                     EraseCount;
  UINT8                                     EraseCmd;
  UINTN                                     BlockDivQuotient  = 0;
  UINTN                                     BlockDivRemainder = 0;
  EFI_STATUS                                Status;
 
  //
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
    EraseCmd   = EFI_METHOD_ONE_BLOCK_CMD;
  } else {
    EraseSize  = FLASH_SECTOR_SIZE;
    EraseCount = EraseBlockSize / FLASH_SECTOR_SIZE ;   
    EraseCmd   = EFI_METHOD_ONE_SECTOR_CMD;
  }

  //
  // Erase loop
  //
  while (EraseCount > 0) {
    if (EraseSize == FLASH_BLOCK_SIZE) {
      Status = EraseBlockMethodOne (
                 LbaWriteAddress,
                 EraseCmd
                 );
    } else {
      Status = EraseSectorMethodOne (
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



EFI_STATUS
EraseMethodTwo (
  IN  UINTN                     LbaWriteAddress,
  IN  UINTN                     EraseBlockSize
  )
/*++

Routine Description:

Arguments:

  LbaWriteAddress -             Erase Address
  EraseBlockSize -              Block size

Returns:

  EFI status

--*/
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
    Status = EraseBlockMethodTwo (
               LbaWriteAddress
               ); 
    ASSERT_EFI_ERROR (Status);   
    LbaWriteAddress += FLASH_BLOCK_SIZE;
    BlockDivQuotient--;
  }

  return EFI_SUCCESS;
}

