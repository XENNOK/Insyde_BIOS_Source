/** @file
 PEI IPMI LPC library header file.

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


#ifndef _PEI_IPMI_LPC_LIB_H_
#define _PEI_IPMI_LPC_LIB_H_


#include <PiPei.h>


/**
 Programming PCH LPC Generic I/O Decode Range Register for BMC use.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiSmmBaseAddress  Value from FixedPcdGet16 (PcdIpmiSmmBaseAddress).
 @param[in]         IpmiSmmRegOffset    Value from FixedPcdGet16 (PcdIpmiSmmRegOffset).
 @param[in]         IpmiPostBaseAddress Value from FixedPcdGet16 (PcdIpmiPostBaseAddress).
 @param[in]         IpmiPostRegOffset   Value from FixedPcdGet16 (PcdIpmiPostRegOffset).
 @param[in]         IpmiOsBaseAddress   Value from FixedPcdGet16 (PcdIpmiOsBaseAddress).
 @param[in]         IpmiOsRegOffset     Value from FixedPcdGet16 (PcdIpmiOsRegOffset).

 @retval EFI_SUCCESS                    Programming success.
 @retval EFI_OUT_OF_RESOURCES           No unused Generic Decode Range Register can decode.
 @retval EFI_UNSUPPORTED                Does not implement this function.
*/
EFI_STATUS
IpmiProgramLpc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINT16                       IpmiSmmBaseAddress,
  IN       UINT16                       IpmiSmmRegOffset,
  IN       UINT16                       IpmiPostBaseAddress,
  IN       UINT16                       IpmiPostRegOffset,
  IN       UINT16                       IpmiOsBaseAddress,
  IN       UINT16                       IpmiOsRegOffset
  );


#endif
