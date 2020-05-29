/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcSataComReset().
 The function DxeCsSvcSataComReset() use chipset services to reset specified SATA port.
	
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
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset
                    
 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcSataComReset (
  IN  UINTN                    PortNumber
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, SataComReset) + sizeof(VOID*)) || 
      mChipsetSvc->SataComReset == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member SataComReset() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->SataComReset (PortNumber);   
}  
