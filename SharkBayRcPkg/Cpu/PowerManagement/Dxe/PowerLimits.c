/** @file
  This file contains power management configuration functions for
  Haswell processors.

  Acronyms:
    PPM - Processor Power Management
    TM  - Thermal Monitor
    IST - Intel(R) Speedstep technology
    HT  - Hyper-Threading Technology

@copyright
  Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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

PPM_CTDP_OVERRIDE_TABLE mHswUltPpmCtdpOverideTable[]={
///  TDP  Icc  MSR PL1  MSR PL2   TdpUp   TdpUp   TdpNominal  TdpNominal  TdpDown   TdpDown
///       Max                     PL1     PL2     PL1         PL2         PL1       PL2
  { 5700, 0,  6700,    8375,     0,      8375,   0,          8375,       0,        8375 },  /// 57W Sku Overrides
  { 1500, 0,  2500,    2500,     0,      2500,   0,          2500,       0,        2500 },  /// 15W Sku Overrides
  { 1150, 0,  0,       2500,     0,      2500,   0,          2500,       0,        2500 },  /// 11.5W Sku Overrides
  { 2800, 40,  0,      3500,     0,      3500,   0,          3500,       0,        3500 }   /// 28W 40A Sku Overrides  
};

PPM_CTRL_TDP_SKU_TBL mHswCtrlTdpSkuTable[] = {
  {"Celeron",03,{"2955U","2957U","2005U"}}
};

/**
  Configurable TDP BIOS Initialization

  @param[in] CpuPmConfig      Pointer to policy protocol instance
  @param[in] FvidPointer      Pointer to Fvid Table

  @exception EFI_UNSUPPORTED  Ctdp not Supported
  @retval EFI_SUCCESS         Ctdp Initiation done
**/
EFI_STATUS
InitializeConfigurableTdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  EFI_STATUS Status;
  EFI_STATUS CustomCtdpSettings;
  UINTN      Index;


  if (mPpmGlobalNvsAreaProtocol == NULL) {
    DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP Require mPpmGlobalNvsAreaProtocol.\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Intialize PPM Global NVS with custom CTDP level settings or CPU provided.
  ///
  CustomCtdpSettings = InitCustomConfigurableTdp (CpuPmConfig);
  if (CustomCtdpSettings != EFI_SUCCESS) {
    Status = InitConfigurableTdpSettings (CpuPmConfig);
    if (Status != EFI_SUCCESS) {
      ///
      /// Check for Controllable TDP enable if Ctdp not supported
      ///
      InitControllableTdp(CpuPmConfig);
      return EFI_UNSUPPORTED;
    }
  }
  ///
  /// In case of LFM == TDP Down Ratio/Tdp Nominal , consider TDP Down TAR as the new LFM to insert fake P state.
  ///
  for (Index = 0; Index < (mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported); Index++) {
    if (mMinBusRatio == mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar+1) {
      mMinBusRatio = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar;
      DEBUG ((EFI_D_INFO, "PPM:: mMinBusRatio Modified for Ctdp %d\n", mMinBusRatio));
    }
  }

  return EFI_SUCCESS;
}

