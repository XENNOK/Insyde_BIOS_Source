/** @file
  Simple code for H2O eMMC 5.0 FFU

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <Guid/FileInfo.h>
#include <Guid/H2OSdhcFfuImageGuid.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

#include <Protocol/FirmwareManagement.h>
#include <Protocol/SimpleFileSystem.h>

CHAR16 mFileName[] = L"fw.bin";

/**
  Load the firmware named mFileName from Simple File System Protocol.

  @param[out] FileData        Firmware binary file
  @param[out] FileSize        File size

  @retval EFI_SUCCESS           Operation succeeded
  @retval Other                 Operation failed
  
**/
EFI_STATUS
LoadFwFileFromSimpleFileSystem (
  OUT   VOID                       **FileData,
  OUT   UINTN                      *FileSize
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *FileSystemHandles;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL       *SimpleFileSystem;
  EFI_FILE_PROTOCOL                     *Root;  
  EFI_FILE_HANDLE                       File;
  EFI_FILE_INFO                         *FileInfo;
  UINTN                                 FileInfoSize;
  UINTN                                 NumberFileSystemHandles;
  UINTN                                 Index;

  NumberFileSystemHandles = 0;
  FileSystemHandles       = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberFileSystemHandles,
                  &FileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Root     = NULL;
  File     = NULL;
  FileInfo = NULL;
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID **) &SimpleFileSystem
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    Status = SimpleFileSystem->OpenVolume (SimpleFileSystem, &Root);
    if (EFI_ERROR(Status)) {
      continue;
    }
  
    Status = Root->Open (
                     Root,
                     &File,
                     mFileName,
                     EFI_FILE_MODE_READ,
                     0
                    );
    if (EFI_ERROR(Status)) {
      Root->Close(Root);
      continue;
    } else {
      break;
    }
  }

  if (NumberFileSystemHandles != 0) {
    FreePool(FileSystemHandles);
  }
  
  if (EFI_ERROR(Status)) {
    return Status;
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
  Perform FFU process.

  @param[in] ImageHandle        EFI_HANDLE
  @param[in] SystemTable        EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Operation succeeded
  @retval Other                 Operation failed
  
**/
EFI_STATUS
EFIAPI
FfuDriverEntryPoint (
  IN    EFI_HANDLE              ImageHandle,
  IN    EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                                       Status;
  EFI_HANDLE                                       *HandleBuffer;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL                 *FirmwareManagement;
  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS    Progress;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                    *FmpImageInfoBuf;
  CHAR16                                           *AbortReason;
  CHAR16                                           *PackageVersionName;
  UINTN                                            DescriptorSize;
  UINTN                                            ImageInfoSize;
  UINT32                                           FmpImageInfoDescriptorVer;
  UINT32                                           PackageVersion;
  UINT32                                           ImageUpdatable;
  UINT32                                           VendorCode;
  UINT8                                            FmpImageInfoCount;
  VOID                                             *FileData;
  UINTN                                            FileSize;
  UINTN                                            HandleCount;
  UINTN                                            Index;
  BOOLEAN                                          Found;

  HandleCount  = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareManagementProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Found = FALSE;
  FirmwareManagement = NULL;
  for (Index = 0; Index < HandleCount && !Found; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiFirmwareManagementProtocolGuid, (VOID *) &FirmwareManagement);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    ImageInfoSize = 0;
    Status = FirmwareManagement->GetImageInfo (
                                   FirmwareManagement,
                                   &ImageInfoSize,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL
                                   );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }
    
    FmpImageInfoBuf = NULL;
    FmpImageInfoBuf = AllocateZeroPool (ImageInfoSize);
    if (FmpImageInfoBuf == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    PackageVersionName = NULL;
    Status = FirmwareManagement->GetImageInfo (
                                   FirmwareManagement,
                                   &ImageInfoSize,               // ImageInfoSize
                                   FmpImageInfoBuf,              // ImageInfo
                                   &FmpImageInfoDescriptorVer,   // DescriptorVersion
                                   &FmpImageInfoCount,           // DescriptorCount
                                   &DescriptorSize,              // DescriptorSize
                                   &PackageVersion,              // PackageVersion
                                   &PackageVersionName           // PackageVersionName
                                   );

    //
    // If FMP GetInformation interface failed, skip this resource
    //
    if (EFI_ERROR(Status)) {
      FreePool(FmpImageInfoBuf);
      continue;
    }

    if (PackageVersionName != NULL) {
      FreePool(PackageVersionName);
    }

    //
    // Find the instance of FMP that distributed by SDHCD
    //
    if (CompareGuid(&FmpImageInfoBuf->ImageTypeId, &gH2OSdhcFfuImageGuid)) {
      Found = TRUE;
    }

    FreePool(FmpImageInfoBuf);
  }

  if (!Found) {
    Status = EFI_NOT_FOUND;
    goto Exit;    
  }

  Status = LoadFwFileFromSimpleFileSystem ((VOID **) &FileData, &FileSize);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = FirmwareManagement->CheckImage ( 
                                 FirmwareManagement,
                                 1,
                                 FileData,
                                 FileSize,
                                 &ImageUpdatable
                                 );
  if (EFI_ERROR(Status) || ImageUpdatable != IMAGE_UPDATABLE_VALID) {
    goto Exit;
  }

  Progress    = 0;
  AbortReason = NULL;
  Status = FirmwareManagement->SetImage (
                                 FirmwareManagement,
                                 1,
                                 FileData,
                                 FileSize,
                                 &VendorCode,
                                 Progress,
                                 &AbortReason
                                 );
  if (AbortReason != NULL) {
    DEBUG ((EFI_D_ERROR, "%s\n", AbortReason));
    FreePool(AbortReason);
  }

Exit:
 
  if (HandleCount != 0) {
    FreePool(HandleBuffer);
  }

  return Status;
}

