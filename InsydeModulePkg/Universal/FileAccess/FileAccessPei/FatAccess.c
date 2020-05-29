/** @file
  FAT file system access routines for file access PEIM

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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FatAccess.c

Abstract:

  FAT file system access routines for file access PEIM

--*/

#include "FatPeim.h"

/**
  Check if there is a valid FAT in the corresponding Block device
  of the volume and if yes, fill in the relevant fields for the
  volume structure. Note there should be a valid Block device number
  already set.

  @param  PrivateData   Global memory map for accessing global
                                 variables.
  @param  Volume                 On input, the BlockDeviceNumber field of the
                                 Volume  should be a valid value. On successful
                                 output, all  fields except the VolumeNumber
                                 field is initialized.

  @retval EFI_SUCCESS            A FAT is found and the volume structure is
                                 initialized.
  @retval EFI_NOT_FOUND          There is no FAT on the corresponding device.
  @retval EFI_DEVICE_ERROR       There is something error while accessing device.

**/
EFI_STATUS
FatGetBpbInfo (
  IN      PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN OUT  PEI_FAT_VOLUME        *Volume
  )
{
  EFI_STATUS              Status;
  PEI_FAT_BOOT_SECTOR     Bpb;
  PEI_FAT_BOOT_SECTOR_EX  BpbEx;
  UINT32                  Sectors;
  UINT32                  SectorsPerFat;
  UINT32                  RootDirSectors;
  UINT64                  FatLba;
  UINT64                  RootLba;
  UINT64                  FirstClusterLba;

  //
  // Read in the BPB
  //
  Status = FatReadDisk (
             PrivateData,
             Volume->BlockDeviceNo,
             0,
             sizeof (PEI_FAT_BOOT_SECTOR_EX),
             &BpbEx
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (
    (UINT8 *) (&Bpb),
    (UINT8 *) (&BpbEx),
    sizeof (PEI_FAT_BOOT_SECTOR)
    );

  Volume->FatType = FatUnknown;

  Sectors         = Bpb.Sectors;
  if (Sectors == 0) {
    Sectors = Bpb.LargeSectors;
  }

  SectorsPerFat = Bpb.SectorsPerFat;
  if (SectorsPerFat == 0) {
    SectorsPerFat   = BpbEx.LargeSectorsPerFat;
    Volume->FatType = Fat32;
  }
  //
  // Filter out those not a FAT
  //
  if (Bpb.Ia32Jump[0] != 0xe9 && Bpb.Ia32Jump[0] != 0xeb && Bpb.Ia32Jump[0] != 0x49) {
    return EFI_NOT_FOUND;
  }

  if (Bpb.ReservedSectors == 0 || Bpb.NoFats == 0 || Sectors == 0) {
    return EFI_NOT_FOUND;
  }

  if (Bpb.SectorsPerCluster != 1 &&
      Bpb.SectorsPerCluster != 2 &&
      Bpb.SectorsPerCluster != 4 &&
      Bpb.SectorsPerCluster != 8 &&
      Bpb.SectorsPerCluster != 16 &&
      Bpb.SectorsPerCluster != 32 &&
      Bpb.SectorsPerCluster != 64 &&
      Bpb.SectorsPerCluster != 128
      ) {
    return EFI_NOT_FOUND;
  }

  if (Volume->FatType == Fat32 && (SectorsPerFat == 0 || BpbEx.FsVersion != 0)) {
    return EFI_NOT_FOUND;
  }

  if (Bpb.Media != 0xf0 &&
      Bpb.Media != 0xf8 &&
      Bpb.Media != 0xf9 &&
      Bpb.Media != 0xfb &&
      Bpb.Media != 0xfc &&
      Bpb.Media != 0xfd &&
      Bpb.Media != 0xfe &&
      Bpb.Media != 0xff &&
      //
      // FujitsuFMR
      //
      Bpb.Media != 0x00 &&
      Bpb.Media != 0x01 &&
      Bpb.Media != 0xfa
      ) {
    return EFI_NOT_FOUND;
  }

  if (Volume->FatType != Fat32 && Bpb.RootEntries == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // If this is fat32, refuse to mount mirror-disabled volumes
  //
  if (Volume->FatType == Fat32 && ((BpbEx.ExtendedFlags & 0x80) != 0)) {
    return EFI_NOT_FOUND;
  }
  //
  // Fill in the volume structure fields
  // (Sectors & SectorsPerFat is computed earlier already)
  //
  Volume->ClusterSize = Bpb.SectorSize * Bpb.SectorsPerCluster;
  Volume->RootEntries = Bpb.RootEntries;
  Volume->SectorSize  = Bpb.SectorSize;

  RootDirSectors = ((Volume->RootEntries * sizeof (FAT_DIRECTORY_ENTRY)) + (Volume->SectorSize - 1)) / Volume->SectorSize;

  FatLba                  = Bpb.ReservedSectors;
  RootLba                 = Bpb.NoFats * SectorsPerFat + FatLba;
  FirstClusterLba         = RootLba + RootDirSectors;

  Volume->VolumeSize      = MultU64x32 (Sectors, Volume->SectorSize);
  Volume->FatPos          = MultU64x32 (FatLba, Volume->SectorSize);
  Volume->RootDirPos      = MultU64x32 (RootLba, Volume->SectorSize);
  Volume->FirstClusterPos = MultU64x32 (FirstClusterLba, Volume->SectorSize);
  Volume->MaxCluster      = (UINT32) (Sectors - FirstClusterLba) / Bpb.SectorsPerCluster;
  Volume->RootDirCluster  = BpbEx.RootDirFirstCluster;

  //
  // If this is not a fat32, determine if it's a fat16 or fat12
  //
  if (Volume->FatType != Fat32) {
    Volume->FatType = Volume->MaxCluster < 4085 ? Fat12 : Fat16;
  }

  return EFI_SUCCESS;
}

/**
  Get the physical starting address of the cluster.

  @param[in]  PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]  Volume            Pointer to the PEI_FAT_VOLUME structure
  @param[in]  ClusterNo         The cluster number
  @param[out] Position          Pointer to the physical starting address of the cluster

  @retval EFI_SUCCESS           The physical address of the cluster is successfully retrieved
  @retval EFI_INVALID_PARAMETER ClusterNo exceeds the MaxCluster of the volume.

**/
EFI_STATUS
FatGetClusterPos (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_VOLUME        *Volume,
  IN  UINT32                ClusterNo,
  OUT UINT64                *Position
  )
{
  if (ClusterNo > Volume->MaxCluster + 1) {
    return EFI_INVALID_PARAMETER;
  }

  *Position = Volume->FirstClusterPos + MultU64x32 (Volume->ClusterSize, ClusterNo - 2);

  return EFI_SUCCESS;
}

/**
  Get the next cluster in the cluster chain

  @param  PrivateData            Global memory map for accessing global variables
  @param  Volume                 The volume
  @param  Cluster                The cluster
  @param  NextCluster            The cluster number of the next cluster

  @retval EFI_SUCCESS            The address is got
  @retval EFI_INVALID_PARAMETER  ClusterNo exceeds the MaxCluster of the volume.
  @retval EFI_DEVICE_ERROR       Read disk error

**/
EFI_STATUS
FatGetNextCluster (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_VOLUME        *Volume,
  IN  UINT32                Cluster,
  OUT UINT32                *NextCluster
  )
{
  EFI_STATUS  Status;
  UINT64      FatEntryPos;

  Status = EFI_SUCCESS;

  if ((Cluster > Volume->MaxCluster + 1) || (Cluster < 2)) {
    return EFI_INVALID_PARAMETER;
  }

  *NextCluster  = 0;

  if (Volume->FatType == Fat32) {
    FatEntryPos = Volume->FatPos + MultU64x32 (4, Cluster);

    Status      = FatReadDisk (PrivateData, Volume->BlockDeviceNo, FatEntryPos, 4, NextCluster);
    *NextCluster &= 0x0fffffff;

    //
    // Pad high bits for our FAT_CLUSTER_... macro definitions to work
    //
    if ((*NextCluster) >= 0x0ffffff7) {
      *NextCluster |= (-1 &~0xf);
    }

  } else if (Volume->FatType == Fat16) {
    FatEntryPos = Volume->FatPos + MultU64x32 (2, Cluster);

    Status      = FatReadDisk (PrivateData, Volume->BlockDeviceNo, FatEntryPos, 2, NextCluster);

    //
    // Pad high bits for our FAT_CLUSTER_... macro definitions to work
    //
    if ((*NextCluster) >= 0xfff7) {
      *NextCluster |= (-1 &~0xf);
    }

  } else {
    FatEntryPos = Volume->FatPos + DivU64x32 (MultU64x32 (3, Cluster), 2);

    Status      = FatReadDisk (PrivateData, Volume->BlockDeviceNo, FatEntryPos, 2, NextCluster);

    if ((Cluster & 0x01) != 0) {
      *NextCluster = (*NextCluster) >> 4;
    } else {
      *NextCluster = (*NextCluster) & 0x0fff;
    }
    //
    // Pad high bits for our FAT_CLUSTER_... macro definitions to work
    //
    if ((*NextCluster) >= 0x0ff7) {
      *NextCluster |= (-1 &~0xf);
    }
  }

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;

}


/**
  Read the cluster data

  @param[in]  PrivateData       Global memory map for accessing global variables
  @param[in]  Volume            The volume
  @param[in]  Cluster           The cluster number
  @param[in]  BufferSize        Size of the data buffer
  @param[out] Buffer            The data buffer to store the cluster data

  @retval EFI_SUCCESS           The cluster is successfully read
  @retval EFI_INVALID_PARAMETER The cluster number exceeds the max cluster number
                                allowed in this volume
  @retval EFI_DEVICE_ERROR      Something error while accessing media.

**/
EFI_STATUS
FatReadCluster (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_VOLUME        *Volume,
  IN  UINT32                Cluster,
  IN  UINTN                 BufferSize,
  OUT VOID                  *Buffer
  )
{
  EFI_STATUS  Status;
  UINT32      ClusterSize;
  UINT64      ClusterPos;

  Status = EFI_SUCCESS;

  if (Cluster > Volume->MaxCluster + 1) {
    return EFI_INVALID_PARAMETER;
  }

  Status = FatGetClusterPos (PrivateData, Volume, Cluster, &ClusterPos);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  ClusterSize = Volume->ClusterSize;

  if (BufferSize < ClusterSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Status = FatReadDisk (PrivateData, Volume->BlockDeviceNo, ClusterPos, ClusterSize, Buffer);

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Compute the amount of data that could be read continguously from the file's
  current position

  @param[in]  PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]  File              Pointer to the PEI_FAT_FILE structure

  @retval EFI_SUCCESS           Function executed successfully
  @retval EFI_DEVICE_ERROR      Something error while accessing media.

**/
EFI_STATUS
FatComputeStraightRead (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *File
  )
{
  EFI_STATUS  Status;
  UINT32      Cluster;
  UINT32      PrevCluster;
  UINTN       Offset;

  if (File->IsFixedRootDir) {

    File->StraightReadAmount = (UINT32) (MultU64x32 (File->Volume->RootEntries, 32) - File->CurrentPos);
  } else {

    File->StraightReadAmount  = 0;
    Cluster                   = File->CurrentCluster;
    while (!FAT_CLUSTER_FUNCTIONAL (Cluster)) {
      File->StraightReadAmount += File->Volume->ClusterSize;
      PrevCluster = Cluster;
      Status      = FatGetNextCluster (PrivateData, File->Volume, Cluster, &Cluster);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      if (Cluster != PrevCluster + 1) {
        break;
      }
    }

    DivU64x32Remainder (File->CurrentPos, File->Volume->ClusterSize, &Offset);
    File->StraightReadAmount -= (UINT32) Offset;

  }

  return EFI_SUCCESS;
}

/**
  Set a file's CurrentPos and CurrentCluster.

  @param[in]  PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]  File              Pointer to the PEI_FAT_FILE structure
  @param[in]  Pos               The file position
  @param[in]  Absolute          if TRUE, the Position is counted from starting of file
                                if FALSE, tht Position is counted from File->CurrentPos

  @retval EFI_SUCCESS           Function executed successfully
  @retval EFI_INVALID_PARAMETER Pos is beyond file's size.
  @retval EFI_DEVICE_ERROR      Something error while accessing media.

**/
EFI_STATUS
FatSetFilePos (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *File,
  IN  UINT32                Pos,
  IN  BOOLEAN               Absolute
  )
{
  EFI_STATUS  Status;
  UINT32      ClusterSize;
  UINT32      AlignedPos;
  UINTN       Offset;

  if (Absolute) {
    File->CurrentPos      = 0;
    File->CurrentCluster  = File->StartingCluster;
  }

  if (File->IsFixedRootDir) {

    if (Pos >= MultU64x32 (File->Volume->RootEntries, 32) - File->CurrentPos) {
      return EFI_INVALID_PARAMETER;
    }

    File->CurrentPos += Pos;

  } else {

    ClusterSize = File->Volume->ClusterSize;
    DivU64x32Remainder (File->CurrentPos, ClusterSize, &Offset);
    AlignedPos = (UINT32) File->CurrentPos - (UINT32) Offset;

    while (!FAT_CLUSTER_FUNCTIONAL (File->CurrentCluster) && AlignedPos + ClusterSize <= File->CurrentPos + Pos) {
      AlignedPos += ClusterSize;
      Status = FatGetNextCluster (
                 PrivateData,
                 File->Volume,
                 File->CurrentCluster,
                 &File->CurrentCluster
                 );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
    }

    if (FAT_CLUSTER_FUNCTIONAL (File->CurrentCluster)) {
      return EFI_INVALID_PARAMETER;
    }

    File->CurrentPos += Pos;

  }

  return FatComputeStraightRead (PrivateData, File);
}

/**
  Read file data. Updates the file's CurrentPos.

  @param  PrivateData            Global memory map for accessing global variables
  @param  File                   The file.
  @param  Size                   Pointer to the amount of data to read.
  @param  Buffer                 The buffer storing the data.

  @retval EFI_SUCCESS            The data is read.
  @retval EFI_INVALID_PARAMETER  File is invalid.
  @retval EFI_DEVICE_ERROR       Something error while accessing media.

**/
EFI_STATUS
FatReadFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *File,
  IN  UINTN                 *Size,
  OUT VOID                  *Buffer
  )
{
  EFI_STATUS  Status;
  CHAR8       *BufferPtr;
  UINTN       Offset;
  UINT64      PhysicalAddr;
  UINTN       ClusterSize;
  UINTN       Amount;
  UINTN       TotalAmount;

  Status      = EFI_SUCCESS;
  TotalAmount = 0;
  ClusterSize = File->Volume->ClusterSize;

  BufferPtr   = Buffer;

  if (File->IsFixedRootDir) {
    //
    // This is the fixed root dir in FAT12 and FAT16
    //
    *Size = *Size < (File->Volume->RootEntries * 32 - File->CurrentPos) ? *Size : (UINTN) (File->Volume->RootEntries * 32 - File->CurrentPos);

    Status = FatReadDisk (
               PrivateData,
               File->Volume->BlockDeviceNo,
               File->Volume->RootDirPos + File->CurrentPos,
               *Size,
               Buffer
               );
    File->CurrentPos += (UINT32) *Size;
    return Status;

  } else {
    //
    // Fat32 
    //
    if (!(File->Attributes & FAT_ATTR_DIRECTORY)) {
      *Size = *Size < (File->FileSize - File->CurrentPos) ? *Size : (UINTN) (File->FileSize - File->CurrentPos);
    }
    //
    // This is a normal cluster based file
    //
    while (*Size) {
      DivU64x32Remainder (File->CurrentPos, ClusterSize, &Offset);
      Status = FatGetClusterPos (PrivateData, File->Volume, File->CurrentCluster, &PhysicalAddr);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      Amount  = File->StraightReadAmount;
      Amount  = *Size > Amount ? Amount : *Size;

      Status = FatReadDisk (
                 PrivateData,
                 File->Volume->BlockDeviceNo,
                 PhysicalAddr + Offset,
                 Amount,
                 BufferPtr
                 );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      //
      // Advance the file's current pos and current cluster
      //
      FatSetFilePos (PrivateData, File, (UINT32) Amount, FALSE);

      BufferPtr += Amount;
      *Size -= Amount;
      TotalAmount += Amount;

      //
      // if this is the end of cluster chain?
      //
      if (FAT_CLUSTER_FUNCTIONAL (File->CurrentCluster)) {
        *Size = TotalAmount;
        return EFI_SUCCESS;
      }
    }

    *Size = TotalAmount;
    return Status;
  }
}

/**
  Write file data. Updates the file's CurrentPos.

  @param  PrivateData            Global memory map for accessing global variables
  @param  File                   The file.
  @param  Size                   Pointer to the amount of data to write.
  @param  Buffer                 The buffer storing the data.

  @retval EFI_SUCCESS            The data is write.
  @retval EFI_INVALID_PARAMETER  File is invalid.
  @retval EFI_DEVICE_ERROR       Something error while accessing media.

  if write directory  used  FatWriteDisk(), because File is locate at start of one Cluster, directory not.

**/
EFI_STATUS
FatWriteFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *File,
  IN  UINTN                 *Size,
  IN  VOID                  *Buffer
  )
{
  EFI_STATUS  Status;
  CHAR8       *BufferPtr;
  UINTN       Offset;
  UINT64      PhysicalAddr;   //Physical bytes address
  UINTN       ClusterSize;
  UINTN       Amount;
  UINTN       TotalAmount;

  Status      = EFI_SUCCESS;
  TotalAmount = 0;
  ClusterSize = File->Volume->ClusterSize;

  BufferPtr   = Buffer;

  
 if (File->IsFixedRootDir) {
   //
   // This is the fixed root dir in FAT12 and FAT16
   //

   *Size = *Size < (File->Volume->RootEntries * 32 - File->CurrentPos) ? *Size : (UINTN) (File->Volume->RootEntries * 32 - File->CurrentPos);


   Status = FatWriteDisk (
              PrivateData,
              File->Volume->BlockDeviceNo,
              File->Volume->RootDirPos + File->CurrentPos,
              *Size,
              Buffer
              );
   File->CurrentPos += (UINT32) *Size;
   return Status;

 } else {
    //
    // This part is FAT32
    //
    
    if (!(File->Attributes & FAT_ATTR_DIRECTORY)) {
      *Size = *Size < (File->FileSize - File->CurrentPos) ? *Size : (UINTN) (File->FileSize - File->CurrentPos);
    }

    //
    // This is a normal cluster based file
    //
    while (*Size) {
      DivU64x32Remainder (File->CurrentPos, ClusterSize, &Offset);
      Status = FatGetClusterPos (PrivateData, File->Volume, File->CurrentCluster, &PhysicalAddr);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      Amount = File->StraightReadAmount;
      Amount = *Size > Amount ? Amount : *Size;

      Status = FatWriteDisk (
                 PrivateData,
                 File->Volume->BlockDeviceNo,
                 PhysicalAddr + Offset,
                 Amount,
                 BufferPtr
                 );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      //
      // Advance the file's current pos and current cluster
      //
      FatSetFilePos (PrivateData, File, (UINT32) Amount, FALSE);

      BufferPtr += Amount;
      *Size -= Amount;
      TotalAmount += Amount;
      
      //
      // if this is the end of cluster chain?
      //
      if (FAT_CLUSTER_FUNCTIONAL (File->CurrentCluster)) {
        *Size = TotalAmount;
        return EFI_SUCCESS;
      }

    }

    *Size = TotalAmount;
    return Status;

 }

}


/**
  This function reads the next item in the parent directory and
  initializes the output parameter SubFile (CurrentPos is initialized to 0).
  The function updates the CurrentPos of the parent dir to after the item read.
  If no more items were found, the function returns EFI_NOT_FOUND.

  @param[in]  PrivateData       Global memory map for accessing global variables
  @param[in]  ParentDir         The parent directory
  @param[out] SubFile           The retrieved file

  @retval EFI_SUCCESS           The next sub file is successfully retrieved.
  @retval EFI_INVALID_PARAMETER The ParentDir is not a directory.
  @retval EFI_NOT_FOUND         No more sub file found
  @retval EFI_DEVICE_ERROR      Something error while accessing media.

**/
EFI_STATUS
FatReadNextSubFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *ParentDir,
  OUT PEI_FAT_FILE          *SubFile
  )
{
  EFI_STATUS          Status;
  UINTN               Dummy;
  UINTN               DataSize;
  FAT_DIRECTORY_ENTRY DirEntryBuffer;
  FAT_DIRECTORY_ENTRY *DirEntry;
  FAT_DIRECTORY_LFN   *LfnEntry;
  UINT8               LfnOrdinal;
  UINT8               LfnChecksum;
  CHAR16              *Pos;
  BOOLEAN             IsInvalidEntry;
  UINT64              CurrentPos;
  CHAR16              BaseName[9];
  CHAR16              Ext[4];

  Status    = EFI_SUCCESS;
  DirEntry  = (FAT_DIRECTORY_ENTRY *) &DirEntryBuffer;
  LfnEntry  = (FAT_DIRECTORY_LFN *) &DirEntryBuffer;
  (*PrivateData->PeiServices)->SetMem ((UINT8 *) SubFile, sizeof (PEI_FAT_FILE), 0);

  //
  // Check the parameter
  //
  if (!(ParentDir->Attributes & FAT_ATTR_DIRECTORY)) {
    return EFI_INVALID_PARAMETER;
  }

  CurrentPos = ParentDir->CurrentPos;
  DivU64x32Remainder (CurrentPos, 32, &Dummy);
  if (Dummy) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Pick a valid directory entry
  //
  while (1) {

    IsInvalidEntry = FALSE;

    //
    // Read one entry
    //
    DataSize  = 32;
    Status    = FatReadFile (PrivateData, ParentDir, &DataSize, &DirEntryBuffer);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }

    DirEntry  = (FAT_DIRECTORY_ENTRY *) &DirEntryBuffer;
    LfnEntry  = (FAT_DIRECTORY_LFN *) &DirEntryBuffer;

    if (DataSize < 32) {
      return EFI_NOT_FOUND;
    }
    //
    // Just read the 8Dot3 name here, if it's the beginning of a lfn,
    // we will overwrite it soon
    //
    EngFatToStr (8, DirEntry->FileName, BaseName);
    EngFatToStr (3, DirEntry->FileName + 8, Ext);

    Pos = (UINT16 *) SubFile->FileName;

    SetMem ((UINT8 *) Pos, FAT_MAX_FILE_NAME_LENGTH, 0);

    CopyMem ((UINT8 *) Pos, (UINT8 *) BaseName, StrSize (BaseName));

    if (Ext[0] != 0) {
      Pos += StrLen (BaseName);
      *Pos = '.';
      Pos++;
      CopyMem ((UINT8 *) Pos, (UINT8 *) Ext, StrSize (Ext));
    }
    //
    // If the entry is the start of a long file name, start processing it
    //
    if (DirEntry->Attributes == FAT_ATTR_LFN &&
        LfnEntry->MustBeZero == 0 &&
        (LfnEntry->Ordinal & FAT_LFN_LAST) &&
        (LfnEntry->Ordinal &~FAT_LFN_LAST) > 0 &&
        (LfnEntry->Ordinal &~FAT_LFN_LAST) < FAT_MAX_LFN_ENTRIES
        ) {

      LfnOrdinal  = (UINT8) (LfnEntry->Ordinal &~FAT_LFN_LAST);
      LfnChecksum = LfnEntry->Checksum;

      //
      // Loop and read each portion of the name
      //
      for (;;) {
        //
        // Copy this portion of the name to the file string
        //
        if ((LfnOrdinal - 1) * 13 < FAT_MAX_FILE_NAME_LENGTH) {
          Pos = SubFile->FileName + (LfnOrdinal - 1) * 13;
        } else {
          //
          // If the file name is more than FAT_MAX_FILE_NAME_LENGTH,
          // set the positiion to the last.
          //
          Pos = SubFile->FileName + FAT_MAX_FILE_NAME_LENGTH - 1;
        }

        if (Pos + 12 < (SubFile->FileName + FAT_MAX_FILE_NAME_LENGTH)) {
          CopyMem ((UINT8 *) (Pos + 0), (UINT8 *) LfnEntry->Name1, sizeof (CHAR16) * 5);
          CopyMem ((UINT8 *) (Pos + 5), (UINT8 *) LfnEntry->Name2, sizeof (CHAR16) * 6);
          CopyMem ((UINT8 *) (Pos + 11), (UINT8 *) LfnEntry->Name3, sizeof (CHAR16) * 2);
        }
        //
        // If this is the last LFN dir entry, stop
        //
        LfnOrdinal -= 1;
        if (LfnOrdinal == 0) {
          break;
        }
        //
        // Read next name fragment
        //
        DataSize  = 32;
        Status    = FatReadFile (PrivateData, ParentDir, &DataSize, &DirEntryBuffer);
        if (EFI_ERROR (Status)) {
          return EFI_DEVICE_ERROR;
        }

        DirEntry  = (FAT_DIRECTORY_ENTRY *) &DirEntryBuffer;
        LfnEntry  = (FAT_DIRECTORY_LFN *) &DirEntryBuffer;

        if (DataSize < 32) {
          return EFI_NOT_FOUND;
        }
        //
        // If something is wrong with the entry, just return it as unknown
        //
        if (DirEntry->Attributes != FAT_ATTR_LFN ||
            LfnEntry->MustBeZero != 0 ||
            LfnEntry->Ordinal != LfnOrdinal ||
            LfnEntry->Checksum != LfnChecksum
            ) {

          IsInvalidEntry = TRUE;
          break;
        }
      }
      //
      // end of Lfn entries reading
      //
      // Read the 8.3 file entry for this lfn
      //
      DataSize  = 32;
      Status    = FatReadFile (PrivateData, ParentDir, &DataSize, &DirEntryBuffer);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      DirEntry  = (FAT_DIRECTORY_ENTRY *) &DirEntryBuffer;
      LfnEntry  = (FAT_DIRECTORY_LFN *) &DirEntryBuffer;

      if (DataSize < 32) {
        return EFI_NOT_FOUND;
      }
      //
      // Verify the LFN is for this entry
      //
      if (FatDirEntryChecksum (DirEntry) != LfnChecksum) {
        IsInvalidEntry = TRUE;
      }

    }
    //
    // end of Lfn processing
    //
    // if this is a terminator dir entry, just return EFI_NOT_FOUND
    //
    if (DirEntry->FileName[0] == 0) {
      return EFI_NOT_FOUND;
    }
    //
    // If this not an invalid entry neither an empty entry, this is what we want.
    // otherwise we will start a new loop to continue to find something meaningful
    //
    if (!IsInvalidEntry && DirEntry->FileName[0] != (CHAR8)(UINT8)0xe5) {
      break;
    }
  }
  //
  // fill in the output parameter
  //
  SubFile->Attributes             = DirEntry->Attributes;
  SubFile->CurrentCluster         = DirEntry->FileClusterHigh << 16 | DirEntry->FileCluster;
  SubFile->CurrentPos             = 0;
  SubFile->DirEntryPos            = ParentDir->CurrentPos - 32;
  SubFile->FileSize               = DirEntry->FileSize;
  SubFile->ParentStartingCluster  = ParentDir->StartingCluster;
  SubFile->StartingCluster        = SubFile->CurrentCluster;
  SubFile->FileLastCluster        = 0;
  SubFile->Volume                 = ParentDir->Volume;
  SubFile->IsRootDir              = FALSE;

  CopyMem ((UINT8 *)&SubFile->FileDir, (UINT8 *) &DirEntryBuffer, sizeof (FAT_DIRECTORY_ENTRY));

  if (SubFile->StartingCluster) {
    Status = FatSetFilePos (PrivateData, SubFile, 0, TRUE);
  }
  //
  // in Pei phase, time parameters do not need to be filled for minimum use.
  //
  return Status;
}