/**
  Custom Configurable TDP Table BIOS Initialization

  @param[in] CpuPmConfig      Pointer to policy protocol instance

  @exception EFI_UNSUPPORTED  Custom Ctdp settings are not available
  @retval EFI_SUCCESS         Successfully Initialized Custom Ctdp Settings
**/
EFI_STATUS
InitCustomConfigurableTdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  UINT8 Index;

  if (!CpuPmConfig->pCustomCtdpSettings->ConfigTdpCustom) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// CTC value should not be more Custom configured levels.
  ///
  if (CpuPmConfig->pCustomCtdpSettings->CustomBootModeIndex > CpuPmConfig->pCustomCtdpSettings->CustomTdpCount - 1) {
    CpuPmConfig->pCustomCtdpSettings->CustomBootModeIndex = 0;
  }
  if (mPpmGlobalNvsAreaProtocol != NULL) {
    ///
    /// Update Custom ConfigTdp table for ACPI
    ///
    if (CpuPmConfig->pCustomCtdpSettings->CustomTdpCount != 0) {
      mPpmGlobalNvsAreaProtocol->Area->CustomConfigTdp        = PPM_ENABLE;
      mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported    = CpuPmConfig->pCustomCtdpSettings->CustomTdpCount;
      mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex = CpuPmConfig->pCustomCtdpSettings->CustomBootModeIndex;
      for (Index = 0; Index < (CpuPmConfig->pCustomCtdpSettings->CustomTdpCount); Index++) {
        ///
        /// Verify and fix Custom configured CTDP Levels PL1 and PL2
        ///
        CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit1 = VerifyAndFixCustomPowerLimit (CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit1,mCustomPowerUnit);
        CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit2 = VerifyAndFixCustomPowerLimit (CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit2,mCustomPowerUnit);
        CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomTurboActivationRatio = VerifyAndFixCustomRatio (CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomTurboActivationRatio+1)-1;
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1 = (UINT16) CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit1;
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit2 = (UINT16) CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit2;
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimitWindow = CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit1Time;
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar = CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomTurboActivationRatio;
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpCtc = CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomConfigTdpControl;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Verify and fix Custom Power Limit values

  @param[in] CustomPowerLimit  Custom Power Limit value
  @param[in] CustomPlUnit  Custom Power Limit Unit
**/
UINT16
VerifyAndFixCustomPowerLimit (
  IN UINT32 CustomPowerLimit,
  IN UINT16 CustomPlUnit
  )
{
  UINT16 ConvertedPowerLimit;
  UINT16 CpuConvertedPowerLimitMaxLimit;

  ConvertedPowerLimit = (UINT16) ((CustomPowerLimit * mProcessorPowerUnit) / CustomPlUnit);
  if (mPackageMaxPower == 0 && ConvertedPowerLimit >= mPackageMinPower) {
    ///
    ///  If PACKAGE_POWER_SKU_MSR [46:32] = 0 means there is no upper limit ( since this field is 15 bits, the max value is 2^15 - 1 )
    ///
    CpuConvertedPowerLimitMaxLimit = (UINT16) (LShiftU64 (2, 15) - 1);
    if (ConvertedPowerLimit > CpuConvertedPowerLimitMaxLimit) {
      ///
      /// If new Power Limit 1 is > CpuConvertedPowerLimit1MaxLimit, program Power Limit 1 to CpuConvertedPowerLimit1MaxLimit
      ///
      ConvertedPowerLimit = CpuConvertedPowerLimitMaxLimit;
    }
  } else if (mPackageMinPower == 0 && ConvertedPowerLimit > 0 && ConvertedPowerLimit <= mPackageMaxPower) {
    ///
    ///  If PACKAGE_POWER_SKU_MSR [30:16] = 0 means there is no lower limit
    ///
    ConvertedPowerLimit = (UINT16) ((CustomPowerLimit * mProcessorPowerUnit) / CustomPlUnit);
  } else {
    ///
    /// Power Limit 1 needs to be between mPackageMinPower and mPackageMaxPower
    ///
    CpuConvertedPowerLimitMaxLimit = mPackageMaxPower;
    if (ConvertedPowerLimit < mPackageMinPower) {
      ///
      /// If new Power Limit 1 is < mPackageMinPower, program Power Limit 1 to mPackageMinPower
      ///
      ConvertedPowerLimit = mPackageMinPower;
    } else if (ConvertedPowerLimit > CpuConvertedPowerLimitMaxLimit) {
      ///
      /// If new Power Limit 1 is > mPackageMaxPower, program Power Limit 1 to mPackageMaxPower
      ///
      ConvertedPowerLimit = CpuConvertedPowerLimitMaxLimit;
    }
  }

  return ConvertedPowerLimit;
}

/**
  Verify and fix Custom Ratio values
  Custom Ratio should be between MaxTurboFrequency and LFM

  @param[in] CustomRatio  Custom Ratio value
**/
UINT8
VerifyAndFixCustomRatio (
  IN UINT8 CustomRatio
  )
{
  if (CustomRatio > mTurboBusRatio) {
    ///
    /// Use HFM as max value if Turbo is not supported
    ///
    if (mTurboBusRatio == 0) {
      CustomRatio = (UINT8) mMaxBusRatio;
    } else {
      CustomRatio = (UINT8) mTurboBusRatio;
    }
  } else if (CustomRatio < mMinBusRatio) {
    ///
    /// Use LFM as min value
    ///
    CustomRatio = (UINT8) mMinBusRatio;
  }

  return CustomRatio;
}

/**
  CTDP BIOS settings Initialization(From Msrs)

  @param[in] CpuPmConfig      Pointer to policy protocol instance

  @exception EFI_UNSUPPORTED  Ctdp not supported
  @retval EFI_SUCCESS         Ctdp Settings Initialized successfully from MSRs
**/
EFI_STATUS
InitConfigurableTdpSettings (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER TempMsr;
  UINTN        Index;
  UINT16       CpuConfigTdpNominalTdp;
  UINT16       CpuConfigTdpLevel1Tdp;
  UINT16       CpuConfigTdpLevel2Tdp;
  UINT8        CpuConfigTdpNominalRatio;
  UINT8        CpuConfigTdpLevel1Ratio;
  UINT8        CpuConfigTdpLevel2Ratio;
  UINT16       CpuConfigTdpLevels;

  ///
  /// Get the number of configurable TDP Levels supported
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  TempMsr.Qword &= V_CONFIG_TDP_NUM_LEVELS_MASK;
  CpuConfigTdpLevels = (UINT8) RShiftU64 (TempMsr.Qword, N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET);
  DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP Supported Levels=%d\n", CpuConfigTdpLevels));
  ///
  /// Return if ConfigTDP Levels not supported
  ///
  if (CpuConfigTdpLevels == 0) {
    DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP Levels not supported\n"));
    return EFI_UNSUPPORTED;
  }
  mPpmGlobalNvsAreaProtocol->Area->CustomConfigTdp        = PPM_DISABLE;
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported    = (UINT8) CpuConfigTdpLevels + 1;
  mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex = CpuPmConfig->pTurboSettings->ConfigTdpLevel;
  ///
  /// Get PKG_TDP for Config TDP Nominal
  ///
  TempMsr.Qword             = AsmReadMsr64 (MSR_CONFIG_TDP_NOMINAL);
  CpuConfigTdpNominalRatio  = (UINT8) (TempMsr.Dwords.Low & CONFIG_TDP_NOMINAL_RATIO_MASK);
  CpuConfigTdpNominalTdp    = mPackageTdp;
  DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP Nominal Ratio=%d Tdp=%d\n", CpuConfigTdpNominalRatio, CpuConfigTdpNominalTdp));
  ///
  /// Set Level0 as Tdp Nominal
  ///
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit1 = mPackageTdp;
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit2 = GetCtdpPowerLimit2 (mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit1);
  if (IS_SA_DEVICE_ID_MOBILE(mProcessorFlavor)) {
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimitWindow = MB_POWER_LIMIT1_TIME_DEFAULT;
  } else {
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimitWindow = DT_POWER_LIMIT1_TIME_DEFAULT;
  }
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpTar = (UINT8) (CpuConfigTdpNominalRatio - 1);
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpCtc = CONFIG_TDP_NOMINAL;
  ///
  /// Get PKG_TDP and Ratio for Config TDP Level1
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_CONFIG_TDP_LVL1);
  CpuConfigTdpLevel1Ratio = (UINT8) RShiftU64 (
                  TempMsr.Qword & CONFIG_TDP_LVL1_RATIO_MASK,
                  CONFIG_TDP_LVL1_RATIO_OFFSET
                  );
  CpuConfigTdpLevel1Tdp = (UINT16) (TempMsr.Dwords.Low & CONFIG_TDP_LVL1_PKG_TDP_MASK);
  DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP Level1 Ratio=%d Tdp=%d\n", CpuConfigTdpLevel1Ratio, CpuConfigTdpLevel1Tdp));
  ///
  /// Set Level 1
  ///
  Index = 1;
  if (CpuConfigTdpLevel1Ratio != 0) {
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1 = CpuConfigTdpLevel1Tdp;
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit2 = GetCtdpPowerLimit2 (mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1);
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimitWindow = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimitWindow;
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar = (UINT8) (CpuConfigTdpLevel1Ratio - 1);
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpCtc = CONFIG_TDP_LEVEL1;
    Index++;
  }
  ///
  /// If two levels are supported or Level1 was not valid
  /// then read Level2 registers
  ///
  if (CpuConfigTdpLevels == CONFIG_TDP_LEVEL2 || CpuConfigTdpLevel1Ratio == 0) {
    ///
    /// Get PKG_TDP and Ratio for Config TDP Level2
    ///
    TempMsr.Qword = AsmReadMsr64 (MSR_CONFIG_TDP_LVL2);
    CpuConfigTdpLevel2Ratio = (UINT8) RShiftU64 (
                    TempMsr.Qword & CONFIG_TDP_LVL2_RATIO_MASK,
                    CONFIG_TDP_LVL2_RATIO_OFFSET
                    );
    CpuConfigTdpLevel2Tdp = (UINT16) (TempMsr.Dwords.Low & CONFIG_TDP_LVL2_PKG_TDP_MASK);
    DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP Level2 Ratio=%d Tdp=%d\n", CpuConfigTdpLevel2Ratio, CpuConfigTdpLevel2Tdp));
    ///
    /// Set Level2
    ///
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1 = CpuConfigTdpLevel2Tdp;
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit2 = GetCtdpPowerLimit2 (mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1);
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimitWindow = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimitWindow;
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar = (UINT8) (CpuConfigTdpLevel2Ratio - 1);
    mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpCtc = CONFIG_TDP_LEVEL2;
  }

  return EFI_SUCCESS;
}

/**
  Get Power Limit2 based on Power Limit1 on Config TDP

  @param[in] PowerLimit1  Power Limit 1 Value

  @retval Calculated Power Limit2 value
**/
UINT16
GetCtdpPowerLimit2 (
  IN UINT16 PowerLimit1
  )
{
  UINT16 ConvertedPowerLimit2;
  UINT16 Mutliplier;

  ///
  /// By default,for Mobile & Desktop Processors: Short duration Power Limit  = 1.25 * Package TDP
  ///
  Mutliplier = 125;
  ///
  /// For XE/non-ULV skus Configure PL2 as (1.25 x cTDP).
  ///
  ConvertedPowerLimit2 = IDIV_ROUND ((Mutliplier * PowerLimit1), 100);

  return ConvertedPowerLimit2;
}

/**
  Patch Fvid Table with Ctdp Tar ratio and Tar-1 Ratio

  @param[in] FvidPointer  Pointer to Fvid Table
**/
VOID
CtdpPatchFvidTable (
  IN OUT FVID_TABLE *FvidPointer
  )
{
  UINTN PssIndex;
  UINTN Index;
  UINTN TempRatio;
  UINT8 Turbo;

  ///
  /// Check P0 is Turbo Ratio or HFM
  ///
  Turbo = ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) ? 1 : 0);

  ///
  /// Check and patch Fvid table for TAR ratios
  ///
  for (Index = 0; Index < mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported; Index++) {
    TempRatio = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar;
    for (PssIndex = (Turbo + 2); PssIndex < FvidPointer[0].FvidHeader.Gv3States; PssIndex++) {
      if (FvidPointer[PssIndex].FvidState.BusRatio < TempRatio) {
        if (FvidPointer[PssIndex - 1].FvidState.BusRatio != TempRatio) {
          ///
          /// If Tar not Found ,Replace Turbo Active ratio at PssIndex-1
          /// P0 - Turbo ratio P1- HFM ,exclude these two ratios
          ///
          if (PssIndex == (Turbo + 2)) {
            CtdpReplaceFvidRatio (FvidPointer, PssIndex, TempRatio);
            DEBUG ((EFI_D_INFO, " TAR Ratio Replace at %x with %x \n", PssIndex, TempRatio));
         } else {
            CtdpReplaceFvidRatio (FvidPointer, PssIndex - 1, TempRatio);
            DEBUG ((EFI_D_INFO, " TAR Ratio Replace at %x with %x \n", PssIndex-1, TempRatio));

          }
        }
        break;
      }
    }
  }
  ///
  /// Check and patch Fvid table for CTDP ratios.
  /// This is done separately to make sure Ctdp ratios are not override by Tar ratios
  /// when ctdp ratios are adjacent
  ///
  for (Index = 0; Index < mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported; Index++) {
    TempRatio = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpTar + 1;
    for (PssIndex = (Turbo + 1); PssIndex < FvidPointer[0].FvidHeader.Gv3States; PssIndex++) {

      if (FvidPointer[PssIndex].FvidState.BusRatio == TempRatio) {
         mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPpc = (UINT8) FvidPointer[PssIndex].FvidState.State;
      }

      if (FvidPointer[PssIndex].FvidState.BusRatio < TempRatio) {
        if (FvidPointer[PssIndex - 1].FvidState.BusRatio == TempRatio) {
          ///
          /// Found Turbo Active ratio at PssIndex-1
          ///
          mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPpc = (UINT8) FvidPointer[PssIndex - 1].FvidState.State;
          break;
        } else {
          ///
          /// If Tar not Found, Replace Turbo Active ratio at PssIndex-1
          ///
          if(PssIndex == (Turbo + 1)) {
            CtdpReplaceFvidRatio (FvidPointer, PssIndex, TempRatio);
            mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPpc = (UINT8) FvidPointer[PssIndex].FvidState.State;
            DEBUG ((EFI_D_INFO, " CTDP Ratio Replace at %x with %x \n", PssIndex, TempRatio));
          } else {
            CtdpReplaceFvidRatio (FvidPointer, PssIndex - 1, TempRatio);
            mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPpc = (UINT8) FvidPointer[PssIndex - 1].FvidState.State;
            DEBUG ((EFI_D_INFO, " CTDP Ratio Replace at %x with %x \n", PssIndex-1, TempRatio));

          }
          break;
        }
      }
    }
  }
}

