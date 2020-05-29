/** @file

  POST Message Hook Smm implementation.

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

#include <PostMessageHookSmm.h>

//
// Extern Variables
//
extern POST_MESSAGE_LIST                     gPostMessageList[];
extern OEM_POST_MESSAGE_LOG                  gOemPostMessageLog[];

extern UINTN                                 gPostMessageListSize;
extern UINTN                                 gOemPostMessageLogSize;

extern UINT8                                 gPostMessageEn;
extern BOOLEAN                               gProgressCodeEn;
extern BOOLEAN                               gErrorCodeEn;
extern BOOLEAN                               gDebugCodeEn;
extern BOOLEAN                               gLogPostMsg;
extern BOOLEAN                               gShowPostMsg;
extern BOOLEAN                               gBeepPostMsg;
extern BOOLEAN                               gStalltoShow;
extern BOOLEAN                               gSupportPostMessageFun;
extern BOOLEAN                               gOemHookOnly;

/**
 

 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PostMessageStatusCodeHook (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  ) 
{
  UINTN                           Index;

  if (((CodeType == EFI_PROGRESS_CODE) && (gProgressCodeEn == FALSE))  ||
    ((CodeType == EFI_ERROR_CODE)      && (gErrorCodeEn    == FALSE))  ||
    ((CodeType == EFI_DEBUG_CODE)      && (gDebugCodeEn    == FALSE))) {  
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < gPostMessageListSize; Index++) {
    if ((gPostMessageList[Index].CodeType==CodeType) && (gPostMessageList[Index].CodeValue==CodeValue)) {      
      
      if (FeaturePcdGet(PcdH2OEventLogSupport) != 0) {    
        if (gLogPostMsg==TRUE) {
          //
          // Log event
          //        
          if ((gSupportPostMessageFun == TRUE) && (gPostMessageEn == TRUE)) {
            if ((gPostMessageList[Index].BehaviorBitMap&BEHAVIOR_LOG_TO_STORAGE)==BEHAVIOR_LOG_TO_STORAGE) {
              EventLogHandler (CodeType, CodeValue, Instance, CallerId, Data);
            }
          }
        }
      }        
    }
  }

  //
  // Check if OEM hook function only.
  //
  if (PcdGetBool(PcdH2OPostMessageOemHookOnly) == TRUE) {
    return EFI_ABORTED;
  } else {
    return EFI_SUCCESS;
  }
}

/**
 Log event data to the Event Storages(E.g. BIOS, BMC SEL, Memory, DCMI SEL,.....).     
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EventLogHandler (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  )
{
  EFI_STATUS                         Status;
  EVENT_LOG_PROTOCOL                 *EventHandler;
  UINTN                              Index;
  BOOLEAN                            LogEvent=FALSE;
  EVENT_TYPE_ID                      EventTypeId;
  UINT8                              Data8[3]={0};
  
  for (Index = 0; Index < gOemPostMessageLogSize; Index++) {
    if ((gOemPostMessageLog[Index].CodeType == CodeType) && (gOemPostMessageLog[Index].CodeValue == CodeValue)) {
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
  
  Status = gSmst->SmmLocateProtocol (&gH2OSmmEventLogProtocolGuid, NULL, (VOID **)&EventHandler);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Arrange data and log it.
  //
  EventTypeId.SensorType = gOemPostMessageLog[Index].EventID.SensorType;
  EventTypeId.SensorNum  = gOemPostMessageLog[Index].EventID.SensorNum;
  EventTypeId.EventType  = gOemPostMessageLog[Index].EventID.EventType;
  Data8[0] = gOemPostMessageLog[Index].EventData[0];
  Data8[1] = gOemPostMessageLog[Index].EventData[1];
  Data8[2] = gOemPostMessageLog[Index].EventData[2];
  
  Status = EventHandler->LogEvent (EVENT_STORAGE_ALL, &EventTypeId, Data8, 3);  
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;

}

