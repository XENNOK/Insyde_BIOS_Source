/** @file
  Processor Power Management initialization code. This code determines current
  user configuration and modifies and loads ASL as well as initializing chipset
  and processor features to enable the proper power management.

  Acronyms:
    PPM - Processor Power Management
    TM  - Thermal Monitor
    IST - Intel(R) Speedstep technology
    HT  - Hyper-Threading Technology

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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

#include "PowerMgmtInit.h"

//
// Global variables
//

///
/// Power Managment policy configurations
///
POWER_MGMT_CONFIG  *mCpuPmConfig              = NULL;
EFI_CPUID_REGISTER mCpuid01                   = { 0, 0, 0, 0 };            // CPUID 01 values
//
// Values for FVID table calculate.
//
UINT16 mTurboBusRatio             = 0;
UINT16 mMaxBusRatio               = 0;
UINT16 mMinBusRatio               = 0;
UINT16 mProcessorFlavor           = 0;
UINT16 mBspBootRatio              = 0;
UINT16 mPackageTdp                = 0;                        ///< Processor TDP value in MSR_PACKAGE_POWER_SKU.
UINT16 mPackageTdpWatt            = 0;                        ///< Processor TDP value in Watts.
UINT16 mCpuConfigTdpBootRatio     = 0;                        ///< Config TDP Boot settings
UINT16 mCustomPowerUnit           = 1;
UINT16 mCpuCacheSize              = 0;                                     ///< Cache Size in KB
UINT8  mCpuPolicyRevision         = 0;
DXE_CPU_PLATFORM_POLICY_PROTOCOL *mCpuPlatformPolicy = NULL;

///
/// Fractional part of Processor Power Unit in Watts. (i.e. Unit is 1/mProcessorPowerUnit)
///
UINT8 mProcessorPowerUnit        = 0;
///
/// Maximum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
UINT16 mPackageMaxPower           = 0;
///
/// Minimum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
UINT16 mPackageMinPower           = 0;

UINT8                        mControllableTdpEnable     = 0;     ///< Controllable TDP enable/Disable
UINT8                        mRatioLimitProgrammble     = 0;     ///< Programmable Ratio Limit
UINT8                        mTdpLimitProgrammble       = 0;     ///< Porgrammable TDP Limit
PPM_GLOBAL_NVS_AREA_PROTOCOL *mPpmGlobalNvsAreaProtocol  = NULL; ///< Ppm GlobalNvs Protocol
FRAMEWORK_EFI_MP_SERVICES_PROTOCOL      *mMpService                = NULL;  ///< Mp Services Protocol

//
// FVID Table Information
// Default FVID table
// One header field plus states
//
UINT16     mNumberOfStates            = 0;
FVID_TABLE mEmptyFvidTable[FVID_MAX_STATES + 1];
FVID_TABLE *mFvidPointer              = &mEmptyFvidTable[0];

//
// Globals to support updating ACPI Tables
//
EFI_ACPI_SUPPORT_PROTOCOL   *mAcpiSupport              = NULL;
EFI_ACPI_TABLE_PROTOCOL     *mAcpiTable                = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mCpu0IstTable             = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mApIstTable               = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mCpu0CstTable             = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mApCstTable               = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mCpuPmTable               = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mCpu0TstTable             = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mApTstTable               = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mLakeTinyTable            = NULL;
EFI_ACPI_DESCRIPTION_HEADER *mCtdpTable                = NULL;

/**
  Initialize the power management support.
  This function will do boot time configuration:
    Detect HW capabilities and SW configuration
    Initialize HW and software state (primarily MSR and ACPI tables)

  @param[in] ImageHandle  Pointer to the loaded image protocol for this driver
  @param[in] SystemTable  Pointer to the EFI System Table

  @retval EFI_SUCCESS     The driver installes/initialized correctly.
  @retval Driver will ASSERT in debug builds on error.  PPM functionality is considered critical for mobile systems.
**/
EFI_STATUS
EFIAPI
InitializePowerManagement (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  PPM_GLOBAL_NVS_AREA_PROTOCOL     PpmGlobalNvsAreaProtocol;

  Handle            = NULL;

  ///
  /// Locate platform configuration information
  ///
  Status = gBS->LocateProtocol (&gDxeCpuPlatformPolicyProtocolGuid, NULL, (VOID **) &mCpuPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize the Global pointer for Power Managment Platform policy
  ///
  mCpuPmConfig = mCpuPlatformPolicy->PowerMgmtConfig;

  Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, sizeof (PPM_GLOBAL_NVS_AREA), (VOID **) &PpmGlobalNvsAreaProtocol.Area);
  ASSERT_EFI_ERROR (Status);
  ZeroMem ((VOID *) PpmGlobalNvsAreaProtocol.Area, sizeof (PPM_GLOBAL_NVS_AREA));

  PpmGlobalNvsAreaProtocol.Area->Revision = PPM_GLOBAL_NVS_AREA_REVISION_1;
  PpmGlobalNvsAreaProtocol.Area->MiscPowerManagementFlags |= mCpuPmConfig->pFunctionEnables->LakeTiny;

  ///
  /// Install Cpu Power management GlobalNVS Area protocol
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPpmGlobalNvsAreaProtocolGuid,
                  &PpmGlobalNvsAreaProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DumpCpuPmConfig ();

  ///
  /// Locate the S3 resume scripting protocol
  ///

  ///
  /// Initialize Power management Global variables
  ///
  InitializePowerManagementGlobalVariables ();

  ///
  /// Initialize CPU Power management code (determine HW and configured state, configure hardware and software accordingly)
  ///
  Status = InitializePpm ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// Install the PowerMgmtInitDone Protocol so that PowerMgmtS3 driver can load to save the MSRs for S3 resume
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiPowerMgmtInitDoneProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);


  return EFI_SUCCESS;
}

