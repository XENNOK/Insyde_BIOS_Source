/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmmHelpers.c

@brief
  Helper functions for PCH SMM dispatcher.

**/
#include "PchSmmHelpers.h"

///
/// #define BIT_ZERO 0x00000001
///
const UINT32  BIT_ZERO = 0x00000001;

///
/// SUPPORT / HELPER FUNCTIONS (PCH version-independent)
///

/**
  Compare 2 SMM source descriptors' enable settings.

  @param[in] Src1                 Pointer to the PCH SMI source description table 1
  @param[in] Src2                 Pointer to the PCH SMI source description table 2

  @retval TRUE                    The enable settings of the 2 SMM source descriptors are identical.
  @retval FALSE                   The enable settings of the 2 SMM source descriptors are not identical.
**/
BOOLEAN
CompareEnables (
  const IN PCH_SMM_SOURCE_DESC *Src1,
  const IN PCH_SMM_SOURCE_DESC *Src2
  )
{
  BOOLEAN IsEqual;
  UINTN   loopvar;

  IsEqual = TRUE;
  for (loopvar = 0; loopvar < NUM_EN_BITS; loopvar++) {
    ///
    /// It's okay to compare a NULL bit description to a non-NULL bit description.
    /// They are unequal and these tests will generate the correct result.
    ///
    if (Src1->En[loopvar].Bit != Src2->En[loopvar].Bit ||
        Src1->En[loopvar].Reg.Type != Src2->En[loopvar].Reg.Type ||
        Src1->En[loopvar].Reg.Data.raw != Src2->En[loopvar].Reg.Data.raw
        ) {
      IsEqual = FALSE;
      break;
      ///
      /// out of for loop
      ///
    }
  }

  return IsEqual;
}

/**
  Compare 2 SMM source descriptors' statuses.

  @param[in] Src1                 Pointer to the PCH SMI source description table 1
  @param[in] Src2                 Pointer to the PCH SMI source description table 2

  @retval TRUE                    The statuses of the 2 SMM source descriptors are identical.
  @retval FALSE                   The statuses of the 2 SMM source descriptors are not identical.
**/
BOOLEAN
CompareStatuses (
  const IN PCH_SMM_SOURCE_DESC *Src1,
  const IN PCH_SMM_SOURCE_DESC *Src2
  )
{
  BOOLEAN IsEqual;
  UINTN   loopvar;

  IsEqual = TRUE;

  for (loopvar = 0; loopvar < NUM_STS_BITS; loopvar++) {
    ///
    /// It's okay to compare a NULL bit description to a non-NULL bit description.
    /// They are unequal and these tests will generate the correct result.
    ///
    if (Src1->Sts[loopvar].Bit != Src2->Sts[loopvar].Bit ||
        Src1->Sts[loopvar].Reg.Type != Src2->Sts[loopvar].Reg.Type ||
        Src1->Sts[loopvar].Reg.Data.raw != Src2->Sts[loopvar].Reg.Data.raw
        ) {
      IsEqual = FALSE;
      break;
      ///
      /// out of for loop
      ///
    }
  }

  return IsEqual;
}

/**
  Compare 2 SMM source descriptors, based on Enable settings and Status settings of them.

  @param[in] Src1                 Pointer to the PCH SMI source description table 1
  @param[in] Src2                 Pointer to the PCH SMI source description table 2

  @retval TRUE                    The 2 SMM source descriptors are identical.
  @retval FALSE                   The 2 SMM source descriptors are not identical.
**/
BOOLEAN
CompareSources (
  const IN PCH_SMM_SOURCE_DESC *Src1,
  const IN PCH_SMM_SOURCE_DESC *Src2
  )
{
  return (BOOLEAN) (CompareEnables (Src1, Src2) && CompareStatuses (Src1, Src2));
}

