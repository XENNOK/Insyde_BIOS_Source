/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file

  Code for Enhanced Intel Speedstep Technology

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Eist.c

**/

#include "Eist.h"

GLOBAL_REMOVE_IF_UNREFERENCED
EIST_CONFIG EistConfig[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of EIST feature for specified processor.

  This function detects capability of EIST feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
EistDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINT64              Ia32MiscEnables;
  UINTN               MaxRatio;
  UINTN               MinRatio;

  //
  // Processor feature flag checking
  //
  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);

  if (BitFieldRead32 (CpuidRegisters->RegEcx, N_CPUID_EIST_SUPPORT, N_CPUID_EIST_SUPPORT) == 1) {
    //
    // Check whether ratio range is valid
    //
    MaxRatio = GET_CPU_MISC_DATA (ProcessorNumber, MaxCoreToBusRatio);
    MinRatio = GET_CPU_MISC_DATA (ProcessorNumber, MinCoreToBusRatio);

    if (MaxRatio > MinRatio) {
      SetProcessorFeatureCapability (ProcessorNumber, Eist, NULL);
    }

    //
    // Check to see if Turbo Mode is supported on this processor.
    //
    if (FeaturePcdGet (PcdCpuNehalemFamilyFlag) || FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) ||
        FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuHaswellFamilyFlag)) {
      Ia32MiscEnables = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
      CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_SIGNATURE);
      ASSERT (CpuidRegisters != NULL);
      if (CpuidRegisters->RegEax >= 6) {
        CpuidRegisters = GetProcessorCpuid (ProcessorNumber, 6);
        ASSERT (CpuidRegisters != NULL);
        //
        //  +----------------------------------------------------------------------------+
        //  | CPUID.(EAX=06):EAX[1] | MSR_IA32_MISC_ENABLE.bit[38] | Turbo Mode          |
        //  +-----------------------|------------------------------|---------------------+
        //  |          0            |       0                      |Not Available        |
        //  +-----------------------|------------------------------|---------------------+
        //  |          0            |       1                      |Available but hidden |
        //  +-----------------------|------------------------------|---------------------+
        //  |          1            |       0                      |Available and visible|
        //  +-----------------------|------------------------------|---------------------+
        //
        if ((BitFieldRead32 (CpuidRegisters->RegEax, N_CPUID_TURBO_MODE_AVAILABLE, N_CPUID_TURBO_MODE_AVAILABLE) == 1) ||
            (BitFieldRead64 (Ia32MiscEnables, N_MSR_TURBO_MODE_DISABLE, N_MSR_TURBO_MODE_DISABLE) == 1)) {
            SetProcessorFeatureCapability (ProcessorNumber, TurboMode, NULL);
        }
      }
    }
  }
}

