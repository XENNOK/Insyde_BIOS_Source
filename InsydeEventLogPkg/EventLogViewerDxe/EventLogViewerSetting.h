/** @file

  Header file of Event Log Viewer setting implementation.

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

#ifndef _EVENT_LOG_VIEWER_SETTING_H_
#define _EVENT_LOG_VIEWER_SETTING_H_

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, x, y)
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, x)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, x)

#define VE_TITLE_TEXT                           (EFI_BLACK | EFI_BACKGROUND_CYAN)
#define VE_SUBTITLE_TEXT                        (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE)
#define VE_SUBTITLE_DATA                        (EFI_YELLOW| EFI_BACKGROUND_BLUE)
#define VE_NORMAL_POPUP_TEXT                    (EFI_WHITE | EFI_BACKGROUND_BLUE)
#define VE_NORMAL_POPUP_SELECTED_TEXT           (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY)
#define VE_NORMAL_MENU_TEXT                     (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY)
#define VE_SELECTED_MENU_TEXT                   (EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY)
#define VE_LIST_TITLE_TEXT                      (EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY)
#define VE_ITEM_TEXT                            (EFI_LIGHTBLUE | EFI_BACKGROUND_LIGHTGRAY)
#define VE_ITEM_DATA                            (EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY)
#define VE_BODER_LINE_COLOR                     (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY)
#define VE_BOTTOM_HELP_BACKGROUND               (EFI_WHITE | EFI_BACKGROUND_CYAN)
#define VE_BOTTOM_HELP_TEXT                     (EFI_BLACK | EFI_BACKGROUND_CYAN)
#define VE_SELECT_BAR                           (EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY)
#define VE_UNSELECT_BAR                         (EFI_BLACK | EFI_BACKGROUND_LIGHTGRAY)

#endif
