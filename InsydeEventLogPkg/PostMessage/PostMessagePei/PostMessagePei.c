/** @file
  Implementation of PostMessagePei module.

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

#include <PostMessagePei.h>

static H2O_PEI_POST_MESSAGE_PPI             mPeiPostMessagePpi = {
  PeiPostMessageStatusCode
};

static PEI_POST_MESSAGE_LIST                gPeiPostMessageList[] = {

  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,             BEHAVIOR_DEFAULT_SETTING},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,    BEHAVIOR_DEFAULT_SETTING},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,      BEHAVIOR_DEFAULT_SETTING},

  {EFI_PROGRESS_CODE, EFI_IO_BUS_SMBUS | EFI_IOB_PC_INIT,                            BEHAVIOR_DEFAULT_SETTING}
    
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,     BEHAVIOR_DEFAULT_SETTING},
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,      BEHAVIOR_DEFAULT_SETTING}
  
  };

static PEI_OEM_POST_MESSAGE_LOG               gPeiOemPostMessageLog[] = {

  {EFI_PROGRESS_CODE, EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_INIT,             0x0F, 0x02, 0x01},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,    0x0F, 0x00, 0x01},
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,      0x0F, 0x00, 0x02},

  {EFI_PROGRESS_CODE, EFI_IO_BUS_SMBUS | EFI_IOB_PC_INIT,                            0x0F, 0x002, 0x0B}
    
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_RESOURCE_CONFLICT,      },
//  {EFI_ERROR_CODE,    EFI_PERIPHERAL_LOCAL_CONSOLE | EFI_P_EC_CONTROLLER_ERROR,       }
  
  };

static PEI_OEM_POST_MESSAGE_BEEP              gPeiOemPostMessageBeep[] = {
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED, 3,  {1000000,        250000, 
                                                                                       1000000,        250000, 
                                                                                       1000000,        250000, 
                                                                                        250000,         50000,
                                                                                        250000,        750000,
                                                                                           0x0,           0x0,
                                                                                           0x0,           0x0,
                                                                                           0x0,           0x0,
                                                                                           0x0,           0x0}}, //three long and two short beep
  {EFI_ERROR_CODE,    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_USEFUL,   3,  {1000000,        250000, 
                                                                                       1000000,        250000, 
                                                                                       1000000,        250000, 
                                                                                        250000,         50000,
                                                                                        250000,        750000,
                                                                                           0x0,           0x0,
                                                                                           0x0,           0x0,
                                                                                           0x0,           0x0,
                                                                                           0x0,           0x0}} //three long and two short beep
  };


EFI_PEI_PPI_DESCRIPTOR mPpiList = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gH2OPeiPostMessagePpiGuid,
    &mPeiPostMessagePpi
};

EFI_PEI_NOTIFY_DESCRIPTOR mRscHandlerPpiNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRscHandlerPpiGuid,
  RscHandlerPpiNotifyCallback
  };


/**
  Notification function for ReportStatusCode Handler Ppi

  This routine is the notification function for EFI_RSC_HANDLER_PPI

  @param  PeiServices           Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor      Address of the notification descriptor data structure. Type
                                EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                   Address of the PPI that was installed.

  @retval EFI_STATUS            

**/
EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_RSC_HANDLER_PPI           *RscHandlerPpi;

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEfiPeiRscHandlerPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &RscHandlerPpi
                                  );
  if (!EFI_ERROR(Status)) {
    //
    // Register the worker function to ReportStatusCodeRouter
    //
    Status = RscHandlerPpi->Register (PeiPostMessageStatusCode);
  } 
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Report Status Code to PEI Post Message.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     
**/
EFI_STATUS
EFIAPI
PeiPostMessageStatusCode (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  ) 
{
  UINTN                                 Index;
  
  if (PcdGetBool(PcdPostMessageEn) == FALSE) {  
    return EFI_UNSUPPORTED;
  }

  if (((CodeType == EFI_PROGRESS_CODE)  && (PcdGetBool(PcdPostMessageProgressCode) == FALSE))  ||
      ((CodeType == EFI_ERROR_CODE)     && (PcdGetBool(PcdPostMessageErrorCode)    == FALSE))  ||
      ((CodeType == EFI_DEBUG_CODE)     && (PcdGetBool(PcdPostMessageDebugCode)    == FALSE))) {  
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < sizeof (gPeiPostMessageList) / sizeof (PEI_POST_MESSAGE_LIST); Index++) {
    if ((gPeiPostMessageList[Index].CodeType == CodeType) && (gPeiPostMessageList[Index].CodeValue == CodeValue)) {
      if (FeaturePcdGet(PcdH2OEventLogSupport) != 0) {
        //
        // 1. Log event
        //
        if ((gPeiPostMessageList[Index].BehaviorBitMap & BEHAVIOR_LOG_TO_STORAGE) == BEHAVIOR_LOG_TO_STORAGE) {
          if (PcdGetBool(PcdPostMessageLogPostMsg) == TRUE) {
            PeiEventLogHandler (PeiServices, CodeType, CodeValue, Instance, CallerId, Data);
          }
        }
      }
      

      //
      // 2. Create HOB for DXE to handle
      //
      if ((gPeiPostMessageList[Index].BehaviorBitMap & BEHAVIOR_CREATE_HOB) == BEHAVIOR_CREATE_HOB) {
        if ((PcdGetBool(PcdPostMessageLogPostMsg) == TRUE) || (PcdGetBool(PcdPostMessageShowPostMsg) == TRUE)) {
          PeiPostMessageCreateHob (PeiServices, CodeType, CodeValue, Instance, CallerId, Data);
        }
      }

      if (FeaturePcdGet(PcdH2OPostBeepSupport) != 0) {
        //
        // POST Beep
        //
        if (PcdGetBool(PcdPostMessageBeepPostMsg) == TRUE) {
          if ((gPeiPostMessageList[Index].BehaviorBitMap & BEHAVIOR_EVENT_BEEP) == BEHAVIOR_EVENT_BEEP) {
            PeiEventBeepHandler  (PeiServices, CodeType, CodeValue, Instance, CallerId, Data);
          }
        }
      }
    }

  }

  return EFI_SUCCESS;
}

