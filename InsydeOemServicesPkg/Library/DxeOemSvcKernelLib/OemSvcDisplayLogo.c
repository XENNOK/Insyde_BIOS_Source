/** @file
  Provides an opportunity for OEM to decide to display Logo or not.

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
  Provides an opportunity for OEM to decide to display Logo or not.

  @param[in, out]  QuietBoot             Points to the boolean value.
                                         TRUE: Display Logo
                                         FALSE: Don't display Logo

  @retval          EFI_UNSUPPORTED       Returns unsupported by default.
  @retval          EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval          Others                Depends on customization.
**/
EFI_STATUS
OemSvcDisplayLogo (
  IN OUT BOOLEAN                         *QuietBoot
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
