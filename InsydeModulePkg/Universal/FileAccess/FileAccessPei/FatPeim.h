/** @file
  Data structures for file access PEIM

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
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
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

  FatPeim.h

Abstract:

  Data structures for file access PEIM

--*/

#ifndef _FAT_PEIM_H_
#define _FAT_PEIM_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Ppi/BlockIo.h>
#include <Ppi/H2OBlockIoPei.h>
#include <Ppi/FileAccessPei.h>
#include "FatFmt.h"

//
// Definitions
//
#define PEI_FAT_CACHE_SIZE        4
#define PEI_FAT_MAX_BLOCK_SIZE    8192
#define FAT_MAX_FILE_NAME_LENGTH  128
#define FAT_MAX_FILE_PATH_LENGTH  256
#define PEI_FAT_MAX_BLOCK_DEVICE  128
#define PEI_FAT_MAX_BLOCK_IO_PPI  32
#define PEI_FAT_MAX_VOLUME        64
#define PEI_FAT_MAX_FILE          32
#define PEI_FAT_MEMMORY_PAGE_SIZE 0x1000

#define CHAR_FAT_VALID            0x01

//
// Data Structures
//
typedef enum {
  Fat12,
  Fat16,
  Fat32,
  FatUnknown
} PEI_FAT_TYPE;

typedef struct {
  PEI_FAT_TYPE              FatType;
  EFI_PEI_BLOCK_DEVICE_TYPE DevType;        // Acquired from parent Block I/O
  CHAR16                    VolumeLabel[1]; // Null terminated unicode string
} PEI_VOLUME_INFO;

//
// The block device
//
typedef struct {
  UINT32                          BlockSize;
  UINT64                          LastBlock;
  UINT32                          IoAlign;
  BOOLEAN                         Logical;
  BOOLEAN                         PartitionChecked;

  //
  // Following fields only valid for logical device
  //
  CHAR8                           PartitionFlag[8];
  UINT64                          StartingPos;
  UINTN                           ParentDevNo;

  //
  // Following fields only valid for physical device
  //
  EFI_PEI_BLOCK_DEVICE_TYPE       DevType;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *BlockIo;
  H2O_PEI_BLOCK_IO_PPI            *H2OBlockIo;
  UINT8                           PhysicalDevNo;
} PEI_FAT_BLOCK_DEVICE;

//
// the Volume structure
//
typedef struct {
  UINTN         BlockDeviceNo;
  UINTN         VolumeNo;
  UINT64        VolumeSize;       //total size in bytes
  UINTN         MaxCluster;
  CHAR16        VolumeLabel[FAT_MAX_FILE_NAME_LENGTH];
  PEI_FAT_TYPE  FatType;
  UINT64        FatPos;           //Fat start         bytes address
  UINT32        SectorSize;
  UINT32        ClusterSize;      //bytes, in one Cluster
  UINT64        FirstClusterPos;  //FirstCluster     bytes address
  UINT64        RootDirPos;       //Root Directory bytes address 
  UINT32        RootEntries;
  UINT32        RootDirCluster;

  //
  // Current part of fat table that's present
  //
  UINT64        FatEntryPos;      // Location of buffer
  UINTN         FatEntrySize;     // Size of buffer
  UINT32        FatEntryBuffer;   // The buffer  

} PEI_FAT_VOLUME;


//
// File instance
//
typedef struct {

  BOOLEAN         Valid;

  PEI_FAT_VOLUME  *Volume;
  CHAR16          FileName[FAT_MAX_FILE_NAME_LENGTH];
  UINT64          DirEntryPos;
  UINT32          ParentStartingCluster;

  BOOLEAN         IsRootDir;
  BOOLEAN         IsFixedRootDir;

  UINT32          StartingCluster;
  UINT32          CurrentPos;           //offset in file
  UINT32          StraightReadAmount;
  UINT32          CurrentCluster;
  UINT32          FileLastCluster;      

  UINT8           Attributes;
  UINT32          FileSize;
  EFI_TIME        LastAccessTime;
  EFI_TIME        LastModTime;
  EFI_TIME        CreateTime;

  CHAR16          FilePath[FAT_MAX_FILE_PATH_LENGTH];

  FAT_DIRECTORY_ENTRY   FileDir;

} PEI_FAT_FILE;

