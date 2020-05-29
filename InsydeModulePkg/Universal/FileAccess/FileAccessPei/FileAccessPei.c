/** @file
  Implementation of file access PEIM

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FatPeim.h"

PEI_FAT_PRIVATE_DATA  *mPrivateData = NULL;

/**
  Get number of FAT volumes

  @param[in]     PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure

  @return        Number of FAT volumes

**/
UINTN
GetNumberOfVolumes (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData
  )
{
  return PrivateData->VolumeCount;
}

/**
  Discover all the block I/O devices to find the FAT volume.

  @param  PrivateData             Global memory map for accessing global
                                  variables.

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
UpdateBlocksAndVolumes (
  PEI_FAT_PRIVATE_DATA            *PrivateData
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_PPI_DESCRIPTOR          *TempPpiDescriptor;
  UINTN                           BlockIoPpiInstance;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *BlockIoPpi;
  H2O_PEI_BLOCK_IO_PPI            *H2OBlockIoPpi;
  UINTN                           NumberBlockDevices;
  UINTN                           Index;
  EFI_PEI_BLOCK_IO_MEDIA          Media;
  PEI_FAT_VOLUME                  Volume;
  EFI_PEI_SERVICES                **PeiServices;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

  //
  // Clean up caches
  //
  for (Index = 0; Index < PEI_FAT_CACHE_SIZE; Index++) {
    PrivateData->CacheBuffer[Index].Valid = FALSE;
  }

  PrivateData->BlockDeviceCount = 0;

  //
  // Find out all Block Io Ppi instances within the system
  // Assuming all device Block Io Peims are dispatched already
  //
  for (BlockIoPpiInstance = 0; BlockIoPpiInstance < PEI_FAT_MAX_BLOCK_IO_PPI; BlockIoPpiInstance++) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiVirtualBlockIoPpiGuid,
               BlockIoPpiInstance,
               &TempPpiDescriptor,
               (VOID **)&BlockIoPpi
               );
    if (EFI_ERROR (Status)) {
      //
      // Done with all Block Io Ppis
      //
      break;
    }
    
    Status = PeiServicesLocatePpi (
               &gH2OPeiVirtualBlockIoPpiGuid,
               BlockIoPpiInstance,
               &TempPpiDescriptor,
               (VOID **)&H2OBlockIoPpi
               );
    if (EFI_ERROR (Status)) {
      break;
    }    

    Status = BlockIoPpi->GetNumberOfBlockDevices (
                           PeiServices,
                           BlockIoPpi,
                           &NumberBlockDevices
                           );
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (Index = 0; Index < NumberBlockDevices && PrivateData->BlockDeviceCount < PEI_FAT_MAX_BLOCK_DEVICE; Index++) {

      Status = BlockIoPpi->GetBlockDeviceMediaInfo (
                             PeiServices,
                             BlockIoPpi,
                             Index,
                             &Media
                            );
      if (EFI_ERROR (Status) || !Media.MediaPresent) {
        continue;
      }

      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].BlockSize = (UINT32) Media.BlockSize;
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].LastBlock = Media.LastBlock;

      if (Media.DeviceType == MaxDeviceType) {
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].IoAlign   = 4;
      } else {
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].IoAlign   = 0;
      }

      //
      // Not used here
      //
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].Logical           = FALSE;
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].PartitionChecked  = FALSE;

      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].BlockIo           = BlockIoPpi;
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].H2OBlockIo        = H2OBlockIoPpi;
      
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].PhysicalDevNo     = (UINT8) Index;
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].DevType           = Media.DeviceType;

      PrivateData->BlockDeviceCount++;
    }
  }
  //
  // Find out all logical devices
  //
  FatFindPartitions (PrivateData);

  //
  // Build up file system volume array
  //
  PrivateData->VolumeCount = 0;
  for (Index = 0; Index < PrivateData->BlockDeviceCount; Index++) {
    ZeroMem (&Volume, sizeof (PEI_FAT_VOLUME));
    Volume.BlockDeviceNo  = Index;
    Status                = FatGetBpbInfo (PrivateData, &Volume);
    if (Status == EFI_SUCCESS) {
      //
      // Add the detected volume to the volume array
      //
      (*PeiServices)->CopyMem (
                        (UINT8 *) &(PrivateData->Volume[PrivateData->VolumeCount]),
                        (UINT8 *) &Volume,
                        sizeof (PEI_FAT_VOLUME)
                        );
      PrivateData->VolumeCount += 1;
      if (PrivateData->VolumeCount >= PEI_FAT_MAX_VOLUME) {
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  BlockIo installation notification function. Find out all the current BlockIO
  PPIs in the system and add them into private data. Assume there is

  @param  PeiServices             General purpose services available to every
                                  PEIM.
  @param  NotifyDescriptor        The typedef structure of the notification
                                  descriptor. Not used in this function.
  @param  Ppi                     The typedef structure of the PPI descriptor.
                                  Not used in this function.

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
EFIAPI
BlockIoNotifyEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  UpdateBlocksAndVolumes (mPrivateData);

  return EFI_SUCCESS;
}

/**
  Perform File Access Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
FileAccessPeiEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Address;
  PEI_FAT_PRIVATE_DATA  *PrivateData;

  Status = (**PeiServices).AllocatePages (
                             PeiServices,
                             EfiBootServicesCode,
                             (sizeof (PEI_FAT_PRIVATE_DATA) - 1) / PEI_FAT_MEMMORY_PAGE_SIZE + 1,
                             &Address
                             );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData = (PEI_FAT_PRIVATE_DATA *) (UINTN) Address;

  //
  // Initialize Private Data (to zero, as is required by subsequent operations)
  //
  (*PeiServices)->SetMem ((UINT8 *) PrivateData, sizeof (PEI_FAT_PRIVATE_DATA), 0);
  PrivateData->Signature    = PEI_FAT_PRIVATE_DATA_SIGNATURE;
  PrivateData->PeiServices  = (EFI_PEI_SERVICES **)PeiServices;

  //
  // Installs Ppi
  //
  PrivateData->PeiFileAccessPpi.OpenFile             = OpenFile;
  PrivateData->PeiFileAccessPpi.ReadFile             = ReadFile;
  PrivateData->PeiFileAccessPpi.WriteFile            = WriteFile;
  PrivateData->PeiFileAccessPpi.CloseFile            = CloseFile;
  PrivateData->PeiFileAccessPpi.SetPosition          = SetPosition;
  PrivateData->PeiFileAccessPpi.GetPosition          = GetPosition;
  PrivateData->PeiFileAccessPpi.GetFileInfo          = GetFileInfo;
  PrivateData->PeiFileAccessPpi.GetVolumeInfo        = GetVolumeInfo;
  PrivateData->PeiFileAccessPpi.GetRecoveryBlockInfo = GetRecoveryBlockInfo;
  PrivateData->PeiFileAccessPpi.GetRecoveryBlockType = GetRecoveryBlockType;
  PrivateData->PeiFileAccessPpi.GetRecoveryLogical   = GetRecoveryLogical;

  PrivateData->PpiDescriptor.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PrivateData->PpiDescriptor.Guid = &gPeiFileAccessPpiGuid;
  PrivateData->PpiDescriptor.Ppi = &PrivateData->PeiFileAccessPpi;

  Status = (**PeiServices).InstallPpi (PeiServices, &PrivateData->PpiDescriptor);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  DEBUG ((EFI_D_INFO, "Install gPeiFileAccessPpiGuid\n"));
  
  //
  // Other initializations
  //
  PrivateData->BlockDeviceCount = 0;

  UpdateBlocksAndVolumes (PrivateData);

  //
  // PrivateData is allocated now, set it to the module variable
  //
  mPrivateData = PrivateData;

  //
  // Installs Block Io Ppi notification function
  //
  PrivateData->NotifyDescriptor.Flags =
    (
      EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST
    );
  PrivateData->NotifyDescriptor.Guid    = &gH2OPeiVirtualBlockIoPpiGuid;
  PrivateData->NotifyDescriptor.Notify  = BlockIoNotifyEntry;  

  Status  = (**PeiServices).NotifyPpi (PeiServices, &PrivateData->NotifyDescriptor);

  return Status;
}
 
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
EFI_STATUS
GetVolumeInfo (
  IN      PEI_FILE_ACCESS_PPI    *This,
  IN      UINTN                  VolumeIndex,
  IN OUT  UINTN                  *BufferSize,
  OUT     VOID                   *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_VOLUME_INFO       *VolumeInfo;
  PEI_FAT_FILE          ParentDir;
  FAT_DIRECTORY_ENTRY   DirEnt;
  PEI_FAT_PRIVATE_DATA  *PrivateData;  

  PrivateData  = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);  
 
  if (VolumeIndex > PrivateData->VolumeCount - 1) {
    return EFI_INVALID_PARAMETER;
  }
 
  VolumeInfo = Buffer;
  if (*BufferSize < sizeof (PEI_VOLUME_INFO)) {
    *BufferSize = sizeof (PEI_VOLUME_INFO);
 
    return EFI_BUFFER_TOO_SMALL;
  }
 
  Status = FatGetRootDir (PrivateData, &PrivateData->Volume[VolumeIndex], &ParentDir);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  ParentDir.DirEntryPos = 0;
  Status = FatFindVolumeLabel (PrivateData, &ParentDir, &DirEnt);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  VolumeInfo->FatType         = PrivateData->Volume[VolumeIndex].FatType;
  VolumeInfo->DevType         = PrivateData->BlockDevice[PrivateData->Volume[VolumeIndex].BlockDeviceNo].DevType;
  EngFatToStr (11, (CHAR8 *)&DirEnt.FileName, (UINT16 *) VolumeInfo->VolumeLabel);
  //
  // We don't support volume label currently
  //
  return EFI_SUCCESS;
} 
 
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
EFI_STATUS
GetFileInfo (
  IN      PEI_FILE_ACCESS_PPI    *This,
  IN      PEI_FILE_HANDLE        Handle,
  IN OUT  UINTN                  *BufferSize,
  OUT     VOID                   *Buffer
  )
{
  PEI_FAT_FILE  *File;
  PEI_FILE_INFO *FileInfo;
  UINTN         Size;
  PEI_FAT_PRIVATE_DATA  *PrivateData;  

  PrivateData  = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);  
  
  File = Handle;
  if (!File->Valid) {
    return EFI_INVALID_PARAMETER;
  }
 
  Size = sizeof (PEI_FILE_INFO) + StrLen (File->FilePath) * 2;
  if (*BufferSize < Size) {
    *BufferSize = Size;
 
    return EFI_BUFFER_TOO_SMALL;
  }
 
  FileInfo                = Buffer;
  FileInfo->Size          = Size;
  FileInfo->PhysicalSize  = (File->FileSize + File->Volume->ClusterSize - 1) / File->Volume->ClusterSize;
  FileInfo->FileSize      = File->FileSize;
  FileInfo->Attribute     = File->Attributes;
  CopyMem (
    (UINT8 *) FileInfo->FileName,
    (UINT8 *) File->FilePath,
    StrSize(File->FilePath)
    );
 
  //
  // Create, Access, Modification time of file are not supported currently
  //
  return EFI_SUCCESS;
} 
 
/**
  Implement Openfile from one volume
 
  @param[in]     PrivateData       Pointer to the PEI_FAT_PRIVATE_DATA structure
  @param[in]     VolumeIndex      Selet one Volume to open file
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
EFI_STATUS
OpenVolumeAndFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 VolumeIndex,
  IN  CHAR16                *FileName,
  OUT PEI_FILE_HANDLE       *Handle,
  IN  UINT64                OpenMode,
  IN  UINT8                 Attributes  
  )
{
  EFI_STATUS      Status;
  CHAR16          *Path1;
  CHAR16          *Path2;
  CHAR16          TempChar;
  PEI_FAT_FILE    RootDir;
  PEI_FAT_FILE    Parent;
  PEI_FAT_FILE    File;
  UINTN           Index;
  BOOLEAN         IsVolumeLabel;
  BOOLEAN         VolumeLabelValid;
  PEI_VOLUME_INFO *VolumeInfo;
  UINTN           BufferSize;

  //
  // Open the file from the file name
  //
  if (VolumeIndex >= PrivateData->VolumeCount) {
    return EFI_INVALID_PARAMETER;
  }

  memset (&File, 0, sizeof (PEI_FAT_FILE));
  
  IsVolumeLabel    = FALSE;
  VolumeLabelValid = FALSE;
  VolumeInfo       = NULL;
  Path1 = FileName;
  Path2 = FileName;
  Status  = FatGetRootDir (PrivateData, &PrivateData->Volume[VolumeIndex], &RootDir);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
 
  Parent = RootDir;
  do {
 
    while (*Path1 && *Path1 == '/') {
      Path1++;
      Path2++;
      IsVolumeLabel = TRUE;
    }
 
    while (*Path1 && *Path1 == '\\') {
      Path1++;
      Path2++;
      IsVolumeLabel = FALSE;
    }
 
    if (*Path1) {
      while ((*Path2 && *Path2 != '\\') && (*Path2 != ';')) {
        Path2++;
      }
 
      TempChar = *Path2;
      *Path2 = 0;
    } else {
      break;
    }
    if(IsVolumeLabel) {
      BufferSize = sizeof (PEI_VOLUME_INFO) + (sizeof(CHAR16) * 11);
      (*PrivateData->PeiServices)->AllocatePool (
                                     (CONST EFI_PEI_SERVICES **)PrivateData->PeiServices,
                                     BufferSize,
                                     (VOID**)&VolumeInfo
                                     );
      Status = GetVolumeInfo (&PrivateData->PeiFileAccessPpi, VolumeIndex, &BufferSize, VolumeInfo);
      if (EFI_ERROR (Status)) {
        return EFI_NOT_FOUND;
      }
      if (!EngStriColl (PrivateData, Path1, VolumeInfo->VolumeLabel)) {
        *Path2 = TempChar;
        while (*Path2 != ';') {
          Path2++;
          if (*Path2 == 0) {
            break;
          }
        }
        TempChar = *Path2;
      }
    } else {
      //
      // Open this component
      //
      Status = FatReadNextSubFile (PrivateData, &Parent, &File);
      while (Status == EFI_SUCCESS) {
        if ((EngStriColl (PrivateData, Path1, File.FileName)) && (File.Attributes != FAT_ATTR_VOLUME_ID)) {
          break;
        }
 
        Status = FatReadNextSubFile (PrivateData, &Parent, &File);
      }
 
      if (!EFI_ERROR (Status)) {
        CopyMem ((UINT8 *) &Parent, (UINT8 *) &File, sizeof (PEI_FAT_FILE));
        if (File.FileSize != 0) {
          break;
        }
      } else {
        Parent = RootDir;
      }
 
    }
 
    if (TempChar == ';') {
      *Path2 = TempChar;
      Path2++;
    } 
    Path1 = Path2;
 
  } while (1);
  
  if (EFI_ERROR (Status)) {
    //
    // Can't found File
    //
    if ((OpenMode & EFI_FILE_MODE_CREATE) == 0) {
      return EFI_NOT_FOUND;
      
    } else {
      //
      // Create new File & Get this File Handle
      // 
      Status = FatCreateDirEnt (PrivateData, &Parent, FileName, Attributes, &File);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
 
  //
  // If the file's attribute is read only, and the open is for
  // read-write, then the access is denied.
  //
  if ((File.Attributes & EFI_FILE_READ_ONLY) != 0 && (File.Attributes & EFI_FILE_DIRECTORY) == 0 && (OpenMode & EFI_FILE_MODE_WRITE)) {
    return EFI_ACCESS_DENIED;
  }
  
  //
  // Find one empty File structure for this open instance
  //
  for (Index = 0; Index < PEI_FAT_MAX_FILE; Index++) {
    if (!PrivateData->File[Index].Valid) {
      break;
    }
  }
 
  if (Index >= PEI_FAT_MAX_FILE) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Set this file structure
  //
  CopyMem ((UINT8 *) &(PrivateData->File[Index]), (UINT8 *) &File, sizeof (PEI_FAT_FILE));
 
  PrivateData->File[Index].Valid = TRUE;
 
  //
  // Copy the file path
  //
  CopyMem (
    (UINT8 *) PrivateData->File[Index].FilePath,
    (UINT8 *) FileName,
    StrLen (FileName) + 1 < FAT_MAX_FILE_PATH_LENGTH ? StrSize (FileName) : 2 * FAT_MAX_FILE_PATH_LENGTH
    );
 
  PrivateData->File[Index].FilePath[FAT_MAX_FILE_PATH_LENGTH - 1] = 0;
 
  *Handle = &(PrivateData->File[Index]);
 
  return EFI_SUCCESS;
 
}

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
EFI_STATUS
OpenFile (
  IN  PEI_FILE_ACCESS_PPI   *This,
  IN  CHAR16                *FileName,
  OUT PEI_FILE_HANDLE       *Handle,
  IN  UINT64                OpenMode,
  IN  UINT8                 Attributes  
  )
{
  EFI_STATUS            Status  = EFI_ABORTED;
  PEI_FAT_PRIVATE_DATA  *PrivateData;
  UINTN                 Index;
  UINTN                 VolumeCount;

  PrivateData  = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  
  //
  // Check for a valid mode
  //
  switch (OpenMode) {
  case EFI_FILE_MODE_READ:
  case EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE:
  case EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE:
    break;
 
  default:
    return EFI_INVALID_PARAMETER;
  }
 
  //
  // Check for valid Attributes for file creation case. 
  //
  if (((OpenMode & EFI_FILE_MODE_CREATE) != 0) && (Attributes & (EFI_FILE_READ_ONLY | (~EFI_FILE_VALID_ATTR))) != 0) {
    return EFI_INVALID_PARAMETER;
  }  
  
  //
  // Search each volume in the root directory for the Recovery capsule
  //
  VolumeCount  = GetNumberOfVolumes (PrivateData);

  //
  // fine  file name in each volume
  //
  for (Index = 0; Index < VolumeCount; Index++) {
    Status = OpenVolumeAndFile (PrivateData, Index, FileName, Handle, OpenMode, Attributes);
    
    if (!EFI_ERROR (Status)) {
      return Status;
      
    } else if ((OpenMode & EFI_FILE_MODE_CREATE) != 0) {
      break;
    }

  }

  return Status;
  
}

 
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
EFI_STATUS
ReadFile (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN      PEI_FILE_READ_MODE    ReadMode,
  IN OUT  UINTN                 *BufferSize,
  OUT     VOID                  *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_FAT_FILE          *File;
  PEI_FAT_FILE          SubFile;
  UINT32                OrigPos;
  PEI_FAT_PRIVATE_DATA  *PrivateData;  

  PrivateData  = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);   
  File         = Handle;
 
  if (!File->Valid) {
    return EFI_INVALID_PARAMETER;
  }
 
  if (ReadMode == ReadData) {
    //
    // read raw data
    // 
    Status = FatReadFile (PrivateData, Handle, BufferSize, Buffer); 
    return Status;
    
  } else if (ReadMode == ReadDirEntry) {
    //
    // read dir entry with BUFFER_TOO_SMALL rollback
    //
    OrigPos = File->CurrentPos;
    Status  = FatReadNextSubFile (PrivateData, File, &SubFile);
    if (Status == EFI_SUCCESS) {
      Status = GetFileInfo (This, &SubFile, BufferSize, Buffer);
    }
 
    if (Status == EFI_BUFFER_TOO_SMALL) {
      FatSetFilePos (PrivateData, File, OrigPos, TRUE);
    } 
    return Status;
 
  } else if (ReadMode == ReadDirEntryBufferTooSmallOK) {
    //
    // read dir entry (BUFFER_TOO_SMALL OK)
    //
    Status = FatReadNextSubFile (PrivateData, File, &SubFile);
    if (Status == EFI_SUCCESS) {
      Status = GetFileInfo (This, &SubFile, BufferSize, Buffer);
    } 
    return Status;
 
  } else { 
    return EFI_INVALID_PARAMETER;
  }
}

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
EFI_STATUS
WriteFile (
  IN      PEI_FILE_ACCESS_PPI   *This,
  IN      PEI_FILE_HANDLE       Handle,
  IN      UINTN                 *BufferSize,
  IN      VOID                  *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_FAT_FILE          *File;
  UINT64                EndPosition;
  PEI_FAT_VOLUME        *Volume;
  PEI_FAT_PRIVATE_DATA  *PrivateData;  

  PrivateData = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  File        = Handle;
  Volume      = File->Volume;
  
  if (!File->Valid) {
    return EFI_INVALID_PARAMETER;
  }
 
  if ((File->Attributes & FAT_ATTR_READ_ONLY) == FAT_ATTR_READ_ONLY) {
    return EFI_WRITE_PROTECTED;
  }
   
  EndPosition = File->CurrentPos + *BufferSize;
 
  //
  // The position goes beyond the end of file, We expand the file size
  //
  if (EndPosition > File->FileSize) {
    Status = FatGrowEof (PrivateData, Volume, File, EndPosition);    
    if (EFI_ERROR (Status)) {
      return Status;  
    } 
    FatUpdateDirEntClusterSizeInfo (File); 
  } 
  
  Status = FatWriteFile (PrivateData, Handle, BufferSize, Buffer);
 
  if (EFI_ERROR(Status)) {
    return Status;  
  }
 
  Status = FatFileFlush (PrivateData, File);  
  return Status;  
}
 
 
/**
  Close a file
 
  @param[in]     This              Pointer to the PEI_FILE_ACCESS_PPI
  @param[in]     Handle         The File handle with PEI_FILE_HANDLE type
 
  @retval        EFI_SUCCESS    The file is successfuly closed
  @retval        EFI_INVALID_PARAMETER   The give file handle is invalid
 
**/
EFI_STATUS
CloseFile (
  IN  PEI_FILE_ACCESS_PPI   *This,
  IN  PEI_FILE_HANDLE       Handle
  )
{
  PEI_FAT_FILE          *File;
  PEI_FAT_PRIVATE_DATA  *PrivateData;  

  PrivateData = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  File        = Handle;
  
  if (!File->Valid) { 
    return EFI_INVALID_PARAMETER;
  }
 
  File->Valid = FALSE; 
  return EFI_SUCCESS;
}

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
  )
{
  PEI_FAT_FILE   *File; 
  
  File = Handle;
  
  //
  // If this is a directory, we can only set back to position 0
  //
  if ((File->Attributes & FAT_ATTR_DIRECTORY) == FAT_ATTR_DIRECTORY) {
    File->CurrentPos = 0;
  }
 
  //
  // Set the end of position
  //
  if (Position == (UINT64)-1) {
    Position = (UINT32) File->FileSize;
  }
  
  //
  // Set the position
  //
  File->CurrentPos = (UINT32)Position;
  return EFI_SUCCESS;
}
 
 
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
  )
{
  PEI_FAT_FILE   *File; 
  
  File = Handle;
  
  if ((File->Attributes & FAT_ATTR_DIRECTORY) == FAT_ATTR_DIRECTORY) {
    return EFI_UNSUPPORTED;
  }
 
  *Position = (UINT64) File->CurrentPos;
  return EFI_SUCCESS;
}
 
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
  )
{
  PEI_FAT_FILE          *File;   
  PEI_FAT_PRIVATE_DATA  *PrivateData;   

  PrivateData       = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);   
  File              = Handle;

  *BlockDevNo       = File->Volume->BlockDeviceNo;
  *BlockDeviceCount = PrivateData->BlockDeviceCount;
  *LogicalBlock     = PrivateData->BlockDevice[File->Volume->BlockDeviceNo].Logical;
  *ParentDevNo      = PrivateData->BlockDevice[File->Volume->BlockDeviceNo].ParentDevNo;
  
}

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
  )
{
  PEI_FAT_PRIVATE_DATA  *PrivateData;   

  PrivateData       = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);   

  *LogicalBlock     = PrivateData->BlockDevice[*BlockDevNo].Logical;  
}

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
  )
{ 
  PEI_FAT_PRIVATE_DATA  *PrivateData;   

  PrivateData   = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);   
  CopyMem (BlockDevType, &PrivateData->BlockDevice[BlockDevNo].DevType , sizeof (EFI_PEI_BLOCK_DEVICE_TYPE));
  
}
