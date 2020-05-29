/** @file
 IPMI Device Manager Config Access Callback.

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


#define IPV4_ADDRESS_SIZE               4
#define IPV6_ADDRESS_SIZE               16


//
// IPv4 maximun length format: 192.168.100.100
//
#define BMC_IPV4_MAX_STRING_LENGTH      15

//
// IPv6 maximun length format: 0000:0000:0000:0000:0000:0000:192.168.100.100
//
#define BMC_IPV6_MAX_STRING_LENGTH      45


//
// Use for saving current BMC valid Lan setting, recovery when enter invalid setting or load default but whitout save.
//
UINT8                                   mBmcTempIpv4IpAddress[IPV4_ADDRESS_SIZE];
UINT8                                   mBmcTempIpv4SubnetMask[IPV4_ADDRESS_SIZE];
UINT8                                   mBmcTempIpv4GatewayAddress[IPV4_ADDRESS_SIZE];
UINT8                                   mBmcTempIpv6IpAddress[IPV6_ADDRESS_SIZE];
UINT8                                   mBmcTempIpv6GatewayAddress[IPV6_ADDRESS_SIZE];


/**
 This function can enter Ipmi Utility.

 @return Locate gH2OIpmiUtilityProtocolGuid Protocol Status.
*/
EFI_STATUS
ExecuteIpmiUtility (
  VOID
  )
{
  EFI_STATUS                                Status;
  H2O_IPMI_UTILITY_PROTOCOL                 *IpmiUtility;

  Status = gBS->LocateProtocol(
                  &gH2OIpmiUtilityProtocolGuid,
                  NULL,
                  (VOID **)&IpmiUtility
                  );
  if (!EFI_ERROR (Status)) {
    IpmiUtility->ExecuteIpmiUtility (IpmiUtility);
  }

  return Status;

}


