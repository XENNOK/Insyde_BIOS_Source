/** @file
 PEI OEM IPMI Package library implement code - Wait BMC ready.

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


#include <Library/PeiOemIpmiPackageLib.h>


/**
 This package function can wait for BMC ready via OEM specific way.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiPpi             A pointer to H2O_IPMI_INTERFACE_PPI structure.

 @retval EFI_SUCCESS                    Wait for BMC ready success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiWaitBmc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       H2O_IPMI_INTERFACE_PPI       *IpmiPpi
  )
{
  return EFI_UNSUPPORTED;
}

