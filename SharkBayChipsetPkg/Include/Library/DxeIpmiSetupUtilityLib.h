/** @file
 DXE IPMI Setup Utility library header file.

 This file contains functions prototype that can be implemented by OEM to fit
 their requirements.

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


#ifndef _DXE_IPMI_SETUP_UTILITY_LIB_H_
#define _DXE_IPMI_SETUP_UTILITY_LIB_H_


#include <PiDxe.h>


typedef struct {
  UINT16           IpmiConfigFormTitle;
  UINT16           CurrentBmcStatus;
  UINT16           CurrentBmcMacAddress;
  UINT16           CurrentBmcFirmwareVersion;
  UINT16           CurrentIpmiSpecVersion;
  UINT16           CurrentIpmiInterfaceType;
  UINT16           CurrentIpmiSmmBaseAddress;
  UINT16           CurrentIpmiPostBaseAddress;
  UINT16           CurrentIpmiOsBaseAddress;
  UINT16           BmcLanChannelHelp;
  UINT16           CurrentBmcIpv4IpAddress;
  UINT16           CurrentBmcIpv4SubnetMask;
  UINT16           CurrentBmcIpv4GatewayAddress;
  UINT16           CurrentBmcIpv6IpAddress;
  UINT16           CurrentBmcIpv6GatewayAddress;
} IPMI_STR_ID;


/**
 This function can get some platform settings from BMC.
 These settings are platform-dependent.

 @param[out]        ConfigBuffer        Pointer to SYSTEM_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Get some platform settings from BMC success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiGetFromBmc (
  OUT VOID                              *ConfigBuffer
  );


/**
 This function can set some platform settings to BMC.
 These settings are platform-dependent.

 @param[in]         ConfigBuffer        Pointer to SYSTEM_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Set some platform settings to BMC success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiSetToBmc (
  IN  VOID                              *ConfigBuffer
  );


/**
 This function is called in SetupUtilityInit for pre identify available BMC lan channels
 and get some setting from BMC. These settings are platform-dependent.

 @param[in]         ConfigBuffer        Pointer to SYSTEM_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Init IPMI Config data success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol or Ipmi Version error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigInit (
  IN  VOID                              *ConfigBuffer
  );


/**
 IPMI configuration menu callback function.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         QuestionId          The Callback question ID.
 @param[in]         ConfigBuffer        A pointer to SYSTEM_CONFIGURATION struct.
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
  );


/**
 Update all information in IPMI configuration menu.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         ConfigBuffer        A pointer to SYSTEM_CONFIGURATION struct.
 @param[in]         StrIdBuffer         A pointer to IPMI_STR_ID struct.

 @retval EFI_SUCCESS                    Platform implement this function return EFI_SUCCESS.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigUpdate (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  VOID                              *ConfigBuffer,
  IN  IPMI_STR_ID                       *StrIdBuffer
  );


/**
 This function is called when loading default/custom in Setup Utility.
 It will restore default/custom value that related to BMC settings.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         ConfigBuffer        A pointer to SYSTEM_CONFIGURATION struct.
 @param[in]         LoadDefault         TRUE to load default; FALSE to load custom.

 @retval EFI_SUCCESS                    Platform implement this function return EFI_SUCCESS.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigRestore (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  VOID                              *ConfigBuffer,
  IN  BOOLEAN                           LoadDefault
  );


/**
 This function can enter Ipmi Utility.

 @retval EFI_UNSUPPORTED                Platform does not implement this function.
 @return Status                         Locate gH2OIpmiUtilityProtocolGuid Protocol Status.
*/
EFI_STATUS
ExecuteIpmiUtility (
  VOID
  );


#endif

