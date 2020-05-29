/** @file
  This is a misc fucnitons for variable default update driver.

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


#include "VariableDefaultUpdate.h"


/**
  Check the data in factory copy sub-region header  is whether valid.

  @param  VariableDefaultDevice  Point to private data.

  @retval TRUE                   The data in facotry copy sub-region header is valid.
  @retval FALSE                  The data in facotry copy sub-region header is invalid.

**/
BOOLEAN
IsFacotryCopyHeaderValid (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice
  )
{
  FACTORY_DEFAULT_HEADER    *FactoryDefaultHeader;
  BOOLEAN                   IsHeaderValid;

   if (VariableDefaultDevice == NULL) {
    return FALSE;
  }

  FactoryDefaultHeader =  (FACTORY_DEFAULT_HEADER *) (UINTN) VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreAddress;

  IsHeaderValid = FALSE;
  if (FactoryDefaultHeader->Signature == FACTORY_DEFAULT_COPY_SIGNATURE &&
      FactoryDefaultHeader->Length == (UINT32) VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength) {
    IsHeaderValid = TRUE;
  }

  return IsHeaderValid;

}

/**
  Check the data in factory copy sub-region is whether valid.

  @param  VariableDefaultDevice  Point to private data.

  @retval TRUE                   The data in facotry copy region is valid.
  @retval FALSE                  The data in facotry copy region is invalid.

**/
BOOLEAN
IsFactoryCopyValid (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice
  )
{
  UINTN              *ReadBuffer;
  UINTN              NumberOfInteger;
  UINTN              Index;
  EFI_STATUS         Status;

  if (VariableDefaultDevice == NULL) {
    return FALSE;
  }

  if (!IsFacotryCopyHeaderValid (VariableDefaultDevice)) {
    return FALSE;
  }

  ReadBuffer = AllocateZeroPool (VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength);
  if (ReadBuffer == NULL) {
    return FALSE;
  }

  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength,
             FACTORY_COPY_STORE,
             0,
             (UINT8 *) ReadBuffer
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // If all of data in factory copy sub-region is 0xFF, we consider the region isn't initialized.
  // Otherwier, we consider this region is valid.
  //
  NumberOfInteger = VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength / sizeof (UINTN);

  for (Index = sizeof (FACTORY_DEFAULT_HEADER) / sizeof (UINTN); Index < NumberOfInteger; Index++) {
    if (ReadBuffer[Index] != (UINTN) (-1)) {
      return TRUE;
    }
  }

  return FALSE;

}

/**
  Check system state is whether in the restore factory default process

  @param  VariableDefaultDevice  Point to private data.

  @retval TRUE                   System is in resotre factory default process.
  @retval FALSE                  System isn't in resotre factory default process.

**/
BOOLEAN
InRestoreFactoryDefaultProcess (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice
  )
{
  FACTORY_DEFAULT_HEADER    *FactoryDefaultHeader;
  BOOLEAN                   InUpdateProcess;


  ASSERT (VariableDefaultDevice != NULL);

  FactoryDefaultHeader =  (FACTORY_DEFAULT_HEADER *) (UINTN) VariableDefaultDevice->StoreDev[SPARE_STORE].StoreAddress;

  InUpdateProcess = FALSE;
  if (FactoryDefaultHeader->Signature == FACTORY_DEFAULT_COPY_SIGNATURE &&
      FactoryDefaultHeader->Length == (UINT32) VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength) {
    InUpdateProcess = TRUE;
  }

  return InUpdateProcess;
}


