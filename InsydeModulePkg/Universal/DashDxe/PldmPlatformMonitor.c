/** @file
  Platform Level Data Model for Platform Monitoring and Control

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Mctp.h"
#include "PldmPlatformMonitor.h"




VOID
EFIAPI
GetSensorReadingCheck(
IN  EFI_EVENT          Event,
IN  VOID               *Context
)
{
  EFI_STATUS Status;
  UINT8 BankSelect;
  DASH_IO_CFG_PROTOCOL *DashIoCfg;
  EFI_PLDM_PROTOCOL *Pldm;
  PLDM_CONTEXT *PldmContext;
  MCTP_CONTEXT *MctpContext;


  Pldm = (EFI_PLDM_PROTOCOL *)Context;
  PldmContext = PLDM_CONTEXT_FROM_THIS(Pldm);
  MctpContext = MCTP_CONTEXT_FROM_THIS(PldmContext->Mctp);

  if (PldmContext->TransactionInProgress  || MctpContext->TransactionInProgress) {
    return;
  }


  Status = gBS->LocateProtocol (&gDashIoCfgProtocolGuid, NULL, (VOID **)&DashIoCfg);
  if (EFI_ERROR(Status)) {
    return;
  }

  BankSelect = DashIoCfg->AsfReadReg (DashIoCfg, 0x13);
    
  if ((BankSelect&0x0c) != 0) {
    Status = gBS->SignalEvent(PldmContext->SensorReadingEvent);
  }

}

EFI_STATUS
EFIAPI
RegisterSensorReadingFunction (
  EFI_PLDM_PROTOCOL *Pldm,
  EFI_EVENT_NOTIFY SensorReadingFun
  )
{
  EFI_STATUS Status;
  PLDM_CONTEXT *Context;

  Context = PLDM_CONTEXT_FROM_THIS(Pldm);
  if ( (Context->SensorReadingEvent != NULL) || 
       (Context->SensorReadingCheckEvent != NULL)) {
     return EFI_ALREADY_STARTED;
  }
  
  Status = gBS->CreateEvent (
             EVT_NOTIFY_SIGNAL,
             TPL_CALLBACK,
             SensorReadingFun, 
             Pldm, 
             &Context->SensorReadingEvent
             );


  Status = gBS->CreateEvent (
             EVT_TIMER | EVT_NOTIFY_SIGNAL,
             TPL_CALLBACK,
             GetSensorReadingCheck, 
             Pldm, 
             &Context->SensorReadingCheckEvent
             );

  if (!EFI_ERROR(Status)) {
    Status = gBS->SetTimer (Context->SensorReadingCheckEvent, TimerPeriodic, 1000000U);
    if (EFI_ERROR (Status)) {
      gBS->CloseEvent (Context->SensorReadingEvent);    
      return Status;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PldmPlatformEventMessage (
  EFI_PLDM_PROTOCOL *This,
  UINT8 FormatVersion,
  UINT8 Tid,
  UINT8 EventClass,
  VOID  *EventData,
  UINTN EventDataLen,
  UINT8 *ReturnStatus
  )
{
  UINT8 *RequestBuf;
  UINTN RequestMsgSize;
  EFI_STATUS Status;
  UINTN ResultLen;

  RequestMsgSize = EventDataLen + 3;
  Status = gBS->AllocatePool (EfiBootServicesData, RequestMsgSize , (VOID **)&RequestBuf);
  EFI_ERROR_RETURN (Status);

  RequestBuf[0] = FormatVersion;
  RequestBuf[1] = Tid;
  RequestBuf[2] = EventClass;
  gBS->CopyMem (RequestBuf + 3, EventData, EventDataLen);
  ResultLen = 1;
  Status = ProcessSimplePldmMessage( This, 
                                     PLDM_TYPE_PLATFORM_MONITORING_AND_CONTROL, 
                                     PLDM_PLATFORM_EVENT_MESSAGE,
                                     RequestBuf,
                                     RequestMsgSize,
                                     ReturnStatus,
                                     &ResultLen      
                                   );    

  gBS->FreePool(RequestBuf);
  return Status;
}
