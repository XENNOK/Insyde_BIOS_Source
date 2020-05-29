/** @file

  Header file of Post Message PEI implementation.

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

#ifndef _POST_MESSAGE_PEI_H_
#define _POST_MESSAGE_PEI_H_

//
// Statements that include other files.
//

#include <Library/HobLib.h>
#include <Guid/PeiPostMessageHob.h>

#include <Ppi/PostMessagePei.h>
#include <Ppi/EventLogPei.h>
#include <Ppi/Speaker.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Library/DebugLib.h>

//
// Define POST Message Behavior bitmap
// bit0:        Log to Storage
// bit1:        Show on screen after BIOS POST
// bit2:        Store message in reserved memory area
// bit3:        Create a HOB to store event
// bit4:        Generate Beep from PcBeep or audio (CODEC)
// bit5:bit15:  Reserved
//
#define BEHAVIOR_LOG_TO_STORAGE                  (1<<0)
#define BEHAVIOR_SHOW_ON_SCREEN                  (1<<1) // Not used in PEI phase.
#define BEHAVIOR_STORE_MESSAGE                   (1<<2) // Not used in PEI phase.
#define BEHAVIOR_CREATE_HOB                      (1<<3)
#define BEHAVIOR_EVENT_BEEP                      (1<<4)
#define BEHAVIOR_DEFAULT_SETTING                 (BEHAVIOR_LOG_TO_STORAGE | BEHAVIOR_CREATE_HOB | BEHAVIOR_EVENT_BEEP)

#define OEM_SEL_SENSOR_NUM                    0
#define OEM_SEL_EVENT_TYPE                    0x6F
#define OEM_SEL_UNDEFINED                     0xFF

typedef struct {
  UINT8   Code;
  UINT8   Offset;
  UINT8   Data;
} IPMI_SENSOR_TYPE_CODES;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;
  UINT16                         BehaviorBitMap;
} PEI_POST_MESSAGE_LIST;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;
  IPMI_SENSOR_TYPE_CODES         SensorType;
} PEI_OEM_POST_MESSAGE_LOG;

typedef struct {
  UINTN                          BeepDuration;
  UINTN                          TimerInterval;
} BEEP_STRUCTURE;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;
  UINT8                          BeepLoop;
  BEEP_STRUCTURE                 BeepData[9];  
} PEI_OEM_POST_MESSAGE_BEEP;

EFI_STATUS
EFIAPI
PeiPostMessageStatusCode (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

EFI_STATUS
EFIAPI
PeiPostMessageCreateHob (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue OPTIONAL, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

EFI_STATUS
EFIAPI
PeiArrangeData (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN EFI_STATUS_CODE_TYPE                 CodeType,
  IN EFI_STATUS_CODE_VALUE                CodeValue OPTIONAL, 
  IN UINT32                               Instance  OPTIONAL,
  IN CONST EFI_GUID                       *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA           *Data     OPTIONAL,
  OUT PEI_POST_MESSAGE_DATA_HOB           *EventDataHob
  );

EFI_STATUS
EFIAPI
PeiEventLogHandler (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_STATUS_CODE_TYPE       CodeType,
  IN EFI_STATUS_CODE_VALUE      CodeValue OPTIONAL, 
  IN UINT32                     Instance  OPTIONAL,
  IN CONST EFI_GUID             *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA *Data     OPTIONAL
  );

EFI_STATUS
EFIAPI
PeiEventBeepHandler (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_STATUS_CODE_TYPE       CodeType,
  IN EFI_STATUS_CODE_VALUE      CodeValue OPTIONAL, 
  IN UINT32                     Instance  OPTIONAL,
  IN CONST EFI_GUID             *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA *Data     OPTIONAL
  );

EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

#endif

