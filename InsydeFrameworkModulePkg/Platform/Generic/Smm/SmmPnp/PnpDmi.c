//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "Pnp.h"
#include "PnpDmi.h"

extern EFI_PHYSICAL_ADDRESS       gSMBIOSTableEntryAddress;
SMBIOS_TABLE_ENTRY_POINT *mSmbiosTableEntryPoint = NULL;
SMBIOS_STRUCTURE *mSmbiosTable = NULL;

INT32 TotalIncreasedStringLength = 0;

VOID
LocateSmbiosTable (
  )
/*++

Routine Description:

  Ensures that mSmbiosTableEntryPoint and mSmbiosTable are filled in.

Arguments:

Returns:

  none

--*/
{
  UINT32             *Address;

  if ((mSmbiosTableEntryPoint == NULL) || ((UINT32)(UINTN)mSmbiosTableEntryPoint->AnchorString != (UINT32)SMBIOS_SIGNATURE)) {
    for (Address = (UINT32 *)(UINTN) 0xF0000; (UINTN)Address < 0x100000; Address += 4) {
      mSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN) Address;
      if (*(UINT32 *)mSmbiosTableEntryPoint == (UINT32)SMBIOS_SIGNATURE) {
        mSmbiosTable = (SMBIOS_STRUCTURE*)(UINTN)mSmbiosTableEntryPoint->TableAddress;
        return;
      }
    }

  }
  
  //
  // Locate SMBIOS structure table entry point at Runtime.
  //
  mSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT*)(UINTN)gSMBIOSTableEntryAddress;
  mSmbiosTable = (SMBIOS_STRUCTURE*)(UINTN)mSmbiosTableEntryPoint->TableAddress;

  return;
}

EFI_STATUS
LocateSmbiosStructure (
  IN OUT UINT16                         *Structure,
  OUT SMBIOS_STRUCTURE                  **Location,
  OUT UINTN                             *Size
  )
/*++

Routine Description:

  Locates the smbios structure with a handle of *Structure.

Arguments:

  *Structure - Handle of structure to attempt to locate.
    - If 0 on entry, then the first avaiable structure is found.
    - On exit, *Structure is updated with the next structure handle.
      A return handle of 0xffff indicates there are no more structures.
  **Location - Used to return the structure location if it was found.
  *Size - Used to the size of structure location if it was found.

Returns:

  EFI_SUCCESS - the structure was found.  **Location and *Size are valid.
  EFI_NOT_FOUND - the structure was not found.

  *Structure is always updated with useful information

--*/
{
  SMBIOS_STRUCTURE                      *Iter, *Next, *End;

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
    for(Next = (SMBIOS_STRUCTURE *)((UINT8 *)Iter + Iter->Length); *(UINT16 *)Next != 0; Next = (SMBIOS_STRUCTURE *)((UINTN)Next + 1));
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
      *Size = (UINT16)((UINTN)Next - (UINTN)Iter);
      return EFI_SUCCESS;
    }
    Iter = Next;
  }
}

EFI_STATUS
LocateStringOfStructure (
  IN UINT8                              StringRef,
  IN SMBIOS_STRUCTURE                   *Structure,
  OUT CHAR8                             **Location,
  OUT UINTN                             *Size
  )
/*++

Routine Description:

  Find the location and size of a string within a smbios structure

Arguments:

  StringRef - The string number to search for within the structure.
  *Structure - Pointer to the structure to search.

  **Location - Used to return the string location if it was found.
  *Size - Used to the size of string location if it was found.

Returns:

  EFI_SUCCESS - the string was found.  **Location and *Size are valid.
  EFI_NOT_FOUND - the structure was not found.

--*/
{
  CHAR8                                 *Next;

  if (StringRef == 0) {
    return EFI_NOT_FOUND;
  }

  Next = (UINT8*)(((UINTN)Structure) + Structure->Length);
  do {
    *Location = Next;
    *Size = EfiAsciiStrLen (*Location);
    Next = *Location + *Size + 1;
  } while ((--StringRef) != 0 && (*Next != 0));

  return (StringRef == 0) ? EFI_SUCCESS : EFI_NOT_FOUND;
}

