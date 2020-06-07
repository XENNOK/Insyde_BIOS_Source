/** @file
  

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


#ifndef _APP_NONDRIVER_H_
#define _APP_NONDRIVER_H_

#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include "NonDriver_HW.h"

#define SET_CUR_POS(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SET_COLOR(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CLEAN_SCREEN(VOID) gST->ConOut->ClearScreen(gST->ConOut)
#define EN_CURSOR(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define DEF_ROWS    31
#define DEF_COLS    100

#define NO_SELECT   255
#define NO_TYPE 255

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16
#define ITEMS_PER_HUGEPAGE  15

#define TOTAL_ITEMS    3
#define ITEMLIST_OFFSET 5

#define HUGE_TITLE_OFFSET     3
#define DATA_FRAME_OFFSET     4
#define HUGE_TAIL_OFFSET      25

#define HUGE_TABLE_VERT_MIN   5
#define HUGE_TABLE_HORI_MIN   5
#define HUGE_TABLE_HORI_MAX   54
#define HUGE_TABLE_VERT_MAX   20
#define HUGE_FRAME_HORI_MAX   99

#define HUGE_FRAME_HORITITLE_OFFSET  4
#define HUGE_FRAME_VERTTITLE_OFFSET  0

#define HUGE_FRAME_HORI_OFFSET   5
#define HUGE_FRAME_VERT_OFFSET   4

#define HUGE_TABLE_VERT_MAX       20
#define HUGE_TABLE_HORI_BYTE_MAX  51
#define HUGE_TABLE_HORI_WORD_MAX  44
#define HUGE_TABLE_HORI_DWORD_MAX 40 

#define DATA_NUM(x) ((x)*2)-1

typedef enum {
  AllPciDev = 0,
  VgaPciDev,
  WlanPciDev
}__ENUMPCIDEV;

typedef enum _DisplayType {
  TypeByte = 1,
  TypeWord = 2,
  TypeDWord = 4
  };

UINT8
RightSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TotalItems,
  IN CHAR16                                 **ItemLists
  );

EFI_KEY_DATA
keyRead (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );
  
VOID
SetCursorPosColor (
  IN UINTN color,
  IN UINTN ROWS,
  IN UINTN COLS
  );

VOID
CleanFrame (
  UINT8    BackGroundColor,
  UINT8    TopStart,
  UINT8    TailEnd,
  UINT8    LeftStart,
  UINT8    RightEnd
  );

VOID
HugeFrame (
  IN CHAR16 *StrVal,
  IN CHAR16 *BaseTail1,
  IN CHAR16 *BaseTail2,
  IN CHAR16 *BaseTail3
  );

EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
  );


EFI_STATUS
ListAllPciHandler (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  SelectedFunc
  );


VOID
DataFrameHandler (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN PCIDEV                                 *HPciDev,
  IN UINTN                                  DataIndex
  );
#endif
