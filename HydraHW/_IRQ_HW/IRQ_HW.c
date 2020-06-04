/** @file

  IRQ HW
  
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

#include"IRQ_HW.h"
#include"HugeHeader.h"

EFI_LEGACY_PIRQ_TABLE_HEADER  *mPirqTableHeader;

EFI_STATUS
GetIrqAddress (
  VOID
)
{
   UINT8 *MemoryPtr;
   MemoryPtr = (UINT8*)(UINTN) START_ADDRESS;

   //
   // Search $PIR from 0xF0000 to 0xFFFFF
   do {
     mPirqTableHeader = (EFI_LEGACY_PIRQ_TABLE_HEADER *)MemoryPtr++;
   } while ((mPirqTableHeader->Signature != IRQ_SIGNATURE) && (MemoryPtr != (UINT8*)(UINTN)END_ADDRESS));

   if (MemoryPtr == (UINT8*)(UINTN)END_ADDRESS) {
     return EFI_NOT_FOUND;
   }
   return EFI_SUCCESS;
}

EFI_STATUS
IRQFrameHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS                      Status;

  CHAR16                                 *BaseFrameTitle[] = {
                                           L"IRQ"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Enter : Confirm Select               | ",
                                            L" Ctrl+F1 : Exit                       | ",
                                            L" Up / Down Botton: Select Item        | "
                                            };
  CHAR16                                 *ItemLists[] = {
                                           L"IRQ Routing Header",
                                           L"IRQ Routing Entries"
                                           };
  CHAR16                                 **ItemListsPtr;
  UINT8                                  SelectedItem;

  UINT8                                  SlotCount;
  UINT8                                  SelectedSlot;

  ItemListsPtr = ItemLists;
  SelectedItem = NO_SELECT;

  SlotCount = 0;
  SelectedSlot = 0;

  Status = GetIrqAddress ();
  if (Status == EFI_NOT_FOUND) {
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_RED,
      0,
      3
    );
    Print(L"IRQ Table NOT FOUND!!\n");
    Print(L"Press any key to continue");
    SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    Print(L"\n");
    keyRead (InputEx);

    return Status;
  }
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
      case IRQHeader:
      ShowHeaderData (mPirqTableHeader);

      break;
      case IRQEntrier:
        SlotCount = (UINT8) ((mPirqTableHeader->TableSize - (UINT16) HEADER_SIZE) / (UINT16) PER_SLOT_SIZE);
        while (TRUE) {
          SelectedSlot = SlotSelectItems (
                          InputEx,
                          SlotCount
                          );
          if (SelectedSlot == NO_SELECT) {
            break;
          }
          CleanFrame (
            EFI_BACKGROUND_BLACK,
            3,
            25,
            0,
            54
            );
          ShowEntryData (
            mPirqTableHeader,
            SelectedSlot
          );
        }
      break;
      case IRQTable:
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
  IRQFrameHandler (
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
