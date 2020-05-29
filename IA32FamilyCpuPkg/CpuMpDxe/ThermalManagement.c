/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for ThermalManagement Feature

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ThermalManagement.c

**/

#include "ThermalManagement.h"

UINT8    ThermalManagementCapability[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of Thermal Management feature for specified processor.
  
  This function detects capability of Thermal Management feature feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ThermalManagementDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;

  //
  // Retrieve CPUID data from context buffer
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  //
  // If TM is supported.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEdx, N_CPUID_TM_SUPPORT, N_CPUID_TM_SUPPORT) != 0) {
    ThermalManagementCapability[ProcessorNumber] |= CPU_THERMAL_MONITOR1_BIT;
  }
  //
  // If TM2 is supported.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_TM2_SUPPORT, N_CPUID_TM2_SUPPORT) != 0) {
    ThermalManagementCapability[ProcessorNumber] |= CPU_THERMAL_MONITOR2_BIT;
  }

  //
  // If TM or TM2 is supported, set feature capability for Thermal Management feature.
  //
  if ((ThermalManagementCapability[ProcessorNumber] &
       (CPU_THERMAL_MONITOR1_BIT | CPU_THERMAL_MONITOR2_BIT)) != 0) {
    SetProcessorFeatureCapability (ProcessorNumber, ThermalManagement, NULL);
  }
}

/**
  Configures Processor Feature Lists for Thermal Management feature for all processors.
  
  This function configures Processor Feature Lists for Thermal Management feature for all processors.

**/
VOID
ThermalManagementConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_THERMAL_MANAGEMENT_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Thermal Management
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, ThermalManagement, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Thermal Management capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_THERMAL_MANAGEMENT_BIT);
        //
        // Set the bit of Thermal Management setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_THERMAL_MANAGEMENT_BIT);
        }
      }

      //
      // If this logical processor supports Thermal Management, then add feature into feature list for operation
      // on corresponding register.
      //
      if (UserConfigurationSet) {
        AppendProcessorFeatureIntoList (ProcessorNumber, ThermalManagement, NULL);
      }
    }
  }
}

/**
  Produces entry of Thermal Management feature in Register Table for specified processor.
  
  This function produces entry of Thermal Management feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ThermalManagementReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    TmEnabled;
  BOOLEAN    Tm2Enabled;
  UINT32     FamilyId;
  UINT32     ModelId;

  TmEnabled  = FALSE;
  Tm2Enabled = FALSE;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // If TM2 is supported, then write 1 to MSR_IA32_MISC_ENABLE[13].
  //
  if ((ThermalManagementCapability[ProcessorNumber] & CPU_THERMAL_MONITOR2_BIT) != 0) {
    if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) || IS_IVYBRIDGE_PROC (FamilyId, ModelId)) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_AUTOMATIC_TCC_ENABLE, 1, 1);
    } else {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_TM2_ENABLE, 1, 1);
    }
    Tm2Enabled = TRUE;
  }

  //
  // If TM1 is supported
  //
  if ((ThermalManagementCapability[ProcessorNumber] & CPU_THERMAL_MONITOR1_BIT) != 0) {
    //
    // For Pentium 4 processor, enable TM1 if TM1 is supported and TM2 is not enabled.
    // For Conroe processor family, enable TM1 just if it is supported.
    //
    if ((FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) || !Tm2Enabled) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_AUTOMATIC_TCC_ENABLE, 1, 1);
      TmEnabled = TRUE;
    }
  }

  //
  // If TM or TM2 is enabled, then enable bi-directional PROCHOT# or FORCEPR#,
  // depending on the platform type.
  //
  if (TmEnabled || Tm2Enabled) {
    //
    // If this is not a server platform, enable Bi-Directional PROCHOT#
    //
    if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) ||
        IS_IVYBRIDGE_PROC (FamilyId, ModelId)) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_POWER_CTL, N_MSR_BI_DIRECTIONAL_PROCHOT_ENABLE, 1, 1);
    } else {
      //
      // If this is a server platform, enable FORCEPR#
      //
      if (mPlatformType == PLATFORM_SERVER) {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, 21, 1, 1);
      } else {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, 17, 1, 1);
      }
    }
  }

  //
  // For Conroe Processor Family, advanced registers should be set.
  //
  if (FeaturePcdGet (PcdCpuConroeFamilyFlag) || FeaturePcdGet (PcdCpuNehalemFamilyFlag) ||
      FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) ||
      FeaturePcdGet (PcdCpuHaswellFamilyFlag)) {
    if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) {
      //
      // BIOS should enable Lock TM Interrupt(MSR_PIC_SENS_CFG[22]), Lock TM1(MSR_PIC_SENS_CFG[21]).
      // In addition, if both TM1 and TM2 are enabled, we also enable Extended Throttle(MSR_PIC_SENS_CFG[20]).
      //
      if (TmEnabled && Tm2Enabled) {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_PIC_SENS_CFG, 20, 3, 7);
      } else {
        WriteRegisterTable (ProcessorNumber, Msr, MSR_PIC_SENS_CFG, 21, 2, 3);
      }
    } else if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) ||
               IS_IVYBRIDGE_PROC (FamilyId, ModelId) || IS_HASWELL_PROC (FamilyId, ModelId)) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_PWR_MGMT, N_MSR_LOCK_TM_INTERRUPT, 1, 1);
    }
  }
}
