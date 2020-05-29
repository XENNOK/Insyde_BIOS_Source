/** @file
  This SMM driver will install a S3 Entry callback SMI handler for RTC wakeup policy.

;******************************************************************************
;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Mobile Silicon Support Module" and is
  licensed for Intel Mobile CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/RapidStartCommonLib.h>
#include <Library/PcdLib.h>

#include <Protocol/SmmSxDispatch.h>
#include <Protocol/RapidStartGlobalNvsArea.h>

#include <Protocol/IsctNvsArea.h>
#include <Guid/RapidStartPersistentData.h>

#include <RapidStartData.h>
#include <RapidStartAhciReg.h>
#include <PchRegs.h>
#include <SaRegs.h>

#include "RapidStartSmm.h"

#define DAY_IN_SEC            (24 * 60 * 60)
#define RAPID_START_DELTA_SEC 45

RAPID_START_PERSISTENT_DATA *mRapidStartData;
RAPID_START_MEM_DATA        *mRapidStartMemData;
RAPID_START_GLOBAL_NVS_AREA *mRapidStartGlobalNvs;
UINT32                      *mRapidStartZeroPageBitMap;
UINT32                      *mCrc32Record;

ISCT_NVS_AREA  *mIsctNvs;

/**
  Convert RTC_TIME structure data to seconds

  @param[in] tm          A time data structure including second, minute and hour fields.

  @retval A number of seconds converted from given RTC_TIME structure data.
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
  Check if it is leap year

  @param[in] Year            year to be check

  @retval True    year is leap year
  @retval FALSE   year is not a leap year
**/
BOOLEAN
IsLeapYear (
  IN UINT16 Year
  )
{
  return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
}

STATIC UINT8  mDaysOfMonthInfo[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
  Check if it is leap year

  @param[in] Year            Year number
  @param[in] Month           Month number, January is 1, Feburary is 2, ... December is 12.

  @retval Days  Number of day of the Month of the Year
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

  Days = mDaysOfMonthInfo[Month - 1];
  if (Month == 2) {
    Days += IsLeapYear (Year);
  }

  return Days;
}

