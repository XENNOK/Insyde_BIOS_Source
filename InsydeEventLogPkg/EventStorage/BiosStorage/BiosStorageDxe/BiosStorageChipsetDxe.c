/** @file

  BIOS Storage Chipset Dxe implementation.

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

#include <BiosStorageChipsetDxe.h>

STORAGE_EVENT_LOG_INFO                *mBiosStorage;
UINTN                                 mBiosStorageNum = 0;
CHAR16                                *mBiosStorageNameString = L"BIOS";
EVENT_LOG_FULL_OPTION                 mEventLogFullOp = EVENT_LOG_FULL_OVERWRITE;
EVENT_STORAGE_PROTOCOL                gEventStorageProtocol = {
                                                         BiosWriteEvent,
                                                         BiosReadEvent,
                                                         BiosGetStorageNameString,
                                                         BiosGetEventCount,
                                                         BiosClearEvent,
                                                         BiosRefreshDatabase,
                                                         &mEventLogFullOp
                                                         };

H2O_EVENT_LOG_STRING_PROTOCOL         gEventLogString = {
  EventLogStringHandler
  };

//[-start-140213-IB08400246-modify]//
BIOS_EVENT_LOG_STRING                 gBiosStorageChipsetString[] = {
                                        {0x80, 0x8, {0x0C, 0x80,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0}, L"Total memory size has been changed."},
                                        {0x80, 0x6, {OEM_SENSOR_TYPE_EVENT_STORAGE, BIOS_DEFINED_SENSOR_NUM, EVENT_TYPE_OEM, OEM_SENSOR_OVERWRITE, 0xFF, CHANGE_BIOS_STORAGE_DATA3}, L"BIOS Event Storage has been changed."}
                                        };
//[-end-140213-IB08400246-modify]//
/**
 Locate event log protocol to log data.

 @param[in]         EventID       ID for specific event.         
 @param[in]         Data          Data that will log into BIOS storage.     
 @param[in]         DataSize      Size of Data.       
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosWriteEvent (
  IN  EVENT_TYPE_ID                    *EventID,
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
  )
{
  EFI_STATUS                         Status;
  EFI_BIOS_EVENT_LOG_PROTOCOL        *EventLog;  
  UINTN                              LocalDataSize=0;
  UINT8                              *Buffer=NULL;
  UINTN                              EventIdNum;

  EventLog = NULL;  
  EventIdNum = sizeof(EVENT_TYPE_ID);

  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // "3" bytes are for SensorType, SensorNum and EventType information.
  //
  LocalDataSize = DataSize + EventIdNum;
  
  Status = gBS->AllocatePool (
                          EfiBootServicesData,
                          LocalDataSize,
                          (VOID **)&Buffer
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  ZeroMem (Buffer, LocalDataSize);

  //
  // Loop for even fulled state
  //
  do {
    Buffer[0] = EventID->SensorType;
    Buffer[1] = EventID->SensorNum;
    Buffer[2] = EventID->EventType;
    
    CopyMem (&Buffer[3], Data, DataSize);
    
    Status = EventLog->Write (
                          EventLog,
                          0x80,
                          0,
                          0,
                          LocalDataSize,
                          Buffer
                          );    
    if (Status == EFI_OUT_OF_RESOURCES) {
      //
      // It means the storage is full of data, 
      // we need to adjust the storage base on the policy of 
      // "Event Log Full option" setting in PCD.
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
      // After EventLog->Write(),
      // return if Status == EFI_SUCCESS or OTHER ERROR status beside EFI_OUT_OF_RESOURCE.
      //
      return Status;
    } 
  } while (TRUE);
}

/**
 Locate Event log protocol to get logged data in BIOS.

 @param[in]         Index       Logged data of BIOS.                      
 @param[out]        Data        Size of Logged data.     
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosReadEvent (
  IN      UINTN                            Index,
  IN OUT  UINT8                            **Data
  )
{

  if (mBiosStorageNum == 0) {
    return EFI_NOT_FOUND;
  }
  
  if (Index >= mBiosStorageNum) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Data = (UINT8 *)&mBiosStorage[Index];
  return EFI_SUCCESS;
}


/**
 Return storage name string.
             
 @param[out]        String     Name string of the BIOS storage.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosGetStorageNameString (
  CHAR16                                  **String
)
{

  *String = mBiosStorageNameString;

  return EFI_SUCCESS;
}


/**
 Return the count of event count.
            
 @param[out]        Count     Event count of BIOS storage.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosGetEventCount (
  IN  OUT UINTN                           *Count
  )
{
  *Count = mBiosStorageNum;
  
  return EFI_SUCCESS;
}

/**
 Locate Event log protocol to get logged data in BIOS.
              
 @param[out]        DataCount     Number of data count in BIOS storage.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosRefreshDatabase (
  IN OUT  UINTN                   *DataCount
  )
{
  EFI_STATUS                         Status;
  EFI_BIOS_EVENT_LOG_PROTOCOL        *EventLog;  
  BIOS_EVENT_LOG_ORGANIZATION        *EventList = NULL;
  VE_BIOS_EVENT_LOG_STRUCTURE        BiosEvent;
  UINTN                              EventCount = 0;
  UINTN                              Index;

  FreeBiosEventDatabase ();
  
  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  } 

  //
  // Prepare Event Log Data From BIOS ROM Part
  //
  while (TRUE) {
    Status = EventLog->ReadNext (EventLog, (VOID **)&EventList);
    if (EFI_ERROR (Status) || (EventList->Type == 0xFF) && (EventList->Length == 0xFF)) {
      break; 
    }
    EventCount++;
  }

  mBiosStorageNum = EventCount;
  //
  // If there are no logs exist, return to main menu
  //
  if (mBiosStorageNum == 0) {
    *DataCount = 0;
    return EFI_SUCCESS;
  }

  //
  // Fill in the BiosStorageChipset with BiosEvet
  //
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof(STORAGE_EVENT_LOG_INFO) * mBiosStorageNum, (VOID **)&mBiosStorage);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (mBiosStorage, sizeof(STORAGE_EVENT_LOG_INFO) * mBiosStorageNum);
  
  EventList = NULL;
  Index = 0;
  while (TRUE) {
    Status = EventLog->ReadNext (EventLog, (VOID **)&EventList);
    if (EFI_ERROR (Status) || (EventList->Type == 0xFF) && (EventList->Length == 0xFF)) {
      break;
    }
    BiosEvent.EventList = EventList;

    Status = gBS->AllocatePool (
                            EfiBootServicesData, 
                            BiosEvent.EventList->Length, 
                            (VOID **)&(mBiosStorage[Index].EventRawData)
                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ZeroMem (mBiosStorage[Index].EventRawData, BiosEvent.EventList->Length);
    
    mBiosStorage[Index].NextRecordId = 0;
    mBiosStorage[Index].RecordId     = 0;
    mBiosStorage[Index].RecordType   = 0;

    
    mBiosStorage[Index].Year    = 2000 + TimeTranslator (BiosEvent.EventList->Year);
    mBiosStorage[Index].Month   = TimeTranslator (BiosEvent.EventList->Month);
    mBiosStorage[Index].Date    = TimeTranslator (BiosEvent.EventList->Day);
    mBiosStorage[Index].Hour    = TimeTranslator (BiosEvent.EventList->Hour);
    mBiosStorage[Index].Minute  = TimeTranslator (BiosEvent.EventList->Minute);
    mBiosStorage[Index].Second  = TimeTranslator (BiosEvent.EventList->Second);

    mBiosStorage[Index].GeneratorId       = 0;
    mBiosStorage[Index].GeneratorIdEnable = FALSE;
    mBiosStorage[Index].EvMRev            = 0;
    
    if (BiosEvent.EventList->Length > (sizeof(BIOS_EVENT_LOG_ORGANIZATION) - EVENT_LOG_DATA_SIZE)) {
      mBiosStorage[Index].EventID.SensorType = BiosEvent.EventList->Data[0];
      mBiosStorage[Index].EventID.SensorNum  = BiosEvent.EventList->Data[1];
      mBiosStorage[Index].EventID.EventType  = BiosEvent.EventList->Data[2];
      mBiosStorage[Index].Data               = &(mBiosStorage[Index].EventRawData[11]);
    } else {
      mBiosStorage[Index].EventID.SensorType = 0xFF;
      mBiosStorage[Index].EventID.SensorNum  = 0xFF;
      mBiosStorage[Index].EventID.EventType  = 0xFF;
      mBiosStorage[Index].Data               = NULL;
    }
   
    mBiosStorage[Index].EventIdEnable      = TRUE;
    mBiosStorage[Index].EventTypeID       = BiosEvent.EventList->Type;
    mBiosStorage[Index].EventTypeIdEnable = TRUE;
    mBiosStorage[Index].EventRawDataSize  = BiosEvent.EventList->Length;
    CopyMem (
         (UINT8 *)mBiosStorage[Index].EventRawData, 
         (UINT8 *)&(BiosEvent.EventList->Type), 
         mBiosStorage[Index].EventRawDataSize
         );
    
    Index++;
  }


  *DataCount = mBiosStorageNum;
  return EFI_SUCCESS;
}

/**
 Clear logged data.

 Void           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosClearEvent (
  VOID
)
{
  EFI_STATUS                         Status;
  EFI_BIOS_EVENT_LOG_PROTOCOL        *EventLog;  

  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Clear BIOS logged data.
  //
  Status = EventLog->Clear (EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mBiosStorageNum == 0) {
    return EFI_SUCCESS;
  }
  //
  // Clear BIOS event log buffer.
  //
  FreeBiosEventDatabase ();
  return EFI_SUCCESS;
}

/**
 Translate time format to numerical value.

 @param[in]         OrgTime    Original time format.                       
 
 @retval UINT8      Time                  
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
 Free BIOS event data buffer.

 VOID           
 
 @retval VOID            
*/
VOID
EFIAPI
FreeBiosEventDatabase (
  VOID
  )
{
  UINTN                     Index;
  
  if (mBiosStorageNum != 0) { 
    //
    // Clear data of event data buffer.
    //
    for (Index = 0; Index < mBiosStorageNum; Index++) {
       if (mBiosStorage[Index].EventRawData != NULL) {
          gBS->FreePool (mBiosStorage[Index].EventRawData);
          mBiosStorage[Index].EventRawData = NULL;
       } 
    }

    if (mBiosStorage != NULL) {
      gBS->FreePool (mBiosStorage);
      mBiosStorage = NULL;
    }
    mBiosStorageNum = 0;
  }
}

