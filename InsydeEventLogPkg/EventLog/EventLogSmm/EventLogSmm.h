/** @file

  Header file of Event Log Smm implementation.

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

#ifndef _EVENT_LOG_SMM_H_
#define _EVENT_LOG_SMM_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>

#include <Protocol/EventLog.h>
#include <Protocol/EventStorage.h>


typedef struct {
  EVENT_STORAGE_PROTOCOL                 *EventStorage;
  CHAR16                                 *StorageNameString;
  UINTN                                  Type;
} EVENT_STORAGE_HANDLER_BUFFER;

EFI_STATUS
EFIAPI
LogEvent (
  IN  UINT8                              EventStorageType,
  IN  EVENT_TYPE_ID                      *EventID,
  IN  VOID                               *ErrorData,  
  IN  UINTN                              ErrorDataSize
  );

EFI_STATUS
EFIAPI
GetEvent (
  IN      CHAR16                         *StorageString,
  IN      UINTN                          DataIndex,
  IN OUT  UINT8                          **Data
  );

EFI_STATUS
EFIAPI
GetEventCount (
  IN      CHAR16                         *StorageString,
  IN OUT  UINTN                          *EventLogCount  
  );

EFI_STATUS
EFIAPI
ClearEvent (
  IN      CHAR16                         *StorageString
  );

EFI_STATUS
EFIAPI
RefreshDatabase (
  IN      CHAR16                         *StorageString,
  IN OUT  UINTN                          *EventLogCount  
  );

EFI_STATUS
EFIAPI
GetEventStorageName (
  IN OUT  EVENT_STORAGE_INFO             **StorageName,
  IN OUT  UINTN                          *StorageNum
  );

EFI_STATUS
EFIAPI
EventStorageProtocolCallback (
  IN CONST EFI_GUID                      *Protocol,
  IN VOID                                *Interface,
  IN EFI_HANDLE                          Handle
  );

EFI_STATUS
EFIAPI
StorageType2String (
  IN      UINT8                          Type,
  IN  OUT CHAR16                         **String
  );

EFI_STATUS
EFIAPI
FindStorageByString (
  IN     CHAR16                         *String,
  IN OUT UINTN                          *HandlerIndex 
  );

EFI_STATUS
EFIAPI
EventLogPolicyInit (
  VOID
  );

#endif
