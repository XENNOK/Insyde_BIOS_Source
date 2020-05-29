/** @file
  CPU/Chipset/Platform Initialization when PeiMemoryDiscoveredPpi be installed according to project characteristic.

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
  CPU/Chipset/Platform Initialization when PeiMemoryDiscoveredPpi be installed according to project characteristic.

  @param[in, out]  *BootMode          A pointer to EFI_BOOT_MODE.

  @retval          EFI_UNSUPPORTED    Returns unsupported by default.
  @retval          EFI_SUCCESS        The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
  @retval          Others             Depends on customization.
**/
EFI_STATUS
OemSvcInitMemoryDiscoverCallback (
  IN OUT  EFI_BOOT_MODE                         *BootMode
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
