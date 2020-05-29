/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to  
  the additional terms of the license agreement               
**/
/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  Cpu.h

@brief:

  Various CPU-specific definitions.

**/
#ifndef _CPU_H_
#define _CPU_H_
//[-start-121212-IB10820191-remove]//
//#define B_FAMILY_MODEL_STEPPING       0x00000FFF

//#define EFI_MSR_IA32_PERF_STS         0x198
//#define EFI_MSR_IA32_PERF_CTL         0x199
//#define EFI_MSR_IA32_CLOCK_MODULATION 0x19A
//#define EFI_MSR_IA32_THERM_STATUS     0x19C
//#define EFI_MSR_GV_THERM              0x19D

//#define B_BS_VID                      0x0000003F
//#define N_BS_VID                      0
//#define B_BS_RATIO                    0x00001F00
//#define N_BS_RATIO                    8
//[-end-121212-IB10820191-remove]//
//[-start-121212-IB10820191-add]//
//
// Add CPU's define on here if RC code don't define it
//
#define EFI_CACHE_VARIABLE_MTRR_END               0x213
#define B_MSR_MISC_EN_FERR_MULTPLEX_EN            0x400
#define N_EFI_MSR_TURBO_POWER_LIMIT_2_LIMIT       32
#define N_EFI_MSR_TURBO_POWER_LIMIT_1_LIMIT       0
#define EFI_PLATFORM_INFORMATION                  0x000000CE
#define EFI_IA32_CLOCK_FLEX_MAX                   0x00000194

//[-end-121212-IB10820191-add]//
///
/// UINT64 workaround
///
/// The MS compiler doesn't handle QWORDs very well.  I'm breaking
/// them into DWORDs to circumvent the problems.  Converting back
/// shouldn't be a big deal.
///
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
#endif
