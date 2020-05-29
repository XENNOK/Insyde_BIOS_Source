/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcSkipGetPciRom().
 The function DxeCsSvcSkipGetPciRom() use chipset services to return 
 a PCI ROM image for the device is represented or not.
	
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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/PciExpressLib.h>
#include <Protocol/SetupUtility.h>
#include <IndustryStandard/Pci30.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>

/**
 Skip to load PCI ROM for specify device.

 @param[in]         Segment             The PCI segment number for PCI device.
 @param[in]         Bus                 The PCI bus number for PCI device.
 @param[in]         Device              The PCI device number for PCI device.
 @param[in]         Function            The PCI function number for PCI device.
 @param[in]         VendorId            The vendor ID for PCI device.
 @param[in]         DeviceId            The device ID for PCI device.
 @param[in, out]    SkipGetPciRom       If SkipGetPciRom == TRUE means that there is no ROM in this device.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            EFI_PROTOCOL_ERROR  Can't find setup utility protocol.
*/
EFI_STATUS
SkipGetPciRom (
  IN  UINTN        Segment,
  IN  UINTN        Bus,
  IN  UINTN        Device,
  IN  UINTN        Function,
  IN  UINT16       VendorId,
  IN  UINT16       DeviceId,
  OUT BOOLEAN     *SkipGetPciRom
  )
{
  EFI_STATUS                  Status;
  UINT16                      DeviceClass;

  EFI_SETUP_UTILITY_PROTOCOL  *EfiSetupUtility;
  CHIPSET_CONFIGURATION        *SystemConfiguration = NULL;

  //
  //Get NVRAM data
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&EfiSetupUtility);
  if (EFI_ERROR (Status)) {
    return EFI_PROTOCOL_ERROR;
  }

  SystemConfiguration = (CHIPSET_CONFIGURATION *)EfiSetupUtility->SetupNvData;

  DeviceClass = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Function, 0x0A));

  //
  // Do not run AHCI Option ROM if AhciOptionRomSupport is disabled
  //
  if ((Bus == 0) && (Device == PCI_DEVICE_NUMBER_PCH_SATA2) && (Function == PCI_FUNCTION_NUMBER_PCH_SATA)) {
    if ((DeviceClass == ((PCI_CLASS_MASS_STORAGE << 8) | PCI_CLASS_MASS_STORAGE_SATADPA)) &&
         SystemConfiguration->AhciOptionRomSupport == 0 ) {
      *SkipGetPciRom = TRUE;
    }
  } else {
    //
    // Run PXE ROM only if Boot network is enabled
    //
    if ((SystemConfiguration->PxeBootToLan == 0) &&
         (DeviceClass == ((PCI_CLASS_NETWORK << 8) | PCI_CLASS_NETWORK_ETHERNET))
       ) {
      *SkipGetPciRom = TRUE;
    }
  }

  return EFI_SUCCESS;
}
