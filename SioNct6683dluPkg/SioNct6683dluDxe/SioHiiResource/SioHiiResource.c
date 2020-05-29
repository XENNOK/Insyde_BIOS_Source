/** @file

Create variable and SIO page

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SioHiiResource.h>

CHAR16           *mSioVariableName = (CHAR16*) PcdGetPtr (PcdSioNct6683dluSetupStr);
EFI_EVENT        mEvent;
extern SIO_DEVICE_LIST_TABLE *mTablePtr;
SIO_PRIVATE_DATA *mSioPrivateData = NULL;

HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    SIO_NCT6683DLU_SETUP00_FORMSET_GUID
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

  @param[in]   This              Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Request           A null-terminated Unicode string in <ConfigRequest> format.
  @param[out]  Progress          On return, points to a character in the Request
                                 string. Points to the string's null terminator if
                                 request was successful. Points to the most recent
                                 '&' before the first failing name/value pair (or
                                 the beginning of the string if the failure is in
                                 the first name/value pair) if the request was not
                                 successful.
  @param[out]  Results           A null-terminated Unicode string in
                                 <ConfigAltResp> format which has all values filled
                                 in for the names in the Request string. String to
                                 be allocated by the called function.

  @retval EFI_SUCCESS            The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.
**/
EFI_STATUS
EFIAPI
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                          Status;
  UINTN                               BufferSize;
  EFI_STRING                          ConfigRequestHdr;
  EFI_STRING                          ConfigRequest;
  BOOLEAN                             AllocatedRequest;
  UINTN                               Size;
  EFI_HII_CONFIG_ROUTING_PROTOCOL     *HiiConfigRouting;
  SIO_PRIVATE_DATA                    *PrivateData;
  
  PrivateData = SIO_PRIVATE_FROM_THIS (This);

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gSioNct6683dluSetup00FormSetGuid, mSioVariableName)) {
    return EFI_NOT_FOUND;
  }
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof (SIO_CONFIGURATION);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (
                         &gSioNct6683dluSetup00FormSetGuid,
                         mSioVariableName,
                         mSioPrivateData->DriverHandle
                         );
    if (ConfigRequestHdr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    gBS->FreePool (ConfigRequestHdr);
  }
  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = HiiConfigRouting->BlockToConfig (
                               HiiConfigRouting,
                               ConfigRequest,
                               (UINT8 *) &PrivateData->Configuration,
                               BufferSize,
                               Results,
                               Progress
                               );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    gBS->FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

/**
  This function processes the results of changes in configuration.

  @param[in]   This              Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Configuration     A null-terminated Unicode string in <ConfigResp>
                                 format.
  @param[out]  Progress          A pointer to a string filled in with the offset of
                                 the most recent '&' before the first failing
                                 name/value pair (or the beginning of the string if
                                 the failure is in the first name/value pair) or
                                 the terminating NULL if all was successful.

  @retval EFI_SUCCESS            The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                          Status;
  SIO_PRIVATE_DATA                    *PrivateData;
  UINTN                               BufferSize;  
    
  PrivateData = SIO_PRIVATE_FROM_THIS (This);

  if (Configuration == NULL || Progress == NULL || This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &gSioNct6683dluSetup00FormSetGuid, mSioVariableName)) {
    return EFI_NOT_FOUND;
  }

  *Progress = Configuration + StrLen (Configuration);

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = sizeof (SIO_CONFIGURATION);
  Status = PrivateData->HiiConfigRouting->ConfigToBlock (
                               PrivateData->HiiConfigRouting,
                               Configuration,
                               (UINT8 *) &PrivateData->Configuration,
                               &BufferSize,
                               Progress
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Store Buffer Storage back to EFI variable
  //
  Status = gRT->SetVariable(
                  mSioVariableName,
                  &gSioNct6683dluSetup00FormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (SIO_CONFIGURATION),
                  &PrivateData->Configuration
                  );
  
  return EFI_SUCCESS;
}

/**
  This function processes the results of changes in configuration.

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Action             Specifies the type of action taken by the browser.
  @param[in]  QuestionId         A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect.
  @param[in]  Type               The type of value for the question.
  @param[in]  Value              A pointer to the data being sent to the original
                                 exporting driver.
  @param[out] ActionRequest      On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.
**/
EFI_STATUS
EFIAPI
DriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  SIO_PRIVATE_DATA                *SioPrivateData;

  SioPrivateData = SIO_PRIVATE_FROM_THIS (This);

  //
  // Retrive uncommitted data from Browser
  //
  if (!HiiGetBrowserData (&gSioNct6683dluSetup00FormSetGuid, mSioVariableName, sizeof (SIO_CONFIGURATION), (UINT8 *) &SioPrivateData->Configuration)) {
    return EFI_NOT_FOUND;
  }
    
  switch (QuestionId) {
     
  default:
    break;
  }
  FreePool (&SioPrivateData->Configuration);

  //
  // All other action return unsupported.
  //
  return EFI_SUCCESS;
}

