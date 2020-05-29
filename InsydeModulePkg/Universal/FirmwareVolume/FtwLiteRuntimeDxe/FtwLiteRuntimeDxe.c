/** @file
  This is a simple fault tolerant write driver, based on PlatformFd library.
  And it only supports write BufferSize <= SpareAreaLength.

  This boot service only protocol provides fault tolerant write capability for
  block devices.  The protocol has internal non-volatile intermediate storage
  of the data and private information. It should be able to recover
  automatically from a critical fault, such as power failure.

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

EFI_SMM_SYSTEM_TABLE2           *mSmst;
EFI_FTW_LITE_DEVICE             *mFtwLiteDevice;
#ifndef MDEPKG_NDEBUG
UINTN mFtwLiteInfo  = EFI_D_FTW_INFO;
UINTN mFtwLiteError = EFI_D_FTW_LITE;
#endif


STATIC
EFI_STATUS
FtwLiteDeviceInit (
  IN EFI_FTW_LITE_DEVICE                 *DstFtwLiteDevice,
  IN EFI_FTW_LITE_DEVICE                 *ScrFtwLiteDevice
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_RUNTIME_PROTOCOL       *SmmRT;


  CopyMem (DstFtwLiteDevice, ScrFtwLiteDevice, (sizeof (EFI_FTW_LITE_DEVICE) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwWorkingSize)));

  DstFtwLiteDevice->Handle = NULL;
  ZeroMem (&(DstFtwLiteDevice->FtwLiteInstance), sizeof (EFI_FTW_LITE_PROTOCOL));
  DstFtwLiteDevice->FtwFvBlock = NULL;
  DstFtwLiteDevice->FtwBackupFvb = NULL;

  DstFtwLiteDevice->FtwWorkSpace     = (UINT8 *) (DstFtwLiteDevice + 1);
  DstFtwLiteDevice->FtwWorkSpaceHeader = (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *) DstFtwLiteDevice->FtwWorkSpace;
  DstFtwLiteDevice->FtwLastRecord = NULL;

  //
  // Try to get native gEfiSmmFwBlockServiceProtocolGuid instance first. only need try to get
  // ECP gEfiSmmFwBlockServiceProtocolGuid instance if native instance doesn't exist.
  //
  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&(DstFtwLiteDevice->SmmFwbServices)
                    );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmRT->LocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&(DstFtwLiteDevice->SmmFwbServices)
                    );
  return Status;
}


/**
  Starts a target block update. This function will record data about write  in fault tolerant
  storage and will complete the write in a recoverable manner, ensuring at all times that either
  the original contents or the modified contents are available.

  @param  FtwLiteDevice          Pointer to EFI_FTW_LITE_DEVICE instancer.
  @param  BaseAddress            The Firmware Volume BaseAddress of the target block.
  @param  FvbHandle              Hanle which saved Fvb protocol.
  @param  Lba                    The logical block address of the target block.
  @param  Offset                 The offset within the target block to place the data.
  @param  NumBytes               The number of bytes to write to the target block.
  @param  Buffer                 The data to write.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_BAD_BUFFER_SIZE    he write would span a target block, which is not a valid action.
  @retval EFI_ACCESS_DENIED      No allocated writes exist.
  @retval EFI_NOT_FOUND          cannot find Fvb by handle.
  @retval EFI_OUT_OF_RESOURCES   Cannot allocate memory.
  @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
EFIAPI
CommonFtwWrite (
  IN EFI_FTW_LITE_DEVICE         *FtwLiteDevice,
  IN EFI_PHYSICAL_ADDRESS        BaseAddress, OPTIONAL
  IN EFI_HANDLE                  FvbHandle,   OPTIONAL
  IN EFI_LBA                     Lba,
  IN UINTN                       Offset,
  IN OUT UINTN                   *NumBytes,
  IN VOID                        *Buffer
  )
{
  EFI_STATUS                          Status;
  EFI_FTW_LITE_RECORD                 *Record;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  UINTN                               MyLength;
  UINTN                               MyOffset;
  UINTN                               MyBufferSize;
  UINT8                               *MyBuffer;
  UINTN                               SpareBufferSize;
  UINT8                               *SpareBuffer;
  UINTN                               Index;
  UINT8                               *Ptr;
  EFI_DEV_PATH_PTR                    DevPtr;
  UINTN                               TryWritetimes;
  BOOLEAN                             WriteSuccess;
  UINT8                               *ReadRecordBuffer;
  UINTN                               Length;
  EFI_PHYSICAL_ADDRESS                Address;
  //
  // Refresh work space and get last record
  //

  Status = WorkSpaceRefresh (FtwLiteDevice);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  Record = FtwLiteDevice->FtwLastRecord;
  Address = BaseAddress + (UINTN )Lba * FtwLiteDevice->SizeOfSpareBlock + Offset;
  //
  // Check the flags of last write record
  //
  if ((Record->WriteAllocated == FTW_VALID_STATE) || (Record->SpareCompleted == FTW_VALID_STATE)) {
    return EFI_ACCESS_DENIED;
  }
  //
  // IF former record has completed, THEN use next record
  //
  if (Record->WriteCompleted == FTW_VALID_STATE) {
    Record++;
    FtwLiteDevice->FtwLastRecord = Record;
  }

  MyOffset = (UINT8 *) Record - FtwLiteDevice->FtwWorkSpace;

  //
  // Check if the input data can fit within the target block
  //
  if ((Offset +*NumBytes) > FtwLiteDevice->SpareAreaLength) {
    return EFI_BAD_BUFFER_SIZE;
  }
  //
  // Check if there is enough free space for allocate a record
  //
  if ((MyOffset + WRITE_TOTAL_SIZE) > FtwLiteDevice->FtwWorkSpaceSize) {
    Status = FtwReclaimWorkSpace (FtwLiteDevice);
    if (EFI_ERROR (Status)) {

      return EFI_ABORTED;
    }
  }
  //
  // Allocate a write record in workspace.
  // Update Header->WriteAllocated as VALID
  //
  Status = FtwUpdateFvState (
             FtwLiteDevice,
             FtwLiteDevice->FtwFvBlock,
             FtwLiteDevice->FtwWorkSpaceLba,
             FtwLiteDevice->FtwWorkSpaceBase + MyOffset,
             (UINTN)(FtwLiteDevice->WorkSpaceAddress)+ MyOffset,
             WRITE_ALLOCATED
             );

  if (EFI_ERROR (Status)) {

    return EFI_ABORTED;
  }

  Record->WriteAllocated = FTW_VALID_STATE;

  //
  // Prepare data of write record, filling DevPath with memory mapped address.
  //
  DevPtr.MemMap                 = (MEMMAP_DEVICE_PATH *) &Record->DevPath;
  DevPtr.MemMap->Header.Type    = HARDWARE_DEVICE_PATH;
  DevPtr.MemMap->Header.SubType = HW_MEMMAP_DP;
  SetDevicePathNodeLength (&DevPtr.MemMap->Header, sizeof (MEMMAP_DEVICE_PATH));


  DevPtr.MemMap->MemoryType       = EfiMemoryMappedIO;
  DevPtr.MemMap->StartingAddress  = (mSmst != NULL) ? BaseAddress : FtwLiteDevice->WorkSpaceFvBaseAddr;
  DevPtr.MemMap->EndingAddress    = DevPtr.MemMap->StartingAddress + *NumBytes;
  //
  // ignored!
  //
  Record->Lba       = Lba;
  Record->Offset    = Offset;
  Record->NumBytes  = *NumBytes;

  //
  // Write the record to the work space.
  //
  MyOffset  = (UINT8 *) Record - FtwLiteDevice->FtwWorkSpace;
  MyLength  = FTW_LITE_RECORD_SIZE;

  //
  // Allocate pool for ReadRecordBuffer
  //
  ReadRecordBuffer = FtwAllocateZeroBuffer (FtwLiteDevice, FTW_LITE_RECORD_SIZE);
  if (ReadRecordBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Start try write record to workspace and use do-while to make
  // sure at least write once.
  //
  WriteSuccess = FALSE;
  TryWritetimes = 0;
  //
  // doens't need to erase whole block, only need try to write again
  // to make sure the data is correct.
  //
  do {
    if (mSmst == NULL) {
      //
      // if the record across two LBAs, we should write to different LBA.
      //
      if ((MyOffset % FtwLiteDevice->SizeOfSpareBlock + FTW_LITE_RECORD_SIZE) >= FtwLiteDevice->SizeOfSpareBlock) {
        MyLength = FtwLiteDevice->SizeOfSpareBlock - (MyOffset % FtwLiteDevice->SizeOfSpareBlock);
        Status = FtwLiteDevice->FtwFvBlock->Write (
                                  FtwLiteDevice->FtwFvBlock,
                                  FtwLiteDevice->FtwWorkSpaceLba + (MyOffset / FtwLiteDevice->SizeOfSpareBlock),
                                  FtwLiteDevice->FtwWorkSpaceBase + (MyOffset % FtwLiteDevice->SizeOfSpareBlock),
                                  &MyLength,
                                  (UINT8 *) Record
                                  );

        MyLength = FTW_LITE_RECORD_SIZE - MyLength;
        Status = FtwLiteDevice->FtwFvBlock->Write (
                                  FtwLiteDevice->FtwFvBlock,
                                  FtwLiteDevice->FtwWorkSpaceLba + (MyOffset / FtwLiteDevice->SizeOfSpareBlock) + 1,
                                  FtwLiteDevice->FtwWorkSpaceBase,
                                  &MyLength,
                                  ((UINT8 *) Record) + FTW_LITE_RECORD_SIZE - MyLength
                                  );

      } else {
        MyLength  = FTW_LITE_RECORD_SIZE;
        Status = FtwLiteDevice->FtwFvBlock->Write (
                                  FtwLiteDevice->FtwFvBlock,
                                  FtwLiteDevice->FtwWorkSpaceLba + (MyOffset / FtwLiteDevice->SizeOfSpareBlock),
                                  FtwLiteDevice->FtwWorkSpaceBase + (MyOffset % FtwLiteDevice->SizeOfSpareBlock),
                                  &MyLength,
                                  (UINT8 *) Record
                                  );
      }
    } else {
      Status = FtwLiteDevice->SmmFwbServices->Write (
                                FtwLiteDevice->SmmFwbServices,
                                (UINTN) (FtwLiteDevice->WorkSpaceAddress) + MyOffset,
                                &MyLength,
                                (UINT8 *) Record
                                );
    }
    if (EFI_ERROR (Status)) {
      FtwFreePool (FtwLiteDevice, (VOID **)&ReadRecordBuffer);
      return EFI_ABORTED;
    }
    MyLength  = FTW_LITE_RECORD_SIZE;
    if (mSmst == NULL) {
      //
      // if the record across two LBAs, we should read from different LBA.
      //
      if ((MyOffset % FtwLiteDevice->SizeOfSpareBlock + FTW_LITE_RECORD_SIZE) >= FtwLiteDevice->SizeOfSpareBlock) {
        MyLength = FtwLiteDevice->SizeOfSpareBlock - (MyOffset % FtwLiteDevice->SizeOfSpareBlock);
        Status = FtwLiteDevice->FtwFvBlock->Read (
                                  FtwLiteDevice->FtwFvBlock,
                                  FtwLiteDevice->FtwWorkSpaceLba + (MyOffset / FtwLiteDevice->SizeOfSpareBlock),
                                  FtwLiteDevice->FtwWorkSpaceBase + (MyOffset % FtwLiteDevice->SizeOfSpareBlock),
                                  &MyLength,
                                  (UINT8 *) ReadRecordBuffer
                                  );

        MyLength = FTW_LITE_RECORD_SIZE - MyLength;
        Status = FtwLiteDevice->FtwFvBlock->Read (
                                   FtwLiteDevice->FtwFvBlock,
                                   FtwLiteDevice->FtwWorkSpaceLba + (MyOffset / FtwLiteDevice->SizeOfSpareBlock) + 1,
                                   FtwLiteDevice->FtwWorkSpaceBase,
                                   &MyLength,
                                   ((UINT8 *) ReadRecordBuffer) + FTW_LITE_RECORD_SIZE - MyLength
                                   );

      } else {
        MyLength  = FTW_LITE_RECORD_SIZE;
        Status = FtwLiteDevice->FtwFvBlock->Read (
                                  FtwLiteDevice->FtwFvBlock,
                                  FtwLiteDevice->FtwWorkSpaceLba + (MyOffset / FtwLiteDevice->SizeOfSpareBlock),
                                  FtwLiteDevice->FtwWorkSpaceBase + (MyOffset % FtwLiteDevice->SizeOfSpareBlock),
                                  &MyLength,
                                  (UINT8 *) ReadRecordBuffer
                                  );
      }
    } else {
      CopyMem (ReadRecordBuffer, (UINT8 *) (UINTN) (FtwLiteDevice->WorkSpaceAddress + (UINT64) MyOffset), MyLength);
      Status = EFI_SUCCESS;
    }
    if (EFI_ERROR (Status)) {
      FtwFreePool (FtwLiteDevice, (VOID **)&ReadRecordBuffer);
      return EFI_ABORTED;
    }
    //
    // Check writing data is whether correct
    //
    MyLength  = FTW_LITE_RECORD_SIZE;
    WriteSuccess = (CompareMem (ReadRecordBuffer, Record, MyLength) == 0) ? TRUE : FALSE;
    TryWritetimes++;
  } while (!WriteSuccess && TryWritetimes < FTW_MAX_TRY_ACCESS_FVB_TIMES);
  FtwFreePool (FtwLiteDevice, (VOID **)&ReadRecordBuffer);
  if (!WriteSuccess) {
    return EFI_ABORTED;
  }
  //
  // Record has been written to working block, then write data.
  //
  //
  // Allocate a memory buffer
  //
  MyBufferSize  = FtwLiteDevice->SpareAreaLength;
  MyBuffer = FtwAllocateZeroBuffer (FtwLiteDevice, MyBufferSize);
  if (MyBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Fvb = mSmst == NULL ? FtwLiteDevice->FtwFvBlock : NULL;
  if (IsInWorkingBlock (FtwLiteDevice, Fvb, Lba, Address)) {
    //
    // If target block falls into working block, we must follow the process of
    // updating working block.
    //
    FtwLiteReadWorkBlock (FtwLiteDevice, MyBuffer);
    //
    // Update Offset by adding the offset from the start LBA of working block to
    // the target LBA. The target block can not span working block!
    //
    if (mSmst == NULL) {
      Offset = (((UINTN) (Lba - FtwLiteDevice->FtwWorkBlockLba)) * FtwLiteDevice->SizeOfSpareBlock + Offset);
    } else {
      Offset = (UINTN)(Address - FtwLiteDevice->WorkBlockAddr);
    }
    ASSERT ((Offset +*NumBytes) <= FtwLiteDevice->SpareAreaLength);

  } else {

    Ptr = MyBuffer;
    for (Index = 0; Index < FtwLiteDevice->NumberOfSpareBlock; Index += 1) {
      MyLength  = FtwLiteDevice->SizeOfSpareBlock;
      if (mSmst == NULL) {
        Status    = Fvb->Read (Fvb, Lba + Index, 0, &MyLength, Ptr);
      } else {
        Length = FtwLiteDevice->SizeOfSpareBlock * FtwLiteDevice->NumberOfSpareBlock;
        CopyMem (
          MyBuffer,
          (UINT8 *) (UINTN) (BaseAddress + (UINTN) Lba * FtwLiteDevice->SizeOfSpareBlock),
          Length
          );
        Status = EFI_SUCCESS;
      }
      if (EFI_ERROR (Status)) {
        FtwFreePool (FtwLiteDevice, (VOID **)&MyBuffer);
        return EFI_ABORTED;
      }

      Ptr += MyLength;
    }
  }
  //
  // Overwrite the updating range data with
  // the input buffer content
  //
  CopyMem (MyBuffer + Offset, Buffer, *NumBytes);

  //
  // Try to keep the content of spare block
  // Save spare block into a spare backup memory buffer (Sparebuffer)
  //
  SpareBufferSize = FtwLiteDevice->SpareAreaLength;
  SpareBuffer = FtwAllocateZeroBuffer (FtwLiteDevice, SpareBufferSize);
  if (SpareBuffer == NULL) {
    FtwFreePool (FtwLiteDevice, (VOID **)&MyBuffer);
    return EFI_OUT_OF_RESOURCES;
  }
  Status = FtwLiteReadSpareBlock (FtwLiteDevice, SpareBuffer);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&MyBuffer);
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);
    return EFI_ABORTED;
  }
  //
  // Write the memory buffer to spare block
  //
  Status = FtwWriteSpareBlock (FtwLiteDevice, MyBuffer, SpareBufferSize, FTW_MAX_TRY_ACCESS_FVB_TIMES);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&MyBuffer);
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);

    return EFI_ABORTED;
  }
  FtwFreePool (FtwLiteDevice, (VOID **)&MyBuffer);

  //
  // Set the SpareCompleteD in the FTW record,
  //
  MyOffset = (UINT8 *) Record - FtwLiteDevice->FtwWorkSpace;
  Status = FtwUpdateFvState (
             FtwLiteDevice,
             FtwLiteDevice->FtwFvBlock,
             FtwLiteDevice->FtwWorkSpaceLba,
             FtwLiteDevice->FtwWorkSpaceBase + MyOffset,
             (UINTN)(FtwLiteDevice->WorkSpaceAddress)+ MyOffset,
             SPARE_COMPLETED
             );
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);
    return EFI_ABORTED;
  }

  Record->SpareCompleted = FTW_VALID_STATE;

  //
  //  Since the content has already backuped in spare block, the write is
  //  guaranteed to be completed with fault tolerant manner.
  //
  Status = FtwWriteRecord (FtwLiteDevice, Fvb, BaseAddress);
  if (EFI_ERROR (Status)) {
    while (FtwLiteDevice->SpareAreaFvBaseAddr == 0);
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);

    return EFI_ABORTED;
  }

  Record++;
  FtwLiteDevice->FtwLastRecord = Record;
  //
  // Restore spare backup buffer into spare block , if no failure happened during FtwWrite.
  //
  Status = FtwWriteSpareBlock (FtwLiteDevice, SpareBuffer, SpareBufferSize, FTW_MAX_TRY_ACCESS_FVB_TIMES);
  if (EFI_ERROR (Status)) {
    FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);

    return EFI_ABORTED;
  }
  //
  // All success.
  //
  FtwFreePool (FtwLiteDevice, (VOID **)&SpareBuffer);

  return EFI_SUCCESS;
}

//
// In write function, we should check the target range to prevent the user
// from writing Spare block and Working space directly.
//
//
// SmmFtw Protocol API
//

/**
  Starts a target block update. This function will record data about write  in fault tolerant
  storage and will complete the write in a recoverable manner, ensuring at all times that either
  the original contents or the modified contents are available.

  @param  This                   Calling context.
  @param  BaseAddress            The Firmware Volume BaseAddress of the target block.
  @param  Lba                    The logical block address of the target block.
  @param  Offset                 The offset within the target block to place the data.
  @param  NumBytes               The number of bytes to write to the target block.
  @param  Buffer                 The data to write.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_BAD_BUFFER_SIZE    The write would span a target block, which is not a valid action.
  @retval EFI_ACCESS_DENIED      No writes have been allocated.
  @retval EFI_NOT_FOUND          Cannot find FVB by handle.
  @retval EFI_OUT_OF_RESOURCES   Cannot allocate memory.
  @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
EFIAPI
SmmFtwWrite (
  IN EFI_SMM_FTW_PROTOCOL                  *This,
  IN EFI_PHYSICAL_ADDRESS                  BaseAddress,
  IN EFI_LBA                               Lba,
  IN UINTN                                 Offset,
  IN OUT UINTN                             *NumBytes,
  IN VOID                                  *Buffer
  )
{
  EFI_STATUS                          Status;
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice;


  FtwLiteDevice = SMM_FTW_LITE_CONTEXT_FROM_THIS (This);
  Status = CommonFtwWrite (
             FtwLiteDevice,
             BaseAddress,
             NULL,
             Lba,
             Offset,
             NumBytes,
             Buffer
            );
  return Status;

}

/**
  Starts a target block update. This function will record data about write  in fault tolerant
  storage and will complete the write in a recoverable manner, ensuring at all times that either
  the original contents or the modified contents are available.

  @param  This                   Calling context.
  @param  FvbHandle              The handle of FVB protocol that provides services for reading,
                                 writing, and erasing the target block.
  @param  Lba                    The logical block address of the target block.
  @param  Offset                 The offset within the target block to place the data.
  @param  NumBytes               The number of bytes to write to the target block.
  @param  Buffer                 The data to write.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_BAD_BUFFER_SIZE    The write would span a target block, which is not a valid action.
  @retval EFI_ACCESS_DENIED      No writes have been allocated.
  @retval EFI_NOT_FOUND          Cannot find FVB by handle.
  @retval EFI_OUT_OF_RESOURCES   Cannot allocate memory.
  @retval EFI_ABORTED            The function could not complete successfully.

**/
EFI_STATUS
EFIAPI
FtwLiteWrite (
  IN EFI_FTW_LITE_PROTOCOL                 *This,
  IN EFI_HANDLE                            FvbHandle,
  IN EFI_LBA                               Lba,
  IN UINTN                                 Offset,
  IN OUT UINTN                             *NumBytes,
  IN VOID                                  *Buffer
  )
{
  EFI_STATUS                          Status;
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice;


  FtwLiteDevice = FTW_LITE_CONTEXT_FROM_THIS (This);
  Status = CommonFtwWrite (
             FtwLiteDevice,
             0,
             FvbHandle,
             Lba,
             Offset,
             NumBytes,
             Buffer
            );
  return Status;
}

