/** @file
  

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/


#ifndef _EFI_BASE_H_
#define _EFI_BASE_H_

#define _DEBUG

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, x, y)
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, x)
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, x)


#endif
