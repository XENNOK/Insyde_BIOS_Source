/** @file
 Header file for real time clock driver.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2006 - 2007, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#ifndef _RTC_H_
#define _RTC_H_

#include <Uefi.h>

#include <Protocol/RealTimeClock.h>
#include <Protocol/Cpu.h>
#include <Guid/HobList.h>
#include <Guid/DebugMask.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/CmosLib.h>
#include <Library/HobLib.h>

typedef struct {
  EFI_LOCK  RtcLock;
} PC_RTC_MODULE_GLOBALS;

#define TIMEZONE_UPDATED          L"TimeZoneUpdated"

#define PCAT_RTC_ADDRESS_REGISTER 0x70
#define PCAT_RTC_DATA_REGISTER    0x71

//
// Dallas DS12C887 Real Time Clock
//
#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_SECONDS_ALARM     1   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES_ALARM     3   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_HOURS_ALARM       5   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_WEEK   6   // R/W  Range 1..7
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define RTC_ADDRESS_REGISTER_B        11  // R/W
#define RTC_ADDRESS_REGISTER_C        12  // RO
#define RTC_ADDRESS_REGISTER_D        13  // RO
#define RTC_ADDRESS_CENTURY           50  // R/W  Range 19..20 Bit 8 is R/W

//
// Register initial values
//
#define RTC_INIT_REGISTER_A 0x26
#define RTC_INIT_REGISTER_B 0x02
#define RTC_INIT_REGISTER_D 0x0

//
// The maximum retry for RTC reading
//
#define MAX_RETRY       10000

typedef struct {
  UINT8  Address; ///< RTC offset address.
  UINT8  Data;    ///< Corresponding data value.
} RTC_COMPONENT;

#pragma pack(1)
//
// Register A
//
typedef struct {
  UINT8 Rs : 4;   ///< Rate Selection Bits
  UINT8 Dv : 3;   ///< Divisor
  UINT8 Uip : 1;  ///< Update in progress
} RTC_REGISTER_A_BITS;

typedef union {
  RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_A;

//
// Register B
//
typedef struct {
  UINT8 Dse : 1;  ///< 0 - Daylight saving disabled  1 - Daylight savings enabled
  UINT8 Mil : 1;  ///< 0 - 12 hour mode              1 - 24 hour mode
  UINT8 Dm : 1;   ///< 0 - BCD Format                1 - Binary Format
  UINT8 Sqwe : 1; ///< 0 - Disable SQWE output       1 - Enable SQWE output
  UINT8 Uie : 1;  ///< 0 - Update INT disabled       1 - Update INT enabled
  UINT8 Aie : 1;  ///< 0 - Alarm INT disabled        1 - Alarm INT Enabled
  UINT8 Pie : 1;  ///< 0 - Periodic INT disabled     1 - Periodic INT Enabled
  UINT8 Set : 1;  ///< 0 - Normal operation.         1 - Updates inhibited
} RTC_REGISTER_B_BITS;

typedef union {
  RTC_REGISTER_B_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_B;

//
// Register C
//
typedef struct {
  UINT8 Reserved : 4; ///< Read as zero.  Can not be written.
  UINT8 Uf : 1;       ///< Update End Interrupt Flag
  UINT8 Af : 1;       ///< Alarm Interrupt Flag
  UINT8 Pf : 1;       ///< Periodic Interrupt Flag
  UINT8 Irqf : 1;     ///< Iterrupt Request Flag = PF & PIE | AF & AIE | UF & UIE
} RTC_REGISTER_C_BITS;

typedef union {
  RTC_REGISTER_C_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_C;

//
// Register D
//
typedef struct {
  UINT8 Reserved : 7; ///< Read as zero.  Can not be written.
  UINT8 Vrt : 1;      ///< Valid RAM and Time
} RTC_REGISTER_D_BITS;

typedef union {
  RTC_REGISTER_D_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_D;

#pragma pack()

/**
 Initialize RTC.

 @param[in] Global         For global use inside this module.

 @retval EFI_SUCCESS       Initialization successful.
 @retval EFI_DEVICE_ERROR  Initialization failed due to device error.
**/
EFI_STATUS
PcRtcInit (
  IN PC_RTC_MODULE_GLOBALS  *Global
  );

/**
 Sets the current local time and date information.

 @param[in] Time                 A pointer to the current time.
 @param[in] Global               For global use inside this module.

 @retval EFI_SUCCESS             The operation completed successfully.
 @retval EFI_INVALID_PARAMETER   A time field is out of range.
 @retval EFI_DEVICE_ERROR        The time could not be set due due to hardware error.
**/
EFI_STATUS
PcRtcSetTime (
  IN EFI_TIME               *Time,
  IN PC_RTC_MODULE_GLOBALS  *Global
  );

/**
 Returns the current time and date information, and the time-keeping capabilities
 of the hardware platform.

 @param[out] Time          A pointer to storage to receive a snapshot of the current time.
 @param[out] Capabilities  An optional pointer to a buffer to receive the real time clock
                           device's capabilities.
 @param[in]  Global        For global use inside this module.

 @retval EFI_SUCCESS            The operation completed successfully.
 @retval EFI_INVALID_PARAMETER  Time is NULL.
 @retval EFI_DEVICE_ERROR       The time could not be retrieved due to hardware error.
**/
EFI_STATUS
PcRtcGetTime (
  OUT EFI_TIME              *Time,
  OUT EFI_TIME_CAPABILITIES *Capabilities, OPTIONAL
  IN  PC_RTC_MODULE_GLOBALS *Global
  );

