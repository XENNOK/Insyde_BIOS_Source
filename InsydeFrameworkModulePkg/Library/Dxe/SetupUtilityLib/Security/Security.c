//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   Security.c
//;
//; Abstract:
//;
//;   Main funcitons for Security menu

#include "Security.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)
#include EFI_PROTOCOL_CONSUMER (TcgService)

BOOLEAN                     mSupervisorStartFlag  = TRUE;

STATIC
EFI_STATUS
PasswordCallback (
  IN  BOOLEAN                           SupervisorOrUser,
  IN  EFI_IFR_DATA_ARRAY                *Data,
  OUT EFI_HII_CALLBACK_PACKET           **Packet,
  OUT BOOLEAN                           *PState,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo
  )
/*++

Routine Description:

  System password callback funtion

Arguments:

  SupervisorOrUser -     True:  this is supervisor password
                         False: this is user password
  Data -                 A pointer to the data being sent to the original exporting driver.
  Packet -               EFI_HII_CALLBACK_PACKET
  *PState -              Password access is success or not, if access success then return TRUE.
  PswdConsoleService -   Password console service ptr
  SecurityDataInfo -     Security information

Returns:

  EFI_SUCCESS -          Password callback success.
  EFI_ALREADY_STARTED -  Already password in SetupUtility.
  EFI_NOT_READY -        Password confirm error.

--*/
{
  static UINTN                          PasswordState;
  static UINT16                         CurrentPassword[MAX_PASSWORD_NUMBER];
  UINTN                                 NumOfInfo;
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINTN                                 PswLength;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumOfInfo = 0;
  *PState = FALSE;
  Index = 0;

  if (Data->Data->Length == 3) {
    if (Data->Data->Data == 0) {
      //
      // Check has password or not.
      //
      PasswordState = 0;

      if (SecurityDataInfo->Flag == CHANGE_PASSWORD) {
        return EFI_ALREADY_STARTED;
      }

      if (SecurityDataInfo->Flag == DISABLE_PASSWORD) {
        return EFI_SUCCESS;
      }

      Status = PswdConsoleService->GetStatus (
                                     PswdConsoleService,
                                     SupervisorOrUser
                                     );

      SecurityDataInfo->SystemPasswordStatus = Status;

      if (SecurityDataInfo->SystemPasswordStatus == EFI_SUCCESS) {
        return EFI_ALREADY_STARTED;
      }

    } else {
      //
      // Get the Password handle state.
      //
      PasswordState = (UINTN) Data->Data->Data;
    }
  } else {
    switch (PasswordState) {

    case 0:
      break;

    case 1:
      //
      // Enter old password to confirmation.
      //
      PasswordState = 0;

      if (SecurityDataInfo->Flag == CHANGE_PASSWORD) {
        if (EfiStrCmp (Data->Data->Data, SecurityDataInfo->InputString) != 0x00) {
          return EFI_NOT_READY;
        }
        break;
      }

      Status = PswdConsoleService->CheckPassword (
                                     PswdConsoleService,
                                     Data->Data->Data,
                                     MAX_PASSWORD_NUMBER * sizeof (UINT16),
                                     SupervisorOrUser
                                     );

      if (Status != EFI_SUCCESS) {
        return EFI_NOT_READY;
      }

      break;

    case 2:
      *PState = TRUE;
      break;

    case 3:
      //
      // Enter new password.
      //
      PswLength = EfiStrLen ((CHAR16*) Data->Data->Data);
      if (MIN_PASSWORD_LENGTH > 1) {
        if (PswLength < MIN_PASSWORD_LENGTH && PswLength != 0) {
          PasswordState = 0;
          return EFI_NOT_READY;
        }
      }
      EfiStrCpy (CurrentPassword, Data->Data->Data);
      PasswordState = 0;
      break;

    case 4:
      //
      // Confirm new password.
      //
      if (EfiStrCmp (Data->Data->Data, CurrentPassword) != 0x00) {
        PasswordState = 0;
        SuBrowser->SCBuffer->SupervisorFlag = 1;
        if (SupervisorOrUser == SUPERVISOR_FLAG) {
          //
          //Confirm Supervisor password
          //

          //
          //Set Supervisor password at the first time but confirm error
          //
          if ((mSupervisorStartFlag == TRUE) && (SuBrowser->SUCInfo->SupervisorPwdFlag == FALSE)) {
            SecurityDataInfo->Flag = NO_ACCESS_PASSWORD;
          }
          //
          //Supervisor password is set and change password confirm error
          //
          if ((mSupervisorStartFlag == TRUE) && (SuBrowser->SUCInfo->SupervisorPwdFlag == TRUE)) {
            SecurityDataInfo->Flag = NO_ACCESS_PASSWORD;
          }
          //
          //Set Supervisor password at the first time and set password again but confirm error.
          //
          if (mSupervisorStartFlag == FALSE && SuBrowser->SUCInfo->SupervisorPwdFlag == TRUE) {
            SecurityDataInfo->Flag = CHANGE_PASSWORD;
          }
          //
          //Disable Supervisor password
          //
          if ((mSupervisorStartFlag == FALSE) && (SuBrowser->SUCInfo->SupervisorPwdFlag == FALSE)) {
            SecurityDataInfo->Flag = DISABLE_PASSWORD;
          }
#ifdef SUPPORT_USER_PASSWORD
        } else {
          //
          //Confirm User password
          //

          //
          //Set User password at the first time but confirm error
          //
          if ((mSupervisorStartFlag == TRUE) && (SuBrowser->SUCInfo->UserPwdFlag == FALSE)) {
            SecurityDataInfo->Flag = NO_ACCESS_PASSWORD;
          }
          //
          //User password is set and change password confirm error
          //
          if ((mSupervisorStartFlag == TRUE) && (SuBrowser->SUCInfo->UserPwdFlag == TRUE)) {
            SecurityDataInfo->Flag = NO_ACCESS_PASSWORD;
          }
          //
          //Set User password at the first time and set password again but confirm error.
          //
          if (mSupervisorStartFlag == FALSE && SuBrowser->SUCInfo->UserPwdFlag == TRUE) {
            SecurityDataInfo->Flag = CHANGE_PASSWORD;
          }
          //
          //Disable User password
          //
          if ((mSupervisorStartFlag == FALSE) && (SuBrowser->SUCInfo->UserPwdFlag == FALSE)) {
            SecurityDataInfo->Flag = DISABLE_PASSWORD;
          }
        }
#else
        }
#endif
        return EFI_NOT_READY;
      }

#ifdef SUPPORT_USER_PASSWORD
      if ((SupervisorOrUser == USERPASSWORD_FLAG) && (CurrentPassword[0] != 0x00) && EfiStrCmp (SuBrowser->SUCInfo->SupervisorPassword->InputString, CurrentPassword) == 0) {
        PasswordState = 0;
        return EFI_UNSUPPORTED;
      } else {
        //
        //Supervisor can not same as the user password
        //
        if ((SupervisorOrUser == SUPERVISOR_FLAG) && (SuBrowser->SUCInfo->UserPassword->Flag != DISABLE_PASSWORD) && (CurrentPassword[0] != 0x00)) {
          Status = PswdConsoleService->GetStatus (
                                         PswdConsoleService,
                                         SystemUser
                                         );

          Status = PswdConsoleService->CheckPassword (
                                         PswdConsoleService,
                                         CurrentPassword,
                                         MAX_PASSWORD_NUMBER*sizeof(UINT16),
                                         SystemUser
                                         );

          if ((!EFI_ERROR(Status)) || (EfiStrCmp (CurrentPassword, \
               SuBrowser->SUCInfo->UserPassword->InputString) == 0)) {
            PasswordState = 0;
            return EFI_UNSUPPORTED;
          }
        }

        if ((SuBrowser->SUCInfo->SupervisorPassword->Flag != CHANGE_PASSWORD) && (SupervisorOrUser == USERPASSWORD_FLAG)) {
          Status = PswdConsoleService->GetStatus (
                                         PswdConsoleService,
                                         SystemSupervisor
                                         );

           Status = PswdConsoleService->CheckPassword (
                                         PswdConsoleService,
                                         CurrentPassword,
                                         MAX_PASSWORD_NUMBER*sizeof(UINT16),
                                         SystemSupervisor
                                         );

          if (!EFI_ERROR(Status)) {
            PasswordState = 0;
            return EFI_UNSUPPORTED;
          }
        }
      }
#endif
      if (CurrentPassword[0] != 0x00) {
#ifdef SUPPORT_USER_PASSWORD
        if (SuBrowser->SUCInfo->SupervisorPwdFlag == 0 || SuBrowser->SUCInfo->UserPwdFlag == 0) {
#else
        if (SuBrowser->SUCInfo->SupervisorPwdFlag == 0) {
#endif
          mSupervisorStartFlag = FALSE;
        }
        SecurityDataInfo->StringLength = MAX_PASSWORD_NUMBER * sizeof(UINT16);
        EfiZeroMem (SecurityDataInfo->InputString,SecurityDataInfo->StringLength);
        EfiStrCpy (SecurityDataInfo->InputString, CurrentPassword);
        SecurityDataInfo->Flag = CHANGE_PASSWORD;
        if (SupervisorOrUser == SUPERVISOR_FLAG) {
          SuBrowser->SCBuffer->SupervisorFlag = 1;
#ifdef SUPPORT_USER_PASSWORD
        } else {
          SuBrowser->SCBuffer->UserFlag = 1;
        }
#else
        }
#endif
      } else {
        mSupervisorStartFlag = FALSE;
        SecurityDataInfo->Flag = DISABLE_PASSWORD;
        if (SupervisorOrUser == SUPERVISOR_FLAG) {
          SuBrowser->SCBuffer->SupervisorFlag = 0;
#ifdef SUPPORT_USER_PASSWORD
          SuBrowser->SCBuffer->UserFlag = 0;
          SuBrowser->SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
        } else {
          SuBrowser->SCBuffer->UserFlag = 0;
        }
#else
      }
#endif
      }

      PasswordState = 2;
      break;
    }
  }

  return  EFI_SUCCESS;
}