INT16
Pnp0x50 (
  IN PNP_FUNCTION_0x50_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x50, Get SMBIOS Information.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
  UINT8                                 *DmiBIOSRevision;
  UINT16                                *NumStructures;
  UINT16                                *StructureSize;
  UINT32                                *DmiStorageBase;
  UINT16                                *DmiStorageSize;

  LocateSmbiosTable ();

  DmiBIOSRevision = (UINT8 *)PnpFarToLinear (Frame->DmiBIOSRevision);
  *DmiBIOSRevision = mSmbiosTableEntryPoint->SmbiosBcdRevision;

  NumStructures = (UINT16 *)PnpFarToLinear (Frame->NumStructures);
  *NumStructures = mSmbiosTableEntryPoint->NumberOfSmbiosStructures;

  StructureSize = (UINT16 *)PnpFarToLinear (Frame->StructureSize);
  *StructureSize = mSmbiosTableEntryPoint->MaxStructureSize;

  DmiStorageBase = (UINT32 *)PnpFarToLinear (Frame->DmiStorageBase);
  *DmiStorageBase = (UINT32)(UINTN)mSmbiosTable;

  DmiStorageSize = (UINT16 *)PnpFarToLinear (Frame->DmiStorageSize);
  *DmiStorageSize = mSmbiosTableEntryPoint->TableLength;

  return DMI_SUCCESS;
}

INT16
Pnp0x51 (
  IN PNP_FUNCTION_0x51_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x51, Get SMBIOS Structure.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
  UINT16                                *Structure;
  UINT8                                 *DmiStrucBuffer;
  SMBIOS_STRUCTURE                      *Location;
  UINTN                                 Size;

  LocateSmbiosTable();

  Structure = (UINT16 *)PnpFarToLinear (Frame->Structure);
  DmiStrucBuffer = (UINT8 *)PnpFarToLinear (Frame->DmiStrucBuffer);
  Location = (SMBIOS_STRUCTURE *)DmiStrucBuffer;
  if (!EFI_ERROR (LocateSmbiosStructure (Structure, &Location, &Size))) {
    EfiCopyMem (DmiStrucBuffer, (VOID *)Location, Size);
    return DMI_SUCCESS;
  } else {
    return DMI_INVALID_HANDLE;
  }
}

INT16
Pnp0x52 (
  IN PNP_FUNCTION_0x52_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x52, Set SMBIOS Structure

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
#if (defined(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)) && \
    (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE > 9)
  FUNC_0x52_DATA_BUFFER                 *Data;
  SMBIOS_STRUCTURE                      *StructureLocation;
  UINT16                                StructureHandle;
  UINTN                                 StructureSize;
  INT16                                 ReturnStatus;

  Data = (FUNC_0x52_DATA_BUFFER *)PnpFarToLinear (Frame->DmiDataBuffer);
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
  }

  return ReturnStatus;
#else
  return DMI_READ_ONLY;
#endif
}

#if (defined(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)) && \
    (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE > 9)

EFI_STATUS
DMISpaceNotEnough (
  OUT UINT16                            *DmiEnd
  )
/*++

Routine Description:

  If OEM_DMI_STORE address full to arrange.

Arguments:

  Data - Pointer to function 0x52 data buffer input

Returns:

  None

--*/
{
  EFI_STATUS                  Status;
  UINTN                       BlockSize       = FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE;
  UINT8                       *NotEnoughBuffer= 0;
  UINT16                      NotEnoughPtr    = 4;
  UINT8                       *Buffer         = NULL;
  DMI_STRING_STRUCTURE        *CurrentPtr     = NULL;

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    BlockSize,  //0x1000
                    &NotEnoughBuffer
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EfiSetMem (NotEnoughBuffer, FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE, 0xFF);
  *(UINT32 *)NotEnoughBuffer = DMI_UPDATA_STRING_SIGNATURE;

  //
  // Search smbios Valid ,If it be found, set OEM_DMI_STORE address
  //
  Buffer = (UINT8 *)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE + sizeof (DMI_UPDATA_STRING_SIGNATURE));
  Buffer = (UINT8 *)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE + sizeof (DMI_UPDATA_STRING_SIGNATURE));
  while ((UINTN)Buffer <
         (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE +
          FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)) {

    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;
    if (CurrentPtr->Type == 0xFF) {
      break;
    }
    if (CurrentPtr->Valid == 0xFF) {
      EfiCopyMem ((NotEnoughBuffer + NotEnoughPtr), Buffer, CurrentPtr->Length);
      NotEnoughPtr = NotEnoughPtr + CurrentPtr->Length;
    }
    Buffer = Buffer + CurrentPtr->Length;
  }

  //
  // Flash whole buffer to rom
  //
  Status = SmmFwBlockService->EraseBlocks (
                                SmmFwBlockService,
                                FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE,
                                &BlockSize
                                );
  if (!EFI_ERROR (Status)) {
    Status = SmmFwBlockService->Write (
                                  SmmFwBlockService,
                                  FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE,
                                  &BlockSize,
                                  NotEnoughBuffer
                                  );
  }
  mSmst->SmmFreePool (NotEnoughBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  if ((UINTN)NotEnoughPtr >=
      (UINTN)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE +
              FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)) {

    return EFI_OUT_OF_RESOURCES;
  }

  *DmiEnd = NotEnoughPtr;

  return EFI_SUCCESS;
}

