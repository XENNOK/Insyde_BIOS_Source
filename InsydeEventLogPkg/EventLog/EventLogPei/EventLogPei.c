/** @file
  Implementation of EventLogPei module.

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

#include <EventLogPei.h>

static H2O_PEI_EVENT_LOG_PPI mPeiEventLogPpi = {
  PeiLogEvent
};

EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gH2OPeiEventLogPpiGuid,
  &mPeiEventLogPpi
};

/**
 Log event data in PEI phase by Pei Event Log PPI. 
         
 @param[in] PeiServices     Efi Pei services.
 @param[in] EventID         Event ID of the event data.
 @param[in] DataSize        Size of event data.
 @param[in] *LogData        Event data which will be logged.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PeiLogEvent (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *LogData
  )
{
  EFI_STATUS                          Status;
  H2O_PEI_BMC_SEL_STORAGE_PPI         *PeiBmcSelStorage;
    
  //
  // Log to BMC SEL.
  //
  Status = (**PeiServices).LocatePpi(
                                  (CONST EFI_PEI_SERVICES **)PeiServices,
                                  &gH2OPeiBmcSelStoragePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&PeiBmcSelStorage
                                  );
  if (!EFI_ERROR(Status)) {
    Status = PeiBmcSelStorage->LogEvent (PeiServices, EventID, DataSize, LogData);
  }
  
  //
  // Create a GUID hob(PeiEventLogHob) with the event data.
  //
  Status = PeiEventLogCreateHob (PeiServices, EventID, DataSize, LogData);

  return Status;
}

/**
 Log event data to BMC SEL.           

 @param[in] PeiServices     Efi Pei services.
 @param[in] EventID         Event ID of logged data.
 @param[in] DataSize        Size of event data.
 @param[in] *LogData        Event data which will be logged.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PeiEventLogCreateHob (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *Data
  )
{
  EFI_STATUS                          Status;
  PEI_EVENT_LOG_ORGANIZATION          Log;
  UINT32                              NumBytes;
  UINTN                               Index;
  VOID                                *Hob;

  Status = EFI_SUCCESS;
  
  if (DataSize > (sizeof(Log.Data) - sizeof(EVENT_TYPE_ID))) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  NumBytes = PEI_EVENT_LOG_BASE_LENGTH + sizeof(EVENT_TYPE_ID) + DataSize;
  
  Log.Year   = ReadCmos8(0x09);
  Log.Month  = ReadCmos8(0x08);
  Log.Day    = ReadCmos8(0x07);
  Log.Hour   = ReadCmos8(0x04);
  Log.Minute = ReadCmos8(0x02);
  Log.Second = ReadCmos8(0x00);

  Log.Type   = 0x80;
  Log.Length = (UINT8)NumBytes;

  Log.Data[0] = EventID.SensorType;
  Log.Data[1] = EventID.SensorNum;
  Log.Data[2] = EventID.EventType;

  for (Index = 0; Index < DataSize; Index++) {
    Log.Data[3 + Index] = Data[Index];
  }

  Hob = BuildGuidDataHob (
                      &gH2OPeiEventLogHobGuid,
                      &Log,
                      NumBytes
                      );
  if (Hob == NULL) {
    Status = EFI_UNSUPPORTED;
  }
  return Status;

}

/**
  Perform PEI Event Log Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
EventLogPeiEntryPoint ( 
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                            Status;
  
  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);

  return Status;
}

