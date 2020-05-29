/** @file
FAT Device Recovery PEIM

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.
--*/

#include "FatPeim.h"
#include <Guid/RecoveryFileAddress.h>
#include <Library/HobLib.h>
#include <Library/BaseCryptLib.h>
#include <Ppi/FirmwareAuthentication.h>
#include <Library/DebugLib.h>


#define PEI_FAT_RECOVERY_FILE (CHAR16 *)PcdGetPtr(PcdPeiRecoveryFile)

PEI_FAT_PRIVATE_DATA  *mPrivateData = NULL;


/**
 Get BIOS image information from recovery capsule

 @param [in]   Capsule          Recovery capsule buffer
 @param [in]   CapsuleSize      Recovery capsule file size
 @param [out]  BiosImageOffset  The offset of BIOS image from recovery capsule file buffer
 @param [out]  BiosImageSize    The size of the BIOS image


 @retval EFI_SUCCESS            BIOS image information is successfully retrieved
 @return others                 Failed to get BIOS image information from recovery capsule

**/
EFI_STATUS
GetBiosImageFromCapsule (
  IN UINT8    *Capsule, 
  IN UINTN    CapsuleSize,
  OUT UINTN   *BiosImageOffset,
  OUT UINTN   *BiosImageSize
  )
{
  UINTN                       Index;
  ISFLASH_DATA_REGION_HEADER  *DataRegion;

  if ( (Capsule         == NULL) ||
       (CapsuleSize     == 0)    ||
       (BiosImageOffset == NULL) ||
       (BiosImageSize   == NULL) ) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If Secure Flash is supported, the extra signature size needs to be ignored
  // if it is not signed by Insyde SecureFlash tool
  //
  if (!((Capsule[0] == (UINT8)'M') && (Capsule[1] == (UINT8)'Z'))) {
    if (FeaturePcdGet(PcdSecureFlashSupported)) {
      CapsuleSize -= SIGNATURE_SIZE;
    }
  }

  if (FeaturePcdGet(PcdRecoveryHobReportsEntireCapsule)) {
    *BiosImageOffset = 0;
    *BiosImageSize   = CapsuleSize;
    return EFI_SUCCESS;
  }


  if (!((Capsule[0] == (UINT8)'M') && (Capsule[1] == (UINT8)'Z'))) {
    *BiosImageOffset = 0;
    *BiosImageSize = CapsuleSize;
    return EFI_SUCCESS;
  }

  //
  // Capsule image is PE32 image
  // Search for BIOS image
  //
  for (Index = 0; Index < CapsuleSize - ISFLASH_TAG_SIZE; Index++) {
    if (CompareMem(Capsule + Index, ISFLASH_BIOS_IMAGE_TAG, ISFLASH_TAG_SIZE) == 0) {
      break;
    }
  }
  if (Index == CapsuleSize - ISFLASH_TAG_SIZE) {
    return EFI_NOT_FOUND;
  }

  //
  // Get BIOS image offset and size
  //
  DataRegion = (ISFLASH_DATA_REGION_HEADER *)(Capsule + Index);
  *BiosImageOffset = Index + sizeof(ISFLASH_DATA_REGION_HEADER);
  *BiosImageSize = DataRegion->DataSize;
  if (*BiosImageOffset + *BiosImageSize > CapsuleSize) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

/**
 Firmware verification with RSA2048-SHA256

 @param [in]   FirmwareFileData  Firmware file data buffer
 @param [in]   FirmwareFileSize  The firmware file size including signature

 @retval EFI_SUCCESS            The firmware verification is successful
 @retval EFI_OUT_OF_RESOURCES   Out of resources
 @retval EFI_SECURITY_VIOLATION  Failed to verify the firmware

**/
EFI_STATUS
VerifyFirmware (
  UINT8      *FirmwareFileData,
  UINTN      FirmwareFileSize
  )
{
  FIRMWARE_AUTHENTICATION_PPI   *FirmwareAuthPpi;
  EFI_STATUS                    Status;

  Status = PeiServicesLocatePpi (
             &gFirmwareAuthenticationPpiGuid,
             0,
             NULL,
             (VOID **)&FirmwareAuthPpi
             );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  return FirmwareAuthPpi->AuthenticateFirmware(FirmwareFileData, FirmwareFileSize);
}

/**
  Installs the Device Recovery Module PPI, Initialize BlockIo Ppi
  installation notification

  @param  FileHandle              Handle of the file being invoked. Type
                                  EFI_PEI_FILE_HANDLE is defined in
                                  FfsFindNextFile().
  @param  PeiServices             Describes the list of possible PEI Services.

  @retval EFI_SUCCESS             The entry point was executed successfully.
  @retval EFI_OUT_OF_RESOURCES    There is no enough memory to complete the
                                  operations.

**/
EFI_STATUS
EFIAPI
FatPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Address;
  PEI_FAT_PRIVATE_DATA  *PrivateData;
  PEI_FILE_ACCESS_PPI   *FileAccess;

  Status = (**PeiServices).AllocatePages (
                             PeiServices,
                             EfiBootServicesCode,
                             (sizeof (PEI_FAT_PRIVATE_DATA) - 1) / PEI_FAT_MEMMORY_PAGE_SIZE + 1,
                             &Address
                             );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gPeiFileAccessPpiGuid,
                             0,
                             NULL,
                             (VOID **) &FileAccess
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }    

  PrivateData = (PEI_FAT_PRIVATE_DATA *) (UINTN) Address;

  //
  // Initialize Private Data (to zero, as is required by subsequent operations)
  //
  (*PeiServices)->SetMem ((UINT8 *) PrivateData, sizeof (PEI_FAT_PRIVATE_DATA), 0);
  PrivateData->Signature            = PEI_FAT_PRIVATE_DATA_SIGNATURE;
  PrivateData->PeiServices          = (EFI_PEI_SERVICES **)PeiServices;
  PrivateData->PeiFileAccessPpi     = FileAccess;
  //
  // Installs Ppi
  //
  PrivateData->DeviceRecoveryPpi.GetNumberRecoveryCapsules  = GetNumberRecoveryCapsules;
  PrivateData->DeviceRecoveryPpi.GetRecoveryCapsuleInfo     = GetRecoveryCapsuleInfo;
  PrivateData->DeviceRecoveryPpi.LoadRecoveryCapsule        = LoadRecoveryCapsule;

  PrivateData->PpiDescriptor.Flags                          = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PrivateData->PpiDescriptor.Guid                           = &gEfiPeiDeviceRecoveryModulePpiGuid;
  PrivateData->PpiDescriptor.Ppi                            = &PrivateData->DeviceRecoveryPpi;

  Status = (**PeiServices).InstallPpi (PeiServices, &PrivateData->PpiDescriptor);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
 
  return EFI_SUCCESS;
}