/**
  Write a record with fault tolerant mannaer. Since the content has already backuped in spare block,
  the write is guaranteed to be completed with fault tolerant manner.

  @param  FtwLiteDevice      Calling context.
  @param  Fvb                The handle of FVB protocol that provides services for reading,
                             writing, and erasing the target block.
  @param  FvBaseAddr         The logical block address of the target block.

  @retval EFI_SUCCESS        The function completed successfully.
  @retval EFI_ABORTED        The function could not complete successfully.

**/
EFI_STATUS
FtwWriteRecord (
  IN EFI_FTW_LITE_DEVICE                   *FtwLiteDevice,
  IN EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL    *Fvb,
  IN EFI_PHYSICAL_ADDRESS                  FvBaseAddr
  )
{
  EFI_STATUS          Status;
  EFI_FTW_LITE_RECORD *Record;
  EFI_LBA             WorkSpaceLbaOffset;
  UINTN               Offset;
  UINTN               WorkSpaceOffset;
  //
  // Spare Complete but Destination not complete,
  // Recover the targt block with the spare block.
  //
  Record = FtwLiteDevice->FtwLastRecord;

  //
  // IF target block is working block, THEN Flush Spare Block To Working Block;
  // ELSE IF target block is boot block, THEN Flush Spare Block To boot Block;
  // ELSE flush spare block to normal target block.ENDIF
  //
  if (IsInWorkingBlock (FtwLiteDevice, Fvb, Record->Lba, FvBaseAddr)) {
    //
    // If target block is working block, Attention:
    // it's required to set SPARE_COMPLETED to spare block.
    //
    WorkSpaceLbaOffset = FtwLiteDevice->FtwWorkSpaceLba - FtwLiteDevice->FtwWorkBlockLba;
    Offset = (UINT8 *) Record - FtwLiteDevice->FtwWorkSpace;
    WorkSpaceOffset = (UINTN)(FtwLiteDevice->WorkSpaceAddress - FtwLiteDevice->WorkBlockAddr);
    Status = FtwUpdateFvState (
               FtwLiteDevice,
               FtwLiteDevice->FtwBackupFvb,
               FtwLiteDevice->FtwSpareLba + WorkSpaceLbaOffset,
               FtwLiteDevice->FtwWorkSpaceBase + Offset,
               (UINTN)(FtwLiteDevice->SpareAreaAddress + WorkSpaceOffset + Offset),
               SPARE_COMPLETED
               );
    ASSERT_EFI_ERROR (Status);

    Status = FlushSpareBlockToWorkingBlock (FtwLiteDevice);

  } else {
    //
    // Update blocks other than working block or boot block
    //
    Status = FlushSpareBlockToTargetBlock (FtwLiteDevice, Fvb, FvBaseAddr, Record->Lba);
  }

  ASSERT_EFI_ERROR (Status);

  //
  // Set WriteCompleted flag in record
  //
  Offset = (UINT8 *) Record - FtwLiteDevice->FtwWorkSpace;
  Status = FtwUpdateFvState (
             FtwLiteDevice,
             FtwLiteDevice->FtwFvBlock,
             FtwLiteDevice->FtwWorkSpaceLba,
             FtwLiteDevice->FtwWorkSpaceBase + Offset,
             (UINTN) (FtwLiteDevice->WorkSpaceAddress + Offset),
             WRITE_COMPLETED
             );
  ASSERT_EFI_ERROR (Status);

  Record->WriteCompleted = FTW_VALID_STATE;
  return EFI_SUCCESS;
}

