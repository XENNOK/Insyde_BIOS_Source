/** @file
  This file contains P States and Turbo Power Management configuration functions for
  Haswell processors.

  Acronyms:
    PPM - Processor Power Management
    TM  - Thermal Monitor
    IST - Intel(R) Speedstep technology
    HT  - Hyper-Threading Technology

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "PowerMgmtCommon.h"
#include <EfiScriptLib.h>

extern UINT16 mCpuConfigTdpBootRatio;

/**
  Initializes P States and Turbo Power management features
**/
VOID
InitializePStates (
  VOID
  )
{
  MSR_REGISTER          Ia32MiscEnableMsr;

  //
  // InitTurboRatioLimits has to be called before InitGV3 as InitGV3 uses the Turbo Ratio Limit programmed.
  //
  InitTurboRatioLimits (mCpuPmConfig); ///< Initialize InitTurboRatioLimits

  InitEnergyEfficientPState (mCpuPmConfig); ///< Initialize Energy Efficient P-state

  //
  // Initialize P states
  //
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
    InitGv3 (mFvidPointer, mCpuPmConfig);
    mNumberOfStates = mFvidPointer[0].FvidHeader.Gv3States;
  } else {
    //
    // Clear EIST bit in IA32 Misc Enable MSR that was intially set in PEI
    //
    Ia32MiscEnableMsr.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
    Ia32MiscEnableMsr.Qword &= ~B_MSR_IA32_MISC_ENABLE_EIST;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnableMsr.Qword);
  }

  ///
  /// Initialize PAIR Configuration
  /// HSW BWG Rev 0.6.0, Section 16.4.1 Power Aware Interrupt Routing
  ///
  InitPpmIrmConfiguration (mCpuPmConfig);

}

/**
  Initializes Turbo Ratio limits in the processor.

  @param[in] CpuPmConfig  Pointer to PPM Policy protocol instance
**/
VOID
InitTurboRatioLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER TurboRatioLimit;
  MSR_REGISTER CoreThreadCount;
  MSR_REGISTER FlexRatioMsr;
  UINT8        CoreCount;
  UINT8        OverclockingBins;
  UINT8        OneCoreRatioLimit;
  UINT8        TwoCoreRatioLimit;
  UINT8        ThreeCoreRatioLimit;
  UINT8        FourCoreRatioLimit;

  ///
  /// Check if processor turbo-ratio can be overriden
  ///
  //   Haswell BWG Section 15.13.7
  //     If PLATFORM INFO MSR [28] == 1
  //
  if (!mRatioLimitProgrammble) {
    DEBUG ((EFI_D_WARN, "Turbo Ratio Limit is NOT programmable. Platform Info MSR (0xCE) [28] is not set \n"));
    return;
  }
  ///
  /// Read the overclocking bins
  ///
  FlexRatioMsr.Qword  = AsmReadMsr64 (MSR_FLEX_RATIO);
  OverclockingBins    = (UINT8) RShiftU64 ((FlexRatioMsr.Dwords.Low & B_OVERCLOCKING_BINS), 17);
  if (FlexRatioMsr.Dwords.Low & B_OVERCLOCKING_LOCK) { ///<  Check for Overclocking Lock bit
    DEBUG ((EFI_D_ERROR, "ERROR: OverClocking Lock Bit is set. Disable the Lock and reset the system\n"));
    return;
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
  /// For Overclocking and locked parts, verify ratio overide is within the allowable limits
  /// Locked parts will have OverclockingBins value as 0 so the below condition will take care of locked parts also
  ///
  if (OverclockingBins < MAX_OVERCLOCKING_BINS) {
    if (CpuPmConfig->pRatioLimit[0] > (OneCoreRatioLimit + OverclockingBins)) {
      CpuPmConfig->pRatioLimit[0] = OneCoreRatioLimit + OverclockingBins;
    }
    if (CpuPmConfig->pRatioLimit[1] > (TwoCoreRatioLimit + OverclockingBins)) {
      CpuPmConfig->pRatioLimit[1] = TwoCoreRatioLimit + OverclockingBins;
    }
    if (CpuPmConfig->pRatioLimit[2] > (ThreeCoreRatioLimit + OverclockingBins)) {
      CpuPmConfig->pRatioLimit[2] = ThreeCoreRatioLimit + OverclockingBins;
    }
    if (CpuPmConfig->pRatioLimit[3] > (FourCoreRatioLimit + OverclockingBins)) {
      CpuPmConfig->pRatioLimit[3] = FourCoreRatioLimit + OverclockingBins;
    }
  }

  ///
  /// Max Turbo ratio or P0 = Fused 1C Turbo Ratio Limit + No of over clocking Bins.
  ///
  mTurboBusRatio = OneCoreRatioLimit + OverclockingBins;
  ///
  /// Initialize turbo ratio limit MSR.
  /// Find the number of active cores and initialize the ratio limits only if they are available.
  ///
  CoreThreadCount.Qword = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  CoreCount             = (UINT8) RShiftU64 (CoreThreadCount.Dwords.Low, N_CORE_COUNT_OFFSET);
  if (CpuPmConfig->pRatioLimit[0] >= CpuPmConfig->pRatioLimit[1] &&
      CpuPmConfig->pRatioLimit[0] >= CpuPmConfig->pRatioLimit[2] &&
      CpuPmConfig->pRatioLimit[0] >= CpuPmConfig->pRatioLimit[3] &&
      CpuPmConfig->pRatioLimit[1] >= mMaxBusRatio &&
      CpuPmConfig->pRatioLimit[2] >= mMaxBusRatio &&
      CpuPmConfig->pRatioLimit[3] >= mMaxBusRatio
      ) {
    if (CoreCount >= 1) {
      TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_1C;
      TurboRatioLimit.Dwords.Low |= CpuPmConfig->pRatioLimit[0];
    }
    if (CoreCount >= 2) {
      TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_2C;
      TurboRatioLimit.Dwords.Low |= LShiftU64 (CpuPmConfig->pRatioLimit[1], 8);
    }
    if (CoreCount >= 3) {
      TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_3C;
      TurboRatioLimit.Dwords.Low |= LShiftU64 (CpuPmConfig->pRatioLimit[2], 16);
    }
    if (CoreCount >= 4) {
      TurboRatioLimit.Dwords.Low &= ~B_MSR_TURBO_RATIO_LIMIT_4C;
      TurboRatioLimit.Dwords.Low |= LShiftU64 (CpuPmConfig->pRatioLimit[3], 24);
    }
    AsmWriteMsr64 (MSR_TURBO_RATIO_LIMIT, TurboRatioLimit.Qword);
  }
  //
  // For fully unlocked CPU's, configure Turbo Ratio as 0xFF (max possible P-State)
  //
  if (OverclockingBins == MAX_OVERCLOCKING_BINS) {
    mTurboBusRatio  = 0xFF;
  }

  return;
}

