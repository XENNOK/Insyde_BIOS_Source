/** @file
  FileSystem_14 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FileSystem_14.h"

CONST CHAR16 mCmd[9][3] = {L"-h", L"-n", L"-r", L"-w",        \
                           L"-c", L"-d", L"-i", L"-m", L"-I"};

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_LOADED_IMAGE_PROTOCOL         *Image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *SimpleFileSystem;
  EFI_FILE                          *Root;
  BOOLEAN                           OpenSuccess;

  CHAR16                            **Argv;
  UINT64                            Argc;
  EFI_SHELL_PARAMETERS_PROTOCOL     *EfiShellParametersProtocol;
  EFI_SHELL_INTERFACE               *EfiShellInterfaceProtocol;

  Image = NULL;
  SimpleFileSystem = NULL;
  Root = NULL;
  OpenSuccess = FALSE;

  Argv = (CHAR16**)0;
  Argc = 0;
  EfiShellParametersProtocol = NULL;
  EfiShellInterfaceProtocol = NULL;

  gST->ConOut->ClearScreen(gST->ConOut);
 
  //
  // Locate Correct SimpleFileSystemProtocol.
  //
  // 1.Locate LoadedImage By main function's ImageHandle.
  // 2.Locate SimpleFileSystemProtocol By LoadedImage's DeviceHandle.
  //
  Status = gBS->HandleProtocol(
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  &Image
                  );
  if (Status == EFI_SUCCESS) {
    Status = gBS->HandleProtocol(
                    Image->DeviceHandle,
                    &gEfiSimpleFileSystemProtocolGuid,
                    &SimpleFileSystem
                    );
    if (Status == EFI_SUCCESS) {
      Status = SimpleFileSystem->OpenVolume(
                           SimpleFileSystem,
                           &Root
                           );
      if (Status == EFI_SUCCESS) {        
        OpenSuccess = TRUE;
      }
    }
  }


  //
  // Get Argv, Argc.
  //
  Status = gBS->OpenProtocol(
                  ImageHandle,
                  &gEfiShellParametersProtocolGuid,
                  (VOID **)&EfiShellParametersProtocol,
                  ImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    Status = gBS->OpenProtocol(
                    ImageHandle,
                    &gEfiShellInterfaceGuid,
                    (VOID **)&EfiShellInterfaceProtocol,
                    ImageHandle,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Argc = EfiShellInterfaceProtocol->Argc;
    Argv = EfiShellInterfaceProtocol->Argv;

  } else {
    Argc = EfiShellParametersProtocol->Argc;
    Argv = EfiShellParametersProtocol->Argv;
  }

  //
  // To judge command.
  //
  if (OpenSuccess == TRUE) {
    if (Argc == 1) {
      Print(L"Command invalid !!!\n");

    } else {
      if (StrCmp(Argv[1], mCmd[0]) == STRING_MATCH) {
        
        //
        // Help
        //
        if (Argc == 2) {
          ShowHelp();
        } else {
          Print(L"Command invalid !!!\n");
        }

      } else if (StrCmp(Argv[1], mCmd[1]) == STRING_MATCH) {

        //
        // Create
        //
        if (Argc == 3) {
          CreatFile(Root, Argv[2]);
        } else {
          Print(L"Command invalid !!!\n");
        }
        
      } else if (StrCmp(Argv[1], mCmd[2]) == STRING_MATCH) {

        //
        // Read
        //
        if (Argc == 3) {
          ReadFile(Root, Argv[2]);
        } else {
          Print(L"Command invalid !!!\n");
        }
        
      } else if (StrCmp(Argv[1], mCmd[3]) == STRING_MATCH) {

        //
        // Write
        //
        if (Argc == 3) {
          WriteFile(Root, Argv[2]);
        } else {
          Print(L"Command invalid !!!\n");
        }
        
      } else if (StrCmp(Argv[1], mCmd[4]) == STRING_MATCH) {

        //
        // Copy
        //
        if (Argc == 4) {
          CopyFile(Root, Argv[2], Argv[3]);
        } else {
          Print(L"Command invalid !!!\n");
        }
        
      } else if (StrCmp(Argv[1], mCmd[5]) == STRING_MATCH) {

        //
        // Delete
        //
        if (Argc == 3) {
          DeleteFile(Root, Argv[2]);
        } else {
          Print(L"Command invalid !!!\n");
        }
      } else if (StrCmp(Argv[1], mCmd[6]) == STRING_MATCH) {

        //
        // Get file information
        //
        if (Argc == 3) {
          ShowFileInfo(GetFileInfo(Root, Argv[2]));
        } else {
          Print(L"Command invalid !!!\n");
        }
      } else if (StrCmp(Argv[1], mCmd[7]) == STRING_MATCH) {

        //
        // Merge
        //
        if (Argc == 4) {
          MergeFile(Root, Argv[2], Argv[3]);
        } else {
          Print(L"Command invalid !!!\n");
        }
      } else if (StrCmp(Argv[1], mCmd[8]) == STRING_MATCH) {

        //
        // Get system information
        //
        if (Argc == 2) {
          ShowSystemInfo(GetSystemInfo(Root));
        } else {
          Print(L"Command invalid !!!\n");
        }
      }
    }
  }
  
  Status = EFI_SUCCESS;

  return Status;
}

/**
  This function show help page.

**/
VOID
ShowHelp(
  VOID
  ) 
{
  Print(L"h Show    Help       -h\n");
  Print(L"n Creat   File       -n <FileName>\n");
  Print(L"r Read    File       -r <FileName>\n");
  Print(L"w Write   File       -w <FileName>\n");
  Print(L"d Delete  File       -d <FileName>\n");
  Print(L"c copy    File       -c <FileName> <FileName>\n");
  Print(L"m Merge   File       -m <FileName> <FileName>\n");
  Print(L"i File Information   -i <FileName>\n");
  Print(L"I System Information -I\n");

  return;
}

