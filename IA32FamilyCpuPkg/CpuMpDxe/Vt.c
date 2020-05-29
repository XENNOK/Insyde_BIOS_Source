/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Vanderpool Technology and LaGrande Technology

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Vt.c

**/

#include "Vt.h"

BOOLEAN   LtSupported[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
UINT64    MsrValue[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of VT and LT feature for specified processor.
  
  This function detects capability of VT and LT feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
VtLtDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;

  //
  // Check CPUID(1).ECX[5] for VT capability
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_VMX_SUPPORT, N_CPUID_VMX_SUPPORT) == 1) {
    //
    // If VT is supported, check CPUID(1).ECX[6] for LT capability
    //
    if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_SMX_SUPPORT, N_CPUID_SMX_SUPPORT) == 1) {
      LtSupported[ProcessorNumber] = TRUE;
    }

    SetProcessorFeatureCapability (ProcessorNumber, Vt, &LtSupported[ProcessorNumber]);

    //
    // Get the current value of Feature Control MSR
    //
    MsrValue[ProcessorNumber] = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  }
}

/**
  Configures Processor Feature Lists for VT and LT feature for all processors.
  
  This function configures Processor Feature Lists for VT and LT feature for all processors.

**/
VOID
VtLtConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               VtSupported;
  BOOLEAN               LtSupported;
  BOOLEAN               *Attribute;
  UINT64                NewMsrValue;

  VtSupported = TRUE;
  LtSupported = TRUE;

  //
  // Check whether all logical processors support VT
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    if (!GetProcessorFeatureCapability (ProcessorNumber, Vt, (VOID **) &Attribute)) {
      //
      // If VT is not supported by this processor, then VT will be disabled on all logical processors.
      // Therefore, LT will also be disabled on all logical processors.
      //
      VtSupported = FALSE;
      LtSupported = FALSE;
      break;
    }
    if (LtSupported) {
      //
      // If LT is not supported by this processor, then LT will be disabled on all logical processors.
      //
      if (!*Attribute) {
        LtSupported = FALSE;
      }
    }
  }

  //
  // The new value would be assigned to all Feature Control MSRs that are not locked.
  // At lease, it would be locked.
  //
  NewMsrValue = 1;
  
  if (VtSupported) {
    //
    // Set the bit of VT capability
    //
    PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_VT_BIT);

    if (LtSupported) {
      //
      // Set the bit of LT capability
      //
      PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_LT_BIT);
    }

    //
    // Check whether MSR is locked. If locked, we cannot modify Feature Control MSR, and just use current
    // status as the final configuration status.
    // Since VtSupported is TRUE, all logical processors support VT and have Feature Control MSR.
    // Since Feature Control MSR setting is uniform, so we just check the status of processor 0.
    //
    if (BitFieldRead64(MsrValue[0], N_MSR_FEATURE_CONTROL_LOCK, N_MSR_FEATURE_CONTROL_LOCK) == 1) {
      if (BitFieldRead64(MsrValue[0], N_MSR_ENABLE_VMX_OUTSIDE_SMX, N_MSR_ENABLE_VMX_OUTSIDE_SMX) == 1) {
        PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_VT_BIT);
      }
      if (BitFieldRead64(MsrValue[0], N_MSR_ENABLE_VMX_INSIDE_SMX, N_MSR_ENABLE_VMX_INSIDE_SMX) == 1) {
        PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_LT_BIT);
      }
    } else {
      //
      // If not locked, checkes whether user indicates to enable VT.
      //
      if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_VT_BIT) != 0) {
        //
        // Set bit 2 of new MSR value
        //
        NewMsrValue = BitFieldWrite64 (NewMsrValue, N_MSR_ENABLE_VMX_OUTSIDE_SMX, N_MSR_ENABLE_VMX_OUTSIDE_SMX, 1);
        //
        // Set the bit of current setting for VT
        //
        PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_VT_BIT);
        //
        // After VT has been enabled, check if LT can be enabled.
        //
        if (LtSupported && ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_LT_BIT) != 0)) {
          //
          // Set bit 1 and clear bit 2 to enable SMX
          //
          NewMsrValue = BitFieldWrite64 (NewMsrValue, N_MSR_ENABLE_VMX_INSIDE_SMX, N_MSR_ENABLE_VMX_OUTSIDE_SMX, 0x01);
          //
          // Set bit field 8:15 of MSR_IA32_FEATURE_CONTROL for LT
          //
          NewMsrValue = BitFieldWrite64 (NewMsrValue, N_MSR_SENTER_LOCAL_FUNC_ENABLE_START, N_MSR_SENTER_GLOBAL_ENABLE, 0xff);
          //
          // Set the bit of current setting for LT
          //
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_LT_BIT);
        }
      }
    }    
  }

  //
  // For all logical processors supporting VT whose Feature Control MSR is not locked, we must operate on it.
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    if (GetProcessorFeatureCapability (ProcessorNumber, Vt, NULL)) {
      if (BitFieldRead64(MsrValue[ProcessorNumber], N_MSR_FEATURE_CONTROL_LOCK, N_MSR_FEATURE_CONTROL_LOCK) == 0) {
        MsrValue[ProcessorNumber] = NewMsrValue;
        AppendProcessorFeatureIntoList (ProcessorNumber, Vt, &(MsrValue[ProcessorNumber]));
      }
    }
  }
}

/**
  Produces entry of VT and LT feature in Register Table for specified processor.
  
  This function produces entry of VT and LT feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
VtLtReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  UINT64    *Value;

  Value = (UINT64 *) Attribute;

  WriteRegisterTable (
    ProcessorNumber,
    Msr,
    MSR_IA32_FEATURE_CONTROL,
    0,
    63,
    *Value
    );
}
