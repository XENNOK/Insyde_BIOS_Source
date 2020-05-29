/** @file
  Routines for displaying Windows UX capsule

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/VariableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextOut.h>
#include <Guid/FileInfo.h>
#include <Guid/EfiSystemResourceTable.h>

#pragma pack(1)
typedef struct {
  UINT8  Version;
  UINT8  Checksum;
  UINT8  ImageType;
  UINT8  Reserved;
  UINT32 Mode;
  UINT32 ImageOffsetX;
  UINT32 ImageOffsetY;
} WINDOWS_UX_CAPSULE_HEADER;
#pragma pack()

/**
  Dummy function for SetMode() of EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL

  @param  This       The protocol instance pointer.
  @param  ModeNumber The mode number to set.

  @retval EFI_SUCCESS      The requested text mode was set.

**/
EFI_STATUS
EFIAPI
DummySetMode (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN UINTN                              ModeNumber
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DummyGraphicsSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  IN  UINT32                       ModeNumber
  )
{
  return EFI_SUCCESS;
}

/**
  Dummy function for ClearScreen() of EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL

  @param  This              The protocol instance pointer.

  @retval  EFI_SUCCESS      The operation completed successfully.

**/
EFI_STATUS
EFIAPI
DummyClearScreen (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This
  )
{
  return EFI_SUCCESS;
}

/**
  Dummy function for OutputString() of EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL

  @param  This              The protocol instance pointer.
  @param  String            The NULL-terminated string to be displayed

  @retval  EFI_SUCCESS      The operation completed successfully.

**/
EFI_STATUS
EFIAPI
DummyOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL        * This,
  IN CHAR16                                 *String
  )
{
  return EFI_SUCCESS;
}

/**
  Disable the display of firmware update utility, the display will be controlled
  by Windows UX Capsule image if Windows Update is triggered

  @retval  EFI_SUCCESS      The operation completed successfully.

**/
EFI_STATUS
DisableVendorUserInterface (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  gST->ConOut->SetMode = DummySetMode;
  gST->ConOut->ClearScreen = DummyClearScreen;
  gST->ConOut->OutputString = DummyOutputString;
  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  if (Status == EFI_SUCCESS) {
    GraphicsOutput->SetMode = DummyGraphicsSetMode;
  }
  return EFI_SUCCESS;
}

/**
  Display the boot logo

  @retval  EFI_SUCCESS      The operation completed successfully.
  @retval  !EFI_SUCCESS     Failed on displaying the boot logo

**/
EFI_STATUS
DisplayBootLogo (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuffer;
  UINTN                                DestinationX;
  UINTN                                DestinationY;
  UINTN                                Width;
  UINTN                                Height;

  BltBuffer    = NULL;
  DestinationX = 0;
  DestinationY = 0;
  Width        = 0;
  Height       = 0;

  Status = BgrtGetBootImageInfo (
             &BltBuffer,
             &DestinationX,
             &DestinationY,
             &Width,
             &Height
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             BltBuffer,
                             EfiBltBufferToVideo,
                             0,
                             0,
                             DestinationX,
                             DestinationY,
                             Width,
                             Height,
                             Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                             );

  if (BltBuffer != NULL) {
    gBS->FreePool (BltBuffer);
  }

  return Status;
}

