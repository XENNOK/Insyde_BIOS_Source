/** @file

  Header file of Memory Storage Chipset implementation.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _MEMORY_STORAGE_CHIPSET_SMM_H_
#define _MEMORY_STORAGE_CHIPSET_SMM_H_

//
// Statements that include other header files
//
#include <Protocol/MemoryStorage.h>
#include <Protocol/EventLog.h>
#include <Protocol/EventStorage.h>
#include <Protocol/SmmBase2.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>


#define EVENT_LOG_FULL_ADJUST_EVENT_NUM    2
//[-start-140213-IB08400246-modify]//
#define CHANGE_MEMORY_STORAGE_DATA3        0x02
//[-end-140213-IB08400246-modify]//


typedef struct {
  MEMORY_STORAGE_ORGANIZATION       *EventList;
} VE_MEMORY_EVENT_LOG_STRUCTURE;


EFI_STATUS
EFIAPI
MemoryWriteEvent (
  IN  EVENT_TYPE_ID                    *EventID,
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
  );

EFI_STATUS
EFIAPI
MemoryReadEvent (
  IN      UINTN                        Index,
  IN OUT  UINT8                        **Data
  );

EFI_STATUS
EFIAPI
MemoryGetStorageNameString (
  CHAR16                               **String
);

EFI_STATUS
EFIAPI
MemoryGetEventCount (
  IN  OUT UINTN                        *Count
  );

EFI_STATUS
EFIAPI
MemoryRefreshDatabase (
  IN OUT  UINTN                        *DataCount
  );

EFI_STATUS
EFIAPI
MemoryClearEvent (
  VOID
);

UINT8
EFIAPI
TimeTranslator (
  IN  UINT8                            OrgTime
);

VOID
EFIAPI
FreeMemoryEventDatabase (
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

#endif