//
// Cache Buffer
//
typedef struct {

  BOOLEAN Valid;
  UINTN   BlockDeviceNo;
  UINT64  Lba;
  UINT32  Lru;
  UINT64  Buffer[PEI_FAT_MAX_BLOCK_SIZE / 8];
  UINTN   Size;

} PEI_FAT_CACHE_BUFFER;

//
// Private Data.
// This structure abstracts the whole memory usage in FAT PEIM.
// The entry point routine will get a chunk of memory (by whatever
// means) whose size is sizeof(PEI_FAT_PRIVATE_DATA), which is clean
// in both 32 and 64 bit environment. The boundary of the memory chunk
// should be 64bit aligned.
//
#define PEI_FAT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('p', 'f', 'a', 't')

typedef struct {

  UINTN                                 Signature;
  EFI_PEI_SERVICES                      **PeiServices;
  PEI_FILE_ACCESS_PPI                   PeiFileAccessPpi;
  EFI_PEI_PPI_DESCRIPTOR                PpiDescriptor;
  EFI_PEI_NOTIFY_DESCRIPTOR             NotifyDescriptor;

  UINT8                                 UnicodeCaseMap[0x300];
  CHAR8                                 *EngUpperMap;
  CHAR8                                 *EngLowerMap;
  CHAR8                                 *EngInfoMap;

  UINT64                                BlockData[PEI_FAT_MAX_BLOCK_SIZE / 8];
  UINTN                                 BlockDeviceCount;
  PEI_FAT_BLOCK_DEVICE                  BlockDevice[PEI_FAT_MAX_BLOCK_DEVICE];
  UINTN                                 VolumeCount;
  PEI_FAT_VOLUME                        Volume[PEI_FAT_MAX_VOLUME];
  PEI_FAT_FILE                          File[PEI_FAT_MAX_FILE];
  PEI_FAT_CACHE_BUFFER                  CacheBuffer[PEI_FAT_CACHE_SIZE];

} PEI_FAT_PRIVATE_DATA;

#define PEI_FAT_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
          PEI_FAT_PRIVATE_DATA, \
          PeiFileAccessPpi, \
          PEI_FAT_PRIVATE_DATA_SIGNATURE \
      )

//
// Extract INT32 from char array
//
#define UNPACK_INT32(a) \
  (INT32) ((((UINT8 *) a)[0] << 0) | (((UINT8 *) a)[1] << 8) | (((UINT8 *) a)[2] << 16) | (((UINT8 *) a)[3] << 24))

//
// Extract UINT32 from char array
//
#define UNPACK_UINT32(a) \
  (UINT32) ((((UINT8 *) a)[0] << 0) | (((UINT8 *) a)[1] << 8) | (((UINT8 *) a)[2] << 16) | (((UINT8 *) a)[3] << 24))

/**
  Get number of FAT volumes

  @param[in]     PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure

  @return        Number of FAT volumes

**/
UINTN
GetNumberOfVolumes (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData
  );

/**
  Get FAT Volume Information

  @param[in]     PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]     VolumeIndex       The index number of the FAT volume
  @param[in]     BufferSize        
  @param[out]    Buffer

  @retval        EFI_SUCCESS
  @retval        EFI_INVALID_PARAMETER
  @retval        EFI_BUFFER_TOO_SMALL

**/
EFI_STATUS
GetVolumeInfo (
  IN      PEI_FILE_ACCESS_PPI    *This,
  IN      UINTN                  VolumeIndex,
  IN OUT  UINTN                  *BufferSize,
  OUT     VOID                   *Buffer
  );

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
  );

