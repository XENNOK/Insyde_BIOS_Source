/** @file
  Routines supporting partition discovery and logical device reading

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Part.c

Abstract:

  Routines supporting partition discovery and
  logical device reading

--*/


#include <IndustryStandard/ElTorito.h>
#include <IndustryStandard/Mbr.h>
#include <Uefi/UefiGpt.h>
#include "FatPeim.h"

//
// GPT Partition Entry Status
//
typedef struct {
  BOOLEAN OutOfRange;
  BOOLEAN Overlap;
  BOOLEAN OsSpecific;
} EFI_PARTITION_ENTRY_STATUS;

EFI_GUID mEfiPartTypeUnusedGuid = {0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
  This function finds Eltorito partitions

  @param  PrivateData       The global memory map 
  @param  ParentBlockDevNo  The parent block device 

  @retval TRUE              New partitions are detected and logical block devices 
                            are  added to block device array 
  @retval FALSE             No New partitions are added;

**/
BOOLEAN
FatFindEltoritoPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  );

/**
  This function finds Mbr partitions. Main algorithm
  is ported from DXE partition driver.

  @param  PrivateData       The global memory map 
  @param  ParentBlockDevNo  The parent block device 

  @retval TRUE              New partitions are detected and logical block devices 
                            are  added to block device array 
  @retval FALSE             No New partitions are added;

**/
BOOLEAN
FatFindMbrPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  );

/**
  This function finds GPT partitions

  @param  PrivateData       The global memory map 
  @param  ParentBlockDevNo  The parent block device 

  @retval TRUE              New partitions are detected and logical block devices 
                            are  added to block device array 
  @retval FALSE             No New partitions are added;

**/
BOOLEAN
FatFindGptPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  );
  
/**
  This function finds partitions (logical devices) in physical block devices.

  @param  PrivateData       Global memory map for accessing global variables.

**/
VOID
FatFindPartitions (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData
  )
{
  BOOLEAN Found;
  UINTN   Index;

  do {
    Found = FALSE;

    for (Index = 0; Index < PrivateData->BlockDeviceCount; Index++) {
      if (!PrivateData->BlockDevice[Index].PartitionChecked) {
        Found = FatFindMbrPartitions (PrivateData, Index);
        if (!Found) {
          Found = FatFindGptPartitions (PrivateData, Index);
          if (!Found) {
            Found = FatFindEltoritoPartitions (PrivateData, Index);
          }
        }
      }
    }
  } while (Found && PrivateData->BlockDeviceCount <= PEI_FAT_MAX_BLOCK_DEVICE);
}

