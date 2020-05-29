/** @file
 RTC Architectural Protocol GUID as defined in DxeCis 0.96.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#include "PcRtc.h"
#include <PostCode.h>

EFI_TIME                      mTime;
UINT64                        mRecordTick;

BOOLEAN
IsValidYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year < PcdGet16 (PcdRealTimeClockYearMin) || Time->Year > PcdGet16 (PcdRealTimeClockYearMax)) {
    return FALSE;
  }

  return TRUE;
}

/**
 Read RTC content through its registers.

 @param[in] Address  Address offset of RTC. It is recommended to use macros such as RTC_ADDRESS_SECONDS.

 @return The data of UINT8 type read from RTC.
**/
UINT8
RtcRead (
  IN  UINT8 Address
  )
{
  return ReadCmos8 (Address);
}

/**
 Write RTC through its registers.

 @param[in] Address  Address offset of RTC. It is recommended to use macros such as RTC_ADDRESS_SECONDS.
 @param[in] Data     The content you want to write into RTC.
**/
VOID
RtcWrite (
  IN  UINT8   Address,
  IN  UINT8   Data
  )
{
  WriteCmos8 ((UINT8) Address, Data);
}

/**
 Calcluate weekday based on input time. The year of input time only supports in the range [1900,2100).

 @param[in]  Time     Input time which contains date info.
 @param[out] Output   Output weekday value which is between 1(Monday) ~ 7(Sunday).

 @retval EFI_SUCCESS            Get weekday successfully.
 @retval EFI_INVALID_PARAMETER  Input parameter pointer is NULL or input time is invalid.
 @retval EFI_UNSUPPORTED        Year value is not in the range [1900,2100).
**/
EFI_STATUS
GetWeekday (
  IN  EFI_TIME *Time,
  OUT UINT8    *Weekday
  )
{
  UINTN    CenturyOffset;
  UINT16   Year;
  UINTN    YearOffset;
  UINTN    MonthOffset[] = {0,3,3,6,1,4,6,2,5,0,3,5};
  UINTN    WeekdayOffset;

  if (Time == NULL || Weekday == NULL || Time->Month < 1 || Time->Month > 12 || !DayValid (Time)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Time->Year < 1900 || Time->Year > 2099) {
    return EFI_UNSUPPORTED;
  }

  if (Time->Year >= 2000) {
    CenturyOffset = 6;
    Year = Time->Year - 2000;
  } else {
    CenturyOffset = 0;
    Year = Time->Year - 1900;
  }

  YearOffset = (UINTN) (Year + Year / 4);

  WeekdayOffset = CenturyOffset + YearOffset + MonthOffset[Time->Month - 1] + (UINTN) Time->Day;
  if (IsLeapYear (Time) && Time->Month <= 2) {
    WeekdayOffset--;
  }
  WeekdayOffset %= 7;

  if (WeekdayOffset == 0) {
    *Weekday = 7;
  } else {
    *Weekday = (UINT8) WeekdayOffset;
  }

  return EFI_SUCCESS;
}

/**
 Calculate the CMOS checksum and write it.

 @retval EFI_SUCCESS      Calculate 16-bit checksum successfully
**/
EFI_STATUS
WriteCmosChecksum (
  VOID
  )
{
  UINT16  Checksum;
  UINT16  TempChecksum;

  Checksum = SumaryCmos ();

  TempChecksum = (Checksum >> 8) + ((Checksum & 0xff) << 8);

  WriteCmos16 (CmosCheckSum2E, TempChecksum);

  return EFI_SUCCESS;
}

