/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Platform.h>
/**

 @param [in]   DecimalValue


**/
UINT8
ConvertSetupTimeToRtcTime (
  IN  UINT8 DecimalValue
  )
{
  UINTN   High, Low;

  High    = DecimalValue / 10;
  Low     = DecimalValue % 10;

  return ((UINT8)(Low + (High * 16)));
}

//[-start-120413-IB07360188-modify]//
/**

 @param [in]   AcpiBaseAddr
 @param [in]   SetupVariable

 @retval None.

**/
VOID
S5WakeUpSetting (
  IN UINT16                         AcpiBaseAddr,
  IN CHIPSET_CONFIGURATION           *SetupVariable
  )
{
  UINT8   RtcSecond;
  UINT8   RtcMinute;
  UINT8   RtcHour;
  UINT8   Buffer;
  BOOLEAN BcdMode;

  RtcSecond = 0;
  RtcMinute = 0;
  RtcHour   = 0;
  Buffer    = 0;
  BcdMode   = TRUE;

  //
  // Get the Data Mode. (BCD or Binary)
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
  BcdMode = ~(Buffer >> 2);
  BcdMode &= BIT0;

//[-start-120726-IB03530384-modify]//
  if (BcdMode) {
    RtcSecond = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5Time.Second);
    RtcMinute = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5Time.Minute);
    RtcHour   = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5Time.Hour);
  } else {
    RtcSecond = SetupVariable->WakeOnS5Time.Second;
    RtcMinute = SetupVariable->WakeOnS5Time.Minute;
    RtcHour   = SetupVariable->WakeOnS5Time.Hour;
  }
//[-end-120726-IB03530384-modify]//
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_SECONDS_ALARM);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, RtcSecond);

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_MINUTES_ALARM);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, RtcMinute);

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_HOURS_ALARM);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, RtcHour);
  //
  // Clear these bits for S5 Wake up everyday. If it's S5 Wake up on day of month,
  // these bits will be updated at the end of the function.
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
  Buffer = 0;
  IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
  Buffer |= ALARM_INTERRUPT_ENABLE;
  IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);

  Buffer = IoRead8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE);
  Buffer |= RTC_EVENT_ENABLE;
  IoWrite8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE, Buffer);

  if (SetupVariable->WakeOnS5 == S5_WAKE_ON_DAY_OF_MONTH) {
    IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
    if (BcdMode){
      Buffer = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5DayOfMonth);
    }
    IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);
  }
  return;
}
//[-end-120413-IB07360188-modify]//