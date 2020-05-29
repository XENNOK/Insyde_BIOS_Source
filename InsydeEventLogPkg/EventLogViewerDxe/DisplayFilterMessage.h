/** @file

  Header file of Event Log Viewer DXE implementation.

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

#ifndef _DISPLAY_FILTER_MESSAGE_H_
#define _DISPLAY_FILTER_MESSAGE_H_

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/EventLog.h>

//
// color define
//
#define FILTER_SELECT_ITEM                              (EFI_WHITE | EFI_BACKGROUND_GREEN)
#define FILTER_UNSELECT_ITEM                            (EFI_WHITE | EFI_BACKGROUND_BLUE)
#define FILTER_SCROLL_BAR_COLOR                         (EFI_WHITE | EFI_BACKGROUND_BLACK)
#define INPUT_BUFFER_COLOR                              (EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY)


#define ITEM_TYPE_COUNT                                 10

#define WINDOW_LIMIT_HIGH                               25
#define WINDOW_LIMIT_WIDTH                              79
#define FILTER_SELECT_FRAME_ITEM_SIZE                   10

#define NO_DIALOGUE_RESPONSE                            0xFFFFFFFF

#pragma pack(1)

typedef struct {
  UINT32                                FilterType;
  UINT16                                Year;
  UINT8                                 Month;
  UINT8                                 Date;
  UINT8                                 Hour;
  UINT8                                 Minute;
  UINT8                                 Second;
  UINT16                                GeneratorId;
  EVENT_TYPE_ID                         EventID;     // 3-bytes ID:   
  UINT8                                 EventTypeID; // for bios
} FILTER_EVENT_INFO;

typedef struct {
  CHAR16                                *String;
  UINT32                                Number;
} SELECT_ITEM_INFO;

typedef enum {
  TypeDate,
  TypeTime,
  TypeGeneratorID,
  TypeEventID,
  TypeSensorType,
  TypeSensorNum,
  TypeEventType,
  TypeEventTypeID,
  TypeClearFilteredCondition,
  TypeEnd
} TYPE_OF_FILTER_EVENT;


#pragma pack()

EFI_INPUT_KEY 
EFIAPI
WaitForAKey (
  IN UINT64                                             Timeout OPTIONAL
  );

UINT32
EFIAPI
DisplayDialogueMessage (
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen,
  IN      SELECT_ITEM_INFO                              *LogTypeItem,
  ...
  );

VOID 
EFIAPI
ShowCurrentConditionMsg (
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen,
  IN      BOOLEAN                                       ClearMsg,
  IN      SELECT_ITEM_INFO                              *LogTypeItem,
  ...
  );

CHAR16 *
EFIAPI
DisplayMsgAndGetKeyWordByFilterType (
  IN      UINT32                                        FilterType,
  IN      EFI_SCREEN_DESCRIPTOR                         LocalScreen  
  );

EFI_STATUS 
EFIAPI
TransferStringToData (
  IN      CHAR16                                        *String,
  IN OUT  FILTER_EVENT_INFO                             *Data
  );

#endif  
