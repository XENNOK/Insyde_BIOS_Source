/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include "FileHandleLib.h"
#include "FileSelectUILib.h"
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/UnicodeCollation.h>

#include <Guid/FileInfo.h>

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>

/**
  This function will retrieve the information about the file for the handle
  specified and store it in allocated pool memory.
	
  This function allocates a buffer to store the file's information. It is the
  caller's responsibility to free the buffer
	
  @param  FileHandle  The file handle of the file for which information is
  being requested.
	
  @retval NULL information could not be retrieved.
	
  @return the information about the file
**/
EFI_FILE_INFO*
EFIAPI
FsuFileHandleGetInfo (
  IN EFI_FILE                                     *FileHandle
  )
{
  EFI_FILE_INFO   *FileInfo;
  UINTN           FileInfoSize;
  EFI_STATUS      Status;
	
  if (FileHandle == NULL) {
    return (NULL);
  }
	
  //
  // Get the required size to allocate
  //
  FileInfoSize = 0;
  FileInfo = NULL;
  Status = FileHandle->GetInfo(FileHandle,
                               &gEfiFileInfoGuid,
                               &FileInfoSize,
                               NULL);
  if (Status == EFI_BUFFER_TOO_SMALL){
    //
    // error is expected.  getting size to allocate
    //
    FileInfo = AllocateZeroPool(FileInfoSize);
    //
    // now get the information
    //
    Status = FileHandle->GetInfo(FileHandle,
                                 &gEfiFileInfoGuid,
                                 &FileInfoSize,
                                 FileInfo);
    //
    // if we got an error free the memory and return NULL
    //
    if (EFI_ERROR(Status)) {
      FreePool(FileInfo);
      return NULL;
    }
  }
  return (FileInfo);
}
	
//	/**
//	  Function to get a full filename given a EFI_FILE_HANDLE somewhere lower on the
//	  directory 'stack'.
//	
//	  if Handle is NULL, return EFI_INVALID_PARAMETER
//	
//	  @param[in] Handle             Handle to the Directory or File to create path to.
//	  @param[out] FullFileName      pointer to pointer to generated full file name.  It
//	                                is the responsibility of the caller to free this memory
//	                                with a call to FreePool().
//	  @retval EFI_SUCCESS           the operation was sucessful and the FullFileName is valid.
//	  @retval EFI_INVALID_PARAMETER Handle was NULL.
//	  @retval EFI_INVALID_PARAMETER FullFileName was NULL.
//	  @retval EFI_OUT_OF_RESOURCES  a memory allocation failed.
//	**/
//	EFI_STATUS
//	EFIAPI
//	FileHandleGetFileName (
//	  IN CONST EFI_FILE_HANDLE      Handle,
//	  OUT CHAR16                    **FullFileName
//	  )
//	{
//	  EFI_STATUS      Status;
//	  UINTN           Size;
//	  EFI_FILE_HANDLE CurrentHandle;
//	  EFI_FILE_HANDLE NextHigherHandle;
//	  EFI_FILE_INFO   *FileInfo;
//	
//	  Size = 0;
//	
//	  //
//	  // Check our parameters
//	  //
//	  if (FullFileName == NULL || Handle == NULL) {
//	    return (EFI_INVALID_PARAMETER);
//	  }
//	
//	  *FullFileName = NULL;
//	  CurrentHandle = NULL;
//	
//	  Status = Handle->Open(Handle, &CurrentHandle, L".", EFI_FILE_MODE_READ, 0);
//	  if (!EFI_ERROR(Status)) {
//	    //
//	    // Reverse out the current directory on the device
//	    //
//	    for (;;) {
//	      FileInfo = FileHandleGetInfo(CurrentHandle);
//	      if (FileInfo == NULL) {
//	        Status = EFI_OUT_OF_RESOURCES;
//	        break;
//	      } else {
//	        //
//	        // We got info... do we have a name? if yes preceed the current path with it...
//	        //
//	        if (StrLen (FileInfo->FileName) == 0) {
//	          if (*FullFileName == NULL) {
//	            ASSERT((*FullFileName == NULL && Size == 0) || (*FullFileName != NULL));
//	            *FullFileName = StrnCatGrowLeft(FullFileName, &Size, L"\\", 0);
//	          }
//	          FreePool(FileInfo);
//	          break;
//	        } else {
//	          if (*FullFileName == NULL) {
//	            ASSERT((*FullFileName == NULL && Size == 0) || (*FullFileName != NULL));
//	            *FullFileName = StrnCatGrowLeft(FullFileName, &Size, L"\\", 0);
//	          }
//	          ASSERT((*FullFileName == NULL && Size == 0) || (*FullFileName != NULL));
//	          *FullFileName = StrnCatGrowLeft(FullFileName, &Size, FileInfo->FileName, 0);
//	          *FullFileName = StrnCatGrowLeft(FullFileName, &Size, L"\\", 0);
//	          FreePool(FileInfo);
//	        }
//	      }
//	      //
//	      // Move to the parent directory
//	      //
//	      Status = CurrentHandle->Open (CurrentHandle, &NextHigherHandle, L"..", EFI_FILE_MODE_READ, 0);
//	      if (EFI_ERROR (Status)) {
//	        break;
//	      }
//	
//	      FileHandleClose(CurrentHandle);
//	      CurrentHandle = NextHigherHandle;
//	    }
//	  } else if (Status == EFI_NOT_FOUND) {
//	    Status = EFI_SUCCESS;
//	    ASSERT((*FullFileName == NULL && Size == 0) || (*FullFileName != NULL));
//	    *FullFileName = StrnCatGrowLeft(FullFileName, &Size, L"\\", 0);
//	  }
//	
//	  if (CurrentHandle != NULL) {
//	    CurrentHandle->Close (CurrentHandle);
//	  }
//	
//	  if (EFI_ERROR(Status) && *FullFileName != NULL) {
//	    FreePool(*FullFileName);
//	  }
//	
//	  return (Status);
//	}
	
