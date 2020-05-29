/** @file

  The definition of USB Access PEI PPI.
  
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

#ifndef _EFI_PEI_FILE_ACCESS_PPI_H_
#define _EFI_PEI_FILE_ACCESS_PPI_H_

#include <Uefi.h>

#define EFI_PEI_FILE_ACCESS_PPI_GUID \
  { \
    0x7d1a7139, 0xf9a2, 0x41b6, { 0xaa, 0xd9, 0xf3, 0x7c, 0x8b, 0xcc, 0x86, 0xf4 } \
  }

typedef struct _PEI_FILE_ACCESS_PPI  PEI_FILE_ACCESS_PPI;



//
// API data structures
//
typedef VOID  *PEI_FILE_HANDLE;


//
// File attributes
//
#define EFI_FILE_READ_ONLY  0x0000000000000001
#define EFI_FILE_HIDDEN     0x0000000000000002
#define EFI_FILE_SYSTEM     0x0000000000000004
#define EFI_FILE_RESERVIED  0x0000000000000008
#define EFI_FILE_DIRECTORY  0x0000000000000010
#define EFI_FILE_ARCHIVE    0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037

//
// Open modes
//
#define EFI_FILE_MODE_READ    0x0000000000000001
#define EFI_FILE_MODE_WRITE   0x0000000000000002
#define EFI_FILE_MODE_CREATE  0x8000000000000000ULL


typedef enum {
  ReadData,
  ReadDirEntry,
  ReadDirEntryBufferTooSmallOK
} PEI_FILE_READ_MODE;

typedef struct {
  UINT64    Size;
  UINT64    FileSize;
  UINT64    PhysicalSize;
  EFI_TIME  CreateTime;
  EFI_TIME  LastAccessTime;
  EFI_TIME  ModificationTime;
  UINT64    Attribute;
  CHAR16    FileName[1];                // Null terminated unicode string
} PEI_FILE_INFO;

/**
  Implement Openfile from one volume
  if readmode, all volume will search
  in write mode, open first volume
 
  @param[in]     This                Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     FileName           
  @param[out]   Handle             Get the File Handle.
  @param[in]     OpenMode        Read, Write, Create mode
  @param[in]     Attributes         These are the attribute bits for the newly created file. 
  
  @retval        EFI_SUCCESS                  File infomation is successfully returned
  @retval        EFI_INVALID_PARAMETER  Invalid file handle
  @retval        EFI_BUFFER_TOO_SMALL   The given Buffer size is too small
  @retval        EFI_DEVICE_ERROR          Can't get root directory
  @retval        EFI_NOT_FOUND              The specified file could not be found on the device.
 
**/
typedef
EFI_STATUS
(EFIAPI *PEI_OPEN_FILE)(
  IN  PEI_FILE_ACCESS_PPI   *This,
  IN  CHAR16                *FileName,
  OUT PEI_FILE_HANDLE       *Handle,
  IN  UINT64                OpenMode,
  IN  UINT8                 Attributes  
  );

/**
  Read file data
 
  @param[in]     This                Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     Handle           The File handle
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
typedef
EFI_STATUS
(EFIAPI *PEI_READ_FILE) (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN      PEI_FILE_READ_MODE    ReadMode,
  IN OUT  UINTN                 *BufferSize,
  OUT     VOID                  *Buffer
  );

/**
  Write data to file
 
  @param[in]     This                Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     Handle           The File handle
  @param[in]     BufferSize       Input: the given buffer size
  @param[out]    Buffer           The pointer to the data buffer
 
  @retval        EFI_SUCCESS            File data is successfully read
  @retval        EFI_INVALID_PARAMETER  Unknown ReadMode given
  @retval        EFI_NOT_FOUND          File not found
  @retval        EFI_DEVICE_ERROR       Failed to access file
  @retval        EFI_OUT_OF_RESOURCES   Out of memory
 
**/ 
typedef
EFI_STATUS
(EFIAPI *PEI_WRITE_FILE) (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN      UINTN                 *BufferSize,
  IN      VOID                  *Buffer
  );
 
