//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++


Module Name:

  MemoryCheck.c

Abstract:

  BDS Lib functions which relate with check memory consistency

--*/

#include "MemoryCheck.h"

#ifdef EFI_DEBUG
  CHAR16                        *mMemoryTypeDesc[EfiMaxMemoryType] = {
                                   L"reserved  ",
                                   L"LoaderCode",
                                   L"LoaderData",
                                   L"BS_code   ",
                                   L"BS_data   ",
                                   L"RT_code   ",
                                   L"RT_data   ",
                                   L"available ",
                                   L"Unusable  ",
                                   L"ACPI_recl ",
                                   L"ACPI_NVS  ",
                                   L"MemMapIO  ",
                                   L"MemPortIO ",
                                   L"PAL_code  "
                                   };
#endif

#define MAX_BS_DATA_PAGES       0x10000
#define MAX_PRE_ALLOCATE_PAGES  0x40000



#if (defined MEMORY_MAP_CONSISTENCY_CHECK) || (defined EFI_DEBUG)
STATIC
EFI_MEMORY_DESCRIPTOR *
GetMemoryMap (
  OUT UINTN                  *MemoryMapSize,
  OUT UINTN                  *DescriptorSize
  )
/*++

Routine Description:

  The function returns a copy of the current memory map which is allocated from pool.

Arguments:

  MemoryMapSize    - A pointer to the size, in bytes, of the MemoryMap buffer.
  *DescriptorSize  - A pointer to the location in which firmware returns the size, in
                     bytes, of an individual EFI_MEMORY_DESCRIPTOR.
Returns:

  A pointer to the current memory map is returned.
  NULL is returned if space for the memory map could not be allocated from pool.
  It is up to the caller to free the memory if they are no longer needed.

--*/
{
  EFI_STATUS                Status;
  EFI_MEMORY_DESCRIPTOR     *EfiMemoryMap;
  UINTN                     EfiMemoryMapSize;
  UINTN                     EfiMapKey;
  UINTN                     EfiDescriptorSize;
  UINT32                    EfiDescriptorVersion;


  EfiMemoryMapSize = 0;
  EfiMemoryMap     = NULL;
  Status = gBS->GetMemoryMap (
                  &EfiMemoryMapSize,
                  EfiMemoryMap,
                  &EfiMapKey,
                  &EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  while (Status == EFI_BUFFER_TOO_SMALL) {
    Status = gBS->AllocatePool (EfiBootServicesData, EfiMemoryMapSize, &EfiMemoryMap);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = gBS->GetMemoryMap (
                    &EfiMemoryMapSize,
                    EfiMemoryMap,
                    &EfiMapKey,
                    &EfiDescriptorSize,
                    &EfiDescriptorVersion
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (EfiMemoryMap);
      break;
    }
  }

  if (!EFI_ERROR (Status)) {
    *MemoryMapSize  = EfiMemoryMapSize;
    *DescriptorSize = EfiDescriptorSize;
  }

  return EfiMemoryMap;
}
#endif

#ifdef MEMORY_MAP_CONSISTENCY_CHECK
STATIC
BOOLEAN
IsRtMemType (
  IN EFI_MEMORY_TYPE   Type
  )
/*++

Routine Description:

  Check if it is runtime mempry type

Arguments:

  Type               - Memory type

Returns:

  TRUE        - It is runtime mempry type
  FALSE       - It is not runtime mempry type

--*/
{
  if (Type == EfiReservedMemoryType ||
      Type == EfiRuntimeServicesCode ||
      Type == EfiRuntimeServicesData ||
      Type == EfiACPIReclaimMemory ||
      Type == EfiACPIMemoryNVS
      ) {
    return TRUE;
  }

  return FALSE;
}

STATIC
EFI_STATUS
AddSkipMemRange (
  IN     EFI_PHYSICAL_ADDRESS          MemStart,
  IN     EFI_PHYSICAL_ADDRESS          MemEnd,
  IN OUT EFI_LIST_ENTRY                *SkipMemRangeList
  )
/*++

Routine Description:

  Add a memory range into the list of skip memory ranges.

Arguments:

  MemStart           - Memory start address.
  MemEnd             - Memory end address.
  SkipMemRangeList   - Pointer to list of skip memory range

Returns:

  EFI_SUCCESS            - Add new skip memory range successfully.
  EFI_INVALID_PARAMETER  - Input parameter is NULL.
  EFI_OUT_OF_RESOURCES   - Allocate memory is fail.

--*/
{
  EFI_LIST_ENTRY                   *StartLink;
  EFI_LIST_ENTRY                   *CurrentLink;
  SKIP_MEMORY_RANGE                *SkipMemRange;
  SKIP_MEMORY_RANGE                *NewSkipMemRange;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;;
  }

  //
  // Check if input memory range is adjacent with stored skip memory range or not.
  // If yes, combine the input memory range with the stored skip memory range then return.
  //
  StartLink   = SkipMemRangeList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != StartLink) {
    SkipMemRange = SKIP_MEMORY_RANGE_FROM_THIS (CurrentLink);

    if (MemStart == SkipMemRange->MemEnd + 1) {
      SkipMemRange->MemEnd = MemEnd;
      return EFI_SUCCESS;
    } else if (MemEnd == SkipMemRange->MemStart - 1) {
      SkipMemRange->MemStart = MemStart;
      return EFI_SUCCESS;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  //
  // Create a new skip memory range.
  //
  NewSkipMemRange = EfiLibAllocatePool (sizeof (SKIP_MEMORY_RANGE));
  if (NewSkipMemRange == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewSkipMemRange->Signature = SKIP_MEMORY_RANGE_SIGNATURE;
  NewSkipMemRange->MemStart  = MemStart;
  NewSkipMemRange->MemEnd    = MemEnd;

  InsertTailList (SkipMemRangeList, &NewSkipMemRange->Link);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
AddNewSkipMemRangeFromVariable (
  IN OUT EFI_LIST_ENTRY          *SkipMemRangeList
  )
/*++

Routine Description:

  Add New memory skip range to skip memory range list from MemoeryTypeUpdateInfo variable.

Arguments:

  SkipMemRangeList       - Pointer to list of skip memory range

Returns:

  EFI_SUCCESS            - Add skip memory range successfully.
  EFI_INVALID_PARAMETER  - Input parameter is NULL.

--*/
{
  UINT32                            *UpdateInfo;
  UINTN                             InfoVarSize;
  MEMORY_RANGE                      *MemoryRange;
  UINTN                             MemoryRangeCnt;
  UINTN                             Index;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // MemoeryTypeUpdateInfo variable format:
  //   UINT32        AdjustMemoryTimes;
  //   MEMORY_RANGE  SkipMemoryRanges[];
  //
  UpdateInfo = BdsLibGetVariableAndSize (
                L"MemoeryTypeUpdateInfo",
                &gEfiMemoryTypeInformationGuid,
                &InfoVarSize
                );
  if (UpdateInfo == NULL) {
    return EFI_SUCCESS;
  }

  if (InfoVarSize <= sizeof (UINT32)) {
    gBS->FreePool (UpdateInfo);
    return EFI_SUCCESS;
  }

  MemoryRangeCnt = (InfoVarSize - sizeof (UINT32)) / sizeof (MEMORY_RANGE);
  MemoryRange = (MEMORY_RANGE *) (UpdateInfo + 1);
  for (Index = 0; Index < MemoryRangeCnt; Index++) {
    AddSkipMemRange (MemoryRange[Index].MemStart, MemoryRange[Index].MemEnd, SkipMemRangeList);
  }

  gBS->FreePool (UpdateInfo);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CreateSkipMemRangeList (
  IN OUT EFI_LIST_ENTRY          *SkipMemRangeList
  )
/*++

Routine Description:

  Create the list of skip memory ranges.

Arguments:

  SkipMemRangeList   - Pointer to list of skip memory range

Returns:

  EFI_SUCCESS            - Create the list of skip memory range successfully.
  EFI_INVALID_PARAMETER  - Input parameter is NULL.

--*/
{
  EFI_PEI_HOB_POINTERS          Hob;
  UINT64                        Length;
  EFI_PHYSICAL_ADDRESS          iGfxBaseAddress[] = {0x20000000, 0x40000000};
  EFI_STATUS                    Status;
#ifdef INSYDE_DEBUG
  UINTN                         NumberOfHandles;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_GUID                      NameGuid;
  EFI_GUID                      FileGuidUsbDebugIo  = {0xD719DBFC, 0xA4ED, 0x4cab, {0xA4, 0x78, 0xA7, 0x01, 0xB5, 0x1B, 0x95, 0xD6}};
  EFI_GUID                      FileGuidDebugEngine = {0x4803B88E, 0x9E66, 0x45dc, {0x87, 0x09, 0xfc, 0x75, 0xd3, 0x9c, 0xaf, 0x1d}};
  EFI_PHYSICAL_ADDRESS          UsbDebugIoBaseAddress;
  UINTN                         UsbDebugIoSize;
  EFI_PHYSICAL_ADDRESS          DebugEngineBaseAddress;
  UINTN                         DebugEngineSize;
#endif

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;;
  }

  InitializeListHead (SkipMemRangeList);

  //
  // Add the Skip memory range below 1M Bytes for legacy BIOS.
  //
  AddSkipMemRange (
    0,
    0x100000 - 1,
    SkipMemRangeList
    );

  //
  // Skip the memory range for the memory is allocated before DXE pre-allocation operation.
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &Hob.Raw);
  if (!EFI_ERROR (Status)) {
    Hob.Raw = GetHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, Hob.Raw);
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION &&
          IsRtMemType (Hob.MemoryAllocation->AllocDescriptor.MemoryType)) {
        AddSkipMemRange (
          Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress,
          Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress + Hob.MemoryAllocation->AllocDescriptor.MemoryLength - 1,
          SkipMemRangeList
          );
      }

      Hob.Raw = GET_NEXT_HOB (Hob);
    }
  }

  //
  // Add the skip memory range of iGfx memory resources, because this memory is allocated by fixed address.
  //
  Length = 0x200000;
  AddSkipMemRange (
    iGfxBaseAddress[0],
    iGfxBaseAddress[0] + Length - 1,
    SkipMemRangeList
    );
  AddSkipMemRange (
    iGfxBaseAddress[1],
    iGfxBaseAddress[1] + Length - 1,
    SkipMemRangeList
    );