/**
  This function finds Eltorito partitions. Main algorithm
  is ported from DXE partition driver.

  @param  PrivateData       The global memory map 
  @param  ParentBlockDevNo  The parent block device 

  @retval TRUE              New partitions are detected and logical block devices 
                            are  added to block device array 
  @retval FALSE             No New partitions are added;

**/
BOOLEAN
FatFindEltoritoPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  )
{
  EFI_STATUS              Status;
  BOOLEAN                 Found;
  PEI_FAT_BLOCK_DEVICE    *BlockDev;
  PEI_FAT_BLOCK_DEVICE    *ParentBlockDev;
  UINT32                  VolDescriptorLba;
  UINT32                  Lba;
  CDROM_VOLUME_DESCRIPTOR *VolDescriptor;
  ELTORITO_CATALOG        *Catalog;
  UINTN                   Check;
  UINTN                   Index;
  UINTN                   MaxIndex;
  UINT16                  *CheckBuffer;
  UINT32                  SubBlockSize;
  UINT32                  SectorCount;
  UINT32                  VolSpaceSize;

  if (ParentBlockDevNo > PEI_FAT_MAX_BLOCK_DEVICE - 1) {
    return FALSE;
  }

  Found           = FALSE;
  ParentBlockDev  = &(PrivateData->BlockDevice[ParentBlockDevNo]);
  VolSpaceSize    = 0;

  //
  // CD_ROM has the fixed block size as 2048 bytes
  //
  if (ParentBlockDev->BlockSize != 2048) {
    return FALSE;
  }

  VolDescriptor = (CDROM_VOLUME_DESCRIPTOR *) PrivateData->BlockData;
  Catalog       = (ELTORITO_CATALOG *) VolDescriptor;

  //
  // the ISO-9660 volume descriptor starts at 32k on the media
  // and CD_ROM has the fixed block size as 2048 bytes, so...
  //
  VolDescriptorLba = 15;
  //
  // ((16*2048) / Media->BlockSize) - 1;
  //
  // Loop: handle one volume descriptor per time
  //
  while (TRUE) {

    VolDescriptorLba += 1;
    if (VolDescriptorLba > ParentBlockDev->LastBlock) {
      //
      // We are pointing past the end of the device so exit
      //
      break;
    }

    Status = FatReadBlock (
              PrivateData,
              ParentBlockDevNo,
              VolDescriptorLba,
              ParentBlockDev->BlockSize,
              VolDescriptor
              );
    if (EFI_ERROR (Status)) {
      break;
    }
    //
    // Check for valid volume descriptor signature
    //
    if (VolDescriptor->Unknown.Type == CDVOL_TYPE_END ||
        CompareMem (VolDescriptor->Unknown.Id, CDVOL_ID, sizeof (VolDescriptor->Unknown.Id)) != 0
        ) {
      //
      // end of Volume descriptor list
      //
      break;
    }
    //
    // Read the Volume Space Size from Primary Volume Descriptor 81-88 byte
    //
    if (VolDescriptor->Unknown.Type == CDVOL_TYPE_CODED) {
      VolSpaceSize = VolDescriptor->PrimaryVolume.VolSpaceSize[1];
    }
    //
    // Is it an El Torito volume descriptor?
    //
    if (CompareMem (
          VolDescriptor->BootRecordVolume.SystemId,
          CDVOL_ELTORITO_ID,
          sizeof (CDVOL_ELTORITO_ID) - 1
          ) != 0) {
      continue;
    }
    //
    // Read in the boot El Torito boot catalog
    //
    Lba = UNPACK_INT32 (VolDescriptor->BootRecordVolume.EltCatalog);
    if (Lba > ParentBlockDev->LastBlock) {
      continue;
    }

    Status = FatReadBlock (
              PrivateData,
              ParentBlockDevNo,
              Lba,
              ParentBlockDev->BlockSize,
              Catalog
              );
    if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // We don't care too much about the Catalog header's contents, but we do want
    // to make sure it looks like a Catalog header
    //
    if (Catalog->Catalog.Indicator != ELTORITO_ID_CATALOG || Catalog->Catalog.Id55AA != 0xAA55) {
      continue;
    }

    Check       = 0;
    CheckBuffer = (UINT16 *) Catalog;
    for (Index = 0; Index < sizeof (ELTORITO_CATALOG) / sizeof (UINT16); Index += 1) {
      Check += CheckBuffer[Index];
    }

    if ((Check & 0xFFFF) != 0) {
      continue;
    }

    MaxIndex = ParentBlockDev->BlockSize / sizeof (ELTORITO_CATALOG);
    for (Index = 1; Index < MaxIndex; Index += 1) {
      //
      // Next entry
      //
      Catalog += 1;

      //
      // Check this entry
      //
      if (Catalog->Boot.Indicator != ELTORITO_ID_SECTION_BOOTABLE || Catalog->Boot.Lba == 0) {
        continue;
      }

      SubBlockSize  = 512;
      SectorCount   = Catalog->Boot.SectorCount;

      switch (Catalog->Boot.MediaType) {

      case ELTORITO_NO_EMULATION:
        SubBlockSize  = ParentBlockDev->BlockSize;
        SectorCount   = Catalog->Boot.SectorCount;
        break;

      case ELTORITO_HARD_DISK:
        break;

      case ELTORITO_12_DISKETTE:
        SectorCount = 0x50 * 0x02 * 0x0F;
        break;

      case ELTORITO_14_DISKETTE:
        SectorCount = 0x50 * 0x02 * 0x12;
        break;

      case ELTORITO_28_DISKETTE:
        SectorCount = 0x50 * 0x02 * 0x24;
        break;

      default:
        SectorCount   = 0;
        SubBlockSize  = ParentBlockDev->BlockSize;
        break;
      }

      if (SectorCount < 2) {
        SectorCount = (VolSpaceSize > ParentBlockDev->LastBlock + 1) ?
          (UINT32)(ParentBlockDev->LastBlock - Catalog->Boot.Lba + 1) : (UINT32)(VolSpaceSize - Catalog->Boot.Lba);
      }
      //
      // Register this partition
      //
      if (PrivateData->BlockDeviceCount < PEI_FAT_MAX_BLOCK_DEVICE) {

        Found                       = TRUE;

        BlockDev                    = &(PrivateData->BlockDevice[PrivateData->BlockDeviceCount]);

        BlockDev->BlockSize         = SubBlockSize;
        BlockDev->LastBlock         = SectorCount - 1;
        BlockDev->IoAlign           = ParentBlockDev->IoAlign;
        BlockDev->Logical           = TRUE;
        BlockDev->PartitionChecked  = FALSE;
        BlockDev->StartingPos       = MultU64x32 (Catalog->Boot.Lba, ParentBlockDev->BlockSize);
        BlockDev->ParentDevNo       = ParentBlockDevNo;

        PrivateData->BlockDeviceCount++;
      }
    }
  }

  ParentBlockDev->PartitionChecked = TRUE;

  return Found;

}


