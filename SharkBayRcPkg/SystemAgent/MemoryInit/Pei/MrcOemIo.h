/** @file

  This file contains the I/O port related definitions.
  
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
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MRC_OEM_IO_H_
#define _MRC_OEM_IO_H_

#pragma pack (push, 1)

#include <MrcTypes.h>

#define RTC_INDEX_REGISTER        (0x70)
#define RTC_TARGET_REGISTER       (0x71)
#define R_PCH_RTC_INDEX_ALT       (0x74)
#define R_PCH_RTC_TARGET_ALT      (0x75)
#define R_PCH_RTC_EXT_INDEX_ALT   (0x76)
#define R_PCH_RTC_EXT_TARGET_ALT  (0x77)

#define RTC_INDEX_MASK            (0x7F)
#define RTC_BANK_SIZE             (0x80)

#define RTC_SECONDS               (0x00)
#define RTC_MINUTES               (0x02)
#define RTC_HOURS                 (0x04)
#define RTC_DAY_OF_MONTH          (0x07)
#define RTC_MONTH                 (0x08)
#define RTC_YEAR                  (0x09)
#define CMOS_REGA                 (0x0A)
#define CMOS_REGB                 (0x0B)
#define CMOS_REGC                 (0x0C)
#define CMOS_REGD                 (0x0D)

#define RTC_UPDATE_IN_PROGRESS    (0x80)
#define RTC_HOLD                  (0x80)
#define RTC_MODE_24HOUR           (0x02)
#define RTC_CLOCK_DIVIDER         (0x20)
#define RTC_RATE_SELECT           (0x06)

#define BCD2BINARY(A)             (((((A) >> 4) & 0xF) * 10) + ((A) & 0xF))
#define CENTURY_OFFSET            (2000)

#define MRC_POST_CODE_LOW_BYTE_ADDR   (0x48)
#define MRC_POST_CODE_HIGH_BYTE_ADDR  (0x49)

/**

@brief

  8 bit I/O port read.

  @param[in] IoAddress - The I/O port read address.

  @retval The value read.

**/
extern
U8
MrcOemInPort8 (
  IN const U16 IoAddress
  );

/**

@brief

  8 bit I/O port write.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing.

**/
extern
void
MrcOemOutPort8 (
  IN const U16 IoAddress,
  IN const U8  Data
  );

/**

@brief

  16 bit I/O port read.


  @param[in] IoAddress - The I/O port read address.

  @retval The value read.

**/
extern
U16
MrcOemInPort16 (
  IN const U16 IoAddress
  );

/**

@brief

  16 bit I/O port write.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing.

**/
extern
void
MrcOemOutPort16 (
  IN const U16 IoAddress,
  IN const U16 Data
  );

/**

@brief

  32 bit I/O port read.

  @param[in] IoAddress - The I/O port read address.

  @retval The value read.

**/
extern
U32
MrcOemInPort32 (
  IN const U16 IoAddress
  );

/**

@brief

  32 bit I/O port write.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing.

**/
extern
void
MrcOemOutPort32 (
  IN const U16 IoAddress,
  IN const U32 Data
  );

/**

@brief

  The PCI index address.

  @param[in] None.

  @retval The PCI index address.

**/
extern
U16
MrcOemPciIndex (
  void
  );

/**

@brief

  The PCI data address.

  @param[in] None.

  @retval The PCI data address.

**/
extern
U16
MrcOemPciData (
  void
  );

/**

@brief

  Calculate the PCI device address for the given Bus/Device/Function/Offset.

  @param[in] Bus      - PCI bus 
  @param[in] Device   - PCI device 
  @param[in] Function - PCI function 
  @param[in] Offset   - Offset

  @retval The PCI device address.

**/
extern
U32
MrcOemGetPciDeviceAddress (
  IN const U8 Bus,
  IN const U8 Device,
  IN const U8 Function,
  IN const U8 Offset
  );

/**

@brief

  Calculate the PCIE device address for the given Bus/Device/Function/Offset.

  @param[in] Bus      - PCI bus 
  @param[in] Device   - PCI device 
  @param[in] Function - PCI function 
  @param[in] Offset   - Offset

   The PCIe device address.

  @retval 
**/
extern
U32
MrcOemGetPcieDeviceAddress (
  IN const U8 Bus,
  IN const U8 Device,
  IN const U8 Function,
  IN const U8 Offset
  );

/**

@brief

  Read 32-bit value from the specified bus/device/function/offset.

  @param[in] bus      - PCI bus number.
  @param[in] device   - PCI device number.
  @param[in] function - PCI function number.
  @param[in] offset   - PCI address offset.

  @retval 32-bit PCI value.

**/
extern
U32
MrcOemPciRead32 (
  IN const U8 bus,
  IN const U8 device,
  IN const U8 function,
  IN const U8 offset
  );

/**

@brief

  Check if RTC date and time update is in progress and wait util it's finished.
  We have at least 244us when "update in progress bit" is seen as low to
  perform an operation on the RTC.

  @param[in] None.

  @retval Zero on timeout or non-zero and RTC is ready for transaction.

**/
extern
U32
CheckUpdateComplete (
  void
  );

/**

@brief

  Initializes the RTC.

  @param[in] None.

  @retval Nothing.

**/
extern
void
InitRtc (
  void
  );

/**

@brief

  Returns the current time, as determined by reading the Real Time Clock (RTC) on the platform.
  Since RTC time is stored in BCD, convert each value to binary.

  @param[out] Seconds       - The current second (0-59).
  @param[out] Minutes       - The current minute (0-59).
  @param[out] Hours         - The current hour (0-23).
  @param[out] DayOfMonth    - The current day of the month (1-31).
  @param[out] Month         - The current month (1-12).
  @param[out] Year          - The current year (2000-2099).

  @retval Nothing.

**/
extern
void
MrcOemGetRtcTime (
  OUT U8  *const Seconds,
  OUT U8  *const Minutes,
  OUT U8  *const Hours,
  OUT U8  *const DayOfMonth,
  OUT U8  *const Month,
  OUT U16 *const Year
  );

/**

@brief

  Read specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.

**/
extern
U8
RtcRead (
  IN const U8 Location
  );

/**

@brief

  Write specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM

**/
extern
void
RtcWrite (
  IN const U8 Location,
  IN const U8 Value
  );

/**

@brief

  Read word from specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.

**/
extern
U16
RtcRead16 (
  IN const U8 Location
  );

/**

@brief

  Write word to specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM

**/
extern
void
RtcWrite16 (
  IN const U8   Location,
  IN const U16  Value
  );

#pragma pack(pop)
#endif // _MrcOemIo_h_
