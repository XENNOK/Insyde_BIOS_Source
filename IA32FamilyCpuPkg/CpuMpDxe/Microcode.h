//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for microcode update

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Microcode.h

**/

#ifndef _CPU_MICROCODE_H_
#define _CPU_MICROCODE_H_

#include "MpCommon.h"

#define MAX_MICROCODE_DESCRIPTOR_LENGTH  100

typedef struct {
  VOID     *MicrocodeData;
  UINTN    MicrocodeSize;
  UINT32   ProcessorId;
  BOOLEAN  Load;
} MICROCODE_INFO;

/**
  Detect whether specified processor can find matching mirocode patch.
  
  This function detects whether specified processor can find matching mirocode patch.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeDetect (
  IN UINTN   ProcessorNumber,
  IN BOOLEAN FirstLoad
  );


/**
  Generate entry for microcode load in S3 register table.
  
  For first time microcode load, if needed, an entry is generated in the pre-SMM-init register table.
  For second time microcode load, if needed, an entry is generated in the post-SMM-init register table.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeReg (
  IN UINTN    ProcessorNumber,
  IN BOOLEAN  FirstLoad
  );

/**
  Measure loaded CPU microcode.
  
**/
VOID
MeasureMicrocode (
  VOID
  );

#endif
