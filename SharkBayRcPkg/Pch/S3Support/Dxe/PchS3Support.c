/** @file
  This is the driver that implements the PCH S3 Support protocol

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchS3Support.c
  
@brief
  This is the driver that implements the PCH S3 Support protocol

**/
#include <PchS3Support.h>

//
// Global Variables
//
typedef struct {
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH   File;
  EFI_DEVICE_PATH_PROTOCOL            End;
} FV_FILEPATH_DEVICE_PATH;

FV_FILEPATH_DEVICE_PATH mFvDevicePath = {
  {
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH
    }
  }
};

EFI_HANDLE                  mImageHandle;
EFI_PCH_S3_SUPPORT_PROTOCOL mPchS3SupportProtocol;
PCH_S3_PARAMETER_HEADER     *mS3Parameter;
UINT32                      mPchS3ImageEntryPoint;
EFI_GUID                    mPchS3ImageGuid = EFI_PCH_S3_IMAGE_GUID;

//
// Functions
//

/**
  PCH S3 support driver entry point

  @param[in] ImageHandle          Handle for the image of this driver
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
PchS3SupportEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "PchS3SupportEntryPoint() Start\n"));
  mImageHandle = ImageHandle;

  ///
  /// Load the PCH S3 image
  ///
  Status = LoadPchS3Image (&mPchS3ImageEntryPoint);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Set the PCH Init Variable
  /// Make sure NV Variable support is available before this driver
  /// is dispatched
  ///
  Status = SetPchInitVariable ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Install the PCH S3 Support protocol
  ///
  mPchS3SupportProtocol.SetDispatchItem = PchS3SetDispatchItem;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mImageHandle,
                  &gEfiPchS3SupportProtocolGuid,
                  &mPchS3SupportProtocol,
                  NULL
                  );

  DEBUG ((EFI_D_INFO, "PchS3SupportEntryPoint() End\n"));

  return Status;
}

/**
  Set the Pch Init Variable for consumption by PchInitS3 PEIM.
  bugbug: expect to extend the variable service to support <4G EfiReservedMemory
  variable storage, such that memory consumption is flexible and more economical.

  @param[in] VOID

  @retval None
**/
EFI_STATUS
SetPchInitVariable (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Address;
  UINT32                HeaderSize;

  ///
  /// Allcoate <4G EfiReservedMemory
  ///
  Address = 0xFFFFFFFF;
  Status  = (gBS->AllocatePages) (AllocateMaxAddress, EfiReservedMemoryType, 1, &Address);
  ASSERT_EFI_ERROR (Status);
  mS3Parameter  = (PCH_S3_PARAMETER_HEADER *) (UINTN) Address;

  HeaderSize    = sizeof (PCH_S3_PARAMETER_HEADER);
  HeaderSize    = (HeaderSize + 7) / 8 * 8;

  ///
  /// Initialize StorePosition and ExecutePosition
  ///
  mS3Parameter->StorePosition   = HeaderSize;
  mS3Parameter->ExecutePosition = HeaderSize;

  ///
  /// Set PCH_INIT_VARIABLE to point to the allocated buffer
  ///
  Status = gRT->SetVariable (
                  PCH_INIT_VARIABLE_NAME,
                  &gPchInitVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (UINT32),
                  &mS3Parameter
                  );

  return Status;
}