/**
  Restarts a previously interrupted write. The caller must provide the block protocol needed to complete
  the interrupted write.

  @param  FtwLiteDevice      The private data of FTW_LITE driver.
  @param  FvbHandle          The handle of FVB protocol that provides services for reading, writing,
                             and erasing the target block.

  @retval EFI_SUCCESS        The function completed successfully.
  @retval EFI_ACCESS_DENIED  No pending writes exist.
  @retval EFI_NOT_FOUND      FVB protocol not found by the handle.
  @retval EFI_ABORTED        The function could not complete successfully.

**/
STATIC
EFI_STATUS
FtwRestart (
  IN EFI_FTW_LITE_DEVICE    *FtwLiteDevice
  )
{
  EFI_STATUS                          Status;
  EFI_FTW_LITE_RECORD                 *Record;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  EFI_DEV_PATH_PTR                    DevPathPtr;

  //
  // Spare Completed but Destination not complete,
  // Recover the targt block with the spare block.
  //
  Record = FtwLiteDevice->FtwLastRecord;

  //
  // Only support memory mapped FVB device path by now.
  //
  DevPathPtr.MemMap = (MEMMAP_DEVICE_PATH *) &Record->DevPath;
  if (!((DevPathPtr.MemMap->Header.Type == HARDWARE_DEVICE_PATH) && (DevPathPtr.MemMap->Header.SubType == HW_MEMMAP_DP))
      ) {
    DEBUG ((mFtwLiteError, "FtwLite: FVB Device Path is not memory mapped\n"));
    return EFI_ABORTED;
  }
  Fvb = NULL;
  if (mSmst == NULL) {
    Status = GetFvbByAddress (DevPathPtr.MemMap->StartingAddress, &Fvb);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  }
  //
  //  Since the content has already backuped in spare block, the write is
  //  guaranteed to be completed with fault tolerant manner.
  //
  Status = FtwWriteRecord (FtwLiteDevice, Fvb, FtwLiteDevice->WorkSpaceFvBaseAddr);
  DEBUG ((mFtwLiteInfo, "FtwLite: Restart() - %r\n", Status));

  Record++;
  FtwLiteDevice->FtwLastRecord = Record;

  //
  // Erase Spare block
  // This is restart, no need to keep spareblock content.
  //
  FtwEraseSpareBlock (FtwLiteDevice);

  return Status;
}

