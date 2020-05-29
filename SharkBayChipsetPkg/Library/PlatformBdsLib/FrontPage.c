/** @file
  PlatformBdsLib

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

/**
  FrontPage routines to handle the callbacks and browser calls

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "FrontPage.h"
//[-start-130611-IB11120016-add]//
#include <Protocol/PlatformEventProtocolGuid.h>
//[-end-130611-IB11120016-add]//

#ifndef IMAGE_BOOT_FROM_FILE
#define IMAGE_BOOT_FROM_FILE (0)
#endif
#ifndef IMAGE_SECURE_BOOT_OPTION
#define IMAGE_SECURE_BOOT_OPTION (0)
#endif
#ifndef ANI_BOOT_FROM_FILE
#define ANI_BOOT_FROM_FILE (0)
#endif
#ifndef ANI_SECURE_BOOT_OPTION
#define ANI_SECURE_BOOT_OPTION (0)
#endif

BOOLEAN   mModeInitialized = FALSE;
BOOLEAN   mSetupModeInitialized = FALSE;

UINTN     gCallbackKey;

//
// Boot video resolution and text mode.
//
UINT32    mBootHorizontalResolution    = 0;
UINT32    mBootVerticalResolution      = 0;
UINT32    mBootTextModeColumn          = 0;
UINT32    mBootTextModeRow             = 0;
//
// BIOS setup video resolution and text mode.
//
UINT32    mSetupTextModeColumn         = 0;
UINT32    mSetupTextModeRow            = 0;
UINT32    mSetupHorizontalResolution   = 0;
UINT32    mSetupVerticalResolution     = 0;

EFI_FORM_BROWSER2_PROTOCOL      *gFormBrowser2;

FRONT_PAGE_CALLBACK_DATA  gFrontPagePrivate = {
  FRONT_PAGE_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  NULL,
  {
    FakeExtractConfig,
    FakeRouteConfig,
    FrontPageCallback
  }
};

HII_VENDOR_DEVICE_PATH  mFrontPageHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    FRONT_PAGE_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
FakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Request;
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
FakeRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &gBootMaintFormSetGuid, mBootMaintStorageName)
      && !HiiIsConfigHdrMatch (Configuration, &gFileExploreFormSetGuid, mFileExplorerStorageName)) {
    return EFI_NOT_FOUND;
  }

  *Progress = Configuration + StrLen (Configuration);
  return EFI_SUCCESS;
}

/**
  This function processes the results of changes in configuration.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_OUT_OF_RESOURCES  Not enough storage is available to hold the variable and its data.
  @retval  EFI_DEVICE_ERROR      The variable could not be saved.
  @retval  EFI_UNSUPPORTED       The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
FrontPageCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  CHAR8                         *LanguageString;
  CHAR8                         *LangCode;
  CHAR8                         *Lang;
  UINTN                         Index;
  EFI_STATUS                    Status;
  CHAR8                         *PlatformSupportedLanguages;
  CHAR8                         *BestLanguage;

//[-start-140620-IB05080432-add]//
  LanguageString = NULL;
  Lang = NULL;
  PlatformSupportedLanguages = NULL;
//[-end-140620-IB05080432-add]//

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable for specific question IDs.
    //
    if (QuestionId == FRONT_PAGE_KEY_CONTINUE || 
        QuestionId == FRONT_PAGE_KEY_LANGUAGE
        ) {
      Action = EFI_BROWSER_ACTION_CHANGED;
    }
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // All other action return unsupported.
    //
    return EFI_UNSUPPORTED;
  }

  gCallbackKey = QuestionId;

  if (Action == EFI_BROWSER_ACTION_CHANGED) {
    if ((Value == NULL) || (ActionRequest == NULL)) {
      return EFI_INVALID_PARAMETER;
    }

    switch (QuestionId) {
    case FRONT_PAGE_KEY_CONTINUE:
      //
      // This is the continue - clear the screen and return an error to get out of FrontPage loop
      //
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
      break;

    case FRONT_PAGE_KEY_LANGUAGE:
      //
      // Collect the languages from what our current Language support is based on our VFR
      //
      LanguageString = HiiGetSupportedLanguages (gFrontPagePrivate.HiiHandle);
      ASSERT (LanguageString != NULL);
//[-start-140620-IB05080432-add]//
      if (LanguageString == NULL) {
        goto ExitKeyLangCallback;
      }
//[-end-140620-IB05080432-add]//
      //
      // Allocate working buffer for RFC 4646 language in supported LanguageString.
      //
      Lang = AllocatePool (AsciiStrSize (LanguageString));
      ASSERT (Lang != NULL);
//[-start-140620-IB05080432-add]//
      if (Lang == NULL) {
        goto ExitKeyLangCallback;
      }
//[-end-140620-IB05080432-add]//

      Index = 0;
      LangCode = LanguageString;
      while (*LangCode != 0) {
        BdsLibGetNextLanguage (&LangCode, Lang);

        if (Index == Value->u8) {
          break;
        }

        Index++;
      }

      GetEfiGlobalVariable2 (L"PlatformLangCodes", (VOID**)&PlatformSupportedLanguages, NULL);
      if (PlatformSupportedLanguages == NULL) {
        PlatformSupportedLanguages = AllocateCopyPool (
                                       AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLangCodes)),
                                       (CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLangCodes)
                                       );
        ASSERT (PlatformSupportedLanguages != NULL);
//[-start-140620-IB05080432-add]//
        if (PlatformSupportedLanguages == NULL) {
          goto ExitKeyLangCallback;
        }
//[-end-140620-IB05080432-add]//
      }

      //
      // Select the best language in platform supported Language.
      //
      BestLanguage = GetBestLanguage (
                       PlatformSupportedLanguages,
                       FALSE,
                       Lang,
                       NULL
                       );
      if (BestLanguage != NULL) {
        Status = gRT->SetVariable (
                        L"PlatformLang",
                        &gEfiGlobalVariableGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        AsciiStrSize (BestLanguage),
                        Lang
                        );
        ASSERT_EFI_ERROR(Status);
        FreePool (BestLanguage);
      } else {
        ASSERT (FALSE);
      }

      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
      
//[-start-140620-IB05080432-modify]//
ExitKeyLangCallback:
      if (PlatformSupportedLanguages != NULL) {
        FreePool (PlatformSupportedLanguages);
      }
      if (Lang != NULL) {
        FreePool (Lang);
      }
      if (LanguageString != NULL) {
        FreePool (LanguageString);
      }
//[-end-140620-IB05080432-modify]//
      break;

    default:
      break;
    }
  } else if (Action == EFI_BROWSER_ACTION_CHANGING) {
    if (Value == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // The first 4 entries in the Front Page are to be GUARANTEED to remain constant so IHV's can
    // describe to their customers in documentation how to find their setup information (namely
    // under the device manager and specific buckets)
    //
    switch (QuestionId) {

    case FRONT_PAGE_KEY_BOOT_MANAGER:
      //
      // Boot Manager
      //
      break;

    case FRONT_PAGE_KEY_DEVICE_MANAGER:
      //
      // Device Manager
      //
      break;

    case FRONT_PAGE_KEY_BOOT_MAINTAIN:
      //
      // Boot Maintenance Manager
      //
      break;

    case FRONT_PAGE_KEY_SETUP_UTILITY:
      //
      // Setup Utility
      //
      break;

    case FRONT_PAGE_KEY_SECURE_BOOT:
      //
      // Administer Secure Boot
      //
      break;

    case FRONT_PAGE_KEY_MEBX:
      //
      // Mebx
      //
      if (FeaturePcdGet (PcdMe5MbSupported)) {
        break;
      } else {
        gCallbackKey = 0;
      }
      break;

    default:
      gCallbackKey = 0;
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
  Initialize HII information for the FrontPage


  @param InitializeHiiData    TRUE if HII elements need to be initialized.

  @retval  EFI_SUCCESS        The operation is successful.
  @retval  EFI_DEVICE_ERROR   If the dynamic opcode creation failed.

**/
EFI_STATUS
InitializeFrontPage (
  IN BOOLEAN                         InitializeHiiData
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *LanguageString;
  CHAR8                       *LangCode;
  CHAR8                       *Lang;
  CHAR8                       *CurrentLang;
  CHAR8                       *BestLanguage;
  UINTN                       OptionCount;
  CHAR16                      *StringBuffer;
  EFI_HII_HANDLE              HiiHandle;
  VOID                        *OptionsOpCodeHandle;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;
  BOOLEAN                     FirstFlag;

//[-start-140620-IB05080432-add]//
  LanguageString = NULL;
  Lang = NULL;
  CurrentLang = NULL;
  BestLanguage = NULL;
  OptionsOpCodeHandle = NULL;
  StartOpCodeHandle = NULL;
  EndOpCodeHandle = NULL;
  Status = EFI_SUCCESS;
//[-end-140620-IB05080432-add]//
  
  if (InitializeHiiData) {
    //
    // Initialize the Device Manager
    //
    InitializeDeviceManager ();

    //
    // Initialize the Device Manager
    //
    InitializeBootManager ();

    InitializeSecureBoot ();

    gCallbackKey  = 0;

    //
    // Locate Hii relative protocols
    //
    Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &gFormBrowser2);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Install Device Path Protocol and Config Access protocol to driver handle
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &gFrontPagePrivate.DriverHandle,
                    &gEfiDevicePathProtocolGuid,
                    &mFrontPageHiiVendorDevicePath,
                    &gEfiHiiConfigAccessProtocolGuid,
                    &gFrontPagePrivate.ConfigAccess,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    //
    // Publish our HII data
    //
    gFrontPagePrivate.HiiHandle = HiiAddPackages (
                                    &gFrontPageFormSetGuid,
                                    gFrontPagePrivate.DriverHandle,
                                    FrontPageVfrBin,
                                    PlatformBdsLibStrings,
                                    PlatformBdsLibAnimations,
                                    PlatformBdsLibImages,
                                    NULL
                                    );
    if (gFrontPagePrivate.HiiHandle == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }


  //
  // Init OpCode Handle and Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
//[-start-140620-IB05080432-add]//
  if (StartOpCodeHandle == NULL) {
    goto ExitInitFrontPage;
  }
//[-end-140620-IB05080432-add]//

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);
//[-start-140620-IB05080432-add]//
  if (EndOpCodeHandle == NULL) {
    goto ExitInitFrontPage;
  }
