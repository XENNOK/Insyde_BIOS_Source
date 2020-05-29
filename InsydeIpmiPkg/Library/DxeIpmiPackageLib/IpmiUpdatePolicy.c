/** @file
 DXE IPMI Package library implement code - Update Policy.

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


#include <Library/DxeIpmiPackageLib.h>


/**
 This package function can update platform policy based on setup settings.
 Platform implement this function to set correct variable.

 @retval EFI_SUCCESS                    Update platform policy success.
 @return EFI_ERROR (Status)             Implement code execute status.
*/
EFI_STATUS
IpmiUpdatePolicy (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

