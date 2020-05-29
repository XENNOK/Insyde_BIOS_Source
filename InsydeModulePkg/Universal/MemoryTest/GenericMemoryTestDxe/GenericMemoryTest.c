/** @file
  Generic memory test Driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "GenericMemoryTest.h"

//
// Global:
// Since this driver will only ever produce one instance of the memory test
// protocol, so we do not need to dynamically allocate the PrivateData.
//
GENERIC_MEMORY_TEST_PRIVATE mGenericMemoryTestPrivate;
EFI_PHYSICAL_ADDRESS    mCurrentAddress;
LIST_ENTRY              *mCurrentLink;
NONTESTED_MEMORY_RANGE  *mCurrentRange;
UINT64                  mTestedSystemMemory;
UINT64                  mNonTestedSystemMemory;
BOOLEAN                 mReStartTest = FALSE;
EFI_PHYSICAL_ADDRESS    mCurrentWindowAddress;

static EFI_HANDLE       mSelfHandle;

UINT32                  GenericMemoryTestMonoPattern[GENERIC_CACHELINE_SIZE / 4] = {
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5,
  0x5a5a5a5a,
  0xa5a5a5a5
};

/**
  Construct the system DIMMs' information

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful construct system DIMMs' information.
  @retval EFI_OUT_OF_RESOURCE  Could not allocate needed resource in base memory.
  @retval other                Failed to get platform DIMMs information from.
                               platform memory test driver.

**/
EFI_STATUS
ConstructDIMMs (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS            Status;
  EFI_MEMTEST_DIMM_INFO DimmInfo;
  EFI_DIMM_DESCRIPTION  *DimmDsc;

  DimmInfo.DimmId = EFI_INVALID_DIMM_ID;
  Status = Private->PmtMemoryTest->GetNextDimmInfo (
                                     Private->PmtMemoryTest,
                                     &DimmInfo
                                     );
  do {
    DimmDsc = AllocatePool (sizeof (EFI_DIMM_DESCRIPTION));
    if (DimmDsc == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    ZeroMem (DimmDsc, sizeof (EFI_DIMM_DESCRIPTION));
    DimmDsc->Signature                = EFI_DIMM_DESCRIPTION_SIGNATURE;
    DimmDsc->DimmInfo                 = DimmInfo;
    DimmDsc->ReportRequired           = FALSE;
    DimmDsc->CorrectReportRequired    = FALSE;
    DimmDsc->UnCorrectReportRequired  = FALSE;
    DimmDsc->MapoutRequired           = FALSE;
    InsertTailList (&Private->DimmDscList, &DimmDsc->Link);

    Status = Private->PmtMemoryTest->GetNextDimmInfo (
                                       Private->PmtMemoryTest,
                                       &DimmInfo
                                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } while (!EFI_ERROR (Status));
  return EFI_SUCCESS;
}
/**
  Construct the system base memory range through GCD service.

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful construct the base memory range through GCD service.
  @retval EFI_OUT_OF_RESOURCE  Could not allocate needed resource from base memory.
  @retval Others               Failed to construct base memory range through GCD service.

**/
EFI_STATUS
ConstructBaseMemoryRange (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS                      Status;
  BASE_MEMORY_RANGE               *Range;
  UINTN                           NumberOfDescriptors;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap;
  UINTN                           Index;

  //
  // Base memory will always below 4G
  //
  Status = gDS->GetMemorySpaceMap (&NumberOfDescriptors, &MemorySpaceMap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < NumberOfDescriptors; Index++) {
    if (MemorySpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeSystemMemory) {
      Range = AllocatePool (sizeof (BASE_MEMORY_RANGE));
      if (Range == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Range->Signature    = EFI_BASE_MEMORY_RANGE_SIGNATURE;
      Range->StartAddress = MemorySpaceMap[Index].BaseAddress;
      Range->Length       = MemorySpaceMap[Index].Length;
      Range->Capabilities = MemorySpaceMap[Index].Capabilities;
      DEBUG ((EFI_D_INFO, "Base memory at %x", RShiftU64 (Range->StartAddress, 32)));
      DEBUG ((EFI_D_INFO, ",%x\n", Range->StartAddress));
      DEBUG ((EFI_D_INFO, "Base memory size %x\n", Range->Length));
      Private->BaseMemorySize += MemorySpaceMap[Index].Length;
      InsertTailList (&Private->BaseMemRanList, &Range->Link);
    }
  }

  return EFI_SUCCESS;
}

/**
  Construct the system non-tested memory range through GCD service

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful construct the non-tested memory range through GCD service.
  @retval EFI_OUT_OF_RESOURCE  Could not allocate needed resource from base memory.
  @retval Others               Failed to construct non-tested memory range through GCD service.

**/
EFI_STATUS
ConstructNonTestedMemoryRange (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS                      Status;
  NONTESTED_MEMORY_RANGE          *Range;
  BOOLEAN                         NoFound;
  UINTN                           NumberOfDescriptors;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap;
  UINTN                           Index;
  EFI_PHYSICAL_ADDRESS            Ia32Top;
  EFI_PHYSICAL_ADDRESS            MapWindow;
  EFI_PHYSICAL_ADDRESS            MapBoundary;

  //
  // Non tested memory range may be span 4G here
  //
  NoFound     = TRUE;
  Ia32Top     = IA32_MAX_LINEAR_ADDRESS;
  MapWindow   = 0;
  MapBoundary = 0x100000000LL;

  Status      = gDS->GetMemorySpaceMap (&NumberOfDescriptors, &MemorySpaceMap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < NumberOfDescriptors; Index++) {
    if (MemorySpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeReserved &&
        (MemorySpaceMap[Index].Capabilities & (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED)) ==
          (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED)
          ) {

      NoFound = FALSE;

      //
      // Separate the above 4G memory to special non tested memory range and
      // cut of it by MAPWINDOWS_SIZE
      //
      if ((MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length) > IA32_MAX_LINEAR_ADDRESS &&
          Private->Above4GMemoryAccess != NULL
          ) {
        //
        // Process the memory range which total above 4G or span 4G
        //
        if (MemorySpaceMap[Index].BaseAddress > IA32_MAX_LINEAR_ADDRESS) {
          //
          // Cut off the range by MAPWINDOWS_SIZE
          //
          MapBoundary = MemorySpaceMap[Index].BaseAddress;
          while (MapBoundary < (MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length)) {
            Range = AllocatePool (sizeof (NONTESTED_MEMORY_RANGE));
            if (Range == NULL) {
              return EFI_OUT_OF_RESOURCES;
            }

            Range->Signature    = EFI_NONTESTED_MEMORY_RANGE_SIGNATURE;
            Range->StartAddress = MapBoundary;
            if ((MapBoundary + MAPWINDOWS_SIZE) > (MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length)) {
              Range->Length = MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length - MapBoundary;
            } else {
              Range->Length = MAPWINDOWS_SIZE;
            }

            Range->Capabilities   = MemorySpaceMap[Index].Capabilities;
            Range->Above4G        = TRUE;
            Range->AlreadyMapped  = FALSE;
            InsertTailList (&Private->NonTestedMemRanList, &Range->Link);
            DEBUG ((EFI_D_INFO, "Non tested above 4G memory at %x", RShiftU64 (Range->StartAddress, 32)));
            DEBUG ((EFI_D_INFO, ",%x\n", Range->StartAddress));
            DEBUG ((EFI_D_INFO, "Non tested above 4G memory size %x\n", Range->Length));
            if ((MapBoundary + MAPWINDOWS_SIZE) > (MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length)) {
              goto Done;
            }

            MapBoundary += MAPWINDOWS_SIZE;
          }

Done:
          mNonTestedSystemMemory += MemorySpaceMap[Index].Length;

          return EFI_SUCCESS;

        } else {
          //
          // Separate the span 4G memory in two range, one below 4G the other
          // is above 4G
          //
          // Build the below 4G address to be
          //
          Range = AllocatePool (sizeof (NONTESTED_MEMORY_RANGE));
          if (Range == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }

          Range->Signature    = EFI_NONTESTED_MEMORY_RANGE_SIGNATURE;
          Range->StartAddress = MemorySpaceMap[Index].BaseAddress;
          if ((MapBoundary + MAPWINDOWS_SIZE) > (MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length)) {
            Range->Length = MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length - MapBoundary;
          } else {
            Range->Length = IA32_MAX_LINEAR_ADDRESS - MemorySpaceMap[Index].BaseAddress;
          }

          Range->Capabilities   = MemorySpaceMap[Index].Capabilities;
          Range->Above4G        = FALSE;
          Range->AlreadyMapped  = FALSE;
          InsertTailList (&Private->NonTestedMemRanList, &Range->Link);

          //
          // Cut off the above 4G memory
          //
          MapBoundary = Ia32Top + 1;
          while (MapBoundary < (MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length)) {
            Range = AllocatePool (sizeof (NONTESTED_MEMORY_RANGE));
            if (Range == NULL) {
              return EFI_OUT_OF_RESOURCES;
            }

            Range->Signature      = EFI_NONTESTED_MEMORY_RANGE_SIGNATURE;
            Range->StartAddress   = MapBoundary;
            Range->Length         = MAPWINDOWS_SIZE;
            Range->Capabilities   = MemorySpaceMap[Index].Capabilities;
            Range->Above4G        = TRUE;
            Range->AlreadyMapped  = FALSE;
            InsertTailList (&Private->NonTestedMemRanList, &Range->Link);

            if ((MapBoundary + MAPWINDOWS_SIZE) > (MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length)) {
              goto Done1;
            }

            MapBoundary += MAPWINDOWS_SIZE;
          }

Done1:
          mNonTestedSystemMemory += MemorySpaceMap[Index].Length;

          return EFI_SUCCESS;

        }
      }

      Range = AllocatePool (sizeof (NONTESTED_MEMORY_RANGE));
      if (Range == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Range->Signature      = EFI_NONTESTED_MEMORY_RANGE_SIGNATURE;
      Range->StartAddress   = MemorySpaceMap[Index].BaseAddress;
      Range->Length         = MemorySpaceMap[Index].Length;
      Range->Capabilities   = MemorySpaceMap[Index].Capabilities;
      Range->Above4G        = FALSE;
      Range->AlreadyMapped  = FALSE;
      DEBUG ((EFI_D_INFO, "Non tested below 4G memory at %x", RShiftU64 (Range->StartAddress, 32)));
      DEBUG ((EFI_D_INFO, ",%x\n", Range->StartAddress));
      DEBUG ((EFI_D_INFO, "Non tested below 4G memory size %x\n", Range->Length));
      mNonTestedSystemMemory += MemorySpaceMap[Index].Length;
      InsertTailList (&Private->NonTestedMemRanList, &Range->Link);
    }
  }

  if (NoFound) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Destroy the link list base on the correspond link list type

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] ListType          Indicate which type of the link list.

**/
VOID
DestroyLinkList (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  LINK_LIST_TYPE               ListType
  )
{
  LIST_ENTRY              *Link;
  LIST_ENTRY              *NextLink;
  LIST_ENTRY              *ListHead;
  EFI_DIMM_DESCRIPTION    *DimmDsc;
  BASE_MEMORY_RANGE       *BaseRange;
  NONTESTED_MEMORY_RANGE  *NontestedRange;

  Link            = NULL;
  ListHead        = NULL;
  DimmDsc         = NULL;
  BaseRange       = NULL;
  NontestedRange  = NULL;

  switch (ListType) {
  case DimmList:
    Link      = Private->DimmDscList.ForwardLink;
    ListHead  = &Private->DimmDscList;
    break;

  case BaseMemList:
    Link      = Private->BaseMemRanList.ForwardLink;
    ListHead  = &Private->BaseMemRanList;
    break;

  case NontestedMemList:
    Link      = Private->NonTestedMemRanList.ForwardLink;
    ListHead  = &Private->NonTestedMemRanList;
    break;
  }

  NextLink = Link->ForwardLink;

  while (Link != ListHead) {
    RemoveEntryList (Link);
    switch (ListType) {
    case DimmList:
      DimmDsc = EFI_DIMM_DESCRIPTION_FROM_LINK (Link);
      FreePool (DimmDsc);
      break;

    case BaseMemList:
      BaseRange = BASE_MEMORY_RANGE_FROM_LINK (Link);
      FreePool (BaseRange);
      break;

    case NontestedMemList:
      NontestedRange = NONTESTED_MEMORY_RANGE_FROM_LINK (Link);
      break;
    }

    Link      = NextLink;
    NextLink  = Link->ForwardLink;
  }
}

/**
  Re-construct the system non-tested memory range after some error DIMMs have
  been disabled

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful re-construct the non-tested memory range
                               through GCD service, then continue memory test.
  @retval EFI_NOT_FOUND        Could not find non-tested memory, indicate all the
                               memory have been tested.

**/
EFI_STATUS
ReConstructMemoryTest (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS  Status;

  //
  // reset the mReStartTest to false indicate the memory test restart
  //
  mReStartTest = FALSE;
  //
  // First destroied dimm description and NonTestedMemoryRange
  //
  if (Private->PmtMemoryTest != NULL) {
    DestroyLinkList (Private, DimmList);
  }

  DestroyLinkList (Private, NontestedMemList);

  //
  // re-construct the dimm description and NonTestedMemoryRange
  //
  if (Private->PmtMemoryTest != NULL) {
    ConstructDIMMs (Private);
  }

  mNonTestedSystemMemory  = 0;
  Status                  = ConstructNonTestedMemoryRange (Private);
  if (Status == EFI_NOT_FOUND) {
    return Status;
  }
  //
  // reset the test begin address
  //
  mCurrentLink    = Private->NonTestedMemRanList.ForwardLink;
  mCurrentRange   = NONTESTED_MEMORY_RANGE_FROM_LINK (mCurrentLink);
  mCurrentAddress = mCurrentRange->StartAddress;

  return EFI_SUCCESS;
}

/**
  Get the platform specific memory test capability provide by platform memory
  test driver

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful get the platform specific memory test
                               capability provide by platform memory test driver
  @retval Status               Could not find non-tested memory, indicate all the
                               memory have been tested

**/
EFI_STATUS
GetPlatformMemoryTestCAP (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS                        Status;
  EFI_PLATFORM_MEMTEST_CAPABILITIES Capabilities;
  EFI_MEMTEST_ADDRESS_PATTERN       AddressLinePattern;
  EFI_MEMTEST_DATA_PATTERN          DataLinePattern;
  EFI_MEMTEST_DATA_PATTERN          MemoryPattern;
  UINT32                            EntryCount;
  EFI_MEMTEST_POLICY_ENTRY          *PolicyGrid;

  Status = Private->PmtMemoryTest->GetPlatformInfo (
                                     Private->PmtMemoryTest,
                                     &Capabilities,
                                     &AddressLinePattern,
                                     &DataLinePattern,
                                     &MemoryPattern,
                                     &EntryCount,
                                     &PolicyGrid
                                     );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private->PmtCapability          = Capabilities;

  Private->PmtAddressLinePattern  = AddressLinePattern;
  Private->PmtDataLinePattern     = DataLinePattern;

  Private->PmtMemoryPattern       = MemoryPattern;

  Private->PolicyEntryCount       = EntryCount;
  Private->PolicyGrid             = PolicyGrid;

  return EFI_SUCCESS;
}

/**
  This function will remap the above 4G memory to below 4G

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] Above4GAddress    add argument and description to function comment.
  @param[in] MapSize           add argument and description to function comment.
  @param[in] WindowAddress     add argument and description to function comment.
  @param[in] WindowSize        add argument and description to function comment.

  @retval EFI_INVALID_PARAMETER add return value to function comment
  @retval EFI_SUCCESS           add return value to function comment

**/
EFI_STATUS
MemoryRangeMapping (
  IN  GENERIC_MEMORY_TEST_PRIVATE      *Private,
  IN  EFI_PHYSICAL_ADDRESS             Above4GAddress,
  IN  UINTN                            MapSize,
  IN OUT EFI_PHYSICAL_ADDRESS          *WindowAddress,
  IN  UINTN                            WindowSize
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  UINTN       PageSize;

  if (MapSize > WindowSize) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Hardcode the below 4G memory window for above 4G memory mapping use
  //
  *WindowAddress          = MAPWINDOWS_ADDRESS;
  Private->Below4GWindows = *WindowAddress;
  DEBUG ((EFI_D_INFO, "Success get map windows at %X, size is %x\n", (UINTN) (Private->Below4GWindows), WindowSize));

  //
  // Mapping the above 4G memory to below 4G memory window
  //
  Status = Private->Above4GMemoryAccess->GetPageSize (
                                           Private->Above4GMemoryAccess,
                                           &PageSize
                                           );

  Size = MapSize & (~(PageSize - 1));
  Status = Private->Above4GMemoryAccess->Map (
                                           Private->Above4GMemoryAccess,
                                           *WindowAddress,
                                           Size,
                                           Above4GAddress
                                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  This function will unmap the above 4G memory range which is in window

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] Above4GAddress    add argument and description to function comment.
  @param[in] MapSize           add argument and description to function comment.
  @param[in] WindowAddress     add argument and description to function comment.
  @param[in] WindowSize        add argument and description to function comment.

  @retval EFI_SUCCESS          add return value to function comment

**/
EFI_STATUS
MemoryRangeUnmapped (
  IN  GENERIC_MEMORY_TEST_PRIVATE      *Private,
  IN  EFI_PHYSICAL_ADDRESS             Above4GAddress,
  IN  UINTN                            MapSize,
  IN OUT EFI_PHYSICAL_ADDRESS          *WindowAddress,
  IN  UINTN                            WindowSize
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  UINTN       PageSize;

  //
  // Unmap the above 4G memory been tested and free the below 4G memory window
  //
  Status = Private->Above4GMemoryAccess->GetPageSize (
                                           Private->Above4GMemoryAccess,
                                           &PageSize
                                           );

  Size = MapSize & (~(PageSize - 1));
  Status = Private->Above4GMemoryAccess->UnMap (
                                           Private->Above4GMemoryAccess,
                                           Above4GAddress,
                                           Size
                                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Perform the address line and data line walking ones test

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful finished walking ones test
  @retval EFI_OUT_OF_RESOURCE  Could not get resource in base memory
  @retval EFI_ACCESS_DENIED    Code may can not run here because if walking one test
                               failed, system may be already halt

**/
EFI_STATUS
PerformAddressDataLineTest (
  IN  GENERIC_MEMORY_TEST_PRIVATE      *Private
  )
{
  EFI_STATUS              Status;
  UINT32                  Index;
  EFI_PHYSICAL_ADDRESS    *DataLineAddress;
  UINT32                  *DataLineAddressLow;
  UINT32                  *DataLineAddressHigh;
  UINT32                  *DataLinePattern;
  LIST_ENTRY              *ExtendedLink;
  NONTESTED_MEMORY_RANGE  *ExtendedRange;
  BOOLEAN                 InExtendedRange;
  EFI_PHYSICAL_ADDRESS    *TestAddress;
  EFI_PHYSICAL_ADDRESS    *TestAddressPtr;
  UINT64                  *AddressLinePattern;
  EFI_PHYSICAL_ADDRESS    WindowAddress;
  EFI_PHYSICAL_ADDRESS    *WindowAddressPtr;
  UINTN                   Index2;
  BOOLEAN                 WriteLoop;

  //
  // First perform the data line test
  //
  DEBUG ((EFI_D_INFO, "Perform data line walking ones\n"));

  DataLineAddress = AllocatePool (sizeof (EFI_PHYSICAL_ADDRESS));
  if (DataLineAddress == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DataLineAddressLow  = (UINT32 *) DataLineAddress;
  DataLineAddressHigh = (UINT32 *) DataLineAddress + 1;

  DataLinePattern     = Private->PmtDataLinePattern.Pattern;
  for (Index = 0; Index < Private->PmtDataLinePattern.Count; Index++) {
    *DataLineAddressLow = *DataLinePattern;
    if (*DataLineAddressLow != *DataLinePattern) {
      return EFI_ACCESS_DENIED;
    }

    DataLinePattern++;
  }

  DataLinePattern = Private->PmtDataLinePattern.Pattern;
  for (Index = 0; Index < Private->PmtDataLinePattern.Count; Index++) {
    *DataLineAddressHigh = *DataLinePattern;
    if (*DataLineAddressHigh != *DataLinePattern) {
      return EFI_ACCESS_DENIED;
    }

    DataLinePattern++;
  }
  //
  // Then perform the address line test
  //
  DEBUG ((EFI_D_INFO, "Perform address line walking ones\n"));

  //
  // Perform the address line test using platform driver provided pattern
  // First write all the address lines location,
  // then come back and verify the memory location.
  //
  for (Index2 = 0; Index2 < 2; Index2++) {
    WriteLoop = FALSE;
    if (Index2 == 0) {
      WriteLoop = TRUE;

    }

    TestAddressPtr      = Private->PmtAddressLinePattern.Address;
    AddressLinePattern  = Private->PmtAddressLinePattern.Pattern;

    for (Index = 0; Index < Private->PmtAddressLinePattern.Count; Index++) {

      TestAddress = (EFI_PHYSICAL_ADDRESS *) (UINT8 *) (UINTN) ((TestAddressPtr[Index]));
      //
      // only test if the address falls in the enabled range
      //
      InExtendedRange = FALSE;
      ExtendedLink    = Private->NonTestedMemRanList.BackLink;
      while (ExtendedLink != &Private->NonTestedMemRanList) {
        ExtendedRange = NONTESTED_MEMORY_RANGE_FROM_LINK (ExtendedLink);
        if (((EFI_PHYSICAL_ADDRESS) (UINTN) TestAddress >= ExtendedRange->StartAddress) &&
            ((EFI_PHYSICAL_ADDRESS) (UINTN) TestAddress < (ExtendedRange->StartAddress + ExtendedRange->Length))
            ) {
          InExtendedRange = TRUE;
        }

        ExtendedLink = ExtendedLink->BackLink;
      }

      if (InExtendedRange) {
        if (((UINTN) TestAddress) > (UINTN)IA32_MAX_LINEAR_ADDRESS) {
          //
          // This test is for > 4GB
          //
          Status = MemoryRangeMapping (Private, *TestAddress, MAX_PAGESIZE, &WindowAddress, MAX_PAGESIZE);
          if (EFI_ERROR (Status)) {
            return Status;
          }

          WindowAddressPtr = (EFI_PHYSICAL_ADDRESS *) (UINT8 *) (UINTN) WindowAddress;
          if (WriteLoop) {
            *WindowAddressPtr = AddressLinePattern[Index];
          } else {
            if (*WindowAddressPtr != AddressLinePattern[Index]) {
              Status = MemoryRangeUnmapped (Private, *TestAddress, MAX_PAGESIZE, &WindowAddress, MAX_PAGESIZE);
              if (EFI_ERROR (Status)) {
                return Status;
              }
              return EFI_ACCESS_DENIED;
            }
          }

          Status = MemoryRangeUnmapped (Private, *TestAddress, MAX_PAGESIZE, &WindowAddress, MAX_PAGESIZE);
          if (EFI_ERROR (Status)) {
            return Status;
          }
        } else {
          //
          // This test is for < 4GB
          //
          if (WriteLoop) {
            *TestAddress = AddressLinePattern[Index];
          } else {
            if (*TestAddress != AddressLinePattern[Index]) {
              return EFI_ACCESS_DENIED;
            }
          }
        }
      }
    }
  }

  return EFI_SUCCESS;

}

/**
  Any non-fate memory error occured, this function log the error in data hub

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful log error information in data hub

**/
EFI_STATUS
ActionReport (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  LIST_ENTRY                      *Link;
  EFI_DIMM_DESCRIPTION            *DimmDsc;
  EFI_MEMORY_EXTENDED_ERROR_DATA  *ExtendedErrorData;
  EFI_STATUS_CODE_DIMM_NUMBER     *DimmData;

  Link              = Private->DimmDscList.ForwardLink;
  ExtendedErrorData = NULL;
  DimmData          = NULL;

  while (Link != &Private->DimmDscList) {

    DimmDsc = EFI_DIMM_DESCRIPTION_FROM_LINK (Link);

    if (DimmDsc->ReportRequired) {

      ExtendedErrorData = AllocateZeroPool (sizeof (EFI_MEMORY_EXTENDED_ERROR_DATA));
      if (ExtendedErrorData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      ExtendedErrorData->DataHeader.HeaderSize  = sizeof (EFI_STATUS_CODE_DATA);
      ExtendedErrorData->DataHeader.Size        = sizeof (EFI_MEMORY_EXTENDED_ERROR_DATA) - sizeof (EFI_STATUS_CODE_DATA);
      ExtendedErrorData->DataHeader.Type        = gEfiStatusCodeSpecificDataGuid;
      ExtendedErrorData->Granularity            = EFI_MEMORY_ERROR_DEVICE;
      ExtendedErrorData->Operation              = EFI_MEMORY_OPERATION_READ;
      ExtendedErrorData->Syndrome               = 0x0;
      ExtendedErrorData->Address                = mCurrentAddress;
      ExtendedErrorData->Resolution             = 2;

      if (DimmDsc->CorrectReportRequired) {
        //
        // Report correctable errors
        //
        REPORT_STATUS_CODE_EX (
          EFI_ERROR_CODE | EFI_ERROR_MAJOR,
          EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_CORRECTABLE,
          0,
          &gEfiGenericMemTestProtocolGuid,
          NULL,
          (EFI_STATUS_CODE_DATA *) ExtendedErrorData,
          ExtendedErrorData->DataHeader.Size
          );
      }

      if (DimmDsc->UnCorrectReportRequired) {
        //
        // Report uncorrectable errors
        //
        REPORT_STATUS_CODE_EX (
          EFI_ERROR_CODE | EFI_ERROR_MAJOR,
          EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_UNCORRECTABLE,
          0,
          &gEfiGenericMemTestProtocolGuid,
          NULL,
          (EFI_STATUS_CODE_DATA *) ExtendedErrorData,
          ExtendedErrorData->DataHeader.Size
          );
      }

      FreePool (ExtendedErrorData);

      //
      // Report the error dimm status code
      //
      DimmData = AllocateZeroPool (sizeof (EFI_STATUS_CODE_DIMM_NUMBER));
      if (DimmData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      DimmData->DataHeader.HeaderSize = sizeof (EFI_STATUS_CODE_DATA);
      DimmData->DataHeader.Size       = sizeof (EFI_STATUS_CODE_DIMM_NUMBER) - sizeof (EFI_STATUS_CODE_DATA);
      DimmData->DataHeader.Type       = gEfiStatusCodeSpecificDataGuid;
      DimmData->Array                 = 0x0;
      DimmData->Device                = DimmDsc->DimmInfo.DimmId;
      REPORT_STATUS_CODE_EX (
        EFI_ERROR_CODE | EFI_ERROR_MAJOR,
        EFI_COMPUTING_UNIT_MEMORY | EFI_CU_EC_NON_SPECIFIC,
        0,
        &gEfiGenericMemTestProtocolGuid,
        NULL,
        (EFI_STATUS_CODE_DATA *) DimmData,
        DimmData->DataHeader.Size
        );
      FreePool (DimmData);
    }

    Link = Link->ForwardLink;
  }

  return EFI_SUCCESS;

}

/**
  When some fate memory error occured, this function will map out the error
  DIMMs and cause system reboot or memory test restart

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          add return value to function comment
  @retval EFI_OUT_OF_RESOURCES add return value to function comment
  @retval EFI_SUCCESS          add return value to function comment

**/
EFI_STATUS
ActionMapout (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS            Status;
  BOOLEAN               Overlap;
  BOOLEAN               RebootRequired;
  UINT16                DimmCount;
  EFI_DIMM_ID           *DimmIds;
  EFI_DIMM_ID           *IdIndex;
  LIST_ENTRY            *DimmLink;
  EFI_DIMM_DESCRIPTION  *DimmDsc;
  LIST_ENTRY            *BaseLink;
  BASE_MEMORY_RANGE     *BaseRange;

  Overlap         = FALSE;
  RebootRequired  = FALSE;
  DimmCount       = 0;

  //
  // Find out how many dimms of all currently and generate the buffer of
  // dimm ids which need to be disabled.
  //
  DimmLink = Private->DimmDscList.ForwardLink;
  while (DimmLink != &Private->DimmDscList) {
    DimmDsc = EFI_DIMM_DESCRIPTION_FROM_LINK (DimmLink);
    if (DimmDsc->MapoutRequired) {
      DimmCount++;
    }

    DimmLink = DimmLink->ForwardLink;
  }

  if (DimmCount == 0) {
    return EFI_SUCCESS;
  }

  DimmIds = AllocatePool (sizeof (EFI_DIMM_ID) * DimmCount);
  if (DimmIds == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (DimmIds, sizeof (EFI_DIMM_ID) * DimmCount);
  IdIndex = DimmIds;

  //
  // Generate the buffer of dimm list which need to be disabled and check if
  // it will cause the system reboot.
  //
  DimmLink = Private->DimmDscList.ForwardLink;
  while (DimmLink != &Private->DimmDscList) {
    DimmDsc = EFI_DIMM_DESCRIPTION_FROM_LINK (DimmLink);
    if (DimmDsc->MapoutRequired) {
      BaseLink = Private->BaseMemRanList.ForwardLink;
      while (BaseLink != &Private->BaseMemRanList) {
        BaseRange = BASE_MEMORY_RANGE_FROM_LINK (BaseLink);
        Status = Private->PmtMemoryTest->Overlap (
                                           Private->PmtMemoryTest,
                                           DimmDsc->DimmInfo.DimmId,
                                           BaseRange->StartAddress,
                                           BaseRange->Length,
                                           &Overlap
                                           );
        *IdIndex = DimmDsc->DimmInfo.DimmId;
        IdIndex++;
        if (Overlap) {
          RebootRequired = TRUE;
        }

        BaseLink = BaseLink->ForwardLink;
      }
    }

    DimmLink = DimmLink->ForwardLink;
  }
  //
  // First process the dimms which been disabled will cause the system reboot.
  // If first disable the dimms which will not caused system reboot, then
  // the dimm ids will changed after disable dimms and can not get right dimm
  // ids for next disable action.
  //
  if (RebootRequired) {
    Status = Private->PmtMemoryTest->DisableDimm (
                                       Private->PmtMemoryTest,
                                       &DimmCount,
                                       DimmIds,
                                       RebootRequired
                                       );
  } else {
    Status = Private->PmtMemoryTest->DisableDimm (
                                       Private->PmtMemoryTest,
                                       &DimmCount,
                                       DimmIds,
                                       FALSE
                                       );
    //
    // Perform here means the mapout action will not cause the system reboot,
    // the memory test will restart.
    //
    mReStartTest = TRUE;
  }

  return EFI_SUCCESS;

}

/**
  Handle the situation when memory error occured during memory test

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] ErrorInfoCount    The error information numbers
  @param[in] ErrorInfoBuffer   Point to the error information record buffer

**/
VOID
HandleErrors (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  UINTN                        ErrorInfoCount,
  IN  EFI_PLATFORM_MEMTEST_ERROR   *ErrorInfoBuffer
  )
{
  UINT32                      Index;
  EFI_PLATFORM_MEMTEST_ERROR  *ErrorBufferIndex;
  LIST_ENTRY                  *Link;
  EFI_DIMM_DESCRIPTION        *DimmDsc;
  EFI_MEMTEST_POLICY_ENTRY    *PolicyGridIndex;

  //
  // Update the error records of every dimm and free the ErrorInfoBuffer
  //
  ErrorBufferIndex = ErrorInfoBuffer;
  for (Index = 0; Index < ErrorInfoCount; Index++) {
    Link = Private->DimmDscList.ForwardLink;
    while (Link != &Private->DimmDscList) {
      DimmDsc = EFI_DIMM_DESCRIPTION_FROM_LINK (Link);
      if (ErrorBufferIndex->DimmId == DimmDsc->DimmInfo.DimmId) {
        switch ((UINTN)ErrorBufferIndex->Error) {
        case EfiMemtestCorrectableError:
          DimmDsc->CorrectableError++;
          break;

        case EfiMemtestUncorrectableError:
          DimmDsc->UncorrectableError++;
          break;
        }
      }

      Link = Link->ForwardLink;
    }

    ErrorBufferIndex++;
  }

  FreePool (ErrorInfoBuffer);

  //
  // Check every DIMM errors use the policy grid provided by platform
  // memory test driver. First check the report action
  //
  Link = Private->DimmDscList.ForwardLink;
  while (Link != &Private->DimmDscList) {
    DimmDsc         = EFI_DIMM_DESCRIPTION_FROM_LINK (Link);
    PolicyGridIndex = Private->PolicyGrid;
    for (Index = 0; Index < Private->PolicyEntryCount; Index++) {
      if ((
            (PolicyGridIndex->Action == EfiMemtestActionReport) &&
          (PolicyGridIndex->ErrorType == EfiMemtestCorrectableError)
        ) ||
          (
            (PolicyGridIndex->Action == EfiMemtestActionReport) &&
          (PolicyGridIndex->ErrorType == EfiMemtestUncorrectableError)
        )
          ) {
        DimmDsc->ReportRequired = TRUE;
        if (DimmDsc->CorrectableError > PolicyGridIndex->Threshold) {
          DimmDsc->CorrectReportRequired = TRUE;
        }

        if (DimmDsc->UncorrectableError > PolicyGridIndex->Threshold) {
          DimmDsc->UnCorrectReportRequired = TRUE;
        }
      }

      PolicyGridIndex++;
    }

    Link = Link->ForwardLink;
  }
  //
  // Second check disable Dimm action
  //
  Link = Private->DimmDscList.ForwardLink;
  while (Link != &Private->DimmDscList) {
    DimmDsc         = EFI_DIMM_DESCRIPTION_FROM_LINK (Link);
    PolicyGridIndex = Private->PolicyGrid;
    for (Index = 0; Index < Private->PolicyEntryCount; Index++) {
      if ((
            (PolicyGridIndex->Action == EfiMemtestActionDisableMem) &&
          (PolicyGridIndex->ErrorType == EfiMemtestCorrectableError) &&
          (DimmDsc->CorrectableError > PolicyGridIndex->Threshold)
        ) ||
          (
            (PolicyGridIndex->Action == EfiMemtestActionDisableMem) &&
          (PolicyGridIndex->ErrorType == EfiMemtestUncorrectableError) &&
          (DimmDsc->UncorrectableError > PolicyGridIndex->Threshold)
        )
          ) {
        DimmDsc->MapoutRequired = TRUE;
      }

      PolicyGridIndex++;
    }

    Link = Link->ForwardLink;
  }
  //
  // After caculate all the threshold, perform the actions
  //
  ActionReport (Private);
  ActionMapout (Private);

}

/**
  Initialize the ECC memory in software method

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] Start             The ECC memory range's start address
  @param[in] Size              The ECC memory range's size

  @retval EFI_SUCCESS          Successful init the ECC memory in software method

**/
EFI_STATUS
SoftECCInit (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  EFI_PHYSICAL_ADDRESS         Start,
  IN  UINT64                       Size
  )
{
  //
  // Just clear all ECC memory
  //
  ZeroMem ((VOID *) (UINTN) Start, (UINTN) Size);
  return EFI_SUCCESS;
}

/**
  Initialize the ECC memory in hardware assist method

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] Start             The ECC memory range's start address.
  @param[in] Size              The ECC memory range's size.

  @retval EFI_SUCCESS          Successful init the ECC memory in hardware assist method.
  @retval Other                Indicate the platform memory test driver failed to init ECC memory.

**/
EFI_STATUS
HardECCInit (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  EFI_PHYSICAL_ADDRESS         Start,
  IN  UINT64                       Size
  )
{
  EFI_STATUS                      Status;
  EFI_PHYSICAL_ADDRESS            Address;
  UINT64                          InitSize;
  UINTN                           Count;
  UINT64                          Remain;
  EFI_MEMORY_RANGE_EXTENDED_DATA  *RangeData;

  Address   = Start;
  InitSize  = Private->PmtCapability.InitBlockSize;
  Count     = 0;
  Remain    = 0;
  RangeData = NULL;

  while ((Address + InitSize) < (Start + Size)) {
    Status = Private->PmtMemoryTest->InitializeMem (
                                       Private->PmtMemoryTest,
                                       Address,
                                       InitSize
                                       );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Address += InitSize;
    Count++;
  }
  //
  // Report every memory range do the hard ware ECC init
  //
  RangeData                         = AllocateZeroPool (sizeof (EFI_MEMORY_RANGE_EXTENDED_DATA));
  if (RangeData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  RangeData->DataHeader.HeaderSize  = sizeof (EFI_STATUS_CODE_DATA);
  RangeData->DataHeader.Size        = sizeof (EFI_MEMORY_RANGE_EXTENDED_DATA) - sizeof (EFI_STATUS_CODE_DATA);
  RangeData->DataHeader.Type        = gEfiStatusCodeSpecificDataGuid;
  RangeData->Start                  = Address;
  RangeData->Length                 = InitSize;
  REPORT_STATUS_CODE_EX (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,
    0,
    &gEfiGenericMemTestProtocolGuid,
    NULL,
    (EFI_STATUS_CODE_DATA *) RangeData,
    RangeData->DataHeader.Size
    );

  Remain = Size - MultU64x32 (InitSize, (UINT32)Count);
  if (Remain != 0) {
    SoftECCInit (Private, (Start + Size - Remain), Remain);
  }

  return EFI_SUCCESS;
}

/**
  Write the memory test pattern into a range of physical memory

  @param[in] Private           Point to generic memory test driver's private data.
  @param[in] Start             The memory range's start address.
  @param[in] Size              The memory range's size.

  @retval EFI_SUCCESS          Successful write the test pattern into the non-tested memory.
  @retval Others               The test pattern may not really write into the physical memory.

**/
EFI_STATUS
WriteMemory (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  EFI_PHYSICAL_ADDRESS         Start,
  IN  UINT64                       Size
  )
{
  EFI_STATUS            Status;
  EFI_CPU_ARCH_PROTOCOL *Cpu;
  EFI_PHYSICAL_ADDRESS  Address;

  Address = Start;

  //
  // Add 4G memory address check for IA32 platform
  // NOTE: Without page table, there is no way to use memory above 4G.
  //
  if (Start + Size > MAX_ADDRESS) {
    return EFI_SUCCESS;
  }

  while (Address < (Start + Size)) {
    CopyMem ((VOID *) (UINTN) Address, Private->MonoPattern, Private->MonoTestSize);
    Address += Private->CoverageSpan;
  }
  //
  // bug bug: we may need GCD service to make the code cache and data uncache,
  // if GCD do not support it or return fail, then just flush the whole cache.
  //
  //
  // we may flush cache here to make sure it is really write to memory,
  // not just in cache, be aware not only flush cpu cache, also may need to
  // flush chipset cache
  //
  if (Private->PmtMemoryTest != NULL) {
    Status = Private->PmtMemoryTest->FlushBuffer (Private->PmtMemoryTest);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&Cpu);
  if (!EFI_ERROR (Status)) {
    Cpu->FlushDataCache (Cpu, Start, Size, EfiCpuFlushTypeWriteBackInvalidate);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Verify the range of physical memory which covered by memory test pattern.

  This function will also do not return any informatin just cause system reset,
  because the handle error encount fatal error and disable the bad DIMMs.

  @param[in] Private  Point to generic memory test driver's private data.
  @param[in] Start    The memory range's start address.
  @param[in] Size     The memory range's size.

  @retval EFI_SUCCESS Successful verify the range of memory, no errors' location found.
  @retval Others      The range of memory have errors contained.

**/
EFI_STATUS
VerifyMemory (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  EFI_PHYSICAL_ADDRESS         Start,
  IN  UINT64                       Size
  )
{
  EFI_STATUS                      Status;
  EFI_PHYSICAL_ADDRESS            Address;
  EFI_PHYSICAL_ADDRESS            ErrorAddress;
  INTN                            ErrorFound;
  UINT32                          ErrorInfoCount;
  EFI_PLATFORM_MEMTEST_ERROR      *ErrorInfoBuffer;
  BOOLEAN                         Overlap;
  BOOLEAN                         RebootRequired;
  UINT16                          DimmCount;
  UINTN                           Index;
  EFI_MEMORY_EXTENDED_ERROR_DATA  *ExtendedErrorData;

  ErrorFound        = 0;
  ErrorInfoCount    = 0;
  ErrorInfoBuffer   = NULL;
  Address           = Start;
  Overlap           = FALSE;
  RebootRequired    = FALSE;
  DimmCount         = 1;
  ExtendedErrorData = NULL;

  //
  // Add 4G memory address check for IA32 platform
  // NOTE: Without page table, there is no way to use memory above 4G.
  //
  if (Start + Size > MAX_ADDRESS) {
    return EFI_SUCCESS;
  }

  //
  // Use the software memory test to check whether have detected miscompare
  // error here. If there is miscompare error here then check if generic
  // memory test driver can disable the bad DIMM.
  //
  while (Address < (Start + Size)) {
    ErrorFound = CompareMem (
                   (VOID *) (UINTN) (Address),
                   Private->MonoPattern,
                   Private->MonoTestSize
                   );
    if (ErrorFound != 0) {
      ErrorAddress = Address + Private->MonoTestSize / 2;
      if (Private->PmtMemoryTest != NULL) {
        Status = Private->PmtMemoryTest->LocateBadDimm (
                                           Private->PmtMemoryTest,
                                           ErrorAddress,
                                           *(CHAR8 *) ((UINTN) Private->MonoPattern + Private->MonoTestSize / 2),
                                           (UINT8) ErrorFound,
                                           &ErrorInfoCount,
                                           &ErrorInfoBuffer
                                           );
        //
        // If platform memory test driver do not support LocateBadDimm, it will
        // return EFI_UNSUPPORTED.
        //
        if (!EFI_ERROR (Status)) {
          //
          // Mark all Miscompare errors as uncorrectable.
          //
          for (Index = 0; Index < ErrorInfoCount; Index++) {
            ErrorInfoBuffer[Index].Error = EfiMemtestUncorrectableError;
          }

          HandleErrors (Private, ErrorInfoCount, ErrorInfoBuffer);
        } else {
          //
          // Report uncorrectable errors
          //
          ExtendedErrorData = AllocateZeroPool (sizeof (EFI_MEMORY_EXTENDED_ERROR_DATA));
          if (ExtendedErrorData == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }

          ExtendedErrorData->DataHeader.HeaderSize  = sizeof (EFI_STATUS_CODE_DATA);
          ExtendedErrorData->DataHeader.Size        = sizeof (EFI_MEMORY_EXTENDED_ERROR_DATA) - sizeof (EFI_STATUS_CODE_DATA);
          ExtendedErrorData->DataHeader.Type        = gEfiStatusCodeSpecificDataGuid;
          ExtendedErrorData->Granularity            = EFI_MEMORY_ERROR_DEVICE;
          ExtendedErrorData->Operation              = EFI_MEMORY_OPERATION_READ;
          ExtendedErrorData->Syndrome               = 0x0;
          ExtendedErrorData->Address                = ErrorAddress;
          ExtendedErrorData->Resolution             = 0x40;
          REPORT_STATUS_CODE_EX (
            EFI_ERROR_CODE | EFI_ERROR_MAJOR,
            EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_UNCORRECTABLE,
            0,
            &gEfiGenericMemTestProtocolGuid,
            NULL,
            (EFI_STATUS_CODE_DATA *) ExtendedErrorData,
            ExtendedErrorData->DataHeader.Size
            );
          return Status;
        }
      }
    }

    Address += Private->CoverageSpan;
  }
  //
  // If no miscompare error, we may need to check the ECC error if platform
  // memory test driver available
  //
  if (Private->PmtMemoryTest != NULL && (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) != 0) {
    Status = Private->PmtMemoryTest->CheckError (
                                       Private->PmtMemoryTest,
                                       &ErrorInfoCount,
                                       &ErrorInfoBuffer
                                       );
    if (!EFI_ERROR (Status) && ErrorInfoCount != 0) {
      //
      // ECC error can be correctable.
      //
      for (Index = 0; Index < ErrorInfoCount; Index++) {
        ErrorInfoBuffer[Index].Error = EfiMemtestCorrectableError;
      }

      HandleErrors (Private, ErrorInfoCount, ErrorInfoBuffer);
    }

    Status = Private->PmtMemoryTest->ClearError (Private->PmtMemoryTest);
  }

  return EFI_SUCCESS;
}

/**
  Add the extened memory to whole system memory map

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful add all the extended memory to system memory map.
  @retval Ohters               Failed to add the tested extended memory.

**/
EFI_STATUS
UpdateMemoryMap (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY             *Link;
  NONTESTED_MEMORY_RANGE  *Range;

  Link = Private->NonTestedMemRanList.ForwardLink;

  while (Link != &Private->NonTestedMemRanList) {
    Range = NONTESTED_MEMORY_RANGE_FROM_LINK (Link);
    DEBUG ((EFI_D_INFO, "Add memory at %x", RShiftU64 (Range->StartAddress, 32)));
    DEBUG ((EFI_D_INFO, ",%x\n", Range->StartAddress));
    DEBUG ((EFI_D_INFO, "Add memory size %x\n", Range->Length));

    Status = gDS->RemoveMemorySpace (
                    Range->StartAddress,
                    Range->Length
                    );

    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeSystemMemory,
                    Range->StartAddress,
                    Range->Length,
                    Range->Capabilities &~(EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED | EFI_MEMORY_RUNTIME)
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Link = Link->ForwardLink;
  }

  return EFI_SUCCESS;
}

/**
  Test a range of the memory directly

  @param[in] Private           Point to generic memory test driver's private data.

  @retval EFI_SUCCESS          Successful test the range of memory.
  @retval Others               Failed to test the range of memory.

**/
EFI_STATUS
DirectRangeTest (
  IN  GENERIC_MEMORY_TEST_PRIVATE  *Private,
  IN  EFI_PHYSICAL_ADDRESS         StartAddress,
  IN  UINT64                       Length,
  IN  UINT64                       Capabilities
  )
{
  EFI_STATUS  Status;
  //
  // Used for performance improvement if QuickBoot enabled
  //
  UINTN       DataSize;
  UINT8       QuickBootSetting;

  DataSize = sizeof (QuickBootSetting);
  Status = gRT->GetVariable (
                  L"QuickBoot",
                  &gEfiGenericVariableGuid,
                  NULL,
                  &DataSize,
                  &QuickBootSetting
                  );
  //
  // QuickBoot variable exists and QuickBoot is disabled, then do memory test
  //
  if (!EFI_ERROR (Status) && (QuickBootSetting == 0)) {
    //
    // Perform a dummy memory test, so directly write the pattern to all range
    //
    Status = WriteMemory (Private, StartAddress, Length);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Verify the memory range
    //
    Status = VerifyMemory (Private, StartAddress, Length);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  //
  // Add the tested compatible memory to system memory using GCD service
  //
  Status = gDS->RemoveMemorySpace (
                  StartAddress,
                  Length
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeSystemMemory,
                  StartAddress,
                  Length,
                  Capabilities &~(EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED | EFI_MEMORY_RUNTIME)
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Initialize the generic memory test.

  @param[in]  This                The protocol instance pointer.
  @param[in]  Level               The coverage level of the memory test.
  @param[out] RequireSoftECCInit  Indicate if the memory need software ECC init.

  @retval EFI_SUCCESS         The generic memory test is initialized correctly.
  @retval EFI_NO_MEDIA        The system had no memory to be tested.

**/
EFI_STATUS
EFIAPI
InitializeMemoryTest (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL          *This,
  IN  EXTENDMEM_COVERAGE_LEVEL                 Level,
  OUT BOOLEAN                                  *RequireSoftECCInit
  )
{
  EFI_STATUS                          Status;
  GENERIC_MEMORY_TEST_PRIVATE         *Private;
  EFI_PLATFORM_MEMTEST_PROTOCOL       *PmtMemoryTest;
  EFI_VIRTUAL_MEMORY_ACCESS_PROTOCOL  *Above4GAccess;

  Private             = GENERIC_MEMORY_TEST_PRIVATE_FROM_THIS (This);
  *RequireSoftECCInit = FALSE;

  //
  // This is initialize for default value, but some value may be reset base on
  // platform memory test driver.
  //
  Private->CoverLevel   = Level;
  Private->BdsBlockSize = TEST_BLOCK_SIZE;
  Private->MonoPattern  = GenericMemoryTestMonoPattern;
  Private->MonoTestSize = GENERIC_CACHELINE_SIZE;

  //
  // Initialize several internal link list
  //
  InitializeListHead (&Private->DimmDscList);
  InitializeListHead (&Private->BaseMemRanList);
  InitializeListHead (&Private->NonTestedMemRanList);

  //
  // Construct base memory range
  //
  Status = ConstructBaseMemoryRange (Private);

  //
  // we may need above 4G memory access
  //
  Status = gBS->LocateProtocol (
                  &gEfiVirtualMemoryAccessProtocolGuid,
                  NULL,
                  (VOID **)&Above4GAccess
                  );
  if (!EFI_ERROR (Status)) {
    Private->Above4GMemoryAccess = Above4GAccess;
  }
  //
  // we need to get platform memory test driver protocol here
  //
  Status = gBS->LocateProtocol (
                  &gEfiPlatformMemTestGuid,
                  NULL,
                  (VOID **)&PmtMemoryTest
                  );

  if (!EFI_ERROR (Status)) {
    Private->PmtMemoryTest = PmtMemoryTest;
  }
  //
  // Check what capabilities platform memory test driver provide and reset
  // some default value used for perform memory test
  //
  if (Private->PmtMemoryTest != NULL) {
    //
    // Get the platform memory test driver provided capability, it's indicate
    // what the platform memory test can support for generic memory test
    //
    Status = GetPlatformMemoryTestCAP (Private);

    //
    // Reset the Monopattern and Monotestsize which used by generic
    // memory test driver to R/W/V memory
    //
    Private->MonoPattern  = Private->PmtMemoryPattern.Pattern;
    Private->MonoTestSize = Private->PmtMemoryPattern.Count * Private->PmtMemoryPattern.Width;

    //
    // Construct the platform specific DIMM description information, this
    // information can trace every error detected out during memory test.
    //
    Status = ConstructDIMMs (Private);

    //
    // Here to notify platform memory test driver test begin.
    //
    Status = Private->PmtMemoryTest->NotifyPhase (
                                       Private->PmtMemoryTest,
                                       EfiMemtestPhaseBegin
                                       );
  }
  //
  // Create the CoverageSpan of the memory test base on the coverage level
  //
  switch (Private->CoverLevel) {
  case EXTENSIVE:
    if (Private->PmtMemoryTest != NULL) {
      Private->CoverageSpan = Private->MonoTestSize;
    } else {
      Private->CoverageSpan = GENERIC_CACHELINE_SIZE;
    }
    break;

  case SPARSE:
    Private->CoverageSpan = SPARSE_SPAN_SIZE;
    break;

  case QUICK:
    Private->CoverageSpan = QUICK_SPAN_SIZE;
    break;

  case IGNORE:
    //
    // Even the BDS do not need to test any memory, but in some case it
    // still need to init ECC memory.
    //
    Private->CoverageSpan = QUICK_SPAN_SIZE;
    break;

  default:
    Private->CoverageSpan = QUICK_SPAN_SIZE;
    break;
  }
  //
  // This is the first time we construct the non-tested memory range, if no
  // extended memory found, we know the system have not any extended memory
  // need to be test
  //
  Status = ConstructNonTestedMemoryRange (Private);
  if (Status == EFI_NOT_FOUND) {
    return EFI_NO_MEDIA;
  }
  //
  // ready to perform the R/W/V memory test
  //
  if (!EFI_ERROR (Status)) {

    mTestedSystemMemory = Private->BaseMemorySize;
    mCurrentLink        = Private->NonTestedMemRanList.ForwardLink;
    mCurrentRange       = NONTESTED_MEMORY_RANGE_FROM_LINK (mCurrentLink);
    mCurrentAddress     = mCurrentRange->StartAddress;

    //
    // Check if the platform support ECC memory and hardware assist ECC init,
    // if so, call the hardware assist ECC init and indicate do not need soft
    // assist ECC init
    //
    if (Private->PmtMemoryTest != NULL &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) != 0 &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_HW_INIT) != 0
        ) {
      HardECCInit (Private, mCurrentRange->StartAddress, mCurrentRange->Length);
    }
    //
    // Check if the memory need software assist initialization, if the
    // platform do not support hardware assist ECC init, make assumption need soft
    // ECC init.
    //
    if (Private->PmtMemoryTest != NULL &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) != 0 &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_HW_INIT) == 0
        ) {
      *RequireSoftECCInit = TRUE;
    }
    //
    // If the plaform do not support ECC memory, no venture performing
    // the walking ones test here
    //
    if (Private->PmtMemoryTest != NULL && (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) == 0) {
      PerformAddressDataLineTest (Private);
    }

  } else {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Perform the memory test.

  @param[in]  This              The protocol instance pointer.
  @param[out] TestedMemorySize  Return the tested extended memory size.
  @param[out] TotalMemorySize   Return the whole system physical memory size.
                                The total memory size does not include memory in a slot with a disabled DIMM.
  @param[out] ErrorOut          TRUE if the memory error occured.
  @param[in]  IfTestAbort       Indicates that the user pressed "ESC" to skip the memory test.

  @retval EFI_SUCCESS         One block of memory passed the test.
  @retval EFI_NOT_FOUND       All memory blocks have already been tested.
  @retval EFI_DEVICE_ERROR    Memory device error occured, and no agent can handle it.

**/
EFI_STATUS
EFIAPI
GenPerformMemoryTest (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL          *This,
  OUT UINT64                                   *TestedMemorySize,
  OUT UINT64                                   *TotalMemorySize,
  OUT BOOLEAN                                  *ErrorOut,
  IN BOOLEAN                                   TestAbort
  )
{
  EFI_STATUS                      Status;
  GENERIC_MEMORY_TEST_PRIVATE     *Private;
  UINT64                          BlockBoundary;
  UINTN                           ErrorInfoCount;
  EFI_PLATFORM_MEMTEST_ERROR      *ErrorInfoBuffer;
  EFI_MEMORY_RANGE_EXTENDED_DATA  *RangeData;

  Private       = GENERIC_MEMORY_TEST_PRIVATE_FROM_THIS (This);
  *ErrorOut     = FALSE;
  BlockBoundary = 0;
  RangeData     = NULL;

  if (mReStartTest) {
    Status = ReConstructMemoryTest (Private);
    if (Status == EFI_NOT_FOUND) {
      return Status;
    }
  }
  //
  // In extensive mode the boundary of "mCurrentRange->Length" may will lost
  // some range that is not Private->BdsBlockSize size boundry, so need
  // the software mechanism to confirm all memory location be covered.
  //
  if (mCurrentAddress < (mCurrentRange->StartAddress + mCurrentRange->Length)) {
    if ((mCurrentAddress + Private->BdsBlockSize) <= (mCurrentRange->StartAddress + mCurrentRange->Length)) {
      BlockBoundary = Private->BdsBlockSize;
    } else {
      BlockBoundary = mCurrentRange->StartAddress + mCurrentRange->Length - mCurrentAddress;
    }
    //
    // Report status code of every memory range
    //
    RangeData                         = AllocateZeroPool (sizeof (EFI_MEMORY_RANGE_EXTENDED_DATA));
    if (RangeData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    RangeData->DataHeader.HeaderSize  = sizeof (EFI_STATUS_CODE_DATA);
    RangeData->DataHeader.Size        = sizeof (EFI_MEMORY_RANGE_EXTENDED_DATA) - sizeof (EFI_STATUS_CODE_DATA);
    RangeData->DataHeader.Type        = gEfiStatusCodeSpecificDataGuid;
    RangeData->Start                  = mCurrentAddress;
    RangeData->Length                 = BlockBoundary;

    REPORT_STATUS_CODE_EX (
      EFI_PROGRESS_CODE,
      EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TEST,
      0,
      &gEfiGenericMemTestProtocolGuid,
      NULL,
      (EFI_STATUS_CODE_DATA *) RangeData,
      RangeData->DataHeader.Size
      );

    //
    // If there have been hardware assist memory test, we do not need perform
    // software (R/W/V) memory test.
    // Need think about software mechanism to HwTest non Private->BdsBlockSize
    // boundary memory's init
    //
    if (Private->PmtMemoryTest != NULL && (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_HW_TEST) != 0) {
      if (mCurrentRange->AlreadyMapped) {
        Status = Private->PmtMemoryTest->HwTest (
                                           Private->PmtMemoryTest,
                                           Private->CoverLevel,
                                           mCurrentWindowAddress,
                                           mCurrentRange->Length,
                                           &ErrorInfoCount,
                                           &ErrorInfoBuffer
                                           );
        if (!EFI_ERROR (Status)) {
          HandleErrors (Private, ErrorInfoCount, ErrorInfoBuffer);
          goto TestDone;
        }
      } else {
        Status = Private->PmtMemoryTest->HwTest (
                                           Private->PmtMemoryTest,
                                           Private->CoverLevel,
                                           mCurrentRange->StartAddress,
                                           mCurrentRange->Length,
                                           &ErrorInfoCount,
                                           &ErrorInfoBuffer
                                           );
        if (!EFI_ERROR (Status)) {
          HandleErrors (Private, ErrorInfoCount, ErrorInfoBuffer);
          goto TestDone;
        }
      }
    }
    //
    // Check whether the platform support ECC memory and need to be init by
    // generic memory test driver using the software method. If so, then
    // perform the software ECC init.
    //
    if (Private->PmtMemoryTest != NULL &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) != 0 &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_HW_INIT) == 0
        ) {
      if (mCurrentRange->AlreadyMapped) {
        SoftECCInit (Private, mCurrentWindowAddress, BlockBoundary);
      } else {
        SoftECCInit (Private, mCurrentAddress, BlockBoundary);
      }
    }
    //
    // If TestAbort is true, means user cancel the memory test, but may
    // be the ECC memory also need init.
    //
    if (!TestAbort && Private->CoverLevel != IGNORE) {
      //
      // The software memory test (R/W/V) perform here. It will detect the
      // memory mis-compare error.
      //
      DEBUG ((EFI_D_INFO, "Write begin at %x", LShiftU64 (mCurrentAddress, 32)));
      DEBUG ((EFI_D_INFO, ",%x\n", mCurrentAddress));
      DEBUG ((EFI_D_INFO, "BlockBoundary is 0x%x \n", BlockBoundary));
      if (mCurrentRange->AlreadyMapped) {
        Status = WriteMemory (Private, mCurrentWindowAddress, BlockBoundary);
      } else {
        Status = WriteMemory (Private, mCurrentAddress, BlockBoundary);
      }

      if (mCurrentRange->AlreadyMapped) {
        Status = VerifyMemory (Private, mCurrentWindowAddress, BlockBoundary);
      } else {
        Status = VerifyMemory (Private, mCurrentAddress, BlockBoundary);
      }

      DEBUG ((EFI_D_INFO, "Verify begin at %x", RShiftU64 (mCurrentAddress, 32)));
      DEBUG ((EFI_D_INFO, ",%x\n", mCurrentAddress));
      DEBUG ((EFI_D_INFO, "BlockBoundary is 0x%x \n", BlockBoundary));
      if (EFI_ERROR (Status)) {
        //
        // If perform here, means there is mis-compare error, and no agent can
        // handle it, so we return to BDS EFI_DEVICE_ERROR.
        //
        *ErrorOut = TRUE;
        return EFI_DEVICE_ERROR;
      }
    }

    mTestedSystemMemory += BlockBoundary;
    *TestedMemorySize = mTestedSystemMemory;

    //
    // If the memory test restart after the platform driver disable dimms,
    // the NonTestSystemMemory may be changed, but the base memory size will
    // not changed, so we can get the current total memory size.
    //
    *TotalMemorySize = Private->BaseMemorySize + mNonTestedSystemMemory;

    //
    // Update the current test address pointing to next BDS BLOCK
    //
    mCurrentWindowAddress += Private->BdsBlockSize;
    mCurrentAddress += Private->BdsBlockSize;

    return EFI_SUCCESS;
  }

TestDone:
  //
  // All current range memory test finished, if have above 4G memory mapped,
  // then unmapped here
  //
  if (mCurrentAddress >= (mCurrentRange->StartAddress + mCurrentRange->Length)) {
    if (mCurrentRange->Above4G && mCurrentRange->AlreadyMapped) {
      Status = MemoryRangeUnmapped (
                 Private,
                 mCurrentRange->StartAddress,
                 (UINTN) mCurrentRange->Length,
                 &(Private->Below4GWindows),
                 MAPWINDOWS_SIZE
                 );
      if (!EFI_ERROR (Status)) {
        mCurrentRange->AlreadyMapped  = FALSE;
        mCurrentWindowAddress         = 0;
      } else {
        return Status;
      }
    }
  }
  //
  // Change to next non tested memory range
  //
  mCurrentLink = mCurrentLink->ForwardLink;
  if (mCurrentLink != &Private->NonTestedMemRanList) {
    mCurrentRange   = NONTESTED_MEMORY_RANGE_FROM_LINK (mCurrentLink);
    mCurrentAddress = mCurrentRange->StartAddress;

    //
    // Remap the above 4G memory here
    //
    if (mCurrentRange->Above4G && !mCurrentRange->AlreadyMapped) {
      Status = MemoryRangeMapping (
                 Private,
                 mCurrentRange->StartAddress,
                 (UINTN) mCurrentRange->Length,
                 &mCurrentWindowAddress,
                 MAPWINDOWS_SIZE
                 );
      if (!EFI_ERROR (Status)) {
        mCurrentRange->AlreadyMapped = TRUE;
      } else {
        return Status;
      }
    }
    //
    // Here also need to check if the platform support ECC memory and hardware
    // assist ECC init. If so, also call the hardware assist ECC init.
    //
    if (Private->PmtMemoryTest != NULL &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) != 0 &&
        (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_HW_INIT) != 0
        ) {
      if (mCurrentRange->AlreadyMapped) {
        HardECCInit (Private, mCurrentWindowAddress, mCurrentRange->Length);
      } else {
        HardECCInit (Private, mCurrentRange->StartAddress, mCurrentRange->Length);
      }
    }

    return EFI_SUCCESS;
  } else {
    //
    // Here means all the memory test have finished
    //
    *TestedMemorySize = mTestedSystemMemory;
    *TotalMemorySize  = Private->BaseMemorySize + mNonTestedSystemMemory;
    return EFI_NOT_FOUND;
  }

}

/**
  Finish the memory test.

  @param[in] This             The protocol instance pointer.

  @retval EFI_SUCCESS         Success. All resources used in the memory test are freed.

**/
EFI_STATUS
EFIAPI
GenMemoryTestFinished (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL *This
  )
{
  EFI_STATUS                  Status;
  GENERIC_MEMORY_TEST_PRIVATE *Private;
  EFI_HANDLE                  Handle;
  VOID                        *Instance;

  Private = GENERIC_MEMORY_TEST_PRIVATE_FROM_THIS (This);
  Handle  = NULL;

  //
  // If the plaform support ECC memory, at this time ECC init must already be
  // finished, no venture performing the walking ones test here
  //
  if (Private->PmtMemoryTest != NULL && (Private->PmtCapability.HwCap & EFI_MEMTEST_CAP_ECC) != 0) {
    PerformAddressDataLineTest (Private);
  }

  if (mCurrentRange->Above4G && mCurrentRange->AlreadyMapped) {
     Status = MemoryRangeUnmapped (
                Private,
                mCurrentRange->StartAddress,
                (UINTN) mCurrentRange->Length,
                &(Private->Below4GWindows),
                MAPWINDOWS_SIZE
                );
      if (!EFI_ERROR (Status)) {
        mCurrentRange->AlreadyMapped  = FALSE;
        mCurrentWindowAddress         = 0;
      } else {
        return Status;
      }
  }


  //
  // Add the non tested memory range to system memory map through GCD service
  //
  Status = UpdateMemoryMap (Private);

  //
  // we need to free all the memory allocate and notify platform memory test
  // driver finish memory test here
  //
  if (Private->PmtMemoryTest != NULL) {
    DestroyLinkList (Private, DimmList);
    Status = Private->PmtMemoryTest->NotifyPhase (
                                       Private->PmtMemoryTest,
                                       EfiMemtestPhaseEnd
                                       );
  }

  DestroyLinkList (Private, NontestedMemList);
  DestroyLinkList (Private, BaseMemList);

  //
  // Make sure all the compatible memory have been tested and public
  // the gEfiCompatibleMemoryTestedGuid protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiCompatibleMemoryTestedGuid,
                  NULL,
                  (VOID **)&Instance
                  );
  if (EFI_ERROR (Status)) {
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiCompatibleMemoryTestedGuid,
                    EFI_NATIVE_INTERFACE,
                    (VOID *) NULL
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Provides the capability to test the compatible range used by some special drivers.

  @param[in]  This              The protocol instance pointer.
  @param[in]  StartAddress      The start address of the compatible memory range that
                                must be below 16M.
  @param[in]  Length            The compatible memory range's length.

  @retval EFI_SUCCESS           The compatible memory range pass the memory test.
  @retval EFI_INVALID_PARAMETER The compatible memory range are not below Low 16M.

**/
EFI_STATUS
EFIAPI
GenCompatibleRangeTest (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL         *This,
  IN EFI_PHYSICAL_ADDRESS                     StartAddress,
  IN UINT64                                   Length
  )
{
  EFI_STATUS                      Status;
  GENERIC_MEMORY_TEST_PRIVATE     *Private;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR Descriptor;
  BOOLEAN                         InRange;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR NextDescriptor;

  Private = GENERIC_MEMORY_TEST_PRIVATE_FROM_THIS (This);

  InRange = FALSE;

  //
  // Check if the parameter is below 16MB
  //
  if (StartAddress >= 0x1000000 && StartAddress + Length > 0x1000000) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check the required memory range status; if the required memory range span
  // the different GCD memory descriptor, it may be cause different action.
  //
  Status = gDS->GetMemorySpaceDescriptor (
                  StartAddress,
                  &Descriptor
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((StartAddress + Length) <= (Descriptor.BaseAddress + Descriptor.Length)) {
    InRange = TRUE;
  }

  if (InRange) {
    if (Descriptor.GcdMemoryType == EfiGcdMemoryTypeReserved &&
        (Descriptor.Capabilities & (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED)) ==
          (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED)
          ) {
      Status = DirectRangeTest (Private, StartAddress, Length, Descriptor.Capabilities);
      //
      // In this case, whenever the test success we just return the Status.
      // Because the required range all in the current GCD discriptor, if
      // the test success, then just return the Status with EFI_SUCCESS.
      //
      return Status;
    }
  } else {
    //
    // First test the range already in the current GCD memory descriptor
    //
    if (Descriptor.GcdMemoryType == EfiGcdMemoryTypeReserved &&
        (Descriptor.Capabilities & (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED)) ==
          (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED)
          ) {
      Status = DirectRangeTest (
                Private,
                StartAddress,
                (Descriptor.BaseAddress + Descriptor.Length - StartAddress),
                Descriptor.Capabilities
                );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // Test the memory range which out of the StartAddress remained
    // in GCD memory descriptor.
    //
    Status = gDS->GetMemorySpaceDescriptor (
                    Descriptor.BaseAddress + Descriptor.Length,
                    &NextDescriptor
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (NextDescriptor.GcdMemoryType == EfiGcdMemoryTypeReserved &&
        (NextDescriptor.Capabilities & (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED)) ==
          (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED)
          ) {
      Status = DirectRangeTest (
                 Private,
                 NextDescriptor.BaseAddress,
                 StartAddress + Length - NextDescriptor.BaseAddress,
                 NextDescriptor.Capabilities
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
  //
  // Here means the required range already be tested, so just return success.
  //
  return EFI_SUCCESS;
}

/**
  The generic memory test driver's entry point.

  It initializes private data to default value.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval EFI_NOT_FOUND   Can't find HandOff Hob in HobList.
  @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
GenericMemoryTestEntryPoint (
  IN  EFI_HANDLE           ImageHandle,
  IN  EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                      Status;
  VOID                            *HobList;
  EFI_BOOT_MODE                   BootMode;
  EFI_PEI_HOB_POINTERS            Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR     *ResourceMemoryHob;
  UINT64                          Length;
  EFI_PHYSICAL_ADDRESS            BaseAddress;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR GcdDescriptor;


  mSelfHandle = ImageHandle;

  ZeroMem (&mGenericMemoryTestPrivate, sizeof (GENERIC_MEMORY_TEST_PRIVATE));

  //
  // Use the generic pattern to test compatible memory range
  //
  mGenericMemoryTestPrivate.MonoPattern   = GenericMemoryTestMonoPattern;
  mGenericMemoryTestPrivate.MonoTestSize  = GENERIC_CACHELINE_SIZE;

  //
  // Get the platform boot mode
  //
  HobList = GetHobList ();

  Hob.Raw = HobList;
  if (Hob.Header->HobType != EFI_HOB_TYPE_HANDOFF) {
    return EFI_NOT_FOUND;
  }

  BootMode = Hob.HandoffInformationTable->BootMode;

  for (Hob.Raw = HobList; !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob)) {
    if (GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      ResourceMemoryHob = Hob.ResourceDescriptor;

      if (((ResourceMemoryHob->ResourceType == EFI_RESOURCE_FIRMWARE_DEVICE) ||
           (ResourceMemoryHob->ResourceType == EFI_RESOURCE_MEMORY_MAPPED_IO)) &&
          ((ResourceMemoryHob->ResourceAttribute) & EFI_RESOURCE_ATTRIBUTE_PRESENT)) {
        BaseAddress = ResourceMemoryHob->PhysicalStart;
        Length      = ResourceMemoryHob->ResourceLength;

        Status = gDS->GetMemorySpaceDescriptor (BaseAddress, &GcdDescriptor);
        if (!EFI_ERROR (Status)) {
          Status = gDS->SetMemorySpaceAttributes (
                          BaseAddress,
                          Length,
                          GcdDescriptor.Attributes | EFI_MEMORY_RUNTIME
                          );
        }
      }
    }
  }

  //
  // Get the platform boot mode and create the default memory test coverage
  // level and span size
  //
  switch (BootMode) {
  case BOOT_WITH_FULL_CONFIGURATION:
    mGenericMemoryTestPrivate.CoverLevel    = SPARSE;
    mGenericMemoryTestPrivate.CoverageSpan  = SPARSE_SPAN_SIZE;
    break;

  case BOOT_WITH_MINIMAL_CONFIGURATION:
    mGenericMemoryTestPrivate.CoverLevel    = QUICK;
    mGenericMemoryTestPrivate.CoverageSpan  = QUICK_SPAN_SIZE;
    break;

  case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
    mGenericMemoryTestPrivate.CoverLevel    = IGNORE;
    mGenericMemoryTestPrivate.CoverageSpan  = QUICK_SPAN_SIZE;
    break;

  case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
    mGenericMemoryTestPrivate.CoverLevel    = EXTENSIVE;
    mGenericMemoryTestPrivate.CoverageSpan  = GENERIC_CACHELINE_SIZE;
    break;

  case BOOT_WITH_DEFAULT_SETTINGS:
    mGenericMemoryTestPrivate.CoverLevel    = SPARSE;
    mGenericMemoryTestPrivate.CoverageSpan  = SPARSE_SPAN_SIZE;
    break;

  default:
    mGenericMemoryTestPrivate.CoverLevel    = QUICK;
    mGenericMemoryTestPrivate.CoverageSpan  = QUICK_SPAN_SIZE;
    break;
  }
  //
  // Install the protocol
  //
  mGenericMemoryTestPrivate.Signature = EFI_GENERIC_MEMORY_TEST_PRIVATE_SIGNATURE;
  mGenericMemoryTestPrivate.GenericMemoryTest.MemoryTestInit = InitializeMemoryTest;
  mGenericMemoryTestPrivate.GenericMemoryTest.PerformMemoryTest = GenPerformMemoryTest;
  mGenericMemoryTestPrivate.GenericMemoryTest.Finished = GenMemoryTestFinished;
  mGenericMemoryTestPrivate.GenericMemoryTest.CompatibleRangeTest = GenCompatibleRangeTest;

  mGenericMemoryTestPrivate.Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &mGenericMemoryTestPrivate.Handle,
                  &gEfiGenericMemTestProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mGenericMemoryTestPrivate.GenericMemoryTest
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