/**
  Initializes the platform power management global variable.
  This must be called prior to any of the functions being used.

  @retval EFI_SUCCESS       Library was initialized successfully
  @retval EFI_DEVICE_ERROR  If CPU is not supported by this library
**/
VOID
InitializePowerManagementGlobalVariables (
  VOID
  )
{
  EFI_STATUS   Status;
  MSR_REGISTER TempMsr;
  MSR_REGISTER PackagePowerSKUUnitMsr;
  MSR_REGISTER PlatformInfoMsr;
  UINT16       Associativity;
  UINT16       CachePartitions;
  UINT16       CacheLineSize;
  UINT16       CacheNumberofSets;
  EFI_CPUID_REGISTER  Cpuid04;
  //
  // Read the CPUID information
  //
  AsmCpuid (CPUID_VERSION_INFO, &mCpuid01.RegEax, &mCpuid01.RegEbx, &mCpuid01.RegEcx, &mCpuid01.RegEdx);

  ///
  /// Locate Ppm GlobalNvs Protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gPpmGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &mPpmGlobalNvsAreaProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Ppm GloableNvs protocol not found"));
  }
  ASSERT_EFI_ERROR (Status);
  if (mPpmGlobalNvsAreaProtocol != NULL) {
    mPpmGlobalNvsAreaProtocol->Area->Cpuid = GetCpuFamily() | GetCpuStepping();
  }

  ///
  /// Locate MP service protocol
  ///
  Status = gBS->LocateProtocol (
                  &gFrameworkEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &mMpService
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get Platform ID by reading System Agent's Device ID (B0:D0:F0:R02)
  ///
  mProcessorFlavor = McD0PciCfg16 (R_SA_MC_DEVICE_ID);

  //
  // Read the CPUID4 information for LLC size
  //
  AsmCpuidEx (CPUID_FUNCTION_4, 0x3, &Cpuid04.RegEax, &Cpuid04.RegEbx, &Cpuid04.RegEcx, &Cpuid04.RegEdx);
  //
  // Determine Cache Size in Kilo Bytes
  // This Cache Size in Bytes = (Associativity) * (Partitions + 1) * (Line_Size + 1) * (Sets + 1)
  // = (EBX[31:22] + 1) * (EBX[21:12] + 1) * (EBX[11:0] + 1) * (ECX + 1)
  //
  Associativity     = (UINT16) (((Cpuid04.RegEbx >> 22) & CPU_CACHE_ASSOCIATIVITY_MASK) + 1);
  CachePartitions   = (UINT16) (((Cpuid04.RegEbx >> 12) & CPU_CACHE_PARTITION_MASK) + 1);
  CacheLineSize     = (UINT16) (((UINT16) Cpuid04.RegEbx & CPU_CACHE_LINE_SIZE_MASK) + 1);
  CacheNumberofSets = (UINT16) (Cpuid04.RegEcx + 1);
  mCpuCacheSize = (UINT16) ((Associativity * CachePartitions * CacheLineSize * CacheNumberofSets) / 1024);

  ///
  /// Get Maximum Non-Turbo bus ratio (HFM) from Platform Info MSR Bits[15:8]
  ///
  PlatformInfoMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  mMaxBusRatio          = PlatformInfoMsr.Bytes.SecondByte;
  ///
  /// Get Maximum Efficiency bus ratio (LFM) from Platform Info MSR Bits[47:40]
  ///
  mMinBusRatio = PlatformInfoMsr.Bytes.SixthByte;
  ///
  /// Get Max Turbo Ratio from Turbo Ratio Limit MSR Bits [7:0]
  ///
  TempMsr.Qword   = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
  mTurboBusRatio  = (UINT16) (TempMsr.Dwords.Low & B_MSR_TURBO_RATIO_LIMIT_1C);
  ///
  /// Check if Turbo Ratio Limit is programmable
  ///  Platform Info MSR (0xCE) [28]
  ///
  mRatioLimitProgrammble = (UINT8) RShiftU64 ((PlatformInfoMsr.Qword & B_PLATFORM_INFO_RATIO_LIMIT), 28);
  ///
  /// Check if TDP Limit is programmable
  ///  Platform Info MSR (0xCE) [29]
  ///
  mTdpLimitProgrammble = (UINT8) RShiftU64 ((PlatformInfoMsr.Qword & B_PLATFORM_INFO_TDC_TDP_LIMIT), 29);

  ///
  /// Get Processor TDP
  /// Get Maximum Power from Turbo Power Limit MSR Bits[14:0]
  /// and convert it to units specified by Package Power SKU
  /// Unit MSR [3:0]
  ///
  TempMsr.Qword                 = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);
  PackagePowerSKUUnitMsr.Qword  = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  mProcessorPowerUnit           = (PackagePowerSKUUnitMsr.Bytes.FirstByte & PACKAGE_POWER_UNIT_MASK);
  if (mProcessorPowerUnit == 0) {
    mProcessorPowerUnit = 1;
  } else {
    mProcessorPowerUnit = (UINT8) LShiftU64 (2, (mProcessorPowerUnit - 1));
  }
  mPackageTdp = (TempMsr.Dwords.Low & PACKAGE_TDP_POWER_MASK); 
  mPackageTdpWatt = (UINT16) DivU64x32 (mPackageTdp , mProcessorPowerUnit);
  mPackageMaxPower  = (UINT16) (TempMsr.Dwords.High & PACKAGE_MAX_POWER_MASK);
  mPackageMinPower  = (UINT16) RShiftU64 ((TempMsr.Dwords.Low & PACKAGE_MIN_POWER_MASK), 16);
  mCpuPolicyRevision = mCpuPlatformPolicy->Revision;

  ///
  /// Set mCustomPowerUnit to user selected Power unit
  ///
  mCustomPowerUnit = 1;
  if ((mCpuPlatformPolicy->Revision >= DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_6 ) 
      && (mCpuPmConfig->CustomPowerUnit == PowerUnit125MilliWatts)) {
        mCustomPowerUnit = 8; ///< Unit is 125 milli watt 
  }

  ///
  /// If specified, create a custom the FVID table.
  /// (The settings populating the FVID table may not be correct for the
  /// specific processor, and it is up to the user to specify settings
  /// applicable to the processor being used.)
  ///
  ZeroMem (mFvidPointer, sizeof (mEmptyFvidTable));
  if (mCpuPmConfig->pCustomRatioTable->NumberOfEntries >= 2) {
    CreateCustomFvidTable (mFvidPointer);
  }

  ///
  /// Initialize flags based on processor capablities
  ///
  SetPpmFlags ();

  ///
  /// Determine current user configuration
  ///
  SetUserConfigurationPpmFlags ();

  ///
  /// Locate ACPI support protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **) &mAcpiSupport);
  ASSERT_EFI_ERROR (Status);
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &mAcpiTable);
  ASSERT_EFI_ERROR (Status);
  return;
}

