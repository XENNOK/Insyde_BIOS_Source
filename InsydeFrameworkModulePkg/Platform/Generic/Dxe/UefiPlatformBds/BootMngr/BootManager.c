//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  BootManager.c

Abstract:

  The platform boot manager reference implement

--*/

#include "BootManager.h"
#include "SetupConfig.h"

EFI_HII_DATABASE_PROTOCOL          *gBootManagerHiiDatabase;
EFI_HII_STRING_PROTOCOL            *gBootManagerHiiString;
EFI_FORM_BROWSER2_PROTOCOL         *gBootManagerFormBrowser2;
EFI_HII_CONFIG_ROUTING_PROTOCOL    *gBootManagerHiiConfigRouting;
BOOLEAN                            gConnectAllHappened;

EFI_STATUS
EFIAPI
BootManagerFakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
BootManagerFakeRouteConfig (
  IN  EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

UINT16             mKeyInput;
EFI_GUID           mBootManagerGuid = BOOT_MANAGER_FORMSET_GUID;
EFI_LIST_ENTRY     *mBootOptionsList;
BDS_COMMON_OPTION  *gOption;

BOOT_MANAGER_CALLBACK_DATA  gBootManagerPrivate = {
  BOOT_MANAGER_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
//  FakeExtractConfig,
//  FakeRouteConfig,
    BootManagerFakeExtractConfig,
    BootManagerFakeRouteConfig,
    BootManagerCallback
  }
};