/**
  Aborts all previous allocated writes.

  @param  FtwLiteDevice      The private data of FTW_LITE driver.

  @retval EFI_SUCCESS        The function completed successfully.
  @retval EFI_ABORTED        The function could not complete successfully.
  @retval EFI_NOT_FOUND      No allocated writes exist.

**/
STATIC
EFI_STATUS
FtwAbort (
  IN EFI_FTW_LITE_DEVICE    *FtwLiteDevice
  )
{
  EFI_STATUS  Status;
  UINTN       Offset;

  if (FtwLiteDevice->FtwLastRecord->WriteCompleted == FTW_VALID_STATE) {
    return EFI_NOT_FOUND;
  }
  //
  // Update the complete state of the header as VALID and abort.
  //
  Offset = (UINT8 *) FtwLiteDevice->FtwLastRecord - FtwLiteDevice->FtwWorkSpace;
  Status = FtwUpdateFvState (
             FtwLiteDevice,
             FtwLiteDevice->FtwFvBlock,
             FtwLiteDevice->FtwWorkSpaceLba,
             FtwLiteDevice->FtwWorkSpaceBase + Offset,
             (UINTN) (FtwLiteDevice->WorkSpaceAddress + Offset),
             WRITE_COMPLETED
             );
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  FtwLiteDevice->FtwLastRecord->WriteCompleted  = FTW_VALID_STATE;

  Status = FtwGetLastRecord (FtwLiteDevice, &FtwLiteDevice->FtwLastRecord);

  //
  // Erase the spare block
  //
  Status = FtwEraseSpareBlock (FtwLiteDevice);

  DEBUG ((mFtwLiteInfo, "FtwLite: Abort() success \n"));
  return EFI_SUCCESS;
}

