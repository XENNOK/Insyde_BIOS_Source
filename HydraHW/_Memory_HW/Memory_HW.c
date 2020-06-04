/** @file

  Memory Utility
  
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
  Memory Utility Handler.

  @param[in] *CpuIo             CPU_IO2 Portocol.
  @param[in]  *InputEx          Input_ex protocol.

**/
VOID
UtilityHandler (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  UINT8           MemoryTypeUsageItem;
  UINT8           MemoryAllocationServiceItem;
  UINT8           AllocationTypeItem;
  
  CHAR16          *BaseFrameTitle[] = { 
                    L"Memory Allocation Service",
                    L"Memory Type Usage",
                    L"Allocation Type"
                    };


  CHAR16          *MemoryTypeUsage[] = { 
                    L"EfiReservedMemoryType",
                    L"EfiLoaderCode",
                    L"EfiLoaderData",
                    L"EfiBootServicesCode",
                    L"EfiBootServicesData",
                    L"EfiRuntimeServicesCode",
                    L"EfiRuntimeServicesData",
                    L"EfiConventionalMemory",
                    L"EfiUnusableMemory",
                    L"EfiACPIReclaimMemory",
                    L"EfiACPIMemoryNVS",
                    L"EfiMemoryMappedIO",
                    L"EfiMemoryMappedIOPortSpace",
                    L"EfiPalCode"
                    };
  
  CHAR16          *MemoryAllocationService[] = { 
                    L"AllocatePages()",
                    L"AllocatePool()",
                    L"Memory Allocation Map"
                    };
    
  CHAR16          *AllocationType[] = { 
                    L"AllocateAnyPages",
                    L"AllocateMaxAddress",
                    L"AllocateAddress", 
                    };

  
  CHAR16          *BaseFrameTail[] = { 
                    L"Ctrl+F1 : Exit | Enter : Select Item,",
                    L"Up Botton: Select Previous Item",
                    L"Down Botton: Select Next Item"
                    };
  

  CHAR16          **BaseFrameTailPtr;

  MemoryTypeUsageItem = NO_SELECT;
  MemoryAllocationServiceItem = NO_SELECT;
  AllocationTypeItem = NO_SELECT;
  BaseFrameTailPtr = BaseFrameTail;

  for (;;) {

    //
    // Select Memory Allocation Service
    //
    if (MemoryTypeUsageItem == NO_SELECT) {     
      MemoryAllocationServiceItem = SelectItems (
                                      InputEx,
                                      3,
                                      BaseFrameTitle[0],
                                      MemoryAllocationService,
                                      BaseFrameTailPtr
                                      );

    }
    if (MemoryAllocationServiceItem == NO_SELECT) {
      break;
    } else if (MemoryAllocationServiceItem == MEMORY_ALLOCATION_MAP){
      //
      // TODO: MEMORY_ALLOCATION_MAP
      //
      MemAllocMap (
        InputEx
        );
    }
    
    //
    // Select Memory Type Usage
    //
    if ((AllocationTypeItem == NO_SELECT) &&
        !(MemoryAllocationServiceItem == MEMORY_ALLOCATION_MAP)) {
      MemoryTypeUsageItem = SelectItems (
                              InputEx,
                              14,
                              BaseFrameTitle[1],
                              MemoryTypeUsage,
                              BaseFrameTailPtr
                              );
    }
    
    if (MemoryTypeUsageItem == NO_SELECT) {
      continue;
    } else if ((MemoryTypeUsageItem == EfiConventionalMemory) ||
               (MemoryTypeUsageItem == EfiReservedMemoryType)) {
      CLEAN_SCREEN (VOID);
      SetCursorPosColor (
        EFI_WHITE | EFI_BACKGROUND_RED,
        0,
        5
        );
      if (MemoryTypeUsageItem == EfiConventionalMemory) {
        Print ( L"Do Not Selectd EfiConventionalMemory !!\n\n");
      } else {
        Print (L"Can Not Used EfiReservedMemoryType in Application !!\n\n");
      }
      Print (L"Back to Memory Allocation Service selection");
      
      SetCursorPosColor (
        EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
        0,
        0
        );
      
      TimeOut ();
      continue;
    } else if (MemoryTypeUsageItem == EfiUnusableMemory) {
      CLEAN_SCREEN (VOID);
      SetCursorPosColor (
        EFI_WHITE | EFI_BACKGROUND_RED,
        0,
        5
        );
      Print (L"###WARNING : EfiUnusableMemory may NOT trustable###\n\n");
      Print (L"Continue to AllocationType selection...");
      
      SetCursorPosColor (
        EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
        0,
        0
        );
      
      TimeOut ();
    }

    if (MemoryAllocationServiceItem == EnumAllocatePages) {
      //
      //  Select Allocate Pages Type
      //
 
      AllocationTypeItem = SelectItems (
                             InputEx,
                             3,
                             BaseFrameTitle[2],
                             AllocationType,
                             BaseFrameTailPtr
                             );
      if (AllocationTypeItem == NO_SELECT) {
        continue;
      }
      
      //
      // AllocatePage
      //
      AllocatePageMemory (
        CpuIo,
        InputEx,
        MemoryTypeUsageItem,
        MemoryAllocationServiceItem,
        AllocationTypeItem,
        MemoryTypeUsage,
        MemoryAllocationService,
        AllocationType
        ); 

    } else if (MemoryAllocationServiceItem == EnumAllocatePool) {
      //
      // AllocatePool
      //
      AllocatePoolMemory (
        CpuIo,
        InputEx,
        MemoryTypeUsageItem,
        MemoryAllocationServiceItem,
        MemoryTypeUsage,
        MemoryAllocationService
        ); 
    }
  }  
}

/**
  Memory Utility Entry Point.

  @param[in] ImageHnadle        While the driver image loaded be the ImageLoader(), an image handle is assigned to this
                                driver binary, all activities of the driver is tied to this ImageHandle.
  @param[in] SystemTable        A pointer to the system table, for all BS(Boot Services) and RT(Runtime Services).
 
  @return Status Code

**/
EFI_STATUS
EFIAPI
H_BasePages (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                             Status;

  UINTN    TextModeNum;
  UINTN    Columns;
  UINTN    Rows;
  INT32    OringinalMode;
  UINTN    OrgColumns;
  UINTN    OrgRows;
  
  EFI_CPU_IO2_PROTOCOL                   *CpuIo;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  
  //
  // Check Console Mode, save orignal mode and Set specification Mode
  //
  OringinalMode = gST->ConOut->Mode->Mode;
  
  ChkTextModeNum (
    DEF_COLS,
    DEF_ROWS,
    &TextModeNum
    );
  
  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          TextModeNum
                          );
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  //
  // Locate Simple Text Input Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );

  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  //
  // Locate CpuIo2 Protocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiCpuIo2ProtocolGuid,
                  NULL,
                  &CpuIo
                  );
  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiCpuIo2ProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }


  //
  // Memory Utility Handler
  //
  UtilityHandler (
    CpuIo,
    InputEx
    );

  //
  // Reset to Orginal Mode
  //
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }
  
  return EFI_SUCCESS;
}
