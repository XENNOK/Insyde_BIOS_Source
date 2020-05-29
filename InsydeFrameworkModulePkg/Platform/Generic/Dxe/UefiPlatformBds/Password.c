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

  Password.c

Abstract:

  This module provides password relative functions.

--*/

#include "Password.h"
#include "String.h"
#include "SetupConfig.h"

#include EFI_PROTOCOL_CONSUMER (SysPasswordService)
#include EFI_PROTOCOL_CONSUMER (OemFormBrowser2)


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
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = SysPasswordService->GetStatus (SysPasswordService, SystemSupervisor);

  return Status == EFI_SUCCESS ? TRUE : FALSE;

}

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
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->LockPassword (SysPasswordService);

  return Status;

}

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
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->UnlockPassword (SysPasswordService, Password, PasswordLength);

  return Status;
}

UINTN
SysPasswordMaxNum (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  )
/*++

Routine Description:

  Get the length of system password support.

Arguments:

  SysPasswordService    - system password service protocol

Returns:

  

--*/
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }


  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  if (EFI_ERROR(Status)) {
    return 0;
  }

  return SetupInfoBuffer.MaxSysPasswordLength;
}
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
{
  EFI_OEM_FORM_BROWSER2_PROTOCOL         *OemFormBrowser;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL      *SysPasswordService;
  EFI_STATUS                              Status;
  CHAR16                                  *CheckPasswordStr;
  CHAR16                                  *InvalidPasswordStr;
  CHAR16                                  *Password;
  EFI_INPUT_KEY                           Key;
  UINTN                                   TryPasswordTimes;
  UINTN                                   SysPasswordMaximumNumber;

  if (!IsPasswordExist ()) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiOemFormBrowser2ProtocolGuid, NULL, &OemFormBrowser);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CheckPasswordStr = GetStringById (STRING_TOKEN (STR_CHECK_PASSWORD_STRING));
  gST->ConOut->ClearScreen (gST->ConOut);
  TryPasswordTimes = 0;

  SysPasswordMaximumNumber = SysPasswordMaxNum (SysPasswordService);

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  ((SysPasswordMaximumNumber + 1) * sizeof (CHAR16)),
                  &Password
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  do {
    EfiZeroMem (Password, (SysPasswordMaximumNumber + 1) * sizeof (CHAR16));
    OemFormBrowser->DialogIcon (
                    0,
                    FALSE,
                    SysPasswordMaximumNumber + 1,
                    Password,
                    &Key,
                    CheckPasswordStr
                    );
      Status = SysPasswordService->CheckPassword (
                                     SysPasswordService,
                                     Password,
                                   (SysPasswordMaximumNumber + 1) * sizeof(CHAR16),
                                     SystemSupervisor
                                     );
     if (Status == EFI_SUCCESS) {
       SysPasswordService->UnlockPassword (
                             SysPasswordService,
                             Password,
                             (SysPasswordMaximumNumber + 1) * sizeof(CHAR16)
                             );
     }
     TryPasswordTimes++;
  } while (Status != EFI_SUCCESS && TryPasswordTimes < 3);

  //
  // Display check password failed and do a dead loop in this function, if user inputs password is invalid.
  //
  if (Status != EFI_SUCCESS) {
    InvalidPasswordStr = GetStringById (STRING_TOKEN (STR_INVALID_PASSWORD));
    OemFormBrowser->CreatePopUp (
                      2,
                      FALSE,
                      40,
                      NULL,
                      &Key,
                      InvalidPasswordStr
                      );
    gBS->FreePool (InvalidPasswordStr);
    gBS->FreePool (CheckPasswordStr);
    gBS->FreePool (Password);
    while (TRUE);
  }
  gBS->FreePool (CheckPasswordStr);
  gBS->FreePool (Password);
  return Status;
}


