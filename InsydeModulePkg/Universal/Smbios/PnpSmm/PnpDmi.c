/** @file
  Functions for Dmitool

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

#include "PnpSmm.h"
#include <Protocol/LegacyBios.h>
#include <Protocol/SmmCpu.h>

SMBIOS_TABLE_ENTRY_POINT   *mSmbiosTableEntryPoint = NULL;
SMBIOS_STRUCTURE           *mSmbiosTable = NULL;
INT32                      TotalIncreasedStringLength = 0;
EFI_PHYSICAL_ADDRESS       mSMBIOSTableEntryAddress = 0;
EFI_SMM_CPU_PROTOCOL       *mSmmCpu = NULL;
UINTN                      mCpuIndex =0;

PNP_REDIRECTED_ENTRY_POINT mPnpDmiFunctions[] = {
  Pnp0x50,
  Pnp0x51,
  Pnp0x52,
  UnsupportedPnpFunction, //Pnp0x53,
  Pnp0x54,
  Pnp0x55,
  Pnp0x56,
  Pnp0x57
};

STATIC
EFI_STATUS
ClearBiosEventLog (
  IN UINTN                              GPNVBase,
  IN UINTN                              GPNVLength
  );

/**
  This fucntion uses to read saved CPU double word register by CPU index

  @param[In]   RegisterNum   Register number which want to get
  @param[In]   CpuIndex      CPU index number to get register.
  @param[Out]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Read double word register successfully
  @return Other         Any error occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
GetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  OUT UINT32                            *RegisterData
  )
{
  return mSmmCpu->ReadSaveState (
                    mSmmCpu,
                    sizeof (UINT32),
                    RegisterNum,
                    CpuIndex,
                    RegisterData
                    );
}

/**
  This fucntion uses to set saved CPU double word register by CPU index

  @param[In]  RegisterNum   Register number which want to get
  @param[In]  CpuIndex      CPU index number to get register.
  @param[In]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Set double word register successfully
  @return Other         Any error occured while disabling all secure boot SMI functions successful.

**/
EFI_STATUS
SetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  IN  UINT32                            *RegisterData
  )
{
  return mSmmCpu->WriteSaveState (
                    mSmmCpu,
                    sizeof (UINT32),
                    RegisterNum,
                    CpuIndex,
                    RegisterData
                    );
}

/**
  Ensures that mSmbiosTableEntryPoint and mSmbiosTable are filled in.

  @param  none

**/
VOID
LocateSmbiosTable (
  )
{
  UINT32             *Address;

  //
  // Locate SMBIOS structure table entry point when Legacy boot.
  //
  if ((mSmbiosTableEntryPoint == NULL) || ((UINT32)(UINTN)mSmbiosTableEntryPoint->AnchorString != (UINT32)SMBIOS_SIGNATURE)) {
    for (Address = (UINT32 *)(UINTN) 0xF0000; (UINTN)Address < 0x100000; Address += 4) {
      mSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN) Address;
      if (*(UINT32 *)mSmbiosTableEntryPoint == (UINT32)SMBIOS_SIGNATURE) {
        mSmbiosTable = (SMBIOS_STRUCTURE *)(UINTN)mSmbiosTableEntryPoint->TableAddress;
        return;
      }
    }
  }

  //
  // If the entry point is not found in CSM,
  // locate SMBIOS structure table entry point at Runtime.
  //
  mSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT*)(UINTN)mSMBIOSTableEntryAddress;
  mSmbiosTable = (SMBIOS_STRUCTURE*)(UINTN)mSmbiosTableEntryPoint->TableAddress;
}

/**
  Locates the smbios structure with a handle of *Structure.

  @param[In]   Structure      Handle of structure to attempt to locate.
                              - If 0 on entry, then the first avaiable structure is found.
                              - On exit, *Structure is updated with the next structure handle.
                              A return handle of 0xffff indicates there are no more structures.
                              *Structure is always updated with useful information.
  @param[Out]  Location       Return the structure location if it was found.
  @param[Out]  Size           The size of structure location if it was found.

  @retval EFI_SUCCESS    The structure was found. **Location and *Size are valid.
  @retval EFI_NOT_FOUND  The structure was not found.

**/
EFI_STATUS
LocateSmbiosStructure (
  IN OUT UINT16                         *Structure,
  OUT SMBIOS_STRUCTURE                  **Location,
  OUT UINTN                             *Size
  )
{
  SMBIOS_STRUCTURE                      *Iter;
  SMBIOS_STRUCTURE                      *Next;
  SMBIOS_STRUCTURE                      *End;

  if ((*Structure == 0) && ((*Location) != NULL) && (*(UINT32 *)(*Location) == (UINT32)SMBIOS_SIGNATURE)) {
    *Structure = mSmbiosTable->Handle;
  }

  Iter = mSmbiosTable;
  //
  // Adjust the end pointer if SMBIOS data is modified, such as strings.
  //
  End = (SMBIOS_STRUCTURE *)(UINTN)(mSmbiosTableEntryPoint->TableAddress + mSmbiosTableEntryPoint->TableLength);
  if (TotalIncreasedStringLength >= 0x00) {
    End = (SMBIOS_STRUCTURE *)((UINTN)End + (UINTN)TotalIncreasedStringLength);
  } else {
    End = (SMBIOS_STRUCTURE *)((UINTN)End - (UINTN)(~(TotalIncreasedStringLength - 1)));
  }

  while (TRUE) {
    for (Next = (SMBIOS_STRUCTURE *)((UINT8 *)Iter + Iter->Length); *(UINT16 *)Next != 0; Next = (SMBIOS_STRUCTURE *)((UINTN)Next + 1));
    Next = (SMBIOS_STRUCTURE *)((UINTN)Next + 2);
    if (Iter >= End) {
      //
      // End-of-list indicator
      //
      *Structure = 0xffff;
      return EFI_NOT_FOUND;
    } else if (Iter->Handle == *Structure) {
      *Structure = (UINT16)((Next < End) ? Next->Handle : 0xffff);
      *Location = Iter;
      *Size = (UINTN)Next - (UINTN)Iter;
      return EFI_SUCCESS;
    }
    Iter = Next;
  }
}

