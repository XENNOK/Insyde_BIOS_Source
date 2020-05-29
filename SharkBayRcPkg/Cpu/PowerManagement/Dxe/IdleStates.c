/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  IdleStates.c

@brief
  This file contains power management C State configuration functions for
  Haswell processors.

  Acronyms:
    PPM - Processor Power Management
    TM  - Thermal Monitor
    IST - Intel(R) Speedstep technology
    HT  - Hyper-Threading Technology
**/
#include "PowerMgmtCommon.h"

/**
  Initializes C States Power management features

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitializeCState (
  IN POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  EFI_STATUS Status;
  UINT16     mAcpiBaseAddr;
  ///
  /// Get the ACPI Base Address
  ///
  mAcpiBaseAddr = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;

  ///
  /// Initialize C states, some are general, some are processor specific.
  /// Dynamic loading of CST SSDT tables occurs at PpmPostInit.
  ///
  EnableCStates (mAcpiBaseAddr + PM_CST_LVL2);
  ///
  /// Calibrate C State 24MHz BCLK
  ///
//@todo: Need to finalize on whether or not re-calibration option should be provided
  Status = CalibrateBclkForCStates ();
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_INFO, "24MHz BCLK calibration Failed \n"));
//@todo: Need to finalize on how to handle failure of 24 MHz calibration
  }

  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES) {
    ///
    /// Update Fadt table for C State support.
    ///
    ConfigureFadtCStates ();
  }
  InitCstatePreWake (mCpuPmConfig);

}

/**
  Disable/Enable the CState Pre-Wake Feature

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitCstatePreWake (
  IN POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER TempMsr;

  TempMsr.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  TempMsr.Dwords.Low &= ~(B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE);
  if (CpuPmConfig->pFunctionEnables->CStatePreWake == PPM_DISABLE) {
    TempMsr.Dwords.Low |= B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE;
  }
  AsmWriteMsr64 (MSR_POWER_CTL, TempMsr.Qword);

  return;
}

/**
  Enables C-State support as specified by the input flags on all logical
  processors and sets associated timing requirements in the chipset.

  @param[in] C3IoAddress  IO address to generate C3 states (PM base + 014 usually)
**/
VOID
EnableCStates (
  IN UINT16 C3IoAddress
  )
{
  MSR_REGISTER PowerCtl;
  MSR_REGISTER TempMsr;
  UINT32       LCR0Latency;
  UINT32       LCR1Latency;
  UINT32       LCR2Latency;
  UINT32       LCR3Latency;
  UINT32       LCR4Latency;
  UINT32       LCR5Latency;
  UINT16       EnableCStateParameters;
  CPU_FAMILY   mCpuFamilyId;

  mCpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  ///
  /// Load the C-State parameters to pass to the core function.
  ///
  EnableCStateParameters = C3IoAddress;
  ///
  /// Enable C-States on all logical processors.
  ///
  RunOnAllLogicalProcessors (ApSafeEnableCStates, &EnableCStateParameters);
  ///
  /// If C-states are disabled or not supported, Disable C1e  and retrun
  ///
  if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES) == 0) {
    PowerCtl.Qword = AsmReadMsr64 (MSR_POWER_CTL);
    PowerCtl.Dwords.Low &= ~B_MSR_POWER_CTL_C1E;
    AsmWriteMsr64 (MSR_POWER_CTL, PowerCtl.Qword);
    DEBUG (
            (EFI_D_INFO,
             "Setup C state disabled.Disable C1e. MSR(1FC) : %X %X\n",
             PowerCtl.Dwords.High,
             PowerCtl.Dwords.Low)
            );
    return;
  }
  ///
  /// Configure supported enhanced C-states
  ///
  /// Read Power Ctl MSR
  ///
  PowerCtl.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  DEBUG ((EFI_D_INFO, "MSR(1FC) before configuring C1E: %X %X\n", PowerCtl.Dwords.High, PowerCtl.Dwords.Low));
  ///
  /// Enable supported states
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C1E) {
    PowerCtl.Dwords.Low |= B_MSR_POWER_CTL_C1E;
  } else {
    PowerCtl.Dwords.Low &= ~B_MSR_POWER_CTL_C1E;
  }
  ///
  /// Update Power Control MSR
  ///
  AsmWriteMsr64 (MSR_POWER_CTL, PowerCtl.Qword);
  DEBUG ((EFI_D_INFO, "MSR(1FC) after configuring C1E: %X %X\n", PowerCtl.Dwords.High, PowerCtl.Dwords.Low));
  ///
  /// Program Interrupt response time limits used by processor to decided when to get into
  /// package C3, C6 and C7
  ///
  DEBUG ((EFI_D_INFO, "Programming the 0xC3/C6/C7 (MSR 0x60A, 0x60B ,0X60C Latencies \n"));
  //
  // Package C3 Interrupt response time
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_C_STATE_LATENCY_CONTROL_0);
  DEBUG ((EFI_D_INFO, "MSR(60A) before configuring Latency: %X %X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
  TempMsr.Dwords.Low &= ~(B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK | B_TIME_UNIT_MASK | B_PKG_IRTL_VALID);
  ///
  /// Program Interrupt Response Time Unit and Latency for MSR 0x60A
  ///
  TempMsr.Dwords.Low |= mCpuPmConfig->CstateLatencyControl0Irtl;
  TempMsr.Dwords.Low |= LShiftU64 (mCpuPmConfig->CstateLatencyControl0TimeUnit, N_TIME_UNIT_OFFSET);
  TempMsr.Dwords.Low |= B_PKG_IRTL_VALID;
  AsmWriteMsr64 (MSR_C_STATE_LATENCY_CONTROL_0, TempMsr.Qword);
  //
  // Package C6/C7 short Interrupt response time
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_C_STATE_LATENCY_CONTROL_1);
  DEBUG ((EFI_D_INFO, "MSR(60B) before configuring Latency: %X %X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
  TempMsr.Dwords.Low &= ~(B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK | B_TIME_UNIT_MASK | B_PKG_IRTL_VALID);
  ///
  /// Program Interrupt Response Time Unit and Latency for MSR 0x60B
  ///
  TempMsr.Dwords.Low |= mCpuPmConfig->CstateLatencyControl1Irtl;
  TempMsr.Dwords.Low |= LShiftU64 (mCpuPmConfig->CstateLatencyControl1TimeUnit, N_TIME_UNIT_OFFSET);
  TempMsr.Dwords.Low |= B_PKG_IRTL_VALID;
  AsmWriteMsr64 (MSR_C_STATE_LATENCY_CONTROL_1, TempMsr.Qword);
  //
  // Package C6/C7 long Interrupt response time
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_C_STATE_LATENCY_CONTROL_2);
  DEBUG ((EFI_D_INFO, "MSR(60C) before configuring Latency: %X %X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
  TempMsr.Dwords.Low &= ~(B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK | B_TIME_UNIT_MASK | B_PKG_IRTL_VALID);
  ///
  /// Program Interrupt Response Time Unit and Latency for MSR 0x60C
  ///
  TempMsr.Dwords.Low |= mCpuPmConfig->CstateLatencyControl2Irtl;
  TempMsr.Dwords.Low |= LShiftU64 (mCpuPmConfig->CstateLatencyControl2TimeUnit, N_TIME_UNIT_OFFSET);
  TempMsr.Dwords.Low |= B_PKG_IRTL_VALID;
  AsmWriteMsr64 (MSR_C_STATE_LATENCY_CONTROL_2, TempMsr.Qword);
  if (mCpuFamilyId == EnumCpuHswUlt) {
    //
    // Package C8 Interrupt response time
    //
    TempMsr.Qword = AsmReadMsr64 (MSR_C_STATE_LATENCY_CONTROL_3);
    DEBUG ((EFI_D_INFO, "MSR(633) before configuring Latency: %X %X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
    TempMsr.Dwords.Low &= ~(B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK | B_TIME_UNIT_MASK | B_PKG_IRTL_VALID);
    ///
    /// Program Interrupt Response Time Unit and Latency for MSR 0x633
    ///
    TempMsr.Dwords.Low |= mCpuPmConfig->CstateLatencyControl3Irtl;
    TempMsr.Dwords.Low |= LShiftU64 (mCpuPmConfig->CstateLatencyControl3TimeUnit, N_TIME_UNIT_OFFSET);
    TempMsr.Dwords.Low |= B_PKG_IRTL_VALID;
    AsmWriteMsr64 (MSR_C_STATE_LATENCY_CONTROL_3, TempMsr.Qword);
    //
    // Package C9 Interrupt response time
    //
    TempMsr.Qword = AsmReadMsr64 (MSR_C_STATE_LATENCY_CONTROL_4);
    DEBUG ((EFI_D_INFO, "MSR(634) before configuring Latency: %X %X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
    TempMsr.Dwords.Low &= ~(B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK | B_TIME_UNIT_MASK | B_PKG_IRTL_VALID);
    ///
    /// Program Interrupt Response Time Unit and Latency for MSR 0x634
    ///
    TempMsr.Dwords.Low |= mCpuPmConfig->CstateLatencyControl4Irtl;
    TempMsr.Dwords.Low |= LShiftU64 (mCpuPmConfig->CstateLatencyControl4TimeUnit, N_TIME_UNIT_OFFSET);
    TempMsr.Dwords.Low |= B_PKG_IRTL_VALID;
    AsmWriteMsr64 (MSR_C_STATE_LATENCY_CONTROL_4, TempMsr.Qword);
    //
    // Package C10 Interrupt response time
    //
    TempMsr.Qword = AsmReadMsr64 (MSR_C_STATE_LATENCY_CONTROL_5);
    DEBUG ((EFI_D_INFO, "MSR(635) before configuring Latency: %X %X\n", TempMsr.Dwords.High, TempMsr.Dwords.Low));
    TempMsr.Dwords.Low &= ~(B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK | B_TIME_UNIT_MASK | B_PKG_IRTL_VALID);
    ///
    /// Program Interrupt Response Time Unit and Latency for MSR 0x635
    ///
    TempMsr.Dwords.Low |= mCpuPmConfig->CstateLatencyControl5Irtl;
    TempMsr.Dwords.Low |= LShiftU64 (mCpuPmConfig->CstateLatencyControl5TimeUnit, N_TIME_UNIT_OFFSET);
    TempMsr.Dwords.Low |= B_PKG_IRTL_VALID;
    AsmWriteMsr64 (MSR_C_STATE_LATENCY_CONTROL_5, TempMsr.Qword);
  }
  ///
  /// Update the PPM Global NVS Area
  ///
  LCR0Latency = (1 << (mCpuPmConfig->CstateLatencyControl0TimeUnit * 5));
  LCR0Latency = (LCR0Latency * mCpuPmConfig->CstateLatencyControl0Irtl) / 1000;
  //
  // _CST Latency: WordConst, so limit the latency value to max 0xFFFF
  //
  if (LCR0Latency > 0xFFFF) {
    LCR0Latency = 0xFFFF;
  }
  LCR1Latency = (1 << (mCpuPmConfig->CstateLatencyControl1TimeUnit * 5));
  LCR1Latency = (LCR1Latency * mCpuPmConfig->CstateLatencyControl1Irtl) / 1000;
  if (LCR1Latency > 0xFFFF) {
    LCR1Latency = 0xFFFF;
  }
  LCR2Latency = (1 << (mCpuPmConfig->CstateLatencyControl2TimeUnit * 5));
  LCR2Latency = (LCR2Latency * mCpuPmConfig->CstateLatencyControl2Irtl) / 1000;
  if (LCR2Latency > 0xFFFF) {
    LCR2Latency = 0xFFFF;
  }

  LCR3Latency = (1 << (mCpuPmConfig->CstateLatencyControl3TimeUnit * 5));
  LCR3Latency = (LCR3Latency * mCpuPmConfig->CstateLatencyControl3Irtl) / 1000;
  if (LCR3Latency > 0xFFFF) {
    LCR3Latency = 0xFFFF;
  }

  LCR4Latency = (1 << (mCpuPmConfig->CstateLatencyControl4TimeUnit * 5));
  LCR4Latency = (LCR4Latency * mCpuPmConfig->CstateLatencyControl4Irtl) / 1000;
  if (LCR4Latency > 0xFFFF) {
    LCR4Latency = 0xFFFF;
  }

  LCR5Latency = (1 << (mCpuPmConfig->CstateLatencyControl5TimeUnit * 5));
  LCR5Latency = (LCR5Latency * mCpuPmConfig->CstateLatencyControl5Irtl) / 1000;
  if (LCR5Latency > 0xFFFF) {
    LCR5Latency = 0xFFFF;
  }

  ///
  /// Update the PPM Global NVS Area.
  /// Update the PPM NVRAM values for C3
  ///
  mPpmGlobalNvsAreaProtocol->Area->C3MwaitValue = 0x10;
  mPpmGlobalNvsAreaProtocol->Area->C3Latency    = (UINT16) LCR0Latency;
  ///
  /// Update PPM NVRAM Values for C6
  ///
  if ((mCpuPmConfig->pFunctionEnables->LongLatencyC6) &&( mPpmGlobalNvsAreaProtocol->Area->PpmFlags & C6_LONG_LATENCY_ENABLE)) {
    mPpmGlobalNvsAreaProtocol->Area->C6MwaitValue = 0x21;
    mPpmGlobalNvsAreaProtocol->Area->C6Latency    = (UINT16) LCR2Latency;
  } else {
    mPpmGlobalNvsAreaProtocol->Area->C6MwaitValue = 0x20;
    mPpmGlobalNvsAreaProtocol->Area->C6Latency    = (UINT16) LCR1Latency;
  }
  ///
  /// Update PPM NVRAM Values for C7 - select the C-state supported among- C7 / C7S
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C7) { // Is C7 supported ?
    if ((mCpuPmConfig->pFunctionEnables->LongLatencyC7) && (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & C7_LONG_LATENCY_ENABLE)) {
      mPpmGlobalNvsAreaProtocol->Area->C7MwaitValue = 0x31;
      mPpmGlobalNvsAreaProtocol->Area->C7Latency    = (UINT16) LCR2Latency;
    } else {
      mPpmGlobalNvsAreaProtocol->Area->C7MwaitValue = 0x30;
      mPpmGlobalNvsAreaProtocol->Area->C7Latency    = (UINT16) LCR1Latency;
    }
  }
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C7S) { // Is C7S supported ?
    if ((mCpuPmConfig->pFunctionEnables->LongLatencyC7) && (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & C7s_LONG_LATENCY_ENABLE)) {
      mPpmGlobalNvsAreaProtocol->Area->C7MwaitValue = 0x33;
      mPpmGlobalNvsAreaProtocol->Area->C7Latency    = (UINT16) LCR2Latency;
    } else {
      mPpmGlobalNvsAreaProtocol->Area->C7MwaitValue = 0x32;
      mPpmGlobalNvsAreaProtocol->Area->C7Latency    = (UINT16) LCR1Latency;
    }
  }
  ///
  /// Update PPM NVRAM Values for CD - select the deepest C-state supported among- C8 / C9 / C10
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C10) { // C10 supported
    mPpmGlobalNvsAreaProtocol->Area->CDIOLevel    = PCH_ACPI_LV7;
    mPpmGlobalNvsAreaProtocol->Area->CDPowerValue = C10_POWER;
    mPpmGlobalNvsAreaProtocol->Area->CDMwaitValue = 0x60;
    mPpmGlobalNvsAreaProtocol->Area->CDLatency    = (UINT16) LCR5Latency;
  } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C9) { // C9 supported
    mPpmGlobalNvsAreaProtocol->Area->CDIOLevel    = PCH_ACPI_LV6;
    mPpmGlobalNvsAreaProtocol->Area->CDPowerValue = C9_POWER;
    mPpmGlobalNvsAreaProtocol->Area->CDMwaitValue = 0x50;
    mPpmGlobalNvsAreaProtocol->Area->CDLatency    = (UINT16) LCR4Latency;
  } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C8) { // C8 supported
    mPpmGlobalNvsAreaProtocol->Area->CDIOLevel    = PCH_ACPI_LV5;
    mPpmGlobalNvsAreaProtocol->Area->CDPowerValue = C8_POWER;
    mPpmGlobalNvsAreaProtocol->Area->CDMwaitValue = 0x40;
    mPpmGlobalNvsAreaProtocol->Area->CDLatency    = (UINT16) LCR3Latency;
  }

  return;
}

/**
  BootScript for PCode Mailbox function for mailbox write commands.
  This function will poll the mailbox interface for control, issue the command
  during s3 resume

  @param[IN]  MailboxCommand,
  @param[IN]  MailboxData,
**/
VOID
MailboxS3Write (
  IN  UINT32  MailboxCommand,
  IN  UINT32  MailboxData
  )
{
#ifdef ULT_FLAG
  UINT32          Data32Mask;
  UINT32          Data32Value;
  UINT16          StallCount;
  UINT32          MchBar;

  StallCount = 0;

  ///
  ///  Poll the run/busy to ensure the interface is available
  ///
  Data32Mask = BIT31;
  Data32Value = 0;
  MchBar = (MmioRead32 (MmPciAddress (0, 0, 0, 0, MCHBAR_OFFSET)) &~BIT0);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET),
    &Data32Mask,
    &Data32Value,
    MAILBOX_WAIT_STALL,
    MAILBOX_WAIT_TIMEOUT
    );

  ///
  /// Write the PCODE mailbox DATA field
  ///
  MchBar = (MmioRead32 (MmPciAddress (0, 0, 0, 0, MCHBAR_OFFSET)) &~BIT0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) ((MchBar + PCODE_MAILBOX_DATA_OFFSET)),
    1,
    &(MailboxData)
    );
  ///
  /// Write the PCODE mailbox Command field
  ///
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET),
    1,
    &(MailboxCommand)
    );
