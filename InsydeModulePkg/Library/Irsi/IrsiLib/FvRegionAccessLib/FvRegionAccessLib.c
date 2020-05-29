/** @file
  Library Instance implementation for FV Region Read/Write

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
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IrsiLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareManagement.h>
#include <Protocol/LoadedImage.h>

#define GET_IMAGE_PROGRESS_NAME   L"GetImageProgress"

EFI_GUID mFirmwareManagementRuntimeDxeFileGuid = { 0xb795675e, 0x7583, 0x4f98, { 0xac, 0x6a, 0xf9, 0xdd, 0xbb, 0xcc, 0x32, 0xe8 } };

EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *mFirmwareManagement = NULL;
UINT8                             *mProgress;

/**
  Get the ImageIndex for the specified ImageTypeGuid

  @param[in]      ImageTypeGuid  Pointer to the image type GUID
  
  @return                        The ImageIndex for the specified ImageTypeGuid
                                 A value of 0xFF means the Image cannot be found
*/
UINT8
EFIAPI
GetImageIndex (
  EFI_GUID *ImageTypeGuid
  )
{
  UINTN                           ImageInfoSize;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR   *ImageInfo;
  UINT32                          DescriptorVersion;
  UINT8                           DescriptorCount;
  UINTN                           DescriptorSize;
  UINT8                           *IrsiBuf;
  UINTN                           IrsiBufSize;
  UINTN                           Index;
  EFI_STATUS                      Status;

  if (mFirmwareManagement == NULL) {
    ASSERT (mFirmwareManagement != NULL);
    return 0xFF;
  }
  
  ImageInfoSize = 0;
  DescriptorCount = 0;
  ImageInfo = NULL;
  Status = mFirmwareManagement->GetImageInfo (
                         mFirmwareManagement,
                         &ImageInfoSize,
                         ImageInfo,
                         &DescriptorVersion,
                         &DescriptorCount,
                         &DescriptorSize,
                         NULL,
                         NULL
                       );
   if (Status != EFI_BUFFER_TOO_SMALL) {
     ASSERT(Status == EFI_BUFFER_TOO_SMALL);
     return 0xFF;
   }
   
  IrsiGetRuntimeBuffer((VOID **)&IrsiBuf, &IrsiBufSize);
  ASSERT (IrsiBuf != NULL);
  
  ImageInfo = (EFI_FIRMWARE_IMAGE_DESCRIPTOR *)IrsiBuf;
  Status = mFirmwareManagement->GetImageInfo (
                         mFirmwareManagement,
                         &ImageInfoSize,
                         ImageInfo,
                         &DescriptorVersion,
                         &DescriptorCount,
                         &DescriptorSize,
                         NULL,
                         NULL
                       );

  if (EFI_ERROR(Status)) {
    return 0xFF;
  }
  
  for (Index = 0; Index < (UINTN)DescriptorCount; Index++) {
    if (CompareGuid(ImageTypeGuid, &ImageInfo[Index].ImageTypeId)) {
      return ImageInfo[Index].ImageIndex;
    }
  }
  return 0xFF;
}

/**
  Update percentage of progress for FirmwareManagementProtocol.SetImage()

  @param[in]      Completion     Percentage (1 to 100) of completion
                                 A zero value means the completion status is not supported

  @retval         EFI_SUCCESS    function returns successfully
*/
EFI_STATUS
EFIAPI
UpdateProgress (
  IN UINTN Completion
  )
{
  if (mProgress != NULL) {
    *mProgress = (UINT8)Completion;
  }
  return EFI_SUCCESS;
}

/**
  ReadFvRegion function

  This routine reads image from platform with specific image GUID defined in
  RegionBuf

  @param[in]      ImageTypeGuid  Pointer to the image type GUID
  @param[in, out] ImageSize      Pointer to the read image size
  @param[in]      RegionBuf      The region buffer
  @param[out]     Progress       The current progress update (0 to 100)

  @retval EFI_SUCCESS            The FV region was successfully retrieved
  @retval EFI_INVALID_PARAMETER  Invalid parameter list
  @retval EFI_BUFFER_TOO_SMALL   The given ImageSize is too small
  @retval EFI_UNSUPPORTED        Unsupported for the specified image type
  @return others                 FV Region read failed

**/
EFI_STATUS
EFIAPI
ReadFvRegion (
  IN EFI_GUID            *ImageTypeGuid,
  IN OUT UINTN           *ImageSize,
  OUT VOID               *RegionBuf,
  OUT UINT8              *Progress
  )
{
  EFI_STATUS                                     Status;
  UINT8                                          ImageIndex;
  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  ProgressFun;

  ImageIndex = GetImageIndex (ImageTypeGuid);
  if (ImageIndex == 0xFF) {
    return EFI_UNSUPPORTED;
  }
  mProgress = Progress;
  if (Progress != NULL) {
    *Progress = 0;
    ProgressFun = UpdateProgress;
    Status = gRT->SetVariable (
                 GET_IMAGE_PROGRESS_NAME,
                 &gEfiFirmwareManagementProtocolGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                 sizeof (EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS),
                 &ProgressFun
                 );
  }

  Status = mFirmwareManagement->GetImage (
                         mFirmwareManagement,
                         ImageIndex,
                         RegionBuf,
                         ImageSize
                       );
  return Status;
}


