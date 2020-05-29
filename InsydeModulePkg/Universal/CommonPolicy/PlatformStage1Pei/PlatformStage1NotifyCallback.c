/** @file
  Provide a chipset/ common interface after permanent memory discovery.

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

#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <PostCode.h>
#include <Library/PeiChipsetSvcLib.h>

/**
  Install Firmware Volume Hob's once there is main memory.It installs
  firmware volume Hobs when main memory is available.

  @param[in]  **PeiServices,
  @param[in]  *NotifyDescriptor,
  @param[in]  *Ppi
  
  @retval   EFI_SUCCESS - if the interface could be successfully installed.
          
**/
EFI_STATUS
EFIAPI
MemoryDiscoveredNotifyPlatformStage1Callback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS            Status;
  EFI_BOOT_MODE         BootMode;


//  POST_CODE (PEI_MEMORY_CALLBACK);
  DEBUG ((EFI_D_INFO, "Enter MemoryDiscoveredPpiNotifyCallback\n"));

  //
  //Get boot mode
  //
  Status = (**PeiServices).GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    POST_CODE (S3_MEMORY_CALLBACK);
  }

  PeiCsSvcStage1MemoryDiscoverCallback (&BootMode);

  DEBUG ((EFI_D_INFO, "MemoryDiscoveredPpiNotifyCallback Exit\n"));

  return Status;
}

/**
  Use the PEI_READ_ONLY_VARIABLE_ACCESS_PPI to notify and set platform hardware switch value.

  @param  **PeiServices         General purpose services available to every PEIM.
  @param  *NotifyDescriptor     Pointer to EFI_PEI_NOTIFY_DESCRIPTOR.
  @param  *Ppi                  EFI_PEI_FV_FILE_LOADER_PPI
  
  @retval EFI_SUCCESS           if the interface could be successfully installed.
**/
EFI_STATUS
EFIAPI
SetPlatformHardwareSwitchNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                     Status;
  PLATFORM_HARDWARE_SWITCH       PlatformHardwareSwitch;
  PLATFORM_HARDWARE_SWITCH       EmptyPlatformHardwareSwitch;
  PLATFORM_HARDWARE_SWITCH       *PlatformHardwareSwitchDptr;

  ZeroMem (&PlatformHardwareSwitch, sizeof (PLATFORM_HARDWARE_SWITCH));
  ZeroMem (&EmptyPlatformHardwareSwitch, sizeof (PLATFORM_HARDWARE_SWITCH));

  PlatformHardwareSwitchDptr = &PlatformHardwareSwitch;

  PeiCsSvcSetPlatformHardwareSwitch(&PlatformHardwareSwitchDptr);
  //
  // OemServices
  //
  Status = OemSvcSetPlatformHardwareSwitch (
             &PlatformHardwareSwitchDptr
             );

  if (!EFI_ERROR (Status)) {
    return Status;
  }

  if (CompareMem (
	&EmptyPlatformHardwareSwitch, 
	&PlatformHardwareSwitch, 
	sizeof (PLATFORM_HARDWARE_SWITCH)) == 0) {
    return Status;
  }

  //
  // Set PlatformHardwareSwitch data to HOB.
  //
  BuildGuidDataHob (
    &gPlatformHardwareSwitchGuid, 
    PlatformHardwareSwitchDptr, 
    sizeof (PLATFORM_HARDWARE_SWITCH)
    );

  return Status;
}
