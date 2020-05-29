/** @file
  Source file for CD recovery PEIM

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
/*
Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiCdExpress.h"

PEI_CD_EXPRESS_PRIVATE_DATA *mPrivateData = NULL;

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

  @param  FileHandle            The file handle of the image.
  @param  PeiServices           General purpose services available to every PEIM.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES  There is not enough system memory.

**/
EFI_STATUS
EFIAPI
CdExpressPeimEntry (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  PEI_CD_EXPRESS_PRIVATE_DATA *PrivateData;

  if (!EFI_ERROR (PeiServicesRegisterForShadow (FileHandle))) {
    return EFI_SUCCESS;
  }

  PrivateData = AllocatePages (EFI_SIZE_TO_PAGES (sizeof (*PrivateData)));
  if (PrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize Private Data (to zero, as is required by subsequent operations)
  //
  ZeroMem (PrivateData, sizeof (*PrivateData));
  PrivateData->Signature    = PEI_CD_EXPRESS_PRIVATE_DATA_SIGNATURE;
  PrivateData->PeiServices  = (EFI_PEI_SERVICES**)PeiServices;

  PrivateData->BlockBuffer  = AllocatePages (EFI_SIZE_TO_PAGES (PEI_CD_BLOCK_SIZE));
  if (PrivateData->BlockBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData->CapsuleCount = 0;
  Status                    = UpdateBlocksAndVolumes (PrivateData);

  //
  // Installs Ppi
  //
  PrivateData->DeviceRecoveryPpi.GetNumberRecoveryCapsules  = GetNumberRecoveryCapsules;
  PrivateData->DeviceRecoveryPpi.GetRecoveryCapsuleInfo     = GetRecoveryCapsuleInfo;
  PrivateData->DeviceRecoveryPpi.LoadRecoveryCapsule        = LoadRecoveryCapsule;

  PrivateData->PpiDescriptor.Flags                          = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PrivateData->PpiDescriptor.Guid = &gEfiPeiDeviceRecoveryModulePpiGuid;
  PrivateData->PpiDescriptor.Ppi = &PrivateData->DeviceRecoveryPpi;

  Status = PeiServicesInstallPpi (&PrivateData->PpiDescriptor);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // PrivateData is allocated now, set it to the module variable
  //
  mPrivateData = PrivateData;

  //
  // Installs Block Io Ppi notification function
  //
  PrivateData->NotifyDescriptor.Flags =
    (
      EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST
    );
  PrivateData->NotifyDescriptor.Guid    = &gEfiPeiVirtualBlockIoPpiGuid;
  PrivateData->NotifyDescriptor.Notify  = BlockIoNotifyEntry;
  return PeiServicesNotifyPpi (&PrivateData->NotifyDescriptor);

}

/**
  BlockIo installation notification function. 
  
  This function finds out all the current Block IO PPIs in the system and add them
  into private data.

  @param  PeiServices            Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor       Address of the notification descriptor data structure.
  @param  Ppi                    Address of the PPI that was installed.

  @retval EFI_SUCCESS            The function completes successfully.

**/
EFI_STATUS
EFIAPI
BlockIoNotifyEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  UpdateBlocksAndVolumes (mPrivateData);

  return EFI_SUCCESS;
}

/**
  Finds out all the current Block IO PPIs in the system and add them into private data.

  @param PrivateData                    The private data structure that contains recovery module information.

  @retval EFI_SUCCESS                   The blocks and volumes are updated successfully.

**/
EFI_STATUS
UpdateBlocksAndVolumes (
  IN OUT PEI_CD_EXPRESS_PRIVATE_DATA     *PrivateData
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_PPI_DESCRIPTOR          *TempPpiDescriptor;
  UINTN                           BlockIoPpiInstance;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *BlockIoPpi;
  UINTN                           NumberBlockDevices;
  UINTN                           IndexBlockDevice;
  EFI_PEI_BLOCK_IO_MEDIA          Media;
  EFI_PEI_SERVICES                **PeiServices;

  IndexBlockDevice = 0;
  //
  // Find out all Block Io Ppi instances within the system
  // Assuming all device Block Io Peims are dispatched already
  //
  for (BlockIoPpiInstance = 0; BlockIoPpiInstance < PEI_CD_EXPRESS_MAX_BLOCK_IO_PPI; BlockIoPpiInstance++) {
    Status = PeiServicesLocatePpi (
                              &gEfiPeiVirtualBlockIoPpiGuid,
                              BlockIoPpiInstance,
                              &TempPpiDescriptor,
                              (VOID **) &BlockIoPpi
                              );
    if (EFI_ERROR (Status)) {
      //
      // Done with all Block Io Ppis
      //
      break;
    }

    PeiServices = (EFI_PEI_SERVICES  **) GetPeiServicesTablePointer ();
    Status = BlockIoPpi->GetNumberOfBlockDevices (
                          PeiServices,
                          BlockIoPpi,
                          &NumberBlockDevices
                          );
    if (EFI_ERROR (Status) || (NumberBlockDevices == 0)) {
      continue;
    }
    //
    // Just retrieve the first block, should emulate all blocks.
    //
    for (IndexBlockDevice = 0; IndexBlockDevice < NumberBlockDevices && PrivateData->CapsuleCount < PEI_CD_EXPRESS_MAX_CAPSULE_NUMBER; IndexBlockDevice ++) {
      Status = BlockIoPpi->GetBlockDeviceMediaInfo (
                            PeiServices,
                            BlockIoPpi,
                            IndexBlockDevice,
                            &Media
                            );
      if (EFI_ERROR (Status) ||
          !Media.MediaPresent ||
           ((Media.DeviceType != IdeCDROM) && (Media.DeviceType != UsbMassStorage)) ||
          (Media.BlockSize != PEI_CD_BLOCK_SIZE)
          ) {
        continue;
      }

      DEBUG ((EFI_D_INFO, "PeiCdExpress DeviceType is   %d\n", Media.DeviceType));
      DEBUG ((EFI_D_INFO, "PeiCdExpress MediaPresent is %d\n", Media.MediaPresent));
      DEBUG ((EFI_D_INFO, "PeiCdExpress BlockSize is  0x%x\n", Media.BlockSize));
      DEBUG ((EFI_D_INFO, "PeiCdExpress Status is %d\n", Status));

      DEBUG ((EFI_D_INFO, "IndexBlockDevice is %d\n", IndexBlockDevice));
      PrivateData->CapsuleData[PrivateData->CapsuleCount].IndexBlock = IndexBlockDevice;  
      PrivateData->CapsuleData[PrivateData->CapsuleCount].BlockIo    = BlockIoPpi;
      Status = FindRecoveryCapsules (PrivateData);
      DEBUG ((EFI_D_INFO, "Status is %d\n", Status));

      if (EFI_ERROR (Status)) {
        continue;
      }
  
      PrivateData->CapsuleCount++;
    }

  }

  return EFI_SUCCESS;
}

/**
  Finds out the recovery capsule in the current volume.

  @param PrivateData                    The private data structure that contains recovery module information.

  @retval EFI_SUCCESS                   The recovery capsule is successfully found in the volume.
  @retval EFI_NOT_FOUND                 The recovery capsule is not found in the volume.

**/
EFI_STATUS
EFIAPI
FindRecoveryCapsules (
  IN OUT PEI_CD_EXPRESS_PRIVATE_DATA            *PrivateData
  )
{
  EFI_STATUS                      Status;
  UINTN                           Lba;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *BlockIoPpi;
  UINTN                           BufferSize;
  UINT8                           *Buffer;
  UINT8                           Type;
  UINT8                           *StandardID;
  UINT32                          RootDirLBA;
  PEI_CD_EXPRESS_DIR_FILE_RECORD  *RoorDirRecord;
  UINTN                           VolumeSpaceSize;
  BOOLEAN                         StartOfVolume;
  UINTN                           OriginalLBA;
  UINTN                           IndexBlockDevice;  
  UINT8                           *VolumeID;

  Buffer      = PrivateData->BlockBuffer;
  BufferSize  = PEI_CD_BLOCK_SIZE;

  Lba         = 16;
  //
  // The volume descriptor starts on Lba 16
  //
  IndexBlockDevice = PrivateData->CapsuleData[PrivateData->CapsuleCount].IndexBlock;
  BlockIoPpi       = PrivateData->CapsuleData[PrivateData->CapsuleCount].BlockIo;

  VolumeSpaceSize = 0;
  StartOfVolume   = TRUE;
  OriginalLBA     = 16;
  VolumeID        = NULL;

  while (TRUE) {
    SetMem (Buffer, BufferSize, 0);
    Status = BlockIoPpi->ReadBlocks (
                          PrivateData->PeiServices,
                          BlockIoPpi,
                          IndexBlockDevice,
                          Lba,
                          BufferSize,
                          Buffer
                          );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    StandardID = (UINT8 *) (Buffer + PEI_CD_EXPRESS_STANDARD_ID_OFFSET);
    if (!StringCmp (StandardID, (UINT8 *) PEI_CD_STANDARD_ID, PEI_CD_EXPRESS_STANDARD_ID_SIZE, TRUE)) {
      break;
    }

    if (StartOfVolume) {
      OriginalLBA   = Lba;
      StartOfVolume = FALSE;
    }

    Type = *(UINT8 *) (Buffer + PEI_CD_EXPRESS_VOLUME_TYPE_OFFSET);
    if (Type == PEI_CD_EXPRESS_VOLUME_TYPE_TERMINATOR) {
      if (VolumeSpaceSize == 0) {
        break;
      } else {
        Lba             = (OriginalLBA + VolumeSpaceSize);
        VolumeSpaceSize = 0;
        StartOfVolume   = TRUE;
        continue;
      }
    }

    if (Type != PEI_CD_EXPRESS_VOLUME_TYPE_PRIMARY && Type != PEI_CD_EXPRESS_VOLUME_TYPE_SECONDARY) {
      Lba++;
      continue;
    }

    VolumeSpaceSize = *(UINT32 *) (Buffer + PEI_CD_EXPRESS_VOLUME_SPACE_OFFSET);
    VolumeID        = Buffer + PEI_CD_EXPRESS_VOLUME_ID_OFFSET;
    RoorDirRecord   = (PEI_CD_EXPRESS_DIR_FILE_RECORD *) (Buffer + PEI_CD_EXPRESS_ROOT_DIR_RECORD_OFFSET);
    RootDirLBA      = RoorDirRecord->LocationOfExtent[0];
    Status          = EFI_NOT_FOUND;
    if (VolumeLabelMatched(VolumeID)){
    Status          = RetrieveCapsuleFileFromRoot (PrivateData, BlockIoPpi, IndexBlockDevice, RootDirLBA);
    }
    if (!EFI_ERROR (Status)) {
      //
      // Just look for the first primary descriptor
      //
      return EFI_SUCCESS;
    }

    Lba++;
  }

  return EFI_NOT_FOUND;
}

/**
  Retrieves the recovery capsule in root directory of the current volume.

  @param PrivateData                    The private data structure that contains recovery module information.
  @param BlockIoPpi                     The Block IO PPI used to access the volume.
  @param IndexBlockDevice               The index of current block device.
  @param Lba                            The starting logic block address to retrieve capsule.

  @retval EFI_SUCCESS                   The recovery capsule is successfully found in the volume.
  @retval EFI_NOT_FOUND                 The recovery capsule is not found in the volume.
  @retval Others                        

**/
EFI_STATUS
EFIAPI
RetrieveCapsuleFileFromRoot (
  IN OUT PEI_CD_EXPRESS_PRIVATE_DATA        *PrivateData,
  IN EFI_PEI_RECOVERY_BLOCK_IO_PPI          *BlockIoPpi,
  IN UINTN                                  IndexBlockDevice,
  IN UINT32                                 Lba
  )
{
  EFI_STATUS                      Status;
  UINTN                           BufferSize;
  UINT8                           *Buffer;
  PEI_CD_EXPRESS_DIR_FILE_RECORD  *FileRecord;
  UINTN                           Index;
  BOOLEAN                         Unicode;
  UINT8                           *FileName;
  UINT8                           LongName[256];
  UINT32                          LastPos;
  UINT32                          Length;
  
  
  FileName = AllocatePool (256);
  if (FileName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Buffer      = PrivateData->BlockBuffer;
  BufferSize  = PEI_CD_BLOCK_SIZE;
  LastPos     = 0;
  Length      = 0;
  UnicodeStrToAsciiStr (PcdGetPtr(PcdPeiRecoveryFile), (CHAR8 *)FileName);

  SetMem (Buffer, BufferSize, 0);
  SetMem (LongName, 256, 0);
  
  if (FileName[0] == '/') {
    //
    // Check volume label only if it is specified with "//VolumeName\\RecoveryFileName"
    //
    while (*FileName == '/') {
      FileName++;
    }
    if (*FileName) {
      while (*FileName != '\\') {
        FileName++;
      }
      while (*FileName == '\\') {
        FileName++;
      }
    }
  }

  Status = BlockIoPpi->ReadBlocks (
                        PrivateData->PeiServices,
                        BlockIoPpi,
                        IndexBlockDevice,
                        Lba,
                        BufferSize,
                        Buffer
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  while (1) {
    FileRecord = (PEI_CD_EXPRESS_DIR_FILE_RECORD *) Buffer;

    if (FileRecord->Length == 0 || LastPos >= 255) {
      break;
    }

    if (FileRecord->FileIDLength < 2) {
      Buffer += FileRecord->Length;
      continue;
    }
    //
    // Not intend to check other flag now
    //
    if ((FileRecord->Flag & PEI_CD_EXPRESS_DIR_FILE_REC_FLAG_ISDIR)) {
      Unicode = (FileRecord->FileIDLength > 1 && FileRecord->FileID[0] == 0) ? TRUE : FALSE;
      Length = Unicode ? (FileRecord->FileIDLength / 2): FileRecord->FileIDLength;
      //
      // Append directory path
      //
      for (Index = 0 ; Index < Length; Index++) {
        if (!Unicode) {
          LongName[LastPos++] = FileRecord->FileID[Index];
        } else {
          LongName[LastPos++] = FileRecord->FileID[1 + (Index * 2)];
        }
      }
      if (!StringCmp (LongName, FileName, LastPos, FALSE)) {
        Buffer += FileRecord->Length;
        LastPos -= Length;
        Lba++;
        continue;
      }
      LongName[LastPos++] = '\\';
      //
      // Set LBA to next directory path
      //
      Lba = FileRecord->LocationOfExtent[0];
      Status = BlockIoPpi->ReadBlocks (
                             PrivateData->PeiServices,
                             BlockIoPpi,
                             0,
                             Lba,
                             BufferSize,
                             Buffer
                             );
      continue;
    }

    Unicode = (FileRecord->FileIDLength > 1 && FileRecord->FileID[0] == 0) ? TRUE : FALSE;
    Length = Unicode ? (FileRecord->FileIDLength / 2): FileRecord->FileIDLength;
    //
    // Append file name
    //
    for (Index = 0 ; Index < Length; Index++) {
      if (!Unicode) {
        if (FileRecord->FileID[Index] == ';') break;
        LongName[LastPos++] = FileRecord->FileID[Index];
      } else {
        if (FileRecord->FileID[1 + (Index * 2)] == ';') break;
        LongName[LastPos++] = FileRecord->FileID[1 + (Index * 2)];
      }
    }
    //
    // Compare recovery file path
    //
    if (!StringCmp (LongName, FileName, AsciiStrLen ((CHAR8 *)FileName), FALSE)) {
      Buffer += FileRecord->Length;
      LastPos -= Index;
      continue;
    }

    PrivateData->CapsuleData[PrivateData->CapsuleCount].CapsuleStartLba = FileRecord->LocationOfExtent[0];
    PrivateData->CapsuleData[PrivateData->CapsuleCount].CapsuleSize = FileRecord->DataLength[0];

    FreePool(FileName);
    return EFI_SUCCESS;
  }

  FreePool(FileName);
  return EFI_NOT_FOUND;
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
  PEI_CD_EXPRESS_PRIVATE_DATA *PrivateData;

  PrivateData = PEI_CD_EXPRESS_PRIVATE_DATA_FROM_THIS (This);
  UpdateBlocksAndVolumes (PrivateData);
  *NumberRecoveryCapsules = PrivateData->CapsuleCount;

  if (*NumberRecoveryCapsules == 0) {
    return EFI_NOT_FOUND;
  }

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
  PEI_CD_EXPRESS_PRIVATE_DATA *PrivateData;
  UINTN                       NumberRecoveryCapsules;
  EFI_STATUS                  Status;

  Status = GetNumberRecoveryCapsules (PeiServices, This, &NumberRecoveryCapsules);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (FeaturePcdGet (PcdFrameworkCompatibilitySupport)) {
    CapsuleInstance = CapsuleInstance + 1;
  }

  if ((CapsuleInstance == 0) || (CapsuleInstance > NumberRecoveryCapsules)) {
    return EFI_NOT_FOUND;
  }

  PrivateData = PEI_CD_EXPRESS_PRIVATE_DATA_FROM_THIS (This);

  *Size = PrivateData->CapsuleData[CapsuleInstance - 1].CapsuleSize;
  CopyMem (
    CapsuleType,
    &gRecoveryOnDataCdGuid,
    sizeof (EFI_GUID)
    );

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
  IN EFI_PEI_SERVICES                             **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI           *This,
  IN UINTN                                        CapsuleInstance,
  OUT VOID                                        *Buffer
  )
{
  EFI_STATUS                       Status;
  PEI_CD_EXPRESS_PRIVATE_DATA      *PrivateData;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI    *BlockIoPpi;
  UINTN                            NumberRecoveryCapsules;
  EFI_HOB_MEMORY_ALLOCATION_MODULE *MemoryHob;
  UINTN                            Lba;
  UINTN                            Size;
  UINTN                            RemainingSize;
  UINT8                            LastBlock[PEI_CD_BLOCK_SIZE];
  UINTN                            BiosImageOffset;
  UINTN                            BiosImageSize;

  Status = GetNumberRecoveryCapsules (PeiServices, This, &NumberRecoveryCapsules);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (FeaturePcdGet (PcdFrameworkCompatibilitySupport)) {
    CapsuleInstance = CapsuleInstance + 1;
  }

  if ((CapsuleInstance == 0) || (CapsuleInstance > NumberRecoveryCapsules)) {
    return EFI_NOT_FOUND;
  }

  PrivateData = PEI_CD_EXPRESS_PRIVATE_DATA_FROM_THIS (This);
  BlockIoPpi  = PrivateData->CapsuleData[CapsuleInstance - 1].BlockIo;

  Lba = PrivateData->CapsuleData[CapsuleInstance - 1].CapsuleStartLba;
  Size = PrivateData->CapsuleData[CapsuleInstance - 1].CapsuleSize;
  RemainingSize = Size % PEI_CD_BLOCK_SIZE;
  Size -= RemainingSize;
  Status = BlockIoPpi->ReadBlocks (
                         PrivateData->PeiServices,
                         BlockIoPpi,
                         PrivateData->CapsuleData[CapsuleInstance - 1].IndexBlock,
                         Lba,
                         Size,
                         Buffer
                         );
  if ((Status == EFI_SUCCESS) && (RemainingSize > 0)) {
    Lba += Size / PEI_CD_BLOCK_SIZE;
    Status = BlockIoPpi->ReadBlocks (
                           PrivateData->PeiServices,
                           BlockIoPpi,
                           PrivateData->CapsuleData[CapsuleInstance - 1].IndexBlock,
                           Lba,
                           PEI_CD_BLOCK_SIZE,
                           LastBlock
                           );
    if (Status == EFI_SUCCESS) {
      CopyMem((UINT8 *)Buffer + Size, LastBlock, RemainingSize);
    }
  }
  if (Status == EFI_SUCCESS) {
    Status = VerifyFirmware(
               Buffer,
               PrivateData->CapsuleData[CapsuleInstance - 1].CapsuleSize
               );
  }
  if (Status == EFI_SUCCESS) {
    Status = GetBiosImageFromCapsule (
               Buffer,
               PrivateData->CapsuleData[CapsuleInstance - 1].CapsuleSize,
               &BiosImageOffset,
               &BiosImageSize
               );
    if (Status == EFI_SUCCESS) {
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
  return Status;
}

/**
  This function compares two ASCII strings in case sensitive/insensitive way.

  @param  Source1           The first string.
  @param  Source2           The second string.
  @param  Size              The maximum comparison length.
  @param  CaseSensitive     Flag to indicate whether the comparison is case sensitive.

  @retval TRUE              The two strings are the same.
  @retval FALSE             The two string are not the same.

**/
BOOLEAN
StringCmp (
  IN UINT8      *Source1,
  IN UINT8      *Source2,
  IN UINTN      Size,
  IN BOOLEAN    CaseSensitive
  )
{
  UINTN Index;
  UINT8 Dif;

  for (Index = 0; Index < Size; Index++) {
    if (Source1[Index] == Source2[Index]) {
      continue;
    }

    if (!CaseSensitive) {
      Dif = (UINT8) ((Source1[Index] > Source2[Index]) ? (Source1[Index] - Source2[Index]) : (Source2[Index] - Source1[Index]));
      if (Dif == ('a' - 'A')) {
        continue;
      }
    }

    return FALSE;
  }

  return TRUE;
}

/**
  Compare disk Volume ID

  @param[in] VolumeId                   Pointer to compact disk volume label

  @retval TRUE                          The given volume label is matched
  @retval FALSE                         The given volume label is not matched                     

**/
BOOLEAN
VolumeLabelMatched (
  UINT8      *VolumeID
  )
{
  UINT8         *FileName;
  UINT8         *Ptr;
  UINT8         VolumeName[PEI_CD_EXPRESS_VOLUME_ID_SIZE];
  UINT8         UnicodeString[PEI_CD_EXPRESS_VOLUME_ID_SIZE];
  BOOLEAN       Unicode;
  UINTN         Index;
  
  FileName = (UINT8 *)AllocatePool(256);
  if (FileName == NULL) {
    return FALSE;
  }

  Ptr      = VolumeName;
  UnicodeStrToAsciiStr (PcdGetPtr(PcdPeiRecoveryFile), (CHAR8 *)FileName);

  Unicode  = (*VolumeID == 0 && *(VolumeID + 1) != 0) ? TRUE : FALSE;

  ZeroMem (VolumeName, PEI_CD_EXPRESS_VOLUME_ID_SIZE);

  if (FileName[0] == '/') {
    //
    // Check volume label only if it is specified with "//VolumeName\\RecoveryFileName"
    //
    while (*FileName == '/') {
      FileName++;
    }
    if (*FileName) {
      while (*FileName != '\\') {
        *(Ptr++) = *(FileName++);
      }
    }
  }
  FreePool(FileName);
  if (Unicode) {
    ZeroMem (UnicodeString, PEI_CD_EXPRESS_VOLUME_ID_SIZE);
    for (Index = 0 ; Index < (PEI_CD_EXPRESS_VOLUME_ID_SIZE / 2); Index++) {
      UnicodeString[Index] = VolumeID[1 + (Index * 2)];
    }
    VolumeID = (UINT8*)UnicodeString;
  }
  if (StringCmp (VolumeName, VolumeID, AsciiStrLen ((CHAR8 *)VolumeName), FALSE) || AsciiStrLen ((CHAR8 *)VolumeName) == 0) {
    return TRUE;
  }

  return FALSE;
}