/** @file
  Provide OEM to customize resolution and SCU resolution. .

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  Get OEM customization resolution and SCU resolution. 
  Customization resolution provides OEM to define logo resolution directly or call GetLogoResolution () function to get logo resolution automatically.

  @param[out]  **OemLogoResolutionTable    A double pointer to OEM_LOGO_RESOLUTION_DEFINITION

  @retval      EFI_UNSUPPORTED             Returns unsupported by default.
  @retval      EFI_SUCCESS                 Get resolution information successful.
  @retval      EFI_MEDIA_CHANGED           The value of IN OUT parameter is changed. 
  @retval      Others                      Base on OEM design.
**/
EFI_STATUS
OemSvcLogoResolution (
  IN OUT OEM_LOGO_RESOLUTION_DEFINITION    **OemLogoResolutionTable
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