/**
  To create file.
  If file is existent, new file will not be created and we will be informed.

  @param   *Root         The root directory of file system.
  param   *FileNmae      The file name we want to create.
  
  @retval  EFI_STATUS    To check if successful or not.

**/
EFI_STATUS
CreatFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  )
{
  EFI_STATUS  Status;
  EFI_FILE    *FileHandle;

  FileHandle = NULL;

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   FileName,
                   EFI_FILE_MODE_READ,
                   EFI_FILE_ARCHIVE
                   );

  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r.\n", Status);
    Print(L"Create %s file.\n", FileName);

    Status = Root->Open(
                     Root,
                     &FileHandle,
                     FileName,
                     EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                     0
                     );
    if (EFI_ERROR(Status)) {
      Print(L"Open fail: %r.\n", Status);
      return Status;
    } 
    
    Status = Root->Close(FileHandle);
    if (EFI_ERROR(Status)) {
      Print(L"Close fail: %r.\n", Status);
    }
    return Status;

  } else {
    Print(L"File %s already existed.\n", FileName);
    Status = Root->Close(FileHandle);
    if (EFI_ERROR(Status)) {
      Print(L"Close fail: %r.\n", Status);
    }
    return Status;
  }
}

/**
  To delete file.
  If file is not existent, we will be informed.

  @param   *Root         The root directory of file system.
  @param   *FileNmae     The file name we want to create.
  
  @retval  EFI_STATUS    To check if successful or not.

**/
EFI_STATUS
DeleteFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  )
{
  EFI_STATUS  Status;
  EFI_FILE    *FileHandle;

  FileHandle = NULL;

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   FileName,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r.\n", Status);
    if (Status == EFI_NOT_FOUND) {
      Print(L"File %s not found !!!\n", FileName);
    }
    return Status;
  } 
  
  Status = FileHandle->Delete(FileHandle);
  if (EFI_ERROR(Status)) {
    Print(L"Delete fail: %r.\n", Status);
    return Status;

  } else {
    Print(L"File %s is deleted.\n", FileName);
    return Status;
  }
}

