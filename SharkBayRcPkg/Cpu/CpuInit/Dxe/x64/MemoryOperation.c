/** @file
  Memory Operation Functions for IA32 Architecture.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "CpuInitDxe.h"
#include "CpuLib.h"
#include "MpCommon.h"
#include "VirtualMemory.h"
#include "MemoryAttribute.h"
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

VOID
InitializeExternalVectorTablePtr (
  EFI_CPU_INTERRUPT_HANDLER*VectorTable
  );

extern EFI_CPU_INTERRUPT_HANDLER  mExternalVectorTable[];
extern EFI_PHYSICAL_ADDRESS       mBackupBuffer;

UINT8                             *mPageStore     = NULL;
UINTN                             mPageStoreSize  = 16;
UINTN                             mPageStoreIndex = 0;

UINT64                            mValidMtrrAddressMask;
UINT64                            mValidMtrrBitsMask;

///
/// BugBug: Non Portable
///
#if defined (__GNUC__)
#define ALIGN_16BYTE_BOUNDRY  __attribute__ ((aligned (16)))
#else
#define ALIGN_16BYTE_BOUNDRY  __declspec (align (16))
#endif

#pragma pack(1)
typedef struct {
  UINT16  LimitLow;
  UINT16  BaseLow;
  UINT8   BaseMiddle;
  UINT8   Attributes1;
  UINT8   Attributes2;
  UINT8   BaseHigh;

  ///
  /// UINT32                     BaseAddressHighest32;
  /// UINT32                     Reserved;
  ///
} SEGMENT_DESCRIPTOR_x64;

typedef struct {
  UINT16  Limit;
  UINTN   Base;
} PSEUDO_DESCRIPTOR_x64;

#pragma pack()

ALIGN_16BYTE_BOUNDRY SEGMENT_DESCRIPTOR_x64 gGdt[] = {
  {    /// NULL Selector: selector[0]
    0, /// limit 15:0
    0, /// base  15:0
    0, /// base  23:16
    0, ///
    0, /// type & limit 19:16
    0, /// base  31:24
    ///   0,  /// base  63:32
    ///   0   /// reserved
    ///
  },
  {         /// Linear Selector: selector[8]
    0xffff, /// limit 15:0
    0,      /// base  15:0
    0,      /// base  23:16
    0x93,   /// present, ring 0, data, expand-up writable
    0xcf,   /// type & limit 19:16
    0,      /// base  31:24
    ///   0,      /// base  63:32
    ///   0       /// reserved
    ///
  },
  {         /// Linear code Selector: selector[10]
    0xffff, /// limit 15:0
    0,      /// base  15:0
    0,      /// base  23:16
    0x9b,   /// present, ring 0, code, expand-up writable
    0xcf,   /// type & limit 19:16
    0,      /// base  31:24
    ///  0,      /// base  63:32
    ///  0       /// reserved
    ///
  },
  {         /// Compatibility mode data Selector: selector[18]
    0xffff, /// limit 15:0
    0,      /// base  15:0
    0,      /// base  23:16
    0x93,   /// type & limit 19:16
    0xcf,
    0,      /// base  31:24
    /// 0,      /// base  63:32
    /// 0       /// reserved
    ///
  },
  {         /// Compatibility code Selector: selector[20]
    0xffff, /// limit 15:0
    0,      /// base  15:0
    0,      /// base  23:16
    0x9b,   /// type & limit 19:16
    0xcf,
    0,      /// base  31:24
    ///  0,      /// base  63:32
    ///  0       /// reserved
    ///
  },
  {     /// Spare3 Selector: selector[28]
    0,  /// limit 15:0
    0,  /// base  15:0
    0,  /// base  23:16
    0,  /// type & limit 19:16
    0,  /// base  31:24
    0,
    ///
    ///  0,  /// base  63:32
    ///  0   /// reserved
    ///
  },
  {         /// 64-bit data Selector:selector[30]
    0xffff, /// limit 15:0
    0,      /// base  15:0
    0,      /// base  23:16
    0x93,   /// type & limit 19:16
    0xcf,
    0,      /// base  31:24
    ///  0,  /// base  63:32
    ///  0   /// reserved
    ///
  },
  {         /// 64-bit code Selector: selector[38]
    0xffff, /// limit 15:0
    0,      /// base  15:0
    0,      /// base  23:16
    0x9b,   /// type & limit 19:16
    0xaf,
    0,      /// base  31:24
    ///  0,  /// base  63:32
    ///  0   /// reserved
    ///
  },
  {     /// Spare3 Selector: selector[40]
    0,  /// limit 15:0
    0,  /// base  15:0
    0,  /// base  23:16
    0,  /// type & limit 19:16
    0,  /// base  31:24
    0,
    ///
    ///  0,  /// base  63:32
    ///  0   /// reserved
    ///
  }
};

ALIGN_16BYTE_BOUNDRY PSEUDO_DESCRIPTOR_x64 gGdtPseudoDescriptor = {
  sizeof (gGdt) - 1,
  (UINTN) gGdt
};

INTERRUPT_GATE_DESCRIPTOR                   gIdtTable[INTERRUPT_VECTOR_NUMBER] = { 0 };

ALIGN_16BYTE_BOUNDRY PSEUDO_DESCRIPTOR_x64 gLidtPseudoDescriptor = {
  sizeof (gIdtTable) - 1,
  (UINTN) gIdtTable
};

/**

  Init Global Descriptor table

  @param[in] None
 
**/
VOID
InitializeSelectors (
  VOID
  )