#ifdef INSYDE_DEBUG
  //
  // Add the skip memory range of H2O debug.
  //
  UsbDebugIoBaseAddress  = 0;
  DebugEngineBaseAddress = 0;
  NumberOfHandles = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (!EFI_ERROR(Status)) {
    for (Index = 0; Index < NumberOfHandles; Index++) {
      if (UsbDebugIoBaseAddress != 0 && DebugEngineBaseAddress != 0) {
        break;
      }

      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiLoadedImageProtocolGuid,
                      (VOID **) &LoadedImage
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      NameGuid = ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) (LoadedImage->FilePath))->NameGuid;

      if (EfiCompareGuid (&NameGuid, &FileGuidUsbDebugIo)) {
        UsbDebugIoBaseAddress = (EFI_PHYSICAL_ADDRESS) LoadedImage->ImageBase;
        UsbDebugIoSize        = EFI_PAGES_TO_SIZE(EFI_SIZE_TO_PAGES(LoadedImage->ImageSize));
        AddSkipMemRange (
          UsbDebugIoBaseAddress,
          UsbDebugIoBaseAddress + UsbDebugIoSize - 1,
          SkipMemRangeList
          );
        continue;
      }

      if (EfiCompareGuid (&NameGuid, &FileGuidDebugEngine)) {
        DebugEngineBaseAddress = (EFI_PHYSICAL_ADDRESS) LoadedImage->ImageBase;
        DebugEngineSize        = EFI_PAGES_TO_SIZE(EFI_SIZE_TO_PAGES(LoadedImage->ImageSize));
        AddSkipMemRange (
          DebugEngineBaseAddress,
          DebugEngineBaseAddress + DebugEngineSize - 1,
          SkipMemRangeList
          );
        continue;
      }
    }

    if (NumberOfHandles != 0 && HandleBuffer != NULL) {
      gBS->FreePool (HandleBuffer);
    }
  }
