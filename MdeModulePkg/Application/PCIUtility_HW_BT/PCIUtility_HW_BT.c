/** @file
  PciUtility_06 C Source File

;******************************************************************************
;* Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PCIUtility_HW_BT.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{ 
  EFI_STATUS                    Status;  
  EFI_INPUT_KEY                 InputKey;
  POSITION                      CursorPosition;
  UINT8                         Mode;
  UINT8                         AllBranchPage;
  UINT8                         ReadMode;
  ADDRESS                       Address;

  //
  // initialization and into the main page
  //
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
  gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  Mode = MAIN_PAGE_MODE;
  ReadMode = BYTE;
  AllBranchPage = ShowMainPage(&CursorPosition);
  
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
          CursorPosition.row = (AllBranchPage + MainPage_ResetPosition.row - 1);
        }
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);    
        continue;

      case SCAN_DOWN:
        CursorPosition.row++;
        if (CursorPosition.row > (AllBranchPage + MainPage_ResetPosition.row - 1)) {
          CursorPosition.row = MainPage_ResetPosition.row;
        }
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        continue;

      case SCAN_F1:
        Mode = BRANCH_PAGE_MODE;
        ShowRegistPage(&Address, &CursorPosition);
        goto ChangeMode;
      }
    }

    //
    // BRANCH_PAGE_MODE
    // with up, down, right, left, F1, F2, F3, Esc key response
    //
    while (Mode == BRANCH_PAGE_MODE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto Exit;
      }

      switch (InputKey.ScanCode) {

      case SCAN_UP:
        CursorPosition.row--;
        if (CursorPosition.row < ReadMode1_ResetPosition.row) {
          CursorPosition.row = (ReadMode1_ResetPosition.row + 15);;
        }     

        DisplayPosition(ReadMode, &CursorPosition);
        
        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);    
        continue;

      case SCAN_DOWN:
        CursorPosition.row++;
        if (CursorPosition.row > (ReadMode1_ResetPosition.row + 15)) {
          CursorPosition.row = ReadMode1_ResetPosition.row;
        }

        DisplayPosition(ReadMode, &CursorPosition);

        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        continue;
      
      case SCAN_RIGHT:
        CursorPosition.column = CursorPosition.column + (ReadMode * 3);
        if (CursorPosition.column > (52 - 2 * ReadMode)) {
          CursorPosition.column = (ReadMode + 4);
        }

        DisplayPosition(ReadMode, &CursorPosition);

        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        continue;

      case SCAN_LEFT:
        CursorPosition.column = CursorPosition.column - (ReadMode * 3);
        if (CursorPosition.column < (ReadMode + 4)) {
          CursorPosition.column = (52 - 2 * ReadMode);
        }

        DisplayPosition(ReadMode, &CursorPosition);

        gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
        continue;

      case SCAN_F1:
        Mode = MAIN_PAGE_MODE;
        ReadMode = BYTE;
        ShowMainPage(&CursorPosition);
        goto ChangeMode;
      
      case SCAN_F2:
        ReadMode *= 2;
        if (ReadMode > 4) ReadMode = BYTE;
        ReadRegist(&Address, &CursorPosition, ReadMode);
        continue;
      case SCAN_F3:
        Mode = WRITE_MODE;
        break;
      }
    }

    //
    // WRITE_MODE
    // with number(0~9),alphabet(a~f) key response
    //
    if (Mode == WRITE_MODE) {
      WRITE_BUFFER ReadMode1_WriteBuffer[2];
      WRITE_BUFFER ReadMode2_WriteBuffer[4];
      WRITE_BUFFER ReadMode4_WriteBuffer[8];
      UINT8 count;
      UINT8 i;
      UINT32 WriteBufferConvert;

      //
      // initialize write buffer、count
      //
      for (i = 0; i < 2; i++) {
        ReadMode1_WriteBuffer[i].value = 0;
        ReadMode1_WriteBuffer[i].position = ReadMode1_ResetPosition;
      }
      for (i = 0; i < 4; i++) {
        ReadMode2_WriteBuffer[i].value = 0;
        ReadMode2_WriteBuffer[i].position = ReadMode2_ResetPosition;
      }
      for (i = 0; i < 8; i++) {
        ReadMode4_WriteBuffer[i].value = 0;
        ReadMode4_WriteBuffer[i].position = ReadMode4_ResetPosition;
      }
      count = 0;                // for write_buffer operation
      WriteBufferConvert = 0;   // buffer for write_buffer

      while (TRUE) {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

        if (ReadMode == BYTE) {
          if (count == (ReadMode * 2)) {
            count = 0;
            gST->ConOut->ClearScreen(gST->ConOut);
            Address.Offset = ((ReadMode1_WriteBuffer[0].position.column -           \
                               ReadMode1_ResetPosition.column) / 3) * (ReadMode) +  \
                               16 * (ReadMode1_WriteBuffer[0].position.row -        \
                               ReadMode1_ResetPosition.row);

            WriteBufferConvert = ((ReadMode1_WriteBuffer[0].value << 4) | (ReadMode1_WriteBuffer[1].value));
            WriteRegist(&Address, WriteBufferConvert, ReadMode);
            ReadRegist(&Address, &CursorPosition, ReadMode);

            Mode = BRANCH_PAGE_MODE;
            goto ChangeMode;
          }

          if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
            ReadMode1_WriteBuffer[count].value = (UINT8)InputKey.UnicodeChar - 87;
            ReadMode1_WriteBuffer[count].position.column = CursorPosition.column;
            ReadMode1_WriteBuffer[count].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            count++;
            continue;

          } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
            ReadMode1_WriteBuffer[count].value = (UINT8)InputKey.UnicodeChar - 48;
            ReadMode1_WriteBuffer[count].position.column = CursorPosition.column;
            ReadMode1_WriteBuffer[count].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            count++;
            continue;
          }

        } else if (ReadMode == WORD) {
          if (count == (ReadMode * 2)) {
            count = 0;
            gST->ConOut->ClearScreen(gST->ConOut);
            Address.Offset = ((ReadMode2_WriteBuffer[0].position.column -           \
                               ReadMode2_ResetPosition.column) / 6) * (ReadMode) +  \
                               16 * (ReadMode2_WriteBuffer[0].position.row -        \
                               ReadMode2_ResetPosition.row);
            
            WriteBufferConvert = ((ReadMode2_WriteBuffer[0].value << 12) | \
                                  (ReadMode2_WriteBuffer[1].value << 8)  | \
                                  (ReadMode2_WriteBuffer[2].value << 4)  | \
                                  (ReadMode2_WriteBuffer[3].value));

            WriteRegist(&Address, WriteBufferConvert, ReadMode);
            ReadRegist(&Address, &CursorPosition, ReadMode);

            Mode = BRANCH_PAGE_MODE;
            goto ChangeMode;
          }

          if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
            ReadMode2_WriteBuffer[count].value = (UINT8)InputKey.UnicodeChar - 87;
            ReadMode2_WriteBuffer[count].position.column = CursorPosition.column;
            ReadMode2_WriteBuffer[count].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            count++;
            continue;

          } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
            ReadMode2_WriteBuffer[count].value = (UINT8)InputKey.UnicodeChar - 48;
            ReadMode2_WriteBuffer[count].position.column = CursorPosition.column;
            ReadMode2_WriteBuffer[count].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            count++;
            continue;
          }

        } else if (ReadMode == DWORD) {
          if (count == (ReadMode * 2)) {
            count = 0;
            gST->ConOut->ClearScreen(gST->ConOut);
            Address.Offset = ((ReadMode4_WriteBuffer[0].position.column -             \
                               ReadMode4_ResetPosition.column) / 12) * (ReadMode) +   \
                               16 * (ReadMode4_WriteBuffer[0].position.row -          \
                               ReadMode4_ResetPosition.row);
            
            WriteBufferConvert = ((ReadMode4_WriteBuffer[0].value << 28) | \
                                  (ReadMode4_WriteBuffer[1].value << 24) | \
                                  (ReadMode4_WriteBuffer[2].value << 20) | \
                                  (ReadMode4_WriteBuffer[3].value << 16) | \
                                  (ReadMode4_WriteBuffer[4].value << 12) | \
                                  (ReadMode4_WriteBuffer[5].value << 8)  | \
                                  (ReadMode4_WriteBuffer[6].value << 4)  | \
                                  (ReadMode4_WriteBuffer[7].value));

            WriteRegist(&Address, WriteBufferConvert, ReadMode);
            ReadRegist(&Address, &CursorPosition, ReadMode);

            Mode = BRANCH_PAGE_MODE;
            goto ChangeMode;
          }

          if (('a' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= 'f')) {
            ReadMode4_WriteBuffer[count].value = (UINT8)InputKey.UnicodeChar - 87;
            ReadMode4_WriteBuffer[count].position.column = CursorPosition.column;
            ReadMode4_WriteBuffer[count].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            count++;
            continue;

          } else if (('0' <= InputKey.UnicodeChar && InputKey.UnicodeChar <= '9')) {
            ReadMode4_WriteBuffer[count].value = (UINT8)InputKey.UnicodeChar - 48;
            ReadMode4_WriteBuffer[count].position.column = CursorPosition.column;
            ReadMode4_WriteBuffer[count].position.row = CursorPosition.row;
            Print(L"%c", InputKey.UnicodeChar);

            CursorPosition.column = CursorPosition.column + 1;
            gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition.column, CursorPosition.row);
            count++;
            continue;
          }
        }

        if (InputKey.ScanCode == SCAN_ESC) {
          gST->ConOut->ClearScreen(gST->ConOut);
          goto Exit;
        }

        switch (InputKey.ScanCode) {

        case SCAN_F3:
          Mode = BRANCH_PAGE_MODE;
          break;
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

/**
  Show PCI devices's Bus、Device、Function.
  
  Set cursor's position by CursorPosition.
      
  @param  CursorPosition  The cursor's position  
                                               
  @return The number of searched PCI device.

**/
UINT8 
ShowMainPage (
  POSITION *CursorPosition
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_STATUS                           Status;
  UINT64                               PciAddress;
  UINT16                               Bus;
  UINT16                               Device;
  UINT16                               Func;
  UINT16                               Offset;
  UINT8                                PciData8;            // read buffer  
  UINT8                                Num; 
  UINT8                                Buffer[4];

  Num = 0;
  PciData8 = 0;

  gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
  gST->ConOut->ClearScreen(gST->ConOut);

  Print(L"|====================================================|\n");
  Print(L"|                    PCI   List                      |\n");
  Print(L"|====================================================|\n");
  Print(L"|              BUS       DEV       FUN               |\n");
  Print(L"|====================================================|\n");
  for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
    for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
      for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
        for (Offset = 0; Offset < 4; Offset++) {                    // just get vendor_ID & device_ID
          PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
          Status = PciRootBridgeIo->Pci.Read (
                                      PciRootBridgeIo,              // This
                                      EfiPciWidthUint8,             // Width
                                      PciAddress,                   // Address
                                      1,                            // Count
                                      &PciData8                     // *Buffer
                                      );
          Buffer[Offset] = PciData8;                                  
        }
        if (Buffer[0] != 0xff && Buffer[1] != 0xff) {
          Num++;
          Print(L"|%17.2x%10.2x%10.2x               |\n", Bus, Device, Func);
        }
      }
    }
  }
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key] : Choose                               |\n");
  Print(L"| [F1] : Enter                                       |\n");
  Print(L"| [Esc] : Escape                                     |\n");
  Print(L"|====================================================|\n");

  *CursorPosition = MainPage_ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);

  return Num;
}

