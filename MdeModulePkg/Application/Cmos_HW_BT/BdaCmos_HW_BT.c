/** @file
  BdaCmos_HW_BT C Source File

;******************************************************************************
;* Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "BdaCmos_HW_BT.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_INPUT_KEY         InputKey;
  POSITION              CursorPosition;
  UINT8                 AllBranchPage;
  UINT8                 Mode;
  UINT8                 RecordBuffer[Count];  
  BOOLEAN               RecordBufferTime;

  Mode = MAIN_PAGE_MODE;
  AllBranchPage = 3;        // BDA, EBDA, CMOS

  //
  // initialization and into the main page
  //
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  ShowMainPage(&CursorPosition);
  
  //
  // choose mode loop
  //
  while (TRUE) {
    
    //
    // to change mode
    //
    ChangeMode:

    //
    // MAIN_PAGE_MODE
    // with up, down, F1, Esc key response
    //
    while (Mode == MAIN_PAGE_MODE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto Exit;
      }

      switch (InputKey.ScanCode) {

      case SCAN_UP:
        CursorPosition.row--;
        if (CursorPosition.row < MainPage_ResetPosition.row) {
          CursorPosition.row = MainPage_ResetPosition.row;
        }
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);    
        continue;

      case SCAN_DOWN:
        CursorPosition.row++;
        if (CursorPosition.row > (AllBranchPage + MainPage_ResetPosition.row - 1)) {
          CursorPosition.row = (AllBranchPage + MainPage_ResetPosition.row - 1);
        }
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        continue;

      case SCAN_F1:
        RecordBufferTime = 0;
        Mode = BRANCH_PAGE_MODE;

        if (CursorPosition.row == MainPage_ResetPosition.row) {
          ReadBda(&CursorPosition, RecordBuffer, &RecordBufferTime);
        } else if (CursorPosition.row == MainPage_ResetPosition.row + 1) {
          ReadEbda(&CursorPosition, RecordBuffer, &RecordBufferTime);
        } else if (CursorPosition.row == MainPage_ResetPosition.row + 2) {
          ReadCmos(&CursorPosition, RecordBuffer, &RecordBufferTime);
        }

        goto ChangeMode;
      }
    }

    //
    // BRANCH_PAGE_MODE
    // with F1, F2, Esc key response
    //
    while (Mode == BRANCH_PAGE_MODE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto Exit;
      }

      switch (InputKey.ScanCode) {

      case SCAN_F1:
        RecordBufferTime = 0;
        Mode = MAIN_PAGE_MODE;
        ShowMainPage(&CursorPosition);

        goto ChangeMode;

      case SCAN_F2:
        RecordBufferTime = 1;

        if (CursorPosition.column == 0) {
          ReadBda(&CursorPosition, RecordBuffer, &RecordBufferTime);
        } else if (CursorPosition.column == 1) {
          ReadEbda(&CursorPosition, RecordBuffer, &RecordBufferTime);
        } else if (CursorPosition.column == 2) {
          ReadCmos(&CursorPosition, RecordBuffer, &RecordBufferTime);
        }

        continue;
      }
    }  
  }

  //
  // to get out
  //
  Exit:

  Status = EFI_SUCCESS;

  return Status;
}

/**
  To set main page appearance.

**/
VOID 
SetMainPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                   08-BDA_CMOS                      |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|                       BDA                          |\n");
  Print(L"|                       EBDA                         |\n");
  Print(L"|                       CMOS                         |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key] : Choose                               |\n");
  Print(L"| [F1] : Enter                                       |\n");
  Print(L"| [Esc] : Escape                                     |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  To set register page appearance.

**/
VOID 
SetRegisterPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|  | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n");
  Print(L"|====================================================|\n");
  Print(L"|00|                                                 |\n");
  Print(L"|10|                                                 |\n");
  Print(L"|20|                                                 |\n");
  Print(L"|30|                                                 |\n");
  Print(L"|40|                                                 |\n");
  Print(L"|50|                                                 |\n");
  Print(L"|60|                                                 |\n");
  Print(L"|70|                                                 |\n");
  Print(L"|80|                                                 |\n");
  Print(L"|90|                                                 |\n");
  Print(L"|A0|                                                 |\n");
  Print(L"|B0|                                                 |\n");
  Print(L"|C0|                                                 |\n");
  Print(L"|D0|                                                 |\n");
  Print(L"|E0|                                                 |\n");
  Print(L"|F0|                                                 |\n");
  Print(L"|====================================================|\n");
  Print(L"| [F1]:Back     [F2]:renew     [Esc]:Escape          |\n");
  Print(L"|====================================================|\n");

  return;
}

