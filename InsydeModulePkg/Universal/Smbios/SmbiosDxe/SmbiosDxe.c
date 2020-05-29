/** @file
  SMBIOS driver.

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

#include "SmbiosDxe.h"

EFI_SMBIOS_HANDLE   *mMappingTable = NULL;
UINT16              mNumberOfStructure;

//
// Module Global:
// Since this driver will only ever produce one instance of the
// protocol you are not required to dynamically allocate the PrivateData.
//
SMBIOS_INSTANCE mPrivateData;

UINTN mPreAllocatedPages = 0;

BOOLEAN AlreadyExist[256] = {FALSE};
BOOLEAN EndOfSmbiosScm    = FALSE;
//
// Chassis for SMBIOS entry point structure that is to be installed into EFI system config table.
//
SMBIOS_TABLE_ENTRY_POINT *EntryPointStructure    = NULL;
SMBIOS_TABLE_ENTRY_POINT EntryPointStructureData = {
  //
  // AnchorString
  //
  {
    0x5f,
    0x53,
    0x4d,
    0x5f
  },
  //
  // EntryPointStructureChecksum,TO BE FILLED
  //
  0,
  //
  // EntryPointStructure Length
  //
  0x1f,
  //
  // MajorVersion
  //
  (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) >> 8),
  //
  // MinorVersion
  //
  (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) & 0x00ff),
  //
  // MaxStructureSize, TO BE FILLED
  //
  0,
  //
  // EntryPointRevision
  //
  0,
  //
  // FormattedArea
  //
  {
    0,
    0,
    0,
    0,
    0
  },
  //
  // IntermediateAnchorString
  //
  {
    0x5f,
    0x44,
    0x4d,
    0x49,
    0x5f
  },
  //
  // IntermediateChecksum, TO BE FILLED
  //
  0,
  //
  // StructureTableLength, TO BE FILLED
  //
  0,
  //
  // StructureTableAddress, TO BE FILLED
  //
  0,
  //
  // NumberOfSmbiosStructures, TO BE FILLED
  //
  0,
  //
  // SmbiosBcdRevision
  //
  (UINT8) ((FixedPcdGet16 (PcdSmbiosVersion) >> 4) & 0xf0)
  | (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) & 0x0f)
};

VOID
RecordSmbios(
  VOID
  );

/**

  Get the full size of SMBIOS structure including optional strings that follow the formatted structure.

  @param This                   The EFI_SMBIOS_PROTOCOL instance.
  @param Head                   Pointer to the beginning of SMBIOS structure.
  @param Size                   The returned size.
  @param NumberOfStrings        The returned number of optional strings that follow the formatted structure.

  @retval EFI_SUCCESS           Size returned  in Size.
  @retval EFI_INVALID_PARAMETER Input SMBIOS structure mal-formed or Size is NULL.

**/
EFI_STATUS
EFIAPI
GetSmbiosStructureSize (
  IN   CONST EFI_SMBIOS_PROTOCOL        *This,
  IN   EFI_SMBIOS_TABLE_HEADER          *Head,
  OUT  UINTN                            *Size,
  OUT  UINTN                            *NumberOfStrings
  )
{
  UINTN  FullSize;
  UINTN  StrLen;
  UINTN  MaxLen;
  INT8*  CharInStr;

  if (Size == NULL || NumberOfStrings == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FullSize = Head->Length;
  CharInStr = (INT8*)Head + Head->Length;
  *Size = FullSize;
  *NumberOfStrings = 0;
  StrLen = 0;
  //
  // look for the two consecutive zeros, check the string limit by the way.
  //
  while (*CharInStr != 0 || *(CharInStr+1) != 0) {
    if (*CharInStr == 0) {
      *Size += 1;
      CharInStr++;
    }

    if (This->MajorVersion < 2 || (This->MajorVersion == 2 && This->MinorVersion < 7)){
      MaxLen = SMBIOS_STRING_MAX_LENGTH;
    } else {
      //
      // Reference SMBIOS 2.7, chapter 6.1.3, it will have no limit on the length of each individual text string.
      // However, the length of the entire structure table (including all strings) must be reported
      // in the Structure Table Length field of the SMBIOS Structure Table Entry Point,
      // which is a WORD field limited to 65,535 bytes.
      //
      MaxLen = SMBIOS_TABLE_MAX_LENGTH;
    }

    for (StrLen = 0 ; StrLen < MaxLen; StrLen++) {
      if (*(CharInStr+StrLen) == 0) {
        break;
      }
    }

    if (StrLen == MaxLen) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // forward the pointer
    //
    CharInStr += StrLen;
    *Size += StrLen;
    *NumberOfStrings += 1;
  }

  //
  // count ending two zeros.
  //
  *Size += 2;
  return EFI_SUCCESS;
}

/**

  Determine whether an SmbiosHandle has already in use.

  @param Head        Pointer to the beginning of SMBIOS structure.
  @param Handle      A unique handle will be assigned to the SMBIOS record.

  @retval TRUE       SMBIOS handle already in use.
  @retval FALSE      SMBIOS handle is NOT used.

**/
BOOLEAN
EFIAPI
CheckSmbiosHandleExistance (
  IN  LIST_ENTRY           *Head,
  IN  EFI_SMBIOS_HANDLE    Handle
  )
{
  LIST_ENTRY              *Link;
  SMBIOS_HANDLE_ENTRY     *HandleEntry;

  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    HandleEntry = SMBIOS_HANDLE_ENTRY_FROM_LINK(Link);
    if (HandleEntry->SmbiosHandle == Handle) {
      return TRUE;
    }
  }

  return FALSE;
}

/**

  Get the max SmbiosHandle that could be use.

  @param  This           The EFI_SMBIOS_PROTOCOL instance.
  @param  MaxHandle      The max handle that could be assigned to the SMBIOS record.

**/
VOID
EFIAPI
GetMaxSmbiosHandle (
  IN CONST  EFI_SMBIOS_PROTOCOL   *This,
  IN OUT    EFI_SMBIOS_HANDLE     *MaxHandle
  )
{
  if (This->MajorVersion == 2 && This->MinorVersion == 0) {
    *MaxHandle = 0xFFFE;
  } else {
    *MaxHandle = 0xFEFF;
  }
}

