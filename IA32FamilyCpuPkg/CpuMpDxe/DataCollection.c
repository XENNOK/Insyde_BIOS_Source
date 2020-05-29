/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
  Code for Data Collection phase.

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  DataCollection.c

**/

#include "Cpu.h"
#include "Feature.h"
#include "ProgramBeforeReset.h"
#include "Microcode.h"

BOOLEAN    HtCapable  = FALSE;
BOOLEAN    CmpCapable = FALSE;

/**
  Collects Local APIC data of the processor.

  This function collects Local APIC base, verion, and APIC ID of the processor.

  @param  ProcessorNumber    Handle number of specified logical processor

**/
VOID
CollectApicData (
  UINTN    ProcessorNumber
  )
{
  CPU_MISC_DATA          *CpuMiscData;
  UINT32                 LocalApicBaseAddress;

  CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;

  LocalApicBaseAddress = PcdGet32 (PcdCpuLocalApicBaseAddress);
  CpuMiscData->ApicBase    = LocalApicBaseAddress;
  //
  // Read bits 0..7 of Local APIC Version Register for Local APIC version.
  //
  CpuMiscData->ApicVersion = GetApicVersion () & 0xff;
  //
  // Read bits 24..31 of Local APIC ID Register for Local APIC ID
  //
  CpuMiscData->ApicID        = GetApicId ();
  CpuMiscData->InitialApicID = GetInitialApicId ();
}

/**
  Collects all CPUID leafs the processor.

  This function collects all CPUID leafs the processor.

  @param  ProcessorNumber    Handle number of specified logical processor

**/
VOID
CollectCpuidLeafs (
  UINTN    ProcessorNumber
  )
{
  CPU_COLLECTED_DATA   *CpuCollectedData;
  EFI_CPUID_REGISTER   *CpuidRegisters;
  UINT32               Index;

  CpuCollectedData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber];
  //
  // Collect basic CPUID information.
  //
  CpuidRegisters = CpuCollectedData->CpuidData.CpuIdLeaf;
  for (Index = 0; Index < GetNumberOfCpuidLeafs (ProcessorNumber, BasicCpuidLeaf); Index++) {
    AsmCpuid (
      Index,
      &CpuidRegisters->RegEax,
      &CpuidRegisters->RegEbx,
      &CpuidRegisters->RegEcx,
      &CpuidRegisters->RegEdx
      );
    CpuidRegisters++;
  }

  //
  // Collect extended function CPUID information.
  //
  for (Index = 0; Index < GetNumberOfCpuidLeafs (ProcessorNumber, ExtendedCpuidLeaf); Index++) {
    AsmCpuid (
      Index + 0x80000000,
      &CpuidRegisters->RegEax,
      &CpuidRegisters->RegEbx,
      &CpuidRegisters->RegEcx,
      &CpuidRegisters->RegEdx
      );
    CpuidRegisters++;
  }

  //
  // Collect additional Cache & TLB information, if exists.
  //
  for (Index = 1; Index < GetNumberOfCpuidLeafs (ProcessorNumber, CacheAndTlbCpuidLeafs); Index++) {
    AsmCpuid (
      2,
      &CpuidRegisters->RegEax,
      &CpuidRegisters->RegEbx,
      &CpuidRegisters->RegEcx,
      &CpuidRegisters->RegEdx
      );
    CpuidRegisters++;
  }

  //
  // Collect Deterministic Cache Parameters Leaf.
  //
  for (Index = 0; Index < GetNumberOfCpuidLeafs (ProcessorNumber, DeterministicCacheParametersCpuidLeafs); Index++) {
    AsmCpuidEx (
      4,
      Index,
      &CpuidRegisters->RegEax,
      &CpuidRegisters->RegEbx,
      &CpuidRegisters->RegEcx,
      &CpuidRegisters->RegEdx
      );
    CpuidRegisters++;
  }

  //
  // Collect Extended Topology Enumeration Leaf.
  //
  for (Index = 0; Index < GetNumberOfCpuidLeafs (ProcessorNumber, ExtendedTopologyEnumerationCpuidLeafs); Index++) {
    AsmCpuidEx (
      EFI_CPUID_CORE_TOPOLOGY,
      Index,
      &CpuidRegisters->RegEax,
      &CpuidRegisters->RegEbx,
      &CpuidRegisters->RegEcx,
      &CpuidRegisters->RegEdx
      );
    CpuidRegisters++;
  }
}

