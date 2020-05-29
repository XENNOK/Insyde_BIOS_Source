/** @file
  This library contains power management configuration functions for
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

/**
  This will perform general thermal initialization other then TM1, TM2, or
  PROCHOT# on all logical processors.

  @param[in] CpuPmConfig               Pointer to policy protocol instance
  @param[in] PpmGlobalNvsAreaProtocol  Pointer to PPM Global Nvs area

  @retval EFI_SUCCESS  General thermal initialization completed successfully
**/
VOID
InitThermal (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER TempMsr;
  UINT8        MaxRefTemp;
  CPU_FAMILY   CpuFamilyId;
  CPU_STEPPING CpuStepping;
  UINT8        TccActivationOffsetMask;

  ///
  /// Run thermal code on all logical processors.
  ///
  RunOnAllLogicalProcessors (ApSafeInitThermal, CpuPmConfig);
  ///
  /// Automatic Thermal Reporting for Thermal Management
  ///
  if (CpuPmConfig->ThermalFuncEnables->AutoThermalReporting) {
    ///
    /// Thermal Reporting for Critical trip
    /// MSR 1A2 bits 23:16 define the  temperature that this specific processor can
    /// function upto. It is recommended that this value + 5  be used as default Critical trip point
    /// _CRT.
    ///
    TempMsr.Qword = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK;
    MaxRefTemp  = (UINT8) RShiftU64 (TempMsr.Qword, N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET);
    mPpmGlobalNvsAreaProtocol->Area->AutoCriticalTripPoint = MaxRefTemp + 5;
    ///
    /// Thermal Reporting for Active Thermal Management
    /// It is recommended that the processor specific value in MSR 1A2 bits 15:8
    /// be used as the highest Active trip point i.e. _AC0.
    ///
    TempMsr.Qword = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET;
    mPpmGlobalNvsAreaProtocol->Area->AutoActiveTripPoint = MaxRefTemp - (UINT8) RShiftU64 (
                    TempMsr.Qword,
                    N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET
                    );

    ///
    /// Tcc activation offset in temperature target MSR changes from 4 bits [27:24] to 6 bits [29:24] on ULT C step onwards
    ///
    CpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
    CpuStepping = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_STEPPING;
    if ((CpuFamilyId == EnumCpuHswUlt) && (CpuStepping >= EnumHswUltC0)) {
      TccActivationOffsetMask = 0x3F;
    } else {
      TccActivationOffsetMask = 0xF;
    }

    ///
    /// Thermal Reporting for Passive Thermal Management
    /// On all turbo enabled systems, it is recommended that the ACPI _PSV point be
    /// set to a temperature above the Active cooling temperature and Tcc activation
    /// temperature.
    /// If platform embedded controller will issue PECI commands to reduce power as a
    /// passive thermal action, then it is recommended to use the package's max temperature
    /// for passive thermal control.
    ///
    TempMsr.Qword = AsmReadMsr64 (MSR_TEMPERATURE_TARGET);
    TempMsr.Qword &= (TccActivationOffsetMask << N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT);
    mPpmGlobalNvsAreaProtocol->Area->AutoPassiveTripPoint = mPpmGlobalNvsAreaProtocol->Area->AutoCriticalTripPoint+3;
  }

  EnableProcHot ();
  return;
}