//[-end-140620-IB05080432-add]//

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);
//[-start-140620-IB05080432-add]//
  if (OptionsOpCodeHandle == NULL) {
    goto ExitInitFrontPage;
  }
//[-end-140620-IB05080432-add]//
  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_SELECT_LANGUAGE;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;

  //
  // Collect the languages from what our current Language support is based on our VFR
  //
  HiiHandle = gFrontPagePrivate.HiiHandle;
  LanguageString = HiiGetSupportedLanguages (HiiHandle);
  ASSERT (LanguageString != NULL);
//[-start-140620-IB05080432-add]//
  if (LanguageString == NULL) {
    goto ExitInitFrontPage;
  }
//[-end-140620-IB05080432-add]//
  //
  // Allocate working buffer for RFC 4646 language in supported LanguageString.
  //
  Lang = AllocatePool (AsciiStrSize (LanguageString));
  ASSERT (Lang != NULL);
//[-start-140620-IB05080432-add]//
  if (Lang == NULL) {
    goto ExitInitFrontPage;
  }
//[-end-140620-IB05080432-add]//

  CurrentLang = GetEfiGlobalVariable (L"PlatformLang");
  //
  // Select the best language in LanguageString as the default one.
  //
  BestLanguage = GetBestLanguage (
                   LanguageString,
                   FALSE,
                   (CurrentLang != NULL) ? CurrentLang : "",
                   (CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang),
                   LanguageString,
                   NULL
                   );
  //
  // BestLanguage must be selected as it is the first language in LanguageString by default
  //
  ASSERT (BestLanguage != NULL);

  OptionCount = 0;
  LangCode    = LanguageString;
  FirstFlag   = FALSE;

  if (gFrontPagePrivate.LanguageToken == NULL) {
    while (*LangCode != 0) {
      BdsLibGetNextLanguage (&LangCode, Lang);
      OptionCount ++;
    }
    gFrontPagePrivate.LanguageToken = AllocatePool (OptionCount * sizeof (EFI_STRING_ID));
    ASSERT (gFrontPagePrivate.LanguageToken != NULL);
//[-start-140620-IB05080432-add]//
    if (gFrontPagePrivate.LanguageToken == NULL) {
      goto ExitInitFrontPage;
    }
//[-end-140620-IB05080432-add]//
    FirstFlag = TRUE;
  }

  OptionCount = 0;
  LangCode = LanguageString;
  while (*LangCode != 0) {
    BdsLibGetNextLanguage (&LangCode, Lang);

    if (FirstFlag) {
      StringBuffer = HiiGetString (HiiHandle, PRINTABLE_LANGUAGE_NAME_STRING_ID, Lang);
      ASSERT (StringBuffer != NULL);

      //
      // Save the string Id for each language
      //
      gFrontPagePrivate.LanguageToken[OptionCount] = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      FreePool (StringBuffer);
    }

    if (AsciiStrCmp (Lang, BestLanguage) == 0) {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        gFrontPagePrivate.LanguageToken[OptionCount],
        EFI_IFR_OPTION_DEFAULT,
        EFI_IFR_NUMERIC_SIZE_1,
        (UINT8) OptionCount
        );
    } else {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        gFrontPagePrivate.LanguageToken[OptionCount],
        0,
        EFI_IFR_NUMERIC_SIZE_1,
        (UINT8) OptionCount
        );
    }

    OptionCount++;
  }

