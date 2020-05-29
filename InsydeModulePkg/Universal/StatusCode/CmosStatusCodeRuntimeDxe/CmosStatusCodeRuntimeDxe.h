/** @file

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

#ifndef _CMOS_STATUS_CODE_DUNTIME_DXE_H_
#define _CMOS_STATUS_CODE_DUNTIME_DXE_H_

#include <Protocol/ReportStatusCodeHandler.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/StatusCode.h>
#include <Protocol/DevicePath.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/EventGroup.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DebugLib.h>
#include <Library/CmosLib.h>
#include <Library/PcdLib.h>
#include <CmosDebugCode.h>

//
// Worker or Callback function prototypes
//
EFI_STATUS
EFIAPI
CmosStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  );

VOID
EFIAPI
RscHandlerCallBack (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

VOID
EFIAPI
UnregisterBootTimeHandlers (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

//
// Helper function prototypes
//
VOID
SortDebugCodeTbl (
  IN DEBUG_CODE_DATA    *Array,
  IN UINTN              DataCount
  );

EFI_STATUS
SearchDebugCodeValue (
  IN  CONST EFI_STATUS_CODE_DATA    *Data,
  OUT UINT8                         *DebugCodeValue,
  OUT UINT8                         *DebugGroupValue
  );

INT32
HelperCompareGuid (
  IN EFI_GUID    *Guid1,
  IN EFI_GUID    *Guid2
  );

#endif