/**

  Get an SmbiosHandle that could use.

  @param  This                   The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle           A unique handle will be assigned to the SMBIOS record.

  @retval EFI_SUCCESS            SMBIOS handle got.
  @retval EFI_OUT_OF_RESOURCES   SMBIOS handle is NOT available.

**/
EFI_STATUS
EFIAPI
GetAvailableSmbiosHandle (
  IN CONST EFI_SMBIOS_PROTOCOL   *This,
  IN OUT   EFI_SMBIOS_HANDLE     *Handle
  )
{
  LIST_ENTRY              *Head;
  SMBIOS_INSTANCE         *Private;
  EFI_SMBIOS_HANDLE       MaxSmbiosHandle;
  EFI_SMBIOS_HANDLE       AvailableHandle;

  GetMaxSmbiosHandle(This, &MaxSmbiosHandle);

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  Head = &Private->AllocatedHandleListHead;
  for (AvailableHandle = 0; AvailableHandle < MaxSmbiosHandle; AvailableHandle++) {
    if (!CheckSmbiosHandleExistance(Head, AvailableHandle)) {
      *Handle = AvailableHandle;
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

/**
  Add an SMBIOS record in a sorted list.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  ProducerHandle        The handle of the controller or driver associated with the SMBIOS information. NULL
                                means no handle.
  @param  SmbiosHandle          On entry, the handle of the SMBIOS record to add. If FFFEh, then a unique handle
                                will be assigned to the SMBIOS record. If the SMBIOS handle is already in use,
                                EFI_ALREADY_STARTED is returned and the SMBIOS record is not updated.
  @param  Record                The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or
                                a set of null terminated strings and a null.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.
  @retval EFI_ALREADY_STARTED   The SmbiosHandle passed in was already in use.

**/
VOID
SmbiosInsertSortedList (
  IN OUT  LIST_ENTRY                *ListHead,
  IN OUT  LIST_ENTRY                *Entry,
  IN EFI_SMBIOS_TYPE                Type
  )
{
  LIST_ENTRY              *TempList;
  EFI_SMBIOS_ENTRY        *TempEntry;
  EFI_SMBIOS_TABLE_HEADER *CurrentSmbiosHeader, *NextSmbiosHeadr;

  if (IsListEmpty (ListHead)) {
    InsertHeadList (ListHead, Entry);
    return;
  }

  TempList = ListHead;
  for (TempList = ListHead->ForwardLink; TempList != ListHead; TempList = TempList->ForwardLink) {
    TempEntry = SMBIOS_ENTRY_FROM_LINK (TempList);
    CurrentSmbiosHeader = (EFI_SMBIOS_TABLE_HEADER*)(TempEntry->RecordHeader + 1);
    if (Type < CurrentSmbiosHeader->Type) {
      InsertTailList (TempList, Entry);
      return;
    }

    if (IsNodeAtEnd (ListHead, TempList)) {
      InsertHeadList (TempList, Entry);
      return;
    }

    TempEntry = SMBIOS_ENTRY_FROM_LINK (TempList->ForwardLink);
    NextSmbiosHeadr = (EFI_SMBIOS_TABLE_HEADER*)(TempEntry->RecordHeader + 1);
    if (Type < NextSmbiosHeadr->Type) {
      InsertHeadList (TempList, Entry);
      return;
    }
  }
}


/**
  Add an SMBIOS record.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  ProducerHandle        The handle of the controller or driver associated with the SMBIOS information. NULL
                                means no handle.
  @param  SmbiosHandle          On entry, the handle of the SMBIOS record to add. If FFFEh, then a unique handle
                                will be assigned to the SMBIOS record. If the SMBIOS handle is already in use,
                                EFI_ALREADY_STARTED is returned and the SMBIOS record is not updated.
  @param  Record                The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or
                                a set of null terminated strings and a null.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.
  @retval EFI_ALREADY_STARTED   The SmbiosHandle passed in was already in use.

**/
EFI_STATUS
EFIAPI
SmbiosAdd (
  IN CONST EFI_SMBIOS_PROTOCOL  *This,
  IN EFI_HANDLE                 ProducerHandle, OPTIONAL
  IN OUT EFI_SMBIOS_HANDLE      *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  )
{
  VOID                        *Raw;
  UINTN                       TotalSize;
  UINTN                       RecordSize;
  UINTN                       StructureSize;
  UINTN                       NumberOfStrings;
  EFI_STATUS                  Status;
  LIST_ENTRY                  *Head;
  SMBIOS_INSTANCE             *Private;
  EFI_SMBIOS_ENTRY            *SmbiosEntry;
  EFI_SMBIOS_HANDLE           MaxSmbiosHandle;
  SMBIOS_HANDLE_ENTRY         *HandleEntry;
  EFI_SMBIOS_RECORD_HEADER    *InternalRecord;


  if (SmbiosHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  //
  // Check whether SmbiosHandle is already in use
  //
  Head = &Private->AllocatedHandleListHead;
  if (*SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED && CheckSmbiosHandleExistance(Head, *SmbiosHandle)) {
    return EFI_ALREADY_STARTED;
  }

DEBUG_CODE_BEGIN();
  if (EndOfSmbiosScm && AlreadyExist[Record->Type]) {
    DEBUG((EFI_D_INFO, "The SMBIOS type %d already exist when install SMBIOS protocol.\n", Record->Type));
  }
DEBUG_CODE_END();

  //
  // when SmbiosHandle is 0xFFFE, an available handle will be assigned
  //
  if (*SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
    Status = GetAvailableSmbiosHandle(This, SmbiosHandle);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  } else {
    //
    // Check this handle validity
    //
    GetMaxSmbiosHandle(This, &MaxSmbiosHandle);
    if (*SmbiosHandle > MaxSmbiosHandle) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate record size and string number
  //
  Status = GetSmbiosStructureSize(This, Record, &StructureSize, &NumberOfStrings);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (EntryPointStructure->TableLength + StructureSize > SMBIOS_TABLE_MAX_LENGTH) {
    //
    // The length of the entire structure table (including all strings) must be reported
    // in the Structure Table Length field of the SMBIOS Structure Table Entry Point,
    // which is a WORD field limited to 65,535 bytes.
    //
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Enter into critical section
  //
  Status = EfiAcquireLockOrFail (&Private->DataLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RecordSize  = sizeof (EFI_SMBIOS_RECORD_HEADER) + StructureSize;
  TotalSize   = sizeof (EFI_SMBIOS_ENTRY) + RecordSize;

  //
  // Allocate internal buffer
  //
  SmbiosEntry = AllocateZeroPool (TotalSize);
  if (SmbiosEntry == NULL) {
    EfiReleaseLock (&Private->DataLock);
    return EFI_OUT_OF_RESOURCES;
  }
  HandleEntry = AllocateZeroPool (sizeof(SMBIOS_HANDLE_ENTRY));
  if (HandleEntry == NULL) {
    EfiReleaseLock (&Private->DataLock);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Build Handle Entry and insert into linked list
  //
  HandleEntry->Signature     = SMBIOS_HANDLE_ENTRY_SIGNATURE;
  HandleEntry->SmbiosHandle  = *SmbiosHandle;
  InsertTailList(&Private->AllocatedHandleListHead, &HandleEntry->Link);
  InternalRecord  = (EFI_SMBIOS_RECORD_HEADER *) (SmbiosEntry + 1);
  Raw     = (VOID *) (InternalRecord + 1);

  //
  // Build internal record Header
  //
  InternalRecord->Version     = EFI_SMBIOS_RECORD_HEADER_VERSION;
  InternalRecord->HeaderSize  = (UINT16) sizeof (EFI_SMBIOS_RECORD_HEADER);
  InternalRecord->RecordSize  = RecordSize;
  InternalRecord->ProducerHandle = ProducerHandle;
  InternalRecord->NumberOfStrings = NumberOfStrings;
  //
  // Insert record into the internal linked list
  //
  SmbiosEntry->Signature    = EFI_SMBIOS_ENTRY_SIGNATURE;
  SmbiosEntry->RecordHeader = InternalRecord;
  SmbiosEntry->RecordSize   = TotalSize;
  SmbiosInsertSortedList (&Private->DataListHead, &SmbiosEntry->Link, Record->Type);
  CopyMem (Raw, Record, StructureSize);
  ((EFI_SMBIOS_TABLE_HEADER*)Raw)->Handle = *SmbiosHandle;

  //
  // Some UEFI drivers (such as network) need some information in SMBIOS table.
  // Here we create SMBIOS table and publish it in
  // configuration table, so other UEFI drivers can get SMBIOS table from
  // configuration table without depending on PI SMBIOS protocol.
  //
  SmbiosTableConstruction ();

  //
  // Leave critical section
  //
  EfiReleaseLock (&Private->DataLock);
  return EFI_SUCCESS;
}

/**
  Update the string associated with an existing SMBIOS record.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          SMBIOS Handle of structure that will have its string updated.
  @param  StringNumber          The non-zero string number of the string to update
  @param  String                Update the StringNumber string with String.

  @retval EFI_SUCCESS           SmbiosHandle had its StringNumber String updated.
  @retval EFI_INVALID_PARAMETER SmbiosHandle does not exist.
  @retval EFI_UNSUPPORTED       String was not added because it is longer than the SMBIOS Table supports.
  @retval EFI_NOT_FOUND         The StringNumber.is not valid for this SMBIOS record.

**/
EFI_STATUS
EFIAPI
SmbiosUpdateString (
  IN CONST EFI_SMBIOS_PROTOCOL      *This,
  IN EFI_SMBIOS_HANDLE              *SmbiosHandle,
  IN UINTN                          *StringNumber,
  IN CHAR8                          *String
  )
{
  UINTN                     InputStrLen;
  UINTN                     TargetStrLen;
  UINTN                     StrIndex;
  UINTN                     TargetStrOffset;
  UINTN                     NewEntrySize;
  CHAR8                     *StrStart;
  VOID                      *Raw;
  LIST_ENTRY                *Link;
  LIST_ENTRY                *Head;
  EFI_STATUS                Status;
  SMBIOS_INSTANCE           *Private;
  EFI_SMBIOS_ENTRY          *SmbiosEntry;
  EFI_SMBIOS_ENTRY          *ResizedSmbiosEntry;
  EFI_SMBIOS_HANDLE         MaxSmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER   *Record;
  EFI_SMBIOS_RECORD_HEADER  *InternalRecord;

  //
  // Check args validity
  //
  GetMaxSmbiosHandle(This, &MaxSmbiosHandle);

  if (*SmbiosHandle > MaxSmbiosHandle) {
    return EFI_INVALID_PARAMETER;
  }

  if (String == NULL) {
    return EFI_ABORTED;
  }

  if (*StringNumber == 0) {
    return EFI_NOT_FOUND;
  }

  InputStrLen = AsciiStrLen(String);

  if (This->MajorVersion < 2 || (This->MajorVersion == 2 && This->MinorVersion < 7)) {
    if (InputStrLen > SMBIOS_STRING_MAX_LENGTH) {
      return EFI_UNSUPPORTED;
    }
  } else {
    //
    // Reference SMBIOS 2.7, chapter 6.1.3, it will have no limit on the length of each individual text string.
    // However, the length of the entire structure table (including all strings) must be reported
    // in the Structure Table Length field of the SMBIOS Structure Table Entry Point,
    // which is a WORD field limited to 65,535 bytes.
    //
    if (InputStrLen > SMBIOS_TABLE_MAX_LENGTH) {
      return EFI_UNSUPPORTED;
    }
  }

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  //
  // Enter into critical section
  //
  Status = EfiAcquireLockOrFail (&Private->DataLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Head = &Private->DataListHead;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
    Record = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);

    if (Record->Handle == *SmbiosHandle) {
      //
      // Find out the specified SMBIOS record
      //
      if (*StringNumber > SmbiosEntry->RecordHeader->NumberOfStrings) {
        EfiReleaseLock (&Private->DataLock);
        return EFI_NOT_FOUND;
      }
      //
      // Point to unformed string section
      //
      StrStart = (CHAR8 *) Record + Record->Length;

      for (StrIndex = 1, TargetStrOffset = 0; StrIndex < *StringNumber; StrStart++, TargetStrOffset++) {
        //
        // A string ends in 00h
        //
        if (*StrStart == 0) {
          StrIndex++;
        }

        //
        // String section ends in double-null (0000h)
        //
        if (*StrStart == 0 && *(StrStart + 1) == 0) {
          EfiReleaseLock (&Private->DataLock);
          return EFI_NOT_FOUND;
        }
      }

      if (*StrStart == 0) {
        StrStart++;
        TargetStrOffset++;
      }

      //
      // Now we get the string target
      //
      TargetStrLen = AsciiStrLen(StrStart);
      if (InputStrLen == TargetStrLen) {
        AsciiStrCpy(StrStart, String);
        //
        // Some UEFI drivers (such as network) need some information in SMBIOS table.
        // Here we create SMBIOS table and publish it in
        // configuration table, so other UEFI drivers can get SMBIOS table from
        // configuration table without depending on PI SMBIOS protocol.
        //
        SmbiosTableConstruction ();
        EfiReleaseLock (&Private->DataLock);
        return EFI_SUCCESS;
      }

      if (EntryPointStructure->TableLength + InputStrLen - TargetStrLen > SMBIOS_TABLE_MAX_LENGTH) {
        //
        // The length of the entire structure table (including all strings) must be reported
        // in the Structure Table Length field of the SMBIOS Structure Table Entry Point,
        // which is a WORD field limited to 65,535 bytes.
        //
        return EFI_UNSUPPORTED;
      }

      //
      // Original string buffer size is not exactly match input string length.
      // Re-allocate buffer is needed.
      //
      NewEntrySize = SmbiosEntry->RecordSize + InputStrLen - TargetStrLen;
      ResizedSmbiosEntry = AllocateZeroPool (NewEntrySize);

      if (ResizedSmbiosEntry == NULL) {
        EfiReleaseLock (&Private->DataLock);
        return EFI_OUT_OF_RESOURCES;
      }

      InternalRecord  = (EFI_SMBIOS_RECORD_HEADER *) (ResizedSmbiosEntry + 1);
      Raw     = (VOID *) (InternalRecord + 1);

      //
      // Build internal record Header
      //
      InternalRecord->Version     = EFI_SMBIOS_RECORD_HEADER_VERSION;
      InternalRecord->HeaderSize  = (UINT16) sizeof (EFI_SMBIOS_RECORD_HEADER);
      InternalRecord->RecordSize  = SmbiosEntry->RecordHeader->RecordSize + InputStrLen - TargetStrLen;
      InternalRecord->ProducerHandle = SmbiosEntry->RecordHeader->ProducerHandle;
      InternalRecord->NumberOfStrings = SmbiosEntry->RecordHeader->NumberOfStrings;

      //
      // Copy SMBIOS structure and optional strings.
      //
      CopyMem (Raw, SmbiosEntry->RecordHeader + 1, Record->Length + TargetStrOffset);
      CopyMem ((VOID*)((UINTN)Raw + Record->Length + TargetStrOffset), String, InputStrLen + 1);
      CopyMem ((CHAR8*)((UINTN)Raw + Record->Length + TargetStrOffset + InputStrLen + 1),
               (CHAR8*)Record + Record->Length + TargetStrOffset + TargetStrLen + 1,
               SmbiosEntry->RecordHeader->RecordSize - sizeof (EFI_SMBIOS_RECORD_HEADER) - Record->Length - TargetStrOffset - TargetStrLen - 1);

      //
      // Insert new record
      //
      ResizedSmbiosEntry->Signature    = EFI_SMBIOS_ENTRY_SIGNATURE;
      ResizedSmbiosEntry->RecordHeader = InternalRecord;
      ResizedSmbiosEntry->RecordSize   = NewEntrySize;
      InsertTailList (Link->ForwardLink, &ResizedSmbiosEntry->Link);

      //
      // Remove old record
      //
      RemoveEntryList(Link);
      FreePool(SmbiosEntry);
      //
      // Some UEFI drivers (such as network) need some information in SMBIOS table.
      // Here we create SMBIOS table and publish it in
      // configuration table, so other UEFI drivers can get SMBIOS table from
      // configuration table without depending on PI SMBIOS protocol.
      //
      SmbiosTableConstruction ();
      EfiReleaseLock (&Private->DataLock);
      return EFI_SUCCESS;
    }
  }

  EfiReleaseLock (&Private->DataLock);
  return EFI_INVALID_PARAMETER;
}

/**
  Remove an SMBIOS record.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          The handle of the SMBIOS record to remove.

  @retval EFI_SUCCESS           SMBIOS record was removed.
  @retval EFI_INVALID_PARAMETER SmbiosHandle does not specify a valid SMBIOS record.

**/
EFI_STATUS
EFIAPI
SmbiosRemove (
  IN CONST EFI_SMBIOS_PROTOCOL   *This,
  IN EFI_SMBIOS_HANDLE           SmbiosHandle
  )
{
  LIST_ENTRY                 *Link;
  LIST_ENTRY                 *Head;
  EFI_STATUS                 Status;
  EFI_SMBIOS_HANDLE          MaxSmbiosHandle;
  SMBIOS_INSTANCE            *Private;
  EFI_SMBIOS_ENTRY           *SmbiosEntry;
  SMBIOS_HANDLE_ENTRY        *HandleEntry;
  EFI_SMBIOS_TABLE_HEADER    *Record;

  //
  // Check args validity
  //
  GetMaxSmbiosHandle(This, &MaxSmbiosHandle);

  if (SmbiosHandle > MaxSmbiosHandle) {
    return EFI_INVALID_PARAMETER;
  }

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  //
  // Enter into critical section
  //
  Status = EfiAcquireLockOrFail (&Private->DataLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Head = &Private->DataListHead;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
    Record = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);
    if (Record->Handle == SmbiosHandle) {
      //
      // Remove specified SMBIOS record from DataList
      //
      RemoveEntryList(Link);
      FreePool(SmbiosEntry);
      //
      // Remove this handle from AllocatedHandleList
      //
      Head = &Private->AllocatedHandleListHead;
      for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
        HandleEntry = SMBIOS_HANDLE_ENTRY_FROM_LINK(Link);
        if (HandleEntry->SmbiosHandle == SmbiosHandle) {
          RemoveEntryList(Link);
          FreePool(HandleEntry);
          break;
        }
      }
      //
      // Some UEFI drivers (such as network) need some information in SMBIOS table.
      // Here we create SMBIOS table and publish it in
      // configuration table, so other UEFI drivers can get SMBIOS table from
      // configuration table without depending on PI SMBIOS protocol.
      //
      SmbiosTableConstruction ();
      EfiReleaseLock (&Private->DataLock);
      return EFI_SUCCESS;
    }
  }

  //
  // Leave critical section
  //
  EfiReleaseLock (&Private->DataLock);
  return EFI_INVALID_PARAMETER;

}

/**
  Allow the caller to discover all or some of the SMBIOS records.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          On entry, points to the previous handle of the SMBIOS record. On exit, points to the
                                next SMBIOS record handle. If it is FFFEh on entry, then the first SMBIOS record
                                handle will be returned. If it returns FFFEh on exit, then there are no more SMBIOS records.
  @param  Type                  On entry it means return the next SMBIOS record of type Type. If a NULL is passed in
                                this functionally it ignored. Type is not modified by the GetNext() function.
  @param  Record                On exit, points to the SMBIOS Record consisting of the formatted area followed by
                                the unformatted area. The unformatted area optionally contains text strings.
  @param  ProducerHandle        On exit, points to the ProducerHandle registered by Add(). If no ProducerHandle was passed into Add() NULL is returned.
                                If a NULL pointer is passed in no data will be returned

  @retval EFI_SUCCESS           SMBIOS record information was successfully returned in Record.
  @retval EFI_NOT_FOUND         The SMBIOS record with SmbiosHandle was the last available record.

**/
EFI_STATUS
EFIAPI
SmbiosGetNext (
  IN CONST EFI_SMBIOS_PROTOCOL      *This,
  IN OUT EFI_SMBIOS_HANDLE          *SmbiosHandle,
  IN EFI_SMBIOS_TYPE                *Type,          OPTIONAL
  OUT EFI_SMBIOS_TABLE_HEADER       **Record,
  OUT EFI_HANDLE                    *ProducerHandle OPTIONAL
  )
{
  BOOLEAN                  StartPointFound;
  LIST_ENTRY               *Link;
  LIST_ENTRY               *Head;
  SMBIOS_INSTANCE          *Private;
  EFI_SMBIOS_ENTRY         *SmbiosEntry;
  EFI_SMBIOS_TABLE_HEADER  *SmbiosTableHeader;

  if (SmbiosHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StartPointFound = FALSE;
  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  Head = &Private->DataListHead;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
    SmbiosTableHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);

    //
    // If SmbiosHandle is 0xFFFE, the first matched SMBIOS record handle will be returned
    //
    if (*SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
      if ((Type != NULL) && (*Type != SmbiosTableHeader->Type)) {
        continue;
      }

      *SmbiosHandle = SmbiosTableHeader->Handle;
      *Record =SmbiosTableHeader;
      if (ProducerHandle != NULL) {
        *ProducerHandle = SmbiosEntry->RecordHeader->ProducerHandle;
      }
      return EFI_SUCCESS;
    }

    //
    // Start this round search from the next SMBIOS handle
    //
    if (!StartPointFound && (*SmbiosHandle == SmbiosTableHeader->Handle)) {
      StartPointFound = TRUE;
      continue;
    }

    if (StartPointFound) {
      if ((Type != NULL) && (*Type != SmbiosTableHeader->Type)) {
        continue;
      }

      *SmbiosHandle = SmbiosTableHeader->Handle;
      *Record = SmbiosTableHeader;
      if (ProducerHandle != NULL) {
        *ProducerHandle = SmbiosEntry->RecordHeader->ProducerHandle;
      }

      return EFI_SUCCESS;
    }
  }

  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  return EFI_NOT_FOUND;

}

/**
  Allow the caller to discover all of the SMBIOS records.

  @param  This                  The EFI_SMBIOS_PROTOCOL instance.
  @param  CurrentSmbiosEntry    On exit, points to the SMBIOS entry on the list which includes the returned SMBIOS record information.
                                If *CurrentSmbiosEntry is NULL on entry, then the first SMBIOS entry on the list will be returned.
  @param  Record                On exit, points to the SMBIOS Record consisting of the formatted area followed by
                                the unformatted area. The unformatted area optionally contains text strings.

  @retval EFI_SUCCESS           SMBIOS record information was successfully returned in Record.
                                *CurrentSmbiosEntry points to the SMBIOS entry which includes the returned SMBIOS record information.
  @retval EFI_NOT_FOUND         There is no more SMBIOS entry.

**/
EFI_STATUS
EFIAPI
GetNextSmbiosRecord (
  IN CONST EFI_SMBIOS_PROTOCOL         *This,
  IN OUT EFI_SMBIOS_ENTRY              **CurrentSmbiosEntry,
  OUT EFI_SMBIOS_TABLE_HEADER          **Record
  )
{
  LIST_ENTRY               *Link;
  LIST_ENTRY               *Head;
  SMBIOS_INSTANCE          *Private;
  EFI_SMBIOS_ENTRY         *SmbiosEntry;
  EFI_SMBIOS_TABLE_HEADER  *SmbiosTableHeader;

  Private = SMBIOS_INSTANCE_FROM_THIS (This);
  if (*CurrentSmbiosEntry == NULL) {
    //
    // Get the beginning of SMBIOS entry.
    //
    Head = &Private->DataListHead;
  } else {
    //
    // Get previous SMBIOS entry and make it as start point.
    //
    Head = &(*CurrentSmbiosEntry)->Link;
  }

  Link  = Head->ForwardLink;

  if (Link == &Private->DataListHead) {
    //
    // If no more SMBIOS entry in the list, return not found.
    //
    return EFI_NOT_FOUND;
  }

  SmbiosEntry = SMBIOS_ENTRY_FROM_LINK(Link);
  SmbiosTableHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);
  *Record = SmbiosTableHeader;
  *CurrentSmbiosEntry = SmbiosEntry;
  return EFI_SUCCESS;
}

/**
  Assembles SMBIOS table from the SMBIOS protocol. Produce Table
  Entry Point and return the pointer to it.

  @param  TableEntryPointStructure   On exit, points to the SMBIOS entry point structure.

  @retval EFI_SUCCESS                Structure created successfully.
  @retval EFI_NOT_READY              Some of The SMBIOS records was not available yet.
  @retval EFI_OUT_OF_RESOURCES       No enough memory.

**/
EFI_STATUS
EFIAPI
SmbiosCreateTable (
  OUT VOID    **TableEntryPointStructure
  )
{
  UINT8                           *BufferPointer;
  UINTN                           RecordSize;
  UINTN                           NumOfStr;
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_PROTOCOL             *SmbiosProtocol;
  EFI_PHYSICAL_ADDRESS            PhysicalAddress;
  EFI_SMBIOS_TABLE_HEADER         *SmbiosRecord;
  EFI_SMBIOS_TABLE_END_STRUCTURE  EndStructure;
  EFI_SMBIOS_ENTRY                *CurrentSmbiosEntry;

  Status            = EFI_SUCCESS;
  BufferPointer     = NULL;

  //
  // Get SMBIOS protocol to traverse SMBIOS records.
  //
  SmbiosProtocol = &mPrivateData.Smbios;

  //
  // Make some statistics about all the structures
  //
  EntryPointStructure->NumberOfSmbiosStructures = 0;
  EntryPointStructure->TableLength              = 0;
  EntryPointStructure->MaxStructureSize         = 0;

  //
  // Calculate EPS Table Length
  //
  CurrentSmbiosEntry = NULL;
  do {
    Status = GetNextSmbiosRecord (SmbiosProtocol, &CurrentSmbiosEntry, &SmbiosRecord);

    if (Status == EFI_SUCCESS) {
      GetSmbiosStructureSize(SmbiosProtocol, SmbiosRecord, &RecordSize, &NumOfStr);
      //
      // Record NumberOfSmbiosStructures, TableLength and MaxStructureSize
      //
      EntryPointStructure->NumberOfSmbiosStructures++;
      EntryPointStructure->TableLength = (UINT16) (EntryPointStructure->TableLength + RecordSize);
      if (RecordSize > EntryPointStructure->MaxStructureSize) {
        EntryPointStructure->MaxStructureSize = (UINT16) RecordSize;
      }
    }
  } while (!EFI_ERROR(Status));

  //
  // Create End-Of-Table structure
  //
  GetMaxSmbiosHandle(SmbiosProtocol, &SmbiosHandle);
  EndStructure.Header.Type = EFI_SMBIOS_TYPE_END_OF_TABLE;
  EndStructure.Header.Length = (UINT8) sizeof (EFI_SMBIOS_TABLE_HEADER);
  EndStructure.Header.Handle = SmbiosHandle;
  EndStructure.Tailing[0] = 0;
  EndStructure.Tailing[1] = 0;
  EntryPointStructure->NumberOfSmbiosStructures++;
  EntryPointStructure->TableLength = (UINT16) (EntryPointStructure->TableLength + sizeof (EndStructure));
  if (sizeof (EndStructure) > EntryPointStructure->MaxStructureSize) {
    EntryPointStructure->MaxStructureSize = (UINT16) sizeof (EndStructure);
  }

  if ((UINTN) EFI_SIZE_TO_PAGES (EntryPointStructure->TableLength) > mPreAllocatedPages) {
    //
    // If new SMBIOS talbe size exceeds the original pre-allocated page,
    // it is time to re-allocate memory (below 4GB).
    //
    if (EntryPointStructure->TableAddress != 0) {
      //
      // Free the original pre-allocated page
      //
      FreePages (
            (VOID*)(UINTN)EntryPointStructure->TableAddress,
            mPreAllocatedPages
            );
      EntryPointStructure->TableAddress = 0;
      mPreAllocatedPages = 0;
    }

    PhysicalAddress = 0xffffffff;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiRuntimeServicesData,
                    EFI_SIZE_TO_PAGES (EntryPointStructure->TableLength),
                    &PhysicalAddress
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "SmbiosCreateTable() could not allocate SMBIOS table < 4GB\n"));
      EntryPointStructure->TableAddress = 0;
      return EFI_OUT_OF_RESOURCES;
    } else {
      EntryPointStructure->TableAddress = (UINT32) PhysicalAddress;
      mPreAllocatedPages = EFI_SIZE_TO_PAGES (EntryPointStructure->TableLength);
    }
  }

  //
  // Assemble the tables
  //
  ASSERT (EntryPointStructure->TableAddress != 0);
  BufferPointer = (UINT8 *) (UINTN) EntryPointStructure->TableAddress;
  CurrentSmbiosEntry = NULL;
  do {
    Status = GetNextSmbiosRecord (SmbiosProtocol, &CurrentSmbiosEntry, &SmbiosRecord);

    if (Status == EFI_SUCCESS) {
      GetSmbiosStructureSize(SmbiosProtocol, SmbiosRecord, &RecordSize, &NumOfStr);
      CopyMem (BufferPointer, SmbiosRecord, RecordSize);
      BufferPointer = BufferPointer + RecordSize;
    }
  } while (!EFI_ERROR(Status));

  //
  // Assemble End-Of-Table structure
  //
  CopyMem (BufferPointer, &EndStructure, sizeof (EndStructure));

  //
  // Fixup checksums in the Entry Point Structure
  //
  EntryPointStructure->IntermediateChecksum = 0;
  EntryPointStructure->EntryPointStructureChecksum = 0;

  EntryPointStructure->IntermediateChecksum =
    CalculateCheckSum8 ((UINT8 *) EntryPointStructure + 0x10, EntryPointStructure->EntryPointLength - 0x10);
  EntryPointStructure->EntryPointStructureChecksum =
    CalculateCheckSum8 ((UINT8 *) EntryPointStructure, EntryPointStructure->EntryPointLength);

  //
  // Returns the pointer
  //
  *TableEntryPointStructure = EntryPointStructure;

  return EFI_SUCCESS;
}

