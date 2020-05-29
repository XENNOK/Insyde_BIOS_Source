/** @file
  RapidStart Dxe Platform Library.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

**/


//[-start-120725-IB09420086-add]//
#include <Uefi/UefiGpt.h>
//[-end-120725-IB09420086-add]//
#include <Protocol/BlockIo.h>
#include <Protocol/RapidStartPlatformPolicy.h>
//[-start-120813-IB10820103-add]//
#include <Protocol/DiskIo.h>
//[-end-120813-IB10820103-add]//
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/Mbr.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
//[-start-120813-IB10820103-add]//
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
//[-end-120813-IB10820103-add]//
#include <Library/RapidStartDxeLib.h>
//[-start-130127-IB01831000-add]//
#include <Protocol/SetupUtility.h>
#include <ChipsetSetupConfig.h>
//[-end-130127-IB01831000-add]//


#define RAPID_START_PART_TYPE_MBR 0x84  /* Hibernation partition -- APM 1.1f */

extern EFI_GUID  gRapidStartGptGuid;

//
// Extract UINT32 from char array
//
#define UNPACK_UINT32(a) (UINT32)( (((UINT8 *) a)[0] <<  0) |    \
                                   (((UINT8 *) a)[1] <<  8) |    \
                                   (((UINT8 *) a)[2] << 16) |    \
                                   (((UINT8 *) a)[3] << 24) )

/**
  Search device path by specific Type and SubType

  @param[in,out] DevicePath  - A pointer to the device path
  @param[in] Type             - Device path type
  @param[in] SubType          - Device path SubType

  @retval EFI_DEVICE_PATH_PROTOCOL - Device path found and the pointer of device path returned
  @retval NULL                     - Specific device path not found
**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
SearchDevicePath (
  IN OUT   EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN       UINT8                    Type,
  IN       UINT8                    SubType
  )
{
  if (DevicePath == NULL) {
    return NULL;
  }

  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == Type) && (DevicePathSubType (DevicePath) == SubType)) {
      return DevicePath;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  return NULL;
}

/**
  Scan and check if GPT type RapidStart Store present.

  @param[in] Device        - Device handle
  @param[in] DevicePath    - A pointer to the device path
  @param[out] StoreSectors  - Size of RapidStart store partition
  @param[out] StoreLbaAddr  - Address of RapidStart store partition

  @retval EFI_SUCCESS   - GPT type RapidStart Store found.
  @retval EFI_NOT_FOUND - GPT type RapidStart Store not found.
**/
STATIC
EFI_STATUS
ScanForRapidStartGptPartition (
  IN     EFI_HANDLE                    Device,
  IN     EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  OUT    UINT32                        *StoreSectors,
  OUT    UINT64                        *StoreLbaAddr
  )
{
  EFI_STATUS                  Status;
  EFI_BLOCK_IO_PROTOCOL       *BlockIo;
  EFI_DISK_IO_PROTOCOL        *DiskIo;
  EFI_PARTITION_TABLE_HEADER  *PrimaryHeader;
  EFI_PARTITION_ENTRY         *PartitionEntry;
  UINT32                      Index;

  Status = gBS->HandleProtocol (Device, &gEfiBlockIoProtocolGuid, (VOID*)&BlockIo);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->HandleProtocol (Device, &gEfiDiskIoProtocolGuid, (VOID*)&DiskIo);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Read the EFI Partition Table Header
  //
  PrimaryHeader = (EFI_PARTITION_TABLE_HEADER *) AllocatePool (BlockIo->Media->BlockSize);
  if (PrimaryHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = DiskIo->ReadDisk (
                     DiskIo,
                     BlockIo->Media->MediaId,
                     1 * BlockIo->Media->BlockSize,
                     BlockIo->Media->BlockSize,
                     (UINT8 *)PrimaryHeader
                     );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (PrimaryHeader);
    return EFI_DEVICE_ERROR;
  }
  //
  // Check GPT signature to make sure this valid GPT header
  //
  if (PrimaryHeader->Header.Signature != EFI_PTAB_HEADER_ID) {
    FreePool (PrimaryHeader);
    return EFI_DEVICE_ERROR;
  }

  //
  // Read the partition entry.
  //
  PartitionEntry = AllocatePool (PrimaryHeader->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY));

  if (PartitionEntry == NULL) {
    gBS->FreePool (PrimaryHeader);
    return EFI_OUT_OF_RESOURCES;
  }
  Status = DiskIo->ReadDisk (
                     DiskIo,
                     BlockIo->Media->MediaId,
                     MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockIo->Media->BlockSize),
                     PrimaryHeader->NumberOfPartitionEntries * PrimaryHeader->SizeOfPartitionEntry,
                     PartitionEntry
                     );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (PrimaryHeader);
    gBS->FreePool (PartitionEntry);
    return EFI_DEVICE_ERROR;
  }

  //
  // Count the valid partition
  //
  for (Index = 0; Index < PrimaryHeader->NumberOfPartitionEntries; Index++) {
    if (CompareGuid (&PartitionEntry[Index].PartitionTypeGUID, &gRapidStartGptGuid)) {
      *StoreLbaAddr = PartitionEntry[Index].StartingLBA;
      *StoreSectors = (UINT32) (PartitionEntry[Index].EndingLBA - PartitionEntry[Index].StartingLBA + 1);
       DEBUG (
        (EFI_D_INFO,
        "Found RapidStart GPT partition: start=%x size=%x\n",
        *StoreLbaAddr,
        *StoreSectors)
        );
      gBS->FreePool (PrimaryHeader);
      gBS->FreePool (PartitionEntry);
      return EFI_SUCCESS;
    }
  }

  FreePool (PrimaryHeader);
  FreePool (PartitionEntry);
  return EFI_NOT_FOUND;
}

