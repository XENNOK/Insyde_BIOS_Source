/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef D01_PLATFORM_SKU_DIALOG_H_
#define D01_PLATFORM_SKU_DIALOG_H_

#include <Uefi.h>
// #include <../SetupUtility.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODialog.h>
#include <Library/VariableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define CHAR_SPACE             0x0020
#define STRING_MAX_SIZE        70

//
// Platform SKU
//
#define D01_PLATFORM_SKU_REC   L"D01PlatformSku"

typedef struct {
  UINT8   Value;
  CHAR16  *ItemString;
} D01_SKU_INFO;

EFI_STATUS
EFIAPI
D01SkuDialogEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

EFI_STATUS
EFIAPI
D01SkuDialogEntryPoint (
  VOID
  );

#endif