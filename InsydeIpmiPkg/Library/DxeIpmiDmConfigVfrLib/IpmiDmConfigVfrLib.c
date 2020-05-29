/** @file
 IPMI Device Manager Config VFR library implement code - String Functions.

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


#include <Library/DxeIpmiDmConfigVfrLib.h>

#include <Library/HiiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/IpmiInterfaceLib.h>

#include <IpmiDmConfigForm.h>


/**
 Get the string will be update to "STR_CURRENT_BMC_STATUS" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetBmcStatusStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_IPMI_INTERFACE_TYPE" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetInterfaceTypeStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_IPMI_SMM_BASE_ADDRESS" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetSmmBaseAddrStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_IPMI_POST_BASE_ADDRESS" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetPostBaseAddrStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_IPMI_OS_BASE_ADDRESS" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetOsBaseAddrStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_BMC_FIRMWARE_VERSION" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetFirmwareVersionStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_IPMI_SPEC_VERSION" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetSpecVersionStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_BMC_LAN_CHANNEL_HELP" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetLanChannelHelpStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


/**
 Get the string will be update to "STR_CURRENT_BMC_MAC_ADDRESS" token in IPMI Device Manager Config.

 @param[in]         IpmiHob             A pointer to H2O_IPMI_HOB structure.
 @param[in]         IpmiInterface       A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[out]        StringBuffer        String buffer to store string.

 @retval EFI_SUCCESS                    Get string successful.
 @return EFI_ERROR                      Get string fail.
*/
EFI_STATUS
GetMacAddrStr (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *IpmiInterface,
  IN  UINT8                             *IpmiDmConfig,
  OUT CHAR16                            *StringBuffer
  );


//
// IPMI String Function Table
//
IPMI_DM_STRING_FUNCTION_TABLE    gIpmiDmStringFunction[] = {
  //
  // There are two string related function, UPDATE_STRING_FUNCTION & GET_STRING_FUNCTION.
  // UPDATE_STRING_FUNCTION will complete update the string to HII data,
  // GET_STRING_FUNCTION just return the unicode string to caller.
  //
  // Package IPMI DM Config code will call GET_STRING_FUNCTION and update string when UPDATE_STRING_FUNCTION is NULL.
  //
  // STRING_TOKEN,                          UPDATE_STRING_FUNCTION,  GET_STRING_FUNCTION
  //
  {STR_CURRENT_BMC_STATUS,                  NULL,                    GetBmcStatusStr},
  {STR_CURRENT_IPMI_INTERFACE_TYPE,         NULL,                    GetInterfaceTypeStr},
  {STR_CURRENT_IPMI_SMM_BASE_ADDRESS,       NULL,                    GetSmmBaseAddrStr},
  {STR_CURRENT_IPMI_POST_BASE_ADDRESS,      NULL,                    GetPostBaseAddrStr},
  {STR_CURRENT_IPMI_OS_BASE_ADDRESS,        NULL,                    GetOsBaseAddrStr},
  {STR_CURRENT_BMC_MAC_ADDRESS,             NULL,                    GetMacAddrStr},
  {STR_CURRENT_BMC_FIRMWARE_VERSION,        NULL,                    GetFirmwareVersionStr},
  {STR_CURRENT_IPMI_SPEC_VERSION,           NULL,                    GetSpecVersionStr},
  {STR_BMC_LAN_CHANNEL_HELP,                NULL,                    GetLanChannelHelpStr},
  {STRING_FUNCTION_TABLE_END,               NULL,                    NULL},
};

