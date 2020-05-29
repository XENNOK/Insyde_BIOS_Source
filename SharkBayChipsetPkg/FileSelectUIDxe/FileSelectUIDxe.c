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

#include "FileSelectUILib.h"
#include "FileConOut.h"
#include "FileTextOutHook.h"
#include "OpenSaveFile.h"
#include "FileHandleLib.h"
#include <Library/PrintLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/FileSelectUI.h>

EFI_STATUS
UnloadFileSelectDrv (
  IN EFI_HANDLE     ImageHandle
  )
{

  // release SimpleTextOut hook
  FinalizeTextOutHook();

  // uninstall FileSelect ID protocol
  gBS->UninstallProtocolInterface (
          ImageHandle,
          &gFileSelectUIProtocolGuid,
          NULL
          );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SaveFileOperation (
  IN VOID                       *BufferPtr,
  IN UINTN                      *BufferSize
  )
{
  EFI_STATUS              Status;
  INT32                   OldAttribute;
  BOOLEAN                 CursorVisible;
  INT32                   CursorColumn;
  INT32                   CursorRow;
  EFI_FILE                *FileHandle;

  InitializeFileConOut ();

  FcoGetModeInfo (&CursorColumn, &CursorRow, &CursorVisible, &OldAttribute);

  FcoEnableCursor (FALSE);

  //
  //Get File handle for UI operation.
  //
  Status = SaveFileUi (L"Save File", Type_Save, &FileHandle);

  if (EFI_ERROR(Status)) {
    goto EXIT;
  }
  Status = FileHandle->Write(FileHandle, BufferSize, BufferPtr);
  FileHandle->Close(FileHandle);

EXIT:


  FcoSetCursorPosition (CursorColumn, CursorRow);
  FcoEnableCursor (CursorVisible);
  FcoSetAttribute (OldAttribute);

  return Status;
}

EFI_STATUS
EFIAPI
LoadFileOperation (
  OUT VOID                       **BufferPtr,
  OUT UINTN                      *BufferSize
  )
{
  EFI_STATUS              Status;
  INT32                   OldAttribute;
  BOOLEAN                 CursorVisible;
  INT32                   CursorColumn;
  INT32                   CursorRow;
  EFI_FILE                *FileHandle;
  EFI_FILE_INFO           *FileInfo;

  InitializeFileConOut ();

  FcoGetModeInfo (&CursorColumn, &CursorRow, &CursorVisible, &OldAttribute);

  FcoEnableCursor (FALSE);
  //
  //Get File handle for UI operation.
  //
  Status = SaveFileUi (L"Load File", Type_Load, &FileHandle);

  if (EFI_ERROR(Status)) {
    *BufferPtr = NULL;
    *BufferSize = 0;
    goto EXIT;
  }
  FileInfo = FsuFileHandleGetInfo (FileHandle);
//[-start-140625-IB05080432-add]//
  if (FileInfo == NULL) {
    *BufferPtr = NULL;
    *BufferSize = 0;
    goto EXIT;
  }
//[-end-140625-IB05080432-add]//
  *BufferSize = FileInfo->FileSize;
  *BufferPtr = AllocateZeroPool (*BufferSize);


  Status = FileHandle->Read(FileHandle, BufferSize, *BufferPtr);
  FileHandle->Close(FileHandle);

EXIT:

  FcoSetCursorPosition (CursorColumn, CursorRow);
  FcoEnableCursor (CursorVisible);
  FcoSetAttribute (OldAttribute);

  return Status;
}

//
// Define driver entry point
//
//EFI_DRIVER_ENTRY_POINT (FileSelectUIPoint);

EFI_STATUS
EFIAPI
FileSelectUIEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS             Status;
  //EFI_HANDLE             Handle;

  EFI_LOADED_IMAGE_PROTOCOL           *LoadedImage;
  FILE_SELECT_UI_PROTOCOL             *FileSelectProtocol;

  // Check FileSelect driver is loaded.
  Status = LocateDriverIndex (&gFileSelectUIProtocolGuid);

  if (Status == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  // Register unload function
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (Status == EFI_SUCCESS) {
    LoadedImage->Unload = UnloadFileSelectDrv;
  }
  FileSelectProtocol = (FILE_SELECT_UI_PROTOCOL*)AllocatePool (sizeof (FILE_SELECT_UI_PROTOCOL));
//[-start-140625-IB05080432-add]//
  if (FileSelectProtocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//
  FileSelectProtocol->SaveFileOperation = SaveFileOperation;
  FileSelectProtocol->LoadFileOperation = LoadFileOperation;

  //
  // Install protocol
  //
  //Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gFileSelectUIProtocolGuid,
                  FileSelectProtocol,
                  NULL
                  );

  //
  // Initialize module
  //
  InitializeTextOutHook();

  return Status;
}

