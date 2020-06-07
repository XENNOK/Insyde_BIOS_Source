/** @file

Variable_HW Header

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


#ifndef _Variable_HW_H_
#define _Variable_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HandleParsingLib.h>
#include <Base.h>
#include <PiPei.h>
#include <Library/HobLib.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define NO_SELECT   255
#define NO_INPUT    0x80000000
#define RESERVED    254

#define DEF_ROWS    31
#define DEF_COLS    100

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16
#define ITEMS_PER_HUGEPAGE  15

#define TOTAL_ITEMS    4
#define ITEMLIST_OFFSET 5

#define NO_TYPE 255

#define HUGE_TITLE_OFFSET     3
#define DATA_FRAME_OFFSET     4
#define HUGE_TAIL_OFFSET      25
#define HUGE_TABLE_HORI_MAX   54
#define HUGE_TABLE_VERT_MAX   20
#define HUGE_FRAME_HORI_MAX   99

#define INIT_NAME_BUFFER_SIZE 128
#define INIT_DATA_BUFFER_SIZE 1024
#define WRITE_NAME_SIZE 30
#define QUANTITY_PER_TYPE(x) ((x)*2)-1

extern EFI_GUID gEfiHydraVariGuid;

UINT8
RightSelectItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN CHAR16                                 **ItemLists
  );

VOID
CleanFrame (
  UINT8    TopStart,
  UINT8    TailEnd,
  UINT8    LeftStart,
  UINT8    RightEnd
  );

EFI_STATUS
ListAllVariables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );

EFI_STATUS
SearchVariables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );

EFI_STATUS
GuidWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TopStart,
  IN UINT8                                  LeftStart,
  IN UINTN                                  ModifyBytes,
  IN UINTN                                  OrgData,
  IN OUT EFI_GUID                           *TargetGUID
  );

EFI_STATUS
VarNameWrite (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  TopStart,
  IN UINT8                                  LeftStart,
  IN OUT CHAR16                             *VariName
  );

EFI_STATUS
GuidData (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN CHAR16             *ModifyVal,
  IN INT8               DataNum,
  IN UINTN              ModifyBytes,
  IN UINTN              OrgData,
  IN OUT EFI_GUID       *TargetGUID
  );

EFI_STATUS
GetNextVari (
  IN OUT UINTN      *NameSize,
  IN OUT CHAR16     *VariableName,
  IN OUT EFI_GUID   *VendorGuid
  );

EFI_STATUS
GetVariData (
  IN CHAR16                                 *VariableName,
  IN EFI_GUID                               *VendorGuid,
  IN OUT UINTN                              DataBufferSize,
  IN OUT UINT32                             *Attributes,
  IN OUT UINT8                              *DataBuffer,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );

EFI_STATUS
CreateVariables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );

EFI_STATUS
ShowDataBuf (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  DataBufferSize,
  IN UINT8                                  *DataBuffer
  );

EFI_STATUS
ListHydraHobs (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  ); 

typedef enum {
  ListVari = 0,
  SearchVariByGuidName,
  CreateNew,
  GetHobs
} _VariableEnum;

typedef struct {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Guid;
  UINT8                   *Data;
  UINT16                  Size;
} HOB_LIST;

#endif
