/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for C-State feature.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CState.c

**/

#include "CState.h"

BOOLEAN    EnableCState[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableC1AutoDemotion[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableC3AutoDemotion[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
UINTN      MaxCStateValue[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of C-State for specified processor.
  
  This function detects capability of C-State for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
CStateDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT32              FamilyId;
  UINT32              ModelId;
  UINT32              SteppingId;
  UINTN               Index;
  UINTN               CStateValue;
  UINT32              Params;
  
  //
  // C-State feature has dependency on Monitor/Mwait capability, so
  // Monitor/Mwait feature detection should be perform prior to C-State feature.
  //
  if (!GetProcessorFeatureCapability (ProcessorNumber, MonitorMwait, NULL)) {
    return;
  }

  //
  // By default, Monitor/Mwait is available if it is supported. We don't need to enable it here.
  //
  DEBUG_CODE (
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
    ASSERT (CpuidRegisters != NULL);
    ASSERT (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_MONITOR_MWAIT_SUPPORT, N_CPUID_MONITOR_MWAIT_SUPPORT) == 1);  
  );
  
  //
  // Processor feature flag checking
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_MONITOR_MWAIT_PARAMS);
  ASSERT (CpuidRegisters != NULL);

  Params = CpuidRegisters->RegEdx & 0xFFFF0;
  if (Params != 0) {
    // 
    // Calculate MAX C state
    // 
    CStateValue = 0;
    for (Index = 0; Index < 4; Index++) {
      Params >>= 4;
      if((Params & 0xF) != 0) {
        CStateValue++;
      } else {
        break;
      }  
    }
    if (CStateValue > 1) {
      MaxCStateValue[ProcessorNumber] = CStateValue;
      SetProcessorFeatureCapability (ProcessorNumber, CState, NULL);

      GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);
      if (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) {
        SetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL);
        SetProcessorFeatureCapability (ProcessorNumber, C3AutoDemotion, NULL);
      }
    }
  }
}

/**
  Configures Processor Feature Lists for C-State for all processors.
  
  This function configures Processor Feature Lists for C-State for all processors.

**/
VOID
CStateConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;
  BOOLEAN               AllProcessorsSupportedCState;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MONITOR_MWAIT_BIT) != 0 &&
    (PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C_STATE_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  AllProcessorsSupportedCState = UserConfigurationSet;

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports C-State
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, CState, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of C-State capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C_STATE_BIT);
        //
        // Set the bit of C-State setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C_STATE_BIT);
        }

        if (GetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL)) {
          PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C1_AUTO_DEMOTION_BIT);
          if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C1_AUTO_DEMOTION_BIT) != 0) {
            PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C1_AUTO_DEMOTION_BIT);
          }
        }
        if (GetProcessorFeatureCapability (ProcessorNumber, C3AutoDemotion, NULL)) {
          PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_C3_AUTO_DEMOTION_BIT);
          if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C3_AUTO_DEMOTION_BIT) != 0) {
            PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_C3_AUTO_DEMOTION_BIT);
          }
        }
      }

      //
      // If this logical processor supports C-State, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableCState[ProcessorNumber] = UserConfigurationSet;
      EnableC1AutoDemotion[ProcessorNumber] = (BOOLEAN)((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C1_AUTO_DEMOTION_BIT) != 0);
      EnableC3AutoDemotion[ProcessorNumber] =  (BOOLEAN)((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_C3_AUTO_DEMOTION_BIT) != 0);
      AppendProcessorFeatureIntoList (ProcessorNumber, CState, &(EnableCState[ProcessorNumber]));
    } else {
      //
      // Not all processors support C-State.
      //
      AllProcessorsSupportedCState = FALSE;
    }
  }

  //
  // Check if all APs could be placed in Mwait-Loop mode.
  //
  if (PcdGet8 (PcdCpuApLoopMode) == ApInMwaitLoop) {
    if (AllProcessorsSupportedCState) {
      mExchangeInfo->ApLoopMode = ApInMwaitLoop;
    } else {
      DEBUG ((EFI_D_WARN,"AP can not be placed in Mwait-Loop mode since not all processors' C-State are supported by platform.\n"));
    }
  }
}

/**
  Produces entry of C-State feature in Register Table for specified processor.
  
  This function produces entry of C-State feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
CStateReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64      MsrValue;
  UINTN       MaxCState;

  Enable = (BOOLEAN *) Attribute;
  if (*Enable) {
    //
    // For Native MWAIT and IO Redirection Support
    // Enable IO MWAIT translation for processor core C-state control.
    // Also limit the package to the max supported C-state.
    //
    MsrValue = AsmReadMsr64 (MSR_PMG_CST_CONFIG_CONTROL);
    MsrValue &= ~B_MSR_PACKAGE_C_STATE_LIMIT_MASK;
    MsrValue |= (PcdGet8 (PcdCpuPackageCStateLimit) & B_MSR_PACKAGE_C_STATE_LIMIT_MASK);
    MsrValue |= B_MSR_IO_MWAIT_REDIRECTION_ENABLE;

    if (GetProcessorFeatureCapability (ProcessorNumber, C1AutoDemotion, NULL) &&
      EnableC1AutoDemotion[ProcessorNumber]) {
      MsrValue |= B_MSR_C1_STATE_AUTO_DEMOTION_ENABLE;
    } else {
      MsrValue &= ~B_MSR_C1_STATE_AUTO_DEMOTION_ENABLE;
    }

    if (GetProcessorFeatureCapability (ProcessorNumber, C3AutoDemotion, NULL) &&
      EnableC3AutoDemotion[ProcessorNumber]) {
      MsrValue |= B_MSR_C3_STATE_AUTO_DEMOTION_ENABLE;
    } else {
      MsrValue &= ~B_MSR_C3_STATE_AUTO_DEMOTION_ENABLE;
    }

    WriteRegisterTable (ProcessorNumber, Msr, MSR_PMG_CST_CONFIG_CONTROL, 0, 64, MsrValue);

    //
    // For IO Redirection Support
    // Tell processor that 2 C-states (besides C1) are supported (C2 and C3).
    // These correspond to LVL_2 and LVL_3.  Setup LVL_2 base address.
    //
    // Enable the MAX C-State as we can, use _CST to report the correct setup in processor setup page
    //
    MaxCState = MaxCStateValue[ProcessorNumber];
    MsrValue = AsmReadMsr64 (MSR_PMG_IO_CAPTURE_BASE);
    if((MaxCState == CST_C3)|| (MaxCState == CST_C6) ||(MaxCState == CST_C7)) {
      MsrValue |= ((MaxCState - 2) << 16);
    }
    MsrValue |= PcdGet16 (PcdCpuAcpiLvl2Addr);
    WriteRegisterTable (ProcessorNumber, Msr, MSR_PMG_IO_CAPTURE_BASE, 0, 64, MsrValue);
  }
}

