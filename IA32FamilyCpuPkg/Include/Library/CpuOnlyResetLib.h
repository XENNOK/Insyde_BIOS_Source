//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Public include file for the CPU-Only Reset Library.

  Copyright (c) 2007, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CpuOnlyResetLib.h

**/

#ifndef __CPU_ONLY_RESET_LIB_H__
#define __CPU_ONLY_RESET_LIB_H__

/**
  Performs a CPU only reset.
  This function performs a CPU-only reset.

**/
VOID
EFIAPI
CpuOnlyReset(
  VOID
  );    

#endif   
