//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//;   FirmwareUpdateTrigger.c
//; 
//; Abstract:
//; 
//;   Routines for triggering firmware update
//;

#include "CapsuleService.h"
#include EFI_PROTOCOL_DEFINITION(SimpleFileSystem)
#include EFI_PROTOCOL_DEFINITION(DevicePath)
#include EFI_PROTOCOL_DEFINITION(ConsoleControl)
#include EFI_PROTOCOL_DEFINITION(SimpleTextOut)
#include EFI_PROTOCOL_DEFINITION(FileInfo)
#include EFI_PROTOCOL_DEFINITION (UgaSplash)
#include EFI_GUID_DEFINITION(EfiSystemResourceTable)
#include "OemGraphicsLib.h"


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

/*++

Routine Description:
  Convert EFI Status to ESRT status

Arguments:
  Status     - The value of EFI status

Returns:
  The converted ESRT status
  Listing of ESRT status
    ESRT_SUCCESS                      0x00000000
    ESRT_ERROR_UNSUCCESSFUL           0x00000001
    ESRT_ERROR_INSUFFICIENT_RESOURCES 0x00000002
    ESRT_ERROR_INCORRECT_VERSION      0x00000003
    ESRT_ERROR_INVALID_IMAGE_FORMAT   0x00000004
    ESRT_ERROR_AUTHENTICATION         0x00000005
    ESRT_ERROR_AC_NOT_CONNECTED       0x00000006
    ESRT_ERROR_INSUFFICIENT_BATTERY   0x00000007
**/
ESRT_STATUS
GetEsrtStatus (
  EFI_STATUS Status
  )
{
  ESRT_STATUS EsrtStatus;
  switch (Status) {
    case EFI_SUCCESS:
      EsrtStatus = ESRT_SUCCESS;
      break;
    case EFI_INCOMPATIBLE_VERSION:
      EsrtStatus = ESRT_ERROR_INCORRECT_VERSION;
      break;
    case EFI_OUT_OF_RESOURCES:
    case EFI_VOLUME_FULL:
      EsrtStatus = ESRT_ERROR_INSUFFICIENT_RESOURCES;
      break;
    case EFI_LOAD_ERROR:
      EsrtStatus = ESRT_ERROR_INVALID_IMAGE_FORMAT;
      break;
    case EFI_SECURITY_VIOLATION:
      EsrtStatus = ESRT_ERROR_AUTHENTICATION;
      break;
    default:
      EsrtStatus = ESRT_ERROR_UNSUCCESSFUL;
  }
  return EsrtStatus;
}


EFI_STATUS
EFIAPI
DummySetMode (
  IN EFI_SIMPLE_TEXT_OUT_PROTOCOL    *This,
  IN UINTN                              ModeNumber
  )
/*++

Routine Description:
  Dummy function for SetMode() of EFI_SIMPLE_TEXT_OUT_PROTOCOL

Arguments:
  This         - The protocol instance pointer.
  ModeNumber   - The mode number to set.

Returns:
  EFI_SUCCESS  - Function returns successfully

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
DummyGraphicsSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  IN  UINT32                       ModeNumber
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
DummyClearScreen (
  IN EFI_SIMPLE_TEXT_OUT_PROTOCOL    *This
  )
/*++

Routine Description:
  Dummy function for ClearScreen() of EFI_SIMPLE_TEXT_OUT_PROTOCOL

Arguments:
  This         - The protocol instance pointer.

Returns:
  EFI_SUCCESS  - Function returns successfully

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
DummyOutputString (
  IN EFI_SIMPLE_TEXT_OUT_PROTOCOL        * This,
  IN CHAR16                                 *String
  )
/*++

Routine Description:
  Dummy function for OutputString() of EFI_SIMPLE_TEXT_OUT_PROTOCOL

Arguments:
  This         - The protocol instance pointer.
  String       - The NULL-terminated string to be displayed
  
Returns:
  EFI_SUCCESS  - Function returns successfully

--*/
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
/*++

Routine Description:
  Disable the display of firmware update utility, the display will be controlled
  by Windows UX Capsule image if Windows Update is triggered

Arguments:
  None
  
Returns:
  EFI_SUCCESS  - Function returns successfully

--*/
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  gST->ConOut->SetMode = DummySetMode;
  gST->ConOut->ClearScreen = DummyClearScreen;
  gST->ConOut->OutputString = DummyOutputString;
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
  if (Status == EFI_SUCCESS) {
    GraphicsOutput->SetMode = DummyGraphicsSetMode;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
DisplayBootLogo (
  VOID
  )
/*++

Routine Description:
  Disable the display of firmware update utility, the display will be controlled
  by Windows UX Capsule image if Windows Update is triggered

Arguments:
  None
  
Returns:
  EFI_SUCCESS  - Function returns successfully
  Others       - Failed on displaying the boot logo

--*/
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuffer;
  EFI_CONSOLE_CONTROL_PROTOCOL         *ConsoleControl;
  UINTN                                DestinationX;
  UINTN                                DestinationY;
  UINTN                                Width;
  UINTN                                Height;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
  if (!EFI_ERROR(Status)) {
    ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
  }

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

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
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

EFI_STATUS
LoadFileDataFromDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT VOID                      **FileData,
  OUT UINTN                     *FileSize
  )
