/** @file

  CPU Standard Get Data Func
  
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
  Get bits 
  
  @param[in] Value   which value you wants to get.
  
  @param[in] Min     Min bits.

  @param[in] Max     Max bits.
 
  @return UINT32 value
*/
UINT32
GetBit (
  IN UINT32 Value,
  IN UINTN  Min,
  IN UINTN  Max
  )
{
  return (Value << (31 - Max)) >> (31 - (Max - Min));
}


/**
  Get Cache Data for Standard Function2
  
  @param[in] Value   which value you wants to get.
 
*/
void
GetCacheData (
  UINT32 Value
)
{
   switch (Value) {
   case 0x01: 
     Print (L"instruction TLB: 4K pages, 4-way, 32 entries    \n");
     break;
   case 0x02: 
     Print (L"instruction TLB: 4M pages, 4-way, 2 entries     \n");
     break;
   case 0x03: 
     Print (L"data TLB: 4K pages, 4-way, 64 entries           \n");
     break;
   case 0x04: 
     Print (L"data TLB: 4M pages, 4-way, 8 entries            \n");
     break;
   case 0x05:
     Print (L"data TLB: 4M pages, 4-way, 32 entries           \n");
     break;
   case 0x06: 
     Print (L"L1 instruction cache: 8K, 4-way, 32 byte lines  \n");
     break;
   case 0x08: 
     Print (L"L1 instruction cache: 16K, 4-way, 32 byte lines \n");
     break;
   case 0x09: 
     Print (L"L1 instruction cache: 32K, 4-way, 64-byte lines \n");
     break;
   case 0x0a:  
     Print (L"L1 data cache: 8K, 2-way, 32 byte lines         \n");
     break;
   case 0x0b: 
     Print (L"instruction TLB: 4M pages, 4-way, 4 entries     \n");
     break;
   case 0x0c: 
     Print (L"L1 data cache: 16K, 4-way, 32 byte lines        \n");
     break;
   case 0x0d: 
     Print (L"L1 data cache: 16K, 4-way, 64-byte lines        \n");
     break;
   case 0x0e: 
     Print (L"L1 data cache: 24K, 6-way, 64 byte lines        \n");
     break;
   case 0x10: 
     Print (L"L1 data cache: 16K, 4-way, 32 byte lines        \n");
     break;
   case 0x15: 
     Print (L"L1 instruction cache: 16K, 4-way, 32 byte lines \n");
     break;
   case 0x1a: 
     Print (L"L2 cache: 96K, 6-way, 64 byte lines             \n");
     break;
   case 0x21: 
     Print (L"L2 cache: 256K MLC, 8-way, 64 byte lines        \n");
     break;
   case 0x22: 
     Print (L"L3 cache: 512K, 4-way, 64 byte lines            \n");
     break;
   case 0x23: 
     Print (L"L3 cache: 1M, 8-way, 64 byte lines              \n");
     break;
   case 0x25: 
     Print (L"L3 cache: 2M, 8-way, 64 byte lines              \n");
     break;
   case 0x29: 
     Print (L"L3 cache: 4M, 8-way, 64 byte lines              \n");
     break;
   case 0x2c: 
     Print (L"L1 data cache: 32K, 8-way, 64 byte lines        \n");
     break;
   case 0x30: 
     Print (L"L1 cache: 32K, 8-way, 64 byte lines             \n");
     break;
   case 0x39: 
     Print (L"L2 cache: 128K, 4-way, sectored, 64 byte lines  \n");
     break;
   case 0x3a: 
     Print (L"L2 cache: 192K, 6-way, sectored, 64 byte lines  \n");
     break;
   case 0x3b: 
     Print (L"L2 cache: 128K, 2-way, sectored, 64 byte lines  \n");
     break;
   case 0x3c: 
     Print (L"L2 cache: 256K, 4-way, sectored, 64 byte lines  \n");
     break;
   case 0x3d: 
     Print (L"L2 cache: 384K, 6-way, sectored, 64 byte lines  \n");
     break;
   case 0x3e: 
     Print (L"L2 cache: 512K, 4-way, sectored, 64 byte lines  \n");
     break;
   case 0x40: 
     Print (L"No 2nd-level cache or, if processor contain a   ");
     Print (L"valid 2nd-level cache, no 3rd-level cache       \n");
     break;
   case 0x41: 
     Print (L"L2 cache: 128K, 4-way, 32 byte lines            \n");
     break;
   case 0x42: 
     Print (L"L2 cache: 256K, 4-way, 32 byte lines            \n");
     break;
   case 0x43: 
     Print (L"L2 cache: 512K, 4-way, 32 byte lines            \n");
     break;
   case 0x44: 
     Print (L"L2 cache: 1M, 4-way, 32 byte lines              \n");
     break;
   case 0x45: 
     Print (L"L2 cache: 2M, 4-way, 32 byte lines              \n");
     break;
   case 0x46: 
     Print (L"L3 cache: 4M, 4-way, 64 byte lines              \n");
     break;
   case 0x47: 
     Print (L"L3 cache: 8M, 8-way, 64 byte lines              \n");
     break;
   case 0x48: 
     Print (L"L2 cache: 3M, 12-way, 64 byte lines             \n");
     break;
   case 0x49: 
     Print (L"L3 cache: 4M, 16-way, 64 byte lines             \n");
     break;
   case 0x4a: 
     Print (L"L3 cache: 6M, 12-way, 64 byte lines             \n");
     break;
   case 0x4b: 
     Print (L"L3 cache: 8M, 16-way, 64 byte lines             \n");
     break;
   case 0x4c: 
     Print (L"L3 cache: 12M, 12-way, 64 byte lines            \n");
     break;
   case 0x4d: 
     Print (L"L3 cache: 16M, 16-way, 64 byte lines            \n");
     break;
   case 0x4e: 
     Print (L"L2 cache: 6M, 24-way, 64 byte lines             \n");
     break;
   case 0x4f: 
     Print (L"instruction TLB: 4K pages, 32 entries           \n");
     break;
   case 0x50: 
     Print (L"instruction TLB: 4K & 2M/4M pages, 64 entries   \n");
     break;
   case 0x51: 
     Print (L"instruction TLB: 4K & 2M/4M pages, 128 entries  \n");
     break;
   case 0x52: 
     Print (L"instruction TLB: 4K & 2M/4M pages, 256 entries  \n");
     break;
   case 0x55: 
     Print (L"instruction TLB: 2M/4M pages, fully, 7 entries  \n");
     break;
   case 0x56: 
     Print (L"L1 data TLB: 4M pages, 4-way, 16 entries        \n");
     break;
   case 0x57: 
     Print (L"L1 data TLB: 4K pages, 4-way, 16 entries        \n");
     break;
   case 0x59: 
     Print (L"data TLB: 4K pages, 16 entries                  \n");
     break;
   case 0x5a: 
     Print (L"data TLB: 2M/4M pages, 4-way, 32 entries        \n");
     break;
   case 0x5b: 
     Print (L"data TLB: 4K & 4M pages, 64 entries             \n");
     break;
   case 0x5c: 
     Print (L"data TLB: 4K & 4M pages, 128 entries            \n");
     break;
   case 0x5d: 
     Print (L"data TLB: 4K & 4M pages, 256 entries            \n");
     break;
   case 0x60: 
     Print (L"L1 data cache: 16K, 8-way, 64 byte lines        \n");
     break;
   case 0x61: 
     Print (L"instruction TLB: 4K pages, 48 entries           \n");
     break;
   case 0x63: 
     Print (L"data TLB: 1G pages, 4-way, 4 entries            \n");
     break;
   case 0x66: 
     Print (L"L1 data cache: 8K, 4-way, 64 byte lines         \n");
     break;
   case 0x67: 
     Print (L"L1 data cache: 16K, 4-way, 64 byte lines        \n");
     break;
   case 0x68: 
     Print (L"L1 data cache: 32K, 4-way, 64 byte lines        \n");
     break;
   case 0x70: 
     Print (L"Trace cache: 12K-uop, 8-way                     \n");
     break;
   case 0x71: 
     Print (L"Trace cache: 16K-uop, 8-way                     \n");
     break;
   case 0x72: 
     Print (L"Trace cache: 32K-uop, 8-way                     \n");
     break;
   case 0x73: 
     Print (L"Trace cache: 64K-uop, 8-way                     \n");
     break;
   case 0x76: 
     Print (L"instruction TLB: 2M/4M pages, fully, 8 entries  \n");
     break;
   case 0x77: 
     Print (L"L1 instruction cache: 16K, 4-way, sectored,   ");
     Print (L"64 byte lines                                   \n");
     break;
   case 0x78: 
     Print (L"L2 cache: 1M, 4-way, 64 byte lines              \n");
     break;
   case 0x79: 
     Print (L"L2 cache: 128K, 8-way, sectored, 64 byte lines  \n");
     break;
   case 0x7a: 
     Print (L"L2 cache: 256K, 8-way, sectored, 64 byte lines  \n");
     break;
   case 0x7b: 
     Print (L"L2 cache: 512K, 8-way, sectored, 64 byte lines  \n");
     break;
   case 0x7c: 
     Print (L"L2 cache: 1M, 8-way, sectored, 64 byte lines    \n");
     break;
   case 0x7d: 
     Print (L"L2 cache: 2M, 8-way, sectored, 64 byte lines    \n");
     break;
   case 0x7e: 
     Print (L"L2 cache: 256K, 8-way, sectored, 128 byte lines \n");
     break;
   case 0x7f: 
     Print (L"L2 cache: 512K, 2-way, 64 byte lines            \n");
     break;
   case 0x80: 
     Print (L"L2 cache: 512K, 8-way, 64 byte lines            \n");
     break;
   case 0x81: 
     Print (L"L2 cache: 128K, 8-way, 32 byte lines            \n");
     break;
   case 0x82: 
     Print (L"L2 cache: 256K, 8-way, 32 byte lines            \n");
     break;
   case 0x83: 
     Print (L"L2 cache: 512K, 8-way, 32 byte lines            \n");
     break;
   case 0x84: 
     Print (L"L2 cache: 1M, 8-way, 32 byte lines              \n");
     break;
   case 0x85: 
     Print (L"L2 cache: 2M, 8-way, 32 byte lines              \n");
     break;
   case 0x86: 
     Print (L"L2 cache: 512K, 4-way, 64 byte lines            \n");
     break;
   case 0x87: 
     Print (L"L2 cache: 1M, 8-way, 64 byte lines              \n");
     break;
   case 0x88: 
     Print (L"L3 cache: 2M, 4-way, 64 byte lines              \n");
     break;
   case 0x89: 
     Print (L"L3 cache: 4M, 4-way, 64 byte lines              \n");
     break;
   case 0x8a: 
     Print (L"L3 cache: 8M, 4-way, 64 byte lines              \n");
     break;
   case 0x8d: 
     Print (L"L3 cache: 3M, 12-way, 128 byte lines            \n");
     break;
   case 0x90: 
     Print (L"instruction TLB: 4K-256M, fully, 64 entries     \n");
     break;
   case 0x96: 
     Print (L"instruction TLB: 4K-256M, fully, 32 entries     \n");
     break;
   case 0x9b: 
     Print (L"instruction TLB: 4K-256M, fully, 96 entries     \n");
     break;
   case 0xb0: 
     Print (L"instruction TLB: 4K, 4-way, 128 entries         \n");
     break;
   case 0xb1: 
     Print (L"instruction TLB: 2M/4M, 4-way, 4/8 entries      \n");
     break;
   case 0xb2: 
     Print (L"instruction TLB: 4K, 4-way, 64 entries          \n");
     break;
   case 0xb3: 
     Print (L"data TLB: 4K, 4-way, 128 entries                \n");
     break;
   case 0xb4: 
     Print (L"data TLB: 4K, 4-way, 256 entries                \n");
     break;
   case 0xb5:  
     Print (L"instruction TLB: 4K, 8-way, 64 entries          \n");
     break;
   case 0xb6:
     Print (L"instruction TLB: 4K, 8-way, 128 entries         \n");
     break;
   case 0xba: 
     Print (L"data TLB: 4K, 4-way, 64 entries                 \n");
     break;
   case 0xc0: 
     Print (L"data TLB: 4K & 4M pages, 4-way, 8 entries       \n");
     break;
   case 0xc1: 
     Print (L"L2 TLB: 4K/2M pages, 8-way, 1024 entries        \n");
     break;
   case 0xc2: 
     Print (L"DTLB: 2M/4M pages, 4-way, 16 entries            \n");
     break;
   case 0xca: 
     Print (L"L2 TLB: 4K, 4-way, 512 entries                  \n");
     break;
   case 0xd0: 
     Print (L"L3 cache: 512K, 4-way, 64 byte lines            \n");
     break;
   case 0xd1: 
     Print (L"L3 cache: 1M, 4-way, 64 byte lines              \n");
     break;
   case 0xd2: 
     Print (L"L3 cache: 2M, 4-way, 64 byte lines              \n");
     break;
   case 0xd6: 
     Print (L"L3 cache: 1M, 8-way, 64 byte lines              \n");
     break;
   case 0xd7:
     Print (L"L3 cache: 2M, 8-way, 64 byte lines              \n");
     break;
   case 0xd8: 
     Print (L"L3 cache: 4M, 8-way, 64 byte lines              \n");
     break;
   case 0xdc:
     Print (L"L3 cache: 1.5M, 12-way, 64 byte lines           \n");
     break;
   case 0xdd: 
     Print (L"L3 cache: 3M, 12-way, 64 byte lines             \n");
     break;
   case 0xde: 
     Print (L"L3 cache: 6M, 12-way, 64 byte lines             \n");
     break;
   case 0xe2: 
     Print (L"L3 cache: 2M, 16-way, 64 byte lines             \n");
     break;
   case 0xe3: 
     Print (L"L3 cache: 4M, 16-way, 64 byte lines             \n");
     break;
   case 0xe4: 
     Print (L"L3 cache: 8M, 16-way, 64 byte lines             \n");
     break;
   case 0xea: 
     Print (L"L3 cache: 12M, 24-way, 64 byte lines            \n");
     break;
   case 0xeb: 
     Print (L"L3 cache: 18M, 24-way, 64 byte lines            \n");
     break;
   case 0xec: 
     Print (L"L3 cache: 24M, 24-way, 64 byte lines            \n");
     break;
   case 0xf0: 
     Print (L"64 byte prefetching                             \n");
     break;
   case 0xf1: 
     Print (L"128 byte prefetching                            \n");
     break;
   case 0xff: 
     Print (L"cache data is in CPUID 4                        \n");
     break;
   default:   
     Print (L"unknown                                         \n");
     break;
   }
}