/**
  When the main page move into the regist page call this function.

  @param  Address         To operate Pci.Read().
  @param  CursorPosition  When the regist show finished,set the cursor's position  

**/
VOID 
ShowRegistPage (
  ADDRESS *Address, 
  POSITION *CursorPosition
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_STATUS                           Status;
  UINT64                               PciAddress;
  UINT16                               Bus;
  UINT16                               Device;
  UINT16                               Func;
  UINT16                               Offset;
  UINT8                                PciData8;                    // read buffer
  UINT8                                Num; 
  UINT8                                Buffer[256] = {0};

  Num = 0;
  PciData8 = 0;

  gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
  gST->ConOut->ClearScreen(gST->ConOut);

  for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
    for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
      for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
        for (Offset = 0; Offset < 256; Offset++) {                   // get all register
          PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
          Status = PciRootBridgeIo->Pci.Read (
                                      PciRootBridgeIo,               // This
                                      EfiPciWidthUint8,              // Width
                                      PciAddress,                    // Address
                                      1,                             // Count
                                      &PciData8                      // *Buffer
                                      );
          Buffer[Offset] = PciData8;

          if (Buffer[0] == 0xff && Buffer[1] == 0xff) 
            break;
        }
        if (Buffer[0] != 0xff && Buffer[1] != 0xff) {
          Num++;
          if (Num == ((CursorPosition->row) - MainPage_ResetPosition.row + 1)) {
            Address->Bus = Bus;
            Address->Device = Device;
            Address->Func = Func;
            Address->Offset = 0;

            Print(L"|  BUS:%.2x  DEV:%.2x  FUN:%.2x                            |\n", Bus, Device, Func);
            Print(L"|====================================================|\n");
            Print(L"|00| 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n");
            Print(L"|====================================================|\n");

            for (Offset = 0; Offset < 256; Offset++) {
              if ((Offset % 16) == 0) 
                Print(L"|%.2x|", Offset);

              Print(L"%3.2x", Buffer[Offset]);

              if ((Offset % 16) == 15) 
                Print(L" |\n");
            }
          }
        }
      }
    }
  }
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key]:Choose   [F1]:Back      [F2]:Read Mode |\n");
  Print(L"| [F3]:Modify          [Esc]:Escape                  |\n");
  Print(L"|====================================================|\n");

  *CursorPosition = ReadMode1_ResetPosition;
  gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);

  return;
}

