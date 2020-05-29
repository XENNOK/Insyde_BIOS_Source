/** @file

  Header file of POST Message Hook Dxe implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _POST_MESSAGE_HOOK_DXE_H_
#define _POST_MESSAGE_HOOK_DXE_H_

//
// Statements that include other header files
//
#include <PostMessageDxe.h>

#include <Protocol/ConsoleControl.h>

#include <Protocol/EventLog.h>
#include <Protocol/Speaker.h>
#include <Uefi/UefiBaseType.h>
#include <Guid/PeiPostMessageHob.h>
#include <Pi/PiStatusCode.h>

#define TPL_DRIVER                            6

#define OEM_SEL_SENSOR_NUM                    0
#define OEM_SEL_EVENT_TYPE                    0x6F
#define OEM_SEL_UNDEFINED                     0xFF

//
// Define POST Message Behavior bitmap
// bit0:        Log to Storage
// bit1:        Show on screen after BIOS POST
// bit2:        Store message in reserved memory area
// bit3:        Create a HOB to store event
// bit4:        Generate Beep from PcBeep or audio (CODEC)
// bit5:bit15:  Reserved
//
// NOTE:
//    The behavior will be limited by BIOS Setup setting:
//      SCU Items: LogPostMsg, ShowPostMsg, BeepPostMsg.
//
#define BEHAVIOR_LOG_TO_STORAGE                  (1<<0)
#define BEHAVIOR_SHOW_ON_SCREEN                  (1<<1)
#define BEHAVIOR_STORE_MESSAGE                   (1<<2)
#define BEHAVIOR_CREATE_HOB                      (1<<3)
#define BEHAVIOR_EVENT_BEEP                      (1<<4)
#define BEHAVIOR_DEFAULT_SETTING                 (BEHAVIOR_LOG_TO_STORAGE | BEHAVIOR_SHOW_ON_SCREEN | \
                                                  BEHAVIOR_STORE_MESSAGE | BEHAVIOR_EVENT_BEEP)
#define BEHAVIOR_DEFAULT_SETTING_FOR_PEI_MSG     (BEHAVIOR_SHOW_ON_SCREEN | BEHAVIOR_STORE_MESSAGE)

typedef struct {
  UINT8                   SensorType;
  UINT8                   SensorNum;
  UINT8                   EventType;
} IPMI_SENSOR_TYPE_CODES;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;
  UINT16                         BehaviorBitMap;
} POST_MESSAGE_LIST;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;
  CHAR16                         *CodeString;
} OEM_POST_MESSAGE_STRING;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;
  IPMI_SENSOR_TYPE_CODES         EventID;
  UINT8                          EventData[3];
  CHAR16                         *CodeString;
} OEM_POST_MESSAGE_LOG;

typedef struct {
  UINTN                          BeepDuration;
  UINTN                          TimerInterval;
} BEEP_STRUCTURE;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;  
  UINT8                          BeepLoop;
  BEEP_STRUCTURE                 BeepData[9];  
} OEM_POST_MESSAGE_BEEP;

EFI_STATUS
EFIAPI
PostMessageHobHandleHook (
  IN OUT PEI_POST_MESSAGE_DATA_HOB           *PeiPostMessageHob  
  );

EFI_STATUS
EFIAPI
PostMessageStatusCodeHook (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  );

EFI_STATUS
EFIAPI
PublishPostMessageTableHook (
  VOID
  );

VOID
EFIAPI
ScanOtherDevices (
  VOID
  );

EFI_STATUS
EFIAPI
ShowPostMessage (
  VOID
  );

EFI_STATUS
EFIAPI
PopPostMessage (
  VOID
  );

EFI_STATUS
EFIAPI
EventLogHandler (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  );

EFI_STATUS
EFIAPI
PostBeepHandler (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  );

#endif
