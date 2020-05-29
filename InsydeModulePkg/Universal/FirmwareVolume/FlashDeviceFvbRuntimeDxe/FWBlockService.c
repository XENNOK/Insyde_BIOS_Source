/** @file
  Implementation for Firmware Volume Block Protocol

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  FWBlockService.c

Abstract:

Revision History

**/

//
// The package level header files this module uses
//
#include <PiDxe.h>
//
// The protocols, PPI and GUID defintions for this module
//
#include <Guid/EventGroup.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/DevicePath.h>
//
// The Library classes this module consumes
//
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/FdSupportLib.h>
#include <Guid/IrsiFeature.h>
#include <Protocol/FvRegionInfo.h>

#include "FwBlockService.h"

#define EFI_FVB2_STATUS (EFI_FVB2_READ_STATUS | EFI_FVB2_WRITE_STATUS | EFI_FVB2_LOCK_STATUS)


EFI_GUID                mNullGuid = IRSI_NULL_IMAGE_GUID;
ESAL_FWB_GLOBAL         *mFvbModuleGlobal;
BOOLEAN                 mFvbInstalledComplete = FALSE;


FV_MEMMAP_DEVICE_PATH mFvMemmapDevicePathTemplate = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_MEMMAP_DP,
      {
        (UINT8)(sizeof (MEMMAP_DEVICE_PATH)),
        (UINT8)(sizeof (MEMMAP_DEVICE_PATH) >> 8)
      }
    },
    EfiMemoryMappedIO,
    (EFI_PHYSICAL_ADDRESS) 0,
    (EFI_PHYSICAL_ADDRESS) 0,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

FV_PIWG_DEVICE_PATH mFvPIWGDevicePathTemplate = {
  {
    {
      MEDIA_DEVICE_PATH,
      MEDIA_PIWG_FW_VOL_DP,
      {
        (UINT8)(sizeof (MEDIA_FW_VOL_DEVICE_PATH)),
        (UINT8)(sizeof (MEDIA_FW_VOL_DEVICE_PATH) >> 8)
      }
    },
    { 0 }
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

EFI_FW_VOL_BLOCK_DEVICE mFvbDeviceTemplate = {
  FVB_DEVICE_SIGNATURE,
  NULL,
  0,
  {
    FvbProtocolGetAttributes,
    FvbProtocolSetAttributes,
    FvbProtocolGetPhysicalAddress,
    FvbProtocolGetBlockSize,
    FvbProtocolRead,
    FvbProtocolWrite,
    FvbProtocolEraseBlocks,
    NULL
  }
};



/**
 Fixup internal data so that EFI and SAL can be call in virtual mode.
 Call the passed in Child Notify event and convert the mFvbModuleGlobal
 date items to there virtual address.
 mFvbModuleGlobal->FvInstance[FVB_PHYSICAL]  - Physical copy of instance data
 mFvbModuleGlobal->FvInstance[FVB_VIRTUAL]   - Virtual pointer to common
 instance data.

 @param [in]   Event
 @param [in]   Context

 @retval None

**/
VOID
EFIAPI
FvbVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_FW_VOL_INSTANCE *FwhInstance;
  UINTN               Index;
  FVB_MEDIA_INFO      *FvbMediaInfo;

  EfiConvertPointer (0x0, (VOID **) &mFvbModuleGlobal->FvInstance[FVB_VIRTUAL]);

  //
  // Convert the base address of all the instances
  //
  Index       = 0;
  FwhInstance = mFvbModuleGlobal->FvInstance[FVB_PHYSICAL];
  while (Index < mFvbModuleGlobal->NumFv) {
    EfiConvertPointer (0x0, (VOID **) &FwhInstance->FvBase[FVB_VIRTUAL]);
    FwhInstance = (EFI_FW_VOL_INSTANCE *)
      (
        (UINTN) ((UINT8 *) FwhInstance) + FwhInstance->VolumeHeader.HeaderLength +
          (sizeof (EFI_FW_VOL_INSTANCE) - sizeof (EFI_FIRMWARE_VOLUME_HEADER))
      );
    Index++;
  }

  EfiConvertPointer (0x0, (VOID **) &mFvbModuleGlobal->FvbScratchSpace[FVB_VIRTUAL]);
  EfiConvertPointer (0x0, (VOID **) &mFvbModuleGlobal);

  FvbMediaInfo = GetFvbMediaInfo();
  EfiConvertPointer (0x0, (VOID **) &FvbMediaInfo->FvRegionInfo);

}

/**
 Retrieves the physical address of a memory mapped FV

 @param [in]   Instance         The FV instance whose base address is going to be
                                returned
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [out]  FwhInstance      The EFI_FW_VOL_INSTANCE fimrware instance structure
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            Successfully returns
 @retval EFI_INVALID_PARAMETER  Instance not found

**/
EFI_STATUS
GetFvbInstance (
  IN  UINTN                               Instance,
  IN  ESAL_FWB_GLOBAL                     *Global,
  OUT EFI_FW_VOL_INSTANCE                 **FwhInstance,
  IN BOOLEAN                              Virtual
  )
{
  EFI_FW_VOL_INSTANCE *FwhRecord;

  if (Instance >= Global->NumFv) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Find the right instance of the FVB private data
  //
  FwhRecord = Global->FvInstance[Virtual ? 1 : 0];
  while (Instance > 0) {
    FwhRecord = (EFI_FW_VOL_INSTANCE *)
      (
        (UINTN) ((UINT8 *) FwhRecord) + FwhRecord->VolumeHeader.HeaderLength +
          (sizeof (EFI_FW_VOL_INSTANCE) - sizeof (EFI_FIRMWARE_VOLUME_HEADER))
      );
    Instance--;
  }

  *FwhInstance = FwhRecord;

  return EFI_SUCCESS;
}

/**
 Retrieves the physical address of a memory mapped FV

 @param [in]   Instance         The FV instance whose base address is going to be
                                returned
 @param [out]  Address          Pointer to a caller allocated EFI_PHYSICAL_ADDRESS
                                that on successful return, contains the base address
                                of the firmware volume.
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            Successfully returns
 @retval EFI_INVALID_PARAMETER  Instance not found

**/
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
  *Address = FwhInstance->FvBase[Virtual ? 1 : 0];

  return EFI_SUCCESS;
}

