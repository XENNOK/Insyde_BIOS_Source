/** @file

  Memory Event Storage Chipset DXE implementation.

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

#include <MemoryStorageChipsetDxe.h>

STORAGE_EVENT_LOG_INFO                *mMemoryStorageLog;
UINTN                                 mMemoryStorageLogNum = 0;
CHAR16                                *mMemoryStorageNameString = L"MEMORY";
EVENT_LOG_FULL_OPTION                 mEventLogFullOp = EVENT_LOG_FULL_OVERWRITE;                
EVENT_STORAGE_PROTOCOL                gEventStorageProtocol = {
                                                         MemoryWriteEvent,
                                                         MemoryReadEvent,
                                                         MemoryGetStorageNameString,
                                                         MemoryGetEventCount,
                                                         MemoryClearEvent,
                                                         MemoryRefreshDatabase,
                                                         &mEventLogFullOp
                                                         };

//[-start-140213-IB08400246-add]//
H2O_EVENT_LOG_STRING_PROTOCOL         gEventLogString = {
  EventLogStringHandler
  };

MEMORY_EVENT_LOG_STRING               gMemoryEventLogString[] = {
                                                           { {OEM_SENSOR_TYPE_EVENT_STORAGE, BIOS_DEFINED_SENSOR_NUM, EVENT_TYPE_OEM},  // Event ID, 3 bytes
                                                             {OEM_SENSOR_OVERWRITE, 0xFF, CHANGE_MEMORY_STORAGE_DATA3},                 // Data, at most 8 bytes.
                                                             0x3, // Length of Data 
                                                             L"Memory Event Storage has been changed." // String of the event
                                                           }
                                                         };

//[-end-140213-IB08400246-add]//

/**
 Locate event log protocol to log data.           

 @param[in] EventID  ID for specific event.
 @param[in] Data     Data that will log into MEMORY storage.
 @param[in] DataSize Size of Data.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryWriteEvent (
  IN  EVENT_TYPE_ID                    *EventID,
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
  )
{
  EFI_STATUS                         Status;
  EFI_MEMORY_STORAGE_PROTOCOL        *MemEventLog;  
  UINT8                              *Buffer=NULL;

  MemEventLog = NULL;  

  Status = gBS->LocateProtocol (&gH2OMemoryStorageProtocolGuid, NULL, (VOID **)&MemEventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gBS->AllocatePool (
                EfiBootServicesData,
                DataSize,
                (VOID **)&Buffer
                );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  ZeroMem (Buffer, DataSize);

  //
  // Loop for even fulled state
  //

  do {
    CopyMem (Buffer, Data, DataSize);
    
    Status = MemEventLog->Write (
                             EventID->SensorType,
                             EventID->SensorNum,
                             EventID->EventType,
                             DataSize,
                             Buffer
                             );    
    if (EFI_ERROR (Status) && (Status == EFI_OUT_OF_RESOURCES)) {
      //
      // It means the storage is full of data, 
      // we need to adjust the storage base on the policy of 
      // "Event Log Full option" setting in SCU.
      //

      if (mEventLogFullOp == EVENT_LOG_FULL_STOP_LOGGING) {
        //
        // Not log anymore.
        //
        return EFI_OUT_OF_RESOURCES;
      } else {
        //
        // Storage is full, and we still want to log event into it.
        // Need to arrange the data in the storage.
        //
        Status = ArrangeEventStorage ();
        if (EFI_ERROR (Status)) {
          return Status;
        } 
        
      }
    } else {
      //
      // After MemEventLog->Write(),
      // return if Status == EFI_SUCCESS or OTHER ERROR status beside EFI_OUT_OF_RESOURCE.
      //
      return Status;
    } 
    
  } while (TRUE);

}

/**
 Locate Event log protocol to get logged data in MEMORY.          

 @param[in]  Data        Logged data of MEMORY.
 @param[out] DataSize    Size of Logged data.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryReadEvent (
  IN      UINTN                            Index,
  IN OUT  UINT8                            **Data
  )
{

  if (mMemoryStorageLogNum == 0) {
    return EFI_NOT_FOUND;
  }
  
  if (Index >= mMemoryStorageLogNum) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Data = (UINT8 *)&mMemoryStorageLog[Index];
  return EFI_SUCCESS;
}

/**
 Return storage name string.

 @param[in]         String   name string of the MEMORY storage.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryGetStorageNameString (
  CHAR16                                  **String
)
{

  *String = mMemoryStorageNameString;

  return EFI_SUCCESS;
}

/**
 Return the count of event count.

 @param[in]         Count     Event count of MEMORY storage.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryGetEventCount (
  IN  OUT UINTN                           *Count
  )
{
  *Count = mMemoryStorageLogNum;
  
  return EFI_SUCCESS;
}

/**
 Locate Event log protocol to get logged data in MEMORY.

 @param[in]          DataCount   Number of data count in MEMORY storage.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryRefreshDatabase (
  IN OUT  UINTN                   *DataCount
  )
{
  EFI_STATUS                         Status;
  EFI_MEMORY_STORAGE_PROTOCOL        *MemEventLog;  
  MEMORY_STORAGE_ORGANIZATION        *EventList = NULL;
  VE_MEMORY_EVENT_LOG_STRUCTURE      MemoryEvent;
  UINTN                              EventCount;
  UINTN                              Index;

  FreeMemoryEventDatabase ();
  
  Status = gBS->LocateProtocol (&gH2OMemoryStorageProtocolGuid, NULL, (VOID **)&MemEventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  } 

  //
  // Prepare Event Log Data From Memory.
  //
  EventCount = 0;
  while (TRUE) {
    Status = MemEventLog->ReadNext ((VOID **)&EventList);
    if (EFI_ERROR (Status)) {
      break; 
    }

    if ((EventList->SensorType == 0x00) &&
        (EventList->SensorNum  == 0x00) &&
        (EventList->EventType  == 0x00)) {
      break;
    }
    EventCount++;
  }

  mMemoryStorageLogNum = EventCount;
  //
  // If there are no logs exist, return to main menu
  //
  if (mMemoryStorageLogNum == 0) {
    *DataCount = 0;
    return EFI_SUCCESS;
  }
  
  //
  // Fill in the MemoryEventLog with Memory Evet
  //
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof(STORAGE_EVENT_LOG_INFO) * mMemoryStorageLogNum, 
                          (VOID **)&mMemoryStorageLog
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (mMemoryStorageLog, sizeof(STORAGE_EVENT_LOG_INFO)*mMemoryStorageLogNum);
  
  EventList = NULL;
  Index = 0;
  while (TRUE) {
    Status = MemEventLog->ReadNext ((VOID **)&EventList);
    if (EFI_ERROR (Status)) {
      break;
    }
    if ((EventList->SensorType == 0x00) &&
        (EventList->SensorNum  == 0x00) &&
        (EventList->EventType  == 0x00)) {
      break;
    }
    MemoryEvent.EventList = EventList;

    Status = gBS->AllocatePool (
                            EfiBootServicesData, 
                            MemoryEvent.EventList->Length, 
                            (VOID **)&(mMemoryStorageLog[Index].EventRawData)
                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ZeroMem (mMemoryStorageLog[Index].EventRawData, MemoryEvent.EventList->Length);
    
    mMemoryStorageLog[Index].NextRecordId = 0;
    mMemoryStorageLog[Index].RecordId     = 0;
    mMemoryStorageLog[Index].RecordType   = 0;

    
    mMemoryStorageLog[Index].Year    = 2000 + TimeTranslator (MemoryEvent.EventList->Year);
    mMemoryStorageLog[Index].Month   = TimeTranslator (MemoryEvent.EventList->Month);
    mMemoryStorageLog[Index].Date    = TimeTranslator (MemoryEvent.EventList->Day);
    mMemoryStorageLog[Index].Hour    = TimeTranslator (MemoryEvent.EventList->Hour);
    mMemoryStorageLog[Index].Minute  = TimeTranslator (MemoryEvent.EventList->Minute);
    mMemoryStorageLog[Index].Second  = TimeTranslator (MemoryEvent.EventList->Second);

    mMemoryStorageLog[Index].GeneratorId       = 0;
    mMemoryStorageLog[Index].GeneratorIdEnable = FALSE;
    mMemoryStorageLog[Index].EvMRev            = 0;
    
    mMemoryStorageLog[Index].EventID.SensorType = MemoryEvent.EventList->SensorType;
    mMemoryStorageLog[Index].EventID.SensorNum  = MemoryEvent.EventList->SensorNum;
    mMemoryStorageLog[Index].EventID.EventType  = MemoryEvent.EventList->EventType;
    mMemoryStorageLog[Index].EventIdEnable      = TRUE;
    
    mMemoryStorageLog[Index].Data               = &(mMemoryStorageLog[Index].EventRawData[MEMORY_STORAGE_BASE_LENGTH]);

    mMemoryStorageLog[Index].EventTypeID       = 0;
    mMemoryStorageLog[Index].EventTypeIdEnable = FALSE;
    
    mMemoryStorageLog[Index].EventRawDataSize  = MemoryEvent.EventList->Length;
    CopyMem (
         (UINT8 *)mMemoryStorageLog[Index].EventRawData, 
         (UINT8 *)(MemoryEvent.EventList), 
         mMemoryStorageLog[Index].EventRawDataSize
         );
    
    Index++;
  }

  *DataCount = mMemoryStorageLogNum;
  return EFI_SUCCESS;
}

/**
 Clear log data.
          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryClearEvent (
  VOID
)
{
  EFI_STATUS                         Status;
  EFI_MEMORY_STORAGE_PROTOCOL        *MemEventLog;  

  Status = gBS->LocateProtocol (&gH2OMemoryStorageProtocolGuid, NULL, (VOID **)&MemEventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Clear MEMORY logged data.
  //
  Status = MemEventLog->Clear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mMemoryStorageLogNum == 0) {
    return EFI_SUCCESS;
  }
  //
  // Clear MEMORY event log buffer.
  //
  FreeMemoryEventDatabase ();

  return EFI_SUCCESS;
}

/**
 Translate time format to numerical value.

 @param[in]         OrgTime   Original time format.
             
 
 @retval Value of time.                 
*/
UINT8
EFIAPI
TimeTranslator (
  IN  UINT8             OrgTime
)
{
  UINT8       Time;

  Time = ((OrgTime >> 4) * 10) + (OrgTime & 0x0f);

  return Time;

}