/**
  When change read mode,call this function.

  This function is used to other function.

  @param  Address         To operate Pci.Read().
  @param  CursorPosition  When the regist show finished,set the cursor's position
  @param  ReadMode        There are threemode
                          BYTE  -> read a byte
                          WORD  -> read two bytes
                          DWORD -> read four bytes

**/
VOID 
ReadRegist (
  ADDRESS *Address, 
  POSITION *CursorPosition, 
  UINT8 ReadMode
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_STATUS                           Status;
  UINT64                               PciAddress;
  UINT8                                PciData8;          // 8bit  read buffer
  UINT16                               PciData16;         // 16bit read buffer
  UINT32                               PciData32;         // 32bit read buffer
  UINT16                               Offset;

  gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
  gST->ConOut->ClearScreen(gST->ConOut);

  Print(L"|  BUS:%.2x  DEV:%.2x  FUN:%.2x                            |\n", Address->Bus, 
                                                                           Address->Device, 
                                                                           Address->Func);

  Print(L"|====================================================|\n");
  if (ReadMode == BYTE) {
    Print(L"|00| 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |\n");
    Print(L"|====================================================|\n");

    for (Offset = 0; Offset < 256; Offset++) {
      PciAddress = EFI_PCI_ADDRESS (Address->Bus, Address->Device, Address->Func, Offset);
      Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,      // This
                                  EfiPciWidthUint8,     // Width
                                  PciAddress,           // Address
                                  1,                    // Count
                                  &PciData8             // *Buffer
                                  );
      if ((Offset % 16) == 0) 
        Print(L"|%.2x|", Offset);

      Print(L"%3.2x", PciData8);

      if ((Offset % 16) == 15) 
        Print(L" |\n");   
    }
  } else if (ReadMode == WORD) {
    Print(L"|00|  0100  0302  0504  0706  0908  0B0A  0D0C  0F0E |\n");
    Print(L"|====================================================|\n");

    for (Offset = 0; Offset < 256; Offset += 2) {
      PciAddress = EFI_PCI_ADDRESS (Address->Bus, Address->Device, Address->Func, Offset);
      Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,      // This
                                  EfiPciWidthUint16,    // Width
                                  PciAddress,           // Address
                                  1,                    // Count
                                  &PciData16            // *Buffer
                                  );
      if ((Offset % 16) == 0) 
        Print(L"|%.2x|", Offset);

      Print(L"%6.4x", PciData16);

      if ((Offset % 16) == 14) 
        Print(L" |\n");
    }
  } else if (ReadMode == DWORD) {
    Print(L"|00|    03020100    07060504    0B0A0908    0F0E0D0C |\n");
    Print(L"|====================================================|\n");

    for (Offset = 0; Offset < 256; Offset += 4) {
      PciAddress = EFI_PCI_ADDRESS (Address->Bus, Address->Device, Address->Func, Offset);
      Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,      // This
                                  EfiPciWidthUint32,    // Width
                                  PciAddress,           // Address
                                  1,                    // Count
                                  &PciData32            // *Buffer
                                  );

      if ((Offset % 16) == 0) 
        Print(L"|%.2x|", Offset);

      Print(L"%12.8x", PciData32);

      if ((Offset % 16) == 12) 
        Print(L" |\n");
    }
  }                     
  Print(L"|====================================================|\n");
  Print(L"| [Arrow key]:Choose   [F1]:Back      [F2]:Read Mode |\n");
  Print(L"| [F3]:Modify          [Esc]:Escape                  |\n");
  Print(L"|====================================================|\n");

  if (ReadMode == BYTE) {
    *CursorPosition = ReadMode1_ResetPosition;
    gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  } else if (ReadMode == WORD) {
    *CursorPosition = ReadMode2_ResetPosition;
    gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  } else if (ReadMode == DWORD) {
    *CursorPosition = ReadMode4_ResetPosition;
    gST->ConOut->SetCursorPosition(gST->ConOut, CursorPosition->column, CursorPosition->row);
  }                            

  return;
}

