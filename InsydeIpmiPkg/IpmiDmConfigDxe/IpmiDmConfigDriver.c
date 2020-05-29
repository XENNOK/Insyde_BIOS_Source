/** @file
 The driver entry point for H2O IPMI BMC Config in Device Manager menu driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include "IpmiDmConfigAccess.h"


IPMI_DM_CONFIG_PRIVATE_DATA  *mIpmiDmConfigPrivateData = NULL;

CHAR16                       *mIpmiDmConfigVariableName;
EFI_GUID                     mIpmiDmConfigVariableGuid;

HII_VENDOR_DEVICE_PATH       mIpmiHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    IPMI_DM_CONFIG_FORMSET_GUID
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
 Removes a package list from the HII database to remove IPMI Device Manager Config Form,
 uninstall Hii Config Access Protocol and free Driver Private Data.

*/
VOID
UninstallIpmiDmConfigForm (
  VOID
  )
{

  //
  // Uninstall HII package list
  //
  if (mIpmiDmConfigPrivateData->HiiHandle != NULL) {
    HiiRemovePackages (mIpmiDmConfigPrivateData->HiiHandle);
    mIpmiDmConfigPrivateData->HiiHandle = NULL;
  }

  //
  // Uninstall HII Config Access Protocol
  //
  if (mIpmiDmConfigPrivateData->DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           mIpmiDmConfigPrivateData->DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mIpmiHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &(mIpmiDmConfigPrivateData->ConfigAccess),
           NULL
           );
    mIpmiDmConfigPrivateData->DriverHandle = NULL;
  }

  //
  // Free allocate data
  //
  if (mIpmiDmConfigPrivateData->IpmiDmConfigData != NULL) {
    FreePool (mIpmiDmConfigPrivateData->IpmiDmConfigData);
  }

  FreePool (mIpmiDmConfigPrivateData);

}


