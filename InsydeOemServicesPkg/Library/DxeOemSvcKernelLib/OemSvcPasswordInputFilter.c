/** @file
  Provide function for OEM to filter out specific key data before system process this key data for password dialog.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
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
  This function for OEM to filter out specific key data before system process this key data for password dialog.

  @param[in, out]               User input key data

  @retval EFI_UNSUPPORTED       EfiKeyData was not updated.
  @retval EFI_MEDIA_CHANGED     EfiKeyData was updated.
**/
EFI_STATUS
OemSvcPasswordInputFilter (
  IN OUT EFI_KEY_DATA                          *EfiKeyData
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
