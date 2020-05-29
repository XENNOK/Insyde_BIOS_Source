/** @file
  Initial and callback functions for Security page

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

#include "Security.h"
#include <Protocol/TcgService.h>
#include <Protocol/TrEEProtocol.h>

BOOLEAN                     mSupervisorStartFlag = TRUE;
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

  if (PswdConsoleService == NULL) {
    return EFI_INVALID_PARAMETER;
  }

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
      if (StrCmp(Password, SysPasswordDataInfo->InputString) != 0x00) {
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
    if (StrCmp(Password, SysPasswordDataInfo->InputString) != 0x00) {
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
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      if (SuBrowser->SUCInfo->SupervisorPwdFlag == 0 || SuBrowser->SUCInfo->UserPwdFlag == 0) {
        mSupervisorStartFlag = FALSE;
      }
    } else {
      if (SuBrowser->SUCInfo->SupervisorPwdFlag == 0) {
        mSupervisorStartFlag = FALSE;
      }
    }

    SysPasswordDataInfo->StringLength = (SysPasswordMaxNumber (SuBrowser->SUCInfo->SysPasswordService) + 1) * sizeof(UINT16);
    ZeroMem(SysPasswordDataInfo->InputString, SysPasswordDataInfo->StringLength);
    StrCpy(SysPasswordDataInfo->InputString, Password);
    SysPasswordDataInfo->Flag = CHANGE_PASSWORD;

    if (SupervisorOrUser == SUPERVISOR_FLAG) {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = 1;
    } else if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 1;
    }
  } else {
    mSupervisorStartFlag = FALSE;
    SysPasswordDataInfo->Flag = DISABLE_PASSWORD;
    if (SupervisorOrUser == SUPERVISOR_FLAG) {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = 0;
      if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
        ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 0;
        SuBrowser->SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
      }
    } else if (PcdGetBool (PcdSysPasswordSupportUserPswd))  {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 0;
    }
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

  if (PswdConsoleService == NULL) {
    return EFI_INVALID_PARAMETER;
  }

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

  if (PswdConsoleService == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((SuBrowser->SUCInfo->UserPassword->Flag != DISABLE_PASSWORD) && (Password[0] != 0x00)) {
    Status = PswdConsoleService->CheckPassword (
                                   PswdConsoleService,
                                   Password,
                                   SysPasswordMaxNumber (PswdConsoleService) * sizeof(UINT16),
                                   SystemUser
                                   );
    if ((!EFI_ERROR(Status)) ||
        (StrCmp (Password, SuBrowser->SUCInfo->UserPassword->InputString) == 0)) {
      *PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}

/**

  @param SupervisorOrUser
  @param Type
  @param Value
  @param ActionRequest
  @param PState
  @param PswdConsoleService      Password console service ptr
  @param SysPasswordDataInfo     Security information

  @retval EFI_SUCCESS            Password callback success.
  @retval EFI_ALREADY_STARTED    Already password in SetupUtility.
  @retval EFI_NOT_READY          Password confirm error.
  @return EFI_INVALID_PARAMETER  Input parameter is invalid

**/
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

  Password = HiiGetString (
               SuBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle,
               Value->string,
               NULL
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
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      if ((SupervisorOrUser == USERPASSWORD_FLAG) &&
          (Password[0] != 0x00) &&
          StrCmp (SuBrowser->SUCInfo->SupervisorPassword->InputString, Password) == 0) {
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
    }
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

/**
  Get current state of TPM device.

  @param[in]   TcgProtocol          Point to EFI_TCG_PROTOCOL instance.
  @param[out]  TpmDisabled          Flag to indicate TPM is disenabled or not.
  @param[out]  TpmDeactivated       Flag to indicate TPM is deactivated or not.

  @retval EFI_SUCCESS               State is successfully returned.
  @retval EFI_INVALID_PARAMETER     Invalid input.
  @retval EFI_DEVICE_ERROR          Failed to get TPM response.
**/
EFI_STATUS
GetTpm12State (
  IN  EFI_TCG_PROTOCOL          *TcgProtocol,
  OUT BOOLEAN                   *TpmDisabled,
  OUT BOOLEAN                   *TpmDeactivated
  )
{
  EFI_STATUS                    Status;
  TPM_RSP_COMMAND_HDR           *TpmRsp;
  UINT32                        TpmSendSize;
  TPM_PERMANENT_FLAGS           *TpmPermanentFlags;
  UINT8                         CmdBuf[64];

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
  *(UINT16*)&CmdBuf[0]  = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  *(UINT32*)&CmdBuf[2]  = SwapBytes32 (TpmSendSize);
  *(UINT32*)&CmdBuf[6]  = SwapBytes32 (TPM_ORD_GetCapability);

  *(UINT32*)&CmdBuf[10] = SwapBytes32 (TPM_CAP_FLAG);
  *(UINT32*)&CmdBuf[14] = SwapBytes32 (sizeof (TPM_CAP_FLAG_PERMANENT));
  *(UINT32*)&CmdBuf[18] = SwapBytes32 (TPM_CAP_FLAG_PERMANENT);

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
  
  TpmRsp = (TPM_RSP_COMMAND_HDR *) &CmdBuf[0];
  if ((TpmRsp->tag != SwapBytes16 (TPM_TAG_RSP_COMMAND)) || (TpmRsp->returnCode != 0)) {
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


/**
  Update the TPM Status.

  @param HiiHandle               Hii hanlde for security page.
  @param KernelConfig            Pointer to KERNEL_CONFIGURATION sturct.

  @retval None.

**/
STATIC
VOID
UpdateTpmStatus (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  EFI_STATUS                            Status;
  EFI_TCG_PROTOCOL                      *TcgProtocol;
  EFI_TREE_PROTOCOL                     *TrEEProtocol;
  BOOLEAN                               TpmDisabled;
  BOOLEAN                               TpmDeactivated;
  EFI_STRING_ID                         TpmStateToken;
  UINT8                                 TpmDevice;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  
  TpmDisabled     = FALSE;
  TpmDeactivated  = FALSE;
  TpmDevice       = KernelConfig->TpmDevice;
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
    
    KernelConfig->GrayoutTpmClear = (TpmDeactivated | TpmDisabled);
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
    break;

  default:
    TpmDevice = TPM_DEVICE_NULL;
    break;
  }
  
  //
  // Suppress TPM options if TPM is not detected.
  //
  KernelConfig->SuppressTpmOperation = (TpmDevice == TPM_DEVICE_NULL) ? 1 : 0;

  //
  // Don't update TPM state string if TPM is hidden
  //
  if (KernelConfig->TpmHide == 1 && TpmDevice == TPM_DEVICE_NULL) {
    return;
  }
  
  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  
  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = TPM_STATE_LABEL;

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
    if (PcdGetBool (PcdTpm2Enable)) {
      TpmStateToken = STRING_TOKEN (STR_ENABLED_TEXT);
    } else {
      TpmStateToken = STRING_TOKEN (STR_DISABLED_TEXT);
    }
    break;

  default:
    TpmStateToken = STRING_TOKEN (STR_NOT_INSTALLED_TEXT);
    break;
  }

  HiiCreateTextOpCode (StartOpCodeHandle,  STRING_TOKEN (STR_TPM_STATUS_STRING), 0, TpmStateToken);

  HiiUpdateForm (
    HiiHandle,
    NULL,
    ROOT_FORM_ID,
    StartOpCodeHandle,
    NULL
    );
  
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  return;
}

/**
  Initialize security menu for setuputility use

  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @retval Others                 Error occurred during execution.

**/
EFI_STATUS
InitSecurityMenu (
  IN EFI_HII_HANDLE       HiiHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  UINT8                                     *SCBuffer;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  SEC_HDD_PASSWORD_STRUCT                   *SavePasswordPoint;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo  = SuBrowser->SUCInfo;
  SCBuffer = SuBrowser->SCBuffer;

  if (PcdGetBool (PcdH2OTpmSupported) || PcdGetBool (PcdH2OTpm2Supported)) {
    UpdateTpmStatus (HiiHandle, (KERNEL_CONFIGURATION *) SCBuffer);
  }

  SUCInfo->SupervisorPassword = NULL;
  SUCInfo->UserPassword       = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SUCInfo->SysPasswordService
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Check password
    //
    Status = PasswordCheck (
               SUCInfo,
               (KERNEL_CONFIGURATION *) SCBuffer
               );
    ASSERT_EFI_ERROR (Status);
  }

  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SUCInfo->PswdConsoleService
                  );
  if (!EFI_ERROR (Status)) {
    Status = HddPasswordCheck (
               HiiHandle,
               INTO_SCU,
               (KERNEL_CONFIGURATION *) SCBuffer,
               &SavePasswordPoint
               );

    UpdateHddPasswordLabel (
      HiiHandle,
      SUCInfo->HarddiskPassword,
      SUCInfo->NumOfHarddiskPswd
      );
  }

  return EFI_SUCCESS;
}

