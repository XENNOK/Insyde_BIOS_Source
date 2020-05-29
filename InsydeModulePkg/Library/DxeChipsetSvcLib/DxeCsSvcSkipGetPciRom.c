/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcSkipGetPciRom().
 The function DxeCsSvcSkipGetPciRom() use chipset services to return 
 a PCI ROM image for the device is represented or not.
	
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

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
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcSkipGetPciRom (
  IN  UINTN        Segment,
  IN  UINTN        Bus,
  IN  UINTN        Device,
  IN  UINTN        Function,
  IN  UINT16       VendorId,
  IN  UINT16       DeviceId,
  OUT BOOLEAN     *SkipGetPciRom
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, SkipGetPciRom) + sizeof (VOID*)) || 
      mChipsetSvc->SkipGetPciRom == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member SkipGetPciRom() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->SkipGetPciRom (Segment, Bus, Device, Function, VendorId, DeviceId, SkipGetPciRom);   
}  
