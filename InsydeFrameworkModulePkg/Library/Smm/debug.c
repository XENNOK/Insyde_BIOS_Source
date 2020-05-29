//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Debug.c

Abstract:

  Support for Debug primatives.

--*/

#include "Tiano.h"
#include "EfiCommonLib.h"
#include "EfiSmmDriverLib.h"
#include EFI_GUID_DEFINITION (StatusCodeCallerId)
#include EFI_GUID_DEFINITION (StatusCodeDataTypeId)
#include "SmmDriverLib.h"
#include "Peihob.h"
#include EFI_GUID_DEFINITION (Hob)
#include EFI_GUID_DEFINITION (SmramMemoryReserve)
#include EFI_ARCH_PROTOCOL_DEFINITION (StatusCode)

#define EFI_STATUS_CODE_DATA_MAX_SIZE64 (EFI_STATUS_CODE_DATA_MAX_SIZE / 8)

extern UINTN                    mSmmImageDebugMask;
extern EFI_DEBUG_MASK_PROTOCOL  *mDebugMaskInterface;
EFI_STATUS
InitializeSmmBaseRegionInfo (
  );

EFI_STATUS
DriverLibReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
STATIC EFI_STATUS_CODE_PROTOCOL  *gStatusCode = NULL;
#endif

SMM_REGION_BASE_INFO   mSmmBaseInfo = {0,0,0,0};

VOID
EfiDebugAssert (
  IN CHAR8    *FileName,
  IN INTN     LineNumber,
  IN CHAR8    *Description
  )