/**
  Open a file

  @param[in]     PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]     VolumeIndex       The index number of the FAT volume
  @param[in]     FileName          File name in Unicode string        
  @param[out]    Handle            Pointer to the PEI_FILE_HANDLE structure

  @retval        EFI_SUCCESS
  @retval        EFI_INVALID_PARAMETER
  @retval        EFI_NOT_FOUND
  @retval        EFI_DEVICE_ERROR
  @retval        EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
OpenFile (
  IN  PEI_FILE_ACCESS_PPI   *This,
  IN  CHAR16                *FileName,
  OUT PEI_FILE_HANDLE       *Handle,
  IN  UINT64                OpenMode,
  IN  UINT8                 Attributes  
  );

/**
  Get file information 

  @param[in]     PrivateData    Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]     Handle         The File handle
  @param[in,out] BufferSize     Input: the given buffer size 
                                Output: the buffer size of the output buffer        
  @param[out]    Buffer         The pointer to the PEI_FILE_INFO structure

  @retval        EFI_SUCCESS            File infomation is successfully returned
  @retval        EFI_INVALID_PARAMETER  Invalid file handle
  @retval        EFI_BUFFER_TOO_SMALL   The given Buffer size is too small

**/
EFI_STATUS
GetFileInfo (
  IN      PEI_FILE_ACCESS_PPI    *This,
  IN      PEI_FILE_HANDLE        Handle,
  IN OUT  UINTN                  *BufferSize,
  OUT     VOID                   *Buffer
  );

/**
  Read file data

  @param[in]     PrivateData    Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]     Handle         The File handle
  @param[in]     ReadMode       File read mode in PEI_FILE_READ_MODE type
  @param[in,out] BufferSize     Input: the given buffer size 
                                Output: the buffer size of the output buffer        
  @param[out]    Buffer         The pointer to the data buffer 

  @retval        EFI_SUCCESS            File data is successfully read
  @retval        EFI_INVALID_PARAMETER  Unknown ReadMode given
  @retval        EFI_NOT_FOUND          File not found
  @retval        EFI_DEVICE_ERROR       Failed to access file
  @retval        EFI_OUT_OF_RESOURCES   Out of memory

**/
EFI_STATUS
ReadFile (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN      PEI_FILE_READ_MODE    ReadMode,
  IN OUT  UINTN                 *BufferSize,
  OUT     VOID                  *Buffer
  );

EFI_STATUS
WriteFile (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN      UINTN                 *BufferSize,
  IN      VOID                  *Buffer
  );

/**
  Close a file

  @param[in]     PrivateData    Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]     Handle         The File handle with PEI_FILE_HANDLE type

  @retval        EFI_SUCCESS    The file is successfuly closed
  @retval        EFI_INVALID_PARAMETER   The give file handle is invalid

**/
EFI_STATUS
CloseFile (
  IN  PEI_FILE_ACCESS_PPI   *This,
  IN  PEI_FILE_HANDLE       Handle
  );
  
/**
  This version is different from the version in Unicode collation
  protocol in that this version strips off trailing blanks.
  Converts an 8.3 FAT file name using an OEM character set
  to a Null-terminated Unicode string.
  Here does not expand DBCS FAT chars.

  @param  FatSize           The size of the string Fat in bytes. 
  @param  Fat               A pointer to a Null-terminated string that contains 
                            an 8.3 file name using an OEM character set. 
  @param  Str               A pointer to a Null-terminated Unicode string. The 
                            string must be allocated in advance to hold FatSize 
                            Unicode characters

**/
VOID
EngFatToStr (
  IN UINTN                            FatSize,
  IN CHAR8                            *Fat,
  OUT CHAR16                          *Str
  );

