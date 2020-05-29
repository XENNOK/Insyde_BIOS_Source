//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/** @file

  Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _GATE_A20M_H_
#define _GATE_A20M_H_

#include "MpCommon.h"
#include "Cpu.h"

/**
  Detects support for Gate A20M Disable.

  @param  ProcessorNumber      Processor number being evaluated at this time.

  @return Nothing.

**/
VOID
DetectA20mFeature (
  IN  UINTN ProcessorNumber
  );

/**
  Sets the configuration PCD to determine if Gate A20M is supported and if
  it should be enabled.

**/
VOID
A20mConfigureFeatureList (
  VOID
  );

/**
  Creates a table entry to program Gate A20M Disable.

  @param  ProcessorNumber  Processor number being evaluated at this time.
  @param  Attribute        Boolean describing if Gate A20M support should be enabled or disabled.

**/
VOID
A20mRegs (
  UINTN   ProcessorNumber,
  VOID    *Attribute
  );

#endif