/*++

Routine Description:
  Load file data from a file device path

Arguments:
  DevicePath   - The file device path
  FileData     - Pointer to the retrived file data
  FileSize     - The size in bytes of the retrived file data
  
Returns:
  EFI_SUCCESS  - Function returns successfully
  Others       - Failed on displaying the boot logo

--*/
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

  Status = gBS->HandleProtocol (Handle, &gEfiSimpleFileSystemProtocolGuid, &SimpleFileSystem);
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
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  FileInfoSize,
                  (VOID **) &FileInfo
                  );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    goto Done;
  }

  Status = File->GetInfo (
                   File,
                   &gEfiFileInfoGuid,
                   &FileInfoSize,
                   FileInfo
                   );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    gBS->FreePool (FileInfo);
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    FileInfoSize,
                    (VOID **) &FileInfo
                    );
    if (EFI_ERROR(Status)) {
      ASSERT_EFI_ERROR(Status);
      goto Done;
    }                    
    Status = File->GetInfo (
                     File,
                     &gEfiFileInfoGuid,
                     &FileInfoSize,
                     FileInfo
                     );
  }

  if (EFI_ERROR(Status)) {
    goto Done;
  }

  *FileSize = (UINTN)FileInfo->FileSize;
  gBS->FreePool (FileInfo);
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  *FileSize,
                  (VOID **)FileData
                  );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    goto Done;
  }                    
  

  if (*FileData != NULL) {
    Status = File->Read (File, FileSize, *FileData);
    if (EFI_ERROR(Status)) {
      gBS->FreePool(*FileData);
    }
  }

Done:
  File->Close(File);
  Root->Close(Root);

  return Status;
}

BOOLEAN
DisplayWindowsUxCapsule (
  VOID
  )
/*++

Routine Description:
  Display Windows UX capsule image

Arguments:
  None
  
Returns:
  TRUE         - Windows UX capsule is activated
  FALSE        - Windows UX capsule is not activated

--*/
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

  WindowsUxDevicePath = NULL;
  WindowsUxCapsule = NULL;
  WindowsUxFound = FALSE;
  Blt = NULL;
  Size = 0;
  Status = EfiGetVariable (
             L"WindowsUxCapsuleDevicePath",
             &gWindowsUxCapsuleGuid,
             NULL,
             &Size,
             WindowsUxDevicePath
             );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return FALSE;
  }
  WindowsUxFound = TRUE;
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  Size,
                  (VOID **)&WindowsUxDevicePath
                  );
  
  ASSERT_EFI_ERROR(Status);

  Status = EfiGetVariable (
             L"WindowsUxCapsuleDevicePath",
             &gWindowsUxCapsuleGuid,
             NULL,
             &Size,
             WindowsUxDevicePath
             );
  if (EFI_ERROR(Status)) {
    gBS->FreePool (WindowsUxDevicePath);
    ASSERT_EFI_ERROR(Status);
    return WindowsUxFound;
  }
  gBS->RestoreTPL (EFI_TPL_CALLBACK);
  Status = LoadFileDataFromDevicePath (WindowsUxDevicePath, (VOID **)&WindowsUxCapsule, &Size);
  gBS->RaiseTPL (EFI_TPL_NOTIFY);
  if (Status == EFI_SUCCESS) {
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
    if (!EFI_ERROR(Status)) {
      GraphicsOutput->SetMode (GraphicsOutput, WindowsUxCapsule->Mode);    
    }

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
    gBS->FreePool (WindowsUxCapsule);
  }
  gBS->FreePool (WindowsUxDevicePath);
  if (WindowsUxFound && !EFI_ERROR(Status)) {
    DisableVendorUserInterface();
  }

  return WindowsUxFound;
}

