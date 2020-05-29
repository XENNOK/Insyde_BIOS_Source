/** @file

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
#include <FastRecoveryData.h>
#include <Ppi/EmuPei.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <PostCode.h>
#include <Library/PeiChipsetSvcLib.h>

EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  );

/**
  Install Firmware Volume Hob's once there is main memory.It installs
  firmware volume Hobs when main memory is available.

  @param PeiServices            General purpose services available to every PEIM.
  @param NotifyDescriptor       Pointer to EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi                    EFI_PEI_FV_FILE_LOADER_PPI

  @retval   EFI_SUCCESS         if the interface could be successfully installed.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyPlatformStage2Callback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                          Status;
  EFI_BOOT_MODE                       BootMode;
  FAST_RECOVERY_DXE_TO_PEI_DATA       *DxeToPeiData = NULL;

  if (FeaturePcdGet (PcdUseFastCrisisRecovery)) {
    Status = PeiServicesLocatePpi (
               &gEmuPeiPpiGuid,
               0,
               NULL,
               (VOID *)&DxeToPeiData
               );
    if (EFI_ERROR (Status)) {
      //
      // We are not in EmuPei Phase.
      //
      DxeToPeiData = NULL;
    }
  }

  POST_CODE (PEI_MEMORY_CALLBACK);
  DEBUG ((EFI_D_INFO, "Enter MemoryDiscoveredPpiNotifyCallback\n"));

  //
  // Get boot mode
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    POST_CODE (S3_MEMORY_CALLBACK);
  }

  PeiCsSvcStage2MemoryDiscoverCallback (&BootMode);

  //
  // OemServices
  //
  OemSvcInitMemoryDiscoverCallback (&BootMode);

  //
  // Create the firmware volume HOB's
  //
  DEBUG ((EFI_D_INFO, "Create the firmware volume HOB\n"));
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    //
    // Prepare the recovery service
    //
    DEBUG ((EFI_D_INFO, "Prepare the recovery service\n"));
    Status = PeimInitializeRecovery ();
    ASSERT_EFI_ERROR (Status);
  } else if (BootMode != BOOT_ON_S3_RESUME) {
    BuildFvHob (
      FixedPcdGet32 (PcdFlashFvMainBase),
      FixedPcdGet32 (PcdFlashFvMainSize)
      );

//[-start-130625-IB10920029-modify]//
    if (FeaturePcdGet (PcdUnsignedFvSupported)) {
      BuildFvHob (
        FixedPcdGet32 (PcdFlashUnsignedFvRegionBase),
        FixedPcdGet32 (PcdEndOfFlashUnsignedFvRegionTotalSize)
        );
    }
//[-end-130625-IB10920029-modify]//
 
    BuildFvHob (
      FixedPcdGet32 (PcdFlashNvStorageVariableBase),
      (FixedPcdGet32 (PcdFlashNvStorageVariableSize) 
       + FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize) 
       + FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize) 
       + FixedPcdGet32 (PcdFlashNvStorageFactoryCopySize))
    );
  }

  BuildResourceDescriptorHob (
    EFI_RESOURCE_FIRMWARE_DEVICE,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    FixedPcdGet32 (PcdFlashAreaBaseAddress),
    FixedPcdGet32 (PcdFlashAreaSize)
    );

  //
  // Create a CPU hand-off information
  //
  BuildCpuHob (FixedPcdGet8 (PcdMemorySpaceSize), FixedPcdGet8 (PcdIoSpaceSize));
  DEBUG ((EFI_D_INFO, "MemoryDiscoveredPpiNotifyCallback Exit\n"));

  return Status;
}

/**
  To force system into Recovery mode.

  @param PeiServices            General purpose services available to every PEIM.
  @param NotifyDescriptor       Pointer to EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi                    EFI_PEI_FV_FILE_LOADER_PPI

  @retval EFI_SUCCESS           Enter Recovery mode OK.

**/
EFI_STATUS
ForceEnterRecoveryEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  //
  // Due to there is no common method to set a recovery request, so let OEM to decide the method.
  // (OemServices)
  //
  OemSvcSetRecoveryRequest ();

  //
  // Trigger a reset to force system enter recovery mode.
  //
  PeiServicesResetSystem ();

  return EFI_SUCCESS;
}
