//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
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
#include EFI_PROTOCOL_CONSUMER (TrEEProtocol)

#define SWAP_BYTE_16(a) ((a << 8) | (a >> 8))
#define SWAP_BYTE_32(a) (UINT32)(((UINT32)SWAP_BYTE_16 ((UINT16)(a)) << 16) | ((UINT32)SWAP_BYTE_16 ((UINT16)(a >> 16))))

BOOLEAN                     mSupervisorStartFlag  = TRUE;
EFI_CALLBACK_INFO           *mSecurityCallBackInfo;

STATIC
EFI_STATUS
ValidatePassword (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  BOOLEAN                           SupervisorOrUser,
  IN  SYS_PASSWORD_INFO_DATA            *SysPasswordDataInfo,
  IN  CHAR16                            *Password
  )
{
  EFI_STATUS                         Status;

  //
  // Password disable in this boot, need check again return directly
  //
  if (SysPasswordDataInfo->Flag == DISABLE_PASSWORD) {
    return EFI_SUCCESS;
  }
  //
  // Check Password exists or not.
  //
  Status = PswdConsoleService->GetStatus (
                                 PswdConsoleService,
                                 SupervisorOrUser
                                 );

  SysPasswordDataInfo->SystemPasswordStatus = Status;

  //
  // Old password already exist
  //
  if (SysPasswordDataInfo->SystemPasswordStatus == EFI_SUCCESS) {
    //
    // Enter old password to confirmation.
    //
    if (SysPasswordDataInfo->Flag == CHANGE_PASSWORD) {
      if (EfiStrCmp(Password, SysPasswordDataInfo->InputString) != 0x00) {
        return EFI_NOT_READY;
      } else {
        return EFI_SUCCESS;
      }
    }
    Status = PswdConsoleService->CheckPassword (
                                   PswdConsoleService,
                                   Password,
                                   (SysPasswordMaxNumber (PswdConsoleService) + 1) * sizeof(UINT16),
                                   SupervisorOrUser
                                   );

    if ((Status != EFI_SUCCESS)) {
      return EFI_NOT_READY;
    }
  } else if (SysPasswordDataInfo->Flag == CHANGE_PASSWORD) {
    if (EfiStrCmp(Password, SysPasswordDataInfo->InputString) != 0x00) {
      return EFI_NOT_READY;
    } else {
      return EFI_SUCCESS;
    }
  }

  return  EFI_SUCCESS;
}

