/** @file

  Definition of H2O Event Storage protocol.
    
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_EVENT_STORAGE_PROTOCOL_H_
#define _H2O_EVENT_STORAGE_PROTOCOL_H_
    
#include <Protocol/EventLog.h>

/**
  Locate event log protocol to log data.

  @param[in] EventID  ID for specific event.
  @param[in] Data     Data that will log into specific storage.
  @param[in] DataSize Size of Data.  
  
  @retval EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *WRITE_EVENT) (
  IN  EVENT_TYPE_ID                    *EventID,
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
);

/**
  Locate Event log protocol to get logged data in BIOS.

  @param[in]  Data     Logged data of specific storage.
  @param[out] DataSize Size of logged Data.  
  
  @retval EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *READ_EVENT) (
  IN      UINTN                        Index,
  IN OUT  UINT8                        **Data
);

/**
  Return storage name string.

  @param[out] String   name string of the specific storage.
  
  @retval EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *GET_STORAGE_NAME_STRING) (
  CHAR16                               **String
);

/**
  Return the count of event count.

  @param[out] Count     Event count of specific storage.
  
  @retval EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *GET_EVENT_COUNT) (
  IN OUT  UINTN                        *EventCount           
);

/**
  Clear logged data.
  
  @retval EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *CLEAR_EVENT) (
  VOID
);

/**
  Locate Event log protocol to get logged data in specific storage.

  @param[out] DataCount   Number of data count in specific storage.
  
  @retval EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *REFRESH_DATABASE) (
  IN OUT  UINTN                        *DataCount
);

typedef struct _EVENT_STORAGE_PROTOCOL {
  WRITE_EVENT                          WriteEvent;
  READ_EVENT                           ReadEvent;
  GET_STORAGE_NAME_STRING              GetStorageNameString;
  GET_EVENT_COUNT                      GetEventCount;
  CLEAR_EVENT                          ClearEvent;
  REFRESH_DATABASE                     RefreshDatabase;
  EVENT_LOG_FULL_OPTION                *EventLogFullOption;
} EVENT_STORAGE_PROTOCOL;

extern EFI_GUID gH2OEventStorageProtocolGuid;
extern EFI_GUID gH2OSmmEventStorageProtocolGuid;

#endif

