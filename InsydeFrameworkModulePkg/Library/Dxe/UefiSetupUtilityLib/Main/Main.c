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
//;   Main.c
//;
//; Abstract:
//;
//;   Main funcitons for Main menu
//;

#include "Main.h"
#include "OemPnpStrings.h"

EFI_CALLBACK_INFO                         *mMainCallBackInfo;

STRING_REF mCopyrightStrList[] = {
  STRING_TOKEN (STR_COPYRIGHT),
#ifdef GRAPHICS_SETUP_SUPPORT
  STRING_TOKEN (STR_GRAPHICS_SETUP_COPYRIGHT),
#endif
  0
};


STATIC
EFI_STATUS
FindUpdateString (
  IN UINT8                              Type,
  IN UINT8                              Offset,
  IN EFI_HII_HANDLE                     HiiHandle,
  IN STRING_REF                         TokenToUpdate
  )
/*++

Routine Description:

  Update used DMI information from FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE
  , if the used data is in this region.

Arguments:

  Type           - SMBIOS type number
  Offset         - The offset of the string reference
  HiiHandle      - Target EFI_HII_HANDLE instance
  TokenToUpdate  - Updated token number

Returns:

  EFI_SUCESS - Udpate string token successfule
  Other      - Cannot find HII protocol

--*/
{
#if (defined(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)) && \
    (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE > 9)
  EFI_STATUS                            Status;
  CHAR16                                *UniString;
  UINTN                                 Index;
  UINT8                                 *DmiPtr;
  UINT8                                 *Buffer;
  DMI_STRING_STRUCTURE                  *CurrentPtr;
  UINTN                                 StringLen;

  DmiPtr = (UINT8*)(UINTN)(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE);
  if (!(*(UINT32*)DmiPtr == DMI_UPDATA_STRING_SIGNATURE)){
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
    if (CurrentPtr->Type == 0xFF) {
      return EFI_NOT_FOUND;
    }
    if (CurrentPtr->Type == Type &&
        CurrentPtr->Offset == Offset &&
        CurrentPtr->Valid == 0xFF) {
      break;
    }
    Buffer = Buffer + CurrentPtr->Length;
  }

  StringLen = CurrentPtr->Length - EFI_FIELD_OFFSET(DMI_STRING_STRUCTURE, String);
  UniString = EfiLibAllocatePool ((StringLen + 1) * sizeof (CHAR16));
  //
  // UINT8 transform CHAR16
  //
  for (Index = 0; Index < StringLen; Index++) {
    UniString[Index] = (CHAR16)CurrentPtr->String[Index];
  }
  UniString[Index] = (CHAR16) 0x0;
  Status = IfrLibSetString (HiiHandle, TokenToUpdate, UniString);
  gBS->FreePool (UniString);

  return Status;
#else
  return EFI_NOT_FOUND;
#endif
}

STATIC
EFI_STATUS
UpdateBiosVersionFromBvdt (
  IN EFI_HII_HANDLE     HiiHandle
  )
/*++

Routine Description:

  Using BVDT data to update STR_MISC_BIOS_VERSION string token

Arguments:

  HiiHandle - Target EFI_HII_HANDLE instance

Returns:

  EFI_SUCESS - Udpate string token successfule
  Other      - Cannot find HII protocol

--*/
{
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  if (!EFI_ERROR (Status)) {
    Status = IfrLibSetString (HiiHandle, STRING_TOKEN (STR_MISC_BIOS_VERSION), Str);
  }
  return Status;
}

STATIC
EFI_STATUS
UpdateBiosBuildTimeFromBvdt (
  IN EFI_HII_HANDLE     HiiHandle
  )