/**
  Find the location and size of a string within a smbios structure.

  @param[In]   StringRef      The string number to search for within the structure.
  @param[In]   Structure      The Pointer to the structure to search.
  @param[Out]  Location       Return the string location if it was found.
  @param[Out]  Size           The size of string location if it was found.

  @retval EFI_SUCCESS    the string was found. **Location and *Size are valid.
  @retval EFI_NOT_FOUND  The structure was not found.

**/
EFI_STATUS
LocateStringOfStructure (
  IN UINT8                              StringRef,
  IN SMBIOS_STRUCTURE                   *Structure,
  OUT CHAR8                             **Location,
  OUT UINTN                             *Size
  )
{
  CHAR8                                 *Next;

  if (StringRef == 0) {
    return EFI_NOT_FOUND;
  }

  Next = (CHAR8 *)(((UINTN)Structure) + Structure->Length);
  do {
    *Location = Next;
    *Size = AsciiStrLen (*Location);
    Next = *Location + *Size + 1;
  } while ((--StringRef) != 0 && (*Next != 0));

  return (StringRef == 0) ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
  If OEM_DMI_STORE address full to arrange.

  @param[Out]  DmiEnd         Pointer to function 0x52 data buffer input.

  @retval EFI_SUCCESS    The function return valid value.
  @retval EFI_NOT_FOUND  Error occurs.

**/
EFI_STATUS
DMISpaceNotEnough (
  OUT UINT16                            *DmiEnd
  )
{
  EFI_STATUS                  Status;
  UINTN                       BlockSize;
  UINTN                       NvStorageDmiBase;
  UINT8                       *NotEnoughBuffer;
  UINT16                      NotEnoughPtr;
  UINT8                       *Buffer;
  DMI_STRING_STRUCTURE        *CurrentPtr;

  NotEnoughBuffer  = NULL;
  Buffer           = NULL;
  CurrentPtr       = NULL;
  NotEnoughPtr     = 4;
  NvStorageDmiBase = PcdGet32 (PcdFlashNvStorageDmiBase);
  BlockSize        = PcdGet32 (PcdFlashNvStorageDmiSize);

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    BlockSize,  //0x1000
                    (VOID **)&NotEnoughBuffer
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetMem (NotEnoughBuffer, BlockSize, 0xFF);
  *(UINT32 *)NotEnoughBuffer = DMI_UPDATE_STRING_SIGNATURE;

  //
  // Search smbios Valid, If it is found, set OEM_DMI_STORE address
  //
  Buffer = (UINT8 *)(NvStorageDmiBase + sizeof (DMI_UPDATE_STRING_SIGNATURE));
  while ((UINTN)Buffer < (NvStorageDmiBase + BlockSize)) {
    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;
    if (CurrentPtr->Type == 0xFF) {
      break;
    }
    if (CurrentPtr->Valid == 0xFF) {
      CopyMem ((NotEnoughBuffer+NotEnoughPtr), Buffer, CurrentPtr->Length);
      NotEnoughPtr = NotEnoughPtr + CurrentPtr->Length;
    }
    Buffer = Buffer + CurrentPtr->Length;
  }

  //
  // Flash whole buffer to rom
  //
  Status = mSmmFwBlockService->EraseBlocks (
                                 mSmmFwBlockService,
                                 NvStorageDmiBase,
                                 &BlockSize
                                 );
  if (!EFI_ERROR (Status)) {
    Status = mSmmFwBlockService->Write (
                                   mSmmFwBlockService,
                                   NvStorageDmiBase,
                                   &BlockSize,
                                   NotEnoughBuffer
                                   );
  }

  mSmst->SmmFreePool (NotEnoughBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((UINTN)NotEnoughPtr >= (NvStorageDmiBase + BlockSize)) {
    return EFI_OUT_OF_RESOURCES;
  }

  *DmiEnd = NotEnoughPtr;
  return EFI_SUCCESS;
}

/**
  Set SMBIOS Structure to OEM_DMI_STORE address.

  @param[In]  Data           Pointer to function 0x52 data buffer input.
  @param[In]  Structure      The Pointer to the structure to search.

  @retval EFI_SUCCESS    The function sets valid value for Data.
  @retval EFI_NOT_FOUND  Error occurs.

**/
EFI_STATUS
SetDMI (
  IN FUNC_0x52_DATA_BUFFER              *Data,
  IN SMBIOS_STRUCTURE                   *Structure
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       BlockSize;
  UINTN                       BSize;
  UINT8                       Invalid;
  UINT8                       *Buffer;
  UINT8                       *Ptr;
  UINT8                       *SavePtr;
  UINT16                      DmiEnd;
  DMI_STRING_STRUCTURE        *CurrentPtr;

  BlockSize  = 0x1000;
  BSize      = 0x1;
  Invalid    = 0;
  SavePtr    = NULL;
  CurrentPtr = NULL;
  DmiEnd     = 4;

  //
  // First 4 bytes are "$DMI"
  //
  Ptr        = (UINT8 *)(UINTN)(PcdGet32 (PcdFlashNvStorageDmiBase));
  Buffer = Ptr + sizeof (DMI_UPDATE_STRING_SIGNATURE);

  //
  // Search OEM_DMI_STORE free space
  //
  while (DmiEnd < PcdGet32 (PcdFlashNvStorageDmiSize)) {
    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;

    if (CurrentPtr->Type == 0xFF) {
      //
      // The space is free, we could use it, so break.
      //
      break;
    }

    if ((CurrentPtr->Type == Structure->Type) &&
        (CurrentPtr->Offset == Data->FieldOffset) &&
        (CurrentPtr->Valid == 0xFF)) {
      //
      // If we find the same data in OEM_DMI_STORE space, set invalid byte in it.
      //
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     PcdGet32 (PcdFlashNvStorageDmiBase) + DmiEnd + 2,
                                     &BSize,
                                     &Invalid
                                     );
    }

    //
    // Try to find next.
    //
    DmiEnd = DmiEnd + CurrentPtr->Length;
    Buffer = Buffer + CurrentPtr->Length;
  }

  if (DmiEnd + Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof(UINT8) - 1) >
      PcdGet32 (PcdFlashNvStorageDmiSize)) {
    //
    // If OEM_DMI_STORE space full, reclaim area, and return new address of free space address.
    //
    Status = DMISpaceNotEnough (&DmiEnd);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Set update data to OEM_DMI_STORE address.
  //
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8) - 1),
                    (VOID **)&SavePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CurrentPtr         = (DMI_STRING_STRUCTURE *)SavePtr;
  CurrentPtr->Type   = Structure->Type;
  CurrentPtr->Offset = Data->FieldOffset;
  CurrentPtr->Valid  = 0xFF;

  switch (Data->Command) {

  case DMI_STRING_CHANGE:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8) - 1);
    for (Index = 0; Index + 1 < Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;

  default:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8));
    for (Index = 0; Index <= Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;
  }

  BlockSize = CurrentPtr->Length;
  Status = mSmmFwBlockService->Write (
                                 mSmmFwBlockService,
                                 PcdGet32 (PcdFlashNvStorageDmiBase) + DmiEnd,
                                 &BlockSize,
                                 SavePtr
                                 );

  mSmst->SmmFreePool (SavePtr);
  return Status;
}

