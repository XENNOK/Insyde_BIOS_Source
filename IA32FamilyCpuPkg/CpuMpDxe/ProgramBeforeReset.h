//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for core to bus ratio and VID on reset.

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  //Module Name:  FrequencyOnReset.h

**/

#ifndef _CPU_FREQUENCY_ON_RESET_H_
#define _CPU_FREQUENCY_ON_RESET_H_

#include "Cpu.h"

#define   IA32_FLEX_BRVID_SEL             0x1A1

/**
  Detect whether Platform Requried Bit is set by the processor.
  
  This function detects whether Platform Requried Bit is set by the processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PlatformRequirementDetect (
  UINTN   ProcessorNumber
  );

/**
  Program processor before reset.
  
  This function programs processor before reset.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ProgramBeforeReset (
  UINTN     ProcessorNumber
  );

#endif
