/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_GUID  mSecureFlashErrorMsgGuid = { 0x2719F233, 0xDD1A, 0xABCD, { 0xA9, 0xDC, 0xAA, 0xE0, 0x7B, 0x67, 0x88, 0xF2 }};
EFI_EVENT mSecureErrorMsgEvent;

/**
 Show error message box.

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Context          A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
SecureErrorMessageCallBack (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_INPUT_KEY                 Key;
  EFI_STATUS                    *ErrorStatus;
  EFI_STATUS                    Status;

  gBS->CloseEvent (Event);
  gST->ConOut->ClearScreen (gST->ConOut);
  
  Status = gBS->LocateProtocol (&mSecureFlashErrorMsgGuid, NULL, (VOID **)&ErrorStatus);
  if (EFI_ERROR (Status)) {
    *ErrorStatus = EFI_ABORTED;
  }
  
  if (*ErrorStatus == EFI_ACCESS_DENIED) {
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
                   &Key,
                   L"",
                   L" InsydeH2O - Secure Flash ",
                   L"",
                   L" Error : Invalid firmware image!!! ",
                   L"",
                   L"",
                   L" Please press any key to reset system...... ",
                   L"",
                   NULL
                   );
    } while (Key.UnicodeChar == CHAR_NULL);
  } else if (*ErrorStatus == EFI_SUCCESS) {
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
                   &Key,
                   L"",
                   L" InsydeH2O - Secure Flash ",
                   L"",
                   L" Status : Finish!!! ",
                   L"",
                   L"",
                   L" Please press any key to reset system...... ",
                   L"",
                   NULL
                   );
    } while (Key.UnicodeChar == CHAR_NULL);
  } else if (*ErrorStatus == EFI_INVALID_PARAMETER) {
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
                   &Key,
                   L"",
                   L" InsydeH2O - Secure Flash ",
                   L"",
                   L" Error : Decompress failed!!! ",
                   L"",
                   L"",
                   L" Please press any key to reset system...... ",
                   L"",
                   NULL
                   );
    } while (Key.UnicodeChar == CHAR_NULL);
  } else if (*ErrorStatus == EFI_BUFFER_TOO_SMALL) {
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
                   &Key,
                   L"",
                   L" InsydeH2O - Secure Flash ",
                   L"",
                   L" Error : Not enough memory!!! ",
                   L"",
                   L"",
                   L" Please press any key to reset system...... ",
                   L"",
                   NULL
                   );
    } while (Key.UnicodeChar == CHAR_NULL);
  } else {
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
                   &Key,
                   L"",
                   L" InsydeH2O - Secure Flash ",
                   L"",
                   L" Error : Firmware update failed!!! ",
                   L"",
                   L"",
                   L" Please press any key to reset system...... ",
                   L"",
                   NULL
                   );
    } while (Key.UnicodeChar == CHAR_NULL);
  }

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

}

EFI_STATUS
EFIAPI
SecurityErrorHandlingEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS    Status;
  VOID          *SecureErrorMsgReg;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK - 1,
                  SecureErrorMessageCallBack,
                  NULL,
                  &mSecureErrorMsgEvent
                  );
  //
  // This event will be done depends on protocol guid : gSecureFlashErrorMsgGuid.
  //
  Status = gBS->RegisterProtocolNotify (
                  &mSecureFlashErrorMsgGuid,
                  mSecureErrorMsgEvent,
                  &SecureErrorMsgReg
                  );

  return Status;
}