/**
 Get RTC time data. Before reading every RTC data, it will check RTC update status
 to make sure every RTC data is correct.

 @param[out] Time               Pointer to output time data

 @retval EFI_SUCCESS            Get RTC time data successfully.
 @retval EFI_NOT_READY          RTC data is in update progress. Fail to get time data.
 @retval EFI_INVALID_PARAMETER  Pointer of output time data is NULL.
**/
EFI_STATUS
GetTimeWithRtcUpdateCheck (
  OUT EFI_TIME              *Time
  )
{
  RTC_REGISTER_A  RegisterA;
  RTC_REGISTER_B  RegisterB;
  UINT8           Century;
  UINTN           Index;
  UINTN           RtcComponentNum;
  RTC_COMPONENT   RtcComponent[] = {{RTC_ADDRESS_YEAR            , 0},
                                    {RTC_ADDRESS_MONTH           , 0},
                                    {RTC_ADDRESS_DAY_OF_THE_MONTH, 0},
                                    {RTC_ADDRESS_HOURS           , 0},
                                    {RTC_ADDRESS_MINUTES         , 0},
                                    {RTC_ADDRESS_SECONDS         , 0}
                                    };

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RtcComponentNum = sizeof(RtcComponent) / sizeof(RTC_COMPONENT);
  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);

  //
  // In order to preventing RTC data reading error upon RTC updating due to interrupt/SMI caused delay,
  // making RTC update checking before every RTC data reading to make sure every RTC data corrected
  //
  for (Index = 0; Index < RtcComponentNum; Index++) {
    RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
    if (RegisterA.Bits.Uip == 1) {
      MicroSecondDelay (10);
      return EFI_NOT_READY;
    }

    RtcComponent[Index].Data = RtcRead (RtcComponent[Index].Address);
  }

  Time->Year   = (UINT16) RtcComponent[0].Data;
  Time->Month  = RtcComponent[1].Data;
  Time->Day    = RtcComponent[2].Data;
  Time->Hour   = RtcComponent[3].Data;
  Time->Minute = RtcComponent[4].Data;
  Time->Second = RtcComponent[5].Data;

  Time->TimeZone = (INT16) (ReadCmos16 (CmosTimeZone));
  Time->Daylight = ReadCmos8 (CmosDaylight);

  Century = RtcRead (RTC_ADDRESS_CENTURY);

  ConvertRtcTimeToEfiTime (Time, Century, RegisterB);

  return EFI_SUCCESS;
}

/**
 Get CPU frequency.

 @param[out] Frequency          Pointer to CPU frequency in tick/ms.

 @retval EFI_SUCCESS            Success to get CPU frequency.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
 @retval Other                  Fail to locate CPU architecture protocol or CPU protocol function return fail.
**/
EFI_STATUS
GetCpuFrequency (
  OUT UINT64     *Frequency
  )
{
  EFI_STATUS             Status;
  EFI_CPU_ARCH_PROTOCOL  *Cpu;
  UINT64                 CurrentTick;
  UINT64                 TimerPeriod;

  if (Frequency == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&Cpu);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Cpu->GetTimerValue (Cpu, 0, &CurrentTick, &TimerPeriod);
  if (!EFI_ERROR (Status)) {
    //
    // The TimerPeriod is in femtosecond (1 femtosecond is  1e-15 second), so 1e+12
    // is divided by TimerPeriod to produce the Feq in tick/ms.
    //
    *Frequency = DivU64x32 (1000000000000, (UINT32) TimerPeriod);
  }

  return Status;
}


/**
 Directly set wakeup time into CMOS.

 @param[in] WakeupTime     Pointer to wakeup time
 @param[in] Global         For global use inside this module

 @retval EFI_SUCCESS       Success to set wakeup time into CMOS
 @retval Others            RTC in update process
**/
EFI_STATUS
SetWakeupTime (
  IN EFI_TIME               *WakeupTime,
  IN PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS                Status;
  RTC_REGISTER_B            RegisterB;
  UINT8                     Century;
  EFI_TIME                  RtcTime;

  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }

  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    if (!EfiAtRuntime ()) {
      EfiReleaseLock (&Global->RtcLock);
    }
    return Status;
  }

  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);

  CopyMem (&RtcTime, WakeupTime, sizeof (EFI_TIME));
  ConvertEfiTimeToRtcTime (&RtcTime, RegisterB, &Century);

  RtcWrite (RTC_ADDRESS_SECONDS_ALARM, RtcTime.Second);
  RtcWrite (RTC_ADDRESS_MINUTES_ALARM, RtcTime.Minute);
  RtcWrite (RTC_ADDRESS_HOURS_ALARM  , RtcTime.Hour);
  WriteCmos8 (CmosDayWakeUp          , RtcTime.Day);
  WriteCmos8 (CmosMonthWakeUp        , RtcTime.Month);
  WriteCmos8 (CmosYearWakeUp         , (UINT8) RtcTime.Year);
  WriteCmos16 (CmosTimeZoneWakeUp    , RtcTime.TimeZone);
  WriteCmos8 (CmosDaylightWakeUp     , RtcTime.Daylight);

  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  return EFI_SUCCESS;
}


