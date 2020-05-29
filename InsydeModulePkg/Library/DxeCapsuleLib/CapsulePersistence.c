/** @file
  Capsule persistence across reset

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
#include <Guid/Capsule.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/CapsuleLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/EfiSystemResourceTable.h>


#define MAX_BOOT_OPTION_DATA_SIZE             2048
#define MAX_STRING_LENGTH                     128


UINT8                     mBootOptionData[MAX_BOOT_OPTION_DATA_SIZE];
/**
  Convert the capsule GUID to the persistant file path unicode string
  The capsule file name is in the format of
  XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.bin

  @param[in]      CapsuleGuid           Pointer to the Capsule GUID
  @param[in, out] CapsuleImagePath      The unicode string file path of the Capsule file

  @retval         EFI_SUCCESS           The capsule file name is converted successfully
  @retval         EFI_INVALID_PARAMETER Input parameters are NULL

**/
EFI_STATUS
CapsuleGuidToCapsuleImagePath (
  EFI_GUID       *CapsuleGuid,
  CHAR16         *CapsuleImagePath
  )
{
  if (CapsuleGuid == NULL || CapsuleImagePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  UnicodeSPrint ( CapsuleImagePath,
                  MAX_STRING_LENGTH,
                  L"%s\\%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x.bin",
                  PcdGetPtr(PcdCapsuleImageFolder),
                  (UINTN)CapsuleGuid->Data1,
                  (UINTN)CapsuleGuid->Data2,
                  (UINTN)CapsuleGuid->Data3,
                  (UINTN)CapsuleGuid->Data4[0],
                  (UINTN)CapsuleGuid->Data4[1],
                  (UINTN)CapsuleGuid->Data4[2],
                  (UINTN)CapsuleGuid->Data4[3],
                  (UINTN)CapsuleGuid->Data4[4],
                  (UINTN)CapsuleGuid->Data4[5],
                  (UINTN)CapsuleGuid->Data4[6],
                  (UINTN)CapsuleGuid->Data4[7]
                 );
                 
  return EFI_SUCCESS;
}


/**
  Get the offset of the boot loader file path from system partition for the boot
  device path of the current boot option

  @param[in] BootDevicePath      The device path of the boot option
  @param[out] BootFilePathOffset The offset from the boot device path of the boot
                                 loader file path

  @retval EFI_SUCCESS            The BootFilePathOffset is correctly set
  @return others                 Unable to get boot file path offset

**/
EFI_STATUS
EFIAPI
GetBootFilePathDevicePathOffset (
  IN EFI_DEVICE_PATH_PROTOCOL     *BootDevicePath,
  OUT UINTN                       *BootFilePathDevicePathOffset
  )
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  EFI_STATUS                 Status;

  if (BootDevicePath == NULL || BootFilePathDevicePathOffset == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;

  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)BootDevicePath;
  while (!IsDevicePathEnd(DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MEDIA_FILEPATH_DP)) {
      *BootFilePathDevicePathOffset = (UINTN)((UINT8 *)DevicePath - (UINT8 *)BootDevicePath);
      Status = EFI_SUCCESS;
      break;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  return Status;
}

/**
  Get the device path of the specified boot option

  @param[in] BootIndex       The index of the boot option

  @return    Pointer to the device path of the specified boot option

**/
EFI_DEVICE_PATH_PROTOCOL *
GetBootOptionDevicePath (
  UINT16     BootIndex
  )
{
  EFI_STATUS                Status;
  CHAR16                    BootOption[] = L"Boot0000";
  CHAR16                    *BootOptionDesc;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *BootDevicePath;

  BootDevicePath = NULL;

  UnicodeSPrint (BootOption, sizeof(BootOption), L"Boot%04x", BootIndex);
  Size = MAX_BOOT_OPTION_DATA_SIZE;
  Status = gRT->GetVariable (
             BootOption,
             &gEfiGlobalVariableGuid,
             NULL,
             &Size,
             mBootOptionData
             );
  if (Status != EFI_SUCCESS) {
    return NULL;
  }

  //
  // Get the boot loader file path from the current Boot Option data
  //
  BootOptionDesc = (CHAR16 *)(mBootOptionData + sizeof(UINT32) + sizeof(UINT16));
  BootDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)(mBootOptionData + sizeof(UINT32) + sizeof(UINT16) + StrSize(BootOptionDesc));

  return BootDevicePath;
}