/**
  Read fixed length data from specific variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  Length                  Number of bytes want to read.
  @param  DevIndex                Index for variable default update device.
  @param  Offset                  Start offset in this store
  @param  Buffer                  Buffer to put read data.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ABORTED             The function could not complete successfully
  @retval  EFI_INVALID_PARAMETER  Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
DefaultUpdateReadBlock (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 Length,
  IN  UINTN                                 DevIndex,
  IN  UINTN                                 Offset,
  OUT UINT8                                 *Buffer
  )
{
  UINTN                     Index;
  UINTN                     StartOffset;
  EFI_STATUS                Status;
  UINTN                     ReadLength;
  STORE_DEVICE_INSTANCE     *StoreDev;


  if (VariableDefaultDevice == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Length + Offset > VariableDefaultDevice->StoreDev[DevIndex].StoreLength) {
    return EFI_ABORTED;
  }
  StoreDev = &VariableDefaultDevice->StoreDev[DevIndex];
  Index = Offset / StoreDev->SizeofBlock;
  StartOffset = Offset % StoreDev->SizeofBlock;

  Status = EFI_SUCCESS;
  while (Length != 0) {
    ReadLength = StoreDev->SizeofBlock - StartOffset;
    if (Length < ReadLength) {
      ReadLength = Length;
    }
    Status = StoreDev->Fvb->Read (
                              StoreDev->Fvb,
                              StoreDev->StoreLba + Index,
                              StartOffset,
                              &ReadLength,
                              Buffer
                              );
    if (EFI_ERROR (Status)) {
      break;
    }
    Index++;
    Length -= ReadLength;
    StartOffset = 0;
    Buffer += ReadLength;
  }

  return Status;
}

/**
  Write fixed length data to specific variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  Length                  Number of bytes want to write.
  @param  DevIndex                Index for variable default update device.
  @param  Offset                  Start offset in this store
  @param  Buffer                  Buffer saved data want to write.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ABORTED             The function could not complete successfully
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
DefaultUpdateWriteBlock (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 Length,
  IN  UINTN                                 DevIndex,
  IN  UINTN                                 Offset,
  IN  UINT8                                 *Buffer
  )
{
  UINTN                     Index;
  UINTN                     StartOffset;
  EFI_STATUS                Status;
  UINTN                     WriteLength;
  STORE_DEVICE_INSTANCE     *StoreDev;
  UINTN                     TotalLength;
  UINT8                     *WorkingBuffer;
  UINT8                     *ReadBuffer;
  BOOLEAN                   WriteSuccess;
  UINTN                     TryWritetimes;

  if (VariableDefaultDevice == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StoreDev = &VariableDefaultDevice->StoreDev[DevIndex];
  if (Length + Offset > StoreDev->StoreLength) {
    return EFI_ABORTED;
  }

  ReadBuffer = AllocateZeroPool (Length);
  if (ReadBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TryWritetimes = 0;
  WriteSuccess  = FALSE;
  do {
    TotalLength = Length;
    WorkingBuffer = Buffer;
    StartOffset = Offset % StoreDev->SizeofBlock;
    Index = Offset / StoreDev->SizeofBlock;
    while (TotalLength != 0) {
      WriteLength = StoreDev->SizeofBlock - StartOffset;
      if (TotalLength < WriteLength) {
        WriteLength = TotalLength;
      }
      Status = StoreDev->Fvb->Write (
                                StoreDev->Fvb,
                                StoreDev->StoreLba + Index,
                                StartOffset,
                                &WriteLength,
                                WorkingBuffer
                                );
      if (EFI_ERROR (Status)) {
        break;
      }
      Index++;
      TotalLength -= WriteLength;
      StartOffset = 0;
      WorkingBuffer += WriteLength;
    }


    Status = DefaultUpdateReadBlock (
               VariableDefaultDevice,
               Length,
               DevIndex,
               Offset,
               ReadBuffer
               );

    if (!EFI_ERROR (Status) && CompareMem (Buffer, ReadBuffer, Length) == 0) {
      WriteSuccess = TRUE;
    }
    TryWritetimes++;
  } while (!WriteSuccess && TryWritetimes < MAX_TRY_ACCESS_FVB_TIMES);

  gBS->FreePool (ReadBuffer);
  return WriteSuccess ? EFI_SUCCESS : EFI_ABORTED;
}

/**
  This function uses to erase specifc variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  DevIndex                Index for variable default update device.
  @param  StartLba                Start Lba which want to erase.
  @param  NumOfBlock              Number of blocks want to erase.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
DefaultUpdateEraseBlock (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 DevIndex,
  IN  UINTN                                 StartLba,
  IN  UINTN                                 NumOfBlock
  )
{
  EFI_STATUS                              Status;
  UINTN                                   TryErasetimes;
  BOOLEAN                                 EraseSuccess;
  UINTN                                   TotalLength;
  UINTN                                   *Buffer;
  UINTN                                   NumberOfInteger;
  UINTN                                   DataOffset;
  UINTN                                   BlockLength;
  UINTN                                   Index;
  UINT8                                   *Ptr;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *Fvb;
  STORE_DEVICE_INSTANCE                   *StoreDev;

  if (VariableDefaultDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StoreDev = &VariableDefaultDevice->StoreDev[DevIndex];
  if (StartLba + NumOfBlock > StoreDev->NumberOfBlock) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // allocate pool for read data from block
  //
  TotalLength = NumOfBlock * StoreDev->SizeofBlock;
  Buffer = AllocateZeroPool (TotalLength);

  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  //
  // Start try to erase whole block and using do-while to make sure
  // at lease erase once.
  //
  Fvb = StoreDev->Fvb;
  EraseSuccess = FALSE;
  TryErasetimes = 0;

  do {
    //
    //Erase whole block
    //
    Status = Fvb->EraseBlocks (
                    Fvb,
                    StoreDev->StoreLba + StartLba,
                    NumOfBlock,
                    EFI_LBA_LIST_TERMINATOR
                    );

    if (EFI_ERROR (Status)) {
      gBS->FreePool (Buffer);
      return Status;
    }
    //
    // Read whole block
    //
    Ptr = (UINT8 *) Buffer;
    for (Index = 0; Index < NumOfBlock; Index++) {
      BlockLength = StoreDev->SizeofBlock;
      Status = Fvb->Read (
                      Fvb,
                      StoreDev->StoreLba + StartLba + Index,
                      0,
                      &BlockLength,
                      Ptr
                      );
      if (EFI_ERROR (Status)) {
        gBS->FreePool (Buffer);
        return Status;
      }
      Ptr += BlockLength;
    }
    //
    //Set Erase success as default
    //
    EraseSuccess = TRUE;
    NumberOfInteger = TotalLength / sizeof (UINTN);
    for (DataOffset = 0; DataOffset < NumberOfInteger; DataOffset++) {
      //
      // Find any data isn't 0xff indicates erase failed
      //
      if (Buffer[DataOffset] != (UINTN) (-1)) {
        EraseSuccess = FALSE;
        break;
      }
    }
    TryErasetimes++;
  } while (!EraseSuccess && TryErasetimes < MAX_TRY_ACCESS_FVB_TIMES);

  gBS->FreePool (Buffer);
  return EraseSuccess ? EFI_SUCCESS : EFI_ABORTED;
}

/**
  This function uses to erase specifc variable default update device.

  @param  VariableDefaultDevice   Point to private data.
  @param  State                   Variable default update state.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
SetDefaultUpdateState (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 State
  )
{
  UINT8                                 *FdcBuffer;
  FACTORY_DEFAULT_HEADER                FactoryDefaultHeader;
  STORE_DEVICE_INSTANCE                 *StoreDev;
  UINTN                                 WriteSize;

  if (VariableDefaultDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  FdcBuffer = NULL;
  WriteSize = 0;
  switch (State) {

  case FACTORY_DEFAULT_STATE:
    FdcBuffer = (UINT8 *) &FactoryDefaultHeader;
    WriteSize = sizeof (FactoryDefaultHeader);
    SetMem (FdcBuffer, sizeof (FactoryDefaultHeader), 0xFF);
    StoreDev = &VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE];
    FactoryDefaultHeader.Signature = FACTORY_DEFAULT_COPY_SIGNATURE;
    FactoryDefaultHeader.Length = (UINT32) StoreDev->StoreLength;
    break;

  case BACKUP_DEFAULT_STATE:
    break;


  default:
    DEBUG ((EFI_D_ERROR, "Input variable default update state is invalid\n"));
    ASSERT (FALSE);
    break;
  }

  return DefaultUpdateWriteBlock (
           VariableDefaultDevice,
           WriteSize,
           SPARE_STORE,
           0,
           FdcBuffer
           );
}

/**
  This function uses to restore data in factory default sub-region to variable
  store region.

  @param  VariableDefaultDevice   Point to private data.
  @param  DataLength              The size in bytes of the Data buffer.
  @param  Data                    The contents for the variable store.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
RestoreFactoryDefaultToVariableStore (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice,
  IN  UINTN                                 DataSize,
  IN  UINT8                                 *Data
  )
{
  EFI_STATUS                  Status;
  VARIABLE_STORE_HEADER       *VariableHeader;
  ECP_VARIABLE_STORE_HEADER   *EcpVariableHeader;
  UINTN                       FvHeaderSize;
  BOOLEAN                     IsVariableState;
  BOOLEAN                     WriteSuccess;
  UINTN                       TryWritetimes;

  if (VariableDefaultDevice == NULL || DataSize == 0 || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TryWritetimes = 0;
  WriteSuccess = FALSE;
  Status = EFI_ABORTED;
  do {
    Status = DefaultUpdateEraseBlock (VariableDefaultDevice, VARIABLE_STORE, 0, VariableDefaultDevice->StoreDev[VARIABLE_STORE].NumberOfBlock);
    if (EFI_ERROR (Status)) {
      break;
    }
    //
    // Use the state of VARIABLE_STORE_HEADER to check the variable is health or not.
    // Setting the state of VARIABLE_STORE_HEADER to VARIABLE_STORE_HEALTHY, after
    // whole data has been written to NV storage.
    //
    FvHeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY);
    VariableHeader = NULL;
    EcpVariableHeader = NULL;
    IsVariableState = FALSE;

    //
    // Clear the state of VARIABLE_STORE_HEADER
    //
    if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
      EcpVariableHeader = (ECP_VARIABLE_STORE_HEADER *) (Data + FvHeaderSize);
      if (EcpVariableHeader->State == VARIABLE_STORE_HEALTHY) {
        IsVariableState = TRUE;
        EcpVariableHeader->State = 0xFF;
      }
    } else {
      VariableHeader = (VARIABLE_STORE_HEADER *) (Data + FvHeaderSize);
      if (VariableHeader->State == VARIABLE_STORE_HEALTHY) {
        IsVariableState = TRUE;
        VariableHeader->State = 0xFF;
      }
    }

    Status = DefaultUpdateWriteBlock (
               VariableDefaultDevice,
               DataSize,
               VARIABLE_STORE,
               0,
               Data
               );


    if (IsVariableState) {
      if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
        EcpVariableHeader->State = VARIABLE_STORE_HEALTHY;
        Status = DefaultUpdateWriteBlock (
                   VariableDefaultDevice,
                   sizeof (EcpVariableHeader->State),
                   VARIABLE_STORE,
                   FvHeaderSize + sizeof (UINT32) + sizeof (UINT32) + sizeof (UINT8),
                   &EcpVariableHeader->State
                   );
      } else {
        VariableHeader->State = VARIABLE_STORE_HEALTHY;
        Status = DefaultUpdateWriteBlock (
                   VariableDefaultDevice,
                   sizeof (VariableHeader->State),
                   VARIABLE_STORE,
                   FvHeaderSize + sizeof (EFI_GUID) + sizeof (UINT32) + sizeof (UINT8),
                   &VariableHeader->State
                   );
      }
    }

    if (!EFI_ERROR (Status)) {
      WriteSuccess = TRUE;
    }
    TryWritetimes++;
  } while (!WriteSuccess && TryWritetimes < MAX_TRY_ACCESS_FVB_TIMES);

  return Status;

}

/**
  This function uses to recoery factory default sub-region.

  @param  VariableDefaultDevice   Point to private data.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.

**/
EFI_STATUS
RecoveryFactoryDefault (
  IN  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice
  )
{
  EFI_STATUS                            Status;
  UINTN                                 TryWritetimes;
  BOOLEAN                               WriteSuccess;
  STORE_DEVICE_INSTANCE                 *StoreDev;
  FACTORY_DEFAULT_HEADER                FactoryDefaultHeader;

  if (VariableDefaultDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StoreDev = &VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE];
  FactoryDefaultHeader.Signature = FACTORY_DEFAULT_COPY_SIGNATURE;
  FactoryDefaultHeader.Length = (UINT32) StoreDev->StoreLength;

  TryWritetimes = 0;
  WriteSuccess = FALSE;
  do {
    Status = DefaultUpdateEraseBlock (VariableDefaultDevice, FACTORY_COPY_STORE, 0, StoreDev->NumberOfBlock);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = DefaultUpdateWriteBlock (
               VariableDefaultDevice,
               sizeof (FactoryDefaultHeader),
               FACTORY_COPY_STORE,
               0,
               (UINT8 *) &FactoryDefaultHeader
               );
    if (!EFI_ERROR (Status)) {
      WriteSuccess = TRUE;
    }
    TryWritetimes++;
  } while (!WriteSuccess && TryWritetimes < MAX_TRY_ACCESS_FVB_TIMES);

  return WriteSuccess ? EFI_SUCCESS : EFI_ABORTED;
}

