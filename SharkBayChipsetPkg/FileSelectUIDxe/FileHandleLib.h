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

#ifndef _FILE_HANDLE_LIB_H_
#define _FILE_HANDLE_LIB_H_

#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>

#define MAX_FILE_NAME_LENGTH 522 // (20 * (6+5+2))+1) unicode characters from EFI FAT spec (doubled for bytes)
#define FIND_XXXX_FILE_BUFFER_SIZE (SIZE_OF_EFI_FILE_INFO + MAX_FILE_NAME_LENGTH)

EFI_FILE_INFO*
EFIAPI
FsuFileHandleGetInfo (
  IN EFI_FILE_HANDLE            FileHandle
  );

EFI_STATUS
EFIAPI
FsuFileHandleFindFirstFile (
  IN EFI_FILE_HANDLE            DirHandle,
  OUT EFI_FILE_INFO             **Buffer,
  OUT BOOLEAN                   *NoFile
  );

EFI_STATUS
EFIAPI
FsuFileHandleFindNextFile(
  IN EFI_FILE_HANDLE          DirHandle,
  OUT EFI_FILE_INFO          *Buffer,
  OUT BOOLEAN                *NoFile
  );

EFI_STATUS
FsuGetParentHandle (
  EFI_HANDLE      ChildHandle,
  EFI_HANDLE      *ParentHandle
  );

EFI_STATUS
FsuGetFileHandleByHandle (
  EFI_HANDLE                                      Handle,
  EFI_FILE                                        **FileHandle
  );

#endif // _SNAP_WIN_H_
