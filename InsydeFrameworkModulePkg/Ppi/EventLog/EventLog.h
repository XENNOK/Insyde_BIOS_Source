//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _EFI_PPI_EVENT_LOG_H_
#define _EFI_PPI_EVENT_LOG_H_

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (EventLog)
#include EFI_GUID_DEFINITION (DataHubRecords)
#include EFI_GUID_DEFINITION (EventLogHob)

#define EFI_PPI_EVENT_LOG_GUID \
  {0xac7f30c2, 0xf774, 0x47d3, 0x8c, 0x24, 0xe9, 0xc9, 0x94, 0x5f, 0x3c, 0x49}

EFI_FORWARD_DECLARATION(EFI_PEI_EVENT_LOG_PPI);

typedef
EFI_STATUS
(EFIAPI *EVENT_LOG_PPI_WRITE) (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN  EFI_PEI_EVENT_LOG_PPI             *This,
  IN  UINT8                             EventLogType,
  IN EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap1,
  IN EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap2
  )
;

typedef
struct _EFI_PEI_EVENT_LOG_PPI {
  EVENT_LOG_PPI_WRITE     WriteEvent;
} EFI_PEI_EVENT_LOG_PPI;

extern EFI_GUID                   gEfiPeiEventLogGuid;

#endif

