/** @file
 PEI IPMI Package library header file.

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


#ifndef _PEI_IPMI_PACKAGE_LIB_H_
#define _PEI_IPMI_PACKAGE_LIB_H_


#include <PiPei.h>

#include <Ppi/H2OIpmiInterfacePpi.h>


/**
 This package function can wait for BMC ready via platform way.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiPpi             A pointer to H2O_IPMI_INTERFACE_PPI structure.

 @retval EFI_SUCCESS                    Wait for BMC ready success.
 @return EFI_ERROR (Status)             Implement code execute status.
*/
EFI_STATUS
IpmiWaitBmc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       H2O_IPMI_INTERFACE_PPI       *IpmiPpi
  );


/**
 Update IPMI policy according to the "Setup" variable SYSTEM_CONFIGURATION struct data.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.

 @retval EFI_SUCCESS                    Update Policy success.
 @return EFI_ERROR (Status)             Locate gEfiPeiReadOnlyVariable2PpiGuid or GetVariable error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function when IPMI Config in Device Manager.
*/
EFI_STATUS
IpmiUpdatePolicy (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  );


#endif

