//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Cpu.h

Abstract:

  GVIII Ratio/Vid definitions. Now data for Dothan processor is included.

--*/

#ifndef _CPU_H_
#define _CPU_H_

#define EFI_MSR_IA32_FEATURE_CONTROL          0x3A
#define  SMRR_ENABLE                          0x00000008
#define EFI_MSR_IA32_MTRRCAP                  0xFE
#define  B_EFI_MSR_IA32_SMRR_SUPPORTED        0x800       

#define EFI_CPUID_DETERMINISTIC_CACHE_PARAMETERS  0x4
#define EFI_CPUID_MWAIT_INSTRUCTION_PARAMS        0x5
#define EFI_CPUID_EXTENDED_L2CACHE            0x80000006
#define EFI_CPUID_EXTENDED_ADDRESS_SIZES      0x80000008

#define EFI_CPUID_PENRYN                      0x00010670
#define EFI_CPUID_YONAH                       0x000006E0
#define EFI_CPUID_MEROM                       0x000006F0

#define EFI_MSR_IA32_PERF_STS         0x198
#define EFI_MSR_IA32_PERF_CTL         0x199
#define EFI_MSR_IA32_CLOCK_MODULATION 0x19A
#define EFI_MSR_IA32_THERM_STATUS     0x19C
#define EFI_MSR_GV_THERM              0x19D

#define B_BS_VID                      0x0000003F
#define N_BS_VID                      0
#define B_BS_RATIO                    0x00001F00
#define N_BS_RATIO                    8

//
// UINT64 workaround
//
// The MS compiler doesn't handle QWORDs very well.  I'm breaking
// them into DWORDs to circumvent the problems.  Converting back
// shouldn't be a big deal.
//
#pragma pack(1)
typedef union _MSR_REGISTER {
  UINT64  Qword;

  struct _DWORDS {
    UINT32  Low;
    UINT32  High;
  } Dwords;

  struct _BYTES {
    UINT8 FirstByte;
    UINT8 SecondByte;
    UINT8 ThirdByte;
    UINT8 FouthByte;
    UINT8 FifthByte;
    UINT8 SixthByte;
    UINT8 SeventhByte;
    UINT8 EighthByte;
  } Bytes;

} MSR_REGISTER;
#pragma pack()

//
// Synchronize to Cpu\Nehalem\ProcessorPowerManagement\070\Include\PpmCommon.h
//
typedef struct _FVID_HEADER {
  UINT32  Stepping;     // Matches value returned by CPUID function 1
  UINT16  MaxBusRatio;  // Matches BUS_RATIO_MAX field in PERF_STS_MSR
  UINT16  Gv3States;    // Number of states of FVID (N)
} FVID_HEADER;

typedef struct _FVID_STATE {
  UINT32  State;        // State Number (0 - N-1)
  UINT16  BusRatio;     // BUS_RATIO_SEL value to be written to PERF_CTL
  UINT32  Power;        // Typical power consumed by CPU in this state
} FVID_STATE;

typedef union _FVID_TABLE {
  FVID_HEADER FvidHeader;
  FVID_STATE  FvidState;
  UINT64      FvidData;
} FVID_TABLE;

#endif
