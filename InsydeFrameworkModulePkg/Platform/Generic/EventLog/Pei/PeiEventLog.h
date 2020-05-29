//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _EFI_PPI_EVENT_LOG_PEIM_H_
#define _EFI_PPI_EVENT_LOG_PEIM_H_

//
// Statements that include other files.
//
#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "EfiCommonLib.h"
#include "CmosLib.h"

#include EFI_PPI_DEFINITION(EventLog)

#include EFI_GUID_DEFINITION (DataHubRecords)
#include EFI_GUID_DEFINITION (EventLogHob)

EFI_STATUS
EFIAPI
PeiEventLogInit (
  IN EFI_FFS_FILE_HEADER        *FfsHeader,
  IN EFI_PEI_SERVICES           **PeiServices
  );

EFI_STATUS
PeiWriteEvent (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN  EFI_PEI_EVENT_LOG_PPI             *This,
  IN  UINT8                             EventLogType,
  IN EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap1,
  IN EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap2
  );

#endif

