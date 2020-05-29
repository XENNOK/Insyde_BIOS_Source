/** @file

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

#include "FtwLite.h"

/**
  Check to see if it is a valid work space.

  @param  WorkingHeader    Pointer of working block header.

  @retval EFI_SUCCESS      The function completed successfully.
  @retval EFI_ABORTED      The function could not complete successfully.

**/
BOOLEAN
IsValidWorkSpace (
  IN EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *WorkingHeader
  )
{
  EFI_STATUS                              Status;
  EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER WorkingBlockHeader;

  ASSERT (WorkingHeader != NULL);
  if (WorkingHeader->WorkingBlockValid != FTW_VALID_STATE) {
    return FALSE;
  }
  //
  // Check signature with gEfiSystemNvDataFvGuid
  //
  if (!CompareGuid (&gEfiSystemNvDataFvGuid, &WorkingHeader->Signature)) {
    return FALSE;
  }
  //
  // Check the CRC of header
  //
  CopyMem (
    &WorkingBlockHeader,
    WorkingHeader,
    sizeof (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER)
    );

  //
  // Filter out the Crc and State fields
  //
  SetMem (&WorkingBlockHeader.Crc, sizeof (UINT32), FTW_ERASED_BYTE);
  WorkingBlockHeader.WorkingBlockValid    = FTW_ERASE_POLARITY;
  WorkingBlockHeader.WorkingBlockInvalid  = FTW_ERASE_POLARITY;

  //
  // Calculate the Crc of woking block header
  //
  Status = RuntimeDriverCalculateCrc32 (
                  (UINT8 *) &WorkingBlockHeader,
                  sizeof (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER),
                  &WorkingBlockHeader.Crc
                  );


  if (WorkingBlockHeader.Crc != WorkingHeader->Crc) {
    DEBUG ((mFtwLiteError, "FtwLite: Work block header CRC check error\n"));
    return FALSE;
  }

  return TRUE;
}

