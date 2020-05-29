/** @file

  Header file of Event Log Viewer DXE implementation.

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

#ifndef _EVENT_LOG_VIEWER_H_
#define _EVENT_LOG_VIEWER_H_

//
// Statements that include other header files
//
#include <EventLogViewerSetting.h>
#include <EventLogViewerCommonFunc.h>

#include <Protocol/EventLog.h>
#include <Protocol/EventLogViewer.h>
#include <Protocol/EventLogString.h>
#include <Protocol/EventLogStringOverwrite.h>
#include <Protocol/LoadedImage.h>



#define MAX_BUFFER_SIZE                         100
#define DEFAULT_SCROLLBAR_WIDTH                 1

#define CURRENT_POSX    (UINTN)gST->ConOut->Mode->CursorColumn
#define CURRENT_POSY    (UINTN)gST->ConOut->Mode->CursorRow

typedef struct {
  CHAR16                  *StorageString;
  CHAR16                  *ShowString;
  UINTN                   Id;
} VIEW_EVENT_LOG_MENU;

EFI_STATUS
EFIAPI
ViewEventLogDriverUnload ( 
  IN EFI_HANDLE  ImageHandle 
  );

EFI_STATUS
EFIAPI
ExecuteEventLogViewer (
  IN EVENT_LOG_VIEWER_PROTOCOL            *This
  );

EFI_STATUS
EFIAPI
GetEventString (
  IN     VOID                           *EventInfo,
  IN OUT CHAR16                         **EventString,
  IN OUT UINTN                          *StringSize
  );

//
// Global Variables
//
extern UINTN                               gColumn;
extern UINTN                               gTopRow;
extern UINTN                               gBottomRow;
extern UINTN                               gMaxItemPerPage;
extern VIEW_EVENT_LOG_MENU                 *MenuList;
extern EVENT_LOG_PROTOCOL                  *gEventHandler;
extern CHAR16                              *gUtilityVersion;
extern BOOLEAN                             gShowUtilVer;
#endif  
