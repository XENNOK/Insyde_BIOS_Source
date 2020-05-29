/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcInstallPciRomSwitchTextMode().
 The function DxeCsSvcInstallPciRomSwitchTextMode() use chipset services to determine
 should to switch to text mode.
  
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/SetupUtility.h>
#include <IndustryStandard/Pci30.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>

/**
 Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
 The chipset platform library can determine if it requires to switch to text
 mode while installing option rom for specific device.

 @param[in]         DeviceHandle        Handle of device OPROM is associated with.
 @param[in, out]    ForceSwitchTextMode Initial value contains the upper level's switch text
                                        mode policy. Set to TRUE if it requires to switch to
                                        text mode.

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER The parameters Address has an invalid value.
                                          1. DeviceHandle is NULL.
*/
EFI_STATUS
InstallPciRomSwitchTextMode (
  IN      EFI_HANDLE                    DeviceHandle,
  IN OUT  BOOLEAN                       *ForceSwitchTextMode
  )
{
  EFI_STATUS                            Status;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PCI_TYPE00                            PciConfigHeader00;
  EFI_SETUP_UTILITY_PROTOCOL            *EfiSetupUtility;
  CHIPSET_CONFIGURATION                  *SystemConfiguration;

  if (DeviceHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (DeviceHandle,
                                &gEfiPciIoProtocolGuid,
                                (VOID **)&PciIo);
  ASSERT_EFI_ERROR (Status);

  Status = PciIo->Pci.Read (PciIo,
                            EfiPciIoWidthUint32,
                            0,
                            sizeof (PciConfigHeader00) / sizeof (UINT32),
                            &PciConfigHeader00);
  ASSERT_EFI_ERROR (Status);

  //
  // For all devices, it requires to switch to text mode by default.
  //
  *ForceSwitchTextMode = TRUE;

  //
  // Check what device option ROM is going to installed
  //
  if ((PciConfigHeader00.Hdr.VendorId == V_PCH_SATA_VENDOR_ID) 
//[-start-120628-IB06460410-modify]//
    && ((PciConfigHeader00.Hdr.DeviceId == V_PCH_LPTH_SATA_DEVICE_ID_D_AHCI)
    || (PciConfigHeader00.Hdr.DeviceId == V_PCH_LPTH_SATA_DEVICE_ID_M_AHCI)
    || (PciConfigHeader00.Hdr.DeviceId == V_PCH_LPTH_SATA_DEVICE_ID_M_AHCI))) {
//[-end-120628-IB06460410-modify]//
    //
    // If AHCI option ROM is dispatching, don't switch to text mode
    //
    *ForceSwitchTextMode = FALSE;
  }
  
//[-start-140224-IB10920078-modify]//
//[-start-120731-IB10820094-modify]//
  if ((FeaturePcdGet(PcdSwitchableGraphicsSupported)) || (FeaturePcdGet(PcdIntelSwitchableGraphicsSupported))) {
    Status = gBS->LocateProtocol (
            &gEfiSetupUtilityProtocolGuid, 
            NULL, 
            (VOID **)&EfiSetupUtility);

    ASSERT_EFI_ERROR (Status);

    SystemConfiguration = (CHIPSET_CONFIGURATION *)EfiSetupUtility->SetupNvData;
    if ((SystemConfiguration->PrimaryDisplay == DISPLAY_MODE_HYBRID) &&
        (SystemConfiguration->IGDControl != IGD_DISABLE) &&
        (SystemConfiguration->EnSGFunction == SG_ENABLE)) {
      //
      // When in Hybrid mode, we don't switch video to text mode
      //
      *ForceSwitchTextMode = FALSE;
    }
  }
//[-end-120731-IB10820094-modify]//
//[-end-140224-IB10920078-modify]//
  return EFI_SUCCESS;
}