/**
  Converts a Null-terminated string to legal characters in a FAT 
  filename using an OEM character set. 

  @param  This    Protocol instance pointer.
  @param  String  A pointer to a Null-terminated string. The string must
                  be preallocated to hold FatSize characters.
  @param  FatSize The size of the string Fat in bytes.
  @param  Fat     A pointer to a Null-terminated string that contains an 8.3 file
                  name using an OEM character set.

  @retval TRUE    Fat is a Long File Name
  @retval FALSE   Fat is an 8.3 file name

**/
BOOLEAN
EFIAPI
EngStrToFat (
  IN CHAR16                           *String,
  IN UINTN                            FatSize,
  OUT CHAR8                           *Fat
  );

/**
  Performs a case-insensitive comparison of two Null-terminated Unicode strings.

  @param  PrivateData       Global memory map for accessing global variables 
  @param  Str1              First string to perform case insensitive comparison. 
  @param  Str2              Second string to perform case insensitive comparison.
  
  @retval TRUE              Str1 and Str2 are identical with case-insensitive comparison 
  @retval FALSE             Str1 and Str2 are different

**/
BOOLEAN
EngStriColl (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN CHAR16                 *s1,
  IN CHAR16                 *s2
  );

/**
  Reads a block of data from the block device by calling
  underlying Block I/O service.

  @param  PrivateData       Global memory map for accessing global variables 
  @param  BlockDeviceNo     The index for the block device number. 
  @param  Lba               The logic block address to read data from. 
  @param  BufferSize        The size of data in byte to read. 
  @param  Buffer            The buffer of the 

  @retval EFI_DEVICE_ERROR  The specified block device number exceeds the maximum 
                            device number. 
  @retval EFI_DEVICE_ERROR  The maximum address has exceeded the maximum address 
                            of the block device.

**/
EFI_STATUS
FatReadBlock (
  IN PEI_FAT_PRIVATE_DATA   *PrivateData,
  IN UINTN                  BlockDeviceNo,
  IN UINT64                 LBA,
  IN UINTN                  BufferSize,
  IN VOID                   *Buffer
  );

/**
  Write a block of data to the block device by calling
  underlying Block I/O service.

  @param  PrivateData       Global memory map for accessing global variables 
  @param  BlockDeviceNo     The index for the block device number. 
  @param  Lba               The logic block address to read data from. 
  @param  BufferSize        The size of data in byte to read. 
  @param  Buffer            The buffer of the 

  @retval EFI_DEVICE_ERROR  The specified block device number exceeds the maximum 
                            device number. 
  @retval EFI_DEVICE_ERROR  The maximum address has exceeded the maximum address 
                            of the block device.

**/
EFI_STATUS
FatWriteBlock (
  IN  PEI_FAT_PRIVATE_DATA   *PrivateData,
  IN  UINTN                  BlockDeviceNo,
  IN  EFI_PEI_LBA            Lba,
  IN  UINTN                  BufferSize,
  IN  VOID                   *Buffer
  );

/**
  Check if there is a valid FAT in the corresponding Block device
  of the volume and if yes, fill in the relevant fields for the
  volume structure. Note there should be a valid Block device number
  already set.

  @param  PrivateData            Global memory map for accessing global 
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
  );

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
  );

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
  );

/**
  Disk reading.

  @param  PrivateData       the global memory map; 
  @param  BlockDeviceNo     the block device to read; 
  @param  StartingAddress   the starting address. 
  @param  Size              the amount of data to read. 
  @param  Buffer            the buffer holding the data 

  @retval EFI_SUCCESS       The function completed successfully.
  @retval EFI_DEVICE_ERROR  Something error.

**/
EFI_STATUS
FatReadDisk (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 BlockDeviceNo,
  IN  UINT64                StartingAddress,
  IN  UINTN                 Size,
  OUT VOID                  *Buffer
  );

