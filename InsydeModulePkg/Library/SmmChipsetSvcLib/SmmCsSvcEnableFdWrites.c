/** @file
 SMM Chipset Services Library.

 This file contains only one function that is SmmCsSvcEnableFdWrites().
 The function SmmCsSvcEnableFdWrites() use chipset services to enable/disable 
 flash device write access.

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
 Platform specific function to enable/disable flash device write access

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented
 @retval            others              Error occurs
*/
EFI_STATUS
SmmCsSvcEnableFdWrites (
  IN  BOOLEAN           EnableWrites
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mSmmChipsetSvc == NULL ||
      mSmmChipsetSvc->Size < (OFFSET_OF (H2O_SMM_CHIPSET_SERVICES_PROTOCOL, EnableFdWrites) + sizeof (VOID*)) || 
      mSmmChipsetSvc->EnableFdWrites == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O SMM Chipset Services can not be found or member EnableFdWrites() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mSmmChipsetSvc->EnableFdWrites (EnableWrites);   
}  
