/** @file
  Provides an opportunity for OEM to execute the project characteristic code
  when the password entering fails.

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
  The callback function is for OEM to execute the project characteristic code
  when the password entering fails.

  @param  none                  Based on OEM design.

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
  @retval EFI_SUCCESS           The service is customized in the project.
  @retval Others                Depends on customization.
**/
EFI_STATUS
OemSvcCheckPasswordFailCallBack (
  VOID
  )
{ 
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
