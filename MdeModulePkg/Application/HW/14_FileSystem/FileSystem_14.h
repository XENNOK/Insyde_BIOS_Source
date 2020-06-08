/** @file
  FileSystem_14 H Source File

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

#ifndef _FILE_SYSTEM_14_H_
#define _FILE_SYSTEM_14_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

#include <Protocol/EfiShellParameters.h>
#include <Protocol/EfiShellInterface.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>

#include <Library/MemoryAllocationLib.h>

#define STRING_MATCH   0
#define STRING_UNMATCH 1

#define INITIAL_SIZE   10
#define SIZE_INCREMENT 10

VOID 
InitializeArray(
  CHAR16 *Buffer,
  UINT64 BufferSize
  );

CHAR16* 
InputWord (
  VOID
  );

EFI_STATUS
CreatFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  );

EFI_STATUS
DeleteFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  );

EFI_STATUS
ReadFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  );

EFI_STATUS
WriteFile (
  EFI_FILE  *Root,
  CHAR16    *FileName
  );

EFI_FILE_INFO*
GetFileInfo (
  EFI_FILE  *Root,
  CHAR16    *FileName
  );

VOID
ShowFileInfo (
  EFI_FILE_INFO *Buffer
  );

EFI_FILE_SYSTEM_INFO*
GetSystemInfo (
  EFI_FILE  *Root
  );

VOID
ShowSystemInfo (
  EFI_FILE_SYSTEM_INFO *SystemInfo
  );

VOID
ShowHelp(
  VOID
  );

EFI_STATUS
CopyFile (
  EFI_FILE  *Root,
  CHAR16    *CopyFile,
  CHAR16    *PasteFile
  );

EFI_STATUS
MergeFile (
  EFI_FILE  *Root,
  CHAR16    *FileName1,
  CHAR16    *FileName2
  );

#endif