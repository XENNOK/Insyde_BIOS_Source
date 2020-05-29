/** @file

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __METRO_UI_H
#define __METRO_UI_H

#include "UiControls.h"

#define FB_NOTIFY_REFRESH          (WM_APP + 1)
#define FB_NOTIFY_REPAINT          (WM_APP + 2)
#define FB_NOTIFY_CHECK_SELECTION  (WM_APP + 3)
#define FB_NOTIFY_RESET            (WM_APP + 4)
#define FB_NOTIFY_LOAD_DEFAULT     (WM_APP + 5)
#define FB_NOTIFY_SAVE_AND_EXIT    (WM_APP + 6)
#define FB_NOTIFY_SELECT_Q         (WM_APP + 7)

typedef enum {
  HotkeyNoAction,
  HotkeyShowHelpMsg,
  HotKeyDiscardExit,
  HotKeySelectItemUp,
  HotKeySelectItemDown,
  HotkeySelectMenuUp,
  HotkeySelectMenuDown,
  HotkeyModifyValueUp,
  HotkeyModifyValueDown,
  HotKeyLoadDefault,
  HotKeySaveAndExit,
  HotKeyEnter,
  HotKeyMax
} HOT_KEY_ACTION;

extern HWND gWnd;

EFI_STATUS
EFIAPI
LocalMetroDialogNotify (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  );

EFI_STATUS
SendSelectQNotify (
  IN H2O_PAGE_ID                PageId,
  IN EFI_QUESTION_ID            QuestionId,
  IN EFI_IFR_OP_HEADER          *IfrOpCode
  );

EFI_STATUS
SendOpenQNotify (
  IN H2O_PAGE_ID                PageId,
  IN EFI_QUESTION_ID            QuestionId
  );

EFI_STATUS
SendChangeQNotify (
  IN       H2O_PAGE_ID                        PageId,
  IN       EFI_QUESTION_ID                    QuestionId,
  IN       EFI_HII_VALUE                      *HiiValue
  );

EFI_STATUS
SendShutDNotify (
  );


EFI_STATUS
SendSelectPNotify (
  IN H2O_PAGE_ID                PageId
  );

EFI_STATUS
SendShowHelpNotify (
  VOID
  );

EFI_STATUS
SendLoadDefaultNotify (
  VOID
  );

EFI_STATUS
SendSubmitExitNotify (
  VOID
  );

EFI_STATUS
SendDiscardExitNotify (
  VOID
  );

CHAR16 *
GetHotkeyDescription (
  IN H2O_EVT_TYPE                          EvtType
  );


VOID
AddHiiImagePackage (
  VOID
  );

BOOLEAN
IsDayValid (
  IN EFI_TIME                              *EfiTime
  );

BOOLEAN
IsLeapYear (
  IN UINT16                                Year
  );

BOOLEAN
IsRootPage (
  IN H2O_PAGE_ID                           PageId
  );

EFI_STATUS
HotKeyFunc (
  IN HOT_KEY_ACTION             HotkeyAction
  );

EFI_STATUS
GetNextSelectableMenu (
  IN  H2O_PAGE_ID                          PageId,
  IN  BOOLEAN                              GoDown,
  IN  BOOLEAN                              MenuIsLoop,
  OUT H2O_PAGE_ID                          *NextPageId
  );

EFI_STATUS
GetNextQuestionValue (
  IN  H2O_FORM_BROWSER_Q                   *CurrentQ,
  IN  BOOLEAN                              GoDown,
  OUT EFI_HII_VALUE                        *ResultHiiValue
  );

EFI_IMAGE_INPUT *
GetCurrentFormSetImage (
  VOID
  );

CHAR16 *
GetCurrentFormSetTitle (
  VOID
  );

BOOLEAN
NeedShowSetupMenu (
  VOID
  );


CHAR16 *
CatStringArray (
  IN CONST UINT32        StringCount,
  IN CONST CHAR16        **StringArray
  );

VOID
FreeStringArray (
  IN CHAR16                                **StringArray,
  IN UINT32                                StringArrayCount
  );

CHAR16 **
CopyStringArray (
  IN CHAR16                                **StringArray,
  IN UINT32                                StringArrayCount
  );

UINT32
GetStringArrayCount (
  IN CHAR16                                **StringArray
  );

VOID
FreeHiiValueArray (
  IN EFI_HII_VALUE                         *HiiValueArray,
  IN UINT32                                HiiValueArrayCount
  );

CHAR16 **
CreateStrArrayByNumRange (
  IN UINT32                                MinValue,
  IN UINT32                                MaxValue,
  IN UINT32                                Step,
  IN CHAR16                                *FormattedStr
  );

EFI_HII_VALUE *
CopyHiiValueArray (
  IN EFI_HII_VALUE                         *HiiValueArray,
  IN UINT32                                HiiValueArrayCount
  );

EFI_STATUS
CompareHiiValue (
  IN  EFI_HII_VALUE                        *Value1,
  IN  EFI_HII_VALUE                        *Value2,
  OUT INTN                                 *Result
  );

UINT64
GetArrayData (
  IN VOID                                  *Array,
  IN UINT8                                 Type,
  IN UINTN                                 Index
  );

VOID
SetArrayData (
  IN VOID                                  *Array,
  IN UINT8                                 Type,
  IN UINTN                                 Index,
  IN UINT64                                Value
  );

EFI_STATUS
GetRectByName (
  IN  HWND             Wnd,
  IN  CHAR16           *Name,
  OUT RECT             *Rect
  );

VOID
GrayOutBackground (
  IN   HWND       Wnd,
  IN   BOOLEAN    Grayout
  );

EFI_STATUS
InitializeWindows (
  VOID
  );

VOID
RGB2HSV (
  IN CONST EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Pixel,
  OUT HSV_VALUE                            *Hsv
  );


VOID
HSV2RGB (
  IN CONST HSV_VALUE                      *Hsv,
  OUT      EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *Pixel
  );


VOID
GetCurrentHaloHsv (
  OUT HSV_VALUE      *Hsv
  );

VOID
GetCurrentMenuHsv (
  OUT HSV_VALUE      *Hsv
  );

EFI_STATUS
GetInformationField (
  OUT RECT                                    *InfoField
  );

BOOLEAN
IsFrontPage (
  VOID
  );

#endif