{
  CpuLoadGlobalDescriptorTable (&gGdtPseudoDescriptor);
}

VOID
AsmIdtVector00 (
  VOID
  );
  
/**

  Slick around interrupt routines.

  @param[in] None

**/
VOID
InitializeInterruptTables (
  VOID
  )
{
  UINT16                    CodeSegment;
  INTERRUPT_GATE_DESCRIPTOR *IdtEntry;
  UINT8                     *CurrentHandler;
  UINT32                    Index;

  CodeSegment     = CpuCodeSegment ();

  IdtEntry        = gIdtTable;
  CurrentHandler  = (UINT8 *) (UINTN) AsmIdtVector00;
  for (Index = 0; Index < INTERRUPT_VECTOR_NUMBER; Index++) {
    IdtEntry[Index].Offset15To0     = (UINT16) (UINTN) CurrentHandler;
    IdtEntry[Index].SegmentSelector = CodeSegment;
    IdtEntry[Index].Attributes      = INTERRUPT_GATE_ATTRIBUTE;
    ///
    /// 8e00;
    ///
    IdtEntry[Index].Offset31To16  = (UINT16) ((UINTN) CurrentHandler >> 16);
    IdtEntry[Index].Offset63To32  = (UINT32) ((UINTN) CurrentHandler >> 32);

    CurrentHandler += 0x8;
    ///
  }

  CpuLoadInterruptDescriptorTable (&gLidtPseudoDescriptor);

  return ;
}

/**

  Initialize memory attribute

  @param[in] None
 
**/
VOID
InitailizeCacheAttributes (
  VOID
  )

{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Page;
  EFI_CPUID_REGISTER    FeatureInfo;
  EFI_CPUID_REGISTER    FunctionInfo;
  UINT8                 PhysicalAddressBits;
  UINT32                MsrNum;
  UINT64                TempQword;
  UINT64                ComplementBits;
  UINT32                VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  ///
  /// Allocate 16 pages
  ///
  Status            = (gBS->AllocatePages)(AllocateAnyPages, EfiBootServicesData, mPageStoreSize, &Page);
  ASSERT_EFI_ERROR (Status);

  mPageStore = (UINT8 *) (UINTN) Page;

  ZeroMem (mPageStore, 0x1000 * mPageStoreSize);

  ///
  /// Check returned value of Eax for extended CPUID functions
  ///
  AsmCpuid (
    CPUID_EXTENDED_FUNCTION,
    &FunctionInfo.RegEax,
    &FunctionInfo.RegEbx,
    &FunctionInfo.RegEcx,
    &FunctionInfo.RegEdx
    );

  PhysicalAddressBits = 36;

  ///
  /// If CPU supports extended functions, get the Physical Address size by reading EAX[7:0]
  ///
  if (FunctionInfo.RegEax > CPUID_EXTENDED_FUNCTION) {
    AsmCpuid (
      CPUID_VIR_PHY_ADDRESS_SIZE,
      &FeatureInfo.RegEax,
      &FeatureInfo.RegEbx,
      &FeatureInfo.RegEcx,
      &FeatureInfo.RegEdx
      );
    PhysicalAddressBits = (UINT8) FeatureInfo.RegEax;
  }

  mValidMtrrBitsMask    = (((UINT64) 1) << PhysicalAddressBits) - 1;
  mValidMtrrAddressMask = mValidMtrrBitsMask & 0xfffffffffffff000;

  ComplementBits        = mValidMtrrBitsMask & 0xfffffff000000000;
  if (ComplementBits != 0) {
    ///
    PreMtrrChange ();
    for (MsrNum = CACHE_VARIABLE_MTRR_BASE;
         MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1);
         MsrNum += 2
        ) {
      TempQword = AsmReadMsr64 (MsrNum + 1);
      if ((TempQword & B_CACHE_MTRR_VALID) != 0) {
        ///
        /// MTRR Physical Mask
        ///
        TempQword = TempQword | ComplementBits;
        AsmWriteMsr64 (MsrNum + 1, TempQword);
      }
    }

    ///
    /// Enable Cache and set the corresponding MTRR bits
    ///
    PostMtrrChange ();
  }
}

