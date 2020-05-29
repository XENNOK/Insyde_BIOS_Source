//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   CmosLib.h
//;
//; Abstract:
//;
//;   CMOS library include file

#ifndef _CMOS_LIB_H_
#define _CMOS_LIB_H_

#include "Tiano.h"
#include "ChipsetCmos.h"
#include "OemCmos.h"

#define R_CMOS_INDEX                 0x70
#define R_CMOS_DATA                  0x71
#define bit9                         (1 << 9)
#define INTERRUPT_FLAG               bit9

UINT8
EfiReadCmos8 (
  IN UINT8                                 Address
  );

VOID
EfiWriteCmos8 (
  IN UINT8                                 Address,
  IN UINT8                                 Data
  );

UINT16
EfiReadCmos16 (
  IN UINT8                                 Address
  );

VOID
EfiWriteCmos16 (
  IN UINT8                                 Address,
  IN UINT16                                 Data
  );

UINT32
EfiReadCmos32 (
  IN UINT8                                 Address
  );

VOID
EfiWriteCmos32 (
  IN UINT8                                 Address,
  IN UINT32                                 Data
  );

UINT16
EfiSumaryCmos (
  VOID
  );

EFI_STATUS
EfiValidateCmosChecksum (
  VOID
  );

UINT8
EfiReadExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  );

UINT16
EfiReadExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  );

UINT32
EfiReadExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  );

VOID
EfiWriteExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT8                                 Data
  );

VOID
EfiWriteExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT16                                 Data
  );

VOID
EfiWriteExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT32                                 Data
  );

typedef enum {
                                    //0x00~0x0F: Real Time Clock Reserve
                                    //0x10~0x2D: IBM PC Standard Reserve
  CmosCheckSum2E          = 0x2E,   //0x2E~0x2F: CMOS Check Sum
                                    //0x30~0x32: IBM PC Standard Reserve
  SmartBootWatchdogFlag   = 0x33,   //0x33     : SmartBoot Watchdog flag
  CmosTimeZone            = 0x34,   //0x34~0x35: Time zone
  CmosDaylight            = 0x36,   //0x36     : Daylight
  CmosYearWakeUp          = 0x37,   //0x37     : Wake up year
  CurrentDebugGroup       = 0x38,   //0x38~0x3F: H2O Driver Debug Code information
  CurrentDebugCode        = 0x39,
  BackupDebugGroup1       = 0x3A,
  BackupDebugCode1        = 0x3B,
  BackupDebugGroup2       = 0x3C,
  BackupDebugCode2        = 0x3D,
  BackupDebugGroup3       = 0x3E,
  BackupDebugCode3        = 0x3F,
  MonotonicCount          = 0x40,   //0x40~0x43: Monotonic Count
  SimpleBootFlag          = 0x44,   //0x44     : Simple Boot Flag
                                    //0x45     : Chipset Reserve Watchdog Timer status
  CmosMonthWakeUp         = 0x46,   //0x46     : Wake up month
  CmosDayWakeUp           = 0x47,   //0x47     : Wake up day
                                    //0x48~0x4F: OEM Reserve
  UserPassword            = 0x50,   //0x50~0x5B: User Password
  CmosTimeZoneWakeUp      = 0x5C,   //0x5C~0x5D: Wake up Time zone
  CmosDaylightWakeUp      = 0x5E,   //0x5E     : Wake up Daylight
  LastBootDevice          = 0x5F,   //0x5F     : LastBootDeviceIndex 
  SupervisorPassword      = 0x60    //0x60~0x6B: Supervisor Password
                                    //0x6C~0x7F: OEM Reserve

} COMMON_CMOS_TABLE1;

#endif
