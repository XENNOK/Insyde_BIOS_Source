//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/**
 
	Data definition among fast recovery modules.

**/

#ifndef __FAST_RECOVERY_DATA__
#define __FAST_RECOVERY_DATA__

#ifdef SIZE_OF_CAR
#define   EMU_CAR_STACK_SIZE   (SIZE_OF_CAR*1024)
#else
#define   EMU_CAR_STACK_SIZE   (64*1024)
#endif
#pragma pack(1)
typedef struct {
  //
  // Remain for fake stack
  //
  UINT8                   StackData[EMU_CAR_STACK_SIZE];
  
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
