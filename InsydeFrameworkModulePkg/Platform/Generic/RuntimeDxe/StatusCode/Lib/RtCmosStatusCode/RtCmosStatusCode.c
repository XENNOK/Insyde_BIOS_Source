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

  RtCmosStatusCode.c

Abstract:

  Write driver DebugCode code (Driver group/Debug Code) to Cmos offset 0x38/0x39

--*/
#include "RtCmosStatusCode.h"
#include "EfiStatusCode.h"
#include "CmosLib.h"
#include "EfiRuntimeLib.h"
 
#include EFI_PROTOCOL_CONSUMER (StatusCodeReport)

EFI_STATUS
EFIAPI
RtCmosReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
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
{
  EFI_STATUS_CODE_REPORT_PROTOCOL    *StatusCodeReportProtocol = NULL;
  EFI_STATUS                         Status;

  if (Value == (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_BEGIN)) {
    //
    // Ouput the DebugCode defined in DebugCode.h
    //
    Status = gBS->LocateProtocol (&gEfiStatusCodeReportProtocolGuid, NULL, &StatusCodeReportProtocol);
    if (!EFI_ERROR (Status)) {
      if (StatusCodeReportProtocol->OutPutByStatusCode) {
        EfiWriteCmos8 (CurrentDebugGroup, StatusCodeReportProtocol->CurrentStatusGroup);
        EfiWriteCmos8 (CurrentDebugCode, StatusCodeReportProtocol->CurrentStatusCode);
        StatusCodeReportProtocol->OutPutByStatusCode = FALSE;
      } else {
        //
        // Driver GUID is not found.
        // 2.The driver is DxeMain or StatusCodeReport.
        //
        EfiWriteCmos8 (CurrentDebugGroup, 0xFF);
        EfiWriteCmos8 (CurrentDebugCode, 0xFF);
      }
    }
  } else if (Value == (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_END)) {
    //
    // Reserve for expand.
    //
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}