/**
  Allocate memory which used during reclaim process to prevent from allocating
  meory in runtime

  @param  FtwLiteDevice         Point to private data of FTW driver.

  @retval EFI_SUCCESS           Allocate memory successful.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory error.
  @retval EFI_ALREADY_STARTED   Reclaim memory has been initialized.

**/
EFI_STATUS
InitializeReclaimBuffer (
  IN EFI_FTW_LITE_DEVICE           *FtwLiteDevice
  )
{
  UINTN              Index;
  STATIC BOOLEAN     Initialized = FALSE;


  if (FtwLiteDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Initialized) {
    return EFI_ALREADY_STARTED;
  }
  FtwLiteDevice->ReclaimMemory = AllocateZeroRuntimeDataBuffer (sizeof (EFI_FTW_LITE_BUFFER) * MAX_MEMORY_NODE);
  ASSERT (FtwLiteDevice->ReclaimMemory != NULL);

  if (FtwLiteDevice->ReclaimMemory != NULL) {
    FtwLiteDevice->ReclaimMemory[0].BufferSize = FTW_LITE_RECORD_SIZE;
    FtwLiteDevice->ReclaimMemory[0].Buffer = AllocateZeroRuntimeDataBuffer (FTW_LITE_RECORD_SIZE);
    ASSERT (FtwLiteDevice->ReclaimMemory[0].Buffer != NULL);

    for (Index = 1; Index < MAX_MEMORY_NODE; Index++) {
      FtwLiteDevice->ReclaimMemory[Index].BufferSize = FtwLiteDevice->SpareAreaLength;
      FtwLiteDevice->ReclaimMemory[Index].Buffer = AllocateZeroRuntimeDataBuffer (FtwLiteDevice->SpareAreaLength);
      ASSERT (FtwLiteDevice->ReclaimMemory[Index].Buffer != NULL);
    }
  }

  Initialized = TRUE;
  return EFI_SUCCESS;

}

/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It convers pointer to new virtual address.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
FtwLiteAddressChangeEvent (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  )
{
  UINTN            Index;
  //
  // convert FtwLite function  pointer
  //
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwLiteInstance.Write);

  //
  // Convert relative MMIO base address
  //
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->WorkSpaceFvBaseAddr);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->SpareAreaFvBaseAddr);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->WorkBlockAddr);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->WorkSpaceAddress);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->SpareAreaAddress);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwWorkSpaceHeader);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwLastRecord);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwWorkSpace);

  //
  // covnert Fvb relative function pointers and instance
  //
  if (mFtwLiteDevice->FtwFvBlock != mFtwLiteDevice->FtwBackupFvb) {
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->GetBlockSize);
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->GetPhysicalAddress);
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->GetAttributes);
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->SetAttributes);
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->Read);
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->Write);
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb->EraseBlocks);
  }
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwBackupFvb);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->GetBlockSize);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->GetPhysicalAddress);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->GetAttributes);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->SetAttributes);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->Read);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->Write);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock->EraseBlocks);
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->FtwFvBlock);

  //
  // Convert Relcaim relative memory
  //
  for (Index = 0; Index < MAX_MEMORY_NODE; Index++) {
    gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->ReclaimMemory[Index].Buffer);
  }
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice->ReclaimMemory);

  //
  // Finally, convert FtwLiteDevice
  //
  gRT->ConvertPointer (0x0, (VOID **) &mFtwLiteDevice);

}