/**
 Retrieves attributes, insures positive polarity of attribute bits, returns
 resulting attributes in output parameter

 @param [in]   Instance         The FV instance whose attributes is going to be
                                returned
 @param [out]  Attributes       Output buffer which contains attributes
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            Successfully returns
 @retval EFI_INVALID_PARAMETER  Instance not found

**/
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
  *Attributes = FwhInstance->VolumeHeader.Attributes;

  return EFI_SUCCESS;
}

/**
 Retrieves the starting address of an LBA in an FV

 @param [in]   Instance         The FV instance which the Lba belongs to
 @param [in]   Lba              The logical block address
 @param [out]  LbaAddress       On output, contains the physical starting address
                                of the Lba
 @param [out]  LbaLength        On output, contains the length of the block
 @param [out]  NumOfBlocks      A pointer to a caller allocated UINTN in which the
                                number of consecutive blocks starting with Lba is
                                returned. All blocks in this range have a size of
                                BlockSize
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            Successfully returns
 @retval EFI_INVALID_PARAMETER  Instance not found

**/
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
  Status = GetFvbInstance (Instance, Global, &FwhInstance, Virtual);
  ASSERT_EFI_ERROR (Status);

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
        *LbaAddress = FwhInstance->FvBase[Virtual ? 1 : 0] + Offset;
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
 Reads specified number of bytes into a buffer from the specified block

 @param [in]   Instance         The FV instance to be read from
 @param [in]   Lba              The logical block address to be read from
 @param [in]   BlockOffset      Offset into the block at which to begin reading
 @param [in, out] NumBytes      Pointer that on input contains the total size of
                                the buffer. On output, it contains the total number
                                of bytes read
 @param [in]   Buffer           Pointer to a caller allocated buffer that will be
                                used to hold the data read
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            The firmware volume was read successfully and
                                contents are in Buffer
 @retval EFI_BAD_BUFFER_SIZE    Read attempted across a LBA boundary. On output,
                                NumBytes contains the total number of bytes returned
                                in Buffer
 @retval EFI_ACCESS_DENIED      The firmware volume is in the ReadDisabled state
 @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                could not be read
 @retval EFI_INVALID_PARAMETER  Instance not found, or NumBytes, Buffer are NULL

