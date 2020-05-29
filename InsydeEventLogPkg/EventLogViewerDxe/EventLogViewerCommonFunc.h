/** @file

  Header file of Event Log Viewer Common Function DXE implementation.

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

#ifndef _EFI_EVENT_LOG_VIEWER_COMMON_FUNC_H_
#define _EFI_EVENT_LOG_VIEWER_COMMON_FUNC_H_

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Guid/MdeModuleHii.h>

#include <Protocol/EventLog.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>

#include <DisplayFilterMessage.h>

#define VTS_LEFT_ALIGN                    0
#define VTS_RIGHT_ALIGN                   1
#define VTS_LEAD_0                        2

#define NO_STORAGE_BE_SELECTED            0xFFFF

//
// Filtered condition bitmap
//
#define B_FILTERED_BY_YEAR                BIT0
#define B_FILTERED_BY_MONTH               BIT1
#define B_FILTERED_BY_DATE                BIT2
#define B_FILTERED_BY_HOUR                BIT3
#define B_FILTERED_BY_MINUTE              BIT4
#define B_FILTERED_BY_SECOND              BIT5
#define B_FILTERED_BY_GENERATOR_ID        BIT6
#define B_FILTERED_BY_EVENT_ID            BIT7
#define B_FILTERED_BY_SENSOR_TYPE         BIT8
#define B_FILTERED_BY_SENSOR_NUM          BIT9
#define B_FILTERED_BY_EVENT_TYPE          BIT10
#define B_FILTERED_BY_EVENT_TYPE_ID       BIT11


typedef enum {
  VeDlgYesNo,
  VeDlgOk,
  VeDlgNoOperation
} VE_DIALOG_OPERATION;

VOID
EFIAPI
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  );

VOID
EFIAPI
ClearLines (
  UINTN                                       LeftColumn,
  UINTN                                       RightColumn,
  UINTN                                       TopRow,
  UINTN                                       BottomRow,
  UINTN                                       TextAttribute
  );

UINTN
EFIAPI
GetStringWidth (
  CHAR16                                      *String
  );

BOOLEAN
EFIAPI
IsLeapYear (
  IN UINT16   Year
  );

UINTN
EFIAPI
CountNumOfLeapYears (
  IN UINT16           CurYear
  );

EFI_STATUS
EFIAPI
Ts2et (
  IN UINT32             TimeStamp,
  IN EFI_TIME           *EfiTime
  );

VOID
EFIAPI
ProcessString (
  IN  CHAR16  *StringPtr,
  OUT CHAR16  **FormattedString,
  IN  UINTN   StringWidth,
  IN  UINTN   RowCount,
  OUT UINTN   *TotalLines
  );

UINTN
EFIAPI
DisplayPopupMessage (
  IN CHAR16                      *String,
  IN VE_DIALOG_OPERATION         DialogOperator,
  IN EFI_SCREEN_DESCRIPTOR       LocalScreen
  );

VOID
EFIAPI
DisplayScrollbar (
  IN  UINTN                     CurrentLine,
  IN  UINTN                     TotalLines,
  IN  UINTN                     LinesPerPage,
  IN  UINTN                     PrintColumn,
  IN  UINTN                     PrintRow,
  IN  BOOLEAN                   UpArrow,
  IN  BOOLEAN                   DownArrow
  );

EFI_INPUT_KEY
EFIAPI
VeMenuOperation (
  IN     UINTN        TopRow,
  IN     UINTN        MvLimit,
  IN     UINTN        EventCount,
  IN     UINTN        MaxItemPerPage,
  IN OUT UINTN        *Row,
  IN OUT UINTN        *TopOfScreen,
  IN OUT UINTN        *PagSelected
  );

VOID
EFIAPI
DsplayHotkeyInfo1 (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  );

VOID
EFIAPI
DsplayHotkeyInfo2 (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  );

VOID
EFIAPI
DsplayHotkeyInfo3 (
  EFI_SCREEN_DESCRIPTOR             LocalScreen
  );

EFI_STATUS
EFIAPI
CatViewEventLogString (
  IN     CHAR16            *StorageName,
  IN OUT CHAR16            **ViewLogString
);

EFI_STATUS
EFIAPI
CatClearEventLogString (
  IN     CHAR16            *StorageName,
  IN OUT CHAR16            **ClearLogString
);

VOID
EFIAPI
StrIndexedCat (
  IN     CHAR16   *SrcStr,
  IN     UINTN    Index,
  IN     CHAR16   *InsertStr,
  IN OUT CHAR16   **OutStr
  );

EFI_STATUS
EFIAPI
ViewStorageEventLog (
  IN  UINTN                             StorageId,
  IN  EFI_SCREEN_DESCRIPTOR             LocalScreen
  );

EFI_STATUS
EFIAPI
EventLogFilter (
  IN EFI_SCREEN_DESCRIPTOR     LocalScreen,
  IN OUT UINTN                 *EventCount,
  IN OUT UINTN                 TopOfScreen,
  IN OUT UINTN                 PagSelected
  );

VOID 
EFIAPI
ArrangeFilterCondition (
  IN  BOOLEAN                                       ResetFilter,
  IN  FILTER_EVENT_INFO                             *Data
  );

EFI_STATUS 
EFIAPI
SortOutEventLogDataBaseByFilterCondition (
  IN EFI_SCREEN_DESCRIPTOR                         LocalScreen,  
  IN OUT UINTN                                     *EventCount
  );

BOOLEAN 
EFIAPI
IsMatchFilterCondition (
  IN STORAGE_EVENT_LOG_INFO                *EventData
  );

VOID
EFIAPI
PrintStorageEventList (
  IN EFI_SCREEN_DESCRIPTOR     LocalScreen,
  IN UINTN                     EventCount,
  IN OUT UINTN                 TopOfScreen,
  IN OUT UINTN                 PagSelected
  );

BOOLEAN
EFIAPI
DumpStorageEvent (
  IN CHAR16                    *StorageName,
  IN EFI_SCREEN_DESCRIPTOR     LocalScreen,
  IN UINTN                     EventCount,
  IN UINTN                     SelectedLog,
  IN OUT UINTN                 *TopOfScreen,
  IN OUT UINTN                 *PagSelected
  );

EFI_STATUS
EFIAPI
ShowStorageEventLog (
  IN UINTN                                 EventCount,
  IN EFI_SCREEN_DESCRIPTOR                 LocalScreen,
  IN STORAGE_EVENT_LOG_INFO                *EventLog  
  );

EFI_STATUS
EFIAPI
ClearStorageEventLog (
  IN  UINTN                             StorageId,
  IN  EFI_SCREEN_DESCRIPTOR             LocalScreen
  );

EFI_STATUS
EFIAPI
SaveEventToFile (
  IN CHAR16                                *StorageName,
  IN UINTN                                 EventCount,
  IN STORAGE_EVENT_LOG_INFO                *EventLog,
  IN BOOLEAN                               SaveAllEvents
  );

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *
EFIAPI
SearchFileSystem (
  BOOLEAN Removable
  );

EFI_STATUS
EFIAPI
CreateSavingFile (
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *Volume,  
  IN CHAR16                             *FileName,
  IN VOID                               *SavingDataBuffer,
  IN UINTN                              SavingDataSize
  );

EFI_STATUS
EFIAPI
ObtainSavingFileNameByTime (
  IN CHAR16                                 *StorageName,
  IN OUT CHAR16                             **FileName
  );

VOID
EFIAPI
ValueToString (
  UINTN     Value,
  UINTN     Digitals,
  CHAR16    *Buffer,
  UINTN     Flags
  );

#endif

