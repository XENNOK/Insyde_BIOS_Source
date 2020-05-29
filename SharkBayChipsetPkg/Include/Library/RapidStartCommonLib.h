/** @file
  Defines and prototypes for the library module.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

  This library generally contains all functions that can be reused in PEIM, DXE
  and SMM drivers. Some RTC access code and platform KSC functions included are
  just sample code and may need some modification to meet platform implementation.
  Here just a list of functions that will be used in reference code and some
  functions used by this library itself are skipped.

  Most of the functions in this library are common code that can be reused in all
  PEI, DXE and SMM phases. It includes RTC access relevant code and KSC commands
  which currently used by RapidStartPei and RapidStartSmm drivers. Note: KSC
  commands are platform specific and may need modification to work on different
  platform implementation.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

**/
#ifndef _RAPID_START_LIB_H_
#define _RAPID_START_LIB_H_
#include <RapidStartConfig.h>
#include <RapidStartData.h>

/**
  Read specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.
**/
UINT8
RtcRead (
  IN      UINT8        Location
  );

/**
  Write specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM
**/
VOID
RtcWrite (
  IN      UINT8        Location,
  IN      UINT8        Value
  );

/**
  Read word from specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.
**/
UINT16
RtcRead16 (
  IN      UINT8        Location
  );

/**
  Write word to specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM
**/
VOID
RtcWrite16 (
  IN      UINT8        Location,
  IN      UINT16       Value
  );

/**
  Initialize RTC Timer
**/
VOID
RtcInit (
  VOID
  );

/**
  Get current RTC time

  @param[out] tm          RTC time structure including Second, Minute and Hour.

  @retval EFI_SUCCESS     Operation successfully and RTC_TIME structure contained current time.
**/
EFI_STATUS
RtcGetTime (
  OUT      RTC_TIME        *tm
  );

/**
  Check if RTC Alarm has been enabled.

  @retval TRUE      RTC Alarm is enabled
  @retval FALSE     RTC Alarm is not enabled
**/
BOOLEAN
RtcIsAlarmEnabled (
  VOID
  );

/**
  Get current RTC Alarm time.

  @param[out] tm               A structure which will be updated with current RTC Alarm time

  @retval EFI_NOT_STARTED      RTC Alarm has not been enabled yet.
  @retval EFI_SUCCESS          RTC Alarm enabled and RTC_TIME structure contain current Alarm time setting.
**/
EFI_STATUS
RtcGetAlarm (
  OUT      RTC_TIME        *tm
  );

/**
  Set RTC Alarm with specific time

  @param[in] tm             A time interval structure which will be used to setup an RTC Alarm

  @retval EFI_SUCCESS       RTC Alarm has been enabled with specific time interval
**/
EFI_STATUS
RtcSetAlarm (
  IN      RTC_TIME        *tm
  );

/**
  Enable or disable critical battery wakeup event.

  This function enables/disables critical battery wakeup event on KSC controller.
  This KSC specific command will not be sent if KSC not present. Porting is
  required to match platform design.

  @param[in] Enabled - Enable or disable Critical Battery wakeup event.
**/
VOID
RapidStartInitializeCriticalBatteryWakeupEvent (
  IN      BOOLEAN                     Enabled
  );

/**
  Check if current wakeup is because of critical low battery.

  This function checks if current S3 resume caused by critical battery wakeup
  event. This KSC specific command will not be sent if KSC not present. Porting is
  required to match platform design.

  @retval TRUE  - Current wakeup source is critical low battery event.
  @retval FALSE - The wakeup source is not critical low battery event.
**/
BOOLEAN
RapidStartCheckCriticalBatteryWakeupEvent (
  VOID
  );

/**
  Clear all wakeup status.

  This function clears all wakeup status bits on KSC controller. This KSC specific
  command will not be sent if KSC not present. Porting is required to match
  platform design.
**/
VOID
RapidStartClearAllKscWakeStatus (
  VOID
  );

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
/**
  Log CRC32 mismatched address and length

  This is optional OEM hook function. It can be used to log any CRC32 mismatched memory chunk if needed.

  @param[in] MismatchedBaseAddress      - Memory address caused mismatched CRC32
  @param[in] MismatchedDataSize         - memory length for calculating CRC32

  @retval EFI_SUCCESS            Process successfully
**/
EFI_STATUS
LogCrc32MismatchedAddress (
  UINTN  MismatchedBaseAddress,
  UINT32 MismatchedDataSize
  );

