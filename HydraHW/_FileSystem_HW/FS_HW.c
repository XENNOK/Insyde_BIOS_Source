/** @file

  File System Main Func Code.

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

#define SHELL_INTERFACE_PROTOCOL \
  { \
    0x47c7b223, 0xc42a, 0x11d2, 0x8e, 0x57, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b \
  }
EFI_GUID mShellInterfaceProtocol = SHELL_INTERFACE_PROTOCOL;

typedef struct _EFI_SHELL_ARG_INFO {
  UINT32  Attributes;
} EFI_SHELL_ARG_INFO;

typedef struct _EFI_SHELL_INTERFACE {
  //
  // Handle back to original image handle & image info
  //
  EFI_HANDLE                ImageHandle;
  VOID                      *Info;

  //
  // Parsed arg list
  //
  CHAR16                    **Argv;
  UINTN                     Argc;

  //
  // Storage for file redirection args after parsing
  //
  CHAR16                    **RedirArgv;
  UINTN                     RedirArgc;

  //
  // A file style handle for console io
  //
  EFI_FILE_HANDLE           StdIn;
  EFI_FILE_HANDLE           StdOut;
  EFI_FILE_HANDLE           StdErr;
  EFI_SHELL_ARG_INFO        *ArgInfo;
  BOOLEAN                   EchoOn;
} EFI_SHELL_INTERFACE;

/**
  Get File IO.

  @param[in]Root       EFI_FILE_PROTOCOL   
  @param[in]TheHandle  EFI_HANDLE.

  @return EFI_STATUS 
*/
EFI_STATUS 
GetFileIo (
  IN EFI_FILE_PROTOCOL  **Root,
  IN EFI_HANDLE TheHandle
  )
{
	EFI_STATUS  Status = 0;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;

  Status = gBS->OpenProtocol (
                  TheHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID**)&LoadedImage,
                  TheHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                 );
  if (EFI_ERROR (Status)) {
    Print(L"EFI_OPEN_PROTOCOL_GET_PROTOCOL Fail\n");
    return Status;
  }
  
  Status = gBS->HandleProtocol (
                  LoadedImage->DeviceHandle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID**)&SimpleFileSystem
                  );
  if (EFI_ERROR(Status)) {
    Print(L"EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Not Found\n");
    return Status;
  }
  Status = SimpleFileSystem->OpenVolume (
                               SimpleFileSystem,
                               Root
                               );

  return Status;
}

/**
  Print Help Information.
  
*/
VOID
HelpSection (
  VOID
  )
{
  Print (L"\n");
  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"                         File System HELP Information                      \n");
  Print (L"===========================================================================\n");
  Print (L"Instruction [ -n | -d | -r | -m | -c | -w ] [-i | -I | -h ][File1] [File2] \n");
  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-n | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Create [File2].                                                       \n");
  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-d | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Delete [File2].                                                       \n");
  
  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-r | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Read [File2] Data.                                                    \n");

  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-m | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Merge [File1] and [File2] to [File1].                                 \n");

  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-c | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Copy [File1] to cover [File2].                                        \n");

  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-w | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Create new [File1] and write data into it.                            \n");

  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-i | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Show [File2] information.                                             \n");

  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-I | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Show File System information.                                         \n");

  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"-h | ");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
  Print (L"Help Menu.                                                            \n");
  
  SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print (L"===========================================================================\n");
  SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
}

/**
  This function ONLY use for check file is exist or not.

  @param[in]Root      EFI_FILE_PROTOCOL   
  @param[in]FileName  File name.

  @return boolean    exist or not.
*/
BOOLEAN
CheckFileExist (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_STATUS        Status;
  EFI_FILE_PROTOCOL *SystemFile = 0;
  Status = Root->Open (
                   Root,     
                   &SystemFile,
                   FileName, 
                   EFI_FILE_MODE_READ,
                   EFI_FILE_ARCHIVE
                   );
   if (Status == EFI_NOT_FOUND) {
     return FALSE;
   } else if (EFI_ERROR (Status)) {
     return FALSE;
   } else {
     Root->Close (SystemFile);
     return TRUE;
   }
}

