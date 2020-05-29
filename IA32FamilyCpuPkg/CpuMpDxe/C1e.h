//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for C1E Feature

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  C1e.h

**/

#ifndef _C1E_H_
#define _C1E_H_

#include "MpCommon.h"
#include "Cpu.h"
#include "MpService.h"

/**
  Exception handler for exception triggered if C1E is not supported.
  
  If C1E feature is not supported, setting it triggers GP exception.
  This function handles the exception.

**/
VOID
EFIAPI
C1eExceptionHandler (
  VOID
  );

/**
  Detect capability of C1E feature for specified processor.
  
  This function detects capability of C1E feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
C1eDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for C1E feature for all processors.
  
  This function configures Processor Feature Lists for C1E feature for all processors.

**/
VOID
C1eConfigFeatureList (
  VOID
  );

/**
  Produces entry of C1E feature in Register Table for specified processor.
  
  This function produces entry of C1E feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
C1eReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