/**
  Create a custom FVID table based on setup options.
  Caller is responsible for providing a large enough table.

  @param[in] FvidPointer   Table to update, must be initialized.
**/
VOID
CreateCustomFvidTable (
  IN OUT FVID_TABLE *FvidPointer
  )
{
  UINT16 Index;

  ///
  /// BWG Section 14.6.2 Determining Number of Operating Points
  /// It is recommended that system BIOS limit the number of P states to 16
  ///
  if (mCpuPmConfig->pCustomRatioTable->NumberOfEntries > FVID_MAX_STATES) {
    DEBUG (
            (EFI_D_WARN,
             "VidNumber(%d) is greater than maximum(%d) supported.",
             mCpuPmConfig->pCustomRatioTable->NumberOfEntries,
             FVID_MAX_STATES)
            );
    mCpuPmConfig->pCustomRatioTable->NumberOfEntries = FVID_MAX_STATES;
  }
  ///
  /// Fill in the table header
  ///
  FvidPointer[0].FvidHeader.Stepping    = mCpuPmConfig->pCustomRatioTable->Cpuid;
  FvidPointer[0].FvidHeader.MaxBusRatio = mCpuPmConfig->pCustomRatioTable->MaxRatio;
  FvidPointer[0].FvidHeader.Gv3States   = mCpuPmConfig->pCustomRatioTable->NumberOfEntries;
  ///
  /// Fill in the state data
  ///
  for (Index = 0; Index < mCpuPmConfig->pCustomRatioTable->NumberOfEntries; Index++) {
    FvidPointer[Index + 1].FvidState.State    = Index;
    FvidPointer[Index + 1].FvidState.BusRatio = mCpuPmConfig->pCustomRatioTable->StateRatio[Index];
  }
}

