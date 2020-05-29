/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Null instance of Cpu-Only Reset Lib.

  Copyright (c) 2007, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CpuOnlyResetLibNull.c

**/

//
// The package level header files this module uses
//
#include <Base.h>

/**
  Provides null implementation for Cpu-Only Reset Lib.
  This function provides null implementation for Cpu-Only Reset Lib.

**/
VOID
EFIAPI
CpuOnlyReset(
  VOID
  )
{
  return;
}
