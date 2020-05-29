/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcSetSmbiosMemoryData().
 The function DxeCsSvcSetSmbiosMemoryData() use chipset services to collection
 information of memory and set to SMBIOS.
	
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
 To collection information of memory and set to SMBIOS.

 @param[in, out]    ChipsetSmbiosMemData Point to SMBIOS memory data

 @retval            EFI_SUCCESS         This function always return successfully.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcSetSmbiosMemoryData (
  IN OUT VOID        *ChipsetSmbiosMemData
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, SetSmbiosMemoryData) + sizeof(VOID*)) || 
      mChipsetSvc->SetSmbiosMemoryData == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member SetSmbiosMemoryData() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->SetSmbiosMemoryData (ChipsetSmbiosMemData);   
}  
