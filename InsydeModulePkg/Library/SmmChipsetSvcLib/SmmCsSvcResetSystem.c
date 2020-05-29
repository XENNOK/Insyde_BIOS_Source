/** @file
 SMM Chipset Services Library.

 This file contains only one function that is SmmCsSvcResetSystem().
 The function SmmCsSvcResetSystem() use chipset services to reset system.

***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
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
 Reset the system. For details, see UefiSpec.h

 @param[in]         ResetType           warm or cold.

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If ResetType is invalid.
*/
EFI_STATUS
SmmCsSvcResetSystem (
  IN EFI_RESET_TYPE   ResetType
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mSmmChipsetSvc == NULL ||
      mSmmChipsetSvc->Size < (OFFSET_OF (H2O_SMM_CHIPSET_SERVICES_PROTOCOL, ResetSystem) + sizeof (VOID*)) || 
      mSmmChipsetSvc->ResetSystem == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O SMM Chipset Services can not be found or member ResetSystem() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mSmmChipsetSvc->ResetSystem (ResetType);
}  
