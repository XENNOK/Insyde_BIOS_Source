/** @file
 Internal functions to support fault tolerant write.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include "FtwLite.h"

extern EFI_GUID        gEfiAlternateFvBlockGuid;
/**
  Check whether a flash buffer is erased.

  @param  Polarity    All 1 or all 0.
  @param  Buffer      Buffer to check.
  @param  BufferSize  Size of the buffer.

  @retval TRUE        This is an erased buffer.
  @retval FALSE       This isn't an erased buffer.

**/
BOOLEAN
IsErasedFlashBuffer (
  IN BOOLEAN         Polarity,
  IN UINT8           *Buffer,
  IN UINTN           BufferSize
  )
{
  UINT8 ErasedValue;
  UINT8 *Ptr;

  if (Polarity) {
    ErasedValue = 0xFF;
  } else {
    ErasedValue = 0;
  }

  Ptr = Buffer;
  while (BufferSize--) {
    if (*Ptr++ != ErasedValue) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
  To Erase one block. The size is FTW_BLOCK_SIZE.

  @param  FtwLiteDevice  Calling context
  @param  FvBlock        FVB Protocol interface
  @param  FvBaseAddr     The Firmware Volume BaseAddress of target block.
  @param  Lba            Lba of the firmware block

  @retval EFI_SUCCESS    Block LBA is Erased successfully.
  @retval EFI_ABORTED    Error occurs.

**/
EFI_STATUS
FtwEraseBlock (
  IN EFI_FTW_LITE_DEVICE              *FtwLiteDevice,
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  IN EFI_PHYSICAL_ADDRESS             FvBaseAddr,
  EFI_LBA                             Lba
  )
{
  EFI_STATUS        Status;
  UINTN             TryErasetimes;
  BOOLEAN           EraseSuccess;
  UINTN             TotalLength;
  UINT8             *Buffer;
  UINTN             DataOffset;
  UINTN             BlockLength;
  UINTN             Index;
  UINT8             *Ptr;
  UINTN             NumBytes;
  UINTN             Length;

  //
  // allocate pool for read data from block
  //
  TotalLength = FtwLiteDevice->SpareAreaLength;
  Buffer = FtwAllocateZeroBuffer (FtwLiteDevice, TotalLength);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Start try to erase whole block and using do-while to make sure
  // at lease erase once.
  //
  EraseSuccess = FALSE;
  TryErasetimes = 0;
  Status = EFI_SUCCESS;
  do {
    //
    //Erase whole block
    //
    Status = EFI_SUCCESS;
    if (mSmst == NULL) {
      Status = FvBlock->EraseBlocks (
                          FvBlock,
                          Lba,
                          FtwLiteDevice->NumberOfSpareBlock,
                          EFI_LBA_LIST_TERMINATOR
                          );

    } else {
      NumBytes = FtwLiteDevice->SpareAreaLength;
      Status = FtwLiteDevice->SmmFwbServices->EraseBlocks (
                                FtwLiteDevice->SmmFwbServices,
                                (UINTN)(FvBaseAddr + (UINTN)Lba * FtwLiteDevice->SizeOfSpareBlock),
                                &NumBytes
                                );
    }
    if (EFI_ERROR (Status)) {
      FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
      return Status;
    }

    if (mSmst == NULL) {
      //
      // Read whole block
      //
      Ptr = Buffer;
      for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index++) {
        BlockLength = FtwLiteDevice->SizeOfSpareBlock;
        Status = FtwLiteDevice->FtwBackupFvb->Read (
                                                FvBlock,
                                                Lba + Index,
                                                0,
                                                &BlockLength,
                                                Ptr
                                                );
        if (EFI_ERROR (Status)) {
          DEBUG ((mFtwLiteError, "FtwLite: FVB Read spare block - %r\n", Status));
          FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
          return Status;
        }
        Ptr += BlockLength;
      }
    } else {
      Length = FtwLiteDevice->SizeOfSpareBlock * FtwLiteDevice->NumberOfSpareBlock;
      CopyMem (
        Buffer,
        (UINT8 *) (UINTN) (FvBaseAddr + ((UINTN) Lba * FtwLiteDevice->SizeOfSpareBlock)),
        Length
        );
    }
    //
    //Set Erase success as default
    //
    EraseSuccess = TRUE;
    for (DataOffset = 0; DataOffset < TotalLength; DataOffset++) {
      //
      // Find any data isn't 0xff indicates erase failed
      //
      if (Buffer[DataOffset] != 0xff) {
        EraseSuccess = FALSE;
        break;
      }
    }
    TryErasetimes++;
  } while (!EraseSuccess && TryErasetimes < FTW_MAX_TRY_ACCESS_FVB_TIMES);

  FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
  if (!EraseSuccess) {
    Status = EFI_ABORTED;
  }

  return Status;
}

/**
  Function uses to erase whole spare block.

  @param  FtwLiteDevice  Calling context

  @retval EFI_SUCCESS    Whole spare block is Erased successfully.
  @retval EFI_ABORTED    Error occurs.

**/
EFI_STATUS
FtwEraseSpareBlock (
  IN EFI_FTW_LITE_DEVICE   *FtwLiteDevice
  )
{

  return FtwEraseBlock (
                 FtwLiteDevice,
                 FtwLiteDevice->FtwBackupFvb,
                 FtwLiteDevice->SpareAreaFvBaseAddr,
                 FtwLiteDevice->FtwSpareLba
                );
}


/**
  Get firmware block by address.

  @param  FvBlockHandle   Address specified the block.
  @param  FvBlock         The block caller wanted.

  @retval EFI_SUCCESS     Get Fvb protocol successful.
  @retval EFI_NOT_FOUND   Block not found

**/
EFI_STATUS
GetFvbByAddress (
  IN  EFI_PHYSICAL_ADDRESS               Address,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL **FvBlock
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          *HandleBuffer;
  UINTN                               HandleCount;
  UINTN                               Index;
  EFI_PHYSICAL_ADDRESS                FvbBaseAddress;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;

  *FvBlock = NULL;
  //
  // Locate all handles of Fvb protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  //
  // Search all FVB until find the right one
  //
  for (Index = 0; Index < HandleCount; Index += 1) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    (VOID **) &Fvb
                    );
    if (EFI_ERROR (Status)) {
      Status = EFI_NOT_FOUND;

      break;
    }
    //
    // Compare the address and select the right one
    //
    Status = Fvb->GetPhysicalAddress (Fvb, &FvbBaseAddress);
    if (EFI_ERROR (Status)) {

      continue;
    }

    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) FvbBaseAddress);
    if ((Address >= FvbBaseAddress) && (Address <= (FvbBaseAddress + (FwVolHeader->FvLength - 1))) && IsNvStorageHandle (HandleBuffer[Index])) {
      *FvBlock  = Fvb;
      Status    = EFI_SUCCESS;

      break;
    }
  }

  gBS->FreePool (HandleBuffer);

  return Status;
}

