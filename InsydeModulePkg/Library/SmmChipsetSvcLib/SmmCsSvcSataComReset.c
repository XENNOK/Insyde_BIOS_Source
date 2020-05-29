/** @file
 SMM Chipset Services Library.

 This file contains only one function that is SmmCsSvcSataComReset().
 The function SmmCsSvcSataComReset() use chipset services to reset system.

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

#include <PiSmm.h>
#include <Library/DebugLib.h>
#include <Protocol/H2OSmmChipsetServices.h>

extern H2O_SMM_CHIPSET_SERVICES_PROTOCOL *mSmmChipsetSvc;

/**
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset
                    
 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
SmmCsSvcSataComReset (
  IN  UINTN         PortNumber
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mSmmChipsetSvc == NULL ||
      mSmmChipsetSvc->Size < (OFFSET_OF (H2O_SMM_CHIPSET_SERVICES_PROTOCOL, SataComReset) + sizeof (VOID*)) || 
      mSmmChipsetSvc->SataComReset == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O SMM Chipset Services can not be found or member SataComReset() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mSmmChipsetSvc->SataComReset (PortNumber);
}  
