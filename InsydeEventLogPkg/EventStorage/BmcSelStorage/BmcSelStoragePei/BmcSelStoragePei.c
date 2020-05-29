/** @file
  Implementation of BmcSelStoragePei module.

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

#include <BmcSelStoragePei.h>

static H2O_PEI_BMC_SEL_STORAGE_PPI mPeiBmcSelStoragePpi = {
                                                     LogEvent
                                                     };

static EFI_PEI_PPI_DESCRIPTOR mPpiList = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gH2OPeiBmcSelStoragePpiGuid,
    &mPeiBmcSelStoragePpi
};

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
LogEvent (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *LogData
  )
{
  EFI_STATUS                                Status;
  H2O_IPMI_INTERFACE_PPI                    *IpmiPpi;
  BOOLEAN                                   LogFull = FALSE;
  BOOLEAN                                   DeleteEntrySupport = FALSE;

  if (DataSize > MAX_BMC_SEL_LOGGED_DATA_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gH2OIpmiInterfacePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&IpmiPpi
                                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = CheckBmcSelEventLogFull (PeiServices, &LogFull, &DeleteEntrySupport);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((LogFull == TRUE)) {
    if (DeleteEntrySupport == TRUE) {
      ShiftDataOfBmcSel (PeiServices);
    } else {
      //
      // BMC SEL is full and not support delete entry,
      // so that we can not log event anymore.
      //
      return EFI_OUT_OF_RESOURCES;
    }

  }
  
  Status = LogDataToBmcSel (PeiServices, EventID, DataSize, LogData);

  return EFI_SUCCESS;

}

/**
 Check if the storage is full.
 
 @param[in]  PeiServices     Efi Pei services.
 @param[out] LogFull         Status of the storage.
 @param[out] DelEntrySupport Support of "Delete Entry" cmd.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
CheckBmcSelEventLogFull (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN OUT BOOLEAN                    *LogFull,
  IN OUT BOOLEAN                    *DelEntrySupport
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  H2O_IPMI_INTERFACE_PPI                *IpmiPpi;
  H2O_IPMI_SEL_INFO                     *IpmiSelInfo;
  UINT8                                 RecvSize = MAX_BUFFER_SIZE;
  UINT8                                 RecvBuf[MAX_BUFFER_SIZE];

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gH2OIpmiInterfacePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&IpmiPpi
                                  );
  if(EFI_ERROR (Status)) {
    return Status;
  }
  
  RecvSize = MAX_BUFFER_SIZE;
  Status = GetBmcSelInfo (PeiServices, IpmiPpi, RecvBuf, &RecvSize);
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

  
  if ((IpmiSelInfo->OperationSupport & DELETE_SEL_CMD_SUPPORTED) == 0) {
    //
    // Delete SEL cmd not supported.
    //
    *DelEntrySupport = FALSE;
  } else {
    *DelEntrySupport = TRUE;
  }

  return EFI_SUCCESS;
  
}

/**
 Excute IPMI CMD to get BMC SEL information. 
            
 @param[in] PeiServices     Efi Pei services.
 @param[in] IpmiTransport   H2O_IPMI_INTERFACE_PROTOCOL interface   
 @param[in] RecvBuf         BmcSel information.
 @param[in] RecvSize
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
GetBmcSelInfo (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN H2O_IPMI_INTERFACE_PPI       *IpmiPpi,
  IN UINT8                        *RecvBuf,
  IN UINT8                        *RecvSize
)
{
  EFI_STATUS                      Status;
  H2O_IPMI_CMD_HEADER             Request = { H2O_IPMI_BMC_LUN,
                                              H2O_IPMI_NETFN_STORAGE,
                                              H2O_IPMI_CMD_GET_SEL_INFO
                                              };

  Status = IpmiPpi->ExecuteIpmiCmd (
                                IpmiPpi,
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
 Shift event data of the storage.

 @param[in]         PeiServices     Efi Pei services.           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
ShiftDataOfBmcSel (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize = MAX_BUFFER_SIZE;
  UINT8                                 RecvBuf[MAX_BUFFER_SIZE];
  UINT8                                 ResvId[2];
  H2O_IPMI_INTERFACE_PPI                *IpmiPpi;
  H2O_IPMI_CMD_HEADER                   Request = {H2O_IPMI_BMC_LUN,
                                                   H2O_IPMI_NETFN_STORAGE,
                                                   H2O_IPMI_CMD_RESERVE_SEL_ENTRY
                                                   };
  UINT8                                 CmdBuf[4];
  UINTN                                 Index;

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gH2OIpmiInterfacePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&IpmiPpi
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RecvSize = MAX_BUFFER_SIZE;

  for (Index = 0; Index < EVENT_LOG_FULL_ADJUST_EVENT_NUM; Index++) {    
    //
    // Delete SEL Entry
    //
    
    Status = IpmiPpi->ExecuteIpmiCmd (IpmiPpi, Request, NULL, 0, ResvId, &RecvSize, NULL);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Request.Cmd = H2O_IPMI_CMD_DELETE_SEL_ENTRY;
    CmdBuf[0] = ResvId[0];
    CmdBuf[1] = ResvId[1];
    CmdBuf[2] = 0;
    CmdBuf[3] = 0;

    Status = IpmiPpi->ExecuteIpmiCmd (IpmiPpi, Request, CmdBuf, 4, RecvBuf, &RecvSize, NULL);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  AddEventAfterArrangeEventStorage (PeiServices);

  return EFI_SUCCESS;
}

/**
 Adding an event data after arranging the storage.

 @param[in]         PeiServices     Efi Pei services.            
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
AddEventAfterArrangeEventStorage (
  IN  CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                         Status;
  H2O_IPMI_INTERFACE_PPI             *IpmiPpi;
  H2O_IPMI_CMD_HEADER                Request = {
                                             H2O_IPMI_BMC_LUN,
                                             H2O_IPMI_NETFN_SENSOR_EVENT,
                                             H2O_IPMI_CMD_EVENT_MESSAGE
                                             };
  UINT8                              RecvBuf[MAX_BUFFER_SIZE];
  UINT8                              RecvSize;    
  UINT8                              LogData[EVENT_LOG_DATA_SIZE];

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gH2OIpmiInterfacePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&IpmiPpi
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LogData[0] = BIOS_SOFTWARE_ID;
  LogData[1] = EVENT_REV;
  LogData[2] = ADJUST_STORAGE_EVENT_ID1;
  LogData[3] = ADJUST_STORAGE_EVENT_ID2;
  LogData[4] = ADJUST_STORAGE_EVENT_ID3;
  LogData[5] = ADJUST_STORAGE_DATA1;
  LogData[6] = ADJUST_STORAGE_DATA2;
  LogData[7] = ADJUST_STORAGE_DATA3;
  
  Status = IpmiPpi->ExecuteIpmiCmd (
                                IpmiPpi,
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
 Log data into BMC SEL.
            
 @param[in] PeiServices     Efi Pei services.
 @param[in] EventID         Event ID of logged data.
 @param[in] DataSize        Size of the logged data.
 @param[in] Data            Data which will be logged into the BMC SEL.
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
LogDataToBmcSel (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *Data
  )
{
  EFI_STATUS                         Status;
  H2O_IPMI_INTERFACE_PPI             *IpmiPpi;
  H2O_IPMI_CMD_HEADER                Request = {
                                             H2O_IPMI_BMC_LUN,
                                             H2O_IPMI_NETFN_SENSOR_EVENT,
                                             H2O_IPMI_CMD_EVENT_MESSAGE
                                             };
  UINT8                              RecvBuf[MAX_BUFFER_SIZE];
  UINT8                              RecvSize;    
  UINT8                              LogData[EVENT_LOG_DATA_SIZE];
  UINTN                              Index;

  if (DataSize > MAX_BMC_SEL_LOGGED_DATA_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gH2OIpmiInterfacePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&IpmiPpi
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LogData[0] = BIOS_SOFTWARE_ID;
  LogData[1] = EVENT_REV;
  LogData[2] = EventID.SensorType;
  LogData[3] = EventID.SensorNum;
  LogData[4] = EventID.EventType;

  
  for (Index = 0; Index < DataSize; Index++) {
    LogData[5 + Index] = Data[Index];
  }
  
  Status = IpmiPpi->ExecuteIpmiCmd (
                                IpmiPpi,
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
  Perform PEI Event Handler Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
BmcSelStoragePeiEntryPoint ( 
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);

  return Status;
}

