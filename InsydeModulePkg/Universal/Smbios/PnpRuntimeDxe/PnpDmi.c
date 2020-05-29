/** @file

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

#include "PnpDmi.h"
#include "PnpRuntimeDxe.h"
#include <Library/BaseMemoryLib.h>
#include <Guid/DebugMask.h>

#define DMI_UPDATA_STRING_SIGNATURE SIGNATURE_32 ('$', 'D', 'M', 'I')

INT32                          TotalIncreasedStringLength = 0;


/**
 Find Smbios structure by Handle value

 @param [in, out] Structure
 @param [out]  Location
 @param [out]  Size

 @retval EFI_SUCCESS            The Smbios structure is found
 @retval EFI_NOT_FOUND          Not found

**/
EFI_STATUS
LocateSmbiosStructure (
  IN OUT UINT16                         *Structure,
  OUT SMBIOS_STRUCTURE                  **Location,
  OUT UINTN                             *Size
  )
{
  SMBIOS_STRUCTURE            *Iter;
  SMBIOS_STRUCTURE            *Next;
  SMBIOS_STRUCTURE            *End;

  if (mPrivateData == NULL || mPrivateData->SmbiosTable == NULL || mPrivateData->SmbiosTableEntryPoint == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((*Structure == 0) && ((*Location) != NULL) && (*(UINT32 *)(*Location) == (UINT32)SMBIOS_SIGNATURE)) {
    *Structure = mPrivateData->SmbiosTable->Handle;
  }

  Iter = mPrivateData->SmbiosTable;
  End = (SMBIOS_STRUCTURE *)((UINTN)mPrivateData->SmbiosTable + mPrivateData->SmbiosTableEntryPoint->TableLength);
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
      //
      // The Handle is found
      //
      *Structure = (UINT16)((Next < End) ? Next->Handle : 0xffff);
      *Location = Iter;
      *Size = (UINT16)((UINTN)Next - (UINTN)Iter);
      return EFI_SUCCESS;
    }
    Iter = Next;
  }
}


/**
 Find the string in the Smbios structure

 @param [in]   StringRef        Index of the string to be found (1 based)
 @param [in]   Structure
 @param [out]  Location
 @param [out]  Size

 @retval EFI_SUCCESS            The string is found
 @retval EFI_NOT_FOUND          Not found

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
 PnP SMBIOS function 0x50, Get SMBIOS Information

 @param [in]   CommBuf

 @retval EFI_SUCCESS            The DMI status is in ReturnCode of GenericEntryFrame

**/
EFI_STATUS
Pnp0x50 (
  IN  VOID        *CommBuf
  )
{
  UINT8                       *DmiBIOSRevision;
  UINT16                      *NumStructures;
  UINT16                      *StructureSize;
  UINT32                      *DmiStorageBase;
  UINT16                      *DmiStorageSize;

  RS_PNP_FUNCTION_0x50_FRAME  *Frame;
  UINT8                       *PnpFrame;

  PnpFrame = (UINT8 *)CommBuf;
  PnpFrame += sizeof (IRSI_HEADER);
  Frame = (RS_PNP_FUNCTION_0x50_FRAME *)PnpFrame;

  if (mPrivateData == NULL || mPrivateData->SmbiosTableEntryPoint == NULL || mPrivateData->SmbiosTable == NULL) {
    return EFI_UNSUPPORTED;
  }
  //
  // copy to address
  //
  DmiBIOSRevision = (UINT8 *)(UINTN)Frame->DmiBIOSRevisionAddress;
  *DmiBIOSRevision = mPrivateData->SmbiosTableEntryPoint->SmbiosBcdRevision;

  NumStructures = (UINT16 *)(UINTN)Frame->NumStructuresAddress;
  *NumStructures = mPrivateData->SmbiosTableEntryPoint->NumberOfSmbiosStructures;

  StructureSize = (UINT16 *)(UINTN)Frame->StructureSizeAddress;
  *StructureSize = mPrivateData->SmbiosTableEntryPoint->MaxStructureSize;

  DmiStorageBase = (UINT32 *)(UINTN)Frame->DmiStorageBaseAddress;
  //
  // In Runtime (64-bit address), it may be truncated
  //
  *DmiStorageBase = (UINT32)(UINTN)mPrivateData->SmbiosTable;

  DmiStorageSize = (UINT16 *)(UINTN)Frame->DmiStorageSizeAddress;
  *DmiStorageSize = mPrivateData->SmbiosTableEntryPoint->TableLength;

  Frame->GenericEntryFrame.ReturnCode = DMI_SUCCESS;

  return EFI_SUCCESS;

}