BOOLEAN
VaildGPNVCheck (
  IN OUT UINT16                     *Index,
  IN UINT16                         Handle
  )
{
  UINT16                            TempIndex;
  
  if (mIsOemGPNVMap == FALSE ||
      OemGPNVHandleCount >= FixedPcdGet16 (PcdDefaultGpnvMapBufferSize) ||
      *Index >= FixedPcdGet16 (PcdDefaultGpnvMapBufferSize) ||
	  *Index >= OemGPNVHandleCount) {
    return FALSE;
  }

  for (TempIndex = *Index; TempIndex < OemGPNVHandleCount; TempIndex++) {
    if (GPNVMapBuffer.PlatFormGPNVMap[TempIndex].Handle == Handle) {
      *Index = TempIndex;
      return TRUE;
    }
  }

  return FALSE;
}

EFI_STATUS
WriteGPNV (
  IN UINT16                             Handle,
  IN UINT8                              *GPNVBuffer
  )
{
  EFI_STATUS                            Status;
  UINTN                                 BlockSize;
  UINT8                                 *WritingBuffer;
  UINTN                                 BlockBaseAddress;
  UINTN                                 WritingBufferOffset;
  UINT8                                 EraseCount;
  UINT8                                 WriteCount;

  BlockSize = 0x10000;
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    BlockSize,
                    (VOID **)&WritingBuffer
                    );

  BlockBaseAddress = (GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress) & (~0xFFFF);
  //
  // Copy whole block data to buffer
  //
  CopyMem (WritingBuffer, (VOID *)(UINTN)BlockBaseAddress, BlockSize);

  //
  // Copy modified GPNV data to buffer
  //
  WritingBufferOffset = GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress - BlockBaseAddress;
  CopyMem ((VOID *)(WritingBuffer + WritingBufferOffset), (VOID *)GPNVBuffer, GPNVMapBuffer.PlatFormGPNVMap[Handle].GPNVSize);

  //
  // Flash whole buffer to rom
  //
  EraseCount = 0;
  WriteCount = 0;
  do {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   BlockBaseAddress,
                                   (UINTN *)&BlockSize
                                   );

    if (!EFI_ERROR (Status)) {
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     BlockBaseAddress,
                                     (UINTN *)&BlockSize,
                                     WritingBuffer
                                     );
      if (!EFI_ERROR (Status)) {
        mSmst->SmmFreePool (WritingBuffer);
        return Status;
      } else {
        WriteCount++;
      }
    } else {
      EraseCount++;
    }
  } while ((EraseCount < 100) && (WriteCount < 100));

  mSmst->SmmFreePool (WritingBuffer);
  return Status;
}

