/** @file

  Main implementation source file for the Isct SMM driver
  
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
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

#include "IsctSmm.h"
#include "PciAccess.h"

/**
  Read specific RTC/CMOS RAM

  @param[in]  Location        Point to RTC/CMOS RAM offset for read

  @retval   The data of specific location in RTC/CMOS RAM.
**/
UINT8
RtcRead (
  IN      UINT8        Location
  )
{
  UINT8 RtcIndexPort;
  UINT8 RtcDataPort;

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

  IoWrite8 (RtcIndexPort, Location & RTC_INDEX_MASK);
  return IoRead8 (RtcDataPort);
}

/**
  Check if RTC Alarm has been enabled.

  @param[in]  None

  @retval  TRUE      RTC Alarm is enabled
  @retval  FALSE     RTC Alarm is not enabled
**/
BOOLEAN
RtcIsAlarmEnabled (
  VOID
  )
{
  return (RtcRead (R_PCH_RTC_REGB) & B_PCH_RTC_REGB_AIE) != 0;
}

/**  
  Wait for updating RTC process finished.
  
  @param[in]  None

  @retval  None
**/
STATIC
VOID
RtcWaitEndOfUpdate (
  VOID
  )
{
  while (RtcRead (R_PCH_RTC_REGA) & B_PCH_RTC_REGA_UIP) {
  }
}

/**  
  Get current RTC Alarm time.
  
  @param[in]  tm  A structure which will be updated with current RTC Alarm time
  
  @retval  EFI_NOT_STARTED      RTC Alarm has not been enabled yet.
  @retval  EFI_SUCCESS          RTC Alarm enabled and RTC_TIME structure contain current Alarm time setting.

**/
EFI_STATUS
RtcGetAlarm (
  OUT      RTC_TIME        *tm
  )
{
  ASSERT (tm != NULL);
  if (!RtcIsAlarmEnabled ()) {
    return EFI_NOT_STARTED;
  }

  RtcWaitEndOfUpdate ();
  tm->Second  = BcdToDecimal8 (RtcRead (R_PCH_RTC_ALARM_SECOND));
  tm->Minute  = BcdToDecimal8 (RtcRead (R_PCH_RTC_ALARM_MINUTE));
  tm->Hour    = BcdToDecimal8 (RtcRead (R_PCH_RTC_ALARM_HOUR));
  tm->Date    = BcdToDecimal8 (RtcRead (R_PCH_RTC_REGD) & 0x3F);
  tm->Month   = 0;
  tm->Year    = 0;
  return EFI_SUCCESS;
}

/**  
  Write specific RTC/CMOS RAM
  
  @param[in]  Location       Point to RTC/CMOS RAM offset for write
  @param[in]  Value           The data that will be written to RTC/CMOS RAM
  
  @retval  None
**/
VOID
RtcWrite (
  IN      UINT8        Location,
  IN      UINT8        Value
  )
{
  UINT8 RtcIndexPort;
  UINT8 RtcDataPort;

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

  IoWrite8 (RtcIndexPort, Location & RTC_INDEX_MASK);
  IoWrite8 (RtcDataPort, Value);
}

/**  
  Set RTC Alarm with specific time
  
  @param[in]  tm  A time interval structure which will be used to setup an RTC Alarm
  
  @retval  EFI_SUCCESS    RTC Alarm has been enabled with specific time interval
**/
EFI_STATUS
RtcSetAlarm (
  IN      RTC_TIME        *tm
  )
{
  UINT8 RegB;

  ASSERT (tm != NULL);
  //EFI_DEADLOOP();
  RegB = RtcRead (R_PCH_RTC_REGB);

  RtcWaitEndOfUpdate ();

  //
  // Inhibit update cycle
  //
  RtcWrite (R_PCH_RTC_REGB, RegB | B_PCH_RTC_REGB_SET);

  RtcWrite (R_PCH_RTC_ALARM_SECOND, DecimalToBcd8 (tm->Second));
  RtcWrite (R_PCH_RTC_ALARM_MINUTE, DecimalToBcd8 (tm->Minute));
  RtcWrite (R_PCH_RTC_ALARM_HOUR, DecimalToBcd8 (tm->Hour));
  RtcWrite (R_PCH_RTC_REGD, DecimalToBcd8 (tm->Date));

  //
  // Allow update cycle and enable wake alarm
  //
  RegB &= ~B_PCH_RTC_REGB_SET;
  RtcWrite (R_PCH_RTC_REGB, RegB | B_PCH_RTC_REGB_AIE);

  return EFI_SUCCESS;
}

