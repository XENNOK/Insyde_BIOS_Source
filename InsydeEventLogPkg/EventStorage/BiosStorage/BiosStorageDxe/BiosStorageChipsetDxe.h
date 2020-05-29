/** @file

  Header file of BIOS Storage Chipset Dxe implementation.

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


#ifndef _BIOS_STORAGE_CHIPSET_DXE_H_
#define _BIOS_STORAGE_CHIPSET_DXE_H_

#include <Protocol/BiosEventLog.h>
#include <Protocol/EventLog.h>
#include <Protocol/EventStorage.h>
#include <Protocol/EventLogString.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#define EVENT_LOG_FULL_ADJUST_EVENT_NUM    3
//[-start-140213-IB08400246-modify]//
#define CHANGE_BIOS_STORAGE_DATA3          0x01
//[-end-140213-IB08400246-modify]//

typedef struct {
  BIOS_EVENT_LOG_ORGANIZATION       *EventList;
} VE_BIOS_EVENT_LOG_STRUCTURE;

typedef struct {
  UINT8                            Type;
  UINT8                            ValidDataLength;
  UINT8                            Data[8];
  CHAR16                           *EventStr;
  } BIOS_EVENT_LOG_STRING;

EFI_STATUS
EFIAPI
BiosWriteEvent (
  IN  EVENT_TYPE_ID                    *EventID,
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
  );

EFI_STATUS
EFIAPI
BiosReadEvent (
  IN      UINTN                        Index,
  IN OUT  UINT8                        **Data
  );

EFI_STATUS
EFIAPI
BiosGetStorageNameString (
  CHAR16                               **String
);

EFI_STATUS
EFIAPI
BiosGetEventCount (
  IN  OUT UINTN                        *Count
  );

EFI_STATUS
EFIAPI
BiosRefreshDatabase (
  IN OUT  UINTN                        *DataCount
  );

EFI_STATUS
EFIAPI
BiosClearEvent (
  VOID
);

UINT8
EFIAPI
TimeTranslator (
  IN  UINT8                            OrgTime
);

VOID
EFIAPI
FreeBiosEventDatabase (
  VOID
  );

EFI_STATUS
EFIAPI
AddEventAfterArrangeEventStorage (
  VOID
  );

EFI_STATUS
EFIAPI
ArrangeEventStorage (
  VOID
  );

EFI_STATUS
EFIAPI
ShiftDataOfStorage (
  VOID
  );

EFI_STATUS
EFIAPI
EventLogStringHandler (
  IN VOID                                   *EventInfo,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  );

EFI_STATUS
EFIAPI
BiosEventLogStringType16 (
  IN VOID                                   *EventInfo OPTIONAL,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  );

EFI_STATUS
EFIAPI
BiosEventLogStringType80 (
  IN VOID                                   *EventInfo OPTIONAL,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  );

#endif
