/** @file
  Memory Management for USB

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbCoreDxe.h"
#include "MemMng.h"
#include "Legacy.h"

STATIC EFI_GUID                 mUsbWorkingRegionHobGuid = USB_WORKING_REGION_HOB_GUID;

STATIC
VOID*
SearchGuidHob(
  IN EFI_HOB_HANDOFF_INFO_TABLE *HobTable,
  IN EFI_GUID                   *Guid,
  OUT UINTN                     *Length
  )
{
  VOID                          *HobStart;
  EFI_PEI_HOB_POINTERS          GuidHob;

  HobStart = HobTable + 1;
  while (TRUE) {
    GuidHob.Raw = HobStart;
    if (END_OF_HOB_LIST (GuidHob)) break;
    if (GuidHob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      if (*(UINT64*)((UINT8*)Guid + 0) == *(UINT64*)((UINT8*)&GuidHob.Guid->Name + 0) &&
          *(UINT64*)((UINT8*)Guid + 8) == *(UINT64*)((UINT8*)&GuidHob.Guid->Name + 8)) {
        if (Length) *Length = GuidHob.Header->HobLength - sizeof(EFI_HOB_GUID_TYPE);
        return (VOID*)(GuidHob.Guid + 1);
      }
    }
    HobStart = GET_NEXT_HOB (GuidHob);
  }
  return NULL;
}

/**

  Memory copy.

  @param  Destination           Target address.
  @param  Source                Orignal address.
  @param  Length                Data length
  
**/
STATIC
UINTN
Memcmp (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINT8  Length
  )
{
  UINT8 *Destination8;
  UINT8 *Source8;
  
  Destination8 = Destination;
  Source8 = Source;
  while (Length--) {
    if (*(Destination8++) != *(Source8++)) {
     return 0;
    }
  }
  return 1;
}

