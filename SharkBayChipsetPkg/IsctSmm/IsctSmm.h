/** @file

  Header file for Smm iSCT driver
  
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

#ifndef _ISCT_SMM_H_
#define _ISCT_SMM_H_

#include <PchRegs.h>

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/IsctNvsArea.h>
#include <Protocol/SmmSxDispatch.h>

#include <Guid/IsctPersistentData.h>

//
// Module global variables
//
EFI_GUID  gIsctNvsAreaProtocolGuid = ISCT_NVS_AREA_PROTOCOL_GUID;

ISCT_PERSISTENT_DATA              *mIsctData;
ISCT_NVS_AREA                     *mIsctNvs;

STATIC UINT8 mDaysOfMonthInfo[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define R_PCH_RTC_INDEX_ALT       0x74
#define R_PCH_RTC_TARGET_ALT      0x75
#define R_PCH_RTC_EXT_INDEX_ALT   0x76
#define R_PCH_RTC_EXT_TARGET_ALT  0x77

#define R_PCH_RTC_REGC            0x0C
#define B_PCH_RTC_REGC_AF         0x20

#define RTC_INDEX_MASK            0x7F
#define RTC_BANK_SIZE             0x80

#define R_PCH_RTC_SECOND          0x00
#define R_PCH_RTC_ALARM_SECOND    0x01
#define R_PCH_RTC_MINUTE          0x02
#define R_PCH_RTC_ALARM_MINUTE    0x03
#define R_PCH_RTC_HOUR            0x04
#define R_PCH_RTC_ALARM_HOUR      0x05
#define R_PCH_RTC_DAY_OF_WEEK     0x06
#define R_PCH_RTC_DAY_OF_MONTH    0x07
#define R_PCH_RTC_MONTH           0x08
#define R_PCH_RTC_YEAR            0x09

#define DAY_IN_SEC                (24 * 60 * 60)
                 
#pragma pack(1)
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Date;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
} RTC_TIME;
#pragma pack()

UINT8
RtcRead (
  IN      UINT8        Location
  );

BOOLEAN
RtcIsAlarmEnabled (
  VOID
  );

STATIC
VOID
RtcWaitEndOfUpdate (
  VOID
  );

EFI_STATUS
RtcGetAlarm (
  OUT      RTC_TIME        *tm
  );

VOID
RtcWrite (
  IN      UINT8        Location,
  IN      UINT8        Value
  );

EFI_STATUS
RtcSetAlarm (
  IN      RTC_TIME        *tm
  );

EFI_STATUS
RtcGetTime (
  OUT      RTC_TIME        *tm
  );

STATIC
UINT32
TimeToSeconds (
  IN      RTC_TIME        *tm
  );

STATIC
VOID
SecondsToTime (
  OUT      RTC_TIME        *tm,
  IN       UINT32          Seconds
  );

BOOLEAN
IsLeapYear (
 IN UINT16 Year
  );

UINT8
DaysOfMonth (
  IN UINT16 Year,
  IN UINT8  Month
  );

BOOLEAN
IsOver2Days (
  IN      RTC_TIME        *tm1,
  IN      RTC_TIME        *tm2
  );

STATIC
EFI_STATUS
EFIAPI
IsctS3EntryCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

EFI_STATUS
EFIAPI
InstallIsctSmm (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

#endif