/**
  function to determine if a given handle is a directory handle
	
  if DirHandle is NULL then ASSERT()
	
  open the file information on the DirHandle and verify that the Attribute
  includes EFI_FILE_DIRECTORY bit set.
	
  @param DirHandle              Handle to open file
	
  @retval EFI_SUCCESS           DirHandle is a directory
  @retval EFI_INVALID_PARAMETER DirHandle did not have EFI_FILE_INFO available
  @retval EFI_NOT_FOUND         DirHandle is not a directory
**/
EFI_STATUS
EFIAPI
FileHandleIsDirectory (
  IN EFI_FILE                                     *DirHandle
  )
{
  EFI_FILE_INFO *DirInfo;
	
  //
  // ASSERT if DirHandle is NULL
  //
  ASSERT(DirHandle != NULL);
	
  //
  // get the file information for DirHandle
  //
  DirInfo = FsuFileHandleGetInfo (DirHandle);
	
  //
  // Parse DirInfo
  //
  if (DirInfo == NULL) {
    //
    // We got nothing...
    //
    return (EFI_INVALID_PARAMETER);
  }
  if ((DirInfo->Attribute & EFI_FILE_DIRECTORY) == 0) {
    //
    // Attributes say this is not a directory
    //
    FreePool (DirInfo);
    return (EFI_NOT_FOUND);
  }
  //
  // all good...
  //
  FreePool (DirInfo);
  return (EFI_SUCCESS);
}
	
/** Retrieve first entry from a directory.
	
  This function takes an open directory handle and gets information from the
  first entry in the directory.  A buffer is allocated to contain
  the information and a pointer to the buffer is returned in *Buffer.  The
  caller can use FileHandleFindNextFile() to get subsequent directory entries.
	
  The buffer will be freed by FileHandleFindNextFile() when the last directory
  entry is read.  Otherwise, the caller must free the buffer, using FreePool,
  when finished with it.
	
  @param[in]  DirHandle         The file handle of the directory to search.
  @param[out] Buffer            The pointer to pointer to buffer for file's information.
	
  @retval EFI_SUCCESS           Found the first file.
  @retval EFI_NOT_FOUND         Cannot find the directory.
  @retval EFI_NO_MEDIA          The device has no media.
  @retval EFI_DEVICE_ERROR      The device reported an error.
  @retval EFI_VOLUME_CORRUPTED  The file system structures are corrupted.
  @return Others                status of FileHandleGetInfo, FileHandleSetPosition,
                                or FileHandleRead
**/
EFI_STATUS
EFIAPI
FsuFileHandleFindFirstFile (
  IN  EFI_FILE                  *DirHandle,
  OUT EFI_FILE_INFO             **Buffer,
  OUT BOOLEAN                   *NoFile
  )
{
  EFI_STATUS    Status;
  UINTN         BufferSize;
	
  if (Buffer == NULL || DirHandle == NULL) {
    return (EFI_INVALID_PARAMETER);
  }
	
  //
  // verify that DirHandle is a directory
  //
  Status = FileHandleIsDirectory(DirHandle);
  if (EFI_ERROR(Status)) {
    return (Status);
  }
	
  //
  // Allocate a buffer sized to struct size + enough for the string at the end
  //
  BufferSize = FIND_XXXX_FILE_BUFFER_SIZE;
  *Buffer = AllocateZeroPool(BufferSize);
  if (*Buffer == NULL){
    return (EFI_OUT_OF_RESOURCES);
  }
	
  //
  // reset to the begining of the directory
  //
  Status = DirHandle->SetPosition(DirHandle, 0);
  if (EFI_ERROR(Status)) {
    FreePool(*Buffer);
    *Buffer = NULL;
    return (Status);
  }
	
  //
  // read in the info about the first file
  // BUT First file alway display error message, so skip it.
  //
  Status = DirHandle->Read(DirHandle, &BufferSize, *Buffer);
  ASSERT(Status != EFI_BUFFER_TOO_SMALL);
  if (EFI_ERROR(Status) || BufferSize == 0) {
    FreePool(*Buffer);
    *Buffer = NULL;
    if (BufferSize == 0) {
      return (EFI_NOT_FOUND);
    }
    return (Status);
  }

//	  //
//	  // read in the info about the next file
//	  //
//	  Status = DirHandle->Read(DirHandle, &BufferSize, Buffer);
//	  ASSERT(Status != EFI_BUFFER_TOO_SMALL);
//	  if (EFI_ERROR(Status)) {
//	    return (Status);
//	  }
//		
//	  //
//	  // If we read 0 bytes (but did not have erros) we already read in the last file.
//	  //
//	  if (BufferSize == 0) {
//	    FreePool(Buffer);
//	    *NoFile = TRUE;
//	  }
  return (EFI_SUCCESS);
}
	
