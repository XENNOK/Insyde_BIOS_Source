/** @file
 DXE Chipset Services driver.
 	
 This function is "deprecated"
 
***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <FastRecoveryData.h>

//[-start-131203-IB06720240-remove]//
//static PEI_FV_DEFINITION mPeiFvs[] = 
//{
//  { FixedPcdGet32(PcdFlashFvRecoveryBase), 
//    FixedPcdGet32(PcdFlashFvRecoverySize)
//  }
//};
//
////[-start-130724-IB07390110-add]//
//static EFI_GUID mChipsetEmuPeiMaskTable[] =
//{
//  //
//  // End of list
//  //
//  {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}
//};
////[-end-130724-IB07390110-add]//
//[-end-131203-IB06720240-remove]//

//[-start-130724-IB07390110-modify]//
/**
 Get information of recovery FV.

 @param[out]        RecoveryRegion        recovery region table
 @param[out]        NumberOfRegion        number of recovery region
 @param[out]        ChipsetEmuPeiMaskList Specified PEIMs don't to dispatch in recovery mode
                    
 @retval            EFI_SUCCESS           This function always return success.
**/
EFI_STATUS
FvRecoveryInfo (
  OUT  PEI_FV_DEFINITION       **RecoveryRegion,
  OUT  UINTN                    *NumberOfRegion,
  OUT  EFI_GUID                **ChipsetEmuPeiMaskList
  )
{
//[-start-131203-IB06720240-remove]//
//  *RecoveryRegion = mPeiFvs;
//  *NumberOfRegion = sizeof(mPeiFvs) / sizeof(PEI_FV_DEFINITION);
//  *ChipsetEmuPeiMaskList = mChipsetEmuPeiMaskTable;
//[-end-131203-IB06720240-remove]//
  
//[-start-131203-IB06720240-modify]//
  return EFI_UNSUPPORTED;
//[-end-131203-IB06720240-modify]//
}
//[-end-130724-IB07390110-modify]//