/**
  Show main page.
  
  Set cursor's position by CursorPosition.
      
  @param  *CursorPosition  The cursor's position.                                        

**/
VOID 
ShowMainPage (
  POSITION *CursorPosition
  )
{
  SetMainPageAppearance();
  *CursorPosition = MainPage_ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);

  return;
}

/**
  Reset cursor position to {5, 4}.
      
  @param  *CursorPosition  The cursor's position.                                      

**/
VOID 
ResetCursorPosition (
  POSITION *CursorPosition
  )
{
  *CursorPosition = ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);

  return;
}

/**
  To judge the buffer is zero or not.
      
  @param  Buffer  To store read thing by byte.                                       

**/
VOID 
JudgeFontColor (
  UINT8 Buffer
  )
{
  if (Buffer > 0){
    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  } else {
    gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  }
  return;
}

/**
  To read and print BDA register.
      
  @param  *CursorPosition    The cursor's position.
  @param  *RecordBuffer[]     To record previous buffer.
  @param  *RecordBufferTime  To judge is from main page or not.                                        

**/
VOID 
ReadBda (
  POSITION *CursorPosition, 
  UINT8 RecordBuffer[], 
  UINT8 *RecordBufferTime
  )
{
  EFI_CPU_IO2_PROTOCOL               *CpuIo2;
  UINT16                             BdaOffset;
  UINT8                              Buffer[Count];

  gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  SetRegisterPageAppearance();
  ResetCursorPosition(CursorPosition);

  for (BdaOffset = 0; BdaOffset < Count; BdaOffset++) {
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  BdaSegment + BdaOffset,
                  1,
                  &Buffer[BdaOffset]
                  );

    if (*RecordBufferTime == 0) {
      *(RecordBuffer + BdaOffset) = Buffer[BdaOffset];
      JudgeFontColor(Buffer[BdaOffset]);
    } else {
      if (*(RecordBuffer + BdaOffset) != Buffer[BdaOffset]) {
        *(RecordBuffer + BdaOffset) = Buffer[BdaOffset];
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGREEN | EFI_BACKGROUND_BLACK);
      } else {
        JudgeFontColor(Buffer[BdaOffset]);
      }
    }

    if ((BdaOffset % 16) == 0) {
      Print(L"%2.2x", Buffer[BdaOffset]);
    } else if ((BdaOffset % 16) == 15) {
      Print(L"%3.2x", Buffer[BdaOffset]);
      CursorPosition->column = 5;
      CursorPosition->row++;
      gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
    } else {
      Print(L"%3.2x", Buffer[BdaOffset]);
    }
  }

  CursorPosition->column = 2;
  CursorPosition->row    = 0;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print(L"BDA");

  CursorPosition->column = 0;   // Judgement for renew

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  
  return;
}