//[-start-140620-IB05080432-remove]//
//  if (CurrentLang != NULL) {
//    FreePool (CurrentLang);
//  }
//  FreePool (BestLanguage);
//  FreePool (Lang);
//  FreePool (LanguageString);
//[-end-140620-IB05080432-remove]//

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    FRONT_PAGE_KEY_LANGUAGE,
    0,
    0,
    STRING_TOKEN (STR_LANGUAGE_SELECT),
    STRING_TOKEN (STR_LANGUAGE_SELECT_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
    );

  Status = HiiUpdateForm (
             HiiHandle,
             &gFrontPageFormSetGuid,
             FRONT_PAGE_FORM_ID,
             StartOpCodeHandle, // LABEL_SELECT_LANGUAGE
             EndOpCodeHandle    // LABEL_END
             );

//[-start-140620-IB05080432-modify]//
ExitInitFrontPage:
  if (CurrentLang != NULL) {
    FreePool (CurrentLang);
  }
  if (BestLanguage != NULL) {
    FreePool (BestLanguage);
  }
  if (Lang != NULL) {
    FreePool (Lang);
  }
  if (LanguageString != NULL) {
    FreePool (LanguageString);
  }
  if (StartOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (StartOpCodeHandle);
  }
  if (EndOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (EndOpCodeHandle);
  }
  if (OptionsOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (OptionsOpCodeHandle);
  }