/**
  Log Post message to HOB for DXE to handle.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     

**/
EFI_STATUS
EFIAPI
PeiPostMessageCreateHob (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue OPTIONAL, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  )
{
  EFI_STATUS                          Status;
  PEI_POST_MESSAGE_DATA_HOB           EventDataHob;
  VOID                                *Hob;

  Status = EFI_SUCCESS;
  PeiArrangeData (PeiServices, CodeType, CodeValue, Instance, CallerId, Data, &EventDataHob);

  Hob = BuildGuidDataHob (
                      &gH2OPeiPostMessageHobGuid,
                      &EventDataHob,
                      sizeof(PEI_POST_MESSAGE_DATA_HOB)
                      );
  if (Hob == NULL) {
    Status = EFI_UNSUPPORTED;
  }
  
  return Status;

}

/**
  Arrange Post message data.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     

**/
EFI_STATUS
EFIAPI
PeiArrangeData (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN EFI_STATUS_CODE_TYPE                 CodeType,
  IN EFI_STATUS_CODE_VALUE                CodeValue OPTIONAL, 
  IN UINT32                               Instance  OPTIONAL,
  IN CONST EFI_GUID                       *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA           *Data     OPTIONAL,
  OUT PEI_POST_MESSAGE_DATA_HOB           *EventDataHob
  )
{
  EventDataHob->CodeType  = CodeType;
  EventDataHob->CodeValue = CodeValue;
  //
  // Default sample is no data
  //
  EventDataHob->DataLength = 0;
  EventDataHob->Data[0] = 0;
  EventDataHob->Data[1] = 0;
  EventDataHob->Data[2] = 0;
  EventDataHob->Data[3] = 0;
  EventDataHob->Data[4] = 0;

  return EFI_SUCCESS;

}