/**
  Set an item to be dispatched at S3 resume time. At the same time, the entry point
  of the PCH S3 support image is returned to be used in subsequent boot script save
  call

  @param[in] This                       Pointer to the protocol instance.
  @param[in] DispatchItem               The item to be dispatched.
  @param[out] S3DispatchEntryPoint      The entry point of the PCH S3 support image.

  @retval EFI_STATUS                    Successfully completed.
  @retval EFI_OUT_OF_RESOURCES          Out of resources.
**/
EFI_STATUS
EFIAPI
PchS3SetDispatchItem (
  IN     EFI_PCH_S3_SUPPORT_PROTOCOL   *This,
  IN     EFI_PCH_S3_DISPATCH_ITEM      *DispatchItem,
  OUT    EFI_PHYSICAL_ADDRESS          *S3DispatchEntryPoint
  )
{
  EFI_STATUS  Status;
  UINT32      TypeSize;
  UINT32      ParameterSize;
  UINT32      Size;
  UINT8       *CurrentPos;

  DEBUG ((EFI_D_INFO, "PchS3SetDispatchItem() Start\n"));

  Status = EFI_SUCCESS;
  ///
  /// Calculate the size required;
  /// ** Always round up to be 8 byte aligned
  ///
  switch (DispatchItem->Type) {
  case PchS3ItemTypeSendCodecCommand:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    break;

  case PchS3ItemTypeInitPcieRootPortDownstream:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    break;

  case PchS3ItemTypePcieSetPm:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_PCIE_SET_PM);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    break;

  case PchS3ItemTypeProgramIobp:
    ParameterSize = sizeof (EFI_PCH_S3_PARAMETER_PROG_IOBP);
    ParameterSize = (ParameterSize + 7) / 8 * 8;
    break;

  default:
    ParameterSize = 0;
    ASSERT (FALSE);
    break;
  }
  ///
  /// Round up TypeSize to be 8 byte aligned
  ///
  TypeSize  = sizeof (EFI_PCH_S3_DISPATCH_ITEM_TYPE);
  TypeSize  = (TypeSize + 7) / 8 * 8;

  ///
  /// Total size is TypeSize + ParameterSize
  ///
  Size = TypeSize + ParameterSize;

  if (mS3Parameter->StorePosition + Size > EFI_PAGE_SIZE) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Store the dispatch type and dispatch parameter
  ///
  CurrentPos  = (UINT8 *) mS3Parameter + mS3Parameter->StorePosition;
  *(EFI_PCH_S3_DISPATCH_ITEM_TYPE *) CurrentPos = DispatchItem->Type;
  CurrentPos += TypeSize;
  CopyMem (CurrentPos, DispatchItem->Parameter, ParameterSize);

  ///
  /// Move the store position ahead
  ///
  mS3Parameter->StorePosition += Size;

  ///
  /// Return the S3 Image's entry point
  ///
  *S3DispatchEntryPoint = mPchS3ImageEntryPoint;

  DEBUG ((EFI_D_INFO, "PchS3SetDispatchItem() End\n"));

  return Status;
}

EFI_STATUS
GetImageFromFv (
#if (PI_SPECIFICATION_VERSION < 0x00010000)
  IN  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv,
#else
  IN  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv,
#endif
  IN  EFI_GUID           *NameGuid,
  IN  EFI_SECTION_TYPE   SectionType,
  OUT VOID               **Buffer,
  OUT UINTN              *Size
  )
{
  EFI_STATUS                Status;
  EFI_FV_FILETYPE           FileType;
  EFI_FV_FILE_ATTRIBUTES    Attributes;
  UINT32                    AuthenticationStatus;

  //
  // Read desired section content in NameGuid file
  //
  *Buffer     = NULL;
  *Size       = 0;
  Status      = Fv->ReadSection (
                      Fv,
                      NameGuid,
                      SectionType,
                      0,
                      Buffer,
                      Size,
                      &AuthenticationStatus
                      );

  if (EFI_ERROR (Status) && (SectionType == EFI_SECTION_TE)) {
    //
    // Try reading PE32 section, since the TE section does not exist
    //
    *Buffer = NULL;
    *Size   = 0;
    Status  = Fv->ReadSection (
                    Fv,
                    NameGuid,
                    EFI_SECTION_PE32,
                    0,
                    Buffer,
                    Size,
                    &AuthenticationStatus
                    );
  }

  if (EFI_ERROR (Status) && 
      ((SectionType == EFI_SECTION_TE) || (SectionType == EFI_SECTION_PE32))) {
    //
    // Try reading raw file, since the desired section does not exist
    //
    *Buffer = NULL;
    *Size   = 0;
    Status  = Fv->ReadFile (
                    Fv,
                    NameGuid,
                    Buffer,
                    Size,
                    &FileType,
                    &Attributes,
                    &AuthenticationStatus
                    );
  }

  return Status;
}

