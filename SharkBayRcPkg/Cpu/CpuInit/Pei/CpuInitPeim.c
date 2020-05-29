/** @file
  EFI 2.0 PEIM to initialize the cache and program for unlock processor

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Revision History

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement

**/

#include "CpuInitPeim.h"
#include "CpuOcInit.h"
#include "PfatInit.h"
#include <CpuRegs.h>
#include <Uefi/UefiBaseType.h>
#include <Ppi/PchInit.h>
#include <Ppi/SecPlatformInformation.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/PchReset.h>
#include <Ppi/SecPlatformInformation.h>
#include <Guid/HtBistHob.h>
#include <Library/OverclockingLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <CpuDataStruct.h>

static EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gPchInitPpiGuid,
    SetCpuStrap
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMasterBootModePpiGuid,
    BuildBistHob
  }
};

/**
  This fuction compare Setup Options of ActiveCores & HyperThreading against the CPU Strap
  and in case of mismatch requests reset

  @param[in] PeiServices - For printouts
  @param[in] CpuStrapSet - Strap current setting
  @param[in] CpuPlatformPolicyPpi - Platform Policy PPI

  @retval CPU_RESET_TYPE
  @retval CPU_ONLY_RESET - if reset is needed
  @retval NO_RESET - otherwise
**/
CPU_RESET_TYPE
SetActiveCoresAndSmtEnableDisable (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN CPU_STRAP_SET               *CpuStrapSet,
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  CPU_RESET_TYPE     ResetType;
  UINT16             TotalThreadsPerCore;
  UINT32             Data32;
  UINT32             NumberOfActiveCores;
  UINT32             NumberOfActiveThreads;
  EFI_CPUID_REGISTER Cpuid0B = { 0, 0, 0, 0 };

  ResetType = NO_RESET;

  ///
  /// Read CPUID(0xB) with ECX=0 for number of logical processors per Core.
  /// This value does NOT change based on Intel HT Technology disable and core disables.
  ///
  Cpuid0B.RegEcx = 0;
  AsmCpuidEx (CPUID_PROCESSOR_TOPOLOGY, 0, NULL, &Cpuid0B.RegEbx, NULL, NULL);
  TotalThreadsPerCore = (UINT16) Cpuid0B.RegEbx;

  ///
  /// CORE_THREAD_COUNT - msr 0x35
  /// Symbol      Name        MSB:LSB  Description
  /// CoreCount   CoreCount   31:16    The Core Count reflects the enabled cores based
  ///                                  on the above thread count and the value of threads_
  ///                                  available (to determine if HT is on) at reset time.
  ///
  /// ThreadCount ThreadCount 15:0     The Thread Count reflects the enabled threads based
  ///                                  on the factory-configured thread count and the value
  ///                                  of the CSR_DESIRED_CORES register at reset time.
  ///
  ///
  /// Read MSR for Active Core and Thread Count.
  ///
  Data32                = (UINT32) AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  NumberOfActiveCores   = (UINT32) ((Data32 >> 16) & 0xFFFF);
  NumberOfActiveThreads = (UINT32) (Data32 & 0xFFFF);

  DEBUG ((EFI_D_INFO, "Number of Active Cores / Threads = %x / %x\n", NumberOfActiveCores, NumberOfActiveThreads));

  if (TotalThreadsPerCore > 1) {
    ///
    /// SMT is supported
    ///
    DEBUG ((EFI_D_INFO, "SMT is supported\n"));
    ///
    /// Check if the configuration of SMT matches the BIOS Setup option.
    ///
    if (CpuStrapSet->HtDisabled == CpuPlatformPolicyPpi->CpuConfig->HyperThreading) {
      ///
      /// SMT configuration doesn't match BIOS Setup option; update the Strap Set Data and Issue a Warm reset
      ///
      DEBUG ((EFI_D_INFO, "SMT configuration doesn't match the setup value\n"));
      CpuStrapSet->HtDisabled = !CpuPlatformPolicyPpi->CpuConfig->HyperThreading;
      ResetType |= WARM_RESET;
    }
  } else {
    ///
    /// SMT is not supported by default fusing.
    ///
    DEBUG ((EFI_D_WARN, "SMT is NOT supported\n"));
    CpuStrapSet->HtDisabled = 1;
    CpuPlatformPolicyPpi->CpuConfig->HyperThreading = CPU_FEATURE_DISABLE;
  }
  ///
  /// Check if the configuration of "Active Core" matches the BIOS Setup option.
  ///
  if (CpuStrapSet->NumberOfActiveCores != CpuPlatformPolicyPpi->CpuConfig->ActiveCoreCount) {
    DEBUG (
            (EFI_D_INFO,
             "NumberOfActiveCores doesn't match the value from Setup = %x / %x\n",
             CpuStrapSet->NumberOfActiveCores,
             CpuPlatformPolicyPpi->CpuConfig->ActiveCoreCount)
            );
    CpuStrapSet->NumberOfActiveCores = CpuPlatformPolicyPpi->CpuConfig->ActiveCoreCount;
    ///
    /// Haswell CPU doesnt require COLD reset to set number of active cores
    ///
    ResetType |= WARM_RESET;
  }

  return ResetType;
}

/**
  This fuction compare Setup Options of BIST against the CPU Strap
  and in case of mismatch requests reset

  @param[in] PeiServices - For printouts
  @param[in] CpuStrapSet - Strap current setting
  @param[in] CpuPlatformPolicyPpi - Platform Policy PPI

  @retval CPU_RESET_TYPE
  @retval CPU_ONLY_RESET - if reset is needed
  @retval NO_RESET - otherwise
**/
CPU_RESET_TYPE
BistEnableDisable (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN CPU_STRAP_SET               *CpuStrapSet,
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  if (CpuStrapSet->Bist == CpuPlatformPolicyPpi->CpuConfig->BistOnReset) {
    return NO_RESET;
  } else {
    CpuStrapSet->Bist = CpuPlatformPolicyPpi->CpuConfig->BistOnReset;
    DEBUG ((EFI_D_INFO, "BIST configuration doesn't match the setup value\n"));
    return WARM_RESET;
  }
}

/**
  This fuction compare Setup Options of Processor Flex Multiplier against the CPU Strap
  and in case of mismatch requests reset

  @param[in] PeiServices - For printouts
  @param[in] CpuStrapSet - Strap current setting
  @param[in] CpuPlatformPolicyPpi - Platform Policy PPI

  @retval CPU_RESET_TYPE
  @retval CPU_ONLY_RESET - if reset is needed
  @retval NO_RESET - otherwise
**/
CPU_RESET_TYPE
ProgramProcessorFlexMultiplier (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN CPU_STRAP_SET               *CpuStrapSet,
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  UINT64              FlexRatioData;
  UINT64              CurrentClockFlexRatio;
  UINT64              MsrData;
  CPU_RESET_TYPE      ResetType;

  ResetType         = NO_RESET;

  ///
  /// Perform Flex Ratio if enabled by user in Setup screen
  ///
  if (CpuPlatformPolicyPpi->CpuConfig->CpuRatioOverride) {
    FlexRatioData = (UINT64) (CpuPlatformPolicyPpi->CpuConfig->CpuRatio);
  } else {
    ///
    /// Flex Ratio was set to disable.  Reset to Max Non-Turbo Ratio
    ///
    FlexRatioData = 0x0;
  }
  DEBUG ((EFI_D_INFO, "Setup Ratio is 0x%X\n", FlexRatioData));

  ///
  /// Read and save current Flex Ratio data; disregard enable bit (MSR 194h [15:8])
  ///
  MsrData               = AsmReadMsr64 (MSR_FLEX_RATIO);
  CurrentClockFlexRatio = ((RShiftU64 (MsrData, N_FLEX_RATIO)) & 0xFF);
  DEBUG ((EFI_D_INFO, "Current Flex Ratio from MSR is 0x%X\n", CurrentClockFlexRatio));
  DEBUG ((EFI_D_INFO, "Current Flex Ratio from CPU Strap: 0x%X\n", CpuStrapSet->FlexRatio));
  ///
  /// Check and set Flex Ratio to requested ratio if possible
  ///
  if (FlexRatioData == CpuStrapSet->FlexRatio) {
    ///
    /// Do nothing, ratio is already set to requested value and enabled
    ///
    DEBUG ((EFI_D_INFO, "No need to set Flex Ratio.\n"));
  } else {
    ///
    /// Set Flex Ratio to user selected value
    ///
    /// Write new data to Flex Ratio register
    /// First clear MSR of any previous value
    ///
    MsrData &= RATIO_FLEX_CLEAR_MASK;

    ///
    /// Enter the new data
    ///
    MsrData |= (LShiftU64 ((FlexRatioData & 0xFF), N_FLEX_RATIO));
    MsrData |= B_FLEX_EN;
    AsmWriteMsr64 (MSR_FLEX_RATIO, MsrData);

    ///
    /// Set Soft Reset Data for Flex Ratio
    ///
    CpuStrapSet->FlexRatio = (UINT32) FlexRatioData;

    ///
    /// Set RESET flag
    ///
    DEBUG ((EFI_D_INFO, "Setting Flex Ratio to 0x%X\n", CpuStrapSet->FlexRatio));
    ResetType |= WARM_RESET;
  }

  return ResetType;
}


