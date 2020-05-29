/** @file

  Header file of BMC SEL Event String Translation implementation.

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

#ifndef _BMC_SEL_STRING_TRANSLATION_H_
#define _BMC_SEL_STRING_TRANSLATION_H_

#include <Protocol/H2OIpmiInterfaceProtocol.h>
#include <Protocol/H2OIpmiSelInfoProtocol.h>

#include <Protocol/EventLog.h>
#include <Protocol/EventLogString.h>

#include <Standard/H2OIpmiSel.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>


#define MAX_UPDATE_DATA_SIZE                0x1000

typedef struct {
  UINT8                            BmcSensorType;
  UINT8                            BmcSensorNum;
  UINT8                            BmcEventType;
  BOOLEAN                          SkipTranslation;
  } BMC_SEL_TRANSLATION_SKIP_TABLE;

EFI_STATUS
EFIAPI
EventLogStringHandler (
  IN VOID                                   *EventInfo,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  );

EFI_STATUS
EFIAPI
GetBmcSelString (
  IN STORAGE_EVENT_LOG_INFO                 *EventInfo,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  );

BOOLEAN
EFIAPI
SkipTranslationSel (
  IN STORAGE_EVENT_LOG_INFO                     *EventInfo
  );
  
#endif