/**
  Scan and check if MBR type RapidStart Store present.

  @param[in] Device         - Device handle
  @param[out] StoreSectors  - Size of RapidStart store partition
  @param[out] StoreLbaAddr  - Address of RapidStart store partition

  @retval EFI_SUCCESS   - MBR type RapidStart Store found.
  @retval EFI_NOT_FOUND - MBR type RapidStart Store not found.
**/
STATIC
EFI_STATUS
ScanForRapidStartMbrPartition (
  IN     EFI_HANDLE                    Device,
  OUT    UINT32                        *StoreSectors,
  OUT    UINT64                        *StoreLbaAddr
  )
{
  EFI_STATUS            Status;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  UINTN                 Idx;
  UINT8                 OSIndicator;

  MASTER_BOOT_RECORD    BootRecord;
  MASTER_BOOT_RECORD    *Mbr;
  MASTER_BOOT_RECORD    ExtenBootRecord;
  MASTER_BOOT_RECORD    *Ebr;
  UINTN                 EbrLbaAddroffset = 0;

  Mbr     = &BootRecord;
  Ebr     = &ExtenBootRecord;
  Status  = gBS->HandleProtocol (Device, &gEfiBlockIoProtocolGuid, (VOID *) &BlockIo);
  ASSERT_EFI_ERROR (Status);
  ASSERT (BlockIo != NULL);

  ///
  /// read the MBR
  ///
  Status = BlockIo->ReadBlocks (BlockIo, BlockIo->Media->MediaId, 0, sizeof (*Mbr), Mbr);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Cannot read MBR\n"));
    return Status;
  }

  if (Mbr->Signature != MBR_SIGNATURE) {
    DEBUG ((EFI_D_ERROR, "Bad MBR\n"));
    return EFI_NOT_FOUND;
  }

  for (Idx = 0; Idx < MAX_MBR_PARTITIONS; Idx++) {
    OSIndicator = Mbr->Partition[Idx].OSIndicator;

    ///
    /// ignore partitions whose OSIndicator or Size is zero
    ///
    if ((OSIndicator == 0) || (UNPACK_UINT32(Mbr->Partition[Idx].SizeInLBA) == 0)) {
      continue;
    }

    if (OSIndicator == RAPID_START_PART_TYPE_MBR) {
      DEBUG (
        (EFI_D_INFO,
        "Found RapidStart MBR partition: start=%x size=%x\n",
        UNPACK_UINT32(Mbr->Partition[Idx].StartingLBA),
        UNPACK_UINT32(Mbr->Partition[Idx].SizeInLBA))
        );
      *StoreLbaAddr = UNPACK_UINT32(Mbr->Partition[Idx].StartingLBA);
      *StoreSectors = UNPACK_UINT32(Mbr->Partition[Idx].SizeInLBA);
      return EFI_SUCCESS;
    }
  }
  //
  // Search extented partition
  //
  for (Idx = 0; Idx < MAX_MBR_PARTITIONS; Idx++) {
    OSIndicator = Mbr->Partition[Idx].OSIndicator;

    //
    // ignore partitions whose OSIndicator or Size is zero
    //
    if ((OSIndicator == 0) || (UNPACK_UINT32(Mbr->Partition[Idx].SizeInLBA) == 0)) {
      continue;
    }

    if (OSIndicator == EXTENDED_DOS_PARTITION || OSIndicator == EXTENDED_WINDOWS_PARTITION) {

      EbrLbaAddroffset = UNPACK_UINT32(Mbr->Partition[Idx].StartingLBA);

      do{
        Status = BlockIo->ReadBlocks (BlockIo, BlockIo->Media->MediaId, EbrLbaAddroffset, sizeof (*Ebr), Ebr);
        if (EFI_ERROR(Status)){
          DEBUG ((EFI_D_ERROR, "Can not find extended Partition\n"));
          return EFI_NOT_FOUND;          
        }

        if (Ebr->Partition[0].OSIndicator == RAPID_START_PART_TYPE_MBR ){
          DEBUG (
             (EFI_D_INFO,
             "Found RapidStart EBR partition: start=%x size=%x\n",
             UNPACK_UINT32(Ebr->Partition[0].StartingLBA)+EbrLbaAddroffset,
             UNPACK_UINT32(Ebr->Partition[0].SizeInLBA))
             );
          *StoreLbaAddr = UNPACK_UINT32(Ebr->Partition[0].StartingLBA)+EbrLbaAddroffset;
          *StoreSectors = UNPACK_UINT32(Ebr->Partition[0].SizeInLBA);
          return EFI_SUCCESS;          
        }
        //
        //search the next partition
        //
        if (UNPACK_UINT32(Ebr->Partition[1].StartingLBA) != 0){
          EbrLbaAddroffset = EbrLbaAddroffset+UNPACK_UINT32(Ebr->Partition[0].StartingLBA)+UNPACK_UINT32(Ebr->Partition[0].SizeInLBA);
        } 

      }while (UNPACK_UINT32(Ebr->Partition[1].StartingLBA) != 0 ); 

    } 
  }   
  return EFI_NOT_FOUND;
}