/**
  Create SMBIOS Table and install it to the System Table.
**/
VOID
EFIAPI
SmbiosTableConstruction (
  VOID
  )
{
  UINT8       *Eps;
  EFI_STATUS  Status;

  Status = SmbiosCreateTable ((VOID **) &Eps);
  if (!EFI_ERROR (Status)) {
    gBS->InstallConfigurationTable (&gEfiSmbiosTableGuid, Eps);
  }
}

/**
  Search the handle number from the mapping table.
  If the handle number is not found, return 0xFFFF.

  @param  Handle       The old handle number.

  @return              The new handle number.

**/
EFI_SMBIOS_HANDLE
SearchMappingHandle (
  IN EFI_SMBIOS_HANDLE          Handle
  )
{
  EFI_SMBIOS_HANDLE   Index;

  for (Index = 0; Index < mNumberOfStructure; Index++) {
    if (mMappingTable[Index] == Handle) {
      return Index;
    }
  }

  return 0xFFFF;
}

/**
  Fixup the Link Data. Search the correct handle number and update to the record.

  @param  HandlePtr    The pointer to the handle value.

**/
VOID
FixupLinkData (
  IN EFI_SMBIOS_HANDLE          *HandlePtr
  )
{
  EFI_SMBIOS_HANDLE    TempHandle;

  TempHandle = SearchMappingHandle (*HandlePtr);
  CopyMem (HandlePtr, &TempHandle, sizeof (EFI_SMBIOS_HANDLE));
}