/**
  Get the boot device path of the current boot device
  
  @param     None

  @return    Pointer to he boot device path of the current boot device

**/
EFI_DEVICE_PATH_PROTOCOL *
GetCurrentBootDevicePath (
  VOID
  )
{
  EFI_STATUS                Status;
  UINT16                    BootCurrent;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *BootDevicePath;

  BootDevicePath = NULL;
  //
  // Get the current Boot Option
  //
  Size = sizeof(UINT16);
  Status = gRT->GetVariable (
             L"BootCurrent",
             &gEfiGlobalVariableGuid,
             NULL,
             &Size,
             &BootCurrent
             );

  if (EFI_ERROR(Status)) {
    return NULL;
  }
  return GetBootOptionDevicePath(BootCurrent);
}



EFI_STATUS
GetSystemRootInfo (
  EFI_DEVICE_PATH            **SystemRootDevicePath,
  EFI_FILE_HANDLE            *SysDir
  )
{
  EFI_DEVICE_PATH                 *BootDevicePath;
  UINTN                           BootFilePathDevicePathOffset;
  UINTN                           CompareSize;
  BOOLEAN                         Found;
  EFI_STATUS                      Status;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           NumberOfHandles;
  UINTN                           Index;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;

  if (SystemRootDevicePath == NULL || SysDir == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BootDevicePath = GetCurrentBootDevicePath();
  Status = GetBootFilePathDevicePathOffset (BootDevicePath, &BootFilePathDevicePathOffset);
  if (Status == EFI_SUCCESS) {
    CompareSize = BootFilePathDevicePathOffset;
  } else {
    CompareSize = GetDevicePathSize (BootDevicePath) - sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }
  Status = gBS->LocateHandleBuffer(
                  ByProtocol, 
                  &gEfiSimpleFileSystemProtocolGuid, 
                  NULL, 
                  &NumberOfHandles, 
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Found = FALSE;
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index], 
                    &gEfiDevicePathProtocolGuid, 
                    (VOID **)SystemRootDevicePath);
    if (EFI_ERROR(Status)) {
      continue;
    }
    if (CompareMem (
          ((UINT8 *)*SystemRootDevicePath) + GetDevicePathSize (*SystemRootDevicePath) - CompareSize - sizeof(EFI_DEVICE_PATH),
          BootDevicePath,
          CompareSize) == 0) {
      Found = TRUE;
      break;
    }
  }

  if (Found) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index], 
                    &gEfiSimpleFileSystemProtocolGuid, 
                    (VOID **)&SimpleFileSystem
                    );
    if (!EFI_ERROR(Status)) {
      Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, SysDir);
    }
  } else {
    Status = EFI_NOT_FOUND;
  }

  FreePool (HandleBuffer);

  return Status;
}