#endif // ULT_FLAG
  return;
}

/**
  Calibrate 24MHz BCLK support to reduce the power consumption in idle states.

  @retval    EFI_UNSUPPORTED   Unrecognized 24MHz BCLK Calibration Type.
  @retval    EFI_SUCCESS       Processor C-State 24MHz BCLK support calibrated successfully.
**/
EFI_STATUS
CalibrateBclkForCStates (
  VOID
  )
{
  /*****************************************************************************************************************
        - BIOS can choose to configure the conversion factor or allow PCODE to calibrate itself or have NO calibration at all.
        - If NO Calibration then the below steps are needed
                o BIOS should bypass all calibration process and write a constant value via "WRITE convert ratio"
        - If BIOS chooses PCODE calibration then the below steps are needed
                o BIOS writes FSM interval: A value of all Fs is recommended for this
                o Read PCODE calibration factor
                o Store and use if needed on next Power up for BIOS calibration
        - If BIOS chooses to calibrate itself then the below steps are needed
                o Send command for calibration to prevent BCLK shut off
                o Read TSC counter values (send command for atomic sampling of TSC100 and TSC24, then read the values)
                o Delay (what is the maximum tolerable delay?)
                o Read counters again
                o Divide 100 MHz ticks by 24 MHz ticks to get the calibration factor
                o Store the calibration factor value
                o Write calibration factor using mailbox command during subsequent power-ups

  Command:        Command Name:               Description:
  0x80000009      WriteTsc24_100Cmd           Sample 24 MHz and 100 MHz TSC simultaneously
  0x80000109      ReadTsc24LowerCmd           Read lower 32 bits of 24 MHz TSC
  0x80000209      ReadTsc24UpperCmd           Read upper 32 bits of 24 MHz TSC
  0x80000309      ReadTsc100LowerCmd          Read lower 32 bits of 100 MHz TSC
  0x80000409      ReadTsc100UpperCmd          Read upper 32 bits of 100 MHz TSC
  0x80000509      ReadPcodeCalibratedCmd      Read PCODE calibrated conversion factor
  0x80000609      WriteConversionRatioCmd     Command for writing the conversion ratio
  0x80000709      WritePreventBclkOffCmd      Command for calibration prevents BCLK from shutting off and prevents package
                                              from entering deep C states
  0x80000809      WRITE_MEASURE_INTERVAL_CMD  Measurement interval for pCode calibration of TSC24-to-TSC100 conversion factor
  0x80000909      WriteFsmMeasureIntvlCmd     Write FSM measure interval
  0x85000000      StartCalValue               Constant value to start calibration
  0xFFFFFFFF      PcodeIntervalValue          PCODE flow calibration time value

  ***************************************************************************************************************/

  EFI_STATUS Status = EFI_SUCCESS;

#ifdef ULT_FLAG
  CPU_FAMILY mCpuFamilyId;
  mCpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  if (mCpuFamilyId == EnumCpuHswUlt) {
    UINT32 PcodeIntervalValue;
    UINT32 BiosMeasureIntervalValue;
    UINT32 PcalFactor_Lower;
    UINT32 PcalFactor_Upper;
    UINT32 LibStatus;
    UINT64 Tsc24_64;
    UINT64 Tsc100_64;
    UINT64 Temp24_64;
    UINT64 Temp100_64;
    UINT64 PcalFactor;
    UINT64 SafeCalibrationValue;
    UINT64 TscRemainder;

    PCODE_BCLK_CALIBRATION_MAILBOX ReturnCalVal;

    PcodeIntervalValue                = 0xFFFFFFFF;
    BiosMeasureIntervalValue          = 0x7270E00;
    PcalFactor_Lower                  = 0x0;
    PcalFactor_Upper                  = 0x0;
    Tsc24_64                          = 0x0;
    Tsc100_64                         = 0x0;
    Temp24_64                         = 0x0;
    Temp100_64                        = 0x0;
    PcalFactor                        = 0x0;
    SafeCalibrationValue              = 0x0;
    TscRemainder                      = 0x0;
    LibStatus                         = 0x0;
    DEBUG ((EFI_D_INFO, "Initializing 24MHz BCLK calibration \n"));

    switch (mCpuPmConfig->PcodeCalibration) {
      case NO_CALIBRATE:
        ///
        /// Bypass all calibration process
        ///
        DEBUG ((EFI_D_INFO, "Bypass the 24MHz BCLK calibration \n"));
        ///
        /// Write a constant value
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_CONVERTION_RATIO_CMD, START_CAL_VALUE, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed to write a constant value \n"));
          break;
        }
        MailboxS3Write(WRITE_CONVERTION_RATIO_CMD, START_CAL_VALUE);
      break;

      case PCODE_CALIBRATE:
        ///
        /// Initiate the PCODE calibration
        ///
        DEBUG ((EFI_D_INFO, "Initiate PCODE 24MHz BCLK calibration \n"));
        ///
        /// BIOS writes FSM interval: A value of all F's is recommended for this
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_FSM_MEASURE_INTVL_CMD, PcodeIntervalValue, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed BIOS writes FSM interval \n"));
          break;
        }
        MailboxS3Write(WRITE_FSM_MEASURE_INTVL_CMD, PcodeIntervalValue);
        break;

      case BIOS_CALIBRATE:
        ///
        /// Initiate Bios calibration
        ///
        DEBUG ((EFI_D_INFO, "Initiate BIOS 24MHz BCLK calibration \n"));
        ///
        /// Send command for calibration to prevent BCLK shut off
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_PREVENT_BCLKOFF_CMD, 1, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Send command for calibration to prevent BCLK shut off \n"));
          break;
        }
        ///
        /// Read TSC counter values (send command for atomic sampling of TSC100 and TSC24)
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, SAMPLE_TSC_24AND100_CMD, 0, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed First Write TSC counter values (send command for atomic sampling of TSC100 and TSC24) \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC24_LOWER_CMD, &ReturnCalVal.TSC24_L1, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed First read TSC-24 Lower 32 bits \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC24_UPPER_CMD, &ReturnCalVal.TSC24_U1, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed First read TSC-24 Upper 32 bits \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC100_LOWER_CMD, &ReturnCalVal.TSC100_L1, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed First read TSC-100 Lower 32 bits \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC100_UPPER_CMD, &ReturnCalVal.TSC100_U1, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed First read TSC-100 Upper 32 bits \n"));
          break;
        }
        ///
        /// Store initial clock values
        ///
        Tsc24_64 = ((UINT64)ReturnCalVal.TSC24_U1 << 32);
        Temp24_64 = (Tsc24_64 |= (ReturnCalVal.TSC24_L1));
        Tsc100_64 = ((UINT64)ReturnCalVal.TSC100_U1 << 32);
        Temp100_64 = (Tsc100_64 |= (ReturnCalVal.TSC100_L1));

        ///
        /// Delay for 22 ms
        ///
        gBS->Stall (PCODE_BCLK_CALIBRATION_TIMEOUT * STALL_ONE_MILLI_SECOND);

        ///
        /// Read TSC 24 and TSC 100 counters again, and calculate calibration factor
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, SAMPLE_TSC_24AND100_CMD, 0, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Second Write TSC counter values (send command for atomic sampling of TSC100 and TSC24) \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC24_LOWER_CMD, &ReturnCalVal.TSC24_L2, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Second read TSC-24 Lower 32 bits \n"));
           break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC24_UPPER_CMD, &ReturnCalVal.TSC24_U2, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Second read TSC-24 Upper 32 bits \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC100_LOWER_CMD, &ReturnCalVal.TSC100_L2, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Second read TSC-100 Lower 32 bits \n"));
          break;
        }
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_TSC100_UPPER_CMD, &ReturnCalVal.TSC100_U2, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Second read TSC-100 Upper 32 bits \n"));
          break;
        }
        ///
        /// Store current clock values, and calculate difference
        ///
        Tsc24_64 = ((UINT64)ReturnCalVal.TSC24_U2 << 32);
        Tsc24_64 = ((Tsc24_64 |= (ReturnCalVal.TSC24_L2)) - Temp24_64);
        Tsc100_64 = ((UINT64)ReturnCalVal.TSC100_U2 << 32);
        Tsc100_64 = ((Tsc100_64 |= (ReturnCalVal.TSC100_L2)) - Temp100_64);

        ///
        /// Calculate updated conversion factor in fixed point format (U32.3.29)
        ///
        Tsc100_64 = (Tsc100_64 << 29);
        if (Tsc24_64 !=0) {
          PcalFactor = InternalMathDivRemS64x64 (Tsc100_64, Tsc24_64, (INT64 *) &TscRemainder);
        }

        ///
        /// Read the TSC24-to-TSC100 conversion factor currently in use by pCode
        ///
        Status = MailboxRead(MAILBOX_TYPE_PCODE, READ_PCODE_CALIBRATED_CMD, &ReturnCalVal.PCalFactor, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed read TSC24-to-TSC100 PCalFactor from pCode currently\n"));
          break;
        }

        if (PcalFactor <= 0) {
         ///
         /// Set Safe Calibration Value as ReturnCalVal.PCalFactor which is TSC24-to-TSC100 PCalFactor read from pCode by READ_PCODE_CALIBRATED_CMD
         ///
          PcalFactor = ReturnCalVal.PCalFactor;
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Warning - SafeCalibrationValue used \n"));
        }
        PcalFactor_Lower |= PcalFactor;
        PcalFactor_Upper |= (PcalFactor >> 32);
        ///
        /// Calibrate 24MHz BCLK using the calculated calibration factor value
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_CONVERTION_RATIO_CMD, PcalFactor_Lower, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Calibrate 24MHz BCLK using the calculated lower calibration factor value \n"));
          break;
        }
        MailboxS3Write(WRITE_CONVERTION_RATIO_CMD, PcalFactor_Lower);

        ///
        /// Send command for calibration to turn BCLK on
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_PREVENT_BCLKOFF_CMD, 0, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Send command for calibration to turn BCLK on \n"));
          break;
        }
        MailboxS3Write(WRITE_PREVENT_BCLKOFF_CMD, 0);

        ///
        /// Write measurement interval for pCode calibration of TSC24-to-TSC100 conversion factor
        ///
        Status = MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_MEASURE_INTERVAL_CMD, BiosMeasureIntervalValue, &LibStatus);
        if (Status != EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "24 MHz BCLK Calibration: Failed Send command for calibration to meature interval for pCode calibration \n"));
          break;
        }
        MailboxS3Write(WRITE_MEASURE_INTERVAL_CMD, BiosMeasureIntervalValue);
        break;

      default:
        DEBUG ((EFI_D_ERROR, "Unrecognized 24MHz BCLK Calibration Type \n"));
        Status = EFI_UNSUPPORTED;
        break;
    }

    DEBUG ((EFI_D_INFO, "24MHz BCLK calibration completed \n"));

  }
