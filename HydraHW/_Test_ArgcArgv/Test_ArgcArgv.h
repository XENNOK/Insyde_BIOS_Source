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


#ifndef _ARGCARGV_H_
#define _ARGCARGV_H_
#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

#include <Protocol/EfiShellParameters.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Library/MemoryAllocationLib.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, x, y)
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, x)
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, x)

#define END_POSITION 0xFFFFFFFFFFFFFFFF
#define MAX_TYPE_PER_LINE 79

#endif
