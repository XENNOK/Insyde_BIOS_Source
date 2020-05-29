/** @file

  POST Message Smm implementation.

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

#include "PostMessageSmm.h"

EFI_PHYSICAL_ADDRESS                  gPostMessageBuffer;  
EFI_PHYSICAL_ADDRESS                  gPostMsgStatusCodeBuffer=0xFFFFFFFF;  // Store the CodeType and CodeValue information
BOOLEAN                               gPostMessageEn=TRUE;
BOOLEAN                               gProgressCodeEn=TRUE;
BOOLEAN                               gErrorCodeEn=TRUE;
BOOLEAN                               gDebugCodeEn=FALSE;
BOOLEAN                               gLogPostMsg=FALSE;
BOOLEAN                               gShowPostMsg=FALSE;
BOOLEAN                               gBeepPostMsg=TRUE;
BOOLEAN                               gStalltoShow=FALSE;
BOOLEAN                               gSupportPostMessageFun=FALSE;
BOOLEAN                               gOemHookOnly=FALSE;


H2O_POST_MESSAGE_PROTOCOL            gPostMessageProtocol = {
  PostMessageStatusCode
  };

EFI_SMM_RSC_HANDLER_PROTOCOL         *mRscHandlerProtocol   = NULL;
EFI_EVENT                             mExitBootServicesEvent = NULL;

/**
 
 This function is a handler for BIOS to handle POST Message.

 (See Tiano Runtime Specification)           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PostMessageStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  ) 
{
  EFI_STATUS                  Status;

  if (gPostMessageEn==FALSE) {
    return EFI_UNSUPPORTED;
  }
  //
  // 1. OEM/ODM first - Call PostMessageStatusCodeHook() for OEM/ODM to handle event
  //
  Status = PostMessageStatusCodeHook (CodeType, CodeValue, Instance, CallerId, Data);
  if (Status == EFI_ABORTED) {
    //
    // OEM/ODM hook function only.
    // Return EFI_SUCCESS after finishing Hook function.
    //
    return EFI_SUCCESS;
  }
    
  //
  // 2. Log event to Temp buffer in memory area for future use.
  //
  if (gSupportPostMessageFun) {
    LogPostMessage (CodeType, CodeValue);
  }
      
  return EFI_SUCCESS;
}

/**

 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
LogPostMessage (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue
  )   
{
  TEMP_BUFFER_HEAD_STRUCTURE                *TempBufferHead=NULL;
    
  if ((gPostMsgStatusCodeBuffer == 0xFFFFFFFF) || (gPostMsgStatusCodeBuffer == 0)) {
    //
    // No Event Storage can use.
    //
    return EFI_UNSUPPORTED;
  }

  TempBufferHead = (TEMP_BUFFER_HEAD_STRUCTURE *)(UINTN)gPostMsgStatusCodeBuffer;

  if (TempBufferHead->TempSignature != MESG_TEMP_INSTANCE_SIGNATURE) {
    //
    // Tempbuffer memory space don't exist.
    //
    gSupportPostMessageFun=FALSE;
    gPostMsgStatusCodeBuffer=0;
    return EFI_UNSUPPORTED;
  }
  
  if ((TempBufferHead->TempEndAddr+sizeof(TEMP_BUFFER_STRUCTURE)) > 
      (TempBufferHead->TempStartAddr + TempBufferHead->TempSize)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  gBS->CopyMem ((VOID *)(UINTN)TempBufferHead->TempEndAddr, &CodeType, sizeof(EFI_STATUS_CODE_TYPE));
  TempBufferHead->TempEndAddr += sizeof (EFI_STATUS_CODE_TYPE);
  gBS->CopyMem ((VOID *)(UINTN)TempBufferHead->TempEndAddr, &CodeValue, sizeof(EFI_STATUS_CODE_VALUE));
  TempBufferHead->TempEndAddr += sizeof (EFI_STATUS_CODE_VALUE); 

  return EFI_SUCCESS;
}

/**
  Notification function for ReportStatusCode Handler Protocol

  This routine is the notification function for EFI_RSC_HANDLER_PROTOCOL

  @param[in]         Event                
  @param[in]         Context                           
  
  @retval VOID                

**/
static
VOID
EFIAPI
RscHandlerProtocolCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                        Status;

  Status = gSmst->SmmLocateProtocol (
                                 &gEfiSmmRscHandlerProtocolGuid,
                                 NULL,
                                 (VOID **) &mRscHandlerProtocol
                                 );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Register the worker function to ReportStatusCodeRouter
  //
  Status = mRscHandlerProtocol->Register (PostMessageStatusCode);
  
  ASSERT_EFI_ERROR (Status);

  return;
}

/**
  The POST Message driver will handle all events during DXE and BDS phase.
  The events are come from Status Code reported.
  Defaulted setting the event will show on screen after BIOS POST.
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
PostMessageSmmEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  ) 
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                SmmHandle;  
  UINTN                                     VariableSize;
  EFI_EVENT                                 RscHandlerProtocolEvent;

  if (PcdGetBool(PcdPostMessageEn) == FALSE) {
    return EFI_UNSUPPORTED;
  }
  
  SmmHandle = NULL; 

  //
  // Update Non-SMM Setting to SMM Driver
  //
  VariableSize = sizeof (EFI_PHYSICAL_ADDRESS);
  Status = gRT->GetVariable (
                          L"PostMsgStatusCode",
                          &gH2OPostMessageVariableGuid,
                          NULL,
                          &VariableSize,
                          &gPostMsgStatusCodeBuffer
                          );
  if (EFI_ERROR (Status)) {
    gPostMsgStatusCodeBuffer=0xFFFFFFFF;
  }

  gPostMessageEn      = (BOOLEAN)PcdGetBool(PcdPostMessageEn);
  gProgressCodeEn     = (BOOLEAN)PcdGetBool(PcdPostMessageProgressCode);
  gErrorCodeEn        = (BOOLEAN)PcdGetBool(PcdPostMessageErrorCode);
  gDebugCodeEn        = (BOOLEAN)PcdGetBool(PcdPostMessageDebugCode);
  gLogPostMsg         = (BOOLEAN)PcdGetBool(PcdPostMessageLogPostMsg);
  gShowPostMsg        = (BOOLEAN)PcdGetBool(PcdPostMessageShowPostMsg);
  gBeepPostMsg        = (BOOLEAN)PcdGetBool(PcdPostMessageBeepPostMsg);    

  Status = gSmst->SmmInstallProtocolInterface (
                                           &SmmHandle,
                                           &gH2OSmmPostMessageProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           &gPostMessageProtocol
                                           );
  if (!EFI_ERROR(Status)) {
    gSupportPostMessageFun = TRUE;
  }

  Status = gSmst->SmmLocateProtocol (
                                 &gEfiSmmRscHandlerProtocolGuid,
                                 NULL,
                                 (VOID **) &mRscHandlerProtocol
                                 );
  if (EFI_ERROR (Status)) {
    //
    // Register callback for loading Event storage driver.
    //
    Status = gBS->CreateEventEx (
                             EVT_NOTIFY_SIGNAL,
                             TPL_NOTIFY,
                             RscHandlerProtocolCallback,
                             NULL,
                             &gEfiSmmRscHandlerProtocolGuid,
                             &RscHandlerProtocolEvent
                             );
  } else {
    //
    // Register the worker function to ReportStatusCodeRouter
    //
    Status = mRscHandlerProtocol->Register (PostMessageStatusCode);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return Status;
}
