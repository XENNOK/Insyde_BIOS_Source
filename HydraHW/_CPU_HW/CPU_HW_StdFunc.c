/** @file

  CPU Standard Func
  
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
  Show Standard Function
  
  @param[in] SelectedItem   Function which is selected.
  
  @param[in] InputEx        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
 
  @return Status Code
*/
VOID
ShowStandardFunc (
  IN UINT8                                  SelectedItem,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_KEY_DATA HKey;
  UINT32       Eax;
  UINT32       Ebx;
  UINT32       Ecx;
  UINT32       Edx;
  CHAR8        DataString[48];
  CHAR16       *StandardFuncName[] = {
                 L"Vendor-ID and Largest Standard Function",
                 L"Feature Informaion",
                 L"Cache Descriptors",
                 L"Processor Serial Number",
                 L"Deterministic Cache Parameters",
                 L"MONITOR / MWAIT Parameters",
                 L"Digital Thermal Sensor and Power Management Parameters",
                 L"Structured Extended Feature Flags Enumeration",
                 L"Reserved",
                 L"Direct Cache Access(DCA) Parameters",
                 L"Architectural Performance Monitor Features",
                 L"x2APIC Features / Processor Topology",
                 L"Reserved",
                 L"XSAVE Features"
                 };
  CHAR16       *ProcessorType[] ={
                 L"Original OEM Processor",
                 L"OverDrive Processor",
                 L"Dual Processor",
                 L"Intel Reserved"
                 };
  CHAR16       *CacheType[] = {
                 L"Null,no more caches",
                 L"Data Cache",
                 L"Instruction Cache",
                 L"Unified Cache",
                 L"Reserved"
                 };
  UINT8        CacheCount;
  CacheCount = 0;
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
    StandardFuncName[SelectedItem]
    );
  SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  
  switch (SelectedItem) {
  case 0:
    *(UINT32 *)(DataString + 0) = Ebx;
    *(UINT32 *)(DataString + 4) = Edx;
    *(UINT32 *)(DataString + 8) = Ecx;
    DataString [12] = 0;

    Print (
      L"Largest Standard Function Number Suppoeted : %02X\n", 
      Eax
      );
    Print (
      L"Vendor ID:%a\r\n", 
      DataString
      );
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n", 
      Eax, 
      Ebx, 
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    break;
  case 1:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    while (TRUE) {
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
      Print (L"Press F1 to Show ECX Feature Flag\n");
      SetCursorPosColor (
        EFI_YELLOW| EFI_BACKGROUND_BLACK,
        HUGE_TABLE_HORI_MAX,
        (HUGE_TITLE_OFFSET + 5)
        );
      Print (L"Press F2 to Show EDX Feature Flag\n");
      SetCursorPosColor (
        EFI_WHITE | EFI_BACKGROUND_BLACK,
        0,
        (HUGE_TITLE_OFFSET + 3)
        );
      Print (
        L"Processor Type : %s\n", 
        ProcessorType [GetBit (Eax, 12, 13)]
        );
      Print (
        L"FamilyValue   : 0x%02x\n",
        GetBit (Eax, 20, 27) + GetBit (Eax, 8, 11)
        );
      Print (
        L"ModelValue    : 0x%02x\n",
        (GetBit (Eax, 16, 19)<<4) + GetBit (Eax, 4, 7)
        );
      Print (
        L"SteppingValue : 0x%02x\n", 
        GetBit (Eax, 0, 3)
        );
      Print (
        L"IntelR 64   : %s\n",
        GetBit (Edx, 29, 29) == 0?L"   UnSupposed":L"   Supposed"
        );
      Print (
        L"  RDTSCP    : %s\n", 
        GetBit (Edx, 27, 27) == 0?L"   UnSupposed":L"   Supposed"
        );
      Print (
        L"1 GB Pages  : %s\n",
        GetBit (Edx, 26, 26) == 0?L"   UnSupposed":L"   Supposed"
        );
      Print (
        L"  XD Bit    : %s\n",
        GetBit (Edx, 20, 20) == 0?L"   UnSupposed":L"   Supposed"
        );
      Print (
        L"  SYSCALL   : %s\n",
        GetBit (Edx, 11, 11) == 0?L"   UnSupposed":L"   Supposed"
        );
      HKey = keyRead (
             InputEx
             );
      if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
          !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
           (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
           (HKey.Key.ScanCode == SCAN_F1)) {
        ECXFeatureFlag (
          Ecx,
          InputEx
        );
      } else if (HKey.Key.ScanCode == SCAN_F2) {
        EDXFeatureFlag (
          Edx,
          InputEx
        );
      } else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
             	   ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
             	    (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
             	    (HKey.Key.ScanCode == SCAN_F1)) {
        break;
      }
    }
    break;
  case 2:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    if (Eax != 0) {
      SET_COLOR(EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY);
      Print (L"EAX:\n");
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      if (GetBit (Eax, 24, 31) != 0) {
      GetCacheData ( GetBit (Eax, 24, 31) );
      }
      if (GetBit (Eax, 16, 23) != 0) {
      GetCacheData ( GetBit (Eax, 16, 23) );
      }
      if ( GetBit (Eax, 8, 15) != 0) {
      GetCacheData ( GetBit (Eax, 8, 15) );
      }
      if ( GetBit (Eax, 0, 7) != 0) {
      GetCacheData ( GetBit (Eax, 0, 7) );
      }
    }

    if (Ebx != 0) {
      SET_COLOR(EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY);
      Print (L"EBX:\n");
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      if (GetBit (Ebx, 24, 31) != 0) {
      GetCacheData ( GetBit (Ebx, 24, 31) );
      }
      if (GetBit (Ebx, 16, 23) != 0) {
      GetCacheData ( GetBit (Ebx, 16, 23) );
      }
      if ( GetBit (Ebx, 8, 15) != 0) {
      GetCacheData ( GetBit (Ebx, 8, 15) );
      }
      if ( GetBit (Ebx, 0, 7) != 0) {
      GetCacheData ( GetBit (Ebx, 0, 7) );
      }
    }

    if (Ecx != 0) {
      SET_COLOR(EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY);
      Print (L"ECX:\n");
      SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
      if (GetBit (Ecx, 24, 31) != 0) {
      GetCacheData ( GetBit (Ecx, 24, 31) );
      }
      if (GetBit (Ecx, 16, 23) != 0) {
      GetCacheData ( GetBit (Ecx, 16, 23) );
      }
      if ( GetBit (Ecx, 8, 15) != 0) {
      GetCacheData ( GetBit (Ecx, 8, 15) );
      }
      if ( GetBit (Ecx, 0, 7) != 0) {
      GetCacheData ( GetBit (Ecx, 0, 7) );
      }
    }

    if (Edx != 0) {
      SET_COLOR(EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY);
      Print (L"EDX:\n");
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      if (GetBit (Edx, 24, 31) != 0) {
      GetCacheData ( GetBit (Edx, 24, 31) );
      }
      if (GetBit (Edx, 16, 23) != 0) {
      GetCacheData ( GetBit (Edx, 16, 23) );
      }
      if ( GetBit (Edx, 8, 15) != 0) {
      GetCacheData ( GetBit (Edx, 8, 15) );
      }
      if ( GetBit (Edx, 0, 7) != 0) {
      GetCacheData ( GetBit (Edx, 0, 7) );
      }
    }
    break;
  case 3:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (
      L"Processor Serial Number: %s",
      GetBit (Edx,18,18) == 0 ? L"   UnSupposed":L"   Supposed"
      );
    break;
  case 4:
    
    while (TRUE) {
      AsmCpuidEx (
        SelectedItem,
        CacheCount, 
        &Eax,
        &Ebx,
        &Ecx,
        &Edx
        );
      CleanLeftFrame (
        (HUGE_TITLE_OFFSET + 2),
        HUGE_TAIL_OFFSET,
        0,
        HUGE_TABLE_HORI_MAX
        );
      SetCursorPosColor (
        EFI_WHITE | EFI_BACKGROUND_BLACK,
        0,
        (HUGE_TITLE_OFFSET + 2)
        );
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      Print (
        L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
        Eax, 
        Ebx,
        Ecx,
        Edx
        );
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);   
      if (GetBit (Eax, 0, 4) == 0) {
        SET_COLOR(EFI_RED| EFI_BACKGROUND_BLACK);
        Print (L"No More Cache...\n");
        Print (L"Press Up back to previous Cache\n");
        Print (L"Or Press Other to Exit\n");
        SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
        
        HKey = keyRead (
             InputEx
             );
    
        if (HKey.Key.ScanCode == SCAN_UP ) {
          CacheCount--;
          continue;
    		} else {
          break;
        }
      }
      
      Print (L"The Cache %d Information:\n", CacheCount+1);
      
      Print (
        L"Number of Process: %02X\n",
        (GetBit (Eax, 26, 31) + 1)
        );
      Print (
        L"Maxinum number of threads sharing this cache : %02x\n",
        (GetBit (Eax, 14, 25) + 1)
        );
      
      if (GetBit (Eax, 8, 8) == 1) {
        Print (L"Self Initializing cache level \n");
      }
      
      Print (
        L"Cache Level : %d\n",
        GetBit (Eax, 5, 7)
        );
      
      if (GetBit (Eax, 0, 4) < 4) {
        Print(
          L"Cache Type : %s\n",
          CacheType[GetBit (Eax, 0, 4)]
          );
      } else {
        Print(L"Cache Type : Reserved\n");
      }
  
      Print (
        L"Ways of Associativity : %d\n",
        (GetBit (Ebx, 22, 31) + 1)
        );
      Print (
        L"Physical Line partitions  : %d\n",
        (GetBit (Ebx, 12, 21) + 1)
        );
      Print (
        L"System Coherency Line Size : %d\n",
        (GetBit (Ebx, 0, 11) + 1)
        );
      Print (
        L"Number of Sets : %d\n",
        (GetBit (Ecx, 0, 31) + 1)
        );
      SET_COLOR(EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print (L"Press Up / Down to Show Cache\n");
      Print (L"Press Ctrl + Left to Exit\n");
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      
      HKey = keyRead (
             InputEx
             );
    
      if ((HKey.Key.ScanCode != 0 ) &&
          !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
            (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
           
  		  switch (HKey.Key.ScanCode) {
  			case SCAN_UP:
          CacheCount--;
          if (CacheCount == 0xFF){
            CacheCount = 0;
          }
  			  break;
  			case SCAN_DOWN:
          CacheCount++;
  			  break;
  		  }
      } else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
             	   ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
             	    (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
             	    (HKey.Key.ScanCode == SCAN_LEFT)) {
        break;
        }
    }
    break;
  case 5:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (
      L"Smallest monitor line size: 0x%02x\n", 
      GetBit (Eax, 0, 15)
      );
    Print (
      L"Largest  monitor line size: 0x%02x\n", 
      GetBit (Ebx, 0, 15) 
      );
    Print (
      L"Support for treating interrupts: %c\n", 
      GetBit (Ecx, 0, 1) == 0?'F':'T' 
      );
    Print (
      L"MONITOR / MWAIT Extensions supported: %c\n", 
      GetBit (Ecx, 0, 0) == 0?'F':'T' 
      );
    Print (
      L"C4* sub-states supported using MONITOR/MWAIT:%d\n",
      GetBit (Edx, 16, 19)
      );
    Print (
      L"C3* sub-states supported using MONITOR/MWAIT:%d\n", 
      GetBit (Edx, 12, 15)
      );
    Print (
      L"C2* sub-states supported using MONITOR/MWAIT:%d\n", 
      GetBit (Edx, 8, 11)
      );
    Print (
      L"C1* sub-states supported using MONITOR/MWAIT:%d\n", 
      GetBit (Edx, 4, 7)
      );
    Print (
      L"C0* sub-states supported using MONITOR/MWAIT:%d\n", 
      GetBit (Edx, 0, 3) 
      );
    break;
  case 6:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (
      L"PTM   capability: %s\n",
      GetBit (Eax, 6, 6) == 0?L"No":L"YES" 
      );
    Print (
      L"ECMD  capability: %s\n",
      GetBit (Eax, 5, 5) == 0?L"No":L"YES" 
      );
    Print (
      L"PNL   capability: %s\n", 
      GetBit (Eax, 4, 4) == 0?L"No":L"YES" 
      );
    Print (
      L"ARAT  capability: %s\n",
      GetBit (Eax, 2, 2) == 0?L"No":L"YES" 
      );
    Print (
      L"Intel Turbo Boost Technology capability: %s\n",
      GetBit (Eax, 1, 1) == 0?L"No":L"YES" 
      );
    Print (
      L"DTS capability: %s\n",
      GetBit (Eax, 0, 0) == 0?L"No":L"YES"
      );
    Print (
      L"Number of Interrupt Thresholds: %d\n",
      GetBit (Ebx,0,3) 
      );
    Print (
      L"Performance-Energy Bias capability: %s\n", 
      GetBit (Ecx, 3, 3) == 0?L"No":L"YES" 
      );
    Print (
      L"ACNT2 capability: %s\n",
      GetBit (Ecx, 1, 1) == 0?L"Not Available":L"Available" 
      );
    Print (
      L"Hardware Coordination Feedback capability: %s\n",
      GetBit (Ecx, 0, 0) == 0?L"No":L"YES" 
      );
   
    break;
  case 7:
    AsmCpuidEx (
    SelectedItem,
    0,
    &Eax,
    &Ebx, 
    &Ecx,
    &Edx
    );
    
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    if ((Eax == 0) &&
        (Ebx == 0) &&
        (Ecx == 0) &&
        (Edx == 0)) {
      Print (L"No sub-leafs are supposed\n");
    } else {
      Print (
        L"Sub-leaf : 0x%x\n",
        Eax
        );
      if (GetBit (Ebx, 10, 10) == 1) {
        Print (L"INVPCID Supported\n");
      }
      if (GetBit (Ebx, 9, 9) == 1) {
        Print (L"Enhanced REP MOVSB/STOSB Supported\n");
      }
      if (GetBit (Ebx, 7, 7) == 1) {
        Print (L"SMEP ,Supports :\n");
        Print (L"Supervisor Mode Execution Protection\n");
      }
      if (GetBit (Ebx, 0, 0) == 1) {
        Print (L"FSGBASE ,Supports :\n");
        Print (L"RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE \n");
      }
  	}
    break;
  case 8:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (L"Reserved");
    break;
  case 9:
    AsmCpuid (
      1, 
      &Eax, 
      &Ebx,
      &Ecx, 
      &Edx
      );
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (L"Value of PLATFORM_DEA_CAP MSR:");
    Print ( GetBit (Edx, 18, 18) == 0 ? L"   UnSupposed":L"   Supposed:%x");
    break;
  case 10:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (
      L"Length of EBX bit:0x%02x\n", 
      GetBit (Eax, 24, 31)
      );
    Print (
      L"general-purpose bit width:0x%02x\n",
      GetBit (Eax, 16, 23)
      );
    Print (
      L"Number of general-purpose:0x%02x\n", 
      GetBit (Eax, 8, 15)
      );
    Print (
      L"Version ID of architecture :0x%02x\n", 
      GetBit (Eax, 0, 7) 
      );
    Print (
      L"Branch Mispredicts Retired: %s\n", 
      GetBit (Ebx, 6, 6) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Branch Instructions Retired: %s\n",
      GetBit (Ebx, 5, 5) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Last Level Cache Misses: %s\n", 
      GetBit (Ebx, 4, 4) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Last Level Cache References: %s\n",
      GetBit (Ebx, 3, 3) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Reference Cycles: %s\n", 
      GetBit (Ebx, 2, 2) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Instructions Retired: %s\n",
      GetBit (Ebx, 1, 1) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Core Cycles: %s\n",
      GetBit (Ebx, 0, 0) != 0 ? L"UnSupposed":L"Supposed"
      );
    Print (
      L"Number of Bits in the Fixed Counters:0x%x\n",
      GetBit(Edx, 5, 12) 
      );
    Print (
      L"Number of Fixed Counters:0x%x\n", 
      GetBit (Edx, 0, 4) 
      );
    break;
  case 11:
    while (TRUE) {
      AsmCpuidEx (
        SelectedItem,
        CacheCount, 
        &Eax,
        &Ebx,
        &Ecx,
        &Edx
        );
      
      CleanLeftFrame (
        (HUGE_TITLE_OFFSET + 2),
        HUGE_TAIL_OFFSET,
        0,
        HUGE_TABLE_HORI_MAX
        );
      
      SetCursorPosColor (
        EFI_WHITE | EFI_BACKGROUND_BLACK,
        0,
        (HUGE_TITLE_OFFSET + 2)
        );
      
      if ((Eax == 0) &&
          (Ebx == 0)) {
          
        SET_COLOR(EFI_RED| EFI_BACKGROUND_BLACK);
        Print (L"No More Levels...\n");
        Print (L"Press Up back to previous Cache\n");
        Print (L"Or Press Other to Exit\n");
        SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
        
        HKey = keyRead (
             InputEx
             );
    
        if (HKey.Key.ScanCode == SCAN_UP ) {
          CacheCount--;
          continue;
    		} else {
          break;
        }
      }
      
      if (CacheCount == 0) {
        Print (L"Thread Level Processor Topology\n");
      } else {
        Print (L"Core Level Processor Topology\n");
      }
      
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      Print (
        L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
        Eax, 
        Ebx,
        Ecx,
        Edx
        );
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);      
     
      Print (
      L"Number of Bits get next level APIC ID: %d\n",
      GetBit(Eax, 0, 4) 
      );
      
      Print (
      L"Factory-configured logical processors: %d\n",
      GetBit(Ebx, 0, 15) 
      );

      Print (L"Level Type :");
      if (GetBit(Ecx, 8, 15) == 0) {
        Print (L"Invalid\n");
      } else if (GetBit(Ecx, 8, 15) == 1) {
        Print (L"Thread\n");
      } else if (GetBit(Ecx, 8, 15) == 2) {
        Print (L"Core\n");
      } else {
        Print (L"Reserved\n");
      }

      Print (
      L"Level Number: %d\n",
      GetBit(Ecx, 0, 7) 
      );

      Print (
      L"Extended APIC ID : 0x%04X\n",
      GetBit(Edx, 0, 31) 
      );
      
      SET_COLOR(EFI_YELLOW | EFI_BACKGROUND_BLACK);
      Print (L"Press Up / Down to Show Cache\n");
      Print (L"Press Ctrl + Left to Exit\n");
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      
      HKey = keyRead (
             InputEx
             );
    
      if ((HKey.Key.ScanCode != 0 ) &&
          !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
            (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
           
  		  switch (HKey.Key.ScanCode) {
  			case SCAN_UP:
          CacheCount--;
          if (CacheCount == 0xFF){
            CacheCount = 0;
          }
  			  break;
  			case SCAN_DOWN:
          CacheCount++;
  			  break;
  		  }
      } else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
             	   ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
             	    (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
             	    (HKey.Key.ScanCode == SCAN_LEFT)) {
        break;
        }
    }
    break;
  case 12:
    SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
    Print (
      L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
      Eax, 
      Ebx,
      Ecx,
      Edx
      );
    SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print (L"Reserved\n");
    break;
  case 13:
    AsmCpuid (
      1,
      &Eax,
      &Ebx, 
      &Ecx, 
      &Edx
      );
    
    if (GetBit (Ecx,26,26) == 1) {
      AsmCpuidEx (
        SelectedItem,
        0,
        &Eax,
        &Ebx,
        &Ecx,
        &Edx
        );
      Print (L" XSAVE Features:\n");
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      Print (
        L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
        Eax, 
        Ebx,
        Ecx,
        Edx
        );
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    }else{
      AsmCpuidEx (
        SelectedItem,
        0,
        &Eax,
        &Ebx,
        &Ecx,
        &Edx
        );
      Print (L" XRSTOR Features:\n");
      SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
      Print (
        L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n",
        Eax, 
        Ebx,
        Ecx,
        Edx
        );
      SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    }
    break;
  }
  
}
