/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
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

Module Name: Cache.c

**/

#include "CpuPei.h"

/**
  Reset all the MTRRs to a known state.

  This function provides the PPI for PEI phase to Reset all the MTRRs to a known state(UC)
  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 

  @retval EFI_SUCCESS     All MTRRs have been reset successfully.  
                      
**/
EFI_STATUS
EFIAPI
PeiResetCacheAttributes (
  IN  EFI_PEI_SERVICES     **PeiServices,
  IN  PEI_CACHE_PPI        *This
  );

/**
  program the MTRR according to the given range and cache type.

  This function provides the PPI for PEI phase to set the memory attribute by program
  the MTRR according to the given range and cache type. Actually this function is a 
  wrapper of the MTRR libaray to suit the PEI_CACHE_PPI interface

  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 
  @param  MemoryAddress   Base Address of Memory to program MTRR.            
  @param  MemoryLength    Length of Memory to program MTRR.                  
  @param  MemoryCacheType Cache Type.   
  @retval EFI_SUCCESS             Mtrr are set successfully.        
  @retval EFI_LOAD_ERROR          No empty MTRRs to use.            
  @retval EFI_INVALID_PARAMETER   The input parameter is not valid. 
  @retval others                  An error occurs when setting MTTR.
                                          
**/
EFI_STATUS
EFIAPI
PeiSetCacheAttributes (
  IN  EFI_PEI_SERVICES         **PeiServices,
  IN  PEI_CACHE_PPI            *This,
  IN  EFI_PHYSICAL_ADDRESS     MemoryAddress,
  IN  UINT64                   MemoryLength,
  IN  EFI_MEMORY_CACHE_TYPE    MemoryCacheType
  );

PEI_CACHE_PPI   mCachePpi = {
  PeiSetCacheAttributes,
  PeiResetCacheAttributes
};


/**
  program the MTRR according to the given range and cache type.

  This function provides the PPI for PEI phase to set the memory attribute by program
  the MTRR according to the given range and cache type. Actually this function is a 
  wrapper of the MTRR libaray to suit the PEI_CACHE_PPI interface

  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 
  @param  MemoryAddress   Base Address of Memory to program MTRR.            
  @param  MemoryLength    Length of Memory to program MTRR.                  
  @param  MemoryCacheType Cache Type.   
  @retval EFI_SUCCESS             Mtrr are set successfully.        
  @retval EFI_LOAD_ERROR          No empty MTRRs to use.            
  @retval EFI_INVALID_PARAMETER   The input parameter is not valid. 
  @retval others                  An error occurs when setting MTTR.
                                          
**/
EFI_STATUS
EFIAPI
PeiSetCacheAttributes (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI             *This,
  IN EFI_PHYSICAL_ADDRESS      MemoryAddress,
  IN UINT64                    MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE     MemoryCacheType
  )
{

   RETURN_STATUS            Status;
   //
   // call MTRR libary function
   //
   Status = MtrrSetMemoryAttribute(
              MemoryAddress,
              MemoryLength,
              (MTRR_MEMORY_CACHE_TYPE) MemoryCacheType
            );
   return (EFI_STATUS)Status;
}
/**
  Reset all the MTRRs to a known state.

  This function provides the PPI for PEI phase to Reset all the MTRRs to a known state(UC)
  @param  PeiServices     General purpose services available to every PEIM.  
  @param  This            Current instance of Pei Cache PPI.                 

  @retval EFI_SUCCESS     All MTRRs have been reset successfully.  
                      
**/
EFI_STATUS
EFIAPI
PeiResetCacheAttributes (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI             *This
  )
{

   MTRR_SETTINGS  ZeroMtrr;
   
   //
   // reset all Mtrrs to 0 include fixed MTRR and variable MTRR
   //
   ZeroMem(&ZeroMtrr, sizeof(MTRR_SETTINGS));
   MtrrSetAllMtrrs(&ZeroMtrr);
   
   return EFI_SUCCESS;
}
