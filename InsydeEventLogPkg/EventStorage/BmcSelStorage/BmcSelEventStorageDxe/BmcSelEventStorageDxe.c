/** @file

  BMC SEL Event Storage DXE implementation.

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

#include <BmcSelEventStorageDxe.h>

STORAGE_EVENT_LOG_INFO                *mBmcSelEventLog;
UINTN                                 mBmcSelEventLogNum = 0;
CHAR16                                *mBmcSelStorageNameString = L"BMC SEL";
UINTN                                 mDaysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
EVENT_LOG_FULL_OPTION                 mEventLogFullOp = EVENT_LOG_FULL_OVERWRITE;                
EVENT_STORAGE_PROTOCOL                gEventStorageProtocol = {
                                                         BmcSelWriteEvent,
                                                         BmcSelReadEvent,
                                                         BmcSelGetStorageNameString,
                                                         BmcSelGetEventCount,
                                                         BmcSelClearEvent,
                                                         BmcSelRefreshDatabase,
                                                         &mEventLogFullOp
                                                         };
//[-start-140213-IB08400246-add]//
H2O_EVENT_LOG_STRING_PROTOCOL         gEventLogString = {
  EventLogStringHandler
  };

BMC_SEL_EVENT_LOG_STRING              gBmcSelEventLogString[] = {
                                                       { {OEM_SENSOR_TYPE_EVENT_STORAGE, BIOS_DEFINED_SENSOR_NUM, EVENT_TYPE_OEM},  // Event ID, 3 bytes
                                                         {OEM_SENSOR_OVERWRITE, 0xFF, CHANGE_BMC_SEL_STORAGE_DATA3},                 // Data, at most 8 bytes.
                                                         0x3, // Length of Data 
                                                         L"BMC SEL Event Storage has been changed." // String of the event
                                                       }
                                                     };

//[-end-140213-IB08400246-add]//

/**
  Locate H2O IPMI interface protocol to log data.

 @param[in]         EventID       ID for specific event.         
 @param[in]         Data          Data that will log into BMC SEL storage.     
 @param[in]         DataSize      Size of Data.       
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelWriteEvent (
  IN  EVENT_TYPE_ID                    *EventID,
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
  )
{
  EFI_STATUS                      Status;
  H2O_IPMI_INTERFACE_PROTOCOL     *IpmiTransport;
  H2O_IPMI_CMD_HEADER             Request = {H2O_IPMI_BMC_LUN,
                                             H2O_IPMI_NETFN_SENSOR_EVENT,
                                             H2O_IPMI_CMD_EVENT_MESSAGE
                                             };
  UINT8                           RecvBuf[MAX_BUFFER_SIZE];
  UINT8                           RecvSize;    
  UINT8                           LogData[EVENT_LOG_DATA_SIZE];
  BOOLEAN                         LogFull = FALSE;

  if (DataSize > 3) {
    //
    // This type of SEL didn't support more than 3 bytes of data
    //
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&IpmiTransport);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = CheckEventLogFull (&LogFull);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (LogFull == TRUE) {    
    if (mEventLogFullOp == EVENT_LOG_FULL_STOP_LOGGING) {
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
  }

  LogData[0] = BIOS_SOFTWARE_ID;
  LogData[1] = EVENT_REV;
  LogData[2] = EventID->SensorType;
  LogData[3] = EventID->SensorNum;
  LogData[4] = EventID->EventType;

  CopyMem (&LogData[5], Data, DataSize);
  
  Status = IpmiTransport->ExecuteIpmiCmd (
                                      IpmiTransport,
                                      Request,
                                      LogData,
                                      EVENT_LOG_DATA_SIZE,
                                      RecvBuf,
                                      &RecvSize,
                                      NULL
                                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  return EFI_SUCCESS;
}

/**
 Locate Event log protocol to get logged data in BMC SEL.

 @param[in]         Index       Logged data of BIOS.                      
 @param[out]        Data        Size of Logged data.     
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelReadEvent (
  IN      UINTN                            Index,
  IN OUT  UINT8                            **Data
  )
{
  if (mBmcSelEventLogNum == 0) {
    return EFI_NOT_FOUND;
  }
  
  if (Index >= mBmcSelEventLogNum) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Data = (UINT8 *)&mBmcSelEventLog[Index];

  return EFI_SUCCESS;
}


/**
 Return storage name string.
             
 @param[out]        String     Name string of the BMC SEL storage.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelGetStorageNameString (
  CHAR16                                  **String
)
{

  *String = mBmcSelStorageNameString;

  return EFI_SUCCESS;
}


/**
 Return the count of event count.
            
 @param[out]        Count     Event count of BMC SEL storage.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelGetEventCount (
  IN  OUT UINTN                           *Count
  )
{
  *Count = mBmcSelEventLogNum;
  
  return EFI_SUCCESS;
}

/**
 Locate Event log protocol to get logged data in BMC SEL.
              
 @param[out]        DataCount     Number of data count in BIOS storage.        
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelRefreshDatabase (
  IN OUT  UINTN                   *DataCount
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  H2O_IPMI_INTERFACE_PROTOCOL           *IpmiTransport;
  H2O_IPMI_SEL_INFO                     *IpmiSelInfo;
  UINTN                                 RetryCount;
  UINT16                                NextRecordId;
  UINT8                                 RecvSize = MAX_BUFFER_SIZE;
  UINT8                                 RecvBuf[MAX_BUFFER_SIZE];
  UINTN                                 BmcSelCount = 0;
  H2O_IPMI_SEL_ENTRY                    BmcEvent;
  EFI_TIME                              EfiTime;  

  FreeBmcSelEventDatabase ();

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&IpmiTransport);
  if(EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // Prepare Event Log Data From BMC SEL
  //
  RecvSize = MAX_BUFFER_SIZE;
  Status = GetBmcSelInfo (IpmiTransport, RecvBuf, &RecvSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  IpmiSelInfo = (H2O_IPMI_SEL_INFO *)RecvBuf;
  
  mBmcSelEventLogNum = IpmiSelInfo->SelEntries;
  //
  // If there are no logs exist, return to main menu
  //
  if (mBmcSelEventLogNum == 0) {
    *DataCount = 0;
    return EFI_SUCCESS;
  }

  //
  // Fill in the BmcSelEventLog with BmcSelEvet
  //
  Status = gBS->AllocatePool (
                          EfiBootServicesData, 
                          sizeof(STORAGE_EVENT_LOG_INFO) * mBmcSelEventLogNum, 
                          (VOID **)&mBmcSelEventLog
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (mBmcSelEventLog, sizeof(STORAGE_EVENT_LOG_INFO) * mBmcSelEventLogNum);

  NextRecordId = 0;
  for (BmcSelCount = 0; NextRecordId != 0xFFFF; BmcSelCount++) {
    RecvSize = MAX_BUFFER_SIZE;
    Status = GetBmcSelLog (IpmiTransport, NextRecordId, RecvBuf, &RecvSize);
    if (EFI_ERROR (Status)) {
      RetryCount = 0;
      while (RetryCount < MAX_RETRY_COUNT) {
        StallForRetry (ONE_SECOND / 2);  
        //
        // Get BMC SEL event data again.
        //
        Status = GetBmcSelLog (IpmiTransport, NextRecordId, RecvBuf, &RecvSize);
        if (!EFI_ERROR (Status)) {
          break;
        }
        RetryCount++;
      }

      //
      // After retrying 3 times, we still could not get the right data from BMC SEL.
      //
      if (RetryCount >= MAX_RETRY_COUNT) {
        FreeBmcSelEventDatabase ();
        return Status;
      }
    }
    
    CopyMem (&BmcEvent, RecvBuf, RecvSize);
    NextRecordId = BmcEvent.NextRecordId;
    
    Status = gBS->AllocatePool (
                            EfiBootServicesData, 
                            sizeof(H2O_IPMI_SEL_ENTRY) - 2, 
                            (VOID **)&(mBmcSelEventLog[BmcSelCount].EventRawData)
                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }    
    ZeroMem (mBmcSelEventLog[BmcSelCount].EventRawData, sizeof(H2O_IPMI_SEL_ENTRY) - 2);
    
    mBmcSelEventLog[BmcSelCount].NextRecordId = BmcEvent.NextRecordId;
    mBmcSelEventLog[BmcSelCount].RecordId     = BmcEvent.Data.RecordId;
    mBmcSelEventLog[BmcSelCount].RecordType   = BmcEvent.Data.RecordType;

    Ts2et (BmcEvent.Data.TimeStamp, &EfiTime);
    mBmcSelEventLog[BmcSelCount].Year    = EfiTime.Year;
    mBmcSelEventLog[BmcSelCount].Month   = EfiTime.Month;
    mBmcSelEventLog[BmcSelCount].Date    = EfiTime.Day;
    mBmcSelEventLog[BmcSelCount].Hour    = EfiTime.Hour;
    mBmcSelEventLog[BmcSelCount].Minute  = EfiTime.Minute;
    mBmcSelEventLog[BmcSelCount].Second  = EfiTime.Second;

    mBmcSelEventLog[BmcSelCount].GeneratorId        = BmcEvent.Data.GeneratorId;
    mBmcSelEventLog[BmcSelCount].GeneratorIdEnable  = TRUE;
    mBmcSelEventLog[BmcSelCount].EvMRev             = BmcEvent.Data.EvMRev;
    
    mBmcSelEventLog[BmcSelCount].EventID.SensorType = BmcEvent.Data.SensorType;
    mBmcSelEventLog[BmcSelCount].EventID.SensorNum  = BmcEvent.Data.SensorNum;
    mBmcSelEventLog[BmcSelCount].EventID.EventType  = (BmcEvent.Data.EventType) + (BmcEvent.Data.EventDir << 7);
    mBmcSelEventLog[BmcSelCount].EventIdEnable      = FALSE;
    
    mBmcSelEventLog[BmcSelCount].Data               = &(mBmcSelEventLog[BmcSelCount].EventRawData[13]);

    mBmcSelEventLog[BmcSelCount].EventTypeID        = 0;
    mBmcSelEventLog[BmcSelCount].EventTypeIdEnable  = FALSE;
    
    mBmcSelEventLog[BmcSelCount].EventRawDataSize   = sizeof(H2O_IPMI_SEL_ENTRY) - 2;
    CopyMem (
        (UINT8 *)mBmcSelEventLog[BmcSelCount].EventRawData, 
        (UINT8 *)&(BmcEvent.Data.RecordId), 
        mBmcSelEventLog[BmcSelCount].EventRawDataSize
        );
    
    mBmcSelEventLogNum = BmcSelCount + 1;
  }

  *DataCount = mBmcSelEventLogNum;
  
  return EFI_SUCCESS;
}

/**
 Clear logged data.

 Void           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelClearEvent (
  VOID
)
{
  EFI_STATUS                    Status;  
  UINT8                         CmdBuf[0x20];
  UINT8                         RecvBuf[0x20];
  UINT8                         RecvSize;
  UINT8                         ResvId[2];
  H2O_IPMI_INTERFACE_PROTOCOL   *Ipmi = NULL;
  H2O_IPMI_CMD_HEADER           Request = {H2O_IPMI_BMC_LUN,
                                           H2O_IPMI_NETFN_STORAGE,
                                           H2O_IPMI_CMD_RESERVE_SEL_ENTRY
                                           };
  UINTN                         ClearRetryCount;
  UINT8                         RequestData;

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Follow IPMI specification to clear SEL:
  // 1. Get Reserved ID
  // 2. Issue Clear SEL Entry command
  //
  ClearRetryCount = 0;
  RequestData = REQ_DATA_INITIATE_ERASE;
  do {
    //
    // 1. Get Reserved ID
    //
    Request.Lun   = H2O_IPMI_BMC_LUN;
    Request.NetFn = H2O_IPMI_NETFN_STORAGE;
    Request.Cmd   = H2O_IPMI_CMD_RESERVE_SEL_ENTRY;
    
    Status = Ipmi->ExecuteIpmiCmd (Ipmi, Request, NULL, 0, ResvId, &RecvSize, NULL);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // 2. Issue Clear SEL Entry command
    //
    Request.Cmd = H2O_IPMI_CMD_CLEAR_SEL_ENTRY;
    CmdBuf[0] = ResvId[0];
    CmdBuf[1] = ResvId[1];
    CmdBuf[2] = 'C';
    CmdBuf[3] = 'L';
    CmdBuf[4] = 'R';
    CmdBuf[5] = RequestData;

    Status = Ipmi->ExecuteIpmiCmd (Ipmi, Request, CmdBuf, 6, RecvBuf, &RecvSize, NULL);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    if ((RecvBuf[0] & 0x0F) != ERASURE_COMPLETED) {
      //
      // Erasure is in progress, wait and retry.
      //
      StallForRetry (ONE_SECOND / 2); 
      RequestData = REQ_DATA_GET_ERASE_STS;
      ClearRetryCount++;
    }
    
  } while (((RecvBuf[0] & 0x0F) == ERASURE_IN_PROGRESS) && (ClearRetryCount < MAX_CLEAR_RETRY_COUNT));

  if (ClearRetryCount >= MAX_CLEAR_RETRY_COUNT) {
    return EFI_NOT_READY;
  }
  
  //
  // Clear BMC SEL event log buffer.
  //
  if (mBmcSelEventLogNum == 0) {
    return EFI_SUCCESS;
  }
  FreeBmcSelEventDatabase ();

  return EFI_SUCCESS;
}

/**
 Excute IPMI CMD to get BMC SEL information.            

 @param[in] IpmiTransport   H2O_IPMI_INTERFACE_PROTOCOL interface   
 @param[in] RecvBuf         BmcSel information.
 @param[in] RecvSize        Size of logged data.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetBmcSelInfo (
  IN H2O_IPMI_INTERFACE_PROTOCOL  *IpmiTransport,
  IN UINT8                        *RecvBuf,
  IN UINT8                        *RecvSize
  )
{
  EFI_STATUS  Status;
  H2O_IPMI_CMD_HEADER             Request = { H2O_IPMI_BMC_LUN,
                                              H2O_IPMI_NETFN_STORAGE,
                                              H2O_IPMI_CMD_GET_SEL_INFO
                                              };

  Status = IpmiTransport->ExecuteIpmiCmd (
                                      IpmiTransport,
                                      Request,
                                      NULL,
                                      0,
                                      RecvBuf,
                                      RecvSize,
                                      NULL
                                      );
  return Status;
}

/**
 Excute IPMI CMD to get BMC SEL logged data.          

 @param[in] IpmiTransport   H2O_IPMI_INTERFACE_PROTOCOL interface.
 @param[in] Data            Index count of data logged in BMC SEL.
 @param[in] RecvBuf         BmcSel logged data.
 @param[in] RecvSize        Size of logged data.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetBmcSelLog (
  IN H2O_IPMI_INTERFACE_PROTOCOL  *IpmiTransport,
  IN UINT16                       Data,
  IN UINT8                        *RecvBuf,
  IN UINT8                        *RecvSize
  )
{
  EFI_STATUS                      Status;
  UINT8                           CmdBuf[6];
  H2O_IPMI_CMD_HEADER             Request = { H2O_IPMI_BMC_LUN,
                                              H2O_IPMI_NETFN_STORAGE,
                                              H2O_IPMI_CMD_GET_SEL_ENTRY
                                              };

  CmdBuf[0] = 0;
  CmdBuf[1] = 0;
  CmdBuf[4] = 0;
  CmdBuf[5] = 0xff;

  CmdBuf[2] = (UINT8) Data;            // Low 8 bits
  CmdBuf[3] = (UINT8) (Data >> 8);     // High 8 bits

  Status = IpmiTransport->ExecuteIpmiCmd (
                                      IpmiTransport,
                                      Request,
                                      CmdBuf,
                                      6,
                                      RecvBuf,
                                      RecvSize,
                                      NULL
                                      );
  return Status;
}

/**
 This function verifies the leap year

 @param[in]         Year  - year in YYYY format                
 
 @retval BOOLEAN    TRUE  - The year is a leap year
                    FALSE - The year is not a leap year              
*/
BOOLEAN
EFIAPI
IsLeapYear (
  IN UINT16   Year
  )
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}


