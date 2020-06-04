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

typedef struct {
  UINT8   Value;
  CHAR16  *ItemString;
} D01_SKU_INFO;

D01_SKU_INFO mSkuInfo[] = {
  { 0x01 , L" Sku 1 "},
  { 0x02 , L" Sku 2 "},
  { 0x03 , L" Sku 3 "},
  { 0x04 , L" Sku 4 "}
};

#endif