#ifdef TPM_SUPPORT
STATIC
EFI_STATUS
UpdateTpmStatus (
  IN  EFI_HII_PROTOCOL                *Hii,
  IN  EFI_HII_HANDLE                  HiiHandle
  )
/*++

Routine Description:

  Update the TPM Status.

Arguments:

  Hii        - Pointer to EFI HII protocol instance.
  HiiHandle  - Hii hanlde for security page.

Returns:

  EFI_SUCCESS - The updating of TPM Status is successful.

--*/
{
  EFI_STATUS                            Status;
  EFI_TCG_PROTOCOL                      *TcgProtocol;
  TCG_EFI_BOOT_SERVICE_CAPABILITY       TcgCapability;
  UINT8                                 TpmDisable;
  UINT8                                 TpmDeactivated;
  STRING_REF                            TokenToUpdate;
  CHAR16                                *TpmStatus = NULL;
  UINTN                                  TotalLanguageCount;
  UINTN                                  LanguageCount;
  CHAR16                                 *LanguageString;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  if (EFI_ERROR (Status)) {

    return Status;
  }
  //
  // Get TPM capability
  //
  EfiZeroMem (&TcgCapability, sizeof (TcgCapability));
  TcgCapability.Size = sizeof (TcgCapability);
  Status = TcgProtocol->StatusCheck (TcgProtocol, &TcgCapability, NULL, NULL, NULL);
  if (EFI_ERROR (Status)) {

    return Status;
  }
  // 
  // In TPMPresentFlag :
  // bit 1 represents TPM enable state (1-disable, 0 - enable)
  //
  TpmDeactivated = (UINT8)(TcgCapability.TPMDeactivatedFlag);
  TpmDisable = ((UINT8)(TcgCapability.TPMPresentFlag) & 0x02) >> 1;
  Status = SetupUtilityLibGetLangDatabase (&TotalLanguageCount, &LanguageString);
  ASSERT_EFI_ERROR (Status);
  for (LanguageCount = 0; LanguageCount < TotalLanguageCount; LanguageCount++) {
    if (TpmDeactivated == 0x00) {
      if (TpmDisable == 0x00) {
        TpmStatus = SetupUtilityLibGetTokenStringByLanguage (
                      HiiHandle,
                      STRING_TOKEN (STR_ENABLED_AND_ACTIVE_TEXT),
                      &LanguageString[LanguageCount * 3]
                      );
        if (TpmStatus == NULL) {
          continue;
        }
      } else if (TpmDisable == 0x01) {
        TpmStatus = SetupUtilityLibGetTokenStringByLanguage (
                      HiiHandle,
                      STRING_TOKEN (STR_DISABLED_AND_ACTIVE_TEXT),
                      &LanguageString[LanguageCount * 3]
                      );
        if (TpmStatus == NULL) {
          continue;
        }
      }
    } else if (TpmDeactivated == 0x01) {
      if (TpmDisable == 0x00) {
        TpmStatus = SetupUtilityLibGetTokenStringByLanguage (
                      HiiHandle,
                      STRING_TOKEN (STR_ENABLED_AND_INACTIVE_TEXT),
                      &LanguageString[LanguageCount * 3]
                      );
        if (TpmStatus == NULL) {
          continue;
        }
      } else if (TpmDisable == 0x01) {
        TpmStatus = SetupUtilityLibGetTokenStringByLanguage (
                      HiiHandle,
                      STRING_TOKEN (STR_DISABLED_AND_INACTIVE_TEXT),
                      &LanguageString[LanguageCount * 3]
                      );
        if (TpmStatus == NULL) {
          continue;
        }
      }
    }

    TokenToUpdate = STRING_TOKEN (STR_TPM_STATUS_STRING2);
    Hii->NewString (Hii, &LanguageString[LanguageCount * 3], HiiHandle, &TokenToUpdate, TpmStatus);
    gBS->FreePool (TpmStatus);
    TpmStatus = NULL;
  }

  return EFI_SUCCESS;
}
#endif

