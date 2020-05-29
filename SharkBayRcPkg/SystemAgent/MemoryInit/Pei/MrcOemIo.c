/** @file

  This file contains the I/O port related functions.  
  
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

#include <Library/IoLib.h>
#include <MrcTypes.h>
#include <MrcOemIo.h>
/**
  8 bit I/O port read.

  @param[in] IoAddress - The I/O port read address.

  @retval The value read.
*/
U8
MrcOemInPort8 (
  IN const U16 IoAddress
  )
{
  return IoRead8 (IoAddress);
}

/**
  8 bit I/O port write.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing.
*/
void
MrcOemOutPort8 (
  IN const U16 IoAddress,
  IN const U8  Data
  )
{
  IoWrite8 (IoAddress, Data);
}

/**
  16 bit I/O port read.


  @param[in] IoAddress - The I/O port read address.

  @retval The value read.
*/
U16
MrcOemInPort16 (
  IN const U16 IoAddress
  )
{
  return IoRead16 (IoAddress);
}

/**
  16 bit I/O port write.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing.
*/
void
MrcOemOutPort16 (
  IN const U16 IoAddress,
  IN const U16 Data
  )
{
  IoWrite16 (IoAddress, Data);
}

/**
  32 bit I/O port read.

  @param[in] IoAddress - The I/O port read address.

  @retval The value read.
*/
U32
MrcOemInPort32 (
  IN const U16 IoAddress
  )
{
  return IoRead32 (IoAddress);
}

/**
  32 bit I/O port write.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing.
*/
void
MrcOemOutPort32 (
  IN const U16 IoAddress,
  IN const U32 Data
  )
{
  IoWrite32 (IoAddress, Data);
}

/**
  The PCI index address.

  @param[in] None.

  @retval The PCI index address.
*/
U16
MrcOemPciIndex (
  void
  )
{
  return 0xCF8;
}

/**
  The PCI data address.

  @param[in] None.

  @retval The PCI data address.
*/
U16
MrcOemPciData (
  void
  )
{
  return 0xCFC;
}

/**
  Calculate the PCI device address for the given Bus/Device/Function/Offset.

  @param[in] Bus      - PCI bus 
  @param[in] Device   - PCI device 
  @param[in] Function - PCI function 
  @param[in] Offset   - Offset

  @retval The PCI device address.
*/
U32
MrcOemGetPciDeviceAddress (
  IN const U8 Bus,
  IN const U8 Device,
  IN const U8 Function,
  IN const U8 Offset
  )
{
  return (
    ((U32) ((Bus)      & 0xFF) << 16) |
    ((U32) ((Device)   & 0x1F) << 11) |
    ((U32) ((Function) & 0x07) << 8)  |
    ((U32) ((Offset)   & 0xFF) << 0)  |
    (1UL << 31));
}

/**
  Calculate the PCIE device address for the given Bus/Device/Function/Offset.

  @param[in] Bus      - PCI bus 
  @param[in] Device   - PCI device 
  @param[in] Function - PCI function 
  @param[in] Offset   - Offset

   The PCIE device address.

  @retval The PCIe device address
*/
U32
MrcOemGetPcieDeviceAddress (
  IN const U8 Bus,
  IN const U8 Device,
  IN const U8 Function,
  IN const U8 Offset
  )
{
  return ((U32) Bus << 20) + ((U32) Device << 15) + ((U32) Function << 12) + ((U32) Offset << 0);
}

/**
  Read 32-bit value from the specified bus/device/function/offset.

  @param[in] bus      - PCI bus number.
  @param[in] device   - PCI device number.
  @param[in] function - PCI function number.
  @param[in] offset   - PCI address offset.

  @retval 32-bit PCI value.
*/
U32
MrcOemPciRead32 (
  IN const U8 bus,
  IN const U8 device,
  IN const U8 function,
  IN const U8 offset
  )
{
  MrcOemOutPort32 (MrcOemPciIndex (), MrcOemGetPciDeviceAddress (bus, device, function, offset));
  return MrcOemInPort32 (MrcOemPciData ());
}

/**
  Check if RTC date and time update is in progress and wait util it's finished.
  We have at least 244us when "update in progress bit" is seen as low to
  perform an operation on the RTC.

  @param[in] None.

  @retval Zero on timeout or non-zero and RTC is ready for transaction.
*/
U32
CheckUpdateComplete (
  void
  )
{
  U32 Timeout;

  //
  // Wait until RTC "update in progress" bit goes low.
  //
  Timeout = 0x0FFFFF;
  do {
    MrcOemOutPort8 (RTC_INDEX_REGISTER, CMOS_REGA);
    if ((MrcOemInPort8 (RTC_TARGET_REGISTER) & RTC_UPDATE_IN_PROGRESS) != RTC_UPDATE_IN_PROGRESS) {
      break;
    }
  } while (--Timeout > 0);

  return Timeout;
}

