/** @file

  Memory Utility - Memory Allocation map
  
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

  This code Show Memory Allocate Summary frame.
  
  @param[in]  *InputEx              Input_ex protocol.
  @param[in]  TotalItems            Memory map total numbers.
  @param[in]  *MemoryMap            EFI_MEMORY_DESCRIPTOR.
  @param[in]  DescriptorSize        Descriptor Size.

*/
VOID
MemAlloSummaryFrame (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN EFI_MEMORY_DESCRIPTOR                  *MemoryMap,
  IN UINTN                                  DescriptorSize
  )
{
  UINTN                       TempIndex;
  UINT8                       BackGroundIndex;
  EFI_KEY_DATA                HKey;
  EFI_MEMORY_DESCRIPTOR       *MMap = 0;

  CHAR16          *BaseFrameTitle[] = { 
                      L"MemAllocaMap Summarize"
                      };
  
  CHAR16          *BaseFrameTail[] = { 
                    L"F5 : Back to Memory Map",
                    L"",
                    L""
                    };

  UINT64              ReservedPages;
  UINT64              LoadCodePages;
  UINT64              LoadDataPages;
  UINT64              BSCodePages;
  UINT64              BSDataPages;
  UINT64              RTDataPages;
  UINT64              RTCodePages;
  UINT64              AvailPages;
  UINT64              AcpiReclaimPages;
  UINT64              AcpiNvsPages;
  UINT64              MmioSpacePages;
  UINT64              TotalPages;

  UINT64              ReservedPagesSize;
  UINT64              LoadCodePagesSize;
  UINT64              LoadDataPagesSize;
  UINT64              BSCodePagesSize;
  UINT64              BSDataPagesSize;
  UINT64              RTDataPagesSize;
  UINT64              RTCodePagesSize;
  UINT64              AvailPagesSize;
  UINT64              AcpiReclaimPagesSize;
  UINT64              AcpiNvsPagesSize;
  UINT64              MmioSpacePagesSize;
  UINT64              TotalPagesSize;

  ReservedPages = 0;
  LoadCodePages = 0;
  LoadDataPages = 0;
  BSCodePages = 0;
  BSDataPages = 0;
  RTDataPages = 0;
  RTCodePages = 0;
  AvailPages = 0;
  AcpiReclaimPages = 0;
  AcpiNvsPages = 0;
  MmioSpacePages = 0;
  TotalPages = 0;

  ReservedPagesSize = 0;
  LoadCodePagesSize = 0;
  LoadDataPagesSize = 0;  
  BSCodePagesSize = 0;
  BSDataPagesSize = 0;
  RTDataPagesSize = 0;
  RTCodePagesSize = 0;
  AvailPagesSize = 0;
  AcpiReclaimPagesSize = 0;
  AcpiNvsPagesSize = 0;
  MmioSpacePagesSize = 0;
  TotalPagesSize = 0;

  for( TempIndex = 0; TempIndex < TotalItems; TempIndex++)
  {
      MMap = (EFI_MEMORY_DESCRIPTOR*) ( ((CHAR8*)MemoryMap) + TempIndex * DescriptorSize);
      switch (MMap->Type) {
      case EfiReservedMemoryType:
        ReservedPages += MMap->NumberOfPages;
        break;
      case EfiLoaderCode:
        LoadCodePages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiLoaderData:
        LoadDataPages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiBootServicesCode:
        BSCodePages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiBootServicesData:
        BSDataPages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiRuntimeServicesCode:
        RTCodePages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiRuntimeServicesData:
        RTDataPages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiConventionalMemory:
        AvailPages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiUnusableMemory:
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiACPIReclaimMemory:
        AcpiReclaimPages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiACPIMemoryNVS:
        AcpiNvsPages += MMap->NumberOfPages;
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiMemoryMappedIO:
        MmioSpacePages += MMap->NumberOfPages;
        break;
      case EfiMemoryMappedIOPortSpace:
        TotalPages += MMap->NumberOfPages;
        break;
      case EfiPalCode:
        TotalPages += MMap->NumberOfPages;
        break;
      }
  }
  //
  // Calculation the Size
  //
  ReservedPagesSize		= MultU64x64(SIZE_4KB,ReservedPages);
  LoadCodePagesSize		= MultU64x64(SIZE_4KB,LoadCodePages);
  LoadDataPagesSize		= MultU64x64(SIZE_4KB,LoadDataPages);
  BSCodePagesSize		  = MultU64x64(SIZE_4KB,BSCodePages);
  BSDataPagesSize		  = MultU64x64(SIZE_4KB,BSDataPages);
  RTDataPagesSize		  = MultU64x64(SIZE_4KB,RTDataPages);
  RTCodePagesSize		  = MultU64x64(SIZE_4KB,RTCodePages);
  AvailPagesSize		  = MultU64x64(SIZE_4KB,AvailPages);
  TotalPagesSize		  = MultU64x64(SIZE_4KB,TotalPages);
  AcpiReclaimPagesSize     = MultU64x64(SIZE_4KB,AcpiReclaimPages);
  AcpiNvsPagesSize         = MultU64x64(SIZE_4KB,AcpiNvsPages);
  MmioSpacePagesSize       = MultU64x64(SIZE_4KB,MmioSpacePages);
  TotalPagesSize      = MultU64x64(SIZE_4KB,TotalPages);

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
    EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK,
    0,
    (BASE_TITLE_OFFSET + 1)
    );
  Print (L" Reserved  : %5d Pages (%d)\n",ReservedPages, ReservedPagesSize);
  Print (L" LoaderCode: %5d Pages (%d)\n",LoadCodePages, LoadCodePagesSize);
  Print (L" LoaderData: %5d Pages (%d)\n",LoadDataPages, LoadDataPagesSize);
  Print (L" BS_Code   : %5d Pages (%d)\n",BSCodePages, BSCodePagesSize);
  Print (L" BS_Data   : %5d Pages (%d)\n",BSDataPages, BSDataPagesSize);  
  Print (L" RT_Code   : %5d Pages (%d)\n",RTCodePages, RTCodePagesSize);
  Print (L" RT_Data   : %5d Pages (%d)\n",RTDataPages, RTDataPagesSize);
  Print (L" ACPIRec   : %5d Pages (%d)\n",AcpiReclaimPages, AcpiReclaimPagesSize);
  Print (L" ACPI_NVS  : %5d Pages (%d)\n",AcpiNvsPages, AcpiNvsPagesSize);
  Print (L" MMIO      : %5d Pages (%d)\n",MmioSpacePages, MmioSpacePagesSize);
  Print (L" Available : %5d Pages (%d)\n",AvailPages, AvailPagesSize);
  Print (L"Total Memory : %d MB    (%d)\n",DivU64x32(MultU64x64(SIZE_4KB,TotalPages), SIZE_1MB),TotalPagesSize);

  while (TRUE) {
    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.Key.ScanCode == SCAN_F5 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
          break;
    }
  }
}

