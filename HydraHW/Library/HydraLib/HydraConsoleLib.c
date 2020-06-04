/** @file

  Hydra Console Library,handle attribute and mode and base frame

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include "HydraLibInternal.h"
#include <Library/HydraLib.h>

/**

  Set Cursor position and color
  @param[in] color   front and background colors
  @param[in] ROWS    cursor rows
  @param[in] COLS    cursor cols

*/
VOID
SetCursorPosColor (
  IN UINTN color,
  IN UINTN ROWS,
  IN UINTN COLS
  )
{
  SET_CUR_POS (
    ROWS,
    COLS
    );
  SET_COLOR (
    color
    );
}

/**

  Read key
  @param[in] *InputEx   Simple text input ex protocol

  @return    HKey       EFI_KEY_DATA
*/
EFI_KEY_DATA
keyRead (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS      Status;
  UINTN           Index;
  EFI_KEY_DATA    HKey;

  
  Status = gBS->WaitForEvent (
                  1,
                  &(InputEx->WaitForKeyEx),
                  &Index
                  );
  
 if (EFI_ERROR (Status)) {
    CLEAN_SCREEN (
      VOID
      );
    
    Print (
      L"### WaitForEvent ERROR ###\n"
      );
 }
 
  Status = InputEx->ReadKeyStrokeEx (
                      InputEx,
                      &HKey
                      );
  
 if (EFI_ERROR (Status)) {
    CLEAN_SCREEN (
      VOID
      );
    Print (
      L"### ReadKeyStrokeEx ERROR ###\n"
      );
 }
 
 return HKey;
}

/**

  Base Title and tail frame.
  @param[in]  *StrVal    Title string.
  @param[in]  BaseTail1  Tail1.
  @param[in]  BaseTail2  Tail2.
  @param[in]  BaseTail3  Tail3.

*/
VOID
BaseFrame (
  IN CHAR16 *StrVal,
  IN CHAR16 *BaseTail1,
  IN CHAR16 *BaseTail2,
  IN CHAR16 *BaseTail3
  )
{
  CLEAN_SCREEN (
    VOID
    );

  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    0
    );
  
  Print (
    L"===============================================================================\n"   //79 =
    ); 
  Print (
    L"=                                                                             =\n"
    );
  Print (
    L"===============================================================================\n"
    );
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    30,
    1
    );
  
  Print (
    L"%s",
    StrVal
    );
  
	SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    BASE_TAIL_OFFSET
    ); 
  
  Print (
    L"===============================================================================\n"
    );
  Print (
    L"                                                                               \n"
    );
  Print (
    L"                                                                               \n"
    );
  Print (
    L"                                                                               \n"
    );

  SET_CUR_POS (
    0,
    (BASE_TAIL_OFFSET+1)
    );

  Print (
    L"%s\n%s\n%s\n",
    BaseTail1,
    BaseTail2,
    BaseTail3
    );
  Print (
    L"==============================================================================="
    );
  
}

/**

  Check and return Mode number
  @param[in]       RequestedColumns        Requested Columns
  @param[in]       RequestedRows           Requested Rows
  @param[in][out]  *TextModeNum            Mode Number

  @return Status Code

*/
EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  )
{
  UINTN        ModeNum;
  UINTN        Columns;
  UINTN        Rows;
  EFI_STATUS   Status;

  for (ModeNum = 1; ModeNum < (UINTN) (gST->ConOut->Mode->MaxMode); ModeNum++) {
    gST->ConOut->QueryMode (
                   gST->ConOut,
                   ModeNum,
                   &Columns,
                   &Rows
                   );
    
    if ((RequestedColumns == Columns) && (RequestedRows == Rows)) {
      *TextModeNum = ModeNum;
      break;
    }
  }

  if (ModeNum == (UINTN) (gST->ConOut->Mode->MaxMode)) {
    *TextModeNum = ModeNum;
    Status = EFI_UNSUPPORTED;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