/** Retrieve next entries from a directory.
	
  To use this function, the caller must first call the FileHandleFindFirstFile()
  function to get the first directory entry.  Subsequent directory entries are
  retrieved by using the FileHandleFindNextFile() function.  This function can
  be called several times to get each entry from the directory.  If the call of
  FileHandleFindNextFile() retrieved the last directory entry, the next call of
  this function will set *NoFile to TRUE and free the buffer.
	
  @param[in]  DirHandle         The file handle of the directory.
  @param[out] Buffer            The pointer to buffer for file's information.
  @param[out] NoFile            The pointer to boolean when last file is found.
	
  @retval EFI_SUCCESS           Found the next file, or reached last file
  @retval EFI_NO_MEDIA          The device has no media.
  @retval EFI_DEVICE_ERROR      The device reported an error.
  @retval EFI_VOLUME_CORRUPTED  The file system structures are corrupted.
**/
EFI_STATUS
EFIAPI
FsuFileHandleFindNextFile(
  IN EFI_FILE                *DirHandle,
  OUT EFI_FILE_INFO          *Buffer,
  OUT BOOLEAN                *NoFile
  )
{
  EFI_STATUS    Status;
  UINTN         BufferSize;
	
  //
  // ASSERTs for DirHandle or Buffer or NoFile poitners being NULL
  //
  ASSERT (DirHandle != NULL);
  ASSERT (Buffer    != NULL);
  ASSERT (NoFile    != NULL);
	
  //
  // This BufferSize MUST stay equal to the originally allocated one in GetFirstFile
  //
  BufferSize = FIND_XXXX_FILE_BUFFER_SIZE;
	
  //
  // read in the info about the next file
  //
  Status = DirHandle->Read(DirHandle, &BufferSize, Buffer);
  ASSERT(Status != EFI_BUFFER_TOO_SMALL);
  if (EFI_ERROR(Status)) {
    return (Status);
  }
	
  //
  // If we read 0 bytes (but did not have erros) we already read in the last file.
  //
  if (BufferSize == 0) {
    FreePool(Buffer);
    *NoFile = TRUE;
  }
	
  return (EFI_SUCCESS);
}
	

EFI_STATUS
FsuGetParentHandle (
  EFI_HANDLE      ChildHandle,
  EFI_HANDLE      *ParentHandle
  )
{
  EFI_STATUS                    Status = EFI_NOT_FOUND;
  EFI_HANDLE                    Handle;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TmpDevPath;
  EFI_DEVICE_PATH_PROTOCOL      *OrgDevPath;

  Status = gBS->HandleProtocol (ChildHandle, &gEfiDevicePathProtocolGuid, &DevicePath);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  OrgDevPath = DuplicateDevicePath (DevicePath);
//[-start-140625-IB05080432-add]//
  if (OrgDevPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//
  TmpDevPath = OrgDevPath;

  TruncateLastNode (TmpDevPath);
  gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &TmpDevPath, &Handle);

  if (IsDevicePathEnd(TmpDevPath)) {
    *ParentHandle = Handle;
    Status = EFI_SUCCESS;
  }

  gBS->FreePool (TmpDevPath);

  return Status;
}

EFI_STATUS
FsuGetFileHandleByHandle (
  EFI_HANDLE                                      Handle,
  EFI_FILE                                        **FileHandle
  )
{
  EFI_STATUS                                      Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL                 *FileSystemProtocol;

  
  Status = gBS->HandleProtocol (Handle, &gEfiSimpleFileSystemProtocolGuid, &FileSystemProtocol);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  //
  //Opens the root directory on a volume
  //
  Status = FileSystemProtocol->OpenVolume (
                                 FileSystemProtocol, 
                                 FileHandle
                                 );  
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}