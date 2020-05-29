/** @file
  This OemService is queried to assign the default boot mode. 
  OEM can use this service to assign default boot mode, 
  and use the parameter SkipPriorityPolicy to control normal boot mode identification. 
  If SkipPriorityPolicy be set as TRUE, the final boot mode is the same as the parameter "bootmode" which is assigned by this service.

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
  This OemService is queried to assign the default boot mode. 
  OEM can use this service to assign default boot mode, 
  and use the parameter SkipPriorityPolicy to control normal boot mode identification. 
  If SkipPriorityPolicy be set as TRUE, the final boot mode is the same as the parameter "bootmode" which is assigned by this service.

  @param[in, out]  *BootMode           Assign default boot mode.
  @param[in, out]  *SkipPriorityPolicy If SkipPriorityPolicy == TRUE, then normal boot mode identification will be skipped.

  @retval          EFI_UNSUPPORTED     Returns unsupported by default.
  @retval          EFI_SUCCESS         The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED   The value of IN OUT parameter is changed. 
  @retval          Others              Depends on customization.
**/
EFI_STATUS
OemSvcChangeBootMode (
  IN OUT EFI_BOOT_MODE                      *BootMode,
  OUT    BOOLEAN                            *SkipPriorityPolicy
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
