/** @file

  Header file of Event Log PEI implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _EVENT_LOG_PEI_H_
#define _EVENT_LOG_PEI_H_

//
// Statements that include other files.
//

#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Guid/PeiEventLogHob.h>
#include <Ppi/EventLogPei.h>
#include <Ppi/BmcSelStoragePei.h>
#include <Ppi/ReadOnlyVariable2.h>


EFI_STATUS
EFIAPI
PeiLogEvent (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *LogData
  );

EFI_STATUS
EFIAPI
PeiEventLogCreateHob (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *Data
  );

#endif
