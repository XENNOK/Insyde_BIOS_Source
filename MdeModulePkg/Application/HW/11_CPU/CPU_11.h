/** @file
  CPU_11 H Source File

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

#ifndef _CPU_11_H_
#define _CPU_11_H_

#include "tool.h"

typedef struct {
  UINT8  Value;
  CHAR8* Type;
  CHAR8* Description;
} DESCRIPTOR;

VOID
Function_0 (
  VOID
  ); 

VOID
Function_1 (
  VOID
  );

VOID
Function_2 (
  VOID
  );

VOID
Function_3 (
  VOID
  );

VOID
Function_4 (
  VOID
  );

VOID
Function_5 (
  VOID
  );

VOID
Function_6 (
  VOID
  );

VOID
Function_7 (
  VOID
  );

VOID
Function_8 (
  VOID
  );

VOID
Function_9 (
  VOID
  );

VOID
Function_A (
  VOID
  );

VOID
Function_B (
  VOID
  );

VOID
Function_C (
  VOID
  );

VOID
Function_D (
  VOID
  );

VOID
Function_E0 (
  VOID
  );

VOID
Function_E1 (
  VOID
  );

VOID
Function_E2 (
  VOID
  );

VOID
Function_E3 (
  VOID
  );

VOID
Function_E4 (
  VOID
  );

VOID
Function_E5 (
  VOID
  );

VOID
Function_E6 (
  VOID
  );

VOID
Function_E7 (
  VOID
  );

VOID
Function_E8 (
  VOID
  );

VOID
MsrRead(
  VOID
  );

VOID
MsrWrite(
  VOID
  );

DESCRIPTOR
GetDescription(
  UINT8 IndexValue
  );

VOID
ShowFullBrandString (
  VOID
  );

CONST DESCRIPTOR Descriptors[] = {
  {0x00, "General",  "Null Descriptor"},
  {0x01, "TLB",      "Instruction TLB: 4-KB Pages, 4-way set associative, 32 entries"},
  {0x02, "TLB",      "Instruction TLB: 4-MB Pages, fully associative, 2 entries"},
  {0x03, "TLB",      "Data TLB: 4-KB Pages, 4-way set associative, 64 entries"},
  {0x04, "TLB",      "Data TLB: 4-MB Pages, 4-way set associative, 8 entries"},
  {0x05, "TLB",      "Data TLB: 4-MB Pages, 4-way set associative, 32 entries"},
  {0x06, "Cache",    "1st-level instruction cache: 8-KB, 4-way set associative, 32-byte line size"},
  {0x08, "Cache",    "1st-level instruction cache: 16-KB, 4-way set associative, 32-byte line size"},
  {0x09, "Cache",    "1st-level Instruction Cache: 32-KB, 4-way set associative, 64-byte line size"},
  {0x0A, "Cache",    "1st-level data cache: 8-KB, 2-way set associative, 32-byte line size"},
  {0x0B, "TLB",      "Instruction TLB: 4-MB pages, 4-way set associative, 4 entries"},
  {0x0C, "Cache",    "1st-level data cache: 16-KB, 4-way set associative, 32-byte line size"},
  {0x0D, "Cache",    "1st-level Data Cache: 16-KB, 4-way set associative, 64-byte line size"},
  {0x0E, "Cache",    "1st-level Data Cache: 24-KB, 6-way set associative, 64-byte line size"},
  {0x21, "Cache",    "2nd-level cache: 256-KB, 8-way set associative, 64-byte line size"},
  {0x22, "Cache",    "3rd-level cache: 512-KB, 4-way set associative, sectored cache, 64-byte line size"},
  {0x23, "Cache",    "3rd-level cache: 1-MB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x25, "Cache",    "3rd-level cache: 2-MB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x29, "Cache",    "3rd-level cache: 4-MB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x2C, "Cache",    "1st-level data cache: 32-KB, 8-way set associative, 64-byte line size"},
  {0x30, "Cache",    "1st-level instruction cache: 32-KB, 8-way set associative, 64-byte line size"},
  {0x40, "Cache",    "No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache"},
  {0x41, "Cache",    "2nd-level cache: 128-KB, 4-way set associative, 32-byte line size"},
  {0x42, "Cache",    "2nd-level cache: 256-KB, 4-way set associative, 32-byte line size"},
  {0x43, "Cache",    "2nd-level cache: 512-KB, 4-way set associative, 32-byte line size"},
  {0x44, "Cache",    "2nd-level cache: 1-MB, 4-way set associative, 32-byte line size"},
  {0x45, "Cache",    "2nd-level cache: 2-MB, 4-way set associative, 32-byte line size"},
  {0x46, "Cache",    "3rd-level cache: 4-MB, 4-way set associative, 64-byte line size"},
  {0x47, "Cache",    "3rd-level cache: 8-MB, 8-way set associative, 64-byte line size"},
  {0x48, "Cache",    "2nd-level cache: 3-MB, 12-way set associative, 64-byte line size, unified on-die"},
  {0x49, "Cache",    "3rd-level cache: 4-MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP,Family 0Fh, Model 06h)\n2nd-level cache: 4-MB, 16-way set associative, 64-byte line size"},
  {0x4A, "Cache",    "3rd-level cache: 6-MB, 12-way set associative, 64-byte line size"},
  {0x4B, "Cache",    "3rd-level cache: 8-MB, 16-way set associative, 64-byte line size"},
  {0x4C, "Cache",    "3rd-level cache: 12-MB, 12-way set associative, 64-byte line size"},
  {0x4D, "Cache",    "3rd-level cache: 16-MB, 16-way set associative, 64-byte line size"},
  {0x4E, "Cache",    "2nd-level cache: 6-MB, 24-way set associative, 64-byte line size"},
  {0x4F, "TLB",      "Instruction TLB: 4-KB pages, 32 entries"},
  {0x50, "TLB",      "Instruction TLB: 4-KB, 2-MB or 4-MB pages, fully associative, 64 entries"},
  {0x51, "TLB",      "Instruction TLB: 4-KB, 2-MB or 4-MB pages, fully associative, 128 entries"},
  {0x52, "TLB",      "Instruction TLB: 4-KB, 2-MB or 4-MB pages, fully associative, 256 entries"},
  {0x55, "TLB",      "Instruction TLB: 2-MB or 4-MB pages, fully associative, 7 entries"},
  {0x56, "TLB",      "L1 Data TLB: 4-MB pages, 4-way set associative, 16 entries"},
  {0x57, "TLB",      "L1 Data TLB: 4-KB pages, 4-way set associative, 16 entries"},
  {0x59, "TLB",      "Data TLB0: 4-KB pages, fully associative, 16 entries"},
  {0x5A, "TLB",      "Data TLB0: 2-MB or 4-MB pages, 4-way associative, 32 entries"},
  {0x5B, "TLB",      "Data TLB: 4-KB or 4-MB pages, fully associative, 64 entries"},
  {0x5C, "TLB",      "Data TLB: 4-KB or 4-MB pages, fully associative, 128 entries"},
  {0x5D, "TLB",      "Data TLB: 4-KB or 4-MB pages, fully associative, 256 entries"},
  {0x60, "Cache",    "1st-level data cache: 16-KB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x66, "Cache",    "1st-level data cache: 8-KB, 4-way set associative, sectored cache, 64-byte line size"},
  {0x67, "Cache",    "1st-level data cache: 16-KB, 4-way set associative, sectored cache, 64-byte line size"},
  {0x68, "Cache",    "1st-level data cache: 32-KB, 4 way set associative, sectored cache, 64-byte line size"},
  {0x70, "Cache",    "Trace cache: 12K-uops, 8-way set associative"},
  {0x71, "Cache",    "Trace cache: 16K-uops, 8-way set associative"},
  {0x72, "Cache",    "Trace cache: 32K-uops, 8-way set associative"},
  {0x76, "TLB",      "Instruction TLB: 2M/4M pages, fully associative, 8 entries"},
  {0x78, "Cache",    "2nd-level cache: 1-MB, 4-way set associative, 64-byte line size"},
  {0x79, "Cache",    "2nd-level cache: 128-KB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x7A, "Cache",    "2nd-level cache: 256-KB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x7B, "Cache",    "2nd-level cache: 512-KB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x7C, "Cache",    "2nd-level cache: 1-MB, 8-way set associative, sectored cache, 64-byte line size"},
  {0x7D, "Cache",    "2nd-level cache: 2-MB, 8-way set associative, 64-byte line size"},
  {0x7F, "Cache",    "2nd-level cache: 512-KB, 2-way set associative, 64-byte line size"},
  {0x80, "Cache",    "2nd-level cache: 512-KB, 8-way set associative, 64-byte line size"},
  {0x82, "Cache",    "2nd-level cache: 256-KB, 8-way set associative, 32-byte line size"},
  {0x83, "Cache",    "2nd-level cache: 512-KB, 8-way set associative, 32-byte line size"},
  {0x84, "Cache",    "2nd-level cache: 1-MB, 8-way set associative, 32-byte line size"},
  {0x85, "Cache",    "2nd-level cache: 2-MB, 8-way set associative, 32-byte line size"},
  {0x86, "Cache",    "2nd-level cache: 512-KB, 4-way set associative, 64-byte line size"},
  {0x87, "Cache",    "2nd-level cache: 1-MB, 8-way set associative, 64-byte line size"},
  {0xB0, "TLB",      "Instruction TLB: 4-KB Pages, 4-way set associative, 128 entries"},
  {0xB1, "TLB",      "Instruction TLB: 2-MB pages, 4-way, 8 entries or 4M pages, 4-way, 4 entries"},
  {0xB2, "TLB",      "Instruction TLB: 4-KB pages, 4-way set associative, 64 entries"},
  {0xB3, "TLB",      "Data TLB: 4-KB Pages, 4-way set associative, 128 entries"},
  {0xB4, "TLB",      "Data TLB: 4-KB Pages, 4-way set associative, 256 entries"},
  {0xBA, "TLB",      "Data TLB: 4-KB Pages, 4-way set associative, 64 entries"},
  {0xC0, "TLB",      "Data TLB: 4-KB or 4-MB Pages, 4-way set associative, 8 entries"},
  {0xCA, "STLB",     "Shared 2nd-level TLB: 4 KB pages, 4-way set associative, 512 entries"},
  {0xD0, "Cache",    "3rd-level cache: 512-kB, 4-way set associative, 64-byte line size"},
  {0xD1, "Cache",    "3rd-level cache: 1-MB, 4-way set associative, 64-byte line size"},
  {0xD2, "Cache",    "3rd-level cache: 2-MB, 4-way set associative, 64-byte line size"},
  {0xD6, "Cache",    "3rd-level cache: 1-MB, 8-way set associative, 64-byte line size"},
  {0xD7, "Cache",    "3rd-level cache: 2-MB, 8-way set associative, 64-byte line size"},
  {0xD8, "Cache",    "3rd-level cache: 4-MB, 8-way set associative, 64-byte line size"},
  {0xDC, "Cache",    "3rd-level cache: 1.5-MB, 12-way set associative, 64-byte line size"},
  {0xDD, "Cache",    "3rd-level cache: 3-MB, 12-way set associative, 64-byte line size"},
  {0xDE, "Cache",    "3rd-level cache: 6-MB, 12-way set associative, 64-byte line size"},
  {0xE2, "Cache",    "3rd-level cache: 2-MB, 16-way set associative, 64-byte line size"},
  {0xE3, "Cache",    "3rd-level cache: 4-MB, 16-way set associative, 64-byte line size"},
  {0xE4, "Cache",    "3rd-level cache: 8-MB, 16-way set associative, 64-byte line size"},
  {0xEA, "Cache",    "3rd-level cache: 12-MB, 24-way set associative, 64-byte line size"},
  {0xEB, "Cache",    "3rd-level cache: 18-MB, 24-way set associative, 64-byte line size"},
  {0xEC, "Cache",    "3rd-level cache: 24-MB, 24-way set associative, 64-byte line size"},
  {0xF0, "Prefetch", "64-byte Prefetching"},
  {0xF1, "Prefetch", "128-byte Prefetching"},
  {0xFF, "General",  "CPUID Leaf 2 does not report cache descriptor information; use CPUID Leaf 4 to query cache parameters"}
};

#endif