/**
  Capsule persistence across reset

  @param[in] CapsuleHeader       Pointer to the Capsule header of the Capsule to
                                 be persisted, the Capsule header must be followed
                                 by Capsule image data

  @retval EFI_SUCCESS            The Capsule is successfully saved
  @return others                 Failed to persist the Capsule image

**/
EFI_STATUS
EFIAPI
PersistCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *SystemRootDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *CapsuleDevicePath;
  FILEPATH_DEVICE_PATH      *CapsuleFilePathDevicePath;
  UINTN                     CapsuleFilePathDevicePathSize;
  EFI_FILE_HANDLE           SysDir;
  EFI_FILE_HANDLE           File;
  CHAR16                    CapsuleImagePath[MAX_STRING_LENGTH];
  UINTN                     Size;


  Status = GetSystemRootInfo (&SystemRootDevicePath, &SysDir);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Make sure the Capsule Image directory is created
  //
  Status = SysDir->Open ( SysDir,
                          &File,
                          PcdGetPtr(PcdCapsuleImageFolder),
                          EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                          EFI_FILE_ARCHIVE | EFI_FILE_DIRECTORY
                         );
  if (EFI_ERROR(Status)) {
    SysDir->Close(SysDir);
    return Status;
  }
  File->Close(File);


  
  //
  // Save Capsule Image, the Capsule image file are named with Capsule GUID
  //
  CapsuleGuidToCapsuleImagePath (&CapsuleHeader->CapsuleGuid, CapsuleImagePath);

  //
  // If Capsule image file exists, delete it first
  //
  Status = SysDir->Open (
                  SysDir,
                  &File,
                  CapsuleImagePath,
                  EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE,
                  0
                  );
  if (Status == EFI_SUCCESS) {
    Status = File->Delete(File);
    ASSERT_EFI_ERROR(Status);
  }
  Status = SysDir->Open ( SysDir,
                          &File,
                          CapsuleImagePath,
                          EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                          EFI_FILE_ARCHIVE
                         );
  if (EFI_ERROR(Status)) {
    SysDir->Close(SysDir);
    return Status;
  }
  Size = CapsuleHeader->CapsuleImageSize - CapsuleHeader->HeaderSize;
  Status = File->Write (File, &Size, (VOID *)((UINT8*)CapsuleHeader + CapsuleHeader->HeaderSize));
  
  File->Close(File);
  SysDir->Close(SysDir);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  CapsuleFilePathDevicePathSize = sizeof (EFI_DEVICE_PATH_PROTOCOL) + StrSize (CapsuleImagePath);
  CapsuleFilePathDevicePath = (FILEPATH_DEVICE_PATH *)AllocatePool (CapsuleFilePathDevicePathSize + sizeof(EFI_DEVICE_PATH_PROTOCOL));
  if (CapsuleFilePathDevicePath == NULL) {
    ASSERT (CapsuleFilePathDevicePath != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  CapsuleFilePathDevicePath->Header.Type = MEDIA_DEVICE_PATH;
  CapsuleFilePathDevicePath->Header.SubType = MEDIA_FILEPATH_DP;
  *(UINT16 *)(CapsuleFilePathDevicePath->Header.Length) = (UINT16)(sizeof (EFI_DEVICE_PATH_PROTOCOL) + StrSize (CapsuleImagePath));
  CopyMem (CapsuleFilePathDevicePath->PathName, CapsuleImagePath, StrSize (CapsuleImagePath));
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[0] = 0x7F;
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[1] = 0xFF;
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[2] = 0x04;
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[3] = 0x00;

  CapsuleDevicePath = AppendDevicePath (SystemRootDevicePath, (EFI_DEVICE_PATH *)CapsuleFilePathDevicePath);
  FreePool (CapsuleFilePathDevicePath);

  if (CapsuleDevicePath != NULL) {
    if (CompareGuid(&CapsuleHeader->CapsuleGuid, &gWindowsUxCapsuleGuid)) {
      Status = gRT->SetVariable (
                      L"WindowsUxCapsuleDevicePath",
                      &gWindowsUxCapsuleGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      GetDevicePathSize (CapsuleDevicePath),
                      CapsuleDevicePath
                      );
    } else {
      Status = gRT->SetVariable (
                      L"CapsuleDevicePath",
                      &CapsuleHeader->CapsuleGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      GetDevicePathSize (CapsuleDevicePath),
                      CapsuleDevicePath
                      );

    }
    FreePool (CapsuleDevicePath);
  } else {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}

