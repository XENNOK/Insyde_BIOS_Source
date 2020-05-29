/** @file
 H2O IPMI LAN header file.

 This files contains LAN related definitions and structures defined in IPMI
 specification.

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

#ifndef _H2O_IPMI_LAN_H_
#define _H2O_IPMI_LAN_H_


#pragma pack(1)

//
// LAN Device Commands
//
#define H2O_IPMI_CMD_SET_LAN_PARAMETERS         0x01
#define H2O_IPMI_CMD_GET_LAN_PARAMETERS         0x02
#define H2O_IPMI_CMD_SUSPEND_BMC_ARPS           0x03
#define H2O_IPMI_CMD_GET_IP_UDP_RMCP_STATISTICS 0x04

//
// LAN Configuration Parameter
//
#define LAN_SET_IN_PROGRESS                               0
#define LAN_AUTHENTICATION_TYPE_SUPPORT                   1
#define LAN_AUTHENTICATION_TYPE_ENABLES                   2
#define LAN_IP_ADDRESS                                    3
#define LAN_ADDRESS_SOURCE                                4
#define LAN_MAC_ADDRESS                                   5
#define LAN_SUBNET_MASK                                   6
#define LAN_IPV4_HEADER_PARAMETERS                        7
#define LAN_PRIMARY_RMCP_PORT_NUMBER                      8
#define LAN_SECONDARY_RMCP_PORT_NUMBER                    9
#define LAN_BMC_ARP_CONTROL                               10
#define LAN_GRATUITOUS_ARP_INTERVAL                       11
#define LAN_DEFAULT_GATEWAY_ADDRESS                       12
#define LAN_DEFAULT_GATEWAY_MAC_ADDRESS                   13
#define LAN_BACKUP_GATEWAY_ADDRESS                        14
#define LAN_BACKUP_GATEWAY_MAC_ADDRESS                    15
#define LAN_COMMUNITY_STRING                              16
#define LAN_NUMBERS_OF_DESITINATION                       17
#define LAN_DESITINATION_TYPE                             18
#define LAN_DESITINATION_ADDRESS                          19
#define LAN_8021Q_VLAN_ID                                 20
#define LAN_8021Q_VLAN_PRIORITY                           21
#define LAN_RMCP_MESSAGING_CIPHER_SUITE_ENTRY_SUPPORT     22
#define LAN_RMCP_MESSAGING_CIPHER_SUITE_ENTRIES           23
#define LAN_RMCP_MESSAGING_CIPHER_SUITE_PRIVILEGE_LEVELS  24
#define LAN_DESITINATION_ADDRESS_VLAN_TAGS                25
#define LAN_BAD_PASSWORD_THRESHOLD                        26

//
// IP Source Definition
//
#define LAN_ADDRESS_SOURCE_UNKNOWN       0
#define LAN_ADDRESS_SOURCE_STATIC        1
#define LAN_ADDRESS_SOURCE_DHCP          2
#define LAN_ADDRESS_SOURCE_SMS           3
#define LAN_ADDRESS_SOURCE_OTHER         4

#define LAN_CHANNEL_MAX                  0x10

#pragma pack()


#endif

