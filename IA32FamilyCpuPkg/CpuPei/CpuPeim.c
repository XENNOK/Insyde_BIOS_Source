/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/** @file
Implementation of CPU driver for PEI phase.

This PEIM is to expose the Cache Ppi and BIST hob build notification

  Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.                                       
  
Module Name: CpuPeim.c

**/

#include "CpuPei.h"
#include "Bist.h"

extern PEI_CACHE_PPI                mCachePpi;
//
// Ppis to be installed
//
EFI_PEI_PPI_DESCRIPTOR           mPpiList[] = { 
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiCachePpiGuid,
    &mCachePpi
  }
};

//
// Notification to build BIST information
//
STATIC EFI_PEI_NOTIFY_DESCRIPTOR        mNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMasterBootModePpiGuid,
    BuildBistAndCpuInfoHob
  }
};

/**
  Initialize SSE support.
**/
VOID
InitXMM (
  VOID
  )
{

  UINT32  RegEdx;

  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx);

  //
  //Check whether SSE2 is supported
  //
  if ((RegEdx & BIT26) == 0) {
    AsmWriteCr0 (AsmReadCr0 () | BIT1);
    AsmWriteCr4 (AsmReadCr4 () | BIT9 | BIT10);
  }
}


/**
  The Entry point of the CPU PEIM

  This function is the Entry point of the CPU PEIM which will install the CachePpi and 
  BuildBISTHob notifier. And also the function will deal with the relocation to memory when 
  permanent memory is ready
 
  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services. 
                          
  @retval EFI_SUCCESS   CachePpi and BIST hob build notification is installed
                        successfully.

**/
EFI_STATUS
EFIAPI
CpuPeimInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
	EFI_STATUS  Status;

  //
  // Install PPIs
  //
  Status = PeiServicesInstallPpi(&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Register for PPI Notifications
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

   
  InitXMM ();
  
  return Status;
}