/**
  Initialize a work space when there is no work space.

  @param  WorkingHeader    Pointer of working block header.

  @retval EFI_SUCCESS      The function completed successfully.
  @retval EFI_ABORTED      The function could not complete successfully.

**/
EFI_STATUS
InitWorkSpaceHeader (
  IN EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *WorkingHeader
  )
{
  EFI_STATUS  Status;

  ASSERT (WorkingHeader != NULL);

  //
  // Here using gEfiSystemNvDataFvGuid as the signature.
  //
  CopyMem (
    &WorkingHeader->Signature,
    &gEfiSystemNvDataFvGuid,
    sizeof (EFI_GUID)
    );
  WorkingHeader->WriteQueueSize = (UINTN) PcdGet32 (PcdFlashNvStorageFtwWorkingSize) - sizeof (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER);

  //
  // Crc is calculated with all the fields except Crc and STATE
  //
  WorkingHeader->WorkingBlockValid    = FTW_ERASE_POLARITY;
  WorkingHeader->WorkingBlockInvalid  = FTW_ERASE_POLARITY;
  SetMem (&WorkingHeader->Crc, sizeof (UINT32), FTW_ERASED_BYTE);

  //
  // Calculate the CRC value
  //
  Status = RuntimeDriverCalculateCrc32 (
                  (UINT8 *) WorkingHeader,
                  sizeof (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER),
                  &WorkingHeader->Crc
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Restore the WorkingBlockValid flag to VALID state
  //
  WorkingHeader->WorkingBlockValid    = FTW_VALID_STATE;
  WorkingHeader->WorkingBlockInvalid  = FTW_INVALID_STATE;

  return EFI_SUCCESS;
}

/**
  Update a bit of state on a block device. The location of the bit is calculated by the
  (Lba, Offset, bit). Here bit is determined by the the name of a certain bit.

  @param  FtwLiteDevice    The private data of FTW_LITE driver.
  @param  FvBlock          FVB Protocol interface to access SrcBlock and DestBlock.
  @param  Lba              Lba of a block.
  @param  Offset           Offset on the Lba.
  @param  Address          The Address of FvState.
  @param  NewBit           New value that will override the old value if it can be change.

  @retval EFI_SUCCESS      A state bit has been updated successfully
  @retval EFI_ABORTED      Read block fail.
  @return Others           Access block device error.

**/
EFI_STATUS
FtwUpdateFvState (
  IN EFI_FTW_LITE_DEVICE                 *FtwLiteDevice,
  IN EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock,
  IN EFI_LBA                             Lba,
  IN UINTN                               Offset,
  IN UINTN                               Address,
  IN UINT8                               NewBit
  )
{
  EFI_STATUS  Status;
  UINT8       State;
  UINTN       Length;
  UINT8       ReadState;
  UINTN       TryWritetimes;
  BOOLEAN     WriteSuccess;

  //
  // Read state from device, assume State is only one byte.
  //

  Length  = sizeof (UINT8);
  if (mSmst == NULL) {
    Lba += Offset / FtwLiteDevice->SizeOfSpareBlock;
    Offset = Offset % FtwLiteDevice->SizeOfSpareBlock;
    Status  = FvBlock->Read (FvBlock, Lba, Offset, &Length, &State);
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
  } else {
    CopyMem (&State, (UINT8 *) Address, Length);
  }

  State ^= FTW_POLARITY_REVERT;
  State |= NewBit;
  State ^= FTW_POLARITY_REVERT;

  WriteSuccess = FALSE;
  TryWritetimes = 0;
  Status = EFI_ABORTED;
  //
  // Start try write state to device and using do-while to make sure
  // at least write state once.
  //
  do {
    //
    // Write state back to device
    //
    Length  = sizeof (UINT8);
    if (mSmst == NULL) {
      Status  = FvBlock->Write (FvBlock, Lba, Offset, &Length, &State);
    } else {
      Status = FtwLiteDevice->SmmFwbServices->Write (
                                                FtwLiteDevice->SmmFwbServices,
                                                (UINTN)Address,
                                                &Length,
                                                &State
                                                );
    }
    Length  = sizeof (UINT8);
    if (mSmst == NULL) {
      Status  = FvBlock->Read (FvBlock, Lba, Offset, &Length, &ReadState);
    } else {
      CopyMem (&ReadState, (UINT8 *) Address, Length);
    }
    //
    // Check write data is whther correct
    //
    WriteSuccess = (ReadState == State) ? TRUE : FALSE;
    TryWritetimes++;
  } while (!WriteSuccess && TryWritetimes < FTW_MAX_TRY_ACCESS_FVB_TIMES);
  //
  // if write failed, set status as EFI_ABORT
  //
  if (!WriteSuccess) {
    Status = EFI_ABORTED;
  }

  return Status;
}

/**
  Get the last Write record pointer. The last record is the record whose 'complete' state hasn't
  been set. After all, this header may be a EMPTY header entry for next Allocate.

  @param  FtwLiteDevice    Private data of this driver.
  @param  FtwLastRecord    Pointer to retrieve the last write record.

  @retval EFI_SUCCESS      Get the last write record successfully.
  @retval EFI_ABORTED      The FTW work space is damaged.

**/
EFI_STATUS
FtwGetLastRecord (
  IN  EFI_FTW_LITE_DEVICE  *FtwLiteDevice,
  OUT EFI_FTW_LITE_RECORD  **FtwLastRecord
  )
{
  EFI_FTW_LITE_RECORD *Record;

  Record = (EFI_FTW_LITE_RECORD *) (FtwLiteDevice->FtwWorkSpaceHeader + 1);
  while (Record->WriteCompleted == FTW_VALID_STATE) {
    //
    // If Offset exceed the FTW work space boudary, return error.
    //
    if ((UINTN) ((UINT8 *) Record - FtwLiteDevice->FtwWorkSpace) > FtwLiteDevice->FtwWorkSpaceSize) {
      return EFI_ABORTED;
    }

    Record++;
  }
  //
  // Last write record is found
  //
  *FtwLastRecord = Record;
  return EFI_SUCCESS;
}


/**
  Read from working block to refresh the work space in memory.

  @param  FtwLiteDevice    Point to private data of FTW driver.

  @retval EFI_SUCCESS      The function completed successfully.
  @retval EFI_ABORTED      The function could not complete successfully.

**/
EFI_STATUS
WorkSpaceRefresh (
  IN EFI_FTW_LITE_DEVICE  *FtwLiteDevice
  )
{
  EFI_STATUS          Status;
  UINTN               Length;
  UINTN               Offset;
  EFI_FTW_LITE_RECORD *Record;
  UINTN               Index;

  //
  // Initialize WorkSpace as FTW_ERASED_BYTE
  //
  SetMem (FtwLiteDevice->FtwWorkSpace, FtwLiteDevice->FtwWorkSpaceSize, FTW_ERASED_BYTE);

  //
  // Read from working block
  //

  if (mSmst == NULL) {
    if (FtwLiteDevice->NumberOfFtwWorkBlock != 0) {
      for (Index = 0; Index < FtwLiteDevice->NumberOfFtwWorkBlock; Index++) {
        Length = FtwLiteDevice->FtwWorkSpaceSize / FtwLiteDevice->NumberOfFtwWorkBlock;
        Status = FtwLiteDevice->FtwFvBlock->Read (
                                              FtwLiteDevice->FtwFvBlock,
                                              FtwLiteDevice->FtwWorkSpaceLba + Index,
                                              FtwLiteDevice->FtwWorkSpaceBase,
                                              &Length,
                                              FtwLiteDevice->FtwWorkSpace + (Length * Index)
                                              );
        if (EFI_ERROR (Status)) {
          return EFI_ABORTED;
        }
      }
    } else {
      Length = FtwLiteDevice->FtwWorkSpaceSize;
      Status = FtwLiteDevice->FtwFvBlock->Read (
                                            FtwLiteDevice->FtwFvBlock,
                                            FtwLiteDevice->FtwWorkSpaceLba,
                                            FtwLiteDevice->FtwWorkSpaceBase,
                                            &Length,
                                            FtwLiteDevice->FtwWorkSpace
                                            );
    }
  } else {
    Length = FtwLiteDevice->FtwWorkSpaceSize;
    CopyMem (FtwLiteDevice->FtwWorkSpace, (UINT8 *) (UINTN) FtwLiteDevice->WorkSpaceAddress, Length);
  }

  //
  // Refresh the FtwLastRecord
  //
  Status  = FtwGetLastRecord (FtwLiteDevice, &FtwLiteDevice->FtwLastRecord);

  Record  = FtwLiteDevice->FtwLastRecord;
  Offset  = (UINTN) (UINT8 *) Record - (UINTN) FtwLiteDevice->FtwWorkSpace;

  //
  // IF work space has error or Record is out of the workspace limit, THEN
  //   call reclaim.
  //
  if (EFI_ERROR (Status) || (Offset + WRITE_TOTAL_SIZE >= FtwLiteDevice->FtwWorkSpaceSize)) {
    //
    // reclaim work space in working block.
    //
    Status = FtwReclaimWorkSpace (FtwLiteDevice);
    if (EFI_ERROR (Status)) {
      DEBUG ((mFtwLiteError, "FtwLite: Reclaim workspace - %r\n", Status));

      return EFI_ABORTED;
    }
  }

  return EFI_SUCCESS;
}

/**
  Reclaim the work space. Get rid of all the completed write records and write records in
  the Fault Tolerant work space.

  @param  FtwLiteDevice         Point to private data of FTW driver.
  @param  FtwSpaceBuffer        Buffer to contain the reclaimed clean data.
  @param  BufferSize            Size of the FtwSpaceBuffer.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_BUFFER_TOO_SMALL  The FtwSpaceBuffer is too small.
  @retval EFI_ABORTED           The function could not complete successfully.

**/
EFI_STATUS
CleanupWorkSpace (
  IN EFI_FTW_LITE_DEVICE  *FtwLiteDevice,
  IN OUT UINT8            *FtwSpaceBuffer,
  IN UINTN                BufferSize
  )
{
  UINTN               Length;
  EFI_FTW_LITE_RECORD *Record;

  //
  // To check if the buffer is large enough
  //
  Length = FtwLiteDevice->FtwWorkSpaceSize;
  if (BufferSize < Length) {
    return EFI_BUFFER_TOO_SMALL;
  }
  //
  // Clear the content of buffer that will save the new work space data
  //
  SetMem (FtwSpaceBuffer, Length, FTW_ERASED_BYTE);

  //
  // Copy EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER to buffer
  //
  CopyMem (
    FtwSpaceBuffer,
    FtwLiteDevice->FtwWorkSpaceHeader,
    sizeof (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER)
    );

  //
  // Get the last record
  //
  Record = FtwLiteDevice->FtwLastRecord;
  if ((Record != NULL) && (Record->WriteAllocated == FTW_VALID_STATE) && (Record->WriteCompleted != FTW_VALID_STATE)) {
    CopyMem (
      (UINT8 *) FtwSpaceBuffer + sizeof (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER),
      Record,
      WRITE_TOTAL_SIZE
      );
  }

  return EFI_SUCCESS;
}

/**
  Reclaim the work space on the working block.

  @param  FtwLiteDevice         Point to private data of FTW driver.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory error.
  @retval EFI_ABORTED           The function could not complete successfully.

**/
EFI_STATUS
FtwReclaimWorkSpace (
  IN EFI_FTW_LITE_DEVICE  *FtwLiteDevice
  )
{
  EFI_STATUS                              Status;
  UINT8                                   *TempBuffer;
  UINTN                                   TempBufferSize;
  UINT8                                   *Ptr;
  UINTN                                   SpareBufferSize;
  UINT8                                   *SpareBuffer;
  EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *WorkingBlockHeader;

  DEBUG ((mFtwLiteError, "FtwLite: start to reclaim work space\n"));

  //
  // Read all original data from working block to a memory buffer
  //
  TempBufferSize = FtwLiteDevice->SpareAreaLength;
  TempBuffer = FtwAllocateZeroBuffer (FtwLiteDevice, TempBufferSize);
  if (TempBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (TempBuffer, TempBufferSize);

  Ptr = TempBuffer;
  Status = FtwLiteReadWorkBlock (FtwLiteDevice, TempBuffer);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&TempBuffer);

    return EFI_ABORTED;
  }

  //
  // Clean up the workspace, remove all the completed records.
  //
  Ptr = TempBuffer +
    ((UINTN) (FtwLiteDevice->FtwWorkSpaceLba - FtwLiteDevice->FtwWorkBlockLba)) *
    FtwLiteDevice->SizeOfSpareBlock +
    FtwLiteDevice->FtwWorkSpaceBase;
  Status = CleanupWorkSpace (
            FtwLiteDevice,
            Ptr,
            FtwLiteDevice->FtwWorkSpaceSize
            );

  CopyMem (
    FtwLiteDevice->FtwWorkSpace,
    Ptr,
    FtwLiteDevice->FtwWorkSpaceSize
    );

  Status = FtwGetLastRecord (FtwLiteDevice, &FtwLiteDevice->FtwLastRecord);

  //
  // Set the WorkingBlockValid and WorkingBlockInvalid as INVALID
  //
  WorkingBlockHeader                      = (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *) Ptr;
  WorkingBlockHeader->WorkingBlockValid   = FTW_INVALID_STATE;
  WorkingBlockHeader->WorkingBlockInvalid = FTW_INVALID_STATE;

  //
  // Try to keep the content of spare block
  // Save spare block into a spare backup memory buffer (Sparebuffer)
  //
  SpareBufferSize = FtwLiteDevice->SpareAreaLength;

  SpareBuffer = FtwAllocateZeroBuffer (FtwLiteDevice, SpareBufferSize);
  if (SpareBuffer == NULL) {
    FtwFreePool (FtwLiteDevice, (VOID **)&TempBuffer);
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr = SpareBuffer;
  Status = FtwLiteReadSpareBlock (FtwLiteDevice, SpareBuffer);
  //
  // Write the memory buffer to spare block
  //
  Status = FtwWriteSpareBlock (FtwLiteDevice, TempBuffer, SpareBufferSize, FTW_MAX_TRY_ACCESS_FVB_TIMES);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&TempBuffer);
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);
    return EFI_ABORTED;
  }

  FtwFreePool (FtwLiteDevice, (VOID **)&TempBuffer);
  //
  // Write the spare block to working block
  //
  Status = FlushSpareBlockToWorkingBlock (FtwLiteDevice);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);
    return Status;
  }
  //
  // Restore spare backup buffer into spare block , if no failure happened during FtwWrite.
  //
  Status = FtwWriteSpareBlock (FtwLiteDevice, SpareBuffer, SpareBufferSize, FTW_MAX_TRY_ACCESS_FVB_TIMES);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);
    return EFI_ABORTED;
  }

  FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);

  DEBUG ((mFtwLiteError, "FtwLite: reclaim work space success\n"));

  return EFI_SUCCESS;
}