/**
  WriteFvRegion function

  This routine write image to platform with specific capsule GUID defined in
  RegionBuf

  @param[in]      ImageTypeGuid  Pointer to the image type GUID
  @param[in, out] ImageSize      Pointer to the read image size
  @param[in]      RegionBuf      The region buffer
  @param[out]     Progress       The current progress update (0 to 100)

  @retval EFI_SUCCESS            The FV region was successfully written
  @retval EFI_INVALID_PARAMETER  Invalid parameter list
  @retval EFI_BUFFER_TOO_SMALL   The given ImageSize is too small
  @retval EFI_UNSUPPORTED        Unsupported for the specified image type
  @return others                 FV Region write failed

**/
EFI_STATUS
EFIAPI
WriteFvRegion (
  IN EFI_GUID           *ImageTypeGuid,
  IN UINTN              ImageSize,
  IN CONST VOID         *RegionBuf,
  OUT UINT8             *Progress
  )
{
  CHAR16      *AbortReason;
  EFI_STATUS  Status;
  UINT8       ImageIndex;
  UINT8       Sum8;
  
  AbortReason = NULL;
  mProgress = Progress;
  ImageIndex = GetImageIndex (ImageTypeGuid);
  if (ImageIndex == 0xFF) {
    return EFI_UNSUPPORTED;
  }
  
  Sum8 = CalculateSum8(RegionBuf, ImageSize);
  AbortReason = NULL;
  Status = mFirmwareManagement->SetImage (
                         mFirmwareManagement,
                         ImageIndex,
                         RegionBuf,
                         ImageSize,
                         &Sum8,
                         UpdateProgress,
                         &AbortReason
                       );
  if (AbortReason != NULL) {
    FreePool (AbortReason);
  }
  return Status;
}

/**
  Find Firmware Management Protocol instance

  @retval    Firmware Management Protocol instance
**/
EFI_FIRMWARE_MANAGEMENT_PROTOCOL*
EFIAPI
FindFirmwareManagementInstance (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_GUID                          FileName;
  EFI_LOADED_IMAGE_PROTOCOL         *LoadedImage;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *FirmwareManagement;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             HandleCount;
  UINTN                             Index;
  BOOLEAN                           Found;

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
    return NULL;
  }

  Found = FALSE;
  FirmwareManagement = NULL;
  for (Index = 0; Index < HandleCount && !Found; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiFirmwareManagementProtocolGuid, (VOID **) &FirmwareManagement);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiLoadedImageProtocolGuid, (VOID **) &LoadedImage);
    if (EFI_ERROR (Status)) {
      continue;
    }

    FileName = ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) (LoadedImage->FilePath))->FvFileName;

    //
    // Find the instance of FMP that distributes by FirmwareManagementRuntimeDxe
    //
    if (CompareGuid(&FileName, &mFirmwareManagementRuntimeDxeFileGuid)) {
      Found = TRUE;
    }
  }

  if (!Found) {
    FirmwareManagement = NULL;
  }
  
  if (HandleCount != 0) {
    FreePool(HandleBuffer);
  }

  return FirmwareManagement;
}

/**
  Notification event handler to save Firmware Management Protocol instance

  @param[in] Event              The Event whose notification function is being invoked.
  @param[in] Context            Pointer to the notification function's context.

  @retval    None
**/
VOID
EFIAPI
FirmwareManagementNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  mFirmwareManagement = FindFirmwareManagementInstance ();
  if (mFirmwareManagement != NULL) {
    IrsiAddVirtualPointer((VOID **)&mFirmwareManagement);
    gBS->CloseEvent(Event);
  }
}

/**
  FvRegionAccessLib Initialization

  This routine is a LibraryClass constructor for FvRegionAccessLib

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully

**/
EFI_STATUS
EFIAPI
FvRegionAccessLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  VOID                           *Registration;
  EFI_EVENT                      Event;

  mFirmwareManagement = FindFirmwareManagementInstance ();
  if (mFirmwareManagement == NULL) {
    Registration = NULL;
    Event = EfiCreateProtocolNotifyEvent (
                          &gEfiFirmwareManagementProtocolGuid,
                          TPL_CALLBACK,
                          FirmwareManagementNotify,
                          NULL,
                          &Registration
                          );
    ASSERT (Event != NULL);
  } else {
    IrsiAddVirtualPointer ((VOID **)&mFirmwareManagement);
  }
  IrsiAddVirtualPointer ((VOID **)&gRT);
  return EFI_SUCCESS;
}
