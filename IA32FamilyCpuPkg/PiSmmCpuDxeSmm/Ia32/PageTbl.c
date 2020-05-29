/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

Page table manipulation functions for IA-32 processors

Copyright (c) 2009 - 2012, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include "PiSmmCpuDxeSmm.h"

SPIN_LOCK                           mPFLock;

/**
  Create PageTable for SMM use.

  @return     PageTable Address    

**/
UINT32
SmmInitPageTable (
  VOID
  )
{
  //
  // Initialize spin lock
  //
  InitializeSpinLock (&mPFLock);

  return Gen4GPageTable (0);
}

/**
  Page Fault handler for SMM use.

**/
VOID
SmiDefaultPFHandler (
  VOID
  )
{
  CpuDeadLoop ();
}

/**
  Page Fault handler for SMM use.

  @param   IntNumber             The number of the interrupt.
  @param   ErrorCode             The Error code.
  @param   InstructionAddress    The instruction address which caused page fault.

**/
VOID
EFIAPI
SmiPFHandler (
  IN      UINTN                     IntNumber,
  IN      UINTN                     ErrorCode,
  IN      UINTN                     InstructionAddress
  )
{
  UINTN             PFAddress;

  ASSERT (IntNumber == 0x0e);

  AcquireSpinLock (&mPFLock);
  
  PFAddress = AsmReadCr2 ();

  if ((FeaturePcdGet (PcdCpuSmmStackGuard)) && 
      (PFAddress >= gSmmCpuPrivate->SmrrBase) && 
      (PFAddress < (gSmmCpuPrivate->SmrrBase + gSmmCpuPrivate->SmrrSize))) {
    DEBUG ((EFI_D_ERROR, "SMM stack overflow!\n"));
    CpuDeadLoop ();
  }
  
  if (FeaturePcdGet (PcdCpuSmmProfileEnable)) {
    SmmProfilePFHandler (InstructionAddress, ErrorCode);
  } else {
    SmiDefaultPFHandler ();
  }  
  
  ReleaseSpinLock (&mPFLock);
}