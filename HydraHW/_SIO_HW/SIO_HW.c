/** @file

  Keyboard Controller
  
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

#include"SIO_HW.h"
#include"HugeHeader.h"

EFI_STATUS
SIOFrameHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
  )
{
  
  CHAR16                                 *BaseFrameTitle[] = {
                                           L"Super IO"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Enter : Select Item / Confirm Modify | Ctrl+F1 : Exit",
                                            L" Left / Right Button: Select Item     ",
                                            L" Up / Down Botton: Select Previous    "
                                            };
  CHAR16                                 *ItemLists[] = {
                                           L"Reserved",
                                           L"Parallel Port",
                                           L"UART A",
                                           L"UART B",
                                           L"Reserved",
                                           L"Keyboard Controller",
                                           L"CIR",
                                           L"GPIO0~GPIO7",
                                           L"Port80 UART",
                                           L"GPIO8~9 / Ehance",
                                           L"ACPI",
                                           L"EC Space",
                                           L"RTC Timer",
                                           L"Deep Sleep,Power Fault",
                                           L"Fan Assign",
                                           L"Reserved"
                                           };
  UINT8                                  FrameData[16][16] = {0};
  CHAR16                                 **ItemListsPtr;
  UINT8                                  SelectedItem;
  UINT8                                  DefaultOffset;
  ItemListsPtr = ItemLists;
  SelectedItem = NO_SELECT;
  DefaultOffset = 0x07;
  //
  // Show Huge Frame
  //
  HugeFrame (
    BaseFrameTitle[0],
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );

  while (TRUE) {
    SetCursorPosColor (
      EFI_BACKGROUND_BLACK,
      HUGE_TABLE_HORI_MAX,
      ITEMLIST_OFFSET
      );
    Print (L"                                              \n");
    SetCursorPosColor (
      EFI_BACKGROUND_BLACK,
      HUGE_TABLE_HORI_MAX,
      (ITEMLIST_OFFSET + 1)
      );
    Print (L"                                              ");
    
    SelectedItem = HugeRightSelectItems (
                     InputEx,
                     TOTAL_ITEMS,
                     ItemListsPtr
                     );
    if (SelectedItem == RESERVED) {
      continue;
    } else if (SelectedItem == NO_SELECT) {
      break;
    }

    SetCursorPosColor (
      EFI_LIGHTGRAY | EFI_BACKGROUND_MAGENTA,
      HUGE_TABLE_HORI_MAX,
      ITEMLIST_OFFSET
      );
    Print (L"Back to Select ,Press Ctrl + F1");

    SetCursorPosColor (
      EFI_LIGHTGRAY,
      HUGE_TABLE_HORI_MAX,
      (ITEMLIST_OFFSET + 1)
      );
    Print (L"Device : %s (%02X)",ItemLists[SelectedItem],SelectedItem);
    
    WriteSIO (
      CpuIo,
      &SelectedItem,
      DefaultOffset,
      TypeByte
      );
    DataFrameHandler (
      InputEx,
      CpuIo
      );

  }
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
  UINTN    Columns;
  UINTN    Rows;
  INT32    OringinalMode;
  UINTN    OrgColumns;
  UINTN    OrgRows;
  
  EFI_CPU_IO2_PROTOCOL                   *CpuIo;
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
  // Locate CpuIo2 Protocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiCpuIo2ProtocolGuid,
                  NULL,
                  &CpuIo
                  );
  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiCpuIo2ProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }
    
  //
  // Keyboard Controller Handler
  //
  SIOFrameHandler (
    InputEx,
    CpuIo
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
