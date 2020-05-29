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

#ifndef _CMOS_STATUS_CODE_PEI_H_
#define _CMOS_STATUS_CODE_PEI_H_

#include <Ppi/ReportStatusCodeHandler.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Pi/PiStatusCode.h>
#include <Pi/PiPeiCis.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <CmosDebugCode.h>

//
// Worker or Callback function prototypes
//
EFI_STATUS
EFIAPI
CmosStatusCodeReportWorker (
  IN CONST  EFI_PEI_SERVICES        **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  );

EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

//
// Helper function prototypes
//
EFI_STATUS
EFIAPI
SearchDebugCodeValue (
  IN  CONST EFI_STATUS_CODE_DATA  *Data,
  OUT UINT8                       *DebugCodeValue,
  OUT UINT8                       *DebugGroupValue
  );

VOID
ShiftDebugCodeInCmos (
  VOID		
  );

#endif
