/** @file
 IPMI General Config Data.

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


#ifdef _IMPORT_IPMI_GENERAL_CONFIG_


UINT8       IpmiBootOption;
UINT8       IpmiEnable;
UINT8       IpmiSetBiosVersion;
UINT8       IpmiSdrListEnable;
UINT8       IpmiSpmiEnable;
UINT8       BmcWdtEnable;
UINT8       BmcWdtTimeout;
UINT8       BmcWdtAction;
UINT8       BmcWarmupTime;
UINT8       BmcLanPortConfig;
UINT8       BmcLanChannel;
UINT8       BmcIpv4Source;
UINT8       BmcIpv4IpAddress[4];
UINT8       BmcIpv4SubnetMask[4];
UINT8       BmcIpv4GatewayAddress[4];
UINT8       BmcFrontPanelPwrBtn;
UINT8       BmcFrontPanelRstBtn;
UINT8       BmcFrontPanelNmiBtn;
UINT8       BmcStatus;
UINT8       BmcIpv6Mode;
UINT8       BmcIpv6AutoConfig;
UINT8       BmcIpv6PrefixLength;
UINT8       BmcIpv6IpAddress[16];
UINT8       BmcIpv6GatewayAddress[16];
UINT8       BmcPowerCycleTimeEnable;
UINT8       BmcPowerCycleTime;
UINT16      BmcIpv4IpAddressString[16];
UINT16      BmcIpv4SubnetMaskString[16];
UINT16      BmcIpv4GatewayAddressString[16];
UINT16      BmcIpv6IpAddressString[46];
UINT16      BmcIpv6GatewayAddressString[46];


#endif

