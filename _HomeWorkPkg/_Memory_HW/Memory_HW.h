/** @file

 Memory Utility Header

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


#ifndef _MEMORY_HW_H_
#define _MEMORY_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/CpuIo2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define DATA_NUM(x) ((x) * 2)-1

#define __DEBUG     
#define DEF_ROWS    25
#define DEF_COLS    80

#define NO_SELECT 255

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16

#define MEMORY_ALLOCATION_MAP 2

#define PAGE_SIZE 4096
#define MAP_ITEMS_PER_PAGE 16

typedef enum {
  EnumAllocateAnyPages = 0,
  EnumAllocateMaxAddress,
  EnumAllocateAddress,
  EnumMaxAllocateType
}E_ALLOCATIONTYPE;

typedef enum {
  EnumAllocatePages = 0,
  EnumAllocatePool
}E_MEMORYALLOCATIONSERVICE;

UINT8
SelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN CHAR16                                 *BaseFrametitle,
  IN CHAR16                                 **ItemLists,
  IN CHAR16                                 **BaseFrameTail
  );

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
  );

EFI_STATUS
AllocatePoolMemory (
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  MemoryTypeUsageItem,
  IN UINT8                                  MemoryAllocationServiceItem,
  IN CHAR16                                 **MemoryTypeUsage,
  IN CHAR16                                 **MemoryAllocationService
  );

UINT32
HDataWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TypeByte,
  IN UINT8                                  Hori,
  IN UINT8                                  Vert
  );

EFI_STATUS 
TimeOut (
  VOID
  );

VOID
PageRightDataFrame (
  IN     EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN     CHAR16                                 *MemoryTypeUsage,
  IN     CHAR16                                 *MemoryAllocationService,
  IN     CHAR16                                 *AllocationType,
  IN OUT UINTN                                  *PagesNum,
  IN OUT UINTN                                  *AddressNum OPTIONAL
  );

VOID
PoolRightDataFrame (
  IN     EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN     CHAR16                                 *MemoryTypeUsage,
  IN     CHAR16                                 *MemoryAllocationService,
  IN OUT UINTN                                  *PoolSize
  );
  
VOID
DataFrame (
  IN UINT8  *DataBuffer
  );

EFI_STATUS
MemAllocMap (
  VOID
  );
#endif
