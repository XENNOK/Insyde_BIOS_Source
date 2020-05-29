/** @file

  OC CPU Early Post initializations.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
    This software and associated documentation (if any) is furnished
    under a license and may only be used or copied in accordance
    with the terms of the license. Except as permitted by such
    license, no part of this software or documentation may be
    reproduced, stored in a retrieval system, or transmitted in any
    form or by any means without the express written consent of
    Intel Corporation.

*/

#include "CpuInitPeim.h"
#include "CpuOcInit.h"
#include <CpuAccess.h>
#include <Library/OverclockingLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/Wdt.h>

/**
  Initializes Overclocking settings in the processor.

  @param[in] PeiServices - General purpose services available to every PEIM.
  @param[in] OverclockingtConfig      Pointer to Policy protocol instance

  @retval EFI_SUCCESS
**/
EFI_STATUS
CpuOcInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  EFI_STATUS             Status;
  OC_CAPABILITIES_ITEM   OcCaps;
  VOLTAGE_FREQUENCY_ITEM CurrentVfItem;
  VOLTAGE_FREQUENCY_ITEM RequestedVfItem;
  GLOBAL_CONFIG_ITEM     CurrentFivrItem;
  GLOBAL_CONFIG_ITEM     RequestedFivrItem;
  SVID_CONFIG_ITEM       CurrentSvidItem;
  SVID_CONFIG_ITEM       RequestedSvidItem;
  UINT32                 LibStatus;
  UINT8                  DomainId;
  UINT8                  ResetRequired;
  WDT_PPI                *gWdtPei;

  LibStatus = 0; //DEBUG
  ResetRequired = FALSE;
  if (CpuPlatformPolicyPpi->OverclockingConfig->OcSupport == 0) {
    ///
    /// Overclocking is disabled
    ///
    DEBUG ((EFI_D_ERROR, "(OC) Overclocking is disabled. Bypassing CPU core overclocking flow.\n"));
    return EFI_SUCCESS;
  }

  Status = EFI_SUCCESS;
  ZeroMem(&CurrentFivrItem,sizeof(CurrentFivrItem));
  ZeroMem(&RequestedFivrItem,sizeof(RequestedFivrItem));
  ZeroMem(&CurrentSvidItem,sizeof(CurrentSvidItem));
  ZeroMem(&RequestedSvidItem,sizeof(RequestedSvidItem));

  //
  // Locate WDT_PPI (ICC WDT PPI)
  //
  Status = PeiServicesLocatePpi (
                  &gWdtPpiGuid,
                  0,
                  NULL,
                  (VOID **) &gWdtPei
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// We will loop on the CPU domains to manage the voltage/frequency settings
  ///
  for (DomainId = OC_LIB_DOMAIN_ID_IA_CORE; DomainId < OC_LIB_DOMAIN_ID_UNCORE; DomainId++) {
    ///
    /// Only IA_CORE and CLR are valid for CPU Core
    ///
    if ((DomainId == OC_LIB_DOMAIN_ID_IA_CORE) || (DomainId == OC_LIB_DOMAIN_ID_CLR)) {

      ///
      /// Get OC Capabilities of the domain
      ///
      ZeroMem(&OcCaps,sizeof(OcCaps));
      OcCaps.DomainId = DomainId;
      Status = GetOcCapabilities(&OcCaps,&LibStatus);

      if (LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS) {
        ///
        /// If any OC is supported on this domain, then proceed
        ///
        if (OcCaps.RatioOcSupported || OcCaps.VoltageOverridesSupported || OcCaps.VoltageOffsetSupported) {
          ///
          /// Need to populate the user requested settings from the platform policy
          /// to determine if OC changes are desired.
          ///
          ZeroMem(&CurrentVfItem,sizeof(CurrentVfItem));
          CurrentVfItem.DomainId = DomainId;

          ///
          /// Get a copy of the current domain VfSettings from the Mailbox Library
          ///
          Status = GetVoltageFrequencyItem(&CurrentVfItem,&LibStatus);
          if ((Status != EFI_SUCCESS) || (LibStatus != OC_LIB_COMPLETION_CODE_SUCCESS)) {
            continue;
          }

          ///
          /// Populate the user requested VfSettings struct
          ///
          ZeroMem(&RequestedVfItem,sizeof(RequestedVfItem));
          RequestedVfItem.DomainId = DomainId;
          if (DomainId == OC_LIB_DOMAIN_ID_IA_CORE) {
            RequestedVfItem.VfSettings.MaxOcRatio = (UINT8) CpuPlatformPolicyPpi->OverclockingConfig->CoreMaxOcTurboRatio;
          } else if (DomainId == OC_LIB_DOMAIN_ID_CLR) {
            RequestedVfItem.VfSettings.MaxOcRatio = (UINT8) CpuPlatformPolicyPpi->OverclockingConfig->ClrMaxOcTurboRatio;
          }

          ///
          /// VoltageTarget has 2 uses and we need to update the target based
          /// on the voltagemode requested
          ///
          if (DomainId == OC_LIB_DOMAIN_ID_IA_CORE) {
            RequestedVfItem.VfSettings.VoltageTargetMode = CpuPlatformPolicyPpi->OverclockingConfig->CoreVoltageMode;
            if (RequestedVfItem.VfSettings.VoltageTargetMode == OC_LIB_OFFSET_ADAPTIVE) {
              RequestedVfItem.VfSettings.VoltageTarget = CpuPlatformPolicyPpi->OverclockingConfig->CoreExtraTurboVoltage;
            } else {
              RequestedVfItem.VfSettings.VoltageTarget = CpuPlatformPolicyPpi->OverclockingConfig->CoreVoltageOverride;
            }
            RequestedVfItem.VfSettings.VoltageOffset = CpuPlatformPolicyPpi->OverclockingConfig->CoreVoltageOffset;
          } else if (DomainId == OC_LIB_DOMAIN_ID_CLR) {
            RequestedVfItem.VfSettings.VoltageTargetMode = CpuPlatformPolicyPpi->OverclockingConfig->ClrVoltageMode;
            if (RequestedVfItem.VfSettings.VoltageTargetMode == OC_LIB_OFFSET_ADAPTIVE) {
              RequestedVfItem.VfSettings.VoltageTarget = CpuPlatformPolicyPpi->OverclockingConfig->ClrExtraTurboVoltage;
            } else {
              RequestedVfItem.VfSettings.VoltageTarget = CpuPlatformPolicyPpi->OverclockingConfig->ClrVoltageOverride;
            }
            RequestedVfItem.VfSettings.VoltageOffset = CpuPlatformPolicyPpi->OverclockingConfig->ClrVoltageOffset;
          }

          ///
          /// Compare current settings with user requested settings to see if changes are needed
          ///
          if (CompareMem((VOID *)&RequestedVfItem,(VOID *)&CurrentVfItem,sizeof(VOLTAGE_FREQUENCY_ITEM))) {
            ///
            /// Arm watchdog timer for OC changes
            ///
            Status = gWdtPei->ReloadAndStart (WDT_TIMEOUT_BETWEEN_PEI_DXE);

            ///
            /// Need to update the requested voltage/frequency values
            ///
            DEBUG ((EFI_D_INFO, "(OC) Set Voltage Frequency for Domain = %X\n", DomainId));
            DEBUG ((EFI_D_INFO, "(OC) RequestedVfItem.VfSettings.MaxOcRatio     = %X\n", RequestedVfItem.VfSettings.MaxOcRatio));
            DEBUG ((EFI_D_INFO, "(OC) RequestedVfItem.VfSettings.TargetMode     = %X\n", RequestedVfItem.VfSettings.VoltageTargetMode));
            DEBUG ((EFI_D_INFO, "(OC) RequestedVfItem.VfSettings.VoltageTarget  = %X\n", RequestedVfItem.VfSettings.VoltageTarget));
            DEBUG ((EFI_D_INFO, "(OC) RequestedVfItem.VfSettings.VoltageOffset  = %X\n", RequestedVfItem.VfSettings.VoltageOffset));
            DEBUG ((EFI_D_INFO, "(OC) CurrentVfItem.VfSettings.MaxOcRatio       = %X\n", CurrentVfItem.VfSettings.MaxOcRatio));
            DEBUG ((EFI_D_INFO, "(OC) CurrentVfItem.VfSettings.TargetMode       = %X\n", CurrentVfItem.VfSettings.VoltageTargetMode));
            DEBUG ((EFI_D_INFO, "(OC) CurrentVfItem.VfSettings.VoltageTarget    = %X\n", CurrentVfItem.VfSettings.VoltageTarget));
            DEBUG ((EFI_D_INFO, "(OC) CurrentVfItem.VfSettings.VoltageOffset    = %X\n", CurrentVfItem.VfSettings.VoltageOffset));
            Status = SetVoltageFrequencyItem(RequestedVfItem,&LibStatus);
            if ((Status != EFI_SUCCESS) || (LibStatus != OC_LIB_COMPLETION_CODE_SUCCESS)) {
              DEBUG ((EFI_D_ERROR, "(OC) Set Voltage Frequency failed. EFI Status = %X, Library Status = %X\n", Status, LibStatus));
            }
          }
        }
      } else {
        DEBUG ((EFI_D_ERROR, "(OC) GetOcCapabilities message failed. Library Status = %X, Domain = %X\n", LibStatus, DomainId));
      }
    }
  }

  ///
  /// Detect changes to global FIVR settings
  ///
  Status = GetFivrConfig(&CurrentFivrItem,&LibStatus);

  if (LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS) {
    ///
    /// Populate the requested FIVR settings from platform policy. The platform policy defines
    /// these bits as 0-Disabled, 1-Enabled. The Mailbox uses the reverse encoding. Need to convert
    /// the platform policy data to match the mailbox input.
    ///
    RequestedFivrItem.DisableFivrFaults = (~CpuPlatformPolicyPpi->OverclockingConfig->FivrFaultsEnable) & BIT0_MASK;
    RequestedFivrItem.DisableFivrEfficiency = (~CpuPlatformPolicyPpi->OverclockingConfig->FivrEfficiencyEnable) & BIT0_MASK;

    ///
    /// Compare current FIVR settings with requested FIVR settings to see if changes are needed
    ///
    if (CompareMem((VOID *)&RequestedFivrItem,(VOID *)&CurrentFivrItem,sizeof(GLOBAL_CONFIG_ITEM))) {
      ///
      /// Arm watchdog timer for OC changes
      ///
      Status = gWdtPei->ReloadAndStart (8);

      ///
      /// Need to update the requested FIVR values
      ///
      DEBUG ((EFI_D_INFO, "(OC) Set FIVR Config for Domain = %X\n", DomainId));
      Status = SetFivrConfig(RequestedFivrItem, &LibStatus);
      if ((Status != EFI_SUCCESS) || (LibStatus != OC_LIB_COMPLETION_CODE_SUCCESS)) {
        DEBUG ((EFI_D_ERROR, "(OC) Set FIVR Config failed. EFI Status = %X, Library Status = %X\n", Status, LibStatus));
      }
      else {
        ///
        /// If Re-enabling Fivr Faults, system needs to perform a cold reset for hardware to take effect
        ///
        if ((CurrentFivrItem.DisableFivrFaults == 1) && (RequestedFivrItem.DisableFivrFaults == 0)) {
          DEBUG ((EFI_D_ERROR, "(OC) FIVR Faults enable detected. Cold Reset required.\n"));
          ResetRequired = TRUE;
        }
      }
    }
  }
  else {
    DEBUG ((EFI_D_ERROR, "(OC) Get FIVR Config message failed. Library Status = %X\n", LibStatus));
  }

  ///
  /// Detect changes to SVID settings
  ///
  Status = GetSvidConfig(&CurrentSvidItem,&LibStatus);

  if (LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS) {
    ///
    /// Populate the requested SVID settings from platform policy. SvidDisable uses a
    /// reverse encoding from the platform policy defintion and will need to be converted.
    ///
    RequestedSvidItem.VoltageTarget = CpuPlatformPolicyPpi->OverclockingConfig->SvidVoltageOverride;
    RequestedSvidItem.SvidDisable = ~(CpuPlatformPolicyPpi->OverclockingConfig->SvidEnable) & BIT0_MASK;

    ///
    /// Compare current SVID settings with requested SVID settings to see if changes are needed
    ///
    if (CompareMem((VOID *)&RequestedSvidItem,(VOID *)&CurrentSvidItem,sizeof(SVID_CONFIG_ITEM))) {
      ///
      /// Arm watchdog timer for OC changes
      ///
      Status = gWdtPei->ReloadAndStart (8);

      ///
      /// If Re-enabling SVID, system needs to perform a cold reset for hardware to take effect. No write to mailbox needed.
      ///
      if ((CurrentSvidItem.SvidDisable == 1) && (RequestedSvidItem.SvidDisable == 0)) {
        DEBUG ((EFI_D_ERROR, "(OC) SVID Enable detected. Cold Reset required.\n"));
        ResetRequired = TRUE;
      }
      else {
        ///
      /// Need to update the requested SVID values
      ///
      DEBUG ((EFI_D_INFO, "(OC) Set SVID Config for Domain = %X\n", DomainId));
      Status = SetSvidConfig(RequestedSvidItem, &LibStatus);
      if ((Status != EFI_SUCCESS) || (LibStatus != OC_LIB_COMPLETION_CODE_SUCCESS)) {
        DEBUG ((EFI_D_ERROR, "(OC) Set SVID Config failed. EFI Status = %X, Library Status = %X\n", Status, LibStatus));
      }
      }
    }
  } else {
    DEBUG ((EFI_D_ERROR, "(OC) GetFivrConfig message failed. Library Status = %X\n", LibStatus));
  }

  ///
  /// Command was successful and SVID config has changed. CPU must perform a reset
  /// for SVID settings to take effect.
  ///
  if (ResetRequired) {
    DEBUG ((EFI_D_ERROR, "(OC) Perform Cold Reset\n"));
    PerformWarmORColdReset (PeiServices, COLDRESET);
  }

  return Status;
}

