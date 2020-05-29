//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for TM and TM2 Feature

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ThermalManagement.h

**/

#ifndef _CPU_THERMAL_MANAGEMENT_H_
#define _CPU_THERMAL_MANAGEMENT_H_

#include "Cpu.h"

#define CPU_THERMAL_MONITOR1_BIT    0x01
#define CPU_THERMAL_MONITOR2_BIT    0x02

/**
  Detect capability of Thermal Management feature for specified processor.
  
  This function detects capability of Thermal Management feature feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ThermalManagementDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for Thermal Management feature for all processors.
  
  This function configures Processor Feature Lists for Thermal Management feature for all processors.

**/
VOID
ThermalManagementConfigFeatureList (
  VOID
  );

/**
  Produces entry of Thermal Management feature in Register Table for specified processor.
  
  This function produces entry of Thermal Management feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
ThermalManagementReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
