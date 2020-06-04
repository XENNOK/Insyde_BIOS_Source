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

#include "AppNonDriver_HW.h"


EFI_STATUS
PciHandler (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  
  CHAR16                                 *BaseFrameTitle[] = {
                                           L"Non-Driver Model Driver"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Enter : Select Item / Confirm Modify   |  Ctrl+F1     : Exit",
                                            L" Left / Right Button: Select Item       |  ",
                                            L" Up / Down Botton: Select Previous      |  "
                                            };

  CHAR16                                 *ItemLists[] = {
                                             L"List All PCI Devices?",
                                             L"List VGA Devices    ?",
                                             L"List WLAN Devices   ?"
                                           };
  
  CHAR16                                 **ItemListsPtr;
  UINT8                                  SelectedItem;



  ItemListsPtr = ItemLists;
  SelectedItem = NO_SELECT;
  
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
    CleanFrame (
      EFI_BACKGROUND_BLACK,
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      0,
      HUGE_FRAME_HORI_MAX
      );
 
    SelectedItem = RightSelectItems (
                     InputEx,
                     (UINT8)TOTAL_ITEMS,
                     ItemListsPtr
                     );
    if (SelectedItem == NO_SELECT) {
      break;
    }

    ListAllPciHandler (     
      HydraBridgeIo,
      InputEx,
      SelectedItem
      );

  }
  
  return EFI_SUCCESS;
}

EFI_STATUS
HydraMain (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS	Status;
  EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraBridgeIo;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;

  UINTN    TextModeNum;
  INT32    OringinalMode;
  

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
    CLEAN_SCREEN(VOID);
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  //
  // Locate HydraPciProtocol
  //
	Status = gBS->LocateProtocol(
                  &gEfiHydraPciProtocolGuid,
                  NULL,
                  &HydraBridgeIo
                  );
	if(EFI_ERROR(Status)){
    Print(L"LocatrProtcol gEfiHydraPciProtocolGuid Fail\n");
		return Status;		
	}
  
  //
  // Run PciHandler
  //
  PciHandler (
    HydraBridgeIo,
    InputEx
    );

  //
  // Reset to Orginal Mode
  //
  EN_CURSOR(TRUE);
  SET_COLOR(EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

return Status;
}