/**
  When the position has chosen,call this function can write data in.

  @param  Address             To operate Pci.Write().
  @param  WriteBufferConvert  The buffer of write data. 
  @param  ReadMode            There are threemode
                              BYTE  -> read a byte
                              WORD  -> read two bytes
                              DWORD -> read four bytes

**/
VOID 
WriteRegist (
  ADDRESS *Address, 
  UINT32 WriteBufferConvert, 
  UINT8 ReadMode
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_STATUS                           Status;
  UINT64                               PciAddress; 
  UINT8                                PciData8;            // 8bit  write buffer
  UINT16                               PciData16;           // 16bit write buffer
  UINT32                               PciData32;           // 32bit write buffer

  if (ReadMode == BYTE) {

    PciData8 = (UINT8)WriteBufferConvert;

    gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
    gST->ConOut->ClearScreen(gST->ConOut);

    PciAddress = EFI_PCI_ADDRESS (Address->Bus, Address->Device, Address->Func, Address->Offset);
    Status = PciRootBridgeIo->Pci.Write (
                                PciRootBridgeIo,            // This
                                EfiPciWidthUint8,           // Width
                                PciAddress,                 // Address
                                1,                          // Count
                                &PciData8                   // *Buffer
                                );                                 
    return;

  } else if (ReadMode == WORD) {

    PciData16 = (UINT16)WriteBufferConvert;

    gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
    gST->ConOut->ClearScreen(gST->ConOut);

    PciAddress = EFI_PCI_ADDRESS (Address->Bus, Address->Device, Address->Func, Address->Offset);
    Status = PciRootBridgeIo->Pci.Write (
                                PciRootBridgeIo,            // This
                                EfiPciWidthUint16,          // Width
                                PciAddress,                 // Address
                                1,                          // Count
                                &PciData16                  // *Buffer
                                );                                 
    return;

  } else if (ReadMode == DWORD) {

    PciData32 = WriteBufferConvert;

    gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
    gST->ConOut->ClearScreen(gST->ConOut);

    PciAddress = EFI_PCI_ADDRESS (Address->Bus, Address->Device, Address->Func, Address->Offset);
    Status = PciRootBridgeIo->Pci.Write (
                                PciRootBridgeIo,            // This
                                EfiPciWidthUint32,          // Width
                                PciAddress,                 // Address
                                1,                          // Count
                                &PciData32                  // *Buffer
                                );                                 
    return;
  }
}

/**
  In the show regist mode,the function will show the offset site.

  @param  ReadMode        
  @param  CursorPosition  

**/
VOID 
DisplayPosition (
  UINT8 ReadMode, 
  POSITION *CursorPosition
  )
{
  if (ReadMode == BYTE) {                                                                          
    gST->ConOut->SetCursorPosition(gST->ConOut, 1, 2);                                             
    Print(L"%.2x", (CursorPosition->column - 5) / 3 * ReadMode + (CursorPosition->row -               \
                                                                  ReadMode1_ResetPosition.row) * 16); 
  } else if (ReadMode == WORD) {                                                                   
    gST->ConOut->SetCursorPosition(gST->ConOut, 1, 2);                                             
    Print(L"%.2x", (CursorPosition->column - 6) / 6 * ReadMode + (CursorPosition->row -               \
                                                                  ReadMode2_ResetPosition.row) * 16); 
  } else if (ReadMode == DWORD) {                                                                  
    gST->ConOut->SetCursorPosition(gST->ConOut, 1, 2);                                             
    Print(L"%.2x", (CursorPosition->column - 8) / 12 * ReadMode + (CursorPosition->row -              \
                                                                   ReadMode4_ResetPosition.row) * 16); 
  }
}