/**
  Load file data from a file device path

  @param[in] DevicePath         The file device path
  @param[out] FileData          Pointer to the retrived file data
  @param[out] FileSize          The size in bytes of the retrived file data

  @retval  EFI_SUCCESS      The operation completed successfully.
  @retval  !EFI_SUCCESS     Filed to read the file

**/
EFI_STATUS
LoadFileDataFromDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT VOID                      **FileData,
  OUT UINTN                     *FileSize
  )
{
  EFI_STATUS                      Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
  EFI_HANDLE                      Handle;
  EFI_FILE_HANDLE                 Root;
  EFI_FILE_HANDLE                 File;
  EFI_FILE_INFO                   *FileInfo;
  UINTN                           FileInfoSize;

  Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &DevicePath, &Handle);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **)&SimpleFileSystem
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Root->Open(
                   Root,
                   &File,
                   ((FILEPATH_DEVICE_PATH *)DevicePath)->PathName,
                   EFI_FILE_MODE_READ,
                   0
                  );
  if (EFI_ERROR(Status)) {
    Root->Close(Root);
    return Status;
  }

  FileInfoSize = sizeof(EFI_FILE_INFO);
  FileInfo = AllocatePool (FileInfoSize);

  if (FileInfo == NULL) {
    Root->Close(Root);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = File->GetInfo (
                  File,
                  &gEfiFileInfoGuid,
                  &FileInfoSize,
                  FileInfo
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (FileInfo);
    FileInfo = AllocatePool (FileInfoSize);
    if (FileInfo == NULL) {
      Root->Close(Root);
      return EFI_OUT_OF_RESOURCES;
    }
    Status = File->GetInfo (
                    File,
                    &gEfiFileInfoGuid,
                    &FileInfoSize,
                    FileInfo
                    );
  }

  if (Status != EFI_SUCCESS) {
    File->Close(File);
    Root->Close(Root);
    return Status;
  }

  *FileSize = (UINTN)FileInfo->FileSize;
  FreePool (FileInfo);

  *FileData = AllocatePool (*FileSize);

  if (*FileData != NULL) {
    Status = File->Read (File, FileSize, *FileData);
    if (EFI_ERROR(Status)) {
      FreePool(*FileData);
    }
  }

  File->Close(File);
  Root->Close(Root);

  return Status;
}

/**
  Display Windows UX capsule image

  @retval  TRUE                  Windows UX capsule is activated
  @retval  FALSE                 Windows UX capsule is not activated

**/
BOOLEAN
DisplayWindowsUxCapsule (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Size;
  EFI_DEVICE_PATH_PROTOCOL      *WindowsUxDevicePath;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
  BOOLEAN                       WindowsUxFound;
  WINDOWS_UX_CAPSULE_HEADER     *WindowsUxCapsule;
  UINT8                         *ImageData;
  UINTN                         ImageSize;
  UINTN                         BltSize;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINTN                         Height;
  UINTN                         Width;

  WindowsUxCapsule = NULL;
  WindowsUxFound = FALSE;
  Blt = NULL;

  Size = 0;
  WindowsUxDevicePath = NULL;
  Status = CommonGetVariableDataAndSize (
             L"WindowsUxCapsuleDevicePath",
             &gWindowsUxCapsuleGuid,
             &Size,
             (VOID **) &WindowsUxDevicePath
             );
  if (EFI_ERROR (Status)) {
    return WindowsUxFound;
  }
  WindowsUxFound = TRUE;

  gBS->RestoreTPL (TPL_CALLBACK);
  Status = LoadFileDataFromDevicePath (WindowsUxDevicePath, (VOID **)&WindowsUxCapsule, &Size);
  gBS->RaiseTPL (TPL_NOTIFY);
  if ((Status == EFI_SUCCESS) && (WindowsUxCapsule != NULL)) {
    DisplayBootLogo ();

    ImageData = (UINT8 *)(WindowsUxCapsule + 1);
    ImageSize = Size - sizeof (WINDOWS_UX_CAPSULE_HEADER);
    Status = ConvertBmpToGopBlt (
              ImageData,
              ImageSize,
              (VOID **) &Blt,
              &BltSize,
              &Height,
              &Width
              );
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **)&GraphicsOutput
                    );
    if (!EFI_ERROR(Status)) {

      Status = GraphicsOutput->Blt (
                                 GraphicsOutput,
                                 Blt,
                                 EfiBltBufferToVideo,
                                 0,
                                 0,
                                 WindowsUxCapsule->ImageOffsetX,
                                 WindowsUxCapsule->ImageOffsetY,
                                 Width,
                                 Height,
                                 Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                 );

      if (Blt != NULL) {
        gBS->FreePool (Blt);
      }
    }
  }
  if (WindowsUxCapsule != NULL) {
    FreePool (WindowsUxCapsule);
  }
  FreePool (WindowsUxDevicePath);
  if (WindowsUxFound && (WindowsUxCapsule != NULL) && !EFI_ERROR(Status)) {
    DisableVendorUserInterface();
  }

  return WindowsUxFound;
}
