/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Execute Disable Bit Feature

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Xd.c

**/

#include "Xd.h"

BOOLEAN    EnableExecuteDisableBit[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of XD feature for specified processor.
  
  This function detects capability of XD feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
XdDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;

  //
  // Check whether 0x80000001 is supported by CPUID
  //
  if (GetNumberOfCpuidLeafs (ProcessorNumber, ExtendedCpuidLeaf) > 2) {
    //
    // Check CPUID(0x80000001).EDX[20]
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_EXTENDED_CPU_SIG);
    ASSERT (CpuidRegisters != NULL);

    if (BitFieldRead32 (CpuidRegisters->RegEdx, N_CPUID_XD_BIT_AVAILABLE, N_CPUID_XD_BIT_AVAILABLE) == 1) {
      SetProcessorFeatureCapability (ProcessorNumber, ExecuteDisableBit, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for XD feature for all processors.
  
  This function configures Processor Feature Lists for XD feature for all processors.

**/
VOID
XdConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               XdSupported;
  BOOLEAN               Setting;

  XdSupported = TRUE;
  Setting = FALSE;

  //
  // Check whether all logical processors support XD
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    if (!GetProcessorFeatureCapability (ProcessorNumber, ExecuteDisableBit, NULL)) {
      XdSupported = FALSE;
      break;
    }
  }
  
  if (XdSupported) {
    //
    // Set the bit of XD capability
    //
    PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_EXECUTE_DISABLE_BIT);
    //
    // Checks whether user indicates to enable XD
    //
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_EXECUTE_DISABLE_BIT) != 0) {
      //
      // Set the bit of XD setting
      //
      PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_EXECUTE_DISABLE_BIT);
      Setting = TRUE;
    }
  }

  //
  // If XD is not supported by all logical processors, or user indicates to disable XD, then disable XD on all processors.
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // The operation can only be performed on the processors supporting XD feature.
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, ExecuteDisableBit, NULL)) {
      EnableExecuteDisableBit[ProcessorNumber] = Setting;
      AppendProcessorFeatureIntoList (ProcessorNumber, ExecuteDisableBit, &(EnableExecuteDisableBit[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of XD feature in Register Table for specified processor.
  
  This function produces entry of XD feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
XdReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  //
  // If Attribute is TRUE, then write 0 to MSR_IA32_MISC_ENABLE[34].
  // Otherwise, write 1 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  WriteRegisterTable (
    ProcessorNumber,
    Msr,
    MSR_IA32_MISC_ENABLE,
    N_MSR_XD_BIT_DISABLE,
    1,
    Value
    );
}