EFI_STATUS
SetDMI (
  FUNC_0x52_DATA_BUFFER                 *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
/*++

Routine Description:

  Set SMBIOS Structure to OEM_DMI_STORE address.

Arguments:

  Data - Pointer to function 0x52 data buffer input

Returns:

  None

--*/
{
  EFI_STATUS                  Status;
  UINTN                       BlockSize = 0x1000;
  UINTN                       BSize = 0x1;
  UINT8                       Invalid = 0;
  UINT8                       *Buffer = NULL;
  UINT8                       *Ptr = NULL;
  UINT8                       *SavePtr = NULL;
  UINT16                      DmiEnd=4;
  UINTN                       Index;
  DMI_STRING_STRUCTURE        *CurrentPtr = NULL;

  Ptr = (UINT8 *)(UINTN)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE);

  //
  // First 4 bytes are "$DMI"
  //
  Buffer = Ptr + sizeof (DMI_UPDATA_STRING_SIGNATURE);

  //
  // Search OEM_DMI_STORE free space
  //
  while (DmiEnd < FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE) {
    CurrentPtr = (DMI_STRING_STRUCTURE*)Buffer;

    if (CurrentPtr->Type == 0xFF) {
      //
      // The space is free, we could use it, so break.
      //
      break;
    }

    if (CurrentPtr->Type == Structure->Type &&
        CurrentPtr->Offset == Data->FieldOffset &&
        CurrentPtr->Valid == 0xFF) {
      //
      // If we find the same data in OEM_DMI_STORE space,
      // set invalid byte in it.
      //
      Status = SmmFwBlockService->Write (SmmFwBlockService,
                                         FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE + DmiEnd + 2,
                                         &BSize,
                                         &Invalid
                                         );
    }

    //
    // Try to find next.
    //
    DmiEnd = DmiEnd + CurrentPtr->Length;
    Buffer = Buffer + CurrentPtr->Length;
  }//end while

  if (DmiEnd + Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof(UINT8) - 1) >
      FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE) {
    //
    // If OEM_DMI_STORE space full, reclaim area,
    // and return new address of free space address.
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
                    Data->DataLength + (sizeof(DMI_STRING_STRUCTURE) - sizeof(UINT8) - 1), //FLASH_REGION_NVSTORAGE_SUBREGION_OEM_DMI_STORE_SIZE-4, //Data->DataLength+(sizeof(DMI_STRING_STRUCTURE)-sizeof(UINT8)-1),
                    &SavePtr
                    );
  if (EFI_ERROR (Status)) {
      return Status;
  }

  CurrentPtr = (DMI_STRING_STRUCTURE*)SavePtr;
  CurrentPtr->Type = Structure->Type;
  CurrentPtr->Offset = Data->FieldOffset;
  CurrentPtr->Valid = 0xFF;

  switch (Data->Command) {

  case DMI_STRING_CHANGE:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8)-1);
    for (Index = 0; Index + 1 < Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;

  default:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof( UINT8));
    for (Index = 0; Index <= Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;
  }

  BlockSize = CurrentPtr->Length;
  Status = SmmFwBlockService->Write (
                                SmmFwBlockService,
                                FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE + DmiEnd,
                                &BlockSize,
                                SavePtr
                                );

  mSmst->SmmFreePool (SavePtr);

  return Status;
}

