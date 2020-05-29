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
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
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

  CmosStatusCode.c

Abstract:

  Lib to provide report driver debugcode which write to Cmos offset 0x38/0x39

--*/
#include "CmosStatusCode.h"

INT32
EfiCompareGuid2 (
  IN EFI_GUID *Guid1,
  IN EFI_GUID *Guid2
  )
/*++

Routine Description:

  Compares two GUIDs

Arguments:

  Guid1 - guid to compare

  Guid2 - guid to compare

Returns:
  0        if Guid1 == Guid2
  1        if Guid1 > Guid2
  -1       if Guid1 < Guid2

--*/
{
  UINTN Index;
  //
  // compare byte by byte
  //
  for (Index = 0; Index < 16; ++Index) {
    if (*(((UINT8*) Guid1) + Index) > *(((UINT8*) Guid2) + Index)) {
      return 1;
    }
    else if (*(((UINT8*) Guid1) + Index) < *(((UINT8*) Guid2) + Index)) {
      return -1;
    }
  }
  return 0;
}

EFI_STATUS
EFIAPI
SearchDebugCodeValue (
  IN  EFI_PEI_SERVICES         **PeiServices,
  IN  EFI_STATUS_CODE_DATA     * Data,
  OUT UINT8                    * DebugCodeValue,
  OUT UINT8                    * DebugGroupValue
  )
/*++

Routine Description:

  Provide a status code

Arguments:

  Same as ReportStatusCode PPI

Returns:

  EFI_SUCCESS   Always returns success.

--*/
// GC_TODO:    PeiServices - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  EFI_DEVICE_HANDLE_EXTENDED_DATA   *ExtendedDataPtr;
  EFI_FFS_FILE_HEADER               *EfiFfsFileHeaderPtr;
  UINT32                            Index;
  INT32                             CmpValue;
  EFI_STATUS                        Status;

  Status = EFI_NOT_FOUND;
  if (Data != NULL) {
    ExtendedDataPtr = (EFI_DEVICE_HANDLE_EXTENDED_DATA *)Data;
    EfiFfsFileHeaderPtr = (EFI_FFS_FILE_HEADER *)(ExtendedDataPtr->Handle);

    for (Index = 0; Index < mKernelDebugCodeTblSize; Index++) {
      CmpValue = EfiCompareGuid2 ((EFI_GUID *)&mKernelDebugCodeTbl[Index].NameGuid, &(EfiFfsFileHeaderPtr->Name));
      if (CmpValue == 0) {
        *DebugCodeValue  = mKernelDebugCodeTbl[Index].DebugCode;
        *DebugGroupValue = mKernelDebugCodeTbl[Index].DebugGroup;
        Status = EFI_SUCCESS;

        return Status;
      }
    }
    for (Index = 0; Index < mChipsetCodeTblSize; Index++) {
      CmpValue = EfiCompareGuid2 ((EFI_GUID *)&mChipsetCodeTbl[Index].NameGuid, &(EfiFfsFileHeaderPtr->Name));
      if (CmpValue == 0) {
        *DebugCodeValue  = mChipsetCodeTbl[Index].DebugCode;
        *DebugGroupValue = mChipsetCodeTbl[Index].DebugGroup;
        Status = EFI_SUCCESS;

        return Status;
      }
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
CmosReportStatusCode (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN EFI_STATUS_CODE_TYPE          CodeType,
  IN EFI_STATUS_CODE_VALUE         Value,
  IN UINT32                        Instance,
  IN EFI_GUID                      * CallerId,
  IN EFI_STATUS_CODE_DATA          * Data OPTIONAL,
  EFI_STATUS_CODE_REPORT_PROTOCOL  *StatusCodeReport
  )
/*++

Routine Description:

  Storage DebugGroup to CMOS(0x38).
  Storage DebugCode  to CMOS(0x39).

Arguments:

  Same as ReportStatusCode PPI

Returns:

  EFI_SUCCESS   Always returns success.

--*/
// GC_TODO:    PeiServices - add argument and description to function comment
// GC_TODO:    CodeType - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    Instance - add argument and description to function comment
// GC_TODO:    CallerId - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  EFI_STATUS                        Status;
  UINT8                             CodeValue;
  UINT8                             GroupValue;

  //
  // Progress or error code, Output to CMOS Offset 0x38 and 0x39
  //
  if (Value == (EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_BEGIN)) {
    //
    // Ouput the DebugCode defined in DebugCode.h
    //
    Status = SearchDebugCodeValue (PeiServices, Data, &CodeValue, &GroupValue);
    if (!EFI_ERROR (Status)) {
      EfiWriteCmos8 (CurrentDebugGroup, GroupValue);
      EfiWriteCmos8 (CurrentDebugCode, CodeValue);
    } else {
      //
      // Driver GUID is not found.
      //
      EfiWriteCmos8 (CurrentDebugGroup, 0xFF);
      EfiWriteCmos8 (CurrentDebugCode, 0xFF);
    }
  }
  //
  //  this block is only applied under Build-IA32.
  //
  else if (Value == (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_BEGIN)) {
    if (StatusCodeReport != NULL) {
      if (StatusCodeReport->OutPutByStatusCode) {
        EfiWriteCmos8 (CurrentDebugGroup, StatusCodeReport->CurrentStatusGroup);
        EfiWriteCmos8 (CurrentDebugCode, StatusCodeReport->CurrentStatusCode);
        StatusCodeReport->OutPutByStatusCode = FALSE;
      } else {
        //
        // Driver GUID is not found.
        //
        EfiWriteCmos8 (CurrentDebugGroup, 0xFF);
        EfiWriteCmos8 (CurrentDebugCode, 0xFF);
      }
    }
  }

  else if ((Value == (EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_END)) ||
             (Value == (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_END))) {
    //
    // Reserve for expand.
    //
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

VOID
ShiftDebugCodeInCmos ()
/*++

Routine Description:

  Shift DebugGroup and DebugCode in CMOS.

Arguments:

Returns:

--*/
{
  //
  //Reference CmosLib.h:
  //
  //  CurrentDebugGroup  = 0x38, CurrentDebugCode   = 0x39
  //  BackupDebugGroup1  = 0x3A, BackupDebugCode1   = 0x3B
  //  BackupDebugGroup2  = 0x3C, BackupDebugCode2   = 0x3D
  //  BackupDebugGroup3  = 0x3E, BackupDebugCode3   = 0x3F
  //
  EfiWriteCmos8 (BackupDebugGroup3, EfiReadCmos8 (BackupDebugGroup2));
  EfiWriteCmos8 (BackupDebugCode3,  EfiReadCmos8 (BackupDebugCode2));
  EfiWriteCmos8 (BackupDebugGroup2, EfiReadCmos8 (BackupDebugGroup1));
  EfiWriteCmos8 (BackupDebugCode2,  EfiReadCmos8 (BackupDebugCode1));
  EfiWriteCmos8 (BackupDebugGroup1, EfiReadCmos8 (CurrentDebugGroup));
  EfiWriteCmos8 (BackupDebugCode1,  EfiReadCmos8 (CurrentDebugCode));
}