/**
  This will perform enable thermal initialization. TM1, TM2 and adaptive thermal
  throttling are enabled/disabled together.

  This function must be MP safe.

  @param[in] Buffer    Pointer to the function parameters passed in.

  @retval EFI_SUCCESS  General thermal initialization completed successfully
**/
VOID
EFIAPI
ApSafeInitThermal (
  IN OUT VOID *Buffer
  )
{
  MSR_REGISTER      TempMsr;
  POWER_MGMT_CONFIG *This;

  ///
  /// Extract parameters from the buffer
  ///
  This = (POWER_MGMT_CONFIG *) Buffer;
  ///
  /// Configure Adaptive thermal monitor. IA32_MISC_ENABLE[3]
  /// HSW BWG (1A0h)IA32_MISC_ENABLE - Bit3:Intel Adaptive Thermal Monitor Enable
  ///   System BIOS must always set this bit to be operating within spec.
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
  TempMsr.Dwords.Low |= B_MSR_IA32_MISC_ENABLE_TME;
  if (This->ThermalFuncEnables->ThermalMonitor == 0) {
    TempMsr.Dwords.Low &= ~B_MSR_IA32_MISC_ENABLE_TME;
  }
  AsmWriteMsr64 (MSR_IA32_MISC_ENABLE, TempMsr.Qword);
  ///
  /// Set the Lock TM interrupt bit so that thermal interrupts are routed to all the cores
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_MISC_PWR_MGMT);
  TempMsr.Qword |= B_MSR_MISC_PWR_MGMT_LTMI;
  AsmWriteMsr64 (MSR_MISC_PWR_MGMT, TempMsr.Qword);
  ///
  /// Enable Critical Temperature Interrupt
  ///
  TempMsr.Qword = AsmReadMsr64 (IA32_THERM_INTERRUPT);
  TempMsr.Qword |= B_IA32_THERM_INTERRUPT_VIE;
  AsmWriteMsr64 (IA32_THERM_INTERRUPT, TempMsr.Qword);

  return;
}