/**
 PnP SMBIOS function 0x51, Get SMBIOS Structure

 @param [in]   CommBuf

 @retval EFI_SUCCESS            The DMI status is in ReturnCode of GenericEntryFrame

**/
EFI_STATUS
Pnp0x51 (
  IN  VOID        *CommBuf
  )
{
  UINT16                                *Structure;
  UINT8                                 *DmiStrucBuffer;
  SMBIOS_STRUCTURE                      *Location;
  UINTN                                 Size;

  RS_PNP_FUNCTION_0x51_FRAME            *Frame;
  UINT8                                 *PnpFrame;

  PnpFrame = (UINT8 *)CommBuf;
  PnpFrame += sizeof (IRSI_HEADER);
  Frame= (RS_PNP_FUNCTION_0x51_FRAME *)PnpFrame;

  Structure = (UINT16 *)(UINTN)Frame->StructureAddress;
  DmiStrucBuffer = (UINT8 *)(UINTN)Frame->DmiStrucBufferAddress;
  Location = (SMBIOS_STRUCTURE *)DmiStrucBuffer;
  //
  // Locate the Smbios structure by Handle value
  //
  if (!EFI_ERROR (LocateSmbiosStructure (Structure, &Location, &Size))) {
    CopyMem (DmiStrucBuffer, (VOID *)Location, Size);
    Frame->GenericEntryFrame.ReturnCode = DMI_SUCCESS;
    return EFI_SUCCESS;
  } else {
    Frame->GenericEntryFrame.ReturnCode = DMI_INVALID_HANDLE;
    return EFI_SUCCESS;
  }

}


