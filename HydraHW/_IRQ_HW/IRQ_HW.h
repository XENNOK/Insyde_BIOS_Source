/** @file

 IRQ_HW Header

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


#ifndef _IRQ_HW_H_
#define _IRQ_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>

#include <Protocol/LegacyBiosPlatform.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define NO_SELECT   255
#define RESERVED    254

#define DEF_ROWS    31
#define DEF_COLS    100

#define TOTAL_ITEMS    2
#define ITEMLIST_OFFSET 5

#define START_ADDRESS 0xF0000
#define END_ADDRESS 0xFFFFF
#define IRQ_SIGNATURE SIGNATURE_32 ('$', 'P', 'I', 'R')

#define HEADER_SIZE 0x20
#define PER_SLOT_SIZE 0x10

enum {
IRQHeader = 0,
IRQEntrier,
IRQTable
};
#endif
