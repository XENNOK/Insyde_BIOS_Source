/** @file
 IPMI Device Manager Config Access Route.

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


#include "IpmiDmConfigAccess.h"


/**
 This function is called by ConfigAccessRoute, do something necessary in Config Access Route.
 Such as handling the Config Data non-referenced by IFR and set configuration to BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @return EFI_SUCCESS always.
*/
EFI_STATUS
IpmiDmConfigAccessRoute (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;


  //
  // Deal with the Config Data non-referenced by IFR.
  // Save config data from temporary cache for current browser config setting.
  //
  CopyMem (IpmiConfig->BmcIpv4IpAddress, mBmcTempIpv4IpAddress, 4);
  CopyMem (IpmiConfig->BmcIpv4SubnetMask, mBmcTempIpv4SubnetMask, 4);
  CopyMem (IpmiConfig->BmcIpv4GatewayAddress, mBmcTempIpv4GatewayAddress, 4);
  CopyMem (IpmiConfig->BmcIpv6IpAddress, mBmcTempIpv6IpAddress, 16);
  CopyMem (IpmiConfig->BmcIpv6GatewayAddress, mBmcTempIpv6GatewayAddress, 16);


  //
  // Reserved for OEM to do something necessary in Config Access Route.
  //
  OemIpmiDmConfigAccessRoute (IpmiDmConfig);


  //
  // According the Config Data set to BMC.
  //
  IpmiDmSetToBmc (IpmiDmConfig);


  return EFI_SUCCESS;

}

