/** @file
 PEI OEM IPMI Package library implement code - Program Super I/O.

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
 Programming Super I/O configuration for BMC.
 Such as activate KCS interface and change KCS address by Super I/O interface.
 OEM can implement this function by OEM specific way.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiSmmBaseAddress  Value from FixedPcdGet16 (PcdIpmiSmmBaseAddress).
 @param[in]         IpmiSmmRegOffset    Value from FixedPcdGet16 (PcdIpmiSmmRegOffset).
 @param[in]         IpmiPostBaseAddress Value from FixedPcdGet16 (PcdIpmiPostBaseAddress).
 @param[in]         IpmiPostRegOffset   Value from FixedPcdGet16 (PcdIpmiPostRegOffset).
 @param[in]         IpmiOsBaseAddress   Value from FixedPcdGet16 (PcdIpmiOsBaseAddress).
 @param[in]         IpmiOsRegOffset     Value from FixedPcdGet16 (PcdIpmiOsRegOffset).

 @retval EFI_SUCCESS                    Programming success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiProgramSio (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINT16                       IpmiSmmBaseAddress,
  IN       UINT16                       IpmiSmmRegOffset,
  IN       UINT16                       IpmiPostBaseAddress,
  IN       UINT16                       IpmiPostRegOffset,
  IN       UINT16                       IpmiOsBaseAddress,
  IN       UINT16                       IpmiOsRegOffset
  )
{
  return EFI_UNSUPPORTED;
}