/**
  Show ECX Feature Flag Data for Standard Function1
  
  @param[in] Ecx      ECX Data.
  @param[in] InputEx  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  

*/
VOID
ECXFeatureFlag (
  IN UINT32                                 Ecx,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
)
{
  EFI_KEY_DATA HKey;
  CHAR16  *FeatureFlagData[] = {
            L"SSE3",
            L"PCMULDQ",
            L"DTES64 ",
            L"MONITOR",
            L"DS-CPL",
            L"VMX",
            L"SMX",
            L"EIST",
            L"TM2",
            L"SSSE3",
            L"CNXT-ID",
            L"Reserved",
            L"FMA",
            L"CX16",
            L"xTPR",
            L"PDCM",
            L"Reserved",
            L"PCID",
            L"DCA",
            L"SSE4.1",
            L"SSE4.2",
            L"x2APIC",
            L"MOVBE",
            L"POPCNT",
            L"TSC-DEADLINE",
            L"AES",
            L"XSAVE",
            L"OSXSAVE",
            L"AVX",
            L"F16C",
            L"RDRAND",
            L"NotUsed"
            };
  UINT8   FlagIndexY;
  UINT8   FlagIndexX;
  UINT8   BitNow;
  
  BitNow = 0;
  
  CleanLeftFrame (
    (HUGE_TITLE_OFFSET + 3),
    HUGE_TAIL_OFFSET,
    0,
    HUGE_FRAME_HORI_MAX
    );
  
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 3)
    );
  Print (L"Press Ctrl + F1 to Back");
  
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 3)
    );
  
  for (FlagIndexY = 0; FlagIndexY < 16 ; FlagIndexY ++) {
    for (FlagIndexX = 0; FlagIndexX < 2 ; FlagIndexX++) {
      BitNow = (FlagIndexY * 2) + FlagIndexX;
        
      SET_CUR_POS (
        (FlagIndexX * 24),
        ((HUGE_TITLE_OFFSET + 3) + FlagIndexY)
        );
      if ((BitNow == 11) ||
          (BitNow == 16)){
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print (
            L"%12s: ",
            FeatureFlagData[BitNow]
            );
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"Reserved");
      } else if (BitNow == 31) {
        SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print (
          L"%12s: ",
          FeatureFlagData[BitNow]
          );
        SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
        Print (L"Not Used");
      } else {
        SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print (
          L"%12s: ",
          FeatureFlagData[BitNow]
          );
        SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
        Print (
          L"%8s",
          GetBit (Ecx, BitNow, BitNow) != 0?L"TURE":L"FALSE"
          );
      }
    }
  }
  while (TRUE) {
    HKey = keyRead (
             InputEx
             );
    if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
             	   ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
             	    (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
             	    (HKey.Key.ScanCode == SCAN_F1)) {
      CleanLeftFrame (
        (HUGE_TITLE_OFFSET + 3),
        HUGE_TAIL_OFFSET,
        HUGE_TABLE_HORI_MAX,
        HUGE_FRAME_HORI_MAX
        );
      break;
      }
  }
}