EFI_STATUS
FatWriteDisk (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 BlockDeviceNo,
  IN  UINT64                StartingAddress,
  IN  UINTN                 Size,
  IN  VOID                  *Buffer
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  Calculating FAT directory entry checksum

  @param  Dir               Pointer to the FAT_DIRECTORY_ENTRY structure

  @return                   The calculated directory entry checksum

**/
UINT8
FatDirEntryChecksum (
  FAT_DIRECTORY_ENTRY *Dir
  );

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
  );
  
/**
  This function finds partitions (logical devices) in physical block devices.

  @param  PrivateData       Global memory map for accessing global variables.

**/
VOID
FatFindPartitions (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData
  );

/**
  Grow the end of the open file base on the NewSizeInBytes.

  @param[in]  PrivateData        Pointer to the PEF_FAT_PRIVATE_DATA structure
  @param[in]  Volume              Pointer to the PEI_FAT_VOLUME structure
  @param[in]  File                   The open file.
  @param[in]  NewSizeInBytes  The new size in bytes of the open file.
  
  @retval EFI_SUCCESS                  The file is grown sucessfully.
  @retval EFI_UNSUPPORTED           The file size is larger than 4GB.
  @retval EFI_VOLUME_CORRUPTED  There are errors in the files' clusters.
  @retval EFI_VOLUME_FULL            The volume is full and can not grow the file.

**/
EFI_STATUS
FatGrowEof (
  IN PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN PEI_FAT_VOLUME        *Volume,
  IN PEI_FAT_FILE          *File,
  IN UINT64                NewSizeInBytes
  );

/**
  Create a new Fat Directory entry.

  @param[in]  PrivateData        Pointer to the PEF_FAT_PRIVATE_DATA structure
  @param[in]  Parent               Pointer to Parent Directory.
  @param[in]  FileName            
  @param[in]  Attributes          File's Attributes
  @param[out]  PEI_FAT_FILE   return File Entry
  
  @retval EFI_SUCCESS                  Create the directory sucessfully.
  @retval EFI_UNSUPPORTED           The file name is not 8.3 format.  
  @retval EFI_END_OF_MEDIA          There are no spaces to add directory.
  @retval EFI_DEVICE_ERROR           The device error.

**/
EFI_STATUS
FatCreateDirEnt (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,  
  IN  PEI_FAT_FILE          *Parent,
  IN  CHAR16                *FileName,
  IN  UINT8                 Attributes,
  OUT PEI_FAT_FILE          *NewFile
  );

/**
  This function checks whether the input FileName is a valid 8.3 short name.
  If the input FileName is a valid 8.3, the output is the 8.3 short name;
  otherwise, the output is the base tag of 8.3 short name.

 @param[in]     FileName          The input unicode filename. 
 @param[out]   File8Dot3Name  The output ascii 8.3 short name or base tag of 8.3 short name.
 
 @return TRUE   The input unicode filename is a valid 8.3 short name.
 @return FALSE  The input unicode filename is not a valid 8.3 short name.
**/
BOOLEAN
FatCheckIs8Dot3Name (
  IN  CHAR16    *FileName,
  OUT CHAR8     *File8Dot3Name
  );

/**
 Translate EFI time to FAT time.

 @param[in]    ETime  The time of EFI_TIME.
 @param[out]  FTime  The time of FAT_DATE_TIME.

 @return  None.
**/
VOID
FatEfiTimeToFatTime (
  IN  EFI_TIME        *ETime,
  OUT FAT_DATE_TIME   *FTime
  );

/**
 Get Current FAT time.

 @param[in]    PEI_FAT_PRIVATE_DATA   Pointer to the PEI_FAT_PRIVATE_DATA structure
 @param[in]    FAT_DATE_TIME              The time of FAT_DATE_TIME.

 @return  None.
**/
VOID
FatGetCurrentFatTime (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData, 
  OUT FAT_DATE_TIME         *FatNow
  );

/**
  Set the File's cluster and size info in its directory entry.

  @param[in]  File        The open's file entry.
  
  @retval    none.
  
**/
VOID
FatUpdateDirEntClusterSizeInfo (
  IN PEI_FAT_FILE    *File
  );

