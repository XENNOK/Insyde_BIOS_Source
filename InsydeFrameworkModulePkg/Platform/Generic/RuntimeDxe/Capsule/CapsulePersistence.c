//; 
//; Module Name:
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
//;   CapsulePersistence.c
//; 
//; Abstract:
//; 
//;   Capsule persistence across reset
//; 

#include "CapsuleService.h"
#include EFI_GUID_DEFINITION(Capsule)                                 
#include EFI_GUID_DEFINITION(EfiSystemResourceTable)
#include EFI_GUID_DEFINITION(GlobalVariable)
#include EFI_PROTOCOL_DEFINITION(DevicePath)
#include EFI_PROTOCOL_DEFINITION(SimpleFileSystem)
#include "EfiPrintLib.h"

#define MAX_BOOT_OPTION_DATA_SIZE             2048
#define MAX_STRING_LENGTH                     128
#define CAPSULE_IMAGE_FOLDER                  L"\\EFI\\Insyde"

UINT8                     mBootOptionData[MAX_BOOT_OPTION_DATA_SIZE];

STATIC
UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:
  Get the device path size

Arguments:
  DevicePath   - The device path to be calculated

Returns:
  The size in bytes of the device path

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }
  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!EfiIsDevicePathEnd (DevicePath)) {
    DevicePath = EfiNextDevicePathNode (DevicePath);
  }
  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}

STATIC
EFI_DEVICE_PATH_PROTOCOL *
AppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
/*++

Routine Description:
  Function is used to append a Src1 and Src2 together.

Arguments:
  Src1  - A pointer to a device path data structure.

  Src2  - A pointer to a device path data structure.

Returns:

  A pointer to the new device path is returned.
  NULL is returned if space for the new device path could not be allocated from pool.
  It is up to the caller to free the memory used by Src1 and Src2 if they are no longer needed.

--*/
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINTN                     Size1;
  UINTN                     Size2;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *SecondDevicePath;

  //
  // Allocate space for the combined device path. It only has one end node of
  // length EFI_DEVICE_PATH_PROTOCOL
  //
  Size1 = DevicePathSize (Src1);
  Size2 = DevicePathSize (Src2);
  Size  = Size1 + Size2;

  if (Size1 != 0 && Size2 != 0) {
    Size -= sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, Size, (VOID **) &NewDevicePath);

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  EfiCopyMem (NewDevicePath, Src1, Size1);

  //
  // Over write Src1 EndNode and do the copy
  //
  if (Size1 != 0) {
    SecondDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) ((CHAR8 *) NewDevicePath + (Size1 - sizeof (EFI_DEVICE_PATH_PROTOCOL)));
  } else {
    SecondDevicePath = NewDevicePath;

  }

  EfiCopyMem (SecondDevicePath, Src2, Size2);

  return NewDevicePath;
}

EFI_STATUS
CapsuleGuidToCapsuleImagePath (
  IN     EFI_GUID       *CapsuleGuid,
  IN OUT CHAR16         *CapsuleImagePath
  )
