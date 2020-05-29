/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for C1E Feature

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  C1e.c

**/

#include "C1e.h"

BOOLEAN    EnableC1e[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of C1E feature for specified processor.
  
  This function detects capability of C1E feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
C1eDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32                      FamilyId;
  UINT32                      ModelId;
  IA32_DESCRIPTOR             Idtr;
  UINTN                       OriginalInt13;
  INTERRUPT_GATE_DESCRIPTOR   *IdtEntry;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // For Prescott processor family, a GP-fault exception must be installed to
  // detect whether C1E is supported.
  //
  if (FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {
    
    if (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04)) {

      AcquireSpinLock (&mMPSystemData.APSerializeLock);

      //
      // Get IDT address and size.
      //
      AsmReadIdtr ((IA32_DESCRIPTOR *) &Idtr);

      IdtEntry = (INTERRUPT_GATE_DESCRIPTOR *)  ((UINTN) Idtr.Base + (EXCEPT_IA32_GP_FAULT * sizeof (INTERRUPT_GATE_DESCRIPTOR)));
      OriginalInt13 = SetIdtEntry ((UINTN) C1eExceptionHandler, IdtEntry);

      //
      // Let's try write to MSR, if it causes exception, C1E is not supported.
      //
      AsmMsrBitFieldWrite64 (MSR_IA32_MISC_ENABLE, 25, 25, 1);

      //
      // Restore the GPE exception handler.
      //
      SetIdtEntry (OriginalInt13, IdtEntry);

      //
      // Check if C1E is supported on this processor.
      //
      if (AsmMsrBitFieldRead64 (MSR_IA32_MISC_ENABLE, 25, 25) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, C1e, NULL);
      }

      ReleaseSpinLock (&mMPSystemData.APSerializeLock);
    }
  }

  //
  // For Cedar Mill processor family, the default value of IA32_MISC_ENABLE[25]
  // indicates whether C1E is supported.
  //
  if (FeaturePcdGet (PcdCpuCedarMillFamilyFlag)) {
    if (FamilyId == 0x0F && ModelId == 0x06) {
      if (AsmMsrBitFieldRead64 (MSR_IA32_MISC_ENABLE, 25, 25) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, C1e, NULL);
      }
    }
  }

  //
  // Conroe processor family supports C1E
  //
  if (FeaturePcdGet (PcdCpuConroeFamilyFlag)) {
    if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) {
      SetProcessorFeatureCapability (ProcessorNumber, C1e, NULL);
    }
  }
  
  //
  // Nehalem/Sandy Bridge/Ivy Bridge/Haswell processor support.
  //
  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC(FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, C1e, NULL);
  }
}

/**
  Configures Processor Feature Lists for C1E feature for all processors.
  
  This function configures Processor Feature Lists for C1E feature for all processors.

**/
VOID
C1eConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_C1E_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports C1E
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, C1e, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of C1E capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_C1E_BIT);
        //
        // Set the bit of C1E setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_C1E_BIT);
        }
      }

      //
      // If this logical processor supports C1E, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableC1e[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, C1e, &(EnableC1e[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of C1E feature in Register Table for specified processor.
  
  This function produces entry of C1E feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
C1eReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;
  UINT32     FamilyId;
  UINT32     ModelId;
    
  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // If Attribute is TRUE, then write 1 to MSR_IA32_MISC_ENABLE[25].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }
  
  if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) || 
      IS_IVYBRIDGE_PROC (FamilyId, ModelId)) {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_POWER_CTL,
      N_MSR_C1E_ENABLE,
      1,
      Value
      );
  } else {
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_IA32_MISC_ENABLE,
      25,
      1,
      Value
      );
  }
}
