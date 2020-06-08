/** @file
  Tool C Source File

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

#include "tool.h"

/**
  To initialize WriteBuffer.

  @param  *WriteBuffer    The buffer of input.

**/
VOID 
InitializeArray(
  WRITE_BUFFER *WriteBuffer
  )
{
  UINT32  i;

  for (i = 0; i < InputAmount; i++) {
    WriteBuffer[i].value = 0;
    WriteBuffer[i].position.column = 0;
    WriteBuffer[i].position.row = 0;
  }
}

/**
  To calculate exponential function.

  @param  Base      Base number.
  @param  Index     Index number.

  @retval UINT64    The value of Base to the power of Index.

**/
UINT64 Power (
  UINT64 Base,
  UINT64 Index
  )
{  
  if (Index == 0) {
    return 1;
  } else {
    return (Base * Power(Base, Index-1));
  }
}

/**
  Input appointed number(0~f), and return combination in hexadecimal.

  @param  *WriteBuffer       The buffer of input.
  @param  *CursorPosition    For cursor position setting.

  @retval UINT32             The value of Base to the power of Index.

**/
UINT32 
InputValue (
  WRITE_BUFFER *WriteBuffer, 
  POSITION *CursorPosition
  )
{
  EFI_INPUT_KEY  InputKey;
  INT32          WriteCount;
  UINT32         Value, i;

  WriteCount = 0;
  Value = 0;

  while (TRUE) {
    gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);

    if (WriteCount >= InputAmount) {
      for (i = 0; i < (UINT32)WriteCount; i++) {
        Value += (UINT32)(WriteBuffer[i].value * Power((UINT64)0x10, (UINT64)(WriteCount-i-1))); 
      }

      return Value;
    }

    switch(InputKey.UnicodeChar){
    case CHAR_NULL:
      continue;
    
    case CHAR_CARRIAGE_RETURN:
      for (i = 0; i < (UINT32)WriteCount; i++) {
        Value += (UINT32)(WriteBuffer[i].value * Power((UINT64)0x10, (UINT64)(WriteCount-i-1)));            
      }

      return Value;
    
    case CHAR_BACKSPACE:
      WriteCount--;
      if (WriteCount < 0) {
        WriteCount = 0;
        continue;
      }
      WriteBuffer[WriteCount].value = CHAR_NULL;
      Print(L"%c", InputKey.UnicodeChar); 
      
      continue;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 48;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;
      continue;
    
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
      WriteBuffer[WriteCount].value = InputKey.UnicodeChar - 87;
      Print(L"%c", InputKey.UnicodeChar);
      WriteCount++;
      continue;
    }
  }
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
  Print(L"11-CPU (CPUID & MSR)\n\n");
  gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  Print(L"  0. CPUID Function 0\n");
  Print(L"  1. CPUID Function 1\n");
  Print(L"  2. CPUID Function 2\n");
  Print(L"  3. CPUID Function 3\n");
  Print(L"  4. CPUID Function 4\n");
  Print(L"  5. CPUID Function 5\n");
  Print(L"  6. CPUID Function 6\n");
  Print(L"  7. CPUID Function 7\n");
  Print(L"  8. CPUID Function 8\n");
  Print(L"  9. CPUID Function 9\n");
  Print(L"  A. CPUID Function A\n");
  Print(L"  B. CPUID Function B\n");
  Print(L"  C. CPUID Function C\n");
  Print(L"  D. CPUID Function D\n");
  Print(L"  E. CPUID Extended Function 0x80000000\n");
  Print(L"  F. CPUID Extended Function 0x80000001\n");
  Print(L"  G. CPUID Extended Function 0x80000002\n");
  Print(L"  H. CPUID Extended Function 0x80000003\n");
  Print(L"  I. CPUID Extended Function 0x80000004\n");
  Print(L"  J. CPUID Extended Function 0x80000005\n");
  Print(L"  K. CPUID Extended Function 0x80000006\n");
  Print(L"  L. CPUID Extended Function 0x80000007\n");
  Print(L"  M. CPUID Extended Function 0x80000008\n");
  Print(L"  N. MSR Read\n");
  Print(L"  O. MSR Write\n");
  Print(L"  P. Show Whole Brand String\n");

  Print(L"\n[Opt]:Option   [Esc]:Escape\n");
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
}

/**
  To set branch page appearance.

**/
VOID
SetBranchPageSupplementAppearance (
  VOID
  )
{
  Print(L"\n[F1]:Home   [Opt]:Option   [Esc]:Escape\n");
}