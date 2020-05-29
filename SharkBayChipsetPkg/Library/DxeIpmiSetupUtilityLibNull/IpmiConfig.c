/** @file
 DXE IPMI Setup Utility library implement code - IPMI Config.

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


#include <Library/DxeIpmiSetupUtilityLib.h>


/**
 IPMI configuration menu callback function.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         QuestionId          The Callback question ID.
 @param[in]         ConfigBuffer        A pointer to CHIPSET_CONFIGURATION struct.
 @param[in]         BrowserData         A pointer to SETUP_UTILITY_BROWSER_DATA struct.

 @retval EFI_UNSUPPORTED                Platform does not implement this function.
 @return Status                         Callback execute Status.
*/
EFI_STATUS
IpmiConfigCallback (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  VOID                              *ConfigBuffer,
  IN  VOID                              *BrowserData
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Update all information in IPMI configuration menu.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         ConfigBuffer        A pointer to CHIPSET_CONFIGURATION struct.
 @param[in]         StrIdBuffer         A pointer to IPMI_STR_ID struct.

 @retval EFI_SUCCESS                    Platform implement this function return EFI_SUCCESS.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigUpdate (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  VOID                              *ConfigBuffer,
  IN  IPMI_STR_ID                       *StrIdBuffer
  )
{
  return EFI_UNSUPPORTED;
}


/**
 This function is called when loading default/custom in Setup Utility.
 It will restore default/custom value that related to BMC settings.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         ConfigBuffer        A pointer to CHIPSET_CONFIGURATION struct.
 @param[in]         LoadDefault         TRUE to load default; FALSE to load custom.

 @retval EFI_SUCCESS                    Platform implement this function return EFI_SUCCESS.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigRestore (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  VOID                              *ConfigBuffer,
  IN  BOOLEAN                           LoadDefault
  )
{
  return EFI_UNSUPPORTED;
}


/**
 This function is called in SetupUtilityInit for pre identify available BMC lan channels
 and get some setting from BMC. These settings are platform-dependent.

 @param[in]         ConfigBuffer        Pointer to CHIPSET_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Init IPMI Config data success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol or Ipmi Version error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigInit (
  IN  VOID                              *ConfigBuffer
  )
{
  return EFI_UNSUPPORTED;
}

