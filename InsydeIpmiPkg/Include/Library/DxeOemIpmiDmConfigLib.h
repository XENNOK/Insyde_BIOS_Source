/** @file
 DXE OEM IPMI Device Manager Config library header file.

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


#ifndef _DXE_OEM_IPMI_DM_CONFIG_LIB_H_
#define _DXE_OEM_IPMI_DM_CONFIG_LIB_H_


#include <PiDxe.h>

#include <IpmiDmConfigForm.h>

#include <H2OIpmi.h>


/**
 Reserved for OEM to return the size of IPMI Device Manager Config Data.

 @param[out]        ConfigDataSize      A point to UINTN contents IPMI Device Manager Config Data size.

 @retval EFI_SUCCESS                    Return the size of IPMI Device Manager Config Data success.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDmGetConfigDataSize (
  OUT UINTN                             *ConfigDataSize
  );


/**
 Reserved for OEM to init config data or get current configuration from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmInitConfigData (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to init IPMI Deviece Manager Config HII Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmInitHiiData (
  IN  UINT8                             *IpmiDmConfig,
  IN  EFI_HII_HANDLE                    HiiHandle
  );


/**
 Reserved for OEM to do something necessary before extract Config Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigAccessExtract (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to do something necessary in Config Access Route.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigAccessRoute (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to implement other discrete callback to fit their requirements.
 Handling the new VFR object that have INTERACTIVE flags in Config Access Callback.

 @param[in]         QuestionId          A unique value which is sent to the original exporting driver.
 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigAccessCallback (
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to do something necessary when load default in Config Access Callback.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigLoadDefault (
  IN  UINT8                             *IpmiDmConfig,
  IN  EFI_HII_HANDLE                    HiiHandle
  );


/**
 Reserved for OEM to Implement Set Lan Channel Callback Function in Config Access Callback.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigLanChannelCallback (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to Implement IPV4 Callback Function in Config Access Callback.

 @param[in]         QuestionId          A unique value which is sent to the original exporting driver.
 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigIpv4Callback (
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to Implement IPV6 Callback Function in Config Access Callback.

 @param[in]         QuestionId          A unique value which is sent to the original exporting driver.
 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigIpv6Callback (
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to Sync BMC information "DHCP Enable" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSyncIpv4Source (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  );


/**
 Reserved for OEM to Sync BMC information "RMCP IP Address" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSyncIpv4IpAddr (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  );


/**
 Reserved for OEM to Sync BMC information "Subnet Mask" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSyncIpv4SubnetMask (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  );


/**
 Reserved for OEM to Sync BMC information "Gateway Address" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSyncIpv4GatewayAddr (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  );


/**
 Reserved for OEM to Get BMC information "Front Panel Button disable/enable status".

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmGetFrontPanelButton (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to Send "Set Front Panel Enables" command to Set Front Panel Buttons.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSetFrontPanelButton (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to Send "Set Power Cycle Interval" command to set Power Cycle Interval.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmSetPowerCycleTime (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Reserved for OEM to Update IPMI_BMC_SDR_LIST_LABEL in IPMI Device Manager Config.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmUpdateSdrList (
  IN  EFI_HII_HANDLE                    HiiHandle
  );


#endif