/**
  Convert seconds to RTC_TIME structure data

  @param[out] tm              A time data structure which will be updated with converted value.
  @param[in] Seconds          Total seconds that will be converted into RTC_TIME
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
  tm->Hour  = Seconds % 24;
  tm->Date  = 0;
}

/**
  Check if tm2 is after 2 days of tm1

  @param[in] tm1             First time to compare
  @param[in] tm2             Second time to compare

  @retval TRUE  tm2 is 2 days after tm1
  @retval FALSE tm2 is not 2 days after tm1
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
  Adjusts RTC Alarm setting according to RapidStart platform policy.

  @param[in] WakeAfter      Time offset in seconds to wake from S3

  @retval EFI_SUCCESS          Timer started successfully
  @retval EFI_ALREADY_STARTED  Timer already set sooner than desired
**/
STATIC
EFI_STATUS
RapidStartAdjustRtcAlarm (
  IN       UINT32          WakeAfter
  )
{
  UINT32      CurrentTime;
  UINT32      AlarmTime;
  UINT32      WakeTime;
  RTC_TIME    tm;
  RTC_TIME    wake_tm;
  EFI_STATUS  Status, RtcStatus;
  UINT16      PmBase;
  UINT32      IsctWakeAfter;
  RTC_TIME    Isct_wake_tm;

  Status = EFI_SUCCESS;

  ///
  /// For an instant wake 2 seconds is a safe value
  ///
  if (WakeAfter < 2) {
    WakeAfter = 2;
  }
  ///
  /// Make sure RTC is in BCD and 24h format
  ///
  RtcInit ();

  RtcStatus = RtcGetTime (&tm);
  ASSERT_EFI_ERROR (RtcStatus);

  CurrentTime = TimeToSeconds (&tm);

  IsctWakeAfter       = 0;
  Isct_wake_tm.Second = 0;
  Isct_wake_tm.Minute = 0;
  Isct_wake_tm.Hour   = 0;
  Isct_wake_tm.Date   = 0;
  Isct_wake_tm.Month  = 0;
  Isct_wake_tm.Year   = 0;

  if(FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) {
    //
    // Get Isct wake up time
    //
    if (mIsctNvs != NULL) {
      mIsctNvs->IsctOverWrite = 0;

      IsctWakeAfter = mIsctNvs->RtcDurationTime;
      if ((IsctWakeAfter != 0) && (IsctWakeAfter < 2)) {
        IsctWakeAfter = 2;
      }
    }

    SecondsToTime (&Isct_wake_tm, IsctWakeAfter + CurrentTime);
  }
  
  RtcStatus = RtcGetAlarm (&wake_tm);
  if (RtcStatus == EFI_SUCCESS && (!(mRapidStartMemData->EntryCanceled))) {
    AlarmTime = TimeToSeconds (&wake_tm);
    ///
    /// When OS set alarm date to zero,
    /// that would mean the alarm date is today or next day depending alarm time,
    /// and the alarm will happen in 24 hour.
    ///
    if (wake_tm.Date != 0 && wake_tm.Date != tm.Date) {
      ///
      /// OS Wake-up time is over 1 day
      ///
      AlarmTime += DAY_IN_SEC;
      if (IsOver2Days (&tm, &wake_tm)) {
        ///
        /// OS Wake-up time is over 2 day
        ///
        AlarmTime += DAY_IN_SEC;
      }
    } else if (AlarmTime < CurrentTime && wake_tm.Date == 0) {
      ///
      /// When alarm time behind current time and alarm date is zero,
      /// OS set the alarm for next day
      //
      AlarmTime += DAY_IN_SEC;
    }
    ///
    /// OS RTC alarm set sooner than RapidStart entry after
    /// Add some seconds so RapidStart wake time won't overcome the OS timer
    ///
    if (FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) { 
      if ((WakeAfter + RAPID_START_DELTA_SEC >= AlarmTime - CurrentTime)
         && ((IsctWakeAfter >= AlarmTime - CurrentTime) || (IsctWakeAfter == 0))) {
        return EFI_ALREADY_STARTED;

      } 
    } else {
      if ((WakeAfter + RAPID_START_DELTA_SEC >= AlarmTime - CurrentTime)) {
         return EFI_ALREADY_STARTED;
      }
    }    
    ///
    /// Store OS wake-up time, so it can be restored upon entering RapidStart
    ///
    mRapidStartMemData->OsWakeTime        = wake_tm;
    mRapidStartMemData->OsWakeTimeEnabled = 1;
    if (FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) {
      ///
      /// ISCT time is smaller than OS's, so store ISCT wake-up time, so it can be restored upon entering RapidStart
      ///
      if ((IsctWakeAfter <= AlarmTime - CurrentTime) && (IsctWakeAfter != 0)) {
        mRapidStartMemData->OsWakeTime = Isct_wake_tm;
        mIsctNvs->IsctOverWrite = 1;
      }
    }
  }

  WakeTime = CurrentTime + WakeAfter;
  if (FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) {
    ///
    /// ISCT alarm set sooner than RapidStart entry after
    /// Add some seconds so RapidStart wake time won't overcome the ISCT timer
    ///
    if ((IsctWakeAfter <= WakeAfter + RAPID_START_DELTA_SEC) && (IsctWakeAfter != 0)) {
      WakeTime                = CurrentTime + IsctWakeAfter;
      mIsctNvs->IsctOverWrite = 1;
      Status                  = EFI_NOT_STARTED;
    }

    ///
    /// OS hasn't set up the wake-up time, so store ISCT wake-up time then it can be restored upon entering RapidStart
    ///
    if (mRapidStartMemData->OsWakeTimeEnabled == 0 && IsctWakeAfter != 0) {
      mRapidStartMemData->OsWakeTime        = Isct_wake_tm;
      mRapidStartMemData->OsWakeTimeEnabled = 1;
      mIsctNvs->IsctOverWrite               = 1;
    }
  }

  SecondsToTime (&tm, WakeTime);

  RtcStatus = RtcSetAlarm (&tm);
  ASSERT_EFI_ERROR (RtcStatus);

  PmBase = (UINT16) (PciRead32 (
                      PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      R_PCH_LPC_ACPI_BASE)
                      ) & B_PCH_LPC_ACPI_BASE_BAR);

  ///
  /// Clear RTC PM1 status
  ///
  IoWrite16 (PmBase + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);

  ///
  /// set RTC_EN bit in PM1_EN to wake up from the alarm
  ///
  IoWrite16 (
    PmBase + R_PCH_ACPI_PM1_EN,
    (IoRead16 (PmBase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
    );
  return Status;
}

/**
  Adjusts RTC Alarm setting according to Isct.

  @retval EFI_SUCCESS          Timer started successfully
  @retval EFI_ALREADY_STARTED  Timer already set sooner than desired
**/
STATIC
EFI_STATUS
IsctAdjustRtcAlarm (
  )
{
  UINT32      CurrentTime;
  UINT32      AlarmTime;
  UINT32      WakeTime;
  RTC_TIME    tm;
  RTC_TIME    wake_tm;
  EFI_STATUS  Status, RtcStatus;
  UINT16      PmBase;
  UINT32      IsctWakeAfter;
  RTC_TIME    Isct_wake_tm;

  Status = EFI_SUCCESS;

  DEBUG((EFI_D_INFO, "IsctAdjustRtcAlarm() Begin.\n"));

  ///
  /// Make sure RTC is in BCD and 24h format
  ///
  RtcInit ();

  RtcStatus = RtcGetTime (&tm);
  ASSERT_EFI_ERROR (RtcStatus);

  CurrentTime = TimeToSeconds (&tm);

  IsctWakeAfter       = 0;
  Isct_wake_tm.Second = 0;
  Isct_wake_tm.Minute = 0;
  Isct_wake_tm.Hour   = 0;
  Isct_wake_tm.Date   = 0;
  Isct_wake_tm.Month  = 0;
  Isct_wake_tm.Year   = 0;

  if (mIsctNvs == NULL) {
    return EFI_ABORTED;
  }

  mIsctNvs->IsctOverWrite = 0;
  
  //
  // Get Isct wake up time
  //
  IsctWakeAfter = mIsctNvs->RtcDurationTime;
  if ((IsctWakeAfter != 0) && (IsctWakeAfter < 2)) {
    IsctWakeAfter = 2;
  }

  SecondsToTime (&Isct_wake_tm, IsctWakeAfter + CurrentTime);

  RtcStatus = RtcGetAlarm (&wake_tm);
  if (RtcStatus == EFI_SUCCESS) {
    AlarmTime = TimeToSeconds (&wake_tm);
    ///
    /// When OS set alarm date to zero,
    /// that would mean the alarm date is today or next day depending alarm time,
    /// and the alarm will happen in 24 hour.
    ///
    if (wake_tm.Date != 0 && wake_tm.Date != tm.Date) {
      ///
      /// OS Wake-up time is over 1 day
      ///
      AlarmTime += DAY_IN_SEC;
      if (IsOver2Days (&tm, &wake_tm)) {
        ///
        /// OS Wake-up time is over 2 day
        ///
        AlarmTime += DAY_IN_SEC;
      }
    } else if (AlarmTime < CurrentTime && wake_tm.Date == 0) {
      ///
      /// When alarm time behind current time and alarm date is zero,
      /// OS set the alarm for next day
      //
      AlarmTime += DAY_IN_SEC;
    }
    ///
    /// OS RTC alarm set sooner than Isct alarm
    ///
    if ((IsctWakeAfter >= AlarmTime - CurrentTime) || (IsctWakeAfter == 0)) {
      return EFI_ALREADY_STARTED;
    }
  }

  mIsctNvs->IsctOverWrite = 1;

  WakeTime                = CurrentTime + IsctWakeAfter;
  Status                  = EFI_NOT_STARTED;

  SecondsToTime (&tm, WakeTime);

  RtcStatus = RtcSetAlarm (&tm);
  ASSERT_EFI_ERROR (RtcStatus);

  PmBase = (UINT16) (PciRead32 (
                      PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      R_PCH_LPC_ACPI_BASE)
                      ) & B_PCH_LPC_ACPI_BASE_BAR);

  ///
  /// Clear RTC PM1 status
  ///
  IoWrite16 (PmBase + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);

  ///
  /// set RTC_EN bit in PM1_EN to wake up from the alarm
  ///
  IoWrite16 (
    PmBase + R_PCH_ACPI_PM1_EN,
    (IoRead16 (PmBase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
    );

  DEBUG((EFI_D_INFO, "IsctAdjustRtcAlarm() End.\n"));

  return Status;
}

/**
  Scan Zero page within specific memory range.

  @param[in] BaseAddress      Start address of memory
  @param[in] EndAddress       End address of memory

  @retval SaveSize         Save Size requirement
**/
STATIC
UINTN
ScanZeroPageByRange (
  IN       UINT64          BaseAddress,
  IN       UINT64          EndAddress
  )
{
  UINT64  NonZeroPointer;
  UINT32  TempBitMap;
  UINTN   count;
  UINT32  *ZeroPagePointer;
  UINTN   SaveSize;

  count               = 0;
  TempBitMap          = 0;
  ZeroPagePointer     = mRapidStartZeroPageBitMap + 2 + (UINT32) NUMBER_OF_PAGES_IN_DWORD (BaseAddress);
  SaveSize            = 0;
  while (BaseAddress < EndAddress) {
    NonZeroPointer = (UINT64)ScanValueMem64 ((VOID*)BaseAddress, EFI_PAGE_SIZE/ sizeof (UINT64), 0);
    BaseAddress += EFI_PAGE_SIZE;
    NonZeroPointer++;
//[-start-120815-IB06460429-modify]//
//[-start-120418-IB06460388-modify]//
    if (NonZeroPointer == 0) {
      //
      // This is Zero Page so we set the corresponding bit in BitMap.
      //
      TempBitMap |= 1;
    } else {
      SaveSize += EFI_PAGE_SIZE;
    }
//[-end-120418-IB06460388-modify]//
//[-end-120815-IB06460429-modify]//

    TempBitMap = RRotU32 (TempBitMap, 1);
    count++;
    if (count == (sizeof (*mRapidStartZeroPageBitMap) * 8)) {
      *ZeroPagePointer = TempBitMap;
      count                     = 0;
      TempBitMap                = 0;
      ZeroPagePointer += 1;
      if ((UINTN) ((UINTN) ZeroPagePointer - (UINTN) (mRapidStartZeroPageBitMap + 2)) >= mRapidStartData->ZeroBitmapSize) {
        //
        // Zero page filter table is full, skip other checking.
        //
        SaveSize += (UINTN)(EndAddress - BaseAddress);
        break;
      }
    }

  }
  return SaveSize;
}

/**
  Create a bitmap for zero page filter.

  @retval Save Size requirement
**/
STATIC
UINTN
BuildZeroPageBitMap (
  VOID
  )
{
  UINT32  GfxGTTBase;
  UINTN   SaveSize;

  SaveSize = 0;
  GfxGTTBase = (PciRead32 (
                  PCI_LIB_ADDRESS (SA_MC_BUS,
                  SA_MC_DEV,
                  SA_MC_FUN,
                  R_SA_BGSM)
                  ) & B_SA_BGSM_BGSM_MASK);

  ///
  /// Ensure all ZeroPageBitMap were zero-ed
  ///
  ZeroMem (mRapidStartZeroPageBitMap, mRapidStartData->ZeroBitmapSize);

#ifdef RAPID_START_SCAN_ZERO_PAGE
  ///
  /// Handle Below 4GB memory
  /// Skip GTT region
  ///
  SaveSize = ScanZeroPageByRange (0x0, GfxGTTBase);

#if defined(EFIX64)
  ///
  /// Handle Above 4GB memory
  ///
  if (mRapidStartData->Tohm > MEM_EQU_4GB) {
    SaveSize += ScanZeroPageByRange (MEM_EQU_4GB, mRapidStartData->Tohm);
  }
#endif
#endif
  return SaveSize;
}

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
/**
  Log or compare CRC32 value for above 4GB memory.

  @param[in] IsComparingCrc32 - FALSE to save CRC32 value into buffer. TRUE to compare CRC32 value with pre-saved value in buffer.
**/
VOID
SaveOrCompareCrc32ForAbove4GB (
  IN  BOOLEAN          IsComparingCrc32
  )
{
#if defined(EFIX64)
  if (mRapidStartData->Tohm > MEM_EQU_4GB) {
    SaveOrCompareCrc32 (IsComparingCrc32, MEM_EQU_4GB, mRapidStartData->Tohm, mRapidStartData);
  }
#endif
}

/**
  A SW SMI callback to check whole memory CRC32

  @param[in] DispatchHandle  - The handle of this callback, obtained when registering
  @param[in] DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
**/
VOID
RapidStartSwSmiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT   *DispatchContext
  )
{
  if (mRapidStartGlobalNvs->EventsEnabled) {
    SaveOrCompareCrc32ForAbove4GB (TRUE);
  }
}
#endif

/**
  RapidStart S3 entry callback SMI handler

  @param[in] DispatchHandle  - The handle of this callback, obtained when registering
  @param[in] DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
**/
VOID
EFIAPI
RapidStartS3EntryCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  UINT16      PmBase;
  EFI_STATUS  Status;
  UINTN       SaveSize;
  UINT32      CheckingSizeInSector;
  UINT8       EventsEnabledFlag;
  PCH_SERIES  PchSeries;

  mRapidStartMemData->OsWakeTimeEnabled = 0;
  EventsEnabledFlag = 0;
  PchSeries = GetPchSeries();

  Status = EFI_SUCCESS;
  if (mRapidStartMemData->EntryCanceled) {
    ///
    /// Immediate wake system up regardless of original setting.
    ///
    RapidStartAdjustRtcAlarm (0);
    Status  = EFI_NOT_STARTED;
    mRapidStartMemData->EntryCanceled = 0;
  } else {
    /* mRapidStartMemData->EntryCanceled */
    if (mRapidStartGlobalNvs->EventsEnabled) {
      SaveSize = BuildZeroPageBitMap ();
      if (mRapidStartData->ActivePageThresholdSizeInSector == 0) {
        ///
        /// In AUTO mode, RapidStart will check the partition size automatically with active memory size
        ///
        CheckingSizeInSector = mRapidStartData->StoreSectors;
      } else {
        ///
        /// If Active Page Threshold Size is set manually, RapidStart compare this with active memory size
        ///
        CheckingSizeInSector = mRapidStartData->ActivePageThresholdSizeInSector;
      }
      if (MEM_TO_SECT (SaveSize + mRapidStartData->ZeroBitmapSize + mRapidStartData->Crc32RecordSize) > CheckingSizeInSector) {
        Status = EFI_BUFFER_TOO_SMALL;
      }
#ifdef RAPID_START_WHOLE_MEMORY_CHECK
      if (!EFI_ERROR (Status)) {
        SaveOrCompareCrc32ForAbove4GB (FALSE);
      }
#endif
    }
    //
    // Enable and override KSC critical battery wake up event threshold
    // to ensure enough battery capacity for RapidStart Entry transition
    //
    if (!EFI_ERROR (Status)) {
      if (mRapidStartGlobalNvs->EventsEnabled & RAPID_START_ACPI_BATT_WAKE) {
        RapidStartInitializeCriticalBatteryWakeupEvent (TRUE);
        EventsEnabledFlag |= RAPID_START_ACPI_BATT_WAKE;
      }

      if (mRapidStartGlobalNvs->EventsEnabled & RAPID_START_ACPI_RTC_WAKE) {
        Status = RapidStartAdjustRtcAlarm (mRapidStartGlobalNvs->WakeTimerMin * 60);
        if (!EFI_ERROR (Status)) {
          EventsEnabledFlag |= RAPID_START_ACPI_RTC_WAKE;
        }
      } else if (FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) { {
          IsctAdjustRtcAlarm();
        }
      }

    } else if (FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) { {
        IsctAdjustRtcAlarm();
      }
    }

  } /* end else: mRapidStartMemData->EntryCanceled */

  RapidStartSetConfig (EventsEnabledFlag);

  if (!EFI_ERROR (Status)) {
    PmBase = (UINT16) (PciRead32 (
                        PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                        PCI_DEVICE_NUMBER_PCH_LPC,
                        PCI_FUNCTION_NUMBER_PCH_LPC,
                        R_PCH_LPC_ACPI_BASE)
                        ) & B_PCH_LPC_ACPI_BASE_BAR);

    ///
    /// Save wake events
    ///
    if (PchSeries == PchLp) {
      mRapidStartMemData->GPE0 = IoRead32 (PmBase + R_PCH_ACPI_GPE0_EN_127_96);
    } else if (PchSeries == PchH) {
      mRapidStartMemData->GPE0a = IoRead32 (PmBase + R_PCH_ACPI_GPE0a_EN);
      mRapidStartMemData->GPE0b = IoRead32 (PmBase + R_PCH_ACPI_GPE0b_EN);
    }
  }

  AfterInitializingEntryEvent (Status);
  return;
}