/*++

Routine Description:

  Update string tokens and label for BIOS build time from BVDT

Arguments:

  HiiHandle  - Target HII handle instance

Returns:

  EFI_SUCESS - Successfully update BIOS build time item
  Other      - Fail to get BVDT information

--*/
{
  UINTN                StrBufferSize;
  CHAR16               StrBuffer[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;
  EFI_HII_UPDATE_DATA  UpdateData;

  //
  // Update string tokens of BIOS build time item from BVDT
  //
  StrBufferSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo (BvdtBuildDate, &StrBufferSize, StrBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  IfrLibSetString (HiiHandle, STRING_TOKEN (STR_BIOS_BUILD_DATE_STRING), StrBuffer);

  StrBufferSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo (BvdtBuildTime, &StrBufferSize, StrBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  IfrLibSetString (HiiHandle, STRING_TOKEN (STR_BIOS_BUILD_TIME_STRING), StrBuffer);

  //
  // Update the label of BIOS build time item
  //
  IfrLibInitUpdateData (&UpdateData, 0x1000);

  CreateTextOpCode (
    (STRING_REF)STRING_TOKEN(STR_BIOS_BUILD_DATE_PROMPT),
    0,
    (STRING_REF)STRING_TOKEN(STR_BIOS_BUILD_DATE_STRING),
    &UpdateData
    );
  CreateTextOpCode (
    (STRING_REF)STRING_TOKEN(STR_BIOS_BUILD_TIME_PROMPT),
    0,
    (STRING_REF)STRING_TOKEN(STR_BIOS_BUILD_TIME_STRING),
    &UpdateData
    );

  Status = IfrLibUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             BIOS_BUILD_TIME_LABEL,
             TRUE,
             &UpdateData
             );
  IfrLibFreeUpdateData (&UpdateData);

  return Status;
}

STATIC
EFI_STATUS
UpdateLanguage (
  IN EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update the language item with all of support languages for main menu use.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                            Status;
  EFI_HII_STRING_PROTOCOL               *HiiString;
  SYSTEM_CONFIGURATION                  SetupNvData;
  EFI_HII_UPDATE_DATA                   UpdateData;
  UINT16                                QuestionId;
  UINT16                                VarOffset;
  UINT16                                *StringBuffer;
  UINTN                                 BufferSize;
  UINTN                                 Index;
  UINTN                                 OptionCount;
  IFR_OPTION                            *OptionList;
  STRING_REF                            Token;
  UINT8                                 *LanguageString;
  UINTN                                 LangNum;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  UINT8                                 *PlatformLangVar;
  UINT8                                 PlatformLangValue;
  CHAR8                                 DefaultLangCode[]="en-US";
  UINT8                                 DefaultLangValue;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HiiString = SuBrowser->HiiString;

  SetupUtilityLibGetLangDatabase (
    &LangNum,
    &LanguageString
    );

  PlatformLangVar = SetupUtilityLibGetVariableAndSize (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid,
                      &BufferSize
                      );

  OptionCount = 0;
  OptionList = EfiLibAllocateZeroPool (LangNum * sizeof (IFR_OPTION));

  //
  // Try for a 512 byte Buffer
  //
  BufferSize = 0x200;

  //
  // Allocate memory for our Form binary
  //
  StringBuffer = EfiLibAllocateZeroPool (BufferSize);

  DefaultLangValue  = (UINT8) LangNum;
  PlatformLangValue = (UINT8) LangNum;
  for (Index = 0; Index < LangNum; Index++) {
    Token = 0;
    BufferSize = 0x200;
    Status = HiiString->GetString (
                          SuBrowser->HiiString,
                          &LanguageString[Index * RFC_3066_ENTRY_SIZE],
                          HiiHandle,
                          1,
                          StringBuffer,
                          &BufferSize,
                          NULL
                          );

    IfrLibNewString (HiiHandle, &Token, StringBuffer);
    OptionList[OptionCount].StringToken = (UINT16)Token;
    OptionList[OptionCount].Value.u16   = (UINT16)OptionCount;
    OptionList[OptionCount].Flags       = 0;
    OptionCount++;

    if (PlatformLangVar != NULL && EfiAsciiStrCmp (PlatformLangVar, &LanguageString[Index * RFC_3066_ENTRY_SIZE]) == 0) {
      PlatformLangValue = (UINT8) Index;
    }
    if (EfiAsciiStrCmp (DefaultLangCode, &LanguageString[Index * RFC_3066_ENTRY_SIZE]) == 0) {
      DefaultLangValue = (UINT8) Index;
    }
  }
  QuestionId = KEY_LANGUAGE_UPDATE;
  OptionList[0].Flags |= EFI_IFR_OPTION_DEFAULT;

  VarOffset = (UINT16)((UINTN)(&SetupNvData.Language) - (UINTN)(&SetupNvData));

  IfrLibInitUpdateData (&UpdateData, 0x1000);
  Status = CreateOneOfOpCode (
             QuestionId,
             CONFIGURATION_VARSTORE_ID,
             VarOffset,
             STRING_TOKEN(STR_LANGUAGE_PROMPT),
             STRING_TOKEN(STR_LANGUAGE_HELP),
             EFI_IFR_FLAG_CALLBACK,
             0,
             OptionList,
             OptionCount,
             &UpdateData
             );

  //
  // Assign the number of options and the oneof and endoneof op-codes to count
  //

  IfrLibUpdateForm (HiiHandle, NULL, ROOT_FORM_ID, SETUP_UTILITY_LANG_MENU, TRUE, &UpdateData);

  //
  // Make sure that PlatformLang variable is included in language list.
  // Sync language value between PlatformLang variable and language of Setup variable.
  //
  if (PlatformLangValue == LangNum) {
    PlatformLangValue = DefaultLangValue;
    gRT->SetVariable (
           L"PlatformLang",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           sizeof (DefaultLangCode),
           DefaultLangCode
           );
  }
  if (SuBrowser->SCBuffer->Language != PlatformLangValue) {
    SuBrowser->SCBuffer->Language = PlatformLangValue;
    UpdateStringToken (SuBrowser->SCBuffer);
  }


  IfrLibFreeUpdateData (&UpdateData);
  gBS->FreePool (OptionList);
  gBS->FreePool (StringBuffer);
  gBS->FreePool (LanguageString);
  return Status;
}

STATIC
EFI_STATUS
UpdateCopyRight (
  IN EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update the language item with all of support languages for main menu use.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                            Status;
  EFI_HII_UPDATE_DATA                   UpdateData;

  if (mCopyrightStrList[1] == 0) {
    return EFI_SUCCESS;
  }

  IfrLibInitUpdateData (&UpdateData, 0x1000);

  Status = CreateSubTitleOpCode (STRING_TOKEN (STR_BLANK_STRING), 0, 0, 0, &UpdateData);
  Status = CreateActionOpCode (
             KEY_ABOUT_THIS_SOFTWARE,
             STRING_TOKEN(STR_ABOUT_THIS_SOFTWARE),
             STRING_TOKEN(STR_BLANK_STRING),
             EFI_IFR_FLAG_CALLBACK,
             0,
             &UpdateData
             );

  //
  // Assign the number of options and the oneof and endoneof op-codes to count
  //
  IfrLibUpdateForm (HiiHandle, NULL, ROOT_FORM_ID, COPYRIGHT_LABEL, TRUE, &UpdateData);
  IfrLibFreeUpdateData (&UpdateData);

  return Status;
}

STATIC
EFI_STATUS
InitMainMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  )
/*++

Routine Description:

  Initialize main menu for setuputility use

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status;


  UpdateBiosVersionFromBvdt (HiiHandle);
  UpdateBiosBuildTimeFromBvdt (HiiHandle);

  Status = FindUpdateString (
             0,
             0x05,
             HiiHandle,
             ((STRING_REF)STRING_TOKEN (STR_MISC_BIOS_VERSION))
             );
  //
  // Update System Information
  //
  UpdateSystemInfo (HiiHandle);
  //
  // Update current language
  //
  UpdateLanguage (HiiHandle);

  UpdateCopyRight (HiiHandle);

  return Status;
}

EFI_STATUS
UpdateStringToken (
  IN SYSTEM_CONFIGURATION                  *IfrNVData
  )
/*++

Routine Description:

  Update the content of string tokens while changing language.

Arguments:

  IfrNVData -  Point to FSYSTEM_CONFIGURATION instance.

Returns:

  EFI_SUCCESS  -  Update string token successful.
  Other        -  Cannot find SETUP_UTILITY_BROWSER_DATA instance.


--*/
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
  SetupUtilityLibUpdateDeviceString (SUCInfo->MapTable[BootHiiHandle].HiiHandle, IfrNVData);
  return EFI_SUCCESS;

}


