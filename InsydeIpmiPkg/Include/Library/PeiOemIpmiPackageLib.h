/** @file
 PEI OEM IPMI Package library header file.

 This file contains functions prototype that can be implemented by OEM to fit
 their requirements.

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


#ifndef _PEI_OEM_IPMI_PACKAGE_LIB_H_
#define _PEI_OEM_IPMI_PACKAGE_LIB_H_


#include <PiPei.h>

#include <Ppi/H2OIpmiInterfacePpi.h>


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
  );


/**
 Update IPMI policy by OEM specific way.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.

 @retval EFI_SUCCESS                    Update Policy success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiUpdatePolicy (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  );


/**
 Programming PCH LPC Generic I/O Decode Range Register for BMC use.
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
OemIpmiProgramLpc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINT16                       IpmiSmmBaseAddress,
  IN       UINT16                       IpmiSmmRegOffset,
  IN       UINT16                       IpmiPostBaseAddress,
  IN       UINT16                       IpmiPostRegOffset,
  IN       UINT16                       IpmiOsBaseAddress,
  IN       UINT16                       IpmiOsRegOffset
  );


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
  );


/**
 Reserved for OEM to implement something additional requirements after PEI IPMI Interface PPI has been installed.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PPI structure.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiPeiAfterInstallInterface (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       H2O_IPMI_INTERFACE_PPI       *Ipmi
  );


#endif

