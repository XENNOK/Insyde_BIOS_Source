/** @file

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

#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VariableLib.h>

#include <Ppi/MemoryDiscovered.h>
#include <SecureFlash.h>   //For SecureFlash feature

#define  MEMORY_SIZE_FOR_RESERVED          PcdGet32 (PcdReservedMemorySizeForSecureFlash)
#define  MEMORY_SIZE_4KB                   0x1000

EFI_STATUS
EFIAPI
MemoryReservedCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR  mMemoryReservedNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  MemoryReservedCallback
  };

EFI_STATUS
EFIAPI
SecureFlashPeiMain (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{

  return PeiServicesNotifyPpi (&mMemoryReservedNotify);

}

/**
 (1)Set memory type to "EFI_RESOURCE_MEMORY_RESERVED" so it won't be destroy during DXE phase.
 (2)Save image information to Hob.


 @retval None

**/
EFI_STATUS
EFIAPI
MemoryReservedCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  IMAGE_INFO                      ImageInfo;
  EFI_PHYSICAL_ADDRESS            ReservedMemAddress;


  //
  // Allocate a reserved memory for Secure Flash
  //
  Size = MEMORY_SIZE_FOR_RESERVED / MEMORY_SIZE_4KB;
  if ((MEMORY_SIZE_FOR_RESERVED % MEMORY_SIZE_4KB) != 0) {
    Size++;
  }
  Status = (*PeiServices)->AllocatePages (
                             (CONST EFI_PEI_SERVICES **)PeiServices,
                             EfiACPIMemoryNVS,
                             Size,
                             &ReservedMemAddress
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Size = sizeof (ImageInfo);
  Status = CommonGetVariable (
             SECURE_FLASH_INFORMATION_NAME,
             &gSecureFlashInfoGuid,
             &Size,
             &ImageInfo
             );

  if (!(EFI_ERROR(Status)) && (ImageInfo.FlashMode)) {
    CopyMem ((VOID*)(UINTN)ReservedMemAddress, (VOID*)(UINTN)ImageInfo.ImageAddress, ImageInfo.ImageSize);
  } else {
    ImageInfo.ImageSize = MEMORY_SIZE_FOR_RESERVED;
  }
  ImageInfo.ImageAddress = (UINTN)ReservedMemAddress;
  //
  // Save the data of ImageAddress and ImageSize to Hob.
  // Because it cannot SetVariable in PEI phase.
  //
  BuildCvHob (ImageInfo.ImageAddress, ImageInfo.ImageSize);

  return EFI_SUCCESS;
}
