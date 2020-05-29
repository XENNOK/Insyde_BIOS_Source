/** @file

  Event Log Dxe implementation.

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

#include <EventLogDxe.h>

EVENT_STORAGE_HANDLER_BUFFER          *mEventStorageHandlerBuf = NULL;
UINTN                                 mNumOfEventStorageHandler;

STORAGE_INFO                          mStorageInfo[] = {
                                                EVENT_STORAGE_BIOS,      L"BIOS",     TRUE,
                                                EVENT_STORAGE_BMC_SEL,   L"BMC SEL",  FALSE,
                                                EVENT_STORAGE_MEMORY ,   L"MEMORY",   TRUE
                                                };

EVENT_LOG_PROTOCOL                    gEventLogProtocol = {
                                                     LogEvent,
                                                     GetEvent,
                                                     GetEventCount,
                                                     ClearEvent,
                                                     RefreshDatabase,
                                                     GetEventStorageName
                                                     };
EVENT_LOG_FULL_OPTION                 gEventLogFullOption = EVENT_LOG_FULL_OVERWRITE;
PEI_EVENT_LOG_ORGANIZATION            *mEventLogHob;
UINTN                                 mEventLogHobNum = 0;
UINTN                                 mStorageInfoSize = sizeof (mStorageInfo);
EFI_HANDLE                            mEventLogHandle = NULL;

/**
 Locate event storage protocol when it is installing.

 @param[in]         Event                
 @param[in]         Context                           
 
 @retval VOID                 
*/
static
VOID
EFIAPI
EventStorageProtocolCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       NumberOfHandles;
  EFI_STATUS                  Status;
  UINTN                       Index;
  
  if(mEventStorageHandlerBuf != NULL) {
    gBS->FreePool (mEventStorageHandlerBuf);
    mEventStorageHandlerBuf = NULL;
  }

  //
  // Locate EventStorage handler protocol.
  //
  Status = gBS->LocateHandleBuffer (
                                ByProtocol,
                                &gH2OEventStorageProtocolGuid,
                                NULL,
                                &NumberOfHandles,
                                &HandleBuffer
                                );
  if (EFI_ERROR (Status)) {
    return;
  }    
    
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof (EVENT_STORAGE_HANDLER_BUFFER) * NumberOfHandles, 
                          (VOID **)&mEventStorageHandlerBuf
                          );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (HandleBuffer);
    return;
  }
  ZeroMem (mEventStorageHandlerBuf, sizeof (EVENT_STORAGE_HANDLER_BUFFER) * NumberOfHandles);
  
  mNumOfEventStorageHandler = 0;

  //
  // Get all the EventStorage handler instances and their individual EventStorage types.
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                              HandleBuffer[Index],
                              &gH2OEventStorageProtocolGuid,
                              (VOID **)&mEventStorageHandlerBuf[Index].EventStorage
                              );
    if (EFI_ERROR (Status)) {
      continue;
    }
    mNumOfEventStorageHandler++;

    mEventStorageHandlerBuf[Index].EventStorage->GetStorageNameString (&mEventStorageHandlerBuf[Index].StorageNameString);
  }

  gBS->FreePool (HandleBuffer);

  gBS->ReinstallProtocolInterface (
                               mEventLogHandle, 
                               &gH2OEventLogProtocolGuid,
                               &gEventLogProtocol,
                               &gEventLogProtocol
                               );
  LogPeiEventLogHobData ();
}