//[-end-140620-IB05080432-modify]//
  return Status;
}

/**
  Call the browser and display the front page

  @return   Status code that will be returned by
            EFI_FORM_BROWSER2_PROTOCOL.SendForm ().

**/
EFI_STATUS
CallFrontPage (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;

  //
  // Begin waiting for USER INPUT
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_INPUT_WAIT)
    );

  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = gFormBrowser2->SendForm (
                            gFormBrowser2,
                            &gFrontPagePrivate.HiiHandle,
                            1,
                            &gFrontPageFormSetGuid,
                            0,
                            NULL,
                            &ActionRequest
                            );
  //
  // Check whether user change any option setting which needs a reset to be effective
  //
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    EnableResetRequired ();
  }

  return Status;
}


/**
  Convert Processor Frequency Data to a string.

  @param ProcessorFrequency The frequency data to process
  @param Base10Exponent     The exponent based on 10
  @param String             The string that is created

**/
VOID
ConvertProcessorToString (
  IN  UINT16                               ProcessorFrequency,
  IN  UINT16                               Base10Exponent,
  OUT CHAR16                               **String
  )
{
  CHAR16  *StringBuffer;
  UINTN   Index;
  UINT32  FreqMhz;

  if (Base10Exponent >= 6) {
    FreqMhz = ProcessorFrequency;
    for (Index = 0; Index < (UINTN) (Base10Exponent - 6); Index++) {
      FreqMhz *= 10;
    }
  } else {
    FreqMhz = 0;
  }

  StringBuffer = AllocateZeroPool (0x20);
//[-start-140620-IB05080432-modify]//
  if (StringBuffer == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
    DEBUG ((EFI_D_ERROR, "Fail to allocate memory for StringBuffer!!!\n"));
    ASSERT (StringBuffer != NULL);
  } else {
    Index = UnicodeValueToString (StringBuffer, LEFT_JUSTIFY, FreqMhz / 1000, 3);
    StrCat (StringBuffer, L".");
    UnicodeValueToString (StringBuffer + Index + 1, PREFIX_ZERO, (FreqMhz % 1000) / 10, 2);
    StrCat (StringBuffer, L" GHz");
    *String = (CHAR16 *) StringBuffer;
  }
//[-end-140620-IB05080432-modify]//
  return ;
}


/**
  Convert Memory Size to a string.

  @param MemorySize      The size of the memory to process
  @param String          The string that is created

**/
VOID
ConvertMemorySizeToString (
  IN  UINT32          MemorySize,
  OUT CHAR16          **String
  )
{
  CHAR16  *StringBuffer;

  StringBuffer = AllocateZeroPool (0x20);
//[-start-140620-IB05080432-modify]//
  if (StringBuffer == NULL) {
    DEBUG ((EFI_D_ERROR, "%a() line %d\n", __FUNCTION__, __LINE__));
    DEBUG ((EFI_D_ERROR, "Fail to allocate memory for StringBuffer!!!\n"));
    ASSERT (StringBuffer != NULL);
  } else {
    UnicodeValueToString (StringBuffer, LEFT_JUSTIFY, MemorySize, 6);
    StrCat (StringBuffer, L" MB RAM");
  
    *String = (CHAR16 *) StringBuffer;
  }
//[-end-140620-IB05080432-modify]//
  return ;
}

