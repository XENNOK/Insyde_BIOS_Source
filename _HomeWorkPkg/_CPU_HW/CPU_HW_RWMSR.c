/** @file

  A dedicated function to Read/Write MSR

******************************************************************************
* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/
#include "CPU_HW.h"

/**
  Read MSR Data.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
*/
VOID
ReadMsr (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_KEY_DATA  HKey;
  UINT32        Eax;
  UINT32        Ebx;
  UINT32        Ecx;
  UINT32        Edx;
  UINT32        ReadAddress;
  UINT64        CPUAddress;

  CPUAddress = 0;
  ReadAddress = 0;
  
  CleanLeftFrame (
    (HUGE_TITLE_OFFSET + 3),
    HUGE_TAIL_OFFSET,
    0,
    HUGE_TABLE_HORI_MAX
    );
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 3)
    );
  Print (L"Press Ctrl + F1 to Back\n");
  
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 4)
    );
  Print (L"Press Enter to Confirm\n");

  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 5)
    );
  Print (L"Press Backspace to Backspace\n");
  
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
  Print(L"Please input Address : 0x");
  
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    25,
    HUGE_TITLE_OFFSET
    );

  Print (L"________\n");
  ReadAddress = DataWrite (
                  InputEx,
                  HUGE_TITLE_OFFSET,
                  25,
                  4,
                  ReadAddress
                  );
  if (ReadAddress == NO_INPUT) {
    return;
  }
  CPUAddress = AsmReadMsr64 (ReadAddress);
  Eax = (UINT32)CPUAddress;
  Edx = (UINT32)(CPUAddress >> 32);
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 1)
    );
  Print (L"EAX:%08x | EDX:%08x\n", Eax, Edx);
  
}

/**
  Read MSR data and write new data.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.

*/
VOID
WriteMSR (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
)
{
  EFI_KEY_DATA  HKey;
  UINT32        Eax;
  UINT32        Ebx;
  UINT32        Ecx;
  UINT32        Edx;
  UINT32        ReadAddress;
  UINT64        CPUAddress;
  UINT64        NewCPUAddress;

  CPUAddress = 0;
  NewCPUAddress = 0;

  CleanLeftFrame (
    (HUGE_TITLE_OFFSET + 3),
    HUGE_TAIL_OFFSET,
    0,
    HUGE_TABLE_HORI_MAX
    );
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 3)
    );
  Print (L"Press Ctrl + F1 to Back\n");
  
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 4)
    );
  Print (L"Press Enter to Confirm\n");

  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 5)
    );
  Print (L"Press Backspace to Backspace\n");
  
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

  //
  // Read Orignal MSR Value
  //
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    0,
    HUGE_TITLE_OFFSET
    );
  Print(L"Please input Address : 0x");
  
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    25,
    HUGE_TITLE_OFFSET
    );

  Print (L"________\n");
  ReadAddress = DataWrite (
                  InputEx,
                  HUGE_TITLE_OFFSET,
                  25,
                  4,
                  ReadAddress
                  );
  if (ReadAddress == NO_INPUT) {
    return;
  }
  CPUAddress = AsmReadMsr64 (ReadAddress);
  Eax = (UINT32)CPUAddress;
  Edx = (UINT32)(CPUAddress >> 32);
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 1)
    );
  Print (L"EAX:%08x | EDX:%08x\n", Eax, Edx);

  //
  // Input EAX Value
  //
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 2)
    );
  Print (L"Please input new EAX Value : 0x");
  
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    31,
    (HUGE_TITLE_OFFSET + 2)
    );

  Print (L"________\n");
  Eax = DataWrite (
          InputEx,
          (HUGE_TITLE_OFFSET + 2),
          31,
          4,
          Eax
          );
  if (Eax == NO_INPUT) {
    return;
  }
  //
  // Input EDX Value
  //
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 3)
    );
  Print (L"Please input new EDX Value : 0x");
  
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    31,
    (HUGE_TITLE_OFFSET + 3)
    );

  Print (L"________\n");
  Edx = DataWrite (
          InputEx,
          (HUGE_TITLE_OFFSET + 3),
          31,
          4,
          Edx
          );
  if (Edx == NO_INPUT) {
    return;
  }
  //
  // Write New Value to MSR
  //
  NewCPUAddress = ((UINT64)Edx << 32) + (UINT64)Eax;
  AsmWriteMsr64 (ReadAddress, NewCPUAddress);

  //
  // Read New EAX & EDX
  //
  CPUAddress = AsmReadMsr64 (ReadAddress);
  Eax = (UINT32)CPUAddress;
  Edx = (UINT32)(CPUAddress >> 32);
  SetCursorPosColor (
    EFI_DARKGRAY | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 4)
    );
  Print (L"EAX:%08x | EDX:%08x\n", Eax, Edx);
  
}