/**
 Helper function for Pnp0x52, store the SetStructure info into Variable

 @param        Data
 @param        Structure

 @retval Status                 The return status of EfiGetVariable, EfiSetVariable

**/
EFI_STATUS
SetDMI (
  RS_FUNC_0x52_DATA_BUFFER              *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
{
  EFI_STATUS                  Status;
  UINTN                       VarSize;
  UINTN                       DmiEnd;
  UINT8                       *Buffer;
  DMI_STRING_STRUCTURE        *CurrentPtr;
  DMI_STRING_STRUCTURE        *ChangePtr;
  UINTN                       ChangeLength;
  UINTN                       Index;
  UINT32                      Attributes;
  UINTN                       NewVariableSize;
  UINTN                       TotalSmbiosBufferSize;


  Buffer       = NULL;
  CurrentPtr   = NULL;
  ChangePtr    = NULL;
  ChangeLength = 0;

  //
  // This value is based on SmbiosDxe driver. If the buffer allocation method is changed, this size will be different.
  //
  TotalSmbiosBufferSize = EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (mPrivateData->SmbiosTableEntryPoint->TableLength));
  VarSize = PcdGet32 (PcdFlashNvStorageDmiSize);
  DmiEnd = sizeof (DMI_UPDATA_STRING_SIGNATURE);

  Status = EfiGetVariable (
             L"PnpRuntime",
             &gEfiGenericVariableGuid,
             &Attributes,
             &VarSize,
             (VOID *)mPrivateData->DmiVariableBuf
             );

  if (!EFI_ERROR (Status)) {
    //
    // check the signature of buffer
    //
    if (*(UINT32 *)mPrivateData->DmiVariableBuf != DMI_UPDATA_STRING_SIGNATURE) {
      return EFI_NOT_FOUND;
    }
  } else if (Status == EFI_NOT_FOUND) {
    //
    // the variable doesn't exist, create it
    //
    *(UINT32 *)mPrivateData->DmiVariableBuf = DMI_UPDATA_STRING_SIGNATURE;
    VarSize = PcdGet32 (PcdFlashNvStorageDmiSize);
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    Status = EfiSetVariable (
               L"PnpRuntime",
               &gEfiGenericVariableGuid,
               Attributes,
               VarSize,
               (VOID *)mPrivateData->DmiVariableBuf
               );
  } else {
    return Status;
  }

  //
  // save the SetStructure info to the Variable
  //
  Buffer = mPrivateData->DmiVariableBuf + sizeof (DMI_UPDATA_STRING_SIGNATURE);

  while (DmiEnd < VarSize) {
    CurrentPtr = (DMI_STRING_STRUCTURE *)Buffer;

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
      // update directly, don't set invalid bits
      //
      ChangePtr = CurrentPtr;
    }

    //
    // Try to find next.
    //
    DmiEnd = DmiEnd + CurrentPtr->Length;
    Buffer = Buffer + CurrentPtr->Length;
  }

  if (ChangePtr) {
    //
    // the same structure is already in Variable,
    // copy the rest of Variable data to TempStoreArea
    //
    Buffer = (UINT8 *)((UINTN)ChangePtr + (UINTN)ChangePtr->Length);
    ChangeLength = (UINTN)CurrentPtr - (UINTN)Buffer;
    if (ChangeLength > TotalSmbiosBufferSize) {
      return EFI_BUFFER_TOO_SMALL;
    }
    CopyMem (mPrivateData->TempStoreArea, Buffer, ChangeLength);
    Buffer = (UINT8 *)ChangePtr;
    SetMem (Buffer, (UINTN)CurrentPtr - (UINTN)Buffer, 0xFF);
    CurrentPtr = ChangePtr;
  }

  if (CurrentPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // The size of mPrivateData->DmiVariableBuf is defined as PcdFlashNvStorageDmiSize.
  //
  NewVariableSize = (UINTN)CurrentPtr - (UINTN)mPrivateData->DmiVariableBuf +
                    ChangeLength + sizeof (DMI_STRING_STRUCTURE) + (UINTN)Data->DataLength;
  if (NewVariableSize > PcdGet32 (PcdFlashNvStorageDmiSize)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // update CurrentPtr: start of free space
  //
  CurrentPtr->Type = Structure->Type;
  CurrentPtr->Offset = Data->FieldOffset;
  CurrentPtr->Valid = 0xFF;

  switch (Data->Command) {

  case DMI_STRING_CHANGE:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8) - 1);
    for (Index = 0; (Index + 1) < Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;

  default:
    CurrentPtr->Length = Data->DataLength + (sizeof (DMI_STRING_STRUCTURE) - sizeof (UINT8));
    for (Index = 0; Index < Data->DataLength; Index++) {
      CurrentPtr->String[Index] = Data->StructureData[Index];
    }
    break;
  }


  if (ChangePtr) {
    //
    // the same structure is already in Variable,
    // copy the data in TempStoreArea back to variable
    //
    ChangePtr = (DMI_STRING_STRUCTURE *)((UINTN)CurrentPtr + (UINTN)CurrentPtr->Length);
    CopyMem (ChangePtr, mPrivateData->TempStoreArea, ChangeLength);
  }

  VarSize = PcdGet32 (PcdFlashNvStorageDmiSize);
  Status = EfiSetVariable (
             L"PnpRuntime",
             &gEfiGenericVariableGuid,
             Attributes,
             VarSize,
             (VOID *)mPrivateData->DmiVariableBuf
             );

  return Status;
}


