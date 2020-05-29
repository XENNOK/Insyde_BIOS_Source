/** @file
 IPMI Device Manager Config Initialize Config Data.

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
// Use for saving Original BMC Lan Channel before adjust to available Lan Channel.
//
UINT8                                   mBmcLanChannel;

//
// Use for recovering previous valid setting if user enter invalid Lan Channel.
//
UINT8                                   mBmcPrevLanChannel;

//
// Use for marking which Channel Medium Type is 802.3 LAN.
//
UINT16                                  mBmcLanChannelBitmap;


/**
 Pre identify available BMC Lan Channels,
 if channel number in IPMI Device Manager Config is invalid, Adjust it.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmAdjustLanChannel (
  IN  UINT8                             *IpmiDmConfig
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi;
  UINT8                                 Index;
  UINT8                                 RequestData;
  UINT8                                 RecvSize;
  H2O_IPMI_GET_CHANNEL_INFO             Info;
  H2O_IPMI_CMD_HEADER                   Request;
  IPMI_DM_CONFIG                        *IpmiConfig;

  IpmiConfig = (IPMI_DM_CONFIG*)IpmiDmConfig;

  mBmcLanChannel = IpmiConfig->BmcLanChannel;

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (!EFI_ERROR (Status)) {

    Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
    Request.Cmd = H2O_IPMI_CMD_GET_CHANNEL_INFO_COMMAND;
    Request.Lun = H2O_IPMI_BMC_LUN;

    mBmcLanChannelBitmap = 0;
    for (Index = 0; Index < LAN_CHANNEL_MAX; Index++) {
      RequestData = Index;
      ZeroMem (&Info, sizeof (Info));
      Status = Ipmi->ExecuteIpmiCmd (
                       Ipmi,
                       Request,
                       &RequestData,
                       1,
                       &Info,
                       &RecvSize,
                       NULL
                       );
      if (!EFI_ERROR (Status)) {
        if (Info.ChannelMidiumType == CHANNEL_MEDIUM_8023LAN) {
          mBmcLanChannelBitmap = mBmcLanChannelBitmap | (0x01 << Index);
          if ((((mBmcLanChannelBitmap >> (IpmiConfig->BmcLanChannel)) & 0x01) != 0x01) &&
              (Index > IpmiConfig->BmcLanChannel)) {
            IpmiConfig->BmcLanChannel = Index;
          }
        }
      }
    }

  }

  mBmcPrevLanChannel = IpmiConfig->BmcLanChannel;

}


/**
 Init config data and get the current configuration from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmInitConfigData (
  IN UINT8                              *IpmiDmConfig
  )
{

  //
  // Pre identify available BMC lan channels and adjust lan channel in config data.
  //
  IpmiDmAdjustLanChannel (IpmiDmConfig);


  //
  // Reserved an opportunity for OEM to init config data or get current configuration from BMC.
  //
  OemIpmiDmInitConfigData (IpmiDmConfig);


  IpmiDmGetFromBmc (IpmiDmConfig);

}

