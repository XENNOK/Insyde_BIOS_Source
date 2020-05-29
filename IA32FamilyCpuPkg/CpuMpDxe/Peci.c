/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for PECI Feature

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Peci.c

**/

#include "Peci.h"

BOOLEAN    EnablePeci[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of PECI feature for specified processor.
  
  This function detects capability of PECI feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PeciDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;
  UINT64    MsrValue;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if (FamilyId == 0x06) {
    if (ModelId == 0x0F) {
      MsrValue = AsmReadMsr64 (MSR_EXT_CONFIG);
      if (SteppingId >= 0x02 && BitFieldRead64 (MsrValue, 22, 22) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
      }
    } else if (ModelId == 0x17 || ModelId == 0x16) {
      MsrValue = AsmReadMsr64 (MSR_EXT_CONFIG);
      if (BitFieldRead64 (MsrValue, 22, 22) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
      }
    } else if (IS_NEHALEM_PROC(FamilyId, ModelId)) {
      SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for PECI feature for all processors.
  
  This function configures Processor Feature Lists for PECI feature for all processors.

**/
VOID
PeciConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_PECI_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports PECI
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Peci, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of PECI capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_PECI_BIT);
        //
        // Set the bit of PECI setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_PECI_BIT);
        }
      }

      //
      // If this logical processor supports PECI, then add feature into feature list for operation
      // on corresponding register.
      //
      EnablePeci[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, Peci, &(EnablePeci[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of PECI feature in Register Table for specified processor.
  
  This function produces entry of PECI feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
PeciReg (
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
  // If Attribute is TRUE, then write 1 to MSR_PECI_CONTROL[0].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  if (!IS_NEHALEM_PROC(FamilyId, ModelId)) {
    WriteRegisterTable (ProcessorNumber, Msr, MSR_PECI_CONTROL, 0, 1, Value);
  }
}
