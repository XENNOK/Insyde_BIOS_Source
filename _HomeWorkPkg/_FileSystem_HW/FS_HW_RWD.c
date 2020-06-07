/** @file

Read Write Delete Copy Merge  SystemFile   

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
 New File 
  
  @param[in]       Root           EFI_FILE_PROTOCOL.
  @param[in]       FileName       File name.
                             
 
  @return Status Code
*/
EFI_STATUS
NewFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_STATUS         Status;
  EFI_FILE_PROTOCOL  *SystemFile = 0;
  BOOLEAN            CheckExist;
  
  CheckExist = CheckFileExist (
                 Root,
                 FileName
                 );
  if (CheckExist) {
    return EFI_ACCESS_DENIED;
  }
  
  Status = Root->Open (
                   Root,     
                   &SystemFile,
                   FileName, 
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   EFI_FILE_ARCHIVE
                   );

  if (EFI_ERROR (Status)) {
    SET_COLOR (EFI_WHITE | EFI_BACKGROUND_RED);
    Print (
      L"Create Fail %d\n",
      Status
      );
    SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
    Print (L"\n");
    return Status;
   }
  
  Print (
    L"\nCreated File Finish... %r %s\n",
    Status,
    FileName
    );
  Root->Close (SystemFile);
  return EFI_SUCCESS;
}

/**
  Delete File
  
  @param[in]       Root           EFI_FILE_PROTOCOL.
  @param[in]       FileName       File name.
                             
 
  @return Status Code
*/
EFI_STATUS
DeleteFiles (
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
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   EFI_FILE_ARCHIVE
                   );
  if (EFI_ERROR (Status)) {
    Print (L"Open File Fail\n");
    return Status;
  }

  Status = Root->Delete (SystemFile);
  
  if (EFI_ERROR (Status)) {
    SET_COLOR (EFI_WHITE | EFI_BACKGROUND_RED);
    Print (L"FIleDelete Fail.\n");
    SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
    Print (L"\n");
    Root->Close (SystemFile);
    return Status;
  }

return EFI_SUCCESS;
}

/**
  Read File
  
  @param[in]       Root           EFI_FILE_PROTOCOL.
  @param[in]       FileName       File name.
                             
 
  @return Status Code
*/
EFI_STATUS
ReadFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_STATUS                     Status;
  EFI_FILE_INFO                  *FileInfoBuf;
  EFI_FILE_PROTOCOL              *SystemFile = 0;
  UINTN                          BufferSize;
  CHAR8                          FileDataBuf[1024] = {'0'};
  UINTN                          SizeCount;
  UINTN                          TotalSize;
  BOOLEAN                        CheckExist;

  TotalSize = 0;
  CheckExist = CheckFileExist (
                 Root,
                 FileName
                 );
  if (!CheckExist) {
   //
   // File NOT Exist.
   //
    return EFI_ACCESS_DENIED;
  }
  Status = FileInfoGet (
             Root,
             FileName ,
             &FileInfoBuf,
             &SystemFile
             );
  if (EFI_ERROR (Status)) {
     Print (L"Get_FileInfo Fail\n");
   }
  
  BufferSize = 1024;
  Print (L"Read Data:\n");
  do {
   
    Status = SystemFile->Read (
                           SystemFile,
                           &BufferSize,
                           &FileDataBuf
                           );
    if (EFI_ERROR (Status)) {
      SystemFile->Close (SystemFile);
      FreePool (FileInfoBuf);
      SET_COLOR (EFI_WHITE | EFI_BACKGROUND_RED);
      Print (L"Read Fail.");
      SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
      break;
    }
    

    for (SizeCount = 0; SizeCount < BufferSize; SizeCount++) {
      if (FileDataBuf[SizeCount] == '\n') {
        Print (L"\r");
      }
      Print(L"%c",FileDataBuf[SizeCount]);
    }
    TotalSize += BufferSize;
  }while (BufferSize != 0) ;
  
  Print (
    L"\nRead Finish... Total Size: %d\n",
    TotalSize
    );
  
  SystemFile->Close (SystemFile);
  
  FreePool (FileInfoBuf);
  
  return EFI_SUCCESS;
}

