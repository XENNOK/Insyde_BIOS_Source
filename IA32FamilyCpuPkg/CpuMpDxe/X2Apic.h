//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for x2APIC feature.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  X2Apic.h

**/

#ifndef _CPU_X2APIC_H_
#define _CPU_X2APIC_H_

#include "Cpu.h"

/**
  Produces entry for x2APIC feature in Register Table for specified processor.
  
  This function produces entry for x2APIC feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
X2ApicReg (
  IN UINTN      ProcessorNumber
  );

/**
  Construct new APIC IDs based on current APIC IDs and new socket ID configuration.

**/
VOID
ConstructNewApicId (
  VOID
  );

/**
  Sort the APIC ID of all processors.

  This function sorts the APIC ID of all processors so that processor number is assigned in the
  ascending order of APIC ID which eases MP debugging. SMBIOS logic also depends on this assumption.

**/
VOID
SortApicId (
  VOID
  );

/**
  Update collected APIC ID related info after socket IDs were reassigned.

**/
VOID
UpdateApicId (
  VOID
  );

/**
  Check if there is legacy APIC ID conflict among all processors.

  @retval EFI_SUCCESS      No coflict or conflict was resoved by force x2APIC mode
  @retval EFI_UNSUPPORTED  There is legacy APIC ID conflict and can't be rsolved in xAPIC mode
**/
EFI_STATUS
CheckApicId (
  VOID
  );

/**
  Assign a package scope BSP responsible for package scope programming.

  This functions assigns the processor with the least APIC ID within a processor package as
  the package BSP.

**/
VOID
AssignPackageBsp (
  VOID
  );

/**
  Generate entry for socket ID reassignment in pre-SMM-init S3 register table.
  
  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
SocketIdReg (
  IN UINTN      ProcessorNumber
  );

/**
  Re-program Local APIC for virtual wire mode after socket ID for the BSP has been changed.

**/
VOID
ReprogramVirtualWireMode (
  VOID
  );

#endif
