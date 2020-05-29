/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for DCA (Direct Cache Access) feature.

  Copyright (c) 2011 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Dca.c

**/

#include "Dca.h"

BOOLEAN    EnableDca[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of DCA for specified processor.
  
  This function detects capability of DCA for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
DcaDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  //
  // Processor feature flag checking
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  //
  // DCA related MSRs are available only if CPUID.(EAX=01):ECX[18]=1, which
  // is the DCA feature flag.
  //
  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_DCA_SUPPORT, N_CPUID_DCA_SUPPORT) == 1) {
    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

    if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
        (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
      SetProcessorFeatureCapability (ProcessorNumber, Dca, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for DCA for all processors.
  
  This function configures Processor Feature Lists for DCA for all processors.

**/
VOID
DcaConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_DCA_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports AES
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Dca, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of AES capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_DCA_BIT);
        //
        // Set the bit of AES setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_DCA_BIT);
        }
      }

      //
      // If this logical processor supports AES, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableDca[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, Dca, &(EnableDca[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of DCA feature in Register Table for specified processor.
  
  This function produces entry of DCA feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
DcaReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINTN      SwBlockBit;
  UINTN      Type0EnableBit;
  UINT64     CpuDcaValue;

  Enable = (BOOLEAN *) Attribute;
  if (*Enable) {
    SwBlockBit = 0;
    Type0EnableBit = 1;
  } else {
    SwBlockBit = 1;
    Type0EnableBit = 0;
  }

  CpuDcaValue = AsmReadMsr64 (MSR_IA32_CPU_DCA_CAP);
  if ((CpuDcaValue & B_MSR_DCA_TYPE0_SUPPORTED) != 0) {
    WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_DCA_0_CAP, N_MSR_SW_BLOCK, 1, SwBlockBit);
    WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_PLATFORM_DCA_CAP, N_MSR_DCA_TYPE0_ENABLE, 1, Type0EnableBit);
  }
}

