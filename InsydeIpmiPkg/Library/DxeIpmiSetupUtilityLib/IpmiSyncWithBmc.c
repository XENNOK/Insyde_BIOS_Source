/** @file
 DXE IPMI Setup Utility library implement code - Sync With BMC.

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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>

#include <H2OIpmi.h>
#include <ChipsetSetupConfig.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>


/**
 Send "Set Lan" command to BMC and Set IPv4 IP Address.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Send Status.
*/
EFI_STATUS
SetBmcIPv4IpAddress (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT16                                StatusCodes;
  UINT8                                 RequestData[6];
  UINT8                                 RecvData;
  UINT8                                 RecvSize;
  UINTN                                 Index;
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_IP_ADDRESS;

  for (Index = 0; Index < 4 ; ++Index) {
    RequestData[2 + Index] = SetupConfig->BmcIPv4IpAddress[Index];
  }

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_SET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   sizeof (RequestData),
                   &RecvData,
                   &RecvSize,
                   &StatusCodes
                   );

  return Status;

}


/**
 Send "Set Lan" command to BMC and Set subnet mask.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Send Status.
*/
EFI_STATUS
SetBmcIPv4SubnetMask (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT16                                StatusCodes;
  UINT8                                 RequestData[6];
  UINT8                                 RecvData;
  UINT8                                 RecvSize;
  UINTN                                 Index;
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_SUBNET_MASK;

  for (Index = 0; Index < 4 ; ++Index) {
    RequestData[2 + Index] = SetupConfig->BmcIPv4SubnetMask[Index];
  }

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_SET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   sizeof (RequestData),
                   &RecvData,
                   &RecvSize,
                   &StatusCodes
                   );

  return Status;

}


/**
 Send "Set Lan" command to BMC and set gateway address.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Send Status.
*/
EFI_STATUS
SetBmcIPv4GatewayAddress (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT16                                StatusCodes;
  UINT8                                 RequestData[6];
  UINT8                                 RecvData;
  UINT8                                 RecvSize;
  UINTN                                 Index;
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_DEFAULT_GATEWAY_ADDRESS;

  for (Index = 0; Index < 4 ; ++Index) {
    RequestData[2 + Index] = SetupConfig->BmcIPv4GatewayAddress[Index];
  }

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_SET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   sizeof (RequestData),
                   &RecvData,
                   &RecvSize,
                   &StatusCodes
                   );

  return Status;

}


/**
 Send "Set Lan" command to BMC and Set IP Source.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Send Status.
*/
EFI_STATUS
SetBmcIPv4Source (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT16                                StatusCodes;
  UINT8                                 RequestData[3];
  UINT8                                 RecvData;
  UINT8                                 RecvSize;
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_ADDRESS_SOURCE;
  RequestData[2] = SetupConfig->BmcIPv4Source;

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_SET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status =  Ipmi->ExecuteIpmiCmd (
                    Ipmi,
                    Request,
                    RequestData,
                    sizeof (RequestData),
                    &RecvData,
                    &RecvSize,
                    &StatusCodes
                    );

  return Status;

}


