/** @file

 Huge console Header

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

#ifndef _HUGEHEADER_H_
#define _HUGEHEADER_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/CpuIo2.h>

#include <Protocol/LegacyBiosPlatform.h>

#define NO_TYPE 255

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16

#define HUGE_TITLE_OFFSET   3
#define HUGE_TAIL_OFFSET    25


#define HUGE_FRAME_HORITITLE_OFFSET  4
#define HUGE_FRAME_VERTTITLE_OFFSET  0
#define HUGE_FRAME_HORI_OFFSET   5
#define HUGE_FRAME_VERT_OFFSET   4

#define HUGE_TABLE_HORI_MIN   5
#define HUGE_TABLE_VERT_MIN   5
#define HUGE_TABLE_HORI_MAX   54
#define HUGE_TABLE_VERT_MAX   20

#define HUGE_TABLE_VERT_MAX       20
#define HUGE_TABLE_HORI_BYTE_MAX  51
#define HUGE_TABLE_HORI_WORD_MAX  44
#define HUGE_TABLE_HORI_DWORD_MAX 40 

#define QUANTITY_PER_TYPE(x) ((x)*2)-1

#define ITEMS_PER_HUGEPAGE 2
#define NO_SELECT   255
#define ITEMLIST_OFFSET 5

#define TABLES_PER_HUGEPAGE 16

typedef enum _QuantityOfType {
  TypeByte = 1,
  TypeWord = 2,
  TypeDWord = 4
  };

VOID
HugeFrame (
  IN CHAR16 *StrVal,
  IN CHAR16 *BaseTail1,
  IN CHAR16 *BaseTail2,
  IN CHAR16 *BaseTail3
  );

UINT8
HugeRightSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN CHAR16                                 **ItemLists
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
ShowHugeDataFrame (
  IN UINTN     HType,
  UINT8        *FrameData
  );

VOID
ShowHeaderData (
  IN EFI_LEGACY_PIRQ_TABLE_HEADER *PirqTableHeader
);

UINT8
SlotSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems
  );

VOID
ShowEntryData (
  IN EFI_LEGACY_PIRQ_TABLE_HEADER *PirqTableHeader,
  IN UINTN                        SelectedSlot
);
#endif