/**
  Verify if this device path was RapidStart store partition. Get SATA port number if it was.

  @param[in] AhciMode          - TRUE means current SATA operation mode is AHCI, otherwise it is RAID
  @param[in,out] DevicePath   - A pointer to the device path
  @param[in,out] PortNumber   - Port Number connecting to this drive

  @retval EFI_SUCCESS     - This is RapidStart Store partition and Port Number retrieved successfully.
  @retval EFI_NOT_FOUND   - This is not RapidStart Store
**/
//[-start-130127-IB01831000-modify]//
STATIC
EFI_STATUS
VerifyDevicePath (
  IN       BOOLEAN                  AhciMode,
  IN OUT   EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN OUT   UINT8                    *PortNumber
  )
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINT8                     BootType;
//[-start-120813-IB10820103-add]//
//[-start-120419-IB04770225-add]//
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathPtr;
  UINT8                     PortMap;

  DevicePathPtr = DevicePath;
//[-end-120419-IB04770225-add]//
//[-end-120813-IB10820103-add]//


  BootType = DUAL_BOOT_TYPE;
  Size = sizeof (UINT8);
  gRT->GetVariable (
         L"BootType",
         &gSystemConfigurationGuid,
         NULL,
         &Size,
         &BootType
         );
  Status = EFI_NOT_FOUND;
