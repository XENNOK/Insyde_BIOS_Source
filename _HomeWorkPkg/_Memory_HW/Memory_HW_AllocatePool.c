/** @file

  Memory Utility Allocate Pool Memory
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"Memory_HW.h"


/**
  Allocate Pool Memory.

  @param[in] *CpuIo                           CPU_IO2 Portocol.
  @param[in] *InputEx                         Key event, text input_ex protocol.
  @param[in] MemoryTypeUsageItem              Selected Memory Type Usage.
  @param[in] MemoryAllocationServiceItem      Selected Memor Allocation Service.
  @param[in] **MemoryTypeUsage                Memory Type Usage string array.
  @param[in] **MemoryAllocationService        Memory Allocation Service string array.

**/
EFI_STATUS
AllocatePoolMemory (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  MemoryTypeUsageItem,
  IN UINT8                                  MemoryAllocationServiceItem,
  IN CHAR16                                 **MemoryTypeUsage,
  IN CHAR16                                 **MemoryAllocationService
  )
{
  EFI_STATUS              Status;
  EFI_KEY_DATA            HKey;
  UINTN                   PoolSize;
  VOID                    *PoolAddress = NULL;
  VOID                    *BaseLineAddress = NULL;

  UINT8                   SetMemValue;
  
  CHAR16          *BaseFrameTitle[] = { 
                    L"Allocate Pool Memory"
                    };
  
  CHAR16          *BaseFrameTail[] = { 
                    L"Ctrl+Left : Exit | Enter : Enter Value,",
                    L"Up Botton: Select Previous Address | F1 : Set Memory",
                    L"Down Botton: Select Next Address"
                    };
  
  
  PoolSize = 0;
  SetMemValue = 0;
  CLEAN_SCREEN (VOID);
  //
  // Show Base Frame
  //
  BaseFrame (
    BaseFrameTitle[0],
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
  
  //
  // Show Right Frame Data and get Pool Size back.
  //
  PoolRightDataFrame (
    InputEx,
    MemoryTypeUsage [MemoryTypeUsageItem],
    MemoryAllocationService [MemoryAllocationServiceItem],
    &PoolSize
    );

  //
  // Allocate Pool.
  //
  Status = gBS->AllocatePool (
                  (EFI_MEMORY_TYPE) MemoryTypeUsageItem,
                  PoolSize,
                  &PoolAddress
                  );
  if (EFI_ERROR (Status)){
    return Status;
  }
  //
  // Set Data Frame Address Base Line.
  //
  BaseLineAddress = PoolAddress;

  //
  // loop for show data frame and set memory.
  //
  for (; ; ) {
    //
    // show data frame address start and end
    // in right data frame.
    //
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 6)
      );
    Print (
      L"Address From : 0x%08X",
      PoolAddress
      );
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 7)
      );
    Print (
      L"Address To : 0x%08X",
      ((UINT32*)PoolAddress + 64)
      );
    
    //
    // show Data Frame.
    //
    DataFrame ( (UINT8*) PoolAddress);

    //
    // Read key to change data frame
    // or set Memory
    //
    HKey = keyRead (
             InputEx
             );
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
 		  switch (HKey.Key.ScanCode) {
        
 		  case SCAN_UP:
        PoolAddress = (VOID *)((UINT32*)PoolAddress - 4);
        if (PoolAddress < BaseLineAddress){
          PoolAddress = BaseLineAddress;
        }
        break;
 		  case SCAN_DOWN:
        PoolAddress = (VOID *)((UINT32*)PoolAddress + 4);
        break;
      case SCAN_F1:
        SetCursorPosColor (
          EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
          RIGHT_DATA_FRAME_HORI_MIN,
          (BASE_TITLE_OFFSET + 8)
          );
        Print (
          L"SetMem : 0x%02X",
          SetMemValue
          );
        SetMemValue = (UINT8)HDataWrite (
                               InputEx,
                               1,
                               (RIGHT_DATA_FRAME_HORI_MIN + 11),
                               (BASE_TITLE_OFFSET + 8)
                               );
        
        gBS->SetMem (
               PoolAddress,
               PoolSize,
               SetMemValue
               );
        break;
 		  }
    } else if((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
      ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
       (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
      (HKey.Key.ScanCode == SCAN_LEFT)) {
      break;
    }
  }
  Status = gBS->FreePool (
                  PoolAddress
                  ); 
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