/**
  Flush the data associated with an open file.
  In this implementation, only last Mod/Access time is updated.
  
  @param[in]  PrivateData     Pointer to the PEF_FAT_PRIVATE_DATA structure
  @param[in]  File                The open's file entry.
  
  @retval    EFI_INVALID_PARAMETER.  File's Directory have some error.
  @retval    EFI_SUCCESS.                 Flush file successed.
  @retval    EFI_DEVICE_ERROR           Something error.
**/
EFI_STATUS
FatFileFlush (
  IN PEI_FAT_PRIVATE_DATA  *PrivateData, 
  IN PEI_FAT_FILE          *File
  );

/**
  Change file position
 
  @param[in]     Handle         The File handle with PEI_FILE_HANDLE type
  @param[in]     Position        The file's position of the file.
  
  @retval        EFI_SUCCESS    Set the info successfully.
 
**/ 
EFI_STATUS
EFIAPI
SetPosition (
  IN PEI_FILE_HANDLE    Handle,
  IN UINT64             Position
  );

/**
  Get the file's position of the file.
 
  @param[in]     Handle         The File handle with PEI_FILE_HANDLE type
  @param[in]     Position        The file's position of the file.
  
  @retval        EFI_SUCCESS           Get the info successfully.
  @retval        EFI_UNSUPPORTED    The open file is not a file.
  
**/  
EFI_STATUS
EFIAPI
GetPosition (
  IN PEI_FILE_HANDLE    Handle,
  OUT UINT64            *Position
  );

/**
  End of pei phase notification function.
  Reset all USB host controller.

  @param  PeiServices             General purpose services available to every PEIM.
  @param  NotifyDescriptor       The typedef structure of the notification
                                           descriptor. Not used in this function.
  @param  Ppi                        The typedef structure of the PPI descriptor.
                                           Not used in this function.

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
EndOfPeiNotifyEntry (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

/**
  Get the Block device info from PrivateData.
  CRISIS RECOVERY must use this function.
  
  @param[in]     This                      Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     Handle                  The File handle
  @param[out]   LogicalBlock           Is this block a logical device?
  @param[out]   BlockDevNo            Block index in PrivateData
  @param[out]   ParentDevNo          ParentDev index in PrivateData
  @param[out]   BlockDeviceCount    How many Block devices found.
  
  @retval          none
  
**/ 
VOID
EFIAPI
GetRecoveryBlockInfo (
  IN  PEI_FILE_ACCESS_PPI         *This,
  IN  PEI_FILE_HANDLE             Handle,
  OUT BOOLEAN                     *LogicalBlock,
  OUT UINTN                       *BlockDevNo,
  OUT UINTN                       *ParentDevNo,
  OUT UINTN                       *BlockDeviceCount
  );

/**
  Get the Block device type from PrivateData.
  CRISIS RECOVERY must use this function.
  
  @param[in]     This                      Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     BlockDevNo            Block index in PrivateData
  @param[out]   LogicalBlock           Is this block a logical device?
  @param[out]   BlockDevType         Device Type
  
  @retval          none
  
**/ 
VOID
EFIAPI
GetRecoveryBlockType(
  IN  PEI_FILE_ACCESS_PPI         *This,
  IN  UINTN                       BlockDevNo,
  OUT VOID                        *BlockDevType
  );

/**
  Get the parent block device Logical from PrivateData.
  CRISIS RECOVERY must use this function.
  
  @param[in]     This                      Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     BlockDevNo            Block index in PrivateData
  @param[out]   LogicalBlock           Is this block a logical device?
  
  @retval          none
  
**/  
VOID
EFIAPI
GetRecoveryLogical (
  IN  PEI_FILE_ACCESS_PPI         *This,
  IN  UINTN                       *BlockDevNo,
  OUT BOOLEAN                     *LogicalBlock
  );

#endif