/**
  Initializes Energy efficient P-state feature.

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitEnergyEfficientPState (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER       PowerCtlMsr;

  ///
  /// Configure Energy Efficient P-state : POWER_CTL[18]
  ///
  PowerCtlMsr.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  PowerCtlMsr.Dwords.Low &= ~B_ENERGY_EFFICIENT_P_STATE_FEATURE_ENABLE;
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EEPST) {
    PowerCtlMsr.Dwords.Low |= B_ENERGY_EFFICIENT_P_STATE_FEATURE_ENABLE;
  }
  AsmWriteMsr64 (MSR_POWER_CTL, PowerCtlMsr.Qword);

  return;
}

/**
  Sets the MSR_IA32_ENERGY_PERFROMANCE_BIAS.Energy Efficiency Policy.
  This function must be MP safe.

  @param[in] Buffer    Pointer to PPM Policy

  @retval EFI_SUCCESS  Energy policy is set successfully.
**/
EFI_STATUS
EFIAPI
ApSafeSetEnergyPolicy (
  IN OUT VOID *Buffer
  )
{
  POWER_MGMT_CONFIG *CpuPmConfig;
  MSR_REGISTER      Ia32EnergyPerfBiasMsr;

  CpuPmConfig                 = (POWER_MGMT_CONFIG *) Buffer;
  Ia32EnergyPerfBiasMsr.Qword = AsmReadMsr64 (MSR_IA32_ENERGY_PERFORMANCE_BIAS);
  Ia32EnergyPerfBiasMsr.Dwords.Low &= ~B_ENERGY_POLICY_MASK;
  Ia32EnergyPerfBiasMsr.Dwords.Low |= CpuPmConfig->pTurboSettings->EnergyPolicy;
  AsmWriteMsr64 (MSR_IA32_ENERGY_PERFORMANCE_BIAS, Ia32EnergyPerfBiasMsr.Qword);

  return EFI_SUCCESS;
}

