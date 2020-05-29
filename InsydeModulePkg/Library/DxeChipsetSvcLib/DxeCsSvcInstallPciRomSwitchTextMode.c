/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcInstallPciRomSwitchTextMode().
 The function DxeCsSvcInstallPciRomSwitchTextMode() use chipset services to determine
 should to switch to text mode.
	
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
 Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
 The chipset platform library can determine if it requires to switch to text
 mode while installing option rom for specific device.

 @param[in]         DeviceHandle        Handle of device OPROM is associated with.
 @param[in, out]    ForceSwitchTextMode Initial value contains the upper level's switch text
                                        mode policy. Set to TRUE if it requires to switch to
                                        text mode.

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER DeviceHandle is NULL.
 @retval            EFI_UNSUPPORTED       If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcInstallPciRomSwitchTextMode (
  IN      EFI_HANDLE         DeviceHandle,
  IN OUT  BOOLEAN           *ForceSwitchTextMode
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, InstallPciRomSwitchTextMode) + sizeof(VOID*)) || 
      mChipsetSvc->InstallPciRomSwitchTextMode == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member InstallPciRomSwitchTextMode() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->InstallPciRomSwitchTextMode (DeviceHandle, ForceSwitchTextMode);   
}  
