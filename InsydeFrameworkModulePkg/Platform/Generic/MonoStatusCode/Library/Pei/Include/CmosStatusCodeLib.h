//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CmosStatusCode.h

Abstract:

  Lib to provide status code writting to cmos offset 0x38/0x39.

--*/

#ifndef _PEI_CMOS_STATUS_CODE_LIB_H_
#define _PEI_CMOS_STATUS_CODE_LIB_H_

//
// Statements that include other files
//
#include "Tiano.h"
#include "Pei.h"
#include EFI_PROTOCOL_CONSUMER (StatusCodeReport)

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
;

VOID
ShiftDebugCodeInCmos (
  VOID
  )
/*++

Routine Description:

  Shift DebugGroup and DebugCode in CMOS.

Arguments:

Returns:

--*/
;
#endif