/**

  Allocate zeroed pages

  @param[in] None

  @retval Pointer to the page buffer

**/
VOID *
AllocateZeroedPage (
  VOID
  )
{
  if (mPageStoreIndex >= mPageStoreSize) {
    ///
    /// We are out of space
    ///
    return NULL;
  }

  return (VOID *) (UINTN) &mPageStore[0x1000 * mPageStoreIndex++];
}

/**

  Convert 2MB page tables to 4KB page tables

  @param[in] PageAddress             - Page address to convert
  @param[in] PageDirectoryToConvert  - Page table that will be converted

**/
VOID
Convert2MBPageTo4KPages (
  IN      EFI_PHYSICAL_ADDRESS        PageAddress,
  IN OUT  x64_PAGE_TABLE_ENTRY        **PageDirectoryToConvert
  )
{
  UINTN                   Index;
  EFI_PHYSICAL_ADDRESS    WorkingAddress;
  x64_PAGE_TABLE_ENTRY_4K *PageTableEntry;
  x64_PAGE_TABLE_ENTRY    Attributes;

  ///
  /// Save the attributes of the 2MB table
  ///
  Attributes.Page2Mb.Uint64 = (*PageDirectoryToConvert)->Page2Mb.Uint64;

  ///
  /// Convert PageDirectoryEntry2MB into a 4K Page Directory
  ///
  PageTableEntry = AllocateZeroedPage ();
  if (PageTableEntry == NULL) {
    return ;
  }
  (*PageDirectoryToConvert)->Page2Mb.Uint64         = (UINT64) PageTableEntry;
  (*PageDirectoryToConvert)->Page2Mb.Bits.ReadWrite = 1;
  (*PageDirectoryToConvert)->Page2Mb.Bits.Present   = 1;

  WorkingAddress = PageAddress;
  for (Index = 0; Index < 512; Index++, PageTableEntry++, WorkingAddress += 0x1000) {
    PageTableEntry->Uint64        = (UINT64) WorkingAddress;
    PageTableEntry->Bits.Present  = 1;

    ///
    /// Update the new page to have the same attributes as the 2MB page
    ///
    PageTableEntry->Bits.ReadWrite      = Attributes.Common.ReadWrite;
    PageTableEntry->Bits.CacheDisabled  = Attributes.Common.CacheDisabled;
    PageTableEntry->Bits.WriteThrough   = Attributes.Common.WriteThrough;

    if (WorkingAddress == PageAddress) {
      ///
      /// Return back the 4K page that matches the Working addresss
      ///
      *PageDirectoryToConvert = (x64_PAGE_TABLE_ENTRY *) PageTableEntry;
    }
  }
}