/**
  Collects physical location of the processor.

  This function gets Package ID/Core ID/Thread ID of the processor.

  The algorithm below assumes the target system has symmetry across physical package boundaries
  with respect to the number of logical processors per package, number of cores per package.

  @param  InitialApicId  Initial APIC ID for determing processor topology.
  @param  Location       Pointer to EFI_CPU_PHYSICAL_LOCATION structure.
  @param  ThreadIdBits   Number of bits occupied by Thread ID portion.
  @param  CoreIdBits     Number of bits occupied by Core ID portion.

**/
VOID
ExtractProcessorLocation (
  IN  UINT32                    InitialApicId,
  OUT EFI_CPU_PHYSICAL_LOCATION *Location,
  OUT UINTN                     *ThreadIdBits,
  OUT UINTN                     *CoreIdBits
  )
{
  BOOLEAN  TopologyLeafSupported;
  UINTN    ThreadBits;
  UINTN    CoreBits;
  UINT32   RegEax;
  UINT32   RegEbx;
  UINT32   RegEcx;
  UINT32   RegEdx;
  UINT32   MaxCpuIdIndex;
  UINT32   SubIndex;
  UINTN    LevelType;
  UINT32   MaxLogicProcessorsPerPackage;
  UINT32   MaxCoresPerPackage;

  //
  // Check if the processor is capable of supporting more than one logical processor.
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx);
  if ((RegEdx & BIT28) == 0) {
    Location->Thread  = 0;
    Location->Core    = 0;
    Location->Package = 0;
    *ThreadIdBits     = 0;
    *CoreIdBits       = 0;
    return;
  }

  ThreadBits = 0;
  CoreBits = 0;
  
  //
  // Assume three-level mapping of APIC ID: Package:Core:SMT.
  //

  TopologyLeafSupported = FALSE;
  //
  // Get the max index of basic CPUID
  //
  AsmCpuid (EFI_CPUID_SIGNATURE, &MaxCpuIdIndex, NULL, NULL, NULL);

  //
  // If the extended topology enumeration leaf is available, it
  // is the preferred mechanism for enumerating topology.
  //
  if (MaxCpuIdIndex >= EFI_CPUID_EXTENDED_TOPOLOGY) {
    AsmCpuidEx (EFI_CPUID_EXTENDED_TOPOLOGY, 0, &RegEax, &RegEbx, &RegEcx, NULL);
    //
    // If CPUID.(EAX=0BH, ECX=0H):EBX returns zero and maximum input value for
    // basic CPUID information is greater than 0BH, then CPUID.0BH leaf is not 
    // supported on that processor.
    //
    if (RegEbx != 0) {
      TopologyLeafSupported = TRUE;

      //
      // Sub-leaf index 0 (ECX= 0 as input) provides enumeration parameters to extract 
      // the SMT sub-field of x2APIC ID.
      //
      LevelType = (RegEcx >> 8) & 0xff;
      ASSERT (LevelType == EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_SMT);
      ThreadBits = RegEax & 0x1f;

      //
      // Software must not assume any "level type" encoding 
      // value to be related to any sub-leaf index, except sub-leaf 0.
      //
      SubIndex = 1;
      do {
        AsmCpuidEx (EFI_CPUID_EXTENDED_TOPOLOGY, SubIndex, &RegEax, NULL, &RegEcx, NULL);
        LevelType = (RegEcx >> 8) & 0xff;
        if (LevelType == EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_CORE) {
          CoreBits = (RegEax & 0x1f) - ThreadBits;
          break;
        }
        SubIndex++;
      } while (LevelType != EFI_CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID);
    }
  }

  if (!TopologyLeafSupported) {
    AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, &RegEbx, NULL, NULL);
    MaxLogicProcessorsPerPackage = (RegEbx >> 16) & 0xff;
    if (MaxCpuIdIndex >= EFI_CPUID_CACHE_PARAMS) {
      AsmCpuidEx (EFI_CPUID_CACHE_PARAMS, 0, &RegEax, NULL, NULL, NULL);
      MaxCoresPerPackage = (RegEax >> 26) + 1;
    } else {
      //
      // Must be a single-core processor.
      //
      MaxCoresPerPackage = 1;
    }

    ThreadBits = (UINTN) (HighBitSet32 (MaxLogicProcessorsPerPackage / MaxCoresPerPackage - 1) + 1);
    CoreBits = (UINTN) (HighBitSet32 (MaxCoresPerPackage - 1) + 1);
  }

  Location->Thread  = InitialApicId & ~((-1) << ThreadBits);
  Location->Core    = (InitialApicId >> ThreadBits) & ~((-1) << CoreBits);
  Location->Package = (InitialApicId >> (ThreadBits+ CoreBits));
  *ThreadIdBits     = ThreadBits;
  *CoreIdBits       = CoreBits;
}

/**
  Collects physical location of the processor.

  This function collects physical location of the processor.

  @param  ProcessorNumber    Handle number of specified logical processor

**/
VOID
CollectProcessorLocation (
  UINTN    ProcessorNumber
  )
{
  CPU_COLLECTED_DATA  *CpuCollectedData;
  UINT32              InitialApicID;
  UINTN               ThreadIdBits;
  UINTN               CoreIdBits;

  CpuCollectedData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber];
  InitialApicID    = CpuCollectedData->CpuMiscData.InitialApicID;

  ExtractProcessorLocation (
    InitialApicID,
    &CpuCollectedData->ProcessorLocation,
    &ThreadIdBits,
    &CoreIdBits
    );

  CpuCollectedData->PackageIdBitOffset = (UINT8)(ThreadIdBits+ CoreIdBits);

  //
  // Check CMP and HT capabilities
  //
  if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
    if (CoreIdBits > 0) {
      CmpCapable = TRUE;
    }
    if (ThreadIdBits > 0) {
      HtCapable = TRUE;
    }
  }
}

