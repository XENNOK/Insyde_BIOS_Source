/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Max CPUID Limit Feature

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  LimitCpuIdValue.c

**/

#include "LimitCpuIdValue.h"

BOOLEAN    EnableLimitCpuIdValue[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of Max CPUID Limit feature for specified processor.
  
  This function detects capability of Max CPUID Limit feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MaxCpuidLimitDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT8               MaxCpuid;

  CpuidRegisters   = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_SIGNATURE);
  ASSERT (CpuidRegisters != NULL);

  MaxCpuid         = (UINT8) CpuidRegisters->RegEax;
  if (MaxCpuid > 3) {
    SetProcessorFeatureCapability (ProcessorNumber, MaxCpuidValueLimit, NULL);
  }
}

/**
  Configures Processor Feature Lists for Max CPUID Limit feature for all processors.
  
  This function configures Processor Feature Lists for Max CPUID Limit feature for all processors.

**/
VOID
MaxCpuidLimitConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MAX_CPUID_VALUE_LIMIT_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Max CPUID Value Limit
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, MaxCpuidValueLimit, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Max CPUID Value Limit capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_MAX_CPUID_VALUE_LIMIT_BIT);
        //
        // Set the bit of Max CPUID Value Limit setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_MAX_CPUID_VALUE_LIMIT_BIT);
        }
      }

      //
      // If this logical processor supports Max CPUID Value Limit, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableLimitCpuIdValue[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, MaxCpuidValueLimit, &(EnableLimitCpuIdValue[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of Max CPUID Limit feature in Register Table for specified processor.
  
  This function produces entry of Max CPUID Limit feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MaxCpuidLimitReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  //
  // If Attribute is TRUE, then write 1 to MSR_IA32_MISC_ENABLE[22].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  WriteRegisterTable (
    ProcessorNumber,
    Msr,
    MSR_IA32_MISC_ENABLE,
    N_MSR_LIMIT_CPUID_MAXVAL,
    1,
    Value
    );
}