**/
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

  Status = FvbGetLbaAddress (Instance, Lba, &LbaAddress, &LbaLength, NULL, Global, Virtual);
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

//  CopyMem (Buffer, (UINT8 *) (LbaAddress + BlockOffset), (UINTN) (*NumBytes));
  Status = FlashRead (Buffer, (UINT8 *) (LbaAddress + BlockOffset), (UINTN) (*NumBytes));

  return Status;
}

/**
 Writes specified number of bytes from the input buffer to the block

 @param [in]   Instance         The FV instance to be written to
 @param [in]   Lba              The starting logical block index to write to
 @param [in]   BlockOffset      Offset into the block at which to begin writing
 @param [in, out] NumBytes      Pointer that on input contains the total size of
                                the buffer. On output, it contains the total number
                                of bytes actually written
 @param [in]   Buffer           Pointer to a caller allocated buffer that contains
                                the source for the write
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            The firmware volume was written successfully
 @retval EFI_BAD_BUFFER_SIZE    Write attempted across a LBA boundary. On output,
                                NumBytes contains the total number of bytes
                                actually written
 @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state
 @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                could not be written
 @retval EFI_INVALID_PARAMETER  Instance not found, or NumBytes, Buffer are NULL

**/
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

  Status = FvbGetLbaAddress (Instance, Lba, &LbaAddress, &LbaLength, NULL, Global, Virtual);
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
  //
  // Write data
  //
  Status = FlashProgram ((UINT8 *) (LbaAddress + BlockOffset), Buffer, NumBytes, LbaAddress + BlockOffset);

  return Status;
}

/**
 Erases and initializes a firmware volume block

 @param [in]   Instance         The FV instance to be erased
 @param [in]   Lba              The logical block index to be erased
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            The erase request was successfully completed
 @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state
 @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                could not be written. Firmware device may have been
                                partially erased
 @retval EFI_INVALID_PARAMETER  Instance not found

**/
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
  // Get the starting address of the block for erase.
  //
  Status = FvbGetLbaAddress (Instance, Lba, &LbaAddress, &LbaLength, NULL, Global, Virtual);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FlashErase (LbaAddress, LbaLength);

  return EFI_SUCCESS;
}

/**
 Modifies the current settings of the firmware volume according to the
 input parameter, and returns the new setting of the volume

 @param [in]   Instance         The FV instance whose attributes is going to be
                                modified
 @param [in, out] Attributes    On input, it is a pointer to EFI_FVB_ATTRIBUTES_2
                                containing the desired firmware volume settings.
                                On successful return, it contains the new settings
                                of the firmware volume
 @param [in]   Global           Pointer to ESAL_FWB_GLOBAL that contains all
                                instance data
 @param [in]   Virtual          Whether CPU is in virtual or physical mode

 @retval EFI_SUCCESS            Successfully returns
 @retval EFI_ACCESS_DENIED      The volume setting is locked and cannot be modified
 @retval EFI_INVALID_PARAMETER  Instance not found, or The attributes requested are
                                in conflict with the capabilities as declared in the
                                firmware volume header

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
 Retrieves the physical address of the device.

 @param [in]   This             Calling context
 @param [out]  Address          Output buffer containing the address.

 @retval Returns:
 @retval EFI_SUCCESS            Successfully returns

**/
EFI_STATUS
EFIAPI
FvbProtocolGetPhysicalAddress (
  IN CONST EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL   *This,
  OUT EFI_PHYSICAL_ADDRESS                      *Address
  )
{
  EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbGetPhysicalAddress (FvbDevice->Instance, Address, mFvbModuleGlobal, EfiGoneVirtual ());
}