/**  
  Get current RTC time
  
  @param[in]   tm  RTC time structure including Second, Minute and Hour, Date, Month, Year.
  
  @retval  EFI_SUCCESS   Operation successfully and RTC_TIME structure contained current time.
**/
EFI_STATUS
RtcGetTime (
  OUT      RTC_TIME        *tm
  )
{
  ASSERT (tm != NULL);
  RtcWaitEndOfUpdate ();
  tm->Second  = BcdToDecimal8 (RtcRead (R_PCH_RTC_SECOND));
  tm->Minute  = BcdToDecimal8 (RtcRead (R_PCH_RTC_MINUTE));
  tm->Hour    = BcdToDecimal8 (RtcRead (R_PCH_RTC_HOUR));
  tm->Date    = BcdToDecimal8 (RtcRead (R_PCH_RTC_DAY_OF_MONTH));
  tm->Month   = BcdToDecimal8 (RtcRead (R_PCH_RTC_MONTH));
  tm->Year    = (UINT16)BcdToDecimal8 (RtcRead (R_PCH_RTC_YEAR)) + 2000;
  return EFI_SUCCESS;
}

/**  
  Convert RTC_TIME structure data to seconds
  
  @param[in]   tm  A time data structure including second, minute and hour fields.
  
  @retval  A number of seconds converted from given RTC_TIME structure data.
**/
STATIC
UINT32
TimeToSeconds (
  IN      RTC_TIME        *tm
  )
{
  ASSERT (tm->Hour < 24);
  ASSERT (tm->Minute < 60);
  ASSERT (tm->Second < 60);
  return ((tm->Hour * 60) + tm->Minute) * 60 + tm->Second;
}

/**  
  Convert seconds to RTC_TIME structure data
  
  @param[in]  tm          A time data structure which will be updated with converted value.
  @param[in]  Seconds  Total seconds that will be converted into RTC_TIME
  
  @retval  None
**/
STATIC
VOID
SecondsToTime (
  OUT      RTC_TIME        *tm,
  IN       UINT32          Seconds
  )
{
  tm->Second = Seconds % 60;
  Seconds /= 60;
  tm->Minute = Seconds % 60;
  Seconds /= 60;
  tm->Hour = Seconds % 24;
  tm->Date = 0;
}

/**  
  Check if it is leap year
  
  @param[in]  Year  year to be check
  
  @retval  True    year is leap year
  @retval  FALSE   year is not a leap year
**/
BOOLEAN
IsLeapYear (
 IN UINT16 Year
) 
{
  return (Year%4 == 0) && ((Year%100 != 0) || (Year%400 == 0));
}

/**  
  Get days of the month
  
  @param[in]  Year             Year number
  @param[in]  Month           Month number, January is 1, Feburary is 2, ... December is 12.
  
  @retval  Days  Number of day of the Month of the Year
**/
UINT8
DaysOfMonth (
  IN UINT16 Year,
  IN UINT8  Month
)
{
  UINT8 Days;
  if (Month < 1 || Month > 12) {
    return 0;
  }
  Days = mDaysOfMonthInfo[Month-1];
  if (Month == 2) {
    Days += IsLeapYear(Year);
  }
  return (Days);
}

/**  
  check if tm2 is after 2 days of tm1
  
  @param[in]  tm1   First time to compare
  @param[in]  tm2   Second time to compare
  
  @retval  True  tm2 is 2 days after tm1
  @retval  FALSE tm2 is not 2 days after tm1
**/
BOOLEAN
IsOver2Days (
  IN      RTC_TIME        *tm1,
  IN      RTC_TIME        *tm2
)
{
  BOOLEAN RetVal;
  RetVal = TRUE;
  if (tm2->Date > tm1->Date) {
    if (tm2->Date - tm1->Date == 1) {
      RetVal = FALSE;; 
    }        
  } else if ((DaysOfMonth (tm1->Year, tm1->Month) == tm1->Date) && (tm2->Date == 1)) {
    RetVal = FALSE;; 
  }
  return RetVal;
}

