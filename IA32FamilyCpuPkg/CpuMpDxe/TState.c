/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for T State feature.

  Copyright (c) 2011 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  TState.c

**/

#include "TState.h"

BOOLEAN    EnableTState[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of T-State for specified processor.
  
  This function detects capability of T-State for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
TStateDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, TState, NULL);
  }
}

/**
  Configures Processor Feature Lists for T-State for all processors.
  
  This function configures Processor Feature Lists for T-State for all processors.

**/
VOID
TStateConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_TSTATE_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports T-State
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, TState, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of T-State capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_TSTATE_BIT);
        //
        // Set the bit of T-State setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_TSTATE_BIT);
        }
      }

      //
      // If this logical processor supports T-State, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableTState[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, TState, &(EnableTState[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of T-State feature in Register Table for specified processor.
  
  This function produces entry of T-State feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
TStateReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINTN      Value;

  Enable = (BOOLEAN *) Attribute;
  if (*Enable) {
    Value = (1 << 4);
    Value |= PcdGet8 (PcdCpuClockModulationDutyCycle) & B_MSR_CLOCK_MODULATION_DUTY_CYCLE_MASK;
  } else {
    Value  = 0;
  }

  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_CLOCK_MODULATION, N_MSR_CLOCK_MODULATION_DUTY_CYCLE_START, 5, Value);
}