/**
  Returns the number of DXE capsules residing on the device.

  This function searches for DXE capsules from the associated device and returns
  the number and maximum size in bytes of the capsules discovered. Entry 1 is
  assumed to be the highest load priority and entry N is assumed to be the lowest
  priority.

  @param[in]  PeiServices              General-purpose services that are available
                                       to every PEIM
  @param[in]  This                     Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                       instance.
  @param[out] NumberRecoveryCapsules   Pointer to a caller-allocated UINTN. On
                                       output, *NumberRecoveryCapsules contains
                                       the number of recovery capsule images
                                       available for retrieval from this PEIM
                                       instance.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  )
{
  EFI_STATUS            Status;
  PEI_FAT_PRIVATE_DATA  *PrivateData;
  PEI_FILE_HANDLE       Handle;
  PEI_FILE_ACCESS_PPI   *File;

  PrivateData               = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  PrivateData->PeiServices  = PeiServices;
  File                      = PrivateData->PeiFileAccessPpi;
    
  //
  // OpenFile should search each volume in the root directory for the Recovery capsule
  //
  Status = File->OpenFile (
                   File, 
                   PEI_FAT_RECOVERY_FILE, 
                   &Handle, 
                   EFI_FILE_MODE_READ,
                   EFI_FILE_ARCHIVE
                   );

  if (EFI_ERROR (Status)) {
    *NumberRecoveryCapsules = 0;
  } else {
    *NumberRecoveryCapsules = 1;
  }

  File->CloseFile (File, Handle);

  return EFI_SUCCESS;
}

/**
  Returns the size and type of the requested recovery capsule.

  This function gets the size and type of the capsule specified by CapsuleInstance.

  @param[in]  PeiServices       General-purpose services that are available to every PEIM
  @param[in]  This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                instance.
  @param[in]  CapsuleInstance   Specifies for which capsule instance to retrieve
                                the information.  This parameter must be between
                                one and the value returned by GetNumberRecoveryCapsules()
                                in NumberRecoveryCapsules.
  @param[out] Size              A pointer to a caller-allocated UINTN in which
                                the size of the requested recovery module is
                                returned.
  @param[out] CapsuleType       A pointer to a caller-allocated EFI_GUID in which
                                the type of the requested recovery capsule is
                                returned.  The semantic meaning of the value
                                returned is defined by the implementation.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI            *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  )
{
  EFI_STATUS                  Status;
  PEI_FAT_PRIVATE_DATA        *PrivateData;
  BOOLEAN                     Logical;
  UINTN                       BlockDeviceNo;
  UINTN                       ParentDevNo;
  
  UINTN                       BlockDeviceCount;
  PEI_FILE_HANDLE             Handle;
  EFI_PHYSICAL_ADDRESS        Address;
  UINTN                       BufferSize;
  EFI_PEI_BLOCK_DEVICE_TYPE   BlockDevType;
  PEI_FILE_ACCESS_PPI         *File;
  PEI_FILE_INFO               *FileInfo;

  PrivateData               = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  PrivateData->PeiServices  = PeiServices;
  File                      = PrivateData->PeiFileAccessPpi;
  BufferSize                = sizeof (PEI_FILE_INFO) + FAT_MAX_FILE_PATH_LENGTH * sizeof (CHAR16);

  Status = (**PeiServices).AllocatePages (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            EfiBootServicesCode,
                            (BufferSize - 1) / PEI_FAT_MEMMORY_PAGE_SIZE + 1,
                            &Address
                            );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  FileInfo = (PEI_FILE_INFO *) (UINTN) Address;

  //
  // Search each volume in the root directory for the Recovery capsule
  //
  Status = File->OpenFile (
                   File, 
                   PEI_FAT_RECOVERY_FILE, 
                   &Handle, 
                   EFI_FILE_MODE_READ,
                   EFI_FILE_ARCHIVE
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = File->GetFileInfo (File, Handle, &BufferSize, FileInfo);
  if (EFI_ERROR (Status)) {
    File->CloseFile (File, Handle);
    return Status;
  }

  *Size = (UINTN) FileInfo->FileSize;

  //
  // Find corresponding physical block device
  //  
  File->GetRecoveryBlockInfo (
          File, 
          Handle, 
          &Logical, 
          &BlockDeviceNo, 
          &ParentDevNo,
          &BlockDeviceCount
          );
  
  while (Logical && BlockDeviceNo < BlockDeviceCount) {
    BlockDeviceNo = ParentDevNo;
    File->GetRecoveryLogical(File, &BlockDeviceNo, &Logical);
  }
  //
  // Fill in the Capsule Type GUID according to the block device type
  //
  if (BlockDeviceNo < BlockDeviceCount) {
    File->GetRecoveryBlockType (File, BlockDeviceNo, &BlockDevType);

    switch ((UINTN)BlockDevType) {

    case IdeCDROM:
    case IdeLS120:
    case MaxDeviceType:
      CopyMem (
        (UINT8 *) CapsuleType,
        (UINT8 *) &gRecoveryOnFatIdeDiskGuid,
        sizeof (EFI_GUID)
        );
      break;

    case UsbMassStorage:
      CopyMem (
        (UINT8 *) CapsuleType,
        (UINT8 *) &gRecoveryOnFatUsbDiskGuid,
        sizeof (EFI_GUID)
        );
      break;
    }
  }

  File->CloseFile (File, Handle);
  return EFI_SUCCESS;
}

/**
  Loads a DXE capsule from some media into memory.

  This function, by whatever mechanism, retrieves a DXE capsule from some device
  and loads it into memory. Note that the published interface is device neutral.

  @param[in]     PeiServices       General-purpose services that are available
                                   to every PEIM
  @param[in]     This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                   instance.
  @param[in]     CapsuleInstance   Specifies which capsule instance to retrieve.
  @param[out]    Buffer            Specifies a caller-allocated buffer in which
                                   the requested recovery capsule will be returned.

  @retval EFI_SUCCESS        The capsule was loaded correctly.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A requested recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN OUT EFI_PEI_SERVICES                         **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI           *This,
  IN UINTN                                        CapsuleInstance,
  OUT VOID                                        *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_FAT_PRIVATE_DATA  *PrivateData;
  PEI_FILE_HANDLE       Handle;
  EFI_PHYSICAL_ADDRESS  Address;
  UINTN                 BufferSize;
  PEI_FILE_INFO         *FileInfo;
  UINTN                             FileSize;
  UINTN                             BiosImageOffset;
  UINTN                             BiosImageSize;
  EFI_HOB_MEMORY_ALLOCATION_MODULE  *MemoryHob;
  PEI_FILE_ACCESS_PPI               *File;

  PrivateData               = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  PrivateData->PeiServices  = PeiServices;
  File                      = PrivateData->PeiFileAccessPpi;

  BufferSize                = sizeof (PEI_FILE_INFO) + FAT_MAX_FILE_PATH_LENGTH * sizeof (CHAR16);
  Status = (**PeiServices).AllocatePages (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            EfiBootServicesCode,
                            (BufferSize - 1) / PEI_FAT_MEMMORY_PAGE_SIZE + 1,
                            &Address
                            );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  FileInfo = (PEI_FILE_INFO *) (UINTN) Address;
  //
  // Search each volume in the root directory for the Recovery capsule
  //
  Status = File->OpenFile (
                   File, 
                   PEI_FAT_RECOVERY_FILE, 
                   &Handle, 
                   EFI_FILE_MODE_READ,
                   EFI_FILE_ARCHIVE
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = File->GetFileInfo (File, Handle, &BufferSize, FileInfo);
  if (EFI_ERROR (Status)) {
    File->CloseFile (File, Handle);
    return Status;
  }

  FileSize = (UINTN) FileInfo->FileSize;

  Status = File->ReadFile (
             File,
             Handle,
             ReadData,
             &FileSize,
             Buffer
             );
  ASSERT_EFI_ERROR (Status);
  if (Status == EFI_SUCCESS) {
    Status = VerifyFirmware(Buffer, FileSize);
  }
  
  ASSERT_EFI_ERROR (Status);
  if (Status == EFI_SUCCESS) {
    Status = GetBiosImageFromCapsule (Buffer, FileSize, &BiosImageOffset, &BiosImageSize);

    if (Status == EFI_SUCCESS) {
      //
      //Create a Hob transfer the .fd memory base address and ROM part PEI start address to BDS
      //
      Status = (*PeiServices)->CreateHob (
                                 (CONST EFI_PEI_SERVICES **)PeiServices,
                                 EFI_HOB_TYPE_MEMORY_ALLOCATION,
                                 (UINT16) (sizeof (EFI_HOB_MEMORY_ALLOCATION_MODULE)),
                                 (VOID **)&MemoryHob
                                 );
      if (Status == EFI_SUCCESS) {
        MemoryHob->MemoryAllocationHeader.Name = gEfiRecoveryFileAddressGuid;
        MemoryHob->MemoryAllocationHeader.MemoryBaseAddress = (UINTN)Buffer + BiosImageOffset;
        MemoryHob->MemoryAllocationHeader.MemoryLength = BiosImageSize;
      }
    }
  }
  
  Status = File->CloseFile (File, Handle);

  return Status;
}

