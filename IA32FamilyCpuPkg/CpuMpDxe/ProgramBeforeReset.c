/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Code for programming core to bus ratio and VID on reset.

  Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ProgramBeforeReset.c

**/

#include "ProgramBeforeReset.h"
#include "MpCommon.h"

/**
  Detect whether Platform Requried Bit is set by the processor.
  
  This function detects whether Platform Requried Bit is set by the processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PlatformRequirementDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32              BrvidSel;
  CPU_MISC_DATA       *CpuMiscData;
  UINT32              FamilyId;
  UINT32              ModelId;

  //
  // Processor feature flag checking
  //
  if (FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {
    
    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
  
    //
    // Bit 18 of IA32_FLEX_BRVID_SEL is valid only for F3x and F4x
    //
    if (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04)) {
      BrvidSel = AsmReadMsr32 (IA32_FLEX_BRVID_SEL);
      if (BitFieldRead32 (BrvidSel, 18, 18) != 0) {
        CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
        CpuMiscData->PlatformRequirement = BitFieldOr32 (CpuMiscData->PlatformRequirement, 0, 0, 1);
      }
    }
  }
}

/**
  Program processor before reset.
  
  This function programs processor before reset.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ProgramBeforeReset (
  UINTN      ProcessorNumber
  )
{
  EFI_CPUID_REGISTER       *CpuidRegisters;
  CPU_MISC_DATA            *CpuMiscData;
  UINT32                   MaxCoresInPackage;
  CPU_REGISTER_TABLE       *RegisterTable;
  CPU_REGISTER_TABLE_ENTRY *Entry;
  UINT32                   FamilyId;
  UINT32                   ModelId;
  BOOLEAN                  MicrocodeEntryExist;

  //
  // Check if register table entry for microcode update already exists
  // in register table.
  //
  MicrocodeEntryExist = FALSE;
  RegisterTable       = &mCpuConfigConextBuffer.RegisterTable[ProcessorNumber];
  if (RegisterTable->TableLength > 0) {
    ASSERT (RegisterTable->TableLength == 1);
    MicrocodeEntryExist = TRUE;
  }

  //
  // Setting Platform Confirmance Bit and frequency is needed only for single-core Prescott
  //
  if (FeaturePcdGet (PcdCpuPrescottFamilyFlag)) {

    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

    //
    // Check for Prescott family
    //
    if (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04)) {
      //
      // If dual-core, skip setting, for Smithfield does not program IA32_FLEX_BRVID_SEL.
      //
      CpuidRegisters    = GetDeterministicCacheParametersCpuidLeaf (ProcessorNumber, 0);
      MaxCoresInPackage = (((CpuidRegisters->RegEax) >> 26) & 0x3f) + 1;
      if (MaxCoresInPackage == 1) {
        //
        // Check whether platform requirement bit is set.
        //
        CpuMiscData = &mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData;
        if (BitFieldRead32 (CpuMiscData->PlatformRequirement, 0, 0) != 0) {
          //
          // If platform supports high power load line, set the platform conformance bit.
          //
          if (PcdGetBool (PcdPlatformHighPowerLoadLineSupport)) {
            WriteRegisterTable (
              ProcessorNumber,
              Msr,
              IA32_FLEX_BRVID_SEL,
              17,
              1,
              1
              );
            //
            // If frequency is not locked, set core to bus ratio and VID for next CPU-only reset.
            //
            if (!GET_CPU_MISC_DATA (ProcessorNumber, FrequencyLocked)) {
              WriteRegisterTable (
                ProcessorNumber,
                Msr,
                IA32_FLEX_BRVID_SEL,
                8,
                8,
                (UINT8) GET_CPU_MISC_DATA (ProcessorNumber, MaxCoreToBusRatio)
                );
              WriteRegisterTable (
                ProcessorNumber,
                Msr,
                IA32_FLEX_BRVID_SEL,
                0,
                8,
                (UINT8) GET_CPU_MISC_DATA (ProcessorNumber, MaxVid)
                );
            }
          }
        }
      }
    }
  }
  
  //
  // Calculate the number of entries which are just added.
  //
  RegisterTable->NumberBeforeReset = RegisterTable->TableLength;
  if (MicrocodeEntryExist) {
    RegisterTable->NumberBeforeReset--;
  }

  //
  // If CPU-only reset is needed and microcode patch has been applied,
  // then append another register table entry for microcode update, so that
  // the microcode patch would be applied immediately after CPU-only reset.
  //
  if ((RegisterTable->NumberBeforeReset) > 0 && MicrocodeEntryExist) {
    Entry = &(RegisterTable->RegisterTableEntry[0]);
    WriteRegisterTable (
      ProcessorNumber,
      Entry->RegisterType,
      Entry->Index,
      Entry->ValidBitStart,
      Entry->ValidBitLength,
      Entry->Value
      );
  }
}
