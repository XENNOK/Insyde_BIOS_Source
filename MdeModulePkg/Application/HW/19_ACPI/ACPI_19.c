/** @file
  ACPI_19 C Source File

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

#include "ACPI_19.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE  ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
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

    switch (InputKey.UnicodeChar) {
    
    case '0':
      ShowMainPage();
      break;
    case '1':
      ShowAllSignature();
      break;
    case '2':
      ShowFadtData();
      break;
    }
  }  

  Status = EFI_SUCCESS;
  return Status;
}

VOID
ShowAllSignature(
  VOID
  ) 
{
  EFI_STATUS     Status;
  VOID           *Table;          // to store RSDP address (as pointer)

  UINT64         RsdpAddress;
  UINT64         XsdtAddress;
  UINT64         Address;         // to store child table address under XSDT
  UINT32         XsdtLength;
  UINT32         XsdtNumber;
  UINT32         i;               // for loop count

  gST->ConOut->ClearScreen(gST->ConOut);
  
  //
  // to get RSDP address (from UefiLib.h)
  //
  Status = EfiGetSystemConfigurationTable(
                              &gEfiAcpi20TableGuid,
                              &Table
                              );
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);
    return;
  
  } else {
    RsdpAddress = (UINT64)Table;
    XsdtAddress = *(UINT64*)(RsdpAddress + 24);

    XsdtLength = *(UINT32*)(XsdtAddress + 4);
    XsdtNumber = (XsdtLength - 36) / 8;
  }

  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"The table under XSDT:\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  for (i = 0; i < XsdtNumber; i++) {
    Address = *(UINT32*)(XsdtAddress + 36 + (8 * i));
    Print(L"%.2d. %c%c%c%c: 0x%.16x\n", (i + 1), 
                                       *(UINT8*)Address,
                                       *(UINT8*)(Address + 1),
                                       *(UINT8*)(Address + 2),
                                       *(UINT8*)(Address + 3),
                                       Address);
  }

  Print(L"\nPress '0' to back home.\n");

  return;
}

VOID
ShowFadtData(
  VOID
  ) 
{
  EFI_STATUS     Status;
  VOID           *Table;

  UINT64         RsdpAddress;
  UINT64         XsdtAddress;
  UINT64         FadtAddress;
  UINT32         XsdtLength;
  UINT32         XsdtNumber;
  UINT32         i; 
  UINT32         Buffer;

  gST->ConOut->ClearScreen(gST->ConOut);
  
  Status = EfiGetSystemConfigurationTable(
                                       &gEfiAcpi20TableGuid,
                                       &Table
                                       );
  if (EFI_ERROR(Status)) {
    Print(L"%r\n", Status);
    return;
  
  } else {
    RsdpAddress = (UINT64)Table;
    XsdtAddress = *(UINT64*)(RsdpAddress + 24);

    XsdtLength = *(UINT32*)(XsdtAddress + 4);
    XsdtNumber = (XsdtLength - 36) / 8;
  }

  for (i = 0; i < XsdtNumber; i++) {
    FadtAddress = *(UINT32*)(XsdtAddress + 36 + (8 * i));
    Buffer = *(UINT32*)FadtAddress;

    if ((UINT8)(Buffer >>  0) == 'F' &&
        (UINT8)(Buffer >>  8) == 'A' &&
        (UINT8)(Buffer >> 16) == 'C' &&
        (UINT8)(Buffer >> 24) == 'P') {
      
      gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print(L"Root System Description Pointer  : 0x%.16x\n"  , RsdpAddress);
      Print(L"Extended System Description Table: 0x%.16x\n"  , XsdtAddress);
      Print(L"Fixed ACPI Description Table     : 0x%.16x\n\n", FadtAddress);
      gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

      Print (L"Signature         : %c%c%c%c\n", *(UINT8*)FadtAddress, 
                                                *(UINT8*)(FadtAddress + 1), 
                                                *(UINT8*)(FadtAddress + 2), 
                                                *(UINT8*)(FadtAddress + 3));

      Print (L"Length of FADT    : %d\n", *(UINT32*)(FadtAddress + 4));
      Print (L"FADT Major Version: %d\n", *(UINT8*)(FadtAddress + 8));
      Print (L"Checksum          : 0x%.2x\n", *(UINT8*)(FadtAddress + 9));

      Print (L"OEMID             : %c%c%c%c%c%c\n", *(UINT8*)(FadtAddress + 10),
                                                    *(UINT8*)(FadtAddress + 11),
                                                    *(UINT8*)(FadtAddress + 12),
                                                    *(UINT8*)(FadtAddress + 13),
                                                    *(UINT8*)(FadtAddress + 14),
                                                    *(UINT8*)(FadtAddress + 15));

      Print (L"OEM Table ID      : %c%c%c%c%c%c%c%c\n", *(UINT8*)(FadtAddress + 16),
                                                        *(UINT8*)(FadtAddress + 17),
                                                        *(UINT8*)(FadtAddress + 18),
                                                        *(UINT8*)(FadtAddress + 19),
                                                        *(UINT8*)(FadtAddress + 20),
                                                        *(UINT8*)(FadtAddress + 21),
                                                        *(UINT8*)(FadtAddress + 22),
                                                        *(UINT8*)(FadtAddress + 23));

      Print (L"OEM Revision      : %d\n", *(UINT32*)(FadtAddress + 24));

      Print (L"Creator ID        : \"%c%c%c%c\"\n", *(UINT8*)(FadtAddress + 28),
                                                    *(UINT8*)(FadtAddress + 29),
                                                    *(UINT8*)(FadtAddress + 30),
                                                    *(UINT8*)(FadtAddress + 31));

      Print (L"Creator Revision  : %d\n",     *(UINT32*)(FadtAddress + 32));
      Print (L"Firmware Control  : 0x%.8x\n", *(UINT32*)(FadtAddress + 36));
      Print (L"DSDT Address      : 0x%.8x\n", *(UINT32*)(FadtAddress + 40));
      Print (L"Reserved          : 0x%.2x\n", *(UINT8 *)(FadtAddress + 44));
      Print (L"\nPress any key to continue...\n");
      Press2Continue();
      
      Print (L"PM Profile        : %d\n",     *(UINT8 *)(FadtAddress + 45));
      Print (L"SCI Interrupt     : 0x%.4x\n", *(UINT16*)(FadtAddress + 46));
      Print (L"SMI_CMD           : 0x%.8x\n", *(UINT32*)(FadtAddress + 48));
      Print (L"ACPI ENABLE       : 0x%.2x\n", *(UINT8 *)(FadtAddress + 52));
      Print (L"ACPI DISABLE      : 0x%.2x\n", *(UINT8 *)(FadtAddress + 53));
      Print (L"S4 REQ            : 0x%.2x\n", *(UINT8 *)(FadtAddress + 54));
      Print (L"PSTATE CNT        : 0x%.2x\n", *(UINT8 *)(FadtAddress + 55));
      Print (L"PM1a_EVT_BLK      : 0x%.8x\n", *(UINT32*)(FadtAddress + 56));
      Print (L"PM1b_EVT_BLK      : 0x%.8x\n", *(UINT32*)(FadtAddress + 60));
      Print (L"PM1a_CNT_BLK      : 0x%.8x\n", *(UINT32*)(FadtAddress + 64));
      Print (L"PM1b_CNT_BLK      : 0x%.8x\n", *(UINT32*)(FadtAddress + 68));
      Print (L"PM2_CNT_BLK       : 0x%.8x\n", *(UINT32*)(FadtAddress + 72));
      Print (L"PM_TMR_BLK        : 0x%.8x\n", *(UINT32*)(FadtAddress + 76));
      Print (L"\nPress any key to continue...\n");
      Press2Continue();

      Print (L"GPE0_BLK          : 0x%.8x\n", *(UINT32*)(FadtAddress + 80));
      Print (L"GPE1_BLK          : 0x%.8x\n", *(UINT32*)(FadtAddress + 84));
      Print (L"PM1_EVT_LEN       : 0x%.2x\n", *(UINT8 *)(FadtAddress + 88));
      Print (L"PM1_CNT_LEN       : 0x%.2x\n", *(UINT8 *)(FadtAddress + 89));
      Print (L"PM2_CNT_LEN       : 0x%.2x\n", *(UINT8 *)(FadtAddress + 90));
      Print (L"PM_TMR_LEN        : 0x%.2x\n", *(UINT8 *)(FadtAddress + 91));
      Print (L"GPE0_BLK_LEN      : 0x%.2x\n", *(UINT8 *)(FadtAddress + 92));
      Print (L"GPE1_BLK_LEN      : 0x%.2x\n", *(UINT8 *)(FadtAddress + 93));
      Print (L"GPE1_BASE         : 0x%.2x\n", *(UINT8 *)(FadtAddress + 94));
      Print (L"CST_CNT           : 0x%.2x\n", *(UINT8 *)(FadtAddress + 95));
      Print (L"P_LVL2_LAT        : 0x%.4x\n", *(UINT16*)(FadtAddress + 96));
      Print (L"P_LVL3_LAT        : 0x%.4x\n", *(UINT16*)(FadtAddress + 98));
      Print (L"FLUSH_SIZE        : 0x%.4x\n", *(UINT16*)(FadtAddress + 100));
      Print (L"FLUSH_STRIDE      : 0x%.4x\n", *(UINT16*)(FadtAddress + 102));
      Print (L"DUTY_OFFSET       : 0x%.2x\n", *(UINT8 *)(FadtAddress + 104));
      Print (L"DUTY_WIDTH        : 0x%.2x\n", *(UINT8 *)(FadtAddress + 105));
      Print (L"DAY_ALRM          : 0x%.2x\n", *(UINT8 *)(FadtAddress + 106));
      Print (L"MON_ALRM          : 0x%.2x\n", *(UINT8 *)(FadtAddress + 107));
      Print (L"CENTURY           : 0x%.2x\n", *(UINT8 *)(FadtAddress + 108));
      Print (L"IAPC_BOOT_ARCH    : 0x%.4x\n", *(UINT16*)(FadtAddress + 109));
      Print (L"Reserved          : 0x%.2x\n", *(UINT8 *)(FadtAddress + 111));
      Print (L"\nPress any key to continue...\n");
      Press2Continue();

      //
      // Flags
      //
      gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print(L"Flags:\n\n");
      gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
      Print (L"WBINVD                              : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  0) & 0x1);
      Print (L"WBINVD_FLUSH                        : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  1) & 0x1);
      Print (L"PROC_C1                             : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  2) & 0x1);
      Print (L"P_LVL2_UP                           : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  3) & 0x1);
      Print (L"PWR_BUTTON                          : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  4) & 0x1);
      Print (L"SLP_BUTTON                          : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  5) & 0x1);
      Print (L"FIX_RTC                             : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  6) & 0x1);
      Print (L"RTC_S4                              : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  7) & 0x1);
      Print (L"TMR_VAL_EXT                         : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  8) & 0x1);
      Print (L"DCK_CAP                             : %d\n", ((*(UINT32*)(FadtAddress + 112)) >>  9) & 0x1);
      Print (L"RESET_REG_SUP                       : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 10) & 0x1);
      Print (L"SEALED_CASE                         : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 11) & 0x1);
      Print (L"HEADLESS                            : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 12) & 0x1);
      Print (L"CPU_SW_SLP                          : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 13) & 0x1);
      Print (L"PCI_EXP_WAK                         : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 14) & 0x1);
      Print (L"USE_PLATFORM_CLOCK                  : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 15) & 0x1);
      Print (L"S4_RTC_STS_VALID                    : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 16) & 0x1);
      Print (L"REMOTE_POWER_ON_CAPABLE             : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 17) & 0x1);
      Print (L"FORCE_ APIC_CLUSTER_MODEL           : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 18) & 0x1);
      Print (L"FORCE_APIC_PHYSICAL_DESTINATION_MODE: %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 19) & 0x1);
      Print (L"HW_REDUCED_ACPI                     : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 20) & 0x1);
      Print (L"LOW_POWER_S0_IDLE_CAPABLE           : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 21) & 0x1);
      Print (L"Reserved                            : %d\n", ((*(UINT32*)(FadtAddress + 112)) >> 22) & 0x3ff);     
      Print (L"\nPress any key to continue...\n");
      Press2Continue();

      Print (L"RESET_REG         :");
      Print12Byte(FadtAddress + 116);
      Print (L"RESET_VALUE       : 0x%.2x\n" , *(UINT8 *)(FadtAddress + 128));
      Print (L"ARM_BOOT_ARCH     : 0x%.4x\n" , *(UINT16*)(FadtAddress + 129));
      Print (L"FADT Minor Version: 0x%.2x\n" , *(UINT8 *)(FadtAddress + 131));
      Print (L"X_FIRMWARE_CTRL   : 0x%.16x\n", *(UINT64*)(FadtAddress + 132));
      Print (L"X_DSDT            : 0x%.16x\n", *(UINT64*)(FadtAddress + 140));
      Print (L"X_PM1a_EVT_BLK    :");
      Print12Byte(FadtAddress + 148);
      Print (L"X_PM1b_EVT_BLK    :");
      Print12Byte(FadtAddress + 160);
      Print (L"X_PM1a_CNT_BLK    :");
      Print12Byte(FadtAddress + 172);
      Print (L"X_PM1b_CNT_BLK    :");
      Print12Byte(FadtAddress + 184);
      Print (L"X_PM2_CNT_BLK     :");
      Print12Byte(FadtAddress + 196);
      Print (L"X_PM_TMR_BLK      :");
      Print12Byte(FadtAddress + 208);
      Print (L"X_GPE0_BLK        :");
      Print12Byte(FadtAddress + 220);
      Print (L"X_GPE1_BLK        :");
      Print12Byte(FadtAddress + 232);
      Print (L"\nPress any key to continue...\n");
      Press2Continue();
      
      Print (L"SLEEP_CONTROL_REG :");
      Print12Byte(FadtAddress + 244);
      Print (L"SLEEP_STATUS_REG  :");
      Print12Byte(FadtAddress + 256);
      Print (L"Hypervisor Vendor Identity: %.16x\n", *(UINT64*)(FadtAddress + 268));  

      break;
    } 
  }

  Print(L"\nPress '0' to back home.\n");

  // //
  // // I/O write to CNT_PORT from FADT offset 64
  // // Write SLP_TYP bit 12:10 = 111, as S5 soft off
  // // Write SLP_EN  bit 13 = 1, active SLP_TYP setting
  // //
  // Print (L"\nCNT_PORT: %x\n", *(UINT32*)(FadtAddress + 64));
  // Print (L"Write Data, SLP_EN | SLP_TYP_S5: %x\n", SLP_EN | SLP_TYP_S5);
  // Print (L"Press any key to continue...\n");
  
  // JustStop();

  // IoWrite16(*(UINTN*)(FadtAddress + 64), SLP_EN | SLP_TYP_S5);

  return;
}

VOID
Print12Byte (
  UINT64 Address
  )
{
  UINT32 i;

  for (i = 0; i < 12; i++) {
    Print(L"%3.2x", *(UINT8*)(Address + i));
  }
  Print(L"\n");

  return;
}

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

VOID
SetMainPageAppearance (
  VOID
  )
{
  gST->ConOut->SetAttribute(gST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLACK);
  Print(L"19-ACPI\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"  1. Show signature list.\n");
  Print(L"  2. Show FADT table.\n");
  
  Print(L"\n[Opt]:Option   [Esc]:Escape\n");

  return;
}

VOID
Press2Continue (
  VOID  
  )
{
  EFI_INPUT_KEY  InputKey;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (InputKey.ScanCode != SCAN_NULL || InputKey.UnicodeChar != CHAR_NULL) {
      gST->ConOut->ClearScreen(gST->ConOut);
      break;
    }
  }
}