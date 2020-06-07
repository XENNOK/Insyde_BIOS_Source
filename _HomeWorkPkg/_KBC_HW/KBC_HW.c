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

#include"KBC_HW.h"


EFI_STATUS
KBCHandler (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN EFI_CPU_IO2_PROTOCOL                   *CpuIo
  )
{
  CHAR16                                 *BaseFrameTitle[] = {
                                           L"Keyboard Crontroller"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L"Ctrl+F1 : Exit | Enter : Select Item,",
                                            L"Up Botton: Select Previous Item",
                                            L"Down Botton: Select Next Item"
                                            };
  CHAR16                                 *ItemLists[] = {
                                           L"ScrollLock LED",
                                           L"NumLock LED",
                                           L"CapsLock LED",
                                           L"Blink LED",
                                           L"Reset System"
                                           };
                                           
  CHAR16                                 **ItemListsPtr;                                          
  CHAR16                                 **BaseFrameTailPtr;

  UINT8                                  SelectedItem;

  UINT8                                  KbcAdd;
  UINT8                                  KbcData;

  BOOLEAN                                BoolScrollLockLED;
  BOOLEAN                                BoolNumLockLED;
  BOOLEAN                                BoolCapsLockLED;

  SelectedItem = NO_SELECT;
  BaseFrameTailPtr = BaseFrameTail;
  ItemListsPtr = ItemLists;

  BoolScrollLockLED = FALSE;
  BoolNumLockLED = FALSE;
  BoolCapsLockLED = FALSE;
  
  KbcAdd = 0;
  KbcData = 0;

  while (TRUE) {  
    //
    // Select return item position value
    //
    SelectedItem = SelectItems (
                     InputEx,
                     TOTAL_ITEMS,
                     BaseFrameTitle[0],
                     ItemListsPtr,
                     BaseFrameTailPtr,
                     BoolScrollLockLED,
                     BoolNumLockLED,
                     BoolCapsLockLED
                     );
    if (SelectedItem == NO_SELECT) {
      break;
    }
    
    if (SelectedItem == RstSystem) {
      KbcAdd = RST_CMD;
      KbcData = 0x01;
      KBC_Event (
        CpuIo,
        KBC_STD_CMD,
        KBC_KB_DATA,
        &KbcAdd,
        &KbcData
        );
    } else if (SelectedItem == BlinkLED) {
      BlinkLEDs (
        InputEx,
        CpuIo
        );

    } else {
      switch (SelectedItem) {
        case ScrollLockLED:
          if (BoolScrollLockLED) {
            BoolScrollLockLED = FALSE;
            KbcAdd = STATINDIC_CMD;
            KbcData = KbcData & CANCEL_SCRLOCK_CMD;

          } else {
            BoolScrollLockLED = TRUE;
            KbcAdd = STATINDIC_CMD;
            KbcData = KbcData | SCRLOCK_CMD;
          }
          break;
        case NumLockLED:
          if (BoolNumLockLED) {
            BoolNumLockLED = FALSE;
            KbcAdd = STATINDIC_CMD;
            KbcData = KbcData & CANCEL_NUMLOCK_CMD;
          } else {
            BoolNumLockLED = TRUE;
            KbcAdd = STATINDIC_CMD;
            KbcData = KbcData | NUMLOCK_CMD;
          }
          break;
        case CapsLockLED:
          if (BoolCapsLockLED) {
            BoolCapsLockLED = FALSE;
            KbcAdd = STATINDIC_CMD;
            KbcData = KbcData & CANCEL_CAPSLOCK_CMD;
          } else {
            BoolCapsLockLED = TRUE;
            KbcAdd = STATINDIC_CMD;
            KbcData = KbcData | CAPSLOCK_CMD;
          }
          break;  
        default:
          break;
      }
      
      KBC_Event (
        CpuIo,
        KBC_KB_CMD,
        KBC_KB_DATA,
        &KbcAdd,
        &KbcData
        );
      
    }
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
  
  UINT8                                  KbcAdd;
  UINT8                                  KbcData;
  KbcAdd = STATINDIC_CMD;
  KbcData = 0x00;
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
  // Set All Light Off
  //
  KBC_Event (
    CpuIo,
    KBC_KB_CMD,
    KBC_KB_DATA,
    &KbcAdd,
    &KbcData
    );
  //
  // Keyboard Controller Handler
  //
  KBCHandler (
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
