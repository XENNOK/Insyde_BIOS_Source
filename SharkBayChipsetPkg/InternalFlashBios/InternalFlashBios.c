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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/GenericBdsLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/RamDiskProtocol.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/InternalFlashBios.h>

EFI_STATUS
UnloadInternalFlashBiosDrv (
  IN EFI_HANDLE     ImageHandle
  )
{

  gBS->UninstallProtocolInterface (
          ImageHandle,
          &gInternalFlashBiosProtocolGuid,
          NULL
          );

  return EFI_SUCCESS;
}

EFI_STATUS
LocateDriverIndex (
  EFI_GUID        *ProtocolGuid
  )
{
  EFI_STATUS      Status;
  UINTN           HandleCount;
  EFI_HANDLE      *HandleBuffer;
  UINTN           HIndex;
  UINTN           PIndex;
  UINTN           ProtocolCount;
  EFI_GUID        **ProtocolBuffer;
  VOID            *Interface;

  Status = gBS->LocateProtocol (ProtocolGuid, NULL, &Interface);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateHandleBuffer (
                AllHandles,
                NULL,
                NULL,
                &HandleCount,
                &HandleBuffer
                );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HIndex = HandleCount; HIndex > 0; HIndex--) {
    
    Status = gBS->ProtocolsPerHandle (HandleBuffer[HIndex-1], &ProtocolBuffer, &ProtocolCount);
    
    for (PIndex = 0; PIndex < ProtocolCount; PIndex++) {
      if(CompareGuid(ProtocolBuffer[PIndex], ProtocolGuid) == TRUE) {
        gBS->FreePool (ProtocolBuffer);
        gBS->FreePool (HandleBuffer);
        return EFI_SUCCESS;
      }
    }
    
    gBS->FreePool (ProtocolBuffer);
  }
  
  gBS->FreePool (HandleBuffer);
  return EFI_NOT_FOUND;
}

EFI_STATUS
SetArgs (
  IN  EFI_HANDLE                          ImageHandle,
  IN  CHAR16                              *Command
  )
/*++
  pass a "canned" argument to the shell script, "DoRecovery.nsh"

  @param  ImageHandle   

  @retval EFI_SUCCESS   
  @retval Other         

--*/
{
  EFI_STATUS                              Status;
  EFI_LOADED_IMAGE_PROTOCOL               *Image;
  UINTN                                   ArgsLen;
  CHAR16                                  *ArgsCopy;
  CHAR16                                 RootString[] = L"\\\0";
  FILEPATH_DEVICE_PATH                   *TempFilePath;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID*) &Image
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ArgsLen = (StrLen (Command) + 1) * 2;

  if (ArgsLen >= 256) {
    ArgsCopy = AllocatePool (ArgsLen);
  } else {
    ArgsCopy = AllocatePool (256);
  }

//[-start-140625-IB05080432-add]//
  if (ArgsCopy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//

  StrCpy (ArgsCopy, Command);

  if (Image->LoadOptions != NULL) {
    gBS->FreePool (Image->LoadOptions);
  }

  TempFilePath = (FILEPATH_DEVICE_PATH *)Image->FilePath;

  StrCpy ( TempFilePath->PathName, RootString);

  Image->LoadOptions = ArgsCopy;
  Image->LoadOptionsSize = (UINT32)ArgsLen;

  return EFI_SUCCESS;
}

EFI_STATUS
GetRamDiskHandles (
  OUT EFI_HANDLE                    *Handle,
  OUT EFI_FILE_HANDLE               *RootHandle
  )
{
  EFI_STATUS                        Status;
  EFI_FILE_HANDLE                   RootFileHandle;
  EFI_HANDLE                        *FileSystemHandles;
  UINTN                             Index;
  UINTN                             NumberFileSystemHandles;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *TempDevicePath;
  BOOLEAN                           RamDiskFound;
  EFI_GUID                          RamdiskGuid = {0x06ED4DD0, 0xFF78, 0x11D3, {0xBD, 0xC4, 0x00, 0xA0, 0xC9, 0x40, 0x53, 0xD1 }};
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;

  RamDiskFound = FALSE;
  //
  // to perform bios update procedure
  // create EFI shell on ramdisk
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberFileSystemHandles,
                  &FileSystemHandles
                  );
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );
    TempDevicePath = DevicePath;
    while (!IsDevicePathEnd(TempDevicePath)) {
      if (DevicePathType (TempDevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType(TempDevicePath) == MSG_VENDOR_DP) {
        if (CompareGuid (&(((VENDOR_DEVICE_PATH*) TempDevicePath)->Guid), &RamdiskGuid)) {
          RamDiskFound = TRUE;
          break;
        }
      }
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }
    if (RamDiskFound == FALSE) {
      continue;
    } else {
      //
      // open the volume
      //
      Status = gBS->HandleProtocol (FileSystemHandles[Index], &gEfiSimpleFileSystemProtocolGuid, (VOID **) &Volume);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Status = Volume->OpenVolume (Volume, &RootFileHandle);
      if (!EFI_ERROR(Status)) {
        *Handle     = FileSystemHandles[Index];
        *RootHandle = RootFileHandle;
        return Status;
      }
    }
  }
  return EFI_NOT_FOUND;
}

