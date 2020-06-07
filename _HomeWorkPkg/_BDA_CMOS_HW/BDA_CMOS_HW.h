/** @file

 BDA CMOS Header

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


#ifndef _BDA_CMOS_H_
#define _BDA_CMOS_H_

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

#define DEF_ROWS    25
#define DEF_COLS    80

#define KEY_EVENT   0
#define TIMER_EVENT 1

#define CMOS_CMD    0x70
#define CMOS_DATA   0x71
#define BDA_PORT    0x400
#define EBDA_PORT   0x40e
#define TOTAL_NUM   3

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16
#define CMOS_MAX_VERT  8
#define BDA_MAX_VERT   16
#define CMOS_MAX_BYTE  128

typedef enum displaytype{
               T_BDA = 0,
               T_EBDA,
               T_CMOS
               };
#endif
