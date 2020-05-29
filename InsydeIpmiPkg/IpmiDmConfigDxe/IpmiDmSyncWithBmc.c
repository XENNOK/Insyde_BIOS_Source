/** @file
 IPMI Device Manager Config Sync With BMC.

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


/**
 Sync BMC information "DHCP Enable" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmSyncIpv4Source (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[2];

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  ZeroMem (RequestData, sizeof (RequestData));
  ZeroMem (RecvData, sizeof (RecvData));

  //
  // Channel number
  //
  RequestData[0] = IpmiConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_ADDRESS_SOURCE;


  if (SetToBmc) {
    //
    // Set Config to BMC
    //

    RequestData[2] = IpmiConfig->BmcIpv4Source;

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_SET_LAN_PARAMETERS,
               RequestData,
               3,
               RecvData,
               &RecvSize
               );

  } else {
    //
    // Get Config from BMC
    //

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_GET_LAN_PARAMETERS,
               RequestData,
               4,
               RecvData,
               &RecvSize
               );
    if (!EFI_ERROR (Status)) {
      //
      // According to IPMI specification, we only accept the value 1 and 2.
      // 1: static
      // 2: dhcp
      // if the value is 0(unspecified), we treat it as 1(static).
      // if the value is 3(load by BIOS or SMS), we treat it as 1(static).
      // if the value is 4(load by other protocol by BMC), we treat ias 2(dhcp).
      // all other value we treat it as 1(static).
      //
      // these code prevents wrong value from some bad BMC firmware.
      //
      switch (RecvData[1]) {
      case LAN_ADDRESS_SOURCE_DHCP:
      case LAN_ADDRESS_SOURCE_OTHER:
        IpmiConfig->BmcIpv4Source = LAN_ADDRESS_SOURCE_DHCP;
        break;

      default:
        IpmiConfig->BmcIpv4Source = LAN_ADDRESS_SOURCE_STATIC;
        break;
      }
    }

  }


  return Status;

}


/**
 Sync BMC information "RMCP IP Address" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmSyncIpv4IpAddr (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RequestData[6];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[5];
  UINTN                                 Index;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  ZeroMem (RequestData, sizeof (RequestData));
  ZeroMem (RecvData, sizeof (RecvData));

  //
  // Channel number
  //
  RequestData[0] = IpmiConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_IP_ADDRESS;


  if (SetToBmc) {
    //
    // Set Config to BMC
    //

    for (Index = 0; Index < 4; ++Index) {
      RequestData[2 + Index] = IpmiConfig->BmcIpv4IpAddress[Index];
    }

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_SET_LAN_PARAMETERS,
               RequestData,
               6,
               RecvData,
               &RecvSize
               );

  } else {
    //
    // Get Config from BMC
    //

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_GET_LAN_PARAMETERS,
               RequestData,
               4,
               RecvData,
               &RecvSize
               );
    if (!EFI_ERROR (Status)) {
      IpmiConfig->BmcIpv4IpAddress[0] = RecvData[1];
      IpmiConfig->BmcIpv4IpAddress[1] = RecvData[2];
      IpmiConfig->BmcIpv4IpAddress[2] = RecvData[3];
      IpmiConfig->BmcIpv4IpAddress[3] = RecvData[4];
    }

  }


  return Status;

}


/**
 Sync BMC information "Subnet Mask" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmSyncIpv4SubnetMask (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RequestData[6];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[5];
  UINTN                                 Index;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  ZeroMem (RequestData, sizeof (RequestData));
  ZeroMem (RecvData, sizeof (RecvData));

  //
  // Channel number
  //
  RequestData[0] = IpmiConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_SUBNET_MASK;


  if (SetToBmc) {
    //
    // Set Config to BMC
    //

    for (Index = 0; Index < 4; ++Index) {
      RequestData[2 + Index] = IpmiConfig->BmcIpv4SubnetMask[Index];
    }

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_SET_LAN_PARAMETERS,
               RequestData,
               6,
               RecvData,
               &RecvSize
               );

  } else {
    //
    // Get Config from BMC
    //

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_GET_LAN_PARAMETERS,
               RequestData,
               4,
               RecvData,
               &RecvSize
               );
    if (!EFI_ERROR (Status)) {
      IpmiConfig->BmcIpv4SubnetMask[0] = RecvData[1];
      IpmiConfig->BmcIpv4SubnetMask[1] = RecvData[2];
      IpmiConfig->BmcIpv4SubnetMask[2] = RecvData[3];
      IpmiConfig->BmcIpv4SubnetMask[3] = RecvData[4];
    }

  }


  return Status;

}


/**
 Sync BMC information "Gateway Address" by set config to BMC or get config from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         SetToBmc            Boolean flag to decide set to BMC or get from BMC.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmSyncIpv4GatewayAddr (
  IN  UINT8                             *IpmiDmConfig,
  IN  BOOLEAN                           SetToBmc
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RequestData[6];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[5];
  UINTN                                 Index;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  ZeroMem (RequestData, sizeof (RequestData));
  ZeroMem (RecvData, sizeof (RecvData));

  //
  // Channel number
  //
  RequestData[0] = IpmiConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_DEFAULT_GATEWAY_ADDRESS;


  if (SetToBmc) {
    //
    // Set Config to BMC
    //

    for (Index = 0; Index < 4; ++Index) {
      RequestData[2 + Index] = IpmiConfig->BmcIpv4GatewayAddress[Index];
    }

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_SET_LAN_PARAMETERS,
               RequestData,
               6,
               RecvData,
               &RecvSize
               );

  } else {
    //
    // Get Config from BMC
    //

    Status = IpmiLibExecuteIpmiCmd (
               H2O_IPMI_NETFN_TRANSPORT,
               H2O_IPMI_CMD_GET_LAN_PARAMETERS,
               RequestData,
               4,
               RecvData,
               &RecvSize
               );
    if (!EFI_ERROR (Status)) {
      IpmiConfig->BmcIpv4GatewayAddress[0] = RecvData[1];
      IpmiConfig->BmcIpv4GatewayAddress[1] = RecvData[2];
      IpmiConfig->BmcIpv4GatewayAddress[2] = RecvData[3];
      IpmiConfig->BmcIpv4GatewayAddress[3] = RecvData[4];
    }

  }


  return Status;

}


/**
 Get BMC information "Front Panel Button disable/enable status".

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmGetFrontPanelButton (
  IN  UINT8                             *IpmiDmConfig
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RecvSize;
  UINT8                                 RecvData[4];

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_CHASSIS,
             H2O_IPMI_CMD_GET_CHASSIS_STATUS,
             NULL,
             0,
             RecvData,
             &RecvSize
             );
  if (!EFI_ERROR (Status)) {
    IpmiConfig->BmcFrontPanelPwrBtn = (RecvData[3] & DISABLE_BUTTON_POWER) ? FALSE : TRUE;
    IpmiConfig->BmcFrontPanelRstBtn = (RecvData[3] & DISABLE_BUTTON_RESET) ? FALSE : TRUE;
    IpmiConfig->BmcFrontPanelNmiBtn = (RecvData[3] & DISABLE_BUTTON_DIAGNOSTIC) ? FALSE : TRUE;
  }

  return Status;

}


/**
 Get some general current configuration from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmGetFromBmc (
  IN UINT8                              *IpmiDmConfig
  )
{

  if (OemIpmiDmSyncIpv4Source (IpmiDmConfig, FALSE) == EFI_UNSUPPORTED) {
    IpmiDmSyncIpv4Source (IpmiDmConfig, FALSE);
  }
  if (OemIpmiDmSyncIpv4IpAddr (IpmiDmConfig, FALSE) == EFI_UNSUPPORTED) {
    IpmiDmSyncIpv4IpAddr (IpmiDmConfig, FALSE);
  }
  if (OemIpmiDmSyncIpv4SubnetMask (IpmiDmConfig, FALSE) == EFI_UNSUPPORTED) {
    IpmiDmSyncIpv4SubnetMask (IpmiDmConfig, FALSE);
  }
  if (OemIpmiDmSyncIpv4GatewayAddr (IpmiDmConfig, FALSE) == EFI_UNSUPPORTED) {
    IpmiDmSyncIpv4GatewayAddr (IpmiDmConfig, FALSE);
  }

  if (OemIpmiDmGetFrontPanelButton (IpmiDmConfig) == EFI_UNSUPPORTED) {
    IpmiDmGetFrontPanelButton (IpmiDmConfig);
  }

}


/**
 Send "Set Front Panel Enables" command to Set Front Panel Buttons.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmSetFrontPanelButton (
  IN  UINT8                             *IpmiDmConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RequestData;
  UINT8                                 RecvData[2];
  UINT8                                 RecvSize;
  H2O_IPMI_GET_CHASSIS_STATUS           Chassis;
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  //
  // Get Current Front Panel Button Status
  //
  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_CHASSIS,
             H2O_IPMI_CMD_GET_CHASSIS_STATUS,
             NULL,
             0,
             &Chassis,
             &RecvSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check if NMI button was allowed to disable
  //
  RequestData = 0;
  if (Chassis.DiagIntBtnDisableAllow && IpmiConfig->BmcFrontPanelNmiBtn == FALSE) {
    RequestData |= DISABLE_BUTTON_DIAGNOSTIC;
  }

  //
  // Check if Reset button was allowed to disable
  //
  if (Chassis.ResetBtnDisableAllow && IpmiConfig->BmcFrontPanelRstBtn == FALSE) {
    RequestData |= DISABLE_BUTTON_RESET;
  }

  //
  // Check if Power button was allowed to disable
  //
  if (Chassis.PowerBtnDisableAllow && IpmiConfig->BmcFrontPanelPwrBtn == FALSE) {
    RequestData |= DISABLE_BUTTON_POWER;
  }

  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_CHASSIS,
             H2O_IPMI_CMD_SET_FRONT_PANEL_BTN_ENABLE,
             &RequestData,
             1,
             RecvData,
             &RecvSize
             );

  return Status;

}


/**
 Send "Set Power Cycle Interval" command to set Power Cycle Interval.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @return Execute Lib IPMI Command Status.
*/
EFI_STATUS
IpmiDmSetPowerCycleTime (
  IN  UINT8                             *IpmiDmConfig
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RecvSize;
  UINT8                                 RecvData;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_CHASSIS,
             H2O_IPMI_CMD_SET_POWER_CYCLE_INTERVAL,
             &(IpmiConfig->BmcPowerCycleTime),
             1,
             &RecvData,
             &RecvSize
             );

  return Status;

}