/**
  According physical address or LBA to check this address is whether
  located in working block.

  @param  FtwLiteDevice   Calling context.
  @param  FvBlock         Fvb protocol instance.
  @param  Lba             input LBA for proteced mode use.
  @param  Address         input target address for SMM mode use.

  @retval TRUE            This address or LBA is located in working block
  @retval FALSE           This address or LBA isn't located in working block

**/
BOOLEAN
IsInWorkingBlock (
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  EFI_LBA                             Lba,
  EFI_PHYSICAL_ADDRESS                Address
  )
{
  //
  // If matching the following condition, the target block is in working block.
  // 1. Target block is on the FV of working block (Using the same FVB protocol instance).
  // 2. Lba falls into the range of working block.
  //
  if (mSmst == NULL) {
    return (BOOLEAN)
      (
        (FvBlock == FtwLiteDevice->FtwFvBlock) &&
        (Lba >= FtwLiteDevice->FtwWorkBlockLba) &&
        (Lba <= FtwLiteDevice->FtwWorkSpaceLba)
      );
  } else {
    return (BOOLEAN)
      (
        (Address >= FtwLiteDevice->WorkBlockAddr) &&
        (Address <= FtwLiteDevice->WorkSpaceAddress)
      );
  }
}

/**
  Copy the content of spare block to a target block. Size is FTW_BLOCK_SIZE. Spare block
  is accessed by FTW backup FVB protocol interface. LBA is FtwLiteDevice->FtwSpareLba.
  Target block is accessed by FvBlock protocol interface. LBA is Lba.

  @param  FtwLiteDevice   The private data of FTW_LITE driver.
  @param  FvBlock         FVB Protocol interface to access target block.
  @param  FvBaseAddr      Frimware volume base address.
  @param  Lba             Lba of the target block.

  @retval EFI_SUCCESS            Spare block content is copied to target block.
  @retval EFI_INVALID_PARAMETER  Input parameter error.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory error.
  @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
FlushSpareBlockToTargetBlock (
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  IN EFI_PHYSICAL_ADDRESS             FvBaseAddr,
  EFI_LBA                             Lba
  )
{
  EFI_STATUS                    Status;
  UINTN                         Length;
  UINT8                         *Buffer;
  UINTN                         Count;
  UINT8                         *Ptr;
  UINTN                         Index;

  if ((FtwLiteDevice == NULL) || (FvBlock == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Allocate a memory buffer
  //
  Length  = FtwLiteDevice->SpareAreaLength;
  Buffer = FtwAllocateZeroBuffer (FtwLiteDevice, Length);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Read all content of spare block to memory buffer
  //
  Ptr = Buffer;
  for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index += 1) {
    Count = FtwLiteDevice->SizeOfSpareBlock;
    Status = FtwLiteDevice->FtwBackupFvb->Read (
                                            FtwLiteDevice->FtwBackupFvb,
                                            FtwLiteDevice->FtwSpareLba + Index,
                                            0,
                                            &Count,
                                            Ptr
                                            );
    if (EFI_ERROR (Status)) {
      FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
      return Status;
    }

    Ptr += Count;
  }
  //
  // Write memory buffer to target block
  //
  Status = WriteTargetBlock (FtwLiteDevice, FvBlock, FvBaseAddr, Lba, Buffer, Length, FTW_MAX_TRY_ACCESS_FVB_TIMES);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
    return EFI_ABORTED;
  }

  FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);

  return Status;
}

/**
  Copy the content of spare block to working block. Size is FTW_BLOCK_SIZE. Spare block is
  accessed by FTW backup FVB protocol interface. LBA is FtwLiteDevice->FtwSpareLba. Working
  block is accessed by FTW working FVB protocol interface. LBA is FtwLiteDevice->FtwWorkBlockLba.

  @param  FtwLiteDevice          The private data of FTW_LITE driver.

  @retval EFI_SUCCESS            Spare block content is copied to target block.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory error.
  @retval EFI_ABORTED            The function could not complete successfully.

Notes:
    Since the working block header is important when FTW initializes, the state of the operation
    should be handled carefully. The Crc value is calculated without STATE element.

**/
EFI_STATUS
FlushSpareBlockToWorkingBlock (
  EFI_FTW_LITE_DEVICE          *FtwLiteDevice
  )
{
  EFI_STATUS                              Status;
  UINTN                                   Length;
  UINT8                                   *Buffer;
  EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *WorkingBlockHeader;
  EFI_LBA                                 WorkSpaceLbaOffset;
  UINTN                                   WorkSpaceOffset;
  VARIABLE_STORE_HEADER                   *VariableHeader;
  ECP_VARIABLE_STORE_HEADER               *EcpVariableHeader;
  UINTN                                   FvHeaderSize;
  BOOLEAN                                 IsVariableState;
  UINTN                                   VariableStateOffset;
  //
  // Allocate a memory buffer
  //
  Length  = FtwLiteDevice->SpareAreaLength;
  Buffer = FtwAllocateZeroBuffer (FtwLiteDevice, Length);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // To  guarantee that the WorkingBlockValid is set on spare block
  //
  WorkSpaceLbaOffset = FtwLiteDevice->FtwWorkSpaceLba - FtwLiteDevice->FtwWorkBlockLba;
  WorkSpaceOffset = (UINTN)(FtwLiteDevice->WorkSpaceAddress - FtwLiteDevice->WorkBlockAddr);
  Status = FtwUpdateFvState (
             FtwLiteDevice,
             FtwLiteDevice->FtwBackupFvb,
             FtwLiteDevice->FtwSpareLba + WorkSpaceLbaOffset,
             FtwLiteDevice->FtwWorkSpaceBase + sizeof (EFI_GUID) + sizeof (UINT32),
             (UINTN)(FtwLiteDevice->SpareAreaAddress + WorkSpaceOffset) + (UINTN)(sizeof (EFI_GUID) + sizeof (UINT32)),
             WORKING_BLOCK_VALID
             );

  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
    return EFI_ABORTED;
  }

  //
  // Read from spare block to memory buffer
  //
  Status = FtwLiteReadSpareBlock (FtwLiteDevice, Buffer);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
    return Status;
  }
  //
  // Clear the CRC and STATE, copy data from spare to working block.
  //
  WorkingBlockHeader = (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *) (Buffer + (UINTN) WorkSpaceLbaOffset * FtwLiteDevice->SizeOfSpareBlock + FtwLiteDevice->FtwWorkSpaceBase);
  InitWorkSpaceHeader (WorkingBlockHeader);
  WorkingBlockHeader->WorkingBlockValid   = FTW_ERASE_POLARITY;
  WorkingBlockHeader->WorkingBlockInvalid = FTW_ERASE_POLARITY;

  //
  // target block is working block, then
  //   Set WorkingBlockInvalid in EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER
  //   before erase the working block.
  //
  //  Offset = EFI_FIELD_OFFSET(EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER,
  //                            WorkingBlockInvalid);
  // To skip Signature and Crc: sizeof(EFI_GUID)+sizeof(UINT32).
  //
  Status = FtwUpdateFvState (
            FtwLiteDevice,
            FtwLiteDevice->FtwFvBlock,
            FtwLiteDevice->FtwWorkSpaceLba,
            FtwLiteDevice->FtwWorkSpaceBase + sizeof (EFI_GUID) + sizeof (UINT32),
            (UINTN) (FtwLiteDevice->WorkSpaceAddress) + (UINTN) (sizeof (EFI_GUID) + sizeof (UINT32)),
            WORKING_BLOCK_INVALID
            );
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
    return EFI_ABORTED;
  }

  FtwLiteDevice->FtwWorkSpaceHeader->WorkingBlockInvalid = FTW_VALID_STATE;
  //
  // Use the state of VARIABLE_STORE_HEADER to check the variable is health or not.
  // Setting the state of VARIABLE_STORE_HEADER to VARIABLE_STORE_HEALTHY, after
  // whole data has been written to NV storage.
  //
  FvHeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY);
  //
  // Clear the state of VARIABLE_STORE_HEADER
  //
  IsVariableState = FALSE;
  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    EcpVariableHeader = (ECP_VARIABLE_STORE_HEADER *) (Buffer + FvHeaderSize);
    if (EcpVariableHeader->State == VARIABLE_STORE_HEALTHY) {
      IsVariableState = TRUE;
      EcpVariableHeader->State = 0xFF;
    }
  } else {
    VariableHeader = (VARIABLE_STORE_HEADER *) (Buffer + FvHeaderSize);
    if (VariableHeader->State == VARIABLE_STORE_HEALTHY) {
      IsVariableState = TRUE;
      VariableHeader->State = 0xFF;
    }
  }
  //
  // Write memory buffer to working block
  //
  Status = FtwWriteWorkingBlock (FtwLiteDevice, Buffer, Length, FTW_MAX_TRY_ACCESS_FVB_TIMES);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