//[-start-130906-IB08620309-add]//
/**
  Sets the MSR_IA32_ENERGY_PERFROMANCE_BIAS.Energy Efficiency Policy.
  This function must be MP safe.

  @param[in] Buffer  Pointer to arguments - not used

  @retval EFI_SUCCESS
**/
EFI_STATUS
ApSafeSetEnergyPolicy2 (
  IN OUT VOID *Buffer
  )
{
  POWER_MGMT_CONFIG *CpuPmConfig;
  MSR_REGISTER      Ia32EnergyPerfBiasMsr;
  MSR_REGISTER      PowerCtlMsr;

  PowerCtlMsr.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  if ((PowerCtlMsr.Dwords.Low & B_ENERGY_EFFICIENT_P_STATE_FEATURE_ENABLE) == 0) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// ENERGY_POLICY (R/W) The energy efficiency policy chosen by Operating System software. 
  /// This bit is only writable when POWER_CTL[18] == 1
  /// 0-3 Maximum Power, Aggressive turbo Performance for Energy Efficient P state.
  /// 4-7 Balanced Performance, Maximize performance over for Energy Efficient P state.
  /// 8-11 Balanced Efficiency, 12-15 Low Power, Maximize average performance over time for Energy Efficient P state
  ///

  CpuPmConfig                 = (POWER_MGMT_CONFIG *) mCpuPmConfig;
  Ia32EnergyPerfBiasMsr.Qword = AsmReadMsr64 (MSR_IA32_ENERGY_PERFORMANCE_BIAS);
  Ia32EnergyPerfBiasMsr.Dwords.Low &= ~B_ENERGY_POLICY_MASK;
  Ia32EnergyPerfBiasMsr.Dwords.Low |= CpuPmConfig->pTurboSettings->EnergyPolicy;
  AsmWriteMsr64 (MSR_IA32_ENERGY_PERFORMANCE_BIAS, Ia32EnergyPerfBiasMsr.Qword);

  return EFI_SUCCESS;
}
//[-end-130906-IB08620309-add]//

/**
  Initializes required structures for P-State table creation and enables GV3
  support in the processor.

  @param[in] FvidPointer  Table to update, must be initialized.
  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitGv3 (
  IN OUT FVID_TABLE        *FvidPointer,
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER       Ia32MiscEnableMsr;
  EFI_CPUID_REGISTER Cpuid = { 0, 0, 0, 0 };

  ///
  /// Test for Turbo Mode supported and initialize if true.
  ///
  AsmCpuid (CPUID_POWER_MANAGEMENT_PARAMS, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  Ia32MiscEnableMsr.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) {
    ///
    /// Clear Turbo Mode disable bit in IA32 Misc Enable MSR
    ///
    Ia32MiscEnableMsr.Qword &= ~B_MSR_IA32_MISC_DISABLE_TURBO;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnableMsr.Qword);
  } else if (((Ia32MiscEnableMsr.Qword & B_MSR_IA32_MISC_DISABLE_TURBO) == 0) &&
             ((Cpuid.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_TURBO) == B_CPUID_POWER_MANAGEMENT_EAX_TURBO)) {
    ///
    /// If Turbo mode is supported but required to be disabled (by platform policy setting)
    /// Set Turbo Mode disable bit in IA32 Misc Enable MSR since it might be temporarily enabled earlier.
    ///
    Ia32MiscEnableMsr.Qword |= B_MSR_IA32_MISC_DISABLE_TURBO;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnableMsr.Qword);
  }

  ///
  /// Initialize the FVID tables.
  ///
  InitFvidTable (FvidPointer, FVID_MAX_STATES, FVID_MIN_STEP_SIZE, FALSE);
  ASSERT (FvidPointer->FvidHeader.Gv3States != 0);

  ///
  /// Enable GV3 on all logical processors.
  ///
  RunOnAllLogicalProcessors (ApSafeEnableGv3, NULL);

//[-start-130906-IB08620309-add]//
  ///
  /// Set Energy Policy on all logical processors.
  ///
  RunOnAllLogicalProcessors (ApSafeSetEnergyPolicy2, NULL);
//[-end-130906-IB08620309-add]//

  return;
}

/**
  Enables GV3 support in a logical processor.

  This function must be MP safe.

  @param[in] Buffer  Pointer to arguments - not used

  @retval EFI_SUCCESS
**/
VOID
EFIAPI
ApSafeEnableGv3 (
  IN OUT VOID *Buffer
  )
{
  MSR_REGISTER Ia32MiscEnable;
  MSR_REGISTER MiscPwrMgmt;

  ///
  /// Enable GV3 in the CPU MSR.
  ///
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  Ia32MiscEnable.Qword |= B_MSR_IA32_MISC_ENABLE_EIST;
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);

  ///
  /// If CMP is disabled, disable hardware coordination.
  ///
  if (!(mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP)) {
    MiscPwrMgmt.Qword = AsmReadMsr64 (MSR_MISC_PWR_MGMT);
    MiscPwrMgmt.Qword |= B_MSR_MISC_PWR_MGMT_EIST_HW;
    AsmWriteMsr64 (MSR_MISC_PWR_MGMT, MiscPwrMgmt.Qword);
  }

  return;
}

