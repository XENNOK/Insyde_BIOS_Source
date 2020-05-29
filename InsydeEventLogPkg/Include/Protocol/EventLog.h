/** @file

  Definition of H2O Event Log protocol.
    
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

#ifndef _H2O_EVENT_LOG_PROTOCOL_H_
#define _H2O_EVENT_LOG_PROTOCOL_H_

#define EVENT_LOG_DATA_SIZE                    0x08

#define BIOS_DEFINED_EVENT_DATA                  0xC0
#define BIOS_DEFINED_SENSOR_NUM                  0x80

#define EVENT_TYPE_DISCRETE                      0x6F
#define EVENT_TYPE_OEM                           0x70

#define EVENT_STORAGE_ALL                        0
#define EVENT_STORAGE_BIOS                       1
#define EVENT_STORAGE_BMC_SEL                    2
#define EVENT_STORAGE_DCMI_SEL                   3
#define EVENT_STORAGE_MEMORY                     4
#define EVENT_STORAGE_UNDEFINED                  0xFF

//
// BIOS Definition
//
#define OEM_SENSOR_TYPE_EVENT_STORAGE            0xE0
#define OEM_SENSOR_TYPE_IIO_EVENT                0xE1

#define OEM_SENSOR_OVERWRITE                     0x00

//
// Option for if the event storage is full.
//
typedef enum {
  EVENT_LOG_FULL_OVERWRITE = 0,
  EVENT_LOG_FULL_CLEAR_ALL = 1,
  EVENT_LOG_FULL_STOP_LOGGING = 2
} EVENT_LOG_FULL_OPTION;

typedef struct {
  UINT8                  EventStorageType;
  CHAR16                 *StorageNameString;
  BOOLEAN                LogPeiEventLogHob; // Determine if the storage(BIOS, BMC, or Memory) can log "PeiEventLog" HOB data.
  } STORAGE_INFO;

typedef struct {
  CHAR16                 *EventStorageName;
  } EVENT_STORAGE_INFO;

#pragma pack(1)

typedef struct {
  UINT8                            SensorType;
  UINT8                            SensorNum;
  UINT8                            EventType;
  } EVENT_TYPE_ID;

//
// Structure to store System Event Log. 
// Adapted from IPMI SEL event records.
//
typedef struct {
  UINT16  NextRecordId;
  UINT16  RecordId;
  UINT8   RecordType;
  
  UINT16  Year;
  UINT8   Month;
  UINT8   Date;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;

  UINT16  GeneratorId;
  BOOLEAN GeneratorIdEnable;
  UINT8   EvMRev;  

  EVENT_TYPE_ID  EventID;// 3-bytes ID: 
  BOOLEAN EventIdEnable;// for the storage that wants to show Sensor Type, Sensor Num, and Event Type.
  UINT8   *Data; // for log data
  
  UINT8   EventTypeID; // for bios
  BOOLEAN EventTypeIdEnable;


  UINT8   *EventRawData;
  UINTN   EventRawDataSize;
  
} STORAGE_EVENT_LOG_INFO;
#pragma pack()

/**
  Log event data and event ID to the specific storage.
  
  @param [in]  EventStorageType        Storage type which will log event data into.
  @param [in]  EventID                 ID for specific event.
  @param [in]  ErrorData               Event data.
  @param [in]  ErrorDataSize           Size of event data.
    
  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *LOG_EVENT) (
  IN  UINT8                            EventStorageType,
  IN  EVENT_TYPE_ID                    *EventID,
  IN  VOID                             *ErrorData,
  IN  UINTN                            ErrorDataSize
);

/**
  Get event data from the specific storage.
  
  @param [in]  StorageString           Storage type which will get event data from.
  @param [in]  Index                   Index of data that in selected storage.
  @param [in]  Data                    Event data in the selected storage.
    
  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *GET_EVENT) (
  IN      CHAR16                       *StorageString,
  IN      UINTN                        Index,
  IN OUT  UINT8                        **Data
);

/**
  Get event count from the selected storage.
  
  @param [in]  StorageString        Storage type which will get event data from.
  @param [out] EventLogCount        Event count of the selected storage.
    
  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *GET_STORAGE_EVENT_LOG_COUNT) (
  IN      CHAR16                       *StorageString,
  IN OUT  UINTN                        *EventLogCount           
);

/**
  Clear event log data from the selected storage.
  
  @param [in]  StorageString        Storage type which will get event data from.
    
  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *CLEAR_STORAGE_LOG) (
  IN      CHAR16                       *StorageString
);

/**
  Update event data of the selected storage driver.
  
  @param [in]  StorageString        Storage type which will get event data from.
  @param [out] EventLogCount        Event count of the selected storage.
  
  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *REFRESH_STORAGE_DATABASE) (
  IN      CHAR16                       *StorageString,
  IN OUT  UINTN                        *EventLogCount
);

/**
  Get all the names of event storage drivers.
  
  @param [out] StorageName        Storage names.
  @param [out] StorageNum         Number of storage drivers.  
  
  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *GET_STORAGE_NAME) (
  IN OUT  EVENT_STORAGE_INFO           **StorageName,
  IN OUT  UINTN                        *StorageNum
);

typedef struct _EVENT_LOG_PROTOCOL {
  LOG_EVENT                            LogEvent;
  GET_EVENT                            GetEvent;
  GET_STORAGE_EVENT_LOG_COUNT          GetEventCount;
  CLEAR_STORAGE_LOG                    ClearStorage;
  REFRESH_STORAGE_DATABASE             RefreshDatabase;
  GET_STORAGE_NAME                     GetStorageName;
} EVENT_LOG_PROTOCOL;


extern EFI_GUID gH2OEventLogProtocolGuid;
extern EFI_GUID gH2OSmmEventLogProtocolGuid;

#endif

