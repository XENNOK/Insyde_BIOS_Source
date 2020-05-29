/** @file
  Provides an opportunity for OEM to check update platform display device or not.

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
  Provides an opportunity for OEM to check if platform display device was updated or not.

  @param[out]  SkipOriginalCode      Points to the boolean value.
                                     TRUE:  skip platform display device update
                                     FALSE: need to do platform display device update

  @retval      EFI_SUCCESS           Return success only when setting *SkipOriginalCode = TRUE.
  @retval      EFI_UNSUPPORTED       Don't skip kernel code below the oem services.
**/
EFI_STATUS
OemSvcBootDisplayDeviceReplace (
  IN OUT BOOLEAN                     *SkipOriginalCode
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

