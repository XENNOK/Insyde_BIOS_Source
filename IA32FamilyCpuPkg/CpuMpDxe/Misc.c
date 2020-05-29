/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for misc CPU features.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Misc.c

**/

#include "Misc.h"

BOOLEAN    EnableMonitorMwait[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableThreeStrikeCounter[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableEnergyPerformanceBias[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of MONITOR/MWAIT instructions for specified processor.
  
  This function detects capability of MONITOR/MWAIT instructions for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MonitorMwaitDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, MonitorMwait, NULL);
  }
}

/**
  Configures Processor Feature Lists for MONITOR/MWAIT instructions for all processors.
  
  This function configures Processor Feature Lists for MONITOR/MWAIT instructions for all processors.

**/
VOID
MonitorMwaitConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MONITOR_MWAIT_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports MONITOR/MWAIT instructions
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, MonitorMwait, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of MONITOR/MWAIT instructions capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_MONITOR_MWAIT_BIT);
        //
        // Set the bit of MONITOR/MWAIT instructions setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_MONITOR_MWAIT_BIT);
        }
      }

      //
      // If this logical processor supports MONITOR/MWAIT instructions, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableMonitorMwait[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, MonitorMwait, &(EnableMonitorMwait[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of MONITOR/MWAIT instruction feature in Register Table for specified processor.
  
  This function produces entry of MONITOR/MWAIT instruction feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MonitorMwaitReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_ENABLE_MONITOR_FSM, 1, Value);
}

/**
  Detect capability of Three Strike Counter for specified processor.
  
  This function detects capability of Three Strike Counter Prefetch for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ThreeStrikeCounterDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, ThreeStrikeCounter, NULL);
  }
}

/**
  Configures Processor Feature Lists for Three Strike Counter Prefetch for all processors.
  
  This function configures Processor Feature Lists for Three Strike Counter Prefetch for all processors.

**/
VOID
ThreeStrikeCounterConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_THREE_STRIKE_COUNTER_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Three Strike Counter
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, ThreeStrikeCounter, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Three Strike Counter capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_THREE_STRIKE_COUNTER_BIT);
        //
        // Set the bit of Three Strike Counter setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_THREE_STRIKE_COUNTER_BIT);
        }
      }

      //
      // If this logical processor supports Three Strike Counter, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableThreeStrikeCounter[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, ThreeStrikeCounter, &(EnableThreeStrikeCounter[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of Three Strike Counter in Register Table for specified processor.
  
  This function produces entry of Three Strike Counter in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ThreeStrikeCounterReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_FEATURE_CONTROL, N_MSR_THREE_STRIKE_COUNTER_DISABLE, 1, Value);
}

/**
  Detect capability of Energy Performance Bias for specified processor.
  
  This function detects capability of Energy Performance Bias for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
EnergyPerformanceBiasDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, EnergyPerformanceBias, NULL);
  }
}

/**
  Configures Processor Feature Lists for Energy Performance Bias for all processors.
  
  This function configures Processor Feature Lists for Energy Performance Bias for all processors.

**/
VOID
EnergyPerformanceBiasConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_ENERGY_PERFORMANCE_BIAS_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Energy Performance Bias
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, EnergyPerformanceBias, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Energy Performance Bias capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_ENERGY_PERFORMANCE_BIAS_BIT);
        //
        // Set the bit of Energy Performance Bias setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_ENERGY_PERFORMANCE_BIAS_BIT);
        }
      }

      //
      // If this logical processor supports Energy Performance Bias, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableEnergyPerformanceBias[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, EnergyPerformanceBias, &(EnableEnergyPerformanceBias[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of Energy Performance Bias in Register Table for specified processor.
  
  This function produces entry of Energy Performance Bias in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
EnergyPerformanceBiasReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  Enable = (BOOLEAN *) Attribute;
  if (*Enable) {
    Value = 1;
    WriteRegisterTable (ProcessorNumber, Msr, MSR_POWER_CTL, N_MSR_IA32_ENERGY_PERF_BIAS_ACCESS_ENABLE, 1, Value);
    WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_ENERGY_PERFORMANCE_BIAS, N_MSR_POWER_POLICY_PREFERENCE_START, 4, PcdGet8 (PcdCpuEnergyPolicy)); 
  } else {
    Value  = 0;
    WriteRegisterTable (ProcessorNumber, Msr, MSR_POWER_CTL, N_MSR_IA32_ENERGY_PERF_BIAS_ACCESS_ENABLE, 1, Value);
  }
}

/**
  Detect capability of APIC TPR Update Message for specified processor.
  
  This function detects capability of APIC TPR Update Message for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ApicTprUpdateMessageDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId))  ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, ApicTprUpdateMessage, NULL);
  }
}

/**
  Configures Processor Feature Lists for APIC TPR Update Message for all processors.
  
  This function configures Processor Feature Lists for APIC TPR Update Message for all processors.

**/
VOID
ApicTprUpdateMessageConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;

  //
  // BIOS must clear Apic Tpr Update Message Disable bit, otherwise OS initialization of interrupts will fail.
  // No user selection is allowed.
  //

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports APIC TPR Update Message
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, ApicTprUpdateMessage, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of APIC TPR Update Message capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_APIC_TPR_UPDATE_MESSAGE_BIT);
        //
        // Set the bit of APIC TPR Update Message setting according to BSP setting.
        //
        PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_APIC_TPR_UPDATE_MESSAGE_BIT);
      }

      AppendProcessorFeatureIntoList (ProcessorNumber, ApicTprUpdateMessage, NULL);
    }
  }
}

/**
  Produces entry of APIC TPR Update Message feature in Register Table for specified processor.
  
  This function produces entry of APIC TPR Update Message feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ApicTprUpdateMessageReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  WriteRegisterTable (ProcessorNumber, Msr, MSR_PIC_MSG_CONTROL, N_MSR_APIC_TPR_UPD_MSG_DISABLE, 1, 0);
}