INT16
Pnp0x52ChangeString(
  FUNC_0x52_DATA_BUFFER                 *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
/*++

Routine Description:

  Function 0x52, command = change a single byte of information

Arguments:

  Data - Pointer to function 0x52 data buffer input

Returns:

  INT16 to return from Pnp function call

--*/
{
  CHAR8                                 *StructureString;
  CHAR8                                 *TempstructureString = NULL;
  UINTN                                 StructureStringSize;
  UINTN                                 TempstructureStringSize = 0;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  UINT8                                 *SmbiosStoreArea = NULL;
  UINTN                                 CurrentTableSize;
  UINT8                                 *DmiPtr = NULL;
  UINTN                                 TotalSmbiosBufferSize;
  UINTN                                 NewTableSize;
  

  DmiPtr = (UINT8 *)(UINTN)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE);

  if (!(*(UINT32 *)DmiPtr == DMI_UPDATA_STRING_SIGNATURE)){
    return DMI_READ_ONLY;
  }

  if (Data->DataLength > 0xff || Data->DataLength <= 1) {
    return DMI_BAD_PARAMETER;
  }

  //
  // Find string within the SMBIOS structure
  //
  if (Structure->Type == 0x0b || Structure->Type == 0x0c) {
    if (EFI_ERROR (LocateStringOfStructure(
                     (UINT8) Data->FieldOffset,
                     Structure,
                     &StructureString,
                     &StructureStringSize
                     ))) {
      return DMI_BAD_PARAMETER;
    }
    Data->FieldOffset += 4;
  } else {
    if (EFI_ERROR (LocateStringOfStructure(
                     ((UINT8*)Structure)[Data->FieldOffset],
                     Structure,
                     &StructureString,
                     &StructureStringSize
                     ))) {
      return DMI_BAD_PARAMETER;
    }
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
    TempstructureStringSize = (UINTN)(mSmbiosTableEntryPoint->TableLength + (UINT16)TotalIncreasedStringLength) - (UINTN)(TempstructureString - mSmbiosTableEntryPoint->TableAddress);
  } else {
    CurrentTableSize = (UINTN)(mSmbiosTableEntryPoint->TableLength - (UINT16)(~(TotalIncreasedStringLength - 1)));
    TempstructureStringSize = (UINTN)(mSmbiosTableEntryPoint->TableLength - (UINT16)(~(TotalIncreasedStringLength - 1))) - (UINTN)(TempstructureString - mSmbiosTableEntryPoint->TableAddress);
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

  Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, CurrentTableSize, &SmbiosStoreArea);
  if (EFI_ERROR (Status)) {
    return PNP_BUFFER_TOO_SMALL;
  }

  EfiCopyMem (SmbiosStoreArea, TempstructureString, TempstructureStringSize);
  EfiCopyMem (StructureString, Data->StructureData, Data->DataLength);
  TempstructureString = StructureString + Data->DataLength;
  EfiCopyMem (TempstructureString, SmbiosStoreArea, TempstructureStringSize);
  Status = mSmst->SmmFreePool (SmbiosStoreArea);

  TotalIncreasedStringLength = TotalIncreasedStringLength + (UINT32)Data->DataLength - (UINT32)(StructureStringSize + 1);
  return DMI_SUCCESS;
}