/**
  To get file information.
  If file is not existent, we will be informed.

  @param   *Root             The root directory of file system.
  @param   *FileNmae         The file name we want to create.
  
  @retval  EFI_FILE_INFO*    The pointer of file information structure.

**/
EFI_FILE_INFO*
GetFileInfo (
  EFI_FILE  *Root,
  CHAR16    *FileName
  )
{
  EFI_STATUS     Status;
  EFI_FILE       *FileHandle;
  EFI_FILE_INFO  *Buffer;
  UINT64         BufferSize;
  
  FileHandle = NULL;
  Buffer = NULL;
  BufferSize = 0;

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   FileName,
                   EFI_FILE_MODE_READ,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r\n", Status);
    return NULL;

  } else {
    Status = FileHandle->GetInfo(
                        FileHandle,
                        &gEfiFileInfoGuid,
                        &BufferSize,
                        Buffer
                        );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      gBS->AllocatePool(
                     EfiBootServicesData,
                     BufferSize,                        
                     &Buffer
                     );
      Status = FileHandle->GetInfo(
                                FileHandle,
                                &gEfiFileInfoGuid,
                                &BufferSize,
                                Buffer
                                );
      if (EFI_ERROR(Status)) {
        Print(L"Get information fail: %r.\n", Status);
        gBS->FreePool(Buffer);
        return NULL;

      } else {
        Status = Root->Close(FileHandle);
        if (EFI_ERROR(Status)) {
          Print(L"Close fail: %r.\n", Status);
          gBS->FreePool(Buffer);
          return NULL;

        } else {
          return Buffer;
        }
      }
    } else {
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"Close fail: %r.\n", Status);
        return NULL;

      } else {
        return Buffer;
      }
    }
  }
}

/**
  To show file information.

  @param  *FileInfo    The file name we want to create.

**/
VOID
ShowFileInfo (
  EFI_FILE_INFO *FileInfo
  )
{
  if (FileInfo != NULL) {
    Print (L"File Name         : %s.\n", FileInfo->FileName);
    Print (L"File Size         : %d Byte.\n", FileInfo->FileSize);
    Print (L"Physical Size     : %d Byte.\n", FileInfo->PhysicalSize);
    Print (L"Create Time       : %t.\n", FileInfo->CreateTime);
    Print (L"LastAccess Time   : %t.\n", FileInfo->LastAccessTime);
    Print (L"Modification Time : %t.\n", FileInfo->ModificationTime);
    Print (L"Attribute         : %d.\n", FileInfo->Attribute);
    // Free(FileInfo);
    return;

  } else {
    Print(L"Information not found !!!\n");
    return;
  }
}

/**
  To get system information.
  If get information fail, we will be informed.

  @param   *Root                    The root directory of file system.
  
  @retval  EFI_FILE_SYSTEM_INFO*    The pointer of system information structure.

**/
EFI_FILE_SYSTEM_INFO*
GetSystemInfo (
  EFI_FILE  *Root
  )
{
  EFI_STATUS            Status;
  EFI_FILE_SYSTEM_INFO  *Buffer;
  UINT64                BufferSize;
  EFI_FILE_SYSTEM_INFO  *Info;

  Buffer = NULL;
  BufferSize = 0;
  Info = NULL;

  Status = Root->GetInfo(
                      Root,
                      &gEfiFileSystemInfoGuid,
                      &BufferSize,
                      Buffer
                      );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    gBS->AllocatePool(
                   EfiBootServicesData,
                   BufferSize,                        
                   &Buffer
                   );
    Status = Root->GetInfo(
                        Root,
                        &gEfiFileSystemInfoGuid,
                        &BufferSize,
                        Buffer
                        );
    if (EFI_ERROR(Status)) {
      Print(L"Get system information fail: %r.\n", Status);
      gBS->FreePool(Buffer);
      return NULL;

    } else {
      Info = Buffer;
      gBS->FreePool(Buffer);
      return Buffer;
    }
  } else {
    Info = Buffer;
    gBS->FreePool(Buffer);
    return Buffer;
  }
}