/**
  Enables the bi-directional PROCHOT# signal.

  @retval EFI_SUCCESS  PROCHOT# configured successfully
**/
EFI_STATUS
EnableProcHot (
  VOID
  )
{
  MSR_REGISTER PowerCtl;

  ///
  /// Enable PROCHOT# in the CPU MSR if TM is enabled,
  ///  else disable it.
  ///
  PowerCtl.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & (PPM_TM)) {
    PowerCtl.Qword &= ~B_MSR_POWER_CTL_DISABLE_VR_THERMAL_ALERT;
    if ((mCpuPmConfig->ThermalFuncEnables->DisableVRThermalAlert == 1)) {
      DEBUG ((EFI_D_INFO, "VR Thermal Alert is disabled\n"));
      PowerCtl.Qword |= B_MSR_POWER_CTL_DISABLE_VR_THERMAL_ALERT;
    }
    ///
    /// Check PROCHOT Lock,skip programming the below as it will lock bits 0, 21, 22
    ///
    if (!(PowerCtl.Qword & B_MSR_POWER_CTL_PROC_HOT_LOCK)) {
      PowerCtl.Qword &= ~B_MSR_POWER_CTL_BROCHOT;
      if (mCpuPmConfig->ThermalFuncEnables->BiProcHot) {
        PowerCtl.Qword |= B_MSR_POWER_CTL_BROCHOT;
        ///
        /// Initialize PROCHOT# OUT basing on Bi-directional PROCHOT# setting
        /// If Bi-directional PROCHOT# is enabled, PROCHOT# OUT can be disabled selectively
        ///
        PowerCtl.Qword &= ~B_MSR_POWER_CTL_DISABLE_PHOT_OUT;
        if ((mCpuPmConfig->ThermalFuncEnables->DisableProcHotOut == 1)) {
          DEBUG ((EFI_D_INFO, "PROCHOT# OUT is disabled\n"));
          PowerCtl.Qword |= B_MSR_POWER_CTL_DISABLE_PHOT_OUT;
        }
      }
      PowerCtl.Qword &= ~B_MSR_POWER_CTL_PROC_HOT_RESPONSE;
      if ((mCpuPmConfig->ThermalFuncEnables->ProcHotResponce == 1)) {
        DEBUG ((EFI_D_INFO, "PROCHOT# Response is enabled\n"));
        PowerCtl.Qword |= B_MSR_POWER_CTL_PROC_HOT_RESPONSE;
      }
    }
    AsmWriteMsr64 (MSR_POWER_CTL, PowerCtl.Qword);
  }

  return EFI_SUCCESS;
}
/**
  This will perform PowerLimit 1 algorithm will be used to control Thermal Throttling features
  
  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitPl1ThermalControl (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  UINT32               MailBoxStatus;
  PL1_THERMAL_CONTROL  WritePL1ThermalControl;
  PL1_THERMAL_CONTROL  ReadPL1ThermalControl;
  CPU_FAMILY           CpuFamilyId;
  CPU_STEPPING         CpuSteppingId;
  MSR_REGISTER         PakagePowerLimitMsr;
  UINTN                PciD0F0RegBase;
  UINTN                MchBar;

  MailBoxStatus = 0;
  ReadPL1ThermalControl.Uint32  = 0;
  WritePL1ThermalControl.Uint32 = 0;

  CpuFamilyId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  CpuSteppingId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_STEPPING;


  if (((CpuFamilyId == EnumCpuHswUlt) && (CpuSteppingId >= EnumHswUltC0))
      || ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0))
      || ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwC0))
    ) {
    ///
    /// If User selects Auto,Enable by default on ULX and disable on orther processor
    ///
    if (CpuPmConfig->ThermalFuncEnables->Pl1ThermalControl == 1) {  // 0 = disable, 1 = enable (manual), 2 = auto
         WritePL1ThermalControl.Bits.Disable = 0;
    } else if (CpuPmConfig->ThermalFuncEnables->Pl1ThermalControl == 2) {
      if (mPackageTdp < 15 * mProcessorPowerUnit) { // ULX
         WritePL1ThermalControl.Bits.Disable = 0;
      } else {
         WritePL1ThermalControl.Bits.Disable = 1;
      }
    } else {
        WritePL1ThermalControl.Bits.Disable = 1;
    }
    if (mCpuPolicyRevision >= 7) {
      WritePL1ThermalControl.Bits.FloorIA = CpuPmConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorIA;
      WritePL1ThermalControl.Bits.FloorGT = CpuPmConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorGT;
      WritePL1ThermalControl.Bits.FloorPCH = CpuPmConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorPCH;
    }
    
    MailboxRead(MAILBOX_TYPE_PCODE, READ_PL1_DUTY_CYCLE_CLAMP_ENABLE, (UINT32 *) &ReadPL1ThermalControl, &MailBoxStatus);
    if (MailBoxStatus != PCODE_MAILBOX_CC_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Failure to read PowerLimit1 duty sysle clamp enable. \n"));
      return;
    }
    DEBUG ((EFI_D_INFO, "Current Pl1ThermalControl reading from Mailbox : %d. Requested setting to Mailbox: %d\n", ReadPL1ThermalControl.Uint32, WritePL1ThermalControl.Uint32));
    ///
    /// If Mailbox returns differnt from user selection send command to set user selection
    ///
    if (ReadPL1ThermalControl.Uint32 != WritePL1ThermalControl.Uint32) {
      MailboxWrite(MAILBOX_TYPE_PCODE, WRITE_PL1_DUTY_CYCLE_CLAMP_ENABLE, WritePL1ThermalControl.Uint32,&MailBoxStatus);
      if (MailBoxStatus != PCODE_MAILBOX_CC_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "Failure to write PowerLimit1 duty sysle clamp enable. \n"));
        return;
      }
    }
    MailboxS3Write(WRITE_PL1_DUTY_CYCLE_CLAMP_ENABLE, WritePL1ThermalControl.Uint32);

    ///
    /// Set PACKAGE_POWER_LIMIT.CRITICAL_POWER_CLAMP_1(bit 16)
    /// And MMIO_TURBO_POWER_LIMIT.CRITICAL_POWER_CLAMP_1(bit 16)
    ///
    PakagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
    PakagePowerLimitMsr.Dwords.Low |= B_CRITICAL_POWER_CLAMP_ENABLE;
    AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, PakagePowerLimitMsr.Qword);
    
    PciD0F0RegBase  = MmPciAddress (0, 0, 0, 0, 0);
    MchBar          = MmioRead32 (PciD0F0RegBase + 0x48) &~BIT0;
    MmioOr32 (MchBar + MMIO_TURBO_POWER_LIMIT, B_CRITICAL_POWER_CLAMP_ENABLE);

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (MchBar + MMIO_TURBO_POWER_LIMIT),
      1,
      (VOID *) (UINTN) (MchBar + MMIO_TURBO_POWER_LIMIT)
      );
  }
  return;
}