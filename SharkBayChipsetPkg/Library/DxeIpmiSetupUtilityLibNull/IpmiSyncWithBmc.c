/** @file
 DXE IPMI Setup Utility library implement code - Sync With BMC.

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
 This function can set some platform settings to BMC.
 These settings are platform-dependent.

 @param[in]         ConfigBuffer        Pointer to CHIPSET_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Set some platform settings to BMC success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiSetToBmc (
  IN  VOID                              *ConfigBuffer
  )
{
  return EFI_UNSUPPORTED;
}


/**
 This function can get some platform settings from BMC.
 These settings are platform-dependent.

 @param[out]        ConfigBuffer        Pointer to CHIPSET_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Get some platform settings from BMC success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiGetFromBmc (
  OUT VOID                              *ConfigBuffer
  )
{
  return EFI_UNSUPPORTED;
}