/**
  Initializes the RTC.

  @param[in] None.

  @retval Nothing.
*/
void
InitRtc (
  void
  )
{
  MrcOemOutPort8 (RTC_INDEX_REGISTER, CMOS_REGB);
  MrcOemOutPort8 (RTC_TARGET_REGISTER, RTC_HOLD | RTC_MODE_24HOUR);

  MrcOemOutPort8 (RTC_INDEX_REGISTER, CMOS_REGA);
  MrcOemOutPort8 (RTC_TARGET_REGISTER, RTC_CLOCK_DIVIDER | RTC_RATE_SELECT);

  MrcOemOutPort8 (RTC_INDEX_REGISTER, CMOS_REGC);
  MrcOemInPort8 (RTC_TARGET_REGISTER);

  MrcOemOutPort8 (RTC_INDEX_REGISTER, CMOS_REGD);
  MrcOemInPort8 (RTC_TARGET_REGISTER);

  MrcOemOutPort8 (RTC_INDEX_REGISTER, CMOS_REGB);
  MrcOemOutPort8 (RTC_TARGET_REGISTER, RTC_MODE_24HOUR);

  return;
}

/**
  Returns the current time, as determined by reading the Real Time Clock (RTC) on the platform.
  Since RTC time is stored in BCD, convert each value to binary.

  @param[out] Seconds       - The current second (0-59).
  @param[out] Minutes       - The current minute (0-59).
  @param[out] Hours         - The current hour (0-23).
  @param[out] DayOfMonth    - The current day of the month (1-31).
  @param[out] Month         - The current month (1-12).
  @param[out] Year          - The current year (2000-2099).

  @retval Nothing.
*/
void
MrcOemGetRtcTime (
  OUT U8  *const Seconds,
  OUT U8  *const Minutes,
  OUT U8  *const Hours,
  OUT U8  *const DayOfMonth,
  OUT U8  *const Month,
  OUT U16 *const Year
  )
{
  if (0 == CheckUpdateComplete ()) {
    InitRtc ();
  }
  //
  // Read seconds
  //
  MrcOemOutPort8 (RTC_INDEX_REGISTER, RTC_SECONDS);
  *Seconds = MrcOemInPort8 (RTC_TARGET_REGISTER);

  //
  // Read minutes
  //
  MrcOemOutPort8 (RTC_INDEX_REGISTER, RTC_MINUTES);
  *Minutes = MrcOemInPort8 (RTC_TARGET_REGISTER);

  //
  // Read hours
  //
  MrcOemOutPort8 (RTC_INDEX_REGISTER, RTC_HOURS);
  *Hours = MrcOemInPort8 (RTC_TARGET_REGISTER);

  //
  // Read day of month
  //
  MrcOemOutPort8 (RTC_INDEX_REGISTER, RTC_DAY_OF_MONTH);
  *DayOfMonth = MrcOemInPort8 (RTC_TARGET_REGISTER);

  //
  // Read month
  //
  MrcOemOutPort8 (RTC_INDEX_REGISTER, RTC_MONTH);
  *Month = MrcOemInPort8 (RTC_TARGET_REGISTER);

  //
  // Read year and add current century.
  //
  MrcOemOutPort8 (RTC_INDEX_REGISTER, RTC_YEAR);
  *Year = MrcOemInPort8 (RTC_TARGET_REGISTER);

  *Seconds    = BCD2BINARY (*Seconds);
  *Minutes    = BCD2BINARY (*Minutes);
  *Hours      = BCD2BINARY (*Hours);
  *DayOfMonth = BCD2BINARY (*DayOfMonth);
  *Month      = BCD2BINARY (*Month);
  *Year       = BCD2BINARY (*Year) + CENTURY_OFFSET;
}

/**
  Read specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.
*/
U8
RtcRead (
  IN const U8 Location
  )
{
  U8  RtcIndexPort;
  U8  RtcDataPort;

  //
  // CMOS access registers (using alternative access not to handle NMI bit)
  //
  if (Location < RTC_BANK_SIZE) {
    //
    // First bank
    //
    RtcIndexPort  = R_PCH_RTC_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_TARGET_ALT;
  } else {
    //
    // Second bank
    //
    RtcIndexPort  = R_PCH_RTC_EXT_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_EXT_TARGET_ALT;
  }

  MrcOemOutPort8 (RtcIndexPort, Location & RTC_INDEX_MASK);
  return MrcOemInPort8 (RtcDataPort);
}

/**
  Write specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM
*/
void
RtcWrite (
  IN const U8 Location,
  IN const U8 Value
  )
{
  U8  RtcIndexPort;
  U8  RtcDataPort;

  //
  // CMOS access registers (using alternative access not to handle NMI bit)
  //
  if (Location < RTC_BANK_SIZE) {
    //
    // First bank
    //
    RtcIndexPort  = R_PCH_RTC_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_TARGET_ALT;
  } else {
    //
    // Second bank
    //
    RtcIndexPort  = R_PCH_RTC_EXT_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_EXT_TARGET_ALT;
  }

  MrcOemOutPort8 (RtcIndexPort, Location & RTC_INDEX_MASK);
  MrcOemOutPort8 (RtcDataPort, Value);
}

/**
  Read word from specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.
*/
U16
RtcRead16 (
  IN const U8 Location
  )
{
  return RtcRead (Location) | (RtcRead (Location + 1) << 8);
}

/**
  Write word to specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM
*/
void
RtcWrite16 (
  IN const U8   Location,
  IN const U16  Value
  )
{
  RtcWrite (Location, (U8) Value);
  RtcWrite (Location + 1, (U8) (Value >> 8));
}