#endif  // ULT_FLAG

  return Status;
}

/**
  Enable C-State support as specified by the input flags on a logical processor.
    Configure BIOS C1 Coordination (SMI coordination)
    Enable IO redirection coordination
    Choose proper coordination method
    Configure extended C-States

  This function must be MP safe.

  @param[in] Buffer    Pointer to a ENABLE_CSTATE_PARAMS containing the necessary
                       information to enable C-States

  @retval EFI_SUCCESS  Processor C-State support configured successfully.
**/
VOID
EFIAPI
ApSafeEnableCStates (
  IN OUT VOID *Buffer
  )
{
  MSR_REGISTER Ia32MiscEnable;
  MSR_REGISTER PmCfgCtrl;
  MSR_REGISTER IoCaptAddr;
  UINT16       C3IoAddress;
  CPU_STEPPING mCpuSteppingId;
  CPU_FAMILY   mCpuFamilyId;
  mCpuFamilyId   = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  mCpuSteppingId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_STEPPING;
  ///
  /// Extract parameters from the buffer
  ///
  C3IoAddress = *((UINT16 *) Buffer);
  ///
  /// If C-states are disabled in setup, disable C-states
  ///
  if (!(mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES)) {
    PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PMG_CST_CONFIG);
    PmCfgCtrl.Dwords.Low &= ~B_PACKAGE_C_STATE_LIMIT;
    AsmWriteMsr64 (MSR_PMG_CST_CONFIG, PmCfgCtrl.Qword);
    return;
  }
  ///
  /// Set C-state package limit to the highest C-state enabled
  ///
  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PMG_CST_CONFIG);
  if (mCpuPmConfig->PkgCStateLimit != PkgCpuDefault) {
    PmCfgCtrl.Dwords.Low &= ~B_PACKAGE_C_STATE_LIMIT;

    if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C10) && (mCpuPmConfig->PkgCStateLimit == PkgAuto)) {
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C10;
    } else if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C9) && (mCpuPmConfig->PkgCStateLimit == PkgAuto)) {
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C9;
    } else if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C8) && (mCpuPmConfig->PkgCStateLimit == PkgAuto)) {
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C8;
    } else if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C7S) &&
               !((mCpuFamilyId == EnumCpuHsw) && (mCpuSteppingId == EnumHswA0) && (mCpuPmConfig->PkgCStateLimit == PkgAuto))) { // When user selects Auto - Disable Package C7s state on Haswell A-step processors
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C7S;
    } else if ((mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C7)&&
               !((mCpuFamilyId == EnumCpuHsw) && (mCpuSteppingId == EnumHswA0) && (mCpuPmConfig->PkgCStateLimit == PkgAuto))) { // When user selects Auto - Disable Package C7 state on Haswell A-step processors
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C7;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C6) {
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C6;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C3) {
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C3;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C1) {
      PmCfgCtrl.Dwords.Low |= V_CSTATE_LIMIT_C1;
    }
    if (mCpuPmConfig->PkgCStateLimit < PkgCMax) {
      PmCfgCtrl.Dwords.Low &= ~B_PACKAGE_C_STATE_LIMIT;
      PmCfgCtrl.Dwords.Low |= (mCpuPmConfig->PkgCStateLimit & B_PACKAGE_C_STATE_LIMIT);
    }
  }
  ///
  /// Enable C State IO redirection by default
  ///
  PmCfgCtrl.Dwords.Low |= B_IO_MWAIT_REDIRECTION_ENABLE;
  //
  // Enable TimedMwait
  //
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TIMED_MWAIT) {
    PmCfgCtrl.Dwords.Low &= (~B_TIMED_MWAIT_ENABLE);
    PmCfgCtrl.Dwords.Low |= B_TIMED_MWAIT_ENABLE;
  }
  ///
  /// Configure C-state auto-demotion
  ///
  PmCfgCtrl.Dwords.Low &= ~(B_C1_AUTO_DEMOTION_ENABLE | B_C3_AUTO_DEMOTION_ENABLE);
  if (mCpuPmConfig->pFunctionEnables->C3AutoDemotion) {
    ///
    /// Enable C6/C7 Auto-demotion to C3
    ///
    PmCfgCtrl.Dwords.Low |= B_C3_AUTO_DEMOTION_ENABLE;
  }
  if (mCpuPmConfig->pFunctionEnables->C1AutoDemotion) {
    ///
    /// Enable C3/C6/C7 Auto-demotion to C1
    ///
    PmCfgCtrl.Dwords.Low |= B_C1_AUTO_DEMOTION_ENABLE;
  }
  ///
  /// Configure C-state un-demotion
  ///
  PmCfgCtrl.Dwords.Low &= ~(B_C1_AUTO_UNDEMOTION_ENABLE | B_C3_AUTO_UNDEMOTION_ENABLE);
  if (mCpuPmConfig->pFunctionEnables->C3UnDemotion) {
    ///
    /// Enable un-demotion from demoted C3
    ///
    PmCfgCtrl.Dwords.Low |= B_C3_AUTO_UNDEMOTION_ENABLE;
  }
  if (mCpuPmConfig->pFunctionEnables->C1UnDemotion) {
    ///
    /// Enable un-demotion from demoted C1
    ///
    PmCfgCtrl.Dwords.Low |= B_C1_AUTO_UNDEMOTION_ENABLE;
  }
  ///
  /// Configure Package C-state Demotion / un-demotion - Supported only on HSW B0 and Above
  ///
  if (mCpuSteppingId > EnumHswA0) {
    PmCfgCtrl.Dwords.Low &= ~(B_PKG_CSTATE_DEMOTION_ENABLE | B_PKG_CSTATE_UNDEMOTION_ENABLE);
    if (mCpuPmConfig->pFunctionEnables->PkgCStateDemotion) {
      ///
      /// Enable Package C-state Demotion
      ///
      PmCfgCtrl.Dwords.Low |= B_PKG_CSTATE_DEMOTION_ENABLE;
    }
    if (mCpuPmConfig->pFunctionEnables->PkgCStateUnDemotion) {
      ///
      /// Enable Package C-state un-demotion
      ///
      PmCfgCtrl.Dwords.Low |= B_PKG_CSTATE_UNDEMOTION_ENABLE;
    }
  }
  AsmWriteMsr64 (MSR_PMG_CST_CONFIG, PmCfgCtrl.Qword);
  ///
  /// Enable MONITOR/MWAIT support
  /// (already done on BSP, but must be done on all components.)
  ///
  Ia32MiscEnable.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  Ia32MiscEnable.Qword |= B_MSR_IA32_MISC_ENABLE_MONITOR;
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, Ia32MiscEnable.Qword);
  ///
  /// Haswell specific configuration of I/O capture and I/O coordination SMI MSR.
  /// Configure the base port and range in the MSR to match LVL_X settings in ACPI tables
    /// Set I/O capture base port and range
    ///
    IoCaptAddr.Qword = AsmReadMsr64 (MSR_PMG_IO_CAPTURE_BASE);
    ///
    /// Mask off CST range and set the CST range
    ///
    IoCaptAddr.Dwords.Low &= ~B_MSR_PMG_CST_RANGE;
    if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C10) {
      IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL7;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C9) {
      IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL6;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C8) {
      IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL5;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C7) {
      IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL4;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C6) {
      IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL3;
    } else if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C3) {
      IoCaptAddr.Dwords.Low |= V_IO_CAPT_LVL2;
    }
    ///
    /// Set the base CST address
    ///
    IoCaptAddr.Dwords.Low &= ~(V_IO_CAPT_LVL2_BASE_ADDR_MASK);
    IoCaptAddr.Dwords.Low |= C3IoAddress;
    AsmWriteMsr64 (MSR_PMG_IO_CAPTURE_BASE, IoCaptAddr.Qword);
  return;
}


