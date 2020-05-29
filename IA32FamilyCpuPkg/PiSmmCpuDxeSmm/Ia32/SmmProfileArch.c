/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  IA-32 processor specific functions to enable SMM profile.

  Copyright (c) 2012, Intel Corporation. All rights reserved. <BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "PiSmmCpuDxeSmm.h"
#include "SmmProfileInternal.h"

/**
  Create SMM page table for S3 path.
  
**/
VOID
InitSmmS3Cr3 (
  VOID
  )
{
  mSmmS3ResumeState->SmmS3Cr3 = Gen4GPageTable (0);

  return ;
}

/**
  Allocate pages for creating 4KB-page based on 2MB-page when page fault happens.
  32-bit firmware does not need it.
  
**/
VOID
InitPagesForPFHandler (
  VOID
  )
{
}

/**
  Update page table to map the memory correctly in order to make the instruction 
  which caused page fault execute successfully. And it also save the original page
  table to be restored in single-step exception. 32-bit firmware does not need it.

  @param  PageTable           PageTable Address.
  @param  PFAddress           The memory address which caused page fault exception.
  @param  CpuIndex            The index of the processor.
  @param  ErrorCode           The Error code of exception.
  @param  IsValidPFAddress    The flag indicates if SMM profile data need be added.

**/
VOID
RestorePageTableAbove4G (
  UINT64        *PageTable,
  UINT64        PFAddress,
  UINTN         CpuIndex,
  UINTN         ErrorCode,
  BOOLEAN       *IsValidPFAddress
  )
{
}
