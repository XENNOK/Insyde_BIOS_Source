//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SdramMemorySetup.h

Abstract:

  Protocol used for generic SDRAM memory Setup. This protocol should be
  published by a driver that has knowledge of the spcific memory controller
  implementation.

--*/

#ifndef _EFI_SDRAM_MEMORY_SETUP_H_
#define _EFI_SDRAM_MEMORY_SETUP_H_

#include "Tiano.h"

EFI_FORWARD_DECLARATION (EFI_SDRAM_MEMORY_SETUP_PROTOCOL);

#define EFI_SDRAM_MEMORY_SETUP_PROTOCOL_GUID \
  { \
    0xa4ed3f1d, 0x96be, 0x4339, 0xba, 0x3c, 0xcb, 0xde, 0x12, 0x5a, 0x52, 0xb4 \
  }

//
// SDR and DDR timing data definitions.
// These are used to create bitmaps of valid timing configurations.
//
#define PEI_SDR_DDR_TIMING_DATA_FREQUENCY_UNKNOWN     0xFFFF
#define PEI_SDR_DDR_TIMING_DATA_REFRESH_RATE_UNKNOWN  0xFFFF

#define PEI_SDR_DDR_TIMING_DATA_TCL_10                0x01
#define PEI_SDR_DDR_TIMING_DATA_TCL_15                0x02
#define PEI_SDR_DDR_TIMING_DATA_TCL_20                0x04
#define PEI_SDR_DDR_TIMING_DATA_TCL_25                0x08
#define PEI_SDR_DDR_TIMING_DATA_TCL_30                0x10
#define PEI_SDR_DDR_TIMING_DATA_TCL_35                0x20
#define PEI_SDR_DDR_TIMING_DATA_TCL_40                0x40
#define PEI_SDR_DDR_TIMING_DATA_TCL_ALL               0x7F

#define PEI_SDR_DDR_TIMING_DATA_TCL_ALL               0x7F

#define PEI_SDR_DDR_TIMING_DATA_TRCD_02               0x02
#define PEI_SDR_DDR_TIMING_DATA_TRCD_03               0x04
#define PEI_SDR_DDR_TIMING_DATA_TRCD_ALL              0x06

#define PEI_SDR_DDR_TIMING_DATA_TRP_02                0x02
#define PEI_SDR_DDR_TIMING_DATA_TRP_03                0x04
#define PEI_SDR_DDR_TIMING_DATA_TRP_ALL               0x06

#define PEI_SDR_DDR_TIMING_DATA_TRRD_02               0x02
#define PEI_SDR_DDR_TIMING_DATA_TRRD_03               0x04
#define PEI_SDR_DDR_TIMING_DATA_TRRD_ALL              0x06

#define PEI_SDR_DDR_TIMING_DATA_TRAS_05               0x04
#define PEI_SDR_DDR_TIMING_DATA_TRAS_06               0x08
#define PEI_SDR_DDR_TIMING_DATA_TRAS_07               0x10
#define PEI_SDR_DDR_TIMING_DATA_TRAS_ALL              0x1C

#define PEI_SDR_DDR_TIMING_DATA_TRC_07                0x02
#define PEI_SDR_DDR_TIMING_DATA_TRC_08                0x04
#define PEI_SDR_DDR_TIMING_DATA_TRC_09                0x08
#define PEI_SDR_DDR_TIMING_DATA_TRC_10                0x10
#define PEI_SDR_DDR_TIMING_DATA_TRC_11                0x20
#define PEI_SDR_DDR_TIMING_DATA_TRC_12                0x40
#define PEI_SDR_DDR_TIMING_DATA_TRC_13                0x80
#define PEI_SDR_DDR_TIMING_DATA_TRC_ALL               0xFE

#define PEI_SDR_DDR_TIMING_DATA_TAC_UNKNOWN           0xFF

#define PEI_SDR_DDR_DATA_TYPE_REGISTERED              0x01
#define PEI_SDR_DDR_DATA_TYPE_UNREGISTERED            0x02
#define PEI_SDR_DDR_DATA_TYPE_BUFFERED                0x04
#define PEI_SDR_DDR_DATA_TYPE_UNBUFFERED              0x08
#define PEI_SDR_DDR_DATA_TYPE_SDR                     0x10
#define PEI_SDR_DDR_DATA_TYPE_DDR                     0x20

typedef struct {
  UINT16  Frequency;    // Frequency in Mhz for below timings
  UINT16  RefreshRate;  // Fastest refresh rate (nanoseconds)
  UINT8   Type;         // SDRAM type supported by this timing data (bit mask)
  UINT8   Tcl;          // Cas Latency(s) (bit mask)
  UINT8   Tac;          // Access from clock, (100ths of nanoseconds, BCD)
  UINT8   Trcd;         // Row to Col Delay (bit mask)
  UINT8   Trp;          // Ras Precharge (bit mask)
  UINT8   Trrd;         // Row to Row Delay, different bank (bit mask)
  UINT8   Tras;         // Ras Active Time (bit mask)
  UINT8   Trc;          // Row to Row Delay, same bank (bit mask) (tRP + tRAS)
} PEI_SDR_DDR_TIMING_DATA;

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SDRAM_SUPPORTED_TIMINGS) (
  IN EFI_SDRAM_MEMORY_SETUP_PROTOCOL          * This,
  IN OUT PEI_SDR_DDR_TIMING_DATA              **TimingData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SDRAM_CURRENT_TIMINGS) (
  IN EFI_SDRAM_MEMORY_SETUP_PROTOCOL          * This,
  IN OUT PEI_SDR_DDR_TIMING_DATA              * TimingData
  );

typedef struct _EFI_SDRAM_MEMORY_SETUP_PROTOCOL {
  EFI_GET_SDRAM_SUPPORTED_TIMINGS GetSupportedTimings;
  EFI_GET_SDRAM_CURRENT_TIMINGS   GetCurrentTimings;
} EFI_SDRAM_MEMORY_SETUP_PROTOCOL;

extern EFI_GUID gEfiSdramMemorySetupProtocolGuid;

#endif
