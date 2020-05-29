/** @file
 DXE IPMI Setup Utility library implement code - Execute IPMI Utility.

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


#include <Library/DxeIpmiSetupUtilityLib.h>


/**
 This function can enter Ipmi Utility.

 @retval EFI_UNSUPPORTED                Platform does not implement this function.
 @return Status                         Locate gH2OIpmiUtilityProtocolGuid Protocol Status.
*/
EFI_STATUS
ExecuteIpmiUtility (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

