/** @file

  This file contains the definitions necessary for the instantiation
  of the voltage specific XTU BIOS Tables

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _EFI_SPTT_VOLTAGE_H
#define _EFI_SPTT_VOLTAGE_H

#include <PchRegsLpc.h>
#include <ChipsetSetupConfig.h>
#include <PlatformBaseAddrDef.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/PerfTune.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/XtuPolicy/XtuPolicy.h>

// UQI Strings
#define StrCpuVoltagePromptUqi                  0x04CA  // STR_STATIC_VID_VALUE_PROMPT
#define StrDynamicCpuVoltageOffsetPromptUqi     0x049C  // STR_DYNAMIC_CPU_VID_OFFSET_PROMPT
#define StrFsbVoltagePromptUqi                  0x012C  // STR_VTT_BURN_IN_MODE_PROMPT
#define StrQpiVoltOvrPromptUqi                  0x0446  // STR_QPI_VOLTAGE_OVERRIDE_PROMPT
#define StrIohVoltagePromptUqi                  0x0447  // STR_IOH_VOLTAGE_OVERRIDE_PROMPT
#define StrMemVoltagePromptUqi                  0x042A  // STR_BEARLAKE_MEMORY_VOLTAGE_PROMPT

#define CPU_VOLTAGE_OFFSET_VALUE                300     // 300 mV to be added to the voltage display
                                                        // The voltage display has a precision of 4 which means
                                                        // we can add 300 directly to the voltage value unless this
                                                        // precision gets changed.
#define CPU_VOLTAGE_PRECISION                   4       // This is the precision of the value coming back from GetDefaultValue

//
// Voltage Type data structure
//
typedef enum {
  CPU_VOLTAGE       = 0,
  FSB_VOLTAGE,
  IOH_VOLTAGE,
  MEMORY_VOLTAGE
} VOLTAGE_TYPE;


#define NUM_CPU_VOLT_VAL_ENTRIES            3
#define EFI_MEM_SMBUS_ADDRESS                26
#define CLKCHIP_SMBUS_ADDR                  (0xD2 >> 1)
#define CPU_VOLTAGE_SMBUS_ADDRESS           (0x42 >> 1)

#ifndef BIT0
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000
#define BIT32 0x100000000
#define BIT33 0x200000000
#define BIT34 0x400000000
#define BIT35 0x800000000
#define BIT36 0x1000000000
#define BIT37 0x2000000000
#define BIT38 0x4000000000
#define BIT39 0x8000000000
#define BIT40 0x10000000000
#define BIT41 0x20000000000
#define BIT42 0x40000000000
#define BIT43 0x80000000000
#define BIT44 0x100000000000
#define BIT45 0x200000000000
#define BIT46 0x400000000000
#define BIT47 0x800000000000
#define BIT48 0x1000000000000
#define BIT49 0x2000000000000
#define BIT50 0x4000000000000
#define BIT51 0x8000000000000
#define BIT52 0x10000000000000
#define BIT53 0x20000000000000
#define BIT54 0x40000000000000
#define BIT55 0x80000000000000
#define BIT56 0x100000000000000
#define BIT57 0x200000000000000
#define BIT58 0x400000000000000
#define BIT59 0x800000000000000
#define BIT60 0x1000000000000000
#define BIT61 0x2000000000000000
#define BIT62 0x4000000000000000
#define BIT63 0x8000000000000000
#endif

EFI_STATUS
GetRealtimeSettingData (
  OUT UINT8                       **DataBuffer,
  OUT UINTN                       *DataLen
  );

#endif