/**
  Add packlist to Create Setup page.

  @retval EFI_SUCCESS                The function completed successfully.
**/
EFI_STATUS
EFIAPI
CreateSetupPage (
  VOID
  )
{
  EFI_STATUS                          Status;
  BOOLEAN                             ActionFlag;  
  UINTN                               BufferSize;
  EFI_STRING                          ConfigRequestHdr; 
  SIO_DEVICE_LIST_TABLE               *PcdPointer;  
  SIO_DEVICE_LIST_TABLE               *GetByConfiguration; 
  
  ConfigRequestHdr = NULL;

  //
  // Initialize driver private data
  //
  mSioPrivateData = AllocateZeroPool (sizeof (SIO_PRIVATE_DATA));
  if (mSioPrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mSioPrivateData->Signature = SIO_PRIVATE_SIGNATURE;
  mSioPrivateData->ConfigAccess.ExtractConfig = ExtractConfig;
  mSioPrivateData->ConfigAccess.RouteConfig = RouteConfig;
  mSioPrivateData->ConfigAccess.Callback = DriverCallback;

  //
  // Locate ConfigRouting protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid, 
                  NULL, 
                  (VOID **) &mSioPrivateData->HiiConfigRouting
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  // Install Platform Driver Override Protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSioPrivateData->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mSioPrivateData->ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mSioPrivateData->HiiHandle = HiiAddPackages (
                   &gSioNct6683dluSetup00FormSetGuid,
                   mSioPrivateData->DriverHandle,
                   SioHiiResourceVfrBin,
                   SioNct6683dluDxeStrings,
                   NULL
                   );
  
  if (mSioPrivateData->HiiHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize configuration data
  //
  ZeroMem (&mSioPrivateData->Configuration, sizeof (SIO_CONFIGURATION));

  //
  // Try to read NV config EFI variable first
  //
  ConfigRequestHdr = HiiConstructConfigHdr (
                       &gSioNct6683dluSetup00FormSetGuid, 
                       mSioVariableName, 
                       mSioPrivateData->DriverHandle
                       );
  ASSERT (ConfigRequestHdr != NULL);

  BufferSize = sizeof (SIO_CONFIGURATION);
  Status = gRT->GetVariable (
                  mSioVariableName, 
                  &gSioNct6683dluSetup00FormSetGuid, 
                  NULL, 
                  &BufferSize, 
                  &mSioPrivateData->Configuration
                  );
  
  if (EFI_ERROR (Status)) {
    //
    // Store zero data Buffer Storage to EFI variable
    //
    Status = gRT->SetVariable(
                    mSioVariableName,
                    &gSioNct6683dluSetup00FormSetGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (SIO_CONFIGURATION),
                    &mSioPrivateData->Configuration
                    );
    ASSERT_EFI_ERROR (Status);
    
    //
    // EFI variable for NV config doesn't exit, we should build this variable
    // based on default values stored in IFR
    //
    ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);    
    ASSERT (ActionFlag);

    //
    // To define "Device" and "DeviceInstance" for display items
    //    
    PcdPointer = mTablePtr;
    GetByConfiguration = (SIO_DEVICE_LIST_TABLE*) &mSioPrivateData->Configuration;
    while (!((PcdPointer->TypeH == NONE_ID) && (PcdPointer->TypeL == NONE_ID))) {    
      if ((PcdPointer->Device == COM) || (PcdPointer->Device == LPT) || (PcdPointer->Device == WDT)) {
        if (PcdPointer->DeviceEnable == TRUE) {         
          GetByConfiguration->Device = PcdPointer->Device;
          GetByConfiguration->DeviceInstance = PcdPointer->DeviceInstance;
        }          
        GetByConfiguration++;
      }
      PcdPointer++;
    }    
    //
    // Store items that we want to display
    //
    Status = gRT->SetVariable(
                    mSioVariableName,
                    &gSioNct6683dluSetup00FormSetGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (SIO_CONFIGURATION),
                    &mSioPrivateData->Configuration
                    );
    ASSERT_EFI_ERROR (Status); 
  } else {
    //
    // EFI variable does exist and Validate Current Setting
    //
    ActionFlag = HiiValidateSettings (ConfigRequestHdr);
    
    ASSERT (ActionFlag);
  }

  FreePool (ConfigRequestHdr);

  return Status;
}

