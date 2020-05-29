/** @file

  Event Log Smm implementation.

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

#include <EventLogSmm.h>

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
UINTN                                 mStorageInfoSize = sizeof (mStorageInfo);

EFI_HANDLE                            mEventLogHandle = NULL;

/**
 Locate event storage protocol when it is installing.

 @param[in]         Protocol                
 @param[in]         Interface                           
 @param[in]         Handle
 
 @retval VOID                 
*/
EFI_STATUS
EFIAPI
EventStorageProtocolCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       NumberOfHandles;
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       BufferSize;
  
  if(mEventStorageHandlerBuf != NULL) {
    gSmst->SmmFreePool (mEventStorageHandlerBuf);
    mEventStorageHandlerBuf = NULL;
  }

  //
  // Locate EventStorage handler protocol.
  //
  BufferSize      = 0;
  NumberOfHandles = 0;
  HandleBuffer    = NULL;
  Status = gSmst->SmmLocateHandle (
                               ByProtocol,
                               &gH2OSmmEventStorageProtocolGuid,
                               NULL,
                               &BufferSize,
                               HandleBuffer
                               );
  if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }    
  
  Status = gSmst->SmmAllocatePool (
                               EfiRuntimeServicesData,
                               BufferSize,
                               (VOID **)&HandleBuffer
                               );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateHandle (
                               ByProtocol,
                               &gH2OSmmEventStorageProtocolGuid,
                               NULL,
                               &BufferSize,
                               HandleBuffer
                               );
  if (EFI_ERROR (Status)) {
    gSmst->SmmFreePool (HandleBuffer);
    return Status;
  }

  NumberOfHandles = BufferSize / sizeof(EFI_HANDLE);
  
  Status = gSmst->SmmAllocatePool (
                               EfiRuntimeServicesData,
                               sizeof (EVENT_STORAGE_HANDLER_BUFFER) * NumberOfHandles,
                               (VOID **)&mEventStorageHandlerBuf
                               );
  if (EFI_ERROR (Status)) {
    gSmst->SmmFreePool (HandleBuffer);
    return Status;
  }
  ZeroMem (mEventStorageHandlerBuf, sizeof (EVENT_STORAGE_HANDLER_BUFFER) * NumberOfHandles);
  
  mNumOfEventStorageHandler = 0;

  //
  // Get all the EventStorage handler instances and their individual EventStorage types.
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {      
    Status = gSmst->SmmHandleProtocol (
                                   HandleBuffer[Index],
                                   &gH2OSmmEventStorageProtocolGuid,
                                   (VOID **)&mEventStorageHandlerBuf[Index].EventStorage
                                   );
    if (EFI_ERROR (Status)) {
      continue;
    }
    mNumOfEventStorageHandler++;
    
    mEventStorageHandlerBuf[Index].EventStorage->GetStorageNameString (&mEventStorageHandlerBuf[Index].StorageNameString);
  }

  gSmst->SmmFreePool (HandleBuffer);

  //
  // Reinstall the protocol if some drivers need this protocol notification.
  //
  Status = gSmst->SmmUninstallProtocolInterface (
                                             mEventLogHandle,
                                             &gH2OSmmEventLogProtocolGuid,
                                             &gEventLogProtocol
                                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mEventLogHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                                           &mEventLogHandle,
                                           &gH2OSmmEventLogProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           &gEventLogProtocol
                                           );
  return Status;
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

  Status = gSmst->SmmAllocatePool (
                               EfiRuntimeServicesData,
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
  
  DEBUG((EFI_D_ERROR, "EventLogSmm: PcdEventLogFullOption: %x\n", PcdGet8(PcdEventLogFullOption)));

  return EFI_SUCCESS;  
  
}

/**
 Entry point of this driver. Install Event Handler protocol into SMM.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogSmmEntryPoint ( 
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_EVENT                   EventStorageProtocolEvent;

  //
  // Register callback for loading Event storage driver.
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                                         &gH2OSmmEventStorageProtocolGuid,
                                         EventStorageProtocolCallback,
                                         &EventStorageProtocolEvent
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
  // Install Event Log SMM protocol.
  //
  Status = gSmst->SmmInstallProtocolInterface (
                                           &mEventLogHandle,
                                           &gH2OSmmEventLogProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           &gEventLogProtocol
                                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  return Status;
}