/**
  Search the handle number for SMBIOS type.
  If the handle number is not found, return 0xFFFF.

  @param  Head    Link list head for SMBIOS records.
  @param  Type    SMBIOS type number.

**/
EFI_SMBIOS_HANDLE
SearchHandle (
  IN LIST_ENTRY      *Head,
  IN EFI_SMBIOS_TYPE  Type
  )
{
  EFI_SMBIOS_HANDLE           ReturnHandle;
  EFI_SMBIOS_ENTRY           *SmbiosEntry;
  LIST_ENTRY                 *Link;
  EFI_SMBIOS_TABLE_HEADER    *SmbiosHeader;

  ReturnHandle = 0xFFFF;
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry  = SMBIOS_ENTRY_FROM_LINK (Link);
    SmbiosHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);

    if (SmbiosHeader->Type == Type) {
      ReturnHandle = SmbiosHeader->Handle;
      break;
    }
  }

  return ReturnHandle;
}

EFI_STATUS
EFIAPI
SmbiosUpdateValue (
)
{
  SMBIOS_INSTANCE                *Private;
  LIST_ENTRY                     *Head;
  LIST_ENTRY                     *Link;
  EFI_SMBIOS_ENTRY               *SmbiosEntry;
  EFI_SMBIOS_TABLE_HEADER        *CurrentSmbiosHeader;
  SMBIOS_TABLE_TYPE13            *Type13Record;
  CHAR8                          *LanguageTable = NULL;
  CHAR8                          *SCULanguage = NULL;
  CHAR8                          *Type13Lang = NULL;
  UINT8                          LanguageIndex = 0;
  EFI_SMBIOS_HANDLE              Index;

  Private = SMBIOS_INSTANCE_FROM_THIS (&mPrivateData.Smbios);
  Head = &Private->DataListHead;

  //
  // Update SMBIOS value before boot.
  //
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK (Link);
    CurrentSmbiosHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);

    switch (CurrentSmbiosHeader->Type) {

    //
    // Type13 current language setting need to depedant on SCU language setting.
    //
    case EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION:
      //
      //Get SCU language and Type 13 Language Table .
      //
      SCULanguage = GetEfiGlobalVariable (L"PlatformLang");
      if (SCULanguage == NULL) {
        SCULanguage = AllocatePool (AsciiStrSize("en-US"));
        if (SCULanguage == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        AsciiStrCpy (SCULanguage, (CHAR8 *) "en-US");
      }
      LanguageTable = (CHAR8 *)((UINTN)CurrentSmbiosHeader + sizeof(SMBIOS_TABLE_TYPE13));
      Type13Lang = AllocatePool (AsciiStrSize (LanguageTable));
      if (Type13Lang == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      LanguageIndex = 1;
      Index = 0;
      //
      //Change SCU_Language string to upper .
      //
      do {
        if (*(SCULanguage + Index) >= 'a' && *(SCULanguage + Index) <= 'z') {
          *(SCULanguage + Index) = *(SCULanguage + Index) - ('a' - 'A');
        }
        Index ++;
      } while (*(SCULanguage + Index) != '\0');
      //
      //Search the match language.
      //
      do {
        Index = 0;
        do {
          *(Type13Lang + Index) = *LanguageTable;
          if (*(Type13Lang + Index) >= 'a' && *(Type13Lang + Index) <= 'z') {
            *(Type13Lang + Index) = *(Type13Lang + Index) - ('a' - 'A');
          }
          Index ++;
          LanguageTable ++;
        } while (*LanguageTable != 0);
        LanguageTable ++;

        if (AsciiStrnCmp(Type13Lang,SCULanguage,2) == 0) {  //only check the first 2 char
          break;
        }
        LanguageIndex ++;
      } while (*LanguageTable != 0);
      //
      //if no any match language. set to default language.
      //
      if (*LanguageTable == 0) {
        LanguageIndex = 1;
      }
      //
      // update type13 current language.
      //
      FreePool(SCULanguage);
      FreePool(Type13Lang);
      Type13Record = (SMBIOS_TABLE_TYPE13 *) CurrentSmbiosHeader;
      Type13Record->CurrentLanguages = LanguageIndex;
      break;

    default:
      break;
    }
  }
  return EFI_SUCCESS;
}


