/** @file
 IPMI Device Manager Config Init HII Data.

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


//
// Use for save BMC status after init, IpmiConfigData.BmcStatus will Keep this status when load default.
//
UINT8                                   mBmcStatus;


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
  )
{
  H2O_IPMI_BMC_STATUS                   BmcStatus;

  if (IpmiInterface != NULL) {
    BmcStatus = IpmiInterface->GetBmcStatus (IpmiInterface);
  } else if (IpmiHob != NULL) {
    BmcStatus = IpmiHob->BmcStatus;
  } else {
    BmcStatus = BMC_STATUS_UNKNOWN;
  }

  switch (BmcStatus) {
  case BMC_STATUS_OK:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"OK");
    break;

  case BMC_STATUS_ERROR:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"Error");
    break;

  case BMC_STATUS_NOT_READY:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"Not Ready");
    break;

  case BMC_STATUS_NOT_FOUND:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"Not Found");
    break;

  case BMC_STATUS_UNKNOWN:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"Unknown");
    break;
  }

  return EFI_SUCCESS;

}


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
  )
{
  H2O_IPMI_INTERFACE_TYPE               InterfaceType;

  if (IpmiInterface != NULL) {
    return IpmiInterface->GetIpmiInterfaceName (IpmiInterface, StringBuffer);
  } else if (IpmiHob != NULL) {
    InterfaceType = IpmiHob->InterfaceType;
  } else {
    return EFI_NOT_FOUND;
  }

  switch (InterfaceType) {
  case H2O_IPMI_KCS:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"KCS");
    break;

  case H2O_IPMI_SMIC:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"SMIC");
    break;

  case H2O_IPMI_BT:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"BT");
    break;

  case H2O_IPMI_SSIF:
    UnicodeSPrint (StringBuffer, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16), L"SSIF");
    break;
  }

  return EFI_SUCCESS;

}


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
  )
{
  H2O_IPMI_INTERFACE_TYPE               InterfaceType;
  UINT16                                Address;
  UINT16                                Offset;

  if (IpmiInterface != NULL) {
    InterfaceType = IpmiInterface->GetIpmiInterfaceType(IpmiInterface);
    Address = IpmiInterface->GetIpmiBaseAddress (IpmiInterface, H2O_IPMI_SMM_BASE_ADDRESS_INDEX);
    Offset = IpmiInterface->GetIpmiBaseAddressOffset (IpmiInterface, H2O_IPMI_SMM_BASE_ADDRESS_INDEX);
  } else if (IpmiHob != NULL) {
    InterfaceType = IpmiHob->InterfaceType;
    Address = IpmiHob->InterfaceAddress[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Addr;
    Offset = IpmiHob->InterfaceAddress[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Offset;
  } else {
    return EFI_NOT_FOUND;
  }

  if (InterfaceType == H2O_IPMI_KCS) {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%x/%x",
      (UINTN)Address,
      (UINTN)(Address + Offset)
      );

  } else {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%x",
      (UINTN)Address
      );

  }

  return EFI_SUCCESS;

}


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
  )
{
  H2O_IPMI_INTERFACE_TYPE               InterfaceType;
  UINT16                                Address;
  UINT16                                Offset;

  if (IpmiInterface != NULL) {
    InterfaceType = IpmiInterface->GetIpmiInterfaceType(IpmiInterface);
    Address = IpmiInterface->GetIpmiBaseAddress (IpmiInterface, H2O_IPMI_POST_BASE_ADDRESS_INDEX);
    Offset = IpmiInterface->GetIpmiBaseAddressOffset (IpmiInterface, H2O_IPMI_POST_BASE_ADDRESS_INDEX);
  } else if (IpmiHob != NULL) {
    InterfaceType = IpmiHob->InterfaceType;
    Address = IpmiHob->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr;
    Offset = IpmiHob->InterfaceAddress[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Offset;
  } else {
    return EFI_NOT_FOUND;
  }

  if (InterfaceType == H2O_IPMI_KCS) {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%x/%x",
      (UINTN)Address,
      (UINTN)(Address + Offset)
      );

  } else {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%x",
      (UINTN)Address
      );

  }

  return EFI_SUCCESS;

}


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
  )
{
  H2O_IPMI_INTERFACE_TYPE               InterfaceType;
  UINT16                                Address;
  UINT16                                Offset;

  if (IpmiInterface != NULL) {
    InterfaceType = IpmiInterface->GetIpmiInterfaceType(IpmiInterface);
    Address = IpmiInterface->GetIpmiBaseAddress (IpmiInterface, H2O_IPMI_OS_BASE_ADDRESS_INDEX);
    Offset = IpmiInterface->GetIpmiBaseAddressOffset (IpmiInterface, H2O_IPMI_OS_BASE_ADDRESS_INDEX);
  } else if (IpmiHob != NULL) {
    InterfaceType = IpmiHob->InterfaceType;
    Address = IpmiHob->InterfaceAddress[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr;
    Offset = IpmiHob->InterfaceAddress[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Offset;
  } else {
    return EFI_NOT_FOUND;
  }

  if (InterfaceType == H2O_IPMI_KCS) {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%x/%x",
      (UINTN)Address,
      (UINTN)(Address + Offset)
      );

  } else {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%x",
      (UINTN)Address
      );

  }

  return EFI_SUCCESS;

}


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
  )
{
  UINT16                                FwVersion;

  if (IpmiInterface == NULL || H2O_IPMI_VERSION_DEFAULT == IpmiInterface->GetIpmiVersion (IpmiInterface)) {
    return EFI_NOT_FOUND;
  }

  FwVersion = IpmiInterface->GetBmcFirmwareVersion (IpmiInterface);

  UnicodeSPrint (
    StringBuffer,
    IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
    L"%d.%02x",
    (UINTN)(FwVersion >> 8),
    (UINTN)(FwVersion & 0xFF)
    );

  return EFI_SUCCESS;

}


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
  )
{
  UINT8                                 IpmiVersion;

  if (IpmiInterface != NULL) {
    IpmiVersion = IpmiInterface->GetIpmiVersion (IpmiInterface);
    if (IpmiVersion == H2O_IPMI_VERSION_DEFAULT) {
      return EFI_NOT_FOUND;
    }
  } else {
    return EFI_NOT_FOUND;
  }

  UnicodeSPrint (
    StringBuffer,
    IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
    L"%d.%d",
    (UINTN)(IpmiVersion >> 4),
    (UINTN)(IpmiVersion & 0x0F)
    );

  return EFI_SUCCESS;

}


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
  )
{
  CHAR16                                Temp[0x10];
  UINT8                                 Index;

  if (IpmiInterface == NULL || H2O_IPMI_VERSION_DEFAULT == IpmiInterface->GetIpmiVersion (IpmiInterface)) {
    return EFI_NOT_FOUND;
  }

  StrCpy (StringBuffer, L"This is Channel Number for BMC, Set/Get LAN information according to it. ");
  StrCat (StringBuffer, L"The Valid Channel Number :");

  ZeroMem (Temp, sizeof (Temp));

  for (Index = 0; Index < LAN_CHANNEL_MAX; Index++) {

    if (((mBmcLanChannelBitmap >> Index) & 0x01) == 0x01) {
      UnicodeSPrint (Temp, sizeof (Temp), L" %02d", Index);
      StrCat (StringBuffer, Temp);
    }

  }

  return EFI_SUCCESS;

}


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
  )
{
  EFI_STATUS                            Status;
  IPMI_DM_CONFIG                        *IpmiConfig;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[7];

  if (IpmiInterface == NULL || H2O_IPMI_VERSION_DEFAULT == IpmiInterface->GetIpmiVersion (IpmiInterface)) {
    return EFI_NOT_FOUND;
  }

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  ZeroMem (RequestData, sizeof (RequestData));

  //
  // Channel number
  //
  RequestData[0] = IpmiConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_MAC_ADDRESS;

  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_TRANSPORT,
             H2O_IPMI_CMD_GET_LAN_PARAMETERS,
             RequestData,
             4,
             RecvData,
             &RecvSize
             );
  if (!EFI_ERROR (Status)) {

    UnicodeSPrint (
      StringBuffer,
      IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16),
      L"%02x:%02x:%02x:%02x:%02x:%02x",
      (UINTN)RecvData[1],
      (UINTN)RecvData[2],
      (UINTN)RecvData[3],
      (UINTN)RecvData[4],
      (UINTN)RecvData[5],
      (UINTN)RecvData[6]
      );

  }

  return Status;

}


/**
 Update "IPv4 IP Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv4IpAddrStr (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  UnicodeSPrint (
    (CHAR16*) IpmiConfig->BmcIpv4IpAddressString,
    sizeof (IpmiConfig->BmcIpv4IpAddressString),
    L"%d.%d.%d.%d",
    (UINTN)IpmiConfig->BmcIpv4IpAddress[0],
    (UINTN)IpmiConfig->BmcIpv4IpAddress[1],
    (UINTN)IpmiConfig->BmcIpv4IpAddress[2],
    (UINTN)IpmiConfig->BmcIpv4IpAddress[3]
    );

}


/**
 Update "IPv4 Subnet Mask" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv4SubnetMaskStr (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  UnicodeSPrint (
    (CHAR16*) IpmiConfig->BmcIpv4SubnetMaskString,
    sizeof (IpmiConfig->BmcIpv4SubnetMaskString),
    L"%d.%d.%d.%d",
    (UINTN)IpmiConfig->BmcIpv4SubnetMask[0],
    (UINTN)IpmiConfig->BmcIpv4SubnetMask[1],
    (UINTN)IpmiConfig->BmcIpv4SubnetMask[2],
    (UINTN)IpmiConfig->BmcIpv4SubnetMask[3]
    );

}


/**
 Update "IPv4 Gateway Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv4GatewayAddrStr (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  UnicodeSPrint (
    (CHAR16*) IpmiConfig->BmcIpv4GatewayAddressString,
    sizeof (IpmiConfig->BmcIpv4GatewayAddressString),
    L"%d.%d.%d.%d",
    (UINTN)IpmiConfig->BmcIpv4GatewayAddress[0],
    (UINTN)IpmiConfig->BmcIpv4GatewayAddress[1],
    (UINTN)IpmiConfig->BmcIpv4GatewayAddress[2],
    (UINTN)IpmiConfig->BmcIpv4GatewayAddress[3]
    );

}


/**
 Update "IPv6 IP Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv6IpAddrStr (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  UnicodeSPrint (
    (CHAR16*) IpmiConfig->BmcIpv6IpAddressString,
    sizeof (IpmiConfig->BmcIpv6IpAddressString),
    L"%x:%x:%x:%x:%x:%x:%x:%x",
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[0] << 8) | IpmiConfig->BmcIpv6IpAddress[1]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[2] << 8) | IpmiConfig->BmcIpv6IpAddress[3]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[4] << 8) | IpmiConfig->BmcIpv6IpAddress[5]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[6] << 8) | IpmiConfig->BmcIpv6IpAddress[7]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[8] << 8) | IpmiConfig->BmcIpv6IpAddress[9]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[10] << 8) | IpmiConfig->BmcIpv6IpAddress[11]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[12] << 8) | IpmiConfig->BmcIpv6IpAddress[13]),
    (UINTN)((IpmiConfig->BmcIpv6IpAddress[14] << 8) | IpmiConfig->BmcIpv6IpAddress[15])
    );

}


/**
 Update "IPv6 Gateway Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv6GatewayAddrStr (
  IN  UINT8                             *IpmiDmConfig
  )
{
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  UnicodeSPrint (
    (CHAR16*) IpmiConfig->BmcIpv6GatewayAddressString,
    sizeof (IpmiConfig->BmcIpv6GatewayAddressString),
    L"%x:%x:%x:%x:%x:%x:%x:%x",
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[0] << 8) | IpmiConfig->BmcIpv6GatewayAddress[1]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[2] << 8) | IpmiConfig->BmcIpv6GatewayAddress[3]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[4] << 8) | IpmiConfig->BmcIpv6GatewayAddress[5]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[6] << 8) | IpmiConfig->BmcIpv6GatewayAddress[7]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[8] << 8) | IpmiConfig->BmcIpv6GatewayAddress[9]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[10] << 8) | IpmiConfig->BmcIpv6GatewayAddress[11]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[12] << 8) | IpmiConfig->BmcIpv6GatewayAddress[13]),
    (UINTN)((IpmiConfig->BmcIpv6GatewayAddress[14] << 8) | IpmiConfig->BmcIpv6GatewayAddress[15])
    );

}


/**
 Init & Update IPMI Device Manager Config HII Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
IpmiDmInitHiiData (
  IN  UINT8                             *IpmiDmConfig,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi;
  IPMI_DM_CONFIG                        *IpmiConfig;
  H2O_IPMI_HOB                          *IpmiHob;
  H2O_IPMI_BMC_STATUS                   BmcStatus;
  UINT16                                Index;
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;
  BmcStatus = BMC_STATUS_UNKNOWN;

  //
  // Locate H2OIpmiInterfaceProtocol
  //
  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (!EFI_ERROR (Status)) {
    //
    // Get BMC status
    //
    BmcStatus = Ipmi->GetBmcStatus (Ipmi);
  }


  //
  // Get information from IPMI Hob.
  //
  Status = IpmiLibGetIpmiHob (&IpmiHob);
  if (!EFI_ERROR (Status) && Ipmi == NULL) {
    BmcStatus = IpmiHob->BmcStatus;
  }


  IpmiConfig->BmcStatus = BmcStatus;
  mBmcStatus = BmcStatus;


  //
  // Call UPDATE_STRING_FUNCTION or GET_STRING_FUNCTION to update relative string token.
  //
  Index = 0;
  while (gIpmiDmStringFunction[Index].StringToken != STRING_FUNCTION_TABLE_END) {
    if (gIpmiDmStringFunction[Index].UpdateString == NULL) {

      //
      // When UPDATE_STRING_FUNCTION is NULL, call GET_STRING_FUNCTION to get the string and update string token.
      //
      if (gIpmiDmStringFunction[Index].GetString != NULL) {
        ZeroMem (TempString, sizeof (TempString));
        Status = gIpmiDmStringFunction[Index].GetString (IpmiHob, Ipmi, IpmiDmConfig, TempString);
        if (!EFI_ERROR(Status)) {
          TokenToUpdate = STRING_TOKEN (gIpmiDmStringFunction[Index].StringToken);
          HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);
        }
      }

    } else {

      //
      // When UPDATE_STRING_FUNCTION is not NULL, means OEM implement function to update string token by specific way.
      //
      gIpmiDmStringFunction[Index].UpdateString(HiiHandle, IpmiHob, Ipmi, IpmiDmConfig);

    }
    Index++;
  }


  if (BmcStatus == BMC_STATUS_OK) {

    if (IpmiConfig->IpmiSdrListEnable) {
      if (OemIpmiDmUpdateSdrList (HiiHandle) == EFI_UNSUPPORTED) {
        IpmiDmUpdateSdrList (HiiHandle);
      }
    }

    UpdateIpv4IpAddrStr (IpmiDmConfig);
    UpdateIpv4SubnetMaskStr (IpmiDmConfig);
    UpdateIpv4GatewayAddrStr (IpmiDmConfig);

    UpdateIpv6IpAddrStr (IpmiDmConfig);
    UpdateIpv6GatewayAddrStr (IpmiDmConfig);

  }


  //
  // Reserved for OEM to init HII Data.
  //
  OemIpmiDmInitHiiData (IpmiDmConfig, HiiHandle);

}