/**
  Add the EFI_MEMORY_RUNTIME memory attribute to input memory region.

  @param[in] BaseAddress    Input memory base address.
  @param[in] Length         Input memory size.

  @retval    EFI_SUCCESS    Add EFI_MEMORY_RUNTIME memory attribute successfully.
  @retval    other          Any other occurred while adding EFI_MEMORY_RUNTIME memory attribute.
**/
STATIC
EFI_STATUS
SetRuntimeMemoryAttribute (
  IN EFI_PHYSICAL_ADDRESS                BaseAddress,
  IN UINT64                              Length
  )
{
  EFI_STATUS                            Status;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR       GcdDescriptor;

  //
  // Mark the Flash part memory space as EFI_MEMORY_RUNTIME
  //
  BaseAddress = BaseAddress & (~EFI_PAGE_MASK);
  Length = (Length + EFI_PAGE_SIZE - 1) & (~EFI_PAGE_MASK);

  Status      = gDS->GetMemorySpaceDescriptor (BaseAddress, &GcdDescriptor);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  GcdDescriptor.Attributes | EFI_MEMORY_RUNTIME
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  This function is the entry point of the Fault Tolerant Write driver.

  @param  ImageHandle        EFI_HANDLE: A handle for the image that is initializing this driver
  @param  SystemTable        EFI_SYSTEM_TABLE: A pointer to the EFI system table.

  @retval EFI_SUCCESS        FTW has finished the initialization.
  @retval EFI_ABORTED        FTW initialization error.

**/
EFI_STATUS
EFIAPI
InitializeFtwLite (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  UINTN                               Index;
  EFI_HANDLE                          *HandleBuffer;
  UINTN                               HandleCount;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;
  EFI_PHYSICAL_ADDRESS                BaseAddress;
  EFI_FTW_LITE_DEVICE                 *FtwLiteDevice;
  EFI_FTW_LITE_RECORD                 *Record;
  UINTN                               Length;
  EFI_STATUS                          Status;
  UINTN                               Offset;
  EFI_FV_BLOCK_MAP_ENTRY              *FvbMapEntry;
  UINT32                              LbaIndex;
  EFI_LBA                             WorkSpaceLbaOffset;
  EFI_SMM_BASE2_PROTOCOL              *SmmBase;
  EFI_SMM_RUNTIME_PROTOCOL            *SmmRT;
  EFI_FTW_LITE_DEVICE                 *SmmFtwLiteDevice;
  EFI_FTW_LITE_DEVICE                 *SmmFtwLiteGlobal;
  EFI_FTW_LITE_PROTOCOL               *FtwLiteProtocol;
  BOOLEAN                             InSmm;
  BOOLEAN                             IsInReclaimProcess;
  EFI_HANDLE                          Handle;
  EFI_EVENT                           Event;

  InSmm        = FALSE;
  SmmBase      = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (!InSmm) {

    if (!IsRuntimeDriver (ImageHandle)) {
       Status = RelocateImageToRuntimeDriver (ImageHandle);
       ASSERT_EFI_ERROR (Status);
        //
        // We only want to load runtime services code to memory and don't load boot services code to memory,
        // so just return EFI_ALREADY_STARTED if it isn't a runtime driver.
        //
        return EFI_ALREADY_STARTED;
    }
    //
    // Great!!! Now, this is a runtime driver.
    //
    RuntimeDriverInitializeCrc32Table();

    //
    // Allocate Private data of this driver, INCLUDING THE FtwWorkSpace[FTW_WORK_SPACE_SIZE].
    //
    FtwLiteDevice = NULL;
    FtwLiteDevice = AllocateZeroRuntimeDataBuffer (sizeof (EFI_FTW_LITE_DEVICE) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwWorkingSize));
    ASSERT (FtwLiteDevice != NULL);
    if (FtwLiteDevice == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    mFtwLiteDevice = FtwLiteDevice;
    //
    // Initialize FtwLiteDevice, and set WorkSpace as FTW_ERASED_BYTE.
    //
    FtwLiteDevice->Signature = FTW_LITE_DEVICE_SIGNATURE;
    FtwLiteDevice->FtwWorkSpace     = (UINT8 *) (FtwLiteDevice + 1);
    FtwLiteDevice->FtwWorkSpaceSize = (UINTN) PcdGet32 (PcdFlashNvStorageFtwWorkingSize);
    SetMem (FtwLiteDevice->FtwWorkSpace, FtwLiteDevice->FtwWorkSpaceSize, FTW_ERASED_BYTE);
    FtwLiteDevice->FtwWorkSpaceHeader = (EFI_FAULT_TOLERANT_WORKING_BLOCK_HEADER *) FtwLiteDevice->FtwWorkSpace;

    FtwLiteDevice->FtwLastRecord      = NULL;

    //
    // Get the FTW work space Flash Map SUB area
    //
    FtwLiteDevice->WorkSpaceAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) (PcdGet64 (PcdFlashNvStorageFtwWorkingBase64) != 0 ?
                                                                      PcdGet64 (PcdFlashNvStorageFtwWorkingBase64) :
                                                                      PcdGet32 (PcdFlashNvStorageFtwWorkingBase)
                                                                      );
    FtwLiteDevice->WorkSpaceLength  = (UINTN) PcdGet32 (PcdFlashNvStorageFtwWorkingSize);
    Status = SetRuntimeMemoryAttribute (FtwLiteDevice->WorkSpaceAddress, (UINT64) FtwLiteDevice->WorkSpaceLength);
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
    //
    // Get the FTW backup SUB area
    //
    FtwLiteDevice->SpareAreaAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) (PcdGet64 (PcdFlashNvStorageFtwSpareBase64) != 0 ?
                                                                      PcdGet64 (PcdFlashNvStorageFtwSpareBase64) :
                                                                      PcdGet32 (PcdFlashNvStorageFtwSpareBase)
                                                                      );
    FtwLiteDevice->SpareAreaLength  = (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize);
    Status = SetRuntimeMemoryAttribute (FtwLiteDevice->SpareAreaAddress, (UINT64) FtwLiteDevice->SpareAreaLength);
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }

    ASSERT ((FtwLiteDevice->WorkSpaceLength != 0) && (FtwLiteDevice->SpareAreaLength != 0));

    //
    // Locate FVB protocol
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    ASSERT_EFI_ERROR (Status);
    ASSERT (HandleCount > 0);

    FtwLiteDevice->FtwFvBlock       = NULL;
    FtwLiteDevice->FtwBackupFvb     = NULL;
    FtwLiteDevice->FtwWorkSpaceLba  = (EFI_LBA) (-1);
    FtwLiteDevice->FtwSpareLba      = (EFI_LBA) (-1);
    for (Index = 0; Index < HandleCount; Index += 1) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiFirmwareVolumeBlockProtocolGuid,
                      (VOID **) &Fvb
                      );
      ASSERT_EFI_ERROR (Status);

      Status = Fvb->GetPhysicalAddress (Fvb, &BaseAddress);
      if (EFI_ERROR (Status)) {
        continue;
      }

      FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) BaseAddress);

      if ((FtwLiteDevice->WorkSpaceAddress >= BaseAddress) &&
          (FtwLiteDevice->WorkSpaceAddress < (BaseAddress + FwVolHeader->FvLength) && IsNvStorageHandle (HandleBuffer[Index]))
          ) {
        FtwLiteDevice->FtwFvBlock = Fvb;
        //
        // To get the LBA of work space
        //
        if ((FwVolHeader->FvLength) > (FwVolHeader->HeaderLength)) {
          //
          // FV may have multiple types of BlockLength
          //
          FvbMapEntry = &FwVolHeader->BlockMap[0];
          while (!((FvbMapEntry->NumBlocks == 0) && (FvbMapEntry->Length == 0))) {
            for (LbaIndex = 1; LbaIndex <= FvbMapEntry->NumBlocks; LbaIndex += 1) {
              if (FtwLiteDevice->WorkSpaceAddress < (BaseAddress + FvbMapEntry->Length * LbaIndex)) {
                FtwLiteDevice->FtwWorkSpaceLba = LbaIndex - 1;
                //
                // Get the Work space size and Base(Offset)
                //
                FtwLiteDevice->FtwWorkSpaceSize     = FtwLiteDevice->WorkSpaceLength;
                FtwLiteDevice->NumberOfFtwWorkBlock = FtwLiteDevice->FtwWorkSpaceSize / FvbMapEntry->Length;
                FtwLiteDevice->FtwWorkSpaceBase     = (UINTN) (FtwLiteDevice->WorkSpaceAddress - (BaseAddress + FvbMapEntry->Length * (LbaIndex - 1)));
                break;
              }
            }
            //
            // end for
            //
            FvbMapEntry++;
          }
          //
          // end while
          //
        }
      }

      if ((FtwLiteDevice->SpareAreaAddress >= BaseAddress) &&
          (FtwLiteDevice->SpareAreaAddress < (BaseAddress + FwVolHeader->FvLength) && IsNvStorageHandle (HandleBuffer[Index]))
          ) {
        FtwLiteDevice->FtwBackupFvb = Fvb;
        //
        // To get the LBA of spare
        //
        if ((FwVolHeader->FvLength) > (FwVolHeader->HeaderLength)) {
          //
          // FV may have multiple types of BlockLength
          //
          FvbMapEntry = &FwVolHeader->BlockMap[0];
          while (!((FvbMapEntry->NumBlocks == 0) && (FvbMapEntry->Length == 0))) {
            for (LbaIndex = 1; LbaIndex <= FvbMapEntry->NumBlocks; LbaIndex += 1) {
              if (FtwLiteDevice->SpareAreaAddress < (BaseAddress + FvbMapEntry->Length * LbaIndex)) {
                //
                // Get the NumberOfSpareBlock and SizeOfSpareBlock
                //
                FtwLiteDevice->FtwSpareLba        = LbaIndex - 1;
                FtwLiteDevice->SizeOfSpareBlock   = FvbMapEntry->Length;
                FtwLiteDevice->NumberOfSpareBlock = FtwLiteDevice->SpareAreaLength / FtwLiteDevice->SizeOfSpareBlock;
                //
                // Check the range of spare area to make sure that it's in FV range
                //
                ASSERT ((FtwLiteDevice->FtwSpareLba + FtwLiteDevice->NumberOfSpareBlock) <= FvbMapEntry->NumBlocks);
                break;
              }
            }

            FvbMapEntry++;
          }
          //
          // end while
          //
        }
      }
    }
    if (FtwLiteDevice->FtwBackupFvb == NULL || FtwLiteDevice->FtwFvBlock == NULL) {
      return EFI_ABORTED;
    }
    //
    // Calculate the start LBA of working block. Working block is an area which
    // contains working space in its last block and has the same size as spare
    // block, unless there are not enough blocks before the block that contains
    // working space.
    //
    FtwLiteDevice->FtwWorkBlockLba = FtwLiteDevice->FtwWorkSpaceLba - FtwLiteDevice->NumberOfSpareBlock + 1;
    if ((INT64) (FtwLiteDevice->FtwWorkBlockLba) < 0) {
      FtwLiteDevice->FtwWorkBlockLba = 0;
    }

    if ((FtwLiteDevice->FtwWorkSpaceLba == (EFI_LBA) (-1)) ||
        (FtwLiteDevice->FtwSpareLba == (EFI_LBA) (-1))
        ) {
      DEBUG ((EFI_D_ERROR, "FtwLite: Working or spare FVB not ready\n"));
      return EFI_ABORTED;
    }
    Status = InitializeReclaimBuffer (FtwLiteDevice);
    ASSERT_EFI_ERROR (Status);
    //
    // Initialize WorkSpaceFvBaseAddr for runtime use
    //
    Status = FtwLiteDevice->FtwFvBlock->GetPhysicalAddress (FtwLiteDevice->FtwFvBlock, &FtwLiteDevice->WorkSpaceFvBaseAddr);
    //
    // Refresh workspace data from working block
    //
    Status = WorkSpaceRefresh (FtwLiteDevice);
    ASSERT_EFI_ERROR (Status);

    IsInReclaimProcess = FALSE;
    //
    // If the working block workspace is not valid, try the spare block
    //
    if (!IsValidWorkSpace (FtwLiteDevice->FtwWorkSpaceHeader)) {
      DEBUG ((mFtwLiteError, "FtwLite: Workspace invalid, read from backup\n"));
      IsInReclaimProcess = TRUE;
      //
      // Read from spare block
      //
      WorkSpaceLbaOffset = FtwLiteDevice->FtwWorkSpaceLba - FtwLiteDevice->FtwWorkBlockLba;
      if (FtwLiteDevice->NumberOfFtwWorkBlock != 0) {
        for (Index = 0; Index < FtwLiteDevice->NumberOfFtwWorkBlock; Index++) {
          Length = FtwLiteDevice->FtwWorkSpaceSize / FtwLiteDevice->NumberOfFtwWorkBlock;
          Status = FtwLiteDevice->FtwBackupFvb->Read (
                                    FtwLiteDevice->FtwBackupFvb,
                                    FtwLiteDevice->FtwSpareLba + WorkSpaceLbaOffset + Index,
                                    FtwLiteDevice->FtwWorkSpaceBase,
                                    &Length,
                                    FtwLiteDevice->FtwWorkSpace + (Length * Index)
                                    );
        }
      } else {
        Length = FtwLiteDevice->FtwWorkSpaceSize;
        Status = FtwLiteDevice->FtwFvBlock->Read (
                                              FtwLiteDevice->FtwBackupFvb,
                                              FtwLiteDevice->FtwSpareLba,
                                              FtwLiteDevice->FtwWorkSpaceBase,
                                              &Length,
                                              FtwLiteDevice->FtwWorkSpace
                                              );
      }
      ASSERT_EFI_ERROR (Status);

      //
      // If spare block is valid, then replace working block content.
      //
      if (IsValidWorkSpace (FtwLiteDevice->FtwWorkSpaceHeader)) {
        Status = FlushSpareBlockToWorkingBlock (FtwLiteDevice);
        DEBUG ((mFtwLiteError, "FtwLite: Restart working block in Init() - %r\n", Status));
        ASSERT_EFI_ERROR (Status);

        FtwAbort (FtwLiteDevice);
        //
        // Refresh work space.
        //
        Status = WorkSpaceRefresh (FtwLiteDevice);
        if (EFI_ERROR (Status)) {
          return EFI_ABORTED;
        }
      } else {
        DEBUG ((mFtwLiteError, "FtwLite: Both are invalid, init workspace\n"));
        //
        // If both are invalid, then initialize work space.
        //
        SetMem (FtwLiteDevice->FtwWorkSpace, FtwLiteDevice->FtwWorkSpaceSize, FTW_ERASED_BYTE);
        InitWorkSpaceHeader (FtwLiteDevice->FtwWorkSpaceHeader);

        //
        // Need use reclaim to re-write Fault tolerant working block. if only write data,
        // it will fail
        //
        Status = FtwReclaimWorkSpace (FtwLiteDevice);

        if (EFI_ERROR (Status)) {
          return EFI_ABORTED;
        }
      }
    }
    //
    // Hook the protocol API
    //
    FtwLiteDevice->FtwLiteInstance.Write = FtwLiteWrite;

    //
    // Install protocol interface
    //
    Status = gBS->InstallProtocolInterface (
                    &FtwLiteDevice->Handle,
                    &gEfiFaultTolerantWriteLiteProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &FtwLiteDevice->FtwLiteInstance
                    );
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
    //
    // If (!SpareCompleted)  THEN  Abort to rollback.
    //
    if ((FtwLiteDevice->FtwLastRecord->WriteAllocated == FTW_VALID_STATE) &&
        (FtwLiteDevice->FtwLastRecord->SpareCompleted != FTW_VALID_STATE)
        ) {
      DEBUG ((mFtwLiteError, "FtwLite: Init.. record not SpareCompleted, abort()\n"));
      FtwAbort (FtwLiteDevice);
    }
    //
    // if (SpareCompleted) THEN  Restart to fault tolerant write.
    //
    if ((FtwLiteDevice->FtwLastRecord->SpareCompleted == FTW_VALID_STATE) &&
        (FtwLiteDevice->FtwLastRecord->WriteCompleted != FTW_VALID_STATE)
        ) {
      IsInReclaimProcess = TRUE;
      Status = FtwRestart (FtwLiteDevice);
      DEBUG ((mFtwLiteError, "FtwLite: Restart last write - %r\n", Status));
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // To check the workspace buffer behind last records is EMPTY or not.
    // If it's not EMPTY, FTW_LITE also need to call reclaim().
    //
    Record  = FtwLiteDevice->FtwLastRecord;
    Offset  = (UINT8 *) Record - FtwLiteDevice->FtwWorkSpace;
    if (FtwLiteDevice->FtwWorkSpace[Offset] != FTW_ERASED_BYTE) {
      Offset += WRITE_TOTAL_SIZE;
    }

    if (!IsErasedFlashBuffer (
          FTW_ERASE_POLARITY,
          FtwLiteDevice->FtwWorkSpace + Offset,
          FtwLiteDevice->FtwWorkSpaceSize - Offset
          )) {
      DEBUG ((mFtwLiteError, "FtwLite: Workspace is dirty, call reclaim...\n"));
      Status = FtwReclaimWorkSpace (FtwLiteDevice);
      if (EFI_ERROR (Status)) {
        DEBUG ((mFtwLiteError, "FtwLite: Workspace reclaim - %r\n", Status));
        return EFI_ABORTED;
      }
    }

    if (IsInReclaimProcess) {
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      return EFI_ABORTED;
    }
    //
    // Register the event to convert the pointer for runtime.
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    FtwLiteAddressChangeEvent,
                    NULL,
                    &gEfiEventVirtualAddressChangeGuid,
                    &Event
                    );
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // Don't output EFI debug message in SMM mode
    //
