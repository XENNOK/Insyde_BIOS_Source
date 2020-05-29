/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++


Module Name:

 RestoreMtrr.c


--*/

//[-start-120712-IB10820084-modify]//	
#include <Library/HobLib.h>
#include <ChipsetInit.h>
#include <CpuRegs.h>
#include <Library/BaseLib.h>
//[-end-120712-IB10820084-modify]//	

/**

  If plug memory > 16GB, Bios will calculate the MTRR setting value and push this value in Hob at PEI phase, 
  Here the Bios will get the MTRR setting value from Hob to replace present MTRR value to the > 16GB setting.  

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE pointer

  @retval 

**/
EFI_STATUS
EFIAPI
RestoreMtrrEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  UINT32                  NumberOfMtrr;
  UINT64                  TotalMemorySize = 0;
  UINT32                  Index;
  VOID                    *HobList = NULL;
  UINT8                   *MtrrRestoreData; 
 
  HobList = GetHobList ();
  MtrrRestoreData = GetNextGuidHob ((CONST EFI_GUID*)&gMtrrDataInfoGuid, HobList);

  if (MtrrRestoreData != NULL) {	
    TotalMemorySize = ((HOB_MTRR_RESTORE_DATA*)GET_GUID_HOB_DATA(MtrrRestoreData))->TotalMemorySize;
                       
    if (TotalMemorySize > MEM_EQU_16GB) {
    	
      AsmWbinvd ();
	  
      NumberOfMtrr = (UINT32)((AsmReadMsr64 (IA32_MTRR_CAP) & 0xFF) - 1);
      
      for (Index = 0; Index <=NumberOfMtrr; Index++) {
//[-start-140625-IB05080432-add]//
        if (Index >= 20) break;
//[-end-140625-IB05080432-add]//
        AsmWriteMsr64((UINT32)(CACHE_VARIABLE_MTRR_BASE + 1 + (Index * 2)), 0);	
        AsmWriteMsr64((UINT32)(CACHE_VARIABLE_MTRR_BASE + (Index * 2)), 0);       
        //
        // Restore MTRR.
        //    
        AsmWriteMsr64((UINT32)(CACHE_VARIABLE_MTRR_BASE + (Index * 2)), ((HOB_MTRR_RESTORE_DATA*)GET_GUID_HOB_DATA(MtrrRestoreData))->MtrrBase[Index]);
        AsmWriteMsr64((UINT32)(CACHE_VARIABLE_MTRR_BASE + 1 + (Index * 2)), ((HOB_MTRR_RESTORE_DATA*)GET_GUID_HOB_DATA(MtrrRestoreData))->MtrrSize[Index]);
        
      }
    }
  }
  return EFI_SUCCESS;
}