/**
 Check wakeup time is valid or not. If wakeup time is not valid, set to default time.

 @param[in] Global         For global use inside this module

 @retval EFI_SUCCESS       Success to check wakeup time
 @retval EFI_DEVICE_ERROR  Read wakeup time fail by RTC updating process interrupt
 @retval Others            RTC in update process or set wakeup time fail
**/
EFI_STATUS
CheckWakeupTime (
  IN PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS                Status;
  RTC_REGISTER_A            RegisterA;
  RTC_REGISTER_B            RegisterB;
  EFI_TIME                  WakeupTime;
  UINT8                     Century;
  UINTN                     Retry;
  UINTN                     Index;
  UINTN                     RtcComponentNum;
  RTC_COMPONENT             RtcComponent[] = {{CmosYearWakeUp            , 0},
                                              {CmosMonthWakeUp           , 0},
                                              {CmosDayWakeUp             , 0},
                                              {RTC_ADDRESS_HOURS_ALARM   , 0},
                                              {RTC_ADDRESS_MINUTES_ALARM , 0},
                                              {RTC_ADDRESS_SECONDS_ALARM , 0},
                                              {CmosDaylightWakeUp        , 0},
                                              {RTC_ADDRESS_CENTURY       , 0}
                                              };

  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }

  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    if (!EfiAtRuntime ()) {
      EfiReleaseLock (&Global->RtcLock);
    }
    return Status;
  }

  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);

  //
  // Read wakeup time with checking RTC update status
  //
  RtcComponentNum = sizeof(RtcComponent) / sizeof(RTC_COMPONENT);

  for (Retry = 0; Retry < MAX_RETRY; Retry++) {
    for (Index = 0; Index < RtcComponentNum; Index++) {
      RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
      if (RegisterA.Bits.Uip == 1) {
        MicroSecondDelay (10);
        break;
      }

      RtcComponent[Index].Data = RtcRead (RtcComponent[Index].Address);
    }

    if (Index < RtcComponentNum) {
      continue;
    }

    RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
    if (RegisterA.Bits.Uip == 1) {
      MicroSecondDelay (10);
      continue;
    }
    WakeupTime.TimeZone = (INT16) (ReadCmos16 (CmosTimeZoneWakeUp));

    break;
  }

  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  if (Retry == MAX_RETRY) {
    return EFI_DEVICE_ERROR;
  }

  WakeupTime.Year     = (UINT16) RtcComponent[0].Data;
  WakeupTime.Month    = RtcComponent[1].Data;
  WakeupTime.Day      = RtcComponent[2].Data;
  WakeupTime.Hour     = RtcComponent[3].Data;
  WakeupTime.Minute   = RtcComponent[4].Data;
  WakeupTime.Second   = RtcComponent[5].Data;
  WakeupTime.Daylight = RtcComponent[6].Data;
  Century             = RtcComponent[7].Data;

  ConvertRtcTimeToEfiTime (&WakeupTime, Century, RegisterB);

  //
  // If wakeup time is invalid, set to default time.
  //
  Status = RtcTimeFieldsValid (&WakeupTime);
  if (EFI_ERROR (Status) || !IsValidYear (&WakeupTime)) {
    WakeupTime.Second   = PcdGet8 (PcdRealTimeClockInitSecond);;
    WakeupTime.Minute   = PcdGet8 (PcdRealTimeClockInitMinute);
    WakeupTime.Hour     = PcdGet8 (PcdRealTimeClockInitHour);
    WakeupTime.Day      = PcdGet8 (PcdRealTimeClockInitDay);
    WakeupTime.Month    = PcdGet8 (PcdRealTimeClockInitMonth);
    WakeupTime.Year     = PcdGet16 (PcdRealTimeClockInitYear);
    WakeupTime.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    WakeupTime.Daylight = 0;

    Status = SetWakeupTime (&WakeupTime, Global);
  }

  return Status;
}

/**
 Set RTC time in BCD format

 @param[in] Global         For global use inside this module

 @retval EFI_SUCCESS       Success to set time in BCD format
 @retval Others            Get or set time fail
**/
EFI_STATUS
SetTimeInBcdFormat (
  IN PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS                Status;
  EFI_TIME                  Time;
  EFI_TIME                  WakeupTime;
  BOOLEAN                   Enabled;
  BOOLEAN                   Pending;
  RTC_REGISTER_B            RegisterB;

  Status = PcRtcGetTime (&Time, NULL, Global);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PcRtcGetWakeupTime (&Enabled, &Pending, &WakeupTime, Global);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }
  RegisterB.Data    = RtcRead (RTC_ADDRESS_REGISTER_B);
  RegisterB.Bits.Dm = 0;
  RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);
  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  Status = PcRtcSetTime (&Time, Global);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SetWakeupTime (&WakeupTime, Global);

  return Status;
}