/**
  Check if VT is fused and disabled by Setup Option so reset is needed

  @param[in] CpuPlatformPolicyPpi - Platform Policy PPI

  @retval CPU_RESET_TYPE
  @retval NO_RESET - If no reset is needed
  @retval COLDRESET - otherwise
**/
CPU_RESET_TYPE
CheckVmxIfNeedReset (
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  UINT64             MsrIa32FeatureControl;
  BOOLEAN            CurrentVmxState;
  EFI_CPUID_REGISTER Cpuid01 = { 0, 0, 0, 0 };

  AsmCpuid (CPUID_VERSION_INFO, &Cpuid01.RegEax, &Cpuid01.RegEbx, &Cpuid01.RegEcx, &Cpuid01.RegEdx);

  ///
  /// Check if VMX is supported
  ///
  if ((Cpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_VME) == 0) {
    return NO_RESET;
  }

  MsrIa32FeatureControl = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);

  ///
  /// If unlocked, no reset needed.
  ///
  if ((MsrIa32FeatureControl & B_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
    return NO_RESET;
  }

  CurrentVmxState = (BOOLEAN) ((MsrIa32FeatureControl & B_MSR_IA32_FEATURE_CONTROL_EVT) != 0);

  ///
  /// Need to reset only if locked and VMX state has to be changed.
  ///
  if ((CurrentVmxState == (CpuPlatformPolicyPpi->CpuConfig->VmxEnable)) == CPU_FEATURE_ENABLE) {
    return NO_RESET;
  }
  ///
  /// We need a power good reset to unlock MSR_IA32_FEATURE_CONTROL.
  ///
  return COLDRESET;
}

/**
  Set CPU strap setting for feature change

  @param[in] PeiServices       - Indirect reference to the PEI Services Table.
  @param[in] NotifyDescriptor  - Address of the notification descriptor data structure. Type
                      EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param[in] Ppi               - Address of the PPI that was installed.

  @retval EFI_SUCCESS - Function completed successfully
**/
EFI_STATUS
EFIAPI
SetCpuStrap (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi
  )
{
  EFI_STATUS                  Status;
  CPU_STRAP_SET               CpuStrapSet = { 0 };
  UINT16                      CpuStrapSetData;
  volatile CPU_RESET_TYPE     ResetType;
  PCH_INIT_PPI                *PchInitPpi;
  PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi;
  UINT8                       Data;

  ResetType       = NO_RESET;
  CpuStrapSetData = 0;
  Data            = 0;

  Status = PeiServicesLocatePpi (
                  &gPeiCpuPlatformPolicyPpiGuid,
                  0,
                  NULL,
                  (VOID **) &CpuPlatformPolicyPpi
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Locate PchInit Ppi
  ///
  Status = PeiServicesLocatePpi (
                  &gPchInitPpiGuid,
                  0,
                  NULL,
                  (VOID **) &PchInitPpi
                  );
  if (Status == EFI_SUCCESS) {
    ///
    /// Get Cpu Strap Set Data
    ///
    DEBUG ((EFI_D_INFO, "Set PCH CPU Straps \n"));
    Status                        = PchInitPpi->CpuStrapSet ((CONST EFI_PEI_SERVICES **)PeiServices, GetCpuStrapSetData, &CpuStrapSetData);
    *((UINT16 *) (&CpuStrapSet))  = CpuStrapSetData;
    DEBUG ((EFI_D_INFO, "Current CPU Strap Data = 0x%04X\n", CpuStrapSetData));

    if (Status == EFI_SUCCESS) {
      ResetType |= SetActiveCoresAndSmtEnableDisable (PeiServices, &CpuStrapSet, CpuPlatformPolicyPpi);
      ResetType |= BistEnableDisable (PeiServices, &CpuStrapSet, CpuPlatformPolicyPpi);

      ///
      /// Perform Flex Ratio if processor is fused to perform Flex Ratio
      ///
      if ((AsmReadMsr64 (MSR_FLEX_RATIO) & B_FLEX_EN) == B_FLEX_EN) {
        ResetType |= ProgramProcessorFlexMultiplier (PeiServices, &CpuStrapSet, CpuPlatformPolicyPpi);
      }

      if (ResetType != NO_RESET) {
        CpuStrapSetData = *((UINT16 *) (&CpuStrapSet));
        DEBUG ((EFI_D_INFO, "New CPU Strap Data = 0x%04X\n", CpuStrapSetData));
        Status = PchInitPpi->CpuStrapSet (
                        (CONST EFI_PEI_SERVICES **)PeiServices,
                        SetCpuStrapSetData,
                        &CpuStrapSetData
                        );
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_INFO, "NO Reset\n"));
          ResetType = NO_RESET;
        }
      }
    }
  }

  ResetType |= CheckVmxIfNeedReset (CpuPlatformPolicyPpi);

  if (ResetType != NO_RESET) {
    ///
    /// Perform Warm or Cold Reset
    ///
    DEBUG (
            (EFI_D_INFO,
             "Reset Required. Performing Cold/Warm Reset to read the new strap values - ResetType = %x\n",
             ResetType)
            );
    PerformWarmORColdReset (PeiServices, ResetType);
  }

  Status = PchInitPpi->CpuStrapSet (
                  (CONST EFI_PEI_SERVICES **)PeiServices,
                  LockCpuStrapSetData,
                  &CpuStrapSetData
                  );
  DEBUG ((EFI_D_INFO, "No Reset Required\n"));

  return EFI_SUCCESS;
}

