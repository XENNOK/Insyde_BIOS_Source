/** @file
  Runtime DXE driver implementation for Firmware Management Protocol

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FirmwareManagement.h>
#include <Guid/EventGroup.h>
#include <Guid/IrsiFeature.h>
#include <Library/IrsiRegistrationLib.h>
#include <Protocol/FvRegionInfo.h>
#include <Library/BvdtLib.h>
#include "FirmwareManagement.h"

IMAGE_MAP mImageMap[] =
{
  {IRSI_BIOS_IMAGE_GUID,         L"System Bios"},
  {IRSI_PEI_IMAGE_GUID,          L"PEI"},
  {IRSI_DXE_IMAGE_GUID,          L"DXE"},
  {IRSI_VARIABLE_IMAGE_GUID,     L"Variable"},
  {IRSI_FACTORY_COPY_IMAGE_GUID, L"FactoryCopy"},
  {IRSI_MICROCODE_IMAGE_GUID,    L"Microcode"},
  {IRSI_BVDT_IMAGE_GUID,         L"BVDT"},
  {IRSI_DMI_IMAGE_GUID,          L"DMI"},
  {IRSI_NULL_IMAGE_GUID,         L""}
};

CHAR16   DummyString[] = L"";

UINT8   *mSourceBuffer       = NULL;
UINT8   *mVerifyBuffer       = NULL;
UINTN   mMaxFvbBlockSize     = MAX_FVB_BLOCK_SIZE;
BOOLEAN mWriteProtect        = FALSE;
UINT32  mEsrtFirmwareVersion = 0x00000000;;


//
// The firmware regions share the same version number - the ESRT firmware version
//
CHAR16                           mVersionName[MAX_STRING_LEN]=L"00.00.00.0000";
EFI_FIRMWARE_MANAGEMENT_PROTOCOL mFirmwareManagement;
EFI_EVENT                        mFvRegionInfoEvent;
VOID                             *mFvRegionInfoReg;
LIST_ENTRY                       mFirmwareImageList;
UINTN                            mImageCount = 0;
EFI_GUID                         mNullGuid = IRSI_NULL_IMAGE_GUID;

/**
  Conversion of Firmware Version with UINT32 type to a unicode string

  @param[in]  ImageIndex        A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.

  @returns                      The Image Entry pointer with the specified ImageIndex
                                NULL if Image Entry was not found

**/
EFI_STATUS
EFIAPI
FirmwareVersionToString (
  UINT32    Version,
  UINTN     Size,
  CHAR16    *VersionString
  )
{
  if (VersionString == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  UnicodeSPrint ( VersionString,
                  Size,
                  L"%02x.%02x.%02x.%04x",
                  (UINTN)(Version & 0xF0000000) >> 28,
                  (UINTN)(Version & 0x0F000000) >> 24,
                  (UINTN)(Version & 0x00FF0000) >> 16,
                  (UINTN)(Version & 0x0000FFFF)
                  );
  return EFI_SUCCESS;

}

/**
  Find the Firmware Image Entry with specific ImageIndex from the FirmwareImageList

  @param[in]  ImageIndex        A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  
  @returns                      The Image Entry pointer with the specified ImageIndex
                                NULL if Image Entry was not found

**/
FIRMWARE_IMAGE_ENTRY *
EFIAPI 
FindImageEntryByIndex (
  UINT8   ImageIndex
  )
{
  LIST_ENTRY             *Link;
  FIRMWARE_IMAGE_ENTRY   *ImageEntry;

  Link = GetFirstNode (&mFirmwareImageList);
  while (!IsNull (&mFirmwareImageList, Link)) {
    ImageEntry = FIRMWARE_IMAGE_ENTRY_FROM_LINK(Link);
    if (ImageEntry->ImageIndex == ImageIndex) {
      return ImageEntry;
    }
    Link = GetNextNode(&mFirmwareImageList, Link);
  };

  return NULL;
}

/**
  Find the Firmware Image Entry with specific ImageTypeGuid from the FirmwareImageList

  @param[in]  ImageTypeGuid     Pointer to the Image Type GUID
  
  @returns                      The Image Entry pointer with the specified ImageTypeGuid
                                NULL if Image Entry was not found

**/
FIRMWARE_IMAGE_ENTRY *
EFIAPI FindImageEntryByGuid (
  EFI_GUID   *ImageTypeGuid
  )
{
  LIST_ENTRY             *Link;
  FIRMWARE_IMAGE_ENTRY   *ImageEntry;

  Link = GetFirstNode (&mFirmwareImageList);
  while (!IsNull (&mFirmwareImageList, Link)) {
    ImageEntry = FIRMWARE_IMAGE_ENTRY_FROM_LINK(Link);
    if (CompareGuid(&ImageEntry->ImageTypeGuid, ImageTypeGuid)) {
      return ImageEntry;
    }
    Link = GetNextNode(&mFirmwareImageList, Link);
  };

  return NULL;
}

/**
  Returns information about the current firmware image(s) of the device.

  This function allows a copy of the current firmware image to be created and saved.
  The saved copy could later been used, for example, in firmware image recovery or rollback.

  @param[in]      This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in, out] ImageInfoSize      A pointer to the size, in bytes, of the ImageInfo buffer. 
                                     On input, this is the size of the buffer allocated by the caller.
                                     On output, it is the size of the buffer returned by the firmware 
                                     if the buffer was large enough, or the size of the buffer needed
                                     to contain the image(s) information if the buffer was too small.
  @param[in, out] ImageInfo          A pointer to the buffer in which firmware places the current image(s)
                                     information. The information is an array of EFI_FIRMWARE_IMAGE_DESCRIPTORs.
  @param[out]     DescriptorVersion  A pointer to the location in which firmware returns the version number
                                     associated with the EFI_FIRMWARE_IMAGE_DESCRIPTOR. 
  @param[out]     DescriptorCount    A pointer to the location in which firmware returns the number of
                                     descriptors or firmware images within this device.
  @param[out]     DescriptorSize     A pointer to the location in which firmware returns the size, in bytes,
                                     of an individual EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out]     PackageVersion     A version number that represents all the firmware images in the device.
                                     The format is vendor specific and new version must have a greater value
                                     than the old version. If PackageVersion is not supported, the value is
                                     0xFFFFFFFF. A value of 0xFFFFFFFE indicates that package version comparison
                                     is to be performed using PackageVersionName. A value of 0xFFFFFFFD indicates
                                     that package version update is in progress.
  @param[out]     PackageVersionName A pointer to a pointer to a null-terminated string representing the
                                     package version name. The buffer is allocated by this function with 
                                     AllocatePool(), and it is the caller's responsibility to free it with a call
                                     to FreePool().

  @retval EFI_SUCCESS                The device was successfully updated with the new image.
  @retval EFI_BUFFER_TOO_SMALL       The ImageInfo buffer was too small. The current buffer size 
                                     needed to hold the image(s) information is returned in ImageInfoSize.                                               
  @retval EFI_INVALID_PARAMETER      ImageInfoSize is NULL.
  @retval EFI_DEVICE_ERROR           Valid information could not be returned. Possible corrupted image.

**/                         
EFI_STATUS
EFIAPI 
FirmwareManagementGetImageInfo (
  IN EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN OUT    UINTN                           *ImageInfoSize,
  IN OUT    EFI_FIRMWARE_IMAGE_DESCRIPTOR   *ImageInfo,
  OUT       UINT32                          *DescriptorVersion,
  OUT       UINT8                           *DescriptorCount,
  OUT       UINTN                           *DescriptorSize,
  OUT       UINT32                          *PackageVersion,
  OUT       CHAR16                          **PackageVersionName
  )
{
  UINTN                  Index;
  UINTN                  ImageCount;
  LIST_ENTRY             *Link;
  FIRMWARE_IMAGE_ENTRY   *ImageEntry;

  if (This == NULL ||
      ImageInfoSize == NULL ||
      DescriptorCount == NULL ||
      DescriptorSize == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // PackageVersion is not supported
  //
  if (PackageVersion != NULL) {
    *PackageVersion = 0xFFFFFFFF;
  }

  ImageCount = mImageCount;
  if (*ImageInfoSize < ImageCount * sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR) ||
      *DescriptorCount < ImageCount) {
    *ImageInfoSize = ImageCount * sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
    *DescriptorCount = (UINT8)ImageCount;
    *DescriptorSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR); 
    if (DescriptorVersion != NULL) {     
      *DescriptorVersion = EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION;
    }      
    return EFI_BUFFER_TOO_SMALL;
  }
  *ImageInfoSize = ImageCount * sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
  *DescriptorCount = (UINT8)ImageCount;
  *DescriptorSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR); 
  if (DescriptorVersion != NULL) {     
    *DescriptorVersion = EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION;
  }    
  if (ImageInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Link = GetFirstNode (&mFirmwareImageList);
  Index = 0;
  while (!IsNull (&mFirmwareImageList, Link)) {
    ImageEntry = FIRMWARE_IMAGE_ENTRY_FROM_LINK(Link);
    ImageInfo[Index].ImageIndex          = (UINT8)ImageEntry->ImageIndex;
    ImageInfo[Index].ImageId             = (UINT64)ImageEntry->ImageIndex;
    ImageInfo[Index].ImageTypeId         = ImageEntry->ImageTypeGuid;
    ImageInfo[Index].ImageIdName         = DummyString;
    ImageInfo[Index].Size                = ImageEntry->ImageSize;
    
    ImageInfo[Index].Version             = mEsrtFirmwareVersion;
	  FirmwareVersionToString (ImageInfo[Index].Version, StrSize(mVersionName), mVersionName);
    ImageInfo[Index].VersionName         = mVersionName;
    ImageInfo[Index].AttributesSupported = IMAGE_ATTRIBUTE_IMAGE_UPDATABLE;
    ImageInfo[Index].AttributesSetting   = IMAGE_ATTRIBUTE_IMAGE_UPDATABLE;
    ImageInfo[Index].Compatibilities     = 0;
    Link = GetNextNode(&mFirmwareImageList, Link);
    Index++;
  }
  
  return EFI_SUCCESS;
}

