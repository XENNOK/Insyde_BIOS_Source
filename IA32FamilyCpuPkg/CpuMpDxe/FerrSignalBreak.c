/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Signal Break on FERR# Feature

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  FerrSignalBreak.c

**/

#include "FerrSignalBreak.h"

BOOLEAN    EnableFerrSignalBreak[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of Signal Break on FERR# feature for specified processor.
  
  This function detects capability of Signal Break on FERR# feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
FerrSignalBreakDetect (
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
  // If PBE is supported, set it in context buffer.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEdx, N_CPUID_PBE_SUPPORT, N_CPUID_PBE_SUPPORT) != 0) {
    SetProcessorFeatureCapability (ProcessorNumber, FerrSignalBreak, NULL);
  }
}

/**
  Configures Processor Feature Lists for Signal Break on FERR# feature for all processors.
  
  This function configures Processor Feature Lists for Signal Break on FERR# feature for all processors.

**/
VOID
FerrSignalBreakConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_FERR_SIGNAL_BREAK_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Signal Break on FERR#
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, FerrSignalBreak, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Signal Break on FERR# capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_FERR_SIGNAL_BREAK_BIT);
        //
        // Set the bit of Signal Break on FERR# setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_FERR_SIGNAL_BREAK_BIT);
        }
      }

      //
      // If this logical processor supports Signal Break on FERR#, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableFerrSignalBreak[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, FerrSignalBreak, &(EnableFerrSignalBreak[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of Signal Break on FERR# feature in Register Table for specified processor.
  
  This function produces entry of Signal Break on FERR# feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
FerrSignalBreakReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN   *Enable;
  UINT64    Value;
  UINT32    FamilyId;
  UINT32    ModelId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
  
  //
  // If Attribute is TRUE, then write 1 to MSR_IA32_MISC_ENABLE[10].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  if (!IS_NEHALEM_PROC (FamilyId, ModelId) && !IS_SANDYBRIDGE_PROC (FamilyId, ModelId) &&
      !IS_IVYBRIDGE_PROC (FamilyId, ModelId)) {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_IA32_MISC_ENABLE,
      N_MSR_FERR_MULTIPLEXING_ENABLE,
      1,
      Value
      );
  }
}
