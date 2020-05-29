//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for AES (Advanced Encryption Standard) feature.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Aes.h

**/

#ifndef _CPU_AES_H_
#define _CPU_AES_H_

#include "Cpu.h"

/**
  Detect capability of AES for specified processor.
  
  This function detects capability of AES for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
AesDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for AES for all processors.
  
  This function configures Processor Feature Lists for AES for all processors.

**/
VOID
AesConfigFeatureList (
  VOID
  );

/**
  Produces entry of AES feature in Register Table for specified processor.
  
  This function produces entry of AES feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
AesReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