/**
 Log event data and event ID to the specific storage.           

 @param[in] EventStorageType        Storage type which will log event data into.
 @param[in] EventID                 ID for specific event.
 @param[in] ErrorData               Event data. 
 @param[in] ErrorDataSize           Event Data Size.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
LogEvent (
  IN  UINT8                            EventStorageType,
  IN  EVENT_TYPE_ID                    *EventID,
  IN  VOID                             *ErrorData,  
  IN  UINTN                            ErrorDataSize
  )
{
  UINTN                       Index;
  EFI_STATUS                  Status = EFI_UNSUPPORTED;
  CHAR16                      *StorageString;
  
  if (EventStorageType == EVENT_STORAGE_ALL) {
    // 
    // Log event data to all storages(Depend on how many the storage handlers were got).
    //
    for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {      
      *(mEventStorageHandlerBuf[Index].EventStorage->EventLogFullOption) = gEventLogFullOption; 
      Status = mEventStorageHandlerBuf[Index].EventStorage->WriteEvent (EventID, ErrorData, ErrorDataSize);
      if (EFI_ERROR (Status)) {        
        continue;
      } 
    }
    
  } else {  
    //
    // Log event data to the selected event storage.
    // Check the event storage type to get the right EventStorage handler.
    //

    //
    // Translate the storage type to storage name string.
    //
    Status = StorageType2String (EventStorageType, &StorageString);
    if (EFI_ERROR (Status)) {
      return Status;
    }    

    //
    // Use the name string to find the right storage handler index.
    //
    Status = FindStorageByString (StorageString, &Index);
    if (EFI_ERROR (Status)) {
      return Status;
    } 
    
    *(mEventStorageHandlerBuf[Index].EventStorage->EventLogFullOption) = gEventLogFullOption; 

    //
    // Call storage handler to log event.
    //
    Status = mEventStorageHandlerBuf[Index].EventStorage->WriteEvent (EventID, ErrorData, ErrorDataSize);
    if (EFI_ERROR (Status)) {      
      return Status;
    } else {
      //
      // Complete logging action for specific event storage.
      //
      return EFI_SUCCESS;
    }
  }
  
  return Status;
}

/**
 Get event data from the specific storage.
            
 @param[in] StorageString           Storage type which will get event data from.
 @param[in] Index                   Index of data that in selected storage.
 @param[in] Data                    Event data in the selected storage.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetEvent (
  IN      CHAR16                           *StorageString,
  IN      UINTN                            DataIndex,
  IN OUT  UINT8                            **Data
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;

  for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {
    if (!StrCmp (StorageString, mEventStorageHandlerBuf[Index].StorageNameString)) {
      Status = mEventStorageHandlerBuf[Index].EventStorage->ReadEvent (DataIndex, Data);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
  return EFI_SUCCESS;
}

/**
 Get event count from the selected storage.
            
 @param[in]  StorageString        Storage type which will log event data into.
 @param[out] EventLogCount        Event count of the selected storage.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetEventCount (
  IN      CHAR16                           *StorageString,
  IN OUT  UINTN                            *EventLogCount  
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;

  for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {
    if (!StrCmp (StorageString, mEventStorageHandlerBuf[Index].StorageNameString)) {
      Status = mEventStorageHandlerBuf[Index].EventStorage->GetEventCount (EventLogCount);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }
  return EFI_SUCCESS;

}


/**
 Update event data of the selected storage.
            
 @param[in]  StorageString        Storage type which will log event data into.
 @param[out] EventLogCount        Event count of the selected storage.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
RefreshDatabase (
  IN      CHAR16                           *StorageString,
  IN OUT  UINTN                            *EventLogCount  
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;

  for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {
    if (!StrCmp (StorageString, mEventStorageHandlerBuf[Index].StorageNameString)) {
      Status = mEventStorageHandlerBuf[Index].EventStorage->RefreshDatabase (EventLogCount);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;

}

/**
 Clear event log data from the selected storage.

 @param[in]         
 StorageString        Storage type which logged event data into.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ClearEvent (
  IN      CHAR16                           *StorageString
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;

  for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {
    if (!StrCmp (StorageString, mEventStorageHandlerBuf[Index].StorageNameString)) {
      Status = mEventStorageHandlerBuf[Index].EventStorage->ClearEvent ();
      if (EFI_ERROR (Status)) {
        return Status;
      } else {
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_UNSUPPORTED;

}

/**
 Get all the names of event storage drivers.
          
 @param[out] StorageName        Storage names.
 @param[out] StorageNum         Number of storage drivers.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetEventStorageName (
  IN OUT  EVENT_STORAGE_INFO               **StorageName,
  IN OUT  UINTN                            *StorageNum
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;
  EVENT_STORAGE_INFO           *StorageNameBuf;

  if (mNumOfEventStorageHandler == 0) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof(EVENT_STORAGE_INFO) * mNumOfEventStorageHandler,
                          (VOID **)&StorageNameBuf
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {
    StorageNameBuf[Index].EventStorageName = mEventStorageHandlerBuf[Index].StorageNameString;
  }
  
  *StorageName = StorageNameBuf;
  *StorageNum  = mNumOfEventStorageHandler;
  return EFI_SUCCESS;

}

/**
 Translate selected storage type to the related string.
            
 @param[in]  Type                    storage type.
 @param[out] String                  storage name string.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
StorageType2String (
  IN      UINT8                          Type,
  IN  OUT CHAR16                         **String
  )
{
  UINTN                     Index;

  for (Index = 0; Index < (sizeof(mStorageInfo) / sizeof(STORAGE_INFO)); Index++) {    
    if (Type == mStorageInfo[Index].EventStorageType) {
      *String = mStorageInfo[Index].StorageNameString;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
 Use the storage name string to the related handler.            

 @param[in]  String                  Storage name string.
 @param[out] HandlerIndex            Index of Event Storage Handler buffer. 
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
FindStorageByString (
  IN     CHAR16                         *String,
  IN OUT UINTN                          *HandlerIndex 
  )
{
  UINTN                     Index;

  for (Index = 0; Index < mNumOfEventStorageHandler; Index++) {   
    if (!StrCmp (String, mEventStorageHandlerBuf[Index].StorageNameString)) {      
      *HandlerIndex = Index;      
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

/**
 Initialize settings base on PCD.

 VOID         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogPolicyInit (
  VOID
  )
{
  gEventLogFullOption = PcdGet8(PcdEventLogFullOption);
  
  DEBUG((EFI_D_ERROR, "EventLogDxe: PcdEventLogFullOption: %x\n", PcdGet8(PcdEventLogFullOption)));
  return EFI_SUCCESS; 
}

/**
 Get "PeiEventHandler" HOBs, and store them.
 After Event Storage Driver is installed, we will log those data to event storage.

 VOID           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
CollectPeiEventLogHob (
  VOID
  )
{
  VOID                                 *HobList;
  VOID                                 *OrgHobList;
  VOID                                 *Buffer;
  PEI_EVENT_LOG_ORGANIZATION           *PeiEventLogHob;
  EFI_STATUS                           Status;
  UINTN                                Index;

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OrgHobList = HobList;
  mEventLogHobNum = 0;
  Buffer = NULL;
  for (;;) {
    Buffer = GetNextGuidHob (&gH2OPeiEventLogHobGuid, HobList);
    if (Buffer == NULL) {
      break;
    }
    mEventLogHobNum++;
    HobList = GET_NEXT_HOB (Buffer);
  }

  //
  // Allocate pool for store Pei Event Handler Hobs.
  //
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof(PEI_EVENT_LOG_ORGANIZATION) * mEventLogHobNum,
                          (VOID **)&mEventLogHob
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (mEventLogHob, sizeof (PEI_EVENT_LOG_ORGANIZATION) * mEventLogHobNum);

  //
  // Get all PeiEventHandler HOBs, and store them.
  //
  HobList = OrgHobList;
  Buffer = NULL;
  for (Index = 0; Index < mEventLogHobNum; Index++) {
    Buffer = GetNextGuidHob (&gH2OPeiEventLogHobGuid, HobList);
    if (Buffer == NULL) {
      break;
    }
    PeiEventLogHob = (PEI_EVENT_LOG_ORGANIZATION *)((UINT8 *)Buffer + sizeof (EFI_HOB_GUID_TYPE));
    CopyMem (&mEventLogHob[Index], PeiEventLogHob, PeiEventLogHob->Length);
    HobList = GET_NEXT_HOB (Buffer);
  }
  
  return EFI_SUCCESS;
}

/**
 Log HOB data into the event storage.
 
 @param[in] EventStorageHandlerIndex  Index of mEventStorageHandlerBuf to call the selected Event Storage handler.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
LogHobToEventStorage (
  UINTN                    EventStorageHandlerIndex
  )
{
  UINTN                                EventLogHobIndex;  
  EFI_STATUS                           Status;

  for (EventLogHobIndex = 0; EventLogHobIndex < mEventLogHobNum; EventLogHobIndex++) {
    *(mEventStorageHandlerBuf[EventStorageHandlerIndex].EventStorage->EventLogFullOption) = gEventLogFullOption; 
    //
    // Call storage handler to log event.
    //
    Status = mEventStorageHandlerBuf[EventStorageHandlerIndex].EventStorage->WriteEvent (
                                                                              (EVENT_TYPE_ID *)&mEventLogHob[EventLogHobIndex].Data[0], 
                                                                              &(mEventLogHob[EventLogHobIndex].Data[3]), 
                                                                              (mEventLogHob[EventLogHobIndex].Length - PEI_EVENT_LOG_BASE_LENGTH - sizeof(EVENT_TYPE_ID))
                                                                              );
    if (EFI_ERROR (Status)) {      
      return Status;
    } 
  }

  return EFI_SUCCESS;
}

/**
 Log HOB data into the event storage.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
LogPeiEventLogHobData (
  VOID
  )
{
  UINTN                                StorageInfoIndex;
  UINTN                                EventStorageHandlerIndex;

  for (StorageInfoIndex = 0; StorageInfoIndex < (mStorageInfoSize / sizeof (STORAGE_INFO)); StorageInfoIndex++) {
    if (mStorageInfo[StorageInfoIndex].LogPeiEventLogHob == TRUE) {
      for (EventStorageHandlerIndex = 0; EventStorageHandlerIndex < mNumOfEventStorageHandler; EventStorageHandlerIndex++) {
        if (!StrCmp (mStorageInfo[StorageInfoIndex].StorageNameString, mEventStorageHandlerBuf[EventStorageHandlerIndex].StorageNameString)) {
          //
          // Find the Event Storage handler whose Event Storage needs to log the PEI Event Log HOB.
          //
          LogHobToEventStorage (EventStorageHandlerIndex);
          mStorageInfo[StorageInfoIndex].LogPeiEventLogHob = FALSE;
          break;
        }
      }
    }
  }
  
  return EFI_SUCCESS;
}

/**
 Entry point of this driver. Install Event Log protocol into DXE.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogDxeEntryPoint ( 
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_EVENT                   EventStorageProtocolEvent;
  VOID                        *EventStorageRegistration;

  //
  // Register callback for loading Event storage driver.
  //
  Status = gBS->CreateEvent (
                         EVT_NOTIFY_SIGNAL,
                         TPL_CALLBACK,
                         EventStorageProtocolCallback,
                         NULL,
                         &EventStorageProtocolEvent
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  }
    
  //
  // Register an event that is to be signaled whenever an 
  // interface is installed for a specified protocol. 
  //
  Status = gBS->RegisterProtocolNotify (
                                    &gH2OEventStorageProtocolGuid,
                                    EventStorageProtocolEvent,
                                    &EventStorageRegistration
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get policy settings from PCD.
  //
  Status = EventLogPolicyInit();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get PeiEventHandler Hobs and store them in temp buffer.
  // Once the EventStorage is installed, then log the data of the buffer to EventStorage.
  //
  CollectPeiEventLogHob ();

  //
  // Install Event Log Dxe protocol.
  //
  Status = gBS->InstallProtocolInterface (
                                    &mEventLogHandle,
                                    &gH2OEventLogProtocolGuid,
                                    EFI_NATIVE_INTERFACE,
                                    &gEventLogProtocol
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  return Status;
}

