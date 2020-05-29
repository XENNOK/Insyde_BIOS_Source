//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  Password.h

Abstract:

  The header file of Password realtive functions.

--*/

#ifndef _BDS_PASSWORD_H_
#define _BDS_PASSWORD_H_

#include "Tiano.h"

BOOLEAN
IsPasswordExist (
  VOID
  )
/*++

Routine Description:

  This function uses to check password

Arguments:

  None

Returns:

  TRUE       - Insyde password is in locked state.
  FALSE      - Insyde password is in unloced state.

--*/
;

EFI_STATUS
LockPasswordState (
  VOID
  )
/*++

Routine Description:

  Change password state from unlocked state to locked state.

Arguments:

  None

Returns:

  EFI_SUCCESS           - Change password state to locked state.
  EFI_UNSUPPORTED       - The password doesn't saved in variable storage.
  EFI_ALREADY_STARTED   - The password state is already in locked state.

--*/
;

EFI_STATUS
UnlockPasswordState (
  IN  CHAR16                               *Password,
  IN  UINTN                                PasswordLength
  )
/*++

Routine Description:

  Change the Insyde password state to unlocked state

Arguments:

  Password              - The null-terminated string that is the input password.
  PasswordLength        - The password length in bytes of PasswordPtr.

Returns:

  EFI_SUCCESS           - Change password state to unlocked state.
  EFI_UNSUPPORTED       - The password doesn't saved in variable storage.
  EFI_ALREADY_STARTED   - The password state is already in unlocked state.

--*/
;

EFI_STATUS
CheckPassword (
  VOID
  )
/*++

Routine Description:

  Check the input password is valid or invalid.
  If input password is valid, it will unlock password state and return EFI_SUCCESS.
  If input password is invalid and exceed retry times, it will show error message and do a dead loop.
  If system doesn't have password, return EFI_SUCCESS directly.

Arguments:

  None

Returns:

  EFI_SUCCESS           - The input Password is valid.

--*/
;
#endif