/**
  Update the banner information for the Front Page based on DataHub information.

**/
VOID
UpdateFrontPageStrings (
  VOID
  )
{
  UINT8                             StrIndex;
  CHAR16                            *NewString;
  BOOLEAN                           Find[5];
  EFI_STATUS                        Status;
  EFI_STRING_ID                     TokenToUpdate;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  SMBIOS_TABLE_TYPE0                *Type0Record;
  SMBIOS_TABLE_TYPE1                *Type1Record;
  SMBIOS_TABLE_TYPE4                *Type4Record;
  SMBIOS_TABLE_TYPE19               *Type19Record;
  EFI_SMBIOS_TABLE_HEADER           *Record;

  ZeroMem (Find, sizeof (Find));

  //
  // Update Front Page strings
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
      Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
      StrIndex = Type0Record->BiosVersion;
      BdsLibGetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), StrIndex, &NewString);
      TokenToUpdate = STRING_TOKEN (STR_FRONT_PAGE_BIOS_VERSION);
      HiiSetString (gFrontPagePrivate.HiiHandle, TokenToUpdate, NewString, NULL);
      FreePool (NewString);
      Find[0] = TRUE;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_SYSTEM_INFORMATION) {
      Type1Record = (SMBIOS_TABLE_TYPE1 *) Record;
      StrIndex = Type1Record->ProductName;
      BdsLibGetOptionalStringByIndex ((CHAR8*)((UINT8*)Type1Record + Type1Record->Hdr.Length), StrIndex, &NewString);
      TokenToUpdate = STRING_TOKEN (STR_FRONT_PAGE_COMPUTER_MODEL);
      HiiSetString (gFrontPagePrivate.HiiHandle, TokenToUpdate, NewString, NULL);
      FreePool (NewString);
      Find[1] = TRUE;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
      StrIndex = Type4Record->ProcessorVersion;
      BdsLibGetOptionalStringByIndex ((CHAR8*)((UINT8*)Type4Record + Type4Record->Hdr.Length), StrIndex, &NewString);
      TokenToUpdate = STRING_TOKEN (STR_FRONT_PAGE_CPU_MODEL);
      HiiSetString (gFrontPagePrivate.HiiHandle, TokenToUpdate, NewString, NULL);
      FreePool (NewString);
      Find[2] = TRUE;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
      ConvertProcessorToString(Type4Record->CurrentSpeed, 6, &NewString);
      TokenToUpdate = STRING_TOKEN (STR_FRONT_PAGE_CPU_SPEED);
      HiiSetString (gFrontPagePrivate.HiiHandle, TokenToUpdate, NewString, NULL);
      FreePool (NewString);
      Find[3] = TRUE;
    }

    if ( Record->Type == EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS ) {
      Type19Record = (SMBIOS_TABLE_TYPE19 *) Record;
      ConvertMemorySizeToString (
        (UINT32)(RShiftU64((Type19Record->EndingAddress - Type19Record->StartingAddress + 1), 10)),
        &NewString
        );
      TokenToUpdate = STRING_TOKEN (STR_FRONT_PAGE_MEMORY_SIZE);
      HiiSetString (gFrontPagePrivate.HiiHandle, TokenToUpdate, NewString, NULL);
      FreePool (NewString);
      Find[4] = TRUE;
    }
  } while ( !(Find[0] && Find[1] && Find[2] && Find[3] && Find[4]));
  return ;
}


/**
  Function waits for a given event to fire, or for an optional timeout to expire.

  @param   Event              The event to wait for
  @param   Timeout            An optional timeout value in 100 ns units.

  @retval  EFI_SUCCESS      Event fired before Timeout expired.
  @retval  EFI_TIME_OUT     Timout expired before Event fired..

**/
EFI_STATUS
WaitForSingleEvent (
  IN EFI_EVENT                  Event,
  IN UINT64                     Timeout OPTIONAL
  )
{
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_EVENT   TimerEvent;
  EFI_EVENT   WaitList[2];

  if (Timeout != 0) {
    //
    // Create a timer event
    //
    Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    if (!EFI_ERROR (Status)) {
      //
      // Set the timer event
      //
      gBS->SetTimer (
             TimerEvent,
             TimerRelative,
             Timeout
             );

      //
      // Wait for the original event or the timer
      //
      WaitList[0] = Event;
      WaitList[1] = TimerEvent;
      Status      = gBS->WaitForEvent (2, WaitList, &Index);
      gBS->CloseEvent (TimerEvent);

      //
      // If the timer expired, change the return to timed out
      //
      if (!EFI_ERROR (Status) && Index == 1) {
        Status = EFI_TIMEOUT;
      }
    }
  } else {
    //
    // No timeout... just wait on the event
    //
    Status = gBS->WaitForEvent (1, &Event, &Index);
    ASSERT (!EFI_ERROR (Status));
    ASSERT (Index == 0);
  }

  return Status;
}

