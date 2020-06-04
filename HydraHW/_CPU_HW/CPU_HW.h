/** @file

 CPU_HW Header

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


#ifndef _CPU_HW_H_
#define _CPU_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/CpuIo2.h>
#include <Base.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define NO_SELECT   255
#define NO_INPUT    0x80000000
#define RESERVED    254

#define DEF_ROWS    31
#define DEF_COLS    100

#define ITEMS_PER_HUGEPAGE  15

#define TOTAL_ITEMS    25
#define ITEMLIST_OFFSET 5

#define NO_TYPE 255

#define HUGE_TITLE_OFFSET   3
#define HUGE_TAIL_OFFSET    25
#define HUGE_TABLE_HORI_MAX   54
#define HUGE_TABLE_VERT_MAX   20
#define HUGE_FRAME_HORI_MAX   99

#define QUANTITY_PER_TYPE(x) ((x)*2)-1

typedef enum {
  StandardFunc = 14,
  ExtendedFunc = 23,
  ReadWiteMSR  = 25
}__CPUIDFUNC;
#endif
