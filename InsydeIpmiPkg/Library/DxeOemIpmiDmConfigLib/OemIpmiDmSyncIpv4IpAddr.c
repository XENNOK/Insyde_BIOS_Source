/** @file
 OEM IPMI Device Manager Config library implement code - Sync BMC IPv4 Ip
 Address.

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
 Reserved for OEM to Sync BMC information "RMCP IP Address" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSyncIpv4IpAddr (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  )
{

  if (SetToBmc) {
    //
    // Set Config to BMC
    //
  } else {
    //
    // Get Config from BMC
    //
  }

  return EFI_UNSUPPORTED;

}

