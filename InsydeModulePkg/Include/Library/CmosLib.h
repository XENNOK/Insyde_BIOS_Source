/** @file
 CMOS library include file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CMOS_LIB_H_
#define _CMOS_LIB_H_

#include <Uefi.h>


/**
 Read an 8-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return An 8-bit value in the address offset of CMOS.
**/
UINT8
ReadCmos8 (
  IN UINT8                                 Address
  );

/**
 Write an 8-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
WriteCmos8 (
  IN UINT8                                 Address,
  IN UINT8                                 Data
  );

/**
 Read a 16-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return A 16-bit value in the address offset of CMOS.
**/
UINT16
ReadCmos16 (
  IN UINT8                                 Address
  );

/**
 Write a 16-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
WriteCmos16 (
  IN UINT8                                 Address,
  IN UINT16                                Data
  );

/**
 Read a 32-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return A 32-bit value in the address offset of CMOS.
**/
UINT32
ReadCmos32 (
  IN UINT8                                 Address
  );

/**
 Write a 32-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
WriteCmos32 (
  IN UINT8                                 Address,
  IN UINT32                                Data
  );

/**
 Sum standard CMOS.

 @return Summed bytes 0x10 through 0x2D
**/
UINT16
SumaryCmos (
  VOID
  );

/**
 Validate standard CMOS.

 @retval EFI_SUCCESS            valid checksum
 @retval EFI_VOLUME_CORRUPTED   a corrupted checksum
**/
EFI_STATUS
ValidateCmosChecksum (
  VOID
  );

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Read an 8-bit value from the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.

 @return An 8-bit value read.
**/
UINT8
ReadExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  );

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Read a 16-bit value from the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.

 @return A 16-bit value read.
**/
UINT16
ReadExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  );

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Read an 32-bit value from the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.

 @return A 32-bit value read.
**/
UINT32
ReadExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address
  );

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Write an 8-bit value to the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.
 @param[in] Data        Data written into data port.
**/
VOID
WriteExtCmos8 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT8                                 Data
  );

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Write a 16-bit value to the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.
 @param[in] Data        Data written into data port.
**/
VOID
WriteExtCmos16 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT16                                Data
  );

/**
 Base on writing the 8-bit I/O port specified by index port with the address value.
 Write a 32-bit value to the 8-bit I/O port specified by data port.

 @param[in] XCmosIndex  Index port.
 @param[in] XCmosData   Data port.
 @param[in] Address     Address value.
 @param[in] Data        Data written into data port.
**/
VOID
WriteExtCmos32 (
  IN UINT8                                 XCmosIndex,
  IN UINT8                                 XCmosData,
  IN UINT8                                 Address,
  IN UINT32                                Data
  );

typedef enum {
                                    ///<0x00~0x0F: Real Time Clock Reserve
                                    ///<0x10~0x2D: IBM PC Standard Reserve
  CmosCheckSum2E          = 0x2E,   ///<0x2E~0x2F: CMOS Check Sum
                                    ///<0x30~0x32: IBM PC Standard Reserve
  SmartBootWatchdogFlag   = 0x33,   ///<0x33     : SmartBoot Watchdog flag
  CmosTimeZone            = 0x34,   ///<0x34~0x35: Time zone
  CmosDaylight            = 0x36,   ///<0x36     : Daylight
  CmosYearWakeUp          = 0x37,   ///<0x37     : Wake up year
  CurrentDebugGroup       = 0x38,   ///<0x38~0x3F: H2O Driver Debug Code information
  CurrentDebugCode        = 0x39,
  BackupDebugGroup1       = 0x3A,
  BackupDebugCode1        = 0x3B,
  BackupDebugGroup2       = 0x3C,
  BackupDebugCode2        = 0x3D,
  BackupDebugGroup3       = 0x3E,
  BackupDebugCode3        = 0x3F,
  MonotonicCount          = 0x40,   ///<0x40~0x43: Monotonic Count
  SimpleBootFlag          = 0x44,   ///<0x44     : Simple Boot Flag
                                    ///<0x45     : Chipset Reserve Watchdog Timer status
  CmosMonthWakeUp         = 0x46,   ///<0x46     : Wake up month
  CmosDayWakeUp           = 0x47,   ///<0x47     : Wake up day
                                    ///<0x48~0x4F: OEM Reserve
  UserPassword            = 0x50,   ///<0x50~0x5B: User Password
  CmosTimeZoneWakeUp      = 0x5C,   ///<0x5C~0x5D: Wake up Time zone
  CmosDaylightWakeUp      = 0x5E,   ///<0x5E     : Wake up Daylight
  LastBootDevice          = 0x5F,   ///<0x5F     : LastBootDeviceIndex
  SupervisorPassword      = 0x60    ///<0x60~0x6B: Supervisor Password
                                    ///<0x6C~0x7F: OEM Reserve
} COMMON_CMOS_TABLE1;

#endif