/**
  Replace P state with given ratio

  @param[in] FvidPointer  Pointer to Fvid Table
  @param[in] PssIndex     FVID table index of P state to be replaced
  @param[in] Ratio        Target Ratio to put in
**/
VOID
CtdpReplaceFvidRatio (
  IN OUT FVID_TABLE *FvidPointer,
  UINTN             PssIndex,
  UINTN             Ratio
  )
{
  UINT64 wPower1;
  UINT64 wPower2;

  FvidPointer[PssIndex].FvidState.BusRatio = (UINT16) Ratio; ///< Replace Ratio
  ///
  /// Relative Power calculation per HSW BWG
  ///
  wPower1 = (mMaxBusRatio - FvidPointer[PssIndex].FvidState.BusRatio) * 625;
  wPower1 = (110000 - wPower1);
  wPower1 = DivU64x32 (wPower1, 11);
  wPower1 = MultU64x64 (wPower1, wPower1);
  //
  // Power is calculated in milliwatts
  //
  wPower2 = (((FvidPointer[PssIndex].FvidState.BusRatio * 100) / mMaxBusRatio));
  wPower2 = DivU64x32 (MultU64x32 (MultU64x64 (wPower2, wPower1), mPackageTdpWatt), 10000000);
  FvidPointer[PssIndex].FvidState.Power = (UINT32) wPower2;
}

