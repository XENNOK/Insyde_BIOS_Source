/** @file
  Defines data structure that is the volume header found. These data is
  intented to decouple FVB driver with FV header.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
//
// The protocols, PPI and GUID defintions for this module
//
#include <Guid/EventGroup.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/SystemNvDataGuid.h>
#include <Guid/IrsiFeature.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/DevicePath.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FvRegionInfo.h>
//
// The Library classes this module consumes
//
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>

EFI_EVENT                        mFvbEvent;
VOID                             *mFvbReg;

FV_REGION_INFO mVariableRegionInfo[] =
{
  //
  // FV region information
  //
  {
    IRSI_VARIABLE_IMAGE_GUID,
    0,                         
    FixedPcdGet32 (PcdFlashNvStorageVariableSize)
  },
  {                                                      
    IRSI_FACTORY_COPY_IMAGE_GUID,
    FixedPcdGet32 (PcdFlashNvStorageVariableSize) +
    FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) +
    FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize),
    FixedPcdGet32 (PcdFlashNvStorageFactoryCopySize)
  },
  //
  // The end of Region Info list must be with a NULL GUID
  //
  {
    IRSI_NULL_IMAGE_GUID,
    0,
    0
  }
};


/**
  Notification function of Firmware Volume Block Protocol

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context

  @retval None
**/
VOID
EFIAPI
FvbProtocolNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *Fvb;
  EFI_STATUS                         Status;
  UINTN                              Size;
  EFI_FIRMWARE_VOLUME_HEADER         FvHeader;
  EFI_HANDLE                         *FvbHandles;
  UINTN                              FvbIndex;
  UINTN                              NumberOfFvbs;

  Status = gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolumeBlockProtocolGuid,
        NULL,
        &NumberOfFvbs,
        &FvbHandles
        );
        
  if (EFI_ERROR(Status)) {
    return;
  }      
  for (FvbIndex = 0; FvbIndex < NumberOfFvbs; FvbIndex++) {
    Status = gBS->HandleProtocol (
              FvbHandles[FvbIndex],
              &gEfiFirmwareVolumeBlockProtocolGuid,
              (VOID **)&Fvb
              );
    if (EFI_ERROR(Status)) {
      ASSERT_EFI_ERROR(Status);
      continue;
    }
    Size = sizeof(EFI_FIRMWARE_VOLUME_HEADER);
    Status = Fvb->Read(Fvb, 0, 0, &Size, (UINT8 *)&FvHeader);
    if (EFI_ERROR(Status)) {
      ASSERT_EFI_ERROR(Status);
      continue;
    } 
    
    if (CompareGuid(&FvHeader.FileSystemGuid, &gEfiSystemNvDataFvGuid)) {
       Status = gBS->InstallProtocolInterface (
                    &FvbHandles[FvbIndex],
                    &gFvRegionInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    mVariableRegionInfo
       );
       ASSERT_EFI_ERROR(Status);
       gBS->CloseEvent(Event);
       break;
    }
  }
}

/**
  VariableRegionInfo module entry point


  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS:          Module initialized successfully
  @retval Others                Module initialized unsuccessfully
**/
EFI_STATUS
EFIAPI
VariableRegionInfoInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  mFvbEvent = EfiCreateProtocolNotifyEvent  (
                          &gEfiFirmwareVolumeBlockProtocolGuid,
                          TPL_NOTIFY,
                          FvbProtocolNotify,
                          NULL,
                          &mFvbReg
                          );
                          
  return EFI_SUCCESS;                        

}