/**
  Test to see if the Mbr buffer is a valid MBR

  @param  Mbr               Parent Handle 
  @param  LastLba           Last Lba address on the device. 

  @retval TRUE              Mbr is a Valid MBR 
  @retval FALSE             Mbr is not a Valid MBR

**/
BOOLEAN
PartitionValidMbr (
  IN  MASTER_BOOT_RECORD      *Mbr,
  IN  EFI_PEI_LBA             LastLba
  )
{
  UINT32  StartingLBA;
  UINT32  EndingLBA;
  UINT32  NewEndingLBA;
  INTN    Index1;
  INTN    Index2;
  BOOLEAN MbrValid;

  if (Mbr->Signature != MBR_SIGNATURE) {
    return FALSE;
  }
  //
  // The BPB also has this signature, so it can not be used alone.
  //
  MbrValid = FALSE;
  for (Index1 = 0; Index1 < MAX_MBR_PARTITIONS; Index1++) {
    if (Mbr->Partition[Index1].OSIndicator == 0x00 || 
        UNPACK_UINT32 (Mbr->Partition[Index1].SizeInLBA) == 0 ||
        Mbr->Partition[Index1].OSIndicator == PMBR_GPT_PARTITION) {
      continue;
    }

    MbrValid    = TRUE;
    StartingLBA = UNPACK_UINT32 (Mbr->Partition[Index1].StartingLBA);
    EndingLBA   = StartingLBA + UNPACK_UINT32 (Mbr->Partition[Index1].SizeInLBA) - 1;
    if (EndingLBA > LastLba) {
      //
      // Compatability Errata:
      //  Some systems try to hide drive space with thier INT 13h driver
      //  This does not hide space from the OS driver. This means the MBR
      //  that gets created from DOS is smaller than the MBR created from
      //  a real OS (NT & Win98). This leads to BlockIo->LastBlock being
      //  wrong on some systems FDISKed by the OS.
      //
      //  return FALSE Because no block devices on a system are implemented
      //  with INT 13h
      //
      return FALSE;
    }

    for (Index2 = Index1 + 1; Index2 < MAX_MBR_PARTITIONS; Index2++) {
      if (Mbr->Partition[Index2].OSIndicator == 0x00 || UNPACK_INT32 (Mbr->Partition[Index2].SizeInLBA) == 0) {
        continue;
      }

      NewEndingLBA = UNPACK_UINT32 (Mbr->Partition[Index2].StartingLBA) + UNPACK_UINT32 (Mbr->Partition[Index2].SizeInLBA) - 1;
      if (NewEndingLBA >= StartingLBA && UNPACK_UINT32 (Mbr->Partition[Index2].StartingLBA) <= EndingLBA) {
        //
        // This region overlaps with the Index1'th region
        //
        return FALSE;
      }
    }
  }
  //
  // Non of the regions overlapped so MBR is O.K.
  //
  return MbrValid;
}

