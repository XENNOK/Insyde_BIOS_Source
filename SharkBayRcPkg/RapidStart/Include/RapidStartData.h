/** @file
  Header file to define structure and GUID of RapidStart Persistent Data

//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef RAPID_START_DATA_H_
#define RAPID_START_DATA_H_

#include <Guid/RapidStartPersistentData.h>

#ifndef MEM_EQU_4GB
#define MEM_EQU_4GB         0x100000000ULL
#endif
#define MEM_EQU_1MB         0x100000
#define LEGACY_SMRAM_BASE   0xA0000
#define LEGACY_SMRAM_SIZE   0x20000
#define LEGACY_SMRAM_BUFFER MEM_EQU_1MB

//
// RapidStart Flag definitions
//
#define RAPID_START_FLAG_ENTRY_DONE   BIT0
#define RAPID_START_FLAG_STORE_CHANGE BIT1

#define V_PCH_LAN_IO_LEN             32
#define R_PCH_LAN_IOADDR             0
#define R_PCH_LAN_IODATA             4
#define R_PCH_LAN_MDIC               0x20
#define B_PCH_LAN_MDIC_READY         BIT28
#define B_PCH_LAN_MDIC_ERROR         BIT30
#define V_PCH_LAN_MDIC_SET_PAGE      0x043F0000
#define V_PCH_LAN_MDIC_SET_ADDR      0x04310000
#define V_PCH_LAN_MDIC_WRITE         0x04320000
#define V_PCH_LAN_MDIC_READ          0x08320000
#define R_PCH_LAN_EXTCNF_CTRL        0x0F00
#define B_PCH_LAN_EXTCNF_CTRL_SWFLAG BIT5
#define R_PCH_LAN_FWSM_S             0x5B54
#define B_PCH_LAN_FWSM_S_FW_VAL      BIT15
#define R_PCH_LAN_FFVT               0x9800

#define V_PHY_VALID_SIG              0x8086
#define V_PHY_PAGE_WAKEUP            800
#define R_LAN_PHY_RCTL               0x00
#define R_LAN_PHY_IPAV               0x40
#define R_LAN_PHY_PRXC               0x46

#pragma pack(1)
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Date;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
} RTC_TIME;

typedef struct {
  UINT32  RapidStartMem;
  UINT32  MmioSpace;
  UINT32  RapidStartGlobalNvsPtr;
  UINT32  StoreSectors;
  UINT64  StoreLbaAddr;
  UINT8   StoreSataPort;
  UINT8   SataPortConfiguration;
  UINT32  Tolm;
  UINT64  Tohm;
  UINT64  TotalMem;
  UINT32  RapidStartMemSize;
  UINT32  ZeroBitmapSize;
  UINT32  WlanMmioSpace;
  UINT64  SystemMemoryLengthBelow4GB;
  UINT64  SystemMemoryLengthAbove4GB;
  UINT64  AcpiReservedMemoryBase;
  UINT32  AcpiReservedMemorySize;
  UINT32  Crc32RecordSize;
  UINT64  RapidStartStoreUid;
  UINT32  ActivePageThresholdSizeInSector;
  UINT8   HybridHardDisk;
  UINT8   DisplayType;
  UINT64  DssAddress;
  UINT64  DssSize;
} RAPID_START_PERSISTENT_DATA;

#define FFVT_SIZE 5

typedef struct {
  UINT32    GPE0;
  UINT32    GPE0a;
  UINT32    GPE0b;
  UINT32    Ffvt[FFVT_SIZE];
  RTC_TIME  OsWakeTime;
  UINT8     OsWakeTimeEnabled;
  UINT8     EntryCanceled;
} RAPID_START_MEM_DATA;

#pragma pack()

#define RAPID_START_ACPI_RTC_WAKE   0x01
#define RAPID_START_ACPI_BATT_WAKE  0x02

#define RAPID_START_MEM_DATA_PTR(RapidStartData) \
    ( \
      (RAPID_START_MEM_DATA *) (VOID *) \
        ( \
          ((RapidStartData)->RapidStartMem) + ((RapidStartData)->RapidStartMemSize) - sizeof (RAPID_START_MEM_DATA) - \
            ((RapidStartData)->ZeroBitmapSize) - ((RapidStartData)->Crc32RecordSize) \
        ) \
    )

#define RAPID_START_ZERO_PAGE_BITMAP_PTR(RapidStartData) \
    ( \
      (UINT32 *) (UINTN) \
        ( \
          ((RapidStartData)->RapidStartMem) + \
          ((RapidStartData)->RapidStartMemSize) - \
          ((RapidStartData)->ZeroBitmapSize) - \
          ((RapidStartData)->Crc32RecordSize) \
        ) \
    )

#define RAPID_START_CRC32_RECORD_PTR(RapidStartData) \
    ( \
      (UINT32 *) (UINTN) ( \
          ((RapidStartData)->RapidStartMem) + ((RapidStartData)->RapidStartMemSize) - ((RapidStartData)->Crc32RecordSize) \
        ) \
    )

#define NUMBER_OF_PAGES(MemAddr)          RShiftU64 (MemAddr, 12)
#define NUMBER_OF_PAGES_IN_DWORD(MemAddr) RShiftU64 (MemAddr, 17)
#define ZERO_BITMAP_UNIT                  32  /* (sizeof (UINT32) * 8bits) */
#endif