/**
 Send "Set Front Panel Enables" command to BMC and Set Front panel buttons.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Send Status.
*/
EFI_STATUS
SetFrontPanelButton (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RequestData;
  UINT8                                 RecvData[2];
  UINT8                                 RecvSize;
  H2O_IPMI_GET_CHASSIS_STATUS           Chassis;
  H2O_IPMI_CMD_HEADER                   FPRequest;
  H2O_IPMI_CMD_HEADER                   Request;

  FPRequest.NetFn = H2O_IPMI_NETFN_CHASSIS;
  FPRequest.Cmd = H2O_IPMI_CMD_GET_CHASSIS_STATUS;
  FPRequest.Lun = H2O_IPMI_BMC_LUN;

  //
  // Get Current Front Panel Button Status
  //
  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   FPRequest,
                   NULL,
                   0,
                   &Chassis,
                   &RecvSize,
                   NULL
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check if NMI button was allowed to disable
  //
  RequestData = 0;
  if (Chassis.DiagIntBtnDisableAllow && SetupConfig->BmcFrontPanelNmiBtn == FALSE) {
    RequestData |= DISABLE_BUTTON_DIAGNOSTIC;
  }

  //
  // Check if Reset button was allowed to disable
  //
  if (Chassis.ResetBtnDisableAllow && SetupConfig->BmcFrontPanelRstBtn == FALSE) {
    RequestData |= DISABLE_BUTTON_RESET;
  }

  //
  // Check if Power button was allowed to disable
  //
  if (Chassis.PowerBtnDisableAllow && SetupConfig->BmcFrontPanelPwrBtn == FALSE) {
    RequestData |= DISABLE_BUTTON_POWER;
  }

  Request.NetFn = H2O_IPMI_NETFN_CHASSIS;
  Request.Cmd = H2O_IPMI_CMD_SET_FRONT_PANEL_BTN_ENABLE;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status =  Ipmi->ExecuteIpmiCmd (
                    Ipmi,
                    Request,
                    &RequestData,
                    1,
                    RecvData,
                    &RecvSize,
                    NULL
                    );

  return Status;

}


/**
 Send "Set Power Cycle Interval" command to BMC and Set Power Cycle Interval.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Send Status.
*/
EFI_STATUS
SetBmcPowerCycleTime (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  UINT8                                 RecvSize;
  UINT8                                 RecvBuf;
  EFI_STATUS                            Status;
  UINT16                                StatusCodes;
  H2O_IPMI_CMD_HEADER                   Request;

  Status = EFI_SUCCESS;

  Request.NetFn = H2O_IPMI_NETFN_CHASSIS;
  Request.Cmd = H2O_IPMI_CMD_SET_POWER_CYCLE_INTERVAL;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   &(SetupConfig->BmcPowerCycleTime),
                   1,
                   &RecvBuf,
                   &RecvSize,
                   &StatusCodes
                   );

  return Status;

}


/**
 This function can set some platform settings to BMC.
 These settings are platform-dependent.

 @param[in]         ConfigBuffer        Pointer to CHIPSET_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Set some platform settings to BMC success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiSetToBmc (
  IN  VOID                              *ConfigBuffer
  )
{
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi;
  EFI_STATUS                            Status;
  CHIPSET_CONFIGURATION                 *SetupConfig;

  Ipmi = NULL;
  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  SetupConfig = NULL;
  SetupConfig = (CHIPSET_CONFIGURATION*)ConfigBuffer;

  SetBmcIPv4Source (Ipmi, SetupConfig);

  if (SetupConfig->BmcIPv4Source == LAN_ADDRESS_SOURCE_STATIC) {
    SetBmcIPv4IpAddress (Ipmi, SetupConfig);
    SetBmcIPv4SubnetMask (Ipmi, SetupConfig);
    SetBmcIPv4GatewayAddress (Ipmi, SetupConfig);
  }

  if (SetupConfig->BmcPowerCycleTimeEnable == H2O_IPMI_FUNCTION_ENABLE) {
    SetBmcPowerCycleTime (Ipmi, SetupConfig);
  }

  SetFrontPanelButton (Ipmi, SetupConfig);

  return EFI_SUCCESS;

}


/**
 Update BMC information "RMCP IP Address" and corresponding SetupVariable.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Sync Status.
*/
EFI_STATUS
GetBmcIPv4IpAddress (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[5];
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_IP_ADDRESS;

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_GET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   4,
                   RecvData,
                   &RecvSize,
                   NULL
                   );

  if (!EFI_ERROR (Status)) {
    SetupConfig->BmcIPv4IpAddress[0] = RecvData[1];
    SetupConfig->BmcIPv4IpAddress[1] = RecvData[2];
    SetupConfig->BmcIPv4IpAddress[2] = RecvData[3];
    SetupConfig->BmcIPv4IpAddress[3] = RecvData[4];
  }

  return Status;

}