/**
  Collects intended FSB frequency and core to bus ratio.

  This function collects intended FSB frequency and core to bus ratio.

  @param  ProcessorNumber    Handle number of specified logical processor

**/
VOID
CollectFrequencyData (
  UINTN    ProcessorNumber
  )
{
  CPU_MISC_DATA          *CpuMiscData;
  UINT64                 QWord;
  UINT8                  FsbFrequencyEncoding;
  UINT32                 FamilyId;
  UINT32                 ModelId;
  UINT32                 SteppingId;
  BOOLEAN                OldInterruptState;
  UINT64                 BeginValue;
  UINT64                 EndValue;
  UINT64                 ActualFrequency;
  UINT64                 ActualFsb;
  UINT64                 Tmp64;
  BOOLEAN                XeCapableProcessor;

  CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
  CpuMiscData->FrequencyLocked = FALSE;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  FsbFrequencyEncoding = 0xff;

  switch (FamilyId) {
  case 0x0F:
    switch (ModelId) {
    case 0x03:
    case 0x04:
    case 0x06:
      //
      // Support for Pentium 4 processor family
      //
      if (FeaturePcdGet (PcdCpuCedarMillFamilyFlag) || FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {
        QWord = AsmReadMsr64 (EFI_MSR_EBC_FREQUENCY_ID);
        //
        // Collect intended FSB frequency
        //
        FsbFrequencyEncoding = (UINT8) BitFieldRead64 (QWord, N_MSR_SCALABLE_BUS_SPEED_START, N_MSR_SCALABLE_BUS_SPEED_STOP);

        //
        // Check whether frequency is locked
        //
        if (BitFieldRead64 (QWord, 20, 20) != 0) {
          CpuMiscData->FrequencyLocked = TRUE;
        }

        //
        // Collect core to bus ratio
        //
        CpuMiscData->MaxCoreToBusRatio = (UINTN) BitFieldRead64 (QWord, 0, 7);
        Tmp64 = BitFieldRead64 (QWord, 0, 7);
        CpuMiscData->MinCoreToBusRatio = (UINTN) (Tmp64 - BitFieldRead64 (QWord, 8, 15));

        //
        // Collect VID
        //
        QWord = AsmReadMsr64 (EFI_MSR_IA32_PERF_STS);
        CpuMiscData->MaxVid = (UINTN) BitFieldRead64 (QWord, 32, 39);
        QWord = AsmReadMsr64 (EFI_MSR_GV_THERM);
        CpuMiscData->MinVid = (UINTN) BitFieldRead64 (QWord, 0, 7);
      }
      break;

    default:
      break;
    }
    break;

  case 0x06:
    switch (ModelId) {
    case 0x0F:
    case 0x16:
      //
      // Support for Conroe family
      //
      if (FeaturePcdGet (PcdCpuConroeFamilyFlag)) {
        QWord = AsmReadMsr64 (EFI_MSR_PSB_CLOCK_STATUS);
        FsbFrequencyEncoding = (UINT8) BitFieldRead64 (QWord, 0, 2);

        //
        // Collect core to bus ratio and VID
        //
        QWord = AsmReadMsr64 (EFI_MSR_GV_THERM);
        CpuMiscData->MinCoreToBusRatio =  (UINTN) BitFieldRead64 (QWord, 8, 12);
        CpuMiscData->MinVid            = (UINTN) BitFieldRead64 (QWord, 0, 5);
        QWord = AsmReadMsr64 (EFI_MSR_IA32_PERF_STS);
        CpuMiscData->MaxCoreToBusRatio = (UINTN) BitFieldRead64 (QWord, 56, 60);
        CpuMiscData->MaxVid            = (UINTN) BitFieldRead64 (QWord, 48, 53);
        QWord = AsmReadMsr64 (EFI_MSR_IA32_PLATFORM_ID);
        if (BitFieldRead64 (QWord, 15, 15) != 0 ) {
          CpuMiscData->FrequencyLocked = TRUE;
        }
        break;
      }
      break;

    case BLOOMFIELD_MODEL_ID:
    case LYNNFIELD_MODEL_ID:
    case CLARKDALE_MODEL_ID:
      if (FeaturePcdGet (PcdCpuNehalemFamilyFlag)) {
        //
        // Force 133MHz BCLK value.
        //
        FsbFrequencyEncoding = 1;

        //
        // Get the minimum and maximum (non-turbo) core ratios.
        //
        QWord = AsmReadMsr64 (MSR_PLATFORM_INFO);
        CpuMiscData->MinCoreToBusRatio = (UINTN) BitFieldRead64 (QWord, N_MSR_MAX_EFFICIENCY_RATIO_START, N_MSR_MAX_EFFICIENCY_RATIO_STOP);
        CpuMiscData->MaxCoreToBusRatio = (UINTN) BitFieldRead64 (QWord, N_MSR_MAX_NON_TURBO_RATIO_START, N_MSR_MAX_NON_TURBO_RATIO_STOP);
        XeCapableProcessor = (BOOLEAN) ((QWord & B_MSR_PROGRAMMABLE_TDP_LIMIT_TURBO_MODE) == B_MSR_PROGRAMMABLE_TDP_LIMIT_TURBO_MODE);

        //
        // Get power scaling information.  Just set some default values that assume
        // a high power part.  Some early Bloomfield processors did not allow
        // access to this register if they were not XE processors.
        //
        CpuMiscData->PackageTdp = 130;
        CpuMiscData->CoreTdp = 95;
        if (XeCapableProcessor || !(ModelId == BLOOMFIELD_MODEL_ID && SteppingId < 2)) {
          QWord = AsmReadMsr64 (MSR_TURBO_POWER_CURRENT_LIMIT);
          CpuMiscData->PackageTdp = (UINTN) RShiftU64 (BitFieldRead64 (QWord, N_MSR_TDP_LIMIT_START, N_MSR_TDP_LIMIT_STOP), 3);
          CpuMiscData->CoreTdp = (UINTN) RShiftU64 (BitFieldRead64 (QWord, N_MSR_TDC_LIMIT_START, N_MSR_TDC_LIMIT_STOP), 3);
        }

        //
        // Check to see if this is a locked processor.
        //
        if ((QWord & B_MSR_PROGRAMMABLE_RATIO_LIMIT_TURBO_MODE) == 0) {
          CpuMiscData->FrequencyLocked = TRUE;
        }
      }
      break;

    case SANDYBRIDGE_CLIENT_MODEL_ID:
    case SANDYBRIDGE_SERVER_MODEL_ID:
    case IVYBRIDGE_CLIENT_MODEL_ID:
    case IVYBRIDGE_SERVER_MODEL_ID:
    case HASWELL_CLIENT_MODEL_ID:
    case HASWELL_SERVER_MODEL_ID:

      if (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) || FeaturePcdGet (PcdCpuHaswellFamilyFlag)) {
        //
        // Force 100MHz BCLK value.
        //
        FsbFrequencyEncoding = 5;

        //
        // Get the minimum and maximum non-turbo core ratios and maximum turbo ratio.
        //
        QWord = AsmReadMsr64 (MSR_PLATFORM_INFO);
        CpuMiscData->MinCoreToBusRatio = (UINTN) BitFieldRead64 (QWord, N_MSR_MAX_EFFICIENCY_RATIO_START, N_MSR_MAX_EFFICIENCY_RATIO_STOP);
        CpuMiscData->MaxCoreToBusRatio = (UINTN) BitFieldRead64 (QWord, N_MSR_MAX_NON_TURBO_RATIO_START, N_MSR_MAX_NON_TURBO_RATIO_STOP);
        QWord = AsmReadMsr64 (MSR_TURBO_RATIO_LIMIT);
        CpuMiscData->MaxTurboRatio = (UINTN) BitFieldRead64 (QWord, N_MSR_MAX_RATIO_LIMIT_1C_START, N_MSR_MAX_RATIO_LIMIT_1C_STOP);

        //
        // Get power scaling information.
        //
        QWord = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
        //
        // Convert to milliWatt
        //
        CpuMiscData->PackageTdp = (UINTN) BitFieldRead64 (QWord, N_MSR_POWER_LIMIT_1_START, N_MSR_POWER_LIMIT_1_STOP) * 1000;
        QWord = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
        CpuMiscData->PackageTdp >>= (UINTN) BitFieldRead64 (QWord, N_MSR_POWER_UNIT_START, N_MSR_POWER_UNIT_STOP);

        //
        // Support for overclockable ratios exists on only on SNB CPU stepping 206A3 and higher and IVB CPU.
        //
        if (!(ModelId == SANDYBRIDGE_CLIENT_MODEL_ID && SteppingId < 3)) {
          //
          // Check to see if this is a locked processor.
          //
          if ((QWord & B_MSR_PROGRAMMABLE_RATIO_LIMIT_TURBO_MODE) == 0) {
            CpuMiscData->FrequencyLocked = TRUE;
          }
        }

        if (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId)) {
          QWord = AsmReadMsr64 (MSR_PLATFORM_INFO);
          if ((QWord & B_MSR_NO_OF_CONFIG_TDP_LEVELS) > 0) {
            //
            // Check Config TDP
            //
            if ((AsmReadMsr64 (MSR_CONFIG_TDP_CONTROL) & B_CONFIG_TDP_CONTROL_LVL) == 1) {
                DEBUG ((EFI_D_INFO, "Config TDP feature Level 1 is supported.\n"));

                QWord = AsmReadMsr64 (MSR_CONFIG_TDP_LVL1);
                CpuMiscData->PkgTDPLvl1 = (UINTN) (QWord & B_CONFIG_TDP_LVL_PKG_TDP);
                CpuMiscData->ConfigTDPLvl1Ratio = (UINTN) (RShiftU64 (QWord, N_CONFIG_TDP_LVL_RATIO) & B_CONFIG_TDP_LVL_RATIO);
                CpuMiscData->PkgMaxPwrLvl1 = (UINTN) (RShiftU64 (QWord, N_CONFIG_TDP_PKG_MAX_PWR) & B_CONFIG_TDP_PKG_MAX_PWR);
                CpuMiscData->PkgMinPwrLvl1 = (UINTN) (RShiftU64 (QWord, N_CONFIG_TDP_PKG_MIN_PWR) & B_CONFIG_TDP_PKG_MIN_PWR);

                CpuMiscData->PackageTdp = (UINTN) (CpuMiscData->PkgTDPLvl1 * 1000);
                CpuMiscData->MaxCoreToBusRatio = CpuMiscData->ConfigTDPLvl1Ratio ;
            } else if (((QWord & B_MSR_NO_OF_CONFIG_TDP_LEVELS) == 2) &&
                       ((AsmReadMsr64 (MSR_CONFIG_TDP_CONTROL) & B_CONFIG_TDP_CONTROL_LVL) == 2)) {
                DEBUG ((EFI_D_INFO, "Config TDP feature Level 2 is supported.\n"));

                QWord = AsmReadMsr64 (MSR_CONFIG_TDP_LVL2);
                CpuMiscData->PkgTDPLvl2 = (UINTN) (QWord & B_CONFIG_TDP_LVL_PKG_TDP);
                CpuMiscData->ConfigTDPLvl2Ratio = (UINTN) (RShiftU64 (QWord, N_CONFIG_TDP_LVL_RATIO) & B_CONFIG_TDP_LVL_RATIO);
                CpuMiscData->PkgMaxPwrLvl2 = (UINTN) (RShiftU64 (QWord, N_CONFIG_TDP_PKG_MAX_PWR) & B_CONFIG_TDP_PKG_MAX_PWR);
                CpuMiscData->PkgMinPwrLvl2 = (UINTN) (RShiftU64 (QWord, N_CONFIG_TDP_PKG_MIN_PWR) & B_CONFIG_TDP_PKG_MIN_PWR);

                CpuMiscData->PackageTdp = (UINTN) (CpuMiscData->PkgTDPLvl2 * 1000);
                CpuMiscData->MaxCoreToBusRatio = (UINTN) CpuMiscData->ConfigTDPLvl2Ratio ;
            }
          } 
        }
      }
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  //
  // Collect intended FSB frequency
  //
  switch (FsbFrequencyEncoding) {
  case 0:
    CpuMiscData->IntendedFsbFrequency = 267;
    break;
  case 1:
    CpuMiscData->IntendedFsbFrequency = 133;
    break;
  case 2:
    CpuMiscData->IntendedFsbFrequency = 200;
    break;
  case 3:
    CpuMiscData->IntendedFsbFrequency = 166;
    break;
  case 4:
    CpuMiscData->IntendedFsbFrequency = 333;
    break;
  case 5:
    CpuMiscData->IntendedFsbFrequency = 100;
    break;
  default:
    CpuMiscData->IntendedFsbFrequency = 0;
    break;
  }

  //
  // Calculate actual FSB frequency
  // First calculate actual frequency by sampling some time and counts TSC
  // Use spinlock mechanism because timer library cannot handle concurrent requests.
  //
  AcquireSpinLock (&mMPSystemData.APSerializeLock);
  OldInterruptState = SaveAndDisableInterrupts ();
  BeginValue  = AsmReadTsc ();
  MicroSecondDelay (1000);
  EndValue    = AsmReadTsc ();
  SetInterruptState (OldInterruptState);
  ReleaseSpinLock (&mMPSystemData.APSerializeLock);
  //
  // Calculate raw actual FSB frequency
  //
  ActualFrequency = DivU64x32 (EndValue - BeginValue, 1000);
  ActualFsb = DivU64x32 (ActualFrequency, (UINT32) CpuMiscData->MaxCoreToBusRatio);
  //
  // Round the raw actual FSB frequency to standardized value
  //
  ActualFsb = ActualFsb + RShiftU64 (ActualFsb, 5);
  ActualFsb = DivU64x32 (MultU64x32 (ActualFsb, 3), 100);
  ActualFsb = DivU64x32 (MultU64x32 (ActualFsb, 100), 3);

  CpuMiscData->ActualFsbFrequency = (UINTN) ActualFsb;

  //
  // Default number of P-states is 1
  //
  CpuMiscData->NumberOfPStates = 1;
}

/**
  Collects processor microcode revision of the processor installed in the system.

  This function collects processor microcode revision of the processor installed in the system.

  @param  ProcessorNumber    Handle number of specified logical processor

**/
VOID
CollectMicrocodeRevision (
  UINTN    ProcessorNumber
  )
{
  CPU_MISC_DATA          *CpuMiscData;

  CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;

  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);

  CpuMiscData->MicrocodeRevision = (UINT32) RShiftU64 (AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID), 32);
}

