/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcGetBiosProtectTable().
 The function DxeCsSvcGetBiosProtectTable() use chipset services to return 
 a Bios Protect Region Table.
	
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

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

/**
 Provide bios protect table for Kernel.

 @param[in, out]        *BiosRegionTable    Pointer to BiosRegion Table.
 @param[in, out]        ProtectRegionNum    The number of Bios protect region instances.

 @retval                EFI_SUCCESS         Provide table for kernel to set protect region and lock flash program registers.
*/
EFI_STATUS
DxeCsSvcGetBiosProtectTable (
  IN OUT BIOS_PROTECT_REGION           **BiosRegionTable,
  IN OUT UINT8                         *ProtectRegionNum
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, GetBiosProtectTable) + sizeof (VOID*)) || 
      mChipsetSvc->GetBiosProtectTable == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member GetBiosProtectTable() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->GetBiosProtectTable (BiosRegionTable, ProtectRegionNum); 
}  


