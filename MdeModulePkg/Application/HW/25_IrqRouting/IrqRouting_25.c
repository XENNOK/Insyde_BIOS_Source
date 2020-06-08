/** @file
  IrqRouting_25 C Source File

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

#include "IrqRouting_25.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE  ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_INPUT_KEY           InputKey;
  UINT16                  TableSize;
  UINT8                   *Buffer;

  TableSize = 0;
  Buffer = GetIrqRoutingTable(&TableSize);
  
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

    switch (InputKey.UnicodeChar) {
    
    case '1':
      GetHeaderInfo(Buffer);
      ShowMainPage();
      break;
    case '2':
      GetDeviceInfo(Buffer);
      ShowMainPage();
      break;
    }
  }

  gBS->FreePool(Buffer);

  Status = EFI_SUCCESS;
  return Status;
}

/**
  To get IRQ Routing Table.

  @param  *TableSize    The IRQ routing table size, include header's size.

  @retval UINT8*        All registers of the IRQ routing table.

**/
UINT8*
GetIrqRoutingTable (
  UINT16  *TableSize
  ) 
{
  EFI_CPU_IO2_PROTOCOL    *CpuIo2;
  UINT64                  Address;
  
  UINT8                   CheckBuffer[4];     // for checking "Signature", "Table Size"
  UINT8                   *DataBuffer;        // to store all register
  UINT64                  i;                  // for loop count

  DataBuffer = NULL;

  gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  for (Address = 0xf0000; Address <= 0xfffff; Address++) { 
    //
    // find "Signature"
    //   
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  Address,     
                  1,
                  &CheckBuffer[0]   
                  );
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  Address + 1,     
                  1,
                  &CheckBuffer[1]   
                  );
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  Address + 2,     
                  1,
                  &CheckBuffer[2]   
                  );                     
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  Address + 3,     
                  1,
                  &CheckBuffer[3]   
                  );

    if (CheckBuffer[0] == '$' && CheckBuffer[1] == 'P' && CheckBuffer[2] == 'I' && CheckBuffer[3] == 'R') {
     
      //
      // find "Table Size"
      //  
      CpuIo2->Mem.Read(
                    CpuIo2,
                    EfiCpuIoWidthUint8,
                    Address + 6,     
                    1,
                    &CheckBuffer[0]   
                    );
      CpuIo2->Mem.Read(
                    CpuIo2,
                    EfiCpuIoWidthUint8,
                    Address + 7,     
                    1,
                    &CheckBuffer[1]   
                    );
      *TableSize = CheckBuffer[0] + 0x100 * CheckBuffer[1];   
      break;                      
    }
  }

  gBS->AllocatePool(
                 EfiBootServicesData,
                 *TableSize,
                 &DataBuffer
                 );

  //
  // store all register in "DataBuffer"
  //
  for (i = 0; i < *TableSize; i++) {
    CpuIo2->Mem.Read(
                  CpuIo2,
                  EfiCpuIoWidthUint8,
                  Address + i,     
                  1,
                  DataBuffer + i 
                  );
  }

  return DataBuffer;
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

  SetMainPageAppearance();

  return;
}

/**
  To set main page appearance.

**/
VOID
SetMainPageAppearance (
  VOID
  )
{
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"25-IRQ Routing\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"  1. Get PIRQ Routing Table Header.\n");
  Print(L"  2. Dump Slot IRQ Routing.\n");
  
  Print(L"\n[Opt]:Option   [Esc]:Escape\n");

  return;
}

