/** @file
  TimerEvent_07 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "TimerEvent_07.h"

EFI_EVENT     Event;

EFI_STATUS
EFIAPI
TimerEventEntry (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS    Status;
  VOID*         NullInterface;

  NullInterface = NULL;
  
  Status = gBS->LocateProtocol(
                            &gEfiTimerEventProtocolGuid,
                            NULL, 
                            &NullInterface
                            );
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);
    
  } else {
    Print(L"%r\n", Status);
    Print(L"Already exit.\n");
    return EFI_ALREADY_STARTED;
  }

  Status = gBS->InstallProtocolInterface(
                                      &ImageHandle,
                                      &gEfiTimerEventProtocolGuid, 
                                      EFI_NATIVE_INTERFACE,
                                      NULL
                                      );
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);
    return Status;
  }

  GetTimeEvent();
  Print(L"Load done\n");

  return Status; 
}

EFI_STATUS
TimerEventUnload (
  IN EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS Status;

  Status = gBS->SetTimer(Event, TimerCancel, 0);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->CloseEvent(Event);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  
  Status = gBS->UninstallProtocolInterface(
                                        ImageHandle,
                                        &gEfiTimerEventProtocolGuid,
                                        NULL
                                        );  

  Print(L"Unload done.\n");

  return Status;
}

VOID
GetTimeEvenNotify (
  IN EFI_EVENT      Event,
  IN VOID           *Context
  )
{
  EFI_TIME          Time;
  UINT64            Column;
  UINT64            Row;

  //
  // Save current position
  //
  Column = gST->ConOut->Mode->CursorColumn;
  Row = gST->ConOut->Mode->CursorRow;

  //
  // Set print position
  //
  gST->ConOut->SetCursorPosition(gST->ConOut,60,0);

  gRT->GetTime(&Time, NULL);

  Print(L"%t:%.2d\r\n", Time, Time.Second);

  gST->ConOut->SetCursorPosition(gST->ConOut, Column, Row); 
}

EFI_STATUS
GetTimeEvent (
  VOID
  )
{
  EFI_STATUS    Status;

  Event = NULL;
  
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL | EVT_TIMER,
                  TPL_CALLBACK,
                  (EFI_EVENT_NOTIFY)GetTimeEvenNotify,
                  NULL,
                  &Event
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->SetTimer (
                  Event,
                  TimerPeriodic,
                  10000000          // 100ns
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  return EFI_SUCCESS;
}