EFI_STATUS
InternalFlashBios (
  VOID                       *BufferPtr,
  UINTN                      BufferSize,
  CHAR16                     *CommandString
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  EFI_FILE_HANDLE                   NewFileHandle;
  EFI_FILE_HANDLE                   RootFileHandle;
  UINTN                             FileSize;
  UINT8                             *FileBuffer;
  EFI_DEVICE_PATH_PROTOCOL          *FilePath = NULL;
  EFI_HANDLE                        FlashImageHandle;
  CHAR16                            Command[256] = {0};


  //
  // Install dummy protocol to trigger RamDisk
  //
  DeviceHandle = NULL;
  gBS->InstallProtocolInterface (
         &DeviceHandle,
         &gEfiRamDiskDummyProtocolGuid,
         EFI_NATIVE_INTERFACE,
         NULL
         );
  BdsLibConnectAllDriversToAllControllers ();

  DeviceHandle = NULL;
  Status = GetRamDiskHandles (&DeviceHandle, &RootFileHandle);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // get bios update package
  //
  FileBuffer = (UINT8 *)BufferPtr;
  FileSize    = BufferSize;


  NewFileHandle = NULL;
  Status = RootFileHandle->Open (
                         RootFileHandle,
                         &NewFileHandle,
                         L"biosupdatepkg.efi",
                         EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                         EFI_FILE_ARCHIVE
                         );

  if (EFI_ERROR(Status)) {
    RootFileHandle->Close (RootFileHandle);
    return Status;
  }
  Status = NewFileHandle->Write (NewFileHandle, &FileSize, (VOID *)FileBuffer);
  gBS->FreePool (FileBuffer);
  NewFileHandle->Close (NewFileHandle);

  FilePath = NULL;
  FilePath = FileDevicePath (DeviceHandle, L"biosupdatepkg.efi");

  Status = gBS->LoadImage (
                  TRUE,
                  gImageHandle,
                  FilePath,
                  NULL,
                  0,
                  &FlashImageHandle
                  );
  
  gBS->FreePool (FilePath);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->RestoreTPL (TPL_APPLICATION);

  StrCat (Command, L"biosupdatepkg.efi ");
  StrCat (Command, CommandString);
  Status = SetArgs (FlashImageHandle, Command);

  Status = gBS->StartImage (FlashImageHandle, NULL, NULL);

  gBS->RaiseTPL (TPL_DRIVER);
  return Status;
}

EFI_STATUS
EFIAPI
InternalFlashBiosEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS             Status;
  //EFI_HANDLE             Handle;

  EFI_LOADED_IMAGE_PROTOCOL           *LoadedImage;
  INTERNAL_FLASH_BIOS_PROTOCOL        *InternalFlashBiosProtocol;

  Status = LocateDriverIndex (&gInternalFlashBiosProtocolGuid);

  if (Status == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

  // Register unload function
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (Status == EFI_SUCCESS) {
    LoadedImage->Unload = UnloadInternalFlashBiosDrv;
  }
  InternalFlashBiosProtocol = (INTERNAL_FLASH_BIOS_PROTOCOL*)AllocatePool (sizeof (INTERNAL_FLASH_BIOS_PROTOCOL));
//[-start-140625-IB05080432-add]//
  if (InternalFlashBiosProtocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//
  InternalFlashBiosProtocol->FlashBios = InternalFlashBios;

  //
  // Install protocol
  //
  //Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gInternalFlashBiosProtocolGuid,
                  InternalFlashBiosProtocol,
                  NULL
                  );

  return Status;
}