/**
  Update the string token for password state.

  @param HiiHandle               Hii hanlde for security page.

  @retval EFI_SUCCESS            The updating of password state is successful.
  @retval Other                  Error occurred during execution.

**/
EFI_STATUS
UpdatePasswordState (
  EFI_HII_HANDLE                        HiiHandle
  )
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
    NewString = HiiGetString (
                  HiiHandle,
                  STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                  NULL
                  );

    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = FALSE;
  } else {
    NewString = HiiGetString (
                  HiiHandle,
                  STRING_TOKEN (STR_INSTALLED_TEXT),
                  NULL
                  );

    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = TRUE;
  }

  TokenToUpdate = STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2);
  HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
    if (!SuBrowser->SUCInfo->UserPwdFlag) {
      gBS->FreePool (NewString);

      NewString = HiiGetString (
                    HiiHandle,
                    STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                    NULL
                    );

      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = FALSE;
    }

    TokenToUpdate = STRING_TOKEN (STR_USER_PASSWORD_STRING2);
    HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);
  }

  gBS->FreePool (NewString);

  return EFI_SUCCESS;
}

/**
  This function is called by the forms browser in response to a user action on a question which has the
  EFI_IFR_FLAG_CALLBACK bit set in the EFI_IFR_QUESTION_HEADER. The user action is specified by Action.
  Depending on the action, the browser may also pass the question value using Type and Value. Upon return,
  the callback function may specify the desired browser action. Callback functions should return
  EFI_UNSUPPORTEDfor all values of Action that they do not support.

  @param This                    Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action                  Specifies the type of action taken by the browser.
  @param QuestionId              A unique value which is sent to the original exporting driver so that it can identify the
                                 type of data to expect. The format of the data tends to vary based on the opcode that
                                 generated the callback.
  @param Type                    The type of value for the question.
  @param Value                   A pointer to the data being sent to the original exporting driver. The type is specified
                                 by Type. Type EFI_IFR_TYPE_VALUE is defined in EFI_IFR_ONE_OF_OPTION.
  @param ActionRequest           On return, points to the action requested by the callback function. Type
                                 EFI_BROWSER_ACTION_REQUEST is specified in SendForm() in the Form Browser Protocol.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
SecurityCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               PState;
  CHAR16                                *NewString;
  CHAR16                                *StringPtr;
  KERNEL_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  EFI_INPUT_KEY                         Key;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  UINTN                                 BufferSize;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_GUID                              PasswordConfigGuid = PASSWORD_CONFIGURATION_GUID;
  UINTN                                 PasswordConfigBufferSize;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return SecurityCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }
  
  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_CALLBACK_INFO_FROM_THIS (This);
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  PasswordConfigBufferSize = sizeof (PASSWORD_CONFIGURATION);
  if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
                 );
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
                 );
      return Status;
    }
  }

  Status = SetupVariableConfig (
             NULL,
             NULL,
             BufferSize,
             (UINT8 *) SuBrowser->SCBuffer,
             TRUE
             );
  Status = SetupVariableConfig (
             &PasswordConfigGuid,
             L"PasswordConfig",
             PasswordConfigBufferSize,
             (UINT8 *) &mPasswordConfig,
             TRUE
             );

  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
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
        StringPtr = HiiGetString (
                      HiiHandle,
                      STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2),
                      NULL
                      );
      if (SUCInfo->SupervisorPassword->Flag == DISABLE_PASSWORD) {
        NewString = HiiGetString (
                      HiiHandle,
                      STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                      NULL
                      );
        PState = FALSE;
        if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
          if (SUCInfo->UserPassword->SystemPasswordStatus == EFI_SUCCESS) {
            SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
          }
        }
      } else {
        NewString = HiiGetString (
                      HiiHandle,
                      STRING_TOKEN (STR_INSTALLED_TEXT),
                      NULL
                      );
        PState = TRUE;
        if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
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
                StringPtr = HiiGetString (
                              HiiHandle,
                              STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                              NULL
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
            if (StrCmp (SUCInfo->SupervisorPassword->InputString,
                   SUCInfo->UserPassword->InputString) == 0) {
              SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
              StringPtr = HiiGetString (
                            HiiHandle,
                            STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                            NULL
                            );
              UpdateUserTags (HiiHandle, FALSE, StringPtr, MyIfrNVData);
              SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
            }
          }
        }
      }

      //
      // If Supervisor Password text op-code is not the same as new state, update the string
      //
      if (StrCmp (StringPtr, NewString) != 0) {
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
  case KEY_USER_PASSWORD:
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
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
        StringPtr = HiiGetString (
                      HiiHandle,
                      STRING_TOKEN (STR_USER_PASSWORD_STRING2),
                      NULL
                      );
        if (SUCInfo->UserPassword->Flag == DISABLE_PASSWORD) {
          NewString = HiiGetString (
                        HiiHandle,
                        STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                        NULL
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
            if (StrCmp (SUCInfo->SupervisorPassword->InputString,
                   SUCInfo->UserPassword->InputString) == 0) {
              SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
              Status = EFI_SECURITY_VIOLATION;
              SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
              break;
            }
          }
          NewString = HiiGetString (
                        HiiHandle,
                        STRING_TOKEN (STR_INSTALLED_TEXT),
                        NULL
                        );
          PState = TRUE;
          SuBrowser->SUCInfo->UserPwdFlag       = TRUE;
        }

        //
        // If User Password text op-code is not the same as new state, update the string
        //
        if (StrCmp (StringPtr, NewString) != 0) {
          UpdateUserTags (HiiHandle, PState, NewString, MyIfrNVData);
        }

        gBS->FreePool (NewString);
        gBS->FreePool (StringPtr);
        Status = EFI_SUCCESS;
      }
      SUCInfo->DoRefresh = TRUE;
    }
    break;

  case KEY_CLEAR_USER_PASSWORD:
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      StringPtr = HiiGetString (
                    HiiHandle,
                    STRING_TOKEN (STR_CLEAR_USER_PASSWORD_STRING),
                    NULL
                    );
      SuBrowser->H2ODialog->ConfirmDialog (
                                 0,
                                 FALSE,
                                 0,
                                 NULL,
                                 &Key,
                                 StringPtr
                                 );
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        SUCInfo->UserPassword->Flag = DISABLE_PASSWORD;
        NewString = HiiGetString (
                      HiiHandle,
                      STRING_TOKEN (STR_NOT_INSTALLED_TEXT),
                      NULL
                      );
        UpdateUserTags (HiiHandle, FALSE, NewString, MyIfrNVData);
        gBS->FreePool (NewString);
        SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
      }

      gBS->FreePool (StringPtr);
    }
    break;
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

    case KEY_TPM_HIDE:
    case KEY_TPM2_HIDE:
      //
      // Do nothing if TPM is hidden
      //
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->TpmClear      = 0;
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->TpmOperation  = 0;
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Tpm2Operation = 0;
      break;

    default:
      Status = SuBrowser->HotKeyCallback (
                            This,
                            Action,
                            QuestionId,
                            Type,
                            Value,
                            ActionRequest
                            );
      break;
  }

  SetupVariableConfig (
    NULL,
    NULL,
    BufferSize,
    (UINT8 *) SuBrowser->SCBuffer,
    FALSE
    );

  SetupVariableConfig (
    &PasswordConfigGuid,
    L"PasswordConfig",
    PasswordConfigBufferSize,
    (UINT8 *) &mPasswordConfig,
    FALSE
    );

  return Status;
}

EFI_STATUS
SecurityCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  UINTN                                 BufferSize;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_GUID                              PasswordConfigGuid = PASSWORD_CONFIGURATION_GUID;
  UINTN                                 PasswordConfigBufferSize;

  if ((This == NULL) ||
      ((Value == NULL) &&
       (Action != EFI_BROWSER_ACTION_FORM_OPEN) &&
       (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
      (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PasswordConfigBufferSize = sizeof (PASSWORD_CONFIGURATION);
  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize     = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  Status         = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_FORM_OPEN:
    if (QuestionId == 0) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 FALSE
                 );
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 FALSE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_FORM_CLOSE:
    if (QuestionId == 0) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 TRUE
                 );
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 TRUE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_CHANGING:
    Status = EFI_SUCCESS;
    break;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      Status = SuBrowser->HotKeyCallback (
                            This,
                            Action,
                            QuestionId,
                            Type,
                            Value,
                            ActionRequest
                            );
      SetupVariableConfig (
        NULL,
        NULL,
        sizeof(KERNEL_CONFIGURATION),
        (UINT8 *) SuBrowser->SCBuffer,
        FALSE
        );
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig
    //
    return EFI_SUCCESS;

  default:
    break;
  }

  return Status;
}

/**
  Install Security Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine
  @param HiiHandle               Hii hanlde for security page.

  @retval EFI_SUCCESS            Function has completed successfully.
  @retval Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
InstallSecurityCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_SECURITY;
  EFI_GUID                                  OldFormsetGuid = SYSTEM_CONFIGURATION_GUID;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mSecurityCallBackInfo = AllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (mSecurityCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mSecurityCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mSecurityCallBackInfo->DriverCallback.ExtractConfig = SuBrowser->ExtractConfig;
  mSecurityCallBackInfo->DriverCallback.RouteConfig   = SuBrowser->RouteConfig;
  mSecurityCallBackInfo->DriverCallback.Callback      = SecurityCallbackRoutine;
  mSecurityCallBackInfo->HiiHandle                    = HiiHandle;
  if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    CopyGuid (&mSecurityCallBackInfo->FormsetGuid, &FormsetGuid);
  } else {
    CopyGuid (&mSecurityCallBackInfo->FormsetGuid, &OldFormsetGuid);
  }

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

/**
  Uninstall Security Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
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