/**
  Configures the Interrupt Redirection Mode Selection for Logical Interrupts.

  @param[in] CpuPmConfig           Pointer to PPM Policy structure.
**/
VOID
InitPpmIrmConfiguration (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  UINTN  PciD0F0RegBase;
  UINTN  MchBar;
  UINT32 Data32And;
  UINT32 Data32Or;
  UINT8  PpmIrmSetting;

    //
    ///
    /// HSW BWG Rev 0.6.0, Section 16.4.1 Power Aware Interrupt Routing
    /// Program Interrupt Routiong Control register MCHBAR+0x5418 as PAIR with Fixed Priority
    ///
    PpmIrmSetting = 4;
    ///
    /// Get the MCH space base address and program MMIO register MCHBAR+0x5418 to enable specific routing algorithm.
    ///
    PciD0F0RegBase  = MmPciAddress (0, 0, 0, 0, 0);
    MchBar          = MmioRead32 (PciD0F0RegBase + 0x48) &~BIT0;
    Data32And       = (UINT32) ~(BIT2 + BIT1 + BIT0);
    Data32Or        = (UINT32) (PpmIrmSetting & (BIT2 + BIT1 + BIT0));
    MmioAndThenOr32 (MchBar + 0x5418, Data32And, Data32Or);
    SCRIPT_MEM_WRITE (
            EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
            EfiBootScriptWidthUint32,
            (UINTN) (MchBar + 0x5418),
            1,
            (VOID *) (UINTN) (MchBar + 0x5418)
            );
}

/**
  This function updates the table provided with the FVID data for the processor.
  If CreateDefaultTable is TRUE, a minimam FVID table will be provided.
  The maximum number of states must be greater then or equal to two.
  The table should be initialized in such a way as for the caller to determine if the
  table was updated successfully.  This function should be deprecated in the future when
  Release 8 is integrated in favor of the EIST protocol calculating FVID information.

  @param[in] FvidPointer         Pointer to a table to be updated
  @param[in] MaxNumberOfStates   Number of entries in the table pointed to by FvidPointer
  @param[in] MinStepSize         Minimum step size for generating the FVID table
  @param[in] CreateDefaultTable  Create default FVID table rather then full state support
**/
VOID
InitFvidTable (
  IN OUT FVID_TABLE *FvidPointer,
  IN UINT16         MaxNumberOfStates,
  IN UINT16         MinStepSize,
  IN BOOLEAN        CreateDefaultTable
  )
{
  EFI_STATUS Status;

  ///
  /// Return the function, if the FVID tables have already been created.
  ///
  if (FvidPointer[0].FvidHeader.Gv3States != 0) {
    return;
  }
  ///
  /// Create FVID table
  ///
  if (CreateDefaultTable) {
    CreateDefaultFvidTable (FvidPointer);
    mPpmGlobalNvsAreaProtocol->Area->PpmFlags &= ~PPM_TURBO;
  } else {
    Status = CreateFvidTable (FvidPointer, MaxNumberOfStates);
    if (EFI_ERROR (Status)) {
      CreateDefaultFvidTable (FvidPointer);
      mPpmGlobalNvsAreaProtocol->Area->PpmFlags &= ~PPM_TURBO;
    }
  }

  return;
}

