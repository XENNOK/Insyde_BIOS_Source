/** @file

  Implementation file for Me Policy functionality for PEIM

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include <PeiMePolicyLib.h>


/**
  Check if Me is enabled.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] PeiMePlatformPolicy  The Me Platform Policy protocol instance

  @retval EFI_SUCCESS             ME platform policy Ppi loacted
  @retval All other error conditions encountered when no ME platform policy Ppi
**/
EFI_STATUS
PeiMePolicyLibInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       PEI_ME_PLATFORM_POLICY_PPI  **PeiMePlatformPolicy
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate system configuration variable
  ///
  Status = PeiServicesLocatePpi (
            &gPeiMePlatformPolicyPpiGuid, /// GUID
            0,                            /// INSTANCE
            NULL,                         /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **) PeiMePlatformPolicy /// PPI
            );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