/**
 Count the number of the leap years between 1970 and CurYear

 @param[in]         CurYear The Current year

 @retval UINTN      Count   The count of leapyears            
*/
UINTN
EFIAPI
CountNumOfLeapYears (
  IN UINT16           CurYear
  )
{
  UINT16  NumOfYear;
  UINT16  BaseYear;
  UINT16  Index;
  UINTN   Count;
  Count     = 0;
  BaseYear  = 1970;
  NumOfYear = (UINT16) (CurYear - 1970);
  for (Index = 0; Index <= NumOfYear; Index++) {
    if (IsLeapYear ((UINT16) (BaseYear + Index))) {
      Count += 1;
    }
  }

  return Count;
}

/**
 Count time stamp to Efi time.

 @param[in]         TimeStamp                
 @param[in]         EfiTime                          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
Ts2et (
  IN UINT32             TimeStamp,
  IN EFI_TIME           *EfiTime
  )
{
  UINT16                NumOfYears;
  UINTN                 NumOfLeapYears;
  UINTN                 NumOfDays;
  UINTN                 DayOfYear;
  UINT16                Years;
  UINT32                Mouths;

  // Caculate second;
  EfiTime->Second = (UINT8) (TimeStamp % 60);

  // Caculate minitunes;
  TimeStamp /= 60;
  EfiTime->Minute = (UINT8) (TimeStamp % 60);

  // Caculate Hour;
  TimeStamp /= 60;
  EfiTime->Hour = (UINT8) (TimeStamp % 24);

  NumOfDays = TimeStamp / 24;

  // caculate Year
  NumOfYears = (UINT16) (NumOfDays / 365);
  Years = 1970 + NumOfYears;

  if (Years != 1970) {
    NumOfLeapYears = CountNumOfLeapYears (Years-1);
  } else {
    NumOfLeapYears = 0;
  }

  NumOfDays -= NumOfLeapYears;

  // Year = 1970 + NumOfYear
  EfiTime->Year = (UINT16) (NumOfDays / 365 + 1970);

  DayOfYear = NumOfDays % 365 + 1;

  if (IsLeapYear(EfiTime->Year)) {
    mDaysOfMonth[1] = 29;
  }
  else {
    mDaysOfMonth[1] = 28;
  }

  for (Mouths = 0; DayOfYear > mDaysOfMonth[Mouths];  Mouths++) {
      DayOfYear -= mDaysOfMonth[Mouths];
  }

  EfiTime->Month = (UINT8) Mouths + 1;
  EfiTime->Day = (UINT8) DayOfYear;

  return EFI_SUCCESS;
}

/**
 
 Stall in microsecond.

 @param[in]         MicroSecond                           
 
 @retval VOID                  
*/
VOID
EFIAPI
StallForRetry (
  IN UINTN                              MicroSecond
  )
{
  UINTN Elapse;
  UINT8 Toggle;
  
  Toggle = IoRead8(0x61) & 0x10;
  
  for (Elapse = 0; Elapse < MicroSecond; Elapse += (1000000 / 33333)) {
    while (Toggle == (IoRead8(0x61) & 0x10));
    while (Toggle != (IoRead8(0x61) & 0x10));
  }
  
}

