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
  PchSmmHelpers.h

@brief
  Helper functions for PCH SMM

**/
#ifndef PCH_SMM_HELPERS_H
#define PCH_SMM_HELPERS_H

#include "PchSmm.h"
#include "PchxSmmHelpers.h"

//
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SUPPORT / HELPER FUNCTIONS (PCH version-independent)
//

/**
  Publish SMI Dispatch protocols.

  @param[in] None

  @retval None
**/
VOID
PchSmmPublishDispatchProtocols (
  VOID
  );

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
  );

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
  );

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
  );

/**
  Check if an SMM source is active.

  @param[in] Src                  Pointer to the PCH SMI source description table

  @retval TRUE                    It is active.
  @retval FALSE                   It is inactive.
**/
BOOLEAN
SourceIsActive (
  const IN PCH_SMM_SOURCE_DESC *Src
  );

/**
  Enable the SMI source event by set the SMI enable bit, this function would also clear SMI
  status bit to make initial state is correct

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmEnableSource (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  );

/**
  Disable the SMI source event by clear the SMI enable bit

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmDisableSource (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  );

/**
  Clear the SMI status bit by set the source bit of SMI status register

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
PchSmmClearSource (
  const PCH_SMM_SOURCE_DESC *SrcDesc
  );

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
  );

#endif