#ifndef MDEPKG_NDEBUG
    mFtwLiteInfo  = 0;
    mFtwLiteError = 0;
#endif
    RuntimeDriverInitializeCrc32Table();
    SmmRT = NULL;
    Status = gBS->LocateProtocol(&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
    if (!EFI_ERROR(Status)) {
      Status = SmmRT->LocateProtocol(&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
    }

    Status = SmmBase->GetSmstLocation(SmmBase, &mSmst);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = gBS->LocateProtocol (&gEfiFaultTolerantWriteLiteProtocolGuid, NULL, (VOID **)&FtwLiteProtocol);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    FtwLiteDevice = FTW_LITE_CONTEXT_FROM_THIS (FtwLiteProtocol);
    Status = gBS->AllocatePool (
                    EfiReservedMemoryType,
                    sizeof (EFI_FTW_LITE_DEVICE) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwWorkingSize),
                    (VOID **)&SmmFtwLiteDevice
                    );
    ASSERT_EFI_ERROR (Status);

    Status = FtwLiteDeviceInit (SmmFtwLiteDevice, FtwLiteDevice);
    Status = FtwLiteDevice->FtwFvBlock->GetPhysicalAddress (FtwLiteDevice->FtwFvBlock, &(SmmFtwLiteDevice->WorkSpaceFvBaseAddr));
    Status = FtwLiteDevice->FtwBackupFvb->GetPhysicalAddress (FtwLiteDevice->FtwBackupFvb, &(SmmFtwLiteDevice->SpareAreaFvBaseAddr));
    SmmFtwLiteDevice->WorkBlockAddr = SmmFtwLiteDevice->WorkSpaceFvBaseAddr +
                                      ((UINTN)SmmFtwLiteDevice->FtwWorkBlockLba * SmmFtwLiteDevice->SizeOfSpareBlock);
    SmmFtwLiteDevice->SmmFtwProtocol.Write = SmmFtwWrite;

    //
    // For backward compatible, Install protocol interface by  SmmRT
    //
    if (SmmRT != NULL) {
      Handle = NULL;
      Status = SmmRT->InstallProtocolInterface (
                        &Handle,
                        &gEfiSmmFtwProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &SmmFtwLiteDevice->SmmFtwProtocol
                      );
      if (EFI_ERROR (Status)) {
        return EFI_ABORTED;
      }
    }
    //
    // Use mSmst to install protocol for native consumer driver
    //
    SmmFtwLiteDevice->Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &SmmFtwLiteDevice->Handle,
                      &gEfiSmmFtwProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &SmmFtwLiteDevice->SmmFtwProtocol
                      );
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
    //
    //  Install the Protocol Interface in the Boot Time Space. This is requires to satisfy the
    //  dependency within the drivers that are dependent upon Smm Runtime Driver.
    //
    Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (EFI_FTW_LITE_DEVICE), (VOID **)&SmmFtwLiteGlobal);
    ASSERT_EFI_ERROR (Status);
    CopyMem (SmmFtwLiteGlobal, SmmFtwLiteDevice, sizeof (EFI_FTW_LITE_DEVICE));
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSmmFtwProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &SmmFtwLiteGlobal->SmmFtwProtocol
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