/**
  Configures following fields of MSR 0x610 based on user configuration:
    Configures Long duration Turbo Mode (power limit 1) power level and time window
    Configures Short duration turbo mode (power limit 2)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigurePowerLimitsNonConfigTdpSkus (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER PakagePowerLimitMsr;
  UINT16       ConvertedPowerLimit1;
  UINT8        ConvertedPowerLimit1Time;
  UINT16       ConvertedShortDurationPowerLimit;
  UINT16       CpuConvertedPowerLimit1MaxLimit;
  UINT16       CpuConvertedPowerLimit2MaxLimit;
  UINT16       Multiplier;

  CpuConvertedPowerLimit1MaxLimit = 0;
  CpuConvertedPowerLimit2MaxLimit = 0;
  ConvertedPowerLimit1Time        = 0;
  ///
  /// By default, for Mobile & Desktop Processors: Short duration Power Limit  = 1.25 * Package TDP
  ///
  Multiplier = 125;
  ///
  ///  Check if TDP limits are programmable
  ///   - Platform Info MSR (0xCE) [29]
  ///
  if (mTdpLimitProgrammble) {
    PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
    ///
    /// Initialize the Power Limit 1 and Power Limit 1 enable bit
    ///  - Power Limit 1: Turbo Power Limit MSR  [14:0]
    ///  - Power Limit 1 Enable: Turbo Power Limit MSR  [15]
    ///
    ///
    /// By default, program Power Limit 1 to Package TDP limit
    ///
    ConvertedPowerLimit1 = mPackageTdp;
    if (CpuPmConfig->pTurboSettings->PowerLimit1 != AUTO) {
      ///
      /// CpuPmConfig->pTurboSettings->PowerLimit1 is in mW or watts. We need to convert it to
      /// CPU Power unit, specified in PACKAGE_POWER_SKU_UNIT_MSR[3:0].
      /// Since we are converting from Watts to CPU power units, multiply by
      /// PACKAGE_POWER_SKU_UNIT_MSR[3:0].
      /// Refer to BWG 14.13.7 for Power Limit 1 limits.
      ///
      ConvertedPowerLimit1 = (UINT16) ((CpuPmConfig->pTurboSettings->PowerLimit1 * mProcessorPowerUnit) / mCustomPowerUnit);
      if (mPackageMaxPower == 0 && ConvertedPowerLimit1 >= mPackageMinPower) {
        ///
        ///  If PACKAGE_POWER_SKU_MSR [46:32] = 0 means there is no upper limit ( since this field is 15 bits, the max value is 2^15 - 1 )
        ///
        CpuConvertedPowerLimit1MaxLimit = (UINT16) (LShiftU64 (2, 15) - 1);
        if (ConvertedPowerLimit1 > CpuConvertedPowerLimit1MaxLimit) {
          ///
          /// If new Power Limit 1 is > CpuConvertedPowerLimit1MaxLimit, program Power Limit 1 to CpuConvertedPowerLimit1MaxLimit
          ///
          ConvertedPowerLimit1 = CpuConvertedPowerLimit1MaxLimit;
        }
      } else if (mPackageMinPower == 0 && ConvertedPowerLimit1 > 0 && ConvertedPowerLimit1 <= mPackageMaxPower) {
        ///
        ///  If PACKAGE_POWER_SKU_MSR [30:16] = 0 means there is no lower limit
        ///
        ConvertedPowerLimit1 = (UINT16) ((CpuPmConfig->pTurboSettings->PowerLimit1 * mProcessorPowerUnit) / mCustomPowerUnit);

      } else {
        ///
        /// Power Limit 1 needs to be between mPackageMinPower and mPackageMaxPower
        ///
        CpuConvertedPowerLimit1MaxLimit = mPackageMaxPower;

        if (ConvertedPowerLimit1 < mPackageMinPower) {
          ///
          /// If new Power Limit 1 is < mPackageMinPower, program Power Limit 1 to mPackageMinPower
          ///
          ConvertedPowerLimit1 = mPackageMinPower;
        } else if (ConvertedPowerLimit1 > CpuConvertedPowerLimit1MaxLimit) {
          ///
          /// If new Power Limit 1 is > mPackageMaxPower, program Power Limit 1 to mPackageMaxPower
          ///
          ConvertedPowerLimit1 = CpuConvertedPowerLimit1MaxLimit;
        }
      }
    }
    PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
    PakagePowerLimitMsr.Dwords.Low |= (UINT32) (ConvertedPowerLimit1);
    DEBUG (
            (EFI_D_INFO,
             "New Power Limit 1  %d watt (%d in CPU power unit)\n",
             CpuPmConfig->pTurboSettings->PowerLimit1,
             ConvertedPowerLimit1)
            );
    ///
    /// Force Power Limit 1 override to be enabled
    ///
    PakagePowerLimitMsr.Dwords.Low |= B_POWER_LIMIT_ENABLE;
    ///
    /// Program Power Limit 1 (Long Duration Turbo) Time Window
    ///  If PowerLimit1Time is AUTO OR If PowerLimit1Time is > MAX_POWER_LIMIT_1_TIME_IN_SECONDS
    ///    program default values
    ///
    if ((CpuPmConfig->pTurboSettings->PowerLimit1Time == AUTO) ||
        (CpuPmConfig->pTurboSettings->PowerLimit1Time > MAX_POWER_LIMIT_1_TIME_IN_SECONDS)
        ) {
      if (IS_SA_DEVICE_ID_MOBILE(mProcessorFlavor)) {
        ///
        /// For Mobile, default value is 28 seconds
        ///
        CpuPmConfig->pTurboSettings->PowerLimit1Time = MB_POWER_LIMIT1_TIME_DEFAULT;
      } else {
        ///
        /// For Desktop, default value is 1 second
        ///
        CpuPmConfig->pTurboSettings->PowerLimit1Time = DT_POWER_LIMIT1_TIME_DEFAULT;
      }
    }
    ConvertedPowerLimit1Time = GetConvertedTime (CpuPmConfig->pTurboSettings->PowerLimit1Time, PL12TimeWindowCovert);
    ///
    ///  Configure Power Limit 1 (Long Duration Turbo) time windows: Turbo Power Limit MSR [23:17]
    ///
    PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_1_TIME_MASK;
    PakagePowerLimitMsr.Dwords.Low |= (UINT32) LShiftU64 (ConvertedPowerLimit1Time, 17);
    ///
    ///  Initialize Short Duration Power limit and enable bit
    ///    Short duration Power Limit: Turbo Power Limit MSR (0x450h) [46:32]
    ///    Short duration Power Limit Enable:Turbo Power Limit MSR (0x450h) [47]
    ///
    /// CpuPmConfig->pTurboSettings->PowerLimit2 value is in mW or watts. We need to convert it to
    /// CPU Power unit, specified in PACKAGE_POWER_SKU_UNIT_MSR[3:0].
    /// Since we are converting from Watts to CPU power units, multiply by
    /// PACKAGE_POWER_SKU_UNIT_MSR[3:0]
    ///
    ConvertedShortDurationPowerLimit = (UINT16) ((CpuPmConfig->pTurboSettings->PowerLimit2 * mProcessorPowerUnit) / mCustomPowerUnit);
    PakagePowerLimitMsr.Dwords.High &= ~(POWER_LIMIT_MASK | B_POWER_LIMIT_ENABLE);
    ///
    /// If PowerLimit2 is AUTO OR if PowerLimit2 is > mPackageMaxPower OR if PowerLimit2 < mPackageMinPower
    /// program defaul values.
    ///
    CpuConvertedPowerLimit2MaxLimit = mPackageMaxPower;
    if (CpuConvertedPowerLimit2MaxLimit == 0) {
      CpuConvertedPowerLimit2MaxLimit = (UINT16) (LShiftU64 (2, 15) - 1);
    }
    if (CpuPmConfig->pTurboSettings->PowerLimit2 == AUTO) {
      ConvertedShortDurationPowerLimit = IDIV_ROUND ((Multiplier * mPackageTdp), 100);

    }
    if (ConvertedShortDurationPowerLimit > CpuConvertedPowerLimit2MaxLimit) {
      ConvertedShortDurationPowerLimit = CpuConvertedPowerLimit2MaxLimit;
    }
    if (ConvertedShortDurationPowerLimit < mPackageMinPower) {
      ConvertedShortDurationPowerLimit = mPackageMinPower;
    }
    PakagePowerLimitMsr.Dwords.High |= (UINT32) (ConvertedShortDurationPowerLimit);

    if (CpuPmConfig->pFunctionEnables->PowerLimit2 == PPM_ENABLE) {
      PakagePowerLimitMsr.Dwords.High |= B_POWER_LIMIT_ENABLE;
    } else {
      PakagePowerLimitMsr.Dwords.High &= (~B_POWER_LIMIT_ENABLE);
    }

    DEBUG (
            (EFI_D_INFO,
             "Short duration Power limit enabled, Power Limit = %d Watts\n",
             CpuPmConfig->pTurboSettings->PowerLimit2)
            );

    DEBUG ((EFI_D_INFO,"MSR(610h)=%08X%08X\n",PakagePowerLimitMsr.Dwords.High,PakagePowerLimitMsr.Dwords.Low));

    AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);
  }

    ///
    /// Enable Power Clamp when Controllable TDP is enabled.
    ///
  if (mControllableTdpEnable) {
    PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
    PakagePowerLimitMsr.Dwords.Low |= B_CRITICAL_POWER_CLAMP_ENABLE;
    DEBUG ((EFI_D_INFO, "Critical Power Clamp1 enabled : %x\n",PakagePowerLimitMsr.Qword));
    AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);
  }

  ///
  ///BIOS to override 41W PL1 for 47W 2C parts (Cahce Size = 3MB)
  ///
  if((CpuPmConfig->pTurboSettings->PowerLimit1 == AUTO) && (mPackageTdp == 47) && (mCpuCacheSize == 3 * 1024)) {
    PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
    PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
    PakagePowerLimitMsr.Dwords.Low |= (UINT32) ((41 * mProcessorPowerUnit) & POWER_LIMIT_MASK);
    AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);
  }
  ///
  /// PL1 and PL2 BIOS Overrides for 57W Non CTDP SKU  
  ///
  if((CpuPmConfig->pTurboSettings->PowerLimit1 == AUTO) &&
     (CpuPmConfig->pTurboSettings->PowerLimit2 == AUTO) &&
     (mPackageTdp == 57 * mProcessorPowerUnit)) {
    PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
    ///
    /// PL1=67W
    ///
    PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
    PakagePowerLimitMsr.Dwords.Low |= (UINT32) ((67 * mProcessorPowerUnit) & POWER_LIMIT_MASK);
    ///
    /// PL2=83.75W
    ///
    PakagePowerLimitMsr.Dwords.High &= ~POWER_LIMIT_MASK;
    PakagePowerLimitMsr.Dwords.High |= (UINT32) (((8375 * mProcessorPowerUnit) / 100) & POWER_LIMIT_MASK);

    AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);
  }
  //
  // End if ((PlatformInfoMsr.Dwords.Low & B_EFI_PLATFORM_INFO_TDC_TDP_LIMIT))
  //
  ///
  /// Pass the power limits of the non-CTDP part to the Global NVS Area for use by DPTF
  ///
  PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit1      = (UINT16) (PakagePowerLimitMsr.Dwords.Low & POWER_LIMIT_MASK);
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit2      = (UINT16) (PakagePowerLimitMsr.Dwords.High & POWER_LIMIT_MASK);
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimitWindow = (UINT8) CpuPmConfig->pTurboSettings->PowerLimit1Time;
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpTar              = (UINT8) mTurboBusRatio;
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpCtc              = 1;
  mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported                       = 1;
  mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex                    = 0;
}

/**
  Configures following fields of MSR 0x615
    Configures power limit 3 power level and time window

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigurePL3PowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER PlatformPowerLimitMsr;
  UINT16       ConvertedPowerLimit3;
  UINT8        ConvertedPowerLimit3Time;   
  CPU_STEPPING CpuSteppingId;
  CPU_FAMILY   CpuFamilyId;

  CpuFamilyId   = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  CpuSteppingId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_STEPPING;
  
  ///
  /// PL3 is supported on HSW ULT C0 & HSW  C0 and later 
  ///
  if(((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0))
    || ((CpuFamilyId == EnumCpuHswUlt) && (CpuSteppingId >= EnumHswUltC0))) {
    ///
    /// Return if No user overrides selected.
    ///
    if((CpuPmConfig->pTurboSettings->PowerLimit3 == AUTO) 
        && (CpuPmConfig->pTurboSettings->PowerLimit3Time == AUTO) 
        && (CpuPmConfig->pTurboSettings->PowerLimit3DutyCycle == AUTO)){
      return;
    }

    PlatformPowerLimitMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_POWER_LIMIT);
    DEBUG ((EFI_D_INFO," PL3 MSR 615 Before Writing %x ",PlatformPowerLimitMsr.Dwords.Low));
    ///
    /// Configure PL3 Power Limit if custom value is avaiable
    ///
    if (CpuPmConfig->pTurboSettings->PowerLimit3 != AUTO) {
      ConvertedPowerLimit3 = (UINT16) ((CpuPmConfig->pTurboSettings->PowerLimit3 * mProcessorPowerUnit) / mCustomPowerUnit);
      PlatformPowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
      PlatformPowerLimitMsr.Dwords.Low |= (UINT32) (ConvertedPowerLimit3);
      PlatformPowerLimitMsr.Dwords.Low |= B_POWER_LIMIT_ENABLE;
    }

    ///
    /// Configure PL3 Time window if custom value is avaiable
    ///
    if (CpuPmConfig->pTurboSettings->PowerLimit3Time != AUTO) {
      ConvertedPowerLimit3Time = GetConvertedTime (CpuPmConfig->pTurboSettings->PowerLimit3Time, PL3TimeWindowConvert);
      PlatformPowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_3_TIME_MASK;
      PlatformPowerLimitMsr.Dwords.Low |= (UINT32) LShiftU64 (ConvertedPowerLimit3Time, 17);
      PlatformPowerLimitMsr.Dwords.Low |= B_POWER_LIMIT_ENABLE;
    }

    ///
    /// Configure PL3 Duty Cycle if custom value is avaiable
    ///
    if (CpuPmConfig->pTurboSettings->PowerLimit3DutyCycle != AUTO) {
      PlatformPowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_3_DUTY_CYCLE_MASK;
      PlatformPowerLimitMsr.Dwords.Low |= (UINT32) LShiftU64 (CpuPmConfig->pTurboSettings->PowerLimit3DutyCycle, 24);
      PlatformPowerLimitMsr.Dwords.Low |= B_POWER_LIMIT_ENABLE;
    }
    //
    // Enable/Disable PL3 lock
    //
    if (CpuPmConfig->pTurboSettings->PowerLimit3Lock == PPM_ENABLE) {
      PlatformPowerLimitMsr.Dwords.Low |= (UINT32) B_POWER_LIMIT_LOCK;
    }else {
      PlatformPowerLimitMsr.Dwords.Low &= (~((UINT32) B_POWER_LIMIT_LOCK));
    }

    AsmWriteMsr64 (MSR_PLATFORM_POWER_LIMIT, PlatformPowerLimitMsr.Qword);
    DEBUG ((EFI_D_INFO," PL3 MSR 615 After Writing %x ",PlatformPowerLimitMsr.Dwords.Low));
  } // End if Processor Check 
}

/**
  Configures following fields of MSR 0x610
    Configures Long duration Turbo Mode (power limit 1) power level and time window
    Configures Short duration turbo mode (power limit 2)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigureCtdpPowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER PakagePowerLimitMsr;
  UINT16       ConvertedPowerLimit1;
  UINT16       ConvertedPowerLimit2;
  UINT8        ConvertedPowerLimit1Time;
  UINT16       Mutliplier;
  UINTN        Index;
  CPU_FAMILY   mCpuFamilyId;

  mCpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  ConvertedPowerLimit1Time = 0;
  ///
  /// By default, for Mobile & Desktop Processors: Short duration Power Limit  = 1.25 * Package TDP
  ///
  Mutliplier = 125;
  //
  // For ConfigTdp enabled skus
  //
  PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
  PakagePowerLimitMsr.Dwords.High &= ~POWER_LIMIT_MASK;
  ///
  /// Initialize the Power Limit 1/2 and Power Limit 2 enable bit in MSR
  ///  Power Limit 1: Turbo Power Limit MSR  [14:0] and Power Limit 2: Turbo Power Limit MSR  [46:32]
  ///  Set MSR value for Power Limit 1/2 to Max Package Power Value or Maximum Supported Value
  ///
  ///
  if (mPackageMaxPower) {
    ConvertedPowerLimit1 = mPackageMaxPower;
    ///
    ///  Short duration Power Limit (PL2)  = 1.25 * PL1
    ///
    ConvertedPowerLimit2 = IDIV_ROUND ((Mutliplier * ConvertedPowerLimit1), 100);
    if (ConvertedPowerLimit2 > PACKAGE_TDP_POWER_MASK) {
      ConvertedPowerLimit2 = PACKAGE_TDP_POWER_MASK;
    }
  } else {
    ///
    /// Set Maximum value for Turbo Power Limit MSR  [14:0] and [46:32] =
    /// Max of CTDP Level Power Limts
    ///
    ConvertedPowerLimit1 = 0;
    for (Index = 0; Index < mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported; Index++) {
      if (ConvertedPowerLimit1 < mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1) {
        ConvertedPowerLimit1 = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit1;
      }
    }
    ConvertedPowerLimit2 = 0;
    for (Index = 0; Index < mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported; Index++) {
      if (ConvertedPowerLimit2 < mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit2) {
        ConvertedPowerLimit2 = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[Index].CtdpPowerLimit2;
      }
    }
  }
  ///
  /// Program Power Limit 1 (Long Duration Turbo) Time Window
  ///  If PowerLimit1Time is AUTO OR If PowerLimit1Time is > MAX_POWER_LIMIT_1_TIME_IN_SECONDS
  ///    program default values
  ///
  if (CpuPmConfig->pCustomCtdpSettings->ConfigTdpCustom == PPM_ENABLE) {
    ConvertedPowerLimit1Time = GetConvertedTime (CpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomPowerLimit1Time, PL12TimeWindowCovert);
  } else {
    if (IS_SA_DEVICE_ID_MOBILE(mProcessorFlavor)) {
      ///
      /// For Mobile, default value is 28 seconds
      ///
      CpuPmConfig->pTurboSettings->PowerLimit1Time = MB_POWER_LIMIT1_TIME_DEFAULT;
    } else {
      ///
      /// For Desktop, default value is 1 second
      ///
      CpuPmConfig->pTurboSettings->PowerLimit1Time = DT_POWER_LIMIT1_TIME_DEFAULT;
    }
    ConvertedPowerLimit1Time = GetConvertedTime (CpuPmConfig->pTurboSettings->PowerLimit1Time, PL12TimeWindowCovert);
  }
  ///
  ///  Configure Power Limit 1 (Long Duration Turbo) time windows: Turbo Power Limit MSR [23:17]
  ///
  PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_1_TIME_MASK;
  PakagePowerLimitMsr.Dwords.Low |= (UINT32) LShiftU64 (ConvertedPowerLimit1Time, 17);
  PakagePowerLimitMsr.Dwords.High |= B_POWER_LIMIT_ENABLE;
  PakagePowerLimitMsr.Dwords.Low |= (UINT32) (ConvertedPowerLimit1);
  PakagePowerLimitMsr.Dwords.High |= (UINT32) (ConvertedPowerLimit2);
  AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);
}

/**
  Configures BIOS overrides in MSR 0x610
    Configures Long duration Turbo Mode (power limit 1) power level and time window
    Configures Short duration turbo mode (power limit 2)

  @param[in] None
**/
VOID
ConfigureCtdpPowerLimitsOverrides (  
  ){
  UINTN         Index;
  UINTN         NoOfOverrides;
  CPU_FAMILY    mCpuFamilyId;
  UINT32        LibStatus;
  UINT32        IccMaxValue;
  EFI_STATUS    Status;
  UINTN         PackageTdp;
  MSR_REGISTER  PakagePowerLimitMsr;
  PPM_CTDP_OVERRIDE_TABLE *PpmCtdpOverideTable;

  mCpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;

  if(mCpuFamilyId == EnumCpuHswUlt) {
    PpmCtdpOverideTable = mHswUltPpmCtdpOverideTable;
    NoOfOverrides = (sizeof(mHswUltPpmCtdpOverideTable))/(sizeof(PPM_CTDP_OVERRIDE_TABLE));
  }else {
    return;
  }

  PackageTdp = (mPackageTdpWatt * 100);
  if((mPackageTdp % mProcessorPowerUnit) !=0) {
    PackageTdp += ((mPackageTdp % mProcessorPowerUnit)* 100) / mProcessorPowerUnit ;
  }

  for(Index = 0; Index < NoOfOverrides;Index++,PpmCtdpOverideTable++) {
    if(PpmCtdpOverideTable->SkuPackageTdp == PackageTdp) {
      
      if(PpmCtdpOverideTable->SkuIccMax) {
      ///
      /// If SkuIccMax is not Zero check ICC Max for SKU detection.if No match go to next entry.
      /// Read Icc Max from BIOS P code Mail box
      ///
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_ICC_MAX_CMD, &IccMaxValue, &LibStatus);
        IccMaxValue = IccMaxValue & 0x0FFF;
        IccMaxValue = IccMaxValue /8;
        if ((Status == EFI_SUCCESS) && (IccMaxValue != 0) && (IccMaxValue != PpmCtdpOverideTable->SkuIccMax)) {
           continue;
        }
      }
  
      DEBUG ((EFI_D_INFO, "PPM:: Ctdp BIOS PL1/PL2 Override Ctdp SKU Found  :%d  override table index :%d\n",PpmCtdpOverideTable->SkuPackageTdp,Index));
      ///
      /// MSR Overrides 
      ///
      PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
      if(PpmCtdpOverideTable->MsrCtdpPowerLimit1) {
        PakagePowerLimitMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
        PakagePowerLimitMsr.Dwords.Low |= ((PpmCtdpOverideTable->MsrCtdpPowerLimit1 * mProcessorPowerUnit)/100) & POWER_LIMIT_MASK;
      }
      if(PpmCtdpOverideTable->MsrCtdpPowerLimit2) {
        PakagePowerLimitMsr.Dwords.High &= ~POWER_LIMIT_MASK;
        PakagePowerLimitMsr.Dwords.High |=((PpmCtdpOverideTable->MsrCtdpPowerLimit2 * mProcessorPowerUnit)/100) & POWER_LIMIT_MASK;
      }
      AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);

      ///
      /// MMIO Overrides
      ///
      if(PpmCtdpOverideTable->CtdpNominalPowerLimit1) {
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit1 = (UINT16)(PpmCtdpOverideTable->CtdpNominalPowerLimit1 * mProcessorPowerUnit)/100;
      }
      if(PpmCtdpOverideTable->CtdpNominalPowerLimit2) {
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit2 = (UINT16)(PpmCtdpOverideTable->CtdpNominalPowerLimit2 * mProcessorPowerUnit)/100;
      }
      if(PpmCtdpOverideTable->CtdpDownPowerLimit1) {
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[1].CtdpPowerLimit1 = (UINT16)(PpmCtdpOverideTable->CtdpDownPowerLimit1 * mProcessorPowerUnit)/100;
      }
      if(PpmCtdpOverideTable->CtdpDownPowerLimit2) {
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[1].CtdpPowerLimit2 = (UINT16)(PpmCtdpOverideTable->CtdpDownPowerLimit2 * mProcessorPowerUnit)/100;
      }
      if(PpmCtdpOverideTable->CtdpDownPowerLimit1) {
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[2].CtdpPowerLimit1 = (UINT16)(PpmCtdpOverideTable->CtdpUpPowerLimit1 * mProcessorPowerUnit)/100;
      }
      if(PpmCtdpOverideTable->CtdpUpPowerLimit2) {
        mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[2].CtdpPowerLimit2 = (UINT16)(PpmCtdpOverideTable->CtdpUpPowerLimit2 * mProcessorPowerUnit)/100;
      }
      break;
    } 
  }
}