/**
  Initialize prefetcher settings

  @param[in] MlcStreamerprefecterEnabled - Enable/Disable MLC streamer prefetcher
  @param[in] MlcSpatialPrefetcherEnabled - Enable/Disable MLC spatial prefetcher
//[-start-130906-IB08620308-add]//
  @param[in] DcuStreamerPrefetcher       - Enable/Disable DCU Streamer Prefetcher
  @param[in] DcuIPPrefetcher             - Enable/Disable DCU IP Prefetcher
//[-end-130906-IB08620308-add]//
**/
VOID
ProcessorsPrefetcherInitialization (
  IN UINTN MlcStreamerprefecterEnabled,
  IN UINTN MlcSpatialPrefetcherEnabled
//[-start-130906-IB08620308-add]//
  ,
  IN UINTN DcuStreamerPrefetcher,
  IN UINTN DcuIPPrefetcher
//[-end-130906-IB08620308-add]//
  )
{
  UINT64 MsrValue;
  MsrValue = AsmReadMsr64 (MISC_FEATURE_CONTROL);

  if (MlcStreamerprefecterEnabled == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_MLC_STRP;
  }

  if (MlcSpatialPrefetcherEnabled == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_MLC_SPAP;
  }

//[-start-130906-IB08620308-add]//
  if (DcuStreamerPrefetcher == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_DCU_STRP;
  }

  if (DcuIPPrefetcher == CPU_FEATURE_DISABLE) {
    MsrValue |= B_MISC_FEATURE_CONTROL_DCU_IPP;
  }
//[-end-130906-IB08620308-add]//

//[-start-130906-IB08620308-modify]//
  if ((MsrValue & (B_MISC_FEATURE_CONTROL_MLC_STRP | 
                   B_MISC_FEATURE_CONTROL_MLC_SPAP | 
                   B_MISC_FEATURE_CONTROL_DCU_STRP | 
                   B_MISC_FEATURE_CONTROL_DCU_IPP)) != 0) {
    AsmWriteMsr64 (MISC_FEATURE_CONTROL, MsrValue);
  }
//[-end-130906-IB08620308-modify]//

  return;
}