/**
  Create default FVID table with max and min states only.

  @param[in] FvidPointer  Pointer to a table to be updated
**/
VOID
CreateDefaultFvidTable (
  IN OUT FVID_TABLE *FvidPointer
  )
{
  UINT64 wPower1;
  UINT64 wPower2;

  ///
  /// Fill in the FVid table header.
  ///
  FvidPointer[0].FvidHeader.Stepping    = mPpmGlobalNvsAreaProtocol->Area->Cpuid;
  FvidPointer[0].FvidHeader.MaxBusRatio = mMaxBusRatio;
  FvidPointer[0].FvidHeader.Gv3States   = 2;
  ///
  /// First entry is state 0, highest state.
  ///
  FvidPointer[1].FvidState.State    = 0;
  FvidPointer[1].FvidState.BusRatio = mMaxBusRatio;
  ///
  /// Power is calculated in milliwatts
  ///
  FvidPointer[1].FvidState.Power = (mPackageTdpWatt * 1000);
  ///
  /// Second entry is state 1, lowest state.
  ///
  FvidPointer[2].FvidState.State    = 1;
  FvidPointer[2].FvidState.BusRatio = (UINT16) mMinBusRatio;
  ///
  /// Calculate Relative Power per HSW BWG (0.6.0 section 13.10.4)
  ///
  wPower1 = (mMaxBusRatio - FvidPointer[2].FvidState.BusRatio) * 625;
  wPower1 = (110000 - wPower1);
  wPower1 = DivU64x32 (wPower1, 11);
  wPower1 = DivU64x32 (MultU64x64 (wPower1, wPower1), 1000);
  //
  // Power is calculated in milliwatts
  //
  wPower2 = (((FvidPointer[2].FvidState.BusRatio * 100000) / mMaxBusRatio) / 100);
  wPower2 = DivU64x32 (MultU64x32 (MultU64x64 (wPower2, DivU64x32 (wPower1, 100)), mPackageTdpWatt), 1000);
  FvidPointer[2].FvidState.Power = (UINT16) wPower2;
}

/**
  Calculate the ratio for the requested p state based on HSW BWG recommendation

  @param[in] MaxRatio           Maximum Supported Ratio (HFM)
  @param[in] MinRatio           Minimum Supported Ratio (LFM)
  @param[in] MaxNumberOfStates  Number of entries in the table pointed to by FvidPointer
  @param[in] PStateNumber       Desired P State from range 0..MaxNumberOfStates

  @retval Ratio for the requested Pstate
**/
UINT16
ComputePstateRatio (
  IN UINT16 MaxRatio,
  IN UINT16 MinRatio,
  IN UINT16 MaxNumberOfStates,
  IN UINT16 PStateNumber
  )
{
  UINT16 RatioRange;
  UINT16 NumGaps;
  UINT16 PStateRatio;

  RatioRange  = MaxRatio - MinRatio;
  NumGaps     = MaxNumberOfStates - 1;
  PStateRatio = MaxRatio - (((PStateNumber * RatioRange) + (NumGaps / 2)) / NumGaps);

  return PStateRatio;
}

