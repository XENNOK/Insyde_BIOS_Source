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
//;   Exit.c
//;
//; Abstract:
//;
//;   Main funcitons for Eixt menu
//;

#include "Exit.h"
EFI_CALLBACK_INFO                         *mExitCallBackInfo;

EFI_STATUS
ExitCallbackRoutine (
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
  EFI_STATUS                                Status;
  CHAR16                                    *StringPtr;
  SYSTEM_CONFIGURATION                      *MyIfrNVData;
  EFI_HII_HANDLE                            HiiHandle;
  EFI_INPUT_KEY                             Key;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINTN                                     BufferSize;

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo            = EFI_EXIT_CALLBACK_INFO_FROM_THIS (This);

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
               (BOOLEAN)(QuestionId == GET_SETUP_CONFIG)
               );
    return Status;
  }

  SUCInfo                 = SuBrowser->SUCInfo;
  Status                  = EFI_SUCCESS;
  StringPtr               = NULL;
  HiiHandle               = CallbackInfo->HiiHandle;

  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = SetupVariableConfig (
             NULL,
             NULL,
             &BufferSize,
             (UINT8 *) SuBrowser->SCBuffer,
             TRUE
             );

  Interface               = &SuBrowser->Interface;
  Interface->MyIfrNVData  = (UINT8 *) SuBrowser->SCBuffer;
  MyIfrNVData             = (SYSTEM_CONFIGURATION *) Interface->MyIfrNVData;

  switch (QuestionId) {

  case KEY_SAVE_EXIT:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                          Action,
                          KEY_SCAN_F10,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_EXIT_DISCARD:
    //
    // Discard setup and exit
    //
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                          Action,
                          KEY_SCAN_ESC,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_LOAD_OPTIMAL:
    //
    //Select "Load Optimal Defaults" and Press "Load Optimal hotkey", the functionality is the same,
    //so remove the original code Use "Load Optimal hotkey" to call HotKeyCallBack () directly
    //

    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                          Action,
                          KEY_SCAN_F9,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_LOAD_CUSTOM:
    //
    // Load custom setup.
    //
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        ((MyIfrNVData->UserAccessLevel == 2) ||
         (MyIfrNVData->UserAccessLevel == 3))) {
    	 return EFI_SUCCESS;
    }
    GetStringFromHandle (
      HiiHandle,
      STRING_TOKEN (STR_LOAD_CUSTOM_DEFAULTS_STRING),
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
      Status = LoadCustomOption (This);
      if (!EFI_ERROR (Status)) {
        SUCInfo->DoRefresh = TRUE;
        Interface->Firstin = TRUE;
        Status = SuBrowser->HotKeyCallback (
                              This,
                              SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                              Action,
                              KEY_LOAD_CUSTOM,
                              Type,
                              Value,
                              ActionRequest
                              );
      }
    }

    gBS->FreePool (StringPtr);
    break;

  case KEY_SAVE_CUSTOM:
    //
    // Save custom setup.
    //

    GetStringFromHandle (
      HiiHandle,
      STRING_TOKEN (STR_SAVE_CUSTOM_DEFAULTS_STRING),
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
      Status = SaveCustomOption (This);
      if (!EFI_ERROR (Status)) {
        Status = SuBrowser->HotKeyCallback (
                              This,
                              SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                              Action,
                              KEY_SAVE_CUSTOM,
                              Type,
                              Value,
                              ActionRequest
                              );
      }
    }

    gBS->FreePool (StringPtr);
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
  
  return Status;
}

EFI_STATUS
InstallExitCallbackRoutine (
  IN EFI_HANDLE                            DriverHandle,
  IN EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Install Exit Callback routine.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

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

  mExitCallBackInfo = EfiLibAllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (mExitCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mExitCallBackInfo->Signature               = EFI_EXIT_MENU_SIGNATURE;
  mExitCallBackInfo->DriverCallback.ExtractConfig = SuBrowser->ExtractConfig;
  mExitCallBackInfo->DriverCallback.RouteConfig   = SuBrowser->RouteConfig;
  mExitCallBackInfo->DriverCallback.Callback = ExitCallbackRoutine;
  mExitCallBackInfo->HiiHandle = HiiHandle;

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mExitCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
UninstallExitCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS                                Status;
  if (mExitCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mExitCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mExitCallBackInfo);
  mExitCallBackInfo = NULL;
  return Status;
}

EFI_STATUS
InitExitMenu (
  IN EFI_HII_HANDLE                         HiiHandle
)
/*++

Routine Description:

  Initialize exit menu for setuputility use

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status = EFI_SUCCESS;

  return Status;
}