/**
  Construct root dir file instance for a volume

  @param[in]  PrivateData       Pointer to the PEF_FAT_PRIVATE_DATA structure
  @param[in]  Volume            Pointer to the PEI_FAT_VOLUME structure
  @param[out] RootDir           The retrieved root directory

  @retval EFI_SUCCESS           The root directory is successfully retrieved.
  @retval EFI_INVALID_PARAMETER The volume instance is invalid.
  @retval EFI_DEVICE_ERROR      Something error while accessing media.

**/
EFI_STATUS
FatGetRootDir (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_VOLUME        *Volume,
  OUT PEI_FAT_FILE          *RootDir
  )
{
  RootDir->IsRootDir        = TRUE;
  RootDir->IsFixedRootDir   = (BOOLEAN) ((Volume->FatType == Fat32) ? FALSE : TRUE);
  RootDir->Attributes       = FAT_ATTR_DIRECTORY;
  RootDir->CurrentPos       = 0;
  RootDir->CurrentCluster   = RootDir->IsFixedRootDir ? 0 : Volume->RootDirCluster;
  RootDir->StartingCluster  = RootDir->CurrentCluster;
  RootDir->Volume           = Volume;

  return FatSetFilePos (PrivateData, RootDir, 0, TRUE);
}

/**
  Search the Root directory entry for the possible volume label.

  @param[in]  PrivateData      Pointer to the PEF_FAT_PRIVATE_DATA structure
  @param[in]  ParentDir        The parent directory.
  @param[out] DirEnt           The returned directory entry of volume label.

  @retval EFI_SUCCESS           The search process is completed successfully.
  @return others                An error occurred while searching volume label.

**/
EFI_STATUS
FatFindVolumeLabel (
  IN     PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN     PEI_FAT_FILE          *ParentDir,
  OUT    FAT_DIRECTORY_ENTRY   *DirEnt
  )
{
  EFI_STATUS           Status;
  UINT16               EntryPos;
  FAT_DIRECTORY_ENTRY  *Entry;
  UINTN                BufferSize;

  EntryPos   = 0;
  Entry      = DirEnt;
  BufferSize = sizeof (FAT_DIRECTORY_ENTRY);

  do {
    Status = FatReadFile (PrivateData, ParentDir, &BufferSize, Entry);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((Entry->FileName[0] != FAT_DELETE_ENTRY_MARK) && (((Entry->Attributes) & (~FAT_ATTR_ARCHIVE)) == FAT_ATTR_VOLUME_ID)) {
      break;
    }

    EntryPos++;
  } while (Entry->FileName[0] != FAT_EMPTY_ENTRY_MARK);
  return EFI_SUCCESS;
}

