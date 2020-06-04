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

#ifndef _DM_DRIVER_H_
#define _DM_DRIVER_H_
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci.h>

#include <Library/DevicePathLib.h>

#include <Protocol/PciIo.h>

#define SET_CUR_POS(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SET_COLOR(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CLEAN_SCREEN(VOID) gST->ConOut->ClearScreen(gST->ConOut)
#define EN_CURSOR(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define LANGUAGE_CODE_ENGLISH_ISO639    "eng"
#define LANGUAGE_CODE_ENGLISH_RFC4646   "en-US"
#define IMAGE_DRIVER_NAME       L"Hydra Driver"
#define CONTROLLER_DRIVER_NAME  L"Generic Hydra Driver Controller"
#define AUDIO_DEVICE 0x03

extern EFI_DRIVER_BINDING_PROTOCOL      gEfiDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gDMDComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gDMDComponentName2;

#endif