/**
  Based on ResetType, perform warm or cold reset using PCH Reset PPI

  @param[in] PeiServices       - Indirect reference to the PEI Services Table.
  @param[in] ResetType         - CPU_RESET_TYPE to indicate which reset shoudl be performed.

  @exception EFI_UNSUPPORTED - Reset type unsupported
  @retval EFI_SUCCESS     - function successfully (system should already reset)
**/
EFI_STATUS
PerformWarmORColdReset (
  IN EFI_PEI_SERVICES **PeiServices,
  IN CPU_RESET_TYPE   ResetType
  )
{
  PCH_RESET_PPI *PchResetPpi;
  EFI_STATUS    Status;

  ///
  /// Loccate Reset PPI
  ///
  Status = (*PeiServices)->LocatePpi (
                  (CONST EFI_PEI_SERVICES **)PeiServices,
                  &gPchResetPpiGuid,
                  0,
                  NULL,
                  (VOID **) &PchResetPpi
                  );

  ASSERT_EFI_ERROR (Status);

  ///
  /// Perfrom the requested reset using PCH reset PPI
  ///
  Status = EFI_SUCCESS;
  switch (ResetType) {
    case WARM_RESET:
      PchResetPpi->Reset (PchResetPpi, RESET_PPI_WARM_RESET);
      break;

    case COLDRESET:
      PchResetPpi->Reset (PchResetPpi, RESET_PPI_COLD_RESET);
      break;

    default:
      DEBUG ((EFI_D_ERROR, "CpuInitPeim: PerformWarmORColdReset - ResetType %d not supported: \n", ResetType));
      Status = EFI_UNSUPPORTED;
      ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

/**
  Initializes XE OR Overclocking support in the processor.

  @param[in] PowerMgmtConfig      Pointer to Policy protocol instance

  @retval EFI_SUCCESS
**/
EFI_STATUS
XeInit (
  IN OUT POWER_MGMT_CONFIG_PPI *PowerMgmtConfig,
  IN OUT OVERCLOCKING_CONFIG_PPI *OcConfig
  )
{
  MSR_REGISTER TurboRatioLimit;
  MSR_REGISTER FlexRatioMsr;
  MSR_REGISTER TempMsr;
  MSR_REGISTER RingRatioMsr;
  UINT8        CoreCount;
  UINT8        OverclockingBins;
  UINT8        OneCoreRatioLimit;
  UINT8        TwoCoreRatioLimit;
  UINT8        ThreeCoreRatioLimit;
  UINT8        FourCoreRatioLimit;
  UINT8        RatioLimitProgrammble;
  UINT16       MaxBusRatio;

  if ((PowerMgmtConfig->Xe == 0)) {
    ///
    /// XE is disabled
    ///
    return EFI_SUCCESS;
  }

  TurboRatioLimit.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  OneCoreRatioLimit     = (UINT8) (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_1C);
  TwoCoreRatioLimit = (UINT8) RShiftU64 (
                  (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_2C),
                  N_MSR_TURBO_RATIO_LIMIT_2C
                  );
  ThreeCoreRatioLimit = (UINT8) RShiftU64 (
                  (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_3C),
                  N_MSR_TURBO_RATIO_LIMIT_3C
                  );
  FourCoreRatioLimit = (UINT8) RShiftU64 (
                  (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_4C),
                  N_MSR_TURBO_RATIO_LIMIT_4C
                  );

  ///
  /// Check if XE capable
  ///
  FlexRatioMsr.Qword = AsmReadMsr64 (MSR_FLEX_RATIO);

  ///
  /// Overclocking availablity
  ///
  OverclockingBins = (UINT8) RShiftU64 ((FlexRatioMsr.Dwords.Low & B_OVERCLOCKING_BINS), N_OVERCLOCKING_BINS);

  ///
  /// Check if Turbo Ratio Limit is programmable
  ///  Platform Info MSR (0xCE) [28]
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  RatioLimitProgrammble = (UINT8) RShiftU64 (
                  (TempMsr.Qword & B_PLATFORM_INFO_RATIO_LIMIT),
                  N_PLATFORM_INFO_RATIO_LIMIT
                  );
  ///
  /// Get Maximum Non-Turbo bus ratio (HFM) from Platform Info MSR Bits[15:8]
  ///
  MaxBusRatio = TempMsr.Bytes.SecondByte;

  ///
  /// Check if processor turbo-ratio can be overriden:
  ///   BWG Section 14.14.2
  ///     If PLATFORM INFO MSR [28] == 1 and FLEX_RATIO MSR [19:17] != 0
  ///       Processor with capability to override turbo-ratio detected (either XE or Overclocking part detected)
  ///
  if ((RatioLimitProgrammble == 0) || (OverclockingBins == 0)) {
    ///
    /// Neither XE nor Overclocking Capable processor.
    ///
    DEBUG ((EFI_D_WARN, "WARNING: Trying to configure XE params on a non-XE processor\n"));
    return EFI_SUCCESS;
  }
  ///
  /// For Overclocking parts, verify ratio overide is within the allowable limits
  ///
  if ((RatioLimitProgrammble) && (OverclockingBins < MAX_OVERCLOCKING_BINS)) {
    if (PowerMgmtConfig->RatioLimit[0] > (OneCoreRatioLimit + OverclockingBins)) {
      PowerMgmtConfig->RatioLimit[0] = OneCoreRatioLimit + OverclockingBins;
    }

    if (PowerMgmtConfig->RatioLimit[1] > (TwoCoreRatioLimit + OverclockingBins)) {
      PowerMgmtConfig->RatioLimit[1] = TwoCoreRatioLimit + OverclockingBins;
    }

    if (PowerMgmtConfig->RatioLimit[2] > (ThreeCoreRatioLimit + OverclockingBins)) {
      PowerMgmtConfig->RatioLimit[2] = ThreeCoreRatioLimit + OverclockingBins;
    }

    if (PowerMgmtConfig->RatioLimit[3] > (FourCoreRatioLimit + OverclockingBins)) {
      PowerMgmtConfig->RatioLimit[3] = FourCoreRatioLimit + OverclockingBins;
    }
  }

  if ((RatioLimitProgrammble)) {
    ///
    /// Now initialize turbo ratio limit MSR
    /// Find the number of active cores and initialize the ratio limits only if they are available.
    /// Also program the VID value for the new max Turbo ratio by programming Flex Ratio MSR.
    ///
    TempMsr.Qword = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
    CoreCount     = (UINT8) RShiftU64 (TempMsr.Dwords.Low, N_CORE_COUNT_OFFSET);

    if (PowerMgmtConfig->RatioLimit[0] >= PowerMgmtConfig->RatioLimit[1] &&
        PowerMgmtConfig->RatioLimit[0] >= PowerMgmtConfig->RatioLimit[2] &&
        PowerMgmtConfig->RatioLimit[0] >= PowerMgmtConfig->RatioLimit[3] &&
        PowerMgmtConfig->RatioLimit[1] >= MaxBusRatio &&
        PowerMgmtConfig->RatioLimit[2] >= MaxBusRatio &&
        PowerMgmtConfig->RatioLimit[3] >= MaxBusRatio
        ) {
      if (CoreCount >= 1) {
        TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_1C;
        TurboRatioLimit.Dwords.Low |= PowerMgmtConfig->RatioLimit[0];
      }

      if (CoreCount >= 2) {
        TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_2C;
        TurboRatioLimit.Dwords.Low |= LShiftU64 (PowerMgmtConfig->RatioLimit[1], 8);
      }

      if (CoreCount >= 3) {
        TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_3C;
        TurboRatioLimit.Dwords.Low |= LShiftU64 (PowerMgmtConfig->RatioLimit[2], 16);
      }

      if (CoreCount >= 4) {
        TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_4C;
        TurboRatioLimit.Dwords.Low |= LShiftU64 (PowerMgmtConfig->RatioLimit[3], 24);
      }

      AsmWriteMsr64 (MSR_TURBO_RATIO_LIMIT, TurboRatioLimit.Qword);
    }
  }

  ///
  /// For Overclocking parts, if  a non-default ring ratio is specificed, we need to 
  /// update the ring ratio limit MSR's max limit
  ///
  if ((OverclockingBins != 0) && (OcConfig->ClrMaxOcTurboRatio != 0)) {
    RingRatioMsr.Qword = AsmReadMsr64 (MSR_RING_RATIO_LIMIT);
    RingRatioMsr.Bytes.FirstByte &= ~MSR_MAX_RING_RATIO_LIMIT_MASK;
    RingRatioMsr.Bytes.FirstByte |= OcConfig->ClrMaxOcTurboRatio & MSR_MAX_RING_RATIO_LIMIT_MASK;
    AsmWriteMsr64 (MSR_RING_RATIO_LIMIT, RingRatioMsr.Qword);
  }
  return EFI_SUCCESS;
}

/**
  Initialize performance and power management features

  @param[in] CpuPlatformPolicyPpi    - The CPU Platform Policy PPI instance
**/
VOID
ProcessorsPerfPowerInit (
  IN OUT PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
  EFI_CPUID_REGISTER    Cpuid = { 0, 0, 0, 0 };
  MSR_REGISTER          TempMsr;
  UINT8                 TccActivationOffsetProgrammable;
  UINT16                MaxBusRatio;
  UINT16                MinBusRatio;
  UINT32                Remainder;
  POWER_MGMT_CONFIG_PPI *PowerMgmtConfig;
  MSR_REGISTER          TurboRatioLimit;
  UINT8                 OneCoreRatioLimit;
  MSR_REGISTER          Ia32MiscEnable;
  CPU_FAMILY            CpuFamilyId;
  CPU_STEPPING          CpuStepping;
  UINT8                 TccActivationOffsetMask;
  UINT32                WriteFivrSpreadCmd;
  UINT32                ReadFivrSpreadCmd;
  UINT32                ReadFivrSpreadData;
  UINT32                WriteFivrSpreadData;
  UINT32                MailboxStatus;

  PowerMgmtConfig = CpuPlatformPolicyPpi->PowerMgmtConfig;
  CpuFamilyId = GetCpuFamily();
  CpuStepping = GetCpuStepping();

  ///
  /// Initializes Processor Prefetcher
  ///
  ProcessorsPrefetcherInitialization (
          CpuPlatformPolicyPpi->CpuConfig->MlcStreamerPrefetcher,
          CpuPlatformPolicyPpi->CpuConfig->MlcSpatialPrefetcher
//[-start-130906-IB08620308-add]//
          ,
          CpuPlatformPolicyPpi->CpuConfig->DcuStreamerPrefetcher,
          CpuPlatformPolicyPpi->CpuConfig->DcuIPPrefetcher
//[-end-130906-IB08620308-add]//
          );

  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);

  ///
  /// Get Tcc Activation Offset Programmable Setting from Platform Info MSR Bits[30]
  ///
  TccActivationOffsetProgrammable = (UINT8) RShiftU64 (
                  (TempMsr.Qword & B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET),
                  N_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET
                  );
  ///
  /// Get Maximum Non-Turbo bus ratio (HFM) from Platform Info MSR Bits[15:8]
  ///
  MaxBusRatio = TempMsr.Bytes.SecondByte;
  ///
  /// Get Maximum Efficiency bus ratio (LFM) from Platform Info MSR Bits[47:40]
  ///
  MinBusRatio = TempMsr.Bytes.SixthByte;
  ///
  /// Tcc activation offset in temperature target MSR changes from 4 bits [27:24] to 6 bits [29:24] on ULT C step onwards
  ///
  if ((CpuFamilyId == EnumCpuHswUlt) && (CpuStepping >= EnumHswUltC0)) {
    TccActivationOffsetMask = 0x3F;
  } else {
    TccActivationOffsetMask = 0xF;
  }
  ///
  /// If User slects TccActivationOffset greater than supported progam  max supported.
  ///
  if(PowerMgmtConfig->TccActivationOffset > TccActivationOffsetMask){
      PowerMgmtConfig->TccActivationOffset = TccActivationOffsetMask;
    }
  ///
  ///   First check if TCC Activation Offset is programmable PLATFORM INFO MSR [30]
  ///   If TCC Activation Offset is programable, program the TCC Activation offset value
  ///   from Policy, and the Tcc activation offset programming should be dependency on RESET_CPL done.
  ///
  if (TccActivationOffsetProgrammable) {
    TempMsr.Qword = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    TempMsr.Dwords.Low &= ~(TccActivationOffsetMask << N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT);
    PowerMgmtConfig->TccActivationOffset &= TccActivationOffsetMask;
    TempMsr.Dwords.Low |= LShiftU64 (PowerMgmtConfig->TccActivationOffset, N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT);
    AsmWriteMsr64 (MSR_TEMPERATURE_TARGET, TempMsr.Qword);
  }
  ///
  /// Program Voltage regulator Current Limit's
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_VR_CURRENT_CONFIG);
  TempMsr.Dwords.High &= ~PSIX_THRESHOLD_MASK;
  if (CpuPlatformPolicyPpi->Revision >= PEI_CPU_PLATFORM_POLICY_PPI_REVISION_5) {
    TempMsr.Dwords.High |= ((PowerMgmtConfig->Psi3Threshold  << 20) | (PowerMgmtConfig->Psi2Threshold  << 10) | PowerMgmtConfig->Psi1Threshold );
    if ((CpuFamilyId == EnumCpuHswUlt) && (PowerMgmtConfig->VrPSI4enable)) {
      TempMsr.Dwords.High |= (0x1 << 30);
    }
  }
  if (PowerMgmtConfig->VrCurrentLimit != 0) {
    ///
    /// Check the VR_CURRENT_LOCK bit
    ///
    if ((TempMsr.Dwords.Low & B_CURRENT_LIMIT_LOCK) == 0) {
      TempMsr.Dwords.Low &= ~B_CURRENT_LIMIT_MASK;
      TempMsr.Dwords.Low |= PowerMgmtConfig->VrCurrentLimit;
    } else {
      DEBUG ((EFI_D_INFO, "PPM:: VR Current Limit in MSR_VR_CURRENT_CONFIG is locked\n"));
    }
  }
  ///
  /// Set CURRENT_LIMIT_LOCK bit in VR_Config_MSR
  ///
  if (PowerMgmtConfig->VrCurrentLimitLock) {
    TempMsr.Dwords.Low |= B_CURRENT_LIMIT_LOCK;
  }

  AsmWriteMsr64 (MSR_VR_CURRENT_CONFIG, TempMsr.Qword);

  if (CpuPlatformPolicyPpi->Revision >= PEI_CPU_PLATFORM_POLICY_PPI_REVISION_4) {
    TempMsr.Qword = AsmReadMsr64 (MSR_VR_MISC_CONFIG);
    TempMsr.Dwords.High = RRotU32 (TempMsr.Dwords.High, 8);
    if (PowerMgmtConfig->VrMiscIoutSlope <= 1023) {
      ///
      /// Update IOUT_SLOPE [49:40]
      /// Must be 0 to 0x3FF (1023 decimal)
      ///
      TempMsr.Dwords.High &= ~0x3FF;
      TempMsr.Dwords.High |= PowerMgmtConfig->VrMiscIoutSlope;
    }
    TempMsr.Dwords.High = LRotU32 (TempMsr.Dwords.High, 8);
    if ((PowerMgmtConfig->VrMiscIoutOffset != 0) && (PowerMgmtConfig->VrMiscIoutOffset <= 625)) {
      ///
      /// Update IOUT_OFFSET[39:32]
      /// MSR encoding = int(value*2^8+0.0625) for positive offsets
      ///                inv(int(value*2^8+0.0625))+1 for negative offsets
      /// +0.0625 could be ignored
      ///
      PowerMgmtConfig->VrMiscIoutOffset = (UINT16) DivU64x32Remainder (
                      (UINT64) MultU64x32 (PowerMgmtConfig->VrMiscIoutOffset, (UINT32) LShiftU64 (1, 8)),
                      10000,
                      &Remainder
                      );
      if (Remainder >= 5000) {
        PowerMgmtConfig->VrMiscIoutOffset += 1;
      }
      if (PowerMgmtConfig->VrMiscIoutOffsetSign == 1) {
        ///
        /// This is negative offset
        ///
        PowerMgmtConfig->VrMiscIoutOffset = (UINT8) (~PowerMgmtConfig->VrMiscIoutOffset + 1);
      }
      TempMsr.Dwords.High &= ~0xFF;
      TempMsr.Dwords.High |= PowerMgmtConfig->VrMiscIoutOffset;
    }
    AsmWriteMsr64 (MSR_VR_MISC_CONFIG, TempMsr.Qword);
  }

  if (CpuPlatformPolicyPpi->Revision >= PEI_CPU_PLATFORM_POLICY_PPI_REVISION_3) {
    TempMsr.Qword = AsmReadMsr64 (MSR_VR_MISC_CONFIG);
    TempMsr.Dwords.Low = RRotU32 (TempMsr.Dwords.Low, N_MSR_VR_MISC_CONFIG_MIN_VID_OFFSET);
    ///
    /// Update MIN_VID [31:24]
    /// Must be 0 to 0xFF (255 decimal) resolution 10mV
    /// 0 equal to 0mV; 0xFF (255 decimal) equal to 2550mV
    ///
    TempMsr.Dwords.Low &= ~B_MSR_VR_MISC_CONFIG_MIN_VID_MASK;
    TempMsr.Dwords.Low |= PowerMgmtConfig->VrMiscMinVid;
    TempMsr.Dwords.Low = LRotU32 (TempMsr.Dwords.Low, N_MSR_VR_MISC_CONFIG_MIN_VID_OFFSET);

    ///
    /// Update IDLE_EXIT_RAMP_RATE[50]
    ///
    TempMsr.Qword &= ~B_MSR_VR_MISC_CONFIG_IDLE_EXIT_RAMP_RATE;
    if (PowerMgmtConfig->VrMiscIdleExitRampRate) {
      TempMsr.Qword |= B_MSR_VR_MISC_CONFIG_IDLE_EXIT_RAMP_RATE;
    }

    ///
    /// Update IDLE_ENTRY_RAMP_RATE[51]
    ///
    TempMsr.Qword &= ~B_MSR_VR_MISC_CONFIG_IDLE_ENTRY_RAMP_RATE;
    if (PowerMgmtConfig->VrMiscIdleEntryRampRate) {
      TempMsr.Qword |= B_MSR_VR_MISC_CONFIG_IDLE_ENTRY_RAMP_RATE;
    }

    ///
    /// Update IDLE_ENTRY_DECAY_ENABLE[52]
    ///
    TempMsr.Qword &= ~B_MSR_VR_MISC_CONFIG_IDLE_ENTRY_DECAY_ENABLE;
    if (PowerMgmtConfig->VrMiscIdleEntryDecayEnable) {
      TempMsr.Qword |= B_MSR_VR_MISC_CONFIG_IDLE_ENTRY_DECAY_ENABLE;
    }

    if (CpuFamilyId == EnumCpuHswUlt) {
      ///
      /// Update IDLE_ENTRY_DECAY_ENABLE[52]
      ///
      TempMsr.Qword &= ~B_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_MASK;
      switch (PowerMgmtConfig->VrMiscSlowSlewRateConfig) {
        case 1:
          TempMsr.Qword |= V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_4;
          break;
        case 2:
          TempMsr.Qword |= V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_8;
          break;
        case 3:
          TempMsr.Qword |= V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_16;
          break;
        case 0:
        default:
          TempMsr.Qword |= V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_FAST_2;
          break;
      }
    }
    AsmWriteMsr64 (MSR_VR_MISC_CONFIG, TempMsr.Qword);

    if (CpuFamilyId == EnumCpuHswUlt) {
      TempMsr.Qword = AsmReadMsr64 (MSR_VR_MISC_CONFIG2);
      ///
      /// Update FAST_RAMP_VOLTAGE [7:0]
      /// Must be 0 to 0xFF (255 decimal) resolution 10mV
      /// 0 equal to 0mV; 0xFF (255 decimal) equal to 2550mV
      ///
      TempMsr.Bytes.FirstByte &= ~B_MSR_VR_MISC_CONFIG2_FAST_RAMP_VOLTAGE_MASK;
      TempMsr.Bytes.FirstByte |= PowerMgmtConfig->VrMisc2FastRampVoltage;

      ///
      /// Update MIN_C8_VOLTAGE [15:8]
      /// Must be 0 to 0xFF (255 decimal) resolution 10mV
      /// 0 equal to 0mV; 0xFF (255 decimal) equal to 2550mV
      ///
      TempMsr.Bytes.SecondByte &= ~B_MSR_VR_MISC_CONFIG2_MIN_C8_VOLTAGE_MASK;
      TempMsr.Bytes.SecondByte |= PowerMgmtConfig->VrMisc2MinC8Voltage;

      AsmWriteMsr64 (MSR_VR_MISC_CONFIG2, TempMsr.Qword);
    }
  }
  ///
  /// Initializes XE OR Overclocking support
  ///
  XeInit (CpuPlatformPolicyPpi->PowerMgmtConfig, CpuPlatformPolicyPpi->OverclockingConfig);

  ///
  /// Set processor P state to HFM, LFM or TURBO
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  ///
  /// Check to see if CPU is GV3 capable - GV3 is fused Enabled
  ///
  if ((Cpuid.RegEcx & B_CPUID_VERSION_INFO_ECX_EIST) == B_CPUID_VERSION_INFO_ECX_EIST) {
    Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
    Ia32MiscEnable.Qword |= B_MSR_IA32_MISC_ENABLE_EIST;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);

    TempMsr.Qword = AsmReadMsr64 (MSR_IA32_PERF_CTRL);
    TempMsr.Qword &= ~B_IA32_PERF_CTRLP_STATE_TARGET;

    if ((CpuPlatformPolicyPpi->Revision >= PEI_CPU_PLATFORM_POLICY_PPI_REVISION_3) && (PowerMgmtConfig->BootInLfm == 2)) {
      ///
      /// Set processor P state as TURBO_RATIO_LIMIT_1C if available
      ///
      AsmCpuid (CPUID_POWER_MANAGEMENT_PARAMS, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
      if (((Cpuid.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_TURBO)  == B_CPUID_POWER_MANAGEMENT_EAX_TURBO) ||
          ((Ia32MiscEnable.Qword & B_MSR_IA32_MISC_DISABLE_TURBO) == B_MSR_IA32_MISC_DISABLE_TURBO)) {
        ///
        /// Temporarily enable Turbo and it will be reset in DXE phase
        ///
        Ia32MiscEnable.Qword &= ~B_MSR_IA32_MISC_DISABLE_TURBO;
        AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
        ///
        /// Set processor P state as TURBO_RATIO_LIMIT_1C
        ///
        TurboRatioLimit.Qword = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
        OneCoreRatioLimit     = (UINT8) (TurboRatioLimit.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_1C);
        TempMsr.Qword |= LShiftU64 (OneCoreRatioLimit, N_IA32_PERF_CTRLP_STATE_TARGET);
      } else {
        ///
        /// Turbo is not available, down to HFM
        ///
        DEBUG ((EFI_D_INFO, "CPU: Turbo mode is not available, down to HFM mode.\n"));
        TempMsr.Qword |= LShiftU64 (MaxBusRatio, N_IA32_PERF_CTRLP_STATE_TARGET);
      }
    } else if (PowerMgmtConfig->BootInLfm == CPU_FEATURE_DISABLE) {
      ///
      /// Set processor P state as HFM
      ///
      TempMsr.Qword |= LShiftU64 (MaxBusRatio, N_IA32_PERF_CTRLP_STATE_TARGET);
    } else {
      ///
      /// Set processor P state as LFM
      ///
      TempMsr.Qword |= LShiftU64 (MinBusRatio, N_IA32_PERF_CTRLP_STATE_TARGET);
    }

    AsmWriteMsr64 (MSR_IA32_PERF_CTRL, TempMsr.Qword);
  }

  ///
  /// FIVR Spread Spectrum control is available on C0 steppings onward for all CPU families
  ///
  if ((CpuFamilyId == EnumCpuHswUlt && CpuStepping >= EnumHswUltC0) ||
      (CpuFamilyId == EnumCpuHsw && CpuStepping >= EnumHswC0) ||
      (CpuFamilyId == EnumCpuCrw && CpuStepping >= EnumCrwC0)){

    ///
    /// FivrSscPercent is specified in 1/10th percent increments. Range is 0-249.
    ///
    if (PowerMgmtConfig->FivrSscPercent > MAX_FIVR_SSC_PERCENT) {
      PowerMgmtConfig->FivrSscPercent = MAX_FIVR_SSC_PERCENT;
    }

    ///
    /// Read current FIVR SSC Spread percentage
    ///
    ReadFivrSpreadCmd = 0x8000000E;
    ReadFivrSpreadData = 0;
    MailboxRead(MAILBOX_TYPE_PCODE, ReadFivrSpreadCmd, &ReadFivrSpreadData, &MailboxStatus);

    if (PowerMgmtConfig->FivrSscEnable) {
      WriteFivrSpreadData = ( ((PowerMgmtConfig->FivrSscPercent)*1024)/NUM_TENTHS_TO_PERCENTAGE) | FIVR_SSC_LOCK_BIT;
    }
    else {
      ///
      /// If disabling FIVR SSC, send 0% spread and lock interface
      ///

      WriteFivrSpreadData = FIVR_SSC_LOCK_BIT;
    }

    DEBUG ((EFI_D_INFO, "FIVR: Read FIVR Spread = %X, Write FIVR Spread = %X\n", ReadFivrSpreadData, WriteFivrSpreadData));
    ///
    /// Only update SSC percentage if needed
    ///
    if (ReadFivrSpreadData != WriteFivrSpreadData) {
      DEBUG ((EFI_D_INFO, "FIVR: Update FIVR Spread = %X\n", WriteFivrSpreadData));

      WriteFivrSpreadCmd = 0x8000000F;
      MailboxWrite(MAILBOX_TYPE_PCODE, WriteFivrSpreadCmd, WriteFivrSpreadData, &MailboxStatus);

      if (MailboxStatus != PCODE_MAILBOX_CC_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "FIVR: Error updating FIVR Spread. Mailbox Status = %X\n", MailboxStatus));
      }
    }
  }
  else {
    DEBUG ((EFI_D_ERROR, "FIVR: FIVR Spread adjustment requires C0 stepping or greater. Bypassing FIVR Spread flow.\n"));
  }

  return;
}