/**
 This function is called by ConfigAccessCallback,
 Handling the object that have INTERACTIVE flags in VFR.

 @param[in]         QuestionId              A unique value which is sent to the original exporting driver.
 @param[in]         HiiHandle               EFI_HII_HANDLE.
 @param[in]         IpmiDmConfig            A pointer to IPMI Device Manager Config Data struct.

 @return EFI_SUCCESS always.
*/
EFI_STATUS
IpmiDmConfigAccessCallback (
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT8                             *IpmiDmConfig
  )
{
  EFI_INPUT_KEY                         Key;
  UINT16                                *TempBuffer;
  UINT8                                 *PrevBuffer;
  UINT8                                 Ipv4[IPV4_ADDRESS_SIZE];
  UINT8                                 Ipv6[IPV6_ADDRESS_SIZE];
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  switch (QuestionId) {

  case KEY_DM_IPMI_LOAD_OPTIMAL_DEFAULT:
    IpmiConfig->BmcLanChannel = mBmcLanChannel;
    IpmiConfig->BmcStatus = mBmcStatus;

    //
    // Reserved for OEM to do something necessary when load default in Config Access Callback.
    // (Like Set To Default in Driver Entry)
    //
    OemIpmiDmConfigLoadDefault (IpmiDmConfig, HiiHandle);

    UpdateIpv4IpAddrStr (IpmiDmConfig);
    UpdateIpv4SubnetMaskStr (IpmiDmConfig);
    UpdateIpv4GatewayAddrStr (IpmiDmConfig);
    UpdateIpv6IpAddrStr (IpmiDmConfig);
    UpdateIpv6GatewayAddrStr (IpmiDmConfig);

    //
    // Avoid adjust to remain setting when input invalid value after load default without saving.
    //
    CopyMem (mBmcTempIpv4IpAddress, IpmiConfig->BmcIpv4IpAddress, IPV4_ADDRESS_SIZE);
    CopyMem (mBmcTempIpv4SubnetMask, IpmiConfig->BmcIpv4SubnetMask, IPV4_ADDRESS_SIZE);
    CopyMem (mBmcTempIpv4GatewayAddress, IpmiConfig->BmcIpv4GatewayAddress, IPV4_ADDRESS_SIZE);
    CopyMem (mBmcTempIpv6IpAddress, IpmiConfig->BmcIpv6IpAddress, IPV6_ADDRESS_SIZE);
    CopyMem (mBmcTempIpv6GatewayAddress, IpmiConfig->BmcIpv6GatewayAddress, IPV6_ADDRESS_SIZE);
    break;

  case KEY_DM_EXECUTE_IPMI_UTILITY:
    ExecuteIpmiUtility ();
    break;

  case KEY_DM_IPMI_SDR_LIST_SUPPORT:
    if (IpmiConfig->IpmiSdrListEnable) {
      if (OemIpmiDmUpdateSdrList (HiiHandle) == EFI_UNSUPPORTED) {
        IpmiDmUpdateSdrList (HiiHandle);
      }
    }
    break;

  case KEY_DM_SET_BMC_LAN_CHANNEL_NUM:
    if (OemIpmiDmConfigLanChannelCallback (HiiHandle, IpmiDmConfig) == EFI_UNSUPPORTED) {
      if  (((mBmcLanChannelBitmap >> (IpmiConfig->BmcLanChannel)) & 0x01) != 0x01) {
        IpmiConfig->BmcLanChannel = mBmcPrevLanChannel;
        do {
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            L"Invalid Input BMC LAN Channel Number! Please Look at HELP!",
            L"[OK]",
            NULL
            );
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));
      } else {
        mBmcPrevLanChannel = IpmiConfig->BmcLanChannel;
      }
    }
    break;

  case KEY_DM_SET_BMC_IPV4_IP_ADDRESS:
  case KEY_DM_SET_BMC_IPV4_SUBNET_MASK:
  case KEY_DM_SET_BMC_IPV4_GATEWAY_ADDRESS:
    if (OemIpmiDmConfigIpv4Callback (QuestionId, HiiHandle, IpmiDmConfig) == EFI_UNSUPPORTED) {
      if (QuestionId == KEY_DM_SET_BMC_IPV4_IP_ADDRESS) {
        TempBuffer = IpmiConfig->BmcIpv4IpAddressString;
        PrevBuffer = mBmcTempIpv4IpAddress;
      } else if (QuestionId == KEY_DM_SET_BMC_IPV4_SUBNET_MASK) {
        TempBuffer = IpmiConfig->BmcIpv4SubnetMaskString;
        PrevBuffer = mBmcTempIpv4SubnetMask;
      } else {
        TempBuffer = IpmiConfig->BmcIpv4GatewayAddressString;
        PrevBuffer = mBmcTempIpv4GatewayAddress;
      }
      ZeroMem (Ipv4, IPV4_ADDRESS_SIZE);
      if (IsValidIpv4 ((CHAR16*) TempBuffer, Ipv4)) {
        CopyMem (PrevBuffer, Ipv4, IPV4_ADDRESS_SIZE);
      } else {
        do {
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            L"Invalid Input! Please check it again!",
            L"[OK]",
            NULL
            );
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));
        if (QuestionId == KEY_DM_SET_BMC_IPV4_IP_ADDRESS) {
          CopyMem (IpmiConfig->BmcIpv4IpAddress, PrevBuffer, IPV4_ADDRESS_SIZE);
          UpdateIpv4IpAddrStr (IpmiDmConfig);
        } else if (QuestionId == KEY_DM_SET_BMC_IPV4_SUBNET_MASK) {
          CopyMem (IpmiConfig->BmcIpv4SubnetMask, PrevBuffer, IPV4_ADDRESS_SIZE);
          UpdateIpv4SubnetMaskStr (IpmiDmConfig);
        } else {
          CopyMem (IpmiConfig->BmcIpv4GatewayAddress, PrevBuffer, IPV4_ADDRESS_SIZE);
          UpdateIpv4GatewayAddrStr (IpmiDmConfig);
        }
      }
    }
    break;

  case KEY_DM_SET_BMC_IPV6_IP_ADDRESS:
  case KEY_DM_SET_BMC_IPV6_GATEWAY_ADDRESS:
    if (OemIpmiDmConfigIpv6Callback (QuestionId, HiiHandle, IpmiDmConfig) == EFI_UNSUPPORTED) {
      if (QuestionId == KEY_DM_SET_BMC_IPV6_IP_ADDRESS) {
        TempBuffer = IpmiConfig->BmcIpv6IpAddressString;
        PrevBuffer = mBmcTempIpv6IpAddress;
      } else {
        TempBuffer = IpmiConfig->BmcIpv6GatewayAddressString;
        PrevBuffer = mBmcTempIpv6GatewayAddress;
      }
      ZeroMem (Ipv6, IPV6_ADDRESS_SIZE);
      if (IsValidIpv6 ((CHAR16*) TempBuffer, Ipv6)) {
        CopyMem (PrevBuffer, Ipv6, IPV6_ADDRESS_SIZE);
      } else {
        do {
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            L"Invalid Input! Please check it again!",
            L"[OK]",
            NULL
            );
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));
        if (QuestionId == KEY_DM_SET_BMC_IPV6_IP_ADDRESS) {
          CopyMem (IpmiConfig->BmcIpv6IpAddress, PrevBuffer, IPV6_ADDRESS_SIZE);
          UpdateIpv6IpAddrStr (IpmiDmConfig);
        } else {
          CopyMem (IpmiConfig->BmcIpv6GatewayAddress, PrevBuffer, IPV6_ADDRESS_SIZE);
          UpdateIpv6GatewayAddrStr (IpmiDmConfig);
        }
      }
    }
    break;


  //
  // Reserved for OEM to implement other discrete callback to fit their requirements.
  //
  default:
    OemIpmiDmConfigAccessCallback (QuestionId, HiiHandle, IpmiDmConfig);
    break;

  }

  return EFI_SUCCESS;

}

