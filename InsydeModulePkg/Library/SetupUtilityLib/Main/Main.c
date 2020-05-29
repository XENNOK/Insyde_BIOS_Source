/** @file
  Main funcitons for Main menu

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

#include "Main.h"
#include <Library/DxeOemSvcKernelLib.h>

EFI_CALLBACK_INFO                         *mMainCallBackInfo;

STRING_REF mCopyrightStrList[10] = {
  STRING_TOKEN (STR_COPYRIGHT),
  0
};

/**
  Update used DMI information from FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE
  , if the used data is in this region.

  @param Type                    SMBIOS type number
  @param Offset                  The offset of the string reference
  @param HiiHandle               Target EFI_HII_HANDLE instance
  @param TokenToUpdate           Updated token number

  @retval EFI_SUCCESS            Udpate string token successfully
  @return Other                  Cannot find HII protocol

**/
STATIC
EFI_STATUS
FindUpdateString (
  IN UINT8                              Type,
  IN UINT8                              Offset,
  IN EFI_HII_HANDLE                     HiiHandle,
  IN STRING_REF                         TokenToUpdate
  )
{
  CHAR16                                *UniString;
  UINTN                                 Index;
  UINT8                                 *DmiPtr;
  UINT8                                 *Buffer;
  DMI_STRING_STRUCTURE                  *CurrentPtr;
  UINTN                                 StringLen;

  if (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE == 0 ||
      FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE <= 9) {
    return EFI_NOT_FOUND;
  }

  DmiPtr = (UINT8*)(UINTN)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE);
  if (!(*(UINT32*)DmiPtr == DMI_UPDATE_STRING_SIGNATURE)){
    return EFI_NOT_FOUND;
  }

  Buffer = (UINT8*)(UINTN)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE + 4);
  while (TRUE) {
    CurrentPtr = (DMI_STRING_STRUCTURE*) Buffer;
    if ((UINTN) Buffer >=
    (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE +
    FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)
    ) {
      return EFI_NOT_FOUND;
    }
    if (CurrentPtr->Type == 0xFF || CurrentPtr->Length == 0) {
      return EFI_NOT_FOUND;
    }
    if (CurrentPtr->Type == Type &&
        CurrentPtr->Offset == Offset &&
        CurrentPtr->Valid == 0xFF) {
      break;
    }
    Buffer = Buffer + CurrentPtr->Length;
  }

  StringLen = CurrentPtr->Length - OFFSET_OF(DMI_STRING_STRUCTURE, String);
  UniString = AllocatePool ((StringLen + 1) * sizeof (CHAR16));
  if (UniString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // UINT8 transform CHAR16
  //
  for (Index = 0; Index < StringLen; Index++) {
    UniString[Index] = (CHAR16)CurrentPtr->String[Index];
  }
  UniString[Index] = (CHAR16) 0x0;

  HiiSetString (HiiHandle, TokenToUpdate, UniString, NULL);

  gBS->FreePool (UniString);

  return EFI_SUCCESS;

}

/**
  Using BVDT data to update STR_MISC_BIOS_VERSION string token

  @param HiiHandle               Target EFI_HII_HANDLE instance

  @retval EFI_SUCCESS            Udpate string token successfully
  @return Other                  Cannot find HII protocol

**/
STATIC
EFI_STATUS
UpdateBiosVersionFromBvdt (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  if (!EFI_ERROR (Status)) {
    HiiSetString (
      HiiHandle,
      STRING_TOKEN (STR_MISC_BIOS_VERSION),
      Str,
      NULL
      );
  }

  return Status;
}