/**
  Calculate the FVID table for all logical processors based on their CPU ID.

  @param TurboMode     Whether Turbo mode is enabled or not.
**/
VOID
CalculateFvidTable (
  IN BOOLEAN  TurboMode
  )
{
  UINTN            ProcessorNumber;
  UINTN            Index;

  UINTN            MaxRatio;
  UINTN            MinRatio;
  UINTN            RatioStepSize;
  UINTN            PreviousRatio;

  UINTN            MaxVid;
  UINTN            MinVid;
  UINTN            VidSlope;

  UINTN            NumberOfPStates;
  FVID_ENTRY       *FvidEntry;
  CPU_MISC_DATA    *CpuMiscData;

  UINT32           FamilyId;
  UINT32           ModelId;

  UINT64           PowerSlope;
  UINT64           RatioPower;
  UINTN            CurrentRatio;
  UINTN            PackageTdp;
  UINTN            CoreTdp;
  UINTN            UncoreTdp;
  UINTN            TableIndex;

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Get the core to bus ratio and VID range for the processor
    //
    CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
    MaxRatio    = CpuMiscData->MaxCoreToBusRatio;
    MinRatio    = CpuMiscData->MinCoreToBusRatio;
    MaxVid      = CpuMiscData->MaxVid;
    MinVid      = CpuMiscData->MinVid;
    PackageTdp  = CpuMiscData->PackageTdp;
    CoreTdp     = CpuMiscData->CoreTdp;

    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

    //
    // Algorithm for Prescott and Cedar Mill family
    //
    if (FeaturePcdGet (PcdCpuCedarMillFamilyFlag) || FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {
      if (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04 || ModelId == 0x06)) {
        //
        // For Pentium 4 family, we just support 2 P-states
        //
        CpuMiscData->NumberOfPStates = 2;

        //
        // Allocate memory for FVID table.
        //
        CpuMiscData->FvidTable = AllocatePool (2 * sizeof (FVID_ENTRY));
        ASSERT (CpuMiscData->FvidTable != NULL);

        //
        // Calculate the tuple for the max ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[0]);
        FvidEntry->Ratio = MaxRatio;
        FvidEntry->Vid   = MaxVid;
        FvidEntry->Power = 86 * 1000;

        //
        // Calculate the tuple for the min ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[1]);
        FvidEntry->Ratio = MinRatio;
        FvidEntry->Vid   = MinVid;
        FvidEntry->Power = ((5 * MinRatio + 5 * MinRatio) * (MinVid + 0x42) * (MinVid + 0x42) * 86 * 1000)
                           / (10 * MaxRatio * (MaxVid + 0x42) * (MaxVid + 0x42));
      }
    }

    //
    // Algorithm for Core 2 family
    //
    if (FeaturePcdGet (PcdCpuConroeFamilyFlag)) {
      if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) {
        //
        // Calculate the vid slope multiply by 1000 to maintain accuracy
        // Later on, add 999 and divide by 1000 to get the ceiling
        //
        VidSlope = (MaxVid - MinVid) * 1000 / (MaxRatio - MinRatio);

        //
        // Determine the step
        //
        if ((MaxRatio - MinRatio) == 1) {
          RatioStepSize = 1;
        } else {
          RatioStepSize = 2;
        }

        //
        // Determine the number of P-states suppored by processor
        //
        NumberOfPStates = (MaxRatio - MinRatio) / RatioStepSize + 1;

        CpuMiscData->NumberOfPStates = NumberOfPStates;

        //
        // Allocate memory for FVID table.
        //
        CpuMiscData->FvidTable = AllocatePool (NumberOfPStates * sizeof (FVID_ENTRY));
        ASSERT(CpuMiscData->FvidTable != NULL);

        //
        // Calculate the tuple for the max ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[0]);
        FvidEntry->Ratio = MaxRatio;
        FvidEntry->Vid   = MaxVid;
        FvidEntry->Power = 65 * 1000;

        //
        // Calculate the tuple for the min ratio state
        //
        FvidEntry = &(CpuMiscData->FvidTable[NumberOfPStates - 1]);
        FvidEntry->Ratio = MinRatio;
        FvidEntry->Vid   = MinVid;
        FvidEntry->Power = 32 * 1000;

        PreviousRatio = MinRatio;

        for (Index = NumberOfPStates - 2; Index > 0; Index--) {
          //
          // Calculate for the FVID tuple
          //
          FvidEntry = &(CpuMiscData->FvidTable[Index]);

          FvidEntry->Ratio = PreviousRatio + RatioStepSize;
          FvidEntry->Vid   = ((VidSlope * (FvidEntry->Ratio - MinRatio)) + 999) / 1000 + MinVid;
          //
          // If the calculated VID is greater than the max VID, then use the max VID
          //
          if (FvidEntry->Vid > MaxVid) {
            FvidEntry->Vid = MaxVid;
          }
          FvidEntry->Power = (FvidEntry->Ratio - MinRatio) * (65000 - 32000) / (MaxRatio - MinRatio) + 32000;

          PreviousRatio    = FvidEntry->Ratio;
        }
      }
    }

    //
    // Nehalem based processor support
    //
    if (FeaturePcdGet (PcdCpuNehalemFamilyFlag)) {
      if (IS_NEHALEM_PROC (FamilyId, ModelId)) {
        //
        // Set step size for this processor type.  Currently only a step size of
        // one is supported.
        //
        RatioStepSize = 1;

        //
        // Determine the Uncore TDP based on Package and Core information.
        //
        UncoreTdp = PackageTdp - CoreTdp;

        //
        // Compute the number of P-States.
        //
        CpuMiscData->NumberOfPStates = ((MaxRatio - MinRatio) / RatioStepSize) + 1;
        if (TurboMode) {
          CpuMiscData->NumberOfPStates += 1;
        }

        //
        // Allocate buffer to put P-State information in.
        //
        CpuMiscData->FvidTable = AllocatePool (CpuMiscData->NumberOfPStates * sizeof (FVID_ENTRY));
        ASSERT(CpuMiscData->FvidTable != NULL);

        //
        // Fill in Turbo Mode information if enabled.
        //
        TableIndex = 0;
        if (TurboMode) {
          CpuMiscData->FvidTable[TableIndex].Ratio = MaxRatio + 1;
          CpuMiscData->FvidTable[TableIndex].Vid = 0;
          CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
          CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;
          CpuMiscData->FvidTable[TableIndex].Power = PackageTdp;
          TableIndex++;
        }

        //
        // Fill in ratio information for non-Turbo ratios.
        //
        CurrentRatio = MaxRatio;
        for (;TableIndex <CpuMiscData->NumberOfPStates; TableIndex++, CurrentRatio--) {
          //
          // Fill in ratio data.  VID is ignored on these processors as it is controlled
          // by the PCU.
          //
          CpuMiscData->FvidTable[TableIndex].Ratio = CurrentRatio;
          CpuMiscData->FvidTable[TableIndex].Vid = 0;

          //
          // Processor specific latency information from BWG.
          //
          CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
          CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;

          //
          // Compute power slope.  Floating point math done using fixed point
          // computations (5 decimal places).
          //
          // M = ((1.1 - (MaxNonTurboRatio - Ratio) 0.00625) / 1.1)^2
          //
          PowerSlope = 110000 - (MultU64x32 ((MaxRatio - CurrentRatio), 625));
          PowerSlope = DivU64x32 (MultU64x32 (PowerSlope, 100), 110);
          PowerSlope = DivU64x32 (MultU64x64 (PowerSlope, PowerSlope), 100000);

          //
          // Compute power for this ratio.
          //
          RatioPower = DivU64x64Remainder (MultU64x32 (CurrentRatio, 100), MaxRatio, NULL);
          RatioPower = DivU64x32 (MultU64x64 (RatioPower, PowerSlope), 100);
          RatioPower = DivU64x32 (MultU64x64 (RatioPower, CoreTdp), 100000) + UncoreTdp;
          CpuMiscData->FvidTable[TableIndex].Power = (UINTN) RatioPower;
        }
      }
    }

    //
    // Sandy Bridge/Ivy Bridge/Haswell based processor support
    //
    if (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuHaswellFamilyFlag)) {
      if (IS_SANDYBRIDGE_PROC (FamilyId, ModelId) || IS_IVYBRIDGE_PROC (FamilyId, ModelId) || IS_HASWELL_PROC (FamilyId, ModelId)) {

        RatioStepSize = 1;

        //
        // Compute the number of P-States.
        //
        CpuMiscData->NumberOfPStates = ((MaxRatio - MinRatio) / RatioStepSize) + 1;
        if (TurboMode) {
          CpuMiscData->NumberOfPStates += 1;
        }

        //
        // The ACPI specification strictly recommends no more than 16 performance states, so increase
        // the step size in this case.
        //
        while (CpuMiscData->NumberOfPStates > 16) {
          RatioStepSize += 1;
          CpuMiscData->NumberOfPStates = ((MaxRatio - MinRatio) / RatioStepSize) + 1;
          if (TurboMode) {
            CpuMiscData->NumberOfPStates += 1;
          }
        }
        
        //
        // Allocate buffer to put P-State information in.
        //
        CpuMiscData->FvidTable = AllocatePool (CpuMiscData->NumberOfPStates * sizeof (FVID_ENTRY));
        ASSERT(CpuMiscData->FvidTable != NULL);

        //
        // According to BWG,
        // 1. If Turbo Mode is available and enabled, P(0) = Max Turbo Ratio, P(1) = Max Non-Turbo Ratio,
        //   else P(0) = Max Non-Turbo Ratio and P(1) is computed below based on RatioStepSize.
        // 2. P(n) = Min Non-Turbo Ration, and P(n-1) = P(n) + RatioStepSize.
        //

        //
        // Fill in Turbo Mode information if enabled.
        //
        Index = 0;
        if (TurboMode) {
          //
          // If the Turbo Mode is available and enabled, then the maximum performance state is
          // represented as read from the TURBO_RATIO_LIMIT MSR bits [7:0].
          //
          CpuMiscData->FvidTable[Index].Ratio = CpuMiscData->MaxTurboRatio;
          //
          // Operating Point Protection is always enabled on Sandy Bridge processors. The end result is
          // that BIOS can not configure the processor VID.
          //
          CpuMiscData->FvidTable[Index].Vid = 0;
          //
          // Processor specific latency information from BWG.
          //
          CpuMiscData->FvidTable[Index].TransitionLatency = 10;
          CpuMiscData->FvidTable[Index].BusMasterLatency = 10;
          CpuMiscData->FvidTable[Index].Power = PackageTdp;
          Index++;
        }

        //
        // TO-DO: Get factory-configured maximum Non-Turbo Ratio if FLEX RATIO is enabled.
        //

        //
        // Fill in ratio information for non-Turbo ratios.
        //

        CpuMiscData->FvidTable[Index].Ratio = MaxRatio;
        //
        // Operating Point Protection is always enabled on Sandy Bridge processors. The end result is
        // that BIOS can not configure the processor VID.
        //
        CpuMiscData->FvidTable[Index].Vid = 0;
        //
        // Processor specific latency information from BWG.
        //
        CpuMiscData->FvidTable[Index].TransitionLatency = 10;
        CpuMiscData->FvidTable[Index].BusMasterLatency = 10;
        CpuMiscData->FvidTable[Index].Power = PackageTdp;
        Index++;

        CurrentRatio = MinRatio;
        for (TableIndex = CpuMiscData->NumberOfPStates - 1; TableIndex >= Index; TableIndex--, CurrentRatio += RatioStepSize) {
          //
          // Fill in ratio data.  
          //
          CpuMiscData->FvidTable[TableIndex].Ratio = CurrentRatio;
          //
          // Operating Point Protection is always enabled on Sandy Bridge processors. The end result is
          // that BIOS can not configure the processor VID.
          //
          CpuMiscData->FvidTable[TableIndex].Vid = 0;

          //
          // Processor specific latency information from BWG.
          //
          CpuMiscData->FvidTable[TableIndex].TransitionLatency = 10;
          CpuMiscData->FvidTable[TableIndex].BusMasterLatency = 10;

          //
          // Compute power slope.  Floating point math done using fixed point
          // computations (5 decimal places).
          //
          // M = ((1.1 - (P1_ratio - Ratio) * 0.00625) / 1.1)^2
          //
          PowerSlope = 110000 - (MultU64x32 ((MaxRatio - CurrentRatio), 625));
          PowerSlope = DivU64x32 (MultU64x32 (PowerSlope, 100), 110);
          PowerSlope = DivU64x32 (MultU64x64 (PowerSlope, PowerSlope), 100000);

          //
          // pstate_power[ratio]=((ratio/P1_ratio)*M*TDP)
          //
          RatioPower = DivU64x64Remainder (MultU64x32 (CurrentRatio, 100), MaxRatio, NULL);
          RatioPower = DivU64x32 (MultU64x64 (RatioPower, PowerSlope), 100);
          RatioPower = DivU64x32 (MultU64x64 (RatioPower, PackageTdp), 100000);
          CpuMiscData->FvidTable[TableIndex].Power = (UINTN) RatioPower;
        }
      }
    }
  }

  //
  // Print debug info for FVID table.
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    DEBUG ((EFI_D_INFO, "Proc: %8d\n", ProcessorNumber));
    CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
    NumberOfPStates = CpuMiscData->NumberOfPStates;
    for (Index = 0; Index < NumberOfPStates; Index++) {
      FvidEntry = &(CpuMiscData->FvidTable[Index]);
      DEBUG ((EFI_D_INFO, "  State: %8d\n", Index));
      DEBUG ((EFI_D_INFO, "    Ratio: %8d\n", FvidEntry->Ratio));
      DEBUG ((EFI_D_INFO, "    VID:   %8d\n", FvidEntry->Vid));
      DEBUG ((EFI_D_INFO, "    Power: %8d\n", FvidEntry->Power));
    }
  }
}

