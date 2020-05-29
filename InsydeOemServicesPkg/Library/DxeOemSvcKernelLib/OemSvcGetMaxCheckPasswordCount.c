/** @file
  Provides an opportunity for OEM to get the maximum times that the user is allowed to enter the password.

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
  Provides an opportunity for OEM to get the maximum times that the user is allowed to enter the password.

  @param[out]  ReportPasswordCount   Points to uintn that specifies 
                                     the number of times that the user is allowed to enter the password,

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Depends on customization.
**/
EFI_STATUS
OemSvcGetMaxCheckPasswordCount (
  IN OUT UINTN                       *ReportPasswordCount
  )
{ 
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

