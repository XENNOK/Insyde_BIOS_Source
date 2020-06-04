/** @file

  ACPI HW
  
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

#include"ACPI_HW.h"
#include"HugeHeader.h"

EFI_STATUS
ACPIFrameHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  // EFI_STATUS                      Status;

  CHAR16                                 *BaseFrameTitle[] = {
                                           L"ACPI"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Enter : Confirm Select               | ",
                                            L" Ctrl+F1 : Exit                       | ",
                                            L" Up / Down Botton: Select Item        | "
                                            };
  CHAR16                                 *ItemLists[] = {
                                           L"RSDT",
                                           L"XSDT"
                                           };
  CHAR16                                 **ItemListsPtr;
  UINT8                                  SelectedItem;

  EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Rsdt;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINTN                                         Index;
  UINTN                                         TotalTableNum;

  Rsdp  = NULL;
  Xsdt  = NULL;
  Rsdt  = NULL;
  TotalTableNum = 0;

  ItemListsPtr = ItemLists;
  SelectedItem = NO_SELECT;

  //
  // found ACPI table RSD_PTR from system table
  //
  for (Index = 0; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (&(gST->ConfigurationTable[Index].VendorGuid), &gEfiAcpi20TableGuid)) {
      //
      // A match was found.
      //
      Rsdp = gST->ConfigurationTable[Index].VendorTable;
      break;
    }
  }

  if (Rsdp == NULL) {
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_RED,
      4,
      0
      );
    Print (L"RSDP NOT Found...\n");
    keyRead (InputEx);
    return EFI_NOT_FOUND;
  }

  //
  // Search XSDT
  //
  if (Rsdp->Revision >= EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION) {
    Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->XsdtAddress;
  }

  //
  // Search RSDT
  //
  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->RsdtAddress;

  //
  // Show Huge Frame
  //
  HugeFrame (
    BaseFrameTitle[0],
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
  ShowRsdPtrData (Rsdp);

  while (TRUE) {
    
    
    SelectedItem = HugeRightSelectItems (
                     InputEx,
                     TOTAL_ITEMS,
                     ItemListsPtr
                     );
    if (SelectedItem == NO_SELECT) {
      break;
    }

    CleanFrame (
      EFI_BACKGROUND_BLACK,
      3,
      25,
      0,
      54
      );
    switch (SelectedItem) {
      case ENUM_RSDT:
        ShowXRsdtData (
          Rsdt,
          ENUM_RSDT
          );
        TotalTableNum = (Rsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
        SelectRSDTTables (
          InputEx,
          TotalTableNum,
          Rsdt
          );
      break;
      case ENUM_XSDT:
        ShowXRsdtData (
          Xsdt,
          ENUM_XSDT
          );
        TotalTableNum = (Xsdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
        SelectXSDTTables (
          InputEx,
          TotalTableNum,
          Xsdt
          );
      break;
    }
    
  }
  
  return EFI_SUCCESS;
}

/**
  Keyboard Controller Entry Point.

  @param[in] ImageHnadle        While the driver image loaded be the ImageLoader(), an image handle is assigned to this
                                driver binary, all activities of the driver is tied to this ImageHandle.
  @param[in] SystemTable        A pointer to the system table, for all BS(Boot Services) and RT(Runtime Services).
 
  @return Status Code

**/
EFI_STATUS
EFIAPI
H_BasePages (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                             Status;

  UINTN    TextModeNum;
  INT32    OringinalMode;

  
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  
  //
  // Check Console Mode, save orignal mode and Set specification Mode
  //
  OringinalMode = gST->ConOut->Mode->Mode;
  
  ChkTextModeNum (
    DEF_COLS,
    DEF_ROWS,
    &TextModeNum
    );
  
  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          TextModeNum
                          );
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  //
  // Locate Simple Text Input Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );

  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

    
  //
  // Keyboard Controller Handler
  //
  ACPIFrameHandler (
    InputEx
    );


  //
  // Reset to Orginal Mode
  //
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }
  
  return EFI_SUCCESS;
}