/**
 Free Memory event data buffer.        
 
 @retval EFI Status                  
*/
VOID
EFIAPI
FreeMemoryEventDatabase (
  VOID
  )
{
  UINTN                     Index;
  
  if (mMemoryStorageLogNum != 0) { 
    //
    // Clear data of event data buffer.
    //
    for (Index = 0; Index < mMemoryStorageLogNum; Index++) {
      if (mMemoryStorageLog[Index].EventRawData != NULL) {
        gBS->FreePool (mMemoryStorageLog[Index].EventRawData);
        mMemoryStorageLog[Index].EventRawData = NULL;
      }
    }

    if (mMemoryStorageLog != NULL) {
      gBS->FreePool (mMemoryStorageLog);
      mMemoryStorageLog = NULL;
    }
    
    mMemoryStorageLogNum = 0;
  }
}

/**
 
 Adding an event data after arranging the storage.

 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
AddEventAfterArrangeEventStorage (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_MEMORY_STORAGE_PROTOCOL        *MemEventLog;  
  UINT8                              LogData[EVENT_LOG_DATA_SIZE];

  Status = gBS->LocateProtocol (&gH2OMemoryStorageProtocolGuid, NULL, (VOID **)&MemEventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  LogData[0] = OEM_SENSOR_OVERWRITE;
  LogData[1] = 0xFF;
//[-start-140213-IB08400246-modify]//
  LogData[2] = CHANGE_MEMORY_STORAGE_DATA3;
//[-end-140213-IB08400246-modify]//
  
  Status = MemEventLog->Write (
                           OEM_SENSOR_TYPE_EVENT_STORAGE,
                           BIOS_DEFINED_SENSOR_NUM,
                           EVENT_TYPE_OEM,
                           0x3,
                           LogData
                           ); 
    
  return Status;
}

/**
 Arrange the data in the storage.         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ArrangeEventStorage (
  VOID
  )
{
  EFI_STATUS               Status;

  switch (mEventLogFullOp) {
  case EVENT_LOG_FULL_OVERWRITE:
    Status = ShiftDataOfStorage();
    break;

  case EVENT_LOG_FULL_CLEAR_ALL:
    Status = MemoryClearEvent();
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  if (!EFI_ERROR (Status)) {
    //
    // Logging an event after arranging the storage.
    //
    Status = AddEventAfterArrangeEventStorage ();
  }
  
  return Status;
}

//[-start-140213-IB08400246-add]//
/**
 Translate string of Memory event log.

 @param[in]         EventInfo        Information of BIOS event log data.        
 @param[out]        EventString      Returned string of the input BIOS event log data.      
 @param[out]        StringSize       Size of EventString.      
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogStringHandler (
  IN VOID                                   *EventInfo,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  )
{
  VOID                       *TempString=NULL;
  UINTN                      Index;
  UINTN                      NumOfMemoryEventStr;
  STORAGE_EVENT_LOG_INFO     *StorageEventInfo;
  EFI_STATUS                 Status = EFI_NOT_FOUND;

  NumOfMemoryEventStr = sizeof(gMemoryEventLogString) / sizeof(MEMORY_EVENT_LOG_STRING);
  StorageEventInfo = (STORAGE_EVENT_LOG_INFO *)EventInfo; 
  
  for (Index = 0; Index < NumOfMemoryEventStr; Index++) {
    if ( (StorageEventInfo->EventID.SensorType == gMemoryEventLogString[Index].EventID.SensorType) &&  
         (StorageEventInfo->EventID.SensorNum  == gMemoryEventLogString[Index].EventID.SensorNum)  &&
         (StorageEventInfo->EventID.EventType  == gMemoryEventLogString[Index].EventID.EventType) ) {
      if (CompareMem (StorageEventInfo->Data, gMemoryEventLogString[Index].Data, gMemoryEventLogString[Index].DataLength) == 0) {
        *StringSize = StrLen (gMemoryEventLogString[Index].EventStr) * 2 + 2;
        gBS->AllocatePool (EfiBootServicesData, *StringSize, &TempString);
        StrCpy(TempString, gMemoryEventLogString[Index].EventStr);
        *EventString = TempString;  
        return EFI_SUCCESS;
      }  
    }
    
  } 

  
  return Status;
}
//[-end-140213-IB08400246-add]//

/**
 Shift event data of the storage.           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ShiftDataOfStorage (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           MemoryEventLogCount;
  UINTN                           AdjustMemoryEventLogNum;
  MEMORY_STORAGE_ORGANIZATION     *AdjustMemoryEventLogBuf;
  UINTN                           Index;
  EFI_MEMORY_STORAGE_PROTOCOL     *MemEventLog;  

  AdjustMemoryEventLogNum = 0;
  
  Status = MemoryRefreshDatabase (&MemoryEventLogCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  if (MemoryEventLogCount > EVENT_LOG_FULL_ADJUST_EVENT_NUM) {
    AdjustMemoryEventLogNum = MemoryEventLogCount - EVENT_LOG_FULL_ADJUST_EVENT_NUM;
  } else {
    //
    // If Current count of logged event is equal or less than the adjusting count,
    // Clear all event data immediately.
    //
    MemoryClearEvent();
  }
  
  //
  // Allocate a space to store the addresses of all copy of event data.
  //
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof (MEMORY_STORAGE_ORGANIZATION) * AdjustMemoryEventLogNum, 
                          (VOID **)&AdjustMemoryEventLogBuf
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (AdjustMemoryEventLogBuf, sizeof(MEMORY_STORAGE_ORGANIZATION) * AdjustMemoryEventLogNum);

  for (Index = 0; Index < AdjustMemoryEventLogNum; Index++) {
    CopyMem (
         &AdjustMemoryEventLogBuf[Index], 
         mMemoryStorageLog[Index + EVENT_LOG_FULL_ADJUST_EVENT_NUM].EventRawData, 
         mMemoryStorageLog[Index + EVENT_LOG_FULL_ADJUST_EVENT_NUM].EventRawDataSize
         );
  }

  //
  // clear original event data of the storage.
  //
  Status = MemoryClearEvent ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Restore the adjusted event data into the storage.
  //
  Status = gBS->LocateProtocol (&gH2OMemoryStorageProtocolGuid, NULL, (VOID **)&MemEventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < AdjustMemoryEventLogNum; Index++) {
    Status = MemEventLog->OverWrite (&AdjustMemoryEventLogBuf[Index]);
    if (EFI_ERROR (Status)) {
      //
      // Free the space of storing the addresses of all copy of event data.
      //
      if (AdjustMemoryEventLogBuf != NULL) {
        gBS->FreePool (AdjustMemoryEventLogBuf);
        AdjustMemoryEventLogBuf = NULL;
      }
      
      return Status;
    }
  }
  AsmWbinvd ();

  //
  // Free the space of storing the addresses of all copy of event data.
  //
  if (AdjustMemoryEventLogBuf != NULL) {
    gBS->FreePool (AdjustMemoryEventLogBuf);
    AdjustMemoryEventLogBuf = NULL;
  }
  return EFI_SUCCESS;
}

/**
 Entry point of this driver. Install Memory Event Storage protocol into DXE.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryStorageChipsetDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle=NULL;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                                      &Handle,
                                      &gH2OEventStorageProtocolGuid,
                                      EFI_NATIVE_INTERFACE,
                                      &gEventStorageProtocol
                                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-140213-IB08400246-add]//
  //
  // Install Event String Protocol for user to log Event into Memory Event Storage.
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gH2OEventLogStringProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gEventLogString
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
//[-end-140213-IB08400246-add]//
  return EFI_SUCCESS;

}