EFI_STATUS
InitSecurityMenu (
  IN EFI_HII_HANDLE       HiiHandle
  )
/*++

Routine Description:

  Initialize security menu for setuputility use

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SYSTEM_CONFIGURATION                      *SCBuffer;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  SEC_HDD_PASSWORD_STRUCT                   *SavePasswordPoint;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }
#ifdef TPM_SUPPORT
  UpdateTpmStatus (SuBrowser->Hii, HiiHandle);
#endif
  SUCInfo  = SuBrowser->SUCInfo;
  SCBuffer = SuBrowser->SCBuffer;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  &SUCInfo->SysPasswordService
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Check password
    //
    SUCInfo->SupervisorPassword = NULL;
    SUCInfo->UserPassword       = NULL;
    Status = PasswordCheck (
               SUCInfo,
               SCBuffer
               );
    ASSERT_EFI_ERROR (Status);
  }

  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  &SUCInfo->PswdConsoleService
                  );
  if (!EFI_ERROR (Status)) {
    SUCInfo->HarddiskPassword   = NULL;
    SUCInfo->NumOfHarddiskPswd  = 0;

    Status = HddPasswordCheck (
               HiiHandle,
               INTO_SCU,
               SCBuffer,
               &SavePasswordPoint
               );

    if (Status == EFI_SUCCESS) {
      UpdateHddPasswordLabel (
        SuBrowser->Hii,
        HiiHandle,
        SUCInfo->HarddiskPassword,
        SUCInfo->NumOfHarddiskPswd
        );
    } else {
      UpdateHddPasswordLabel (
        SuBrowser->Hii,
        HiiHandle,
        NULL,
        0
        );
    }

  }

  return EFI_SUCCESS;
}

EFI_STATUS
UpdatePasswordState (
  EFI_HII_PROTOCOL                      *Hii,
  EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update the string token for password state.

Arguments:

  Hii        - Point to EFI HII protocol instance.
  HiiHandle  - HII hanlde for security Menu.

Returns:

  EFI_SUCCESS - The updating of password state is successful.
  other       - Error occurred during execution.

--*/
{
  CHAR16                                *NewString;
  STRING_REF                            TokenToUpdate;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!(SuBrowser->SUCInfo->SupervisorPwdFlag)) {
    NewString = SetupUtilityLibGetTokenString (
                  HiiHandle,
                  STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                  );
  } else {
    NewString = SetupUtilityLibGetTokenString (
                  HiiHandle,
                  STRING_TOKEN (STR_INSTALLED_TEXT)
                  );
  }

  TokenToUpdate = STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2);
  Hii->NewString (
         Hii,
         NULL,
         HiiHandle,
         &TokenToUpdate,
         NewString
         );
