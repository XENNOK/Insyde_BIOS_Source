//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for Execute Disable Bit Feature

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Xd.h

**/

#ifndef _CPU_XD_H_
#define _CPU_XD_H_

#include "MpCommon.h"
#include "Cpu.h"

/**
  Detect capability of XD feature for specified processor.
  
  This function detects capability of XD feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
XdDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for XD feature for all processors.
  
  This function configures Processor Feature Lists for XD feature for all processors.

**/
VOID
XdConfigFeatureList (
  VOID
  );

/**
  Produces entry of XD feature in Register Table for specified processor.
  
  This function produces entry of XD feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
XdReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