EFI_STATUS
MainCallbackRoutine (
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
  CHAR16                                *StringPtr;
  SYSTEM_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *Interface;
  UINTN                                 BufferSize;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  CHAR16                                *TitleString;
  CHAR16                                *CopyrightStr;
  UINTN                                 StrLength;
  UINTN                                 Index;

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_MAIN_CALLBACK_INFO_FROM_THIS (This);

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

  Status = gBS->LocateProtocol (
                 &gEfiSetupUtilityBrowserProtocolGuid,
                 NULL,
                 &Interface
                 );
  if (EFI_ERROR (Status)) {
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
  Status    = EFI_SUCCESS;
  StringPtr = NULL;
  HiiHandle = CallbackInfo->HiiHandle;

  switch (QuestionId) {

  case KEY_LANGUAGE_UPDATE:
    BufferSize = sizeof (SYSTEM_CONFIGURATION);
    MyIfrNVData = EfiLibAllocateZeroPool (sizeof (SYSTEM_CONFIGURATION));
    Status = GetBrowserData (NULL, NULL, &BufferSize, (UINT8 *) MyIfrNVData);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = UpdateLangItem (This, (UINT8 *)&MyIfrNVData->Language);
    SuBrowser->SCBuffer->Language = MyIfrNVData->Language;
    Interface->Firstin = TRUE;
    Interface->JumpToFirstOption = FALSE;
    UpdateStringToken (SuBrowser->SCBuffer);
    gBS->FreePool (MyIfrNVData);
    break;

  case KEY_ABOUT_THIS_SOFTWARE:

    if (mCopyrightStrList[1] == 0) {
      break;
    }

    for (Index = 0, StrLength = 0; mCopyrightStrList[Index] != 0; Index++) {
      Status = GetStringFromHandle (CallbackInfo->HiiHandle, mCopyrightStrList[Index], &StringPtr);
      if (EFI_ERROR (Status) || StringPtr == NULL) {
        return Status;
      }
      StrLength += EfiStrSize (StringPtr);
      gBS->FreePool (StringPtr);
    }

    CopyrightStr = EfiLibAllocateZeroPool (StrLength);
    if (CopyrightStr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; mCopyrightStrList[Index] != 0; Index++) {
      Status = GetStringFromHandle (CallbackInfo->HiiHandle, mCopyrightStrList[Index], &StringPtr);
      if (EFI_ERROR (Status) || StringPtr == NULL) {
        return Status;
      }
      EfiStrCat (CopyrightStr, StringPtr);
      gBS->FreePool (StringPtr);
    }

    GetStringFromHandle (
      CallbackInfo->HiiHandle,
      STRING_TOKEN (STR_ABOUT_THIS_SOFTWARE),
      &TitleString
      );
    if (TitleString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    SuBrowser->OemBrowser2->ShowPageInfo (
                              TitleString,
                              CopyrightStr
                              );
    gBS->FreePool (TitleString);
    gBS->FreePool (CopyrightStr);
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
InstallMainCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
/*++

Routine Description:

  Install Main Callback routine.

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

  mMainCallBackInfo = EfiLibAllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mMainCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mMainCallBackInfo->Signature               = EFI_MAIN_MENU_SIGNATURE;
  mMainCallBackInfo->DriverCallback.ExtractConfig   = SuBrowser->ExtractConfig;
  mMainCallBackInfo->DriverCallback.RouteConfig     = SuBrowser->RouteConfig;
  mMainCallBackInfo->DriverCallback.Callback = MainCallbackRoutine;
  mMainCallBackInfo->HiiHandle = HiiHandle;
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


EFI_STATUS
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