STATIC
EFI_STATUS
SetPassword (
  IN  BOOLEAN                           SupervisorOrUser,
  IN  SYS_PASSWORD_INFO_DATA            *SysPasswordDataInfo,
  IN  CHAR16                            *Password
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Password[0] != 0x00) {
#ifdef SUPPORT_USER_PASSWORD
    if (SuBrowser->SUCInfo->SupervisorPwdFlag == 0 || SuBrowser->SUCInfo->UserPwdFlag == 0) {
#else
    if (SuBrowser->SUCInfo->SupervisorPwdFlag == 0) {
#endif
      mSupervisorStartFlag = FALSE;
    }
    SysPasswordDataInfo->StringLength = (SysPasswordMaxNumber (SuBrowser->SUCInfo->SysPasswordService) + 1) * sizeof(UINT16);
    EfiZeroMem(SysPasswordDataInfo->InputString, SysPasswordDataInfo->StringLength);
    EfiStrCpy(SysPasswordDataInfo->InputString, Password);
    SysPasswordDataInfo->Flag = CHANGE_PASSWORD;
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
    SysPasswordDataInfo->Flag = DISABLE_PASSWORD;
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
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CheckSupervisorPassword (
  IN  SETUP_UTILITY_BROWSER_DATA        *SuBrowser,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  CHAR16                            *Password,
  IN OUT UINTN                          *PasswordState
  )
{
  EFI_STATUS                     Status;

  if (SuBrowser->SUCInfo->SupervisorPassword->Flag != CHANGE_PASSWORD) {

    Status = PswdConsoleService->GetStatus (
                                   PswdConsoleService,
                                   SystemSupervisor
                                   );
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }

    Status = PswdConsoleService->CheckPassword (
                                   PswdConsoleService,
                                   Password,
                                   SysPasswordMaxNumber (PswdConsoleService) * sizeof(UINT16),
                                   SystemSupervisor
                                   );

    if (!EFI_ERROR(Status)) {
      *PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}

#ifdef SUPPORT_USER_PASSWORD
STATIC
EFI_STATUS
CheckUserPassword (
  IN  SETUP_UTILITY_BROWSER_DATA        *SuBrowser,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  CHAR16                            *Password,
  IN OUT UINTN                          *PasswordState
  )
{
  EFI_STATUS                     Status;

  if ((SuBrowser->SUCInfo->UserPassword->Flag != DISABLE_PASSWORD) && (Password[0] != 0x00)) {
    Status = PswdConsoleService->CheckPassword (
                                   PswdConsoleService,
                                   Password,
                                   SysPasswordMaxNumber (PswdConsoleService) * sizeof(UINT16),
                                   SystemUser
                                   );
    if ((!EFI_ERROR(Status)) ||
        (EfiStrCmp (Password, SuBrowser->SUCInfo->UserPassword->InputString) == 0)) {
      *PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}
#endif

STATIC
EFI_STATUS
PasswordCallback (
  IN  BOOLEAN                           SupervisorOrUser,
  IN  UINT8                             Type,
  IN  EFI_IFR_TYPE_VALUE                *Value,
  OUT EFI_BROWSER_ACTION_REQUEST        *ActionRequest,
  OUT BOOLEAN                           *PState,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SYS_PASSWORD_INFO_DATA            *SysPasswordDataInfo
  )
/*++

Routine Description:

Arguments:

  Data -                 A pointer to the data being sent to the original exporting driver.
  Packet -               EFI_HII_CALLBACK_PACKET
  *PState -              Password access is success or not, if access success then return TRUE.
  PswdConsoleService -   Password console service ptr
  SysPasswordDataInfo -  Security information

Returns:

  EFI_SUCCESS -          Password callback success.
  EFI_Already_started -  Already password in SetupUtility.
  EFI_NOT_READY -        Password confirm error.
  EFI_INVALID_PARAMETER - Input parameter is invalid

--*/
{
  STATIC UINTN                   PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
  EFI_STATUS                     Status;
  CHAR16                         *Password;
  SETUP_UTILITY_BROWSER_DATA     *SuBrowser;



  *PState = FALSE;
  if (Type != EFI_IFR_TYPE_STRING) {
    return  EFI_INVALID_PARAMETER;
  }
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value->string == 0) {
    //
    // If Value->String == 0, only need reset the state machine to BROWSER_STATE_VALIDATE_PASSWORD
    //
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    return EFI_SUCCESS;
  }

  Password = NULL;
  GetStringFromHandle (
    SuBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle,
    Value->string,
    &Password
    );
  //
  // When try to set a new password, user will be chanlleged with old password.
  // The Callback is responsible for validating old password input by user,
  // If Callback return EFI_SUCCESS, it indicates validation pass.
  //
  switch (PasswordState) {

  case BROWSER_STATE_VALIDATE_PASSWORD:
    Status = ValidatePassword (PswdConsoleService, SupervisorOrUser, SysPasswordDataInfo, Password);
    if (Status == EFI_SUCCESS) {
      PasswordState = BROWSER_STATE_SET_PASSWORD;
    }
    break;

  case BROWSER_STATE_SET_PASSWORD:
#ifdef SUPPORT_USER_PASSWORD
    if ((SupervisorOrUser == USERPASSWORD_FLAG) &&
        (Password[0] != 0x00) &&
        EfiStrCmp (SuBrowser->SUCInfo->SupervisorPassword->InputString, Password) == 0) {
      PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
      return EFI_UNSUPPORTED;
    } else {
      if (SupervisorOrUser == USERPASSWORD_FLAG) {
        Status = CheckSupervisorPassword (SuBrowser, PswdConsoleService, Password, &PasswordState);
      } else {
        Status = CheckUserPassword (SuBrowser, PswdConsoleService, Password, &PasswordState);
      }
      if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
      }
    }
#endif
    Status = SetPassword (SupervisorOrUser, SysPasswordDataInfo, Password);
    *PState = TRUE;
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    break;

  default:
    Status = EFI_NOT_AVAILABLE_YET;
    break;
  }

  if (Password != NULL) {
    gBS->FreePool (Password);
  }

  return Status;
}

EFI_STATUS
GetTpm12State (
  IN  EFI_TCG_PROTOCOL          *TcgProtocol,
  OUT BOOLEAN                   *TpmDisabled,
  OUT BOOLEAN                   *TpmDeactivated
  )
/*++

Routine Description:

  Get current state of TPM device.

Arguments:

  TcgProtocol     - Point to EFI_TCG_PROTOCOL instance.
  TpmDisabled     - Flag to indicate TPM is disenabled or not.
  TpmDeactivated  - Flag to indicate TPM is deactivated or not.

Returns:

  EFI_SUCCESS            -   State is successfully returned.
  EFI_INVALID_PARAMETER  -   Invalid input.
  EFI_DEVICE_ERROR       -   Failed to get TPM response.

--*/
{
  EFI_STATUS                    Status;
  TPM_RSP_COMMAND_HDR           *TpmRsp;
  UINT32                        TpmSendSize;
  TPM_PERMANENT_FLAGS           *TpmPermanentFlags;
  UINT8                         CmdBuf[64];
  UINT16                        RspHeader;

  if (TcgProtocol == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((TpmDisabled == NULL) && (TpmDeactivated == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Get TPM Permanent flags
  //
  TpmSendSize           = sizeof (TPM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
  *(UINT16*)&CmdBuf[0]  = SWAP_BYTE_16 (TPM_TAG_RQU_COMMAND);
  *(UINT32*)&CmdBuf[2]  = SWAP_BYTE_32 (TpmSendSize);
  *(UINT32*)&CmdBuf[6]  = SWAP_BYTE_32 (TPM_ORD_GetCapability);

  *(UINT32*)&CmdBuf[10] = SWAP_BYTE_32 (TPM_CAP_FLAG);
  *(UINT32*)&CmdBuf[14] = SWAP_BYTE_32 (sizeof (TPM_CAP_FLAG_PERMANENT));
  *(UINT32*)&CmdBuf[18] = SWAP_BYTE_32 (TPM_CAP_FLAG_PERMANENT);

  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          TpmSendSize,
                          CmdBuf,
                          sizeof (CmdBuf),
                          CmdBuf
                          ); 
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  RspHeader = SWAP_BYTE_16 (TPM_TAG_RSP_COMMAND);
  TpmRsp    = (TPM_RSP_COMMAND_HDR *) &CmdBuf[0];
  if ((TpmRsp->tag != RspHeader) || (TpmRsp->returnCode != 0)) {
    return EFI_DEVICE_ERROR;
  }

  TpmPermanentFlags = (TPM_PERMANENT_FLAGS *) &CmdBuf[sizeof (TPM_RSP_COMMAND_HDR) + sizeof (UINT32)];

  if (TpmDisabled != NULL) {
    *TpmDisabled = (BOOLEAN) TpmPermanentFlags->disable;
  }

  if (TpmDeactivated != NULL) {
    *TpmDeactivated = (BOOLEAN) TpmPermanentFlags->deactivated;
  }
 
  return EFI_SUCCESS;  
}

STATIC
VOID
UpdateTpmStatus (
  IN  EFI_HII_HANDLE                  HiiHandle,
  IN  SYSTEM_CONFIGURATION            *SetupConfig
  )
/*++

Routine Description:

  Update the TPM Status.

Arguments:

  HiiHandle    - Hii hanlde for security page.
  SetupConfig  - Pointer to SYSTEM_CONFIGURATION struct

Returns:

  None.

--*/
{
  EFI_STATUS                            Status;
  EFI_TCG_PROTOCOL                      *TcgProtocol;
  EFI_TREE_PROTOCOL                     *TrEEProtocol;
  BOOLEAN                               TpmDisabled;
  BOOLEAN                               TpmDeactivated;
  TREE_BOOT_SERVICE_CAPABILITY          ProtocolCapability;
  EFI_STRING_ID                         TpmStateToken;
  UINT8                                 TpmDevice;
  EFI_HII_UPDATE_DATA                   UpdateData;
  UINTN                                 VariableSize;
  UINT8                                 Tpm2Disabled;
  
  TpmDisabled    = FALSE;
  TpmDeactivated = FALSE;
  TpmDevice      = SetupConfig->TpmDevice;
  EfiZeroMem (&ProtocolCapability, sizeof (ProtocolCapability));
  
  switch (TpmDevice) {
   
  case TPM_DEVICE_1_2:
    //
    // Verify if TPM1.2 is present
    //
    Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID**)&TcgProtocol);
    if (EFI_ERROR (Status)) {
      TpmDevice = TPM_DEVICE_NULL;
      break;
    }
    
    Status = GetTpm12State (TcgProtocol, &TpmDisabled, &TpmDeactivated);
    if (EFI_ERROR (Status)) {
      TpmDevice = TPM_DEVICE_NULL;
      break;
    }
    
    SetupConfig->GrayoutTpmClear = (TpmDeactivated | TpmDisabled);
    break;
    
  case TPM_DEVICE_2_0:
    //
    // Verify if TPM2.0 is present
    //
    Status = gBS->LocateProtocol (&gEfiTrEEProtocolGuid, NULL, (VOID**)&TrEEProtocol);
    if (EFI_ERROR (Status)) {
      TpmDevice = TPM_DEVICE_NULL;
      break;
    }

    ProtocolCapability.Size = (UINT8) sizeof (ProtocolCapability);
    Status = TrEEProtocol->GetCapability (
                             TrEEProtocol,
                             &ProtocolCapability
                             );
    if (EFI_ERROR (Status)) {
      TpmDevice = TPM_DEVICE_NULL;
    }
    break;

  default:
    TpmDevice = TPM_DEVICE_NULL;
    break;
  }
  
  //
  // Suppress TPM options if TPM is not detected.
  //
  SetupConfig->SuppressTpmOperation = (TpmDevice == TPM_DEVICE_NULL) ? 1 : 0;

  //
  // Don't update TPM state string if TPM is hidden
  //
  if (SetupConfig->TpmHide == 1 && TpmDevice == TPM_DEVICE_NULL) {
    return;
  }
  
  //
  // Allocate space for creation of UpdateData Buffer
  //
  IfrLibInitUpdateData (&UpdateData, 0x1000);
 
  TpmStateToken = 0;
  switch (TpmDevice) {
   
  case TPM_DEVICE_1_2:
    if (TpmDeactivated) {
      if (TpmDisabled) {
        TpmStateToken = STRING_TOKEN (STR_TPM_DISABLED_DEACTIVATED_STRING);
      } else {
        TpmStateToken = STRING_TOKEN (STR_TPM_ENABLED_DEACTIVATED_STRING);
      }
    } else {
      if (TpmDisabled) {
        TpmStateToken = STRING_TOKEN (STR_TPM_DISABLED_ACTIVATED_STRING);
      } else {
        TpmStateToken = STRING_TOKEN (STR_TPM_ENABLED_ACTIVATED_STRING);
      }
    }
    break;

  case TPM_DEVICE_2_0:
    VariableSize = sizeof (Tpm2Disabled);
    Status = gRT->GetVariable (
                    L"Tpm2Disabled",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &VariableSize,
                    &Tpm2Disabled
                    );
    if (EFI_ERROR (Status)) {
      TpmStateToken = STRING_TOKEN (STR_NOT_INSTALLED_TEXT);
      break;
    }
    
    if (Tpm2Disabled == 1) {
      TpmStateToken = STRING_TOKEN (STR_DISABLED_TEXT);
    } else {
      TpmStateToken = STRING_TOKEN (STR_ENABLED_TEXT);
    }
    break;

  default:
    TpmStateToken = STRING_TOKEN (STR_NOT_INSTALLED_TEXT);
    break;
  }

  CreateTextOpCode (STRING_TOKEN (STR_TPM_STATUS_STRING), 0, TpmStateToken, &UpdateData);

  IfrLibUpdateForm (HiiHandle, NULL, ROOT_FORM_ID, TPM_STATE_LABEL, TRUE, &UpdateData);

  IfrLibFreeUpdateData (&UpdateData);
  return;
}

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

  SUCInfo  = SuBrowser->SUCInfo;
  SCBuffer = SuBrowser->SCBuffer;

#if defined(TPM_SUPPORT) || defined(TPM2_SUPPORT)
  UpdateTpmStatus (HiiHandle, SCBuffer);
#endif
  
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
    Status = HddPasswordCheck (
               HiiHandle,
               INTO_SCU,
               SCBuffer,
               &SavePasswordPoint
               );

    if (Status == EFI_SUCCESS) {
      UpdateHddPasswordLabel (
        HiiHandle,
        SUCInfo->HarddiskPassword,
        SUCInfo->NumOfHarddiskPswd
        );
    } else {
      UpdateHddPasswordLabel (
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
  EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update the string token for password state.

Arguments:

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
    GetStringFromHandle (
      HiiHandle,
      STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
      &NewString
      );
    SuBrowser->SCBuffer->SupervisorFlag = FALSE;
  } else {
    GetStringFromHandle (
      HiiHandle,
      STRING_TOKEN (STR_INSTALLED_TEXT),
      &NewString
      );
    SuBrowser->SCBuffer->SupervisorFlag = TRUE;
  }

  TokenToUpdate = STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2);
  IfrLibSetString (HiiHandle, TokenToUpdate, NewString);
#ifdef SUPPORT_USER_PASSWORD
  if (!SuBrowser->SUCInfo->UserPwdFlag) {
    gBS->FreePool (NewString);
    GetStringFromHandle (
      HiiHandle,
      STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
      &NewString
      );
    SuBrowser->SCBuffer->UserFlag = FALSE;
  }

  TokenToUpdate = STRING_TOKEN (STR_USER_PASSWORD_STRING2);
  IfrLibSetString (HiiHandle, TokenToUpdate, NewString);
#endif
  gBS->FreePool (NewString);

  return EFI_SUCCESS;
}

EFI_STATUS
SecurityCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
/*++

Routine Description:

  This function is called by the forms browser in response to a user action on a question which has the
  EFI_IFR_FLAG_CALLBACK bit set in the EFI_IFR_QUESTION_HEADER. The user action is specified by Action.
  Depending on the action, the browser may also pass the question value using Type and Value. Upon return,
  the callback function may specify the desired browser action. Callback functions should return
  EFI_UNSUPPORTEDfor all values of Action that they do not support.

Arguments:

  This           - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  Action         - Specifies the type of action taken by the browser.
  QuestionId     - A unique value which is sent to the original exporting driver so that it can identify the
                   type of data to expect. The format of the data tends to vary based on the opcode that
                   generated the callback.
  Type           - The type of value for the question.
  Value          - A pointer to the data being sent to the original exporting driver. The type is specified
                   by Type. Type EFI_IFR_TYPE_VALUE is defined in EFI_IFR_ONE_OF_OPTION.
  ActionRequest  - On return, points to the action requested by the callback function. Type
                   EFI_BROWSER_ACTION_REQUEST is specified in SendForm() in the Form Browser Protocol.

Returns:

  EFI_SUCCESS           -  The callback successfully handled the action.
  EFI_OUT_OF_RESOURCES  -  Not enough storage is available to hold the variable and its data.
  EFI_DEVICE_ERROR      -  The variable could not be saved.
  EFI_UNSUPPORTED       -  The specified Action is not supported by the callback.

--*/
{
  EFI_STATUS                            Status;
  BOOLEAN                               PState;
  CHAR16                                *NewString;
  CHAR16                                *StringPtr;
  SYSTEM_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
#ifdef SUPPORT_USER_PASSWORD
  EFI_INPUT_KEY                         Key;
#endif
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  UINTN                                 BufferSize;
  EFI_GUID                              PasswordConfigGuid = PASSWORD_CONFIGURATION_GUID;

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_SECURITY_CALLBACK_INFO_FROM_THIS (This);
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
    BufferSize = sizeof (SYSTEM_CONFIGURATION);
    Status = SetupVariableConfig (
               NULL,
               NULL,
               &BufferSize,
               (UINT8 *) SuBrowser->SCBuffer,
               (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
               );
    BufferSize = sizeof (PASSWORD_CONFIGURATION);
    Status = SetupVariableConfig (
               &PasswordConfigGuid,
               L"PasswordConfig",
               &BufferSize,
               (UINT8 *) &mPasswordConfig,
               (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
               );
    return Status;
  }

  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = SetupVariableConfig (
             NULL,
             NULL,
             &BufferSize,
             (UINT8 *) SuBrowser->SCBuffer,
             TRUE
             );

  MyIfrNVData = SuBrowser->SCBuffer;
  Status = EFI_SUCCESS;
  StringPtr = NULL;
  HiiHandle = CallbackInfo->HiiHandle;
  SUCInfo   = SuBrowser->SUCInfo;

  switch (QuestionId) {

  case KEY_SUPERVISOR_PASSWORD:
    Status = PasswordCallback (
               SUPERVISOR_FLAG,
               Type,
               Value,
               ActionRequest,
               &PState,
               SUCInfo->SysPasswordService,
               SUCInfo->SupervisorPassword
               );
    if (PState) {
        GetStringFromHandle (
          HiiHandle,
          STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2),
          &StringPtr
          );
      if (SUCInfo->SupervisorPassword->Flag == DISABLE_PASSWORD) {
        GetStringFromHandle (
          HiiHandle,
          STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
          &NewString
          );
        PState = FALSE;
#ifdef SUPPORT_USER_PASSWORD
        if (SUCInfo->UserPassword->SystemPasswordStatus == EFI_SUCCESS) {
          SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
        }
#endif
      } else {
        GetStringFromHandle (
          HiiHandle,
          STRING_TOKEN (STR_INSTALLED_TEXT),
          &NewString
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
              GetStringFromHandle (
                HiiHandle,
                STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                &StringPtr
                );
              UpdateUserTags (
                HiiHandle,
                FALSE,
                StringPtr,
                MyIfrNVData
                );
              SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
            }
          }
        } else if (SUCInfo->UserPassword->Flag == CHANGE_PASSWORD) {
          if (EfiStrCmp (SUCInfo->SupervisorPassword->InputString,
                 SUCInfo->UserPassword->InputString) == 0) {
            SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
            GetStringFromHandle (
              HiiHandle,
              STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
              &StringPtr
              );
            UpdateUserTags (HiiHandle, FALSE, StringPtr, MyIfrNVData);
            SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
          }
        }
#endif
      }

      //
      // If Supervisor Password text op-code is not the same as new state, update the string
      //
      if (EfiStrCmp (StringPtr, NewString) != 0) {
        UpdateSupervisorTags (HiiHandle, PState, NewString, MyIfrNVData);
        SuBrowser->SUCInfo->SupervisorPwdFlag = !(SuBrowser->SUCInfo->SupervisorPwdFlag);
        if (SuBrowser->SUCInfo->SupervisorPwdFlag == FALSE) {
          SuBrowser->SUCInfo->UserPwdFlag = FALSE;
        }
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
               Type,
               Value,
               ActionRequest,
               &PState,
               SUCInfo->SysPasswordService,
               SUCInfo->UserPassword
               );

    if (PState) {
      GetStringFromHandle (
        HiiHandle,
        STRING_TOKEN (STR_USER_PASSWORD_STRING2),
        &StringPtr
        );
      if (SUCInfo->UserPassword->Flag == DISABLE_PASSWORD) {
        GetStringFromHandle (
          HiiHandle,
          STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
          &NewString
          );
        PState = FALSE;
        SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
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
              SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
              break;
            }
          }
        } else if (SUCInfo->SupervisorPassword->Flag == CHANGE_PASSWORD)  {
          if (EfiStrCmp (SUCInfo->SupervisorPassword->InputString,
                 SUCInfo->UserPassword->InputString) == 0) {
            SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
            Status = EFI_SECURITY_VIOLATION;
            SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
            break;
          }
        }
        GetStringFromHandle (
          HiiHandle,
          STRING_TOKEN (STR_INSTALLED_TEXT),
          &NewString
          );
        PState = TRUE;
        SuBrowser->SUCInfo->UserPwdFlag       = TRUE;
      }

      //
      // If User Password text op-code is not the same as new state, update the string
      //
      if (EfiStrCmp (StringPtr, NewString) != 0) {
        UpdateUserTags (HiiHandle, PState, NewString, MyIfrNVData);
      }

      gBS->FreePool (NewString);
      gBS->FreePool (StringPtr);
      Status = EFI_SUCCESS;
    }
    SUCInfo->DoRefresh = TRUE;
    break;

  case KEY_CLEAR_USER_PASSWORD:
    GetStringFromHandle (
      HiiHandle,
      STRING_TOKEN (STR_CLEAR_USER_PASSWORD_STRING),
      &StringPtr
      );
    SuBrowser->OemBrowser2->CreatePopUp (
                               0,
                               FALSE,
                               0,
                               NULL,
                               &Key,
                               StringPtr
                               );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
      GetStringFromHandle (
        HiiHandle,
        STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
        &NewString
        );
      UpdateUserTags (HiiHandle, FALSE, NewString, MyIfrNVData);
      gBS->FreePool (NewString);
      SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
    }

    gBS->FreePool (StringPtr);
    break;