/*++

Routine Description:

  Worker function for ASSERT (). If Error Logging hub is loaded log ASSERT
  information. If Error Logging hub is not loaded DEADLOOP ().

Arguments:

  FileName    - File name of failing routine.

  LineNumber  - Line number of failing ASSERT().

  Description - Description, usually the assertion,

Returns:

  None

--*/
{
  UINT64                      Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  EFI_PHYSICAL_ADDRESS        PhysicalAddress;
  EFI_STATUS                  Status;

  if (mSmmBaseInfo.SmmRegionBase1 == 0 ) {
    //
    //Initialize mSmmBaseInfo
    //
    Status = InitializeSmmBaseRegionInfo ();
    if (EFI_ERROR (Status)) {

      return;
    }
  }

  PhysicalAddress = (UINTN) EfiDebugAssert;

  if (((PhysicalAddress > mSmmBaseInfo.SmmRegionBase1) &&
       (PhysicalAddress < mSmmBaseInfo.SmmRegionBase1 + mSmmBaseInfo.SmmRegionSize1)) ||
      ((PhysicalAddress > mSmmBaseInfo.SmmRegionBase2) &&
       (PhysicalAddress < mSmmBaseInfo.SmmRegionBase2 + mSmmBaseInfo.SmmRegionSize2))) {
    //
    //In SMM phase.
    //
    if (mSmmDebug != NULL) {
      EfiDebugAssertWorker (FileName, LineNumber, Description, sizeof (Buffer), Buffer);
      mSmmDebug->ReportStatusCode (
                  mSmmDebug,
                  (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
                  (EFI_SOFTWARE_SMM_DRIVER | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE),
                  0,
                  &gEfiCallerIdGuid,
                  (EFI_STATUS_CODE_DATA *) Buffer
                  );
    }
    //
    // don't return, this is an assert.
    //
    EFI_DEADLOOP ();

  } else {
    //
    //In DXE phase.
    //
    EfiDebugAssertWorker (FileName, LineNumber, Description, sizeof (Buffer), Buffer);

    DriverLibReportStatusCode (
      (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
      (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE),
      0,
      &gEfiCallerIdGuid,
      (EFI_STATUS_CODE_DATA *) Buffer
      );

    //
    // Put break point in module that contained the error.
    //
    EFI_BREAKPOINT ();
  }
}

VOID
EfiDebugVPrint (
  IN  UINTN   ErrorLevel,
  IN  CHAR8   *Format,
  IN  VA_LIST Marker
  )
/*++

Routine Description:

  Worker function for DEBUG(). If Error Logging hub is loaded log ASSERT
  information. If Error Logging hub is not loaded do nothing.

Arguments:

  ErrorLevel - If error level is set do the debug print.

  Format     - String to use for the print, followed by Print arguments.

  Marker     - VarArgs

Returns:

  None

--*/
{
  UINT64                            Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  EFI_PHYSICAL_ADDRESS              PhysicalAddress;
  UINTN                             ImageDebugMask;

  //
  //Get the function physical address to decide SMM or DXE phase.
  //
  PhysicalAddress = (UINTN)EfiDebugVPrint;

  if (((PhysicalAddress > mSmmBaseInfo.SmmRegionBase1) &&
       (PhysicalAddress < mSmmBaseInfo.SmmRegionBase1 + mSmmBaseInfo.SmmRegionSize1)) ||
      ((PhysicalAddress > mSmmBaseInfo.SmmRegionBase2) &&
       (PhysicalAddress < mSmmBaseInfo.SmmRegionBase2 + mSmmBaseInfo.SmmRegionSize2))) {
    //
    //In SMM phase.
    //
    if (!(mSmmImageDebugMask & ErrorLevel)) {
      return ;
    }

    EfiDebugVPrintWorker (ErrorLevel, Format, Marker, sizeof (Buffer), Buffer);

    if (mSmmDebug != NULL) {
      mSmmDebug->ReportStatusCode (
                  mSmmDebug,
                  EFI_DEBUG_CODE,
                  (EFI_SOFTWARE_SMM_DRIVER | EFI_DC_UNSPECIFIED),
                  0,
                  &gEfiCallerIdGuid,
                  (EFI_STATUS_CODE_DATA *) Buffer
                  );
    }
  } else {
    //
    //In DXE phase.
    //
    // Check driver debug mask value and global mask
    //
    if (mDebugMaskInterface != NULL) {
      mDebugMaskInterface->GetDebugMask (mDebugMaskInterface, &ImageDebugMask);
      if (!(ErrorLevel & ImageDebugMask)) {

        return ;
      }
    } else if (!(gErrorLevel & ErrorLevel)) {

      return ;
    }

    EfiDebugVPrintWorker (ErrorLevel, Format, Marker, sizeof (Buffer), Buffer);

    ASSERT (NULL != gRT);

    DriverLibReportStatusCode (
      EFI_DEBUG_CODE,
      (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_DC_UNSPECIFIED),
      0,
      &gEfiCallerIdGuid,
      (EFI_STATUS_CODE_DATA *) Buffer
      );
  }

  return;
}

VOID
EfiDebugPrint (
  IN  UINTN                   ErrorLevel,
  IN  CHAR8                   *Format,
  ...
  )
/*++

Routine Description:

  Worker function for DEBUG(). If Error Logging hub is loaded log ASSERT
  information. If Error Logging hub is not loaded do nothing.

  We use UINT64 buffers due to IPF alignment concerns.

Arguments:

  ErrorLevel - If error level is set do the debug print.

  Format     - String to use for the print, followed by Print arguments.

  ...        - VAR args for Format

Returns:

  None

--*/
{
  EFI_STATUS        Status;
  VA_LIST           Marker;

  VA_START (Marker, Format);

  if (mSmmBaseInfo.SmmRegionBase1 == 0 ) {
    //
    //Initialize mSmmBaseInfo
    //
    Status = InitializeSmmBaseRegionInfo ();
    if (EFI_ERROR (Status)) {

      return;
    }
  }

  EfiDebugVPrint (ErrorLevel, Format, Marker);
  VA_END (Marker);
}

UINTN
Memcmp (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINT8  Length
  )
/*++

Routine Description:

  Memory copy.

Arguments:

  Destination - Target address.
  Source - Orignal address.
  Length - Data length

Returns:

  None

--*/
{
  CHAR8 *Destination8;
  CHAR8 *Source8;

  Destination8 = Destination;
  Source8 = Source;

  while (Length--) {
    if (*(Destination8++) != *(Source8++)) {

     return 0;
    }
  }//end while

  return 1;
}

STATIC
EFI_STATUS
GetSystemConfigurationTable(
  IN EFI_GUID *Guid,
  OUT VOID    **Table
  )
/*++

Routine Description:
  Function returns a system configuration table that is stored in the
  EFI System Table based on the provided GUID.

Arguments:
  TableGuid        - A pointer to the table's GUID type.

  Table            - On exit, a pointer to a system configuration table.

Returns:

  EFI_SUCCESS      - A configuration table matching TableGuid was found

  EFI_NOT_FOUND    - A configuration table matching TableGuid was not found

--*/
{
  UINTN Index;

  for (Index = 0; Index < gST->NumberOfTableEntries; Index ++) {
    if (Memcmp(Guid, &(gST->ConfigurationTable[Index].VendorGuid), sizeof(EFI_GUID))) {
      *Table = gST->ConfigurationTable[Index].VendorTable;

      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

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
      	if (Length) {
      	  *Length = GuidHob.Header->HobLength - sizeof(EFI_HOB_GUID_TYPE);
        }

        return (VOID*)(GuidHob.Guid + 1);
      }
    }
    HobStart = GET_NEXT_HOB (GuidHob);
  }

  return NULL;
}

EFI_STATUS
InitializeSmmBaseRegionInfo (
  )
/*++

Routine Description:

  Get Smm Memory Region through Hob

Arguments:


Returns:

  EFI_SUCCESS      - A configuration table matching TableGuid was found

  EFI_NOT_FOUND    - A configuration table matching TableGuid was not found

--*/
{
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK            *SmramBlock;
  EFI_HOB_HANDOFF_INFO_TABLE                *HobTable;
  EFI_STATUS                                Status;
  UINTN                                     Length;

  //
  // Get Smm Memory Region through Hob
  //
  Status = GetSystemConfigurationTable(&gEfiHobListGuid, &HobTable);
  if (EFI_ERROR (Status)) {

    return Status;
  }
  SmramBlock = SearchGuidHob(HobTable, &gEfiSmmPeiSmramMemoryReserve, &Length);
  if (SmramBlock) {
    mSmmBaseInfo.SmmRegionBase1 = SmramBlock->Descriptor[0].PhysicalStart;
    mSmmBaseInfo.SmmRegionSize1 = (UINT32)SmramBlock->Descriptor[0].PhysicalSize;
    if (SmramBlock->NumberOfSmmReservedRegions > 1) {
      mSmmBaseInfo.SmmRegionBase2 = SmramBlock->Descriptor[1].PhysicalStart;
      mSmmBaseInfo.SmmRegionSize2 = (UINT32)SmramBlock->Descriptor[1].PhysicalSize;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DriverLibReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  )
/*++

Routine Description:

  Report device path through status code.

Arguments:

  Type        - Code type
  Value       - Code value
  Instance    - Instance number
  CallerId    - Caller name
  DevicePath  - Device path that to be reported

Returns:

  Status code.

  EFI_OUT_OF_RESOURCES - No enough buffer could be allocated

--*/
{
  EFI_STATUS  Status;

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  if (gStatusCode == NULL) {
    Status = gBS->LocateProtocol (&gEfiStatusCodeRuntimeProtocolGuid, NULL, (VOID **)&gStatusCode);
    if (EFI_ERROR (Status) || gStatusCode == NULL) {

      return EFI_UNSUPPORTED;
    }
  }
  Status = gStatusCode->ReportStatusCode (Type, Value, Instance, CallerId, Data);

  return Status;
#else
  Status = gRT->ReportStatusCode (Type, Value, Instance, CallerId, Data);

  return Status;
#endif
}
