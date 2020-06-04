/** @file

  Memory Utility Allocate Page Memory.
  
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
  Allocate Page Memory.

  @param[in] *CpuIo                           CPU_IO2 Portocol.
  @param[in] *InputEx                         Key event, text input_ex protocol.
  @param[in] MemoryTypeUsageItem              Selected Memory Type Usage.
  @param[in] MemoryAllocationServiceItem      Selected Memory Allocation Service.
  @param[in] AllocationTypeItem               Selected Allocation Type.
  @param[in] **MemoryTypeUsage                Memory Type Usage string array.
  @param[in] **MemoryAllocationService        Memory Allocation Service string array.
  @param[in] **AllocationType                 Allocation Type string array.

**/
EFI_STATUS
AllocatePageMemory (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  MemoryTypeUsageItem,
  IN UINT8                                  MemoryAllocationServiceItem,
  IN UINT8                                  AllocationTypeItem,
  IN CHAR16                                 **MemoryTypeUsage,
  IN CHAR16                                 **MemoryAllocationService,
  IN CHAR16                                 **AllocationType
  )
{
  EFI_STATUS      Status;
  EFI_KEY_DATA    HKey;
  UINTN           PagesNum;
  UINTN           PageAddress;
  UINTN           BaseLineAddress;
  UINTN           LimitLineAddress;
  CHAR16          *BaseFrameTitle[] = { 
                    L"Allocate Page Memory"
                    };
  
  CHAR16          *BaseFrameTail[] = { 
                    L"Ctrl+Left : Exit | Enter : Enter Value,",
                    L"Up Botton: Select Previous Address | F1 : Set Memory",
                    L"Down Botton: Select Next Address"
                    };
  UINT8           SetMemValue;
  UINTN           SetMemSize;
  
  PagesNum = 0;
  PageAddress = 0;
  SetMemValue = 0;
  SetMemSize = 0;
  
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

  SetCursorPosColor (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
    0,
    3
    );
  
  PageRightDataFrame (
    InputEx,
    MemoryTypeUsage[MemoryTypeUsageItem],
    MemoryAllocationService[MemoryAllocationServiceItem],
    AllocationType[AllocationTypeItem],
    &PagesNum,
    &PageAddress
  );
  
  Status = gBS->AllocatePages (
                  (EFI_ALLOCATE_TYPE) AllocationTypeItem,
                  (EFI_MEMORY_TYPE) MemoryTypeUsageItem,
                  PagesNum,
                  (EFI_PHYSICAL_ADDRESS*) &PageAddress
                  );
  if (EFI_ERROR (Status)){
    return Status;
  }
  
  BaseLineAddress = PageAddress;
  LimitLineAddress = (PageAddress + (PagesNum * 0x1000));

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
      PageAddress
      );
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY,
      RIGHT_DATA_FRAME_HORI_MIN,
      (BASE_TITLE_OFFSET + 7)
      );
    Print (
      L"Address To : 0x%08X",
      (PageAddress + 0x100)
      );
    
    //
    // show Data Frame.
    //
    DataFrame ( (UINT8*) PageAddress);

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
        PageAddress = (VOID *)(PageAddress - 0x100);
        if (PageAddress < BaseLineAddress){
          PageAddress = BaseLineAddress;
        }
        break;
 		  case SCAN_DOWN:
        PageAddress = (VOID *)(PageAddress + 0x100);
        if (PageAddress >= LimitLineAddress) {
          PageAddress = LimitLineAddress - 0x100;
        }
        break;
      case SCAN_F1:
        //
        // Set Memory Value
        //
        SetCursorPosColor (
          EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
          RIGHT_DATA_FRAME_HORI_MIN,
          (BASE_TITLE_OFFSET + 9)
          );
        Print (
          L"SetMem : 0x%02X",
          SetMemValue
          );
        SetMemValue = (UINT8)HDataWrite (
                               InputEx,
                               1,
                               (RIGHT_DATA_FRAME_HORI_MIN + 11),
                               (BASE_TITLE_OFFSET + 9)
                               );
        //
        // Set how many byte you want to SetMem
        //
        SetCursorPosColor (
          EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
          RIGHT_DATA_FRAME_HORI_MIN,
          (BASE_TITLE_OFFSET + 10)
          );
        Print (
          L"Size : 0x%04X",
          SetMemSize
          );
        SetMemSize = (UINTN)HDataWrite (
                              InputEx,
                              2,
                              (RIGHT_DATA_FRAME_HORI_MIN + 9),
                              (BASE_TITLE_OFFSET + 10)
                              );
        gBS->SetMem (
               PageAddress,
               SetMemSize,
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
  Status = gBS->FreePages (
                  PageAddress,
                  PagesNum
                  );
  if (EFI_ERROR (Status)){
    return Status;
  }

  return Status;
}

