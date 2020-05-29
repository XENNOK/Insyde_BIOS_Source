/** @file
 Formset Guids, form ID and Config Data structure for IPMI Device Manager Config.

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


#ifndef _IPMI_DM_CONFIG_FORM_
#define _IPMI_DM_CONFIG_FORM_


#include <Guid/HiiPlatformSetupFormset.h>


#define IPMI_DM_CONFIG_FORMSET_GUID \
  { 0xD2FF7385, 0x3450, 0x4a99, {0xB9, 0x38, 0x80, 0xAC, 0xEC, 0x93, 0xAF, 0xC5} }

#define IPMI_CONFIG_DATA_NAME                    L"IpmiConfigData"

//
// IPMI BMC Config rlative definitions.
//
#define IPMI_BMC_CONFIG_VARSTORE_ID              0x2C01
#define IPMI_BMC_CONFIG_FORM_ID                  0x2C01
#define BMC_CONFIG_SUBFORM_FORM_ID               0x2C02
#define IPMI_BMC_SDR_LIST_FORM_ID                0x2C03

#define IPMI_BMC_SDR_LIST_LABEL                  0x2C04
#define IPMI_BMC_SDR_LIST_END_LABEL              0x2C05

#define KEY_DM_IPMI_SDR_LIST_SUPPORT             0x2C81
#define KEY_DM_EXECUTE_IPMI_UTILITY              0x2C82
#define KEY_DM_SET_BMC_LAN_CHANNEL_NUM           0x2C83
#define KEY_DM_SET_BMC_IPV4_IP_ADDRESS           0x2C84
#define KEY_DM_SET_BMC_IPV4_SUBNET_MASK          0x2C85
#define KEY_DM_SET_BMC_IPV4_GATEWAY_ADDRESS      0x2C86
#define KEY_DM_SET_BMC_IPV6_IP_ADDRESS           0x2C87
#define KEY_DM_SET_BMC_IPV6_GATEWAY_ADDRESS      0x2C88
#define KEY_DM_IPMI_LOAD_OPTIMAL_DEFAULT         0x2C89


#pragma pack(1)

//
// IPMI OEM Configuration Structure.
//
typedef struct {
  //
  // Import IPMI General configuration definition from IpmiGeneralConfigData.h
  //
  #define _IMPORT_IPMI_GENERAL_CONFIG_
  #include <IpmiGeneralConfigData.h>
  #undef _IMPORT_IPMI_GENERAL_CONFIG_
} IPMI_DM_CONFIG;

#pragma pack()


#endif