/**
 Free BMC SEL event data buffer.

 VOID          
 
 @retval VOID                 
*/
VOID
EFIAPI
FreeBmcSelEventDatabase (
  VOID
  )
{
  UINTN                     Index;
  
  if (mBmcSelEventLogNum != 0) { 
    //
    // Clear data of event data buffer.
    //
    for (Index = 0; Index < mBmcSelEventLogNum; Index++) {
      if (mBmcSelEventLog[Index].EventRawData != NULL) {
        gBS->FreePool (mBmcSelEventLog[Index].EventRawData);
        mBmcSelEventLog[Index].EventRawData = NULL;
      }
    }

    if (mBmcSelEventLog != NULL) {
      gBS->FreePool (mBmcSelEventLog);
      mBmcSelEventLog = NULL;
    }
    mBmcSelEventLogNum = 0;
  }
}

/**
 Check if the storage is full.

 @param[in]         LogFull    Status of the storage.                          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
CheckEventLogFull (
  IN OUT BOOLEAN                    *LogFull
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  H2O_IPMI_INTERFACE_PROTOCOL           *IpmiTransport;
  H2O_IPMI_SEL_INFO                     *IpmiSelInfo;
  UINT8                                 RecvSize = MAX_BUFFER_SIZE;
  UINT8                                 RecvBuf[MAX_BUFFER_SIZE];

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&IpmiTransport);
  if(EFI_ERROR (Status)) {
    return Status;
  }
  
  RecvSize = MAX_BUFFER_SIZE;
  Status = GetBmcSelInfo (IpmiTransport, RecvBuf, &RecvSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  IpmiSelInfo = (H2O_IPMI_SEL_INFO *)RecvBuf;

  //
  // Use the free space to check if the storage is full.
  //
  if (IpmiSelInfo->FreeSpace < 16) {
    *LogFull = TRUE;
  } else {
    *LogFull = FALSE;
  }

  return EFI_SUCCESS;
  
}

/**
 Adding an event data after arranging the storage.

 VOID           
 
 @retval VOID                 
*/
EFI_STATUS
EFIAPI
AddEventAfterArrangeEventStorage (
  VOID
  )
{
  EFI_STATUS                         Status;
  H2O_IPMI_INTERFACE_PROTOCOL        *IpmiTransport;
  H2O_IPMI_CMD_HEADER                Request = {H2O_IPMI_BMC_LUN,
                                             H2O_IPMI_NETFN_SENSOR_EVENT,
                                             H2O_IPMI_CMD_EVENT_MESSAGE
                                             };
  UINT8                              RecvBuf[MAX_BUFFER_SIZE];
  UINT8                              RecvSize;    
  UINT8                              LogData[EVENT_LOG_DATA_SIZE];

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&IpmiTransport);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LogData[0] = BIOS_SOFTWARE_ID;
  LogData[1] = EVENT_REV;
  LogData[2] = OEM_SENSOR_TYPE_EVENT_STORAGE;
  LogData[3] = BIOS_DEFINED_SENSOR_NUM;
  LogData[4] = EVENT_TYPE_OEM;
  LogData[5] = OEM_SENSOR_OVERWRITE;
  LogData[6] = 0xFF;
