//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for C-State feature.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CState.h

**/

#ifndef _CPU_CSTATE_H_
#define _CPU_CSTATE_H_

#include "Cpu.h"

#define CST_C0          00
#define CST_C1          01
#define CST_C3          02
#define CST_C6          03
#define CST_C7          04
#define CST_NOLIMIT     07

/**
  Detect capability of C-State for specified processor.
  
  This function detects capability of C-State for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
CStateDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for C-State for all processors.
  
  This function configures Processor Feature Lists for C-State for all processors.

**/
VOID
CStateConfigFeatureList (
  VOID
  );

/**
  Produces entry of C-State feature in Register Table for specified processor.
  
  This function produces entry of C-State feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
CStateReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