/**
 The entry point for IPMI Configuration in Device Manager driver.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_ALREADY_STARTED            The driver already exists in system.
 @retval EFI_SUCCES                     All the related protocols are installed on the driver.
 @return Others                         Fail to install protocols as indicated.
*/
EFI_STATUS
EFIAPI
IpmiDmConfigDriverEntryPoint (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  UINTN                                 VariableSize;
  UINT8                                 *VfrBin;
  EFI_STRING                            ConfigRequestHdr;
  BOOLEAN                               ActionFlag;


  Status = gBS->OpenProtocol (
                  ImageHandle,
                  &gEfiCallerIdGuid,
                  NULL,
                  ImageHandle,
                  ImageHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }


  //
  // Allocate & zero a private data structure.
  //
  mIpmiDmConfigPrivateData = AllocateZeroPool (sizeof (IPMI_DM_CONFIG_PRIVATE_DATA));
  if (mIpmiDmConfigPrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }


  //
  // Initialize driver private data.
  //
  mIpmiDmConfigPrivateData->Signature = IPMI_CONFIG_PRIVATE_DATA_SIGNATURE;

  mIpmiDmConfigPrivateData->ConfigAccess.ExtractConfig = ConfigAccessExtract;
  mIpmiDmConfigPrivateData->ConfigAccess.RouteConfig = ConfigAccessRoute;
  mIpmiDmConfigPrivateData->ConfigAccess.Callback = ConfigAccessCallback;


  //
  // Install Device Path Protocol and Config Access Protocol to driver handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(mIpmiDmConfigPrivateData->DriverHandle),
                  &gEfiDevicePathProtocolGuid,
                  &mIpmiHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &(mIpmiDmConfigPrivateData->ConfigAccess),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }


  //
  // Publish the IPMI Device Manager Config Form.
  // Registers IPMI Device Manager Config Packages in the HII Database
  // and returns the HII Handle associated with that registration.
  //
  mIpmiDmConfigPrivateData->HiiHandle = HiiAddPackages (
                                          &mIpmiDmConfigFormSetGuid,
                                          mIpmiDmConfigPrivateData->DriverHandle,
                                          DxeIpmiDmConfigVfrLibStrings,
                                          IpmiDmConfigVfrBin,
                                          NULL
                                          );
  if (mIpmiDmConfigPrivateData->HiiHandle == NULL) {
    goto ErrorExit;
  }


  //
  // Decide IPMI Device Manager Config data size, get from OEM first.
  //
  Status = OemIpmiDmGetConfigDataSize (&mIpmiDmConfigPrivateData->IpmiDmConfigDataSize);
  if (Status == EFI_UNSUPPORTED) {
    mIpmiDmConfigPrivateData->IpmiDmConfigDataSize = sizeof (IPMI_DM_CONFIG);
  }


  //
  // Allocate zero pool for IPMI Device Manager Config Data.
  //
  mIpmiDmConfigPrivateData->IpmiDmConfigData = AllocateZeroPool (mIpmiDmConfigPrivateData->IpmiDmConfigDataSize);


  mIpmiDmConfigVariableName = IPMI_CONFIG_DATA_NAME;
  CopyMem (&mIpmiDmConfigVariableGuid, &mIpmiDmConfigFormSetGuid, sizeof (EFI_GUID));


  //
  // Get IPMI Device Manager Config Data from Variable,
  // if not found set to VFR default.
  //
  VariableSize = mIpmiDmConfigPrivateData->IpmiDmConfigDataSize;
  Status = gRT->GetVariable (
                  mIpmiDmConfigVariableName,
                  &mIpmiDmConfigVariableGuid,
                  NULL,
                  &VariableSize,
                  mIpmiDmConfigPrivateData->IpmiDmConfigData
                  );
  if (EFI_ERROR (Status)) {
    //
    // If IPMI Device Manager Config Data Variable doesn't exist, set to VFR default.
    //
    VfrBin = IpmiDmConfigVfrBin;
    IpmiDmExtractVfrDefault (
      mIpmiDmConfigPrivateData->IpmiDmConfigData,
      VfrBin,
      mIpmiDmConfigPrivateData->IpmiDmConfigDataSize
      );


    //
    // Reserved for OEM to do something necessary when set to default in driver entry.
    // (Like Load Default in Config Access Callback)
    //
    OemIpmiDmConfigLoadDefault (mIpmiDmConfigPrivateData->IpmiDmConfigData, mIpmiDmConfigPrivateData->HiiHandle);


    //
    // According to the PCD to select set to default behavior.
    // When PcdIpmiInitConfigDefaultPolicy is 1, according to the VFR default value to set to BMC.
    //
    if (FixedPcdGet8 (PcdIpmiInitConfigDefaultPolicy) == 1) {
      IpmiDmSetToBmc (mIpmiDmConfigPrivateData->IpmiDmConfigData);
    }

  }


  //
  // Init IPMI Device Manager Config Data.
  //
  IpmiDmInitConfigData (mIpmiDmConfigPrivateData->IpmiDmConfigData);


  //
  // Init IPMI Device Manager Config HII Data.
  //
  IpmiDmInitHiiData (mIpmiDmConfigPrivateData->IpmiDmConfigData, mIpmiDmConfigPrivateData->HiiHandle);


  Status = gRT->SetVariable(
                  IPMI_CONFIG_DATA_NAME,
                  &mIpmiDmConfigVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  mIpmiDmConfigPrivateData->IpmiDmConfigDataSize,
                  mIpmiDmConfigPrivateData->IpmiDmConfigData
                  );
  ASSERT (Status == EFI_SUCCESS);


  ConfigRequestHdr = HiiConstructConfigHdr (
                       &mIpmiDmConfigFormSetGuid,
                       IPMI_CONFIG_DATA_NAME,
                       mIpmiDmConfigPrivateData->DriverHandle
                       );
  ASSERT (ConfigRequestHdr != NULL);

  //
  // Validate Current Setting.
  //
  ActionFlag = HiiValidateSettings (ConfigRequestHdr);
  ASSERT (ActionFlag);

  FreePool (ConfigRequestHdr);


  //
  // Install private GUID.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiCallerIdGuid,
                  mIpmiDmConfigPrivateData,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  return EFI_SUCCESS;

ErrorExit:

  if (mIpmiDmConfigPrivateData != NULL) {
    UninstallIpmiDmConfigForm ();
  }

  return Status;

}


/**
 Unload this driver & remove the IPMI Device Manager Config form.

 @param[in]         ImageHandle         The driver's image handle.

 @retval EFI_SUCCESS                    The IPMI Device Manager Config driver is unloaded.
 @return Others                         Failed to unload the driver.
*/
EFI_STATUS
EFIAPI
IpmiDmConfigDriverUnload (
  IN  EFI_HANDLE                        ImageHandle
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG_PRIVATE_DATA           *PrivateData;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiCallerIdGuid,
                  (VOID**) &PrivateData
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (PrivateData->Signature == IPMI_CONFIG_PRIVATE_DATA_SIGNATURE);

  gBS->UninstallMultipleProtocolInterfaces (
         ImageHandle,
         &gEfiCallerIdGuid,
         PrivateData,
         NULL
         );

  UninstallIpmiDmConfigForm ();

  return EFI_SUCCESS;

}

