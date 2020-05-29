/** @file
 DXE IPMI Package library implement code - Enter Setup.

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
 This package function can enter Setup Utility as IPMI boot option request.
 Platform implement this function since entering Setup Utility is a platform dependent function.

 @retval EFI_SUCCESS                    Entering Setup Utility success.
 @return EFI_ERROR (Status)             Locate gEfiSetupUtilityProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiEnterSetup (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

