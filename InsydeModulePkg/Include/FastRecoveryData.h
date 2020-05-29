/** @file
  	Data definition among fast recovery modules.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __FAST_RECOVERY_DATA__
#define __FAST_RECOVERY_DATA__

#include <Uefi.h>

typedef struct {
  EFI_PHYSICAL_ADDRESS  BaseAddress;
  UINT32                Length;
} PEI_FV_DEFINITION;

#pragma pack(1)
typedef struct {
  //
  // Remain for fake stack
  //
  EFI_PHYSICAL_ADDRESS    StackData;
  
  //
  // Data address
  //
  EFI_PHYSICAL_ADDRESS    EmuPeiFv;
  EFI_PHYSICAL_ADDRESS    EmuPeiFvBase;
  EFI_PHYSICAL_ADDRESS    EmuPeiEntry;
  EFI_PHYSICAL_ADDRESS    EmuSecEntry;
  EFI_PHYSICAL_ADDRESS    EmuSecFfsAddress;         // Will mask after we execute
  EFI_PHYSICAL_ADDRESS    RecoveryImageAddress;
  EFI_PHYSICAL_ADDRESS    OriginalHobList;
  EFI_PHYSICAL_ADDRESS    RomToRamDifference;
  
  //
  // Data values
  //
  UINT64                  EmuPeiFvSize;           // Refer Flashmap.h
  UINT64                  RecoveryImageSize;
  
} FAST_RECOVERY_DXE_TO_PEI_DATA;


#pragma pack()

#endif
