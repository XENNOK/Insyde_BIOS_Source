/** @file
 DXE OEM IPMI Package library implement code - Enter Setup.

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
 This package function can enter Setup Utility as IPMI boot option request.
 OEM can implement this function since entering Setup Utility is a OEM specific function.

 @retval EFI_SUCCESS                    Entering Setup Utility success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiEnterSetup (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