/**
  File Syatem Entry Point.

  @param[in] ImageHnadle        While the driver image loaded be the ImageLoader(), an image handle is assigned to this
                                driver binary, all activities of the driver is tied to this ImageHandle.
  @param[in] SystemTable        A pointer to the system table, for all BS(Boot Services) and RT(Runtime Services).
 
  @return Status Code

**/
EFI_STATUS
HydraMain (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                             Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  EFI_FILE_PROTOCOL                      *Root = 0;
  EFI_SHELL_PARAMETERS_PROTOCOL          *EfiShellParametersProtocol;
  EFI_SHELL_INTERFACE                    *EfiShellInterfaceProtocol;
  CHAR16                                 **Argv;
  UINTN                                  Argc;
  UINTN                                  ArgIndex;
  UINTN                                  MaxArgvValue;
  BOOLEAN                                CheckExist;

  CheckExist = FALSE;

  //
  // Locate Simple Text Input Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );

  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  //
  // grep Argc and Argv
  //
  Status = gBS->OpenProtocol (
                  ImageHandle,
                  &gEfiShellParametersProtocolGuid,
                  (VOID **) &EfiShellParametersProtocol,
                  ImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (Status != EFI_SUCCESS) {
    Status = gBS->OpenProtocol (
                    ImageHandle,
                    &mShellInterfaceProtocol,
                    (VOID **) &EfiShellInterfaceProtocol,
                    ImageHandle,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (Status != EFI_SUCCESS) {
      return Status;
    }
    Argc = EfiShellInterfaceProtocol->Argc;
    Argv = EfiShellInterfaceProtocol->Argv;
  } else {
    Argc = EfiShellParametersProtocol->Argc;
    Argv = EfiShellParametersProtocol->Argv;
  }
  
  MaxArgvValue = Argc - 1;
  
  if ((Argc == 1) || 
      (Argv[1][0] != '-')) {
    Print (L"Error parameter!! \n -h to viewing parameter Usage\n");
    return	EFI_SUCCESS;
  } else if ((Argc >= 4) &&
             (Argv[MaxArgvValue - 2][0] != '-')) {
    Print(L"Error parameter!! \n -h to viewing parameter Usage\n");
    return	EFI_SUCCESS;
  }
  
  GetFileIo(
    &Root,
    ImageHandle
    );
  
  for (ArgIndex = 1; ArgIndex < Argc; ArgIndex++) {

    if (Argv[ArgIndex][0] == '-') {
      if (Argv[ArgIndex][1] == 'n') {
        //
        // New File
        //
      	if (Argv[MaxArgvValue][0] == '-') {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          break;
      	}
        Status = NewFiles (
                   Root,
                   Argv[MaxArgvValue]
                   );
        if (Status == EFI_ACCESS_DENIED) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"File Already Exist / Accedd Denied");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          return Status;
        }
        
      } else if ((Argv[ArgIndex][1] == 'd') ||
                 (Argv[ArgIndex][1] == 'D')) {
        //
        // Delete File
        //
        if (Argv[MaxArgvValue][0] == '-') {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          break;
      	}
        Status = DeleteFiles (
                   Root,
                   Argv[MaxArgvValue]
                   );
        if (EFI_ERROR (Status)) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"DEL_Files Fail\n");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
        } else {
          Print (L"Delete Files\n");
        }
        
      } else if (Argv[ArgIndex][1] == 'r') {
        //
        // Read File
        //
        if (Argv[MaxArgvValue][0] == '-') {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          break;
      	}
        Status = ReadFiles (
                   Root,
                   Argv[MaxArgvValue]
                   );
        if (Status == EFI_ACCESS_DENIED) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"File NOT Exist / Accedd Denied\n");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          return Status;
        }
      } else if (Argv[ArgIndex][1] == 'm') {
        //
        // Merge File
        //
        if ((Argv[MaxArgvValue][0] == '-') ||
            (Argv[MaxArgvValue - 1][0] == '-')) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          break;
      	}
        Status = MergeFiles (
                   Root,
                   Argv[MaxArgvValue - 1],
                   Argv[MaxArgvValue]
                   );
        if (EFI_ERROR (Status)) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Merge Fail %r\n",Status);
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          return Status;
        }
        
      } else if (Argv[ArgIndex][1] == 'c') {
        //
        // Copy File
        //
        if ((Argv[MaxArgvValue][0] == '-') ||
            (Argv[MaxArgvValue - 1][0] == '-')) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          break;
      	}
        Status = CopyFiles ( 
                   Root,
                   Argv[MaxArgvValue - 1],
                   Argv[MaxArgvValue],
                   InputEx
                   );
        if (EFI_ERROR (Status)) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Copy Files Fail %r",Status);
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
        }
        
      } else if (Argv[ArgIndex][1] == 'w') {
        //
        // Write File
        //
        if (Argv[MaxArgvValue][0] == '-') {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          break;
      	}
        Status = WriteFiles (
                   Root,
                   Argv[MaxArgvValue],
                   InputEx
                   );
        
      } else if (Argv[ArgIndex][1] == 'i') {
        //
        // File Info
        //
        if (Argv[MaxArgvValue][0] == '-') {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"Incomplete Arguments");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          break;;
      	}
        CheckExist = CheckFileExist (
                       Root,
                       Argv[MaxArgvValue]
                       );
        if (CheckExist) {
          PrintFileInfo (
            Root,
            Argv[MaxArgvValue]
            );    
        } else {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"File NOT Exist\n");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
        }
      } else if (Argv[ArgIndex][1] == 'I') {
        //
        // System Info
        //
        Status = SysInfoGet (
                   Root
                   );
        if (EFI_ERROR (Status)) {
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_RED);
          Print(L"File System Information Fail\n");
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"\n");
          return	EFI_SUCCESS;
        }
        
      } else if ((Argv[ArgIndex][1] == 'h') ||
                 (Argv[ArgIndex][1] == 'H')) {
        CLEAN_SCREEN(VOID);
        HelpSection();
  
      }
    }
  }
    
  return EFI_SUCCESS;
}