/**
  To read and print EBDA register.
      
  @param  *CursorPosition    The cursor's position.
  @param  RecordBuffer[]     To record previous buffer.
  @param  *RecordBufferTime  To judge is from main page or not.                                        

**/
VOID 
ReadEbda (
  POSITION *CursorPosition, 
  UINT8 RecordBuffer[], 
  UINT8 *RecordBufferTime
  )
{
  EFI_CPU_IO2_PROTOCOL               *CpuIo2;
  UINT32                             EBdaSegment;
  UINT16                             ShiftToEbdaAddSegment;
  UINT16                             EbdaOffset;
  UINT8                              Buffer[Count];

  ShiftToEbdaAddSegment = 0x0e;
  EBdaSegment = 0;

  gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  SetRegisterPageAppearance();
  ResetCursorPosition(CursorPosition);

  CpuIo2->Mem.Read(
                CpuIo2,
                EfiCpuIoWidthUint16,
                BdaSegment + ShiftToEbdaAddSegment,
                1,
                &EBdaSegment
                );

  EBdaSegment = EBdaSegment << 4;

  for (EbdaOffset = 0; EbdaOffset < Count; EbdaOffset++) {
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  EBdaSegment + EbdaOffset,
                  1,
                  &Buffer[EbdaOffset]
                  );

    if (*RecordBufferTime == 0) {
      *(RecordBuffer + EbdaOffset) = Buffer[EbdaOffset];
      JudgeFontColor(Buffer[EbdaOffset]);
    } else {
      if (*(RecordBuffer + EbdaOffset) != Buffer[EbdaOffset]) {
        *(RecordBuffer + EbdaOffset) = Buffer[EbdaOffset];
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGREEN | EFI_BACKGROUND_BLACK);
      } else {
        JudgeFontColor(Buffer[EbdaOffset]);
      }
    }

    if ((EbdaOffset % 16) == 0) {
      Print(L"%2.2x", Buffer[EbdaOffset]);
    } else if ((EbdaOffset % 16) == 15) {
      Print(L"%3.2x\n", Buffer[EbdaOffset]);
      CursorPosition->column = 5;
      CursorPosition->row++;
      gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
    } else {
      Print(L"%3.2x", Buffer[EbdaOffset]);
    }
  }

  CursorPosition->column = 2;
  CursorPosition->row    = 0;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print(L"EBDA");

  CursorPosition->column = 1;   // Judgement for renew

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  return;
}

/**
  To read and print CMOS register.
      
  @param  *CursorPosition    The cursor's position.
  @param  *RecordBuffer[]     To record previous buffer.
  @param  *RecordBufferTime  To judge is from main page or not.                                        

**/
VOID 
ReadCmos (
  POSITION *CursorPosition, 
  UINT8 RecordBuffer[], 
  UINT8 *RecordBufferTime
  )
{
  EFI_CPU_IO2_PROTOCOL               *CpuIo2;
  UINT8                              CmosIndex;
  UINT8                              Buffer[CmosMaxIndex];

  gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  SetRegisterPageAppearance();
  ResetCursorPosition(CursorPosition);

  for (CmosIndex = 0; CmosIndex <= CmosMaxIndex; CmosIndex++) {
    CpuIo2->Io.Write(
                 CpuIo2,
                 EfiCpuIoWidthUint8,
                 CmosWritePort,
                 1,
                 &CmosIndex
                 );

    CpuIo2->Io.Read(
                 CpuIo2,
                 EfiCpuIoWidthUint8,
                 CmosReadPort,
                 1,
                 &Buffer[CmosIndex]
                 );
    
    if (*RecordBufferTime == 0) {
      *(RecordBuffer + CmosIndex) = Buffer[CmosIndex];
      JudgeFontColor(Buffer[CmosIndex]);
    } else {
      if (*(RecordBuffer + CmosIndex) != Buffer[CmosIndex]) {
        *(RecordBuffer + CmosIndex) = Buffer[CmosIndex];
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGREEN | EFI_BACKGROUND_BLACK);
      } else {
        JudgeFontColor(Buffer[CmosIndex]);
      }
    }

    if ((CmosIndex % 16) == 0) {
      Print(L"%2.2x", Buffer[CmosIndex]);
    } else if ((CmosIndex % 16) == 15) {
      Print(L"%3.2x", Buffer[CmosIndex]);
      CursorPosition->column = 5;
      CursorPosition->row++;
      gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
    } else {
      Print(L"%3.2x", Buffer[CmosIndex]);
    }
  }

  CursorPosition->column = 2;
  CursorPosition->row    = 0;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  Print(L"CMOS");

  CursorPosition->column = 2;   // Judgement for renew

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  return;
}