UINT32
LogicalToPhysicalAddress (
  IN UINT32  LinearAddress,
  IN UINT32  CR3Value,
  IN UINT32  CR4Value,
  IN UINTN   SegementSelector
  )
{
  UINT32  *TmpPtr;
  UINT32  Buffer32;
  UINT32  Edi;

  if ((CR3Value & 0x00000001) == 0) {
    return ((UINT32)SegementSelector << 4 ) + LinearAddress;
  }

  //
  // 32bit Mode SMI: transfer to physical address
  // IA-32 Intel Architecture Software Developer's Manual
  // Volume 3 - Chapter 3.7
  // there is 2 necessay condition:
  // 1.Base address in Segement Selector(GDT) must be 0
  //   if not 0, the Linear address need add the base address first
  //   Check Chapter 3.4 "Logical and Linear Addresses"
  if (SegementSelector != 0x10) {
    //
    // Base Address in SegementSelector 0x10 is set to 0 in SEC Phase.
    // Other will need to be read GDT...not impelement yet.Just return.
    //
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RDI, mCpuIndex, &Edi);
    LinearAddress += Edi;
  }

  //
  // 2.Only support 4MByte Pages now for XP-SP2
  //   4KByte pages isn't verified.
  //   if need support 4KByte Pages for Win2K...etc,
  //   check Chapter 3.7.1
  //

  // Use CR4 Bit5 to check 2MBytes Page for XD enable
  if (CR4Value & 0x20) {
    Buffer32 = CR3Value & 0xFFFFFFE0;

    //
    // Page Directory Pointer Table Entry
    //
    TmpPtr = (UINT32 *)(UINTN)(Buffer32 + ((LinearAddress & 0xC0000000) >> 27));
    Buffer32 = *TmpPtr;

    //
    // 2MByte Page - Page Directory Entry
    //
    TmpPtr = (UINT32 *)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x3FE00000) >> 18));
    Buffer32 = *TmpPtr;

    if ((Buffer32 & 0x80) == 0) {
      //
      // 4KByte Pages - Page Table
      //
      TmpPtr = (UINT32 *)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x001FF000) >> 9));
      Buffer32 = *TmpPtr;
      //
      // 4-KByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFFFF000) + (LinearAddress & 0x00000FFF);
    } else {
      //
      // 2MByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFE00000) + (LinearAddress & 0x001FFFFF);
    }
  } else {
    //
    // Get Page-Directory from CR3
    //
    Buffer32 = CR3Value & 0xFFFFF000;

    //
    // Page Directory Entry
    //
    TmpPtr = (UINT32 *)(UINTN)(Buffer32 + ((LinearAddress & 0xFFC00000) >> 20));
    Buffer32 = *TmpPtr;

    //
    // Check 4KByte/4MByte Pages
    //
    if ((Buffer32 & 0x80) == 0) {
      //
      // 4KByte Pages - Page Table
      //
      TmpPtr = (UINT32 *)(UINTN)((Buffer32 & 0xFFFFF000) + ((LinearAddress & 0x003FF000) >> 10));
      Buffer32 = *TmpPtr;
      //
      // 4-KByte Page - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFFFF000) + (LinearAddress & 0x00000FFF);
    } else {
      //
      // 4MByte Pages - Physical Address
      //
      Buffer32 = (Buffer32 & 0xFFC00000) + (LinearAddress & 0x003FFFFF);
    }
  }

  return Buffer32;
}

/**
  Does a quick conversion from a 16-bit C far pointer to a linear address.
  Unfortunately, this can only support far pointers from 16-bit real mode.

  @param[In]  Ptr     16-bit far pointer.

  @return  VOID*      The linear address of Ptr.

**/
VOID *
PnpFarToLinear (
  IN PNP_FAR_PTR                        Ptr
  )
{
  UINT32                                CR3Value;
  UINT32                                CR4Value;
  UINT32                                Ebx, Ecx;
  UINTN                                 SegementSelector;


  GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &Ebx);
  GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RCX, mCpuIndex, &Ecx);

  //
  // Call by new way that AP trigger software SMI directly when the signature is "$ISB'
  //
  if (Ebx == SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    return (VOID *)(UINTN)(UINT32)(Ptr.Offset | Ptr.Segment << 16);
  }

  CR3Value = Ebx;
  CR4Value = Ecx;
  SegementSelector = (UINTN)Ptr.Segment;

  return (VOID *)(UINTN)LogicalToPhysicalAddress ((UINT32)Ptr.Offset, CR3Value, CR4Value, SegementSelector);
}

/**
  Helper function for Pnp0x52, process command DMI_BYTE_CHANGE, DMI_WORD_CHANGE, and DMI_DWORD_CHANGE.

  @param[In]  Data           Buffer of data to SetStructure.
  @param[In]  Structure      The structure to be set.

  @retval DMI_SUCCESS    The function executed successfully.
  @retval DMI_READ_ONLY  The structure is not Type1, the block to be set is not at offset 8.

**/
INT16
Pnp0x52ChangeFixedLength (
  IN FUNC_0x52_DATA_BUFFER             *Data,
  IN SMBIOS_STRUCTURE                  *Structure
  )
{
  UINT8                       *ByteTmpPtr;
  UINT16                      *WordTmpPtr;
  UINT32                      *DWordTmpPtr;
  EFI_STATUS                  Status;
  UINT8                       ByteTemp;
  UINT16                      WordTemp;
  UINT32                      DWordTemp;
  UINTN                       Index;

  ByteTmpPtr  = NULL;
  WordTmpPtr  = NULL;
  DWordTmpPtr = NULL;
  ByteTemp    = 0;
  WordTemp    = 0;
  DWordTemp   = 0;

  for (Index = 0; Index < UpdateableStringCount; Index++) {
    if ((Structure->Type == mUpdatableStrings[Index].Type) &&
        (Data->FieldOffset == mUpdatableStrings[Index].FixedOffset)) {
      break;
    }
  }

  if (Index == UpdateableStringCount) {
    return DMI_READ_ONLY;
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    ByteTmpPtr = (UINT8 *)((UINTN)Structure + Data->FieldOffset);
    ByteTemp   = *ByteTmpPtr;
    ByteTemp   = ByteTemp & ((UINT8)(Data->ChangeMask));
    ByteTemp   = ByteTemp | ((UINT8)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT8);
    CopyMem ((VOID *)&Data->StructureData, (VOID *)&ByteTemp, Data->DataLength);
    break;

  case DMI_WORD_CHANGE:
    WordTmpPtr = (UINT16 *) ((UINTN)Structure + Data->FieldOffset);
    WordTemp   = *WordTmpPtr;
    WordTemp   = WordTemp & ((UINT16)(Data->ChangeMask));
    WordTemp   = WordTemp | ((UINT16)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT16);
    CopyMem ((VOID *)&Data->StructureData, (VOID *)&WordTemp, Data->DataLength);
    break;

  case DMI_DWORD_CHANGE:
    DWordTmpPtr = (UINT32 *) ((UINTN)Structure + Data->FieldOffset);
    DWordTemp   = *DWordTmpPtr;
    DWordTemp   = DWordTemp & ((UINT32)(Data->ChangeMask));
    DWordTemp   = DWordTemp | ((UINT32)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT32);
    CopyMem ((VOID *)&Data->StructureData, (VOID *)&DWordTemp, Data->DataLength);

    break;
  }

  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    CopyMem (ByteTmpPtr, &ByteTemp, sizeof (UINT8));
    break;

  case DMI_WORD_CHANGE:
    CopyMem (WordTmpPtr, &WordTemp, sizeof (UINT16));
    break;

  case DMI_DWORD_CHANGE:
    CopyMem (DWordTmpPtr, &DWordTemp, sizeof (UINT32));
    break;

  default:
    break;
  }

  return DMI_SUCCESS;
}