#ifdef SUPPORT_USER_PASSWORD
  if (!SuBrowser->SUCInfo->UserPwdFlag) {
    gBS->FreePool (NewString);
    NewString = SetupUtilityLibGetTokenString (
                HiiHandle,
                STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                );
  }

  TokenToUpdate = STRING_TOKEN (STR_USER_PASSWORD_STRING2);
  Hii->NewString (
         Hii,
         NULL,
         HiiHandle,
         &TokenToUpdate,
         NewString
         );
#endif
  gBS->FreePool (NewString);

  return EFI_SUCCESS;
}

EFI_STATUS
SecurityCallbackRoutine (
  IN EFI_FORM_CALLBACK_PROTOCOL       *This,
  IN UINT16                           KeyValue,
  IN EFI_IFR_DATA_ARRAY               *Data,
  OUT EFI_HII_CALLBACK_PACKET         **Packet
  )
/*++

Routine Description:

  This is the Security page callback function that is called to provide results data to the driver.
  This dataconsists of a unique key which is used to identify what data is either being passed
  back or being asked for.

Arguments:

  This      -  Pointer to Form Callback protocol instance.
  KeyValue  -  A unique value which is sent to the original exporting driver so that it
               can identify the type of data to expect.  The format of the data tends to
               vary based on the op-code that geerated the callback.
  Data      -  A pointer to the data being sent to the original exporting driver.
  Packet    -  The result data returned.

Returns:

  EFI_SUCCESS  -  Must return success upon completion.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                            Status;
  BOOLEAN                               UsePacket;
  BOOLEAN                               PState;
  CHAR16                                *NewString;
  CHAR16                                *StringPtr;
  SYSTEM_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_PROTOCOL                      *Hii;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
#ifdef SUPPORT_USER_PASSWORD
  EFI_INPUT_KEY                         Key;
#endif
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  EFI_FORM_BROWSER_PROTOCOL             *Browser;
  OEM_SERVICES_PROTOCOL                 *OemServices;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CallbackInfo = EFI_SECURITY_CALLBACK_INFO_FROM_THIS (This);
  MyIfrNVData = (SYSTEM_CONFIGURATION *) Data->NvRamMap;

  Status = EFI_SUCCESS;
  UsePacket = FALSE;
  StringPtr = NULL;
  Hii       = SuBrowser->Hii;
  HiiHandle = CallbackInfo->HiiHandle;
  SUCInfo   = SuBrowser->SUCInfo;
  Browser   = SuBrowser->Browser;

  if (Packet != NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (EFI_HII_CALLBACK_PACKET) + 2,
                    Packet
                    );

    EfiZeroMem (*Packet, sizeof (EFI_HII_CALLBACK_PACKET) + 2);

    (*Packet)->DataArray.EntryCount = 1;
    (*Packet)->DataArray.NvRamMap = NULL;
  }
  OemServices = NULL;
  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );

  switch (KeyValue) {

  case KEY_SUPERVISOR_PASSWORD:
    Status = PasswordCallback (
               SUPERVISOR_FLAG,
               Data,
               Packet,
               &PState,
               SUCInfo->SysPasswordService,
               SUCInfo->SupervisorPassword
               );
    if (PState) {
      StringPtr = SetupUtilityLibGetTokenString (
                    HiiHandle,
                    STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2)
                    );
      if (SUCInfo->SupervisorPassword->Flag == DISABLE_PASSWORD) {
        NewString = SetupUtilityLibGetTokenString (
                      HiiHandle,
                      STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                      );
        PState = FALSE;
#ifdef SUPPORT_USER_PASSWORD
        if (SUCInfo->UserPassword->SystemPasswordStatus == EFI_SUCCESS) {
          SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
        }
#endif
      } else {
        NewString = SetupUtilityLibGetTokenString (
                      HiiHandle,
                      STRING_TOKEN (STR_INSTALLED_TEXT)
                      );
        PState = TRUE;
#ifdef SUPPORT_USER_PASSWORD
        //
        // If the new supervisor password is the same as user password, then clear user password.
        //
        if (SUCInfo->UserPassword->Flag == NO_ACCESS_PASSWORD) {
          if (SUCInfo->UserPassword->SystemPasswordStatus == EFI_SUCCESS) {
            Status = SUCInfo->SysPasswordService->CheckPassword (
                                                    SUCInfo->SysPasswordService,
                                                    SUCInfo->SupervisorPassword->InputString,
                                                    SUCInfo->SupervisorPassword->StringLength,
                                                    SystemUser
                                                    );

            if (Status == EFI_SUCCESS) {
              SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
              StringPtr = SetupUtilityLibGetTokenString (
                            HiiHandle,
                            STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                            );
              UpdateUserTags (
                Hii,
                HiiHandle,
                FALSE,
                StringPtr,
                MyIfrNVData
                );
              SuBrowser->SUCInfo->UserPwdFlag = FALSE;
            }
          }
        } else if (SUCInfo->UserPassword->Flag == CHANGE_PASSWORD) {
          if (EfiStrCmp (SUCInfo->SupervisorPassword->InputString,
                 SUCInfo->UserPassword->InputString) == 0) {
            SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
            StringPtr = SetupUtilityLibGetTokenString (
                          HiiHandle,
                          STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                          );
            UpdateUserTags (Hii, HiiHandle, FALSE, StringPtr, MyIfrNVData);
            SuBrowser->SUCInfo->UserPwdFlag = FALSE;
          }
        }
#endif
      }

      //
      // If Supervisor Password text op-code is not the same as new state, update the string
      //
      if (EfiStrCmp (StringPtr, NewString) != 0) {
        UpdateSupervisorTags (Hii, HiiHandle, PState, NewString, MyIfrNVData);
        SuBrowser->SUCInfo->SupervisorPwdFlag = !(SuBrowser->SUCInfo->SupervisorPwdFlag);
      }

      gBS->FreePool (NewString);
      gBS->FreePool (StringPtr);
      Status = EFI_SUCCESS;
    }
    SUCInfo->DoRefresh = TRUE;
    break;

#ifdef SUPPORT_USER_PASSWORD
  case KEY_USER_PASSWORD:
    Status = PasswordCallback (
               USERPASSWORD_FLAG,
               Data,
               Packet,
               &PState,
               SUCInfo->SysPasswordService,
               SUCInfo->UserPassword
               );

    if (PState) {
      StringPtr = SetupUtilityLibGetTokenString (
                    HiiHandle,
                    STRING_TOKEN (STR_USER_PASSWORD_STRING2)
                    );
      if (SUCInfo->UserPassword->Flag == DISABLE_PASSWORD) {
        NewString = SetupUtilityLibGetTokenString (
                      HiiHandle,
                      STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                      );
        PState = FALSE;
        SuBrowser->SUCInfo->UserPwdFlag = FALSE;
      } else {
        if (SUCInfo->SupervisorPassword->Flag == NO_ACCESS_PASSWORD) {
          if (SUCInfo->SupervisorPassword->SystemPasswordStatus == EFI_SUCCESS) {
            Status = SUCInfo->SysPasswordService->CheckPassword (
                                                    SUCInfo->SysPasswordService,
                                                    SUCInfo->UserPassword->InputString,
                                                    SUCInfo->UserPassword->StringLength,
                                                    SystemSupervisor
                                                    );

            if (Status == EFI_SUCCESS) {
              SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
              Status = EFI_SECURITY_VIOLATION;
              SuBrowser->SUCInfo->UserPwdFlag = FALSE;
              break;
            }
          }
        } else if (SUCInfo->SupervisorPassword->Flag == CHANGE_PASSWORD)  {
          if (EfiStrCmp (SUCInfo->SupervisorPassword->InputString, SUCInfo->UserPassword->InputString) == 0) {
            SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
            Status = EFI_SECURITY_VIOLATION;
            SuBrowser->SUCInfo->UserPwdFlag = FALSE;
            break;
          }
        }
        NewString = SetupUtilityLibGetTokenString (
                      HiiHandle,
                      STRING_TOKEN (STR_INSTALLED_TEXT)
                      );
        PState = TRUE;
        SuBrowser->SUCInfo->UserPwdFlag = TRUE;
      }

      //
      // If User Password text op-code is not the same as new state, update the string
      //
      if (EfiStrCmp (StringPtr, NewString) != 0) {
        UpdateUserTags (Hii, HiiHandle, PState, NewString, MyIfrNVData);
      }

      gBS->FreePool (NewString);
      gBS->FreePool (StringPtr);
      Status = EFI_SUCCESS;
    }
    SUCInfo->DoRefresh = TRUE;
    break;

  case KEY_CLEAR_USER_PASSWORD:
    StringPtr = SetupUtilityLibGetTokenString (
                  HiiHandle,
                  STRING_TOKEN (STR_CLEAR_USER_PASSWORD_STRING)
                  );
    Browser->CreatePopUp (
               0,
               FALSE,
               0,
               NULL,
               &Key,
               StringPtr
               );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
      NewString = SetupUtilityLibGetTokenString (
                    HiiHandle,
                    STRING_TOKEN (STR_NOT_INSTALLED_TEXT)
                    );
      UpdateUserTags (Hii, HiiHandle, FALSE, NewString, MyIfrNVData);
      gBS->FreePool (NewString);
      SuBrowser->SUCInfo->UserPwdFlag = FALSE;
    }

    gBS->FreePool (StringPtr);
    break;
#endif

  case KEY_LOCK_ALL_HDD_SECURITY:
    Status = SetAllHarddiskPswd (
               This,
               USER_PSW,
               Hii,
               HiiHandle,
               Data,
               Packet,
               &PState,
               SUCInfo->HarddiskPassword,
               SUCInfo->NumOfHarddiskPswd
               );
    break;

  case KEY_LOCK_ALL_MASTER_HDD_SECURITY:
    Status = SetAllHarddiskPswd (
               This,
               MASTER_PSW,
               Hii,
               HiiHandle,
               Data,
               Packet,
               &PState,
               SUCInfo->HarddiskPassword,
               SUCInfo->NumOfHarddiskPswd
               );
    break;

  case KEY_HDD_PSW_KEY_NUM_0:
  case KEY_HDD_PSW_KEY_NUM_1:
  case KEY_HDD_PSW_KEY_NUM_2:
  case KEY_HDD_PSW_KEY_NUM_3:
  case KEY_HDD_PSW_KEY_NUM_4:
  case KEY_HDD_PSW_KEY_NUM_5:
  case KEY_HDD_PSW_KEY_NUM_6:
  case KEY_HDD_PSW_KEY_NUM_7:
  case KEY_HDD_PSW_KEY_NUM_8:
  case KEY_HDD_PSW_KEY_NUM_9:
  case KEY_HDD_PSW_KEY_NUM_A:
  case KEY_HDD_PSW_KEY_NUM_B:
  case KEY_HDD_PSW_KEY_NUM_C:
  case KEY_HDD_PSW_KEY_NUM_D:
  case KEY_HDD_PSW_KEY_NUM_E:
  case KEY_HDD_PSW_KEY_NUM_F:
    Status = HddPasswordCallback (
               This,
               Hii,
               HiiHandle,
               Data,
               Packet,
               &PState,
               (KeyValue & 0x0f),
               SUCInfo->PswdConsoleService,
               SUCInfo->HarddiskPassword,
               USER_PSW,
               SUCInfo->NumOfHarddiskPswd
               );
    break;

  default:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->AdvancedHiiHandle,
                          KeyValue,
                          Data,
                          &UsePacket,
                          Packet
                          );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if ( OemServices != NULL ) {
      Status = OemServices->Funcs[COMMON_SCU_CALLBACK_FUNCTION] (
                              OemServices,
                              COMMON_SCU_CALLBACK_FUNCTION_ARG_COUNT,
                              This,
                              KeyValue,
                              (EFI_IFR_DATA_ARRAY *) Data,
                              Packet,
                              MyIfrNVData,
                              &(SuBrowser->SUCInfo->AdvancedHiiHandle),
                              &UsePacket,
                              SuBrowser->Browser,
                              NULL
                              );
    }
    break;
  }

  if (!UsePacket && Packet != NULL) {
    gBS->FreePool (*Packet);
    *Packet = NULL;
  }

  return Status;
}

EFI_STATUS
InstallSecurityCallbackRoutine (
  IN EFI_HII_HANDLE                         HiiHandle
)
/*++

Routine Description:

  Install Security Callback routine.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_CALLBACK_INFO                         *SecurityCallBackInfo;
  EFI_HII_UPDATE_DATA                       UpdateData;
  EFI_STATUS                                Status;
  EFI_HII_PROTOCOL                          *Hii;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SecurityCallBackInfo = EfiLibAllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (SecurityCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SecurityCallBackInfo->Signature               = EFI_SECURITY_MENU_SIGNATURE;
  SecurityCallBackInfo->DriverCallback.NvRead   = SuBrowser->NvRead;
  SecurityCallBackInfo->DriverCallback.NvWrite  = SuBrowser->NvWrite;
  SecurityCallBackInfo->DriverCallback.Callback = SecurityCallbackRoutine;

  //
  // Install protocol interface
  //
  SecurityCallBackInfo->CallbackHandle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &SecurityCallBackInfo->CallbackHandle,
                  &gEfiFormCallbackProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &SecurityCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Hii = SuBrowser->Hii;

  EfiZeroMem (&UpdateData, sizeof (EFI_HII_UPDATE_DATA));

  UpdateData.FormSetUpdate        = TRUE;
  UpdateData.FormCallbackHandle   = (EFI_PHYSICAL_ADDRESS) SecurityCallBackInfo->CallbackHandle;
  SecurityCallBackInfo->HiiHandle = HiiHandle;

  Hii->UpdateForm (
         Hii,
         HiiHandle,
         0,
         TRUE,
         &UpdateData
         );

  Status = InitSecurityMenu (HiiHandle);

  return Status;
}