VOID
EFIAPI
TriggerFirmwareUpdate (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
/*++

Routine Description:
  ReadyToBoot notification event handler for capsule image update.

Arguments:
  Event        - Event whose notification function is being invoked.
  Context      - Pointer to the notification function's context.
  
Returns:
  None

--*/
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *CapsuleDevicePath;
  UINTN                       Size;
  EFI_FILE_HANDLE             CapsuleHandle;
  EFI_SYSTEM_RESOURCE_TABLE   *Esrt;
  UINTN                       Index;
  ESRT_STATUS                 EsrtStatus;
  BOOLEAN                     WindowsUxCapsuleFound;
  EFI_HANDLE                  ImageHandle;
  
  ImageHandle = (EFI_HANDLE)Context;

  gBS->CloseEvent(Event);

  Status = EfiLibGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, &Esrt);
  if (EFI_ERROR(Status)) {
    return;
  }

  WindowsUxCapsuleFound = DisplayWindowsUxCapsule();

  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    Size = 0;
    CapsuleDevicePath = NULL;
    CapsuleHandle = NULL;
    Status = EfiGetVariable (
               L"CapsuleDevicePath",
               &Esrt->FirmwareResources[Index].FirmwareClass,
               NULL,
               &Size,
               CapsuleDevicePath
               );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }
    CapsuleDevicePath = NULL;
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    Size, 
                    (VOID **)&CapsuleDevicePath
                    );
    if (Status == EFI_SUCCESS) {                
      Status = EfiGetVariable (
                 L"CapsuleDevicePath",
                 &Esrt->FirmwareResources[Index].FirmwareClass,
                 NULL,
                 &Size,
                 CapsuleDevicePath
                 );
    }
    if (EFI_ERROR(Status)) {              
      ASSERT_EFI_ERROR(Status);
      if (CapsuleDevicePath != NULL) {
        gBS->FreePool (CapsuleDevicePath);
      }
      continue;
    }

    Status = EfiSetVariable (
               L"CapsuleDevicePath",
               &Esrt->FirmwareResources[Index].FirmwareClass,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
    if (CapsuleDevicePath != NULL) {
      Status = EnableCapsuleSecurityCheck();
      if (!EFI_ERROR(Status)) {
        Status = gBS->LoadImage (FALSE, ImageHandle, CapsuleDevicePath, NULL, 0, &CapsuleHandle);
      }
      gBS->FreePool(CapsuleDevicePath);
      if (Status == EFI_SUCCESS) {
        if (!WindowsUxCapsuleFound) {
          //
          // Windows UX Capsule is not found, call DisableQuietBoot() to enable SecureFlash display
          // 
          DisableQuietBoot();
        }
        Status = gBS->StartImage(CapsuleHandle, NULL, NULL);
      } else {
        Status = EFI_SECURITY_VIOLATION;
      }
      DisableCapsuleSecurityCheck();
      EsrtStatus = GetEsrtStatus(Status);
      EfiSetVariable (
        L"EsrtLastAttemptStatus",
        &Esrt->FirmwareResources[Index].FirmwareClass,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
        sizeof(UINT32),
        &EsrtStatus
        );
      EfiResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
  //
  // Remove WindowsUxCapsuleDevicePath variable after all the firmware images have been updated
  //
  if (WindowsUxCapsuleFound) {
    Status = EfiSetVariable (
               L"WindowsUxCapsuleDevicePath",
               &gWindowsUxCapsuleGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
  }
}