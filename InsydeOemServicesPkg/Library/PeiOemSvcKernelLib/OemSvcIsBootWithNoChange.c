/** @file
  Determine if "Boot with no change" is true according to project characteristic.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcKernelLib.h>

/**
  Determine if "Boot with no change" is true according to project characteristic.

  @param[in, out]  *IsNoChange        If IsNoChange == TRUE, then boot mode will be set to 
                                      BOOT_ASSUMING_NO_CONFIGURATION_CHANGES which might reduce the POST time.

  @retval          EFI_UNSUPPORTED    Returns unsupported by default.
  @retval          EFI_SUCCESS        The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
  @retval          Others             Depends on customization.
**/
EFI_STATUS
OemSvcIsBootWithNoChange (
  IN OUT BOOLEAN                               *IsNoChange
  )
{

  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