/**
  Configure cTDP BIOS MSRs to Boot Ctdp values
    - Configures CONFIG_TDP_CONTROL MSR
    - Configures TURBO_ACTIVATION_RATIO MSR

  @param[in] CpuPmConfig            Pointer to policy protocol instance
  @param[in] CpuConfigTdpBootLevel  ConfigTdpBootLevel policy setting by user
**/
VOID
SelectCtdpLevel (
  IN POWER_MGMT_CONFIG *CpuPmConfig,
  IN UINT8             CpuConfigTdpBootLevel
  )
{
  MSR_REGISTER TempMsr;

  ///
  /// Select cTDP Nominal if Ctdp disabled or boot level not supported.
  ///
  if (CpuConfigTdpBootLevel == CONFIG_TDP_DEACTIVATE || CpuConfigTdpBootLevel >= mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported) {
    CpuConfigTdpBootLevel = 0;
  }

  mCpuConfigTdpBootRatio     = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[CpuConfigTdpBootLevel].CtdpTar+1;
  mPpmGlobalNvsAreaProtocol->Area->ConfigurablePpc = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[CpuConfigTdpBootLevel].CtdpPpc;
  ///
  /// Program the selected level 00:nominal,01:level1,10:level2 to
  /// CONFIG TDP CONTROL MSR.
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_CONFIG_TDP_CONTROL);
  if ((TempMsr.Qword & CONFIG_TDP_CONTROL_LOCK) == 0) {
    TempMsr.Dwords.Low  = (UINT16) TempMsr.Dwords.Low &~CONFIG_TDP_CONTROL_LVL_MASK;
    TempMsr.Dwords.Low  = (UINT16) TempMsr.Dwords.Low | (CpuConfigTdpBootLevel & CONFIG_TDP_CONTROL_LVL_MASK);
    if (CpuPmConfig->pTurboSettings->ConfigTdpLock == PPM_ENABLE) {
      TempMsr.Dwords.Low |= CONFIG_TDP_CONTROL_LOCK;
      DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP MSR_CONFIG_TDP_CONTROL is locked\n"));
    }
    AsmWriteMsr64 (MSR_CONFIG_TDP_CONTROL, TempMsr.Qword);
    DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP MSR_CONFIG_TDP_CONTROL=%x\n", TempMsr.Qword));
  } else {
    DEBUG ((EFI_D_INFO, "PPM:: Could not write MSR_CONFIG_TDP_CONTROL\n"));
  }
  ///
  /// Program the max non-turbo ratio corresponding to default selected level
  /// in TURBO_ACTIVATION_RATIO MSR.
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_TURBO_ACTIVATION_RATIO);
  if ((TempMsr.Qword & MSR_TURBO_ACTIVATION_RATIO_LOCK) == 0) {
    TempMsr.Dwords.Low &= ~MSR_TURBO_ACTIVATION_RATIO_MASK;
    TempMsr.Dwords.Low |= (UINT16) ((mCpuConfigTdpBootRatio-1) & MSR_TURBO_ACTIVATION_RATIO_MASK);
    if (CpuPmConfig->pTurboSettings->ConfigTdpLock == PPM_ENABLE ) {
      TempMsr.Dwords.Low |= MSR_TURBO_ACTIVATION_RATIO_LOCK;
      DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP MSR_TURBO_ACTIVATION_RATIO is locked\n"));
    }
    AsmWriteMsr64 (MSR_TURBO_ACTIVATION_RATIO, TempMsr.Qword);
    DEBUG ((EFI_D_INFO, "PPM:: ConfigTDP MSR_TURBO_ACTIVATION_RATIO=%x\n", TempMsr.Qword));
  } else {
    DEBUG ((EFI_D_INFO, "PPM:: Could not write MSR_TURBO_ACTIVATION_RATIO\n"));
  }
}

