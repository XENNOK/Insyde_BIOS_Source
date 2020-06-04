/** @file

Hydra Library Header  

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


#ifndef __PCI_UTILITY_H__
#define __PCI_UTILITY_H__

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

#define SET_CUR_POS(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SET_COLOR(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CLEAN_SCREEN(VOID) gST->ConOut->ClearScreen(gST->ConOut)
#define EN_CURSOR(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define DATA_NUM(x) ((x)*2)-1

#define DATA_ARRAY_COLS_MAX 15
#define DATA_ARRAY_ROW_MAX 15

#define BASE_TITLE_OFFSET 3

#define BASE_TITLE_OFFSET 3
#define DATA_FRAME_OFFSET 4
#define BASE_TAIL_OFFSET  20
#define BASE_FRAME_OFFSET 25

#define ITEMS_PER_PAGE   17

#define DATA_TABLE_OFFSET_HORI    4  // -
#define DATA_TABLE_OFFSET_VERT    4  // |
#define DATA_TABLE_VERT_MIN       4
#define DATA_TABLE_HORI_MIN       5
#define DATA_TABLE_VERT_MAX       19
#define DATA_TABLE_HORI_BYTE_MAX  51
#define DATA_TABLE_HORI_WORD_MAX  44
#define DATA_TABLE_HORI_DWORD_MAX 40 

#define RIGHT_DATA_FRAME_HORI_MIN 54

typedef struct __PCIDEV {
  UINT16     BusNum;
  UINT8      DevNum;
  UINT8      FunNum;
  PCI_TYPE00 Pci;
  UINT8      PcidevData[16][16]; 
} PCIDEV;

typedef enum _DisplayType {
  T_Byte = 1,
  T_Word = 2,
  T_DWord = 4
  };

UINT32
Pow (
  IN UINT32    MakeUpDataNum
  );

VOID
HydrakeyRead (
  VOID
  );

EFI_STATUS
ChkTextModeNum (
  IN UINTN     RequestedColumns,
  IN UINTN     RequestedRows,
  OUT UINTN    *TextModeNum
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

VOID RightDataFrame (
  IN PCIDEV  HPciDev,
  IN BOOLEAN AscOrData
  );

EFI_STATUS 
WritePCIData (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN PCIDEV                              *HPciDev,
  IN UINT8                               DataIndex,
  IN UINT32                              *data,
  IN UINT8                               FrameHoriPos,
  IN UINT8                               FrameVertPos,
  IN UINTN                               HType
  );

UINT32 
AscToInt (
  IN CHAR16        *writedata,
  IN UINTN         HType
  );

VOID 
CurHorizontalMove (
  IN UINT16             ScanCode,
  IN OUT INT8           *HoriPos,
  IN UINTN              HType
  );

VOID
DataBaseFrame (
  IN UINT16  Bus,
  IN UINT8   Dev,
  IN UINT8   Fun,
  IN CHAR16  *Tail1,
  IN CHAR16  *Tail2,
  IN CHAR16  *Tail3
  );

VOID
BaseFrame (
  IN CHAR16 *StrVal,
  IN CHAR16 *BaseTail1,
  IN CHAR16 *BaseTail2,
  IN CHAR16 *BaseTail3
  );

VOID
DumpPCIData (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN OUT PCIDEV                          *HPciDev,
  IN UINTN                                DataIndex
  );

UINT16 
GrepPCIDevices (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN OUT PCIDEV                          *HPciDev
  );

#endif
