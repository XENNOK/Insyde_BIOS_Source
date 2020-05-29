/** @file
 Provide some misc subfunctions

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <PchAccess.h>
#include <PchRegs.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/S3BootScriptLib.h>
//[-start-130620-IB05670176-remove]//
//#include <Compatibility/FlashMap.h>
//[-end-130620-IB05670176-modify]//
#include "BiosRegionLockInfo.h"
#include "BiosRegionLockHelpFun.h"


#define B_PCH_SPI_PRB_MASK        0x00001FFF

/**
 This function sort the BIOS regions described in BiosLock record array.

 @param[in, out]    Array          Private protected BIOS region record.
 @param[in]         Length         The length of input Array.

*/
STATIC
VOID
Sort (
  IN OUT BIOS_REGION_LOCK_BASE *Array,
  IN     UINTN                  Length
  )
{
  UINTN Index1;
  UINTN Index2;
  UINTN Swap;
  
  for (Index1 = 0; Index1 < Length; ++Index1) {
    for (Index2 = Index1 + 1; Index2 <= Length; ++Index2) {
      if (Array[Index1].Base < Array[Index2].Base) {
        Swap = Array[Index1].Base;
        Array[Index1].Base = Array[Index2].Base;
        Array[Index2].Base = Swap;
        
        Swap = Array[Index1].Length;
        Array[Index1].Length = Array[Index2].Length;
        Array[Index2].Length = Swap;  
      }
    }
  }
}

/**
 This function merges the requested BIOS region to the private protected BIOS region record.

 @param[in]         BiosLock            Private protected BIOS region record.
 @param[in]         BaseAddress         The start address of the BIOS region which need to be merged.
 @param[in]         Length              The Length of the BIOS region which need to be merged.

 @retval EFI_OUT_OF_RESOURCES The max number of BIOS protect regions have been reached and the requested region 
                              can not be merge to existing protected region.
 @retval EFI_SUCCESS          Merge successfully
*/
EFI_STATUS
MergeToBase (
  IN BIOS_REGION_LOCK_BASE  *BiosLock,
  IN UINTN                   Base,
  IN UINTN                   Length
  )
{
  INTN                  Index1;
  INTN                  Index2;
  UINTN                 Top1;
  UINTN                 Top2;
  BIOS_REGION_LOCK_BASE LockTemp[MAX_BIOS_REGION_LOCK + 1];

//[-start-121211-IB06460478-modify]//
  ZeroMem ((VOID *)LockTemp, sizeof(BIOS_REGION_LOCK_BASE) * (MAX_BIOS_REGION_LOCK + 1));
  CopyMem ((VOID *)LockTemp, (VOID *)BiosLock, sizeof(BIOS_REGION_LOCK_BASE) * MAX_BIOS_REGION_LOCK);
//[-end-121211-IB06460478-modify]//

  for (Index1 = 0; Index1 <= MAX_BIOS_REGION_LOCK; ++Index1) {
    if (LockTemp[Index1].Base == 0) {
      LockTemp[Index1].Base   = Base;
      LockTemp[Index1].Length = Length;
      break;
    } 
  }
  
#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nMergeToBase\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Array Status (Before Sort)....\n"));
  for (Index1 = 0; Index1 <= MAX_BIOS_REGION_LOCK; ++Index1) {
    Top1 = LockTemp[Index1].Base + LockTemp[Index1].Length - 1;
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x Top:%8x\n", Index1 + 1,  LockTemp[Index1].Base, LockTemp[Index1].Length, Top1) );
  }
#endif

  Sort (LockTemp, MAX_BIOS_REGION_LOCK);

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nArray Status (After Sort)....\n"));
  for (Index1 = 0; Index1 <= MAX_BIOS_REGION_LOCK; ++Index1) {
    Top1 = LockTemp[Index1].Base + LockTemp[Index1].Length - 1;
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x Top:%8x\n", Index1 + 1,  LockTemp[Index1].Base, LockTemp[Index1].Length, Top1) );
  }
#endif

  for (Index1 = MAX_BIOS_REGION_LOCK - 1; Index1 >= 0; --Index1) {
    Index2 = Index1 + 1;
    if (LockTemp[Index2].Base == 0) {
      continue;
    }
    
    Top1 = LockTemp[Index1].Base + LockTemp[Index1].Length - 1;
    Top2 = LockTemp[Index2].Base + LockTemp[Index2].Length - 1;
    
    if (LockTemp[Index2].Base == LockTemp[Index1].Base) {
      if (LockTemp[Index2].Length > LockTemp[Index1].Length) {
        LockTemp[Index1].Length = LockTemp[Index2].Length;
      }
      
      LockTemp[Index2].Base    = 0;
      LockTemp[Index2].Length  = 0;
    } else if ((LockTemp[Index1].Base - 1) <= Top2) {
      if (Top2 > Top1) {
        LockTemp[Index1].Base   = LockTemp[Index2].Base;
        LockTemp[Index1].Length = LockTemp[Index2].Length;
      } else {
        LockTemp[Index1].Base   = LockTemp[Index2].Base;
        LockTemp[Index1].Length = Top1 - LockTemp[Index2].Base + 1;
      }
      
      LockTemp[Index2].Base   = 0;
      LockTemp[Index2].Length = 0;
    }
  }

  Sort (LockTemp, MAX_BIOS_REGION_LOCK);

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nArray Status (After Merge)....\n"));
  for (Index1 = 0; Index1 <= MAX_BIOS_REGION_LOCK; ++Index1) {
    Top1 = LockTemp[Index1].Base + LockTemp[Index1].Length - 1;
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x Top:%8x\n", Index1 + 1,  LockTemp[Index1].Base, LockTemp[Index1].Length, Top1));
  }
#endif

  if (LockTemp[MAX_BIOS_REGION_LOCK].Base != 0) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-start-121211-IB06460478-add]//
  CopyMem ((VOID *)BiosLock, (VOID *)LockTemp, sizeof (BIOS_REGION_LOCK_BASE) * MAX_BIOS_REGION_LOCK);