/**

  Change memory type from UINT32 to string.

  @param[in] type             memory type.

*/
CHAR16 * 
TypeToStr (
  UINT32 type
  ) 
{
 CHAR16 *memory_types[] = { //10
         L"Reserved  ",
         L"LoaderCode",
         L"LoaderData",
         L"BS_Code   ",
         L"BS_Data   ",
         L"RT_Code   ",
         L"RT_Data   ",
         L"Available ",
         L"Unusable  ",
         L"ACPIRec   ",
         L"ACPI_NVS  ",
         L"MMIO      ",
         L"MMIOPort  ",
         L"PalCode   ",
         };
         
 if (type > sizeof(memory_types)/sizeof(CHAR16 *)) {
     return L"Unknown";
 }
 return memory_types[type];

}

/**

  This code Show Memory Allocate frame.
  
  @param[in]  *InputEx              Input_ex protocol.
  @param[in]  TotalItems            Memory map total numbers.
  @param[in]  *MemoryMap            EFI_MEMORY_DESCRIPTOR.
  @param[in]  DescriptorSize        Descriptor size.
  @param[in]  **BaseFrameTitle      Frame title.
  @param[in]  **BaseFrameTail       Frame tail.

*/
VOID
ListMemAlloPages (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN EFI_MEMORY_DESCRIPTOR                  *MemoryMap,
  IN UINTN                                  DescriptorSize,
  IN CHAR16                                 **BaseFrameTitle,
  IN CHAR16                                 **BaseFrameTail
  )
{
  EFI_KEY_DATA       HKey;
  UINT8              ItemIndex;
  UINT8              ItemCounts;
  UINT8              PageNumber;
  UINT8              ItemPosition;
  UINT8              MaxPages;
  UINT8              MaxPageItemNumber;
  UINT8              ItemsPerPage;
  UINT8              CleanLine;
  UINT8              SetSpace;

  EFI_MEMORY_DESCRIPTOR       *MMap = 0;
  
  ItemsPerPage = MAP_ITEMS_PER_PAGE;
  PageNumber = 0;
  ItemPosition = 0;
  
  if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber = MAP_ITEMS_PER_PAGE;
  } else {
    MaxPages = (TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

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
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TITLE_OFFSET
    );
  Print (L"   Type   | #Pages |   Start   <->    End    |Attribute       \n");  //45
          
  while (1) {
    
    EN_CURSOR (
      FALSE
      );
    
    ItemCounts = PageNumber * MAP_ITEMS_PER_PAGE;
    
    if (PageNumber == (MaxPages  -1)) {
      ItemsPerPage = MaxPageItemNumber;
    } else {
      ItemsPerPage = MAP_ITEMS_PER_PAGE;
    }
    
    for (ItemIndex = 0; ItemIndex < ItemsPerPage; ItemIndex++) {
      
      SetCursorPosColor (
        EFI_LIGHTGRAY,
        0,
        (ItemIndex + BASE_TITLE_OFFSET + 1)
        );

      //
      //  Show Memory Map
      //
      MMap = (EFI_MEMORY_DESCRIPTOR*) ( ((CHAR8*)MemoryMap) + ((ItemCounts + ItemIndex) * DescriptorSize));
      Print (
        L"%s|%08X|%010lX <-> %010lX|%016lX",
        TypeToStr (MMap->Type),
        MMap->NumberOfPages,
        MMap->PhysicalStart,
        (MMap->PhysicalStart) + ((MMap->NumberOfPages * 0x1000) - 1),
        MMap->Attribute
        );

      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (ITEMS_PER_PAGE-1); CleanLine > ItemsPerPage; CleanLine--) {
          SetCursorPosColor (
            EFI_LIGHTGRAY,
            0,
            (CleanLine + BASE_TITLE_OFFSET)
            );
          
            Print (
              L"                                                                ")
              ;
        }
      }
    }
    
    //
    // Read key up / down to change page
    //          F5 to show Summarize
    //          ctrl + Left to exit
    //
    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        ItemPosition -= MAP_ITEMS_PER_PAGE;
        
        if (ItemPosition == 0xF0) {
          
          PageNumber --;
          
          if (PageNumber == 0xFF) {
            PageNumber = MaxPages-1;
            ItemPosition = 0; 
          }else{
            ItemPosition = 0; 
          }
        }
			  break;
			case SCAN_DOWN:
        ItemPosition += MAP_ITEMS_PER_PAGE;
        
        if (ItemPosition >= ItemsPerPage) {
          ItemPosition=0;
          PageNumber++;
          
          if (PageNumber >= MaxPages) {
            PageNumber = 0;
          }
        }				  
			  break;
      case SCAN_F5:
        //
        // show summarize
        //
        MemAlloSummaryFrame (
          InputEx,
          TotalItems,
          MemoryMap,
          DescriptorSize
          );
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
          EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
          0,
          BASE_TITLE_OFFSET
          );
        Print (L"   Type   | #Pages |   Start   <->    End    |Attribute       \n");  //45
          
        break;

		  }
    }else if((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
      ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
       (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
      (HKey.Key.ScanCode == SCAN_LEFT)) {
       
		  SET_CUR_POS (
        0,
        BASE_FRAME_OFFSET
        );
      
      Print (
        L"\n"
        );
      
			break;
		}
  }
}