/**
  Pass the data of Status code to Pei Event Handler.
  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     

**/
EFI_STATUS
EFIAPI
PeiEventLogHandler (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_STATUS_CODE_TYPE       CodeType,
  IN EFI_STATUS_CODE_VALUE      CodeValue OPTIONAL, 
  IN UINT32                     Instance  OPTIONAL,
  IN CONST EFI_GUID             *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA *Data     OPTIONAL
  )
{
  EFI_STATUS                              Status;
  H2O_PEI_EVENT_LOG_PPI                   *PeiEventLogPpi;
  EVENT_TYPE_ID                           EventTypeId;
  UINT8                                   Data8[3]={0}; 
  UINTN                                   Index;
  BOOLEAN                                 LogEvent;

  LogEvent = FALSE;
  for (Index = 0; Index < sizeof (gPeiOemPostMessageLog) / sizeof (PEI_OEM_POST_MESSAGE_LOG); Index++) {
    if ((gPeiOemPostMessageLog[Index].CodeType == CodeType) && (gPeiOemPostMessageLog[Index].CodeValue == CodeValue)) {
      //
      // We got it. Log it.
      //
      LogEvent=TRUE;
      break;
    }
  }

  if (LogEvent==FALSE) {
    return EFI_UNSUPPORTED;
  }
  
  EventTypeId.SensorType = gPeiOemPostMessageLog[Index].SensorType.Code;
  EventTypeId.SensorNum  = OEM_SEL_SENSOR_NUM;
  EventTypeId.EventType  = OEM_SEL_EVENT_TYPE;
  Data8[0] = gPeiOemPostMessageLog[Index].SensorType.Offset;
  Data8[1] = gPeiOemPostMessageLog[Index].SensorType.Data;
  Data8[2] = OEM_SEL_UNDEFINED;

  Status = (*PeiServices)->LocatePpi (
                                  PeiServices,
                                  &gH2OPeiEventLogPpiGuid,
                                  0,                      
                                  NULL,                    
                                  (VOID **)&PeiEventLogPpi    
                                  );
  if (!EFI_ERROR (Status)) {
    Status = PeiEventLogPpi->WriteEvent (
                                     PeiServices,
                                     EventTypeId,
                                     3,
                                     Data8
                                     );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Pass the data of Status code to Pei Event Handler.
  
  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     

**/
EFI_STATUS
EFIAPI
PeiEventBeepHandler (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_STATUS_CODE_TYPE       CodeType,
  IN EFI_STATUS_CODE_VALUE      CodeValue OPTIONAL, 
  IN UINT32                     Instance  OPTIONAL,
  IN CONST EFI_GUID             *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA *Data     OPTIONAL
  )
{
  EFI_STATUS                              Status=EFI_UNSUPPORTED;
  UINTN                                   Index;
  PEI_SPEAKER_IF_PPI                      *SpeakerPpi;
  UINT8                                   BeepDataCount=0;
  UINT8                                   BeepLoopIndex=0;

  //
  // Locate speaker
  //
  Status = (*PeiServices)->LocatePpi (
                                  PeiServices,
                                  &gPeiSpeakerInterfacePpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&SpeakerPpi
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < (sizeof (gPeiOemPostMessageBeep) / sizeof (PEI_OEM_POST_MESSAGE_BEEP)); Index++) {
    if ((gPeiOemPostMessageBeep[Index].CodeType == CodeType) && (gPeiOemPostMessageBeep[Index].CodeValue == CodeValue)) {
      //
      // We got it. Beep it.
      //
      for (BeepLoopIndex=0; BeepLoopIndex<gPeiOemPostMessageBeep[Index].BeepLoop; BeepLoopIndex++) {
        BeepDataCount=0;
        while (gPeiOemPostMessageBeep[Index].BeepData[BeepDataCount].BeepDuration != 0x0){
          Status = SpeakerPpi->GenerateBeep (
                                         (UINTN)0x1, 
                                         gPeiOemPostMessageBeep[Index].BeepData[BeepDataCount].BeepDuration,
                                         gPeiOemPostMessageBeep[Index].BeepData[BeepDataCount].TimerInterval 
                                         );

          BeepDataCount++;
        }
      }
      return Status;
    }
  }

  return Status;
}


/**
  Perform PEI Post Message Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
PostMessagePeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_RSC_HANDLER_PPI               *RscHandlerPpi;

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEfiPeiRscHandlerPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &RscHandlerPpi
                                  );
  if (EFI_ERROR (Status)) {
    //
    // Register callback function for PEI_RSC_HANDLER_PPI.
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mRscHandlerPpiNotify); 
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    Status = RscHandlerPpi->Register (PeiPostMessageStatusCode);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  
  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);

  return Status;
}