/**
  Copy File
  
  @param[in] Root            EFI_FILE_PROTOCOL.
  @param[in] FirstFileName   First File Name.
  @param[in] SecondFileName  Second File Name.                           
  @param[in] InputEx         EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  
  @return Status Code
*/
EFI_STATUS
CopyFiles (
  IN EFI_FILE_PROTOCOL                      *Root,
  IN CHAR16                                 *FirstFileName,
  IN CHAR16                                 *SecondFileName,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{

  EFI_STATUS                     Status;
  EFI_KEY_DATA                   HKey;
  EFI_FILE_PROTOCOL              *FirstSystemFile = 0;
  EFI_FILE_PROTOCOL              *SecondSystemFile = 0;
  
  UINTN                          BufferSize;
  UINTN                          SecondBufferSize;
  UINTN                          TotalSize;
  
  CHAR8                          FileDataBuf[1024] = {'0'};
  BOOLEAN                        CheckExist;
  
  BufferSize = 1024;
  SecondBufferSize = 1024;
  TotalSize = 0;

  //
  // Check First File exist?
  // if NOT exist return.
  // else open.
  //
  CheckExist = CheckFileExist (
                 Root,
                 FirstFileName
                 );
  if (!CheckExist) {
    return EFI_NOT_FOUND;
  } else {
    Status = Root->Open (
                     Root,     
                     &FirstSystemFile,
                     FirstFileName, 
                     EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                     EFI_FILE_ARCHIVE
                     );
     if (EFI_ERROR (Status)) {
     Print (L"Open First File Fail\n");
     Print (L"\n");
     }
  }

  //
  // Check First File exist?
  // if exist return.
  // else open.
  //
  CheckExist = CheckFileExist (
                 Root,
                 SecondFileName
                 );
  if (CheckExist) {
    Print (L"\nFile Exist. Do you wants to cover or Merge old File?(Y/M/N) ");
    while (TRUE) {
      
      HKey = keyRead (
               InputEx
               );
      if ((HKey.Key.ScanCode == 0) &&
         (HKey.Key.UnicodeChar == 'y' || HKey.Key.UnicodeChar == 'Y')) {
         Print (L"YES\n");
         DeleteFiles (
           Root,
           SecondFileName
           );
         break;
         
      } else if ((HKey.Key.ScanCode == 0) &&
                 (HKey.Key.UnicodeChar == 'm' || HKey.Key.UnicodeChar == 'M')) {
        Print (L"Merge\n");
        Status = MergeFiles (
                   Root,
                   FirstFileName,
                   SecondFileName
                   );
        return Status;
        
      } else if ((HKey.Key.ScanCode == 0) &&
                 (HKey.Key.UnicodeChar == 'n' || HKey.Key.UnicodeChar == 'N')) {
        Print (L"No\n");
        return EFI_ACCESS_DENIED;
      }
    }
  }
  Status = Root->Open (
                   Root,     
                   &SecondSystemFile,
                   SecondFileName, 
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   EFI_FILE_ARCHIVE
                   );
   if (EFI_ERROR (Status)) {
     Print (L"Open Second File Fail\n");
   }
  
  //
  // Copy First File to Second File
  //
  do {
    Status = FirstSystemFile->Read (
                                FirstSystemFile,
                                &BufferSize,
                                &FileDataBuf
                                );
    if (EFI_ERROR (Status)) {
      Print (L"Read Fail\n");
      FirstSystemFile->Close (FirstSystemFile);
      return Status;
    }

    SecondBufferSize = BufferSize;
    SecondSystemFile->SetPosition (
                        SecondSystemFile,
                        TotalSize
                        );
    
    Status = SecondSystemFile->Write (
                                 SecondSystemFile,
                                 &BufferSize,
                                 &FileDataBuf
                                 );
    if (EFI_ERROR (Status)) {
      Print (L"Write Fail\n"); 
      SecondSystemFile->Close (SecondSystemFile);  
      return Status;
    }
    TotalSize += BufferSize;
  } while ( SecondBufferSize != 0);
  Print (L"\nCopy Finish...\n");

  FirstSystemFile->Close (FirstSystemFile);
  SecondSystemFile->Close (SecondSystemFile);
  
  return EFI_SUCCESS;
}


/**
  Merge File
  
  @param[in] Root            EFI_FILE_PROTOCOL.
  @param[in] FirstFileName   First File Name.
  @param[in] SecondFileName  Second File Name.                           

  
  @return Status Code
*/
EFI_STATUS
MergeFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FirstFileName,
  IN CHAR16            *SecondFileName
  )
{

  EFI_STATUS                     Status;
  EFI_FILE_PROTOCOL              *FirstSystemFile = 0;
  EFI_FILE_PROTOCOL              *SecondSystemFile = 0;
  
  UINTN                          BufferSize;
  UINTN                          SecondBufferSize;
  UINTN                          TotalSize;
  
  CHAR8                          FileDataBuf[1024] = {'0'};
  BOOLEAN                        CheckExist;
  
  BufferSize = 1024;
  SecondBufferSize = 1024;
  TotalSize = 0;

  //
  // Check First File Exist?
  // If Exist, Open file.
  //
  CheckExist = CheckFileExist (
                 Root,
                 FirstFileName
                 );
  if (!CheckExist) {
    return EFI_NOT_FOUND;
  } else {
    Status = Root->Open (
                     Root,     
                     &FirstSystemFile,
                     FirstFileName, 
                     EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                     EFI_FILE_ARCHIVE
                     );
     if (EFI_ERROR (Status)) {
       Print (L"Open First File Fail\n");
       return Status;
   }
  }

  //
  // Open Second File.
  //
  Status = Root->Open (
                   Root,     
                   &SecondSystemFile,
                   SecondFileName, 
                   EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                   EFI_FILE_ARCHIVE
                   );
   if (EFI_ERROR (Status)) {
     Print (L"Open Second File Fail\n");
     return Status;
   }

  //
  // Merge First File to Second File's tail.
  //
  do {
    Status = FirstSystemFile->Read (
                                FirstSystemFile,
                                &BufferSize,
                                &FileDataBuf
                                );
    if (EFI_ERROR (Status)) {
      Print (L"Read Fail\n");
      FirstSystemFile->Close (FirstSystemFile);
      return Status;
    }

    SecondBufferSize = BufferSize;
    SecondSystemFile->SetPosition (
                        SecondSystemFile,
                        END_POSITION
                        );
    
    Status = SecondSystemFile->Write (
                                 SecondSystemFile,
                                 &BufferSize,
                                 &FileDataBuf
                                 );
    if (EFI_ERROR (Status)) {
      Print (L"Write Fail\n"); 
      SecondSystemFile->Close (SecondSystemFile);  
      return Status;
    }
    TotalSize += BufferSize;
  } while ( SecondBufferSize != 0);
  Print (L"\nMerge Finish...\n");

  FirstSystemFile->Close (FirstSystemFile);
  SecondSystemFile->Close (SecondSystemFile);
  
  return EFI_SUCCESS;
}