/**
  Create an FVID table based on the algorithm provided by the HSW BIOS writer's guide.

  @param[in] FvidPointer         Pointer to a table to be updated
  @param[in] MaxNumberOfStates   Number of entries in the table pointed to by FvidPointer

  @retval EFI_SUCCESS            FVID table created successfully.
  @retval EFI_INVALID_PARAMETER  The bus ratio range don't permit FVID table calculation;
                                 a default FVID table should be constructed.
**/
EFI_STATUS
CreateFvidTable (
  IN OUT FVID_TABLE *FvidPointer,
  IN UINT16         MaxNumberOfStates
  )
{
  UINT16 BusRatioRange;
  UINT16 PowerRange;
  UINT16 NumberOfStates;
  UINT16 Turbo;
  UINT16 index;
  UINT64 wPower1;
  UINT64 wPower2;

  ///
  /// Determine the bus ratio range
  ///
  BusRatioRange = mMaxBusRatio - mMinBusRatio;
  if (((INT16) BusRatioRange < 0) || (MaxNumberOfStates == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Determine the Power range
  ///
  PowerRange = FVID_MAX_POWER - FVID_MIN_POWER;
  ///
  /// Determine the HFM state index
  ///
  Turbo = ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) ? 1 : 0);
  ///
  ///  Determine the number of states as cpu supported range or Maximum _PSS limit
  ///
  NumberOfStates = ((BusRatioRange + 1) < MaxNumberOfStates ? (BusRatioRange + 1) : MaxNumberOfStates);
  ///
  /// Ensure we have at least two states
  ///
  if ((NumberOfStates + Turbo) < 2) {
    ///
    /// In case HFM = LFM and no Turbo, at least have two states with same ratio values
    ///
    NumberOfStates = 2;
  }
  ///
  /// Fill in the table header
  ///
  FvidPointer[0].FvidHeader.Stepping    = mPpmGlobalNvsAreaProtocol->Area->Cpuid;
  FvidPointer[0].FvidHeader.MaxBusRatio = (Turbo ? mTurboBusRatio : mMaxBusRatio);
  FvidPointer[0].FvidHeader.Gv3States = (UINT16) (NumberOfStates < MaxNumberOfStates ? (NumberOfStates + Turbo) : NumberOfStates);
  ///
  /// Add Turbo as P0 if Turbo Mode supported and initialize.
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) {
    FvidPointer[1].FvidState.BusRatio = mTurboBusRatio;
    FvidPointer[1].FvidState.Power = (mPackageTdpWatt * 1000); // power is calculated in milliwatts
    ///
    /// Reserve on P-State for Max Turbo
    ///
    if (NumberOfStates == MaxNumberOfStates) {
      NumberOfStates--;
    }
  }
  ///
  /// Add HFM as P0 or P1 based on Max Turbo availablity
  ///
  FvidPointer[1 + Turbo].FvidState.State    = Turbo;
  FvidPointer[1 + Turbo].FvidState.BusRatio = mMaxBusRatio;
  //
  // Power is calculated in milliwatts
  //
  FvidPointer[1 + Turbo].FvidState.Power = (mPackageTdpWatt * 1000);
  ///
  /// Fill in the table starting at the last entry
  /// The algorithm is available in the processor BIOS writer's guide.
  ///
  for (index = 1; index < NumberOfStates; index++) {
    FvidPointer[index + 1 + Turbo].FvidState.State    = index + Turbo;
    FvidPointer[index + 1 + Turbo].FvidState.BusRatio = ComputePstateRatio (mMaxBusRatio, mMinBusRatio, NumberOfStates, index);
    ///
    /// Calculate Relative Power per HSW BWG
    ///
    wPower1 = (mMaxBusRatio - FvidPointer[index + 1 + Turbo].FvidState.BusRatio) * 625;
    wPower1 = (110000 - wPower1);
    wPower1 = DivU64x32 (wPower1, 11);
    wPower1 = MultU64x64 (wPower1, wPower1);
    ///
    /// Power is calculated in milliwatts
    ///
    wPower2 = (((FvidPointer[index + 1 + Turbo].FvidState.BusRatio * 100) / mMaxBusRatio));
    wPower2 = DivU64x32 (MultU64x32 (MultU64x64 (wPower2, wPower1), mPackageTdpWatt), 10000000);
    FvidPointer[index + 1 + Turbo].FvidState.Power = (UINT32) wPower2;
    ///
    /// For Controllable Tdp -- Configure PPC as LFM (i.e fake LFM + 1) 
    ///
    if (mControllableTdpEnable == 1 && FvidPointer[index + 1 + Turbo].FvidState.BusRatio == (mMinBusRatio + 1)) {
       mPpmGlobalNvsAreaProtocol->Area->ConfigurablePpc = (UINT8)FvidPointer[index + 1 + Turbo].FvidState.State;
    }
  }

  return EFI_SUCCESS;
}

/**
  Set processor P state to HFM or LFM.

  @exception EFI_UNSUPPORTED  EIST not supported.
  @retval EFI_SUCCESS         Processor P state has been set.
**/
VOID
SetBootPState (
  VOID
  )
{
  MSR_REGISTER Ia32MiscEnable;
  MSR_REGISTER Ia32PerfCtl;
  BOOLEAN      EistEnabled;

  ///
  /// This function will be executed even when EIST is disabled so processor can be switched to HFM
  /// Only skip this when EIST is not capable.
  ///
  if ((mCpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_EIST) == 0) {
    return;
  }
  ///
  /// Read EIST.
  ///
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  EistEnabled = (BOOLEAN) RShiftU64 (
                  (Ia32MiscEnable.Qword & B_MSR_IA32_MISC_ENABLE_EIST),
                  N_MSR_IA32_MISC_ENABLE_EIST_OFFSET
                  );
  ///
  /// If EIST is disabled, temporarily enable it
  ///
  if (EistEnabled == 0) {
    Ia32MiscEnable.Qword |= B_MSR_IA32_MISC_ENABLE_EIST;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  }
  Ia32PerfCtl.Qword = AsmReadMsr64 (MSR_IA32_PERF_CTRL);
  Ia32PerfCtl.Qword &= B_IA32_PERF_CTRLP_STATE_TARGET;
  mBspBootRatio = (UINT16) RShiftU64 (Ia32PerfCtl.Qword, N_IA32_PERF_CTRLP_STATE_TARGET);
  ///
  /// Set P-state on all cores
  ///
  RunOnAllLogicalProcessors (ApSafeSetBootPState, NULL);
  ///
  /// Disable EIST if we enabled it previously
  ///
  if (EistEnabled == 0) {
    Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
    Ia32MiscEnable.Qword &= ~B_MSR_IA32_MISC_ENABLE_EIST;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  }

  return;
}