/**
  Configures Processor Feature Lists for EIST feature for all processors.

  This function configures Processor Feature Lists for EIST feature for all processors.

**/
VOID
EistConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               EistSupported;
  BOOLEAN               TurboSupported;
  BOOLEAN               EistSetting;
  BOOLEAN               TurboSetting;
  BOOLEAN               HwCoordinationSetting;

  EistSupported = TRUE;
  TurboSupported = TRUE;
  EistSetting = FALSE;
  TurboSetting = FALSE;
  HwCoordinationSetting = FALSE;

  //
  // Check whether all logical processors support EIST
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    if (!GetProcessorFeatureCapability (ProcessorNumber, Eist, NULL)) {
      EistSupported = FALSE;
      break;
    }
    if (!GetProcessorFeatureCapability (ProcessorNumber, TurboMode, NULL)) {
      TurboSupported = FALSE;
    }
  }

  if (EistSupported) {
    //
    // Set the bit of EIST capability
    //
    PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_EIST_BIT);
    //
    // Set the bit of Turbo Mode capability
    //
    if (TurboSupported) {
      PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_TURBO_MODE_BIT);
    }

    //
    // Checks whether user indicates to enable EIST
    //
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_EIST_BIT) != 0) {
      //
      // Set the bit of current setting for EIST
      //
      PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_EIST_BIT);
      EistSetting = TRUE;

      //
      // Set the bit of current setting for Turbo Mode
      //
      if (TurboSupported && (PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_TURBO_MODE_BIT) != 0) {
        PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_TURBO_MODE_BIT);
        TurboSetting = TRUE;
      }

      HwCoordinationSetting = PcdGetBool (PcdCpuHwCoordination);

      //
      // Calculate the FVID table for all logical processors.
      //
      CalculateFvidTable (TurboSetting);

    }
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    EistConfig[ProcessorNumber].EnableEist = EistSetting;
    EistConfig[ProcessorNumber].EnableTurbo = TurboSetting;
    EistConfig[ProcessorNumber].EnableHwCoordination = HwCoordinationSetting;
    AppendProcessorFeatureIntoList (ProcessorNumber, Eist, &(EistConfig[ProcessorNumber]));
  }
}