#endif
  case KEY_LOCK_ALL_HDD_SECURITY:
    Status = SetAllHarddiskPswd (
               This,
               USER_PSW,
               HiiHandle,
               Type,
               Value,
               ActionRequest,
               &PState,
               SUCInfo->HarddiskPassword,
               SUCInfo->NumOfHarddiskPswd
               );
    break;

  case KEY_LOCK_ALL_MASTER_HDD_SECURITY:
    Status = SetAllHarddiskPswd (
               This,
               MASTER_PSW,
               HiiHandle,
               Type,
               Value,
               ActionRequest,
               &PState,
               SUCInfo->HarddiskPassword,
               SUCInfo->NumOfHarddiskPswd
               );
    break;

    case KEY_SET_HDD_PSW_NUM_0:
    case KEY_SET_HDD_PSW_NUM_1:
    case KEY_SET_HDD_PSW_NUM_2:
    case KEY_SET_HDD_PSW_NUM_3:
    case KEY_SET_HDD_PSW_NUM_4:
    case KEY_SET_HDD_PSW_NUM_5:
    case KEY_SET_HDD_PSW_NUM_6:
    case KEY_SET_HDD_PSW_NUM_7:
    case KEY_SET_HDD_PSW_NUM_8:
    case KEY_SET_HDD_PSW_NUM_9:
    case KEY_SET_HDD_PSW_NUM_A:
    case KEY_SET_HDD_PSW_NUM_B:
    case KEY_SET_HDD_PSW_NUM_C:
    case KEY_SET_HDD_PSW_NUM_D:
    case KEY_SET_HDD_PSW_NUM_E:
    case KEY_SET_HDD_PSW_NUM_F:
    case KEY_CHECK_HDD_PSW_NUM_0:
    case KEY_CHECK_HDD_PSW_NUM_1:
    case KEY_CHECK_HDD_PSW_NUM_2:
    case KEY_CHECK_HDD_PSW_NUM_3:
    case KEY_CHECK_HDD_PSW_NUM_4:
    case KEY_CHECK_HDD_PSW_NUM_5:
    case KEY_CHECK_HDD_PSW_NUM_6:
    case KEY_CHECK_HDD_PSW_NUM_7:
    case KEY_CHECK_HDD_PSW_NUM_8:
    case KEY_CHECK_HDD_PSW_NUM_9:
    case KEY_CHECK_HDD_PSW_NUM_A:
    case KEY_CHECK_HDD_PSW_NUM_B:
    case KEY_CHECK_HDD_PSW_NUM_C:
    case KEY_CHECK_HDD_PSW_NUM_D:
    case KEY_CHECK_HDD_PSW_NUM_E:
    case KEY_CHECK_HDD_PSW_NUM_F:
      Status = HddPasswordCallback (
                 This,
                 HiiHandle,
                 Type,
                 Value,
                 ActionRequest,
                 &PState,
                 (QuestionId & 0x0f),
                 SUCInfo->PswdConsoleService,
                 SUCInfo->HarddiskPassword,
                 USER_PSW,
                 SUCInfo->NumOfHarddiskPswd
                 );
      break;

    default:
      Status = SuBrowser->HotKeyCallback (
                            This,
                            SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                            Action,
                            QuestionId,
                            Type,
                            Value,
                            ActionRequest
                            );
      break;
  }

  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  SetupVariableConfig (
    NULL,
    NULL,
    &BufferSize,
    (UINT8 *) SuBrowser->SCBuffer,
    FALSE
    );
  BufferSize = sizeof (PASSWORD_CONFIGURATION);
  SetupVariableConfig (
    &PasswordConfigGuid,
    L"PasswordConfig",
    &BufferSize,
    (UINT8 *) &mPasswordConfig,
    FALSE
    );
  return Status;
}


EFI_STATUS
InstallSecurityCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
)
/*++

Routine Description:

  Install Security Callback routine.

Arguments:

  DriverHandle - Specific driver handle for the call back routine
  HiiHandle    - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mSecurityCallBackInfo = EfiLibAllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (mSecurityCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mSecurityCallBackInfo->Signature               = EFI_SECURITY_MENU_SIGNATURE;
  mSecurityCallBackInfo->DriverCallback.ExtractConfig   = SuBrowser->ExtractConfig;
  mSecurityCallBackInfo->DriverCallback.RouteConfig     = SuBrowser->RouteConfig;
  mSecurityCallBackInfo->DriverCallback.Callback = SecurityCallbackRoutine;
  mSecurityCallBackInfo->HiiHandle = HiiHandle;
  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSecurityCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitSecurityMenu (HiiHandle);

  return Status;

}


EFI_STATUS
UninstallSecurityCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS       Status;

  if (mSecurityCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mSecurityCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mSecurityCallBackInfo);
  mSecurityCallBackInfo = NULL;
  return Status;
}
