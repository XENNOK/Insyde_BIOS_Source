/** @file
  

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


#ifndef _FS_HW_H_
#define _FS_HW_H_
#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

#include <Protocol/EfiShellParameters.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Library/MemoryAllocationLib.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, x, y)
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, x)
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, x)

#define END_POSITION 0xFFFFFFFFFFFFFFFF
#define MAX_TYPE_PER_LINE 79
EFI_STATUS
SysInfoGet (
  IN EFI_FILE_PROTOCOL *Root
  );

EFI_STATUS
FileInfoGet (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16 *FileName ,
  IN EFI_FILE_INFO **FileInfo_Buf,
  IN EFI_FILE_PROTOCOL **File 
  );
  
VOID
PrintFileInfo (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  );

EFI_STATUS
NewFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  );

EFI_STATUS
ReadFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  );

EFI_STATUS
DeleteFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16 *FileName
  );

EFI_STATUS
MergeFiles (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16 *FirstFileName,
  IN CHAR16 *SecondFileName
  );

EFI_STATUS
CopyFiles (
  IN EFI_FILE_PROTOCOL                      *Root,
  IN CHAR16                                 *FirstFileName,
  IN CHAR16                                 *SecondFileName,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );
EFI_STATUS
WriteFiles (
  IN EFI_FILE_PROTOCOL                      *Root,
  IN CHAR16                                 *FileName,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );
  
BOOLEAN
CheckFileExist (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  );
#endif