/**
  Helper function for Pnp0x52, process command DMI_STRING_CHANGE.

  @param[In]  Data           Buffer of data to SetStructure.
  @param[In]  Structure      The structure to be set.

  @retval DMI_SUCCESS          The function executed successfully.
  @retval DMI_BAD_PARAMETER    The length of data is invalid or can not locate the string in the structure.
  @retval DMI_READ_ONLY        Cannot set the data in the structure.

**/
INT16
Pnp0x52ChangeString (
  IN FUNC_0x52_DATA_BUFFER                 *Data,
  IN SMBIOS_STRUCTURE                      *Structure
  )
{
  CHAR8                                 *StructureString;
  CHAR8                                 *TempstructureString;
  UINTN                                 StructureStringSize;
  UINTN                                 TempstructureStringSize;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  UINT8                                 *SmbiosStoreArea;
  UINTN                                 CurrentTableSize;
  UINT8                                 *DmiPtr;
  UINTN                                 TotalSmbiosBufferSize;
  UINTN                                 NewTableSize;


  TempstructureString = NULL;
  TempstructureStringSize = 0;
  SmbiosStoreArea = NULL;
  DmiPtr = (UINT8 *)(UINTN)(PcdGet32 (PcdFlashNvStorageDmiBase));

  if (!(*(UINT32 *)DmiPtr == DMI_UPDATE_STRING_SIGNATURE)){
    return DMI_READ_ONLY;
  }

  if ((Data->DataLength > 0xff) || (Data->DataLength <= 1)) {
    return DMI_BAD_PARAMETER;
  }

  //
  // Find string within the SMBIOS structure
  //
  if ((Structure->Type == 0x0b) || (Structure->Type == 0x0c)) {
    Status = LocateStringOfStructure (
               (UINT8)Data->FieldOffset,
               Structure,
               &StructureString,
               &StructureStringSize
               );
    Data->FieldOffset += 4;
  } else {
    Status = LocateStringOfStructure (
               ((UINT8 *)Structure)[Data->FieldOffset],
               Structure,
               &StructureString,
               &StructureStringSize
               );
  }
  if (EFI_ERROR (Status)) {
    return DMI_BAD_PARAMETER;
  }

  for (Index = 0; Index < UpdateableStringCount; Index++) {
    if ((Structure->Type == mUpdatableStrings[Index].Type) &&
        (Data->FieldOffset == mUpdatableStrings[Index].FixedOffset)) {
      break;
    }
  }
  if (Index == UpdateableStringCount) {
    return DMI_READ_ONLY;
  }

  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }

  //
  // This value is based on SmbiosDxe driver. If the buffer allocation method is changed, this size will be different.
  //
  TotalSmbiosBufferSize = EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (mSmbiosTableEntryPoint->TableLength));
  TempstructureString = StructureString + StructureStringSize + 1;
  if (TotalIncreasedStringLength >= 0x00) {
    CurrentTableSize = (UINTN)(mSmbiosTableEntryPoint->TableLength + (UINT16)TotalIncreasedStringLength);
    TempstructureStringSize = (UINTN)(mSmbiosTableEntryPoint->TableLength + (UINT16)TotalIncreasedStringLength) -
                              (UINTN)(TempstructureString - mSmbiosTableEntryPoint->TableAddress);
  } else {
    CurrentTableSize = (UINTN)(mSmbiosTableEntryPoint->TableLength - (UINT16)(~(TotalIncreasedStringLength - 1)));
    TempstructureStringSize = (UINTN)(mSmbiosTableEntryPoint->TableLength - (UINT16)(~(TotalIncreasedStringLength - 1))) -
                              (UINTN)(TempstructureString - mSmbiosTableEntryPoint->TableAddress);
  }

  //
  // If new SMBIOS data is over than the buffer, nothing will change.
  // Data->DataLength : new string length, including '0'
  // structureStringSize : original string length
  //
  if ((UINTN)Data->DataLength > (StructureStringSize + 1)) {
    NewTableSize = CurrentTableSize + (UINTN)Data->DataLength - (StructureStringSize + 1);
    if (NewTableSize > TotalSmbiosBufferSize) {
      return DMI_NO_CHANGE;
    }
  }

  Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, CurrentTableSize, (VOID **)&SmbiosStoreArea);
  if (EFI_ERROR (Status)) {
    return PNP_BUFFER_TOO_SMALL;
  }

  CopyMem (SmbiosStoreArea, TempstructureString, TempstructureStringSize);
  CopyMem (StructureString, Data->StructureData, Data->DataLength);
  TempstructureString = StructureString + Data->DataLength;
  CopyMem (TempstructureString, SmbiosStoreArea, TempstructureStringSize);
  Status = mSmst->SmmFreePool (SmbiosStoreArea);

  TotalIncreasedStringLength = TotalIncreasedStringLength + (UINT32)Data->DataLength - (UINT32)(StructureStringSize + 1);
  return DMI_SUCCESS;
}