INT16
Pnp0x52ChangeBlock(
  FUNC_0x52_DATA_BUFFER                 *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
/*++

Routine Description:

  Function 0x52, command = change a block of information

Arguments:

  Data - Pointer to function 0x52 data buffer input

Returns:

  INT16 to return from Pnp function call

--*/
{
  UINT8                       *TmpPtr;
  UINT8                       *DataPtr;
  UINTN                       CopyLength;
  EFI_STATUS                  Status;


  TmpPtr = (UINT8 *) ((UINTN)Structure + Data->FieldOffset);
  DataPtr = (UINT8 *)Data->StructureData;
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
  
  EfiCopyMem (TmpPtr, Data->StructureData, Data->DataLength);
  return DMI_SUCCESS;
}

INT16
Pnp0x52ChangeFixedLength(
  FUNC_0x52_DATA_BUFFER             *Data,
  SMBIOS_STRUCTURE                  *Structure
  )
{
  UINT8                       *ByteTmpPtr = 0;
  UINT16                      *WordTmpPtr = 0;
  UINT32                      *DWordTmpPtr = 0;
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT8                       ByteTemp = 0;
  UINT16                      WordTemp = 0;
  UINT32                      DWordTemp = 0;
  UINTN                       Index;

  for (Index = 0; Index < UpdateableStringCount; Index++) {
    if ((Structure->Type == mUpdatableStrings[Index].Type)
        &&(Data->FieldOffset == mUpdatableStrings[Index].FixedOffset)) {
      break;
    }
  }

  if (Index == UpdateableStringCount) {
    return DMI_READ_ONLY;
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    ByteTmpPtr = (UINT8 *) ((UINTN)Structure + Data->FieldOffset);
    ByteTemp = *ByteTmpPtr;
    ByteTemp = ByteTemp & ((UINT8)(Data->ChangeMask));
    ByteTemp = ByteTemp | ((UINT8)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT8);
    EfiCopyMem((VOID *)&Data->StructureData, (VOID *)&ByteTemp, Data->DataLength);

    break;

  case DMI_WORD_CHANGE:
    WordTmpPtr = (UINT16 *) ((UINTN)Structure + Data->FieldOffset);
    WordTemp = *WordTmpPtr;
    WordTemp = WordTemp & ((UINT16)(Data->ChangeMask));
    WordTemp = WordTemp | ((UINT16)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT16);
    EfiCopyMem((VOID *)&Data->StructureData, (VOID *)&WordTemp, Data->DataLength);

    break;

  case DMI_DWORD_CHANGE:
    DWordTmpPtr = (UINT32 *) ((UINTN)Structure + Data->FieldOffset);
    DWordTemp = *DWordTmpPtr;
    DWordTemp = DWordTemp & ((UINT32)(Data->ChangeMask));
    DWordTemp = DWordTemp | ((UINT32)((Data->ChangeValue) & ~(Data->ChangeMask)));
    
    Data->DataLength = sizeof (UINT32);
    EfiCopyMem((VOID *)&Data->StructureData, (VOID *)&DWordTemp, Data->DataLength);

    break;
  }

  Status = SetDMI (Data, Structure);

  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    EfiCopyMem (ByteTmpPtr, &ByteTemp, sizeof (UINT8));
    break;

  case DMI_WORD_CHANGE:
    EfiCopyMem (WordTmpPtr, &WordTemp, sizeof (UINT16));
    break;

  case DMI_DWORD_CHANGE:
    EfiCopyMem (DWordTmpPtr, &DWordTemp, sizeof(UINT32));
    break;
  }
  
  return DMI_SUCCESS;
}

#endif