/**
  Loads the Processor Microcode & Install the Cache PPI

  @param[in] FfsHeader   - Pointer to an alleged FFS file.
  @param[in] PeiServices - General purpose services available to every PEIM.

  @retval EFI_STATUS - the status code returned from any sub-routine
**/
EFI_STATUS
PeimInitializeCpu (
  IN EFI_PEI_FILE_HANDLE              FileHandle,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS                  Status;
  PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi;

  Status = PeiServicesLocatePpi (
                  &gPeiCpuPlatformPolicyPpiGuid,
                  0,
                  NULL,
                  (VOID **) &CpuPlatformPolicyPpi
                  );
  if (EFI_ERROR (Status)) {
    return Status;

  }

  //
  // Dump Cpu Platform Policy
  //
  CpuPeiPolicyDump(CpuPlatformPolicyPpi);

  ///
  /// Install Cache PPI
  ///
  Status = CacheInitPpiInit ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// Install Notify
  ///
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initializes Overclocking support
  ///
  CpuOcInit((EFI_PEI_SERVICES **)PeiServices, CpuPlatformPolicyPpi);

  ///
  /// Initializes Performance and Power Settings
  ///
  ProcessorsPerfPowerInit (CpuPlatformPolicyPpi);

  ///
  /// Init XMM support
  ///
  XmmInit ();

  ///
  /// Pfat Initialization
  ///
  PfatInit ((EFI_PEI_SERVICES **)PeiServices, CpuPlatformPolicyPpi);

  ///
  /// Boot Guard Initializatoin
  ///
  BootGuardInit ((EFI_PEI_SERVICES **)PeiServices, CpuPlatformPolicyPpi);
  
  return Status;
}

