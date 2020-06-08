/** @file
  SIO_09 C Source File

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

#include "SIO_09.h"

CONST POSITION  MainPage_ResetPosition  = {46, 6};
CONST POSITION  ResetPosition           = {5, 4};
CONST POSITION  OffsetPosition          = {1, 2};
CONST POSITION  TitlePosition           = {2, 0};

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_INPUT_KEY         InputKey;
  UINT8                 Mode;
  POSITION              CursorPosition;
  WRITE_BUFFER          WriteBuffer[2];
  UINT8                 Ldn;
  UINT32                InputAmount;
  UINT64                ModifyValue;
  
  Mode                  = MAIN_PAGE_MODE;
  Ldn                   = 0;
  ModifyValue           = 0;
  CursorPosition.column = 0;
  CursorPosition.row    = 0;
  InitializeArray(WriteBuffer);

  //
  // initialization and into the main page
  //
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  

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
    // with Number(0~f), Esc key response
    //
    if (Mode == MAIN_PAGE_MODE) {
      ShowMainPage(&CursorPosition);
      
      InputAmount = 1;
      Ldn = (UINT8)InputValue(WriteBuffer, &InputAmount);

      //
      // when press esc to trigger
      //
      if (InputAmount == 0) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto Exit;

      } else {
        ShowSio(&CursorPosition, &Ldn);
        Mode = BRANCH_PAGE_MODE;
        goto ChangeMode;
      }
    }

    //
    // BRANCH_PAGE_MODE
    // with up, down, right, left, F1(home), F2(modify), Esc key response
    //
    if (Mode == BRANCH_PAGE_MODE) {
      while (TRUE) {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

        if (InputKey.ScanCode == SCAN_ESC) {
          gST->ConOut->ClearScreen(gST->ConOut);
          goto Exit;
        }
        
        switch (InputKey.ScanCode) {

        case SCAN_F1:
          Mode = MAIN_PAGE_MODE;
          ShowMainPage(&CursorPosition);
          InitializeArray(WriteBuffer);
          goto ChangeMode;

        case SCAN_F2:
          if (((CursorPosition.column - ResetPosition.column) / 3 + \
               (CursorPosition.row - ResetPosition.row) * 16) == 0xaa) {
            continue;
          }
          InputAmount = 2;
          ModifyValue = InputValue(WriteBuffer, &InputAmount);

          //
          // when press esc to trigger
          //
          if (InputAmount == 0) {
            ShowSio(&CursorPosition, &Ldn);
            InitializeArray(WriteBuffer);
            continue;

          } else {
            WriteSio(&CursorPosition, &Ldn, ModifyValue);
            ShowSio(&CursorPosition, &Ldn);
            continue;
          }

        case SCAN_UP:
          CursorPosition.row--;
          if (CursorPosition.row < ResetPosition.row) {
            CursorPosition.row = ResetPosition.row;
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);   
          ShowOffset(&CursorPosition); 
          continue;

        case SCAN_DOWN:
          CursorPosition.row++;
          if (CursorPosition.row > (ResetPosition.row + DownEdgeInterval)) {
            CursorPosition.row = (ResetPosition.row + DownEdgeInterval);
          }

          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          ShowOffset(&CursorPosition);
          continue;
        
        case SCAN_RIGHT:
          CursorPosition.column += 3;
          if (CursorPosition.column > RightEdgeInterval) {
            CursorPosition.column = RightEdgeInterval;
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          ShowOffset(&CursorPosition);
          continue;

        case SCAN_LEFT:
          CursorPosition.column -= 3;
          if (CursorPosition.column < LeftEdgeInterval) {
            CursorPosition.column = LeftEdgeInterval;
          }
          gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
          ShowOffset(&CursorPosition);
          continue;
        }
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

EFI_STATUS
WriteSio (
  POSITION *CursorPosition,
  UINT8    *Ldn,
  UINT64   ModifyValue
  )
{
  EFI_STATUS            Status;
  EFI_CPU_IO2_PROTOCOL  *CpuIo2;
  UINT8                 AddressBuffer;
  UINT8                 DataBuffer;
  
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  //
  // Unlock
  //
  AddressBuffer = Unlock;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                ); 
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                ); 

  //
  // Select device
  //
  AddressBuffer = LdnReg;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                ); 

  AddressBuffer = *Ldn;               
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                DataPort,
                1,
                &AddressBuffer   
                );

  //
  // Write register
  //
  AddressBuffer = (UINT8)((CursorPosition->column - ResetPosition.column) / 3 + \
                          (CursorPosition->row - ResetPosition.row) * 16);
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                );

  DataBuffer = (UINT8)ModifyValue;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                DataPort,
                1,
                &DataBuffer   
                );
  
  if (AddressBuffer == LdnReg) {
    *Ldn = (UINT8)ModifyValue;
  }
  
  //
  // Lock
  //
  AddressBuffer = Lock;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                0x2E,
                1,
                &AddressBuffer   
                ); 

  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
ReadSio (
  POSITION *CursorPosition,
  UINT8    *Ldn
  )
{
  EFI_STATUS            Status;
  EFI_CPU_IO2_PROTOCOL  *CpuIo2;
  UINT8                 AddressBuffer;
  UINT8                 DataBuffer;
  UINT32                i;              // for loop count
  
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);  

  //
  // Unlock
  //
  AddressBuffer = Unlock;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                ); 
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                ); 

  //
  // Select device
  //
  AddressBuffer = LdnReg;
  Status = CpuIo2->Io.Write (
                         CpuIo2,
                         EfiCpuIoWidthUint8,
                         AddressPort,
                         1,
                         &AddressBuffer   
                         );
  if (EFI_ERROR(Status)) {
    gST->ConOut->ClearScreen(gST->ConOut);
    Print(L"%r\nPlease press F1 to home.", Status);
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    return Status;
  } 

  AddressBuffer = *Ldn;               
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                DataPort,
                1,
                &AddressBuffer   
                );

  //
  // Show register
  //
  for (i = 0; i < MaxIndex; i++){
    if ( i == 0xaa) {
      Print(L" --");
      continue;
    }
    AddressBuffer = (UINT8)i;
    CpuIo2->Io.Write (
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  AddressPort,
                  1,
                  &AddressBuffer   
                  );

    CpuIo2->Io.Read (
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  DataPort,
                  1,
                  &DataBuffer   
                  );

    if ((i % 16) == 0) {
      Print(L"%2.2x", DataBuffer);
    } else if ((i % 16) == 15) {
      Print(L"%3.2x\n", DataBuffer);
      CursorPosition->column = ResetPosition.column;
      CursorPosition->row++;
      gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
    } else {
      Print(L"%3.2x", DataBuffer);
    }
  }

  //
  // Lock
  //
  AddressBuffer = Lock;
  CpuIo2->Io.Write (
                CpuIo2,
                EfiCpuIoWidthUint8,
                AddressPort,
                1,
                &AddressBuffer   
                ); 

  *CursorPosition = ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);

  Status = EFI_SUCCESS;

  return Status;
}

VOID 
SetMainPageAppearance (
  VOID
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|                      09-SIO                        |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"|                                                    |\n");
  Print(L"|     Please input logical device number(0~f):       |\n");
  Print(L"|                                                    |\n");
  Print(L"|====================================================|\n");
  Print(L"| [Num]:Number            [Esc]:Escape               |\n");
  Print(L"|====================================================|\n");

  return;
}

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
  Print(L"| [F1]:Home               [F2]:Modify                |\n");
  Print(L"| [Arrow key]:Choose      [Esc]:Escape               |\n");
  Print(L"|====================================================|\n");

  return;
}

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

VOID 
ShowRegisterPage (
  POSITION *CursorPosition
  )
{
  SetRegisterPageAppearance();
  *CursorPosition = ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);

  return;
}

VOID 
ShowOffset (
  POSITION *CursorPosition
  )
{
  POSITION PreviousPosition;
  UINT8    Offset;

  PreviousPosition = *CursorPosition;
  Offset = (UINT8)((CursorPosition->column - ResetPosition.column) / 3 + \
                          (CursorPosition->row - ResetPosition.row) * 16);

  *CursorPosition = OffsetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  Print(L"%.2x", Offset);

  *CursorPosition = PreviousPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);

  return;
}

VOID 
ShowTitle (
  POSITION *CursorPosition,
  UINT8    *Ldn
  )
{
  POSITION PreviousPosition;
  PreviousPosition = *CursorPosition;

  *CursorPosition = TitlePosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);

  switch(*Ldn) {

  case 0x0:
  case 0x4:
  case 0xf:
    Print(L"LDN:%2x (Reserved)", *Ldn);
    break;
  case 0x1:
    Print(L"LDN:%2x (Parallel Port)", *Ldn);
    break;
  case 0x2:
    Print(L"LDN:%2x (UARTA)", *Ldn);
    break;
  case 0x3:
    Print(L"LDN:%2x (UARTB)", *Ldn);
    break;
  case 0x5:
    Print(L"LDN:%2x (Keyboard Controller)", *Ldn);
    break;
  case 0x6:
    Print(L"LDN:%2x (CIR)", *Ldn);
    break;
  case 0x7:
    Print(L"LDN:%2x (GPIO0~GPIO7)", *Ldn);
    break;
  case 0x8:
    Print(L"LDN:%2x (PORT80 UART)", *Ldn);
    break;
  case 0x9:
    Print(L"LDN:%2x (GPIO8~9, GPIO0 Enhance, GPIO1 Enhance)", *Ldn);
    break;
  case 0xa:
    Print(L"LDN:%2x (ACPI)", *Ldn);
    break;
  case 0xb:
    Print(L"LDN:%2x (EC Space)", *Ldn);
    break;
  case 0xc:
    Print(L"LDN:%2x (RTC Timer)", *Ldn);
    break;
  case 0xd:
    Print(L"LDN:%2x (Deep Sleep, Power Fault)", *Ldn);
    break;
  case 0xe:
    Print(L"LDN:%2x (Fan Assign)", *Ldn);
    break;
  default:
    Print(L"LDN:%2x (Unknow)", *Ldn);
    break;
  }

  *CursorPosition = PreviousPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  return;
}

VOID 
ShowSio (
  POSITION *CursorPosition,
  UINT8    *Ldn
  )
{
  ShowRegisterPage(CursorPosition);
  ReadSio(CursorPosition, Ldn);
  ShowTitle(CursorPosition, Ldn);
  ShowOffset(CursorPosition);
  return;
}

UINT64
InputValue (
  WRITE_BUFFER  *WriteBuffer,
  UINT32        *InputAmount
  )
{
  EFI_INPUT_KEY  InputKey;
  INT32          WriteCount;
  UINT32         i;           // for loop count
  UINT64         Value;

  WriteCount = 0;
  Value = 0;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if ((UINT32)WriteCount == *InputAmount) {

      for (i = 0; i < *InputAmount; i++) {
        Value |= WriteBuffer[i].value << ((*InputAmount-i-1) * 4);
      }

      return Value;
    }

    if (InputKey.ScanCode == SCAN_ESC) {
      *InputAmount = 0;
      return 0;
    }

    if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
      
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 87;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;
      
      continue;

    } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 48;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;

      continue;
    } else if (InputKey.UnicodeChar == CHAR_BACKSPACE) {

      WriteCount--;
    
      if(WriteCount < 0) {
        WriteCount = 0;
      } 
    
      WriteBuffer[WriteCount].value = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar);

      continue;
    }
  }
}

VOID 
InitializeArray(
  WRITE_BUFFER  *WriteBuffer
  )
{
  UINT32 i;   // for loop count

  for (i = 0; i < 2; i++) {
    WriteBuffer[i].value = 0;
    WriteBuffer[i].position.column = 0;
    WriteBuffer[i].position.row = 0;
  }
}