/**
 Sets the system wakeup alarm clock time.

 @param[in] Enabled  Enable or disable the wakeup alarm.
 @param[in] Time     If Enable is TRUE, the time to set the wakeup alarm for.
                     If Enable is FALSE, then this parameter is optional, and may be NULL.
 @param[in] Global   For global use inside this module.

 @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was enabled.
                               If Enable is FALSE, then the wakeup alarm was disabled.
 @retval EFI_INVALID_PARAMETER A time field is out of range.
 @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a hardware error.
 @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.
**/
EFI_STATUS
PcRtcSetWakeupTime (
  IN BOOLEAN                Enable,
  IN EFI_TIME               *Time,  OPTIONAL
  IN PC_RTC_MODULE_GLOBALS  *Global
  );

/**
 Returns the current wakeup alarm clock setting.

 @param[out] Enabled  Indicates if the alarm is currently enabled or disabled.
 @param[out] Pending  Indicates if the alarm signal is pending and requires acknowledgment.
 @param[out] Time     The current alarm setting.
 @param[in]  Global   For global use inside this module.

 @retval EFI_SUCCESS           The alarm settings were returned.
 @retval EFI_INVALID_PARAMETER Enabled is NULL.
 @retval EFI_INVALID_PARAMETER Pending is NULL.
 @retval EFI_INVALID_PARAMETER Time is NULL.
 @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a hardware error.
 @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.
**/
EFI_STATUS
PcRtcGetWakeupTime (
  OUT BOOLEAN               *Enabled,
  OUT BOOLEAN               *Pending,
  OUT EFI_TIME              *Time,
  IN  PC_RTC_MODULE_GLOBALS *Global
  );

/**
 The user Entry Point for PcRTC module.

 This is the entrhy point for PcRTC module. It installs the UEFI runtime service
 including GetTime(),SetTime(),GetWakeupTime(),and SetWakeupTime().

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS    The entry point is executed successfully.
 @retval Others         Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
InitializePcRtc (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  );

/**
 See if all fields of a variable of EFI_TIME type is correct.

 @param[in] Time                The time to be checked.

 @retval EFI_SUCCESS            Time is a valid EFI_TIME variable.
 @retval EFI_INVALID_PARAMETER  Some fields of Time are not correct.
**/
EFI_STATUS
RtcTimeFieldsValid (
  IN EFI_TIME *Time
  );

/**
 Converts time from EFI_TIME format defined by UEFI spec to RTC's.

 This function converts time from EFI_TIME format defined by UEFI spec to RTC's.
 If data mode of RTC is BCD, then converts EFI_TIME to it.
 If RTC is in 12-hour format, then converts EFI_TIME to it.

 @param[in, out] Time       On input, the time data read from UEFI to convert
                            On output, the time converted to RTC format
 @param[in]      RegisterB  Value of Register B of RTC, indicating data mode
 @param[out]     Century    It is set according to EFI_TIME Time.
**/
VOID
ConvertEfiTimeToRtcTime (
  IN OUT EFI_TIME        *Time,
  IN     RTC_REGISTER_B  RegisterB,
  OUT    UINT8           *Century
  );

/**
 Converts time read from RTC to EFI_TIME format defined by UEFI spec.

 This function converts raw time data read from RTC to the EFI_TIME format defined by UEFI spec.
 If data mode of RTC is BCD, then converts it to decimal,
 If RTC is in 12-hour format, then converts it to 24-hour format.

 @param[in, out] Time       On input, the time data read from RTC to convert
                            On output, the time converted to UEFI format
 @param[in]      Century    Value of century read from RTC.
 @param[in]      RegisterB  Value of Register B of RTC, indicating data mode and hour format.
**/
VOID
ConvertRtcTimeToEfiTime (
  IN OUT EFI_TIME        *Time,
  IN     UINT8           Century,
  IN     RTC_REGISTER_B  RegisterB
  );

/**
 Wait for a period for the RTC to be ready.

 @param[in] Timeout         Tell how long it should take to wait.

 @retval EFI_SUCCESS        RTC is updated and ready.
 @retval EFI_DEVICE_ERROR   RTC device error.
**/
EFI_STATUS
RtcWaitToUpdate (
  IN UINTN Timeout
  );

/**
 See if field Day of an EFI_TIME is correct.

 @param[in] Time   Its Day field is to be checked.

 @retval TRUE      Day field of Time is correct.
 @retval FALSE     Day field of Time is NOT correct.
**/
BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  );

/**
 Check if it is a leap year.

 @param[in] Time   The time to be checked.

 @retval TRUE      It is a leap year.
 @retval FALSE     It is NOT a leap year.
**/
BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  );

/**
 Compare the Hour, Minute and Second of the From time and the To time.

 Only compare H/M/S in EFI_TIME and ignore other fields here.

 @param[in] From   the first time
 @param[in] To     the second time

 @return >0   The H/M/S of the From time is later than those of To time
 @return ==0  The H/M/S of the From time is same as those of To time
 @return <0   The H/M/S of the From time is earlier than those of To time
**/
INTN
CompareHMS (
  IN EFI_TIME   *From,
  IN EFI_TIME   *To
  );

/**
 To check if second date is later than first date within 24 hours.

 @param[in] From   the first date
 @param[in] To     the second date

 @retval TRUE      From is previous to To within 24 hours.
 @retval FALSE     From is later, or it is previous to To more than 24 hours.
**/
BOOLEAN
IsWithinOneDay (
  IN EFI_TIME   *From,
  IN EFI_TIME   *To
  );

#endif