/**
  Show EDX Feature Flag Data for Standard Function1
  
  @param[in] Edx      EDX Data.
  @param[in] InputEx  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  

*/
VOID
EDXFeatureFlag (
  IN UINT32                                 Edx,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
)
{
  EFI_KEY_DATA HKey;
  CHAR16  *FeatureFlagData[] = {
            L"FPU",
            L"VME",
            L"DE",
            L"PSE",
            L"TSC",
            L"MSR",
            L"PAE",
            L"MCE",
            L"CX8",
            L"APIC",
            L"Reserved",
            L"SEP",
            L"MTRR",
            L"PGE",
            L"MCA",
            L"CMOV",
            L"PAT",
            L"PSE-36",
            L"PSN",
            L"CLFSH",
            L"Reserved",
            L"DS",
            L"ACPI",
            L"MMX",
            L"FXSR",
            L"SSE",
            L"SSE2",
            L"SS",
            L"HTT",
            L"TM",
            L"Reserved",
            L"PBE"
            };
  UINT8   FlagIndexY;
  UINT8   FlagIndexX;
  UINT8   BitNow;
  
  BitNow = 0;
  
  CleanLeftFrame (
    (HUGE_TITLE_OFFSET + 3),
    HUGE_TAIL_OFFSET,
    0,
    HUGE_FRAME_HORI_MAX
    );
  
  SetCursorPosColor (
    EFI_YELLOW| EFI_BACKGROUND_BLACK,
    HUGE_TABLE_HORI_MAX,
    (HUGE_TITLE_OFFSET + 3)
    );
  Print (L"Press Ctrl + F1 to Back");
  
  SetCursorPosColor (
    EFI_WHITE | EFI_BACKGROUND_BLACK,
    0,
    (HUGE_TITLE_OFFSET + 3)
    );
  
  for (FlagIndexY = 0; FlagIndexY < 16 ; FlagIndexY ++) {
    for (FlagIndexX = 0; FlagIndexX < 2 ; FlagIndexX++) {
      BitNow = (FlagIndexY * 2) + FlagIndexX;
        
      SET_CUR_POS (
        (FlagIndexX * 24),
        ((HUGE_TITLE_OFFSET + 3) + FlagIndexY)
        );
      
      if ((BitNow == 10) ||
          (BitNow == 20) ||
          (BitNow == 30)){
          SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
          Print (
            L"%12s: ",
            FeatureFlagData[BitNow]
            );
          SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
          Print (L"Reserved");
      } else {
        SET_COLOR(EFI_WHITE | EFI_BACKGROUND_BLACK);
        Print (
          L"%12s: ",
          FeatureFlagData[BitNow]
          );
        SET_COLOR(EFI_LIGHTGRAY| EFI_BACKGROUND_BLACK);
        Print (
          L"%8s",
          GetBit (Edx, BitNow, BitNow) != 0?L"TURE":L"FALSE"
          );
      }
    }
  }
  while (TRUE) {
    HKey = keyRead (
             InputEx
             );
    
    if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
             	   ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
             	    (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
             	    (HKey.Key.ScanCode == SCAN_F1)) {
      CleanLeftFrame (
        (HUGE_TITLE_OFFSET + 3),
        HUGE_TAIL_OFFSET,
        HUGE_TABLE_HORI_MAX,
        HUGE_FRAME_HORI_MAX
        );
      break;
      }
  }
}