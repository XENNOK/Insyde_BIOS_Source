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

#include <H2OKeyDesc.h>

/**
  Converts from standard EFI key data structures to strings.

  @param[in]      This             Pointer to current instance of this protocol.
  @param[out]     String           The key data user want to change.
  @param[in]      KeyData          The string has been changed.

  @retval EFI_SUCCESS            Function completed successfully.
  @retval EFI_OUT_OF_RESOURCES   Unable to allocate required resources.
*/
EFI_STATUS
EFIAPI
H2OKeyDescKeyToString (
  IN     H2O_KEY_DESC_PROTOCOL                *This,
  IN     EFI_KEY_DATA                         *KeyData,
  OUT    CHAR16                               **String
  )
{
  CHAR16                                      *Str;

  //
  // Format:
  //  [CTRL-/Ctrl-/ctrl-][SHIFT-/Shift-/shift-][ALT-/Alt-/alt-][other]
  // other:
  //  1.Esc/Ins/Del/Up/Down/Right/Left/Home/End/PgUp/PgDn
  //  2.F1~F12
  //  3.Number/Alphabet
  //  4.Enter/Backspace/Tab/Space
  //

  Str = CatSPrint (NULL, L"");
  //
  // Ctrl / Shift / Alt
  //
  if (
    (KeyData->KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED) == EFI_RIGHT_CONTROL_PRESSED ||
    (KeyData->KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) == EFI_LEFT_CONTROL_PRESSED
    ) {
    Str = CatSPrint (Str, L"CTRL-");
  }
  if (
    (KeyData->KeyState.KeyShiftState & EFI_RIGHT_SHIFT_PRESSED) == EFI_RIGHT_SHIFT_PRESSED ||
    (KeyData->KeyState.KeyShiftState & EFI_LEFT_SHIFT_PRESSED) == EFI_LEFT_SHIFT_PRESSED
    ) {
    Str = CatSPrint (Str, L"SHIFT-");
  }
  if (
    (KeyData->KeyState.KeyShiftState & EFI_RIGHT_ALT_PRESSED) == EFI_RIGHT_ALT_PRESSED ||
    (KeyData->KeyState.KeyShiftState & EFI_LEFT_ALT_PRESSED) == EFI_LEFT_ALT_PRESSED
    ) {
    Str = CatSPrint (Str, L"ALT-");
  }

  //
  // Esc / Ins / Del / Up / Down / Right / Left / Home / End / PgUp / PgDn / F1~F12
  //
  if (KeyData->Key.UnicodeChar == CHAR_NULL) {
    switch (KeyData->Key.ScanCode) {

      case SCAN_ESC:
        Str = CatSPrint (Str, L"ESC");
        goto EXIT;

      case SCAN_INSERT:
        Str = CatSPrint (Str, L"INS");
        goto EXIT;

      case SCAN_DELETE:
        Str = CatSPrint (Str, L"DEL");
        goto EXIT;

      case SCAN_UP:
        Str = CatSPrint (Str, L"UP");
        goto EXIT;

      case SCAN_DOWN:
        Str = CatSPrint (Str, L"DOWN");
        goto EXIT;

      case SCAN_RIGHT:
        Str = CatSPrint (Str, L"RIGHT");
        goto EXIT;

      case SCAN_LEFT:
        Str = CatSPrint (Str, L"LEFT");
        goto EXIT;

      case SCAN_HOME:
        Str = CatSPrint (Str, L"HOME");
        goto EXIT;

      case SCAN_END:
        Str = CatSPrint (Str, L"END");
        goto EXIT;

      case SCAN_PAGE_UP:
        Str = CatSPrint (Str, L"PGUP");
        goto EXIT;

      case SCAN_PAGE_DOWN:
        Str = CatSPrint (Str, L"PGDN");
        goto EXIT;

      case SCAN_F1:
        Str = CatSPrint (Str, L"F1");
        goto EXIT;

      case SCAN_F2:
        Str = CatSPrint (Str, L"F2");
        goto EXIT;

      case SCAN_F3:
        Str = CatSPrint (Str, L"F3");
        goto EXIT;

      case SCAN_F4:
        Str = CatSPrint (Str, L"F4");
        goto EXIT;

      case SCAN_F5:
        Str = CatSPrint (Str, L"F5");
        goto EXIT;

      case SCAN_F6:
        Str = CatSPrint (Str, L"F6");
        goto EXIT;

      case SCAN_F7:
        Str = CatSPrint (Str, L"F7");
        goto EXIT;

      case SCAN_F8:
        Str = CatSPrint (Str, L"F8");
        goto EXIT;

      case SCAN_F9:
        Str = CatSPrint (Str, L"F9");
        goto EXIT;

      case SCAN_F10:
        Str = CatSPrint (Str, L"F10");
        goto EXIT;

      case SCAN_F11:
        Str = CatSPrint (Str, L"F11");
        goto EXIT;

      case SCAN_F12:
        Str = CatSPrint (Str, L"F12");
        goto EXIT;

      default:
        break;
    }
  }

  if (KeyData->Key.ScanCode == SCAN_NULL) {
    //
    // Number / Alphabet
    //
    if (
      (KeyData->Key.UnicodeChar >= '0' && KeyData->Key.UnicodeChar <= '9') ||
      (KeyData->Key.UnicodeChar >= 'a' && KeyData->Key.UnicodeChar <= 'z') ||
      (KeyData->Key.UnicodeChar >= 'A' && KeyData->Key.UnicodeChar <= 'Z')
      ) {
      Str = CatSPrint (Str, L"%c", KeyData->Key.UnicodeChar);
      goto EXIT;
    }
    //
    // Enter / Backspace / Tab / Space
    //
    if (KeyData->Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Str = CatSPrint (Str, L"ENTER");
      goto EXIT;
    } else if (KeyData->Key.UnicodeChar == CHAR_BACKSPACE) {
      Str = CatSPrint (Str, L"BACKSPACE");
      goto EXIT;
    } else if (KeyData->Key.UnicodeChar == CHAR_TAB) {
      Str = CatSPrint (Str, L"TAB");
      goto EXIT;
    } else if (KeyData->Key.UnicodeChar == ' ') {
      Str = CatSPrint (Str, L"SPACE");
      goto EXIT;
    }
  }

EXIT:
  *String = Str;

  return EFI_SUCCESS;
}