/**
  Close a file
 
  @param[in]     This              Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     Handle         The File handle with PEI_FILE_HANDLE type
 
  @retval        EFI_SUCCESS    The file is successfuly closed
  @retval        EFI_INVALID_PARAMETER   The give file handle is invalid
 
**/ 
typedef
EFI_STATUS
(EFIAPI *PEI_CLOSE_FILE) (
  IN  PEI_FILE_ACCESS_PPI   *This,
  IN  PEI_FILE_HANDLE       Handle
  );
 
/**
  Change file position
 
  @param[in]     Handle         The File handle with PEI_FILE_HANDLE type
  @param[in]     Position        The file's position of the file.
  
  @retval        EFI_SUCCESS    Set the info successfully.
 
**/ 
typedef
EFI_STATUS
(EFIAPI *PEI_SET_POSITION) (
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
typedef
EFI_STATUS
(EFIAPI *PEI_GET_POSITION) (
  IN  PEI_FILE_HANDLE   Handle,
  OUT UINT64            *Position
  );

/**
  Get file information
 
  @param[in]     This             Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     Handle         The File handle
  @param[in,out] BufferSize     Input:  the given buffer size
                                         Output: the buffer size of the output buffer
  @param[out]    Buffer         The pointer to the PEI_FILE_INFO structure
 
  @retval        EFI_SUCCESS            File infomation is successfully returned
  @retval        EFI_INVALID_PARAMETER  Invalid file handle
  @retval        EFI_BUFFER_TOO_SMALL   The given Buffer size is too small
 
**/ 
typedef
EFI_STATUS
(EFIAPI *PEI_GET_FILE_INFO) (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN OUT  UINTN                 *BufferSize,
  OUT     VOID                  *Buffer
  );

/**
  Get FAT Volume Information
 
  @param[in]     This                  Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     VolumeIndex       The index number of the FAT volume
  @param[in]     BufferSize
  @param[out]    Buffer
 
  @retval        EFI_SUCCESS
  @retval        EFI_INVALID_PARAMETER
  @retval        EFI_BUFFER_TOO_SMALL
 
**/ 
typedef
EFI_STATUS
(EFIAPI *PEI_GET_VOLUME_INFO) (
  IN      PEI_FILE_ACCESS_PPI    *This,
  IN      UINTN                  VolumeIndex,
  IN OUT  UINTN                  *BufferSize,
  OUT     VOID                   *Buffer
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
typedef
VOID 
(EFIAPI *PEI_GET_RECOVERY_BLOCK_INFO) (
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
typedef
VOID
(EFIAPI *PEI_GET_RECOVERY_BLOCK_TYPE) (
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
typedef
VOID
(EFIAPI *PEI_GET_RECOVERY_LOGICAL) (
  IN  PEI_FILE_ACCESS_PPI         *This,
  IN  UINTN                       *BlockDevNo,
  OUT BOOLEAN                     *LogicalBlock
  );


struct _PEI_FILE_ACCESS_PPI {
  PEI_OPEN_FILE               OpenFile;
  PEI_READ_FILE               ReadFile;
  PEI_WRITE_FILE              WriteFile;
  PEI_CLOSE_FILE              CloseFile;
  PEI_SET_POSITION            SetPosition;
  PEI_GET_POSITION            GetPosition;
  PEI_GET_FILE_INFO           GetFileInfo;
  PEI_GET_VOLUME_INFO         GetVolumeInfo;  
  PEI_GET_RECOVERY_BLOCK_INFO GetRecoveryBlockInfo;
  PEI_GET_RECOVERY_BLOCK_TYPE GetRecoveryBlockType;
  PEI_GET_RECOVERY_LOGICAL    GetRecoveryLogical;
};

extern EFI_GUID  gPeiFileAccessPpiGuid;

#endif