/**
  To show IRQ Routing Table header infomation.

  @param  *Buffer    The all IRQ routing table.

**/
VOID
GetHeaderInfo (
  UINT8  *Buffer
  )
{

  EFI_INPUT_KEY  InputKey;

  gST->ConOut->ClearScreen(gST->ConOut);

  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"PIRQ Routing Table Header\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);



  Print (L"  Signature                                : %c%c%c%c\n",
    Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
  Print (L"  Minor Version                            : 0x%.2x\n", Buffer[4]);
  Print (L"  Major Version                            : 0x%.2x\n", Buffer[5]);
  Print (L"  Table Size                               : %d\n", ((Buffer[7]*0x100) + Buffer[6]));
  Print (L"  PCI Interrupt Router's Bus               : 0x%.2x\n", Buffer[8]);
  Print (L"  PCI Interrupt Router's Device            : 0x%.2x\n", Buffer[9]>>3);
  Print (L"  PCI Interrupt Router's Function          : 0x%.2x\n", Buffer[9]&0x07);
  Print (L"  PCI Exclusive IRQs                       : 0x%.2x%.2x\n", Buffer[11], Buffer[10]);
  Print (L"  Compatible PCI Interrupt Router Vendor ID: 0x%.2x%.2x\n", Buffer[13], Buffer[12]);
  Print (L"  Compatible PCI Interrupt Router Device ID: 0x%.2x%.2x\n", Buffer[15], Buffer[14]);
  Print (L"  Miniport Data                            : 0x%.2x%.2x%.2x%.2x\n", 
    Buffer[19], Buffer[18], Buffer[17], Buffer[16]);
  Print (L"  Reserved                                 : 0x%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n", 
    Buffer[30], Buffer[29], Buffer[28], Buffer[27],
    Buffer[26], Buffer[25], Buffer[24], Buffer[23],
    Buffer[22], Buffer[21], Buffer[20]);
  Print (L"  Checksum                                 : 0x%.2x\n", Buffer[31]);

  Print(L"\n[Esc]:Home\n");
  
  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (InputKey.ScanCode == SCAN_ESC) {
      gST->ConOut->ClearScreen(gST->ConOut);
      break;
    }
  }

  return;
}

/**
  To show IRQ Routing Table device infomation.

  @param  *Buffer    The all IRQ routing table.
  
**/
VOID
GetDeviceInfo (
  UINT8  *Buffer
  )
{
  EFI_INPUT_KEY    InputKey;
  UINT64           Number;
  UINT64           i;

  i = 0;
  Number = ((Buffer[6] + 0x100 * Buffer[7]) - 0x20) / 0x10;

  while (TRUE) {
    gST->ConOut->ClearScreen(gST->ConOut);

    gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
    Print(L"%.2dth Slot IRQ Routing\n\n", i + 1);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
   
    Print(L"  PciBusNumber     : 0x%.2x\n", Buffer[0x20 + 0x10 * i]);
    Print(L"  PciDeviceNumber  : 0x%.2x\n", (Buffer[0x20 + 0x10 * i + 1]) >> 3);
    Print(L"  LinkValueforINTA : 0x%.2x\n", Buffer[0x20 + 0x10 * i + 2]);
    Print(L"  IrqBitmapforINTA : 0x%.2x%.2x\n", Buffer[0x20 + 0x10 * i + 4], Buffer[0x20 + 0x10 * i + 3]);
    Print(L"  LinkValueforINTB : 0x%.2x\n", Buffer[0x20 + 0x10 * i + 5]);
    Print(L"  IrqBitmapforINTB : 0x%.2x%.2x\n", Buffer[0x20 + 0x10 * i + 7], Buffer[0x20 + 0x10 * i + 6]);
    Print(L"  LinkValueforINTC : 0x%.2x\n", Buffer[0x20 + 0x10 * i + 8]);
    Print(L"  IrqBitmapforINTC : 0x%.2x%.2x\n", Buffer[0x20 + 0x10 * i + 10], Buffer[0x20 + 0x10 * i + 9]);
    Print(L"  LinkValueforINTD : 0x%.2x\n", Buffer[0x20 + 0x10 * i + 11]);
    Print(L"  IrqBitmapforINTD : 0x%.2x%.2x\n", Buffer[0x20 + 0x10 * i + 13], Buffer[0x20 + 0x10 * i + 12]);
    Print(L"  SlotNumber       : 0x%.2x\n", Buffer[0x20 + 0x10 * i + 14]);
    Print(L"  Reserved         : 0x%.2x\n", Buffer[0x20 + 0x10 * i + 15]);

    Print(L"\n[Arrow Key]:Choose   [ESC]:Escape\n");

    while (TRUE) {
      gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

      if (InputKey.ScanCode == SCAN_ESC) {
        gST->ConOut->ClearScreen(gST->ConOut);
        goto EXIT;

      } else if (InputKey.ScanCode == SCAN_RIGHT) {
        i++;
        if (i + 1 > Number) i = 0;
        break;

      } else if (InputKey.ScanCode == SCAN_LEFT) {
        i--;
        if (i + 1 < 1) i = (Number - 1);
        break;
      }      
    }
  }

  EXIT:;

  return;
}