//[-start-140625-IB05080432-modify]//
  if (BootType != EFI_BOOT_TYPE) {
    DevicePath = SearchDevicePath (DevicePath, MESSAGING_DEVICE_PATH, MSG_SATA_DP);
    if ((SATA_DEVICE_PATH *) DevicePath != NULL) {
      * PortNumber = (UINT8) ((SATA_DEVICE_PATH *) DevicePath)->HBAPortNumber;
      Status = EFI_SUCCESS;
    }
  }
  
  if (AhciMode) {
    DevicePath = SearchDevicePath (DevicePath, MESSAGING_DEVICE_PATH, MSG_SATA_DP);
    if ((SATA_DEVICE_PATH *) DevicePath != NULL) {
      * PortNumber = (UINT8) ((SATA_DEVICE_PATH *) DevicePath)->HBAPortNumber;
      Status = EFI_SUCCESS;
    }
  }
//[-end-140625-IB05080432-modify]//
  if (!AhciMode) {
    DevicePath = SearchDevicePath (DevicePath, HARDWARE_DEVICE_PATH, HW_VENDOR_DP);
    if ((SATA_DEVICE_PATH *) DevicePath != NULL) {
      *PortNumber = (UINT8) ((UNKNOWN_DEVICE_VENDOR_DEVICE_PATH *)DevicePath)->LegacyDriveLetter;
      Status = EFI_SUCCESS;
    }
//[-start-120813-IB10820103-add]//
//[-start-120419-IB04770225-add]//
    if (EFI_ERROR (Status)) {
      //
      // For RST EFI driver, the SATA port number bitmap can be retrieved from DevicePath->HBAPortNumber field.
      //
      DevicePathPtr = SearchDevicePath (DevicePathPtr, MESSAGING_DEVICE_PATH, MSG_SATA_DP);
      if ((SATA_DEVICE_PATH *) DevicePathPtr != NULL) {
        PortMap = (UINT8) ((SATA_DEVICE_PATH *) DevicePathPtr)->HBAPortNumber;
        for (*PortNumber = 0xFF; PortMap != 0; (*PortNumber)++, PortMap >>= 1);
        DevicePath = DevicePathPtr;
        Status = EFI_SUCCESS;
      }
    }
//[-end-120419-IB04770225-add]//
//[-end-120813-IB10820103-add]//
  }
  if (Status == EFI_SUCCESS) {
    DEBUG ((EFI_D_INFO, "Port number=%X\n", *PortNumber));
  }

  return Status;
}
//[-end-130127-IB01831000-modify]//