/**
  Retrieves a copy of the current firmware image of the device. 

  This function allows a copy of the current firmware image to be created and saved.
  The saved copy could later been used, for example, in firmware image recovery or rollback.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[out] Image              Points to the buffer where the current image is copied to.
  @param[out] ImageSize          On entry, points to the size of the buffer pointed to by Image, in bytes.
                                 On return, points to the length of the image, in bytes.

  @retval EFI_SUCCESS            The device was successfully updated with the new image.
  @retval EFI_BUFFER_TOO_SMALL   The buffer specified by ImageSize is too small to hold the 
                                 image. The current buffer size needed to hold the image is returned 
                                 in ImageSize.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_NOT_FOUND          The current image is not copied to the buffer.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATIO  The operation could not be performed due to an authentication failure.

**/                         
EFI_STATUS
EFIAPI 
FirmwareManagementGetImage (
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN  UINT8                             ImageIndex,
  IN  OUT  VOID                         *Image,
  IN  OUT  UINTN                        *ImageSize
  )
{
  EFI_STATUS                                     Status;
  UINTN                                          Retry;
  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  Progress;
  UINTN                                          Offset;
  UINTN                                          BlockSize;
  UINTN                                          NumberOfBlocks;
  EFI_LBA                                        BlockIndex;
  EFI_LBA                                        StartBlock;
  EFI_LBA                                        EndBlock;
  UINTN                                          Size;
  UINTN                                          TotalBytesRead;
  UINTN                                          Completion;
  FIRMWARE_IMAGE_ENTRY                           *ImageEntry;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL             *Fvb;

  
  if (ImageSize == NULL || ImageIndex == 0 || ImageIndex > mImageCount) {
    return EFI_INVALID_PARAMETER;
  }

  ImageEntry = FindImageEntryByIndex (ImageIndex);
  if (ImageEntry == NULL || ImageEntry->Signature != FIRMWARE_IMAGE_SIGNATURE) {
    return EFI_NOT_FOUND;
  }
  
  if (*ImageSize < ImageEntry->ImageSize) {
    *ImageSize = ImageEntry->ImageSize;
    return EFI_BUFFER_TOO_SMALL;
  } 

  if (Image == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Progress = NULL;
  //
  // Get the progress reporting function variable
  //
  Size = sizeof (EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS);
  Status = EfiGetVariable (
              GET_IMAGE_PROGRESS_NAME,
              &gEfiFirmwareManagementProtocolGuid,
              NULL,
              &Size,
              &Progress
              );

  Fvb = ImageEntry->Fvb;
  Status = Fvb->GetBlockSize(Fvb, 0, &BlockSize, &NumberOfBlocks);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  if (ImageEntry->ImageSize > BlockSize * NumberOfBlocks) {
    return EFI_UNSUPPORTED;
  }

  *ImageSize     = ImageEntry->ImageSize;
  StartBlock     = ImageEntry->ImageOffset/BlockSize;
  EndBlock       = (ImageEntry->ImageOffset + ImageEntry->ImageSize - 1)/BlockSize;
  TotalBytesRead = 0;

  for (BlockIndex = StartBlock; BlockIndex <= EndBlock; BlockIndex++) {
    //
    // The following code handles the conditions that the image does not start at the block beginning
    // or the image does not end at the block end
    //
    if (BlockIndex == StartBlock) {
      Offset = ImageEntry->ImageOffset % BlockSize;
      if ((*ImageSize - TotalBytesRead) < BlockSize - Offset) {
        Size = *ImageSize - TotalBytesRead;
      } else {
        Size = BlockSize - Offset;
      }
    } else {
      Offset = 0;
      if ((*ImageSize - TotalBytesRead) < BlockSize) {
        Size = *ImageSize - TotalBytesRead;
      } else {
        Size = BlockSize;
      }
    }

    //
    // The remaining size of the last block access might not be equal to block size
    //
    if ((*ImageSize - TotalBytesRead) < BlockSize) {
      Size = *ImageSize - TotalBytesRead;
    } else {
      Size = BlockSize;
    }

    for (Retry = 0; Retry < MAX_RETRY_CNT; Retry++) {
      Status = Fvb->Read(
                      Fvb,
                      BlockIndex,
                      Offset,
                      &Size,
                      (UINT8 *)Image + TotalBytesRead
                     );
      if (Status == EFI_SUCCESS) {
        TotalBytesRead += Size;
        break;
      }
    }
    if (EFI_ERROR(Status)) {
      break;
    }
    if (Progress != NULL) {
      Completion = (UINT8)(100 * TotalBytesRead / *ImageSize);
      if (Completion == 0) {
        Completion = 1;
      } else if (Completion > 100) {
        Completion = 100;
      }
      Progress(Completion);
    }
  }
  if ( Progress != NULL ) {
    EfiSetVariable (
                GET_IMAGE_PROGRESS_NAME,
                &gEfiFirmwareManagementProtocolGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                0,
                NULL
              );
  }

  return Status;
}


/**
  Checks if the firmware image is valid for the device.

  This function allows firmware update application to validate the firmware image without
  invoking the SetImage() first.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[out] ImageUpdatable     Indicates if the new image is valid for update. It also provides,
                                 if available, additional information if the image is invalid.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
FirmwareManagementCheckImage (
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN  UINT8                             ImageIndex,
  IN  CONST VOID                        *Image,
  IN  UINTN                             ImageSize,
  OUT UINT32                            *ImageUpdatable
  )
{
  FIRMWARE_IMAGE_ENTRY       *ImageEntry;
  if (Image == NULL || ImageUpdatable == NULL || ImageIndex == 0  || ImageIndex > mImageCount) {
    return EFI_INVALID_PARAMETER;
  }

  ImageEntry = FindImageEntryByIndex (ImageIndex);
  if(ImageEntry == NULL) {
    return EFI_NOT_FOUND;
  }
  if (ImageSize != ImageEntry->ImageSize) {
    return EFI_BAD_BUFFER_SIZE;
  }
  *ImageUpdatable = (UINT32)TRUE;

  return EFI_SUCCESS;
}

/**
  Updates the firmware image of the device.

  This function updates the hardware with the new firmware image.
  This function returns EFI_UNSUPPORTED if the firmware image is not updatable.
  If the firmware image is updatable, the function should perform the following minimal validations
  before proceeding to do the firmware image update.
  - Validate the image authentication if image has attribute 
    IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED.  The function returns 
    EFI_SECURITY_VIOLATION if the validation fails.
  - Validate the image is a supported image for this device.  The function returns EFI_ABORTED if 
    the image is unsupported.  The function can optionally provide more detailed information on 
    why the image is not a supported image.
  - Validate the data from VendorCode if not null.  Image validation must be performed before 
    VendorCode data validation.  VendorCode data is ignored or considered invalid if image 
    validation failed.  The function returns EFI_ABORTED if the data is invalid.
  
  VendorCode enables vendor to implement vendor-specific firmware image update policy.  Null if 
  the caller did not specify the policy or use the default policy.  As an example, vendor can implement 
  a policy to allow an option to force a firmware image update when the abort reason is due to the new 
  firmware image version is older than the current firmware image version or bad image checksum.  
  Sensitive operations such as those wiping the entire firmware image and render the device to be
  non-functional should be encoded in the image itself rather than passed with the VendorCode.
  AbortReason enables vendor to have the option to provide a more detailed description of the abort
  reason to the caller.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[in]  VendorCode         This enables vendor to implement vendor-specific firmware image update policy.  
                                 Null indicates the caller did not specify the policy or use the default policy.
  @param[in]  Progress           A function used by the driver to report the progress of the firmware update.
  @param[out] AbortReason        A pointer to a pointer to a null-terminated string providing more
                                 details for the aborted operation. The buffer is allocated by this function 
                                 with AllocatePool(), and it is the caller's responsibility to free it with a
                                 call to FreePool().

  @retval EFI_SUCCESS            The device was successfully updated with the new image.
  @retval EFI_ABORTED            The operation is aborted.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/                         
EFI_STATUS
EFIAPI 
FirmwareManagementSetImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL                 *This,
  IN  UINT8                                            ImageIndex,
  IN  CONST VOID                                       *Image,
  IN  UINTN                                            ImageSize,
  IN  CONST VOID                                       *VendorCode,
  IN  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS    Progress,
  OUT CHAR16                                           **AbortReason
  )
{
  EFI_STATUS Status;
  UINT32     Updatable;
  UINTN      BlockSize;
  UINTN      NumberOfBlocks;
  UINTN      Offset;
  EFI_LBA    BlockIndex;
  EFI_LBA    StartBlock;
  EFI_LBA    EndBlock;
  UINTN      RetryCnt;
  UINTN      Completion;
  UINTN      Size;
  UINTN      UpdateSize;
  UINTN      TotalBytesWritten;
  FIRMWARE_IMAGE_ENTRY *ImageEntry;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *Fvb;
  UINT8      Sum8;
  

  Status = FirmwareManagementCheckImage (This, ImageIndex, Image, ImageSize, &Updatable);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (!Updatable) {
    return EFI_UNSUPPORTED;
  }
  
  if (mSourceBuffer == NULL || mVerifyBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  ImageEntry = FindImageEntryByIndex (ImageIndex);
  if (ImageEntry == NULL || ImageEntry->Signature != FIRMWARE_IMAGE_SIGNATURE) {
    return EFI_NOT_FOUND;
  }

  Fvb = ImageEntry->Fvb;
  Status = Fvb->GetBlockSize(Fvb, 0, &BlockSize, &NumberOfBlocks);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  if (ImageEntry->ImageSize != ImageSize) {
    return EFI_BAD_BUFFER_SIZE;
  }
  
  if (VendorCode == NULL) {
    return EFI_ABORTED;
  }
  
  Sum8 = CalculateSum8(Image, ImageSize);
  if ( *(UINT8 *)VendorCode != Sum8 ) {
    return EFI_ABORTED;
  }
  
  StartBlock     = ImageEntry->ImageOffset/BlockSize;
  EndBlock       = (ImageEntry->ImageOffset + ImageSize - 1)/BlockSize;
  TotalBytesWritten = 0;

  for (BlockIndex = StartBlock; BlockIndex <= EndBlock; BlockIndex++) {
    if (BlockIndex == StartBlock) {
      //
      // The image might not started from the block boundary
      //
      Offset = ImageEntry->ImageOffset % BlockSize;
    } else {
      Offset = 0;
    }
    UpdateSize = BlockSize - Offset;
    if (UpdateSize > ImageSize - TotalBytesWritten) {
      UpdateSize = ImageSize - TotalBytesWritten;
    }

    for (RetryCnt = 0; RetryCnt < MAX_RETRY_CNT; RetryCnt++) {
      Size = BlockSize;
      Status = Fvb->Read(
                      Fvb,
                      BlockIndex,
                      0,
                      &Size,
                      mSourceBuffer
                     );
      if (EFI_ERROR(Status)) {
        continue;
      }
      if (CompareMem(mSourceBuffer + Offset, (UINT8 *)Image + TotalBytesWritten, UpdateSize) == 0) {
        //
        // Content identical, no need to write
        //
        Status = EFI_SUCCESS;
        break;
      }
      Status = Fvb->EraseBlocks (
                         Fvb,
                         BlockIndex,
                         (UINT32)1,
                         EFI_LBA_LIST_TERMINATOR
                         );
      if (EFI_ERROR(Status)) {
        continue;
      }
      
      CopyMem(mSourceBuffer + Offset, (UINT8 *)Image + TotalBytesWritten, UpdateSize);

      Size = BlockSize;
      Status = Fvb->Write(
                      Fvb,
                      BlockIndex,
                      0,
                      &Size,
                      mSourceBuffer
                     );
      if (EFI_ERROR(Status)) {
        continue;
      }             
      Size = BlockSize;
      Status = Fvb->Read(
                      Fvb,
                      BlockIndex,
                      0,
                      &Size,
                      mVerifyBuffer
                   );
      if (EFI_ERROR(Status)) {
        continue;
      }             
      if (CompareMem(mSourceBuffer, mVerifyBuffer, BlockSize) == 0) {
        Status = EFI_SUCCESS;
        break;
      } else {                    
        Status = RETURN_CRC_ERROR;
        continue;
      }
    }
    if (Status == EFI_SUCCESS) {
      TotalBytesWritten += UpdateSize;
    }
    if (Progress != NULL) {
      Completion = (UINT8)(100 * TotalBytesWritten / ImageSize);
      if (Completion == 0) {
        Completion = 1;
      } else if (Completion > 100) {
        Completion = 100;
      }
      Progress(Completion);
    }
    if (EFI_ERROR(Status)) {
      break;
    }
  }

  return Status;
}


/**
  Returns information about the firmware package. 

  This function returns package information.

  @param[in]  This                     A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[out] PackageVersion           A version number that represents all the firmware images in the device.
                                       The format is vendor specific and new version must have a greater value
                                       than the old version. If PackageVersion is not supported, the value is
                                       0xFFFFFFFF. A value of 0xFFFFFFFE indicates that package version
                                       comparison is to be performed using PackageVersionName. A value of
                                       0xFFFFFFFD indicates that package version update is in progress.
  @param[out] PackageVersionName       A pointer to a pointer to a null-terminated string representing
                                       the package version name. The buffer is allocated by this function with
                                       AllocatePool(), and it is the caller's responsibility to free it with a
                                       call to FreePool().
  @param[out] PackageVersionNameMaxLen The maximum length of package version name if device supports update of
                                       package version name. A value of 0 indicates the device does not support
                                       update of package version name. Length is the number of Unicode characters,
                                       including the terminating null character.
  @param[out] AttributesSupported      Package attributes that are supported by this device. See 'Package Attribute 
                                       Definitions' for possible returned values of this parameter. A value of 1 
                                       indicates the attribute is supported and the current setting value is
                                       indicated in AttributesSetting. A value of 0 indicates the attribute is not
                                       supported and the current setting value in AttributesSetting is meaningless.
  @param[out] AttributesSetting        Package attributes. See 'Package Attribute Definitions' for possible returned
                                       values of this parameter

  @retval EFI_SUCCESS                  The package information was successfully returned.
  @retval EFI_UNSUPPORTED              The operation is not supported.

**/
EFI_STATUS
EFIAPI 
FirmwareManagementGetPackageInfo (
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL *This,
  OUT UINT32                           *PackageVersion,
  OUT CHAR16                           **PackageVersionName,
  OUT UINT32                           *PackageVersionNameMaxLen,
  OUT UINT64                           *AttributesSupported,
  OUT UINT64                           *AttributesSetting
  )
{
  *PackageVersion = 0xFFFFFFFF;
  if (PackageVersionName != NULL) {
    *PackageVersionName = DummyString;
  }  
  if (PackageVersionNameMaxLen != NULL) {
    *PackageVersionNameMaxLen = 0;
  }
  if (AttributesSupported != NULL) {
    *AttributesSupported = 0;
  }
  if (AttributesSetting != NULL) {
    *AttributesSetting = 0;
  }
  return EFI_UNSUPPORTED;
}

/**
  Updates information about the firmware package.

  This function updates package information.
  This function returns EFI_UNSUPPORTED if the package information is not updatable.
  VendorCode enables vendor to implement vendor-specific package information update policy.
  Null if the caller did not specify this policy or use the default policy.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  Image              Points to the authentication image.
                                 Null if authentication is not required.
  @param[in]  ImageSize          Size of the authentication image in bytes.
                                 0 if authentication is not required.
  @param[in]  VendorCode         This enables vendor to implement vendor-specific firmware
                                 image update policy.  
                                 Null indicates the caller did not specify this policy or use
                                 the default policy.
  @param[in]  PackageVersion     The new package version.
  @param[in]  PackageVersionName A pointer to the new null-terminated Unicode string representing
                                 the package version name. 
                                 The string length is equal to or less than the value returned in 
                                 PackageVersionNameMaxLen.

  @retval EFI_SUCCESS            The device was successfully updated with the new package
                                 information.
  @retval EFI_INVALID_PARAMETER  The PackageVersionName length is longer than the value 
                                 returned in PackageVersionNameMaxLen.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATIO  The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI 
FirmwareManagementSetPackageInfo (
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL   *This,
  IN  CONST VOID                         *Image,
  IN  UINTN                              ImageSize,
  IN  CONST VOID                         *VendorCode,
  IN  UINT32                             PackageVersion,
  IN  CONST CHAR16                       *PackageVersionName
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Notification function of Firmware Volume Block Protocol

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
FvRegionInfoProtocolNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINTN                              FvbIndex;
  UINTN                              Index;
  UINTN                              NumberOfFvbs;
  EFI_HANDLE                         *FvbHandles;
  EFI_STATUS                         Status;
  FV_REGION_INFO                     *FvRegionInfo;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *Fvb;
  FIRMWARE_IMAGE_ENTRY               *Image;
  
  Status = gBS->LocateHandleBuffer (
        ByProtocol,
        &gFvRegionInfoProtocolGuid,
        NULL,
        &NumberOfFvbs,
        &FvbHandles
        );
        
  if (Status == EFI_SUCCESS) {
  
    for (FvbIndex = 0; FvbIndex < NumberOfFvbs; FvbIndex++) {
      Status = gBS->HandleProtocol (
                FvbHandles[FvbIndex],
                &gEfiFirmwareVolumeBlockProtocolGuid,
                (VOID **)&Fvb
                );
      if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        break;
      }

      Status = gBS->HandleProtocol (
                FvbHandles[FvbIndex],
                &gFvRegionInfoProtocolGuid,
                (VOID **)&FvRegionInfo
                );
      if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        break;
      }    
      Index = 0;
      while (!CompareGuid(&FvRegionInfo[Index].ImageTypeGuid, &mNullGuid)) {
        if (FindImageEntryByGuid(&FvRegionInfo[Index].ImageTypeGuid) == NULL) {
          Image = AllocateRuntimePool(sizeof(FIRMWARE_IMAGE_ENTRY));
          ASSERT(Image != NULL);
          if (Image != NULL) {
            mImageCount++;
            Image->Signature    = FIRMWARE_IMAGE_SIGNATURE;
            Image->ImageIndex   = (UINT8)mImageCount;
            Image->ImageOffset  = FvRegionInfo[Index].ImageOffset;
            Image->ImageSize    = FvRegionInfo[Index].ImageSize;
            Image->Fvb          = Fvb;
            CopyMem(&Image->ImageTypeGuid, &FvRegionInfo[Index].ImageTypeGuid, sizeof(EFI_GUID));
            InsertTailList (&mFirmwareImageList, &Image->Link);
          }
        }
        Index++;
      }
    }      
    FreePool (FvbHandles);
  } 
  
  return; 
}


/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
VirtualAddressChangeNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  LIST_ENTRY             *Link;
  FIRMWARE_IMAGE_ENTRY   *ImageEntry;
  
  EfiConvertPointer(0, (VOID **)&mFirmwareManagement.GetImageInfo);
  EfiConvertPointer(0, (VOID **)&mFirmwareManagement.GetImage);
  EfiConvertPointer(0, (VOID **)&mFirmwareManagement.SetImage);
  EfiConvertPointer(0, (VOID **)&mFirmwareManagement.CheckImage);
  EfiConvertPointer(0, (VOID **)&mFirmwareManagement.GetPackageInfo);
  EfiConvertPointer(0, (VOID **)&mFirmwareManagement.SetPackageInfo);
  
  if (mSourceBuffer != NULL) {
    EfiConvertPointer(0, (VOID **)&mSourceBuffer);
  }
  if (mVerifyBuffer != NULL) {
    EfiConvertPointer(0, (VOID **)&mVerifyBuffer);
  }
  
  Link = GetFirstNode (&mFirmwareImageList);
  while (!IsNull (&mFirmwareImageList, Link)) {
    ImageEntry = FIRMWARE_IMAGE_ENTRY_FROM_LINK(Link);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->GetBlockSize);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->GetPhysicalAddress);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->GetAttributes);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->SetAttributes);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->Read);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->Write);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb->EraseBlocks);
    EfiConvertPointer (0x0, (VOID **)&ImageEntry->Fvb);

    Link = GetNextNode(&mFirmwareImageList, Link);
  }
  EfiConvertList(0, &mFirmwareImageList);
}


/**
  FirmwareManagement RuntimeDxe module entry point


  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS:          Module initialized successfully
  @retval Others                Module initialized unsuccessfully
**/
EFI_STATUS
EFIAPI
InstallFirmwareManagement (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS           Status;
  EFI_EVENT            VirtualAddressChangeEvent;
  EFI_GUID             EsrtFirmwareGuid;

  Status = GetEsrtFirmwareInfo(&EsrtFirmwareGuid, &mEsrtFirmwareVersion);
  ASSERT_EFI_ERROR(Status);

  mFirmwareManagement.GetImageInfo   = FirmwareManagementGetImageInfo;
  mFirmwareManagement.GetImage       = FirmwareManagementGetImage;
  mFirmwareManagement.SetImage       = FirmwareManagementSetImage;
  mFirmwareManagement.CheckImage     = FirmwareManagementCheckImage;
  mFirmwareManagement.GetPackageInfo = FirmwareManagementGetPackageInfo;
  mFirmwareManagement.SetPackageInfo = FirmwareManagementSetPackageInfo;

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiFirmwareManagementProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mFirmwareManagement
                );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  mSourceBuffer = AllocateRuntimePages (EFI_SIZE_TO_PAGES (mMaxFvbBlockSize));
  ASSERT(mSourceBuffer != NULL);
  mVerifyBuffer = AllocateRuntimePages (EFI_SIZE_TO_PAGES (mMaxFvbBlockSize));
  ASSERT(mVerifyBuffer != NULL);
    
  InitializeListHead (&mFirmwareImageList);
  mFvRegionInfoEvent = EfiCreateProtocolNotifyEvent  (
                          &gFvRegionInfoProtocolGuid,
                          TPL_NOTIFY,
                          FvRegionInfoProtocolNotify,
                          NULL,
                          &mFvRegionInfoReg
                          );

  Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    VirtualAddressChangeNotify,
                    NULL,
                    &gEfiEventVirtualAddressChangeGuid,
                    &VirtualAddressChangeEvent
                    );
  ASSERT_EFI_ERROR(Status);

  return Status;
}