//
// Update ACPI IdleStates tables
//

/**
  Configure the FACP for C state support
**/
VOID
ConfigureFadtCStates (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_ACPI_DESCRIPTION_HEADER               *Table;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *FadtPointer;
  INTN                                      Index;
  UINTN                                     Handle;
  EFI_ACPI_TABLE_VERSION                    Version;

  ///
  /// Locate table with matching ID
  ///
  Index = 0;
  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **) &Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }
    ASSERT_EFI_ERROR (Status);
    Index++;
  } while (Table->Signature != EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE);
  //
  // Can't have ACPI without FADT, so safe to assert
  //
  ASSERT (Table->Signature == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE);
  FadtPointer = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *) Table;
  //
  // Verify expected state.  Should be initialized to off during build.
  //
  ASSERT (FadtPointer->PLvl3Lat >= FADT_C3_LATENCY_DISABLED);
  ///
  /// Configure C states
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C3) {
    ///
    /// Enable C3 in FADT.
    ///
    FadtPointer->PLvl3Lat = FADT_C3_LATENCY;
  }
  ///
  /// Update the table
  ///
//[-start-130910-IB10930055-modify]//
  Status = mAcpiSupport->SetAcpiTable (
                  mAcpiSupport,
                  FadtPointer,
                  TRUE,
                  Version,
                  &Handle
                  );
//[-end-130910-IB10930055-modify]//
  FreePool (Table);

  return;
}