/**
 Retrieve the size of a logical block

 @param [in]   This             Calling context
 @param [in]   Lba              Indicates which block to return the size for.
 @param [out]  BlockSize        A pointer to a caller allocated UINTN in which
                                the size of the block is returned
 @param [out]  NumOfBlocks      a pointer to a caller allocated UINTN in which the
                                number of consecutive blocks starting with Lba is
                                returned. All blocks in this range have a size of
                                BlockSize

 @retval EFI_SUCCESS            The firmware volume was read successfully and
                                contents are in Buffer

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
          EfiGoneVirtual ()
          );
}

/**
 Retrieves Volume attributes.  No polarity translations are done.

 @param [in]   This             Calling context
 @param [out]  Attributes       output buffer which contains attributes

 @retval EFI_SUCCESS            Successfully returns

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

  return FvbGetVolumeAttributes (FvbDevice->Instance, Attributes, mFvbModuleGlobal, EfiGoneVirtual ());
}

/**
 Sets Volume attributes. No polarity translations are done.

 @param [in]   This             Calling context
 @param [in, out] Attributes    output buffer which contains attributes

 @retval EFI_SUCCESS            Successfully returns

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

  return FvbSetVolumeAttributes (FvbDevice->Instance, Attributes, mFvbModuleGlobal, EfiGoneVirtual ());
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
                                partially erased

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

  Status    = GetFvbInstance (FvbDevice->Instance, mFvbModuleGlobal, &FwhInstance, EfiGoneVirtual ());
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
      Status = FvbEraseBlock (FvbDevice->Instance, StartingLba, mFvbModuleGlobal, EfiGoneVirtual ());
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
 Writes data beginning at Lba:Offset from FV. The write terminates either
 when *NumBytes of data have been written, or when a block boundary is
 reached.  *NumBytes is updated to reflect the actual number of bytes
 written. The write opertion does not include erase. This routine will
 attempt to write only the specified bytes. If the writes do not stick,
 it will return an error.

 @param [in]   This             Calling context
 @param [in]   Lba              Block in which to begin write
 @param [in]   Offset           Offset in the block at which to begin write
 @param [in, out] NumBytes      On input, indicates the requested write size. On
                                output, indicates the actual number of bytes written
 @param [in]   Buffer           Buffer containing source data for the write.

 @retval EFI_SUCCESS            The firmware volume was written successfully
 @retval EFI_BAD_BUFFER_SIZE    Write attempted across a LBA boundary. On output,
                                NumBytes contains the total number of bytes
                                actually written
 @retval EFI_ACCESS_DENIED      The firmware volume is in the WriteDisabled state
 @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                could not be written
 @retval EFI_INVALID_PARAMETER  NumBytes or Buffer are NULL

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

  return FvbWriteBlock (FvbDevice->Instance, (EFI_LBA)Lba, (UINTN)Offset, NumBytes, (UINT8 *)Buffer, mFvbModuleGlobal, EfiGoneVirtual ());
}

/**
 Reads data beginning at Lba:Offset from FV. The Read terminates either
 when *NumBytes of data have been read, or when a block boundary is
 reached.  *NumBytes is updated to reflect the actual number of bytes
 written. The write opertion does not include erase. This routine will
 attempt to write only the specified bytes. If the writes do not stick,
 it will return an error.

 @param [in]   This             Calling context
 @param [in]   Lba              Block in which to begin Read
 @param [in]   Offset           Offset in the block at which to begin Read
 @param [in, out] NumBytes      On input, indicates the requested write size. On
                                output, indicates the actual number of bytes Read
 @param [in]   Buffer           Buffer containing source data for the Read.

 @retval EFI_SUCCESS            The firmware volume was read successfully and
                                contents are in Buffer
 @retval EFI_BAD_BUFFER_SIZE    Read attempted across a LBA boundary. On output,
                                NumBytes contains the total number of bytes returned
                                in Buffer
 @retval EFI_ACCESS_DENIED      The firmware volume is in the ReadDisabled state
 @retval EFI_DEVICE_ERROR       The block device is not functioning correctly and
                                could not be read
 @retval EFI_INVALID_PARAMETER  NumBytes or Buffer are NULL

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

  if (!mFvbInstalledComplete) {
    return EFI_ACCESS_DENIED;
  }

  FvbDevice = FVB_DEVICE_FROM_THIS (This);

  return FvbReadBlock (FvbDevice->Instance, Lba, Offset, NumBytes, Buffer, mFvbModuleGlobal, EfiGoneVirtual ());
}

/**
 Check the integrity of firmware volume header

 @param        FwVolHeader      A pointer to a firmware volume header

 @retval EFI_SUCCESS            The firmware volume is consistent
 @retval EFI_NOT_FOUND          The firmware volume has corrupted. So it is not an FV

**/
EFI_STATUS
ValidateFvHeader (
  EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader
  )
{
  //
  // Verify the header revision, header signature, length
  // Length of FvBlock cannot be 2**64-1
  // HeaderLength cannot be an odd number
  //
  if (
      (FwVolHeader->Revision != EFI_FVH_REVISION)   ||
      (FwVolHeader->Signature != EFI_FVH_SIGNATURE) ||
      (FwVolHeader->FvLength == ((UINTN) -1)) ||
      ((FwVolHeader->HeaderLength & 0x01) != 0)
      ) {
    return EFI_NOT_FOUND;
  }

  //
  // Verify the header checksum
  //
  if (CalculateCheckSum16 ((UINT16 *) FwVolHeader, FwVolHeader->HeaderLength) != 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
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
 This function does common initialization for FVB services

 @param [in]   ImageHandle
 @param [in]   SystemTable


**/
EFI_STATUS
EFIAPI
FlashDeviceFvbInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;
  EFI_FW_VOL_INSTANCE                 *FwhInstance;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;
  EFI_DXE_SERVICES                    *DxeServices;
  UINT32                              BufferSize;
  EFI_FV_BLOCK_MAP_ENTRY              *PtrBlockMapEntry;
  EFI_HANDLE                          FwbHandle;
  EFI_FW_VOL_BLOCK_DEVICE             *FvbDevice;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *OldFwbInterface;
  UINT32                              MaxLbaSize;
  EFI_PHYSICAL_ADDRESS                BaseAddress;
  UINT64                              Length;
  UINTN                               NumOfBlocks;
  FVB_MEDIA_INFO                      *FvbMediaInfo;
  EFI_EVENT                           VirtualAddressChangeEvent;


  //
  // Get the DXE services table
  //
  DxeServices = gDS;

  //
  // Allocate runtime services data for global variable, which contains
  // the private data of all firmware volume block instances
  //
  mFvbModuleGlobal = AllocateRuntimePool (sizeof (ESAL_FWB_GLOBAL));
  if (mFvbModuleGlobal == NULL) {
    ASSERT (mFvbModuleGlobal != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Calculate the total size for all firmware volume block instances
  //
  FvbMediaInfo = GetFvbMediaInfo();


  BaseAddress = FvbMediaInfo->BaseAddress;
  Length      = FvbMediaInfo->FvHeader.FvLength;
  Status      = SetRuntimeMemoryAttribute (BaseAddress, Length);
  ASSERT_EFI_ERROR (Status);

  FwVolHeader = &FvbMediaInfo->FvHeader;
  FwVolHeader->Checksum = CalculateCheckSum16 ((UINT16 *)FwVolHeader, FwVolHeader->HeaderLength);
  Status      = ValidateFvHeader (FwVolHeader);
  ASSERT_EFI_ERROR (Status);


  BufferSize = (sizeof (EFI_FW_VOL_INSTANCE) + FwVolHeader->HeaderLength - sizeof (EFI_FIRMWARE_VOLUME_HEADER));


  //
  // Only need to allocate once. There is only one copy of physical memory for
  // the private data of each FV instance. But in virtual mode or in physical
  // mode, the address of the the physical memory may be different.
  //
  mFvbModuleGlobal->FvInstance[FVB_PHYSICAL] = AllocateRuntimePool (BufferSize);
  if (mFvbModuleGlobal->FvInstance[FVB_PHYSICAL] == NULL) {
    ASSERT (mFvbModuleGlobal->FvInstance[FVB_PHYSICAL] != NULL);
    FreePool (mFvbModuleGlobal);
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Make a virtual copy of the FvInstance pointer.
  //
  FwhInstance                               = mFvbModuleGlobal->FvInstance[FVB_PHYSICAL];
  mFvbModuleGlobal->FvInstance[FVB_VIRTUAL] = FwhInstance;
  mFvbModuleGlobal->NumFv                   = 0;
  MaxLbaSize                                = 0;


  FwhInstance->FvBase[FVB_PHYSICAL] = (UINTN) BaseAddress;
  FwhInstance->FvBase[FVB_VIRTUAL]  = (UINTN) BaseAddress;

  CopyMem ((UINTN *) &(FwhInstance->VolumeHeader), (UINTN *) FwVolHeader, FwVolHeader->HeaderLength);
  FwVolHeader = &(FwhInstance->VolumeHeader);
  EfiInitializeLock (&(FwhInstance->FvbDevLock), TPL_HIGH_LEVEL);

  NumOfBlocks = 0;

  for (PtrBlockMapEntry = FwVolHeader->BlockMap; PtrBlockMapEntry->NumBlocks != 0; PtrBlockMapEntry++) {
    //
    // Get the maximum size of a block.
    //
    if (MaxLbaSize < PtrBlockMapEntry->Length) {
      MaxLbaSize = PtrBlockMapEntry->Length;
    }

    NumOfBlocks = NumOfBlocks + PtrBlockMapEntry->NumBlocks;
  }
  //
  // The total number of blocks in the FV.
  //
  FwhInstance->NumOfBlocks = NumOfBlocks;

  //
  // Add a FVB Protocol Instance
  //
  FvbDevice = AllocateRuntimePool (sizeof (EFI_FW_VOL_BLOCK_DEVICE));
  if (FvbDevice == NULL) {
    ASSERT (FvbDevice != NULL);
    FreePool (mFvbModuleGlobal->FvInstance[FVB_PHYSICAL]);
    FreePool (mFvbModuleGlobal);
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (FvbDevice, &mFvbDeviceTemplate, sizeof (EFI_FW_VOL_BLOCK_DEVICE));

  FvbDevice->Instance = mFvbModuleGlobal->NumFv;
  mFvbModuleGlobal->NumFv++;

  //
  // Set up the devicepath
  //
  if (FwVolHeader->ExtHeaderOffset == 0) {
    //
    // FV does not contains extension header, then produce MEMMAP_DEVICE_PATH
    //
    FvbDevice->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (sizeof (FV_MEMMAP_DEVICE_PATH), &mFvMemmapDevicePathTemplate);
    ((FV_MEMMAP_DEVICE_PATH *) FvbDevice->DevicePath)->MemMapDevPath.StartingAddress = BaseAddress;
    ((FV_MEMMAP_DEVICE_PATH *) FvbDevice->DevicePath)->MemMapDevPath.EndingAddress   = BaseAddress + FwVolHeader->FvLength - 1;
  } else {
    FvbDevice->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (sizeof (FV_PIWG_DEVICE_PATH), &mFvPIWGDevicePathTemplate);
    CopyGuid (
      &((FV_PIWG_DEVICE_PATH *)FvbDevice->DevicePath)->FvDevPath.FvName,
      (GUID *)(UINTN)(BaseAddress + FwVolHeader->ExtHeaderOffset)
      );
  }
  //
  // Find a handle with a matching device path that has supports FW Block protocol
  //
  Status = gBS->LocateDevicePath (&gEfiFirmwareVolumeBlockProtocolGuid, &FvbDevice->DevicePath, &FwbHandle);
  if (EFI_ERROR (Status)) {
    //
    // LocateDevicePath fails so install a new interface and device path
    //
    FwbHandle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                      &FwbHandle,
                      &gEfiFirmwareVolumeBlockProtocolGuid,
                      &FvbDevice->FwVolBlockInstance,
                      &gEfiDevicePathProtocolGuid,
                      FvbDevice->DevicePath,
                      NULL
                      );
    ASSERT_EFI_ERROR (Status);
  } else if (IsDevicePathEnd (FvbDevice->DevicePath)) {
    //
    // Device allready exists, so reinstall the FVB protocol
    //
    Status = gBS->HandleProtocol (
                    FwbHandle,
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    (VOID**)&OldFwbInterface
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->ReinstallProtocolInterface (
                    FwbHandle,
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    OldFwbInterface,
                    &FvbDevice->FwVolBlockInstance
                    );
    ASSERT_EFI_ERROR (Status);

  } else {
    //
    // There was a FVB protocol on an End Device Path node
    //
    ASSERT (FALSE);
  }

  if (FvbMediaInfo->FvRegionInfo != NULL) {
    Status = gBS->InstallProtocolInterface (
                    &FwbHandle,
                    &gFvRegionInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    FvbMediaInfo->FvRegionInfo
                    );
     ASSERT_EFI_ERROR(Status);
  }

  mFvbInstalledComplete = TRUE;

  Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    FvbVirtualddressChangeEvent,
                    NULL,
                    &gEfiEventVirtualAddressChangeGuid,
                    &VirtualAddressChangeEvent
                    );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}