/**
  Update BIOS build time string tokens

  @param[in] HiiHandle           Target EFI_HII_HANDLE instance

  @retval EFI_SUCCESS            Udpate string token successfully
  @retval Other                  Fail to get BVDT information
**/
STATIC
EFI_STATUS
UpdateBiosBuildDateTimeFromBvdt (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  UINTN                StrBufferSize;
  CHAR16               StrBuffer[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;
  VOID                 *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL   *StartLabel;

  //
  // Update string tokens of BIOS build time item from BVDT
  //
  StrBufferSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo (BvdtBuildDate, &StrBufferSize, StrBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiSetString (HiiHandle, (EFI_STRING_ID) STRING_TOKEN (STR_BIOS_BUILD_DATE_STRING), StrBuffer, NULL);

  StrBufferSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo (BvdtBuildTime, &StrBufferSize, StrBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiSetString (HiiHandle, (EFI_STRING_ID)STRING_TOKEN (STR_BIOS_BUILD_TIME_STRING), StrBuffer, NULL);

  //
  // Update the label of BIOS build time item
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = BIOS_BUILD_TIME_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    (EFI_STRING_ID)STRING_TOKEN(STR_BIOS_BUILD_DATE_PROMPT),
    0,
    (EFI_STRING_ID)STRING_TOKEN(STR_BIOS_BUILD_DATE_STRING)
    );
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    (EFI_STRING_ID) STRING_TOKEN(STR_BIOS_BUILD_TIME_PROMPT),
    0,
    (EFI_STRING_ID)STRING_TOKEN(STR_BIOS_BUILD_TIME_STRING)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update the language item with all of support languages for main menu use.

  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Udpate string token successfully
  @return Other                  Error occurred during execution.

**/
STATIC
EFI_STATUS
UpdateLanguage (
  IN EFI_HII_HANDLE                        HiiHandle
  )
{
  EFI_STATUS                            Status;
  KERNEL_CONFIGURATION                  SetupNvData;
  UINT16                                VarOffset;
  UINT16                                *StringBuffer;
  UINTN                                 Index;
  UINTN                                 OptionCount;
  STRING_REF                            Token;
  CHAR8                                 *LanguageString;
  UINTN                                 LangNum;
  VOID                                  *StartOpCodeHandle;
  VOID                                  *OptionsOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  CHAR8                                 *PlatformLangVar;
  UINT8                                 PlatformLangValue;
  CHAR8                                 DefaultLangCode[]="en-US";
  UINT8                                 DefaultLangValue;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetupUtilityLibGetLangDatabase (
    &LangNum,
    (UINT8 **) &LanguageString
    );

  PlatformLangVar = CommonGetVariableData (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid
                      );

  //
  // Init OpCode Handle and Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle   = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = SETUP_UTILITY_LANG_MENU;


  OptionCount = 0;

  DefaultLangValue  = (UINT8) LangNum;
  PlatformLangValue = (UINT8) LangNum;
  for (Index = 0; Index < LangNum; Index++) {
    StringBuffer = HiiGetString (
                     HiiHandle,
                     PRINTABLE_LANGUAGE_NAME_STRING_ID,
                     &LanguageString[Index * RFC_3066_ENTRY_SIZE]
                     );
    ASSERT (StringBuffer != NULL);

    Token = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    FreePool (StringBuffer);

    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      Token,
      OptionCount == 0 ? EFI_IFR_OPTION_DEFAULT : 0,
      EFI_IFR_NUMERIC_SIZE_1,
      (UINT8) OptionCount
      );

    if (PlatformLangVar != NULL && AsciiStrCmp (PlatformLangVar, &LanguageString[Index * RFC_3066_ENTRY_SIZE]) == 0) {
      PlatformLangValue = (UINT8) Index;
    }
    if (AsciiStrCmp (DefaultLangCode, &LanguageString[Index * RFC_3066_ENTRY_SIZE]) == 0) {
      DefaultLangValue = (UINT8) Index;
    }

    OptionCount++;
  }

  VarOffset = (UINT16)((UINTN)(&SetupNvData.Language) - (UINTN)(&SetupNvData));

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    KEY_LANGUAGE_UPDATE,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN (STR_LANGUAGE_PROMPT),
    STRING_TOKEN (STR_LANGUAGE_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  //
  // Make sure that PlatformLang variable is included in language list.
  // Sync language value between PlatformLang variable and language of Setup variable.
  //
  if (PlatformLangValue == LangNum) {
    PlatformLangValue = DefaultLangValue;
    CommonSetVariable (
      L"PlatformLang",
      &gEfiGlobalVariableGuid,
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
      sizeof (DefaultLangCode),
      DefaultLangCode
      );
  }

  if (((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Language != PlatformLangValue) {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Language = PlatformLangValue;
    UpdateStringToken ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer);
  }

  if (LangNum != 0) {
    gBS->FreePool (LanguageString);
  }

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);
  if (PlatformLangVar != NULL) {
    FreePool (PlatformLangVar);
  }

  return Status;
}

/**
 Update the copyright for main menu use.

 @param [in]   HiiHandle        Hii hanlde for the call back routine

 @retval EFI_SUCCESS            Function has completed successfully.
 @return Others         Error occurred during execution.

**/
STATIC
EFI_STATUS
UpdateCopyRight (
  IN EFI_HII_HANDLE                        HiiHandle
  )
{
  EFI_STATUS                            Status;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  UINTN                                 Index;


  Index = 1;

  if (FeaturePcdGet (PcdGraphicsSetupSupported) || FeaturePcdGet (PcdH2OFormBrowserLocalMetroDESupported)) {
    mCopyrightStrList[Index++] = STRING_TOKEN (STR_GRAPHICS_SETUP_COPYRIGHT);
  }

  if (mCopyrightStrList[1] == 0) {
    return EFI_SUCCESS;
  }

  //
  // Init OpCode Handle and Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle   = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = COPYRIGHT_LABEL;

  HiiCreateSubTitleOpCode (StartOpCodeHandle, STRING_TOKEN (STR_BLANK_STRING), 0, 0, 0);
  HiiCreateActionOpCode (
    StartOpCodeHandle,
    KEY_ABOUT_THIS_SOFTWARE,
    STRING_TOKEN(STR_ABOUT_THIS_SOFTWARE),
    STRING_TOKEN(STR_BLANK_STRING),
    EFI_IFR_FLAG_CALLBACK,
    0
    );

  Status = HiiUpdateForm (HiiHandle, NULL, ROOT_FORM_ID, StartOpCodeHandle, NULL);
  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return Status;
}

/**
  Initialize main menu for setuputility use

  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Udpate string token successfully
  @return Other                  Error occurred during execution.

**/
STATIC
EFI_STATUS
InitMainMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;


  UpdateBiosVersionFromBvdt (HiiHandle);
  UpdateBiosBuildDateTimeFromBvdt (HiiHandle);

  Status = FindUpdateString (
             0,
             0x05,
             HiiHandle,
             ((STRING_REF)STRING_TOKEN (STR_MISC_BIOS_VERSION))
             );

  //
  // Update System Information
  //
  Status = UpdateSystemInfo (HiiHandle);

  //
  // Update current language
  //
  Status = UpdateLanguage (HiiHandle);

  UpdateCopyRight (HiiHandle);

  return Status;
}

/**
  Update the content of string tokens while changing language.

  @param IfrNVData               Point to KERNEL_CONFIGURATION instance.

  @retval EFI_SUCCESS            Udpate string token successfully
  @return Other                  Cannot find SETUP_UTILITY_BROWSER_DATA instance.

**/
EFI_STATUS
UpdateStringToken (
  IN KERNEL_CONFIGURATION               *IfrNVData
  )
{
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

  UpdatePasswordState (SUCInfo->MapTable[SecurityHiiHandle].HiiHandle);
  if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    SetupUtilityLibUpdateDeviceString (SUCInfo->MapTable[BootHiiHandle].HiiHandle, IfrNVData);
  }
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
  @return EFI_UNSUPPORTED        The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
MainCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  CHAR16                                *StringPtr;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *Interface;
  UINTN                                 BufferSize;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  CHAR16                                *TitleString;
  CHAR16                                *CopyrightStr;
  UINTN                                 StrLength;
  UINTN                                 Index;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return MainCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_CALLBACK_INFO_FROM_THIS (This);

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  Interface  = NULL;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 (BOOLEAN)(QuestionId == GET_SETUP_CONFIG)
                 );
      return Status;
    }

    Status = gBS->LocateProtocol (
                   &gEfiSetupUtilityBrowserProtocolGuid,
                   NULL,
                   (VOID **) &Interface
                   );
    if (EFI_ERROR (Status)) {
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
  Status    = EFI_SUCCESS;
  StringPtr = NULL;
  HiiHandle = CallbackInfo->HiiHandle;

  switch (QuestionId) {

  case KEY_LANGUAGE_UPDATE:
    UpdateLangItem (This, &(((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Language));
    UpdateStringToken ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer);
    if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
      BrowserRefreshFormSet ();
    } else {
      Interface->Firstin = TRUE;
      Interface->JumpToFirstOption = FALSE;
    }
    break;

  case KEY_ABOUT_THIS_SOFTWARE:

    if (mCopyrightStrList[1] == 0) {
      break;
    }

    for (Index = 0, StrLength = 0; mCopyrightStrList[Index] != 0; Index++) {
      StringPtr = HiiGetString  (CallbackInfo->HiiHandle, mCopyrightStrList[Index], NULL);
      if (StringPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      StrLength += StrSize (StringPtr);
      gBS->FreePool (StringPtr);
    }

    CopyrightStr = AllocateZeroPool (StrLength);
    if (CopyrightStr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; mCopyrightStrList[Index] != 0; Index++) {
      StringPtr = HiiGetString (CallbackInfo->HiiHandle, mCopyrightStrList[Index], NULL);
      if (StringPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      StrCat (CopyrightStr, StringPtr);
      gBS->FreePool (StringPtr);
    }

    TitleString = HiiGetString (CallbackInfo->HiiHandle, STRING_TOKEN (STR_ABOUT_THIS_SOFTWARE), NULL);
    if (TitleString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    SuBrowser->H2ODialog->ShowPageInfo (
                              TitleString,
                              CopyrightStr
                              );
    gBS->FreePool (TitleString);
    gBS->FreePool (CopyrightStr);
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

  return Status;
}

EFI_STATUS
MainCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  UINTN                                 BufferSize;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

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

  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize     = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  Status         = EFI_UNSUPPORTED;
  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

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
  Install Main Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine
  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
InstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_MAIN;
  EFI_GUID                                  OldFormsetGuid = SYSTEM_CONFIGURATION_GUID;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mMainCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mMainCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mMainCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mMainCallBackInfo->DriverCallback.ExtractConfig = SuBrowser->ExtractConfig;
  mMainCallBackInfo->DriverCallback.RouteConfig   = SuBrowser->RouteConfig;
  mMainCallBackInfo->DriverCallback.Callback      = MainCallbackRoutine;
  mMainCallBackInfo->HiiHandle                    = HiiHandle;
  if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    CopyGuid (&mMainCallBackInfo->FormsetGuid, &FormsetGuid);
  } else {
    CopyGuid (&mMainCallBackInfo->FormsetGuid, &OldFormsetGuid);
  }

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mMainCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitMainMenu (HiiHandle);

  return Status;
}

/**
  Uninstall Main Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
UninstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS       Status;

  if (mMainCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mMainCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mMainCallBackInfo);
  mMainCallBackInfo = NULL;
  return Status;
}