/**
 Update BMC information "BMC subnet mask" and corresponding SetupVariable.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Sync Status.
*/
EFI_STATUS
GetBmcIPv4SubnetMask (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[5];
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_SUBNET_MASK;

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_GET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   4,
                   RecvData,
                   &RecvSize,
                   NULL
                   );

  if (!EFI_ERROR (Status)) {
    SetupConfig->BmcIPv4SubnetMask[0] = RecvData[1];
    SetupConfig->BmcIPv4SubnetMask[1] = RecvData[2];
    SetupConfig->BmcIPv4SubnetMask[2] = RecvData[3];
    SetupConfig->BmcIPv4SubnetMask[3] = RecvData[4];
  }

  return Status;

}


/**
 Update BMC information "BMC Gateway Address" and corresponding SetupVariable.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Sync Status.
*/
EFI_STATUS
GetBmcIPv4GatewayAddress (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[5];
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_DEFAULT_GATEWAY_ADDRESS;

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_GET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   4,
                   RecvData,
                   &RecvSize,
                   NULL
                   );

  if (!EFI_ERROR (Status)) {
    //
    // Update SetupVariable
    //
    SetupConfig->BmcIPv4GatewayAddress[0] = RecvData[1];
    SetupConfig->BmcIPv4GatewayAddress[1] = RecvData[2];
    SetupConfig->BmcIPv4GatewayAddress[2] = RecvData[3];
    SetupConfig->BmcIPv4GatewayAddress[3] = RecvData[4];
  }

  return Status;

}


/**
 Update BMC information "DHCP Enable" and corresponding SetupVariable.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Sync Status.
*/
EFI_STATUS
GetBmcIPv4Source (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[2];
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_ADDRESS_SOURCE;

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_GET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   4,
                   RecvData,
                   &RecvSize,
                   NULL
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
      SetupConfig->BmcIPv4Source = LAN_ADDRESS_SOURCE_DHCP;
      break;

    default:
      SetupConfig->BmcIPv4Source = LAN_ADDRESS_SOURCE_STATIC;
      break;
    }

  }

  return Status;

}


/**
 Update BMC information "BMC Front Panel Buttons" and corresponding SetupVariable.

 @param[in]         Ipmi                A pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION instance.

 @return Sync Status.
*/
EFI_STATUS
GetFrontPanelButton (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  UINT8                                 RecvData[4];
  H2O_IPMI_CMD_HEADER                   Request;


  Request.NetFn = H2O_IPMI_NETFN_CHASSIS;
  Request.Cmd = H2O_IPMI_CMD_GET_CHASSIS_STATUS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   NULL,
                   0,
                   RecvData,
                   &RecvSize,
                   NULL
                   );

  if (!EFI_ERROR (Status)) {
    SetupConfig->BmcFrontPanelPwrBtn = (RecvData[3] & DISABLE_BUTTON_POWER) ? FALSE : TRUE;
    SetupConfig->BmcFrontPanelRstBtn = (RecvData[3] & DISABLE_BUTTON_RESET) ? FALSE : TRUE;
    SetupConfig->BmcFrontPanelNmiBtn = (RecvData[3] & DISABLE_BUTTON_DIAGNOSTIC) ? FALSE : TRUE;
  }

  return Status;

}


/**
 This function can get some platform settings from BMC.
 These settings are platform-dependent.

 @param[out]        ConfigBuffer        Pointer to CHIPSET_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Get some platform settings from BMC success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiGetFromBmc (
  OUT VOID                              *ConfigBuffer
  )
{
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi;
  EFI_STATUS                            Status;
  CHIPSET_CONFIGURATION                 *SetupConfig;

  Ipmi = NULL;
  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  SetupConfig = NULL;
  SetupConfig = (CHIPSET_CONFIGURATION*)ConfigBuffer;

  GetBmcIPv4Source (Ipmi, SetupConfig);
  GetBmcIPv4IpAddress (Ipmi, SetupConfig);
  GetBmcIPv4SubnetMask (Ipmi, SetupConfig);
  GetBmcIPv4GatewayAddress (Ipmi, SetupConfig);

  GetFrontPanelButton (Ipmi, SetupConfig);

  return EFI_SUCCESS;

}