//[-start-140213-IB08400246-modify]//
  LogData[7] = CHANGE_BMC_SEL_STORAGE_DATA3;
//[-end-140213-IB08400246-modify]//
  
  Status = IpmiTransport->ExecuteIpmiCmd (
                                      IpmiTransport,
                                      Request,
                                      LogData,
                                      EVENT_LOG_DATA_SIZE,
                                      RecvBuf,
                                      &RecvSize,
                                      NULL
                                      );
    
  return Status;
}

/**
 Arrange the data in the storage.

 VOID            
 
 @retval VOID           
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
    Status = BmcSelClearEvent();
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

  NumOfMemoryEventStr = sizeof(gBmcSelEventLogString) / sizeof(BMC_SEL_EVENT_LOG_STRING);
  StorageEventInfo = (STORAGE_EVENT_LOG_INFO *)EventInfo; 
  
  for (Index = 0; Index < NumOfMemoryEventStr; Index++) {
    if ( (StorageEventInfo->EventID.SensorType == gBmcSelEventLogString[Index].EventID.SensorType) &&  
         (StorageEventInfo->EventID.SensorNum  == gBmcSelEventLogString[Index].EventID.SensorNum)  &&
         (StorageEventInfo->EventID.EventType  == gBmcSelEventLogString[Index].EventID.EventType) ) {
      if (CompareMem (StorageEventInfo->Data, gBmcSelEventLogString[Index].Data, gBmcSelEventLogString[Index].DataLength) == 0) {
        *StringSize = StrLen (gBmcSelEventLogString[Index].EventStr) * 2 + 2;
        gBS->AllocatePool (EfiBootServicesData, *StringSize, &TempString);
        StrCpy(TempString, gBmcSelEventLogString[Index].EventStr);
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

 VOID           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ShiftDataOfStorage (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize = MAX_BUFFER_SIZE;
  UINT8                                 RecvBuf[MAX_BUFFER_SIZE];
  UINT8                                 ResvId[2];
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi = NULL;
  H2O_IPMI_CMD_HEADER                   Request = {H2O_IPMI_BMC_LUN,
                                                   H2O_IPMI_NETFN_STORAGE,
                                                   H2O_IPMI_CMD_RESERVE_SEL_ENTRY
                                                   };
  UINT8                                 CmdBuf[4];
  H2O_IPMI_SEL_INFO                     *IpmiSelInfo;
  UINTN                                 Index;

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RecvSize = MAX_BUFFER_SIZE;
  Status = GetBmcSelInfo (Ipmi, RecvBuf, &RecvSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  IpmiSelInfo = (H2O_IPMI_SEL_INFO *)RecvBuf;

  if ((IpmiSelInfo->OperationSupport & DELETE_SEL_CMD_SUPPORTED) == 0) {
    //
    // Delete SEL cmd not supported.
    //
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < EVENT_LOG_FULL_ADJUST_EVENT_NUM; Index++) {    
    //
    // Delete SEL Entry
    //
    
    Status = Ipmi->ExecuteIpmiCmd (Ipmi, Request, NULL, 0, ResvId, &RecvSize, NULL);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Request.Cmd = H2O_IPMI_CMD_DELETE_SEL_ENTRY;
    CmdBuf[0] = ResvId[0];
    CmdBuf[1] = ResvId[1];
    CmdBuf[2] = 0;
    CmdBuf[3] = 0;

    Status = Ipmi->ExecuteIpmiCmd (Ipmi, Request, CmdBuf, 4, RecvBuf, &RecvSize, NULL);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}


/**
 Entry point of this driver. Install BMC SEL Event Storage protocol into DXE.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
BmcSelEventStorageDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle=NULL;
  H2O_IPMI_INTERFACE_PROTOCOL           *IpmiTransport;
  UINT8                                 IpmiVersion;

  //
  // Check if BMC is ready, then install Event Storage Protocol and callback function 
  // for SMM Runtime Protocol.
  //
  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&IpmiTransport);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    IpmiVersion = IpmiTransport->GetIpmiVersion(IpmiTransport);
    if (IpmiVersion == H2O_IPMI_VERSION_DEFAULT) {
      //
      // BMC is not ready or there is no BMC.
      //
      return EFI_UNSUPPORTED;
    }
  }
  
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
  // Install Event String Protocol for user to log Event into BMC SEL Event Storage.
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