//    while (TRUE);
    return EFI_ABORTED;
  }
  //
  // Since the memory buffer will not be used, free memory Buffer.
  //
  FtwFreePool (FtwLiteDevice, (VOID **)&Buffer);
  //
  // update the state of VARIABLE_STORE_HEADER to VARIABLE_STORE_HEALTHY
  //
  if (IsVariableState) {
    if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
      VariableStateOffset = FvHeaderSize + sizeof (UINT32) + sizeof (UINT32) + sizeof (UINT8);
      Status = FtwUpdateFvState (
                 FtwLiteDevice,
                 FtwLiteDevice->FtwFvBlock,
                 FtwLiteDevice->FtwWorkBlockLba,
                 VariableStateOffset,
                 (UINTN) FtwLiteDevice->WorkBlockAddr + VariableStateOffset,
                 0x01
                 );
    } else {
      VariableStateOffset = FvHeaderSize + sizeof (EFI_GUID) + sizeof (UINT32) + sizeof (UINT8);
      Status = FtwUpdateFvState (
                 FtwLiteDevice,
                 FtwLiteDevice->FtwFvBlock,
                 FtwLiteDevice->FtwWorkBlockLba,
                 VariableStateOffset,
                 (UINTN) FtwLiteDevice->WorkBlockAddr + VariableStateOffset,
                 0x01
                 );
    }
  }
  //
  // Update the VALID of the working block
  //
  // Offset = EFI_FIELD_OFFSET(EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER,
  //                           WorkingBlockValid);
  // Hardcode offset sizeof(EFI_GUID)+sizeof(UINT32), to skip Signature and Crc
  //
  Status = FtwUpdateFvState (
            FtwLiteDevice,
            FtwLiteDevice->FtwFvBlock,
            FtwLiteDevice->FtwWorkSpaceLba,
            FtwLiteDevice->FtwWorkSpaceBase + sizeof (EFI_GUID) + sizeof (UINT32),
            (UINTN) (FtwLiteDevice->WorkSpaceAddress) + (UINTN) (sizeof (EFI_GUID) + sizeof (UINT32)),
            WORKING_BLOCK_VALID
            );
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  FtwLiteDevice->FtwWorkSpaceHeader->WorkingBlockValid = FTW_VALID_STATE;

  return EFI_SUCCESS;
}