/**
  Write File
  
  @param[in] Root            EFI_FILE_PROTOCOL.
  @param[in] FileName        File Name.                         
  @param[in] InputEx         EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  
  @return Status Code
*/
EFI_STATUS
WriteFiles (
  IN EFI_FILE_PROTOCOL                      *Root,
  IN CHAR16                                 *FileName,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
 EFI_KEY_DATA       HKey;
 EFI_STATUS         Status;
 EFI_FILE_PROTOCOL  *TempSystemFile = 0;
 CHAR16             *TempFile[] = {
                       L"TmpFile"
                       };
 CHAR8              TempFileBuf;
 UINTN              TempBufSize;
 UINT64             TempFilePosition;
 UINT8              PositionX;
 UINT8              PositionY;

 PositionX = 0;
 PositionY = 1;
 TempFilePosition = 0;
 TempBufSize = 1;
 
 Status = Root->Open (
                  Root,     
                  &TempSystemFile,
                  TempFile[0], 
                  EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                  EFI_FILE_ARCHIVE
                  );

 if (EFI_ERROR (Status)) {
   SET_COLOR (EFI_WHITE | EFI_BACKGROUND_RED);
   Print (
     L"Create Fail %d\n",
     Status
     );
   SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
   Print (L"\n");
   return Status;
 }
 
 CLEAN_SCREEN (VOID);
 SetCursorPosColor (
   EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
   PositionX,
   0
   );
 
 Print (L"Please Input Data : (Press Ctrl+F1 to confirm , Esc to cancel)\n");
 SetCursorPosColor (
   EFI_WHITE | EFI_BACKGROUND_BLACK,
   PositionX,
   PositionY
   );
 
 while (TRUE) {
   SET_CUR_POS (
      PositionX,
      PositionY 
      );
   HKey = keyRead (
            InputEx
            );

   if ((HKey.Key.ScanCode == 0) &&
       !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
         (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
       (HKey.Key.UnicodeChar >= 0x20 && HKey.Key.UnicodeChar <= 0x7E)) {
       
     Print (L"%c",HKey.Key.UnicodeChar);
     
     TempFileBuf = (CHAR8) HKey.Key.UnicodeChar;
     TempSystemFile->SetPosition (
                       TempSystemFile,
                       TempFilePosition
                       );
     Status = TempSystemFile->Write (
                                TempSystemFile,
                                &TempBufSize,
                                &TempFileBuf
                                );
     TempFilePosition++;
     PositionX ++;
     
     if (PositionX > MAX_TYPE_PER_LINE) {
       PositionX = 0;
       PositionY++ ;
     }
   } else if (HKey.Key.UnicodeChar == CHAR_BACKSPACE) {

     TempFilePosition--;
     TempFileBuf = ' ';
     
     TempSystemFile->SetPosition (
                       TempSystemFile,
                       TempFilePosition
                       );
     Status = TempSystemFile->Write (
                                TempSystemFile,
                                &TempBufSize,
                                &TempFileBuf
                                );
     PositionX --;
     
     if (PositionX == 0xFF) {
       PositionX = MAX_TYPE_PER_LINE ;
       PositionY-- ;
     }
     
     SET_CUR_POS (
       PositionX,
       PositionY 
       );
     Print (L" ");
     SET_CUR_POS (
       PositionX,
       PositionY 
       );

   } else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
   
     Print (L"\n");
     TempFileBuf = '\n';
     
     TempSystemFile->SetPosition (
                       TempSystemFile,
                       TempFilePosition
                       );
     Status = TempSystemFile->Write (
                                TempSystemFile,
                                &TempBufSize,
                                &TempFileBuf
                                );
     TempFilePosition++;
     PositionX = 0;
     PositionY++ ;
     
     if (PositionX > MAX_TYPE_PER_LINE) {
       PositionX = 0;
       PositionY++ ;
     }
   } else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
     ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
      (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
     (HKey.Key.ScanCode == SCAN_F1)) {
     Status = CopyFiles (
                Root,
                TempFile[0],
                FileName,
                InputEx
                );
     
     CLEAN_SCREEN (VOID);
     
     if (EFI_ERROR (Status)) {
       SET_COLOR (EFI_WHITE | EFI_BACKGROUND_RED);
       Print (
        L"Write into %s Fail",
        FileName
        );
       SET_COLOR (EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
       Print (L"\n");
       return Status;
     }
     Print (
      L"Write into %s SUCCESS\n",
      FileName
      ); 
     break;
   } else if (HKey.Key.ScanCode == SCAN_ESC) {
     break;
   }
   
 }
 
 TempSystemFile->Close (TempSystemFile);
 DeleteFiles (
   Root,
   TempFile[0]
   );
 return EFI_SUCCESS;
}

