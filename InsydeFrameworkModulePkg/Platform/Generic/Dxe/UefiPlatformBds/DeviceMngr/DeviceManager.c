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

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name: 

  DeviceManager.c

Abstract:

  The platform device manager reference implement

--*/

#include "DeviceManager.h"
#include "Password.h"

EFI_HII_DATABASE_PROTOCOL          *gDMHiiDatabase;
EFI_HII_STRING_PROTOCOL            *gDMHiiString;
EFI_FORM_BROWSER2_PROTOCOL         *gDMFormBrowser2;
EFI_HII_CONFIG_ROUTING_PROTOCOL    *gDMHiiConfigRouting;
UINTN                              gDMCallbackKey;
#ifdef FRONTPAGE_SUPPORT
extern UINTN                       gCallbackKey;
#endif
extern BOOLEAN                     gConnectAllHappened;

EFI_STATUS
EFIAPI
DMFakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
DMFakeRouteConfig (
  IN  EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

DEVICE_MANAGER_CALLBACK_DATA  gDeviceManagerPrivate = {
  DEVICE_MANAGER_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  NULL,
  NULL,
  {
    DMFakeExtractConfig,
    DMFakeRouteConfig,
    DeviceManagerCallback
  },
  {
    DMFakeExtractConfig,
    DMFakeRouteConfig,
    DriverHealthCallback
  }
};

EFI_GUID mDeviceManagerGuid = DEVICE_MANAGER_FORMSET_GUID;
EFI_GUID gDriverHealthFormSetGuid = DRIVER_HEALTH_FORMSET_GUID;
EFI_GUID mPlatformSetupClassGuid = EFI_HII_PLATFORM_SETUP_FORMSET_GUID;

DEVICE_MANAGER_MENU_ITEM  mDeviceManagerMenuItemTable[] = {
  { STRING_TOKEN (STR_DISK_DEVICE),     EFI_DISK_DEVICE_CLASS },
  { STRING_TOKEN (STR_VIDEO_DEVICE),    EFI_VIDEO_DEVICE_CLASS },
  { STRING_TOKEN (STR_NETWORK_DEVICE),  EFI_NETWORK_DEVICE_CLASS },
  { STRING_TOKEN (STR_INPUT_DEVICE),    EFI_INPUT_DEVICE_CLASS },
  { STRING_TOKEN (STR_ON_BOARD_DEVICE), EFI_ON_BOARD_DEVICE_CLASS },
  { STRING_TOKEN (STR_OTHER_DEVICE),    EFI_OTHER_DEVICE_CLASS }
};

#define MENU_ITEM_NUM  \
  (sizeof (mDeviceManagerMenuItemTable) / sizeof (DEVICE_MANAGER_MENU_ITEM))

EFI_STATUS
EFIAPI
DMFakeExtractConfig (
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
DMFakeRouteConfig (
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
DeviceManagerCallback (
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
  DEVICE_MANAGER_CALLBACK_DATA *PrivateData;

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DEVICE_MANAGER_CALLBACK_DATA_FROM_THIS (This);

  switch (QuestionId) {
  case DEVICE_MANAGER_KEY_VBIOS:
    PrivateData->VideoBios = Value->u8;
    gRT->SetVariable (
           L"VBIOS",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof (UINT8),
           &PrivateData->VideoBios
           );

    //
    // Tell browser not to ask for confirmation of changes,
    // since we have already applied.
    //
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;
    break;

  default:
    //
    // The key corresponds the Handle Index which was requested to be displayed
    //
    gDMCallbackKey = QuestionId;

    //
    // Request to exit SendForm(), so as to switch to selected form
    //
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
    break;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeDeviceManager (
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

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, &gDMHiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, &gDMHiiString);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, &gDMFormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, &gDMHiiConfigRouting);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create driver handle used by HII database
  //
  Status = CreateHiiDriverHandle (&gDeviceManagerPrivate.DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install Config Access protocol to driver handle
  //
  Status = gBS->InstallProtocolInterface (
                  &gDeviceManagerPrivate.DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gDeviceManagerPrivate.ConfigAccess
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  PackageList = PreparePackageList (2, &mDeviceManagerGuid, DeviceManagerVfrBin, BdsStrings);
  ASSERT (PackageList != NULL);

  Status = gDMHiiDatabase->NewPackageList (
                             gDMHiiDatabase,
                             PackageList,
                             gDeviceManagerPrivate.DriverHandle,
                             &gDeviceManagerPrivate.HiiHandle
                             );
  gBS->FreePool (PackageList);

  Status = CreateHiiDriverHandle (&gDeviceManagerPrivate.DriverHealthHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->InstallProtocolInterface (
                  &gDeviceManagerPrivate.DriverHealthHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gDeviceManagerPrivate.DriverHealthConfigAccess
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
CallDeviceManager (
  VOID
  )
/*++

Routine Description:

  Call the browser and display the device manager

Arguments:

  None

Returns:
  EFI_SUCCESS            - Operation is successful.
  EFI_INVALID_PARAMETER  - If the inputs to SendForm function is not valid.

--*/
{
  EFI_STATUS                  Status;
  UINTN                       Count;
  UINTN                       Index;
  CHAR16                      *String;
  UINTN                       StringLength;
  EFI_HII_UPDATE_DATA         UpdateData[MENU_ITEM_NUM];
  EFI_STRING_ID               Token;
  EFI_STRING_ID               TokenHelp;
  IFR_OPTION                  *IfrOptionList;
  UINT8                       *VideoOption;
  UINTN                       VideoOptionSize;
  EFI_HII_HANDLE              *HiiHandles;
  UINTN                       HandleBufferLength;
  UINTN                       NumberOfHiiHandles;
  EFI_HII_HANDLE              HiiHandle;
  UINT16                      FormSetClass;
  UINT16                      FormSetClass2;
  EFI_STRING_ID               FormSetTitle;
  EFI_STRING_ID               FormSetHelp;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;
  EFI_HII_PACKAGE_LIST_HEADER *PackageList;
  UINT8                       NumberOfClassGuid;
  EFI_GUID                    *ClassGuid;
  UINTN                       NumHandles;
  EFI_HANDLE                  *DriverHealthHandles;

  IfrOptionList       = NULL;
  VideoOption         = NULL;
  HiiHandles          = NULL;
  HandleBufferLength  = 0;
  NumHandles          = 0;
  DriverHealthHandles = NULL;

  Status        = EFI_SUCCESS;
  gDMCallbackKey = 0;

  //
  // Connect all prior to entering the platform setup menu.
  //
  if (!gConnectAllHappened) {
    BdsLibConnectAllDriversToAllControllers ();
    gConnectAllHappened = TRUE;
  }

  //
  // Create Subtitle OpCodes
  //
  for (Index = 0; Index < MENU_ITEM_NUM; Index++) {
    //
    // Allocate space for creation of UpdateData Buffer
    //
    UpdateData[Index].BufferSize = 0x1000;
    UpdateData[Index].Offset = 0;
    UpdateData[Index].Data = EfiLibAllocatePool (0x1000);
    ASSERT (UpdateData[Index].Data != NULL);

    CreateSubTitleOpCode (mDeviceManagerMenuItemTable[Index].StringId, 0, 0, 1,  &UpdateData[Index]);
  }

  //
  // Get all the Hii handles
  //
  Status = GetHiiHandles (&HandleBufferLength, &HiiHandles);
  ASSERT_EFI_ERROR (Status);

  HiiHandle = gDeviceManagerPrivate.HiiHandle;

  StringLength  = 0x1000;
  String        = EfiLibAllocateZeroPool (StringLength);
  ASSERT (String != NULL);

  //
  // Search for formset of each class type
  //
  NumberOfHiiHandles = HandleBufferLength / sizeof (EFI_HII_HANDLE);
  for (Index = 0; Index < NumberOfHiiHandles; Index++) {

    FormSetClass = EFI_NON_DEVICE_CLASS;

    ClassGuid = NULL;
    NumberOfClassGuid = 0;
    ExtractClassGuidFromHiiHandle (HiiHandles[Index], &NumberOfClassGuid, &ClassGuid, &FormSetTitle, &FormSetHelp);
    if (NumberOfClassGuid != 0) {
      //
      // Check whether ClassGuid match EFI_HII_PLATFORM_SETUP_FORMSET_GUID
      //
      while (NumberOfClassGuid > 0) {
        if (EfiCompareGuid (&ClassGuid[NumberOfClassGuid - 1], &mPlatformSetupClassGuid)) {
          FormSetClass = EFI_OTHER_DEVICE_CLASS;
          break;
        }

        NumberOfClassGuid--;
      }
      gBS->FreePool (ClassGuid);
    }

    //
    // Try to extrat Tiano extend GUIDed Class
    //
    Status = ExtractClassFromHiiHandle (HiiHandles[Index], &FormSetClass2, &FormSetTitle, &FormSetHelp);
    if (!EFI_ERROR (Status)) {
      FormSetClass = FormSetClass2;
    }

    if (FormSetClass == EFI_NON_DEVICE_CLASS) {
      continue;
    }

    Token = 0;
    *String = 0;
    StringLength = 0x1000;
    IfrLibGetString (HiiHandles[Index], FormSetTitle, String, &StringLength);
    IfrLibNewString (HiiHandle, &Token, String);

    TokenHelp = 0;
    *String = 0;
    StringLength = 0x1000;
    IfrLibGetString (HiiHandles[Index], FormSetHelp, String, &StringLength);
    IfrLibNewString (HiiHandle, &TokenHelp, String);

    for (Count = 0; Count < MENU_ITEM_NUM; Count++) {
      if (FormSetClass & mDeviceManagerMenuItemTable[Count].Class) {
        CreateActionOpCode (
          (EFI_QUESTION_ID) (Index + DEVICE_KEY_OFFSET),
          Token,
          TokenHelp,
          EFI_IFR_FLAG_CALLBACK,
          0,
          &UpdateData[Count]
          );
      }
    }
  }
  gBS->FreePool (String);

  for (Index = 0; Index < MENU_ITEM_NUM; Index++) {
    //
    // Add End Opcode for Subtitle
    //
    CreateEndOpCode (&UpdateData[Index]);

    IfrLibUpdateForm (
      HiiHandle,
      &mDeviceManagerGuid,
      DEVICE_MANAGER_FORM_ID,
      mDeviceManagerMenuItemTable[Index].Class,
      FALSE,
      &UpdateData[Index]
      );
  }

  //
  // Add oneof for video BIOS selection
  //
  VideoOption = BdsLibGetVariableAndSize (
                  L"VBIOS",
                  &gEfiGenericVariableGuid,
                  &VideoOptionSize
                  );
  if (NULL == VideoOption) {
    gDeviceManagerPrivate.VideoBios = 0;
  } else {
    gDeviceManagerPrivate.VideoBios = VideoOption[0];
    gBS->FreePool (VideoOption);
  }

  ASSERT (gDeviceManagerPrivate.VideoBios <= 1);

  IfrOptionList = EfiLibAllocatePool (2 * sizeof (IFR_OPTION));
  ASSERT (IfrOptionList != NULL);
  IfrOptionList[0].Flags        = 0;
  IfrOptionList[0].StringToken  = STRING_TOKEN (STR_ONE_OF_PCI);
  IfrOptionList[0].Value.u8     = 0;
  IfrOptionList[1].Flags        = 0;
  IfrOptionList[1].StringToken  = STRING_TOKEN (STR_ONE_OF_AGP);
  IfrOptionList[1].Value.u8     = 1;
  IfrOptionList[gDeviceManagerPrivate.VideoBios].Flags |= EFI_IFR_OPTION_DEFAULT;

  UpdateData[0].Offset = 0;
  CreateOneOfOpCode (
    DEVICE_MANAGER_KEY_VBIOS,
    0,
    0,
    STRING_TOKEN (STR_ONE_OF_VBIOS),
    STRING_TOKEN (STR_ONE_OF_VBIOS_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,
    IfrOptionList,
    2,
    &UpdateData[0]
    );

  IfrLibUpdateForm (
    HiiHandle,
    &mDeviceManagerGuid,
    DEVICE_MANAGER_FORM_ID,
    LABEL_VBIOS,
    FALSE,
    &UpdateData[0]
    );

  if (IsPasswordExist ()) {
    Status = CheckPassword ();
  } else {
    UnlockPasswordState (NULL, 0);
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDriverHealthProtocolGuid,
                  NULL,
                  &NumHandles,
                  &DriverHealthHandles
                  );
  //
  // If there are no drivers installed driver health protocol, do not create driver health entry in UI
  //
  if (NumHandles != 0) {
    //
    // If driver health protocol is installed, create Driver Health subtitle and entry
    //
    UpdateData[0].Offset = 0;
    CreateSubTitleOpCode (STRING_TOKEN (STR_DM_DRIVER_HEALTH_TITLE), 0, 0, 0, &UpdateData[0]);

    CreateGotoOpCode (
      DRIVER_HEALTH_FORM_ID,
      STRING_TOKEN(STR_DRIVER_HEALTH_ALL_HEALTHY),      // Prompt text
      STRING_TOKEN(STR_DRIVER_HEALTH_STATUS_HELP),      // Help text
      EFI_IFR_FLAG_CALLBACK,
      DEVICE_MANAGER_KEY_DRIVER_HEALTH,
      &UpdateData[0]
      );

    //
    // Check All Driver health status
    //
    if (!PlaformHealthStatusCheck ()) {
      //
      // At least one driver in the platform are not in healthy status
      //
      IfrLibSetString (HiiHandle, STRING_TOKEN (STR_DRIVER_HEALTH_ALL_HEALTHY), GetStringById (STRING_TOKEN (STR_DRIVER_NOT_HEALTH)));
    } else {
      //
      // For the string of STR_DRIVER_HEALTH_ALL_HEALTHY previously has been updated and we need to update it while re-entry.
      //
      IfrLibSetString (HiiHandle, STRING_TOKEN (STR_DRIVER_HEALTH_ALL_HEALTHY), GetStringById (STRING_TOKEN (STR_DRIVER_HEALTH_ALL_HEALTHY)));
    }

    IfrLibUpdateForm (
      HiiHandle,
      &mDeviceManagerGuid,
      DEVICE_MANAGER_FORM_ID,
      LABEL_DEVICES_LIST,
      FALSE,
      &UpdateData[0]
      );
  }

  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  gBS->RestoreTPL (EFI_TPL_APPLICATION);

  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = gDMFormBrowser2->SendForm (
                              gDMFormBrowser2,
                              &HiiHandle,
                              1,
                              &mDeviceManagerGuid,
                              0,
                              NULL,
                              &ActionRequest
                              );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    EnableResetRequired ();
  }

  //
  // We will have returned from processing a callback - user either hit ESC to exit, or selected
  // a target to display
  //
  if (gDMCallbackKey != 0 && gDMCallbackKey != DEVICE_MANAGER_KEY_DRIVER_HEALTH) {
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    Status = gDMFormBrowser2->SendForm (
                                gDMFormBrowser2,
                                &HiiHandles[gDMCallbackKey - DEVICE_KEY_OFFSET],
                                1,
                                NULL,
                                0,
                                NULL,
                                &ActionRequest
                                );

    if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
      EnableResetRequired ();
    }

    //
    // Force return to Device Manager
    //
    gDMCallbackKey = FRONT_PAGE_KEY_DEVICE_MANAGER;
  }
  LockPasswordState ();
  //
  // Driver Health item chose. 
  //
  if (gDMCallbackKey == DEVICE_MANAGER_KEY_DRIVER_HEALTH) {
    gDMCallbackKey = FRONT_PAGE_KEY_DEVICE_MANAGER;
    CallDriverHealth ();
    //
    // Force return to Device Manager
    //
    gDMCallbackKey = FRONT_PAGE_KEY_DEVICE_MANAGER;
  }

  //
  // Cleanup dynamic created strings in HII database by reinstall the packagelist
  //
  gDMHiiDatabase->RemovePackageList (gDMHiiDatabase, HiiHandle);
  PackageList = PreparePackageList (2, &mDeviceManagerGuid, DeviceManagerVfrBin, BdsStrings);
  ASSERT (PackageList != NULL);
  Status = gDMHiiDatabase->NewPackageList (
                             gDMHiiDatabase,
                             PackageList,
                             gDeviceManagerPrivate.DriverHandle,
                             &gDeviceManagerPrivate.HiiHandle
                             );
  gBS->FreePool (PackageList);

  for (Index = 0; Index < MENU_ITEM_NUM; Index++) {
    gBS->FreePool (UpdateData[Index].Data);
  }
  gBS->FreePool (HiiHandles);

  gBS->RaiseTPL (EFI_TPL_DRIVER);

#ifdef FRONTPAGE_SUPPORT
  gCallbackKey = gDMCallbackKey;
#endif

  return Status;
}


EFI_STATUS
EFIAPI
DriverHealthCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
/*++

Routine Description:

  This function is invoked if user selected a interactive opcode from Driver Health's
  Formset. The decision by user is saved to gCallbackKey for later processing.

Arguments:

  This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  Action          Specifies the type of action taken by the browser.
  QuestionId      A unique value which is sent to the original exporting driver
                  so that it can identify the type of data to expect.
  Type            The type of value for the question.
  Value           A pointer to the data being sent to the original exporting driver.
  ActionRequest   On return, points to the action requested by the callback function.

Returns:
  EFI_SUCCESS            - The callback successfully handled the action.
  EFI_INVALID_PARAMETER  - The setup browser call this function with invalid parameters.

--*/
{
  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  gDMCallbackKey = QuestionId;

  //
  // Request to exit SendForm(), so as to switch to selected form
  //
  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;

  return EFI_SUCCESS;

}


VOID
CallDriverHealth (
  VOID
  )
/*++

Routine Description:

  Collect and display the platform's driver health relative information, allow user to do interactive 
  operation while the platform is unhealthy.

  This function display a form which divided into two parts. The one list all modules which has installed 
  driver health protocol. The list usually contain driver name, controller name, and it's health info.
  While the driver name can't be retrieved, will use device path as backup. The other part of the form provide
  a choice to the user to repair all platform.

Arguments:

Returns:

--*/
{
  EFI_STATUS                  Status; 
  EFI_HII_HANDLE              HiiHandle;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;
  UINTN                       Index;
  EFI_STRING_ID               Token;
  EFI_STRING_ID               TokenHelp;
  EFI_STRING                  String;
  EFI_STRING                  TmpString;
  EFI_STRING                  DriverName;
  EFI_STRING                  ControllerName;
  EFI_LIST_ENTRY                  DriverHealthList;
  DRIVER_HEALTH_INFO          *DriverHealthInfo;
  EFI_LIST_ENTRY                  *Link;
  EFI_DEVICE_PATH_PROTOCOL    *DriverDevicePath;
  BOOLEAN                     RebootRequired;
  EFI_HII_PACKAGE_LIST_HEADER *PackageList;
  EFI_HII_UPDATE_DATA         UpdateData;
  UINTN                       StringLength;

  Index               = 0;
  DriverHealthInfo    = NULL;  
  DriverDevicePath    = NULL;
  InitializeListHead (&DriverHealthList);

  HiiHandle = gDeviceManagerPrivate.DriverHealthHiiHandle;
  if (HiiHandle == NULL) {
    Status = CreateHiiDriverHandle (&gDeviceManagerPrivate.DriverHealthHandle);
    if (EFI_ERROR (Status)) {
      return;
    }

   
    PackageList = PreparePackageList (2, &mDeviceManagerGuid, DriverHealthVfrBin, BdsStrings);
    ASSERT (PackageList != NULL);

    Status = gDMHiiDatabase->NewPackageList (
                               gDMHiiDatabase,
                               PackageList,
                               gDeviceManagerPrivate.DriverHealthHandle,
                               &gDeviceManagerPrivate.DriverHealthHiiHandle
                               );
    HiiHandle = gDeviceManagerPrivate.DriverHealthHiiHandle;
    gBS->FreePool (PackageList);
    if (EFI_ERROR (Status)) {
      return;
    }
  }

  UpdateData.BufferSize = 0x1000;
  UpdateData.Data = EfiLibAllocateZeroPool (0x1000);
  ASSERT (UpdateData.Data != NULL);

  CreateSubTitleOpCode (STRING_TOKEN (STR_DH_STATUS_LIST), 0, 0, 1,  &UpdateData);

  Status = GetAllControllersHealthStatus (&DriverHealthList);
  ASSERT (Status != EFI_OUT_OF_RESOURCES);

  Link = GetFirstNode (&DriverHealthList);
  while (!IsNull (&DriverHealthList, Link)) {   
    DriverHealthInfo = DEVICE_MANAGER_HEALTH_INFO_FROM_LINK (Link);
    
    //
    // Assume no line strings is longer than 512 bytes.
    //
    String = (EFI_STRING) EfiLibAllocateZeroPool (0x200);
    ASSERT (String != NULL);

    Status = DriverHealthGetDriverName (DriverHealthInfo->DriverHandle, &DriverName);
    if (EFI_ERROR (Status)) {
      //
      // Can not get the Driver name, so use the Device path
      //
      DriverDevicePath = EfiDevicePathFromHandle (DriverHealthInfo->DriverHandle);
      DriverName       = DevicePathToStr (DriverDevicePath);
    }
    //
    // Add the Driver name & Controller name into FormSetTitle string
    // 
    EfiStrnCat (String, DriverName, EfiStrLen (DriverName));

    Status = DriverHealthGetControllerName (
               DriverHealthInfo->DriverHandle, 
               DriverHealthInfo->ControllerHandle, 
               DriverHealthInfo->ChildHandle, 
               &ControllerName
               );
    if (!EFI_ERROR (Status)) {
      //
      // Can not get the Controller name, just let it empty.
      //
      EfiStrnCat (String, L"    ", EfiStrLen (L"    "));
      EfiStrnCat (String, ControllerName, EfiStrLen (ControllerName));   
    }
   
    //
    // Add the message of the Module itself provided after the string item.
    //
    if ((DriverHealthInfo->MessageList != NULL) && 
        (DriverHealthInfo->MessageList->StringId != 0)) {
      EfiStrnCat (String, L"    ", EfiStrLen (L"    "));

      StringLength = 0x200;
      TmpString    = EfiLibAllocateZeroPool (StringLength);
      ASSERT (TmpString != NULL);
      
      IfrLibGetString (DriverHealthInfo->MessageList->HiiHandle, DriverHealthInfo->MessageList->StringId, TmpString, &StringLength);
    } else {
      //
      // Update the string will be displayed base on the driver's health status
      //
      switch(DriverHealthInfo->HealthStatus) {
       
      case EfiDriverHealthStatusRepairRequired:
        TmpString = GetStringById (STRING_TOKEN (STR_REPAIR_REQUIRED));
        break;
      case EfiDriverHealthStatusConfigurationRequired:
        TmpString = GetStringById (STRING_TOKEN (STR_CONFIGURATION_REQUIRED));
        break;
      case EfiDriverHealthStatusFailed:
        TmpString = GetStringById (STRING_TOKEN (STR_OPERATION_FAILED));
        break;
      case EfiDriverHealthStatusReconnectRequired:
        TmpString = GetStringById (STRING_TOKEN (STR_RECONNECT_REQUIRED));
        break;
      case EfiDriverHealthStatusRebootRequired:
        TmpString = GetStringById (STRING_TOKEN (STR_REBOOT_REQUIRED));
        break;
      default:
        TmpString = GetStringById (STRING_TOKEN (STR_DRIVER_HEALTH_HEALTHY));
        break;
      }
    }

    ASSERT (TmpString != NULL);
    EfiStrCat (String, TmpString);
    gBS->FreePool (TmpString);

    IfrLibNewString (HiiHandle, &Token, String);
    gBS->FreePool (String);

    IfrLibNewString (HiiHandle, &TokenHelp, GetStringById( STRING_TOKEN (STR_DH_REPAIR_SINGLE_HELP)));


    CreateActionOpCode (
      (EFI_QUESTION_ID) (Index + DRIVER_HEALTH_KEY_OFFSET),
      Token,
      TokenHelp,
      EFI_IFR_FLAG_CALLBACK,
      0,
      &UpdateData
      );
    Index++;
    Link = GetNextNode (&DriverHealthList, Link);
  }

  //
  // Add End Opcode for Subtitle
  // 
  CreateEndOpCode (&UpdateData);

  Status = IfrLibUpdateForm (
             HiiHandle,
             &gDriverHealthFormSetGuid,
             DRIVER_HEALTH_FORM_ID,
             LABEL_DRIVER_HEALTH,
             FALSE,
             &UpdateData
             );
  ASSERT (Status != EFI_NOT_FOUND);
  ASSERT (Status != EFI_BUFFER_TOO_SMALL);

  UpdateData.Offset = 0;
  CreateSubTitleOpCode (STRING_TOKEN (STR_DRIVER_HEALTH_REPAIR_ALL), 0, 0, 1,  &UpdateData);
  IfrLibNewString (HiiHandle, &TokenHelp, GetStringById( STRING_TOKEN (STR_DH_REPAIR_ALL_HELP)));

  if (PlaformHealthStatusCheck ()) {
    //
    // No action need to do for the platform
    //
    IfrLibNewString (HiiHandle, &Token, GetStringById( STRING_TOKEN (STR_DRIVER_HEALTH_ALL_HEALTHY)));
    CreateActionOpCode (
      0,
      Token,
      TokenHelp,
      EFI_IFR_FLAG_READ_ONLY,
      0,
      &UpdateData
      );
  } else {
    //
    // Create ActionOpCode only while the platform need to do health related operation.
    //
    IfrLibNewString (HiiHandle, &Token, GetStringById( STRING_TOKEN (STR_DH_REPAIR_ALL_TITLE)));
    CreateActionOpCode (
      (EFI_QUESTION_ID) DRIVER_HEALTH_REPAIR_ALL_KEY,
      Token,
      TokenHelp,
      EFI_IFR_FLAG_CALLBACK,
      0,
      &UpdateData
      );
  }
  CreateEndOpCode (&UpdateData);

  Status = IfrLibUpdateForm (
             HiiHandle,
             &gDriverHealthFormSetGuid,
             DRIVER_HEALTH_FORM_ID,
             LABEL_DRIVER_HEALTH_REAPIR_ALL,
             FALSE,
             &UpdateData
             );
  ASSERT (Status != EFI_NOT_FOUND);
  ASSERT (Status != EFI_BUFFER_TOO_SMALL);
  
  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = gDMFormBrowser2->SendForm (
                            gDMFormBrowser2,
                            &HiiHandle,
                            1,
                            &gDriverHealthFormSetGuid,
                            DRIVER_HEALTH_FORM_ID,
                            NULL,
                            &ActionRequest
                            );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    EnableResetRequired ();
  }

  //
  // We will have returned from processing a callback - user either hit ESC to exit, or selected a target to display.
  // Process the diver health status states here.
  // 
  if (gDMCallbackKey >= DRIVER_HEALTH_KEY_OFFSET && 
      gDMCallbackKey != DRIVER_HEALTH_REPAIR_ALL_KEY && 
      gDMCallbackKey != FRONT_PAGE_KEY_DEVICE_MANAGER) {
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

    Link = GetFirstNode (&DriverHealthList);
    Index = 0;

    while (!IsNull (&DriverHealthList, Link)) {
      //
      // Got the item relative node in the List
      //
      if (Index == (gDMCallbackKey - DRIVER_HEALTH_KEY_OFFSET)) { 
        DriverHealthInfo = DEVICE_MANAGER_HEALTH_INFO_FROM_LINK (Link);
        //
        // Process the driver's healthy status for the specify module
        //
        RebootRequired = FALSE;
        ProcessSingleControllerHealth (
          DriverHealthInfo->DriverHealth,
          DriverHealthInfo->ControllerHandle,      
          DriverHealthInfo->ChildHandle,
          DriverHealthInfo->HealthStatus,
          &(DriverHealthInfo->MessageList),
          DriverHealthInfo->HiiHandle,
          &RebootRequired
          );
        if (RebootRequired) {
          gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
        }
        break;
      }
      
      Index++;
      Link = GetNextNode (&DriverHealthList, Link);
    }

    if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
      EnableResetRequired ();
    }
    
    //
    // Force return to the form of Driver Health in Device Manager 
    //
    gDMCallbackKey = DRIVER_HEALTH_RETURN_KEY;
  }
  
  //
  // Repair the whole platform
  //
  if (gDMCallbackKey == DRIVER_HEALTH_REPAIR_ALL_KEY) {
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    
    PlatformRepairAll (&DriverHealthList);

    gDMCallbackKey = DRIVER_HEALTH_RETURN_KEY;
  }
   
  //
  // Remove driver health packagelist from HII database.
  //
  gDMHiiDatabase->RemovePackageList (gDMHiiDatabase, HiiHandle);
  gDeviceManagerPrivate.DriverHealthHiiHandle = NULL;

  //
  // Free driver health info list
  //
  while (!IsListEmpty (&DriverHealthList)) {
    Link = GetFirstNode(&DriverHealthList);
    DriverHealthInfo = DEVICE_MANAGER_HEALTH_INFO_FROM_LINK (Link);
    RemoveEntryList (Link);

    if (DriverHealthInfo->MessageList != NULL) {
      gBS->FreePool (DriverHealthInfo->MessageList);
      gBS->FreePool (DriverHealthInfo);
    }   
  }

  if (gDMCallbackKey == DRIVER_HEALTH_RETURN_KEY) {
    //
    // Force return to Driver Health Form
    //
    gDMCallbackKey = DEVICE_MANAGER_KEY_DRIVER_HEALTH;
    CallDriverHealth ();
  }
}


EFI_STATUS
EFIAPI
GetSingleControllerHealthStatus (
  IN OUT EFI_LIST_ENTRY                   *DriverHealthList,
  IN EFI_HANDLE                       DriverHandle,
  IN EFI_HANDLE                       ControllerHandle,  OPTIONAL
  IN EFI_HANDLE                       ChildHandle,       OPTIONAL
  IN EFI_DRIVER_HEALTH_PROTOCOL       *DriverHealth,
  IN EFI_DRIVER_HEALTH_STATUS         *HealthStatus
  )
/*++

Routine Description:

  Check the Driver Health status of a single controller and try to process it if not healthy.
  This function called by CheckAllControllersHealthStatus () function in order to process a specify contoller's health state.

Arguments:

  DriverHealthList   A Pointer to the list contain all of the platform driver health information. 
  DriverHandle       The handle of driver.
  ControllerHandle   The class guid specifies which form set will be displayed.
  ChildHandle        The handle of the child controller to retrieve the health status on.
                     This is an optional parameter that may be NULL. 
  DriverHealth       A pointer to the EFI_DRIVER_HEALTH_PROTOCOL instance.
  HealthStatus       The health status of the controller.

Returns:

  EFI_INVALID_PARAMETER   HealthStatus or DriverHealth is NULL.
  HealthStatus            The Health status of specify controller.
  EFI_OUT_OF_RESOURCES    The list of Driver Health Protocol handles can not be retrieved.
  EFI_NOT_FOUND           No controller in the platform install Driver Health Protocol.
  EFI_SUCCESS             The Health related operation has been taken successfully.

--*/
{
  EFI_STATUS                     Status;
  EFI_DRIVER_HEALTH_HII_MESSAGE  *MessageList;
  EFI_HII_HANDLE                 FormHiiHandle;
  DRIVER_HEALTH_INFO             *DriverHealthInfo;

  if (HealthStatus == NULL) {
    //
    // If HealthStatus is NULL, then return EFI_INVALID_PARAMETER
    //
    return EFI_INVALID_PARAMETER;
  }

  //
  // Assume the HealthStatus is healthy
  //
  *HealthStatus = EfiDriverHealthStatusHealthy;

  if (DriverHealth == NULL) {
    //
    // If DriverHealth is NULL, then return EFI_INVALID_PARAMETER
    //
    return EFI_INVALID_PARAMETER;
  }

  if (ControllerHandle == NULL) {
    //
    // If ControllerHandle is NULL, the return the cumulative health status of the driver
    //
    Status = DriverHealth->GetHealthStatus (DriverHealth, NULL, NULL, HealthStatus, NULL, NULL);
    if (*HealthStatus == EfiDriverHealthStatusHealthy) {
      //
      // Add the driver health related information into the list
      //
      DriverHealthInfo = EfiLibAllocateZeroPool (sizeof (DRIVER_HEALTH_INFO));
      if (DriverHealthInfo == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      DriverHealthInfo->Signature          = DEVICE_MANAGER_DRIVER_HEALTH_INFO_SIGNATURE;
      DriverHealthInfo->DriverHandle       = DriverHandle;
      DriverHealthInfo->ControllerHandle   = NULL;
      DriverHealthInfo->ChildHandle        = NULL;
      DriverHealthInfo->HiiHandle          = NULL;
      DriverHealthInfo->DriverHealth       = DriverHealth;
      DriverHealthInfo->MessageList        = NULL;
      DriverHealthInfo->HealthStatus       = *HealthStatus;

      InsertTailList (DriverHealthList, &DriverHealthInfo->Link);
    }
    return Status;
  }

  MessageList   = NULL;
  FormHiiHandle = NULL;

  //
  // Collect the health status with the optional HII message list
  //
  Status = DriverHealth->GetHealthStatus (DriverHealth, ControllerHandle, ChildHandle, HealthStatus, &MessageList, &FormHiiHandle);
  if (EFI_ERROR (Status)) {
    //
    // If the health status could not be retrieved, then return immediately
    //
    return Status;
  }

  //
  // Add the driver health related information into the list
  //
  DriverHealthInfo = EfiLibAllocateZeroPool (sizeof (DRIVER_HEALTH_INFO));
  if (DriverHealthInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DriverHealthInfo->Signature          = DEVICE_MANAGER_DRIVER_HEALTH_INFO_SIGNATURE; 
  DriverHealthInfo->DriverHandle       = DriverHandle;
  DriverHealthInfo->ControllerHandle   = ControllerHandle;
  DriverHealthInfo->ChildHandle        = ChildHandle;
  DriverHealthInfo->HiiHandle          = FormHiiHandle;
  DriverHealthInfo->DriverHealth       = DriverHealth;
  DriverHealthInfo->MessageList        = MessageList;
  DriverHealthInfo->HealthStatus       = *HealthStatus;

  InsertTailList (DriverHealthList, &DriverHealthInfo->Link);

  return EFI_SUCCESS;
}


EFI_STATUS
GetAllControllersHealthStatus (
  IN OUT EFI_LIST_ENTRY  *DriverHealthList
  )
/*++

Routine Description:

  Collects all the EFI Driver Health Protocols currently present in the EFI Handle Database, 
  and queries each EFI Driver Health Protocol to determine if one or more of the controllers 
  managed by each EFI Driver Health Protocol instance are not healthy.

Arguments:

  DriverHealthList   A Pointer to the list contain all of the platform driver health information.

Returns:

  EFI_NOT_FOUND         No controller in the platform install Driver Health Protocol.
  EFI_SUCCESS           All the controllers in the platform are healthy.
  EFI_OUT_OF_RESOURCES  The list of Driver Health Protocol handles can not be retrieved.

--*/
{
  EFI_STATUS                 Status; 
  UINTN                      NumHandles;
  EFI_HANDLE                 *DriverHealthHandles;
  EFI_DRIVER_HEALTH_PROTOCOL *DriverHealth;
  EFI_DRIVER_HEALTH_STATUS   HealthStatus;
  UINTN                      DriverHealthIndex;
  EFI_HANDLE                 *Handles;
  UINTN                      HandleCount;
  UINTN                      ControllerIndex;
  UINTN                      ChildIndex;
 
  //
  // Initialize local variables
  //
  Handles                 = NULL;
  DriverHealthHandles     = NULL;
  NumHandles              = 0;
  HandleCount             = 0;

  HealthStatus = EfiDriverHealthStatusHealthy;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDriverHealthProtocolGuid,
                  NULL,
                  &NumHandles,
                  &DriverHealthHandles
                  );
  if (Status == EFI_NOT_FOUND || NumHandles == 0) {
    //
    // If there are no Driver Health Protocols handles, then return EFI_NOT_FOUND
    //
    return EFI_NOT_FOUND;
  }

  if (EFI_ERROR (Status) || DriverHealthHandles == NULL) {
    //
    // If the list of Driver Health Protocol handles can not be retrieved, then 
    // return EFI_OUT_OF_RESOURCES
    //
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Check the health status of all controllers in the platform
  // Start by looping through all the Driver Health Protocol handles in the handle database
  //
  for (DriverHealthIndex = 0; DriverHealthIndex < NumHandles; DriverHealthIndex++) {
    //
    // Skip NULL Driver Health Protocol handles
    //
    if (DriverHealthHandles[DriverHealthIndex] == NULL) {
      continue;
    }

    //
    // Retrieve the Driver Health Protocol from DriverHandle
    //
    Status = gBS->HandleProtocol ( 
                    DriverHealthHandles[DriverHealthIndex],
                    &gEfiDriverHealthProtocolGuid,
                    (VOID **)&DriverHealth
                    );
    if (EFI_ERROR (Status)) {
      //
      // If the Driver Health Protocol can not be retrieved, then skip to the next Driver Health Protocol handle
      //
      continue;
    }

    //
    // Check the health of all the controllers managed by a Driver Health Protocol handle
    //
    Status = GetSingleControllerHealthStatus (DriverHealthList, DriverHealthHandles[DriverHealthIndex], NULL, NULL, DriverHealth, &HealthStatus);
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // If all the controllers managed by this Driver Health Protocol are healthy, then skip to the next Driver Health Protocol handle
    //
    if (HealthStatus == EfiDriverHealthStatusHealthy) {
      continue;
    }

    //
    // See if the list of all handles in the handle database has been retrieved
    //
    //
    if (Handles == NULL) {
      //
      // Retrieve the list of all handles from the handle database
      //
      Status = gBS->LocateHandleBuffer (
                      AllHandles,
                      NULL,
                      NULL,
                      &HandleCount,
                      &Handles
                      );
      if (EFI_ERROR (Status) || Handles == NULL) {
        //
        // If all the handles in the handle database can not be retrieved, then 
        // return EFI_OUT_OF_RESOURCES
        //
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
    }
    
    //
    // Loop through all the controller handles in the handle database
    //
    for (ControllerIndex = 0; ControllerIndex < HandleCount; ControllerIndex++) {
      //
      // Skip NULL controller handles
      //
      if (Handles[ControllerIndex] == NULL) {
        continue;
      }

      Status = GetSingleControllerHealthStatus (DriverHealthList, DriverHealthHandles[DriverHealthIndex], Handles[ControllerIndex], NULL, DriverHealth, &HealthStatus);
      if (EFI_ERROR (Status)) {
        //
        // If Status is an error code, then the health information could not be retrieved, so assume healthy
        //
        HealthStatus = EfiDriverHealthStatusHealthy;
      }

      //
      // If CheckHealthSingleController() returned an error on a terminal state, then do not check the health of child controllers
      //
      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Loop through all the child handles in the handle database
      //
      for (ChildIndex = 0; ChildIndex < HandleCount; ChildIndex++) {
        //
        // Skip NULL child handles
        //
        if (Handles[ChildIndex] == NULL) {
          continue;
        }

        Status = GetSingleControllerHealthStatus (DriverHealthList, DriverHealthHandles[DriverHealthIndex], Handles[ControllerIndex], Handles[ChildIndex], DriverHealth, &HealthStatus);
        if (EFI_ERROR (Status)) {
          //
          // If Status is an error code, then the health information could not be retrieved, so assume healthy
          //
          HealthStatus = EfiDriverHealthStatusHealthy;
        }

        //
        // If CheckHealthSingleController() returned an error on a terminal state, then skip to the next child
        //
        if (EFI_ERROR (Status)) {
          continue;
        }
      }
    }
  }

  Status = EFI_SUCCESS;

Done:
  if (Handles != NULL) {
    gBS->FreePool (Handles);
  }
  if (DriverHealthHandles != NULL) {
    gBS->FreePool (DriverHealthHandles);
  }

  return Status;
}


BOOLEAN
PlaformHealthStatusCheck (
  VOID
  )
/*++

Routine Description:

  Check the healthy status of the platform, this function will return immediately while found one driver in the platform are not healthy.

Arguments:

Returns:

  FALSE      at least one driver in the platform are not healthy.
  TRUE       No controller install Driver Health Protocol,
             or all controllers in the platform are in healthy status.

--*/
{
  EFI_DRIVER_HEALTH_STATUS          HealthStatus;
  EFI_STATUS                        Status;
  UINTN                             Index;
  UINTN                             NoHandles;
  EFI_HANDLE                        *DriverHealthHandles;
  EFI_DRIVER_HEALTH_PROTOCOL        *DriverHealth;
  BOOLEAN                           AllHealthy;

  //
  // Initialize local variables
  //
  DriverHealthHandles = NULL;
  DriverHealth        = NULL;

  HealthStatus = EfiDriverHealthStatusHealthy;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDriverHealthProtocolGuid,
                  NULL,
                  &NoHandles,
                  &DriverHealthHandles
                  );
  //
  // There are no handles match the search for Driver Health Protocol has been installed.
  //
  if (Status == EFI_NOT_FOUND) {
    return TRUE;
  }
  //
  // Assume all modules are healthy.
  // 
  AllHealthy = TRUE;

  //
  // Found one or more Handles.
  //
  if (!EFI_ERROR (Status)) {    
    for (Index = 0; Index < NoHandles; Index++) {
      Status = gBS->HandleProtocol (
                      DriverHealthHandles[Index],
                      &gEfiDriverHealthProtocolGuid,
                      (VOID **) &DriverHealth
                      );
      if (!EFI_ERROR (Status)) {
        Status = DriverHealth->GetHealthStatus (
                                 DriverHealth,
                                 NULL,
                                 NULL,
                                 &HealthStatus,
                                 NULL,
                                 NULL
                                 );
      }
      //
      // Get the healthy status of the module
      //
      if (!EFI_ERROR (Status)) {
         if (HealthStatus != EfiDriverHealthStatusHealthy) {
           //
           // Return immediately one driver's status not in healthy.
           //
           return FALSE;         
         }
      }
    }
  }
  return AllHealthy;
}

VOID
ProcessSingleControllerHealth (
  IN  EFI_DRIVER_HEALTH_PROTOCOL         *DriverHealth,
  IN  EFI_HANDLE                         ControllerHandle, //OPTIONAL
  IN  EFI_HANDLE                         ChildHandle,      //OPTIONAL
  IN  EFI_DRIVER_HEALTH_STATUS           HealthStatus,
  IN  EFI_DRIVER_HEALTH_HII_MESSAGE      **MessageList,    //OPTIONAL
  IN  EFI_HII_HANDLE                     FormHiiHandle,
  IN OUT BOOLEAN                         *RebootRequired
  )
/*++

Routine Description:

  Processes a single controller using the EFI Driver Health Protocol associated with that controller.
  This algorithm continues to query the GetHealthStatus() service until one of the legal terminal states of the EFI Driver Health Protocol is reached. 
  This may require the processing of HII Messages, HII Form, and invocation of repair operations.

Arguments:

  DriverHealth       A pointer to the EFI_DRIVER_HEALTH_PROTOCOL instance.
  ControllerHandle   The class guid specifies which form set will be displayed.
  ChildHandle        The handle of the child controller to retrieve the health status on.
                     This is an optional parameter that may be NULL. 
  HealthStatus       The health status of the controller.
  MessageList        An array of warning or error messages associated with the controller specified by ControllerHandle and ChildHandle.
                     This is an optional parameter that may be NULL.
  FormHiiHandle      The HII handle for an HII form associated with the controller specified by ControllerHandle and ChildHandle.
  RebootRequired     Indicate whether a reboot is required to repair the controller.

Returns:

--*/
{
  EFI_STATUS                         Status;
  EFI_DRIVER_HEALTH_STATUS           LocalHealthStatus;
  
  LocalHealthStatus = HealthStatus;
  //
  // If the module need to be repaired or reconfiguration, will process it until reach a terminal status.
  // The status from EfiDriverHealthStatusRepairRequired after repair will be in (Health, Failed, Configuration Required).
  //
  while (LocalHealthStatus == EfiDriverHealthStatusConfigurationRequired ||
         LocalHealthStatus == EfiDriverHealthStatusRepairRequired) {
    if (LocalHealthStatus == EfiDriverHealthStatusRepairRequired) {
      Status = DriverHealth->Repair (
                               DriverHealth,
                               ControllerHandle,
                               ChildHandle,
                               (EFI_DRIVER_HEALTH_REPAIR_PROGRESS_NOTIFY) RepairNotify
                               );
    }
    //
    // Via a form of the driver need to do configuration provided to process of status in EfiDriverHealthStatusConfigurationRequired.
    // The status after configuration should be in (Healthy, Reboot Required, Failed, Reconnect Required, Repair Required).   
    //
    if (LocalHealthStatus == EfiDriverHealthStatusConfigurationRequired) {
      if (FormHiiHandle != NULL) {
        Status = gDMFormBrowser2->SendForm (
                                    gDMFormBrowser2,
                                    &FormHiiHandle,
                                    1,
                                    &gEfiHiiDriverHealthFormsetGuid,
                                    0,
                                    NULL,
                                    NULL
                                    );
        ASSERT(!EFI_ERROR (Status));
      } else {
        //
        // Exit the loop in case no FormHiiHandle is supplied to prevent dead-loop
        //
        break;
      }
    }

    Status = DriverHealth->GetHealthStatus (
                             DriverHealth,
                             ControllerHandle,
                             ChildHandle,
                             &LocalHealthStatus,
                             NULL,
                             &FormHiiHandle
                             );
    ASSERT_EFI_ERROR (Status);

    if (*MessageList != NULL) {
      ProcessMessages (*MessageList);
    }  
  }
  
  //
  // Health status in {Healthy, Failed} may also have Messages need to process
  //
  if (LocalHealthStatus == EfiDriverHealthStatusHealthy || 
      LocalHealthStatus == EfiDriverHealthStatusFailed) {
    if (*MessageList != NULL) {
      ProcessMessages (*MessageList);
    }
  }
  
  //
  // Check for RebootRequired or ReconnectRequired
  //
  if (LocalHealthStatus == EfiDriverHealthStatusRebootRequired) {
    *RebootRequired = TRUE;
  }
  
  //
  // Do reconnect if need.
  //
  if (LocalHealthStatus == EfiDriverHealthStatusReconnectRequired) {
    Status = gBS->DisconnectController (ControllerHandle, NULL, NULL);
    if (EFI_ERROR (Status)) {
      //
      // Disconnect failed.  Need to promote reconnect to a reboot.
      //
      *RebootRequired = TRUE;
    } else {
      gBS->ConnectController (ControllerHandle, NULL, NULL, TRUE);
    }
  }
}

VOID
RepairNotify (
  IN  UINTN Value,
  IN  UINTN Limit
  )
/*++

Routine Description:

  Platform specific notification function for controller repair operations.

  If the driver for a controller support the Driver Health Protocol and the
  current state of the controller is EfiDriverHealthStatusRepairRequired then
  when the Repair() service of the Driver Health Protocol is called, this 
  platform specific notification function can display the progress of the repair
  operation.  Some platforms may choose to not display anything, other may choose
  to show the percentage complete on text consoles, and other may choose to render
  a progress bar on text and graphical consoles.

  This function displays the percentage of the repair operation that has been
  completed on text consoles.  The percentage is Value / Limit * 100%.
  
Arguments:

  Value               Value in the range 0..Limit the the repair has completed..
  Limit               The maximum value of Value
  
Returns:

--*/
{
  UINTN Percent;

  if (Limit  == 0) {
    Print(L"Repair Progress Undefined\n\r");
  } else {
    Percent = Value * 100 / Limit;
    Print(L"Repair Progress = %3d%%\n\r", Percent);
  }
}

VOID
ProcessMessages (
  IN  EFI_DRIVER_HEALTH_HII_MESSAGE      *MessageList
  )
/*++

Routine Description:

  Processes a set of messages returned by the GetHealthStatus() service of the EFI Driver Health Protocol
  
Arguments:

  MessageList         MessageList  The MessageList point to messages need to processed.  
  
Returns:

--*/
{
  UINTN                           MessageIndex;
  EFI_STRING                      MessageString;

  for (MessageIndex = 0;
       MessageList[MessageIndex].HiiHandle != NULL;
       MessageIndex++) {
    MessageString = NULL;
    GetStringFromHandle (MessageList[MessageIndex].HiiHandle, MessageList[MessageIndex].StringId, &MessageString);
    if (MessageString != NULL) {
      //
      // User can customize the output. Just simply print out the MessageString like below. 
      // Also can use the HiiHandle to display message on the front page.
      // 
      // Print(L"%s\n",MessageString);
      // gBS->Stall (100000);
      gBS->FreePool (MessageString);
    }
  }
}

VOID
PlatformRepairAll (
  IN EFI_LIST_ENTRY  *DriverHealthList
  )
/*++

Routine Description:

  Repair the whole platform.

  This function is the main entry for user choose "Repair All" in the front page.
  It will try to do recovery job till all the driver health protocol installed modules reach a terminal state.
  
Arguments:

  DriverHealthList   A Pointer to the list contain all of the platform driver health information.
  
Returns:

--*/
{ 
  DRIVER_HEALTH_INFO          *DriverHealthInfo;
  EFI_LIST_ENTRY              *Link;
  BOOLEAN                     RebootRequired;

  ASSERT (DriverHealthList != NULL);

  RebootRequired = FALSE;

  for (Link = GetFirstNode (DriverHealthList); 
       !IsNull (DriverHealthList, Link); 
       Link = GetNextNode (DriverHealthList, Link)) {
    DriverHealthInfo = DEVICE_MANAGER_HEALTH_INFO_FROM_LINK (Link);
    //
    // Do driver health status operation by each link node
    //
    ASSERT (DriverHealthInfo != NULL);

    ProcessSingleControllerHealth ( 
      DriverHealthInfo->DriverHealth,
      DriverHealthInfo->ControllerHandle,
      DriverHealthInfo->ChildHandle,
      DriverHealthInfo->HealthStatus,
      &(DriverHealthInfo->MessageList),
      DriverHealthInfo->HiiHandle,
      &RebootRequired
      );
  }

  if (RebootRequired) {
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }
}

CHAR8 *
EFIAPI
GetBestLanguage (
  IN CONST CHAR8  *SupportedLanguages, 
  IN BOOLEAN      Iso639Language,
  ...
  )
{
  VA_LIST      Args;
  CHAR8        *Language;
  UINTN        CompareLength;
  UINTN        LanguageLength;
  CONST CHAR8  *Supported;
  CHAR8        *BestLanguage;

  ASSERT (SupportedLanguages != NULL);

  VA_START (Args, Iso639Language);
  while ((Language = VA_ARG (Args, CHAR8 *)) != NULL) {
    //
    // Default to ISO 639-2 mode
    //
    CompareLength  = 3;
    LanguageLength = EFI_MIN (3, EfiAsciiStrLen (Language));

    //
    // If in RFC 4646 mode, then determine the length of the first RFC 4646 language code in Language
    //
    if (!Iso639Language) {
      for (LanguageLength = 0; Language[LanguageLength] != 0 && Language[LanguageLength] != ';'; LanguageLength++)
        ;
    }

    //
    // Trim back the length of Language used until it is empty
    //
    while (LanguageLength > 0) {
      //
      // Loop through all language codes in SupportedLanguages
      //
      for (Supported = SupportedLanguages; *Supported != '\0'; Supported += CompareLength) {
        //
        // In RFC 4646 mode, then Loop through all language codes in SupportedLanguages
        //
        if (!Iso639Language) {
          //
          // Skip ';' characters in Supported
          //
          for (; *Supported != '\0' && *Supported == ';'; Supported++);
          //
          // Determine the length of the next language code in Supported
          //
          for (CompareLength = 0; Supported[CompareLength] != 0 && Supported[CompareLength] != ';'; CompareLength++);
          //
          // If Language is longer than the Supported, then skip to the next language
          //
          if (LanguageLength > CompareLength) {
            continue;
          }
        }
        //
        // See if the first LanguageLength characters in Supported match Language
        //
        if (EfiAsciiStrnCmp ((CHAR8 *) Supported, Language, LanguageLength) == 0) {
          VA_END (Args);
          //
          // Allocate, copy, and return the best matching language code from SupportedLanguages
          //
          BestLanguage = EfiLibAllocateZeroPool (CompareLength + 1);
          if (BestLanguage == NULL) {
            return NULL;
          }
          EfiCopyMem (BestLanguage, (VOID *) Supported, CompareLength);
          return BestLanguage;
        }
      }

      if (Iso639Language) {
        //
        // If ISO 639 mode, then each language can only be tested once
        //
        LanguageLength = 0;
      } else {
        //
        // If RFC 4646 mode, then trim Language from the right to the next '-' character 
        //
        for (LanguageLength--; LanguageLength > 0 && Language[LanguageLength] != '-'; LanguageLength--)
          ;
      }
    }
  }
  VA_END (Args);

  //
  // No matches were found 
  //
  return NULL;
}

CHAR8 *
DriverHealthSelectBestLanguage (
  IN CHAR8        *SupportedLanguages,
  IN BOOLEAN      Iso639Language
  )
/*++

Routine Description:

  Select the best matching language according to front page policy for best user experience.   
  This function supports both ISO 639-2 and RFC 4646 language codes, but language code types may not be mixed in a single call to this function. 
  
Arguments:

  SupportedLanguages   A pointer to a Null-terminated ASCII string that contains a set of language codes in the format specified by Iso639Language.
  Iso639Language       If TRUE , then all language codes are assumed to be in ISO 639-2 format.  
                       If FALSE, then all language codes are assumed to be in RFC 4646 language format.
  
Returns:

  NULL                 The best matching language could not be found in SupportedLanguages.
  NULL                 There are not enough resources available to return the best matching language.
  Other                A pointer to a Null-terminated ASCII string that is the best matching language in SupportedLanguages.
  
--*/
{
  CHAR8           *LanguageVariable;
  CHAR8           *BestLanguage;
  UINTN           VariableSize;

  LanguageVariable = BdsLibGetVariableAndSize (
                       Iso639Language ? L"Lang" : L"PlatformLang",
                       &gEfiGenericVariableGuid,
                       &VariableSize
                       );

  BestLanguage = GetBestLanguage (
                   SupportedLanguages,
                   Iso639Language,
                   (LanguageVariable != NULL) ? LanguageVariable : "",
                   Iso639Language ? "eng" : "en-US",
                   NULL
                   );
  if (LanguageVariable != NULL) {
    gBS->FreePool (LanguageVariable);
  }

  return BestLanguage;
}

EFI_STATUS
GetComponentNameWorker (
  IN  EFI_GUID                    *ProtocolGuid,
  IN  EFI_HANDLE                  DriverBindingHandle,
  OUT EFI_COMPONENT_NAME_PROTOCOL **ComponentName,
  OUT CHAR8                       **SupportedLanguage
  )
/*++

Routine Description:

  This is an internal worker function to get the Component Name (2) protocol interface and the language it supports.
  
Arguments:

  ProtocolGuid         A pointer to an EFI_GUID. It points to Component Name (2) protocol GUID.
  DriverBindingHandle  The handle on which the Component Name (2) protocol instance is retrieved.
  ComponentName        A pointer to the Component Name (2) protocol interface.
  SupportedLanguage    The best suitable language that matches the SupportedLangues interface for the located Component Name (2) instance.
  
Returns:

  EFI_SUCCESS          The Component Name (2) protocol instance is successfully located and we find the best matching language it support.
  EFI_UNSUPPORTED      The input Language is not supported by the Component Name (2) protocol.
  Other                Some error occurs when locating Component Name (2) protocol instance or finding the supported language.
  
--*/
{
  EFI_STATUS                      Status;

  //
  // Locate Component Name (2) protocol on the driver binging handle.
  //
  Status = gBS->OpenProtocol (
                 DriverBindingHandle,
                 ProtocolGuid,
                 (VOID **) ComponentName,
                 NULL,
                 NULL,
                 EFI_OPEN_PROTOCOL_GET_PROTOCOL
                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Apply shell policy to select the best language.
  //
  *SupportedLanguage = DriverHealthSelectBestLanguage (
                         (*ComponentName)->SupportedLanguages,
                         (BOOLEAN) (ProtocolGuid == &gEfiComponentNameProtocolGuid)
                         );
  if (*SupportedLanguage == NULL) {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

EFI_STATUS
GetDriverNameWorker (
  IN  EFI_GUID    *ProtocolGuid,
  IN  EFI_HANDLE  DriverBindingHandle,
  OUT CHAR16      **DriverName
  )
/*++

Routine Description:

  This is an internal worker function to get driver name from Component Name (2) protocol interface.
  
Arguments:

  ProtocolGuid         A pointer to an EFI_GUID. It points to Component Name (2) protocol GUID.
  DriverBindingHandle  The handle on which the Component Name (2) protocol instance is retrieved.
  DriverName           A pointer to the Unicode string to return. This Unicode string is the name of the driver specified by This.
  
Returns:

  EFI_SUCCESS          The driver name is successfully retrieved from Component Name (2) protocol interface.
  Other                The driver name cannot be retrieved from Component Name (2) protocol interface.
  
--*/
{
  EFI_STATUS                     Status;
  CHAR8                          *BestLanguage;
  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName;

  //
  // Retrieve Component Name (2) protocol instance on the driver binding handle and find the best language this instance supports. 
  //
  Status = GetComponentNameWorker (
             ProtocolGuid,
             DriverBindingHandle,
             &ComponentName,
             &BestLanguage
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
 
  //
  // Get the driver name from Component Name (2) protocol instance on the driver binging handle.
  //
  Status = ComponentName->GetDriverName (
                            ComponentName,
                            BestLanguage,
                            DriverName
                            );
  gBS->FreePool (BestLanguage);
 
  return Status;
}

EFI_STATUS
DriverHealthGetDriverName (
  IN  EFI_HANDLE  DriverBindingHandle,
  OUT CHAR16      **DriverName
  )
/*++

Routine Description:

  This function gets driver name from Component Name 2 protocol interface and Component Name protocol interface in turn.
  It first tries UEFI 2.0 Component Name 2 protocol interface and try to get the driver name.
  If the attempt fails, it then gets the driver name from EFI 1.1 Component Name protocol for backward compatibility support. 
  
Arguments:

  DriverBindingHandle  The handle on which the Component Name (2) protocol instance is retrieved.
  DriverName           A pointer to the Unicode string to return. This Unicode string is the name of the driver specified by This.
  
Returns:

  EFI_SUCCESS          The driver name is successfully retrieved from Component Name (2) protocol interface.
  Other                The driver name cannot be retrieved from Component Name (2) protocol interface.
  
--*/
{
  EFI_STATUS      Status;

  //
  // Get driver name from UEFI 2.0 Component Name 2 protocol interface.
  //
  Status = GetDriverNameWorker (&gEfiComponentName2ProtocolGuid, DriverBindingHandle, DriverName);
  if (EFI_ERROR (Status)) {
    //
    // If it fails to get the driver name from Component Name protocol interface, we should fall back on
    // EFI 1.1 Component Name protocol interface.
    //
    Status = GetDriverNameWorker (&gEfiComponentNameProtocolGuid, DriverBindingHandle, DriverName);
  }

  return Status;
}

EFI_STATUS
GetControllerNameWorker (
  IN  EFI_GUID    *ProtocolGuid,
  IN  EFI_HANDLE  DriverBindingHandle,
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_HANDLE  ChildHandle,
  OUT CHAR16      **ControllerName
  )
/*++

Routine Description:

  This function gets controller name from Component Name 2 protocol interface and Component Name protocol interface
  in turn. It first tries UEFI 2.0 Component Name 2 protocol interface and try to get the controller name.
  If the attempt fails, it then gets the controller name from EFI 1.1 Component Name protocol for backward
  compatibility support. 
  
Arguments:

  ProtocolGuid         A pointer to an EFI_GUID. It points to Component Name (2) protocol GUID.
  DriverBindingHandle  The handle on which the Component Name (2) protocol instance is retrieved.
  ControllerHandle     The handle of a controller that the driver specified by This is managing.
                       This handle specifies the controller whose name is to be returned.
  ChildHandle          The handle of the child controller to retrieve the name of. This is an
                       optional parameter that may be NULL. It will be NULL for device drivers.
                       It will also be NULL for bus drivers that attempt to retrieve the name
                       of the bus controller. It will not be NULL for a bus driver that attempts
                       to retrieve the name of a child controller.
  ControllerName       A pointer to the Unicode string to return. This Unicode string
                      is the name of the controller specified by ControllerHandle and ChildHandle.
  
Returns:

  EFI_SUCCESS         The controller name is successfully retrieved from Component Name (2) protocol
                      interface.
  Other               The controller name cannot be retrieved from Component Name (2) protocol.
  
--*/
{
  EFI_STATUS                     Status;
  CHAR8                          *BestLanguage;
  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName;

  //
  // Retrieve Component Name (2) protocol instance on the driver binding handle and 
  // find the best language this instance supports. 
  //
  Status = GetComponentNameWorker (
             ProtocolGuid,
             DriverBindingHandle,
             &ComponentName,
             &BestLanguage
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get the controller name from Component Name (2) protocol instance on the driver binging handle.
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            ControllerHandle,
                            ChildHandle,
                            BestLanguage,
                            ControllerName
                            );
  gBS->FreePool (BestLanguage);

  return Status;
}

EFI_STATUS
DriverHealthGetControllerName (
  IN  EFI_HANDLE  DriverBindingHandle,
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_HANDLE  ChildHandle,
  OUT CHAR16      **ControllerName
  )
/*++

Routine Description:

  This function gets controller name from Component Name 2 protocol interface and Component Name protocol interface
  in turn. It first tries UEFI 2.0 Component Name 2 protocol interface and try to get the controller name. 
  If the attempt fails, it then gets the controller name from EFI 1.1 Component Name protocol for backward
  compatibility support. 
  
Arguments:

  DriverBindingHandle  The handle on which the Component Name (2) protocol instance is retrieved.
  ControllerHandle     The handle of a controller that the driver specified by This is managing.
                       This handle specifies the controller whose name is to be returned.
  ChildHandle          The handle of the child controller to retrieve the name of. This is an
                       optional parameter that may be NULL. It will be NULL for device drivers.
                       It will also be NULL for bus drivers that attempt to retrieve the name
                       of the bus controller. It will not be NULL for a bus driver that attempts
                       to retrieve the name of a child controller.
  ControllerName       A pointer to the Unicode string to return. This Unicode string
                       is the name of the controller specified by ControllerHandle and ChildHandle.
  
Returns:

  EFI_SUCCESS          The controller name is successfully retrieved from Component Name (2) protocol
                       interface.
  Other                The controller name cannot be retrieved from Component Name (2) protocol.
  
--*/
{
  EFI_STATUS      Status;

  //
  // Get controller name from UEFI 2.0 Component Name 2 protocol interface.
  //
  Status = GetControllerNameWorker (
             &gEfiComponentName2ProtocolGuid,
             DriverBindingHandle,
             ControllerHandle,
             ChildHandle,
             ControllerName
             );
  if (EFI_ERROR (Status)) {
    //
    // If it fails to get the controller name from Component Name protocol interface, we should fall back on
    // EFI 1.1 Component Name protocol interface.
    //
    Status = GetControllerNameWorker (
               &gEfiComponentNameProtocolGuid,
               DriverBindingHandle,
               ControllerHandle,
               ChildHandle,
               ControllerName
               );
  }

  return Status;
}