EFI_STATUS
EFIAPI
BootManagerFakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
/*++

  Routine Description:
    This function allows a caller to extract the current configuration for one
    or more named elements from the target driver.

  Arguments:
    This       - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
    Request    - A null-terminated Unicode string in <ConfigRequest> format.
    Progress   - On return, points to a character in the Request string.
                 Points to the string's null terminator if request was successful.
                 Points to the most recent '&' before the first failing name/value
                 pair (or the beginning of the string if the failure is in the
                 first name/value pair) if the request was not successful.
    Results    - A null-terminated Unicode string in <ConfigAltResp> format which
                 has all values filled in for the names in the Request string.
                 String to be allocated by the called function.

  Returns:
    EFI_SUCCESS           - The Results is filled with the requested values.
    EFI_OUT_OF_RESOURCES  - Not enough memory to store the results.
    EFI_INVALID_PARAMETER - Request is NULL, illegal syntax, or unknown name.
    EFI_NOT_FOUND         - Routing data doesn't match any storage in this driver.

--*/
{
  if (Request == NULL) {
    Progress = NULL;
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Request;
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
BootManagerFakeRouteConfig (
  IN  EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
/*++

  Routine Description:
    This function processes the results of changes in configuration.

  Arguments:
    This          - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
    Configuration - A null-terminated Unicode string in <ConfigResp> format.
    Progress      - A pointer to a string filled in with the offset of the most
                    recent '&' before the first failing name/value pair (or the
                    beginning of the string if the failure is in the first
                    name/value pair) or the terminating NULL if all was successful.

  Returns:
    EFI_SUCCESS           - The Results is processed successfully.
    EFI_INVALID_PARAMETER - Configuration is NULL.
    EFI_NOT_FOUND         - Routing data doesn't match any storage in this driver.

--*/
{
  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
BootManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
/*++

  Routine Description:
    This function processes the results of changes in configuration.

  Arguments:
    This          - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
    Action        - Specifies the type of action taken by the browser.
    QuestionId    - A unique value which is sent to the original exporting driver
                    so that it can identify the type of data to expect.
    Type          - The type of value for the question.
    Value         - A pointer to the data being sent to the original exporting driver.
    ActionRequest - On return, points to the action requested by the callback function.

  Returns:
    EFI_SUCCESS          - The callback successfully handled the action.
    EFI_OUT_OF_RESOURCES - Not enough storage is available to hold the variable and its data.
    EFI_DEVICE_ERROR     - The variable could not be saved.
    EFI_UNSUPPORTED      - The specified Action is not supported by the callback.

--*/
{
  BDS_COMMON_OPTION       *Option;
  EFI_LIST_ENTRY          *Link;
  UINT16                  KeyCount;

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the key count
  //
  KeyCount = 0;

  for (Link = mBootOptionsList->ForwardLink; Link != mBootOptionsList; Link = Link->ForwardLink) {
    Option = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);

    KeyCount++;

    gOption = Option;

    //
    // Is this device the one chosen?
    //
    if (KeyCount == QuestionId) {
      //
      // Assigning the returned Key to a global allows the original routine to know what was chosen
      //
      mKeyInput = QuestionId;

      //
      // Request to exit SendForm(), so that we could boot the selected option
      //
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
      break;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeBootManager (
  VOID
  )
/*++

Routine Description:

  Initialize HII information for the FrontPage

Arguments:
  None

Returns:

--*/
{
  EFI_STATUS                  Status;
  EFI_HII_PACKAGE_LIST_HEADER *PackageList;

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, &gBootManagerHiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, &gBootManagerHiiString);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, &gBootManagerFormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, &gBootManagerHiiConfigRouting);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create driver handle used by HII database
  //
  Status = CreateHiiDriverHandle (&gBootManagerPrivate.DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install Config Access protocol to driver handle
  //
  Status = gBS->InstallProtocolInterface (
                  &gBootManagerPrivate.DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gBootManagerPrivate.ConfigAccess
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  PackageList = PreparePackageList (2, &mBootManagerGuid, BootManagerVfrBin, BdsStrings);
  ASSERT (PackageList != NULL);

  Status = gBootManagerHiiDatabase->NewPackageList (
                                      gBootManagerHiiDatabase,
                                      PackageList,
                                      gBootManagerPrivate.DriverHandle,
                                      &gBootManagerPrivate.HiiHandle
                                      );
  UpdateBvdtToHii (gBootManagerPrivate.HiiHandle);
  gBS->FreePool (PackageList);

  return Status;
}

CHAR16 *
GetPromptString (
  IN BDS_COMMON_OPTION                  *Option,
  IN HARDWARE_BOOT_DEVICE_INFO          *HwBootDeviceInfo,
  IN UINTN                              HwBootDeviceCount
  )
/*++

Routine Description:

  Get prompt string which is consist of the description of EFI boot option and hardware device name

Arguments:

  Option            - Pointer to boot option data
  HwBootDeviceInfo  - Pointer to the array of hardware device info
  HwBootDeviceCount - The number of hardware device info

Returns:

  Pointer to the prompt string or NULL if input parameter is invalid

--*/
{
  CHAR16                                NoDeviceStr[] = L"No Device";
  UINTN                                 Index;
  UINT8                                 *Ptr;
  UINT8                                 *VarData;
  UINTN                                 DeviceCount;
  UINTN                                 Size;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  CHAR16                                *HwDeviceName;
  CHAR16                                *PromptString;


  if (Option == NULL) {
    return NULL;
  }

  if (!((DevicePathType (Option->DevicePath) == MEDIA_DEVICE_PATH &&
         DevicePathSubType (Option->DevicePath) == MEDIA_HARDDRIVE_DP) ||
        (DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH &&
         DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP))) {
    return EfiLibAllocateCopyPool (EfiStrSize (Option->Description), Option->Description);
  }

  VarData      = NULL;
  HwDeviceName = NULL;
  DeviceCount  = 1;
  DevicePath   = Option->DevicePath;

  if (DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH &&
      DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP) {
    //
    // For Windows To Go, assign no device string as device name if device number is 0.
    //
    VarData = BdsLibGetVariableAndSize (
                L"WindowsToGo",
                &gEfiGenericVariableGuid,
                &Size
                );
    if (VarData != NULL) {
      Ptr         = VarData;
      DevicePath  = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
      Ptr        += EfiDevicePathSize (DevicePath);
      DeviceCount = *(UINT16 *) Ptr;
    } else {
      DeviceCount = 0;
    }

    if (DeviceCount == 0) {
      HwDeviceName = NoDeviceStr;
    }
  }

  //
  // Append device name to boot option description if BlkIo device path is match.
  //
  if (HwDeviceName == NULL) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      if (BdsLibCompareBlockIoDevicePath (HwBootDeviceInfo[Index].BlockIoDevicePath, DevicePath)) {
        HwDeviceName = HwBootDeviceInfo[Index].HwDeviceName;
        break;
      }
    }
  }

  if (HwDeviceName != NULL) {
    if (DeviceCount > 1) {
      Size = EfiStrSize (Option->Description) + EfiStrSize (L" (") + EfiStrSize (HwDeviceName) + EfiStrSize (L",...") + EfiStrSize (L")");
    } else {
      Size = EfiStrSize (Option->Description) + EfiStrSize (L" (") + EfiStrSize (HwDeviceName) + EfiStrSize (L")");
    }
    PromptString = EfiLibAllocateZeroPool (Size);
    if (PromptString != NULL) {
      EfiStrCat (PromptString, Option->Description);
      EfiStrCat (PromptString, L" (");
      EfiStrCat (PromptString, HwDeviceName);
      if (DeviceCount > 1) {
        EfiStrCat (PromptString, L",...");
      }
      EfiStrCat (PromptString, L")");
    }
  } else {
    PromptString = EfiLibAllocateCopyPool (EfiStrSize (Option->Description), Option->Description);
  }

  if (VarData != NULL) {
    gBS->FreePool (VarData);
  }

  return PromptString;
}

VOID
CallBootManager (
  VOID
  )
/*++

Routine Description:
  Hook to enable UI timeout override behavior.

Arguments:
  BdsDeviceList - Device List that BDS needs to connect.

  Entry - Pointer to current Boot Entry.

Returns:
  NONE

--*/
{
  EFI_STATUS                  Status;
  BDS_COMMON_OPTION           *Option;
  EFI_LIST_ENTRY              *Link;
  EFI_HII_UPDATE_DATA         UpdateData;
  CHAR16                      *ExitData;
  UINTN                       ExitDataSize;
  EFI_STRING_ID               Token;
  EFI_INPUT_KEY               Key;
  EFI_LIST_ENTRY              BdsBootOptionList;
  CHAR16                      *HelpString;
  EFI_STRING_ID               HelpToken;
  UINT16                      *TempStr;
  EFI_HII_HANDLE              HiiHandle;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;
#ifdef ENABLE_CONSOLE_EX
  EFI_KEY_DATA                       KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx = NULL;
#endif
  HARDWARE_BOOT_DEVICE_INFO          HwBootDeviceInfo[MAX_BOOT_DEVICES_NUMBER];
  UINTN                              HwBootDeviceCount;
  UINTN                              Index;
  CHAR16                             *PromptString;

  Token = 0;

  gOption = NULL;
  InitializeListHead (&BdsBootOptionList);

  //
  // Connect all prior to entering the platform setup menu.
  //
  if (!gConnectAllHappened) {
    BdsLibConnectAllDriversToAllControllers ();
    gConnectAllHappened = TRUE;
  }
  //
  // BugBug: Here we can not remove the legacy refresh macro, so we need
  // get the boot order every time from "BootOrder" variable.
  // Recreate the boot option list base on the BootOrder variable
  //
  BdsLibEnumerateAllBootOption (FALSE, &BdsBootOptionList);

  mBootOptionsList  = &BdsBootOptionList;

  HiiHandle = gBootManagerPrivate.HiiHandle;

  //
  // Allocate space for creation of UpdateData Buffer
  //
  UpdateData.BufferSize = 0x1000;
  UpdateData.Offset = 0;
  UpdateData.Data = EfiLibAllocateZeroPool (0x1000);
  ASSERT (UpdateData.Data != NULL);

  mKeyInput = 0;

  BdsLibGetAllHwBootDeviceInfo (&HwBootDeviceCount, HwBootDeviceInfo);

  for (Link = BdsBootOptionList.ForwardLink; Link != &BdsBootOptionList; Link = Link->ForwardLink) {
    Option = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
    //
    // At this stage we are creating a menu entry, thus the Keys are reproduceable
    //
    mKeyInput++;

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
    //
    // Don't display the boot option marked as LOAD_OPTION_HIDDEN
    //
    if (Option->Attribute & LOAD_OPTION_HIDDEN) {
      continue;
    }
#endif

    PromptString = GetPromptString (
                     Option,
                     HwBootDeviceInfo,
                     HwBootDeviceCount
                     );
    if (PromptString != NULL) {
      IfrLibNewString (HiiHandle, &Token, PromptString);
      gBS->FreePool (PromptString);
    } else {
      IfrLibNewString (HiiHandle, &Token, Option->Description);
    }

    TempStr = DevicePathToStr (Option->DevicePath);
    HelpString = EfiLibAllocateZeroPool (EfiStrSize (TempStr) + EfiStrSize (L"Device Path : "));
    EfiStrCat (HelpString, L"Device Path : ");
    EfiStrCat (HelpString, TempStr);

    IfrLibNewString (HiiHandle, &HelpToken, HelpString);
    gBS->FreePool (HelpString);

    CreateActionOpCode (
      mKeyInput,
      Token,
      //
      // Remove help string to suitable for current boot manager layout
      //
      0,
      EFI_IFR_FLAG_CALLBACK,
      0,
      &UpdateData
      );
  }

  IfrLibUpdateForm (
    HiiHandle,
    &mBootManagerGuid,
    BOOT_MANAGER_FORM_ID,
    LABEL_BOOT_OPTION,
    FALSE,
    &UpdateData
    );
  gBS->FreePool (UpdateData.Data);

  if (HwBootDeviceCount != 0) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      gBS->FreePool (HwBootDeviceInfo[Index].HwDeviceName);
    }
  }

  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  gBS->RestoreTPL (EFI_TPL_APPLICATION);

  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = gBootManagerFormBrowser2->SendForm (
                                       gBootManagerFormBrowser2,
                                       &HiiHandle,
                                       1,
                                       &mBootManagerGuid,
                                       0,
                                       NULL,
                                       &ActionRequest
                                       );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    EnableResetRequired ();
  }

  if (gOption == NULL) {
    gBS->RaiseTPL (EFI_TPL_DRIVER);
    return ;
  }

  //
  // Will leave browser, check any reset required change is applied? if yes, reset system
  //
  SetupResetReminder ();

  //
  // Raise the TPL level back to EFI_TPL_DRIVER
  //
  gBS->RaiseTPL (EFI_TPL_DRIVER);

  //
  // parse the selected option
  //
  Status = BdsLibBootViaBootOption (gOption, gOption->DevicePath, &ExitDataSize, &ExitData);

  if (!EFI_ERROR (Status)) {
    PlatformBdsBootSuccess (gOption);
  } else {
    PlatformBdsBootFail (gOption, Status, ExitData, ExitDataSize);
#ifdef ENABLE_CONSOLE_EX
    gBS->HandleProtocol (
                 gST->ConsoleInHandle,
                 &gEfiSimpleTextInputExProtocolGuid,
                 &SimpleTextInEx
                 );
    if (SimpleTextInEx != NULL) {
      SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
      Key = KeyData.Key;
    } else {
#endif
      gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
#ifdef ENABLE_CONSOLE_EX
    }
#endif

  }
}