/**
  Configures the TURBO_POWER_LIMIT MMIO for Boot ConfigTdp Level

  @param[in] CpuPmConfig            Pointer to policy protocol instance
  @param[in] CpuConfigTdpBootLevel  ConfigTdpBootLevel policy setting by user
**/
VOID
SelectCtdpPowerLimits (
  IN POWER_MGMT_CONFIG *CpuPmConfig,
  IN UINT8             CpuConfigTdpBootLevel
  )
{
  UINTN        PciD0F0RegBase;
  UINTN        MchBar;
  UINT32       Data32And;
  UINT32       Data32Or;
  UINT16       PowerLimit1;
  UINT16       PowerLimit2;
  MSR_REGISTER TempMsr;

  ///
  /// If Ctdp deactivate, Program MSRs to Nominal and MMIO to 0 to enable overclocking
  ///  
  if (CpuConfigTdpBootLevel == CONFIG_TDP_DEACTIVATE) {
    PowerLimit1 = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit1;
    PowerLimit2 = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[0].CtdpPowerLimit2;
    TempMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
    TempMsr.Dwords.Low &= ~POWER_LIMIT_MASK;
    TempMsr.Dwords.Low |= PowerLimit1;
    TempMsr.Dwords.High &= ~POWER_LIMIT_MASK;
    TempMsr.Dwords.High |= PowerLimit2;
    AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, TempMsr.Qword);
    return;
  }

  ///
  /// Select cTDP Nominal if Ctdp disabled or boot level not supported.
  ///
  if (CpuConfigTdpBootLevel >= mPpmGlobalNvsAreaProtocol->Area->CtdpLevelsSupported) {
    CpuConfigTdpBootLevel = 0;
  }

  PowerLimit1 = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[CpuConfigTdpBootLevel].CtdpPowerLimit1;
  PowerLimit2 = mPpmGlobalNvsAreaProtocol->Area->CtdpLevelSettings[CpuConfigTdpBootLevel].CtdpPowerLimit2;
  ///
  /// Get the MCH space base address.
  /// Program Turbo Power Limit MMIO register MCHBAR+0x59A0 Bits [14:0] and [46:32]
  /// for ConfigTdp mode PL1 and PL2
  ///
  PciD0F0RegBase  = MmPciAddress (0, 0, 0, 0, 0);
  MchBar          = MmioRead32 (PciD0F0RegBase + 0x48) &~BIT0;
  ///
  /// Read PowerLimit MSR
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  ///
  /// Program cTDP Power Limit1
  ///
  Data32And = (UINT32) ~(PACKAGE_TDP_POWER_MASK);
  Data32Or  = (UINT32) (PowerLimit1 | (TempMsr.Dwords.Low &~PACKAGE_TDP_POWER_MASK));
  MmioAndThenOr32 (MchBar + MMIO_TURBO_POWER_LIMIT, Data32And, Data32Or);
  SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (MchBar + MMIO_TURBO_POWER_LIMIT),
          1,
          (VOID *) (UINTN) (MchBar + MMIO_TURBO_POWER_LIMIT)
          );
  ///
  /// Program cTDP Power Limit2
  ///
  Data32And = (UINT32) ~(PACKAGE_TDP_POWER_MASK);
  Data32Or  = (UINT32) (PowerLimit2 | (TempMsr.Dwords.High &~PACKAGE_TDP_POWER_MASK));
  MmioAndThenOr32 (MchBar + MMIO_TURBO_POWER_LIMIT + 4, Data32And, Data32Or);
  SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (MchBar + MMIO_TURBO_POWER_LIMIT + 4),
          1,
          (VOID *) (UINTN) (MchBar + MMIO_TURBO_POWER_LIMIT + 4)
          );
}