/**
  Check if an SMM source is active.

  @param[in] Src                  Pointer to the PCH SMI source description table

  @retval TRUE                    It is active.
  @retval FALSE                   It is inactive.
**/
BOOLEAN
SourceIsActive (
  const IN PCH_SMM_SOURCE_DESC *Src
  )
{
  BOOLEAN IsActive;
  UINTN   loopvar;

  BOOLEAN SciEn;

  IsActive  = TRUE;

  SciEn     = PchSmmGetSciEn ();

  if ((Src->Flags & PCH_SMM_SCI_EN_DEPENDENT) && (SciEn)) {
    ///
    /// This source is dependent on SciEn, and SciEn == 1.  An ACPI OS is present,
    /// so we shouldn't do anything w/ this source until SciEn == 0.
    ///
    IsActive = FALSE;

  } else {
    ///
    /// Read each bit desc from hardware and make sure it's a one
    ///
    for (loopvar = 0; loopvar < NUM_EN_BITS; loopvar++) {

      if (!IS_BIT_DESC_NULL (Src->En[loopvar])) {

        if (ReadBitDesc (&Src->En[loopvar]) == 0) {
          IsActive = FALSE;
          break;
          ///
          /// out of for loop
          ///
        }

      }
    }

    if (IsActive) {
      ///
      /// Read each bit desc from hardware and make sure it's a one
      ///
      for (loopvar = 0; loopvar < NUM_STS_BITS; loopvar++) {

        if (!IS_BIT_DESC_NULL (Src->Sts[loopvar])) {

          if (ReadBitDesc (&Src->Sts[loopvar]) == 0) {
            IsActive = FALSE;
            break;
            ///
            /// out of for loop
            ///
          }

        }
      }
    }
  }

  return IsActive;
}

/**
  Enable the SMI source event by set the SMI enable bit, this function would also clear SMI
  status bit to make initial state is correct

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmEnableSource (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  )
{
  UINTN loopvar;

  ///
  /// Set enables to 1 by writing a 1
  ///
  for (loopvar = 0; loopvar < NUM_EN_BITS; loopvar++) {
    if (!IS_BIT_DESC_NULL (SrcDesc->En[loopvar])) {
      WriteBitDesc (&SrcDesc->En[loopvar], 1, FALSE);
    }
  }
  ///
  /// Clear statuses to 0 by writing a 1
  ///
  for (loopvar = 0; loopvar < NUM_STS_BITS; loopvar++) {
    if (!IS_BIT_DESC_NULL (SrcDesc->Sts[loopvar])) {
      WriteBitDesc (&SrcDesc->Sts[loopvar], 1, TRUE);
    }
  }
}

/**
  Disable the SMI source event by clear the SMI enable bit

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmDisableSource (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  )
{
  UINTN loopvar;

  for (loopvar = 0; loopvar < NUM_EN_BITS; loopvar++) {
    if (!IS_BIT_DESC_NULL (SrcDesc->En[loopvar])) {
      WriteBitDesc (&SrcDesc->En[loopvar], 0, FALSE);
    }
  }
}

/**
  Clear the SMI status bit by set the source bit of SMI status register

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmClearSource (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  )
{
  UINTN loopvar;

  for (loopvar = 0; loopvar < NUM_STS_BITS; loopvar++) {
    if (!IS_BIT_DESC_NULL (SrcDesc->Sts[loopvar])) {
      WriteBitDesc (&SrcDesc->Sts[loopvar], 1, TRUE);
    }
  }
}

/**
  Sets the source to a 1 and then waits for it to clear.
  Be very careful when calling this function -- it will not
  ASSERT.  An acceptable case to call the function is when
  waiting for the NEWCENTURY_STS bit to clear (which takes
  3 RTCCLKs).

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmClearSourceAndBlock (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  )
{
  UINTN   loopvar;
  BOOLEAN IsSet;

  for (loopvar = 0; loopvar < NUM_STS_BITS; loopvar++) {

    if (!IS_BIT_DESC_NULL (SrcDesc->Sts[loopvar])) {
      ///
      /// Write the bit
      ///
      WriteBitDesc (&SrcDesc->Sts[loopvar], 1, TRUE);

      ///
      /// Don't return until the bit actually clears.
      ///
      IsSet = TRUE;
      while (IsSet) {
        IsSet = ReadBitDesc (&SrcDesc->Sts[loopvar]);
        ///
        /// IsSet will eventually clear -- or else we'll have
        /// an infinite loop.
        ///
      }
    }
  }
}