/**
  To show System information.

  @param  *SystemInfo    The system we want to get information.

**/
VOID
ShowSystemInfo (
  EFI_FILE_SYSTEM_INFO *SystemInfo
  )
{
  if (SystemInfo != NULL) {
    Print (L"Read Only    : %s.\n", (SystemInfo->ReadOnly)? L"TRUE":L"FALSE");
    Print (L"Volume Size  : %ld Byte.\n", SystemInfo->VolumeSize);
    Print (L"Free Space   : %ld Byte.\n", SystemInfo->FreeSpace);
    Print (L"Block Size   : %d Byte.\n", SystemInfo->BlockSize);
    Print (L"Volume Label : %s.\n", SystemInfo->VolumeLabel);
    return;

  } else {
    Print(L"Information not found !!!\n");
    return;
  }
}

/**
  To read file.
  If file is not existent, we will be informed.

  @param   *Root         The root directory of file system.
  @param   *FileNmae     The file name we want to create.
  
  @retval  EFI_STATUS    To check if successful or not.

**/
EFI_STATUS
ReadFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  )
{
  EFI_STATUS      Status;
  EFI_FILE        *FileHandle;
  CHAR16          *Buffer;
  UINT64          BufferSize;

  FileHandle = NULL;
  Buffer = NULL;
  BufferSize = 0;

  if (GetFileInfo(Root, FileName) == NULL) {
    Print(L"File %s not found !!!\n", FileName);
    return EFI_NOT_FOUND;

  } else {
    BufferSize = GetFileInfo(Root, FileName)->FileSize;

    if (BufferSize == 0) {
      Print(L"%s has no content !!!\n", FileName);
      return EFI_SUCCESS;
    }
  }

  gBS->AllocatePool(
                 EfiBootServicesData,
                 BufferSize,                        
                 &Buffer
                 );

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   FileName,
                   EFI_FILE_MODE_READ,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r.\n", Status);
    return Status;

  } else {
    Status = FileHandle->Read(
                           FileHandle,
                           &BufferSize,
                           Buffer
                           );
    if (EFI_ERROR(Status)) {
      Print(L"Read fail: %r.\n", Status);
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"Close fail: %r.\n", Status);
      }
      FreePool(Buffer);
      return Status;

    } else {
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"Close fail: %r.\n", Status);
        FreePool(Buffer);
        return Status;

      } else {
        Print(L"File content:\n");
        gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print(L"%s\n\n", Buffer);
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
        FreePool(Buffer);
        return Status;
      }      
    }
  }
}

/**
  To write file.
  If file is not existent, we will be informed.

  @param   *Root         The root directory of file system.
  @param   *FileNmae     The file name we want to create.
  
  @retval  EFI_STATUS    To check if successful or not.

**/
EFI_STATUS
WriteFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  )
{
  EFI_STATUS      Status;
  EFI_FILE        *FileHandle;
  CHAR16          *Buffer;
  UINT64          BufferSize;

  FileHandle = NULL;
  Buffer = NULL;
  BufferSize = 0;

  //
  // To get write content and its length.
  //
  Buffer = InputWord();
  
  BufferSize = StrLen(Buffer) * 2 + 2;

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   FileName,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"\nOpen fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;

  } else {
    FileHandle->Write(
                   FileHandle,
                   &BufferSize,
                   Buffer
                   );
    if (EFI_ERROR(Status)) {
      Print(L"\nWrite fail: %r.\n", Status);
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"\nClose fail: %r.\n", Status);
      }
      FreePool(Buffer);
      return Status;
      
    } else {
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"\nClose fail: %r\n", Status);
        FreePool(Buffer);
        return Status;

      } else {
        Print(L"\nWrite done.\n");        
        FreePool(Buffer);
        return Status;
      }
    }
  }
}

/**
  To initialize array.

  @param   *Array       The array we want to initialize.
  @param   ArraySize    The array size.

**/
VOID 
InitializeArray(
  CHAR16 *Array,
  UINT64 ArraySize
  )
{
  UINT32  i;  // For loop count.

  for (i = 0; i < ArraySize; i++) {
    Array[i] = 0;
  }

  return;
}