/**
  Produces entry of EIST feature in Register Table for specified processor.

  This function produces entry of EIST feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
EistReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  EIST_CONFIG  *Enable;
  UINT64       Value;
  UINT32       FamilyId;
  UINT32       ModelId;
  UINT64       Ia32MiscEnables;

  //
  // If EIST is TRUE, then write 1 to MSR_IA32_MISC_ENABLE[16].
  // Otherwise, write 0 to the bit.
  //
  Enable = (EIST_CONFIG *) Attribute;
  Value  = 0;
  if (Enable->EnableEist) {
    Value = 1;
  }

  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_EIST_ENABLE, 1, Value);

  if ((PcdGet32 (PcdCpuProcessorFeatureCapability) & PCD_CPU_TURBO_MODE_BIT) != 0) {
    Value  = 1;
    if (Enable->EnableTurbo) {
      Value = 0;
    }
    Ia32MiscEnables = AsmReadMsr64 (MSR_IA32_MISC_ENABLE);
    if (BitFieldRead64 (Ia32MiscEnables, N_MSR_TURBO_MODE_DISABLE, N_MSR_TURBO_MODE_DISABLE) != Value) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_TURBO_MODE_DISABLE, 1, Value);
    }
  }

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

  //
  // For Conroe family, we need to set EIST Select Lock bit after programming EIST
  //
  if (FeaturePcdGet (PcdCpuConroeFamilyFlag)) {
    if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16 || ModelId == 0x17)) {
      WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_EIST_SELECT_LOCK, 1, 1);
    }
  }

  //
  // HW Coordination programming for Nehalem processors.
  //
  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    Value  = 1;
    if (Enable->EnableHwCoordination) {
      Value = 0;
    }
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_MISC_PWR_MGMT,
      N_MSR_EIST_HW_COORDINATION_DISABLE,
      1,
      Value
      );
  }
}
