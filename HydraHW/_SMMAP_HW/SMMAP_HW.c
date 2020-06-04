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

#include"SMMAP_HW.h"

/**

  Base Title and tail frame.
  @param[in]  *StrVal    Title string.
  @param[in]  BaseTail1  Tail1.
  @param[in]  BaseTail2  Tail2.
  @param[in]  BaseTail3  Tail3.

*/
VOID
HugeFrame (
  IN CHAR16 *StrVal,
  IN CHAR16 *BaseTail1,
  IN CHAR16 *BaseTail2,
  IN CHAR16 *BaseTail3
  )
{
  CLEAN_SCREEN (
    VOID
    );

  SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    0
    );
  
  Print (
    L"===================================================================================================\n"   //99 =
    ); 
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"===================================================================================================\n"
    );
  
  SetCursorPosColor (
    EFI_BLACK| EFI_BACKGROUND_LIGHTGRAY,
    40,
    1
    );
  
  Print (
    L"%s",
    StrVal
    );
  
	SetCursorPosColor (
    EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY,
    0,
    HUGE_TAIL_OFFSET
    ); 
  
  Print (
    L"===================================================================================================\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );
  Print (
    L"=                                                                                                 =\n"
    );

  SET_CUR_POS (
    0,
    (HUGE_TAIL_OFFSET+1)
    );

  Print (
    L"%s\n%s\n%s\n",
    BaseTail1,
    BaseTail2,
    BaseTail3
    );
  Print (
    L"===================================================================================================\n"
    );
  
}


EFI_STATUS
EFIAPI
H_BasePages (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                             Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  EFI_KEY_DATA                           HKey;
  EFI_SMM_CONTROL2_PROTOCOL            *SmmControl;
  UINT8                                SmiDataValue;
  UINT8                                SmiDataSize;
  
  UINTN    TextModeNum;
  INT32    OringinalMode;
  CHAR16                                 *BaseFrameTitle[] = {
                                           L"System Management Mode AP"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Esc : Exit",
                                            L" F1  : Trigger     ",
                                            L"     "
                                            };

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
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %r\n",
      Status
      );
    
    return Status;
  }

  SmiDataValue = EFI_HYDRA_ENABLE_SW_SMI;
  SmiDataSize  = 1;
  Status = gBS->LocateProtocol (
                  &gEfiSmmControl2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmControl
                  );
  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSmmControl2ProtocolGuid Fail : %r\n",
      Status
      );
    
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

    HKey = keyRead (InputEx);

    if (HKey.Key.ScanCode == SCAN_F1) {
      Status = SmmControl->Trigger (
                             SmmControl,
                             &SmiDataValue,
                             &SmiDataSize,
                             FALSE,
                             0
                             );
      if (EFI_ERROR (Status)) {
        Print(
          L"LocateProtocol SmmControl->Trigger Fail : %r\n",
          Status
          );
        
        return Status;
      } 
    } else if (HKey.Key.ScanCode == SCAN_ESC) {
      // Status = SmmControl->Clear (SmmControl,0);
      // if (EFI_ERROR (Status)) {
      //   Print(
      //     L"LocateProtocol SmmControl->Clear Fail : %r\n",
      //     Status
      //     );
        
      //   return Status;
      // } 
      break;
    }
  }

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