/**
  Get Memory Map and show.
  
  @param[in]  *InputEx              Input_ex protocol.
  
**/

EFI_STATUS
MemAllocMap (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
    EFI_STATUS                  Status;
    UINTN                       MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR       *MemoryMap = 0;
    UINTN                       MapKey;
    UINTN                       DescriptorSize;
    UINT32                      DescriptorVersion;
    
    EFI_MEMORY_DESCRIPTOR       *MMap = 0;

    UINTN                       TotalItems;

    CHAR16          *BaseFrameTitle[] = { 
                      L"Memory Allcation Map"
                      };
  
    CHAR16          *BaseFrameTail[] = { 
                      L"Ctrl+Left : Exit | F5 : List Summary / Memory Map",
                      L"Up Botton: Select Previous Address",
                      L"Down Botton: Select Next Address"
                      };
    CHAR16          **BaseFrameTitlePtr;
    CHAR16          **BaseFrameTailPtr;

    BaseFrameTitlePtr = BaseFrameTitle;
    BaseFrameTailPtr = BaseFrameTail;
    MemoryMapSize = 0;
    MapKey = 0;
    DescriptorSize = 0;
    DescriptorVersion = 0;

    Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );
   
    do  {
      MemoryMapSize += PAGE_SIZE;
      Status = gBS->AllocatePool (EfiLoaderData, MemoryMapSize, (void**)&MemoryMap);
      if (EFI_ERROR (Status)) {
        break;
      }
      Status = gBS->GetMemoryMap (
                      &MemoryMapSize,
                      MemoryMap,
                      &MapKey,
                      &DescriptorSize,
                      &DescriptorVersion
                      );
      if (Status == EFI_SUCCESS) {
        break;
      }
      Status = gBS->FreePool(MemoryMap);
    } while (Status == EFI_BUFFER_TOO_SMALL);

    if (EFI_ERROR(Status)) {
      return Status;
    }
    
    TotalItems = MemoryMapSize / (DescriptorSize);

    
    ListMemAlloPages (
      InputEx,
      TotalItems,
      MemoryMap,
      DescriptorSize,
      BaseFrameTitlePtr,
      BaseFrameTailPtr
      );

    Status = gBS->FreePool(MemoryMap);
    return Status;
}

