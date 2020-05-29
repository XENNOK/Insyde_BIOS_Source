/** @file
  Implementation for Firmware Volume Block Protocol.

  According to PcdFvbAccessThroughSmi to determine accessing firmware through SMI.
  If this feature PCD is TRUE, accessing firmware volume through SMI.
  If this feature PCD is FALSE, accesing firmware volume in protected mode directly.

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
#include "FvbServicesRuntimeDxe.h"
#include "RuntimeFunctions.h"
#include "CommonFunctions.h"
#include "SmmFunctions.h"

#define EFI_FVB2_STATUS (EFI_FVB2_READ_STATUS | EFI_FVB2_WRITE_STATUS | EFI_FVB2_LOCK_STATUS)

ESAL_FWB_GLOBAL         *mFvbModuleGlobal;
EFI_SMM_SYSTEM_TABLE2   *mSmst;


/**
  Retrieves the physical address of a memory mapped FV.

  @param [in]  Instance          The FV instance whose base address is going to be
                                 returned.
  @param [in]  Global            Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param [out] FwhInstance       The EFI_FW_VOL_INSTANCE fimrware instance structure.
  @param [in]  Virtual           Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            Successfully returns.
  @retval EFI_INVALID_PARAMETER  Input Instance is too large or FwhInstance is NULL.
  @retval EFI_NOT_FOUND          Can not find correct FWH instance.
**/
STATIC
EFI_STATUS
GetFvbInstance (
  IN  UINTN                               Instance,
  IN  ESAL_FWB_GLOBAL                     *Global,
  OUT EFI_FW_VOL_INSTANCE                 **FwhInstance,
  IN BOOLEAN                              Virtual
  )
{
  EFI_FW_VOL_INSTANCE *FwhRecord;

  if (Instance >= Global->NumFv || FwhInstance == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find the right instance of the FVB private data
  //
  FwhRecord = Global->FvInstance[Virtual ? 1 : 0];

  while (Instance > 0) {
    FwhRecord = GetNextFwhInstance (FwhRecord);
    Instance--;
  }
  if (FwhRecord == NULL) {
    return EFI_NOT_FOUND;
  }

  *FwhInstance = FwhRecord;
  return EFI_SUCCESS;
}

/**
  Retrieves the physical address of a memory mapped FV.

  @param[in]  Instance           The FV instance whose base address is going to be
                                 returned.
  @param[out] Address            Pointer to a caller allocated EFI_PHYSICAL_ADDRESS
                                 that on successful return, contains the base address
                                 of the firmware volume.
  @param[in]  Global             Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param[in]  Virtual            Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            Successfully returns.
  @retval EFI_INVALID_PARAMETER  Instance not found.
**/
STATIC
EFI_STATUS
FvbGetPhysicalAddress (
  IN UINTN                                Instance,
  OUT EFI_PHYSICAL_ADDRESS                *Address,
  IN ESAL_FWB_GLOBAL                      *Global,
  IN BOOLEAN                              Virtual
  )
{
  EFI_FW_VOL_INSTANCE *FwhInstance;
  EFI_STATUS          Status;

  //
  // Find the right instance of the FVB private data
  //
  Status = GetFvbInstance (Instance, Global, &FwhInstance, Virtual);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *Address = FwhInstance->FvBase[Virtual ? 1 : 0];

  return EFI_SUCCESS;
}

/**
  Retrieves attributes, insures positive polarity of attribute bits, returns
  resulting attributes in output parameter.

  @param[in]  Instance           The FV instance whose attributes is going to be
                                 returned.
  @param[out] Attributes         Output buffer which contains attributes.
  @param[in]  Global             Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param[in]  Virtual            Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            Successfully returns.
  @retval EFI_INVALID_PARAMETER  Instance not found.
**/
STATIC
EFI_STATUS
FvbGetVolumeAttributes (
  IN UINTN                                Instance,
  OUT EFI_FVB_ATTRIBUTES_2                *Attributes,
  IN ESAL_FWB_GLOBAL                      *Global,
  IN BOOLEAN                              Virtual
  )
{
  EFI_FW_VOL_INSTANCE *FwhInstance;
  EFI_STATUS          Status;

  //
  // Find the right instance of the FVB private data
  //
  Status = GetFvbInstance (Instance, Global, &FwhInstance, Virtual);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *Attributes = FwhInstance->VolumeHeader.Attributes;

  return EFI_SUCCESS;
}

/**
  Retrieves the starting address of an LBA in an FV.

  @param[in]  Instance           The FV instance which the Lba belongs to.
  @param[in]  Lba                The logical block address.
  @param[out] LbaAddress         On output, contains the physical starting address
                                 of the Lba.
  @param[out] LbaLength          On output, contains the length of the block.
  @param[out] NumOfBlocks        A pointer to a caller allocated UINTN in which the
                                 number of consecutive blocks starting with Lba is
                                 returned. All blocks in this range have a size of
                                 BlockSize.
  @param[in]  Global             Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param[in]  Virtual            Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            Successfully returns.
  @retval EFI_INVALID_PARAMETER  Instance not found.
**/
STATIC
EFI_STATUS
FvbGetLbaAddress (
  IN  UINTN                               Instance,
  IN  EFI_LBA                             Lba,
  OUT UINTN                               *LbaAddress,
  OUT UINTN                               *LbaLength,
  OUT UINTN                               *NumOfBlocks,
  IN  ESAL_FWB_GLOBAL                     *Global,
  IN  BOOLEAN                             Virtual
  )
{
  UINT32                  NumBlocks;
  UINT32                  BlockLength;
  UINTN                   Offset;
  EFI_LBA                 StartLba;
  EFI_LBA                 NextLba;
  EFI_FW_VOL_INSTANCE     *FwhInstance;
  EFI_FV_BLOCK_MAP_ENTRY  *BlockMap;
  EFI_STATUS              Status;

  //
  // Find the right instance of the FVB private data
  //
  Status = GetFvbInstance (Instance, Global, &FwhInstance, GoneVirtual ());
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  StartLba  = 0;
  Offset    = 0;
  BlockMap  = &(FwhInstance->VolumeHeader.BlockMap[0]);

  //
  // Parse the blockmap of the FV to find which map entry the Lba belongs to
  //
  while (TRUE) {
    NumBlocks   = BlockMap->NumBlocks;
    BlockLength = BlockMap->Length;

    if (NumBlocks == 0 || BlockLength == 0) {
      return EFI_INVALID_PARAMETER;
    }

    NextLba = StartLba + NumBlocks;

    //
    // The map entry found
    //
    if (Lba >= StartLba && Lba < NextLba) {
      Offset = Offset + (UINTN) MultU64x32 ((Lba - StartLba), BlockLength);
      if (LbaAddress != NULL) {
        *LbaAddress = FwhInstance->FvBase[Virtual] + Offset;
      }

      if (LbaLength != NULL) {
        *LbaLength = BlockLength;
      }

      if (NumOfBlocks != NULL) {
        *NumOfBlocks = (UINTN) (NextLba - Lba);
      }

      return EFI_SUCCESS;
    }

    StartLba  = NextLba;
    Offset    = Offset + NumBlocks * BlockLength;
    BlockMap++;
  }
}

/**
  Reads specified number of bytes into a buffer from the specified block.

  @param[in]      Instance       The FV instance to be read from.
  @param[in]      Lba            The logical block address to be read from.
  @param[in]      BlockOffset    Offset into the block at which to begin reading.
  @param[in, out] NumBytes       Pointer that on input contains the total size of
                                 the buffer. On output, it contains the total number
                                 of bytes read.
  @param[in]      Buffer         Pointer to a caller allocated buffer that will be
                                 used to hold the data read.
  @param[in]      Global         Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param[in]      Virtual        Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            The firmware volume was read successfully and
                                 contents are in Buffer.
  @retval EFI_BAD_BUFFER_SIZE    Read attempted across a LBA boundary. On output,
                                 NumBytes contains the total number of bytes returned
                                 in Buffer.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the ReadDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be read.
  @retval EFI_INVALID_PARAMETER  Instance not found, or NumBytes, Buffer are NULL.
**/
STATIC
EFI_STATUS
FvbReadBlock (
  IN UINTN                                Instance,
  IN EFI_LBA                              Lba,
  IN UINTN                                BlockOffset,
  IN OUT UINTN                            *NumBytes,
  IN UINT8                                *Buffer,
  IN ESAL_FWB_GLOBAL                      *Global,
  IN BOOLEAN                              Virtual
  )
{
  EFI_FVB_ATTRIBUTES_2  Attributes;
  UINTN                 LbaAddress;
  UINTN                 LbaLength;
  EFI_STATUS            Status;

  //
  // Check for invalid conditions
  //
  if ((NumBytes == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*NumBytes == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get physical address if want to accessing FVB through SMI, due to system uses physical address to access memory.
  //
  Status = FvbGetLbaAddress (Instance, Lba, &LbaAddress, &LbaLength, NULL, Global, IsFvbAccessThroughSmi () ? FALSE: Virtual);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check if the FV is read enabled
  //
  FvbGetVolumeAttributes (Instance, &Attributes, Global, Virtual);

  if ((Attributes & EFI_FVB2_READ_STATUS) == 0) {
    return EFI_ACCESS_DENIED;
  }
  //
  // Perform boundary checks and adjust NumBytes
  //
  if (BlockOffset > LbaLength) {
    return EFI_INVALID_PARAMETER;
  }

  if (LbaLength < (*NumBytes + BlockOffset)) {
    *NumBytes = (UINT32) (LbaLength - BlockOffset);
    Status    = EFI_BAD_BUFFER_SIZE;
  }

  return CommonFlashRead (LbaAddress + BlockOffset, NumBytes, (VOID *) Buffer);
}

/**
  Writes specified number of bytes from the input buffer to the block.

  @param [in]      Instance      The FV instance to be written to.
  @param [in]      Lba           The starting logical block index to write to.
  @param [in]      BlockOffset   Offset into the block at which to begin writing.
  @param [in, out] NumBytes      Pointer that on input contains the total size of
                                 the buffer. On output, it contains the total number
                                 of bytes actually written.
  @param [in]      Buffer        Pointer to a caller allocated buffer that contains
                                 the source for the write.
  @param [in]      Global        Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param [in]      Virtual       Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            The firmware volume was written successfully.
  @retval EFI_BAD_BUFFER_SIZE    Write attempted across a LBA boundary. On output,
                                 NumBytes contains the total number of bytes
                                 actually written.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be written.
  @retval EFI_INVALID_PARAMETER  Instance not found, or NumBytes, Buffer are NULL.
**/
STATIC
EFI_STATUS
FvbWriteBlock (
  IN UINTN                                Instance,
  IN EFI_LBA                              Lba,
  IN UINTN                                BlockOffset,
  IN OUT UINTN                            *NumBytes,
  IN UINT8                                *Buffer,
  IN ESAL_FWB_GLOBAL                      *Global,
  IN BOOLEAN                              Virtual
  )
{
  EFI_FVB_ATTRIBUTES_2  Attributes;
  UINTN                 LbaAddress;
  UINTN                 LbaLength;
  EFI_STATUS            Status;

  //
  // Check for invalid conditions
  //
  if ((NumBytes == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*NumBytes == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get physical address if want to accessing FVB through SMI, due to system uses physical address to access memory.
  //
  Status = FvbGetLbaAddress (Instance, Lba, &LbaAddress, &LbaLength, NULL, Global, IsFvbAccessThroughSmi () ? FALSE: Virtual);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check if the FV is write enabled
  //
  FvbGetVolumeAttributes (Instance, &Attributes, Global, Virtual);

  if ((Attributes & EFI_FVB2_WRITE_STATUS) == 0) {
    return EFI_ACCESS_DENIED;
  }
  //
  // Perform boundary checks and adjust NumBytes
  //
  if (BlockOffset > LbaLength) {
    return EFI_INVALID_PARAMETER;
  }

  if (LbaLength < (*NumBytes + BlockOffset)) {
    *NumBytes = (UINT32) (LbaLength - BlockOffset);
    Status    = EFI_BAD_BUFFER_SIZE;
  }

  return CommonFlashWrite (LbaAddress + BlockOffset, NumBytes, Buffer);
}

/**
  Erases and initializes a firmware volume block.

  @param[in] Instance            The FV instance to be erased.
  @param[in] Lba                 The logical block index to be erased.
  @param[in] Global              Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param[in] Virtual             Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            The erase request was successfully completed.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be written. Firmware device may have been
                                 partially erased.
  @retval EFI_INVALID_PARAMETER  Instance not found.
**/
STATIC
EFI_STATUS
FvbEraseBlock (
  IN UINTN                                Instance,
  IN EFI_LBA                              Lba,
  IN ESAL_FWB_GLOBAL                      *Global,
  IN BOOLEAN                              Virtual
  )
{

  EFI_FVB_ATTRIBUTES_2  Attributes;
  UINTN                 LbaAddress;
  UINTN                 LbaLength;
  EFI_STATUS            Status;

  //
  // Check if the FV is write enabled
  //
  FvbGetVolumeAttributes (Instance, &Attributes, Global, Virtual);

  if ((Attributes & EFI_FVB2_WRITE_STATUS) == 0) {
    return EFI_ACCESS_DENIED;
  }
  //
  // Get physical address if want to accessing FVB through SMI, due to system uses physical address to access memory.
  //
  Status = FvbGetLbaAddress (Instance, Lba, &LbaAddress, &LbaLength, NULL, Global, IsFvbAccessThroughSmi () ? FALSE: Virtual);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return CommonFlashErase (LbaAddress, LbaLength);
}

/**
  Modifies the current settings of the firmware volume according to the
  input parameter, and returns the new setting of the volume.

  @param[in]      Instance       The FV instance whose attributes is going to be
                                 modified.
  @param[in, out] Attributes     On input, it is a pointer to EFI_FVB_ATTRIBUTES_2
                                 containing the desired firmware volume settings.
                                 On successful return, it contains the new settings
                                 of the firmware volume.
  @param[in]      Global         Pointer to ESAL_FWB_GLOBAL that contains all
                                 instance data.
  @param[in]      Virtual        Whether CPU is in virtual or physical mode.

  @retval EFI_SUCCESS            Successfully returns.
  @retval EFI_ACCESS_DENIED      The volume setting is locked and cannot be modified.
  @retval EFI_INVALID_PARAMETER  Instance not found, or The attributes requested are
                                 in conflict with the capabilities as declared in the
                                 firmware volume header.
**/
EFI_STATUS
FvbSetVolumeAttributes (
  IN UINTN                                  Instance,
  IN OUT EFI_FVB_ATTRIBUTES_2               *Attributes,
  IN ESAL_FWB_GLOBAL                        *Global,
  IN BOOLEAN                                Virtual
  )
{
  EFI_FW_VOL_INSTANCE   *FwhInstance;
  EFI_FVB_ATTRIBUTES_2  OldAttributes;
  EFI_FVB_ATTRIBUTES_2  *AttribPtr;
  UINT32                Capabilities;
  UINT32                OldStatus;
  UINT32                NewStatus;
  EFI_STATUS            Status;
  EFI_FVB_ATTRIBUTES_2  UnchangedAttributes;

  //
  // Find the right instance of the FVB private data
  //
  Status = GetFvbInstance (Instance, Global, &FwhInstance, Virtual);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AttribPtr     = (EFI_FVB_ATTRIBUTES_2 *) &(FwhInstance->VolumeHeader.Attributes);
  OldAttributes = *AttribPtr;
  Capabilities  = OldAttributes & (EFI_FVB2_READ_DISABLED_CAP | \
                                   EFI_FVB2_READ_ENABLED_CAP | \
                                   EFI_FVB2_WRITE_DISABLED_CAP | \
                                   EFI_FVB2_WRITE_ENABLED_CAP | \
                                   EFI_FVB2_LOCK_CAP \
                                   );
  OldStatus     = OldAttributes & EFI_FVB2_STATUS;
  NewStatus     = *Attributes & EFI_FVB2_STATUS;

  UnchangedAttributes = EFI_FVB2_READ_DISABLED_CAP  | \
                        EFI_FVB2_READ_ENABLED_CAP   | \
                        EFI_FVB2_WRITE_DISABLED_CAP | \
                        EFI_FVB2_WRITE_ENABLED_CAP  | \
                        EFI_FVB2_LOCK_CAP           | \
                        EFI_FVB2_STICKY_WRITE       | \
                        EFI_FVB2_MEMORY_MAPPED      | \
                        EFI_FVB2_ERASE_POLARITY     | \
                        EFI_FVB2_READ_LOCK_CAP      | \
                        EFI_FVB2_WRITE_LOCK_CAP     | \
                        EFI_FVB2_ALIGNMENT;

  //
  // Some attributes of FV is read only can *not* be set
  //
  if ((OldAttributes & UnchangedAttributes) ^ (*Attributes & UnchangedAttributes)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // If firmware volume is locked, no status bit can be updated
  //
  if (OldAttributes & EFI_FVB2_LOCK_STATUS) {
    if (OldStatus ^ NewStatus) {
      return EFI_ACCESS_DENIED;
    }
  }
  //
  // Test read disable
  //
  if ((Capabilities & EFI_FVB2_READ_DISABLED_CAP) == 0) {
    if ((NewStatus & EFI_FVB2_READ_STATUS) == 0) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Test read enable
  //
  if ((Capabilities & EFI_FVB2_READ_ENABLED_CAP) == 0) {
    if (NewStatus & EFI_FVB2_READ_STATUS) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Test write disable
  //
  if ((Capabilities & EFI_FVB2_WRITE_DISABLED_CAP) == 0) {
    if ((NewStatus & EFI_FVB2_WRITE_STATUS) == 0) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Test write enable
  //
  if ((Capabilities & EFI_FVB2_WRITE_ENABLED_CAP) == 0) {
    if (NewStatus & EFI_FVB2_WRITE_STATUS) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Test lock
  //
  if ((Capabilities & EFI_FVB2_LOCK_CAP) == 0) {
    if (NewStatus & EFI_FVB2_LOCK_STATUS) {
      return EFI_INVALID_PARAMETER;
    }
  }

  *AttribPtr  = (*AttribPtr) & (0xFFFFFFFF & (~EFI_FVB2_STATUS));
  *AttribPtr  = (*AttribPtr) | NewStatus;
  *Attributes = *AttribPtr;

  return EFI_SUCCESS;
}
//
// FVB protocol APIs
//

/**
  Retrieves Volume attributes.  No polarity translations are done.

  @param[in]  This        Calling context.
  @param[out] Attributes  Output buffer which contains attributes

  @retval EFI_SUCCESS     Successfully returns.
**/
EFI_STATUS
EFIAPI
FvbProtocolGetAttributes (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  OUT EFI_FVB_ATTRIBUTES_2                              *Attributes
  )
{
  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbGetVolumeAttributes (FvbDevice->Instance, Attributes, mFvbModuleGlobal, GoneVirtual ());
}

/**
  Sets Volume attributes. No polarity translations are done.

  @param [in]     This        Calling context.
  @param [in,out] Attributes  Output buffer which contains attributes.

  @retval EFI_SUCCESS         Successfully returns.
**/
EFI_STATUS
EFIAPI
FvbProtocolSetAttributes (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN OUT EFI_FVB_ATTRIBUTES_2                           *Attributes
  )
{
  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbSetVolumeAttributes (FvbDevice->Instance, Attributes, mFvbModuleGlobal, GoneVirtual ());
}

/**
  Retrieves the physical address of the device.

  @param [in]  This     Calling context.
  @param [out] Address  Output buffer containing the address.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
EFIAPI
FvbProtocolGetPhysicalAddress (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL     *This,
  OUT EFI_PHYSICAL_ADDRESS                        *Address
  )
{
  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbGetPhysicalAddress (FvbDevice->Instance, Address, mFvbModuleGlobal, GoneVirtual ());
}

/**
  Retrieve the size of a logical block.

  @param [in]   This         Calling context.
  @param [in]   Lba          Indicates which block to return the size for.
  @param [out]  BlockSize    A pointer to a caller allocated UINTN in which
                             the size of the block is returned.
  @param [out]  NumOfBlocks  a pointer to a caller allocated UINTN in which the
                             number of consecutive blocks starting with Lba is
                             returned. All blocks in this range have a size of
                             BlockSize.

  @retval EFI_SUCCESS        The firmware volume was read successfully and
                             contents are in Buffer.
**/
EFI_STATUS
EFIAPI
FvbProtocolGetBlockSize (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL          *This,
  IN CONST EFI_LBA                                     Lba,
  OUT UINTN                                            *BlockSize,
  OUT UINTN                                            *NumOfBlocks
  )
{
  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbGetLbaAddress (
          FvbDevice->Instance,
          Lba,
          NULL,
          BlockSize,
          NumOfBlocks,
          mFvbModuleGlobal,
          GoneVirtual ()
          );
}


/**
  Reads data beginning at Lba:Offset from FV. The Read terminates either
  when *NumBytes of data have been read, or when a block boundary is
  reached.  *NumBytes is updated to reflect the actual number of bytes
  written. The write opertion does not include erase. This routine will
  attempt to write only the specified bytes. If the writes do not stick,
  it will return an error.

  @param[in]      This           Calling context.
  @param[in]      Lba            Block in which to begin Read.
  @param[in]      Offset         Offset in the block at which to begin Read.
  @param[in, out] NumBytes       On input, indicates the requested write size. On
                                 output, indicates the actual number of bytes Read.
  @param[in]      Buffer         Buffer containing source data for the Read.

  @retval EFI_SUCCESS            The firmware volume was read successfully and
                                 contents are in Buffer.
  @retval EFI_BAD_BUFFER_SIZE    Read attempted across a LBA boundary. On output,
                                 NumBytes contains the total number of bytes returned
                                 in Buffer.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the ReadDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be read.
  @retval EFI_INVALID_PARAMETER  NumBytes or Buffer are NULL.
**/
EFI_STATUS
EFIAPI
FvbProtocolRead (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN CONST EFI_LBA                                      Lba,
  IN CONST UINTN                                        Offset,
  IN OUT UINTN                                          *NumBytes,
  IN UINT8                                              *Buffer
  )
{

  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbReadBlock (FvbDevice->Instance, Lba, Offset, NumBytes, Buffer, mFvbModuleGlobal, GoneVirtual ());
}

/**
  Writes data beginning at Lba:Offset from FV. The write terminates either
  when *NumBytes of data have been written, or when a block boundary is
  reached.  *NumBytes is updated to reflect the actual number of bytes
  written. The write opertion does not include erase. This routine will
  attempt to write only the specified bytes. If the writes do not stick,
  it will return an error.

  @param[in]      This           Calling context.
  @param[in]      Lba            Block in which to begin write.
  @param[in]      Offset         Offset in the block at which to begin write.
  @param[in, out] NumBytes       On input, indicates the requested write size. On
                                 output, indicates the actual number of bytes written.
  @param[in]      Buffer         Buffer containing source data for the write.

  @retval EFI_SUCCESS            The firmware volume was written successfully.
  @retval EFI_BAD_BUFFER_SIZE    Write attempted across a LBA boundary. On output,
                                 NumBytes contains the total number of bytes
                                 actually written.
  @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state.
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be written.
  @retval EFI_INVALID_PARAMETER  NumBytes or Buffer are NULL.
**/
EFI_STATUS
EFIAPI
FvbProtocolWrite (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL           *This,
  IN       EFI_LBA                                      Lba,
  IN       UINTN                                        Offset,
  IN OUT   UINTN                                        *NumBytes,
  IN       UINT8                                        *Buffer
  )
{

  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);
  return FvbWriteBlock (FvbDevice->Instance, (EFI_LBA)Lba, (UINTN)Offset, NumBytes, (UINT8 *)Buffer, mFvbModuleGlobal, GoneVirtual ());
}

/**
  The EraseBlock() function erases one or more blocks as denoted by the
  variable argument list. The entire parameter list of blocks must be verified
  prior to erasing any blocks.  If a block is requested that does not exist
  within the associated firmware volume (it has a larger index than the last
  block of the firmware volume), the EraseBlock() function must return
  EFI_INVALID_PARAMETER without modifying the contents of the firmware volume.

  @retval EFI_SUCCESS            The erase request was successfully completed
  @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state
  @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                 could not be written. Firmware device may have been
                                 partially erased.
**/
EFI_STATUS
EFIAPI
FvbProtocolEraseBlocks (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL    *This,
  ...
  )
{
  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;
  EFI_FW_VOL_INSTANCE     *FwhInstance;
  UINTN                   NumOfBlocks;
  VA_LIST                 args;
  EFI_LBA                 StartingLba;
  UINTN                   NumOfLba;
  EFI_STATUS              Status;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  Status    = GetFvbInstance (FvbDevice->Instance, mFvbModuleGlobal, &FwhInstance, GoneVirtual ());
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumOfBlocks = FwhInstance->NumOfBlocks;

  VA_START (args, This);

  do {
    StartingLba = VA_ARG (args, EFI_LBA);
    if (StartingLba == EFI_LBA_LIST_TERMINATOR) {
      break;
    }

    NumOfLba = VA_ARG (args, UINT32);

    //
    // Check input parameters
    //
    if (NumOfLba == 0 || (StartingLba + NumOfLba) > NumOfBlocks) {
      VA_END (args);
      return EFI_INVALID_PARAMETER;
    }
  } while (1);

  VA_END (args);

  VA_START (args, This);
  do {
    StartingLba = VA_ARG (args, EFI_LBA);
    if (StartingLba == EFI_LBA_LIST_TERMINATOR) {
      break;
    }

    NumOfLba = VA_ARG (args, UINT32);

    while (NumOfLba > 0) {
      Status = FvbEraseBlock (FvbDevice->Instance, StartingLba, mFvbModuleGlobal, GoneVirtual ());
      if (EFI_ERROR (Status)) {
        VA_END (args);
        return Status;
      }

      StartingLba++;
      NumOfLba--;
    }

  } while (1);

  VA_END (args);

  return EFI_SUCCESS;
}

/**
  The entry of FvbServicesRuntimeDxe driver.

  Provide interface to access FVB in protected mode directly or send SMI
  to access FVB in SMM mode according to PcdFvbAccessThroughSmi

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       FVB service successfully initialized.
**/
EFI_STATUS
EFIAPI
FvbEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_SMM_BASE2_PROTOCOL       *SmmBase;
  BOOLEAN                      InSmm;
  EFI_HANDLE                   Handle;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  InSmm = FALSE;
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
    if (PcdGetBool (PcdFvbAccessThroughSmi)) {
      Status = EnableFvbAccessThroughSmi ();
      return Status;
    }

    Status = InitializeFVbServices ();
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // Get Smm Syatem Table
    //
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = InitializeSmmFvbAccess ();
    ASSERT_EFI_ERROR (Status);
    //
    // After all SMM related services for access through SMI is completed,
    // install gFvbAccessThroughSmiGuid to signal callback function which
    // located in EfiRuntimeServicesCode memory.
    //
    Handle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gFvbAccessThroughSmiGuid,
                    NULL,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}
