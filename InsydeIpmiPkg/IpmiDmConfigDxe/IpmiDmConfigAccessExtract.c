/** @file
 IPMI Device Manager Config Access Extract.

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
 This function is called by ConfigAccessExtract, do something necessary before extract Config Data.
 Such as handling the Config Data non-referenced by IFR.

 @param[in]         IpmiDmConfig            A pointer to IPMI Device Manager Config Data struct.

 @return EFI_SUCCESS always.
*/
EFI_STATUS
IpmiDmConfigAccessExtract (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;


  //
  // Deal with the Config Data non-referenced by IFR.
  // Get current setting in Extract and this data will be temporary cache the not saved browser config data.
  //
  CopyMem (mBmcTempIpv4IpAddress, IpmiConfig->BmcIpv4IpAddress, 4);
  CopyMem (mBmcTempIpv4SubnetMask, IpmiConfig->BmcIpv4SubnetMask, 4);
  CopyMem (mBmcTempIpv4GatewayAddress, IpmiConfig->BmcIpv4GatewayAddress, 4);
  CopyMem (mBmcTempIpv6IpAddress, IpmiConfig->BmcIpv6IpAddress, 16);
  CopyMem (mBmcTempIpv6GatewayAddress, IpmiConfig->BmcIpv6GatewayAddress, 16);


  //
  // Reserved for OEM to do something necessary before extract Config Data.
  //
  OemIpmiDmConfigAccessExtract (IpmiDmConfig);

  return EFI_SUCCESS;

}