/**
  Helper function for Pnp0x52, process command DMI_BLOCK_CHANGE.

  @param[In]  Data           Buffer of data to SetStructure.
  @param[In]  Structure      The structure to be set.

  @retval DMI_SUCCESS          The function executed successfully.
  @retval DMI_BAD_PARAMETER    The length of data is invalid or can not locate the string in the structure.
  @retval DMI_READ_ONLY        The structure is not Type1, the block to be set is not at offset 8.

**/
INT16
Pnp0x52ChangeBlock (
  IN FUNC_0x52_DATA_BUFFER                 *Data,
  IN SMBIOS_STRUCTURE                      *Structure
  )
{
  UINT8                       *TmpPtr;
  UINT8                       *DataPtr;
  UINTN                       CopyLength;
  EFI_STATUS                  Status;


  TmpPtr     = (UINT8 *)((UINTN)Structure + Data->FieldOffset);
  DataPtr    = (UINT8 *)Data->StructureData;
  CopyLength = (UINTN)Data->DataLength;

  if ((Structure->Type != 1) || (Data->FieldOffset != 8)) {
    return DMI_READ_ONLY;
  }

  if (CopyLength != sizeof (EFI_GUID)){
    return DMI_BAD_PARAMETER;
  }

  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }
  CopyMem (TmpPtr, Data->StructureData, Data->DataLength);

  return DMI_SUCCESS;
}

/**
  PnP function 0x50, Get SMBIOS Information.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS     The function was executed successfully.

**/
INT16
Pnp0x50 (
  IN VOID            *Frame
  )
{
  UINT8                                 *DmiBIOSRevision;
  UINT16                                *NumStructures;
  UINT16                                *StructureSize;
  UINT32                                *DmiStorageBase;
  UINT16                                *DmiStorageSize;

  LocateSmbiosTable ();

  DmiBIOSRevision = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiBIOSRevision);
  *DmiBIOSRevision = mSmbiosTableEntryPoint->SmbiosBcdRevision;

  NumStructures = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->NumStructures);
  *NumStructures = mSmbiosTableEntryPoint->NumberOfSmbiosStructures;

  StructureSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->StructureSize);
  *StructureSize = mSmbiosTableEntryPoint->MaxStructureSize;

  DmiStorageBase = (UINT32 *)PnpFarToLinear (((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiStorageBase);
  *DmiStorageBase = (UINT32)(UINTN)mSmbiosTable;

  DmiStorageSize = (UINT16 *)PnpFarToLinear(((PNP_FUNCTION_0x50_FRAME *)Frame)->DmiStorageSize);
  *DmiStorageSize = mSmbiosTableEntryPoint->TableLength;

  return DMI_SUCCESS;
}

/**
  PnP function 0x51, Get SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x51 (
  IN VOID            *Frame
  )
{
  UINT16                                *Structure;
  UINT8                                 *DmiStrucBuffer;
  SMBIOS_STRUCTURE                      *Location;
  UINTN                                 Size;

  LocateSmbiosTable ();

  Structure      = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x51_FRAME *)Frame)->Structure);
  DmiStrucBuffer = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x51_FRAME *)Frame)->DmiStrucBuffer);
  Location = (SMBIOS_STRUCTURE *)DmiStrucBuffer;

  if (!EFI_ERROR (LocateSmbiosStructure (Structure, &Location, &Size))) {
    CopyMem (DmiStrucBuffer, (VOID *)Location, Size);
    return DMI_SUCCESS;
  } else {
    return DMI_INVALID_HANDLE;
  }
}

/**
  PnP function 0x52, Set SMBIOS Structure.

  @param[In]      Frame                 Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS           The function was executed successfully.
  @retval     DMI_BAD_PARAMETER     Invalid command.

**/
INT16
Pnp0x52 (
  IN VOID            *Frame
  )
{
  FUNC_0x52_DATA_BUFFER                 *Data;
  SMBIOS_STRUCTURE                      *StructureLocation;
  UINT16                                StructureHandle;
  UINTN                                 StructureSize;
  INT16                                 ReturnStatus;

  Data = (FUNC_0x52_DATA_BUFFER *)PnpFarToLinear (((PNP_FUNCTION_0x52_FRAME *)Frame)->DmiDataBuffer);
  StructureLocation = NULL;
  StructureHandle = Data->StructureHeader.Handle;
  LocateSmbiosStructure (&StructureHandle, &StructureLocation, &StructureSize);

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
  case DMI_WORD_CHANGE:
  case DMI_DWORD_CHANGE:
    ReturnStatus = Pnp0x52ChangeFixedLength (Data, StructureLocation);
    break;

  case DMI_STRING_CHANGE:
    ReturnStatus = Pnp0x52ChangeString (Data, StructureLocation);
    break;

  case DMI_BLOCK_CHANGE:
    ReturnStatus = Pnp0x52ChangeBlock (Data, StructureLocation);
    break;

  default:
    ReturnStatus = DMI_BAD_PARAMETER;
    break;
  }

  return ReturnStatus;
}