/**

  Get current memory mapping information

  @param[in] BaseAddress - get current memory mapping by this Base address
  @param[in] PageTable   - page table that translated this base address
  @param[in] Page2MBytes - TRUE if this is 2MBytes page table

  @retval EFI_NOT_FOUND - page table not found
  @retval EFI_SUCCESS   - page table found

**/
EFI_STATUS
GetCurrentMapping (
  IN  EFI_PHYSICAL_ADDRESS    BaseAddress,
  OUT x64_PAGE_TABLE_ENTRY    **PageTable,
  OUT BOOLEAN                 *Page2MBytes
  )
{
  UINT64                                    Cr3;
  x64_PAGE_MAP_AND_DIRECTORY_POINTER_2MB_4K *PageMapLevel4Entry;
  x64_PAGE_MAP_AND_DIRECTORY_POINTER_2MB_4K *PageDirectoryPointerEntry;
  x64_PAGE_TABLE_ENTRY_2M                   *PageTableEntry2Mb;
  x64_PAGE_DIRECTORY_ENTRY_4K               *PageDirectoryEntry4k;
  x64_PAGE_TABLE_ENTRY_4K                   *PageTableEntry4k;
  UINTN                                     Pml4Index;
  UINTN                                     PdpIndex;
  UINTN                                     Pde2MbIndex;
  UINTN                                     PteIndex;

  Cr3                 = AsmReadCr3 ();

  PageMapLevel4Entry  = (x64_PAGE_MAP_AND_DIRECTORY_POINTER_2MB_4K *) (Cr3 & 0x000ffffffffff000);

  Pml4Index           = (UINTN) RShiftU64 (BaseAddress, 39) & 0x1ff;
  if (PageMapLevel4Entry[Pml4Index].Bits.Present == 0) {
    return EFI_NOT_FOUND;
  }

  PageDirectoryPointerEntry = (x64_PAGE_MAP_AND_DIRECTORY_POINTER_2MB_4K *) (PageMapLevel4Entry[Pml4Index].Uint64 & 0x000ffffffffff000);
  PdpIndex = (UINTN) RShiftU64 (BaseAddress, 30) & 0x1ff;
  if (PageDirectoryPointerEntry[PdpIndex].Bits.Present == 0) {
    return EFI_NOT_FOUND;
  }

  PageTableEntry2Mb = (x64_PAGE_TABLE_ENTRY_2M *) (PageDirectoryPointerEntry[PdpIndex].Uint64 & 0x000ffffffffff000);
  Pde2MbIndex       = (UINTN) RShiftU64 (BaseAddress, 21) & 0x1ff;
  if (PageTableEntry2Mb[Pde2MbIndex].Bits.Present == 0) {
    return EFI_NOT_FOUND;
  }

  if (PageTableEntry2Mb[Pde2MbIndex].Bits.MustBe1 == 1) {
    ///
    /// We found a 2MByte page so lets return it
    ///
    *Page2MBytes  = TRUE;
    *PageTable    = (x64_PAGE_TABLE_ENTRY *) &PageTableEntry2Mb[Pde2MbIndex].Uint64;
    return EFI_SUCCESS;
  }
  ///
  /// 4K page so keep walking
  ///
  PageDirectoryEntry4k  = (x64_PAGE_DIRECTORY_ENTRY_4K *) &PageTableEntry2Mb[Pde2MbIndex].Uint64;

  PageTableEntry4k      = (x64_PAGE_TABLE_ENTRY_4K *) (PageDirectoryEntry4k[Pde2MbIndex].Uint64 & 0x000ffffffffff000);
  PteIndex              = (UINTN) RShiftU64 (BaseAddress, 12) & 0x1ff;
  if (PageTableEntry4k[PteIndex].Bits.Present == 0) {
    return EFI_NOT_FOUND;
  }

  *Page2MBytes  = FALSE;
  *PageTable    = (x64_PAGE_TABLE_ENTRY *) &PageTableEntry4k[PteIndex];
  return EFI_SUCCESS;
}

/**

  Prepare memory for essential system tables.

  @param[in] None.

  @retval EFI_SUCCESS              - Memory successfully prepared.

**/
EFI_STATUS
PrepareMemory (
  VOID
  )
{
  ///
  /// Allocate space to convert 2MB page tables to 4K tables.
  ///  This can not be done a call time as the TPL level will
  ///  not be correct.
  ///
  InitailizeCacheAttributes ();

  InitializeExternalVectorTablePtr (mExternalVectorTable);
  ///
  /// Initialize the Interrupt Descriptor Table
  ///
  InitializeInterruptTables ();

  return EFI_SUCCESS;
}

