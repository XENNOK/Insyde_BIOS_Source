/** @file
 DXE OEM IPMI Package library implement code - Update Policy.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/DxeOemIpmiPackageLib.h>


/**
 This package function can update platform policy based on setup settings.
 OEM can implement this function to set correct variable by OEM specific way.

 @retval EFI_SUCCESS                    Update platform policy success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiUpdatePolicy (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

