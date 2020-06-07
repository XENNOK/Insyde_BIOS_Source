/** @file

  CPU Extended Func
  
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

#include"CPU_HW.h"

/**
  Show Extended Function
  
  @param[in] SelectedItem   Function which is selected.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
 
  @return Status Code
*/
VOID
ShowExtendedFunc (
  IN UINT8                                  SelectedItem,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_KEY_DATA HKey;
  UINT32       Eax;
  UINT32       Ebx;
  UINT32       Ecx;
  UINT32       Edx;
  CHAR8        DataString[64];
  CHAR16       *ExtendedFuncName[] = {
                 L"Largest Extended Function",
                 L"Extended Feature Bits",
                 L"Processor Brand String",
                 L"Processor Brand String",
                 L"Processor Brand String",
                 L"Reserved",
                 L"Extended L2 Cache Features",
                 L"Advanced Power Management",
                 L"Virtual Physical Address Sizes"
                 };
                 
  Eax = Ebx = Ecx = Edx  = 0;
  AsmCpuid ((UINT32)SelectedItem, &Eax, &Ebx, &Ecx, &Edx);

  CleanLeftFrame (
    HUGE_TITLE_OFFSET,
    HUGE_TAIL_OFFSET,
    0,
    HUGE_TABLE_HORI_MAX
    );
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    0,
    HUGE_TITLE_OFFSET
    );
  
  Print(
    L"Func %02X\n",
    SelectedItem
    );
  SET_COLOR(EFI_BLUE| EFI_BACKGROUND_BLACK);
  Print (
    L"%s\n",
    ExtendedFuncName[SelectedItem]
    );
  SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);

  switch (SelectedItem) {
  case 0:
    Print (L"Largest extended function number:");
    Print (L"    0x%x ",Eax);
    break;
  case 1:
    Print ("LAHF        : %s\n", GetBit (Ecx, 0, 0) == 0?L"   UnSupposed":L"   Supposed");
    Print (L"IntelR 64   : %s\n", GetBit (Edx, 29, 29) == 0?L"   UnSupposed":L"   Supposed");
    Print (L"  RDTSCP    : %s\n", GetBit (Edx, 27, 27) == 0?L"   UnSupposed":L"   Supposed");
    Print (L"1 GB Pages  : %s\n", GetBit (Edx, 26, 26) == 0?L"   UnSupposed":L"   Supposed");
    Print (L"  XD Bit    : %s\n", GetBit (Edx, 20, 20) == 0?L"   UnSupposed":L"   Supposed");
    Print (L"  SYSCALL   : %s\n", GetBit (Edx, 11, 11) == 0?L"   UnSupposed":L"   Supposed");
    break;
  case 2:
  case 3:
  case 4:
    AsmCpuid (0x80000002, &Eax, &Ebx, &Ecx, &Edx);
    *(UINT32 *)(DataString + 0)  = Eax;
    *(UINT32 *)(DataString + 4)  = Ebx;
    *(UINT32 *)(DataString + 8)  = Ecx;
    *(UINT32 *)(DataString + 12) = Edx;
    AsmCpuid (0x80000003, &Eax, &Ebx, &Ecx, &Edx);
    *(UINT32 *)(DataString + 16) = Eax;
    *(UINT32 *)(DataString + 20) = Ebx;
    *(UINT32 *)(DataString + 24) = Ecx;
    *(UINT32 *)(DataString + 28) = Edx;
    AsmCpuid (0x80000004, &Eax, &Ebx, &Ecx, &Edx);
    *(UINT32 *)(DataString + 32) = Eax;
    *(UINT32 *)(DataString + 36) = Ebx;
    *(UINT32 *)(DataString + 40) = Ecx;
    *(UINT32 *)(DataString + 44) = Edx;
    DataString[48] = 0;
    Print (L"Processor Name :\n%a\r\n", DataString);
    break;
  case 5:
    Print (L"Reserved");
    break;
  case 6:
    Print (L"L2 Cache size : 0x%02x\n", GetBit (Eax, 16, 31));
    Print (L"L2 Cache Associativity : %s\n",
    GetBit (Eax, 12, 15) == 0x00 ? L"Disabled" : 
    GetBit (Eax, 12, 15) == 0x01 ? L"Direct mapped" :
    GetBit (Eax, 12, 15) == 0x02 ? L"2-Way" :
    GetBit (Eax, 12, 15) == 0x04 ? L"4-Way" :
    GetBit (Eax, 12, 15) == 0x06 ? L"8-Way" :
    GetBit (Eax, 12, 15) == 0x08 ? L"16-Way" : L"Fully associative" );
    Print (L"L2 Cache Line Size : 0x%x", GetBit (Ecx, 0, 7));
    break;
  case 7:
    Print (L"TSC Invariance : %s\n", GetBit (Edx, 8, 8) != 0 ? L"UnAvailable":L"Available");
    break;
  case 8:
    Print (L"Virtual Address Size : 0x%04x\n", GetBit (Eax, 8, 15));
    Print (L"Physical Address Size: 0x%04x\n", GetBit (Eax, 0, 7));
    break;
  }
}
