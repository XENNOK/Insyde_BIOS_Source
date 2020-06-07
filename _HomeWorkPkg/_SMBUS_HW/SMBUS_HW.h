/** @file

 SIO_HW Header

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


#ifndef _SMBUS_HW_H_
#define _SMBUS_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/CpuIo2.h>

#include <Protocol/SaPlatformPolicy.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/MemInfo.h>
#include <IndustryStandard/SmBus.h>
#include <Protocol/SmbusHc.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define NO_SELECT   255
#define RESERVED    254

#define DEF_ROWS    31
#define DEF_COLS    100

#define ITEMS_PER_HUGEPAGE  4

#define TOTAL_ITEMS    4
#define ITEMLIST_OFFSET 5


EFI_STATUS
SMBusFrameHandler (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL        *DxePlatformSaPolicy,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  );

#endif