/*++

  Allocate Buffer

  @param  AllocSize             Alloc Size
  @param  Pool                  Place to store pointer to the memory buffer

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

--*/
EFI_STATUS
EFIAPI
AllocateBuffer (
  IN  UINTN                             AllocSize,
  IN  UINTN                             Alignment,
  OUT VOID                              **Pool
  )
{
  UINT8                 ByteValue;
  UINT8                 BitValue;
  UINTN                 TempBytePos;
  UINTN                 FoundBytePos;
  UINTN                 Index;
  UINTN                 FoundBitPos;
  UINTN                 NumberOfZeros;
  UINTN                 Count;
  UINTN                 RealAllocSize;
  UINTN                 Stepping;
  UINTN                 BitArraySizeInBits;
  UINTN                 CurrBitPos;
  UINTN                 MemoryType;
  UINT8                 *MemoryBitArray;
  //
  // Parameter checking
  //
  if ((Alignment & BOUNDARY_CHECK) && AllocSize > 0x1000) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Prevent SMI interfere memory allocation
  //
  EnterCriticalSection();
  *Pool         = NULL;
  //
  // Decide the memory type is Non-Smm or Smm
  //
  if (Alignment & (NON_SMM_ADDRESS | SMM_ADDRESS)) {
    //
    // Force memory type
    //
    MemoryType = (Alignment & NON_SMM_ADDRESS) ? 0 : 1;
    Alignment &= ~(NON_SMM_ADDRESS | SMM_ADDRESS);
  } else {
    //
    // Default, decided by current mode
    //
    MemoryType = (mPrivate->CurrMode != USB_CORE_RUNTIME_MODE) ? 0 : 1;
  }
  MemoryBitArray     = mPrivate->MemoryBitArray[MemoryType];
  BitArraySizeInBits = BITARRAY_SIZE_IN_BYTES << 3;
  if (MemoryType == 1) BitArraySizeInBits = BitArraySizeInBits / SMM_MEMORY_BLOCK_DEVIDER;
  //
  // allocate unit is 32 bytes (align on 32 byte)
  //
  RealAllocSize = (AllocSize + 0x1f) >> 5;
  //
  // There may be linked MemoryHeaders.
  // To allocate a free pool in Memory blocks,
  // must search in the MemoryHeader link list
  // until enough free pool is found.
  //
  Stepping           = (Alignment & 0xffff) >> 5;
  FoundBitPos        = (mPrivate->MemoryLastBitPos[MemoryType] + (Stepping - 1)) & ~(Stepping - 1);
  ByteValue          = MemoryBitArray[FoundBitPos >> 3];
  NumberOfZeros      = 0;

  for (Index = 0, CurrBitPos = FoundBitPos; Index < BitArraySizeInBits; Index ++) {
    //
    // Page boundary checking if BOUNDARY_CHECK
    //
    if ((Alignment & BOUNDARY_CHECK) && NumberOfZeros == 0) {
      if ((CurrBitPos >> 7) != ((CurrBitPos + RealAllocSize) >> 7)) {
        Count        = ((CurrBitPos + RealAllocSize) & ~0x7f) - CurrBitPos;
        Index       += Count;
        FoundBitPos += Count;
        CurrBitPos   = FoundBitPos;
        if (CurrBitPos >= BitArraySizeInBits) {
          CurrBitPos  = 0;
          FoundBitPos = 0;
        }
      }
    }
    if ((CurrBitPos & 0x07) == 0) {
      //
      // step forward a byte, getting the byte value,
      // and reset the bit pos.
      //
      ByteValue = MemoryBitArray[CurrBitPos >> 3];
    }
    //
    // Pop out BitValue from a byte in TempBytePos.
    //
    BitValue = (UINT8) (ByteValue & (0x01 << (CurrBitPos & 0x07)));

    if (BitValue == 0) {
      //
      // Found a free bit, the NumberOfZeros only record the number
      // of those consecutive zeros
      //
      NumberOfZeros++;
      //
      // Found enough consecutive free space, break the loop
      //
      if (NumberOfZeros >= RealAllocSize) {
        break;
      }
    } else {
      //
      // Encountering a '1', meant the bit is ocupied.
      //
      if (NumberOfZeros >= RealAllocSize) {
        //
        // Found enough consecutive free space,break the loop
        //
        break;
      } else {
        //
        // the NumberOfZeros only record the number of those consecutive zeros,
        // so reset the NumberOfZeros to 0 when encountering '1' before finding
        // enough consecutive '0's
        //
        NumberOfZeros = 0;
        //
        // reset the CurrBitPos and FoundBitPos
        //
        Count        = ((CurrBitPos - FoundBitPos + 1) + (Stepping - 1)) & ~(Stepping - 1);
        Index       += Count - (CurrBitPos - FoundBitPos + 1);
        FoundBitPos += Count;
        CurrBitPos   = FoundBitPos - 1;
        ByteValue    = MemoryBitArray[FoundBitPos >> 3];
      }
    }

    //
    // step forward a bit
    //
    CurrBitPos ++;
    if (CurrBitPos >= BitArraySizeInBits) {
      CurrBitPos    = 0;
      FoundBitPos   = 0;
      NumberOfZeros = 0;
    }
  }
  if (NumberOfZeros < RealAllocSize) {
    LeaveCriticalSection();
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Make the MemoryLastBitPos from start position if alloc size is 4k.
  //
  mPrivate->MemoryLastBitPos[MemoryType] = (RealAllocSize < 128) ? FoundBitPos + RealAllocSize : 0;
  //
  // Round down to 0 if size exceed
  //
  if (mPrivate->MemoryLastBitPos[MemoryType] >= BitArraySizeInBits) mPrivate->MemoryLastBitPos[MemoryType] = 0;
  //
  // Setup FoundBytePos and FoundBitPos
  //
  FoundBytePos = FoundBitPos >> 3;
  FoundBitPos &= 0x07;
  //
  // Set the memory as allocated
  //
  for (TempBytePos = FoundBytePos, Index = FoundBitPos, Count = 0; Count < RealAllocSize; Count++) {
    MemoryBitArray[TempBytePos] |= (1 << Index);
    Index++;
    if (Index == 8) {
      TempBytePos += 1;
      Index = 0;
    }
  }
  *Pool = mPrivate->MemoryBlockPtr[MemoryType] + (FoundBytePos * 8 + FoundBitPos) * 32;
  ZeroMem(*Pool, RealAllocSize << 5);
  LeaveCriticalSection();
  return EFI_SUCCESS;
}

/**

  Free Buffer

  @param  AllocSize             Pool size
  @param  Pool                  Pool to free

**/
EFI_STATUS
EFIAPI
FreeBuffer (
  IN UINTN                              AllocSize,
  IN VOID                               *Addr
  )
{
  UINTN                 StartBytePos;
  UINTN                 Index;
  UINTN                 StartBitPos;
  UINTN                 Index2;
  UINTN                 Count;
  UINTN                 RealAllocSize;
  UINTN                 Pool;
  UINTN                 MemoryBase;
  UINTN                 MemoryType;
  UINT8                 *MemoryBitArray;
  UINTN                 MemoryBlockSize;
  //
  // Prevent SMI interfere memory allocation
  //
  EnterCriticalSection();
  Pool = (UINTN)Addr;
  //
  // Decide the memory type is Non-Smm or Smm
  //
  if (Pool >= (UINTN)mPrivate->MemoryBlockPtr[0] && 
      Pool < (UINTN)(mPrivate->MemoryBlockPtr[0] + MEMORY_BLOCK_SIZE_IN_BYTES)) {
    MemoryType      = 0;
    MemoryBlockSize = MEMORY_BLOCK_SIZE_IN_BYTES;
  } else {
    MemoryType      = 1;
    MemoryBlockSize = MEMORY_BLOCK_SIZE_IN_BYTES / SMM_MEMORY_BLOCK_DEVIDER;
  }
  MemoryBitArray = mPrivate->MemoryBitArray[MemoryType];
  MemoryBase     = (UINTN)mPrivate->MemoryBlockPtr[MemoryType];
  //
  // allocate unit is 32 byte (align on 32 byte)
  //
  RealAllocSize = (AllocSize + 0x1f) & ~0x1f;
  //
  // scan the memory header linked list for
  // the asigned memory to free.
  //
  if ((Pool >= MemoryBase) &&
      ((Pool + RealAllocSize) <= (MemoryBase + MemoryBlockSize))
      ) {
    //
    // Pool is in the Memory Block area,
    // find the start byte and bit in the bit array
    //
    StartBitPos                            = (Pool - MemoryBase) >> 5;
    if (mPrivate->MemoryLastBitPos[MemoryType] > StartBitPos) {
      //
      // Restrict the grow direction as negative to reduce the memory fragmental
      //
      mPrivate->MemoryLastBitPos[MemoryType] = StartBitPos;
    }
    StartBytePos                           = StartBitPos >> 3;
    StartBitPos                           &= 0x07;

    //
    // reset associated bits in bit arry
    //
    for (Index = StartBytePos, Index2 = StartBitPos, Count = 0; Count < (RealAllocSize / 32); Count++) {
      MemoryBitArray[Index] ^= (UINT8) (1 << Index2);
      Index2++;
      if (Index2 == 8) {
        Index += 1;
        Index2 = 0;
      }
    }
  }
  LeaveCriticalSection();
  return EFI_SUCCESS;
}

/**

  Build the USB working region record for use of other drivers

  @param  Address               Region start address
  @param  Length                Region length

**/
STATIC
VOID
BuildUsbWorkingRegionRecord (
  IN UINT64                     Address,
  IN UINT64                     Length
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  USB_WORKING_REGION    *UsbWorkingRegion;
  
  for (Index = 0, UsbWorkingRegion = NULL; Index < gST->NumberOfTableEntries; Index ++) {
    if (Memcmp (&mUsbWorkingRegionHobGuid, &(gST->ConfigurationTable[Index].VendorGuid), sizeof(EFI_GUID))) {
      UsbWorkingRegion = (USB_WORKING_REGION*)gST->ConfigurationTable[Index].VendorTable;
      break;
    }
  }
  if (!UsbWorkingRegion) {
    //
    // The working region record did not exist so add one.
    //
    Status = AllocateBuffer (
               sizeof (USB_WORKING_REGION),
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               (VOID **)&UsbWorkingRegion
               );
    if (Status != EFI_SUCCESS) return;
    UsbWorkingRegion->DxeWorkingMemoryStart  = Address;
    UsbWorkingRegion->DxeWorkingMemoryLength = Length;
    UsbWorkingRegion->SmmWorkingMemoryStart  = 0;
    UsbWorkingRegion->SmmWorkingMemoryLength = 0;
    gBS->InstallConfigurationTable (&mUsbWorkingRegionHobGuid, UsbWorkingRegion);
  } else {
    //
    // The Hob exist, set the Hob content
    //
    UsbWorkingRegion->SmmWorkingMemoryStart  = Address;
    UsbWorkingRegion->SmmWorkingMemoryLength = Length;
  }
}

/**

  Memory Map for 4G concern

  @param  Direction             Direction type
  @param  Addr                  Data address
  @param  Length                Data size
  @param  AdjustedAddr          Adjusted data address for output
  @param  Handle                Handle for output

**/
EFI_STATUS
EFIAPI
MemoryMapping (
  IN  UINTN                             Direction,
  IN  EFI_PHYSICAL_ADDRESS              Addr,
  IN  UINTN                             Length,
  OUT VOID                              **AdjustedAddr,
  OUT VOID                              **Handle
  )
{
  EFI_STATUS    Status;
  VOID          *HandleBuffer;

  if ((Addr < 0x100000000LL) &&
      (Addr < 0xa0000 || Addr >= 0x100000) && 
      !((Direction & USB_CORE_BUS_MASTER_ALIGN4) && (Addr & 0x03)) &&
      !((Addr >= mPrivate->SmmRegionStart1 && Addr < mPrivate->SmmRegionEnd1) || 
        (Addr >= mPrivate->SmmRegionStart2 && Addr < mPrivate->SmmRegionEnd2))) {
    *AdjustedAddr = *(VOID**)&Addr;
    *Handle = NULL;
  } else {
    Status = AllocateBuffer(
               32 + Length,
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               &HandleBuffer
               );
    if (Status != EFI_SUCCESS) {
      return EFI_OUT_OF_RESOURCES;
    }
    *AdjustedAddr = (UINT8*)HandleBuffer + 32;
    Direction     &= 1;
    if (Direction == USB_CORE_BUS_MASTER_READ) {
      CopyMem(
        *AdjustedAddr,
        *(VOID**)&Addr,
        Length
        );
    }
    ((MEMORY_MAPPING*)HandleBuffer)->Address = Addr;
    ((MEMORY_MAPPING*)HandleBuffer)->Length = Length;
    ((MEMORY_MAPPING*)HandleBuffer)->AdjustedAddress = *AdjustedAddr;
    ((MEMORY_MAPPING*)HandleBuffer)->Direction = Direction;
    *Handle = (VOID*)HandleBuffer;
  }
  return EFI_SUCCESS;
}

/**

  Memory Unmapping

  @param  Handle                Handle produced by MemoryMapping

**/
EFI_STATUS
EFIAPI
MemoryUnmapping (
  IN VOID                               *Handle
  )
{
  if (Handle != NULL) {
    if (((MEMORY_MAPPING*)Handle)->Direction == USB_CORE_BUS_MASTER_WRITE) {
      CopyMem(
        *(VOID**)&((MEMORY_MAPPING*)Handle)->Address,
        ((MEMORY_MAPPING*)Handle)->AdjustedAddress,
        ((MEMORY_MAPPING*)Handle)->Length
        );
    }
    FreeBuffer(
      32 + ((MEMORY_MAPPING*)Handle)->Length,
      Handle
      );
  }
  return EFI_SUCCESS;
}

/*++

  Initialize Memory Management

  @param  Type                  0 : NON-SMM initialization, 1 : SMM initialization

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail
  
--*/
EFI_STATUS
EFIAPI
InitialMemoryManagement (
  IN UINTN                              Type
  )
{
  EFI_STATUS                            Status;
  EFI_PHYSICAL_ADDRESS                  Addr;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramBlock;
  EFI_HOB_HANDOFF_INFO_TABLE            *HobTable;
  UINTN                                 Index;
  UINTN                                 Length;
  USB_CORE_PRIVATE                      *Private;
  UINT64                                WorkingMemoryStart;
  UINT64                                WorkingMemoryLength;
  
  if (!Type) {
    //
    // Pre-allocate private storage before memory management initialize
    //
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (USB_CORE_PRIVATE),
                    (VOID **)&mPrivate
                    );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Initialize variables
    //
    ZeroMem (mPrivate, sizeof (USB_CORE_PRIVATE));
    //
    // Memory Block uses MemoryBlockSizeInPages pages,
    // and it is allocated as common buffer use.
    //
    Addr = 0xffffffff;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    MEMORY_BLOCK_UNIT_IN_PAGES,
                    &Addr
                    );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    mPrivate->MemoryBlockPtr[0] = *(UINT8**)&Addr;
    ZeroMem ((VOID*)((UINTN)(mPrivate->MemoryBlockPtr[0])), MEMORY_BLOCK_SIZE_IN_BYTES);
    //
    // Get Smm Memory Region through Hob
    //
    for (Index = 0, HobTable = NULL; Index < gST->NumberOfTableEntries; Index ++) {
      if (Memcmp(&gEfiHobListGuid, &(gST->ConfigurationTable[Index].VendorGuid), sizeof(EFI_GUID))) {
        HobTable = (EFI_HOB_HANDOFF_INFO_TABLE*)gST->ConfigurationTable[Index].VendorTable;
        break;
      }
    }
    if (HobTable) {
      SmramBlock = SearchGuidHob(HobTable, &gEfiSmmPeiSmramMemoryReserveGuid, &Length);
      if (SmramBlock) {
        mPrivate->SmmRegionStart1 = SmramBlock->Descriptor[0].PhysicalStart;
        mPrivate->SmmRegionEnd1   = mPrivate->SmmRegionStart1 + SmramBlock->Descriptor[0].PhysicalSize;
        if (SmramBlock->NumberOfSmmReservedRegions > 1) {
          mPrivate->SmmRegionStart2 = SmramBlock->Descriptor[1].PhysicalStart;
          mPrivate->SmmRegionEnd2   = mPrivate->SmmRegionStart2 + SmramBlock->Descriptor[1].PhysicalSize;
        }
      }
    }
    //
    // Initial linking head(in the temporary mPrivate) in used for AllocateBuffer
    //
    InitializeListHead(&mPrivate->DeferredHcCallbackListHead);
    //
    // Allocate private storage inside managed memory
    //
    Status = AllocateBuffer(
               sizeof (USB_CORE_PRIVATE),
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               (VOID **)&Private
               );
    if (Status != EFI_SUCCESS) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Copy the previous private data into permanent private storage
    //
    CopyMem (Private, mPrivate, sizeof (USB_CORE_PRIVATE));
    //
    // Setup permanent private storage
    //
    gBS->FreePool (mPrivate);
    mPrivate = Private;
    //
    // Initial linking head(in the permanent mPrivate) in used for AllocateBuffer
    //
    InitializeListHead(&mPrivate->DeferredHcCallbackListHead);
    //
    // Setting the memory address of DXE working region
    //
    WorkingMemoryStart  = (UINT64)(UINTN)mPrivate->MemoryBlockPtr[0];
    WorkingMemoryLength = MEMORY_BLOCK_SIZE_IN_BYTES;
  } else {
    //
    // Allocate SMM memory as quarter of NON-SMM memmory in used for SMM
    //
    Status = mPrivate->Smst->SmmAllocatePages (
                               AllocateAnyPages,
                               EfiRuntimeServicesData,
                               MEMORY_BLOCK_UNIT_IN_PAGES / SMM_MEMORY_BLOCK_DEVIDER,
                               &Addr
                               );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    mPrivate->MemoryBlockPtr[1] = *(UINT8**)&Addr;
    ZeroMem ((VOID*)((UINTN)(mPrivate->MemoryBlockPtr[1])), MEMORY_BLOCK_SIZE_IN_BYTES / 4);
    //
    // Setting the memory address of SMM working region
    //
    WorkingMemoryStart  = (UINT64)(UINTN)mPrivate->MemoryBlockPtr[1];
    WorkingMemoryLength = MEMORY_BLOCK_SIZE_IN_BYTES / SMM_MEMORY_BLOCK_DEVIDER;
  }
  //
  // Create Hob for storing the memory address of DXE/SMM working region
  //
  BuildUsbWorkingRegionRecord (WorkingMemoryStart, WorkingMemoryLength);
  return EFI_SUCCESS;
}