/*++

Routine Description:
  Convert the capsule GUID to the persistant file path unicode string
  The capsule file name is in the format of
  XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.bin

Arguments:
  CapsuleGuid       - Pointer to the Capsule GUID
  CapsuleImagePath  - The unicode string file path of the Capsule file

Returns:
  EFI_SUCCESS            - The capsule file name is converted successfully
  EFI_INVALID_PARAMETER  - Input parameters are NULL

--*/
{
  if (CapsuleGuid == NULL || CapsuleImagePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  SPrint ( CapsuleImagePath,
                  MAX_STRING_LENGTH,
                  L"%s\\%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x.bin",
                  CAPSULE_IMAGE_FOLDER,
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


EFI_STATUS
EFIAPI
GetBootFilePathDevicePathOffset (
  IN EFI_DEVICE_PATH_PROTOCOL     *BootDevicePath,
  OUT UINTN                       *BootFilePathDevicePathOffset
  )
/*++

Routine Description:
  Get the offset of the boot loader file path from system partition for the boot
  device path of the current boot option

Arguments:
  BootDevicePath     - The device path of the boot option
  BootFilePathOffset - The offset from the boot device path of the boot loader file path

Returns:
  EFI_SUCCESS        - The BootFilePathOffset is correctly set
  Others             - Unable to get boot file path offset

--*/
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

EFI_DEVICE_PATH_PROTOCOL *
GetBootOptionDevicePath (
  UINT16     BootIndex
  )
/*++

Routine Description:
  Get the device path of the specified boot option
  
Arguments:
  BootIndex          - The index of the boot option
  BootFilePathOffset - The offset from the boot device path of the boot loader file path

Returns:
  Pointer to the device path of the specified boot option

--*/
{
  EFI_STATUS                Status;
  CHAR16                    BootOption[] = L"Boot0000";
  CHAR16                    *BootOptionDesc;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *BootDevicePath;

  BootDevicePath = NULL;

  SPrint (BootOption, sizeof(BootOption), L"Boot%04x", BootIndex);
  Size = MAX_BOOT_OPTION_DATA_SIZE;
  Status = EfiGetVariable (
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
  BootDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)(mBootOptionData + sizeof(UINT32) + sizeof(UINT16) + EfiStrSize(BootOptionDesc));

  return BootDevicePath;
}

EFI_DEVICE_PATH_PROTOCOL *
GetCurrentBootDevicePath (
  VOID
  )
/*++

Routine Description:
  Get the boot device path of the current boot device
  
Arguments:
  None
  
Returns:
  Pointer to he boot device path of the current boot device

--*/
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
  Status = EfiGetVariable (
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
  EFI_DEVICE_PATH_PROTOCOL   **SystemRootDevicePath,
  EFI_FILE_HANDLE            *SysDir
  )
/*++

Routine Description:
  Get system root directory information
  
Arguments:
  SystemRootDevicePath  - The device path of the system root directory
  SysDir                - Pointer to the EFI_FILE_HANDLE of the system root directory
  
Returns:
  EFI_SUCCESS           - System root information is successfully retrieved
  EFI_INVALID_PARAMETER - NULL input parameters given
  EFI_NOT_FOUND         - Failed to find system root directory 

--*/
{
  EFI_DEVICE_PATH_PROTOCOL        *BootDevicePath;
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
    CompareSize = DevicePathSize (BootDevicePath) - sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &NumberOfHandles, &HandleBuffer);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Found = FALSE;
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiDevicePathProtocolGuid, SystemRootDevicePath);
    if (EFI_ERROR(Status)) {
      continue;
    }
    if (EfiCompareMem (
          ((UINT8 *)*SystemRootDevicePath) + DevicePathSize (*SystemRootDevicePath) - CompareSize - sizeof(EFI_DEVICE_PATH_PROTOCOL),
          BootDevicePath,
          CompareSize) == 0) {
      Found = TRUE;
      break;
    }
  }

  if (Found) {
    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiSimpleFileSystemProtocolGuid, &SimpleFileSystem);
    if (!EFI_ERROR(Status)) {
      Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, SysDir);
    }
  } else {
    Status = EFI_NOT_FOUND;
  }

  gBS->FreePool (HandleBuffer);

  return Status;
}

EFI_STATUS
EFIAPI
PersistCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Capsule persistence across reset
  
Arguments:
  CapsuleHeader       - Pointer to the Capsule header of the Capsule to be persisted,
                        the Capsule header must be followed by Capsule image data
                          
Returns:
  EFI_SUCCESS           - Functions successfully returned
  Others                - Failed to store the capsule image

--*/
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
                          CAPSULE_IMAGE_FOLDER,
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

  CapsuleFilePathDevicePathSize = sizeof (EFI_DEVICE_PATH_PROTOCOL) + EfiStrSize (CapsuleImagePath);   
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  CapsuleFilePathDevicePathSize + sizeof(EFI_DEVICE_PATH_PROTOCOL),
                  (VOID **)&CapsuleFilePathDevicePath
                  );
  ASSERT_EFI_ERROR(Status);                
  
  CapsuleFilePathDevicePath->Header.Type = MEDIA_DEVICE_PATH;
  CapsuleFilePathDevicePath->Header.SubType = MEDIA_FILEPATH_DP;
  *(UINT16 *)(CapsuleFilePathDevicePath->Header.Length) = (UINT16)(sizeof (EFI_DEVICE_PATH_PROTOCOL) + EfiStrSize (CapsuleImagePath));
  EfiCopyMem (CapsuleFilePathDevicePath->PathName, CapsuleImagePath, EfiStrSize (CapsuleImagePath));
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[0] = 0x7F;
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[1] = 0xFF;
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[2] = 0x04;
  ((UINT8 *)CapsuleFilePathDevicePath + CapsuleFilePathDevicePathSize)[3] = 0x00;

  CapsuleDevicePath = AppendDevicePath (SystemRootDevicePath, (EFI_DEVICE_PATH_PROTOCOL *)CapsuleFilePathDevicePath);
  gBS->FreePool (CapsuleFilePathDevicePath);

  if (CapsuleDevicePath != NULL) {
    if (EfiCompareGuid(&CapsuleHeader->CapsuleGuid, &gWindowsUxCapsuleGuid)) {
      Status = EfiSetVariable (
                      L"WindowsUxCapsuleDevicePath",
                      &gWindowsUxCapsuleGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      DevicePathSize (CapsuleDevicePath),
                      CapsuleDevicePath
                      );
    } else {
      Status = EfiSetVariable (
                      L"CapsuleDevicePath",
                      &CapsuleHeader->CapsuleGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      DevicePathSize (CapsuleDevicePath),
                      CapsuleDevicePath
                      );

    }
    gBS->FreePool (CapsuleDevicePath);
  } else {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}