/**
  Set processor P state based on Boot ConfigTdp level.

  @param[in] Buffer    Unused

  @retval EFI_SUCCESS  Processor MSR setting is saved.
**/
VOID
EFIAPI
ApSafeSetBootPState (
  IN OUT VOID *Buffer
  )
{
  MSR_REGISTER Ia32PerfCtl;
  UINT16       BootRatio;

  Ia32PerfCtl.Qword = AsmReadMsr64 (MSR_IA32_PERF_CTRL);
  Ia32PerfCtl.Qword &= ~B_IA32_PERF_CTRLP_STATE_TARGET;
  if (mCpuConfigTdpBootRatio != 0) {
    ///
    /// For ConfigTDP enabled SKU use (ConfigTDP boot ratio - 1 / TAR Ratio) as max non-turbo ratio
    ///
    BootRatio = mCpuConfigTdpBootRatio-1;
    //
    // If EIST is disabled use boot ratio ConfigTDP boot ratio / TAR+1.
    //
    if((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST)== 0) {
     BootRatio = mCpuConfigTdpBootRatio;
    }
  } else {
    ///
    /// For Non-ConfigTDP enabled SKU set BSP ratio on all threads.
    ///
    BootRatio = mBspBootRatio;
  }
  Ia32PerfCtl.Qword |= LShiftU64 (BootRatio, N_IA32_PERF_CTRLP_STATE_TARGET);
  AsmWriteMsr64 (MSR_IA32_PERF_CTRL, Ia32PerfCtl.Qword);

  return;
}

//
// Update ACPI PerfomanceStates tables
//