#endif

  AddNewSkipMemRangeFromVariable (SkipMemRangeList);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
DestroySkipMemRangeList (
  IN OUT EFI_LIST_ENTRY          *SkipMemRangeList
  )
/*++

Routine Description:

  Destroy the list of skip memory ranges.

Arguments:

  SkipMemRangeList       - Pointer to list of skip memory range

Returns:

  EFI_SUCCESS            - Destroy the list of skip memory range successfully.
  EFI_INVALID_PARAMETER  - Input parameter is NULL.

--*/
{
  EFI_LIST_ENTRY                   *StartLink;
  EFI_LIST_ENTRY                   *CurrentLink;
  SKIP_MEMORY_RANGE                *SkipMemRange;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;;
  }

  StartLink   = SkipMemRangeList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != StartLink) {
    SkipMemRange = SKIP_MEMORY_RANGE_FROM_THIS (CurrentLink);
    CurrentLink = CurrentLink->ForwardLink;

    gBS->FreePool (SkipMemRange);
  }

  InitializeListHead (SkipMemRangeList);

  return EFI_SUCCESS;
}

STATIC
BOOLEAN
IsSkipMemDescriptor (
  IN EFI_LIST_ENTRY           *SkipMemRangeList,
  IN EFI_MEMORY_DESCRIPTOR    *MemDescriptor
  )