/**
 Set some general configuration to BMC according to the IPMI Config Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmSetToBmc (
  IN UINT8                              *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  if (OemIpmiDmSyncIpv4Source (IpmiDmConfig, TRUE) == EFI_UNSUPPORTED) {
    IpmiDmSyncIpv4Source (IpmiDmConfig, TRUE);
  }

  if (IpmiConfig->BmcIpv4Source == LAN_ADDRESS_SOURCE_STATIC) {
    if (OemIpmiDmSyncIpv4IpAddr (IpmiDmConfig, TRUE) == EFI_UNSUPPORTED) {
      IpmiDmSyncIpv4IpAddr (IpmiDmConfig, TRUE);
    }
    if (OemIpmiDmSyncIpv4SubnetMask (IpmiDmConfig, TRUE) == EFI_UNSUPPORTED) {
      IpmiDmSyncIpv4SubnetMask (IpmiDmConfig, TRUE);
    }
    if (OemIpmiDmSyncIpv4GatewayAddr (IpmiDmConfig, TRUE) == EFI_UNSUPPORTED) {
      IpmiDmSyncIpv4GatewayAddr (IpmiDmConfig, TRUE);
    }
  }

  if (OemIpmiDmSetFrontPanelButton (IpmiDmConfig) == EFI_UNSUPPORTED) {
    IpmiDmSetFrontPanelButton (IpmiDmConfig);
  }

  if (IpmiConfig->BmcPowerCycleTimeEnable == H2O_IPMI_FUNCTION_ENABLE) {
    if (OemIpmiDmSetPowerCycleTime (IpmiDmConfig) == EFI_UNSUPPORTED) {
      IpmiDmSetPowerCycleTime (IpmiDmConfig);
    }
  }

}

