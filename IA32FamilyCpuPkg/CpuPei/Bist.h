//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/** @file
Implementation of CPU driver for PEI phase.

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.                                       

Module Name: Bist.h

**/

#ifndef _CPU_BIST_H_
#define _CPU_BIST_H_

/**

  Implementation of the PlatformInformation service
  
  Implementation of the PlatformInformation service in EFI_SEC_PLATFORM_INFORMATION_PPI.
  This function will parse SecPlatform Information from BIST Hob.

  @param  PeiServices                Pointer to the PEI Services Table.
  @param  StructureSize              Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord  Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.
  
  @retval EFI_SUCCESS                The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL       The buffer was too small.
  
**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN OUT UINT64                               *StructureSize,
     OUT EFI_SEC_PLATFORM_INFORMATION_RECORD *PlatformInformationRecord
  );

/**
  A callback function to build CPU(only BSP) BIST. 

  This function is a callback function to build bsp's BIST by calling SecPlatformInformationPpi

  @param  PeiServices      Pointer to PEI Services Table      
  @param  NotifyDescriptor Address if the notification descriptor data structure 
  @param  Ppi              Address of the PPI that was installed     
  @retval EFI_SUCCESS      Retrieve of the BIST data successfully 
  @retval EFI_SUCCESS      No sec platform information ppi export   
  @retval EFI_SUCCESS      The boot mode is S3 path   
**/
EFI_STATUS
EFIAPI
BuildBistAndCpuInfoHob (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

#endif
