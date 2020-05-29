/** @file

  POST Message Dxe implementation.

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

#include <WatchDogReportStatusCodeDxe.h>
#include <Library/WatchDogLib.h>
#include <Library/PcdLib.h>

EFI_RSC_HANDLER_PROTOCOL              *mRscHandlerProtocol   = NULL;

/**
 
 This function is a handler for BIOS to handle Post message.

 (See Tiano Runtime Specification)           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
DxeWatchDogStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  ) 
{

  UINT32 CodePhase = 0;
  TCO_WATCHDOG_TYPE  WatchDogType = 0;
  
  
  if (FeaturePcdGet(PcdH2OWatchDogSupported) == FALSE) {  
    return EFI_UNSUPPORTED;
  }

  CodePhase = CodeValue & (EFI_SOFTWARE|0xFF0000);
  
  switch (CodePhase) {
    case EFI_SOFTWARE_SEC:
    case EFI_SOFTWARE_PEI_CORE:
    case EFI_SOFTWARE_PEI_MODULE:
      WatchDogType = PEI_WATCH_DOG;
        break;
    case EFI_SOFTWARE_DXE_CORE:
    case EFI_SOFTWARE_DXE_BS_DRIVER:
    case EFI_SOFTWARE_DXE_RT_DRIVER:
      WatchDogType = DXE_WATCH_DOG;
        break;

    case EFI_SOFTWARE_SMM_DRIVER:
      WatchDogType = SMM_WATCH_DOG;
        break;
    default:
      WatchDogType = END_WATCH_DOG;
        break;
  }

  if ((CodeType == EFI_PROGRESS_CODE) && (CodeValue & EFI_SW_PC_INIT_BEGIN)) {
    WatchDogStart (WatchDogType);
  }

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

  Status = gBS->LocateProtocol (
                            &gEfiRscHandlerProtocolGuid,
                            NULL,
                            (VOID **) &mRscHandlerProtocol
                            );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Register the worker function to ReportStatusCodeRouter
  //
  Status = mRscHandlerProtocol->Register (DxeWatchDogStatusCode, TPL_HIGH_LEVEL);
  
  ASSERT_EFI_ERROR (Status);

  return;
}


/**
 The Post Message driver will handle all events during DXE and BDS phase.
 The events are come from Status Code reported.
 Defaulted setting the event will show on screen after BIOS POST.         
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
WatchDogReportStatusCodeDxeEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  ) 
{
  EFI_STATUS                                Status;
  EFI_EVENT                                 RscHandlerProtocolEvent;

  if (FeaturePcdGet(PcdH2OWatchDogSupported) == FALSE) {  
    return EFI_SUCCESS;
  }
  
    Status = gBS->LocateProtocol (
                              &gEfiRscHandlerProtocolGuid,
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
                               &gEfiRscHandlerProtocolGuid,
                               &RscHandlerProtocolEvent
                               );
      
    } else {
      //
      // Register the worker function to ReportStatusCodeRouter
      //
      Status = mRscHandlerProtocol->Register (DxeWatchDogStatusCode, TPL_HIGH_LEVEL);
    }
    
  return Status;
}