/**
  Converts from strings to standard EFI key data structures.

  @param[in]      This             Pointer to current instance of this protocol.
  @param[in]      String           The string user want to change.
  @param[out]     KeyData          The key data has been changed.

  @retval EFI_SUCCESS            Function completed successfully.
  @retval EFI_INVALID_PARAMETER  Invalid input string.
*/
EFI_STATUS
EFIAPI
H2OKeyDescKeyFromString (
  IN     H2O_KEY_DESC_PROTOCOL                *This,
  IN     CHAR16                               *String,
  OUT    EFI_KEY_DATA                         *KeyData
  )
{
  CHAR16                                      *StrPtr;

  //
  // Format:
  //  [CTRL-/Ctrl-/ctrl-][SHIFT-/Shift-/shift-][ALT-/Alt-/alt-][other]
  // other:
  //  1.Esc/Ins/Del/Up/Down/Right/Left/Home/End/PgUp/PgDn
  //  2.F1~F12
  //  3.Number/Alphabet
  //  4.Enter/Backspace/Tab/Space
  //

  if (String == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init
  //
  StrPtr = String;
  KeyData->Key.ScanCode = 0;
  KeyData->Key.UnicodeChar = 0;
  KeyData->KeyState.KeyShiftState = 0;
  KeyData->KeyState.KeyToggleState = 0;

  KeyData->KeyState.KeyShiftState |= EFI_SHIFT_STATE_VALID;
  KeyData->KeyState.KeyToggleState |= EFI_TOGGLE_STATE_VALID;

  //
  // Ctrl
  //
  if (
    (StrnCmp (StrPtr, L"CTRL-", StrLen (L"CTRL-")) == 0) ||
    (StrnCmp (StrPtr, L"Ctrl-", StrLen (L"Ctrl-")) == 0) ||
    (StrnCmp (StrPtr, L"ctrl-", StrLen (L"ctrl-")) == 0)
    ) {
    StrPtr += 5;
    KeyData->KeyState.KeyShiftState |= EFI_LEFT_CONTROL_PRESSED;
  }

  //
  // Shift
  //
  if (
    (StrnCmp (StrPtr, L"SHIFT-", StrLen (L"SHIFT-")) == 0) ||
    (StrnCmp (StrPtr, L"Shift-", StrLen (L"Shift-")) == 0) ||
    (StrnCmp (StrPtr, L"shift-", StrLen (L"shift-")) == 0)
    ) {
    StrPtr += 6;
    KeyData->KeyState.KeyShiftState |= EFI_LEFT_SHIFT_PRESSED;
  }

  //
  // Alt
  //
  if (
    (StrnCmp (StrPtr, L"ALT-", StrLen (L"ALT-")) == 0) ||
    (StrnCmp (StrPtr, L"Alt-", StrLen (L"Alt-")) == 0) ||
    (StrnCmp (StrPtr, L"alt-", StrLen (L"alt-")) == 0)
    ) {
    StrPtr += 4;
    KeyData->KeyState.KeyShiftState |= EFI_LEFT_ALT_PRESSED;
  }

  //
  // Esc / Ins / Del / Up / Down / Right / Left / Home / End / PgUp / PgDn
  //
  if (
    (StrnCmp (StrPtr, L"ESC", StrLen (L"ESC")) == 0) ||
    (StrnCmp (StrPtr, L"Esc", StrLen (L"Esc")) == 0) ||
    (StrnCmp (StrPtr, L"esc", StrLen (L"esc")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_ESC;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"INS", StrLen (L"INS")) == 0) ||
    (StrnCmp (StrPtr, L"Ins", StrLen (L"Ins")) == 0) ||
    (StrnCmp (StrPtr, L"ins", StrLen (L"ins")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_INSERT;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"DEL", StrLen (L"DEL")) == 0) ||
    (StrnCmp (StrPtr, L"Del", StrLen (L"Del")) == 0) ||
    (StrnCmp (StrPtr, L"del", StrLen (L"del")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_DELETE;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"UP", StrLen (L"UP")) == 0) ||
    (StrnCmp (StrPtr, L"Up", StrLen (L"Up")) == 0) ||
    (StrnCmp (StrPtr, L"up", StrLen (L"up")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_UP;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"DOWN", StrLen (L"DOWN")) == 0) ||
    (StrnCmp (StrPtr, L"Down", StrLen (L"Down")) == 0) ||
    (StrnCmp (StrPtr, L"down", StrLen (L"down")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_DOWN;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"RIGHT", StrLen (L"RIGHT")) == 0) ||
    (StrnCmp (StrPtr, L"Right", StrLen (L"Right")) == 0) ||
    (StrnCmp (StrPtr, L"right", StrLen (L"right")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_RIGHT;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"LEFT", StrLen (L"LEFT")) == 0) ||
    (StrnCmp (StrPtr, L"Left", StrLen (L"Left")) == 0) ||
    (StrnCmp (StrPtr, L"left", StrLen (L"left")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_LEFT;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"HOME", StrLen (L"HOME")) == 0) ||
    (StrnCmp (StrPtr, L"Home", StrLen (L"Home")) == 0) ||
    (StrnCmp (StrPtr, L"home", StrLen (L"home")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_HOME;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"END", StrLen (L"END")) == 0) ||
    (StrnCmp (StrPtr, L"End", StrLen (L"End")) == 0) ||
    (StrnCmp (StrPtr, L"end", StrLen (L"end")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_END;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"PGUP", StrLen (L"PGUP")) == 0) ||
    (StrnCmp (StrPtr, L"PgUp", StrLen (L"PgUp")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_PAGE_UP;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"PGDN", StrLen (L"PGDN")) == 0) ||
    (StrnCmp (StrPtr, L"PgDn", StrLen (L"PgDn")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_PAGE_DOWN;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  }

  //
  // Function 1~12
  //
  if (StrnCmp (StrPtr, L"F10", StrLen (L"F10")) == 0) {
    KeyData->Key.ScanCode = SCAN_F10;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F11", StrLen (L"F11")) == 0) {
    KeyData->Key.ScanCode = SCAN_F11;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F12", StrLen (L"F12")) == 0) {
    KeyData->Key.ScanCode = SCAN_F12;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F1", StrLen (L"F1")) == 0) {
    KeyData->Key.ScanCode = SCAN_F1;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F2", StrLen (L"F2")) == 0) {
    KeyData->Key.ScanCode = SCAN_F2;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F3", StrLen (L"F3")) == 0) {
    KeyData->Key.ScanCode = SCAN_F3;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F4", StrLen (L"F4")) == 0) {
    KeyData->Key.ScanCode = SCAN_F4;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F5", StrLen (L"F5")) == 0) {
    KeyData->Key.ScanCode = SCAN_F5;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F6", StrLen (L"F6")) == 0) {
    KeyData->Key.ScanCode = SCAN_F6;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F7", StrLen (L"F7")) == 0) {
    KeyData->Key.ScanCode = SCAN_F7;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F8", StrLen (L"F8")) == 0) {
    KeyData->Key.ScanCode = SCAN_F8;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  } else if (StrnCmp (StrPtr, L"F9", StrLen (L"F9")) == 0) {
    KeyData->Key.ScanCode = SCAN_F9;
    KeyData->Key.UnicodeChar = CHAR_NULL;
    goto EXIT;
  }

  //
  // Number / Alphabet / Space
  //
  if (
    (*StrPtr >= '0' && *StrPtr <= '9') ||
    (*StrPtr >= 'a' && *StrPtr <= 'z') ||
    (*StrPtr >= 'A' && *StrPtr <= 'Z')
    ) {
    KeyData->Key.ScanCode = SCAN_NULL;
    KeyData->Key.UnicodeChar = *StrPtr;
    goto EXIT;
  }

  //
  // Enter / Backspace / Tab
  //
  if (
    (StrnCmp (StrPtr, L"ENTER", StrLen (L"ENTER")) == 0) ||
    (StrnCmp (StrPtr, L"Enter", StrLen (L"Enter")) == 0) ||
    (StrnCmp (StrPtr, L"enter", StrLen (L"enter")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_NULL;
    KeyData->Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
    goto EXIT;
  } else if (
    (StrnCmp (StrPtr, L"BACKSPACE", StrLen (L"BACKSPACE")) == 0) ||
    (StrnCmp (StrPtr, L"Backspace", StrLen (L"Backspace")) == 0) ||
    (StrnCmp (StrPtr, L"backspace", StrLen (L"backspace")) == 0)
    ) {
    KeyData->Key.ScanCode = SCAN_NULL;
    KeyData->Key.UnicodeChar = CHAR_BACKSPACE;
    goto EXIT;
    } else if (
      (StrnCmp (StrPtr, L"TAB", StrLen (L"TAB")) == 0) ||
      (StrnCmp (StrPtr, L"Tab", StrLen (L"Tab")) == 0) ||
      (StrnCmp (StrPtr, L"tab", StrLen (L"tab")) == 0)
      ) {
      KeyData->Key.ScanCode = SCAN_NULL;
      KeyData->Key.UnicodeChar = CHAR_TAB;
      goto EXIT;
    } else if (
      (StrnCmp (StrPtr, L"SPACE", StrLen (L"SPACE")) == 0) ||
      (StrnCmp (StrPtr, L"Space", StrLen (L"Space")) == 0) ||
      (StrnCmp (StrPtr, L"space", StrLen (L"space")) == 0)
      ) {
      KeyData->Key.ScanCode = SCAN_NULL;
      KeyData->Key.UnicodeChar = ' ';
      goto EXIT;
    }

EXIT:
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
H2OKeyDescEntryPoint (
  IN     EFI_HANDLE                            ImageHandle,
  IN     EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS                                   Status;
  H2O_KEY_DESC_PROTOCOL                        *H2OKeyDescProtocol;

  //
  // Install H2O_KEY_DESC_PROTOCOL
  //
  H2OKeyDescProtocol = (H2O_KEY_DESC_PROTOCOL*) AllocateZeroPool (sizeof (H2O_KEY_DESC_PROTOCOL));
  if (H2OKeyDescProtocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  H2OKeyDescProtocol->Size           = (UINT32) sizeof (H2O_KEY_DESC_PROTOCOL);
  H2OKeyDescProtocol->KeyToString    = H2OKeyDescKeyToString;
  H2OKeyDescProtocol->KeyFromString  = H2OKeyDescKeyFromString;

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gH2OKeyDescProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  H2OKeyDescProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

