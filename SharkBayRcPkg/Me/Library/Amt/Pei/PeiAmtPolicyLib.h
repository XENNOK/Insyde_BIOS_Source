/** @file

  Header file for PEI AMT Policy functionality

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

#ifndef _PEI_AMT_POLICY_LIB_H_
#define _PEI_AMT_POLICY_LIB_H_

#include <Uefi.h>
#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Ppi/AmtPlatformPolicyPei.h>

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
  );

/**
  Check if AMT WatchDog is enabled in setup options.

  @param[in] PeiServices          Point to Pei Services structure

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
PeiAmtWatchDog (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  Get WatchDog BIOS Timmer.

  @param[in] PeiServices          Point to Pei Services structure

  @retval UINT16                  WatchDog BIOS Timer
**/
UINT16
PeiAmtWatchTimerBiosGet (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  Check if AMT is enabled in setup options.

  @param[in] PeiServices          Point to Pei Services structure

  @retval FALSE                   ActiveManagement is disabled.
  @retval TRUE                    ActiveManagement is enabled.
**/
BOOLEAN
PeiAmtSupported (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  Check if ASF is enabled in setup options.

  @param[in] PeiServices          Point to Pei Services structure

  @retval FALSE                   ASF is disabled.
  @retval TRUE                    ASF is enabled.
**/
BOOLEAN
PeiAsfSupported (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @param[in] PeiServices          Point to Pei Services structure

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );

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
  );

#endif