/**
 Helper function for Pnp0x52,
 process command DMI_BYTE_CHANGE, DMI_WORD_CHANGE, and DMI_DWORD_CHANGE

 @param        Data
 @param        Structure

 @retval DMI_READ_ONLY          Can not set the data in the structure

**/
INT16
Pnp0x52ChangeFixedLength (
  RS_FUNC_0x52_DATA_BUFFER               *Data,
  SMBIOS_STRUCTURE                       *Structure
  )
{
  EFI_STATUS                  Status;
  UINT8                       *ByteTmpPtr;
  UINT16                      *WordTmpPtr;
  UINT32                      *DWordTmpPtr;
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

  if (mPrivateData->UpdatableStringCount != 0) {
    //
    // check if the data in the structure can be changed
    //
    for (Index = 0; Index < mPrivateData->UpdatableStringCount; Index++) {
      if ((Structure->Type == mPrivateData->UpdatableStrings[Index].Type)
          &&(Data->FieldOffset == mPrivateData->UpdatableStrings[Index].FixedOffset)) {
        break;
      }
    }

    if (Index == mPrivateData->UpdatableStringCount) {
      return DMI_READ_ONLY;
    }
  }

  switch (Data->Command) {

  case DMI_BYTE_CHANGE:
    ByteTmpPtr = (UINT8 *) ((UINTN)Structure + Data->FieldOffset);
    ByteTemp = *ByteTmpPtr;
    ByteTemp = ByteTemp & ((UINT8)(Data->ChangeMask));
    ByteTemp = ByteTemp | ((UINT8)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT8);
    CopyMem((VOID *)&Data->StructureData, (VOID *)&ByteTemp, Data->DataLength);
    break;

  case DMI_WORD_CHANGE:
    WordTmpPtr = (UINT16 *) ((UINTN)Structure + Data->FieldOffset);
    WordTemp = *WordTmpPtr;
    WordTemp = WordTemp & ((UINT16)(Data->ChangeMask));
    WordTemp = WordTemp | ((UINT16)((Data->ChangeValue) & ~(Data->ChangeMask)));

    Data->DataLength = sizeof (UINT16);
    CopyMem((VOID *)&Data->StructureData, (VOID *)&WordTemp, Data->DataLength);
    break;

  case DMI_DWORD_CHANGE:
    DWordTmpPtr = (UINT32 *) ((UINTN)Structure + Data->FieldOffset);
    DWordTemp = *DWordTmpPtr;
    DWordTemp = DWordTemp & ((UINT32)(Data->ChangeMask));
    DWordTemp = DWordTemp | ((UINT32)((Data->ChangeValue) & ~(Data->ChangeMask)));
    Data->DataLength = sizeof (UINT32);
    CopyMem((VOID *)&Data->StructureData, (VOID *)&DWordTemp, Data->DataLength);
    break;
  }

  //
  // store the SetStructure info to the variable
  //
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
    CopyMem (DWordTmpPtr, &DWordTemp, sizeof(UINT32));
    break;
  }

  return DMI_SUCCESS;
}


/**
 Helper function for Pnp0x52, process command DMI_STRING_CHANGE

 @param        Data
 @param        Structure

 @retval DMI_BAD_PARAMETER      The length of data is invalid
                                or can not locate the string in the structure
 @retval DMI_READ_ONLY          Can not set the data in the structure

**/
INT16
Pnp0x52ChangeString (
  RS_FUNC_0x52_DATA_BUFFER              *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
{
  CHAR8                                 *StructureString;
  CHAR8                                 *TempstructureString;
  UINTN                                 StructureStringSize;
  UINTN                                 TempstructureStringSize;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  UINTN                                 CurrentTableSize;
  UINTN                                 TotalSmbiosBufferSize;
  UINTN                                 NewTableSize;


  TempstructureString = NULL;
  TempstructureStringSize = 0;

  if (Data->DataLength > 0xff || Data->DataLength <= 1) {
    return DMI_BAD_PARAMETER;
  }

  //
  // Find string within the SMBIOS structure
  //
  if (Structure->Type == 0x0b || Structure->Type == 0x0c) {
    if (EFI_ERROR (LocateStringOfStructure (
                     (UINT8)Data->FieldOffset,
                     Structure,
                     &StructureString,
                     &StructureStringSize
                     ))) {
      return DMI_BAD_PARAMETER;
    }
    Data->FieldOffset += 4;
  } else {
    if (EFI_ERROR (LocateStringOfStructure (
                     ((UINT8*)Structure)[Data->FieldOffset],
                     Structure,
                     &StructureString,
                     &StructureStringSize
                     ))) {
      return DMI_BAD_PARAMETER;
    }
  }

  if (mPrivateData->UpdatableStringCount != 0) {
    //
    // check if the data in the structure can be changed
    //
    for (Index = 0; Index < mPrivateData->UpdatableStringCount; Index++) {
      if ((Structure->Type == mPrivateData->UpdatableStrings[Index].Type) &&
          (Data->FieldOffset == mPrivateData->UpdatableStrings[Index].FixedOffset)) {
        break;
      }
    }

    if (Index == mPrivateData->UpdatableStringCount) {
      return DMI_READ_ONLY;
    }
  }

  //
  // store the SetStructure info to the variable
  //
  Status = SetDMI (Data, Structure);
  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }

  //
  // This value is based on SmbiosDxe driver. If the buffer allocation method is changed, this size will be different.
  //
  TotalSmbiosBufferSize = EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (mPrivateData->SmbiosTableEntryPoint->TableLength));
  TempstructureString = StructureString + StructureStringSize + 1;
  if (TotalIncreasedStringLength >= 0x00) {
    CurrentTableSize = (UINTN)(mPrivateData->SmbiosTableEntryPoint->TableLength + (UINT16)TotalIncreasedStringLength);
    TempstructureStringSize = (UINTN)(mPrivateData->SmbiosTableEntryPoint->TableLength + (UINT16)TotalIncreasedStringLength) - (UINTN)(TempstructureString - (UINTN)mPrivateData->SmbiosTable);
  } else {
    CurrentTableSize = (UINTN)(mPrivateData->SmbiosTableEntryPoint->TableLength - (UINT16)(~(TotalIncreasedStringLength - 1)));
    TempstructureStringSize = (UINTN)(mPrivateData->SmbiosTableEntryPoint->TableLength - (UINT16)(~(TotalIncreasedStringLength - 1))) - (UINTN)(TempstructureString - (UINTN)mPrivateData->SmbiosTable);
  }

  //
  // If new SMBIOS data is over than the buffer, nothing will change on SMBIOS data,
  // but new string will be written to the Dmi region.
  // Data->DataLength : new string length, including '0'
  // structureStringSize : original string length
  //
  if ((UINTN)Data->DataLength > (StructureStringSize + 1)) {
    NewTableSize = CurrentTableSize + (UINTN)Data->DataLength - (StructureStringSize + 1);
    if (NewTableSize > TotalSmbiosBufferSize) {
      return DMI_NO_CHANGE;
    }
  }

  CopyMem (mPrivateData->TempStoreArea, TempstructureString, TempstructureStringSize);
  CopyMem (StructureString, Data->StructureData, Data->DataLength);
  TempstructureString = StructureString + Data->DataLength;
  CopyMem (TempstructureString, mPrivateData->TempStoreArea, TempstructureStringSize);
  TotalIncreasedStringLength = TotalIncreasedStringLength + (UINT32)Data->DataLength - (UINT32)(StructureStringSize + 1);

  return DMI_SUCCESS;
}