/**
  This funciotn uses to export factory default setting to input buffer.

  @param  VariableDefaultDevice   Point to private data.
  @param  DataSize                On input, the size in bytes of the return Data buffer.
                                  On output the size of data returned in Data buffer.
  @param  Data                    The buffer to return the contents of the variable.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL    The VariableStoreLength is too small for the result.  DataSize has
                                  been updated with the size needed to complete the request.
  @return Other                   Other error cause merge factory default setting failed.

**/
EFI_STATUS
ExportFactoryDefaultSetting (
  IN     VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice,
  IN OUT UINTN                                *DataSize,
  OUT    UINT8                                *Data
  )
{

  UINTN                                 HeaderSize;
  UINT8                                 *HeaderBuffer;
  UINT8                                 *FactoryDefaultBuffer;
  EFI_STATUS                            Status;
  UINTN                                 RequiredDataSize;

  if (VariableDefaultDevice == NULL || DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RequiredDataSize = VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength - sizeof (FACTORY_DEFAULT_HEADER);
  if (*DataSize < RequiredDataSize) {
    *DataSize = RequiredDataSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  HeaderBuffer = NULL;
  FactoryDefaultBuffer = NULL;
  //
  // Read original firmware volume header and variable store header
  //
  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  HeaderBuffer = AllocateZeroPool (HeaderSize);
  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             HeaderSize,
             VARIABLE_STORE,
             0,
             HeaderBuffer
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Read whole factory store data except for factory default header, firmware volume header and variable store header.
  //
  FactoryDefaultBuffer = AllocateZeroPool (RequiredDataSize - HeaderSize);
  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             RequiredDataSize - HeaderSize,
             FACTORY_COPY_STORE,
             sizeof (FACTORY_DEFAULT_HEADER) + HeaderSize,
             FactoryDefaultBuffer
             );

  if (EFI_ERROR (Status)) {
    goto Done;
  }

  CopyMem (Data, HeaderBuffer, HeaderSize);
  CopyMem (Data + HeaderSize, FactoryDefaultBuffer, RequiredDataSize - HeaderSize);
  *DataSize = RequiredDataSize;

Done:

  if (FactoryDefaultBuffer != NULL) {
    gBS->FreePool (FactoryDefaultBuffer);
  }

  if (HeaderBuffer != NULL) {
    gBS->FreePool (HeaderBuffer);
  }

  return Status;
}


/**
  This funciotn uses to export merged factory default setting. The merged factory copy
  default setting means keep all of original variable data and then merge variable data
  in factory copy region with original data. If finding the same variable, system will
  use the variable in factory default to replace oringnal variable.

  @param  VariableDefaultDevice   Point to private data.
  @param  DataSize                On input, the size in bytes of the return Data buffer.
                                  On output the size of data returned in Data buffer.
  @param  Data                    The buffer to return the contents of the variable.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL    The VariableStoreLength is too small for the result. DataSize has
                                  been updated with the size needed to complete the request.
  @return Other                   Other error cause merge factory default setting failed.

**/
EFI_STATUS
ExportMergedFactoryDefaultSetting (
  IN     VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice,
  IN OUT UINTN                                *DataSize,
  OUT    UINT8                                *Data
  )
{
  UINTN                                 VariableBufferLength;
  UINTN                                 WorkingBufferLength;
  UINT8                                 *VariableBuffer;
  UINT8                                 *WorkingBuffer;
  EFI_STATUS                            Status;

  if (VariableDefaultDevice == NULL || DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  WorkingBuffer       = NULL;
  VariableBuffer      = NULL;
  WorkingBufferLength = 0;

  ASSERT (VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength <= VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength);
  //
  // Read whole factory copy sub-region data
  //
  WorkingBufferLength = VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength;
  WorkingBuffer = AllocateZeroPool (WorkingBufferLength);
  SetMem (WorkingBuffer, WorkingBufferLength, 0xFF);
  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             VariableDefaultDevice->StoreDev[FACTORY_COPY_STORE].StoreLength - sizeof (FACTORY_DEFAULT_HEADER),
             FACTORY_COPY_STORE,
             sizeof (FACTORY_DEFAULT_HEADER),
             WorkingBuffer
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Read whole variable store data
  //
  VariableBufferLength = VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength;
  VariableBuffer = AllocateZeroPool (VariableBufferLength);
  SetMem (VariableBuffer, VariableBufferLength, 0xFF);
  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength,
             VARIABLE_STORE,
             0,
             VariableBuffer
             );

  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = MergeVariableToFactoryCopy (WorkingBuffer, &WorkingBufferLength, VariableBuffer, VariableBufferLength);

Done:

  if (!EFI_ERROR (Status)) {
    if (WorkingBufferLength <= *DataSize) {
      CopyMem (Data, WorkingBuffer, WorkingBufferLength);
    } else {
      Status = EFI_BUFFER_TOO_SMALL;
    }
    *DataSize = WorkingBufferLength;
  }
  if (WorkingBuffer != NULL) {
    gBS->FreePool (WorkingBuffer);
  }

  if (VariableBuffer != NULL) {
    gBS->FreePool (VariableBuffer);
  }

  return Status;


}

/**
  According to restore type to collect factory default settings and restore these settings
  in output buffer.

  @param  VariableDefaultDevice   Point to private data.
  @param  RestoreType             Restore type to update for the variable store.
  @param  VariableStoreLength     On input, the size in bytes of the return VariableBuffer.
                                  On output the size of data returned in VariableBuffer.
  @param  VariableBuffer          The buffer to return the contents of the variable.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_ABORTED             The function could not complete successfully.
  @retval EFI_INVALID_PARAMETER   Some input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate meory failed or the range is out of the input device.
  @retval EFI_BUFFER_TOO_SMALL    The VariableStoreLength is too small for the result.  VariableStoreLength has
                                  been updated with the size needed to complete the request.
**/
EFI_STATUS
CollectFactoryDefaultSetting (
  IN     VARIABLE_DEFAULT_UPDATE_DEVICE       *VariableDefaultDevice,
  IN     UINT32                               RestoreType,
  IN OUT UINTN                                *VariableStoreLength,
  OUT    UINT8                                *VariableBuffer
  )
{
  EFI_STATUS                            Status;

  if (VariableDefaultDevice == NULL || VariableStoreLength == NULL || VariableBuffer == NULL ||
      (RestoreType != RESTORE_WITH_CLEARING_ALL_SETTINGS && RestoreType != RESTORE_WITH_RESERVING_OTHER_SETTINGS)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsFactoryCopyValid (VariableDefaultDevice)) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_BUFFER_TOO_SMALL;
  switch (RestoreType) {

  case RESTORE_WITH_CLEARING_ALL_SETTINGS:
    Status = ExportFactoryDefaultSetting (VariableDefaultDevice, VariableStoreLength, VariableBuffer);
    break;

  case RESTORE_WITH_RESERVING_OTHER_SETTINGS:
    Status = ExportMergedFactoryDefaultSetting (VariableDefaultDevice, VariableStoreLength, VariableBuffer);
    break;

  }

  return Status;
}

/**
  This function uses backup data to update system setting to factory default.

  @param  This                    Pointer to EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL instance.

  @retval EFI_INVALID_PARAMETER   Input parameter is invalid.
  @retval EFI_SUCCESS             Update system setting to factory default successful.
  @return Other                   Other error cause update system to factory default failed.

**/
EFI_STATUS
UpdateFactorySettingFromBackup (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This
  )
{
  VARIABLE_DEFAULT_UPDATE_DEVICE        *VariableDefaultDevice;
  UINT8                                 *SpareBuffer;
  EFI_STATUS                            Status;
  UINTN                                 Offset;
  UINTN                                 WriteSize;


  if (This == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  VariableDefaultDevice = VARIABLE_DEFAULT_INSTANCE_FROM_THIS (This);

  //
  // Check system is whether in factory resotre process. System only execute
  // restore factory copy from bakcup if system is factory restore process.
  //
  if (!InRestoreFactoryDefaultProcess (VariableDefaultDevice)) {
    return EFI_ABORTED;;
  }

  SpareBuffer    = NULL;
  WriteSize = VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength;
  SpareBuffer = AllocateZeroPool (VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength);
  if (SpareBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }


  //
  // 1.Read backup factory default setting from spare
  //
  Status = DefaultUpdateReadBlock (
             VariableDefaultDevice,
             VariableDefaultDevice->StoreDev[SPARE_STORE].StoreLength,
             SPARE_STORE,
             0,
             SpareBuffer
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  for (Offset = WriteSize; Offset > 0; Offset--) {
    if (SpareBuffer[Offset - 1] != 0xFF) {
      WriteSize = Offset;
      break;
    }
  }

  if (WriteSize > VariableDefaultDevice->StoreDev[VARIABLE_STORE].StoreLength + sizeof (FACTORY_DEFAULT_HEADER) ||
      WriteSize < sizeof (FACTORY_DEFAULT_HEADER)) {
    ASSERT (FALSE);
    Status = EFI_ABORTED;
    goto Done;
  }

  //
  // 2.Write backup factory default setting to variable store.
  //
  Status = RestoreFactoryDefaultToVariableStore (
             VariableDefaultDevice,
             WriteSize - sizeof (FACTORY_DEFAULT_HEADER),
             SpareBuffer + sizeof (FACTORY_DEFAULT_HEADER)
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

Done:

  //
  // Clear all of data in spare store before exiting this funciotn to prevent from system will enter this function every POST.
  //
  DefaultUpdateEraseBlock (
    VariableDefaultDevice,
    SPARE_STORE,
    0,
    VariableDefaultDevice->StoreDev[SPARE_STORE].NumberOfBlock
    );
  if (SpareBuffer != NULL) {
    gBS->FreePool (SpareBuffer);
  }

  return Status;
}