/**
  Collects capabilities of various features of the processor.

  This function collects capabilities of various features of the processor.

  @param  ProcessorNumber    Handle number of specified logical processor

**/
VOID
CollectFeatureCapability (
  UINTN    ProcessorNumber
  )
{
  //
  // Collect capability for Thermal Management
  //
  if (FeaturePcdGet (PcdCpuThermalManagementFlag)) {
    ThermalManagementDetect (ProcessorNumber);
  }
  //
  // Collect capability for Max CPUID Value Limit
  //
  if (FeaturePcdGet (PcdCpuMaxCpuIDValueLimitFlag)) {
    MaxCpuidLimitDetect (ProcessorNumber);
  }
  //
  // Collect capability for MONITOR/MWAIT instructions
  //
  if (FeaturePcdGet (PcdCpuMonitorMwaitFlag)) {
    MonitorMwaitDetect (ProcessorNumber);
  }
  //
  // Collect capability for C1E
  //
  if (FeaturePcdGet (PcdCpuEnhancedCStateFlag)) {
    C1eDetect (ProcessorNumber);
  }
  //
  // Collect capability for machine check
  //
  if (FeaturePcdGet (PcdCpuMachineCheckFlag)) {
    MachineCheckDetect (ProcessorNumber);
  }
  //
  // Collect capability for Enhanced Intel Speedstep Technology
  //
  if (FeaturePcdGet (PcdCpuEistFlag)) {
    EistDetect (ProcessorNumber);
  }
  //
  // Collect capability for execute disable bit
  //
  if (FeaturePcdGet (PcdCpuExecuteDisableBitFlag)) {
    XdDetect (ProcessorNumber);
  }
  //
  // Collect capability for VT and LT
  //
 if (FeaturePcdGet (PcdCpuVtLtFlag)) {
    VtLtDetect (ProcessorNumber);
  }
  //
  // Collect capability for fast string
  //
  if (FeaturePcdGet (PcdCpuFastStringFlag)) {
    FastStringDetect (ProcessorNumber);
  }
  //
  // Collect capability for hardware prefetcher
  //
  if (FeaturePcdGet (PcdCpuHardwarePrefetcherFlag)) {
    HardwarePrefetcherDetect (ProcessorNumber);
  }
  //
  // Collect capability for adjacent cache line prefetch
  //
  if (FeaturePcdGet (PcdCpuAdjacentCacheLinePrefetchFlag)) {
    AdjacentCacheLinePrefetchDetect (ProcessorNumber);
  }
  //
  // Collect capability for DCU prefetcher
  //
  if (FeaturePcdGet (PcdCpuDcuPrefetcherFlag)) {
    DcuPrefetcherDetect (ProcessorNumber);
  }
  //
  // Collect capability for IP prefetcher
  //
  if (FeaturePcdGet (PcdCpuIpPrefetcherFlag)) {
    IpPrefetcherDetect (ProcessorNumber);
  }
  //
  // Collect capability for MLC Streamer prefetcher
  //
  if (FeaturePcdGet (PcdCpuMlcStreamerPrefetcherFlag)) {
    MlcStreamerPrefetcherDetect (ProcessorNumber);
  }
  //
  // Collect capability for MLC Spatial prefetcher
  //
  if (FeaturePcdGet (PcdCpuMlcSpatialPrefetcherFlag)) {
    MlcSpatialPrefetcherDetect (ProcessorNumber);
  }
  //
  // Collect capability for Signal Break on FERR#
  //
  if (FeaturePcdGet (PcdCpuFerrSignalBreakFlag)) {
    FerrSignalBreakDetect (ProcessorNumber);
  }
  //
  // Collect capability for Platform Environment Control Interface
  //
  if (FeaturePcdGet (PcdCpuPeciFlag)) {
    PeciDetect (ProcessorNumber);
  }
  //
  // Collect capability for Gate A20M processor register programming.
  //
  if (FeaturePcdGet (PcdCpuGateA20MDisableFlag)) {
    DetectA20mFeature (ProcessorNumber);
  }
  //
  // Collect capability for Three Strike Counter feature.
  //
  if (FeaturePcdGet (PcdCpuThreeStrikeCounterFlag)) {
    ThreeStrikeCounterDetect (ProcessorNumber);
  }
  //
  // Collect capability for Engergy Performance Bias feature.
  //
  if (FeaturePcdGet (PcdCpuEnergyPerformanceBiasFlag)) {
    EnergyPerformanceBiasDetect (ProcessorNumber);
  }
  //
  // Collect capability for T-State feature.
  //
  if (FeaturePcdGet (PcdCpuTStateFlag)) {
    TStateDetect (ProcessorNumber);
  }
  //
  // Collect capability for AES feature.
  //
  if (FeaturePcdGet (PcdCpuAesFlag)) {
    AesDetect (ProcessorNumber);
  }
  //
  // Collect capability for DCA feature.
  //
  if (FeaturePcdGet (PcdCpuDcaFlag)) {
    DcaDetect (ProcessorNumber);
  }
  //
  // Collect capability for C-State feature.
  //
  if (FeaturePcdGet (PcdCpuCStateFlag)) {
    CStateDetect (ProcessorNumber);
  }
  //
  // Collect capability for APIC TPR Update Message feature.
  //
  if (FeaturePcdGet (PcdCpuApicTprUpdateMessageFlag)) {
    ApicTprUpdateMessageDetect (ProcessorNumber);
  }
}

