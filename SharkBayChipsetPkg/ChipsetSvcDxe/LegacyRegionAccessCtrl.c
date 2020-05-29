/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcLegacyRegionAccessCtrl().
 The function DxeCsSvcLegacyRegionAccessCtrl() use chipset services to control
 legacy region can be access or not.
	
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/PciCf8Lib.h>
#include <Protocol/H2ODxeChipsetServices.h>
#include <SaRegs.h>

#define PCI_BASE_ADDRESS              0x80000000
#define SYSTEM_DECODER_BASE (((UINTN)PCI_BASE_ADDRESS + (UINTN)(SA_MC_BUS << 16 ) + \
                              (UINTN)(SA_MC_DEV << 11) + (UINTN)(SA_MC_FUN << 8) + (UINTN)R_SA_PAM0));

typedef enum {
  LegacyRegionSegment_F = 0,
  LegacyRegionSegment_C = 1,
  LegacyRegionSegment_D = 3,
  LegacyRegionSegment_E = 5,
  LegacyRegionSegment_All = 7
} LEGACY_REGION_SEGMENT;

LEGACY_REGION_SEGMENT mSegment[4] = {
                                      LegacyRegionSegment_C, 
                                      LegacyRegionSegment_D, 
                                      LegacyRegionSegment_E, 
                                      LegacyRegionSegment_F
                                    };
UINT8 mLockData[7] = {0x10, 0x11, 0x11, 0x11, 0x11, 0x33, 0x33};
UINT8 mUnLockData[7] = {0x30, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};

/**
 LegacyRegionManipulation is consumed by LegacyRegionAccessCtrl().

 @param[in]         Segment                
 @param[in]         IsWriteProtect         

 @retval            None
*/
STATIC
VOID
LegacyRegionManipulation (
  IN  LEGACY_REGION_SEGMENT          Segment,
  IN  BOOLEAN                        IsWriteProtect
  )
{ 
  UINT32                SystemDecoderBase;
  UINT8                 *Data;
  UINTN                 Index;

  Index = 0;
  SystemDecoderBase = SYSTEM_DECODER_BASE; 

  if (IsWriteProtect) {
    Data = mLockData;
  } else {
    Data = mUnLockData;
  }

  if (Segment == (UINTN)LegacyRegionSegment_F) {
    PciCf8Write8 (SystemDecoderBase, Data[(UINTN)Segment]);
  } else if (Segment == (UINTN)LegacyRegionSegment_All) {
    for (Index = 0; Index < (UINTN)Segment; Index++) {
      PciCf8Write8 ((SystemDecoderBase + (UINT32)Index), Data[Index]);
    }
  } else {
    PciCf8Write8 ((SystemDecoderBase + (UINT32)Segment), Data[(UINTN)Segment]);
    PciCf8Write8 ((SystemDecoderBase + (UINT32)Segment + 1), Data[(UINTN)(Segment + 1)]);
  }
  
  return;
}

/**
 Legacy Region Access Control. 
 
 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                LEGACY_REGION_ACCESS_LOCK or LEGACY_REGION_ACCESS_UNLOCK
                                        If LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl()
                                        lock the specific legacy region.
                    
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
EFI_STATUS
LegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  )
{ 
  UINTN                                     End;
  LEGACY_REGION_SEGMENT                     IndexStart;
  LEGACY_REGION_SEGMENT                     IndexEnd;
  UINTN                                     Index;
    
  if (Mode != LEGACY_REGION_ACCESS_UNLOCK && Mode != LEGACY_REGION_ACCESS_LOCK) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (Start < 0xC0000) {
    return EFI_INVALID_PARAMETER;
  }

  End = Start + Length - 1;
  Start = Start & 0xF0000;
  End = End & 0xF0000;

  if (End > 0xF0000) {
    return EFI_INVALID_PARAMETER;
  }

//[-start-140708-IB05080434-modify]//
  if (End < 0xC0000) {
    return EFI_INVALID_PARAMETER;
  }
//[-end-140708-IB05080434-modify]//

  Start = Start >> 16; 
  End = End >> 16;
  Start = Start - 0x0C;
//[-start-140625-IB05080432-add]//
  if (Start >= 4) {
    return EFI_INVALID_PARAMETER;
  }
//[-end-140625-IB05080432-add]//
  IndexStart = mSegment[Start];
  End = End - 0x0C;
//[-start-140625-IB05080432-add]//
  if (End >= 4) {
    return EFI_INVALID_PARAMETER;
  }
//[-end-140625-IB05080432-add]//
  IndexEnd = mSegment[End];

  if (IndexStart == IndexEnd) {
    LegacyRegionManipulation (IndexStart, (BOOLEAN)Mode);
  } else if (IndexStart == LegacyRegionSegment_C && IndexEnd == LegacyRegionSegment_F) {
    LegacyRegionManipulation (LegacyRegionSegment_All, (BOOLEAN)Mode);
  }else {
    for (Index = Start; Index <= End; Index++ ) {
      LegacyRegionManipulation (mSegment[Index], (BOOLEAN)Mode);
    }
  }

  return EFI_SUCCESS;
}
