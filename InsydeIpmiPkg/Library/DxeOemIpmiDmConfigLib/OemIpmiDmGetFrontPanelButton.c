/** @file
 OEM IPMI Device Manager Config library implement code - Get Front Panel Button.

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


#include <Library/DxeOemIpmiDmConfigLib.h>
#include <Library/IpmiInterfaceLib.h>

#include <H2OIpmi.h>


/**
 Reserved for OEM to Get BMC information "Front Panel Button disable/enable status".

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmGetFrontPanelButton (
  IN  UINT8                             *IpmiDmConfig
  )
{

  return EFI_UNSUPPORTED;

}

