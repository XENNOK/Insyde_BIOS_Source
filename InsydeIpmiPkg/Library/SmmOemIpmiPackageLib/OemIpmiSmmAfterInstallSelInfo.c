/** @file
 SMM OEM IPMI Package library implement code - After Install SEL Info Protocol.

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


#include <Library/SmmOemIpmiPackageLib.h>


/**
 Reserved for OEM to implement something additional requirements after SMM IPMI SEL Info Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSmmAfterInstallSelInfo (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

