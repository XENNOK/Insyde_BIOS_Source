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


#ifndef _TIMER_EVENT_H_
#define _TIMER_EVENT_H_
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>

#define SET_CUR_POS(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SET_COLOR(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CLEAN_SCREEN(VOID) gST->ConOut->ClearScreen(gST->ConOut)
#define EN_CURSOR(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define DEF_ROWS    31
#define DEF_COLS    100

#define PER_SECOND (10 * 1000 * 1000)
extern EFI_GUID gEfiHydraProtocolGuid;

#endif