/*++

Routine Description:

  Check if need to skip the memory descriptior based on the list of skip memory ranges.

Arguments:

  SkipMemRangeList   - Pointer to list of skip memory range
  MemDescriptor      - Pointer to memory descriptor

Returns:

  TRUE        - Need to skip memory descriptor.
  FALSE       - Does not need to skip memory descriptor.

--*/
{
  EFI_PHYSICAL_ADDRESS          PhysicalStart;
  EFI_PHYSICAL_ADDRESS          PhysicalEnd;
  EFI_LIST_ENTRY                *StartLink;
  EFI_LIST_ENTRY                *CurrentLink;
  SKIP_MEMORY_RANGE             *SkipMemRange;

  if (MemDescriptor == NULL || SkipMemRangeList == NULL) {
    return FALSE;
  }

  //
  // Skip the memory range of non-system memory.
  //
  if (MemDescriptor->Type == EfiReservedMemoryType &&
      MemDescriptor->Attribute == 0) {
    return TRUE;
  }

  PhysicalStart = MemDescriptor->PhysicalStart;
  PhysicalEnd   = MemDescriptor->PhysicalStart + (UINT64) EFI_PAGES_TO_SIZE(MemDescriptor->NumberOfPages) - 1;

  StartLink   = SkipMemRangeList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != StartLink) {
    SkipMemRange = SKIP_MEMORY_RANGE_FROM_THIS (CurrentLink);

    if (PhysicalStart >= SkipMemRange->MemStart &&
        PhysicalEnd   <= SkipMemRange->MemEnd) {
      return TRUE;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  return FALSE;
}

STATIC
UINT32
CalSuggestionValue (
  IN UINT32            UsedPageNum
  )
/*++

Routine Description:

  Calculate suggestion page number which is 1.5 tims of used page number.

Arguments:

  UsedPageNum       - Used page number

Returns:

  Suggestion page number.

--*/
{
  UINT32                SuggestionValue;

  SuggestionValue = UsedPageNum * 3 / 2;
  return (SuggestionValue & 0xF) == 0 ? SuggestionValue : (SuggestionValue & ~0xF) + 0x10;
}


STATIC
EFI_MEMORY_TYPE_INFORMATION *
GetMemoryTypeInfo (
  OUT UINTN       *MemoryInfoCnt
  )
/*++

Routine Description:

  Internal function uses to get memory type information and the count of memory information.

Arguments:

  MemoryInfoCnt  - A pointer to the number of memory type information in returned pointer to EFI_MEMORY_TYPE_INFORMATION.

Returns:

  Pointer to EFI_MEMORY_TYPE_INFORMATION buffer.

--*/
{
  EFI_STATUS                      Status;
  VOID                            *HobList;
  EFI_MEMORY_TYPE_INFORMATION     *MemTypeInfo;
  UINTN                           Size;


  EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  Status = GetNextGuidHob (&HobList, &gEfiMemoryTypeInformationGuid, &MemTypeInfo, &Size);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  if (MemoryInfoCnt != NULL) {
    *MemoryInfoCnt = Size / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  }

  return MemTypeInfo;

}

STATIC
BOOLEAN
IsNeedAddToUsedMemory (
  IN EFI_LIST_ENTRY           *SkipMemRangeList,
  IN EFI_MEMORY_DESCRIPTOR    *MemDescriptor
  )
/*++

Routine Description:

  Deternime the memory entry information is whether need add to used memory

Arguments:

  SkipMemRangeList   - Pointer to list of skip memory range
  MemDescriptor      - Pointer to memory descriptor

Returns:

  TRUE        - Need add to used memory
  FALSE       - Needn't  add to used memory

--*/
{
  if (IsSkipMemDescriptor (SkipMemRangeList, MemDescriptor)) {
    return FALSE;
  }
  //
  // Since EfiBootServicesData type memory only used to prevent from need too many memory node to save memory,
  // If a memory size in memory is large than 256MB, we can skip this node.
  //
  if (MemDescriptor->Type == EfiBootServicesData && MemDescriptor->NumberOfPages >= MAX_BS_DATA_PAGES) {
    return FALSE;
  }
  return TRUE;

}

STATIC
EFI_STATUS
GetUsedPageNum (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  EFI_LIST_ENTRY           *SkipMemRangeList,
  OUT UINT32                   *UsedPageNum
  )
/*++

Routine Description:

  Internal function uses to get current used page number of memory by specifc memory type.
  The number of used memory DOES NOT include the memory which is located in skip memory range list.

Arguments:

  Type              - The memory type.
  MemoryMap         - A pointer to the memory map.
  MemoryMapSize     - The size, in bytes, of the MemoryMap buffer.
  DescriptorSize    - The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  SkipMemRangeList  - Pointer to list of skip memory range.
  UsedPageNum       - Pointer to used memory page number for input memry type.

Returns:

  EFI_SUCCESS            - Get used memory page number successfully.
  EFI_INVALID_PARAMETER  - MemoryMap or UsedPageNum is NULL.

--*/
{
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR             *EfiEntry;


  if (MemoryMap == NULL || UsedPageNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *UsedPageNum    = 0;
  EfiEntry        = MemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) MemoryMap + MemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    if (EfiEntry->Type == Type) {
      if (IsNeedAddToUsedMemory (SkipMemRangeList, EfiEntry)) {
        *UsedPageNum +=  (UINT32) (UINTN) EfiEntry->NumberOfPages;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, DescriptorSize);
  }

  return EFI_SUCCESS;
}

STATIC
BOOLEAN
IsMemoryOverUsage (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  EFI_LIST_ENTRY           *SkipMemRangeList
  )
/*++

Routine Description:

  This function uses to check the allocated memory size of input type memory is whether over than
  pre-allocated memory size of this memory type.

Arguments:

  Type              - The memory type.
  MemoryMap         - A pointer to the memory map.
  MemoryMapSize     - The size, in bytes, of the MemoryMap buffer.
  DescriptorSize    - The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  SkipMemRangeList  - Pointer to list of skip memory range.

Returns:

  TRUE   - This memory type is over usage.
  FALSE  - This memory type isn't over usage.

--*/
{
  EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo;
  UINTN                             MemoryInfoCnt;
  UINTN                             Index;
  UINT32                            PreAllocPageNum;
  UINT32                            UsedPageNum;

  if (MemoryMap == NULL) {
    return TRUE;
  }

  MemTypeInfo = GetMemoryTypeInfo (&MemoryInfoCnt);
  if (MemTypeInfo == NULL) {
    return TRUE;
  }
  PreAllocPageNum = 0;
  for (Index = 0; Index < MemoryInfoCnt; Index++) {
    if (MemTypeInfo[Index].Type == Type) {
      PreAllocPageNum = MemTypeInfo[Index].NumberOfPages;
      break;
    }
  }

  UsedPageNum = 0;
  GetUsedPageNum (Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &UsedPageNum);

  return (BOOLEAN) (UsedPageNum > PreAllocPageNum);
}


STATIC
BOOLEAN
IsRtMemoryOverUsage (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  EFI_LIST_ENTRY           *SkipMemRangeList
  )
/*++

Routine Description:

  This function uses to check the allocated memory size of runtime types memory is whether over than
  pre-allocated memory size of runtime memory types.

Arguments:

  MemoryMap         - A pointer to the memory map.
  MemoryMapSize     - The size, in bytes, of the MemoryMap buffer.
  DescriptorSize    - The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  SkipMemRangeList  - Pointer to list of skip memory range

Returns:

  TRUE   - Runtime memory types is over usage.
  FALSE  - Runtime memory type isn't over usage.

--*/
{
  UINTN                    RtTypeMemCnt;
  UINTN                    Index;
  UINT32                   RtMemoryType[] = {
                             EfiReservedMemoryType,
                             EfiACPIMemoryNVS,
                             EfiACPIReclaimMemory,
                             EfiRuntimeServicesData,
                             EfiRuntimeServicesCode
                             };

  RtTypeMemCnt = sizeof(RtMemoryType) / sizeof(UINT32);
  for (Index = 0; Index < RtTypeMemCnt; Index++) {
    if (IsMemoryOverUsage (RtMemoryType[Index], MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
      return TRUE;
    }
  }
  return FALSE;
}

STATIC
BOOLEAN
IsBsMemoryOverUsage (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  EFI_LIST_ENTRY           *SkipMemRangeList
  )
/*++

Routine Description:

  This function uses to check the allocated memory size of EfiBootServicesCode and EfiBootServicesData is whether
  over than pre-allocated memory size of EfiBootServicesCode and EfiBootServicesData memory.

Arguments:

  MemoryMap         - A pointer to the memory map.
  MemoryMapSize     - The size, in bytes, of the MemoryMap buffer.
  DescriptorSize    - The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  SkipMemRangeList  - Pointer to list of skip memory range

Returns:

  TRUE   - Boot services memory type is over usage.
  FALSE  - Boot services memory type isn't over usage.

--*/
{
  if (IsMemoryOverUsage (EfiBootServicesCode, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList) ||
      IsMemoryOverUsage (EfiBootServicesData, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
    return TRUE;
  }
  return FALSE;
}

STATIC
BOOLEAN
IsBsOrRtMemoryOverUsage (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  EFI_LIST_ENTRY           *SkipMemRangeList
  )
/*++

Routine Description:

  This function uses to check the allocated memory size of runtime and boot services memory types are whether
  over than pre-allocated memory size of runtime and boot services memory types.

Arguments:

  MemoryMap         - A pointer to the memory map.
  MemoryMapSize     - The size, in bytes, of the MemoryMap buffer.
  DescriptorSize    - The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  SkipMemRangeList  - Pointer to list of skip memory range

Returns:

  TRUE   - Runtime or boot services memory types is over usage.
  FALSE  - Both Runtime and boot services memory types aren't over usage.

--*/
{
  if (IsBsMemoryOverUsage (MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList) ||
      IsRtMemoryOverUsage (MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
    return TRUE;
  }
  return FALSE;
}

STATIC
EFI_STATUS
AddNewSkipMemRange (
  IN      UINT32             Type,
  IN      EFI_LIST_ENTRY     *SkipMemRangeList,
  IN OUT  UINT32             **UpdateInfo,
  IN OUT  UINTN              *InfoVarSize,
  OUT     UINT32             *PageNum
  )
/*++

Routine Description:

  Add new skip memory ranges to update information buffer and increase the information size for
  specific memory type.

Arguments:

  Type              - The memory type.
  SkipMemRangeList  - Pointer to list of skip memory range.
  UpdateInfo        - On input, Pointer to original information buffer.
                      On output, pointer to updated information buffer.
  InfoVarSize       - On input, Pointer to the size by bytes of original information buffer.
                      On output, Pointer to the size by bytes of updated information buffer.
  PageNum           - Number of pages which add to new skip memory range.

Returns:

  EFI_SUCCESS            - Add new memory range information successfully.
  EFI_INVALID_PARAMETER  - UpdateInfo, *UpdateInfo, InfoVarSize, PageNum is NULL.
  EFI_OUT_OF_RESOURCES   - Allocate pool failed.
  EFI_UNSUPPORTED        - Not find any new skip range in input memory type.

--*/
{

  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMap;
  UINTN                         EfiMemoryMapSize;
  UINTN                         EfiDescriptorSize;
  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR         *EfiEntry;
  UINTN                         NumOfSkipMemRange;
  UINTN                         NumOfMemRange;
  UINT32                        *CurrentUpdateInfo;
  UINTN                         CurrentInfoVarSize;
  MEMORY_RANGE                  *MemoryRangeEntry;
  UINT64                        SkipPageNum;

  if (UpdateInfo == NULL || *UpdateInfo == NULL || InfoVarSize == NULL || PageNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiMemoryMap = GetMemoryMap (&EfiMemoryMapSize, &EfiDescriptorSize);
  if (EfiMemoryMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Calcuate number of memory ranges.
  //
  NumOfMemRange = 0;
  EfiEntry      = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    if (EfiEntry->Type == Type) {
      if (!IsSkipMemDescriptor (SkipMemRangeList, EfiEntry)) {
        NumOfMemRange++;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }
  //
  // Cannot find pre-allocated memory and new skip memory range, just return EFI_NOT_FOUND.
  //
  if (NumOfMemRange < 1) {
    gBS->FreePool (EfiMemoryMap);
    return EFI_NOT_FOUND;
  }
  //
  // System doesn't have any skip memory range, just return EFI_SUCCESS.
  //
  NumOfSkipMemRange = NumOfMemRange - 1;
  if (NumOfSkipMemRange == 0) {
    gBS->FreePool (EfiMemoryMap);
    *PageNum = 0;
    return EFI_SUCCESS;
  }
  //
  // Copy original data to new buffer and
  //
  CurrentInfoVarSize = *InfoVarSize + NumOfSkipMemRange * sizeof (MEMORY_RANGE);
  CurrentUpdateInfo = EfiLibAllocateZeroPool (CurrentInfoVarSize);
  if (CurrentUpdateInfo == NULL) {
    gBS->FreePool (EfiMemoryMap);
    return EFI_OUT_OF_RESOURCES;
  }
  EfiCopyMem (CurrentUpdateInfo, *UpdateInfo, *InfoVarSize);

  //
  // Append new skip memory ranges.
  //
  MemoryRangeEntry = (MEMORY_RANGE *) (((UINT8 *) CurrentUpdateInfo) + *InfoVarSize);
  EfiEntry         = EfiMemoryMap;
  EfiMemoryMapEnd  = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  SkipPageNum      = 0;
  while (EfiEntry < EfiMemoryMapEnd && NumOfSkipMemRange != 0) {
    if (EfiEntry->Type == Type) {
      if (!IsSkipMemDescriptor (SkipMemRangeList, EfiEntry)) {
        MemoryRangeEntry->MemStart = EfiEntry->PhysicalStart;
        MemoryRangeEntry->MemEnd = EfiEntry->PhysicalStart + MultU64x32(EfiEntry->NumberOfPages , EFI_PAGE_SIZE) - 1;
        DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n%s memory located at 0x%08x is fixed address and the size is 0x%08x!!!\n",   \
               mMemoryTypeDesc[Type], MemoryRangeEntry->MemStart, MultU64x32(EfiEntry->NumberOfPages, EFI_PAGE_SIZE)) \
              );
        SkipPageNum += EfiEntry->NumberOfPages;
        MemoryRangeEntry++;
        NumOfSkipMemRange--;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  gBS->FreePool (EfiMemoryMap);
  gBS->FreePool (*UpdateInfo);
  *UpdateInfo  = CurrentUpdateInfo;
  *InfoVarSize = CurrentInfoVarSize;
  *PageNum     = (UINT32) (UINTN) SkipPageNum;

  return EFI_SUCCESS;
}

STATIC
BOOLEAN
IsMemoryTyepInfoValid (
  IN EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo,
  IN UINTN                             MemTypeInfoCnt
  )
/*++

Routine Description:

  Check the input memory type information is whether valid.

Arguments:

  MemTypeInfo     - Pointer to input EFI_MEMORY_TYPE_INFORMATION array
  MemTypeInfoCnt  - The count of EFI_MEMORY_TYPE_INFORMATION instance.

Returns:

  TRUE            - The input EFI_MEMORY_TYPE_INFORMATION is valid.
  FALSE           - Any of EFI_MEMORY_TYPE_INFORMATION instance in input array is invalid.

--*/
{
  UINTN         Index;

  if (MemTypeInfo == NULL && MemTypeInfoCnt != 0) {
    return FALSE;
  }

  for (Index = 0; Index < MemTypeInfoCnt; Index++) {
    if ((MemTypeInfo[Index].NumberOfPages & 0x80000000) != 0 || MemTypeInfo[Index].Type > EfiMaxMemoryType) {
        DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n%s memory size is too large (0x%08x pages )!!!\n",   \
               mMemoryTypeDesc[Index], MemTypeInfo[Index].NumberOfPages) \
              );
      return FALSE;
    }
  }

  return TRUE;
}

STATIC
BOOLEAN
IsTotalMemoryTooLarge (
  IN EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo,
  IN UINTN                             MemTypeInfoCnt
  )
/*++

Routine Description:

  Check the total memory size in memory type information is whether to large.

Arguments:

  MemTypeInfo     - Pointer to input EFI_MEMORY_TYPE_INFORMATION array
  MemTypeInfoCnt  - The count of EFI_MEMORY_TYPE_INFORMATION instance.

Returns:

  TRUE            - The total memory size in memory type information is too large.
  FALSE           - The total memory size in memory type information isn't too large.

--*/
{
  UINTN         Index;
  UINT32        TotalPages;

  TotalPages = 0;
  for (Index = 0; Index < MemTypeInfoCnt; Index++) {
    TotalPages += MemTypeInfo[Index].NumberOfPages;
  }
  if (TotalPages >= MAX_PRE_ALLOCATE_PAGES) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nTotal memory size need pre-allocate is too large(0x%08x pages )!!!\n", TotalPages));
    return TRUE;
  }
  return FALSE;

}

STATIC
EFI_STATUS
SetMemoryInfoVariable (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  EFI_LIST_ENTRY           *SkipMemRangeList
  )
/*++

Routine Description:

  Update MemoryTypeInformation and MemoeryTypeUpdateInfo variable.

Arguments:

  MemoryMap         - A pointer to the memory map.
  MemoryMapSize     - The size, in bytes, of the MemoryMap buffer.
  DescriptorSize    - The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  SkipMemRangeList  - Pointer to list of skip memory range

Returns:

  EFI_SUCCESS            - Update memory information related variables successfully.
  EFI_INVALID_PARAMETER  - MemoryMap is NULL.
  EFI_ABORTED            - The times of adjust memory exceeds.
  EFI_OUT_OF_RESOURCES   - Allocate pool failed.
  Other                  - Set variable failed.

--*/
{
  EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo;
  UINTN                             MemoryInfoCnt;
  EFI_MEMORY_TYPE_INFORMATION       *CurrentMemTypeInfo;
  UINTN                             CurrentMemoryInfoCnt;
  UINTN                             Index;
  UINTN                             RtIndex;
  UINTN                             BsIndex;
  UINT32                            *UpdateInfo;
  UINTN                             InfoVarSize;
  EFI_STATUS                        Status;
  UINT32                            SkipPageNum;
  UINTN                             RtTypeMemCnt;
  UINTN                             BsTypeMemCnt;
  EFI_MEMORY_TYPE_INFORMATION       RtTypeMem[] = {
                                      {EfiReservedMemoryType,  0},
                                      {EfiACPIMemoryNVS,       0},
                                      {EfiACPIReclaimMemory,   0},
                                      {EfiRuntimeServicesData, 0},
                                      {EfiRuntimeServicesCode, 0}
                                      };
  EFI_MEMORY_TYPE_INFORMATION       BsTypeMem[] = {
                                      {EfiBootServicesCode,    0},
                                      {EfiBootServicesData,    0}
                                      };


  if (MemoryMap == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsBsOrRtMemoryOverUsage (MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
    return EFI_SUCCESS;
  }

  //
  // MemoeryTypeUpdateInfo variable format:
  //   UINT32        AdjustMemoryTimes;
  //   MEMORY_RANGE  SkipMemoryRanges[];
  //
  // Check the adjust memory times.
  //
  UpdateInfo = BdsLibGetVariableAndSize (
                L"MemoeryTypeUpdateInfo",
                &gEfiMemoryTypeInformationGuid,
                &InfoVarSize
                );
  if (UpdateInfo != NULL && *UpdateInfo >= MAX_ADJUST_MEMORY_TIMES) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nSystem cannot adjust runtime type memory automatically!!!\n"));
    gBS->FreePool (UpdateInfo);
    return EFI_ABORTED;
  }
  if (UpdateInfo == NULL) {
    UpdateInfo = EfiLibAllocateZeroPool (sizeof (UINT32));
    if (UpdateInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    *UpdateInfo = 0;
    InfoVarSize = sizeof (UINT32);
  }

  //
  // Calculate recommended runtime types memory size
  //
  RtTypeMemCnt = sizeof (RtTypeMem) / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  for (Index = 0; Index < RtTypeMemCnt; Index++) {
    GetUsedPageNum (RtTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &RtTypeMem[Index].NumberOfPages);
    if (IsMemoryOverUsage (RtTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
      RtTypeMem[Index].NumberOfPages = CalSuggestionValue (RtTypeMem[Index].NumberOfPages);
    }
  }
  //
  // Calculate recommended boot services types memory size
  //
  BsTypeMemCnt = sizeof (BsTypeMem) / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  for (Index = 0; Index < BsTypeMemCnt; Index++) {
    GetUsedPageNum (BsTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &BsTypeMem[Index].NumberOfPages);
    if (IsMemoryOverUsage (BsTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
      BsTypeMem[Index].NumberOfPages = CalSuggestionValue (BsTypeMem[Index].NumberOfPages);
    }
  }
  //
  // Three steps to update memory infomation:
  // 1. Create copied original memory inforamtion.
  // 2. Merge recommended memory size to specific memory type or update new skip memory range.
  // 3. Write updated memory information and new skip memory range to variable.
  //

  //
  // 1. Create copied original memory information.
  //
  MemTypeInfo = GetMemoryTypeInfo (&MemoryInfoCnt);
  if (MemTypeInfo == NULL) {
    return EFI_ABORTED;
  }
  CurrentMemTypeInfo = EfiLibAllocateZeroPool ((MemoryInfoCnt + RtTypeMemCnt + BsTypeMemCnt) * sizeof (EFI_MEMORY_TYPE_INFORMATION));
  if (CurrentMemTypeInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  EfiCopyMem (CurrentMemTypeInfo, MemTypeInfo, MemoryInfoCnt * sizeof (EFI_MEMORY_TYPE_INFORMATION));
  CurrentMemoryInfoCnt = MemoryInfoCnt;

  //
  // 2. Merge recommended memory size to specific memory type or update new skip memory range.
  // <1> The steps for runtime types memory:
  //   (1) First time to enlarge pre-allocated memory size if it isn't large enough.
  //   (2) Since first time enlarges pre-allocated memory size, we can check memory which is allocated by
  //       AllocateAddress at second time. And then add these skip memory ranges to variable.
  //   (3) Third time to prevent from pre-allocated memory isn't enough after the adjustment of first and second time.
  //   (4) System should not enter fourth time adjustment. If occurred, it will assert by EFI_DEBUG
  // <2> For Boot services memory type, we only need enlarge pre-allocate memory size if it isn't large enough.
  //
  *UpdateInfo += 1;

  //
  // Check Runtime types memory.
  //
  for (RtIndex = 0; RtIndex < RtTypeMemCnt; RtIndex++) {
    for (Index = 0; Index < CurrentMemoryInfoCnt; Index++) {
      if (CurrentMemTypeInfo[Index].Type == RtTypeMem[RtIndex].Type) {
        //
        // used memory page numbers and pre-allocated memory page numbers is the same, just check next memory type.
        //
        if (CurrentMemTypeInfo[Index].NumberOfPages >= RtTypeMem[RtIndex].NumberOfPages) {
          break;
        }

        if (*UpdateInfo != 2) {
          DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nIncrease %s memory from 0x%x pages to 0x%x pages!!!\n",   \
                 mMemoryTypeDesc[RtTypeMem[RtIndex].Type], CurrentMemTypeInfo[Index].NumberOfPages, \
                 RtTypeMem[RtIndex].NumberOfPages) \
                );
          CurrentMemTypeInfo[Index].NumberOfPages = RtTypeMem[RtIndex].NumberOfPages;
        } else {
          Status = AddNewSkipMemRange (CurrentMemTypeInfo[Index].Type, SkipMemRangeList, &UpdateInfo, &InfoVarSize, &SkipPageNum);
          if (!EFI_ERROR (Status)) {
            DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nReduce %s memory from 0x%x pages to 0x%x pages!!!\n",   \
                   mMemoryTypeDesc[RtTypeMem[RtIndex].Type], CurrentMemTypeInfo[Index].NumberOfPages, \
                   CurrentMemTypeInfo[Index].NumberOfPages - CalSuggestionValue (SkipPageNum)) \
                  );
            if (CurrentMemTypeInfo[Index].NumberOfPages > CalSuggestionValue (SkipPageNum)) {
              CurrentMemTypeInfo[Index].NumberOfPages -= CalSuggestionValue (SkipPageNum);
            }
          } else {
            gBS->FreePool (CurrentMemTypeInfo);
            return EFI_OUT_OF_RESOURCES;
          }
        }
        break;
      }
    }
    //
    // Add new memory type information if it doesn't exist.
    //
    if (Index == CurrentMemoryInfoCnt) {
      CurrentMemTypeInfo[Index].NumberOfPages = RtTypeMem[RtIndex].NumberOfPages;
      CurrentMemoryInfoCnt++;
    }
  }

  //
  // Check boot services type memory
  //
  for (BsIndex = 0; BsIndex < BsTypeMemCnt; BsIndex++) {
    for (Index = 0; Index < CurrentMemoryInfoCnt; Index++) {
      if (CurrentMemTypeInfo[Index].Type == BsTypeMem[BsIndex].Type) {
        //
        // used memory page numbers is smaller than or equal pre-allocated memory page numbers, just check next memory type.
        //
        if (BsTypeMem[BsIndex].NumberOfPages <= CurrentMemTypeInfo[Index].NumberOfPages) {
          break;
        }
        DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nIncrease %s memory from 0x%x pages to 0x%x pages!!!\n",   \
               mMemoryTypeDesc[BsTypeMem[BsIndex].Type], CurrentMemTypeInfo[Index].NumberOfPages, \
               BsTypeMem[BsIndex].NumberOfPages) \
              );
        CurrentMemTypeInfo[Index].NumberOfPages = BsTypeMem[BsIndex].NumberOfPages;
        break;
      }
    }
    //
    // Add new memory type information if it doesn't exist.
    //
    if (Index == CurrentMemoryInfoCnt) {
      CurrentMemTypeInfo[Index].NumberOfPages = BsTypeMem[BsIndex].NumberOfPages;
      CurrentMemoryInfoCnt++;
    }
  }

  //
  // 3. Update MemoryTypeInformation variable first and then update MemoeryTypeUpdateInfo variable.
  //    Note: We MUST follow this sequence to (update MemoryTypeInformation first and then update MemoeryTypeUpdateInfo).
  //    If we don't follow this sequence, system may save incorrect MemoryTypeInformation variable data if power off system
  //    when MemoeryTypeUpdateInfo variable is set but MemoryTypeInformation variable isn't set.
  //
  if (IsMemoryTyepInfoValid (CurrentMemTypeInfo, CurrentMemoryInfoCnt)) {
    if (IsTotalMemoryTooLarge (CurrentMemTypeInfo, CurrentMemoryInfoCnt)){
      gBS->FreePool (UpdateInfo);
      gBS->FreePool (CurrentMemTypeInfo);
      ASSERT (FALSE);
      return EFI_ABORTED;
    }
    Status = gRT->SetVariable (
                    EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                    &gEfiMemoryTypeInformationGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    CurrentMemoryInfoCnt * sizeof (EFI_MEMORY_TYPE_INFORMATION),
                    CurrentMemTypeInfo
                    );
    ASSERT_EFI_ERROR (Status);
    Status = gRT->SetVariable (
                    L"MemoeryTypeUpdateInfo",
                    &gEfiMemoryTypeInformationGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    InfoVarSize,
                    UpdateInfo
                    );
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // Code should never enter this condition. We assert the code in debug mode. But for prevent from system hang
    // in normal mode, we clear MemoryTypeInformation and MemoeryTypeUpdateInfo variable and reset system
    //
    Status = gRT->SetVariable (
                    L"MemoeryTypeUpdateInfo",
                    &gEfiMemoryTypeInformationGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nClear MemoeryTypeUpdateInfo variable!!!\n"));
    Status = gRT->SetVariable (
                    EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                    &gEfiMemoryTypeInformationGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nClear MemoryTypeInformation variable!!!\n"));
    ASSERT (FALSE);
  }


  gBS->FreePool (UpdateInfo);
  gBS->FreePool (CurrentMemTypeInfo);
  return Status;
}

STATIC
EFI_STATUS
ClearUpdateMemoryInfoTimes (
  VOID
  )
/*++

Routine Description:

  Function uses to clear adjust memory times to zero.

Arguments:

Returns:

  EFI_SUCCESS  - Clear Update memory information times to zero successfully.
  Other        - Set MemoeryTypeUpdateInfo variable failed.

--*/
{

  UINT32                            *UpdateInfo;
  UINTN                             InfoVarSize;

  //
  // MemoeryTypeUpdateInfo variable format:
  //   UINT32        AdjustMemoryTimes;
  //   MEMORY_RANGE  SkipMemoryRanges[];
  //
  UpdateInfo = BdsLibGetVariableAndSize (
                L"MemoeryTypeUpdateInfo",
                &gEfiMemoryTypeInformationGuid,
                &InfoVarSize
                );
  if (UpdateInfo == NULL) {
    return EFI_SUCCESS;
  }

  *UpdateInfo = 0;
  return gRT->SetVariable (
                L"MemoeryTypeUpdateInfo",
                &gEfiMemoryTypeInformationGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                InfoVarSize,
                UpdateInfo
                );
}

VOID
CheckRtAndBsMemUsage (
  VOID
  )
/*++

Routine Description:

  Check the usage size of each runtime and boot services memory type.
  If the usage size exceeds the default size, adjust memory type information automatically.
  And then reset system.

Arguments:

Returns:

--*/
{
  EFI_MEMORY_DESCRIPTOR     *EfiMemoryMap;
  UINTN                     EfiMemoryMapSize;
  UINTN                     EfiDescriptorSize;
  EFI_LIST_ENTRY            SkipMemRangeList;
  EFI_STATUS                Status;
  EFI_BOOT_MODE             BootMode;
  STATIC BOOLEAN            MemoryChecked = FALSE;

  BdsLibGetBootMode (&BootMode);
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return;
  }

  if (MemoryChecked) {
    return;
  }

  EfiMemoryMap = GetMemoryMap (&EfiMemoryMapSize, &EfiDescriptorSize);
  if (EfiMemoryMap == NULL) {
    return;
  }
  CreateSkipMemRangeList (&SkipMemRangeList);

  if (IsBsOrRtMemoryOverUsage (EfiMemoryMap, EfiMemoryMapSize, EfiDescriptorSize, &SkipMemRangeList)) {
    Status = SetMemoryInfoVariable (EfiMemoryMap, EfiMemoryMapSize, EfiDescriptorSize, &SkipMemRangeList);
    if (!EFI_ERROR (Status)) {
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nSet memory information to variable failed!!!\n"));
    ASSERT (FALSE);
  } else {
    ClearUpdateMemoryInfoTimes ();
  }
  DestroySkipMemRangeList (&SkipMemRangeList);
  gBS->FreePool (EfiMemoryMap);
  MemoryChecked = TRUE;
}
#endif

#ifdef EFI_DEBUG
STATIC
BOOLEAN
IsRealMemory (
  IN  EFI_MEMORY_TYPE   Type
  )
{
  if (Type == EfiLoaderCode ||
      Type == EfiLoaderData ||
      Type == EfiBootServicesCode ||
      Type == EfiBootServicesData ||
      Type == EfiRuntimeServicesCode ||
      Type == EfiRuntimeServicesData ||
      Type == EfiConventionalMemory ||
      Type == EfiACPIReclaimMemory ||
      Type == EfiACPIMemoryNVS ||
      Type == EfiPalCode
      ) {
    return TRUE;
  }

  return FALSE;
}

VOID
DumpMemoryMap (
  VOID
  )
/*++

Routine Description:

  Dump memory information if EFI_DEBUG is enabled.

Arguments:

Returns:

--*/
{
  UINTN                             Index;
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMap;
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR             *EfiEntry;
  UINTN                             EfiMemoryMapSize;
  UINTN                             EfiDescriptorSize;
  UINT64                            MemoryBlockLength;
  UINT64                            NoPages[EfiMaxMemoryType];
  INT64                             TotalMemory;


  EfiMemoryMap = GetMemoryMap (&EfiMemoryMapSize, &EfiDescriptorSize);
  if (EfiMemoryMap == NULL) {
    return;
  }
  //
  // Calculate the system memory size from EFI memory map and resourceHob
  //
  EfiEntry        = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  EfiSetMem (NoPages, EfiMaxMemoryType * sizeof (UINT64) , 0);
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Dump Memory Map: \n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Type \t\t Start \t\t End \t\t #page \t Attributes\n"));
  while (EfiEntry < EfiMemoryMapEnd) {
    MemoryBlockLength = (UINT64) EFI_PAGES_TO_SIZE(EfiEntry->NumberOfPages);

    DEBUG (\
      (EFI_D_INFO | EFI_D_ERROR, "%s \t %10x \t %10x \t %x \t %x\n",\
      mMemoryTypeDesc[EfiEntry->Type],\
      EfiEntry->PhysicalStart,\
      EfiEntry->PhysicalStart + (UINTN) MemoryBlockLength - 1,\
      EfiEntry->NumberOfPages,\
      EfiEntry->Attribute)\
      );

    //
    // count pages of each type memory
    //
    NoPages[EfiEntry->Type] += EfiEntry->NumberOfPages;
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  //
  // Print each memory type summary
  //
  for (Index = 0, TotalMemory = 0; Index < EfiMaxMemoryType; Index += 1) {
    if (NoPages[Index]) {
      DEBUG (\
        (EFI_D_INFO | EFI_D_ERROR,"  %s: %8d Pages(%ld) \n",\
        mMemoryTypeDesc[Index],\
        NoPages[Index],\
        EFI_PAGES_TO_SIZE(NoPages[Index]))\
        );
      //
      // Count total memory
      //
      if (IsRealMemory ((EFI_MEMORY_TYPE) (Index))) {
        TotalMemory += NoPages[Index];
      }
    }
  }
  DEBUG (\
    (EFI_D_INFO | EFI_D_ERROR, "Total Memory: %d MB (%ld) Bytes \n",\
    RShiftU64 (TotalMemory, 8),\
    EFI_PAGES_TO_SIZE (TotalMemory))\
    );
}
#endif