/**
  Patch the native _PSS package with the GV3 values
  Uses ratio/VID values from the FVID table to fix up the control values in the _PSS.

  (1) Find _PSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the _PSS AML name object.
  (2) Resize the _PSS package.
  (3) Fix up the _PSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.

  @retval EFI_SUCCESS   - on success
  @retval EFI_NOT_FOUND - if _PR_.CPU0 scope is not foud in the ACPI tables
**/
EFI_STATUS
AcpiPatchPss (
  VOID
  )
{
  UINT8              *CurrPtr;
  UINT8              *EndOfTable;
  UINT8              index;
  UINT16             NewPackageLength;
  UINT16             MaxPackageLength;
  UINT16             Temp;
  UINT16             *PackageLength;
  UINT16             *ScopePackageLengthPtr;
  UINT32             *Signature;
  PSS_PACKAGE_LAYOUT *PssPackage;
  MSR_REGISTER        TempMsr;
  UINT16              MaximumEfficiencyRatio;
  UINT16              MaximumNonTurboRatio;
  UINT16              PnPercent;

  ScopePackageLengthPtr = NULL;
  PssPackage            = NULL;

  //
  // Get Maximum Efficiency bus ratio (LFM) from Platform Info MSR Bits[47:40]
  // Get Maximum Non Turbo bus ratio from Platform Info MSR Bits[15:8]
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  MaximumEfficiencyRatio = TempMsr.Bytes.SixthByte;
  MaximumNonTurboRatio = TempMsr.Bytes.SecondByte;

  ///
  /// Calculate new package length
  ///
  NewPackageLength      = Temp = (UINT16) (mNumberOfStates * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  MaxPackageLength      = (UINT16) (FVID_MAX_STATES * sizeof (PSS_PACKAGE_LAYOUT) + 3);
  ///
  /// Locate the SSDT package in the IST table
  ///
  CurrPtr     = (UINT8 *) mCpu0IstTable;
  EndOfTable  = (UINT8 *) (CurrPtr + mCpu0IstTable->Length);
  for (CurrPtr; CurrPtr <= EndOfTable; CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    ///
    /// If we find the _PR_CPU0 scope, save a pointer to the package length
    ///
    if ((*CurrPtr == AML_SCOPE_OP) &&
        (*(Signature + 1) == SIGNATURE_32 ('_', 'P', 'R', '_')) &&
        (*(Signature + 2) == SIGNATURE_32 ('C', 'P', 'U', '0'))
        ) {
      ScopePackageLengthPtr = (UINT16 *) (CurrPtr + 1);
    }
    ///
    /// Patch the native _PSS package with the GV3 values
    ///
    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('_', 'P', 'S', 'S'))) {
      ///
      /// Check table dimensions.
      /// PSS package reserve space for FVID_MAX_STATES number of P-states so check if the
      /// current number of P- states is more than FVID_MAX_STATES. Also need to update the SSDT contents
      /// if the current number of P-states is less than FVID_MAX_STATES.
      ///
      ASSERT (mNumberOfStates <= FVID_MAX_STATES);
      if (mNumberOfStates <= FVID_MAX_STATES) {
        *(CurrPtr + 8)  = (UINT8) mNumberOfStates;
        PackageLength   = (UINT16 *) (CurrPtr + 6);
        ///
        /// Update the Package length in AML package length format
        ///
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
        ///
        /// Move SSDT contents
        ///
        CopyMem (
                (CurrPtr + NewPackageLength),
                (CurrPtr + MaxPackageLength),
                EndOfTable - (CurrPtr + MaxPackageLength)
                );
        ///
        /// Save the new end of the SSDT
        ///
        EndOfTable = EndOfTable - (MaxPackageLength - NewPackageLength);
      }
      PssPackage = (PSS_PACKAGE_LAYOUT *) (CurrPtr + 9);
      for (index = 1; index <= mNumberOfStates; index++) {
        ///
        /// Update the _PSS table
        ///
        ///
        /// If Turbo mode is supported, add one to the Max Non-Turbo frequency
        ///
        if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) && (index == 1)) {
            PssPackage->CoreFrequency = (UINT32)((mFvidPointer[index + 1].FvidState.BusRatio)* 100)+1;
        }else if (mFvidPointer[index].FvidState.BusRatio < MaximumEfficiencyRatio) {
          //
          // If cTDP Down Ratio == LFM, set it to 1% lower than LFM.
          //
          PnPercent = (MaximumEfficiencyRatio * 100) / MaximumNonTurboRatio;
          PssPackage->CoreFrequency = (MaximumNonTurboRatio * (PnPercent - 1)); // Simplified Calculation.
        } else {
          PssPackage->CoreFrequency = (UINT32)(mFvidPointer[index].FvidState.BusRatio) * 100;
        }
        PssPackage->Power = (UINT32) mFvidPointer[index].FvidState.Power;
        ///
        /// If it's PSS table, Control is the PERF_CTL value.
        /// Status entry is the same as control entry.
        /// TransLatency uses 10
        ///
        PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
        PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
        //
        // Ensure any future OS would not look for the IA32_PERF_STATUS MSR to check if the value matches
        //
        if (mFvidPointer[index].FvidState.BusRatio < MaximumEfficiencyRatio) {
          PssPackage->Status        = (UINT32) LShiftU64 (MaximumEfficiencyRatio, 8);
        } else {
          PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
        }
        PssPackage->BMLatency     = PSTATE_BM_LATENCY;
        PssPackage++;
      }
    }
  }
  ASSERT (ScopePackageLengthPtr != NULL);
  if (ScopePackageLengthPtr == NULL) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Update the Package length in AML package length format
  ///
  CurrPtr                 = (UINT8 *) ScopePackageLengthPtr;
  NewPackageLength        = Temp = (UINT16) (EndOfTable - CurrPtr);
  *ScopePackageLengthPtr  = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
  mCpu0IstTable->Length   = (UINT32) (EndOfTable - (UINT8 *) mCpu0IstTable);

  return EFI_SUCCESS;
}

/**
  Completes processor power management initialization
    (1) Initializes the TSC update variables.
    (2) Initializes the GV state for processors.
    (3) Adds a callback (SMI) in S3 resume script to restore the MSR
    (4) Registers callback (SMI) for late PPM Initialization
**/
VOID
PpmPostInit (
  VOID
  )
{
  UINT8 Data8;

  ///
  /// Set Boot P-state based on Policy.
  ///
  SetBootPState ();
  ///
  /// Save the SW SMI number to trigger SMI to restore the MSRs when resuming from S3
  ///
  Data8 = mCpuPmConfig->S3RestoreMsrSwSmiNumber;
  SCRIPT_IO_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (R_PCH_APM_CNT),
          1,
          &Data8
          );
  ///
  /// Lock down all settings
  ///
  PpmLockDown (mCpuPmConfig);
}