/**
  Collects basic processor data for calling processor.

  This function collects basic processor data for calling processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
CollectBasicProcessorData (
  IN UINTN  ProcessorNumber
  )
{
  CPU_MISC_DATA        *CpuMiscData;

  CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;

  if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
    //
    // BIST for the BSP will be updated in CollectBistDataFromHob().
    //
    CpuMiscData->HealthData = 0;
  } else {
    CpuMiscData->HealthData = mExchangeInfo->BistBuffer[ProcessorNumber].Bist;
  }

  //
  // A loop to check APIC ID and processor number
  //
  CollectApicData (ProcessorNumber);

  //
  // Get package number, core number and thread number.
  //
  CollectProcessorLocation (ProcessorNumber);
}

/**
  Collects processor data for calling processor.

  This function collects processor data for calling processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
CollectProcessorData (
  IN UINTN  ProcessorNumber
  )
{
  CPU_MISC_DATA        *CpuMiscData;

  CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;

  //
  // Collect all leafs for CPUID after second time microcode load.
  //
  CollectCpuidLeafs (ProcessorNumber);

  //
  // Get intended FSB frequency and core to bus ratio
  //
  CollectFrequencyData (ProcessorNumber);

  //
  // Check Platform Requirement Bits
  //
  PlatformRequirementDetect (ProcessorNumber);

  //
  // Collect capabilities for various features.
  //
  CollectFeatureCapability (ProcessorNumber);
}

/**
  Checks the number of CPUID leafs need by a processor.

  This function check the number of CPUID leafs need by a processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
CountNumberOfCpuidLeafs (
  IN UINTN  ProcessorNumber
  )
{
  UINT32               MaxCpuidIndex;
  UINT32               MaxExtendedCpuidIndex;
  UINT32               NumberOfCacheAndTlbRecords;
  UINT32               NumberOfDeterministicCacheParametersLeafs;
  UINT32               NumberOfExtendedTopologyEnumerationLeafs;
  UINT32               RegValue;
  CPU_COLLECTED_DATA   *CpuCollectedData;

  CpuCollectedData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber];

  //
  // Second time microcode load.
  // Microcode patch must be applied before collecting capability
  // of processor features.
  //
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    MicrocodeDetect (ProcessorNumber, FALSE);
  }

  //
  // Get the max index of basic CPUID
  //
  AsmCpuid (0, &MaxCpuidIndex, NULL, NULL, NULL);
  //
  // Get the max index of extended CPUID
  //
  AsmCpuid (0x80000000, &MaxExtendedCpuidIndex, NULL, NULL, NULL);
  //
  // Get the number of cache and TLB CPUID leafs
  //
  AsmCpuid (2, &NumberOfCacheAndTlbRecords, NULL, NULL, NULL);
  NumberOfCacheAndTlbRecords = NumberOfCacheAndTlbRecords & 0xff;

  //
  // Get the number of deterministic cache parameter CPUID leafs
  //
  NumberOfDeterministicCacheParametersLeafs = 0;
  do {
    AsmCpuidEx (4, NumberOfDeterministicCacheParametersLeafs++, &RegValue, NULL, NULL, NULL);
  } while ((RegValue & 0x0f) != 0);

  //
  // Get the number of Extended Topology Enumeration CPUID leafs
  //
  NumberOfExtendedTopologyEnumerationLeafs = 0;
  if (MaxCpuidIndex >= EFI_CPUID_CORE_TOPOLOGY) {
    do {
      AsmCpuidEx (EFI_CPUID_CORE_TOPOLOGY, NumberOfExtendedTopologyEnumerationLeafs++, NULL, &RegValue, NULL, NULL);
    } while ((RegValue & 0x0FFFF) != 0);
  }

  //
  // Save collected data in Processor Configuration Context Buffer
  //
  CpuCollectedData->CpuidData.NumberOfBasicCpuidLeafs                        = MaxCpuidIndex + 1;
  CpuCollectedData->CpuidData.NumberOfExtendedCpuidLeafs                     = (MaxExtendedCpuidIndex - 0x80000000) + 1;
  CpuCollectedData->CpuidData.NumberOfCacheAndTlbCpuidLeafs                  = NumberOfCacheAndTlbRecords;
  CpuCollectedData->CpuidData.NumberOfDeterministicCacheParametersCpuidLeafs = NumberOfDeterministicCacheParametersLeafs;
  CpuCollectedData->CpuidData.NumberOfExtendedTopologyEnumerationLeafs       = NumberOfExtendedTopologyEnumerationLeafs;
}

/**
  Checks the number of CPUID leafs of all logical processors, and allocate memory for them.

  This function checks the number of CPUID leafs of all logical processors, and allocates memory for them.

**/
VOID
AllocateMemoryForCpuidLeafs (
  VOID
  )
{
  CPU_COLLECTED_DATA   *CpuCollectedData;
  UINTN                ProcessorNumber;
  UINTN                NumberOfLeafs;

  //
  // Wakeup all APs for CPUID checking.
  //
  DispatchAPAndWait (
    TRUE,
    0,
    CountNumberOfCpuidLeafs
    );
  //
  // Check number of CPUID leafs for BSP.
  // Try to accomplish in first wakeup, and MTRR.
  //
  CountNumberOfCpuidLeafs (mCpuConfigConextBuffer.BspNumber);

  //
  // Allocate memory for CPUID leafs of all processors
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {

    if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
      MicrocodeReg (ProcessorNumber, FALSE);
    }

    CpuCollectedData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber];

    //
    // Get the number of basic CPUID leafs.
    //
    NumberOfLeafs = CpuCollectedData->CpuidData.NumberOfBasicCpuidLeafs;
    //
    // Get the number of extended CPUID leafs.
    //
    NumberOfLeafs += CpuCollectedData->CpuidData.NumberOfExtendedCpuidLeafs;
    //
    // Get the number of cache and TLB CPUID leafs.
    //
    NumberOfLeafs += CpuCollectedData->CpuidData.NumberOfCacheAndTlbCpuidLeafs - 1;
    //
    // Get the number of deterministic cache parameters CPUID leafs.
    //
    NumberOfLeafs += CpuCollectedData->CpuidData.NumberOfDeterministicCacheParametersCpuidLeafs;
    //
    // Get the number of Extended Topology Enumeration CPUID leafs
    //
    NumberOfLeafs += CpuCollectedData->CpuidData.NumberOfExtendedTopologyEnumerationLeafs;

    CpuCollectedData->CpuidData.CpuIdLeaf = AllocateZeroPool (sizeof (EFI_CPUID_REGISTER) * NumberOfLeafs);
  }
}