/**
  Set the PPM flags specific to Haswell processors

  @retval EFI_SUCCESS  PpmFlags updated with the features supported by the processor
**/
VOID
SetPpmFlags (
  VOID
  )
{
  MSR_REGISTER       CoreThreadCount;
  MSR_REGISTER       Ia32MiscEnable;
  EFI_CPUID_REGISTER Cpuid01;
  EFI_CPUID_REGISTER Cpuid05;
  EFI_CPUID_REGISTER Cpuid06;
  UINTN              States;
  BOOLEAN            CpuidLimitingEnabled;
  UINT32             PpmFlags;

  ZeroMem (&Cpuid01, sizeof (Cpuid01));
  ZeroMem (&Cpuid05, sizeof (Cpuid05));
  ZeroMem (&Cpuid06, sizeof (Cpuid06));
  PpmFlags = 0;

  ///
  /// Check if the processor has multiple cores
  ///
  CoreThreadCount.Qword = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  if ((CoreThreadCount.Dwords.Low & B_THREAD_COUNT_MASK) > 1) {
    PpmFlags |= PPM_CMP;
  }
  if (mCpuid01.RegEdx & B_CPUID_VERSION_INFO_EDX_TM1) { ///< Check TM capable and update the flag
    PpmFlags |= PPM_TM;
  }
  ///
  /// Check EIST capable. If EIST capable, also set the boot P-state to HFM flag.
  ///
  if (mCpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_EIST) {
    PpmFlags |= (PPM_EIST);
    DEBUG ((EFI_D_INFO, "GV3 capable\n"));
  }
  ///
  /// Disable CPUID limiting (and save current setting) if enabled
  /// and enable MONITOR/MWAIT support
  ///
  Ia32MiscEnable.Qword  = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  CpuidLimitingEnabled  = (BOOLEAN) (Ia32MiscEnable.Qword & B_MSR_IA32_MISC_ENABLE_CPUID_MAX);
  if (CpuidLimitingEnabled) {
    Ia32MiscEnable.Qword &= ~B_MSR_IA32_MISC_ENABLE_CPUID_MAX;
  }
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  ///
  /// Read the CPUID values we care about.  We cannot use the stored
  /// values because they may have changes since we disabled limiting
  /// and enabled MONITOR/MWAIT
  ///
  AsmCpuid (1, &Cpuid01.RegEax, &Cpuid01.RegEbx, &Cpuid01.RegEcx, &Cpuid01.RegEdx);
  AsmCpuid (5, &Cpuid05.RegEax, &Cpuid05.RegEbx, &Cpuid05.RegEcx, &Cpuid05.RegEdx);
  AsmCpuid (6, &Cpuid06.RegEax, &Cpuid06.RegEbx, &Cpuid06.RegEcx, &Cpuid06.RegEdx);
  ///
  /// Determine if the MONITOR/MWAIT instructions are supported.
  ///
  if ((Cpuid01.RegEcx & B_CPUID_VERSION_INFO_ECX_MWAIT && Cpuid05.RegEcx & B_CPUID_MONITOR_MWAIT_ECX_EXTENSIONS)) {
    PpmFlags |= PPM_MWAIT_EXT;
  }
  ///
  /// Determine the C-State and Enhanced C-State support present.
  /// Monitor/MWAIT parameters function describes the numbers supported.
  ///
  States = RShiftU64 (Cpuid05.RegEdx, 4) & 0xF;
  if (States >= ENHANCED_CSTATE_SUPPORTED) {
    PpmFlags |= PPM_C1 + PPM_C1E;
  } else if (States == CSTATE_SUPPORTED) {
    PpmFlags |= PPM_C1;
  }
  States = RShiftU64 (Cpuid05.RegEdx, 8) & 0xF;
  if ((States >= CSTATE_SUPPORTED) && (PpmFlags & PPM_C1)) {
    PpmFlags |= PPM_C3;
  }
  States = RShiftU64 (Cpuid05.RegEdx, 12) & 0xF;
  if (States >= C6_C7_LONG_LATENCY_SUPPORTED) { // Both Long and Short Latency C6 supported
    PpmFlags |= (PPM_C6 | C6_LONG_LATENCY_ENABLE);
  } else if (States >= C6_C7_SHORT_LATENCY_SUPPORTED) { // Only Short Latency C6 supported.
    PpmFlags |= PPM_C6;
  }

  States = RShiftU64 (Cpuid05.RegEdx, 16) & 0xF;
  switch (States) {
    case C7s_LONG_LATENCY_SUPPORTED:
      //
      // C7 & C7s Long and Short supported
      //
      PpmFlags |= (PPM_C7S | C7s_LONG_LATENCY_ENABLE | PPM_C7 | C7_LONG_LATENCY_ENABLE);
      break;
    case C7s_SHORT_LATENCY_SUPPORTED:
      //
      // C7s Long Latency is not supported.
      //
      PpmFlags |= (PPM_C7S | PPM_C7 | C7_LONG_LATENCY_ENABLE);
      break;
    case C6_C7_LONG_LATENCY_SUPPORTED:
      //
      // C7 Long and Short supported
      //
      PpmFlags |= (PPM_C7 | C7_LONG_LATENCY_ENABLE);
      break;
    case C6_C7_SHORT_LATENCY_SUPPORTED:
      //
      // C7 Long Latency is not supported.
      //
      PpmFlags |= PPM_C7;
      break;
  }

  States = RShiftU64 (Cpuid05.RegEdx, 20) & 0xF;
  if (States >= CSTATE_SUPPORTED) {
    PpmFlags |= PPM_C8;
  }
  States = RShiftU64 (Cpuid05.RegEdx, 24) & 0xF;
  if (States >= CSTATE_SUPPORTED) {
    PpmFlags |= PPM_C9;
  }
  States = RShiftU64 (Cpuid05.RegEdx, 28) & 0xF;
  if (States >= CSTATE_SUPPORTED) {
    PpmFlags |= PPM_C10;
  }
  ///
  /// Check TimedMwait is supported and update the flag
  ///
  if (AsmReadMsr64 (MSR_PLATFORM_INFO) & B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED) {
    PpmFlags |= PPM_TIMED_MWAIT;
  }
  if (PpmFlags & (PPM_C8 |PPM_C9 | PPM_C10)) {
    PpmFlags |= PPM_CD;
  }
  ///
  /// Check if turbo mode is supported and update the flag
  ///
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  if (((Cpuid06.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_TURBO) == 0) &&
      ((Ia32MiscEnable.Qword & B_MSR_IA32_MISC_DISABLE_TURBO) == 0)
      ) {
    ///
    /// Turbo Mode is not available in this physical processor package.
    /// BIOS should not attempt to enable Turbo Mode via IA32_MISC_ENABLE MSR.
    /// BIOS should show Turbo Mode as Disabled and Not Configurable.
    ///
  } else if ((Cpuid06.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_TURBO) == 0) {
    ///
    /// Turbo Mode is available but globally disabled for the all logical
    /// processors in this processor package.
    /// BIOS can enable Turbo Mode by IA32_MISC_ENABLE MSR 1A0h bit [38] = 0.
    ///
    PpmFlags |= PPM_TURBO;
  } else if ((Cpuid06.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_TURBO) == B_CPUID_POWER_MANAGEMENT_EAX_TURBO) {
    ///
    /// Turbo Mode is factory-configured as available and enabled for all logical processors in this processor package.
    /// This case handles the cases where turbo mode is enabled before PPM gets chance to enable it
    ///
    PpmFlags |= PPM_TURBO;
  }
  ///
  /// Restore the CPUID limit setting.
  ///
  if (CpuidLimitingEnabled) {
    Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
    Ia32MiscEnable.Qword |= B_MSR_IA32_MISC_ENABLE_CPUID_MAX;
    AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  }
  PpmFlags |= PPM_TSTATES; ///< Set the T-states flag
  ///
  /// Determine if Fine grained clock modulation contol is supported
  ///
  if (Cpuid06.RegEax & B_CPUID_POWER_MANAGEMENT_EAX_FINE_GRAINED_CLOCK_MODULATION) {
    PpmFlags |= PPM_TSTATE_FINE_GRAINED;
  }
  PpmFlags |= PPM_EEPST;                                ///< Energy Efficient P-state feature is supported
  mPpmGlobalNvsAreaProtocol->Area->PpmFlags = PpmFlags; ///< Update the PPM NVS area PPM flags

  return;
}