/**
 Adding an event data after arranging the storage.

 VOID
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
AddEventAfterArrangeEventStorage (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_BIOS_EVENT_LOG_PROTOCOL        *EventLog;  
  UINT8                              LogData[EVENT_LOG_DATA_SIZE];

  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  LogData[0] = OEM_SENSOR_TYPE_EVENT_STORAGE;
  LogData[1] = BIOS_DEFINED_SENSOR_NUM;
  LogData[2] = EVENT_TYPE_OEM;

  LogData[3] = OEM_SENSOR_OVERWRITE;
  LogData[4] = 0xFF;
//[-start-140213-IB08400246-modify]//
  LogData[5] = CHANGE_BIOS_STORAGE_DATA3;
//[-end-140213-IB08400246-modify]//
  
  Status = EventLog->Write (
                        EventLog,
                        0x80,
                        0,
                        0,
                        EVENT_LOG_DATA_SIZE,
                        LogData
                        ); 
    
  return Status;
}

/**
 Arrange the data in the storage.

 VOID           
 
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
    Status = BiosClearEvent();
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

/**
 Shift event data of the storage.

 VOID          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ShiftDataOfStorage (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           BiosStorageChipsetCount;
  UINTN                           AdjustBiosStorageChipsetNum = 0;
  BIOS_EVENT_LOG_ORGANIZATION     *AdjustBiosStorageChipsetBuf;
  UINTN                           Index;
  EFI_BIOS_EVENT_LOG_PROTOCOL     *EventLog;  

  Status = BiosRefreshDatabase (&BiosStorageChipsetCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  if (BiosStorageChipsetCount > EVENT_LOG_FULL_ADJUST_EVENT_NUM) {
    AdjustBiosStorageChipsetNum = BiosStorageChipsetCount - EVENT_LOG_FULL_ADJUST_EVENT_NUM;
  } else {
    //
    // If Current count of logged event is equal or less than the adjusting count,
    // Clear all event data immediately.
    //
    BiosClearEvent();
  }
  
  //
  // Allocate a space to store the addresses of all copy of event data.
  //
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof (BIOS_EVENT_LOG_ORGANIZATION) * AdjustBiosStorageChipsetNum, 
                          (VOID **)&AdjustBiosStorageChipsetBuf
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (AdjustBiosStorageChipsetBuf, sizeof(BIOS_EVENT_LOG_ORGANIZATION) * AdjustBiosStorageChipsetNum);

  for (Index = 0; Index < AdjustBiosStorageChipsetNum; Index++) {
    CopyMem (
         &AdjustBiosStorageChipsetBuf[Index], 
         mBiosStorage[Index + EVENT_LOG_FULL_ADJUST_EVENT_NUM].EventRawData, 
         mBiosStorage[Index + EVENT_LOG_FULL_ADJUST_EVENT_NUM].EventRawDataSize
         );
  }

  //
  // clear original event data of the storage.
  //
  Status = BiosClearEvent ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Restore the adjusted event data into the storage.
  //
  Status = gBS->LocateProtocol (&gEfiBiosEventLogProtocolGuid, NULL, (VOID **)&EventLog);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < AdjustBiosStorageChipsetNum; Index++) {
    Status = EventLog->OverWrite (EventLog, &AdjustBiosStorageChipsetBuf[Index]);
    if (EFI_ERROR (Status)) {
      //
      // Free the space of storing the addresses of all copy of event data.
      //
      if (AdjustBiosStorageChipsetBuf != NULL) {
        gBS->FreePool (AdjustBiosStorageChipsetBuf);
        AdjustBiosStorageChipsetBuf = NULL;
      }
      return Status;
    }
  }
  
  //
  // Free the space of storing the addresses of all copy of event data.
  //
  if (AdjustBiosStorageChipsetBuf != NULL) {
    gBS->FreePool (AdjustBiosStorageChipsetBuf);
    AdjustBiosStorageChipsetBuf = NULL;
  }
  
  return EFI_SUCCESS;
}

/**
 Translate string of BIOS event log.

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
  EFI_STATUS                            Status = EFI_NOT_FOUND;
  STORAGE_EVENT_LOG_INFO                *StorageEventInfo;
  
  StorageEventInfo = (STORAGE_EVENT_LOG_INFO*) EventInfo;
  if ((StorageEventInfo->EventTypeIdEnable != TRUE)) {
    //
    // Not BIOS event log event, just return not found.
    //
    return EFI_NOT_FOUND;
  }

  switch (StorageEventInfo->EventTypeID) {
  case EfiEventLogTypeLogAreaResetCleared:
    Status = BiosEventLogStringType16 (EventInfo, EventString, StringSize);
    break;
    
  default:
    Status = BiosEventLogStringType80 (EventInfo, EventString, StringSize);
    break;
  }
  
  return Status;
}

/**
 Translate string of BIOS event log for Event Type is 0x16.          

 @param[in]  EventInfo    Information of BIOS event log data.
 @param[out] EventString  Returned string of the input BIOS event log data.
 @param[out] StringSize   Size of EventString.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosEventLogStringType16 (
  IN VOID                                   *EventInfo OPTIONAL,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  )
{
  VOID                       *TempString = NULL;
  CHAR16                     *EventStr = L"Event Log has been Cleared.";

  *StringSize = StrLen (EventStr) * 2 + 2;
  gBS->AllocatePool (EfiBootServicesData, *StringSize, (VOID **)&TempString);
  StrCpy(TempString, EventStr);

  *EventString = TempString;
  return EFI_SUCCESS;
}

/**
 Translate string of BIOS event log for Event Type is 0x80.           

 @param[in]   EventInfo     Information of BIOS event log data.
 @param[out]  EventString   Returned string of the input BIOS event log data.
 @param[out]  StringSize    Size of EventString.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosEventLogStringType80 (
  IN VOID                                   *EventInfo OPTIONAL,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
  )
{
  VOID                       *TempString=NULL;
  UINTN                      Index;
  UINTN                      NumOfBiosEventStr;
  STORAGE_EVENT_LOG_INFO     *StorageEventInfo;
  EFI_STATUS                 Status = EFI_NOT_FOUND;

  NumOfBiosEventStr = sizeof(gBiosStorageChipsetString) / sizeof(BIOS_EVENT_LOG_STRING);
  StorageEventInfo = (STORAGE_EVENT_LOG_INFO *)EventInfo; 

  if (StorageEventInfo->EventTypeID != 0x80) {
    return EFI_NOT_FOUND;
  }
  
  for (Index = 0; Index < NumOfBiosEventStr; Index++) {
    if (CompareMem(&(StorageEventInfo->EventRawData[8]), gBiosStorageChipsetString[Index].Data, gBiosStorageChipsetString[Index].ValidDataLength) == 0) {
      *StringSize = StrLen (gBiosStorageChipsetString[Index].EventStr) * 2 + 2;
      gBS->AllocatePool (EfiBootServicesData, *StringSize, (VOID **)&TempString);
      StrCpy(TempString, gBiosStorageChipsetString[Index].EventStr);
      *EventString = TempString;  
      return EFI_SUCCESS;
    }
  } 

  return Status;
}


/**
 Entry point of this driver. Install Bios Event Storage protocol into DXE.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BiosStorageChipsetDxeEntryPoint (
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

  //
  // Install Event String Protocol for user to log Event into BIOS ROM.
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
     
  return EFI_SUCCESS;

}