/**
  default function returning that the call was to an unsupported function.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.(unused)

  @retval     DMI_SUCCESS                 The function was executed successfully.
  @retval     DMI_INVALID_SUBFUNCTION     Invalid command.

**/
INT16
UnsupportedPnpFunction (
  IN VOID               *Frame
  )
{
  return PNP_FUNCTION_NOT_SUPPORTED;
}

/**
  PnP function 0x54.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS     The function was executed successfully.

**/
INT16
Pnp0x54 (
  IN VOID            *Frame
  )
{
  INT16                                SubFunction;
  UINT8                                *Data;
  UINT8                                Control;
  UINTN                                Index;

  SubFunction = (UINT16)((PNP_FUNCTION_0x54_FRAME *)Frame)->SubFunction;
  Data = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x54_FRAME *)Frame)->Data);
  Control = (UINT8)((PNP_FUNCTION_0x54_FRAME *)Frame)->Control;

  if ((Control & 0x01) == 0) {
    if (SubFunction >= 1) {
      return DMI_INVALID_SUBFUNCTION;
    }
    return DMI_SUCCESS;
  }

  switch (SubFunction) {

  case 0:
    //
    // for event log
    //
    if (!mIsOemGPNVMap) {
      return DMI_FUNCTION_NOT_SUPPORTED;
    }
    for (Index = 0; Index < OemGPNVHandleCount; Index++) {
      ClearBiosEventLog (GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress, GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize);
    }
    break;

  case 1:
    return DMI_INVALID_SUBFUNCTION;
    break;

  case 2:
    return DMI_INVALID_SUBFUNCTION;
    break;

  default:
    return DMI_INVALID_SUBFUNCTION;
    break;
  }

  return DMI_SUCCESS;
}

/**
  PnP function 0x55, Set SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x55 (
  IN VOID            *Frame
  )
{
  UINT16                                *Handle;
  UINT16                                HandleTemp;
  UINT16                                *MinGPNVRWSize;
  UINT16                                *GPNVSize;
  UINT32                                *NVStorageBase;
  UINT16                                Index;


  Handle = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->Handle);
  Index = HandleTemp = *Handle;

  if (!VaildGPNVCheck (&Index, HandleTemp)) {
    return  DMI_INVALID_HANDLE;
  }

  MinGPNVRWSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->MinGPNVRWSize);
  *MinGPNVRWSize = (UINT16)GPNVMapBuffer.PlatFormGPNVMap[Index].MinGPNVSize;

  GPNVSize = (UINT16 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->GPNVSize);
  *GPNVSize = (UINT16)GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize;

  NVStorageBase = (UINT32 *)PnpFarToLinear (((PNP_FUNCTION_0x55_FRAME *)Frame)->NVStorageBase);
  *NVStorageBase = (UINT32)GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress;

  Index++;
  if (Index >= FixedPcdGet16 (PcdDefaultGpnvMapBufferSize)) {
    return DMI_INVALID_HANDLE;
  }

  *Handle = GPNVMapBuffer.PlatFormGPNVMap[Index].Handle;

  return DMI_SUCCESS;
}

/**
  PnP function 0x56, Set SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x56 (
  IN VOID            *Frame
  )
{
  UINT16                                Handle;
  UINT8                                 *GPNVBuffer;
  UINT16                                Index;


  Handle = (UINT16)((PNP_FUNCTION_0x56_FRAME *)Frame)->Handle;
  GPNVBuffer = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x56_FRAME *)Frame)->GPNVBuffer);

  Index = Handle;
  if (!VaildGPNVCheck (&Index, Handle)) {
    return DMI_INVALID_HANDLE;
  }

  CopyMem (
    GPNVBuffer,
    (VOID *)(UINTN)GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress,
    GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize
    );

  return DMI_SUCCESS;
}

/**
  PnP function 0x57, Set SMBIOS Structure.

  @param[In]      Frame           Pointer to input stack frame of the PnP call.

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
INT16
Pnp0x57 (
  IN VOID            *Frame
  )
{
  UINT16                                Handle;
  UINT8                                 *GPNVBuffer;
  UINT16                                Index;


  Handle = (UINT16)((PNP_FUNCTION_0x57_FRAME *)Frame)->Handle;
  GPNVBuffer = (UINT8 *)PnpFarToLinear (((PNP_FUNCTION_0x57_FRAME *)Frame)->GPNVBuffer);

  Index = Handle;
  if (! VaildGPNVCheck (&Index, Handle)) {
    return DMI_INVALID_HANDLE;
  }

  WriteGPNV (Index, GPNVBuffer);

  return DMI_SUCCESS;
}

VOID
PnpRedirectedEntryPoint (
  IN OUT EFI_DWORD_REGS                 *RegBufferPtr
  )
{
  PNP_GENERIC_ENTRY_FRAME               *Frame;
  INT16                                 ReturnStatus;
  UINT32                                CR3Value;
  UINT32                                CR4Value;


  GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &CR3Value);
  GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RCX, mCpuIndex, &CR4Value);

  Frame = (PNP_GENERIC_ENTRY_FRAME *)(UINTN)LogicalToPhysicalAddress (
                                              (UINT32)RegBufferPtr->ESP,
                                              CR3Value,
                                              CR4Value,
                                              (UINTN)RegBufferPtr->SS
                                              );

  if ((Frame->Function & ~0x7) == 0x50) {
    ReturnStatus = mPnpDmiFunctions[Frame->Function & 0x7]((VOID *)Frame);
  } else {
    ReturnStatus = PNP_FUNCTION_NOT_SUPPORTED;
  }

  *((INT16 *)&(RegBufferPtr->EAX)) = ReturnStatus;
}

/**
  This fucntion is triggered by SMI function call 0x47.

  @param[In]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[In]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[In]      CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[In]      CommBufferSize  The size of the CommBuffer.

  @retval     EFI_SUCCESS              The callabck was handled successfully.
  @retval     EFI_OUT_OF_RESOURCES     Cannot locate SmmCpuProtocol.
  @retval     EFI_NOT_FOUND            Cannot determine the source of SMI.

**/
EFI_STATUS
EFIAPI
PnPBiosCallback (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  )
{
  EFI_DWORD_REGS                        *RegBufferPtr;
  UINT32                                CR3Value;
  UINT32                                CR4Value;
  UINTN                                 SegementSelector;
  UINTN                                 Index;
  PNP_GENERIC_ENTRY_FRAME               *Frame;
  INT16                                 ReturnStatus;
  EFI_STATUS                            Status;
  UINT32                                Eax;
  UINT32                                Edx;
  UINT32                                Ebx;
  UINT32                                Ecx;
  UINT32                                Esi;
  UINT32                                Ss;


  //
  // Locate Smm Cpu protocol for Cpu save state manipulation
  //
  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **)&mSmmCpu
                    );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RAX, Index, &Eax);
    Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RDX, Index, &Edx);
    //
    // Find out which CPU triggered PnP SMI
    //
    if (((Eax & 0xff) == SMM_PnP_BIOS_CALL) && ((Edx & 0xffff) == PcdGet16 (PcdSoftwareSmiPort))) {
      //
      // Cpu found!
      //
      break;
    }
  }
  if (Index == mSmst->NumberOfCpus) {
    //
    // Error out due to CPU not found
    //
    return EFI_NOT_FOUND;
  }
  mCpuIndex = Index;

  //
  // This should be called once during POST, in order to get the SMBIOS table entry address.
  //
  if (mSMBIOSTableEntryAddress == 0) {
    EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID **)&mSMBIOSTableEntryAddress);
    return EFI_SUCCESS;
  }

  Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &Ecx);
  Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, mCpuIndex, &Ebx);
  Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RSI, mCpuIndex, &Esi);
  if (Ebx == SIGNATURE_32 ('$', 'I', 'S', 'B')) {
    //
    // Call by new way that AP trigger software SMI directly when the signature is "$ISB'
    //
    Frame = (PNP_GENERIC_ENTRY_FRAME *)(UINTN)Esi;

    if ((Frame->Function & ~0x7) == 0x50) {
      ReturnStatus = mPnpDmiFunctions[Frame->Function & 0x7](Frame);
    } else {
      ReturnStatus = PNP_FUNCTION_NOT_SUPPORTED;
    }

    Eax = (UINT32)ReturnStatus;
    Status = SetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RAX, mCpuIndex, &Eax);
  } else {
    Status = GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_SS, mCpuIndex, &Ss);
    //
    // Call by csm16
    //
    CR3Value = Ebx;
    CR4Value = Ecx;
    SegementSelector = (UINTN)Ss;
    RegBufferPtr = (EFI_DWORD_REGS *)(UINTN)LogicalToPhysicalAddress (Esi, CR3Value, CR4Value, SegementSelector);
    PnpRedirectedEntryPoint (RegBufferPtr);
  }

  return EFI_SUCCESS;
}