/**
  Collects BIST data from HOB.

  This function collects BIST data from HOB built by SEC_PLATFORM_PPI.

**/
VOID
CollectBistDataFromHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  UINT32                  *DataInHob;
  UINTN                   NumberOfData;
  UINTN                   ProcessorNumber;
  UINT32                  InitialLocalApicId;
  CPU_MISC_DATA           *CpuMiscData;

  GuidHob = GetFirstGuidHob (&gEfiHtBistHobGuid);
  if (GuidHob == NULL) {
    return;
  }

  DataInHob    = GET_GUID_HOB_DATA (GuidHob);
  NumberOfData = GET_GUID_HOB_DATA_SIZE (GuidHob) / sizeof (UINT32);

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    InitialLocalApicId = GetInitialLocalApicId (ProcessorNumber);
    if (InitialLocalApicId < NumberOfData) {
      CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
      CpuMiscData->HealthData = DataInHob[InitialLocalApicId];
      //
      // Initialize CPU health status for MP Services Protocol according to BIST data.
      //
      mMPSystemData.CpuHealthy[ProcessorNumber] = (BOOLEAN) (CpuMiscData->HealthData == 0);
    }
  }
}

/**
  Collects data from all logical processors.

  This function collects data from all logical processors.

**/
VOID
DataCollectionPhase (
  VOID
  )
{
  UINT8                     CallbackSignalValue;

  //
  // Set PcdCpuCallbackSignal to trigger callback function, and reads the value back.
  //
  CallbackSignalValue = SetAndReadCpuCallbackSignal (CPU_DATA_COLLECTION_SIGNAL);
  //
  // Check whether the callback function requests to bypass Setting phase.
  //
  if (CallbackSignalValue == CPU_BYPASS_SIGNAL) {
    return;
  }

  //
  // Check the number of CPUID leafs of all logical processors, and allocate memory for them.
  //
  AllocateMemoryForCpuidLeafs ();

  //
  // Wakeup all APs for data collection.
  //
  DispatchAPAndWait (
    TRUE,
    0,
    CollectProcessorData
    );

  //
  // Collect data for BSP.
  //
  CollectProcessorData (mCpuConfigConextBuffer.BspNumber);

  CollectBistDataFromHob ();

  //
  // Set PCD data for HT and CMP information
  //
  if (CmpCapable) {
    PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_CMP_BIT);
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_CMP_BIT) != 0) {
      PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_CMP_BIT);
    }
  }
  if (HtCapable) {
    PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_HT_BIT);
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_HT_BIT) != 0) {
      PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_HT_BIT);
    }
  }
}