EFI_STATUS
GetImageEx (
  IN  EFI_HANDLE         ImageHandle,
  IN  EFI_GUID           *NameGuid,
  IN  EFI_SECTION_TYPE   SectionType,
  OUT VOID               **Buffer,
  OUT UINTN              *Size,
  BOOLEAN                WithinImageFv
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
#if (PI_SPECIFICATION_VERSION < 0x00010000)
  EFI_FIRMWARE_VOLUME_PROTOCOL  *ImageFv;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
#else
  EFI_FIRMWARE_VOLUME2_PROTOCOL *ImageFv;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
#endif

  if (ImageHandle == NULL && WithinImageFv) {
    return EFI_INVALID_PARAMETER;
  }

  Status  = EFI_NOT_FOUND;
  ImageFv = NULL;
  if (ImageHandle != NULL) {
    Status = gBS->HandleProtocol (
               ImageHandle,
               &gEfiLoadedImageProtocolGuid,
               (VOID **) &LoadedImage
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->HandleProtocol (
                    LoadedImage->DeviceHandle,
                  #if (PI_SPECIFICATION_VERSION < 0x00010000)    
                    &gEfiFirmwareVolumeProtocolGuid,
                  #else
                    &gEfiFirmwareVolume2ProtocolGuid,
                  #endif
                    (VOID **) &ImageFv
                    );
    if (!EFI_ERROR (Status)) {
      Status = GetImageFromFv (ImageFv, NameGuid, SectionType, Buffer, Size);
    }
  }

  if (Status == EFI_SUCCESS || WithinImageFv) {
    return Status;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                #if (PI_SPECIFICATION_VERSION < 0x00010000)
                  &gEfiFirmwareVolumeProtocolGuid,
                #else
                  &gEfiFirmwareVolume2ProtocolGuid,
                #endif
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; ++Index) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                  #if (PI_SPECIFICATION_VERSION < 0x00010000)
                    &gEfiFirmwareVolumeProtocolGuid,
                  #else
                    &gEfiFirmwareVolume2ProtocolGuid,
                  #endif
                    (VOID**)&Fv
                    );

    if (EFI_ERROR (Status)) {
      gBS->FreePool(HandleBuffer);
      return Status;
    }

    if (ImageFv != NULL && Fv == ImageFv) {
      continue;
    }

    Status = GetImageFromFv (Fv, NameGuid, SectionType, Buffer, Size);

    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  gBS->FreePool(HandleBuffer);

  //
  // Not found image
  //
  if (Index == HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Load the PCH S3 Image into Efi Reserved Memory below 4G.

  @param[out] ImageEntryPoint     The ImageEntryPoint after success loading

  @retval EFI_STATUS
**/
EFI_STATUS
LoadPchS3Image (
  OUT   UINT32          *ImageEntryPoint
  )
{
  EFI_STATUS                Status;
  UINTN                     PageCount;
  UINTN                     OrgPageCount;
  UINTN                     PrePageCount;
  EFI_PHYSICAL_ADDRESS      DstBuffer;
  EFI_PHYSICAL_ADDRESS      EntryPoint;
  EFI_PE32_IMAGE_PROTOCOL   *PeLoader;
  VOID                      *SrcBuffer;
  UINTN                     SrcSize;
  EFI_HANDLE                ImageHandle;
  PCH_S3_PEIM_VARIABLE      PchS3PeimData;
  EFI_DEVICE_PATH_PROTOCOL  *FvDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FileNameDevicePath;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;

  Status = GetImageEx (
            mImageHandle,
            &mPchS3ImageGuid,
            EFI_SECTION_PE32,
            &SrcBuffer,
            &SrcSize,
            TRUE
            );
  ASSERT_EFI_ERROR (Status);

  //
  // Build a device path to the file in the FV to pass into LoadImage
  //
  Status = gBS->HandleProtocol (
                  mImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **) &LoadedImage
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->HandleProtocol (LoadedImage->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID **)&FvDevicePath);
  ASSERT_EFI_ERROR (Status);

  EfiInitializeFwVolDevicepathNode (&mFvDevicePath.File, &mPchS3ImageGuid);
  FileNameDevicePath = AppendDevicePath (
                          FvDevicePath,
                          (EFI_DEVICE_PATH_PROTOCOL *)&mFvDevicePath
                          );

  //
  // Load PeImage Protocol to Load PE image.
  //
  Status = gBS->LocateProtocol (&gEfiLoadPeImageProtocolGuid, NULL, (VOID **) &PeLoader);
  ASSERT_EFI_ERROR (Status);

  PrePageCount  = 16;
  PageCount     = PrePageCount;

  do {
    OrgPageCount  = PageCount;
    DstBuffer     = 0xFFFFFFFF;
    ///
    /// 4G
    ///
    Status = (gBS->AllocatePages) (AllocateMaxAddress, EfiReservedMemoryType, PageCount, &DstBuffer);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = PeLoader->LoadPeImage (
                        PeLoader,
                        mImageHandle,
                        FileNameDevicePath,
                        SrcBuffer,
                        SrcSize,
                        DstBuffer,
                        &PageCount,
                        &ImageHandle,
                        &EntryPoint,
                        EFI_LOAD_PE_IMAGE_ATTRIBUTE_NONE
                        );
    if (EFI_ERROR (Status)) {
      (gBS->FreePages) (DstBuffer, OrgPageCount);
    } else {
      if (PageCount < PrePageCount) {
        (gBS->FreePages) (DstBuffer + EFI_PAGES_TO_SIZE (PageCount), PrePageCount - PageCount);
      }
    }
  } while (Status == EFI_BUFFER_TOO_SMALL);

  if (!EFI_ERROR (Status)) {
    *ImageEntryPoint                = (UINT32) EntryPoint;

    PchS3PeimData.EntryPointAddress = EntryPoint;
    PchS3PeimData.PeimSize          = PageCount;
    Status = gRT->SetVariable (
                    PCH_S3_PEIM_VARIABLE_NAME,
                    &gPchInitVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (PCH_S3_PEIM_VARIABLE),
                    &PchS3PeimData
                    );
    ASSERT_EFI_ERROR (Status);
  } else {
    *ImageEntryPoint = 0;
  }
  
  FreePool (FileNameDevicePath);
  return Status;
}