/**
  Log or compare CRC32 value for specific memory range. Each 16MB block will have one CRC32 value.

  This function checks the memory integrity between Rapid Start Entry and Resume.
  It calculates CRC32 for each memory chunk during Rapid Start Entry, and when
  Rapid Start Resume it compares each memory chunk CRC32 value with previously
  saved value to see if any mismatch found. Basically the debug mode BIOS can
  display all information regarding mismatched memory address. In non-debug mode
  BIOS, the mismatched memory address and size can be logged if needed. Refer to
  section "Porting recommendation" for more information.

  @param[in] IsComparingCrc32 - FALSE to save CRC32 value into buffer. TRUE to compare CRC32 value with pre-saved value in buffer.
  @param[in] BaseAddress      - Base memory address for logging or checking CRC32
  @param[in] EndAddress       - End memory address for logging or checking CRC32
  @param[in] RapidStartData   - A data buffer stored RapidStart internal non-volatile information.
**/
VOID
SaveOrCompareCrc32 (
  IN  BOOLEAN                      IsComparingCrc32,
  IN  UINTN                        BaseAddress,
  IN  UINTN                        EndAddress,
  IN  RAPID_START_PERSISTENT_DATA  *RapidStartData
  );
#endif
/**
  Gets RapidStart non-volatile flag.

  Bit0 (RAPID_START_FLAG_ENTRY_DONE):
    - Rapid Start Entry flow has completed successfully.
    - Next boot will perform Rapid Start Resume.
  Bit1 (RAPID_START_FLAG_STORE_CHANGE):
    - Rapid Start Store change detected in Rapid Start Resume flow.
    - Rapid Start Resume should be aborted.

  This function will get Rapid Start non-volatile flag which controls overall
  Rapid Start transition behavior. If Rapid Start Entry already performed in last
  boot (Rapid Start Flag bit1 was set) the Rapid Start Exit transition can be
  executed in current boot. After got the Rapid Start Entry state it is
  recommended to clear this flag (Rapid Start Flag bit1) after drive lock password
  has been retrieved and scrubbed from non-volatile memory (if drive-lock feature
  enabled). There are several ways to store Rapid Start Flag and it is platform
  specific. It must be stored in non-volatile memory and one of the methods is to
  use CMOS RAM.

  @param[out] Value     - RapidStart non-volatile flag

  @retval EFI_SUCCESS   - Return the RapidStart flag in Value argument.
**/
EFI_STATUS
RapidStartGetFlag (
  OUT     UINT8                   *Value
  );

/**
  Sets RapidStart non-volatile flag.

  Bit0 (RAPID_START_FLAG_ENTRY_DONE):
    - Rapid Start Entry flow has completed successfully.
    - Next boot will perform Rapid Start Resume.
  Bit1 (RAPID_START_FLAG_STORE_CHANGE):
    - Rapid Start Store change detected in Rapid Start Resume flow.
    - Rapid Start Resume should be aborted.

  This function will set Rapid Start non-volatile flag which controls overall
  Rapid Start transition behavior. In end of Rapid Start Entry it will set bit0 to
  indicate next boot may perform Rapid Start Resume. It will also be used to clear
  Rapid Start Flag when required information has been retrieved.

  @param[in] Value         - RapidStart flag state

  @retval EFI_SUCCESS   - Flag is set.
**/
EFI_STATUS
RapidStartSetFlag (
  IN      UINT8                     Value
  );

/**
  Retrieves non-volatile RapidStart settings.

  @param[out] Value         - RapidStart non-volatile settings

  @retval EFI_SUCCESS
**/
EFI_STATUS
RapidStartGetConfig (
  OUT     UINT8                   *Value
  );

/**
  Sets non-volatile RapidStart settings.

  @param[in] Value         - RapidStart non-volatile settings

  @retval EFI_SUCCESS
**/
EFI_STATUS
RapidStartSetConfig (
  IN      UINT8                     Value
  );

/**
  This callback is executed at the end of RapidStart Entry events initialization. (Inside sleep SMI handler)

  This function will be called in the end of sleep SMI handler which initializes
  Rapid Start Entry trigger events (RTC alarm timer or critical battery wakeup
  threshold). It can be used to log the status of initialization like Entry
  cancellation or Entry aborted due to a sooner wakeup timer has be initialized by
  OS/application. In these cases Rapid Start trigger events will not be
  initialized and system will directly enter standard S3 state.

  This function can be used to log any error status of initializing Rapid Start
  Entry trigger events in sleep SMI handler and then inform users for why Rapid
  Start Entry not happened.

  @param[in] TransitionStatus - Status of initialization for RapidStart Entry events.
**/
VOID
AfterInitializingEntryEvent (
  IN      EFI_STATUS                TransitionStatus
  );

/**
  Check whether RapidStart Resume.

  @retval TRUE   - Rapid Start Entry flow has completed successfully
**/
BOOLEAN
RapidStartResumeCheck (
  VOID
);
#endif