/**
 Initialize RTC.

 @param[in] Global         For global use inside this module.

 @retval EFI_SUCCESS       Initialization successful.
 @retval EFI_DEVICE_ERROR  Initialization failed due to device error.
**/
EFI_STATUS
PcRtcInit (
  IN PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS      Status;
  RTC_REGISTER_A  RegisterA;
  RTC_REGISTER_B  RegisterB;
  UINTN           Index;
  EFI_TIME        Time;
  EFI_BOOT_MODE   BootMode;
  UINT8           Weekday;
  UINTN           BufferSize;
  UINT8           Buffer;

  //
  // PostCode = 0x52, RTC Initial
  //
  POST_CODE (DXE_PCRTC_INIT);

  //
  // Acquire RTC Lock to make access to RTC atomic
  //
  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }

  //
  // Initialize RTC Register
  //
  BootMode = GetBootModeHob ();
  if ((BootMode == BOOT_WITH_DEFAULT_SETTINGS) || EFI_ERROR (ValidateCmosChecksum ())) {
    //
    // Set Register B as default value
    //
    RegisterB.Data = RTC_INIT_REGISTER_B;
    RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);
    //
    // Calculate the CMOS checksum and write it
    //
    WriteCmosChecksum ();
  }

  //
  // Make sure Division Chain is properly configured,
  // or RTC clock won't "tick" -- time won't increment
  //
  RegisterA.Data = RTC_INIT_REGISTER_A;
  RtcWrite (RTC_ADDRESS_REGISTER_A, RegisterA.Data);

  //
  // Clear RTC flag register
  //
  RtcRead (RTC_ADDRESS_REGISTER_C);

  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    if (!EfiAtRuntime ()) {
      EfiReleaseLock (&Global->RtcLock);
    }
    return EFI_DEVICE_ERROR;
  }

  //
  // Get the Time/Date/Daylight Savings values.
  //
  //
  for (Index = 0; Index < MAX_RETRY; Index++) {
    Status = GetTimeWithRtcUpdateCheck (&Time);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = RtcTimeFieldsValid (&Time);
    if (EFI_ERROR (Status)) {
      continue;
    }

    break;
  }

  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);
  RegisterB.Data = RegisterB.Data | RTC_INIT_REGISTER_B;
  //
  // Set RTC configuration after get original time
  //
  RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  //
  // Release RTC Lock.
  //
  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  Status = RtcTimeFieldsValid (&Time);
  if (EFI_ERROR (Status) || !IsValidYear (&Time)) {
    //
    // If Time Error occur, write defalut time into CMOS
    //
    Time.Second = PcdGet8 (PcdRealTimeClockInitSecond);
    Time.Minute = PcdGet8 (PcdRealTimeClockInitMinute);
    Time.Hour   = PcdGet8 (PcdRealTimeClockInitHour);
    Time.Day    = PcdGet8 (PcdRealTimeClockInitDay);
    Time.Month  = PcdGet8 (PcdRealTimeClockInitMonth);
    Time.Year   = PcdGet16 (PcdRealTimeClockInitYear);
    Time.Nanosecond = 0;
    Time.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    Time.Daylight = 0;

    PcRtcSetTime (&Time, Global);
  }

  //
  // Update the Time zone and Daylight to the values of last SetTime function
  //
  if (Time.TimeZone != EFI_UNSPECIFIED_TIMEZONE) {
    BufferSize = sizeof (UINT8);
    Status = EfiGetVariable (
               TIMEZONE_UPDATED,
               &gEfiGenericVariableGuid,
               NULL,
               &BufferSize,
               &Buffer
               );
    if (EFI_ERROR (Status)) {
      //
      // Set the default values when the value not found in variable
      //
      Time.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
      Time.Daylight = 0;
      PcRtcSetTime (&Time, Global);
    }
  }

  //
  // Make sure weekday stored in CMOS is correct. If not correct, update it.
  //
  Status = GetWeekday (&Time, &Weekday);
  if (!EFI_ERROR (Status) && Weekday != RtcRead (RTC_ADDRESS_DAY_OF_THE_WEEK)) {
    PcRtcSetTime (&Time, Global);
  }

  CheckWakeupTime (Global);

  //
  // Always set time/date in BCD format because OS always assume time/date are BCD format.
  //
  if (RegisterB.Bits.Dm == 1) {
    SetTimeInBcdFormat (Global);
  }

  CopyMem (&mTime, &Time, sizeof(EFI_TIME));
  mRecordTick = AsmReadTsc ();

  return EFI_SUCCESS;
}

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
  OUT  EFI_TIME               *Time,
  OUT  EFI_TIME_CAPABILITIES  *Capabilities,  OPTIONAL
  IN   PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS      Status;
  UINTN           Index;
  UINT64          CurrentTick;
  UINT64          CpuFrequency;
  UINT32          ElapsedTimeInMs;

  //
  // Check parameters for null pointer
  //
  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;

  }

  if (!EfiAtRuntime ()) {
    Status = GetCpuFrequency (&CpuFrequency);
    if (!EFI_ERROR(Status)) {
      CurrentTick = AsmReadTsc ();
      ElapsedTimeInMs = (UINT32) DivU64x32 (CurrentTick - mRecordTick, (UINT32) (CpuFrequency));

      if (ElapsedTimeInMs < 300) {
        //
        // If within update period, use record time.
        //
        Time->Year   = mTime.Year;
        Time->Month  = mTime.Month;
        Time->Day    = mTime.Day;

        Time->Hour   = mTime.Hour;
        Time->Minute = mTime.Minute;
        Time->Second = mTime.Second;

        Time->TimeZone = mTime.TimeZone;
        Time->Daylight = mTime.Daylight;
        Time->Nanosecond = mTime.Nanosecond;

        if (Capabilities != NULL) {
          Capabilities->Resolution = 1;
          Capabilities->Accuracy   = 50000000;
          Capabilities->SetsToZero = FALSE;
        }
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Acquire RTC Lock to make access to RTC atomic
  //
  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    if (!EfiAtRuntime ()) {
      EfiReleaseLock (&Global->RtcLock);
    }
    return Status;
  }

  for (Index = 0; Index < MAX_RETRY; Index++) {
    //
    // Get the Time/Date/Daylight Savings values.
    //
    Status = GetTimeWithRtcUpdateCheck (Time);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = RtcTimeFieldsValid (Time);
    if (EFI_ERROR (Status)) {
      continue;
    }

    break;
  }
  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }
  if (Index == MAX_RETRY) {
    return EFI_DEVICE_ERROR;
  }

  //
  //  Fill in Capabilities if it was passed in
  //
  if (Capabilities != NULL) {
    Capabilities->Resolution = 1;
    //
    // 1 hertz
    //
    Capabilities->Accuracy = 50000000;
    //
    // 50 ppm
    //
    Capabilities->SetsToZero = FALSE;
  }

  if (!EfiAtRuntime ()) {
    CopyMem (&mTime, Time, sizeof(EFI_TIME));
    mRecordTick = AsmReadTsc ();
  }

  return EFI_SUCCESS;
}

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
  IN EFI_TIME                *Time,
  IN PC_RTC_MODULE_GLOBALS   *Global
  )
{
  EFI_STATUS      Status;
  EFI_TIME        RtcTime;
  RTC_REGISTER_B  RegisterB;
  UINT8           Century;
  UINT8           Weekday;
  UINTN           BufferSize;
  UINT8           Buffer;

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Make sure that the time fields are valid
  //
  Status = RtcTimeFieldsValid (Time);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetWeekday (Time, &Weekday);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (&RtcTime, Time, sizeof (EFI_TIME));

  //
  // Acquire RTC Lock to make access to RTC atomic
  //
  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
     if (!EfiAtRuntime ()) {
       EfiReleaseLock (&Global->RtcLock);
     }
    return Status;
  }
  //
  // Read Register B, and inhibit updates of the RTC
  //
  RegisterB.Data      = RtcRead (RTC_ADDRESS_REGISTER_B);
  RegisterB.Bits.Set  = 1;
  RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  ConvertEfiTimeToRtcTime (&RtcTime, RegisterB, &Century);

  RtcWrite (RTC_ADDRESS_SECONDS, RtcTime.Second);
  RtcWrite (RTC_ADDRESS_MINUTES, RtcTime.Minute);
  RtcWrite (RTC_ADDRESS_HOURS, RtcTime.Hour);
  RtcWrite (RTC_ADDRESS_DAY_OF_THE_MONTH, RtcTime.Day);
  RtcWrite (RTC_ADDRESS_MONTH, RtcTime.Month);
  RtcWrite (RTC_ADDRESS_YEAR, (UINT8) RtcTime.Year);
  //
  // Set the variable that containts the TimeZone and Daylight fields
  //
  WriteCmos16 (CmosTimeZone, (UINT16) RtcTime.TimeZone);
  WriteCmos8 (CmosDaylight, (UINT8) RtcTime.Daylight);

  RtcWrite (RTC_ADDRESS_CENTURY, Century);
  RtcWrite (RTC_ADDRESS_DAY_OF_THE_WEEK, Weekday);

  //
  // Allow updates of the RTC registers
  //
  RegisterB.Bits.Set = 0;
  RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  //
  // Release RTC Lock.
  //
  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  //
  // Update the last Time zone and Daylight for use of GetTime service
  //
  if (Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE) {
    //
    // Set the variable to indicates the Timezone be set to the value other then EFI_UNSPECIFIED_TIMEZONE
    //
    BufferSize = sizeof (UINT8);
    Buffer     = 0x01;
  } else {
    //
    // Erase the variable to indicates the Timezone be set to EFI_UNSPECIFIED_TIMEZONE
    //
    BufferSize = 0;
  }
  EfiSetVariable (
    TIMEZONE_UPDATED,
    &gEfiGenericVariableGuid,
    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
    BufferSize,
    &Buffer
    );

  if (!EfiAtRuntime ()) {
    CopyMem (&mTime, Time, sizeof(EFI_TIME));
    mRecordTick = AsmReadTsc ();
  }

  return EFI_SUCCESS;
}

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
  OUT BOOLEAN                *Enabled,
  OUT BOOLEAN                *Pending,
  OUT EFI_TIME               *Time,
  IN  PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS      Status;
  RTC_REGISTER_B  RegisterB;
  RTC_REGISTER_C  RegisterC;
  UINT8           Century;

  //
  // Check parameters for null pointers
  //
  if ((Enabled == NULL) || (Pending == NULL) || (Time == NULL)) {
    return EFI_INVALID_PARAMETER;

  }
  //
  // Acquire RTC Lock to make access to RTC atomic
  //
  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
    }
    return EFI_DEVICE_ERROR;
  }
  //
  // Read Register B and Register C
  //
  RegisterB.Data  = RtcRead (RTC_ADDRESS_REGISTER_B);
  RegisterC.Data  = RtcRead (RTC_ADDRESS_REGISTER_C);

  //
  // Get the Time/Date/Daylight Savings values.
  //
  *Enabled = RegisterB.Bits.Aie;
  *Pending = RegisterC.Bits.Af;

  Time->Second   = RtcRead (RTC_ADDRESS_SECONDS_ALARM);
  Time->Minute   = RtcRead (RTC_ADDRESS_MINUTES_ALARM);
  Time->Hour     = RtcRead (RTC_ADDRESS_HOURS_ALARM);
  Time->Day      = ReadCmos8 (CmosDayWakeUp);
  Time->Month    = ReadCmos8 (CmosMonthWakeUp);
  Time->Year     = ReadCmos8 (CmosYearWakeUp);
  Time->TimeZone = (INT16) (ReadCmos16 (CmosTimeZoneWakeUp));
  Time->Daylight = ReadCmos8 (CmosDaylightWakeUp);
  Century = RtcRead (RTC_ADDRESS_CENTURY);

  ConvertRtcTimeToEfiTime (Time, Century, RegisterB);

  //
  // Check day of month alart again, because in SmmPlatform driver,
  // EnableAcpi SmmCallback will initialized it as not care to
  // guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  //
  if (!DayValid (Time)) {
    Time->Day = RtcRead (RTC_ADDRESS_DAY_OF_THE_MONTH);
    if (RegisterB.Bits.Dm == 0) {
      Time->Day = BcdToDecimal8 (Time->Day);
    }
  }

  //
  // Release RTC Lock.
  //
  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  //
  // Make sure all field values are in correct range
  //
  Status = RtcTimeFieldsValid (Time);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

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
  IN EFI_TIME               *Time,   OPTIONAL
  IN PC_RTC_MODULE_GLOBALS  *Global
  )
{
  EFI_STATUS            Status;
  EFI_TIME              RtcTime;
  RTC_REGISTER_B        RegisterB;
  UINT8                 Century;
  EFI_TIME_CAPABILITIES Capabilities;

  ZeroMem (&RtcTime, sizeof (RtcTime));

  if (Enable) {
    if (Time == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // Make sure that the time fields are valid
    //
    Status = RtcTimeFieldsValid (Time);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // Just support set alarm time within 24 hours
    //
    PcRtcGetTime (&RtcTime, &Capabilities, Global);
    Status = RtcTimeFieldsValid (&RtcTime);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    if (!IsWithinOneDay (&RtcTime, Time)) {
      return EFI_UNSUPPORTED;
    }
    //
    // Make a local copy of the time and date
    //
    CopyMem (&RtcTime, Time, sizeof (EFI_TIME));
  }

  //
  // Acquire RTC Lock to make access to RTC atomic
  //
  if (!EfiAtRuntime ()) {
    EfiAcquireLock (&Global->RtcLock);
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    if (!EfiAtRuntime ()) {
      EfiReleaseLock (&Global->RtcLock);
    }
    return EFI_DEVICE_ERROR;
  }
  //
  // Read Register B, and inhibit updates of the RTC
  //
  RegisterB.Data      = RtcRead (RTC_ADDRESS_REGISTER_B);

  RegisterB.Bits.Set  = 1;
  RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  if (Enable) {
    ConvertEfiTimeToRtcTime (&RtcTime, RegisterB, &Century);

    //
    // Set RTC alarm time
    //
    RtcWrite (RTC_ADDRESS_SECONDS_ALARM, RtcTime.Second);
    RtcWrite (RTC_ADDRESS_MINUTES_ALARM, RtcTime.Minute);
    RtcWrite (RTC_ADDRESS_HOURS_ALARM  , RtcTime.Hour);
    WriteCmos8 (CmosDayWakeUp, RtcTime.Day);
    WriteCmos8 (CmosMonthWakeUp, RtcTime.Month);
    WriteCmos8 (CmosYearWakeUp, (UINT8) RtcTime.Year);
    WriteCmos16 (CmosTimeZoneWakeUp, RtcTime.TimeZone);
    WriteCmos8 (CmosDaylightWakeUp, (UINT8) RtcTime.Daylight);
    RegisterB.Bits.Aie = 1;
  } else {
    RegisterB.Bits.Aie = 0;
  }

  //
  // Allow updates of the RTC registers
  //
  RegisterB.Bits.Set = 0;
  RtcWrite (RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  //
  // Release RTC Lock.
  //
  if (!EfiAtRuntime ()) {
    EfiReleaseLock (&Global->RtcLock);
  }

  return EFI_SUCCESS;
}

/**
 Checks an 8-bit BCD value, and converts to an 8-bit value if valid.

 This function checks the 8-bit BCD value specified by Value.
 If valid, the function converts it to an 8-bit value and returns it.
 Otherwise, return 0xff.

 @param[in] Value       The 8-bit BCD value to check and convert

 @return The 8-bit value converted. Or 0xff if Value is invalid.
**/
UINT8
CheckAndConvertBcd8ToDecimal8 (
  IN  UINT8  Value
  )
{
  if ((Value < 0xa0) && ((Value & 0xf) < 0xa)) {
    return BcdToDecimal8 (Value);
  }

  return 0xff;
}

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
  )
{
  BOOLEAN IsPM;

  if ((Time->Hour & 0x80) != 0) {
    IsPM = TRUE;
  } else {
    IsPM = FALSE;
  }

  Time->Hour = (UINT8) (Time->Hour & 0x7f);

  if (RegisterB.Bits.Dm == 0) {
    Time->Year    = CheckAndConvertBcd8ToDecimal8 ((UINT8) Time->Year);
    Time->Month   = CheckAndConvertBcd8ToDecimal8 (Time->Month);
    Time->Day     = CheckAndConvertBcd8ToDecimal8 (Time->Day);
    Time->Hour    = CheckAndConvertBcd8ToDecimal8 (Time->Hour);
    Time->Minute  = CheckAndConvertBcd8ToDecimal8 (Time->Minute);
    Time->Second  = CheckAndConvertBcd8ToDecimal8 (Time->Second);
  }
  Century = CheckAndConvertBcd8ToDecimal8 (Century);

  Time->Year = (UINT16) (Century * 100 + Time->Year);

  //
  // If time is in 12 hour format, convert it to 24 hour format
  //
  if (RegisterB.Bits.Mil == 0) {
    if (IsPM && Time->Hour < 12) {
      Time->Hour = (UINT8) (Time->Hour + 12);
    }

    if (!IsPM && Time->Hour == 12) {
      Time->Hour = 0;
    }
  }

  Time->Nanosecond  = 0;
}

/**
 Wait for a period for the RTC to be ready.

 @param[in] Timeout         Tell how long it should take to wait.

 @retval EFI_SUCCESS        RTC is updated and ready.
 @retval EFI_DEVICE_ERROR   RTC device error.
**/
EFI_STATUS
RtcWaitToUpdate (
  IN UINTN Timeout
  )
{
  RTC_REGISTER_A  RegisterA;
  RTC_REGISTER_D  RegisterD;

  //
  // See if the RTC is functioning correctly
  //
  RegisterD.Data = RtcRead (RTC_ADDRESS_REGISTER_D);

  if (RegisterD.Bits.Vrt == 0) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be ready.
  //
  Timeout         = (Timeout / 10) + 1;
  RegisterA.Data  = RtcRead (RTC_ADDRESS_REGISTER_A);
  while (RegisterA.Bits.Uip == 1 && Timeout > 0) {
    MicroSecondDelay (10);
    RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
    Timeout--;
  }

  RegisterD.Data = RtcRead (RTC_ADDRESS_REGISTER_D);
  if (Timeout == 0 || RegisterD.Bits.Vrt == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
 See if all fields of a variable of EFI_TIME type is correct.

 @param[in] Time                The time to be checked.

 @retval EFI_SUCCESS            Time is a valid EFI_TIME variable.
 @retval EFI_INVALID_PARAMETER  Some fields of Time are not correct.
**/
EFI_STATUS
RtcTimeFieldsValid (
  IN EFI_TIME *Time
  )
{
  if (Time->Year < 1900 || // Follow the definition of year range (1900~9999) from UEFI spec
      Time->Year > 9999 ||
      Time->Month < 1 ||
      Time->Month > 12 ||
      (!DayValid (Time)) ||
      Time->Hour > 23 ||
      Time->Minute > 59 ||
      Time->Second > 59 ||
      Time->Nanosecond > 999999999 ||
      (!(Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE || (Time->TimeZone >= -1440 && Time->TimeZone <= 1440))) ||
      ((Time->Daylight & (~(EFI_TIME_ADJUST_DAYLIGHT | EFI_TIME_IN_DAYLIGHT))) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
 See if field Day of an EFI_TIME is correct.

 @param[in] Time   Its Day field is to be checked.

 @retval TRUE      Day field of Time is correct.
 @retval FALSE     Day field of Time is NOT correct.
**/
BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  )
{
  UINT8  DayOfMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  //
  // The validity of Time->Month field should be checked before
  //
  ASSERT (Time->Month >=1);
  ASSERT (Time->Month <=12);
  if (Time->Day < 1 ||
      Time->Day > DayOfMonth[Time->Month - 1] ||
      (Time->Month == 2 && (!IsLeapYear (Time) && Time->Day > 28))
      ) {
    return FALSE;
  }

  return TRUE;
}

/**
 Check if it is a leap year.

 @param[in] Time   The time to be checked.

 @retval TRUE      It is a leap year.
 @retval FALSE     It is NOT a leap year.
**/
BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year % 4 == 0) {
    if (Time->Year % 100 == 0) {
      if (Time->Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

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
     OUT UINT8           *Century
  )
{
  BOOLEAN IsPM;

  IsPM = TRUE;
  //
  // Adjust hour field if RTC is in 12 hour mode
  //
  if (RegisterB.Bits.Mil == 0) {
    if (Time->Hour < 12) {
      IsPM = FALSE;
    }

    if (Time->Hour >= 13) {
      Time->Hour = (UINT8) (Time->Hour - 12);
    } else if (Time->Hour == 0) {
      Time->Hour = 12;
    }
  }
  //
  // Set the Time/Date/Daylight Savings values.
  //
  *Century    = DecimalToBcd8 ((UINT8) (Time->Year / 100));

  Time->Year  = (UINT16) (Time->Year % 100);

  if (RegisterB.Bits.Dm == 0) {
    Time->Year    = DecimalToBcd8 ((UINT8) Time->Year);
    Time->Month   = DecimalToBcd8 (Time->Month);
    Time->Day     = DecimalToBcd8 (Time->Day);
    Time->Hour    = DecimalToBcd8 (Time->Hour);
    Time->Minute  = DecimalToBcd8 (Time->Minute);
    Time->Second  = DecimalToBcd8 (Time->Second);
  }
  //
  // If we are in 12 hour mode and PM is set, then set bit 7 of the Hour field.
  //
  if (RegisterB.Bits.Mil == 0 && IsPM) {
    Time->Hour = (UINT8) (Time->Hour | 0x80);
  }
}

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
  )
{
  if ((From->Hour > To->Hour) ||
     ((From->Hour == To->Hour) && (From->Minute > To->Minute)) ||
     ((From->Hour == To->Hour) && (From->Minute == To->Minute) && (From->Second > To->Second))) {
    return 1;
  } else if ((From->Hour == To->Hour) && (From->Minute == To->Minute) && (From->Second == To->Second)) {
    return 0;
  } else {
    return -1;
  }
}

/**
 To check if second date is later than first date within 24 hours.

 @param[in] From   the first date
 @param[in] To     the second date

 @retval TRUE      From is previous to To within 24 hours.
 @retval FALSE     From is later, or it is previous to To more than 24 hours.
**/
BOOLEAN
IsWithinOneDay (
  IN EFI_TIME  *From,
  IN EFI_TIME  *To
  )
{
  UINT8   DayOfMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  BOOLEAN Adjacent;

  Adjacent = FALSE;

  //
  // The validity of From->Month field should be checked before
  //
  ASSERT (From->Month >=1);
  ASSERT (From->Month <=12);

  if (From->Year == To->Year) {
    if (From->Month == To->Month) {
      if ((From->Day + 1) == To->Day) {
        if ((CompareHMS(From, To) >= 0)) {
          Adjacent = TRUE;
        }
      } else if (From->Day == To->Day) {
        if ((CompareHMS(From, To) <= 0)) {
          Adjacent = TRUE;
        }
      }
    } else if (((From->Month + 1) == To->Month) && (To->Day == 1)) {
      if ((From->Month == 2) && !IsLeapYear(From)) {
        if (From->Day == 28) {
          if ((CompareHMS(From, To) >= 0)) {
            Adjacent = TRUE;
          }
        }
      } else if (From->Day == DayOfMonth[From->Month - 1]) {
        if ((CompareHMS(From, To) >= 0)) {
           Adjacent = TRUE;
        }
      }
    }
  } else if (((From->Year + 1) == To->Year) &&
             (From->Month == 12) &&
             (From->Day   == 31) &&
             (To->Month   == 1)  &&
             (To->Day     == 1)) {
    if ((CompareHMS(From, To) >= 0)) {
      Adjacent = TRUE;
    }
  }

  return Adjacent;
}