/**
  Set the PPM flags based on current user configuration
**/
VOID
SetUserConfigurationPpmFlags (
  VOID
  )
{
  UINT32 UserPpmFlag;
  //
  // In advance to clear following PPM flags which are related with policies that user can enabled/disabled.
  //
  UserPpmFlag = (UINT32)~(PPM_EIST | PPM_C1 | PPM_C1E | PPM_C3 | PPM_C6 | C6_LONG_LATENCY_ENABLE |
                 PPM_C7S | PPM_C7 | C7_LONG_LATENCY_ENABLE | C7s_LONG_LATENCY_ENABLE | PPM_CD |
                 PPM_C8 | PPM_C9 | PPM_C10 | PPM_TM | PPM_TURBO | PPM_TSTATES | PPM_TSTATE_FINE_GRAINED |
                 PPM_EEPST | PPM_TIMED_MWAIT);
  ///
  /// Configure flag based on user selections
  ///
  if (mCpuPmConfig->pFunctionEnables->Eist) {
    UserPpmFlag |= PPM_EIST;
  }
  if (mCpuPmConfig->pFunctionEnables->Cx) {
    UserPpmFlag |= PPM_C1;
    if (mCpuPmConfig->pFunctionEnables->C1e) {
      UserPpmFlag |= PPM_C1E;
    }
    if (mCpuPmConfig->pFunctionEnables->C3) {
      UserPpmFlag |= PPM_C3;
    }
    if (mCpuPmConfig->pFunctionEnables->C6) {
      UserPpmFlag |= PPM_C6;
    }
    if (mCpuPmConfig->pFunctionEnables->LongLatencyC6) {
      UserPpmFlag |= C6_LONG_LATENCY_ENABLE;
    }
    switch (mCpuPmConfig->pFunctionEnables->DeepCState) {
      case DeepC7S:
        UserPpmFlag |= PPM_C7S;
      case DeepC7:
        UserPpmFlag |= PPM_C7;
        break;
    }
    if (mCpuPmConfig->pFunctionEnables->LongLatencyC7) {
      UserPpmFlag |= (C7_LONG_LATENCY_ENABLE | C7s_LONG_LATENCY_ENABLE);
    }
    if (mCpuPmConfig->pFunctionEnables->C8) {
      UserPpmFlag |= PPM_C8;
    }
    if (mCpuPmConfig->pFunctionEnables->C9) {
      UserPpmFlag |= PPM_C9;
    }
    if (mCpuPmConfig->pFunctionEnables->C10) {
      UserPpmFlag |= PPM_C10;
    }
    if (UserPpmFlag & (PPM_C8 |PPM_C9 | PPM_C10)) {
       UserPpmFlag |= PPM_CD;
    }
  }
  if (mCpuPmConfig->ThermalFuncEnables->ThermalMonitor) {
    UserPpmFlag |= PPM_TM;
  }
  if (mCpuPmConfig->pFunctionEnables->TurboMode) {
    UserPpmFlag |= PPM_TURBO;
  }
  if (mCpuPmConfig->ThermalFuncEnables->TStates) {
    UserPpmFlag |= (PPM_TSTATES | PPM_TSTATE_FINE_GRAINED);
  }
  if (mCpuPmConfig->pFunctionEnables->EnergyEfficientPState) {
    UserPpmFlag |= PPM_EEPST;
  }
  if (mCpuPmConfig->pFunctionEnables->TimedMwait) {
    UserPpmFlag |= PPM_TIMED_MWAIT;
  }
  ///
  /// Modify PpmFlags based on user selections
  ///
  mPpmGlobalNvsAreaProtocol->Area->PpmFlags &= UserPpmFlag;
}