INT16
Pnp0x54 (
  IN PNP_FUNCTION_0x54_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x54, Set SMBIOS Structure

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
  INT16                                SubFunction;
  UINT8                                *Data;
  UINT8                                Control;

  SubFunction = (UINT16)Frame->SubFunction;
  Data = (UINT8 *)PnpFarToLinear (Frame->Data);
  Control = (UINT8)Frame->Control;

  if ((Control&0x01) == 0) {
    if (SubFunction >= 1) {
      return DMI_INVALID_SUBFUNCTION;
    }
    return DMI_SUCCESS;
  }
  
  switch (SubFunction) {

  case 0:
    mSmmRT->LocateProtocol (
              &gEfiEventLogProtocolGuid,
              NULL,
              &SmmEventLog
              );
    if (SmmEventLog!=NULL) {
      SmmEventLog->Clear (SmmEventLog);
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

INT16
Pnp0x55 (
  IN PNP_FUNCTION_0x55_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x55, Get SMBIOS Information.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
  UINT16                                *Handle;
  UINT16                                HandleTemp;
  UINT16                                *MinGPNVRWSize;
  UINT16                                *GPNVSize;
  UINT32                                *NVStorageBase;
  UINT16                                Index;


  Handle = (UINT16 *)PnpFarToLinear (Frame->Handle);

  Index = HandleTemp = *Handle;

  if (!VaildGPNVCheck(&Index, HandleTemp)) {
    return  DMI_INVALID_HANDLE;
  }

  MinGPNVRWSize = (UINT16 *)PnpFarToLinear (Frame->MinGPNVRWSize);
  *MinGPNVRWSize = (UINT16)GPNVMapBuffer.PlatFormGPNVMap[Index].MinGPNVSize;

  GPNVSize = (UINT16 *)PnpFarToLinear (Frame->GPNVSize);
  *GPNVSize = (UINT16)GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize;

  NVStorageBase = (UINT32 *)PnpFarToLinear (Frame->NVStorageBase);
  *NVStorageBase = (UINT32)GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress;

  Index++;
  *Handle = GPNVMapBuffer.PlatFormGPNVMap[Index].Handle;

  return DMI_SUCCESS;
}


INT16
Pnp0x56 (
  IN PNP_FUNCTION_0x56_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x56, Get SMBIOS Information.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
  UINT16                                Handle;
  UINT8                                 *GPNVBuffer;
  UINT16                                Index;


  Handle = (UINT16)Frame->Handle;
  GPNVBuffer = (UINT8 *)PnpFarToLinear (Frame->GPNVBuffer);

  Index = Handle;
  if (!VaildGPNVCheck (&Index, Handle)) {
    return DMI_INVALID_HANDLE;
  }

  Memcpy(
    GPNVBuffer,
    (VOID *)(UINTN)GPNVMapBuffer.PlatFormGPNVMap[Index].BaseAddress,
    GPNVMapBuffer.PlatFormGPNVMap[Index].GPNVSize
    );

  return DMI_SUCCESS;
}

BOOLEAN
VaildGPNVCheck (
  IN OUT UINT16                     *Index,
  IN UINT16                         Handle
  )
{
  UINT16                            TempIndex;

  TempIndex = *Index;
  if (mOemGPNVHandleTab == NULL) {
    if (Handle > (sizeof (PLATFORM_GPNV_MAP_BUFFER) / sizeof (PLATFORM_GPNV_MAP)))
      return FALSE;

    if (GPNVMapBuffer.PlatFormGPNVMap[TempIndex].Handle == 0xffff)
      return FALSE;
  } else {
    for (TempIndex = 0; TempIndex < OemGPNVHandleCount; TempIndex++) {
      if (GPNVMapBuffer.PlatFormGPNVMap[TempIndex].Handle == Handle) {
        *Index = TempIndex;
        return TRUE;
      }
    }
    return FALSE;
  }

  return TRUE;
}

INT16
Pnp0x57 (
  IN PNP_FUNCTION_0x57_FRAME            *Frame
  )
/*++

Routine Description:

  PnP function 0x57, Get SMBIOS Information.

Arguments:

  Frame - Pointer to input stack frame of the PnP call.

Returns:

  None

--*/
{
  UINT16                                Handle;
  UINT8                                 *GPNVBuffer;
  EFI_STATUS                            Status;
  UINT16                                Index;


  Handle = (UINT16)Frame->Handle;
  GPNVBuffer = (UINT8 *)PnpFarToLinear (Frame->GPNVBuffer);

  if (!VaildGPNVCheck (&Index, Handle)) {
    return DMI_INVALID_HANDLE;
  }

  Status = WriteGPNV (Index,GPNVBuffer);
  return DMI_SUCCESS;
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
                    &WritingBuffer
                    );

  BlockBaseAddress = (GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress) & (~0xFFFF);
  //
  // Copy whole block data to buffer
  //
  Memcpy(
    WritingBuffer,
    (VOID *)(UINTN)BlockBaseAddress,
    BlockSize
    );

  //
  // Copy modified GPNV data to buffer
  //
  WritingBufferOffset = GPNVMapBuffer.PlatFormGPNVMap[Handle].BaseAddress - BlockBaseAddress;
  Memcpy(
    (void *)(WritingBuffer + WritingBufferOffset),
    (void *)GPNVBuffer,
    GPNVMapBuffer.PlatFormGPNVMap[Handle].GPNVSize
    );

  //
  // Flash whole buffer to rom
  //
  EraseCount = 0;
  WriteCount = 0;
  do {
    Status = SmmFwBlockService->EraseBlocks (
                                  SmmFwBlockService,
                                  BlockBaseAddress,
                                  (UINTN *)&BlockSize
                                  );

    if (!EFI_ERROR (Status)) {
      Status = SmmFwBlockService->Write (
                                    SmmFwBlockService,
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

VOID
Memcpy (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  )

{
  CHAR8 *Destination8;
  CHAR8 *Source8;

  Destination8 = Destination;
  Source8 = Source;
  while (Length--) {
    *(Destination8++) = *(Source8++);
  }
}