//[-end-121211-IB06460478-add]//

  return EFI_SUCCESS;
}

/**
 This function removes the requested BIOS region from the private protected BIOS region record.

 @param[in]         BiosLock            Private protected BIOS region record.
 @param[in]         BaseAddress         The start address of the BIOS region which need to be removed.
 @param[in]         Length              The Length of the BIOS region which need to be removed.

 @retval EFI_OUT_OF_RESOURCES The BIOS protect region registers are not enough to set for all discontinuous BIOS region
 @retval EFI_SUCCESS          Remove successfully
*/
EFI_STATUS
RemoveFromBase (
  IN BIOS_REGION_LOCK_BASE  *BiosLock,
  IN UINTN                   Base,
  IN UINTN                   Length
  )
{
  UINTN                 Index;
  UINTN                 Top1;
  UINTN                 Top2;
  BIOS_REGION_LOCK_BASE LockTemp[MAX_BIOS_REGION_LOCK + 1];

//[-start-121211-IB06460478-modify]//
  CopyMem ((VOID *)LockTemp, (VOID *)BiosLock, sizeof(BIOS_REGION_LOCK_BASE) * MAX_BIOS_REGION_LOCK);
//[-end-121211-IB06460478-modify]//

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nRemoveFromBase\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Array Status (Before Remove)....\n"));
  for (Index = 0; Index <= MAX_BIOS_REGION_LOCK; ++Index) {
    Top1 = LockTemp[Index].Base + LockTemp[Index].Length - 1;
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x Top:%8x\n", Index + 1,  LockTemp[Index].Base, LockTemp[Index].Length, Top1));
  }
#endif

  for (Index = 0; Index < MAX_BIOS_REGION_LOCK; ++Index) {
    Top1 = LockTemp[Index].Base + LockTemp[Index].Length - 1;
    Top2 = Base + Length - 1;

    if (Top1 <= (Base - 1) || Top2 <= (LockTemp[Index].Base - 1)) {
      continue;
    }
    
    if (LockTemp[Index].Base >= Base) {
      if (Top1 <= Top2) {
        LockTemp[Index].Base   = 0;
        LockTemp[Index].Length = 0;

        if (Top1 == Top2) {
          break;
        }
      } else {
        LockTemp[Index].Base = Top2 + 1;
        LockTemp[Index].Length = Top1 - Top2;
      }
    } else {
      LockTemp[Index].Length = Base - LockTemp[Index].Base;
      if (Top1 == Top2) {
        break;
      } else if (Top1 > Top2) {
        LockTemp[MAX_BIOS_REGION_LOCK].Base   = Top2 + 1;
        LockTemp[MAX_BIOS_REGION_LOCK].Length = Top1 - Top2;
      }
    }
  }

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nArray Status (After Remove)....\n"));
  for (Index = 0; Index <= MAX_BIOS_REGION_LOCK; ++Index) {
    Top1 = LockTemp[Index].Base + LockTemp[Index].Length - 1;
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x Top:%8x\n", Index + 1,  LockTemp[Index].Base, LockTemp[Index].Length, Top1));
  }
#endif

  Sort (LockTemp, MAX_BIOS_REGION_LOCK);

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nArray Status (After Sort)....\n"));
  for (Index = 0; Index <= MAX_BIOS_REGION_LOCK; ++Index) {
    Top1 = LockTemp[Index].Base + LockTemp[Index].Length - 1;
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x Top:%8x\n", Index + 1,  LockTemp[Index].Base, LockTemp[Index].Length, Top1));
  }
#endif

  if (LockTemp[MAX_BIOS_REGION_LOCK].Base != 0) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-start-121211-IB06460478-modify]//
  CopyMem ((VOID *)BiosLock, (VOID *)LockTemp, sizeof(BIOS_REGION_LOCK_BASE) * MAX_BIOS_REGION_LOCK);