/**
  Function show progress bar to wait for user input.


  @param   TimeoutDefault  The fault time out value before the system continue to boot.

  @retval  EFI_SUCCESS       User pressed some key except "Enter"
  @retval  EFI_TIME_OUT      Timeout expired or user press "Enter"

**/
EFI_STATUS
ShowProgress (
  IN UINT16                       TimeoutDefault
  )
{
  CHAR16                        *TmpStr;
  UINT16                        TimeoutRemain;
  EFI_STATUS                    Status;
  EFI_INPUT_KEY                 Key;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;

  if (TimeoutDefault == 0) {
    return EFI_TIMEOUT;
  }

  DEBUG ((EFI_D_INFO, "\n\nStart showing progress bar... Press any key to stop it! ...Zzz....\n"));

  SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
  SetMem (&Color, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);

  TmpStr = PlatformBdsGetStringById (STRING_TOKEN (STR_START_BOOT_OPTION));

  if (!FeaturePcdGet(PcdBootLogoOnlyEnable)) {
    //
    // Clear the progress status bar first
    //
    if (TmpStr != NULL) {
      BdsLibShowProgress (Foreground, Background, TmpStr, Color, 0, 0);
    }
  }


  TimeoutRemain = TimeoutDefault;
  while (TimeoutRemain != 0) {
    DEBUG ((EFI_D_INFO, "Showing progress bar...Remaining %d second!\n", TimeoutRemain));

    Status = WaitForSingleEvent (gST->ConIn->WaitForKey, ONE_SECOND);
    if (Status != EFI_TIMEOUT) {
      break;
    }
    TimeoutRemain--;

    if (!FeaturePcdGet(PcdBootLogoOnlyEnable)) {
      //
      // Show progress
      //
      if (TmpStr != NULL) {
        BdsLibShowProgress (
          Foreground,
          Background,
          TmpStr,
          Color,
          ((TimeoutDefault - TimeoutRemain) * 100 / TimeoutDefault),
          0
          );
      }
    }
  }

  if (TmpStr != NULL) {
    gBS->FreePool (TmpStr);
  }

  //
  // Timeout expired
  //
  if (TimeoutRemain == 0) {
    return EFI_TIMEOUT;
  }

  //
  // User pressed some key
  //
  Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
    //
    // User pressed enter, equivalent to select "continue"
    //
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  This function is the main entry of the platform setup entry.
  The function will present the main menu of the system setup,
  this is the platform reference part and can be customize.


  @param TimeoutDefault     The fault time out value before the system
                            continue to boot.
  @param ConnectAllHappened The indicater to check if the connect all have
                            already happened.

**/
VOID
PlatformBdsEnterFrontPage (
  IN UINT16                       TimeoutDefault,
  IN BOOLEAN                      ConnectAllHappened
  )
{
  EFI_STATUS                         Status;
  EFI_BOOT_LOGO_PROTOCOL             *BootLogo;
  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *SimpleTextOut;
  EFI_CONSOLE_CONTROL_PROTOCOL       *ConsoleControl;
  VOID                               *StartOpCodeHandle;
  VOID                               *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL                 *StartLabel;
  EFI_IFR_GUID_LABEL                 *EndLabel;
  EFI_IFR_REF                        *RefOpCode;
  EFI_IFR_IMAGE                      ImageOp;
  EFI_IFR_ANIMATION                  AnimationOp;
  DXE_MBP_DATA_PROTOCOL              *MBPDataProtocol;
  DXE_ME_POLICY_PROTOCOL             *MEPlatformPolicy;
  EFI_INPUT_KEY                      Key;
  CHAR16                             *StringBuffer1;
  CHAR16                             *StringBuffer2;
  EFI_TPL                            OriginalTpl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    ConsoleControl = NULL;
  }

  GraphicsOutput = NULL;
  SimpleTextOut = NULL;
  StartOpCodeHandle = NULL;
  EndOpCodeHandle = NULL;

  PERF_START (NULL, "BdsTimeOut", "BDS", 0);
  //
  // Indicate if we need connect all in the platform setup
  //
  if (ConnectAllHappened) {
    gConnectAllHappened = TRUE;
  }

  if (TimeoutDefault != 0xffff) {
    gBS->RestoreTPL (TPL_APPLICATION);
    Status = ShowProgress (TimeoutDefault);
    gBS->RaiseTPL (TPL_DRIVER);

    if (!FeaturePcdGet(PcdBootLogoOnlyEnable)){
      //
      // Ensure screen is clear when switch Console from Graphics mode to Text mode
      // Skip it in normal boot
      //
      gST->ConOut->EnableCursor (gST->ConOut, TRUE);
      gST->ConOut->ClearScreen (gST->ConOut);
    }

    if (EFI_ERROR (Status)) {
      //
      // Timeout or user press enter to continue
      //
      goto Exit;
    }
  }

  //
  //If support UEFI boot, create "Boot From File" option
  //
  if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
     StartOpCodeHandle = HiiAllocateOpCodeHandle ();
     ASSERT (StartOpCodeHandle != NULL);

     EndOpCodeHandle = HiiAllocateOpCodeHandle ();
     ASSERT (EndOpCodeHandle != NULL);

     StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
     StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
     StartLabel->Number       = FRONT_PAGE_BOOT_FROM_FILE_ID;

     EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
     EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
     EndLabel->Number       = FRONT_PAGE_BOOT_FROM_FILE_TAIL_ID;

     RefOpCode = (EFI_IFR_REF *) HiiCreateGotoOpCode (
                                   StartOpCodeHandle,
                                   0x0005,
                                   STRING_TOKEN (STR_BOOT_FROM_FILE),
                                   STRING_TOKEN (STR_BOOT_FROM_FILE_HELP),
                                   EFI_IFR_FLAG_CALLBACK,
                                   FRONT_PAGE_KEY_BOOT_MAINTAIN
                                   );
     RefOpCode->Header.Scope   = 1;
     if (FeaturePcdGet(PcdH2OFormBrowserSupported)) {
       ImageOp.Header.OpCode = EFI_IFR_IMAGE_OP;
       ImageOp.Header.Length = sizeof (EFI_IFR_IMAGE);
       ImageOp.Header.Scope  = 0;
       ImageOp.Id            = IMAGE_TOKEN (IMAGE_BOOT_FROM_FILE);
       HiiCreateRawOpCodes (StartOpCodeHandle, (UINT8 *) &ImageOp, sizeof(ImageOp));
     } else {
       AnimationOp.Header.OpCode = EFI_IFR_ANIMATION_OP;
       AnimationOp.Header.Length = sizeof (EFI_IFR_ANIMATION);
       AnimationOp.Header.Scope  = 0;
       AnimationOp.Id            = ANIMATION_TOKEN (ANI_BOOT_FROM_FILE);
       HiiCreateRawOpCodes (StartOpCodeHandle, (UINT8 *) &AnimationOp, sizeof(AnimationOp));
	 }
     HiiCreateEndOpCode (StartOpCodeHandle);

     RefOpCode = (EFI_IFR_REF *) HiiCreateGotoOpCode (
                                   StartOpCodeHandle,
                                   0x0007,
                                   STRING_TOKEN (STR_ADMINISTER_SECURE_BOOT),
                                   STRING_TOKEN (STR_ADMINISTER_SECURE_BOOT_HELP),
                                   EFI_IFR_FLAG_CALLBACK,
                                   FRONT_PAGE_KEY_SECURE_BOOT
                                   );
     RefOpCode->Header.Scope   = 1;
     if (FeaturePcdGet(PcdH2OFormBrowserSupported)) {
       ImageOp.Header.OpCode = EFI_IFR_IMAGE_OP;
       ImageOp.Header.Length = sizeof (EFI_IFR_IMAGE);
       ImageOp.Header.Scope  = 0;
       ImageOp.Id            = IMAGE_TOKEN (IMAGE_SECURE_BOOT_OPTION);
       HiiCreateRawOpCodes (StartOpCodeHandle, (UINT8 *) &ImageOp, sizeof(ImageOp));
     } else {
       AnimationOp.Header.OpCode = EFI_IFR_ANIMATION_OP;
       AnimationOp.Header.Length = sizeof (EFI_IFR_ANIMATION);
       AnimationOp.Header.Scope  = 0;
       AnimationOp.Id            = ANIMATION_TOKEN (ANI_SECURE_BOOT_OPTION);
       HiiCreateRawOpCodes (StartOpCodeHandle, (UINT8 *) &AnimationOp, sizeof(AnimationOp));
     }
     HiiCreateEndOpCode (StartOpCodeHandle);

     HiiUpdateForm (
       gFrontPagePrivate.HiiHandle,
       NULL,
       FRONT_PAGE_FORM_ID,
       StartOpCodeHandle,
       EndOpCodeHandle
       );
     HiiFreeOpCodeHandle (StartOpCodeHandle);
     HiiFreeOpCodeHandle (EndOpCodeHandle);
  }

  //
  // Boot Logo is corrupted, report it using Boot Logo protocol.
  //
  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **) &BootLogo);
  if (!EFI_ERROR (Status) && (BootLogo != NULL)) {
    BootLogo->SetBootLogo (BootLogo, NULL, 0, 0, 0, 0);
  }

  OriginalTpl = EfiGetCurrentTpl ();
  gBS->RestoreTPL (TPL_APPLICATION);

  Status = EFI_SUCCESS;
  do {
    //
    // Set proper video resolution and text mode for setup
    //
    BdsLibSetConsoleMode (TRUE);

    InitializeFrontPage (FALSE);

    //
    // Update Front Page strings
    //
    UpdateFrontPageStrings ();

//[-start-130611-IB11120016-add]//
    EfiNamedEventSignal (&gStartOfFrontPageProtocolGuid);
//[-end-130611-IB11120016-add]//

    gCallbackKey = 0;
    PERF_START (0, "BdsTimeOut", "BDS", 0);
    CallFrontPage ();
    PERF_END (0, "BdsTimeOut", "BDS", 0);

//[-start-130611-IB11120016-add]//
    EfiNamedEventSignal (&gEndOfFrontPageProtocolGuid);
//[-end-130611-IB11120016-add]//

//[-start-121119-IB08520065-modify]//
    //
    //  Let system password check when enter SCU when system password is set.
    //
    SetSysPasswordCheck ();
//[-end-121119-IB08520065-modify]//

    //
    // If gCallbackKey is greater than 1 and less or equal to 5,
    // it will launch configuration utilities.
    // 2 = set language
    // 3 = boot manager
    // 4 = device manager
    // 5 = boot maintenance manager
    //
    if (gCallbackKey != 0) {
      REPORT_STATUS_CODE (
        EFI_PROGRESS_CODE,
        (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP)
        );
    }
    //
    // Based on the key that was set, we can determine what to do
    //
    switch (gCallbackKey) {
    //
    // The first 4 entries in the Front Page are to be GUARANTEED to remain constant so IHV's can
    // describe to their customers in documentation how to find their setup information (namely
    // under the device manager and specific buckets)
    //
    // These entries consist of the Continue, Select language, Boot Manager, and Device Manager
    //
    case FRONT_PAGE_KEY_CONTINUE:
      //
      // User hit continue
      //
      break;

    case FRONT_PAGE_KEY_LANGUAGE:
      //
      // User made a language setting change - display front page again
      //
      break;

    case FRONT_PAGE_KEY_BOOT_MANAGER:
      //
      // User chose to run the Boot Manager
      //
      CallBootManager ();
      break;

    case FRONT_PAGE_KEY_DEVICE_MANAGER:
      //
      // Display the Device Manager
      //
      do {
        gCallbackKey = CallDeviceManager ();
      } while (gCallbackKey == FRONT_PAGE_KEY_DEVICE_MANAGER);
      break;

    case FRONT_PAGE_KEY_BOOT_MAINTAIN:
      //
      // Display the Boot Maintenance Manager
      //
      BdsStartBootMaint ();
      break;

    case FRONT_PAGE_KEY_SETUP_UTILITY:
      //
      // Display SetupUtility
      //
      BdsLibStartSetupUtility (FALSE);
      break;

    case FRONT_PAGE_KEY_SECURE_BOOT:
      //
      // Display SetupUtility
      //
      CallSecureBootMgr ();
      break;

    case FRONT_PAGE_KEY_MEBX:
      if (FeaturePcdGet (PcdMe5MbSupported)) {
        //
        // Display SetupUtility
        //
        Status = gBS->LocateProtocol (
                        &gMeBiosPayloadDataProtocolGuid,
                        NULL,
                        (VOID **)&MBPDataProtocol
                        );
        Status = gBS->LocateProtocol (
                        &gDxePlatformMePolicyGuid,
                        NULL,
                        (VOID **)&MEPlatformPolicy
                        );
        ASSERT_EFI_ERROR ( Status );

        if (MEPlatformPolicy->MeConfig.EndOfPostDone) {
          StringBuffer1 = AllocateZeroPool (200 * sizeof (CHAR16));
          ASSERT (StringBuffer1 != NULL);
          StringBuffer2 = AllocateZeroPool (200 * sizeof (CHAR16));
          ASSERT (StringBuffer2 != NULL);
          StrCpy (StringBuffer1, L"Mebx cannot enter after End Of Post. ");
          StrCpy (StringBuffer2, L"Please press any key to continue...");
          do {
            CreatePopUp (2, &Key, StringBuffer1, StringBuffer2);
          } while ((Key.ScanCode == SCAN_NULL) && (Key.UnicodeChar == SCAN_NULL));
        } else {
          if ( MBPDataProtocol->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType == INTEL_ME_5MB_FW ) {
            DisableQuietBoot ();
            gST->ConOut->EnableCursor (gST->ConOut, TRUE);
            if (ConsoleControl != NULL) {
              ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
            }
            BdsLibStartSetupUtility (TRUE);
            InvokeMebxHotKey ();
            //
            // need continue boot to enter MEBx
            //
            goto EndOfFrontPage;
          } else {
            StringBuffer1 = AllocateZeroPool (200 * sizeof (CHAR16));
            ASSERT (StringBuffer1 != NULL);
            StringBuffer2 = AllocateZeroPool (200 * sizeof (CHAR16));
            ASSERT (StringBuffer2 != NULL);
            StrCpy (StringBuffer1, L"Mebx is only supported with 5MB ME. ");
            StrCpy (StringBuffer2, L"Please press any key to continue...");
            do {
              CreatePopUp (2, &Key, StringBuffer1, StringBuffer2);
            } while ((Key.ScanCode == SCAN_NULL) && (Key.UnicodeChar == SCAN_NULL));
          }
        }
      }
      break;
    }
  } while ((Status == EFI_SUCCESS) && (gCallbackKey != FRONT_PAGE_KEY_CONTINUE));

EndOfFrontPage:
  gBS->RaiseTPL (OriginalTpl);

  //
  //Will leave browser, check any reset required change is applied? if yes, reset system
  //
  SetupResetReminder ();
Exit:
  //
  // Automatically load current entry
  // Note: The following lines of code only execute when Auto boot
  // takes affect
  //
  PERF_END (NULL, "BdsTimeOut", "BDS", 0);
}

