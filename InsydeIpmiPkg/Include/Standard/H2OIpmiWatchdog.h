/** @file
 H2O IPMI Watchdog header file.

 This files contains watchdog timer related definitions and structures defined
 in IPMI specification.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _H2O_IPMI_WATCHDOG_H_
#define _H2O_IPMI_WATCHDOG_H_


#pragma pack(1)


//
// Watchdog Timer Commands
//
#define H2O_IPMI_CMD_RESET_WATCHDOG_TIMER    0x22
#define H2O_IPMI_CMD_SET_WATCHDOG_TIMER      0x24
#define H2O_IPMI_CMD_GET_WATCHDOG_TIMER      0x25

//
// Set Watchdog Timer bits
//
#define H2O_IPMI_WDT_LOG_BIT          0x00
#define H2O_IPMI_WDT_NO_LOG_BIT       0x80
#define H2O_IPMI_WDT_STOP_TIMER       0x00
#define H2O_IPMI_WDT_NO_STOP_TIMER    0x60

#define H2O_IPMI_WDT_ACTION_NONE          0x00
#define H2O_IPMI_WDT_ACTION_HARD_RESET    0x01
#define H2O_IPMI_WDT_ACTION_POWER_DOWN    0x02
#define H2O_IPMI_WDT_ACTION_POWER_CYCLE   0x03

//
// Set/Get Watchdog Timer Structure
//
typedef struct {
  UINT8  TimerUse     :3; ///< Logged on expiration (0:Reserved, 1:BIOS FRB2, 2:BIOS/POST, 3:OS Load, 4:SMS/OS, 5:OEM)
  UINT8  Reserved     :3;
  UINT8  NotStopTimer :1; ///< Don't Stop timer when received Set Watchdog Timer command.
  UINT8  NotLog       :1; ///< Don't log when watchdog timer expires.
  UINT8  TimerAction;
  UINT8  PreTimeoutValue;
  UINT8  TimerUseExpirationFlagsClear;
  UINT16 CountDown;
} H2O_IPMI_SET_WATCHDOG_TIMER;

typedef struct {
  UINT8  TimerUse     :3; ///< Logged on expiration (0:Reserved, 1:BIOS FRB2, 2:BIOS/POST, 3:OS Load, 4:SMS/OS, 5:OEM)
  UINT8  Reserved     :3;
  UINT8  TimerRuning  :1; ///< Timer is started (running).
  UINT8  NotLog       :1; ///< Don't log when watchdog timer expires.
  UINT8  TimerAction;
  UINT8  PreTimeoutValue;
  UINT8  TimerUseExpirationFlagsClear;
  UINT16 CountDown;
  UINT16 CurrCountDown;
} H2O_IPMI_GET_WATCHDOG_TIMER;


#pragma pack()


#endif