/**
  Look through all device handles to detect if any GPT/MBR type RapidStart Store present

  @param[in] AhciMode         - TRUE means current SATA operation mode is AHCI, otherwise it is RAID
  @param[in] RapidStartPolicy - RapidStart Platform Policy protocol
  @param[out] StoreSectors     - Size of RapidStart store partition
  @param[out] StoreLbaAddr     - Address of RapidStart store partition
  @param[out] StoreSataPort    - Port number for RapidStart store partition

  @retval EFI_SUCCESS   - RapidStart Store found
  @retval EFI_NOT_FOUND - RapidStart Store not found
**/
STATIC
EFI_STATUS
DetectRapidStartPartition (
  IN     BOOLEAN                              AhciMode,
  IN     RAPID_START_PLATFORM_POLICY_PROTOCOL *RapidStartPolicy,
  OUT    UINT32                               *StoreSectors,
  OUT    UINT64                               *StoreLbaAddr,
  OUT    UINT8                                *StoreSataPort
  )
{
  EFI_STATUS                Status;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     Index;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (!EFI_ERROR (Status)) {
    //
    // Loop through all the device handles that support the BLOCK_IO Protocol
    //
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &DevicePath);
      if (EFI_ERROR (Status) || DevicePath == NULL) {
        continue;
      }

      if (VerifyDevicePath (AhciMode, DevicePath, StoreSataPort) == EFI_SUCCESS) {
//[-start-120813-IB10820103-add]//	  
        Status = ScanForRapidStartGptPartition (HandleBuffer[Index], DevicePath, StoreSectors, StoreLbaAddr);
//[-end-120813-IB10820103-add]//		
        if (Status == EFI_SUCCESS) {
          DEBUG ((EFI_D_INFO, "Found Gpt RapidStart Store on SATA port=%d\n", *StoreSataPort));
          break;
        }
        Status = ScanForRapidStartMbrPartition (HandleBuffer[Index], StoreSectors, StoreLbaAddr);
        if (Status == EFI_SUCCESS) {
          DEBUG ((EFI_D_INFO, "Found Mbr RapidStart Store on SATA port=%d\n", *StoreSataPort));
          break;
        }
      }
    }

    gBS->FreePool (HandleBuffer);
  }

  return Status;
}

/**
  Search if any type of RapidStart Store partition present

  @param[in] RapidStartPolicy  - RapidStart Platform Policy protocol
  @param[out] StoreSectors     - Size of RapidStart store partition
  @param[out] StoreLbaAddr     - Address of RapidStart store partition
  @param[out] StoreSataPort    - Port number for RapidStart store partition

  @retval EFI_SUCCESS   - GPT or MBR type RapidStart Store found
  @retval EFI_NOT_FOUND - GPT or MBR type RapidStart Store not found
**/
EFI_STATUS
SearchRapidStartStore (
  IN     RAPID_START_PLATFORM_POLICY_PROTOCOL *RapidStartPolicy,
  OUT    UINT32                               *StoreSectors,
  OUT    UINT64                               *StoreLbaAddr,
  OUT    UINT8                                *StoreSataPort
  )
{
  BOOLEAN     AhciMode;
  EFI_STATUS  Status;

  //
  // Get current SATA operation mode (only AHCI or RAID mode is supported)
  //
  AhciMode = (MmioRead8 (
                MmPciAddress (
                0,
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_SATA,
                PCI_FUNCTION_NUMBER_PCH_SATA,
                R_PCH_SATA_SUB_CLASS_CODE)
                ) == V_PCH_SATA_SUB_CLASS_CODE_AHCI);

  Status = DetectRapidStartPartition (AhciMode, RapidStartPolicy, StoreSectors, StoreLbaAddr, StoreSataPort);

  return Status;
}

/**
  Generate RapidStart Store UID

  @retval UINT64 as RapidStart Store UID
**/
UINT64
GenerateRapidStartStoreUid (
  VOID
  )
{
  UINT64  Uid;
  UINT32  HpetBase;
  UINT32  HpetSetting;

  Uid         = AsmReadTsc ();

  HpetSetting = MmioRead32 (PCH_RCRB_BASE + R_PCH_RCRB_HPTC);
  if (HpetSetting & B_PCH_RCRB_HPTC_AE) {
    HpetBase = R_PCH_PCH_HPET_CONFIG + (HpetSetting & B_PCH_RCRB_HPTC_AS) * 0x1000;
    Uid ^= (LShiftU64 ((*(UINT64 *) (UINTN) (HpetBase + 0xF0)), 32));
  } else {
    //
    // Use certain random memory content as part of UID.
    //
    Uid ^= (LShiftU64 ((*(UINT64 *) (UINTN) (0xF5C00)), 32));
  }

  return Uid;
}