/**

  Prepare Wakeup Buffer and stack for APs.

  @param[in] WakeUpBuffer             - Pointer to the address of wakeup buffer for output.
  @param[in] StackAddressStart        - Pointer to the stack address of APs for output.
  @param[in] MaximumCPUsForThisSystem - Maximum CPUs in this system.

  @retval EFI_SUCCESS              - Memory successfully prepared for APs.
  @retval Other                    - Error occurred while allocating memory.

**/
EFI_STATUS
PrepareMemoryForAPs (
  OUT EFI_PHYSICAL_ADDRESS       *WakeUpBuffer,
  OUT VOID                       **StackAddressStart,
  IN UINTN                       MaximumCPUsForThisSystem
  )
{
  EFI_STATUS              Status;
  MP_ASSEMBLY_ADDRESS_MAP AddressMap;

  ///
  /// Release All APs with a lock and wait for them to retire to rendezvous procedure.
  /// We need a page (4KB) of memory for IA-32 to use broadcast APIs, on a temporary basis.
  ///
  Status = AllocateWakeUpBuffer (WakeUpBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Claim memory for AP stack.
  ///
  Status = AllocateReservedMemoryBelow4G (
            MaximumCPUsForThisSystem * STACK_SIZE_PER_PROC,
            StackAddressStart
            );

  if (EFI_ERROR (Status)) {
    (gBS->FreePages) (*WakeUpBuffer, 1);
    return Status;
  }

  AsmGetAddressMap (&AddressMap);
  CopyMem ((VOID *) (UINTN) *WakeUpBuffer, AddressMap.RendezvousFunnelAddress, AddressMap.Size);
  *(UINT32 *) (UINTN) (*WakeUpBuffer + AddressMap.FlatJumpOffset + 3) = (UINT32) (*WakeUpBuffer + AddressMap.PModeEntryOffset);
  *(UINT32 *) (UINTN) (*WakeUpBuffer + AddressMap.LongJumpOffset + 2) = (UINT32) (*WakeUpBuffer + AddressMap.LModeEntryOffset);

  return EFI_SUCCESS;
}

/**

  Prepare exchange information for APs.

  @param[in] ExchangeInfo      - Pointer to the exchange info buffer for output.
  @param[in] StackAddressStart - Start address of APs' stacks.
  @param[in] ApFunction        - Address of function assigned to AP.
  @param[in] WakeUpBuffer      - Pointer to the address of wakeup buffer.

  @retval EFI_SUCCESS       - Exchange Info successfully prepared for APs.

**/
EFI_STATUS
PrepareExchangeInfo (
  OUT MP_CPU_EXCHANGE_INFO           *ExchangeInfo,
  IN  VOID                           *StackAddressStart,
  IN  VOID                           *ApFunction,
  IN  EFI_PHYSICAL_ADDRESS           WakeUpBuffer
  )
{
  ZeroMem ((VOID *) ExchangeInfo, EFI_PAGE_SIZE - MP_CPU_EXCHANGE_INFO_OFFSET);

  ExchangeInfo->Lock        = VacantFlag;
  ExchangeInfo->StackStart  = StackAddressStart;
  ExchangeInfo->StackSize   = STACK_SIZE_PER_PROC;
  ExchangeInfo->ApFunction  = ApFunction;

  CopyMem (
    (VOID *) (UINTN) &ExchangeInfo->GdtrProfile,
    (VOID *) (UINTN) mAcpiCpuData->GdtrProfile,
    sizeof (PSEUDO_DESCRIPTOR)
    );
  CopyMem (
    (VOID *) (UINTN) &ExchangeInfo->IdtrProfile,
    (VOID *) (UINTN) mAcpiCpuData->IdtrProfile,
    sizeof (PSEUDO_DESCRIPTOR)
    );

  ExchangeInfo->BufferStart = (UINT32) WakeUpBuffer;
  ExchangeInfo->Cr3         = (UINT32) (AsmReadCr3 ());
  ExchangeInfo->InitFlag    = 1;

  return EFI_SUCCESS;
}

/**

  Prepare Wakeup Buffer and stack for APs.

  @param[in] WakeUpBuffer      - Pointer to the address of wakeup buffer for output.
  @param[in] StackAddressStart - Pointer to the stack address of APs for output.

  @retval EFI_SUCCESS       - Memory successfully prepared for APs.

**/
EFI_STATUS
S3PrepareMemoryForAPs (
  OUT EFI_PHYSICAL_ADDRESS       *WakeUpBuffer,
  OUT VOID                       **StackAddressStart
  )
{
  return EFI_SUCCESS;
}

/**

  Prepare exchange information for APs.

  @param[in] ExchangeInfo      - Pointer to the exchange info for output.
  @param[in] StackAddressStart - Start address of APs' stacks.
  @param[in] ApFunction        - Address of function assigned to AP.
  @param[in] WakeUpBuffer      - Pointer to the address of wakeup buffer.

  @retval EFI_SUCCESS       - Exchange Info successfully prepared for APs.

**/
EFI_STATUS
S3PrepareExchangeInfo (
  OUT MP_CPU_EXCHANGE_INFO           *ExchangeInfo,
  IN  VOID                           *StackAddressStart,
  IN  VOID                           *ApFunction,
  IN  EFI_PHYSICAL_ADDRESS           WakeUpBuffer
  )
{
  return EFI_SUCCESS;
}

/**

  Dynamically write the far jump destination in APs' wakeup buffer,
  in order to refresh APs' CS registers for mode switching.

  @param[in] None.


**/
VOID
RedirectFarJump (
  VOID
  )
{
  MP_ASSEMBLY_ADDRESS_MAP AddressMap;

  AsmGetAddressMap (&AddressMap);
  *(UINT32 *) (UINTN) (mAcpiCpuData->WakeUpBuffer + AddressMap.FlatJumpOffset + 3) = (UINT32) (mAcpiCpuData->WakeUpBuffer + AddressMap.PModeEntryOffset);
  *(UINT32 *) (UINTN) (mAcpiCpuData->WakeUpBuffer + AddressMap.LongJumpOffset + 2) = (UINT32) (mAcpiCpuData->WakeUpBuffer + AddressMap.LModeEntryOffset);

  return ;
}

/**
  Prepare GDTR and IDTR for AP

  @param[in] Gdtr  - The GDTR profile
  @param[in] Idtr  - The IDTR profile

  @retval EFI_STATUS  - status returned by each sub-routine
  @retval EFI_SUCCESS - GDTR and IDTR has been prepared for AP

**/
EFI_STATUS
PrepareGdtIdtForAP (
  OUT PSEUDO_DESCRIPTOR          *Gdtr,
  OUT PSEUDO_DESCRIPTOR          *Idtr
  )
{
  INTERRUPT_GATE_DESCRIPTOR *IdtForAP;
  SEGMENT_DESCRIPTOR        *GdtForAP;

  PSEUDO_DESCRIPTOR         *IdtrForBSP;
  PSEUDO_DESCRIPTOR         *GdtrForBSP;

  UINT16                    *MceHandler;
  EFI_STATUS                Status;

  AsmGetGdtrIdtr (&GdtrForBSP, &IdtrForBSP);

  ///
  /// Allocate reserved memory for IDT
  ///
  Status = AllocateAlignedReservedMemory (
            IdtrForBSP->Limit + 1,
            8,
            (VOID **) &IdtForAP
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Allocate reserved memory for GDT
  ///
  Status = AllocateAlignedReservedMemory (
            GdtrForBSP->Limit + 1,
            8,
            (VOID **) &GdtForAP
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (gBS->AllocatePool)(EfiReservedMemoryType, SIZE_OF_MCE_HANDLER, (VOID **) &MceHandler);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// McheHandler content: iret (opcode = 0xcf)
  ///
  *MceHandler = 0xCF48;

  CopyMem (GdtForAP, (VOID *) GdtrForBSP->Base, GdtrForBSP->Limit + 1);
  CopyMem (IdtForAP, (VOID *) IdtrForBSP->Base, IdtrForBSP->Limit + 1);

  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].Offset15To0   = (UINT16) (UINTN) MceHandler;
  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].Offset31To16  = (UINT16) ((UINTN) MceHandler >> 16);
  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].Offset63To32  = (UINT32) ((UINTN) MceHandler >> 32);

  ///
  /// Create Gdtr, IDTR profile
  ///
  Gdtr->Base  = (UINTN) GdtForAP;
  Gdtr->Limit = GdtrForBSP->Limit;

  Idtr->Base  = (UINTN) IdtForAP;
  Idtr->Limit = IdtrForBSP->Limit;

  return EFI_SUCCESS;
}