/**
  This function finds Mbr partitions. Main algorithm
  is ported from DXE partition driver.

  @param  PrivateData       The global memory map 
  @param  ParentBlockDevNo  The parent block device 

  @retval TRUE              New partitions are detected and logical block devices 
                            are  added to block device array 
  @retval FALSE             No New partitions are added;

**/
BOOLEAN
FatFindMbrPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  )
{
  EFI_STATUS            Status;
  MASTER_BOOT_RECORD    *Mbr;
  UINTN                 Index;
  BOOLEAN               Found;
  PEI_FAT_BLOCK_DEVICE  *ParentBlockDev;
  PEI_FAT_BLOCK_DEVICE  *BlockDev;

  if (ParentBlockDevNo > PEI_FAT_MAX_BLOCK_DEVICE - 1) {
    return FALSE;
  }

  ParentBlockDev  = &(PrivateData->BlockDevice[ParentBlockDevNo]);

  Found           = FALSE;
  Mbr             = (MASTER_BOOT_RECORD *) PrivateData->BlockData;

  Status = FatReadBlock (
            PrivateData,
            ParentBlockDevNo,
            0,
            ParentBlockDev->BlockSize,
            Mbr
            );

  if (EFI_ERROR (Status) || !PartitionValidMbr (Mbr, ParentBlockDev->LastBlock)) {
    goto Done;
  }
  //
  // We have a valid mbr - add each partition
  //
  for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) {
    if (Mbr->Partition[Index].OSIndicator == 0x00 || UNPACK_INT32 (Mbr->Partition[Index].SizeInLBA) == 0) {
      //
      // Don't use null MBR entries
      //
      continue;
    }
    //
    // Register this partition
    //
    if (PrivateData->BlockDeviceCount < PEI_FAT_MAX_BLOCK_DEVICE) {

      Found                       = TRUE;

      BlockDev                    = &(PrivateData->BlockDevice[PrivateData->BlockDeviceCount]);

      BlockDev->BlockSize         = MBR_SIZE;
      BlockDev->LastBlock         = UNPACK_INT32 (Mbr->Partition[Index].SizeInLBA) - 1;
      BlockDev->IoAlign           = ParentBlockDev->IoAlign;
      BlockDev->Logical           = TRUE;
      BlockDev->PartitionChecked  = FALSE;
      BlockDev->StartingPos = MultU64x32 (
                                UNPACK_INT32 (Mbr->Partition[Index].StartingLBA),
                                ParentBlockDev->BlockSize
                                );
      BlockDev->ParentDevNo = ParentBlockDevNo;

      PrivateData->BlockDeviceCount++;
    }
  }

Done:

  ParentBlockDev->PartitionChecked = TRUE;
  return Found;
}


/**
  This internal function reverses bits for 32bit data.

  @param  Value                 The data to be reversed.

  @return                       Data reversed.

**/
UINT32
ReverseBits (
  UINT32  Value
  )
{
  UINTN   Index;
  UINT32  NewValue;

  NewValue = 0;
  for (Index = 0; Index < 32; Index++) {
    if (Value & (1 << Index)) {
      NewValue = NewValue | (1 << (31 - Index));
    }
  }

  return NewValue;
}

/**
  Calculate CRC32 for target data.

  @param  Data                  The target data.
  @param  DataSize              The target data size.
  @param  CrcOut                The CRC32 for target data.

  @retval EFI_SUCCESS           The CRC32 for target data is calculated successfully.
  @retval EFI_INVALID_PARAMETER Some parameter is not valid, so the CRC32 is not
                                calculated.

**/
EFI_STATUS
EFIAPI
FatCalculateCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Ptr;
  UINT32  CrcTable[256];
  UINTN   TableEntry;
  UINT32  Value;

  for (TableEntry = 0; TableEntry < 256; TableEntry++) {
    Value = ReverseBits ((UINT32) TableEntry);
    for (Index = 0; Index < 8; Index++) {
      if (Value & 0x80000000) {
        Value = (Value << 1) ^ 0x04c11db7;
      } else {
        Value = Value << 1;
      }
    }

    CrcTable[TableEntry] = ReverseBits (Value);
  }

  if (Data == NULL || DataSize == 0 || CrcOut == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Crc = 0xffffffff;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++) {
    Crc = (Crc >> 8) ^ CrcTable[(UINT8) Crc ^ *Ptr];
  }

  *CrcOut = Crc ^ 0xffffffff;
  return EFI_SUCCESS;
}