/**
  This callback function will sort the handle numbers of SMBIOS table.

**/
VOID
EFIAPI
SmbiosSortHandleNumber (
  )
{
  EFI_SMBIOS_HANDLE               Index;
  SMBIOS_INSTANCE                *Private;
  LIST_ENTRY                     *Link;
  LIST_ENTRY                     *Head;
  EFI_SMBIOS_ENTRY               *SmbiosEntry;
  EFI_SMBIOS_TABLE_HEADER        *CurrentSmbiosHeader;
  SMBIOS_TABLE_ENTRY_POINT       *SmbiosTableEntryAddress;
  UINT8                          *TempPtr;
  SMBIOS_TABLE_TYPE5             *SmbiosType5;
  SMBIOS_TABLE_TYPE37            *SmbiosType37;
  SMBIOS_TABLE_TYPE40            *SmbiosType40;
  ADDITIONAL_INFORMATION_ENTRY   *AdditionalInfo;

  //
  // Initialize the data we need.
  //
  SmbiosTableEntryAddress = NULL;
  Private = SMBIOS_INSTANCE_FROM_THIS (&mPrivateData.Smbios);
  Head = &Private->DataListHead;
  EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID **)&SmbiosTableEntryAddress);
  if (SmbiosTableEntryAddress == NULL) {
    return;
  }
  mNumberOfStructure = SmbiosTableEntryAddress->NumberOfSmbiosStructures;
  mMappingTable = AllocateZeroPool (mNumberOfStructure * sizeof (EFI_SMBIOS_HANDLE));
  if (mMappingTable == NULL) {
    return;
  }

  //
  // Sort the handle numbers and make a mapping table.
  //
  for (Link = Head->ForwardLink, Index = 0; Link != Head; Link = Link->ForwardLink, Index++) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK (Link);
    CurrentSmbiosHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);
    mMappingTable[Index] = CurrentSmbiosHeader->Handle;
    CurrentSmbiosHeader->Handle = Index;
  }

  //
  // Adjust LinkData handle values.
  //
  for (Link = Head->ForwardLink; Link != Head; Link = Link->ForwardLink) {
    SmbiosEntry = SMBIOS_ENTRY_FROM_LINK (Link);
    CurrentSmbiosHeader = (EFI_SMBIOS_TABLE_HEADER*)(SmbiosEntry->RecordHeader + 1);

    switch (CurrentSmbiosHeader->Type) {

    case EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION:
      ((SMBIOS_TABLE_TYPE2 *)CurrentSmbiosHeader)->ChassisHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE);
      break;

    case EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION:
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE4 *)CurrentSmbiosHeader)->L1CacheHandle));
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE4 *)CurrentSmbiosHeader)->L2CacheHandle));
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE4 *)CurrentSmbiosHeader)->L3CacheHandle));
      break;

    case EFI_SMBIOS_TYPE_MEMORY_CONTROLLER_INFORMATION:
      SmbiosType5 = (SMBIOS_TABLE_TYPE5 *)CurrentSmbiosHeader;
      for (Index = 0; Index < SmbiosType5->AssociatedMemorySlotNum; Index++) {
        TempPtr = (UINT8 *)(((SMBIOS_TABLE_TYPE5 *)CurrentSmbiosHeader)->MemoryModuleConfigHandles) +
                  Index * sizeof (EFI_SMBIOS_HANDLE);
        FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)TempPtr);
      }
      break;

    case EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS:
      ((SMBIOS_TABLE_TYPE14 *)CurrentSmbiosHeader)->Group->ItemHandle = SearchHandle (Head, ((SMBIOS_TABLE_TYPE14 *)CurrentSmbiosHeader)->Group->ItemType);
      break;

    case EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY:
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE16 *)CurrentSmbiosHeader)->MemoryErrorInformationHandle));
      break;

    case EFI_SMBIOS_TYPE_MEMORY_DEVICE:
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE17 *)CurrentSmbiosHeader)->MemoryArrayHandle));
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE17 *)CurrentSmbiosHeader)->MemoryErrorInformationHandle));
      break;

    case EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS:
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE19 *)CurrentSmbiosHeader)->MemoryArrayHandle));
      break;

    case EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS:
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE20 *)CurrentSmbiosHeader)->MemoryDeviceHandle));
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE20 *)CurrentSmbiosHeader)->MemoryArrayMappedAddressHandle));
      break;

    case EFI_SMBIOS_TYPE_COOLING_DEVICE:
      ((SMBIOS_TABLE_TYPE27 *)CurrentSmbiosHeader)->TemperatureProbeHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_TEMPERATURE_PROBE);
      break;

    case EFI_SMBIOS_TYPE_MANAGEMENT_DEVICE_COMPONENT:
      ((SMBIOS_TABLE_TYPE35 *)CurrentSmbiosHeader)->ManagementDeviceHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_MANAGEMENT_DEVICE);
      FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&(((SMBIOS_TABLE_TYPE35 *)CurrentSmbiosHeader)->ComponentHandle));
      if (((SMBIOS_TABLE_TYPE35 *)CurrentSmbiosHeader)->ThresholdHandle != 0xFFFF) {
        ((SMBIOS_TABLE_TYPE35 *)CurrentSmbiosHeader)->ThresholdHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA);
      }
      break;

    case EFI_SMBIOS_TYPE_MEMORY_CHANNEL:
      SmbiosType37 = (SMBIOS_TABLE_TYPE37 *)CurrentSmbiosHeader;
      for (Index = 0; Index < SmbiosType37->MemoryDeviceCount; Index++) {
        TempPtr = (UINT8 *)(UINTN)&(((SMBIOS_TABLE_TYPE37 *)CurrentSmbiosHeader)->MemoryDevice->DeviceHandle) +
                  Index * sizeof (MEMORY_DEVICE);
        FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)TempPtr);
      }
      break;

    case EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY:
      if (((SMBIOS_TABLE_TYPE39 *)CurrentSmbiosHeader)->InputVoltageProbeHandle != 0xFFFF) {
        ((SMBIOS_TABLE_TYPE39 *)CurrentSmbiosHeader)->InputVoltageProbeHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_VOLTAGE_PROBE);
      }
      if (((SMBIOS_TABLE_TYPE39 *)CurrentSmbiosHeader)->CoolingDeviceHandle != 0xFFFF) {
        ((SMBIOS_TABLE_TYPE39 *)CurrentSmbiosHeader)->CoolingDeviceHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_COOLING_DEVICE);
      }
      if (((SMBIOS_TABLE_TYPE39 *)CurrentSmbiosHeader)->InputCurrentProbeHandle != 0xFFFF) {
        ((SMBIOS_TABLE_TYPE39 *)CurrentSmbiosHeader)->InputCurrentProbeHandle = SearchHandle (Head, EFI_SMBIOS_TYPE_ELECTRICAL_CURRENT_PROBE);
      }
      break;

    case EFI_SMBIOS_TYPE_ADDITIONAL_INFORMATION:
      SmbiosType40 = (SMBIOS_TABLE_TYPE40 *)CurrentSmbiosHeader;
      AdditionalInfo = ((SMBIOS_TABLE_TYPE40 *)CurrentSmbiosHeader)->AdditionalInfoEntries;
      for (Index = 0; Index < SmbiosType40->NumberOfAdditionalInformationEntries; Index++) {
        FixupLinkData ((EFI_SMBIOS_HANDLE *)(UINTN)&AdditionalInfo->ReferencedHandle);
        AdditionalInfo = (ADDITIONAL_INFORMATION_ENTRY *)(((UINT8 *)AdditionalInfo) + AdditionalInfo->EntryLength);
      }
      break;

    default:
      break;
    }

  }

  gBS->FreePool (mMappingTable);
}