/**
  Configures following fields of MSR 0x618 based on corresponding MMIO register (MCHBAR+0x58E0):
    Configures Long duration Turbo Mode (power limit 1) power level and time window for DDR domain
    Configures Short duration Turbo Mode (power limit 2) power level and time window for DDR domain

  @param[in] CpuPmConfig     Pointer to policy protocol instance
**/
VOID
ConfigureDdrPowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER  DdrPowerLimitMsr;
  UINTN         PciD0F0RegBase;
  UINTN         MchBar;

  PciD0F0RegBase  = MmPciAddress (0, 0, 0, 0, 0);
  MchBar          = MmioRead32 (PciD0F0RegBase + 0x48) &~BIT0;

  DdrPowerLimitMsr.Qword = 0;
  DdrPowerLimitMsr.Qword = MmioRead64 (MchBar + MMIO_DDR_RAPL_LIMIT);

  DEBUG (
    (EFI_D_INFO,
    "DDR Power Limit 1 = %d\n",
    DdrPowerLimitMsr.Dwords.Low & POWER_LIMIT_MASK)
    );
  DEBUG (
    (EFI_D_INFO,
    "DDR Power Limit 2 = %d\n",
    DdrPowerLimitMsr.Dwords.High & POWER_LIMIT_MASK)
    );

  AsmWriteMsr64 (MSR_DDR_RAPL_LIMIT, DdrPowerLimitMsr.Qword);
}

