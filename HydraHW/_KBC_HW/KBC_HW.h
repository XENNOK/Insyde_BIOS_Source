/** @file

 KBC_HW Header

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


#ifndef _KBC_HW_H_
#define _KBC_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/CpuIo2.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define NO_SELECT   255
#define DEF_ROWS    25
#define DEF_COLS    80

#define KEY_EVENT   0
#define TIMER_EVENT 1


#define TOTAL_ITEMS    5

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16


#define KBC_STD_CMD 0x64
#define KBC_STD_DATA 0x60
#define KBC_KB_CMD 0x60
#define KBC_KB_DATA 0x60
#define RST_CMD 0xFE
#define STATINDIC_CMD 0xED 

#define SCRLOCK_CMD 0x01
#define NUMLOCK_CMD 0x02
#define CAPSLOCK_CMD 0x04

#define CANCEL_SCRLOCK_CMD 0xFE
#define CANCEL_NUMLOCK_CMD 0xFD
#define CANCEL_CAPSLOCK_CMD 0xFB

typedef enum {
  ScrollLockLED,
  NumLockLED,
  CapsLockLED,
  BlinkLED,
  RstSystem
}_KBCITEMS;

#endif
