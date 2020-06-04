/** @file
  

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"TimerEvent_HW.h"

EFI_EVENT myEvent;
INT32     OringinalMode;

/**

  Check and return Mode number
  @param[in]       RequestedColumns        Requested Columns
  @param[in]       RequestedRows           Requested Rows
  @param[in][out]  *TextModeNum            Mode Number

  @return Status Code

*/
EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  )
{
  UINTN        ModeNum;
  UINTN        Columns;
  UINTN        Rows;
  EFI_STATUS   Status;

  for (ModeNum = 1; ModeNum < (UINTN) (gST->ConOut->Mode->MaxMode); ModeNum++) {
    gST->ConOut->QueryMode (
                   gST->ConOut,
                   ModeNum,
                   &Columns,
                   &Rows
                   );
    
    if ((RequestedColumns == Columns) && (RequestedRows == Rows)) {
      *TextModeNum = ModeNum;
      break;
    }
  }

  if (ModeNum == (UINTN) (gST->ConOut->Mode->MaxMode)) {
    *TextModeNum = ModeNum;
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS ProtocolUnLoad(
  IN EFI_HANDLE       ImageHandle
)
{
  EFI_STATUS Status;

  Status = gBS->CloseEvent (myEvent);
  if (EFI_ERROR (Status)) {
        Print (
          L"EventSignal: CloseEvent error %r!\n",
          Status
          );
  }

  Status = gBS->UninstallProtocolInterface(
                  ImageHandle,
                  &gEfiHydraProtocolGuid,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
        Print (
          L"EventSignal: UninstallProtocolInterface error %r!\n",
          Status
          );
  }

  //
  // Reset to Orginal Mode
  //
  EN_CURSOR(TRUE);
  SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  SET_CUR_POS (0, 0);
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  CLEAN_SCREEN();
  return Status;
}

VOID
EventNoify (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  EFI_STATUS  Status;
  EFI_TIME    EfiTime;

  static UINTN times = 0;

  Status = gRT->GetTime (
                  &EfiTime,
                  NULL
                  );
   if (EFI_ERROR (Status)) {
      Print (
        L"EventSignal: GetTime error %d!\n",
        Status
        );
  }
  EN_CURSOR (FALSE);
  
  if (times %2 == 0) {
    SET_COLOR (EFI_WHITE | EFI_BACKGROUND_BLACK);
  } else {
    SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  }

  SET_CUR_POS(78, 0);
  Print (
    L"Date : %04d / %02d / %02d\n",
    EfiTime.Year,
    EfiTime.Month,
    EfiTime.Day
    );
  SET_CUR_POS(78, 1);
  Print (
    L"Time : %02d : %02d : %02d\n",
    EfiTime.Hour,
    EfiTime.Minute,
    EfiTime.Second
    );
  SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  times++;
}

EFI_STATUS 
EventSingal(
)
{
  EFI_STATUS  Status;

  CLEAN_SCREEN();
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  (EFI_EVENT_NOTIFY) EventNoify,
                  NULL,
                  &myEvent
                  );
  if (EFI_ERROR(Status)) {
      Print (
        L"EventSignal: CreateEvent error %r!\n",
        Status
        );
  }

  Status = gBS->SetTimer (
                  myEvent,
                  TimerPeriodic, 
                  PER_SECOND
                  ); // 10*1000*1000*100ns
  if (EFI_ERROR (Status)) {
    Print (
      L"EventSignal: SetTimer error %r!\n",
      Status
      );
  }    
  return EFI_SUCCESS;
}
  

EFI_STATUS
HydraMain (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                  Status;
  VOID                        *HydraInterface;
  EFI_LOADED_IMAGE_PROTOCOL   *ImageInterface = NULL;

  UINTN    TextModeNum;
  
  //
  // Locate Hydra Protocol to check protocol exist or not.
  //
  Status = gBS->LocateProtocol(
                  &gEfiHydraProtocolGuid,
                  NULL,
                  &HydraInterface
                  );

  if (Status == EFI_SUCCESS) {
    Print (
      L"EventSignal: EFI_ALREADY_START %r!\n",
      Status
      );
    return EFI_ALREADY_STARTED;
  } else if (Status != EFI_NOT_FOUND && EFI_ERROR (Status)) {
    Print (
      L"EventSignal: EFI_ERROR %r!\n",
      Status
      );
    return Status;
  }


  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&ImageInterface
                  );
  if (Status == EFI_SUCCESS) {
    ImageInterface->Unload = ProtocolUnLoad;
  }

  Status = gBS->InstallProtocolInterface(
                  &ImageHandle,
                  &gEfiHydraProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
        Print (
          L"EventSignal: InstallProtocolInterface error %r!\n",
          Status
          );
    return Status;
  }

  //
  // Check Console Mode, save orignal mode and Set specification Mode
  //
  OringinalMode = gST->ConOut->Mode->Mode;
  
  ChkTextModeNum (
    DEF_COLS,
    DEF_ROWS,
    &TextModeNum
    );
  
  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          TextModeNum
                          );
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  //
  // Creat Event Signal.
  //
  Status = EventSingal();
  if (EFI_ERROR (Status)) {
        Print (
          L"EventSignal: EventSingal error %r!\n",
          Status
          );
    return Status;
  }

  return Status;
}
