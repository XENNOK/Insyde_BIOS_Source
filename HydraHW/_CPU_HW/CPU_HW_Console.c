/** @file

  Hydra Console ,handle attribute and mode and base frame

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

#include"CPU_HW.h"
/**
  Clean frame.
  
  @param[in] TopStart       Position from top.
  @param[in] TailEnd        End Position.
  @param[in] LeftStart      Position from left.
  @param[in] RightEnd        End Position.
  
*/
VOID
CleanLeftFrame (
  UINT8    TopStart,
  UINT8    TailEnd,
  UINT8    LeftStart,
  UINT8    RightEnd
  )
{
  UINT8 CleanLine;
  UINT8 LineIndex;

  for (LineIndex = TopStart; LineIndex < TailEnd; LineIndex++) {
    SetCursorPosColor (
      EFI_BACKGROUND_BLACK,
      LeftStart,
      LineIndex
      );

    for (CleanLine = LeftStart; CleanLine < RightEnd ; CleanLine++) {
      Print (L" ");
    }
  }
}
  

/**

  Base Title and tail frame.
  
  @param[in]  *StrVal    Title string.
  @param[in]  BaseTail1  Tail1.
  @param[in]  BaseTail2  Tail2.
  @param[in]  BaseTail3  Tail3.

*/
VOID
HugeFrame (
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
    L"===================================================================================================\n"   //99 =
    ); 
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"===================================================================================================\n"
    );
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    40,
    1
    );
  
  Print (
    L"%s",
    StrVal
    );
  
	SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    HUGE_TAIL_OFFSET
    ); 
  
  Print (
    L"===================================================================================================\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );

  SET_CUR_POS (
    0,
    (HUGE_TAIL_OFFSET+1)
    );

  Print (
    L"%s\n%s\n%s\n",
    BaseTail1,
    BaseTail2,
    BaseTail3
    );
  Print (
    L"===================================================================================================\n"
    );
  
}
