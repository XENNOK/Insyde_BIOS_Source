/** @file

Get and Print SystemFile or File Information   

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/
#include "FS_HW.h"

/**
 Get System Information.
  
  @param[in] Root        EFI_FILE_PROTOCOL.
                             
 
  @return Status Code
*/
EFI_STATUS
SysInfoGet (
  IN EFI_FILE_PROTOCOL *Root
  )
{
  EFI_STATUS                      Status;
  UINTN                           BufferSize;
  EFI_FILE_SYSTEM_INFO            *SysFileInfoBuf;
  
  BufferSize = 0;
  
  SysFileInfoBuf = AllocatePool(100);
  
  Status = Root->GetInfo (
                   Root,
                   &gEfiFileSystemInfoGuid,
                   &BufferSize,
                   SysFileInfoBuf
                   );
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (SysFileInfoBuf);
    SysFileInfoBuf = AllocatePool (BufferSize);
    if (SysFileInfoBuf == NULL) {
      Print (L"SysFileInfoBuf NULL\n");
      return Status;
    }
    Status = Root->GetInfo (
                     Root,
                     &gEfiFileSystemInfoGuid,
                     &BufferSize,
                     SysFileInfoBuf
                     );
  }
  
  Print (L"               File System Information              \n");
  Print (L"====================================================\n");  
  if (SysFileInfoBuf->ReadOnly == 0) {
    Print (L"ReadOnly     : FALSE\n");
  } else {
    Print (L"ReadOnly     : TRUE\n");
  }
  Print (
    L"Volume Label : %s\n",
    SysFileInfoBuf->VolumeLabel
    );
  Print (
    L"Volume Size  : %Ld\n",
    SysFileInfoBuf->VolumeSize
    );
  Print (
    L"Free Space   : %Ld\n",
    SysFileInfoBuf->FreeSpace
    );
  Print (
    L"Block Size   : %d\n",
    SysFileInfoBuf->BlockSize
    );
  Print (L"====================================================\n");
  
  FreePool (SysFileInfoBuf);
  return EFI_SUCCESS;
}

/**
 Get File Information.
  
  @param[in]       Root           EFI_FILE_PROTOCOL.
  @param[in]       FileName       File name.
  @param[in][out]  FileInfoBuf   EFI_FILE_INFO.
  @param[in][out]  File           EFI_FILE_PROTOCOL
                             
 
  @return Status Code
*/
EFI_STATUS
FileInfoGet (
  IN EFI_FILE_PROTOCOL     *Root,
  IN CHAR16                *FileName ,
  IN OUT EFI_FILE_INFO     **FileInfoBuf,
  IN OUT EFI_FILE_PROTOCOL **File 
  )
{

  EFI_STATUS                     Status;
  UINTN                          BufferSize;
  EFI_FILE_PROTOCOL              *SystemFile = 0;
  
  BufferSize = 0;
  Status = Root->Open (
                   Root,     
                   &SystemFile,
                   FileName, 
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   EFI_FILE_ARCHIVE
                   );
  if (EFI_ERROR (Status)) {
    Print (L"Open File Fail\n");
    return Status;
  }
  
  *File = SystemFile;
  *FileInfoBuf = AllocatePool (100);
  
  Status = SystemFile->GetInfo (
                         SystemFile,
                         &gEfiFileInfoGuid,
                         &BufferSize,
                         *FileInfoBuf
                         );
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (*FileInfoBuf);
    *FileInfoBuf = AllocatePool (BufferSize);
    
    if (*FileInfoBuf == NULL) {
      Print (L"FileInfoBuf NULL\n");
      return Status;
    }
  Status = SystemFile->GetInfo(
                         SystemFile,
                         &gEfiFileInfoGuid,
                         &BufferSize,
                         *FileInfoBuf
                         );
  }
  
  return EFI_SUCCESS;
}

/**
 Print File Info
  
  @param[in]       Root           EFI_FILE_PROTOCOL.
  @param[in]       FileName       File name.
                             
 
  @return Status Code
*/
VOID
PrintFileInfo (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_STATUS                     Status;
  EFI_FILE_INFO                  *FileInfoBuf;
  EFI_FILE_PROTOCOL              *SystemFile = 0;

  Status = FileInfoGet (
             Root,
             FileName ,
             &FileInfoBuf,
             &SystemFile
             );
  if (EFI_ERROR (Status)) {
     Print(L"Get_FileInfo Fail\n");
   }
  
  Print (
    L"Get [%s] Info Success\n",
    FileInfoBuf->FileName
    );
  Print (L"                  File  Information                 \n");
  Print (L"====================================================\n");
  Print (
    L"File Size         : %Ld\n",
    FileInfoBuf->FileSize
    );
  Print (
    L"Physical Size     : %Ld\n",
    FileInfoBuf->PhysicalSize
    );
  Print (
    L"Create Time       : %02d/%02d/%04d %02d:%02d\n",
    FileInfoBuf->CreateTime.Month,
    FileInfoBuf->CreateTime.Day,
    FileInfoBuf->CreateTime.Year,
    FileInfoBuf->CreateTime.Hour,
    FileInfoBuf->CreateTime.Minute
    );
  Print (
    L"LastAccess Time   : %02d/%02d/%04d %02d:%02d\n",
    FileInfoBuf->LastAccessTime.Month,
    FileInfoBuf->LastAccessTime.Day,
    FileInfoBuf->LastAccessTime.Year,
    FileInfoBuf->LastAccessTime.Hour,
    FileInfoBuf->LastAccessTime.Minute
    );
  Print (
    L"Modification Time : %02d/%02d/%04d %02d:%02d\n",
    FileInfoBuf->ModificationTime.Month,
    FileInfoBuf->ModificationTime.Day,
    FileInfoBuf->ModificationTime.Year,
    FileInfoBuf->ModificationTime.Hour,
    FileInfoBuf->ModificationTime.Minute
    );
  Print (
    L"Attribute         : 0x%02X\n",
    FileInfoBuf->Attribute
    );
  Print (L"====================================================\n");
  
  SystemFile->Close (SystemFile);
  FreePool (FileInfoBuf);
}