/**
 Helper function for Pnp0x52, process command DMI_BLOCK_CHANGE

 @param        Data
 @param        Structure

 @retval DMI_READ_ONLY          The structure is not Type1,
                                the block to be set is not at offset 8

**/
INT16
Pnp0x52ChangeBlock (
  RS_FUNC_0x52_DATA_BUFFER              *Data,
  SMBIOS_STRUCTURE                      *Structure
  )
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

  //
  // store the SetStructure info to the variable
  //
  Status = SetDMI (Data, Structure);

  if (EFI_ERROR (Status)) {
    return DMI_READ_ONLY;
  }
  CopyMem (TmpPtr, Data->StructureData, Data->DataLength);
  return DMI_SUCCESS;
}

/**
 PnP SMBIOS function 0x52, Set SMBIOS Structure

 @param [in]   CommBuf

 @retval EFI_SUCCESS            The DMI status is in ReturnCode of GenericEntryFrame

**/
EFI_STATUS
Pnp0x52 (
  IN  VOID        *CommBuf
  )
{
  RS_FUNC_0x52_DATA_BUFFER              *Data;
  SMBIOS_STRUCTURE                      *StructureLocation;
  UINT16                                StructureHandle;
  UINTN                                 StructureSize;
  INT16                                 ReturnStatus;

  RS_PNP_FUNCTION_0x52_FRAME            *Frame;
  UINT8                                 *PnpFrame;
  EFI_STATUS                            Status;

  PnpFrame = (UINT8 *)CommBuf;
  PnpFrame += sizeof (IRSI_HEADER);
  Frame= (RS_PNP_FUNCTION_0x52_FRAME *)PnpFrame;

  Data = (RS_FUNC_0x52_DATA_BUFFER *)(UINTN)(Frame->DmiDataBufferAddress);
  StructureHandle = Data->StructureHeader.Handle;
  StructureLocation = NULL;

  Status = LocateSmbiosStructure (&StructureHandle, &StructureLocation, &StructureSize);
  if (!EFI_ERROR (Status)) {

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

  } else {
    ReturnStatus = DMI_BAD_PARAMETER;
  }

  Frame->GenericEntryFrame.ReturnCode = ReturnStatus;

  return EFI_SUCCESS;
}