/**
  Write spare block and check the data which write to spare block is correct.

  @param  FtwLiteDevice          Calling context.
  @param  WriteBuffer            Write Buffer.
  @param  WriteLength            The size of write data.
  @param  TryWriteTimes          The maximum trying to write times.

  @retval EFI_SUCCESS            Write data to spare block success.
  @retval EFI_OUT_OF_RESOURCE    The input value is invalid.
  @retval EFI_ABORTED            Write data to spare block failed.

**/
EFI_STATUS
FtwWriteSpareBlock (
  IN EFI_FTW_LITE_DEVICE   *FtwLiteDevice,
  IN UINT8                 *WriteBuffer,
  IN UINTN                 WriteLength,
  IN UINTN                 MaxTryWriteTimes
  )
{
  EFI_STATUS                          Status;

  //
  // Check the input value is valid
  //
  if ((WriteBuffer == NULL) || (WriteLength != FtwLiteDevice->SpareAreaLength)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Write data to whole target block
  //
  Status = WriteTargetBlock (
                      FtwLiteDevice,
                      FtwLiteDevice->FtwBackupFvb,
                      FtwLiteDevice->SpareAreaFvBaseAddr,
                      FtwLiteDevice->FtwSpareLba,
                      WriteBuffer,
                      WriteLength,
                      MaxTryWriteTimes
                     );
   if (EFI_ERROR (Status)) {
     return Status;
   }

  return EFI_SUCCESS;
}

/**
  Write working block and check the data which write to working block is correct.

  @param  FtwLiteDevice          Calling context.
  @param  WriteBuffer            Write Buffer.
  @param  WriteLength            The size of write data.
  @param  TryWriteTimes          The maximum trying to write times.

  @retval EFI_SUCCESS            Write data to working block success.
  @retval EFI_OUT_OF_RESOURCE    Allocate memory error.
  @retval EFI_ABORTED            Erase, write, or read error.
  @retval EFI_DEVICE_ERROR       Cannont write data to working block.
  @retval EFI_INVALID_PARAMETER  Any input value is invalid

**/
EFI_STATUS
FtwWriteWorkingBlock (
  IN EFI_FTW_LITE_DEVICE   *FtwLiteDevice,
  IN UINT8                 *WriteBuffer,
  IN UINTN                 WriteLength,
  IN UINTN                 MaxTryWriteTimes
  )
{
  EFI_STATUS                          Status;

  //
  // Check the input value is valid
  //
  if ((WriteBuffer == NULL) || (WriteLength != FtwLiteDevice->SpareAreaLength)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Write data to whole target block
  //
  Status = WriteTargetBlock (
                      FtwLiteDevice,
                      FtwLiteDevice->FtwFvBlock,
                      FtwLiteDevice->WorkSpaceFvBaseAddr,
                      FtwLiteDevice->FtwWorkBlockLba,
                      WriteBuffer,
                      WriteLength,
                      MaxTryWriteTimes
                     );

   if (EFI_ERROR (Status)) {
     return Status;
   }

  return EFI_SUCCESS;
}

/**
  Write target block and check the data which write to target block is correct.

  @param  FtwLiteDevice          Calling context.
  @param  FvBlock                FVB Protocol interface to access target block.
  @param  FvBaseAddr             The Firmware Volume BaseAddress of target block.
  @param  StartLba               Lba of the target block.
  @param  WriteBuffer            Write Buffer.
  @param  WriteLength            The size of write data.
  @param  MaxTryWriteTimes       The maximum trying to write times.

  @retval EFI_SUCCESS            Write data to working block success.
  @retval EFI_OUT_OF_RESOURCE    Allocate memory error.
  @retval EFI_ABORTED            Erase, write, or read error.
  @retval EFI_DEVICE_ERROR       Cannont write data to spare block.
  @retval EFI_INVALID_PARAMETER  Any input value is invalid.

**/
EFI_STATUS
WriteTargetBlock (
  IN EFI_FTW_LITE_DEVICE                     *FtwLiteDevice,
  IN EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *FvBlock,
  IN EFI_PHYSICAL_ADDRESS                    FvBaseAddr,
  IN EFI_LBA                                 StartLba,
  IN UINT8                                   *WriteBuffer,
  IN UINTN                                   WriteLength,
  IN UINTN                                   MaxTryWriteTimes
  )
{
  EFI_STATUS                          Status;
  UINTN                               TryWritetimes;
  BOOLEAN                             WriteSuccess;
  UINTN                               BlockLength;
  UINT8                               *ReadBuffer;
  UINTN                               Index;
  UINT8                               *Ptr;
  UINTN                               Length;

  //
  // Check the input value is valid
  //
  if ((WriteBuffer == NULL) || (WriteLength != FtwLiteDevice->SpareAreaLength)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Allocate pool for ReadBuffer
  //
  ReadBuffer = FtwAllocateZeroBuffer (FtwLiteDevice, WriteLength);
  if (ReadBuffer == NULL) {
    return EFI_ABORTED;
  }

  WriteSuccess = FALSE;
  TryWritetimes = 0;
  //
  // Start try to write memory data to target block and use do-while to make
  // sure at least write data once
  //
  do {
    //
    // Erase working block
    //
    Status = FtwEraseBlock (
              FtwLiteDevice,
              FvBlock,
              FvBaseAddr,
              StartLba
              );
    if (EFI_ERROR (Status)) {
      FtwFreePool (FtwLiteDevice, (VOID **)&ReadBuffer);
      return EFI_ABORTED;
    }

    Ptr = WriteBuffer;
    //
    // Write memory buffer to target block
    //
    for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index += 1) {
      BlockLength = FtwLiteDevice->SizeOfSpareBlock;
      Status = EFI_SUCCESS;
      if (mSmst == NULL) {
        Status = FtwLiteDevice->FtwBackupFvb->Write (
                                                FvBlock,
                                                StartLba + Index,
                                                0,
                                                &BlockLength,
                                                Ptr
                                                );
      } else {
        Status = FtwLiteDevice->SmmFwbServices->Write (
                                                  FtwLiteDevice->SmmFwbServices,
                                                  (UINTN)(FvBaseAddr + (UINTN)(StartLba + Index) * FtwLiteDevice->SizeOfSpareBlock),
                                                  &BlockLength,
                                                  Ptr
                                                  );
      }
      if (EFI_ERROR (Status)) {
        FtwFreePool (FtwLiteDevice, (VOID **)&ReadBuffer);
        return Status;
      }

      Ptr += BlockLength;
    }
    //
    // Read target block from NV storage
    //
    if (mSmst == NULL) {
      Ptr = ReadBuffer;
      for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index += 1) {
        BlockLength = FtwLiteDevice->SizeOfSpareBlock;
        Status = FtwLiteDevice->FtwBackupFvb->Read (
                                                FvBlock,
                                                StartLba + Index,
                                                0,
                                                &BlockLength,
                                                Ptr
                                                );
        if (EFI_ERROR (Status)) {
          FtwFreePool (FtwLiteDevice, (VOID **)&ReadBuffer);
          return Status;
        }
        Ptr += BlockLength;
      }
    } else {
      Length = FtwLiteDevice->SizeOfSpareBlock * FtwLiteDevice->NumberOfSpareBlock;
      CopyMem (
        ReadBuffer,
        (UINT8 *) (UINTN) (FvBaseAddr + (UINTN) StartLba * FtwLiteDevice->SizeOfSpareBlock),
        Length
        );
    }
    //
    // Check write data is whther correct
    //
    WriteSuccess = (CompareMem (ReadBuffer, WriteBuffer, WriteLength) == 0) ? TRUE : FALSE;
    TryWritetimes++;
  } while (!WriteSuccess && TryWritetimes < MaxTryWriteTimes);

  FtwFreePool (FtwLiteDevice, (VOID **)&ReadBuffer);
  if (!WriteSuccess) {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Read all data from whole work block.

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Buffer                 Buffer to save work block data.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_ABORTED            The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER  Some input parameter is invalid.

**/
EFI_STATUS
FtwLiteReadWorkBlock (
  IN  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  OUT UINT8                               *Buffer
  )
{
  UINTN                  Index;
  UINTN                  Length;
  EFI_STATUS             Status;

  if (FtwLiteDevice == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (mSmst == NULL) {
    for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index++) {
      Length = FtwLiteDevice->SizeOfSpareBlock;
      Status = FtwLiteDevice->FtwFvBlock->Read (
                                            FtwLiteDevice->FtwFvBlock,
                                            FtwLiteDevice->FtwWorkBlockLba + Index,
                                            0,
                                            &Length,
                                            Buffer
                                            );
      if (EFI_ERROR (Status)) {
        break;
      }
      Buffer += Length;
    }
  } else {
    Length = FtwLiteDevice->SizeOfSpareBlock * FtwLiteDevice->NumberOfSpareBlock;
    CopyMem (
      Buffer,
      (UINT8 *) ((UINTN) FtwLiteDevice->WorkBlockAddr - ((UINTN) FtwLiteDevice->FtwWorkBlockLba * FtwLiteDevice->SizeOfSpareBlock)),
      Length
      );
  }
  return Status;
}

/**
  Read all data from whole work block.

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Buffer                 Buffer to save work block data.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_ABORTED            The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER  Some input parameter is invalid.

**/
EFI_STATUS
FtwLiteReadSpareBlock (
  IN  EFI_FTW_LITE_DEVICE                     *FtwLiteDevice,
  OUT UINT8                                   *Buffer
  )
{
  UINTN                  Index;
  UINTN                  Length;
  EFI_STATUS             Status;

  if (FtwLiteDevice == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (mSmst == NULL) {
    for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index++) {
      Length = FtwLiteDevice->SizeOfSpareBlock;
      Status = FtwLiteDevice->FtwFvBlock->Read (
                                            FtwLiteDevice->FtwBackupFvb,
                                            FtwLiteDevice->FtwSpareLba + Index,
                                            0,
                                            &Length,
                                            Buffer
                                            );
      if (EFI_ERROR (Status)) {
        break;
      }
      Buffer += Length;
    }
  } else {
    Length = FtwLiteDevice->SizeOfSpareBlock * FtwLiteDevice->NumberOfSpareBlock;
    CopyMem (
      Buffer,
      (UINT8 *) (UINTN) FtwLiteDevice->SpareAreaAddress,
      Length
      );
  }
  return Status;
}

/**

  This function uses to allocate runtimeservicesdata memory in protected mode before before exit
  boot services event.

  @param  Size   The size of buffer to allocate.

  @retval NULL   Buffer unsuccessfully allocated.
  @return Buffer Successfully allocated.

**/
VOID *
AllocateZeroRuntimeDataBuffer (
  IN UINTN                  Size
  )
{
  VOID         *Buffer;
  EFI_STATUS   Status;

  Buffer = NULL;
  if (mSmst == NULL) {
    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    Size,
                    &Buffer
                    );
    if (EFI_ERROR (Status)) {
      Buffer = NULL;
    }
  }

  if (Buffer != NULL) {
    ZeroMem (Buffer, Size);
  }

  return Buffer;
}

/**
  According to system mode to allocate pool. Allocate pool from pre-allocate RuntimeServicesData in protect mode.
  Allocate EfiRuntimeServicesData (actually this memory is located in the range of SMM RAM)
  in SMM mode.

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Size                   The size of buffer to allocate.

  @retval NULL                   Allocate buffer failed.
  @return EFI_INVALID_PARAMETER  Start address of allocated buffer.

**/
VOID *
FtwAllocateZeroBuffer (
  IN EFI_FTW_LITE_DEVICE    *FtwLiteDevice,
  IN UINTN                  Size
  )
{
  UINTN        Index;
  VOID         *Buffer;
  EFI_STATUS   Status;

  Buffer = NULL;
  if (mSmst == NULL) {
    for (Index = 0; Index < MAX_MEMORY_NODE; Index++) {
      if (FtwLiteDevice->ReclaimMemory[Index].BufferSize >= Size && !FtwLiteDevice->ReclaimMemory[Index].Used) {
        FtwLiteDevice->ReclaimMemory[Index].Used = TRUE;
        Buffer = FtwLiteDevice->ReclaimMemory[Index].Buffer;
        break;
      }
    }
  } else {
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Size,
                      &Buffer
                      );
    if (EFI_ERROR (Status)) {
      Buffer = NULL;
    }
  }

  if (Buffer != NULL) {
    ZeroMem (Buffer, Size);
  }
  return Buffer;
}

/**
  This function uses to free memory which allocated by FtwAllocateZeroBuffer ()

  @param  FtwLiteDevice          Point to private data of FTW driver.
  @param  Buffer                 [in]:Double pointer saved the pointer of buffer address which want to free
                                 [out]:Double pointer Saved NULL pointer to indicate allocated memory is freed.

  @retval EFI_SUCCESS            Memory return to system
  @return EFI_INVALID_PARAMETER  Buffer was invalid.

**/
EFI_STATUS
FtwFreePool (
  IN     EFI_FTW_LITE_DEVICE    *FtwLiteDevice,
  IN OUT VOID                   **Buffer
  )
{
  UINTN          Index;

  if (Buffer == NULL || *Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmst == NULL) {
    for (Index = 0; Index < MAX_MEMORY_NODE; Index++) {
      if (FtwLiteDevice->ReclaimMemory[Index].Buffer == *Buffer) {
        FtwLiteDevice->ReclaimMemory[Index].Used = FALSE;
        break;
      }
    }
    *Buffer = NULL;
  } else {
    EFI_FREE_POOL (*Buffer);
  }

  return EFI_SUCCESS;
}

/**
  Check this input handle is whether a NV store FVB handle.

  @param[in] Handle    Input EFI_HANDLE instance

  @retval TRUE         This is NV storage FVB handle.
  @retval FALSE        This isn't NV storage FVB handle.
**/
BOOLEAN
IsNvStorageHandle (
  EFI_HANDLE      Handle
  )
{
  EFI_STATUS   Status;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiAlternateFvBlockGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  return (BOOLEAN) (Status == EFI_SUCCESS);
}
