//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for misc CPU features.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Misc.h

**/

#ifndef _CPU_MISC_H_
#define _CPU_MISC_H_

#include "Cpu.h"

/**
  Detect capability of MONITOR/MWAIT instructions for specified processor.
  
  This function detects capability of MONITOR/MWAIT instructions for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MonitorMwaitDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for MONITOR/MWAIT instructions for all processors.
  
  This function configures Processor Feature Lists for MONITOR/MWAIT instructions for all processors.

**/
VOID
MonitorMwaitConfigFeatureList (
  VOID
  );

/**
  Produces entry of MONITOR/MWAIT instruction feature in Register Table for specified processor.
  
  This function produces entry of MONITOR/MWAIT instruction feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MonitorMwaitReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of Three Strike Counter for specified processor.
  
  This function detects capability of Three Strike Counter Prefetch for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ThreeStrikeCounterDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for Three Strike Counter Prefetch for all processors.
  
  This function configures Processor Feature Lists for Three Strike Counter Prefetch for all processors.

**/
VOID
ThreeStrikeCounterConfigFeatureList (
  VOID
  );

/**
  Produces entry of Three Strike Counter in Register Table for specified processor.
  
  This function produces entry of Three Strike Counter in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ThreeStrikeCounterReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of Energy Performance Bias for specified processor.
  
  This function detects capability of Energy Performance Bias for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
EnergyPerformanceBiasDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for Energy Performance Bias for all processors.
  
  This function configures Processor Feature Lists for Energy Performance Bias for all processors.

**/
VOID
EnergyPerformanceBiasConfigFeatureList (
  VOID
  );

/**
  Produces entry of Energy Performance Bias in Register Table for specified processor.
  
  This function produces entry of Energy Performance Bias in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
EnergyPerformanceBiasReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of APIC TPR Update Message for specified processor.
  
  This function detects capability of APIC TPR Update Message for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ApicTprUpdateMessageDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for APIC TPR Update Message for all processors.
  
  This function configures Processor Feature Lists for APIC TPR Update Message for all processors.

**/
VOID
ApicTprUpdateMessageConfigFeatureList (
  VOID
  );

/**
  Produces entry of APIC TPR Update Message feature in Register Table for specified processor.
  
  This function produces entry of APIC TPR Update Message feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ApicTprUpdateMessageReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