//[-end-121211-IB06460478-modify]//

  return EFI_SUCCESS;
}

/**
 This function programs the BIOS regions described in BiosLock record to the BIOS protect region registers.

 @param[in]         BiosLock            Private protected BIOS region record.

 @retval EFI_SUCCESS
*/
EFI_STATUS
ProgramRegister (
  IN BIOS_REGION_LOCK_BASE *BiosLock
  )
{
  UINTN      Index;
  UINT32     PRRegister;
  UINTN      RangeBase;
  UINTN      RangeLimit;
  UINTN      Offset;
  UINTN      BiosRegionStart;
//[-start-120118-IB08450031-add]//
  EFI_STATUS Status;
//[-end-120118-IB08450031-add]//  

  BiosRegionStart = (PchMmRcrb32 (R_PCH_SPI_BFPR) & B_PCH_SPI_PRB_MASK) << 12;
//[-start-130620-IB05670176-modify]//
  Offset          = PcdGet32 (PcdFlashAreaBaseAddress) - BiosRegionStart;
//[-end-130620-IB05670176-modify]//
  
#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nBIOS Region Lock Base ........\n"));
#endif

//[-start-120118-IB08450031-add]//
  //
  // Try to merge content in range registers.
  // 
  for (Index = 0; Index < MAX_BIOS_REGION_LOCK; ++Index) {  
    PRRegister = PchMmRcrb32 (R_PCH_SPI_PR0 + (Index << 2));
    if (PRRegister != 0) {
      RangeBase  = ((PRRegister & B_PCH_SPI_PR0_PRB_MASK) << 12) + Offset;
      RangeLimit = ((PRRegister & B_PCH_SPI_PR0_PRL_MASK) >> 4) + Offset;
      Status = MergeToBase(
                 BiosLock,
                 RangeBase,
                 RangeLimit - RangeBase + 0x1000 
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
//[-end-120118-IB08450031-add]//

//[-start-120118-IB08450031-modify]//
  for (Index = 0; Index < MAX_BIOS_REGION_LOCK; ++Index) {
    PRRegister = 0;
    
    if (BiosLock[Index].Base != 0) {
      RangeBase  = BiosLock[Index].Base - Offset;
      RangeLimit = RangeBase + BiosLock[Index].Length - 1;
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x BiosBase:%8x Limit:%0x\n", Index,  BiosLock[Index].Base, BiosLock[Index].Length, RangeBase, RangeLimit));
      
      RangeBase  = (RangeBase >> 12) & B_PCH_SPI_PR0_PRB_MASK;
      RangeLimit = (RangeLimit << 4) & B_PCH_SPI_PR0_PRL_MASK;
      PRRegister = (UINT32)(RangeBase | RangeLimit | B_PCH_SPI_PR0_WPE);
    }

//[-start-120217-IB07360166-modify]//
    PchMmRcrb32AndThenOr ((R_PCH_SPI_PR0 + (Index << 2)), 0x00, PRRegister);
//[-end-120217-IB07360166-modify]//
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (PCH_RCRB_BASE + R_PCH_SPI_PR0 + (Index << 2)),
      1,
      (VOID *) &PRRegister
      );
    
#ifdef EFI_DEBUG
    PRRegister = PchMmRcrb32 (R_PCH_SPI_PR0 + (Index << 2));
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "     Register:%8x\n", PRRegister));
#endif
  }
//[-end-120118-IB08450031-modify]//

  return EFI_SUCCESS;
}