/**  
  ISCT S3 entry callback SMI handler
  
  @param[in]  DispatchHandle  - The handle of this callback, obtained when registering
  @param[in]  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
  
  @retval   EFI_SUCCESS - Function executed successfully
**/
STATIC
EFI_STATUS
EFIAPI
IsctS3EntryCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  EFI_STATUS  Status;
  RTC_TIME    rtc_tm;
  RTC_TIME    wake_tm;  
  RTC_TIME    alarm_tm;
  UINT32      IsctDuration;
  UINT32      CurrentTime;
  UINT32      AlarmTime; 
  UINT32      WakeTime;
  UINT16      PmBase; 
  UINT8       RegB;
  BOOLEAN     UseIsctTimer;
  BOOLEAN     Over2Days;  
  
  //
  // Check iFFS exist. If iFFS exist, iFFS will handler RTC comparing.
  //
  if (mIsctNvs->RapidStartEnabled == 1 || mIsctNvs->RtcDurationTime == 0) {  
    mIsctNvs->IsctOverWrite = 0;
    return EFI_SUCCESS;
  } 
  UseIsctTimer = FALSE;
  Over2Days = FALSE;
  mIsctNvs->IsctOverWrite = 0;
  IsctDuration = mIsctNvs->RtcDurationTime;

  //
  // Make sure RTC is in BCD and 24h format
  //
  RegB = RtcRead (R_PCH_RTC_REGB);
  RegB |= B_PCH_RTC_REGB_HOURFORM;
  RegB &= ~B_PCH_RTC_REGB_DM;
  RtcWrite (R_PCH_RTC_REGB, RegB);

  //
  // Get RTC Timer and convert RTC_TIME to seconds
  //
  Status = RtcGetTime (&rtc_tm);
  ASSERT_EFI_ERROR (Status);

  CurrentTime = TimeToSeconds (&rtc_tm);
  Status = RtcGetAlarm (&wake_tm);
  if (Status == EFI_SUCCESS) {
    AlarmTime = TimeToSeconds (&wake_tm);
    //
    // When OS set alarm date to zero, 
    // that would mean the alarm date is today or next day depending alarm time, 
    // and the alarm will happen in 24 hour.
    if (wake_tm.Date != 0 && wake_tm.Date != rtc_tm.Date) {
      //
      // OS Wake-up time is over 1 day
      //
      AlarmTime += DAY_IN_SEC;
      if (IsOver2Days (&rtc_tm, &wake_tm)) {
        //
        // OS Wake-up time is over 2 day
        //
        UseIsctTimer = TRUE;
        Over2Days = TRUE;
      }
    } else if (AlarmTime < CurrentTime && wake_tm.Date == 0) {
      //
      // When alarm time behind current time and alarm date is zero, 
      // OS set the alarm for next day
      //
      AlarmTime += DAY_IN_SEC;    
    }
   
    if ((IsctDuration <= (AlarmTime - CurrentTime)) && (Over2Days == FALSE)) {
      UseIsctTimer = TRUE;
    }
  } else {
    UseIsctTimer = TRUE;
  }
  
  //
  // if ISCT Timer <= OS RTC alarm timer, then overwrite RTC alarm by ISCT timer
  //
  if (UseIsctTimer == TRUE) {
    WakeTime = CurrentTime + IsctDuration;
    SecondsToTime (&alarm_tm, WakeTime);  
    Status = RtcSetAlarm (&alarm_tm);
    ASSERT_EFI_ERROR (Status);

//[-start-130221-IB03600539-modify]//
     PmBase = (MmPci16(0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_ACPI_BASE)  & B_PCH_LPC_ACPI_BASE_BAR);
//[-end-130221-IB03600539-modify]//

    //
    // Clear RTC PM1 status
    //
    IoWrite16 (PmBase + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);

    //
    // set RTC_EN bit in PM1_EN to wake up from the alarm
    //
    IoWrite16 (
      PmBase + R_PCH_ACPI_PM1_EN,
      (IoRead16 (PmBase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
      );
    
      mIsctNvs->IsctOverWrite = 1;  
  }
  return EFI_SUCCESS;
}

/**  
  Isct SMM driver entry point function.
  
  @param[in]  ImageHandle   - image handle for this driver image
  @param[in]  SystemTable   - pointer to the EFI System Table
  
  @retval   EFI_SUCCESS   - driver initialization completed successfully
**/
EFI_STATUS
EFIAPI
InstallIsctSmm (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DispatchHandle;
  EFI_SMM_SX_DISPATCH_PROTOCOL      *SxDispatchProtocol;
  EFI_SMM_SX_DISPATCH_CONTEXT       EntryDispatchContext;
  ISCT_NVS_AREA_PROTOCOL            *IsctNvsAreaProtocol;  

  DEBUG ((EFI_D_INFO, "IsctSmm Entry Point- Install\n"));

  //
  // Located ISCT Nvs Protocol
  //
  Status = gBS->LocateProtocol (
                  &gIsctNvsAreaProtocolGuid,
                  NULL,
                  (VOID **)&IsctNvsAreaProtocol
                  );
  DEBUG((EFI_D_INFO, "(ISCT SMM) Located ISCT Nvs protocol Status = %x\n", Status));                    
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No ISCT Nvs protocol available\n"));
    return EFI_SUCCESS;
  }


  if (IsctNvsAreaProtocol->Area->IsctEnabled == 0) {
    DEBUG ((EFI_D_INFO, "ISCT is Disbaled \n"));
    return EFI_SUCCESS;
  }

  //
  // Assign NvsPointer to Global Module Variable
  //
  mIsctData = IsctNvsAreaProtocol->IsctData;
  mIsctNvs = IsctNvsAreaProtocol->Area;

  //
  // Loacted SxDispatchProtocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmSxDispatchProtocolGuid,
                  NULL,
                  (VOID **)&SxDispatchProtocol
                  );
  DEBUG((EFI_D_INFO, "(ISCT SMM) Located SxDispatchProtocol protocol Status = %x\n", Status));                    
  ASSERT_EFI_ERROR (Status);

  //
  // Register S3 entry phase call back function
  //
  EntryDispatchContext.Type   = SxS3;
  EntryDispatchContext.Phase  = SxEntry;
  Status = SxDispatchProtocol->Register (
                                SxDispatchProtocol,
                                IsctS3EntryCallback,
                                &EntryDispatchContext,
                                &DispatchHandle
                                );
  DEBUG((EFI_D_INFO, "(ISCT SMM) Register IsctS3EntryCallback Status = %x\n", Status));                    
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_INFO, "IsctS3EntryCallback failed to load.\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  return EFI_SUCCESS;
}
