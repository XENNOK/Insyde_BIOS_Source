/** @file
FAT FileTime functions

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++

Copyright (c) 2005 - 2007, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  FileTime.c

Abstract:

  Functions for manipulating file names

Revision History

--*/

#include "FatPeim.h"
#include "FileTime.h"

/**
  Checks an 8-bit BCD value, and converts to an 8-bit value if valid.

  This function checks the 8-bit BCD value specified by Value.
  If valid, the function converts it to an 8-bit value and returns it.
  Otherwise, return 0xff.

  @param   Value The 8-bit BCD value to check and convert

  @return  The 8-bit value converted. Or 0xff if Value is invalid.

**/
UINT8
PeiCheckAndConvertBcd8ToDecimal8 (
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

  This function converts raw time data read from RTC to the EFI_TIME format
  defined by UEFI spec.
  If data mode of RTC is BCD, then converts it to decimal,
  If RTC is in 12-hour format, then converts it to 24-hour format.

  @param   Time       On input, the time data read from RTC to convert
                      On output, the time converted to UEFI format
  @param   Century    Value of century read from RTC.
  @param   RegisterB  Value of Register B of RTC, indicating data mode
                      and hour format.

  @retval  EFI_INVALID_PARAMETER  Parameters passed in are invalid.
  @retval  EFI_SUCCESS            Convert RTC time to EFI time successfully.

**/
EFI_STATUS
PeiConvertRtcTimeToEfiTime (
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
    Time->Year    = PeiCheckAndConvertBcd8ToDecimal8 ((UINT8) Time->Year);
    Time->Month   = PeiCheckAndConvertBcd8ToDecimal8 (Time->Month);
    Time->Day     = PeiCheckAndConvertBcd8ToDecimal8 (Time->Day);
    Time->Hour    = PeiCheckAndConvertBcd8ToDecimal8 (Time->Hour);
    Time->Minute  = PeiCheckAndConvertBcd8ToDecimal8 (Time->Minute);
    Time->Second  = PeiCheckAndConvertBcd8ToDecimal8 (Time->Second);
  }
  Century       = PeiCheckAndConvertBcd8ToDecimal8 (Century);

  if (Time->Year == 0xff || Time->Month == 0xff || Time->Day == 0xff ||
      Time->Hour == 0xff || Time->Minute == 0xff || Time->Second == 0xff ||
      Century == 0xff) {
    return EFI_INVALID_PARAMETER;
  }

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

  return EFI_SUCCESS;
}

/**
 Get RTC time data. Before reading every RTC data, it will check RTC update status
 to make sure every RTC data is correct.

  @param[out]   Time                  the time converted to UEFI format
  @retval          EFI_SUCCESS      Get RTC time to EFI time complete.

**/
EFI_STATUS
PeiGetTimeWithRtcUpdateCheck(
  OUT EFI_TIME *Time
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
  
  RtcComponentNum = sizeof(RtcComponent) / sizeof(RTC_COMPONENT);
  RegisterB.Data  = ReadCmos8 (RTC_ADDRESS_REGISTER_B);
  
  //
  // In order to preventing RTC data reading error upon RTC updating due to interrupt/SMI caused delay,
  // making RTC update checking before every RTC data reading to make sure every RTC data corrected
  //
  for (Index = 0; Index < RtcComponentNum; Index++) {
    RegisterA.Data = ReadCmos8 (RTC_ADDRESS_REGISTER_A);
    if (RegisterA.Bits.Uip == 1) {
      MicroSecondDelay (DELAY_TIME);
      return EFI_NOT_READY;
    }    
    RtcComponent[Index].Data = ReadCmos8 (RtcComponent[Index].Address);
  } 

  Time->Year     = (UINT16) RtcComponent[0].Data;
  Time->Month    = RtcComponent[1].Data;
  Time->Day      = RtcComponent[2].Data;
  Time->Hour     = RtcComponent[3].Data;
  Time->Minute   = RtcComponent[4].Data;
  Time->Second   = RtcComponent[5].Data;
  Time->TimeZone = (INT16) ReadCmos16 (CMOS_TIME_ZONE);
  Time->Daylight = ReadCmos8 (CMOS_DAYLIGHT);
  Century        = ReadCmos8 (RTC_ADDRESS_CENTURY);
  
  PeiConvertRtcTimeToEfiTime (Time, Century, RegisterB);
  
  return EFI_SUCCESS;
}

/**
 Translate EFI time to FAT time.

 @param[in]    ETime  The time of EFI_TIME.
 @param[out]  FTime  The time of FAT_DATE_TIME.

 @return  None.
**/
VOID
FatEfiTimeToFatTime (
  IN  EFI_TIME        *ETime,
  OUT FAT_DATE_TIME   *FTime
  )
{
  //
  // ignores timezone info in source ETime
  //
  if (ETime->Year > 1980) {
    FTime->Date.Year = (UINT16) (ETime->Year - 1980);
  }

  if (ETime->Year >= 1980 + FAT_MAX_YEAR_FROM_1980) {
    FTime->Date.Year = FAT_MAX_YEAR_FROM_1980;
  }

  FTime->Date.Month         = ETime->Month;
  FTime->Date.Day           = ETime->Day;
  FTime->Time.Hour          = ETime->Hour;
  FTime->Time.Minute        = ETime->Minute;
  FTime->Time.DoubleSecond  = (UINT16) (ETime->Second / 2);
}

/**
 Get Current FAT time.

 @param[in]    PEI_FAT_PRIVATE_DATA   Pointer to the PEI_FAT_PRIVATE_DATA structure
 @param[in]    FAT_DATE_TIME              The time of FAT_DATE_TIME.

 @return  None.
**/
VOID
FatGetCurrentFatTime (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData, 
  OUT FAT_DATE_TIME         *FatNow
  )
{
  EFI_STATUS Status = EFI_NOT_READY;
  EFI_TIME   Now;
  UINTN      Index;
  
  //
  // Get the Time/Date/Daylight Savings values.
  //
  for (Index = 0; Index < MAX_RETRY; Index++) {
    Status = PeiGetTimeWithRtcUpdateCheck(&Now);
    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  
  if (!EFI_ERROR (Status)) {
    FatEfiTimeToFatTime (&Now, FatNow);
  } else {
    //
    // Set default value.
    //
    ZeroMem (&Now, sizeof (EFI_TIME));
    Now.Year = 2014;
    Now.Month = 1;
    Now.Day = 1;
    FatEfiTimeToFatTime (&Now, FatNow);
  }

}