/**
  Configures PowerLimits and Config TDP values

  @param[in] CpuPmConfig     Pointer to policy protocol instance
**/
VOID
ConfigureCtdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  ///
  /// Configure CTDP power limits.Refer Rev 0.6.0 BWG sec 16.7.1: Enabling Intel Configurable TDP support
  ///
  ConfigureCtdpPowerLimits (CpuPmConfig);

  ///
  /// BIOS power limit overrides
  /// Don't override if custom ctdp settings are provided.
  ///
  if (!CpuPmConfig->pCustomCtdpSettings->ConfigTdpCustom) {
    ConfigureCtdpPowerLimitsOverrides();
  }
  ///
  /// To avoid issues and race conditions it is recommended that the below order of be followed
  ///    - For TDP Up program the Config TDP Level followed by Power Limits
  ///    - For TDP down program the Power Limits followed by Config TDP level
  ///
  if (CpuPmConfig->pTurboSettings->ConfigTdpLevel == CONFIG_TDP_UP) {
    SelectCtdpPowerLimits (CpuPmConfig, mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex);
    SelectCtdpLevel (CpuPmConfig, mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex);
  } else {
    SelectCtdpLevel (CpuPmConfig, mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex);
    SelectCtdpPowerLimits (CpuPmConfig, mPpmGlobalNvsAreaProtocol->Area->ConfigTdpBootModeIndex);
  }
}

EFI_STATUS
InitControllableTdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
/**
  Check for Ctrl TDP enabled SKUs and enable Controllable TDP

  @param[in] CpuPmConfig      Pointer to policy protocol instance

  @exception EFI_UNSUPPORTED   Controllable TDP not Supported
  @retval EFI_SUCCESS         Controllable TDP Supported
**/
{
  UINTN   CpuFamilyIndex;
  UINTN   CpuModelIndex;
  UINT32  CtrlTdpSkuSize;
  UINT8   CtrlTdpSkuDetected;
  CHAR8   BrandIdString[MAXIMUM_CPU_BRAND_STRING_LENGTH + 1];
  CPU_FAMILY            CpuFamilyId;
  CPU_STEPPING          CpuSteppingId;
  EFI_CPUID_REGISTER    CpuExtendedSupport;
  EFI_CPUID_REGISTER    CpuBrandString;
  PPM_CTRL_TDP_SKU_TBL  *CtrlTdpSku;
  MSR_REGISTER          TempMsr;
///
/// Debug
///

  CtrlTdpSkuDetected = 0;
  CtrlTdpSkuSize = 0;

  CpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  CpuSteppingId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_STEPPING;

  if((CpuFamilyId == EnumCpuHswUlt) || (CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    CtrlTdpSku = mHswCtrlTdpSkuTable;
    CtrlTdpSkuSize = 1;
  }else {
    return EFI_UNSUPPORTED;
  } 

  DEBUG ((EFI_D_ERROR, "\n Function InitControllableTdp \n"));
  //
  // Get CPU Brand String
  //
  AsmCpuid (CPUID_EXTENDED_FUNCTION, &CpuExtendedSupport.RegEax, &CpuExtendedSupport.RegEbx, &CpuExtendedSupport.RegEcx, &CpuExtendedSupport.RegEdx);
  AsmCpuid (CPUID_BRAND_STRING1, &CpuBrandString.RegEax, &CpuBrandString.RegEbx, &CpuBrandString.RegEcx, &CpuBrandString.RegEdx);
  //
  // Check if Brand ID String is supported or filled up
  //
  if (CpuExtendedSupport.RegEax == 0) {
    return EFI_UNSUPPORTED;
  }
  CopyMem ((CHAR8 *) &BrandIdString[0],(CHAR8 *) &CpuBrandString,16);
  AsmCpuid (CPUID_BRAND_STRING2, &CpuBrandString.RegEax, &CpuBrandString.RegEbx, &CpuBrandString.RegEcx, &CpuBrandString.RegEdx);
  CopyMem ((CHAR8 *) &BrandIdString[16],(CHAR8 *) &CpuBrandString,16);
  AsmCpuid (CPUID_BRAND_STRING3, &CpuBrandString.RegEax, &CpuBrandString.RegEbx, &CpuBrandString.RegEcx, &CpuBrandString.RegEdx);
  CopyMem ((CHAR8 *) &BrandIdString[31],(CHAR8 *) &CpuBrandString,16);
  BrandIdString[MAXIMUM_CPU_BRAND_STRING_LENGTH] = 0;

  //
  // Check for Match with Controllable TDP SKU list
  //
  for(CpuFamilyIndex = 0;CpuFamilyIndex < CtrlTdpSkuSize;CpuFamilyIndex++) {
    if(AsciiStrStr((CHAR8 *) BrandIdString, (CHAR8 *) CtrlTdpSku[CpuFamilyIndex].CpuFamily)) {
      //
      // If NoOfCpus=0xff ,all processors in the family are ULV skus
      //
      if(CtrlTdpSku[CpuFamilyIndex].NoOfCpus == 0xFF) {
          CtrlTdpSkuDetected = 1;
          DEBUG ((EFI_D_ERROR, "\n Found Control TDP SKU"));
          break;
      }
      for(CpuModelIndex = 0;CpuModelIndex < CtrlTdpSku[CpuFamilyIndex].NoOfCpus; CpuModelIndex++) {
        if(AsciiStrStr((CHAR8 *) BrandIdString,(CHAR8 *) CtrlTdpSku[CpuFamilyIndex].CpuModel[CpuModelIndex])) {
          CtrlTdpSkuDetected = 1;
          DEBUG ((EFI_D_ERROR, "\n Found  Control TDP SKU"));
          break;
        }
      } // end of CpuModelIndex
     break; 
    }
  } // end of if CpuFamilyIndex

  ///
  /// Enable Controllable TDp when
  /// Power Limt1 is less than Package Tdp
  /// Min Power is Zero
  ///
  if (CpuPmConfig->pTurboSettings->PowerLimit1 != AUTO && (CpuPmConfig->pTurboSettings->PowerLimit1 * mProcessorPowerUnit) / mCustomPowerUnit < mPackageTdp 
     && (mPackageMinPower == 0) && (CtrlTdpSkuDetected == 1)){
    mControllableTdpEnable = 1;
    //
    // For ControllableTDP enabled systems add fake P-State below LFM and set PPC as LFM
    //
    mMinBusRatio = mMinBusRatio - 1;
    //
    // Set TAR MSR to LFM-1 (Fake LFM)
    //
    TempMsr.Qword     = AsmReadMsr64 (MSR_TURBO_ACTIVATION_RATIO);
    TempMsr.Dwords.Low &= ~MSR_TURBO_ACTIVATION_RATIO_MASK;
    TempMsr.Dwords.Low |= (UINT16)((mMinBusRatio) & MSR_TURBO_ACTIVATION_RATIO_MASK);
    DEBUG ((EFI_D_INFO, "\n Wrting MSR_TURBO_ACTIVATION_RATIO : %x \n",TempMsr.Qword));
    AsmWriteMsr64 (MSR_TURBO_ACTIVATION_RATIO, TempMsr.Qword);
  }
  return EFI_SUCCESS;
}

