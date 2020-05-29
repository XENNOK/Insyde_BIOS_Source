/** @file
 Definitions for screen colors

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _COLORS_H
#define _COLORS_H

//
// Screen Color Settings
//
#define PICKLIST_HIGHLIGHT_TEXT       EFI_WHITE
#define PICKLIST_HIGHLIGHT_BACKGROUND EFI_BACKGROUND_CYAN
#define TITLE_TEXT                    EFI_WHITE
#define TITLE_BACKGROUND              EFI_BACKGROUND_BLUE
#define KEYHELP_TEXT                  EFI_LIGHTGRAY
#define KEYHELP_BACKGROUND            EFI_BACKGROUND_BLACK
#define SUBTITLE_TEXT                 EFI_BLUE
#define SUBTITLE_BACKGROUND           EFI_BACKGROUND_LIGHTGRAY
#define BANNER_TEXT                   EFI_BLUE
#define BANNER_BACKGROUND             EFI_BACKGROUND_LIGHTGRAY
#define FIELD_TEXT                    EFI_BLACK
#define FIELD_TEXT_GRAYED             EFI_DARKGRAY
#define FIELD_BACKGROUND              EFI_BACKGROUND_LIGHTGRAY
#define FIELD_TEXT_HIGHLIGHT          EFI_LIGHTGRAY
#define FIELD_BACKGROUND_HIGHLIGHT    EFI_BACKGROUND_BLACK
#define POPUP_TEXT                    EFI_LIGHTGRAY
#define POPUP_BACKGROUND              EFI_BACKGROUND_BLUE
#define POPUP_INVERSE_TEXT            EFI_LIGHTGRAY
#define POPUP_INVERSE_BACKGROUND      EFI_BACKGROUND_BLACK
#define HELP_TEXT                     EFI_BLUE
#define ERROR_TEXT                    EFI_RED | EFI_BRIGHT
#define INFO_TEXT                     EFI_YELLOW | EFI_BRIGHT
#define ARROW_TEXT                    EFI_RED | EFI_BRIGHT
#define ARROW_BACKGROUND              EFI_BACKGROUND_LIGHTGRAY
#define MENU_TEXT_UNSEL               EFI_LIGHTGRAY
#define MENU_BACKGROUND_UNSEL         EFI_BACKGROUND_BLUE
#define MENU_TEXT_SEL                 EFI_BLUE
#define MENU_BACKGROUND_SEL           EFI_BACKGROUND_LIGHTGRAY

//
// Screen Color for SetupUtility
//
#define SU_FRAME_LINE                 EFI_BLUE
#define SU_FIELD_TEXT                 EFI_BLUE
#define SU_BACKGROUND                 EFI_BACKGROUND_LIGHTGRAY
#define SU_TEXT_HIGHLIGHT             EFI_WHITE
#define SU_BACKGROUND_HIGHLIGHT       EFI_BACKGROUND_LIGHTGRAY
#define SU_TITLE_TEXT                 EFI_BLACK
#define SU_TITLE_BACKGROUND           EFI_BACKGROUND_CYAN
#define SU_MENU_TEXT                  EFI_LIGHTGRAY
#define SU_MENU_BACKGROUND            EFI_BACKGROUND_BLUE
#define SU_SUBTITLE_TEXT              EFI_BLACK

#endif