/**
  Checks the CRC32 value in the table header.

  @param  MaxSize   Max Size limit
  @param  Hdr       Table to check

  @return TRUE      CRC Valid
  @return FALSE     CRC Invalid

**/
BOOLEAN
PartitionCheckCrc (
  IN UINTN                 MaxSize,
  IN UINTN                 Size,
  IN OUT EFI_TABLE_HEADER  *Hdr
  )
{
  UINT32      Crc;
  UINT32      OrgCrc;
  EFI_STATUS  Status;

  Crc = 0;

  if (Size == 0) {
    //
    // If header size is 0 CRC will pass so return FALSE here
    //
    return FALSE;
  }

  if ((MaxSize != 0) && (Size > MaxSize)) {
    return FALSE;
  }
  //
  // clear old crc from header
  //
  OrgCrc      = Hdr->CRC32;
  Hdr->CRC32  = 0;

  Status      = FatCalculateCrc32 ((UINT8 *) Hdr, Size, &Crc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  //
  // set results
  //
  Hdr->CRC32 = Crc;

  return (BOOLEAN) (OrgCrc == Crc);
}

/**
  Test to see if the Gpt buffer is a valid Gpt table

  @param[in]  PartHdr           Pointer to the EFI_PARTITION_TABLE_HEADER
  @param[in]  BlockSize         The block size
  @param[in]  LastLba           The last Lba of the device

  @retval TRUE      The partition table is valid
  @retval FALSE     The partition table is not valid

**/
BOOLEAN
PartitionValidGptTable (
  IN  EFI_PARTITION_TABLE_HEADER      *PartHdr,
  IN  UINTN                           BlockSize,
  IN  EFI_PEI_LBA                     LastLba
  )
{
  UINT64  PtabHeaderId = EFI_PTAB_HEADER_ID; 
  if (CompareMem (&PartHdr->Header.Signature, (CONST VOID *)&PtabHeaderId, sizeof (UINT64)) != 0 ||
      !PartitionCheckCrc (BlockSize, PartHdr->Header.HeaderSize, &PartHdr->Header) ||
      PartHdr->MyLBA != LastLba
      ) {
    return FALSE;
  }
  return TRUE;
}

/**
  This routine will check GPT partition entry and return entry status.

  Caution: This function may receive untrusted input.
  The GPT partition entry is external input, so this routine
  will do basic validation for GPT partition entry and report status.

  @param[in]    PartHeader    Partition table header structure
  @param[in]    PartEntry     The partition entry array
  @param[out]   PEntryStatus  the partition entry status array 
                              recording the status of each partition

**/
VOID
PartitionCheckGptEntry (
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader,
  IN  EFI_PARTITION_ENTRY         *PartEntry,
  OUT EFI_PARTITION_ENTRY_STATUS  *PEntryStatus
  )
{
  EFI_LBA  StartingLBA;
  EFI_LBA  EndingLBA;
  UINTN    Index1;
  UINTN    Index2;

  for (Index1 = 0; Index1 < PartHeader->NumberOfPartitionEntries; Index1++) {
    if (CompareGuid (&PartEntry[Index1].PartitionTypeGUID, &mEfiPartTypeUnusedGuid)) {
      continue;
    }

    StartingLBA = PartEntry[Index1].StartingLBA;
    EndingLBA   = PartEntry[Index1].EndingLBA;
    if (StartingLBA > EndingLBA ||
        StartingLBA < PartHeader->FirstUsableLBA ||
        StartingLBA > PartHeader->LastUsableLBA ||
        EndingLBA < PartHeader->FirstUsableLBA ||
        EndingLBA > PartHeader->LastUsableLBA
        ) {
      PEntryStatus[Index1].OutOfRange = TRUE;
      continue;
    }

    for (Index2 = Index1 + 1; Index2 < PartHeader->NumberOfPartitionEntries; Index2++) {

      if (CompareGuid (&PartEntry[Index2].PartitionTypeGUID, &mEfiPartTypeUnusedGuid)) {
        continue;
      }

      if (PartEntry[Index2].EndingLBA >= StartingLBA && PartEntry[Index2].StartingLBA <= EndingLBA) {
        //
        // This region overlaps with the Index1'th region
        //
        PEntryStatus[Index1].Overlap  = TRUE;
        PEntryStatus[Index2].Overlap  = TRUE;
        continue;

      }
    }
  }
}

/**
  This function finds GPT partitions

  @param  PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param  ParentBlockDevNo  The parent block device 

  @retval TRUE              New partitions are detected and logical block devices 
                            are  added to block device array 
  @retval FALSE             No New partitions are added;

**/
BOOLEAN
FatFindGptPartitions (
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
  )
{
  EFI_STATUS                  Status;
  MASTER_BOOT_RECORD          *PMbr;
  UINTN                       Index;
  BOOLEAN                     Found;
  PEI_FAT_BLOCK_DEVICE        *ParentBlockDev;
  PEI_FAT_BLOCK_DEVICE        *BlockDev;
  EFI_PARTITION_TABLE_HEADER  *Gpt;
  EFI_PARTITION_ENTRY         *PartEntry;
  EFI_PARTITION_ENTRY         *Entry;
  EFI_PARTITION_ENTRY_STATUS  *PEntryStatus;

  if (ParentBlockDevNo > PEI_FAT_MAX_BLOCK_DEVICE - 1) {
    return FALSE;
  }

  ParentBlockDev  = &(PrivateData->BlockDevice[ParentBlockDevNo]);

  Found           = FALSE;
  PMbr            = (MASTER_BOOT_RECORD *) PrivateData->BlockData;

  Status = FatReadBlock (
             PrivateData,
             ParentBlockDevNo,
             0,
             ParentBlockDev->BlockSize,
             PMbr
             );

  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Verify that the Protective MBR is valid
  //
  for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) {
    if (PMbr->Partition[Index].BootIndicator == 0x00 &&
        PMbr->Partition[Index].OSIndicator == PMBR_GPT_PARTITION &&
        UNPACK_UINT32 (PMbr->Partition[Index].StartingLBA) == 1
        ) {
      break;
    }
  }

  if (Index == MAX_MBR_PARTITIONS) {
    goto Done;
  }
  //
  // Read the EFI Gpt table
  //
  Status = (**PrivateData->PeiServices).AllocatePool (
                                          (CONST EFI_PEI_SERVICES **)PrivateData->PeiServices,
                                          sizeof (EFI_PARTITION_TABLE_HEADER),
                                          (VOID**)&Gpt
                                          );
  Status = FatReadBlock (
             PrivateData,
             ParentBlockDevNo,
             PRIMARY_PART_HEADER_LBA,
             ParentBlockDev->BlockSize,
             Gpt
             );

  if (EFI_ERROR (Status) || !PartitionValidGptTable (Gpt, ParentBlockDev->BlockSize, PRIMARY_PART_HEADER_LBA)) {
    goto Done;
  }
  //
  // Read the EFI Partition Entries
  //
  Status = (**PrivateData->PeiServices).AllocatePool (
                                          (CONST EFI_PEI_SERVICES **)PrivateData->PeiServices,
                                          Gpt->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY),
                                          (VOID**)&PartEntry
                                          );
  Status = FatReadBlock (
             PrivateData,
             ParentBlockDevNo,
             Gpt->PartitionEntryLBA,
             Gpt->NumberOfPartitionEntries * (Gpt->SizeOfPartitionEntry),
             PartEntry
             );

  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = (**PrivateData->PeiServices).AllocatePool (
                                          (CONST EFI_PEI_SERVICES **)PrivateData->PeiServices,
                                          Gpt->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY_STATUS),
                                          (VOID**)&PEntryStatus
                                          );
  if (PEntryStatus == NULL) {
    goto Done;
  }
  ZeroMem (PEntryStatus, Gpt->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY_STATUS));
  //
  // Check the integrity of partition entries
  //
  PartitionCheckGptEntry (Gpt, PartEntry, PEntryStatus);
  //
  // We have a valid mbr - add each partition
  //
  for (Index = 0; Index < Gpt->NumberOfPartitionEntries; Index++) {
    Entry = (EFI_PARTITION_ENTRY *) ((UINT8 *) PartEntry + Index * Gpt->SizeOfPartitionEntry);
    if (CompareGuid (&Entry->PartitionTypeGUID, &mEfiPartTypeUnusedGuid) ||
        PEntryStatus[Index].OutOfRange ||
        PEntryStatus[Index].Overlap
        ) {
      //
      // Don't use null EFI Partition Entries, Invalid Partition Entries or OS specific
      // partition Entries
      //
      continue;
    }
    //
    // Register this partition
    //
    if (PrivateData->BlockDeviceCount < PEI_FAT_MAX_BLOCK_DEVICE) {

      Found                       = TRUE;

      BlockDev                    = &(PrivateData->BlockDevice[PrivateData->BlockDeviceCount]);

      BlockDev->BlockSize         = MBR_SIZE;
      BlockDev->LastBlock         = DivU64x32 (Entry->EndingLBA - Entry->StartingLBA + 1, ParentBlockDev->BlockSize);
      BlockDev->IoAlign           = ParentBlockDev->IoAlign;
      BlockDev->Logical           = TRUE;
      BlockDev->PartitionChecked  = FALSE;
      BlockDev->StartingPos       = MultU64x32 (Entry->StartingLBA, ParentBlockDev->BlockSize);
      BlockDev->ParentDevNo       = ParentBlockDevNo;

      PrivateData->BlockDeviceCount++;
    }
  }

Done:

  ParentBlockDev->PartitionChecked = TRUE;
  return Found;
}

