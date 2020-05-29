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

Module Name:

  StatusCodeReport.c

Abstract:

  Status Code Report Protocol

--*/

#include <Tiano.h>

#include EFI_PROTOCOL_DEFINITION (StatusCodeReport)

EFI_GUID gEfiStatusCodeReportProtocolGuid = EFI_STATUS_CODE_REPORT_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiStatusCodeReportProtocolGuid, "StatusCodeReportProtocol", "Status Code Report Protocol");

