/** @file
  KBC_10 C Source File

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

#include "KBC_10.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS     Status;
  EFI_INPUT_KEY  InputKey;

  ShowMainPage();

  //
  // choose loop
  //
  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (InputKey.ScanCode == SCAN_ESC) {
      gST->ConOut->ClearScreen(gST->ConOut);
      break;
    }

    switch (InputKey.ScanCode) {
    
    case SCAN_F1:
      ShowLedPage();
      ShowLed();
      ShowMainPage();
      continue;
      
    case SCAN_F2:
      RestartSystem();
      continue;
    }
  }

  Status = EFI_SUCCESS;

  return Status;
}

/**
  To lighten LED.

**/
VOID
ShowLed(
  VOID
  )
{
  EFI_CPU_IO2_PROTOCOL  *CpuIo2;
  UINT8                 DataBuffer;

  gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  while (TRUE) {
    
    //
    // 1.If want to write to data port (60h) again, must read once after write. (Unless the first time to write) 
    // 2.Need waiting to write data port; Not need waiting to read data port.
    // 3.Commending Step
    //   (1) Write commendmd to data port
    //   (2) Wait
    //   (3) Read data port
    //   (4) Write subcommendmd to data port
    //   (5) Wait
    //   (6) Read data port
    //
    Led_1_On;
    if (Delay(10000) == 1) {
      Led_Off; 
      break;
    }

    Led_Off;
    if (Delay(10000) == 1) {
      break;
    }

    Led_2_On;
    if (Delay(10000) == 1) {
      Led_Off; 
      break;
    }

    Led_Off;
    if (Delay(10000) == 1) {
      break;
    }

    Led_3_On;
    if (Delay(10000) == 1) {
      Led_Off; 
      break;
    }

    Led_Off;
    if (Delay(10000) == 1) {
      break;
    }
  }
  
  return;
}

/**
  To delay time.

  @param  Count      To choose delay time.

  @retval BOOLEAN    To judge F1 is pressed or not.

**/
BOOLEAN
Delay (
  UINT32 Count
  )
{
  UINT32         i;         // for loop count
  EFI_INPUT_KEY  InputKey;
  
  for (i = 0; i <= Count; i++) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);
    
    if (InputKey.ScanCode == SCAN_F1) {
      return F1_PRESS;
    }
  }

  return F1_NOT_PRESS;
}

/**
  To reboot.

**/
VOID
RestartSystem (
  VOID
  )
{
  EFI_CPU_IO2_PROTOCOL       *CpuIo2;
  UINT8                     CommandBuffer;

  gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  //
  // restart using KBC (Standard CMD)
  //
  CommandBuffer = RESTART_COMMAND;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                COMMAND_PORT,
                1,
                &CommandBuffer
                );
  
  return;
}

/**
  To show main page.

**/
VOID
ShowMainPage (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"10-KBC\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"[F1].  Lighten the LED\n");
  Print(L"[F2].  Restart system\n");
  Print(L"[Esc]. Escape\n");
}

/**
  To show branch page (LED).

**/
VOID
ShowLedPage (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"LED is lightened...\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"[F1].  Back to home page\n");
}