/**
  Initialize the processor power management based on hardware capabilities
  and user configuration settings.

  @retval EFI_SUCCESS - on success
  @retval Appropiate failure code on error
**/
EFI_STATUS
InitializePpm (
  VOID
  )
{
  EFI_STATUS Status;
  EFI_STATUS CtdpSupport;

  Status      = EFI_SUCCESS;
  CtdpSupport = EFI_UNSUPPORTED;

  ///
  /// Initialize Config TDP
  ///
  CtdpSupport = InitializeConfigurableTdp (mCpuPmConfig);

  ///
  /// Initialize P states
  ///
  InitializePStates();

  ///
  /// Initialize C State(IdleStates)
  ///
  InitializeCState(mCpuPmConfig);

  //
  // Patch P state table (Fvid table) with ctdp settings.
  //
  CtdpPatchFvidTable (mFvidPointer);

  Status = InitializePpmAcpiTable ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Initialize thermal features
  ///
  InitThermal (mCpuPmConfig);

  ///
  /// Initialise Miscellaneous features
  ///
  InitMiscFeatures(CtdpSupport);

  ///
  /// Complete with Ppmpost initialization
  ///
  PpmPostInit ();

  return Status;
}


/**
  This is a debug function to print PPM Policy
**/
VOID
DumpCpuPmConfig (
  VOID
  )
{
#ifdef EFI_DEBUG
  UINT32 Index;
  DEBUG ((EFI_D_INFO, "\n\n------------------------ PowerMangement Policy dump Begin -----------------\n\n"));
  DEBUG ((EFI_D_INFO, " S3RestoreMsrSwSmiNumber : %x\n", mCpuPmConfig->S3RestoreMsrSwSmiNumber));
  DEBUG ((EFI_D_INFO, "\n Ppm Lock Enables... \n"));
  DEBUG ((EFI_D_INFO, " PmgCstCfgCtrlLock : %x\n", mCpuPmConfig->pPpmLockEnables->PmgCstCfgCtrlLock));
  DEBUG ((EFI_D_INFO, " ProcHotLock : %x\n", mCpuPmConfig->pPpmLockEnables->ProcHotLock));
  DEBUG ((EFI_D_INFO, "\n FunctionEnables... \n"));
  DEBUG ((EFI_D_INFO, " Eist : %x\n", mCpuPmConfig->pFunctionEnables->Eist));
  DEBUG ((EFI_D_INFO, " Cx : %x\n", mCpuPmConfig->pFunctionEnables->Cx));
  DEBUG ((EFI_D_INFO, " C1e : %x\n", mCpuPmConfig->pFunctionEnables->C1e));
  DEBUG ((EFI_D_INFO, " C3 : %x\n", mCpuPmConfig->pFunctionEnables->C3));
  DEBUG ((EFI_D_INFO, " C6 : %x\n", mCpuPmConfig->pFunctionEnables->C6));
  DEBUG ((EFI_D_INFO, " C6 Long Latency: %x\n", mCpuPmConfig->pFunctionEnables->LongLatencyC6));
  DEBUG ((EFI_D_INFO, " DeepCState : %x\n", mCpuPmConfig->pFunctionEnables->DeepCState));
  DEBUG ((EFI_D_INFO, " C7 Long Latency : %x\n", mCpuPmConfig->pFunctionEnables->LongLatencyC7));
  DEBUG ((EFI_D_INFO, " C1Autodemotion : %x\n", mCpuPmConfig->pFunctionEnables->C1AutoDemotion));
  DEBUG ((EFI_D_INFO, " C3AutoDemotion : %x\n", mCpuPmConfig->pFunctionEnables->C3AutoDemotion));
  DEBUG ((EFI_D_INFO, " C1Undemotion : %x\n", mCpuPmConfig->pFunctionEnables->C1UnDemotion));
  DEBUG ((EFI_D_INFO, " C3UnDemotion : %x\n", mCpuPmConfig->pFunctionEnables->C3UnDemotion));
  DEBUG ((EFI_D_INFO, " PkgCstateUndemotion : %x\n", mCpuPmConfig->pFunctionEnables->PkgCStateUnDemotion));
  DEBUG ((EFI_D_INFO, " PkgCState Demotion : %x\n", mCpuPmConfig->pFunctionEnables->PkgCStateDemotion));
  DEBUG ((EFI_D_INFO, " CStatePreWake : %x\n", mCpuPmConfig->pFunctionEnables->CStatePreWake));
  DEBUG ((EFI_D_INFO, " TurboMode : %x\n", mCpuPmConfig->pFunctionEnables->TurboMode));
  DEBUG ((EFI_D_INFO, " PowerLimit2 : %x\n", mCpuPmConfig->pFunctionEnables->PowerLimit2));
  DEBUG ((EFI_D_INFO, " EnergyEfficientPState : %x\n", mCpuPmConfig->pFunctionEnables->EnergyEfficientPState));
  DEBUG ((EFI_D_INFO, " LakeTiny Support : %x\n", mCpuPmConfig->pFunctionEnables->LakeTiny));
  DEBUG ((EFI_D_INFO, " PkgCStateLimit : %x\n", mCpuPmConfig->PkgCStateLimit));
  DEBUG ((EFI_D_INFO, " TimedMwait : %x\n", mCpuPmConfig->pFunctionEnables->TimedMwait));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl0TimeUnit : %x\n", mCpuPmConfig->CstateLatencyControl0TimeUnit));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl1TimeUnit : %x\n", mCpuPmConfig->CstateLatencyControl1TimeUnit));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl2TimeUnit : %x\n", mCpuPmConfig->CstateLatencyControl2TimeUnit));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl3TimeUnit : %x\n", mCpuPmConfig->CstateLatencyControl3TimeUnit));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl4TimeUnit : %x\n", mCpuPmConfig->CstateLatencyControl4TimeUnit));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl5TimeUnit : %x\n", mCpuPmConfig->CstateLatencyControl5TimeUnit));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl0Irtl : %x\n", mCpuPmConfig->CstateLatencyControl0Irtl));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl1Irtl : %x\n", mCpuPmConfig->CstateLatencyControl1Irtl));
  DEBUG ((EFI_D_INFO, " CstateLatencyControl2Irtl : %x\n", mCpuPmConfig->CstateLatencyControl2Irtl));
  DEBUG ((EFI_D_INFO, " RfiFreqTunningOffsetIsNegative : %x\n", mCpuPmConfig->RfiFreqTunningOffsetIsNegative));
  DEBUG ((EFI_D_INFO, " RfiFreqTunningOffset : %x\n", mCpuPmConfig->RfiFreqTunningOffset));
  DEBUG ((EFI_D_INFO, "\n Turbo settings... \n"));
  DEBUG ((EFI_D_INFO, " PowerLimit1 : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit1));
  DEBUG ((EFI_D_INFO, " PowerLimit1Time : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit1Time));
  DEBUG ((EFI_D_INFO, " PowerLimit2 : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit2));
  DEBUG ((EFI_D_INFO, " PowerLimit3 : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit3));
  DEBUG ((EFI_D_INFO, " PowerLimit3Time : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit3Time));
  DEBUG ((EFI_D_INFO, " PowerLimit3DutyCycle : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit3DutyCycle));
  DEBUG ((EFI_D_INFO, " PowerLimit3Lock : %x\n", mCpuPmConfig->pTurboSettings->PowerLimit3Lock));
  DEBUG ((EFI_D_INFO, " TurboPowerLimitLock : %x\n", mCpuPmConfig->pTurboSettings->TurboPowerLimitLock));
  DEBUG ((EFI_D_INFO, " ConfigTdpLevel : %x\n", mCpuPmConfig->pTurboSettings->ConfigTdpLevel));
  DEBUG ((EFI_D_INFO, " ConfigTdpLock : %x\n", mCpuPmConfig->pTurboSettings->ConfigTdpLock));
  DEBUG ((EFI_D_INFO, " ConfigTdpCustom : %x\n", mCpuPmConfig->pCustomCtdpSettings->ConfigTdpCustom));
  if (mCpuPmConfig->pCustomCtdpSettings->ConfigTdpCustom) {
    DEBUG ((EFI_D_INFO, " CustomTdpCount : %d\n", mCpuPmConfig->pCustomCtdpSettings->CustomTdpCount));
    DEBUG ((EFI_D_INFO, " CustomBootModeIndex : %d\n", mCpuPmConfig->pCustomCtdpSettings->CustomBootModeIndex));
    for (Index = 0; Index < MAX_CUSTOM_CTDP_ENTRIES; Index++) {
      DEBUG (
              (EFI_D_INFO,
               " CustomConfigTdpTable[%d] CustomPowerLimit1 : 0x%x\n",
               Index,mCpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit1)
              );
      DEBUG (
              (EFI_D_INFO,
               " CustomConfigTdpTable[%d] CustomPowerLimit2 : 0x%x\n",
               Index,mCpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit2)
              );
      DEBUG (
              (EFI_D_INFO,
               " CustomConfigTdpTable[%d] CustomPowerLimit1Time : %d\n",
               Index,mCpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomPowerLimit1Time)
              );
      DEBUG (
              (EFI_D_INFO,
               " CustomConfigTdpTable[%d] CustomTurboActivationRatio : %d\n",
               Index,mCpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomTurboActivationRatio)
              );
      DEBUG (
              (EFI_D_INFO,
               " ConfigTdpTable[%d] CustomConfigTdpControl : %d\n",
               Index,mCpuPmConfig->pCustomCtdpSettings->CustomConfigTdpTable[Index].CustomConfigTdpControl)
              );
    }
  }
  DEBUG ((EFI_D_INFO, "\n CustomRatioTable... \n"));
  DEBUG ((EFI_D_INFO, " VidNumber : %x\n", mCpuPmConfig->pCustomRatioTable->NumberOfEntries));
  DEBUG ((EFI_D_INFO, " VidCpuid : %x\n", mCpuPmConfig->pCustomRatioTable->Cpuid));
  DEBUG ((EFI_D_INFO, " VidMaxRatio : %x\n", mCpuPmConfig->pCustomRatioTable->MaxRatio));
  for (Index = 0; Index < MAX_CUSTOM_RATIO_TABLE_ENTRIES; Index++) {
    DEBUG ((EFI_D_INFO, " StateRatio[%d] : %x\n", Index, mCpuPmConfig->pCustomRatioTable->StateRatio[Index]));
  }
  DEBUG ((EFI_D_INFO, "\n XeConfig... \n"));
  DEBUG ((EFI_D_INFO, " Xe : %x\n", mCpuPmConfig->pFunctionEnables->Xe));
  for (Index = 0; Index < 4; Index++) {
    DEBUG ((EFI_D_INFO, " RatioLimit[%d] : %x\n", Index, mCpuPmConfig->pRatioLimit[Index]));
  }
  DEBUG ((EFI_D_INFO, " BiProcHot : %x\n", mCpuPmConfig->ThermalFuncEnables->BiProcHot));
  DEBUG ((EFI_D_INFO, " DisableProcHotOut : %x\n", mCpuPmConfig->ThermalFuncEnables->DisableProcHotOut));
  DEBUG ((EFI_D_INFO, " DisableVRThermalAlert : %x\n", mCpuPmConfig->ThermalFuncEnables->DisableVRThermalAlert));
  DEBUG ((EFI_D_INFO, " ProcHotResponce : %x\n", mCpuPmConfig->ThermalFuncEnables->ProcHotResponce));
  DEBUG ((EFI_D_INFO, " TStates : %x\n", mCpuPmConfig->ThermalFuncEnables->TStates));
  DEBUG ((EFI_D_INFO, " AutoThermalReporting : %x\n", mCpuPmConfig->ThermalFuncEnables->AutoThermalReporting));
  DEBUG ((EFI_D_INFO, " ThermalMonitor : %x\n", mCpuPmConfig->ThermalFuncEnables->ThermalMonitor));
  DEBUG ((EFI_D_INFO, "\n\n------------------------ PowerMangement Policy dump End -------------------\n\n"));
#endif
}