/**
  To input word, press Enter to confirm.
  
  @retval  CHAR16*    The return string.

**/
CHAR16* 
InputWord (
  VOID
  )
{
  EFI_INPUT_KEY    InputKey;
  INT32            Count;           // The amount of we input.
  UINT32           AllocateSize;
  UINT32           i;               // For loop count.
  CHAR16           *Buffer;
  CHAR16           *OldBuffer;
  CHAR16           *Word;

  Count = 0;
  AllocateSize = INITIAL_SIZE;
  Buffer = NULL;
  OldBuffer = NULL;
  Word = NULL;

  Buffer = AllocateZeroPool(AllocateSize);

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (Count >= (INT32)AllocateSize / 2) {
      OldBuffer = Buffer;
      Buffer = ReallocatePool(AllocateSize, AllocateSize + SIZE_INCREMENT, NULL);
      AllocateSize += SIZE_INCREMENT;
      InitializeArray(Buffer, AllocateSize);
      for (i = 0; i < (AllocateSize - SIZE_INCREMENT) / 2; i++) {
        Buffer[i] = OldBuffer[i];
      }
    }

    switch(InputKey.UnicodeChar){
    case CHAR_NULL:
      continue;
    
    case CHAR_CARRIAGE_RETURN:
      Word = Buffer;
      FreePool(OldBuffer);
      FreePool(OldBuffer);
      return Word;
    
    case CHAR_BACKSPACE:
      Count--;
      if (Count < 0) {
        Count = 0;
      }
      Buffer[Count] = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar); 
      
      continue;

    default:
      if (InputKey.UnicodeChar != CHAR_CARRIAGE_RETURN) {
        Buffer[Count] = InputKey.UnicodeChar;
        Print(L"%c", InputKey.UnicodeChar);
        Count++;
        continue;
      }
    }
  }
}

/**
  To copy file from CopyFile to PasteFile.
  If PasteFile have content, it will be overridden.
  If CopyFile is not existent, we will be informed.
  If PasteFile is not existent, it will be created.

  @param   *Root         The root directory of file system.
  @param   *CopyFile     The file we want to copy its content.
  @param   *PasteFile    The file we want to paste CopyFile's content.
  
  @retval  EFI_STATUS    To check if successful or not.

**/
EFI_STATUS
CopyFile (
  EFI_FILE  *Root,
  CHAR16    *CopyFile,
  CHAR16    *PasteFile
  )
{
  EFI_STATUS  Status;
  EFI_FILE    *FileHandle;
  CHAR16      *Buffer;
  UINT64      BufferSize;

  FileHandle = NULL;
  Buffer = NULL;
  BufferSize = 0;

  //
  // Copy content from CopyFile.
  //
  if (GetFileInfo(Root, CopyFile) == NULL) {
    Print(L"File %s not found !!!\n", CopyFile);
    return EFI_NOT_FOUND;

  } else {
    BufferSize = GetFileInfo(Root, CopyFile)->FileSize;

    if (BufferSize == 0) {

      //
      // If no content is in the file, it will do nothing.
      //
      Print(L"%s has no content !!!\n", CopyFile);
      return EFI_SUCCESS;

    } else {
      Buffer = AllocatePool(BufferSize);
    }
  }

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   CopyFile,
                   EFI_FILE_MODE_READ,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;

  } else {
    Status = FileHandle->Read(
                           FileHandle,
                           &BufferSize,
                           Buffer
                           );
    if (EFI_ERROR(Status)) {
      Print(L"Read fail: %r.\n", Status);
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"Close fail: %r.\n", Status);
      }
      FreePool(Buffer);
      return Status;

    } else {
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"Close fail: %r.\n", Status);
        FreePool(Buffer);
        return Status;
      }
    }
  }

  //
  // Patse file to PatseFile.
  //
  Status = Root->Open(
                   Root,
                   &FileHandle,
                   PasteFile,
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"\nOpen fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;

  } else {
    FileHandle->Write(
                   FileHandle,
                   &BufferSize,
                   Buffer
                   );
    if (EFI_ERROR(Status)) {
      Print(L"\nWrite fail: %r.\n", Status);
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"\nClose fail: %r.\n", Status);
      }
      FreePool(Buffer);
      return Status;
      
    } else {
      Status = Root->Close(FileHandle);
      if (EFI_ERROR(Status)) {
        Print(L"\nClose fail: %r.\n", Status);
        FreePool(Buffer);
        return Status;

      } else {
        Print(L"Copy %s to %s.\n", CopyFile, PasteFile);      
        FreePool(Buffer);
        return Status;
      }
    }
  }
}

