/** @file

 ACPI_HW Header

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


#ifndef _ACPI_HW_H_
#define _ACPI_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>

#include <IndustryStandard/Acpi.h>
#include <Library/BaseMemoryLib.h>

#define SetCurPos(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SetColor(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CleanScreen() gST->ConOut->ClearScreen(gST->ConOut)
#define EnCursor(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define NO_SELECT   255
#define RESERVED    254

#define NO_SELECT_SIGN 0xFFFFFFFF

#define DEF_ROWS    31
#define DEF_COLS    100

#define TOTAL_ITEMS    2
#define ITEMLIST_OFFSET 5

extern EFI_GUID gEfiAcpi20TableGuid;

enum {
  ENUM_RSDT = 0,
  ENUM_XSDT
};

VOID
ShowRsdPtrData (
  IN EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp
);

VOID
ShowXRsdtData (
  IN EFI_ACPI_DESCRIPTION_HEADER                   *XRsdt,
  IN UINT8                                         XsdtOrRsdt
);
/**

  This function scan ACPI table in XSDT.

  @param Xsdt      ACPI XSDT
  @param Signature ACPI table signature

  @return ACPI table

**/
VOID *
ScanXSDTTable (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Xsdt,
  IN UINT32                         Signature
  );

  /**

  This function scan ACPI table in RSDT.

  @param Rsdt      ACPI RSDT
  @param Signature ACPI table signature

  @return ACPI table

**/
VOID *
ScanRSDTTable (
  IN EFI_ACPI_DESCRIPTION_HEADER    *Rsdt,
  IN UINT32                         Signature
  );

/**
 * RSDT Table Selection.
  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  
*/
UINT32
SelectRSDTTables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN EFI_ACPI_DESCRIPTION_HEADER            *Rsdt
  );

  /**
 * XSDT Table Selection.
  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  
*/
UINT32
SelectXSDTTables (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINTN                                  TotalItems,
  IN EFI_ACPI_DESCRIPTION_HEADER            *Xsdt
  );

#endif