/**
  Before boot, some data need to be fixed.

**/
VOID
EFIAPI
SmbiosReadyToBootCallback (
  IN EFI_EVENT                              Event,
  IN VOID                                   *Cotext
  )
{
  gBS->CloseEvent (Event);
  //
  // Make this event late enough.
  //
  gBS->RestoreTPL (TPL_APPLICATION);

  SmbiosUpdateValue ();
  SmbiosSortHandleNumber ();
  SmbiosTableConstruction ();

  gBS->RaiseTPL (TPL_CALLBACK);

  return;
}

/**

  Driver to produce SMBIOS protocol and pre-allocate 1 page for the final SMBIOS table.

  @param ImageHandle     Module's image handle
  @param SystemTable     Pointer of EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS    SMBIOS protocol installed
  @retval Other          No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
SmbiosDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  EFI_EVENT             SmbiosSortEvent;


  mPrivateData.Signature                = SMBIOS_INSTANCE_SIGNATURE;
  mPrivateData.Smbios.Add               = SmbiosAdd;
  mPrivateData.Smbios.UpdateString      = SmbiosUpdateString;
  mPrivateData.Smbios.Remove            = SmbiosRemove;
  mPrivateData.Smbios.GetNext           = SmbiosGetNext;
  mPrivateData.Smbios.MajorVersion      = (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) >> 8);
  mPrivateData.Smbios.MinorVersion      = (UINT8) (FixedPcdGet16 (PcdSmbiosVersion) & 0x00ff);

  InitializeListHead (&mPrivateData.DataListHead);
  InitializeListHead (&mPrivateData.AllocatedHandleListHead);
  EfiInitializeLock (&mPrivateData.DataLock, TPL_NOTIFY);

  //
  // Initialize the EntryPointStructure with initial values.
  // Allocate memory (below 4GB).
  //
  PhysicalAddress = 0xffffffff;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES (sizeof (SMBIOS_TABLE_ENTRY_POINT)),
                  &PhysicalAddress
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmbiosDriverEntryPoint() could not allocate EntryPointStructure < 4GB\n"));
    Status = gBS->AllocatePages (
                    AllocateAnyPages,
                    EfiRuntimeServicesData,
                    EFI_SIZE_TO_PAGES (sizeof (SMBIOS_TABLE_ENTRY_POINT)),
                    &PhysicalAddress
                    );
   if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  EntryPointStructure = (SMBIOS_TABLE_ENTRY_POINT *) (UINTN) PhysicalAddress;

  CopyMem (
    EntryPointStructure,
    &EntryPointStructureData,
    sizeof (SMBIOS_TABLE_ENTRY_POINT)
    );

  //
  // Pre-allocate 1 page for SMBIOS table below 4GB.
  // SMBIOS table will be updated when new SMBIOS type is added or
  // existing SMBIOS type is updated. If the total size of SMBIOS table exceeds 1 page,
  // we will re-allocate new memory when creating whole SMBIOS table.
  //
  PhysicalAddress = 0xffffffff;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiRuntimeServicesData,
                  1,
                  &PhysicalAddress
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmbiosDriverEntryPoint() could not allocate SMBIOS table < 4GB\n"));
    EntryPointStructure->TableAddress = 0;
  } else {
    EntryPointStructure->TableAddress = (UINT32) PhysicalAddress;
    mPreAllocatedPages = 1;
  }

  //
  // Init TableLength to the length of End-Of-Table structure for SmbiosAdd() called at the first time
  // to check the TableLength limitation.
  //
  EntryPointStructure->TableLength  = sizeof (EFI_SMBIOS_TABLE_END_STRUCTURE);

  //
  //  Depend on PCD create SMBIOS structures
  //
  RecordSmbios();

  //
  // Make a new handle and install the protocol
  //
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiSmbiosProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.Smbios
                  );

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             SmbiosReadyToBootCallback,
             NULL,
             &SmbiosSortEvent
             );

  return Status;
}
