/** @file

  Implementation file for AMT Policy functionality for PEIM

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

#include <Library/Amt/Pei/PeiAmtPolicyLib.h>

/**
  Check if Amt is enabled in setup options.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] PeiAmtPlatformPolicy The AMT Platform Policy protocol instance

  @retval EFI_SUCCESS             AMT platform policy Ppi located
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PeiAmtPolicyLibInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       PEI_AMT_PLATFORM_POLICY_PPI **PeiAmtPlatformPolicy
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate system configuration variable
  ///
  Status = PeiServicesLocatePpi (
            &gPeiAmtPlatformPolicyPpiGuid,  /// GUID
            0,                              /// INSTANCE
            NULL,                           /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **) PeiAmtPlatformPolicy  /// PPI
            );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Check if AMT WatchDog is enabled in setup options.

  @param[in] PeiServices          Point to Pei Services structure

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
PeiAmtWatchDog (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  PEI_AMT_PLATFORM_POLICY_PPI     *PeiAmtPlatformPolicy;

  Supported = FALSE;
  if (ManageabilityModeSetting (PeiServices) != 0) {
    Status = PeiAmtPolicyLibInit (PeiServices, &PeiAmtPlatformPolicy);
    ASSERT_EFI_ERROR (Status);
    ///
    /// First check if AMT WatchDog is enabled in Setup.
    ///
    if (PeiAmtPlatformPolicy->WatchDog == 1) {
      Supported = TRUE;
    }
  }

  return Supported;
}

/**
  Get WatchDog BIOS Timmer.

  @param[in] PeiServices          Point to Pei Services structure

  @retval UINT16                  WatchDog BIOS Timer
**/
UINT16
PeiAmtWatchTimerBiosGet (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                  Status;
  PEI_AMT_PLATFORM_POLICY_PPI *PeiAmtPlatformPolicy;

  Status = PeiAmtPolicyLibInit (PeiServices, &PeiAmtPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  return PeiAmtPlatformPolicy->WatchDogTimerBios;
}

/**
  Check if AMT is enabled in setup options.

  @param[in] PeiServices          Point to Pei Services structure

  @retval FALSE                   ActiveManagement is disabled.
  @retval TRUE                    ActiveManagement is enabled.
**/
BOOLEAN
PeiAmtSupported (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  PEI_AMT_PLATFORM_POLICY_PPI     *PeiAmtPlatformPolicy;

  Status = PeiAmtPolicyLibInit (PeiServices, &PeiAmtPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  ///
  /// First check if AMT support is enabled in Setup.
  ///
  if (PeiAmtPlatformPolicy->iAmtEnabled == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if ASF is enabled in setup options.

  @param[in] PeiServices          Point to Pei Services structure

  @retval FALSE                   ASF is disabled.
  @retval TRUE                    ASF is enabled.
**/
BOOLEAN
PeiAsfSupported (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  PEI_AMT_PLATFORM_POLICY_PPI     *PeiAmtPlatformPolicy;

  Status = PeiAmtPolicyLibInit (PeiServices, &PeiAmtPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  if (PeiAmtPlatformPolicy->Revision < PEI_AMT_PLATFORM_POLICY_PPI_REVISION_2) {
    return FALSE;
  }
  ///
  /// First check if ASF support is enabled in Setup.
  ///
  if (PeiAmtPlatformPolicy->AsfEnabled == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @param[in] PeiServices          Point to Pei Services structure

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
  PEI_AMT_PLATFORM_POLICY_PPI     *PeiAmtPlatformPolicy;

  Status = PeiAmtPolicyLibInit (PeiServices, &PeiAmtPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  if (PeiAmtPlatformPolicy->Revision < PEI_AMT_PLATFORM_POLICY_PPI_REVISION_2) {
    return 0;
  }

  return (UINT8) (PeiAmtPlatformPolicy->ManageabilityMode);
}

/**
  This will return progress event Option.
  True if the option is enabled.

  @param[in] PeiServices          Point to Pei Services structure

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
PeiFwProgressSupport (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         Supported;
  PEI_AMT_PLATFORM_POLICY_PPI     *PeiAmtPlatformPolicy;

  Status = PeiAmtPolicyLibInit (PeiServices, &PeiAmtPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  Supported = FALSE;

  if (PeiAmtPlatformPolicy->Revision >= PEI_AMT_PLATFORM_POLICY_PPI_REVISION_3) {
    ///
    /// Check if progress event is enabled in Setup.
    ///
    if (PeiAmtPlatformPolicy->FWProgress == 1) {
      Supported = TRUE;
    }
  }

  return Supported;
}
