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

  StatusCodeReport.h

Abstract:

  Status Code Report Protocol

--*/

#ifndef _STATUS_CODE_REPORT_PROTOCOL_H_
#define _STATUS_CODE_REPORT_PROTOCOL_H_

#define EFI_STATUS_CODE_REPORT_PROTOCOL_GUID  \
  {0x915FB9B9, 0x92E2, 0x4fdf, 0xA1, 0x3F, 0xA3, 0x68, 0x0E, 0xD9, 0xF4, 0xEA}


EFI_FORWARD_DECLARATION (EFI_STATUS_CODE_REPORT_PROTOCOL);

typedef struct _EFI_STATUS_CODE_REPORT_PROTOCOL {
  UINT8               CurrentStatusCode;
  UINT8               CurrentStatusGroup;
  BOOLEAN             OutPutByStatusCode;
} EFI_STATUS_CODE_REPORT_PROTOCOL;

extern EFI_GUID gEfiStatusCodeReportProtocolGuid;

#endif