/**
  Clear BIOS event log data from GPNV area.

  @param[In]  GPNVBase               GPNV Start address
  @param[In]  GPNVLength             GPNV Length

  @retval     DMI_SUCCESS            The function was executed successfully.
  @retval     DMI_INVALID_HANDLE     Invalid handle.

**/
STATIC
EFI_STATUS
ClearBiosEventLog (
  IN UINTN                              GPNVBase,
  IN UINTN                              GPNVLength
  )
{
  EFI_STATUS                            Status;
  UINT8                                 *Buffer;
  UINT32                                Index;
  UINTN                                 BlockSize;
  UINTN                                 FdSupportEraseSize;
  UINTN                                 EraseCount;
  UINTN                                 EraseStartAddress;

  FdSupportEraseSize = GetFlashBlockSize();
  EraseStartAddress = GPNVBase & (~(FdSupportEraseSize - 1));
  EraseCount = GET_ERASE_SECTOR_NUM ( 
                                 GPNVBase,
                                 GPNVLength,
                                 EraseStartAddress,
                                 FdSupportEraseSize
                                 );
  
  BlockSize = FdSupportEraseSize * EraseCount;

  Status = mSmst->SmmAllocatePool(
                    EfiRuntimeServicesData,
                    BlockSize,
                    (VOID **)&Buffer
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //Copy whole block data to buffer
  //
  CopyMem(Buffer, (VOID *)(UINTN)EraseStartAddress, BlockSize);

  //
  //Copy modified GPNV data to buffer
  //
  for (Index = 0; Index < GPNVLength; Index++) {
    *(UINT8 *)((Buffer + (GPNVBase) - (GPNVBase & ((UINT32)(~0)) & (~(FdSupportEraseSize - 1))))+Index) = 0xFF;
  }

  //
  //Flash GPNV
  //
  for (Index = 0; Index < EraseCount; Index++) {
    
    Status = FlashErase (
                         EraseStartAddress + FdSupportEraseSize * Index,
                         FdSupportEraseSize
                         );
  }

  Status = FlashProgram (
                        (UINT8 *)EraseStartAddress,
                        Buffer,
                        &BlockSize,
                        EraseStartAddress
                        );

  Status = mSmst->SmmFreePool (Buffer);

  return Status;
}

