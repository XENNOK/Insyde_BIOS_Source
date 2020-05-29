/** @file

  This module provides password relative functions.

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

#include "Password.h"
#include "String.h"
#include <KernelSetupConfig.h>

#include <Protocol/SysPasswordService.h>
#include <Protocol/H2ODialog.h>

/**
  This function uses to check password.

  @retval TRUE     Insyde password is in locked state.
  @retval FALSE    Insyde password is in unloced state.
**/
BOOLEAN
IsPasswordExist (
  VOID
  )
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = SysPasswordService->GetStatus (SysPasswordService, SystemSupervisor);

  return Status == EFI_SUCCESS ? TRUE : FALSE;
}

/**
  Change password state from unlocked state to locked state.

  @retval EFI_SUCCESS          Change password state to locked state.
  @retval EFI_UNSUPPORTED      The password doesn't saved in variable storage.
  @retval EFI_ALREADY_STARTED  The password state is already in locked state.
**/
EFI_STATUS
LockPasswordState (
  VOID
  )
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->LockPassword (SysPasswordService);

  return Status;

}

/**
  Change the Insyde password state to unlocked state.

  @param[in] Password          The null-terminated string that is the input password.
  @param[in] PasswordLength    The password length in bytes of PasswordPtr.

  @retval EFI_SUCCESS          Change password state to unlocked state.
  @retval EFI_UNSUPPORTED      The password doesn't saved in variable storage.
  @retval EFI_ALREADY_STARTED  The password state is already in unlocked state.
**/
EFI_STATUS
UnlockPasswordState (
  IN  CHAR16                               *Password,
  IN  UINTN                                PasswordLength
  )
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->UnlockPassword (SysPasswordService, Password, PasswordLength);

  return Status;
}

/**
  Get the length of system password support.

  @param[in] SysPasswordService system password service protocol

  @retval                      The length of system password support 
**/
UINTN
SysPasswordMaxNum (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
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

/**
  Check the input password is valid or invalid.

  If input password is valid, it will unlock password state and return EFI_SUCCESS.
  If input password is invalid and exceed retry times, it will show error message and do a dead loop.
  If system doesn't have password, return EFI_SUCCESS directly.

  @retval EFI_SUCCESS          Check input password is valid and change password state to unloced.
  @retval EFI_NOT_FOUND        gH2ODialogProtocolGuid protocol doesn't exist
  @retval EFI_ALREADY_STARTED  Input password is invalid.
**/
EFI_STATUS
CheckPassword (
  VOID
  )
{
  H2O_DIALOG_PROTOCOL                    *H2ODialog;
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
                  (VOID **) &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gH2ODialogProtocolGuid, NULL, (VOID **) &H2ODialog);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CheckPasswordStr = GenericUtilityLibGetStringById (STRING_TOKEN (STR_CHECK_PASSWORD_STRING));
  gST->ConOut->ClearScreen (gST->ConOut);
  TryPasswordTimes = 0;

  SysPasswordMaximumNumber = SysPasswordMaxNum (SysPasswordService);

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  ((SysPasswordMaximumNumber + 1) * sizeof (CHAR16)),
                  (VOID **)&Password
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  do {
    ZeroMem (Password, (SysPasswordMaximumNumber + 1) * sizeof (CHAR16));
    H2ODialog->PasswordDialog (
                      0,
                      FALSE,
                      (UINT32) (SysPasswordMaximumNumber + 1),
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
    InvalidPasswordStr = GenericUtilityLibGetStringById (STRING_TOKEN (STR_INVALID_PASSWORD));
    H2ODialog->ConfirmDialog (
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
    CpuDeadLoop ();
  }
  gBS->FreePool (CheckPasswordStr);
  gBS->FreePool (Password);
  return Status;
}

