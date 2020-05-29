/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _OPEN_SAVE_FILE_H_
#define _OPEN_SAVE_FILE_H_

#define EFI_LIST_ENTRY LIST_ENTRY

#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
//
// Position define
//
#define FILE_UI_HEIGHT                            15
#define FILE_UI_WIDTH                             48

#define FILE_NAME_L_OFFSET                              14
#define FILE_NAME_R_OFFSET                              14 + 31

#define FILE_PATH_WIDTH_OFFSET                          3
#define FILE_PATH_HEIGHT_OFFSET                         1

#define DIR_WIDTH_OFFSET                                3
#define DIR_HEIGHT_OFFSET                               1

#define FILE_ITEM_L_WIDTH_OFFSET                        3
#define FILE_ITEM_L_HEIGHT_OFFSET                       4
#define FILE_ITEM_R_WIDTH_OFFSET                        FILE_UI_WIDTH - 3
#define FILE_ITEM_R_HEIGHT_OFFSET                       FILE_UI_HEIGHT - 2

#define OK_BOX_WIDTH_OFFSET                             4
#define OK_BOX_HEIGHT_OFFSET                            FILE_UI_HEIGHT

#define CANCEL_BOX_WIDTH_OFFSET                         FILE_UI_WIDTH - 13
#define CANCEL_BOX_HEIGHT_OFFSET                        FILE_UI_HEIGHT

#define FILE_NAME_SIZE                                  (FILE_NAME_R_OFFSET + 1)
#define FILE_ITEM_SIZE                                  (FILE_ITEM_R_HEIGHT_OFFSET - FILE_ITEM_L_HEIGHT_OFFSET + 1)

//
// Color define
//
#define FILE_UI_TITLE_COLOR                             EFI_TEXT_ATTR (EFI_BLACK, EFI_GREEN)
#define FILE_UI_BODY_COLOR                              EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY)
#define FILE_UI_NOT_KEY_COLOR                           EFI_TEXT_ATTR (EFI_WHITE, EFI_LIGHTGRAY)
#define FILE_UI_ERROR_BODY_COLOR                        (EFI_TEXT_ATTR (EFI_WHITE, EFI_BLUE| EFI_RED)| EFI_BRIGHT)
#define FILE_UI_WARNING_BODY_COLOR                      (EFI_TEXT_ATTR (EFI_WHITE, EFI_BLUE| EFI_RED)| EFI_BRIGHT)
#define FILE_UI_CURSOR_COLOR                            EFI_TEXT_ATTR (EFI_WHITE, EFI_LIGHTGRAY)
#define FILE_UI_SELECT_ITEM_COLOR                       EFI_TEXT_ATTR (EFI_WHITE, EFI_BLACK)
#define FILE_UI_SCROLL_BAR_COLOR                        EFI_TEXT_ATTR (EFI_WHITE, EFI_BLACK)

//
// Key value define
//
#define ALT_N                                           0x118
#define ALT_F                                           0x121
#define CTRL_P                                          0x010
#define CTRL_Q                                          0x011

#define _CR(Record, TYPE, Field)                        ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define FREE(ptr)                                       do { if ((ptr) != NULL) { FreePool (ptr); ptr = NULL;} } while (0)


#pragma pack(1)

typedef struct {
  EFI_LIST_ENTRY                        FilePtr;
  INT32                                 No;
  CHAR16                                *Name;
  EFI_FILE_INFO                         *Info;
  EFI_HANDLE                            Handle;
} FILE_HANDLE_NODE;

//[-start-140402-IB10300106-modify]//
typedef enum {
  Mode_FileName,
  Mode_SelectFile,
  Mode_LoadDefualtFile,
  Mode_OK,
  Mode_Cancel
} MODE_OF_FILE_UI;
//[-end-140402-IB10300106-modify]//

typedef enum {
  Type_Load,
  Type_Save
} TYPE_OF_FILE_UI;
#pragma pack()


EFI_STATUS 
SaveFileUi (
  IN      CHAR16                                        *TitleName,
  IN      UINT32                                        Type,
  OUT     EFI_FILE                                      **Handle
  );

//[-start-140402-IB10300106-add]//
typedef enum {
  STATE_SEARCH_DEVICE,
  STATE_SLELECT_DEVICE,
  STATE_SEARCH_FILE,
  STATE_SELECT_FILE,
  STATE_SEARCH_FILE_COMPLETE,
  STATE_SEARCH_FILE_NOT_FOUND,
  STATE_RETURN_UP_LEVEL,
  STATE_FINISH_SIM_KEY
} SIM_KEY_STATE;
//[-end-140402-IB10300106-add]//

#endif