/**
  Initializes the SMM S3 Handler.

  @param[in] ImageHandle - The image handle of Wake On Lan driver
  @param[in] SystemTable - The standard EFI system table

  @retval EFI_SUCCESS - RapidStart S3 entry callback SMI handle has been registered when RapidStart wake up policy enabled
**/
EFI_STATUS
EFIAPI
RapidStartSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  RAPID_START_GLOBAL_NVS_AREA_PROTOCOL  *RapidStartGlobalNvsAreaProtocol;
  EFI_SMM_SX_DISPATCH_PROTOCOL          *SxDispatchProtocol;
  EFI_SMM_SX_DISPATCH_CONTEXT           EntryDispatchContext;
#ifdef RAPID_START_WHOLE_MEMORY_CHECK
  EFI_SMM_SW_DISPATCH_PROTOCOL          *SwDispatch;
  EFI_SMM_SW_DISPATCH_CONTEXT           SwContext;
#endif
  EFI_HANDLE                            DispatchHandle;
  EFI_STATUS                            Status;
  ISCT_NVS_AREA_PROTOCOL *IsctNvsAreaProtocol;

  DEBUG ((EFI_D_INFO, "RapidStartSmmEntryPoint()\n"));

  Status = gBS->LocateProtocol (
                  &gRapidStartGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &RapidStartGlobalNvsAreaProtocol
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "No RapidStart info protocol available\n"));
    ASSERT (0);
    return Status;
  }

  if (FeaturePcdGet (PcdRapidStartWithIsctWakeSupport)) {
    mIsctNvs                  = NULL;
    ///
    /// Locate ISCT Global NVS protocol.
    ///
    Status = gBS->LocateProtocol (
                    &gIsctNvsAreaProtocolGuid,
                    NULL,
                    (VOID **)&IsctNvsAreaProtocol
                    );
    if (!EFI_ERROR (Status)) {
      mIsctNvs                = IsctNvsAreaProtocol->Area;
      mIsctNvs->IsctOverWrite = 0;
    }
  }
  mRapidStartData           = RapidStartGlobalNvsAreaProtocol->RapidStartData;
  mRapidStartGlobalNvs      = RapidStartGlobalNvsAreaProtocol->Area;
  mRapidStartMemData        = RAPID_START_MEM_DATA_PTR (mRapidStartData);
  mRapidStartZeroPageBitMap = RAPID_START_ZERO_PAGE_BITMAP_PTR (mRapidStartData);
  mCrc32Record              = RAPID_START_CRC32_RECORD_PTR (mRapidStartData);

  DEBUG ((EFI_D_INFO, "RapidStartGlobalNvs:      %x\n", mRapidStartGlobalNvs));
  DEBUG ((EFI_D_INFO, "RapidStartMemData:        %x\n", mRapidStartMemData));
  DEBUG ((EFI_D_INFO, "RapidStartZeroPageBitMap: %x\n", mRapidStartZeroPageBitMap));
  DEBUG ((EFI_D_INFO, "CRC32 record: %x\n", mCrc32Record));

  mRapidStartMemData->OsWakeTimeEnabled = 0;

  Status = gBS->LocateProtocol (
                  &gEfiSmmSxDispatchProtocolGuid,
                  NULL,
                  (VOID **) &SxDispatchProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register S3 entry phase call back function
  ///
  EntryDispatchContext.Type   = SxS3;
  EntryDispatchContext.Phase  = SxEntry;
  Status = SxDispatchProtocol->Register (
                                SxDispatchProtocol,
                                RapidStartS3EntryCallback,
                                &EntryDispatchContext,
                                &DispatchHandle
                                );
  ASSERT_EFI_ERROR (Status);

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
  ///
  /// Locate the SMM SW dispatch protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmSwDispatchProtocolGuid, NULL, (VOID **)&SwDispatch);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register SWSMI handler
  ///
  DEBUG ((EFI_D_INFO, "Register RapidStart SwSmi: %x\n", SW_SMI_WHOLE_MEMORY_CHECK));
  SwContext.SwSmiInputValue = SW_SMI_WHOLE_MEMORY_CHECK;
  Status = SwDispatch->Register (
                        SwDispatch,
                        RapidStartSwSmiCallback,
                        &SwContext,
                        &DispatchHandle
                        );
  ASSERT_EFI_ERROR (Status);
#endif

  return EFI_SUCCESS;
}
