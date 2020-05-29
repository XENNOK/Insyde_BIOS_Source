/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file

  Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "GateA20m.h"

//
// Defines local storage for feature states.
//
STATIC BOOLEAN mGateA20mDisable[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];

/**
  Detects support for Gate A20M Disable.

  @param ProcessorNumber  The handle number of processor.

**/
VOID
DetectA20mFeature (
  IN  UINTN ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;
  UINT32    ProcessorType;

  //
  // Get processor information.
  //
  GetProcessorVersionInfo (
    ProcessorNumber,
    &FamilyId,
    &ModelId,
    &SteppingId,
    &ProcessorType
    );

  //
  // Nehalem family support code.
  //
  if (FeaturePcdGet (PcdCpuNehalemFamilyFlag)) {
    //
    // Check to make sure the this is a supported processor.
    //
    if (IS_NEHALEM_PROC (FamilyId, ModelId)) {
      //
      // Check to see if A20M Disable is supported
      //
      // The legacy A20M# feature will not be supported for the Sandy Bridge processor family
      //
      if ((ModelId == BLOOMFIELD_MODEL_ID && SteppingId >= 2) ||
          (ModelId > BLOOMFIELD_MODEL_ID)) {
        SetProcessorFeatureCapability (ProcessorNumber, GateA20MDisable, NULL);
      }
    }
  }
}


/**
  Sets the configuration PCD to determine if Gate A20M is supported and if
  it should be enabled.

**/
VOID
A20mConfigureFeatureList (
  VOID
  )
{
  UINTN     ProcessorNumber;
  BOOLEAN   UserConfiguration;

  //
  // Assume Gate A20 support should be disabled.  The platform may try to enable
  // the support.
  //
  UserConfiguration = TRUE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_A20M_DISABLE_BIT) == 0) {
    UserConfiguration = FALSE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Program capability and setting values based on support of BSP.
    //
    if (FeaturePcdGet (PcdCpuNehalemFamilyFlag)) {
      if (GetProcessorFeatureCapability (ProcessorNumber, GateA20MDisable, NULL)) {
        if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
          //
          // Mark in PCD that A20M disable is supported.
          //
          PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_A20M_DISABLE_BIT);

          //
          // Check user support to see if the feature should be marked as set.
          //
          if (UserConfiguration) {
            PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_A20M_DISABLE_BIT);
          }
        }

        //
        // Mark local data for this processor entry and also generate a table entry.
        //
        mGateA20mDisable[ProcessorNumber] = UserConfiguration;
        AppendProcessorFeatureIntoList (ProcessorNumber, GateA20MDisable, &(mGateA20mDisable[ProcessorNumber]));
      }
    }
  }
}

/**
  Creates a table entry to program Gate A20M Disable.

  @param  ProcessorNumber   Processor number being evaluated at this time.
  @param  Attribute         Boolean describing if Gate A20M support should be enabled or disabled.

**/
VOID
A20mRegs (
  UINTN   ProcessorNumber,
  VOID    *Attribute
  )
{
  if (FeaturePcdGet (PcdCpuNehalemFamilyFlag)) {
    //
    // Program A20M Disable Feature.
    //
    WriteRegisterTable (
      ProcessorNumber,
      Msr,
      MSR_VLW_CONTROL,
      N_MSR_A20M_DISABLE,
      1,
      *((BOOLEAN*) Attribute) ? 1 : 0
      );
  }
}


