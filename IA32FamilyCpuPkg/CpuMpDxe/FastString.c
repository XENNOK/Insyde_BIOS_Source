/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Fast String Feature

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  FastString.c

**/

#include "FastString.h"

BOOLEAN    EnableFastString[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of Fast String feature for specified processor.
  
  This function detects capability of Fast String feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
FastStringDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;
  UINT32    ProcessorType;

  GetProcessorVersionInfo (
    ProcessorNumber,
    &FamilyId,
    &ModelId,
    &SteppingId,
    &ProcessorType
    );

  //
  // Fast String feature is supported by Pentium 4 processors and Core 2 family processors
  //
  if ((FamilyId == 0x0F) || ((FamilyId == 0x06) && (ModelId == 0x0F || ModelId == 0x16))) {
    SetProcessorFeatureCapability (ProcessorNumber, FastString, NULL);
  }
  
  //
  // Supported on i3, i5 and i7 processors
  //
  if (IS_NEHALEM_PROC (FamilyId, ModelId) || IS_SANDYBRIDGE_PROC (FamilyId, ModelId) ||
      IS_IVYBRIDGE_PROC (FamilyId, ModelId)) {
    SetProcessorFeatureCapability (ProcessorNumber, FastString, NULL);
  }
}

/**
  Configures Processor Feature Lists for Fast String feature for all processors.
  
  This function configures Processor Feature Lists for Fast String feature for all processors.

**/
VOID
FastStringConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_FAST_STRING_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports fast string
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, FastString, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of fast string capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_FAST_STRING_BIT);
        //
        // Set the bit of fast string setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_FAST_STRING_BIT);
        }
      }

      //
      // If this logical processor supports fast string, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableFastString[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, FastString, &(EnableFastString[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of Fast String feature in Register Table for specified processor.
  
  This function produces entry of Fast String feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
FastStringReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  //
  // If Attribute is TRUE, then write 1 to MSR_IA32_MISC_ENABLE[0].
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
    N_MSR_FAST_STRINGS_ENABLE,
    1,
    Value
    );
}