/**
  Build BIST HOB

  @param[in] PeiServices       - Indirect reference to the PEI Services Table.
  @param[in] NotifyDescriptor  - Address of the notification descriptor data structure. Type
                      EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param[in] Ppi               - Address of the PPI that was installed.

  @retval EFI_SUCCESS - Hob built or not necessary
**/
EFI_STATUS
EFIAPI
BuildBistHob (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi
  )
{
  EFI_STATUS                       Status;
  EFI_BOOT_MODE                    BootMode;
  EFI_SEC_PLATFORM_INFORMATION_PPI *SecPlatformInformationPpi;
  UINT64                           InformationSize;
  EFI_SEC_PLATFORM_INFORMATION_RECORD   *SecPlatformInformation;
  VOID                             *Hob;

  Status = (**PeiServices).GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
                  &gEfiSecPlatformInformationPpiGuid,   ///< GUID
                  0,                                    ///< INSTANCE
                  NULL,                                 ///< EFI_PEI_PPI_DESCRIPTOR
                  (VOID ** ) &SecPlatformInformationPpi ///< PPI
                  );

  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  InformationSize         = 0;
  SecPlatformInformation  = NULL;
  Status = SecPlatformInformationPpi->PlatformInformation (
                  (CONST EFI_PEI_SERVICES **)PeiServices,
                  &InformationSize,
                  SecPlatformInformation
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = PeiServicesAllocatePool (
                    (UINTN) InformationSize,
                    (VOID **) &SecPlatformInformation
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = SecPlatformInformationPpi->PlatformInformation (
                    (CONST EFI_PEI_SERVICES **)PeiServices,
                    &InformationSize,
                    SecPlatformInformation
                    );
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hob = BuildGuidDataHob (
                  &gEfiHtBistHobGuid,
                  SecPlatformInformation,
                  (UINTN) InformationSize
                  );
  ASSERT (Hob != NULL);

  return Status;
}

/**
  Dump RC CPU and PPM platform policies

  @param[in] CpuPlatformPolicyPpi  - Address of the cpu platform policy ppi.
**/
VOID
CpuPeiPolicyDump (
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  )
{
#ifdef EFI_DEBUG
  UINTN                 Index;
  CPU_CONFIG_PPI        *CpuConfig;
  POWER_MGMT_CONFIG_PPI *PowerMgmtConfig;
  SECURITY_CONFIG_PPI   *SecurityConfig;
  OVERCLOCKING_CONFIG_PPI *OverclockingConfig;

  CpuConfig = CpuPlatformPolicyPpi->CpuConfig;
  PowerMgmtConfig = CpuPlatformPolicyPpi->PowerMgmtConfig;
  SecurityConfig = CpuPlatformPolicyPpi->SecurityConfig;
  OverclockingConfig = CpuPlatformPolicyPpi->OverclockingConfig;

  DEBUG ((EFI_D_INFO, "\n------------------------ CpuPlatformPolicyPpi Dump Begin -----------------\n"));
  //
  // CPU_CONFIG
  //
  DEBUG ((EFI_D_INFO, " CPU:: BistOnReset : 0x%X\n", CpuConfig->BistOnReset));
  DEBUG ((EFI_D_INFO, " CPU:: HyperThreading : 0x%X\n", CpuConfig->HyperThreading));
  DEBUG ((EFI_D_INFO, " CPU:: CpuRatioOverride : 0x%X\n", CpuConfig->CpuRatioOverride));
  DEBUG ((EFI_D_INFO, " CPU:: VmxEnable : 0x%X\n", CpuConfig->VmxEnable));
  DEBUG ((EFI_D_INFO, " CPU:: Pfat Feature: 0x%X\n", CpuConfig->Pfat));
  DEBUG ((EFI_D_INFO, " CPU:: MlcStreamerPrefetcher : 0x%X\n", CpuConfig->MlcStreamerPrefetcher));
  DEBUG ((EFI_D_INFO, " CPU:: MlcSpatialPrefetcher : 0x%X\n", CpuConfig->MlcSpatialPrefetcher));
  DEBUG ((EFI_D_INFO, " CPU:: ActiveCoreCount : 0x%X\n", CpuConfig->ActiveCoreCount));
  DEBUG ((EFI_D_INFO, " CPU:: CpuRatio : 0x%X\n", CpuConfig->CpuRatio));
  DEBUG ((EFI_D_INFO, " CPU:: MaxNonTurboRatio : 0x%X\n", CpuConfig->CpuMaxNonTurboRatio));
//[-start-130906-IB08620308-add]//
  DEBUG ((EFI_D_INFO, " CPU:: DcuStreamerPrefetcher : 0x%X\n", CpuConfig->DcuStreamerPrefetcher));
  DEBUG ((EFI_D_INFO, " CPU:: DcuIPPrefetcher : 0x%X\n", CpuConfig->DcuIPPrefetcher));
//[-end-130906-IB08620308-add]//
  //
  // POWER_MGMT_CONFIG
  //
  DEBUG ((EFI_D_INFO, " PPM:: BootInLfm : 0x%X\n", PowerMgmtConfig->BootInLfm));
  DEBUG ((EFI_D_INFO, " PPM:: TccActivationOffset : 0x%X\n", PowerMgmtConfig->TccActivationOffset));
  DEBUG ((EFI_D_INFO, " PPM:: VrCurrentLimit : 0x%X\n", PowerMgmtConfig->VrCurrentLimit));
  DEBUG ((EFI_D_INFO, " PPM:: VrCurrentLimitLock : 0x%X\n", PowerMgmtConfig->VrCurrentLimitLock));
  DEBUG ((EFI_D_INFO, " PPM:: Xe : 0x%X\n", PowerMgmtConfig->Xe));
  DEBUG ((EFI_D_INFO, " PPM:: RatioLimit[4] : 0x%X , 0x%X , 0x%X , 0x%X \n",  PowerMgmtConfig->RatioLimit[0], \
          PowerMgmtConfig->RatioLimit[1], \
          PowerMgmtConfig->RatioLimit[2], \
          PowerMgmtConfig->RatioLimit[3]));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscIoutSlope : 0x%X\n", PowerMgmtConfig->VrMiscIoutSlope));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscIoutOffsetSign : 0x%X\n", PowerMgmtConfig->VrMiscIoutOffsetSign));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscIoutOffset : 0x%X\n", PowerMgmtConfig->VrMiscIoutOffset));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscMinVid : 0x%X\n", PowerMgmtConfig->VrMiscMinVid));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscIdleExitRampRate : 0x%X\n", PowerMgmtConfig->VrMiscIdleExitRampRate));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscIdleEntryRampRate : 0x%X\n", PowerMgmtConfig->VrMiscIdleEntryRampRate));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscIdleEntryDecayEnable : 0x%X\n", PowerMgmtConfig->VrMiscIdleEntryDecayEnable));
  DEBUG ((EFI_D_INFO, " PPM:: VrMiscSlowSlewRateConfig : 0x%X\n", PowerMgmtConfig->VrMiscSlowSlewRateConfig));
  DEBUG ((EFI_D_INFO, " PPM:: VrMisc2FastRampVoltage : 0x%X\n", PowerMgmtConfig->VrMisc2FastRampVoltage));
  DEBUG ((EFI_D_INFO, " PPM:: VrMisc2MinC8Voltage : 0x%X\n", PowerMgmtConfig->VrMisc2MinC8Voltage));
  DEBUG ((EFI_D_INFO, " PPM:: FivrSscEnable : 0x%X\n", PowerMgmtConfig->FivrSscEnable));
  DEBUG ((EFI_D_INFO, " PPM:: FivrSscPercent : 0x%X\n", PowerMgmtConfig->FivrSscPercent));

  //
  // SECURITY_CONFIG : PFAT_CONFIG
  //
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : Version : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.Version));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : PlatId[16] :\n"));
  for (Index = 0; Index < 16; Index++) {
    if (Index == 15) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PlatId[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->Ppdt.PlatId[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : PkgAttributes : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.PkgAttributes));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : PslMajorVer : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.PslMajorVer));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : PslMinorVer : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.PslMinorVer));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : ScriptSectionSize : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.ScriptSectionSize));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : DataSectionSize : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.DataSectionSize));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : BiosSvn : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.BiosSvn));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : EcSvn : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.EcSvn));
  DEBUG ((EFI_D_INFO, " PFAT:: PUP_HEADER : VendorSpecific : 0x%X\n", SecurityConfig->PfatConfig->PupHeader.VendorSpecific));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PpdtSize : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PpdtSize));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PpdtMajVer : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PpdtMajVer));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PpdtMinVer : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PpdtMinVer));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PlatId[16] :\n"));
  for (Index = 0; Index < 16; Index++) {
    if (Index == 15) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PlatId[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->Ppdt.PlatId[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PkeySlot0[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PkeySlot0[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->Ppdt.PkeySlot0[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PkeySlot1[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PkeySlot1[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->Ppdt.PkeySlot1[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PkeySlot2[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15)|| (Index == 31)) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PkeySlot2[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->Ppdt.PkeySlot2[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PfatModSvn : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PfatModSvn));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : BiosSvn : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.BiosSvn));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : ExecLim : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.ExecLim));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : PlatAttr : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.PlatAttr));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : EcCmd : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.EcCmd));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : EcData : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.EcData));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : EcCmdGetSvn : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.EcCmdGetSvn));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : EcCmdOpen : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.EcCmdOpen));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : EcCmdClose : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.EcCmdClose));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : EcCmdPortTest : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.EcCmdPortTest));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : LastSfam : 0x%X\n", SecurityConfig->PfatConfig->Ppdt.LastSfam));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : SfamData[64] :\n"));
  //
  // Change the array size according to MAX_SFAM_COUNT
  //
  for (Index = 0; Index < 64; Index++) {
    if ((Index == 15) || (Index == 31) || (Index == 47) || (Index == 63) ) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->Ppdt.SfamData[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->Ppdt.SfamData[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PpdtHash[4] : 0x%lX , 0x%lX , 0x%lX , 0x%lX \n",   SecurityConfig->PfatConfig->PpdtHash[0], \
          SecurityConfig->PfatConfig->PpdtHash[1], \
          SecurityConfig->PfatConfig->PpdtHash[2], \
          SecurityConfig->PfatConfig->PpdtHash[3]));

  DEBUG ((EFI_D_INFO, " PFAT:: NumSpiComponents : 0x%x\n", SecurityConfig->PfatConfig->NumSpiComponents));
  DEBUG ((EFI_D_INFO, " PFAT:: PPDT : ComponentSize[8] :\n"));
  for (Index = 0; Index < SecurityConfig->PfatConfig->NumSpiComponents; Index++) {
    if (Index == 7) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", SecurityConfig->PfatConfig->ComponentSize[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", SecurityConfig->PfatConfig->ComponentSize[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " PFAT:: PfatMemSize : 0x%X\n", SecurityConfig->PfatConfig->PfatMemSize));
  DEBUG ((EFI_D_INFO, " PFAT:: EcCmdDiscovery : 0x%X\n", SecurityConfig->PfatConfig->EcCmdDiscovery));
  DEBUG ((EFI_D_INFO, " PFAT:: EcCmdProvisionEav : 0x%X\n", SecurityConfig->PfatConfig->EcCmdProvisionEav));
  DEBUG ((EFI_D_INFO, " PFAT:: EcCmdLock : 0x%X\n", SecurityConfig->PfatConfig->EcCmdLock));
  DEBUG ((EFI_D_INFO, " PFAT:: PFATLOG:: Version : 0x%X\n", SecurityConfig->PfatConfig->PfatLog.Version));
  DEBUG ((EFI_D_INFO, " PFAT:: PFATLOG:: LastPage : 0x%X\n", SecurityConfig->PfatConfig->PfatLog.LastPage));
  DEBUG ((EFI_D_INFO, " PFAT:: PFATLOG:: LoggingOptions : 0x%X\n", SecurityConfig->PfatConfig->PfatLog.LoggingOptions));
  DEBUG ((EFI_D_INFO, " PFAT:: PFATLOG:: PfatModSvn : 0x%X\n", SecurityConfig->PfatConfig->PfatLog.PfatModSvn));
  DEBUG ((EFI_D_INFO, " PFAT:: PFATLOG:: NumOfEntriesInLog : 0x%X\n", SecurityConfig->PfatConfig->PfatLog.NumOfEntriesInLog));
  //
  // SECURITY_CONFIG : TXT_CONFIG
  //
#ifdef TXT_SUPPORT
  DEBUG ((EFI_D_INFO, " TXT:: SinitMemorySize : 0x%lX\n", SecurityConfig->TxtConfig->SinitMemorySize));
  DEBUG ((EFI_D_INFO, " TXT:: TxtHeapMemorySize : 0x%lX\n", SecurityConfig->TxtConfig->TxtHeapMemorySize));
  DEBUG ((EFI_D_INFO, " TXT:: TxtDprMemoryBase : 0x%lX\n", SecurityConfig->TxtConfig->TxtDprMemoryBase));
  DEBUG ((EFI_D_INFO, " TXT:: TxtDprMemorySize : 0x%lX\n", SecurityConfig->TxtConfig->TxtDprMemorySize));
  DEBUG ((EFI_D_INFO, " TXT:: BiosAcmBase : 0x%lX\n", SecurityConfig->TxtConfig->BiosAcmBase));
  DEBUG ((EFI_D_INFO, " TXT:: BiosAcmSize : 0x%lX\n", SecurityConfig->TxtConfig->BiosAcmSize));
  DEBUG ((EFI_D_INFO, " TXT:: McuUpdateDataAddr : 0x%lX\n", SecurityConfig->TxtConfig->McuUpdateDataAddr));
  DEBUG ((EFI_D_INFO, " TXT:: TgaSize : 0x%lX\n", SecurityConfig->TxtConfig->TgaSize));
  DEBUG ((EFI_D_INFO, " TXT:: TxtLcpPdBase : 0x%lX\n", SecurityConfig->TxtConfig->TxtLcpPdBase));
  DEBUG ((EFI_D_INFO, " TXT:: TxtLcpPdSize : 0x%lX\n", SecurityConfig->TxtConfig->TxtLcpPdSize));
#else
  DEBUG ((EFI_D_INFO, " TXT CONFIG:: UNSUPPORTED \n"));
#endif

  //
  // OVERCLOCKING_CONFIG
  //
  DEBUG ((EFI_D_INFO, " OC:: CoreVoltageOffset : 0x%X\n", OverclockingConfig->CoreVoltageOffset));
  DEBUG ((EFI_D_INFO, " OC:: CoreVoltageOverride : 0x%X\n", OverclockingConfig->CoreVoltageOverride));
  DEBUG ((EFI_D_INFO, " OC:: CoreExtraTurboVoltage : 0x%X\n", OverclockingConfig->CoreExtraTurboVoltage));
  DEBUG ((EFI_D_INFO, " OC:: CoreMaxOcTurboRatio : 0x%X\n", OverclockingConfig->CoreMaxOcTurboRatio));
  DEBUG ((EFI_D_INFO, " OC:: ClrVoltageOffset : 0x%X\n", OverclockingConfig->ClrVoltageOffset));
  DEBUG ((EFI_D_INFO, " OC:: ClrVoltageOverride : 0x%X\n", OverclockingConfig->ClrVoltageOverride));
  DEBUG ((EFI_D_INFO, " OC:: ClrExtraTurboVoltage : 0x%X\n", OverclockingConfig->ClrExtraTurboVoltage));
  DEBUG ((EFI_D_INFO, " OC:: ClrMaxOcTurboRatio : 0x%X\n", OverclockingConfig->ClrMaxOcTurboRatio));
  DEBUG ((EFI_D_INFO, " OC:: SvidVoltageOverride : 0x%X\n", OverclockingConfig->SvidVoltageOverride));
  DEBUG ((EFI_D_INFO, " OC:: SvidEnable : 0x%X\n", OverclockingConfig->SvidEnable));
  DEBUG ((EFI_D_INFO, " OC:: FivrFaultsEnable : 0x%X\n", OverclockingConfig->FivrFaultsEnable));
  DEBUG ((EFI_D_INFO, " OC:: FivrEfficiencyEnable : 0x%X\n", OverclockingConfig->FivrEfficiencyEnable));
  DEBUG ((EFI_D_INFO, " OC:: CoreVoltageMode : 0x%X\n", OverclockingConfig->CoreVoltageMode));
  DEBUG ((EFI_D_INFO, " OC:: ClrVoltageMode : 0x%X\n", OverclockingConfig->ClrVoltageMode));
  DEBUG ((EFI_D_INFO, " OC:: OcSupport : 0x%X\n", OverclockingConfig->OcSupport));

  DEBUG ((EFI_D_INFO, "\n------------------------ CpuPlatformPolicyPpi Dump End -----------------\n\n"));
#endif
}