/**
  To merge file from SourceFile to DestinationFile.
  If DestinationFile is not existent, we will be informed.
  If SourceFile is not existent, we will be informed.

  @param   *Root               The root directory of file system.
  @param   *SourceFile         The file we want to copy its content.
  @param   *DestinationFile    The file we want to paste SourceFile's content to its end.
  
  @retval  EFI_STATUS    To check if successful or not.

**/
EFI_STATUS
MergeFile (
  EFI_FILE  *Root,
  CHAR16    *SourceFile,
  CHAR16    *DestinationFile
  )
{
  EFI_STATUS  Status;
  EFI_FILE    *FileHandle;
  CHAR16      *Buffer;
  UINT64      BufferSize;
  UINT64      Position;

  FileHandle = NULL;
  Buffer = NULL;
  BufferSize = 0;
  Position = 0;

  //
  // Copy content from SourceFile.
  //
  if (GetFileInfo(Root, SourceFile) == NULL) {
    Print(L"File %s not found !!!\n", SourceFile);
    return EFI_NOT_FOUND;

  } else {
    BufferSize = GetFileInfo(Root, SourceFile)->FileSize;

    if (BufferSize == 0) {

      //
      // If no content is in the file, it will do nothing.
      //
      Print(L"%s has no content !!!\n", SourceFile);
      return EFI_SUCCESS;

    } else {
      Buffer = AllocatePool(BufferSize);
    }
  }

  Status = Root->Open(
                   Root,
                   &FileHandle,
                   SourceFile,
                   EFI_FILE_MODE_READ,
                   0
                   );
  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;
  }

  Status = FileHandle->Read(
                         FileHandle,
                         &BufferSize,
                         Buffer
                         );
  if (EFI_ERROR(Status)) {
    Print(L"Read fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;
  }

  Status = Root->Close(FileHandle);
  if (EFI_ERROR(Status)) {
    Print(L"Close fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;
  }

  //
  // Set PasteFile position.
  //
  if (GetFileInfo(Root, DestinationFile) == NULL) {
    Print(L"File %s not found !!!\n", DestinationFile);
    return EFI_NOT_FOUND;

  } else {
    Position = GetFileInfo(Root, DestinationFile)->FileSize;
  }

  //
  // Patse file to DestinationFile's end.
  //
  Status = Root->Open(
                  Root,
                  &FileHandle,
                  DestinationFile,
                  EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                  0
                  );
  if (EFI_ERROR(Status)) {
    Print(L"Open fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;
  }

  Status = FileHandle->SetPosition(FileHandle, Position);
  if (EFI_ERROR(Status)) {
    Print(L"Set position fail: %r.\n", Status);
    Status = Root->Close(FileHandle);
    if (EFI_ERROR(Status)) {
      Print(L"Close fail: %r.\n", Status);
    }
    FreePool(Buffer);
    return Status;
  }

  Status = FileHandle->Write(
                          FileHandle,
                          &BufferSize,
                          Buffer
                          );
  if (EFI_ERROR(Status)) {
    Print(L"Write fail: %r.\n", Status);
    Status = Root->Close(FileHandle);
    if (EFI_ERROR(Status)) {
      Print(L"Close fail: %r.\n", Status);
    }
    FreePool(Buffer);
    return Status;
  }

  Status = Root->Close(FileHandle);
  if (EFI_ERROR(Status)) {
    Print(L"Close fail: %r.\n", Status);
    FreePool(Buffer);
    return Status;
  }

  Print(L"Merge %s to %s.\n", SourceFile, DestinationFile);
  FreePool(Buffer);
  return Status;
}