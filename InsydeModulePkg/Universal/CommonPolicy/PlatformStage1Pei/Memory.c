/** @file
  Provide memory related information for MemoryInit driver.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Guid/MemoryTypeInformation.h>
#include <Ppi/PlatformMemoryRange.h>
#include <Ppi/PlatformMemorySize.h>


#include <Uefi/UefiMultiPhase.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/VariableLib.h>

/**
  Fill in bit masks to specify reserved memory ranges on the  platform

  @param[in]  PeiServices
  @param[in]  This
  @param[in]  OptionRomMask
  @param[in]  SmramMask
  @param[in]  GraphicsMemoryMask
  @param[in]  PciMemoryMask

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
PlatformChooseRanges (
  IN      EFI_PEI_SERVICES                      **PeiServices,
  IN      PEI_PLATFORM_MEMORY_RANGE_PPI         *This,
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  )
{


  //
  // Choose regions to reserve for Option ROM use
  //
  *OptionRomMask = PEI_MR_OPTION_ROM_NONE;

  //
  // Choose regions to reserve for SMM use
  //
  *SmramMask = PEI_MR_SMRAM_NONE;

  *GraphicsMemoryMask = PEI_MR_GRAPHICS_MEMORY_NONE;

  *PciMemoryMask = 0;

  PeiCsSvcModifyMemoryRange (
        OptionRomMask,
        SmramMask,
        GraphicsMemoryMask,
        PciMemoryMask
        );

  return EFI_SUCCESS;
}

/**
  Check the input memory type information is whether valid.

  @param[in] MemTypeInfo    Pointer to input EFI_MEMORY_TYPE_INFORMATION array
  @param[in] MemTypeInfoCnt The count of EFI_MEMORY_TYPE_INFORMATION instance.

  @retval TRUE              The input EFI_MEMORY_TYPE_INFORMATION is valid.
  @retval FALSE             Any of EFI_MEMORY_TYPE_INFORMATION instance in input array is invalid.
--*/
STATIC
BOOLEAN
IsMemoryTyepInfoValid (
  IN EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo,
  IN UINTN                             MemTypeInfoCnt
  )
{
  UINTN         Index;

  if (MemTypeInfo == NULL && MemTypeInfoCnt != 0) {
    return FALSE;
  }

  for (Index = 0; Index < MemTypeInfoCnt; Index++) {
    if ((MemTypeInfo[Index].NumberOfPages & 0x80000000) != 0 || MemTypeInfo[Index].Type > EfiMaxMemoryType) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
  Calculate the size of Page Directory and Page Table Entries for 
  1:1 Virtual to Physical mapping.

  @return The memory size of page table
  
**/
STATIC
UINTN
GetMemorySizeForCpuPageTable (
  VOID
  )
{
  UINT32        RegEax;
  UINT32        RegEdx;
  UINT8         PhysicalAddressBits;
  UINT32        NumberOfPml4EntriesNeeded;
  UINT32        NumberOfPdpEntriesNeeded;
  UINTN         TotalPagesNum;
  BOOLEAN       Page1GSupport;

  Page1GSupport = FALSE;
  if (PcdGetBool(PcdUse1GPageTable)) {
    AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
    if (RegEax >= 0x80000001) {
      AsmCpuid (0x80000001, NULL, NULL, NULL, &RegEdx);
      if ((RegEdx & BIT26) != 0) {
        Page1GSupport = TRUE;
      }
    }
  }
  //
  // Get physical address bits supported.
  //
  PhysicalAddressBits = FixedPcdGet8 (PcdMemorySpaceSize);
  //
  // IA-32e paging translates 48-bit linear addresses to 52-bit physical addresses.
  //
  if (PhysicalAddressBits > 48) {
    PhysicalAddressBits = 48;
  }
  //
  // Calculate the table entries needed.
  //
  if (PhysicalAddressBits <= 39 ) {
    NumberOfPml4EntriesNeeded = 1;
    NumberOfPdpEntriesNeeded = (UINT32)LShiftU64 (1, (PhysicalAddressBits - 30));
  } else {
    NumberOfPml4EntriesNeeded = (UINT32)LShiftU64 (1, (PhysicalAddressBits - 39));
    NumberOfPdpEntriesNeeded = 512;
  }
  //
  // Pre-allocate big pages to avoid later allocations. 
  //
  if (!Page1GSupport) {
    TotalPagesNum = (NumberOfPdpEntriesNeeded + 1) * NumberOfPml4EntriesNeeded + 1;
  } else {
    TotalPagesNum = NumberOfPml4EntriesNeeded + 1;
  }
  return TotalPagesNum * EFI_PAGE_SIZE;
}

/**
  Get platform need memory.

  @param[in]  PeiServices
  @param[in]  This
  @param[in]  MemorySize

  @retval

**/
EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN      EFI_PEI_SERVICES                       **PeiServices,
  IN      struct _PEI_PLATFORM_MEMORY_SIZE_PPI   *This,
  IN OUT  UINT64                                 *MemorySize
  )
{
  EFI_STATUS                        Status;
  UINTN                             Index;
  EFI_BOOT_MODE                     BootMode;
  VOID                              *HobPtr;
  EFI_MEMORY_TYPE_INFORMATION       *MemoryTypeInformation;
  EFI_MEMORY_TYPE_INFORMATION       MemoryData[EfiMaxMemoryType + 1];
  UINTN                             MemoryTableSize;
  //
  // Get Boot Mode
  //
  Status = (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);
  //
  // Get memory type information from variable 
  //
  MemoryTypeInformation = MemoryData;
  MemoryTableSize       = sizeof (MemoryData);
  Status = CommonGetVariable (
             EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
             &gEfiMemoryTypeInformationGuid,
             &MemoryTableSize,
             MemoryTypeInformation
             );
  //
  // Start with minimum memory
  //
  *MemorySize = (BootMode == BOOT_IN_RECOVERY_MODE) ? FixedPcdGet32(PcdPeiRecoveryMinMemorySize) : FixedPcdGet32(PcdPeiMinMemorySize);
  //
  // Accumulate the size of Ia32/x64 CPU page table
  //
#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)
  *MemorySize += GetMemorySizeForCpuPageTable ();
#endif
  //
  // Accumulate maximum amount of memory needed
  //
  if (EFI_ERROR (Status) || !IsMemoryTyepInfoValid (MemoryData, MemoryTableSize / sizeof (EFI_MEMORY_TYPE_INFORMATION))) {
    //
    // Get memory type information from PCD and updated by OemService
    //
    MemoryTypeInformation = (EFI_MEMORY_TYPE_INFORMATION *)PcdGetPtr (PcdPreserveMemoryTable);
    MemoryTableSize =  LibPcdGetSize (PcdToken (PcdPreserveMemoryTable));
    Status = OemSvcUpdateMemoryTypeInformation (&MemoryTypeInformation, (UINT32*)&MemoryTableSize);
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
  }
  for (Index = 0; Index < (MemoryTableSize / sizeof (EFI_MEMORY_TYPE_INFORMATION)); Index++) {
    *MemorySize += MemoryTypeInformation[Index].NumberOfPages * EFI_PAGE_SIZE;
  }
  //
  // Build the GUID HOB for DXE
  //
  HobPtr = BuildGuidDataHob (
             &gEfiMemoryTypeInformationGuid,
             MemoryTypeInformation,
             MemoryTableSize
             );
  if (HobPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

