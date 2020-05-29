/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcProgramChipsetSsid().
 The function DxeCsSvcProgramChipsetSsid() use chipset services to program subsystem vendor identification.
	
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
 Program Chipset SSID

 @param[in]         Bus                 PCI Bus number
 @param[in]         Dev                 PCI Device number
 @param[in]         Func                PCI Function number
 @param[in]         VendorId            Vendor ID
 @param[in]         DeviceId            Device ID
 @param[in]         SsidVid             SsidVid

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     1. The specific ID is not find.
                                        2. The specific device can not be set SSID.
                                        3. If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcProgramChipsetSsid (
  UINT8                        Bus,
  UINT8                        Dev,
  UINT8                        Func,
  UINT16                       VendorId,
  UINT16                       DeviceId,
  UINT32                       SsidSvid
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, ProgramChipsetSsid) + sizeof (VOID*)) || 
      mChipsetSvc->ProgramChipsetSsid == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member ProgramChipsetSsid() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->ProgramChipsetSsid (Bus, Dev, Func, VendorId, DeviceId, SsidSvid);   
}  
