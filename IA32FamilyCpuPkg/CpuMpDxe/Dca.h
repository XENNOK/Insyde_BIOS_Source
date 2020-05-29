//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for DCA (Direct Cache Access) feature.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Dca.h

**/

#ifndef _CPU_DCA_H_
#define _CPU_DCA_H_

#include "Cpu.h"

/**
  Detect capability of DCA for specified processor.
  
  This function detects capability of DCA for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
DcaDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for DCA for all processors.
  
  This function configures Processor Feature Lists for DCA for all processors.

**/
VOID
DcaConfigFeatureList (
  VOID
  );

/**
  Produces entry of DCA feature in Register Table for specified processor.
  
  This function produces entry of DCA feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
DcaReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
