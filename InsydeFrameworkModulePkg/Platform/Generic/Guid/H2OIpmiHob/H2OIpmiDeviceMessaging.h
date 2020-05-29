//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  H2OIpmiDeviceMessaging.h

Abstract:

  Header file that contains IPMI BMC Device and Messageing Commands, data structures and
  definitions

--*/

#ifndef _H2O_IPMI_DEVICE_MESSAGING_H_
#define _H2O_IPMI_DEVICE_MESSAGING_H_

#pragma pack(1)

//
// BMC Device and Messaging Commands
//
#define H2O_IPMI_CMD_SET_BMC_GLOBAL_ENABLES             0x2E
#define H2O_IPMI_CMD_GET_BMC_GLOBAL_ENABLES             0x2F
#define H2O_IPMI_CMD_CLEAR_MESSAGE_FLAGS                0x30
#define H2O_IPMI_CMD_GET_MESSAGE_FLAGS                  0x31
#define H2O_IPMI_CMD_ENABLE_MSG_CHANNEL_RECEIVE         0x32
#define H2O_IPMI_CMD_GET_MESSAGE                        0x33
#define H2O_IPMI_CMD_SEND_MESSAGE                       0x34
#define H2O_IPMI_CMD_READ_EVENT_MESSAGE_BUFFER          0x35
#define H2O_IPMI_CMD_GET_BT_CAPABILITIES                0x36
#define H2O_IPMI_CMD_GET_SYSTEM_GUID                    0x37
#define H2O_IPMI_CMD_SET_SYSTEM_INFO_PARM               0x58
#define H2O_IPMI_CMD_GET_SYSTEM_INFO_PARM               0x59
#define H2O_IPMI_CMD_GET_CHANNEL_AUTH_CAP               0x38
#define H2O_IPMI_CMD_GET_SESSION_CHALLENGE              0x39
#define H2O_IPMI_CMD_ACTIVATE_SESSION                   0x3A
#define H2O_IPMI_CMD_SET_SESSION_PRIVILEGE_LEVEL        0x3B
#define H2O_IPMI_CMD_CLOSE_SESSION                      0x3C
#define H2O_IPMI_CMD_GET_SESSION_INFO                   0x3D
#define H2O_IPMI_CMD_GET_AUTHCODE                       0x3F
#define H2O_IPMI_CMD_SET_CHANNEL_ACCESS                 0x40
#define H2O_IPMI_CMD_GET_CHANNEL_ACCESS                 0x41
#define H2O_IPMI_CMD_GET_CHANNEL_INFO_COMMAND           0x42
#define H2O_IPMI_CMD_SET_USER_ACCESS_COMMAND            0x43
#define H2O_IPMI_CMD_GET_USER_ACCESS_COMMAND            0x44
#define H2O_IPMI_CMD_SET_USERNAME                       0x45
#define H2O_IPMI_CMD_GET_USERNAME_COMMAND               0x46
#define H2O_IPMI_CMD_SET_USER_PASSWORD_COMMAND          0x47
#define H2O_IPMI_CMD_ACTIVATE_PAYLOAD                   0x48
#define H2O_IPMI_CMD_DEACIVATE_PAYLOAD                  0x49
#define H2O_IPMI_CMD_GET_PAYLOAD_ACTIVATION_STATUS      0x4A
#define H2O_IPMI_CMD_GET_PAYLOAD_INSTANCE_INFO          0x4B
#define H2O_IPMI_CMD_SET_USER_PAYLOAD_ACCESS            0x4C
#define H2O_IPMI_CMD_GET_USER_PAYLOAD_ACCESS            0x4D
#define H2O_IPMI_CMD_GET_CHANNEL_PAYLOAD_SUPPORT        0x4E
#define H2O_IPMI_CMD_GET_CHANNEL_PAYLOAD_VERSION        0x4F
#define H2O_IPMI_CMD_GET_CHANNEL_OEM_PAYLOAD_INFO       0x50
#define H2O_IPMI_CMD_MASTER_WRITE_READ                  0x52
#define H2O_IPMI_CMD_GET_CHANNEL_CIPHER_SUITES          0x54
#define H2O_IPMI_CMD_SUSPEND_RESUME_PAYLOAD_ENCRYPTION  0x55
#define H2O_IPMI_CMD_SET_CHANNEL_SECURITY_KEYS          0x56
#define H2O_IPMI_CMD_GET_SYSTEM_INTERFACE_CAPABILITIES  0x57

//
// Message Parameter
//
#define MSG_SET_IN_PROGRESS                 0x0
#define MSG_SYSTEM_FIRMWARE_VERSION         0x1
#define MSG_SYSTEM_NAME                     0x2
#define MSG_PRIMARY_OS_NAME                 0x3
#define MSG_OS_NAME                         0x4

//
// Channel Medium Type
//
typedef enum {
  CHANNEL_MEDIUM_RESERVED = 0,
  CHANNEL_MEDIUM_IPMB,
  CHANNEL_MEDIUM_ICMB10,
  CHANNEL_MEDIUM_ICMB09,
  CHANNEL_MEDIUM_8023LAN,
  CHANNEL_MEDIUM_SERIAL,
  CHANNEL_MEDIUM_OTHERLAN,
  CHANNEL_MEDIUM_PCISMBUS,
  CHANNEL_MEDIUM_SMBUS1011,
  CHANNEL_MEDIUM_SMBUS20,
  CHANNEL_MEDIUM_USB1X,
  CHANNEL_MEDIUM_USB2X,
  CHANNEL_MEDIUM_SYSTEM_INTERFACE,
  CHANNEL_MEDIUM_MAX_TYPE
} H2O_IPMI_CHANNEL_MEDIUM_TYPE;

//
// Channel Protocol Type
//
typedef enum {
  CHANNEL_PROTOCOL_NA = 0,
  CHANNEL_PROTOCOL_IPMB10,
  CHANNEL_PROTOCOL_ICMB10,
  CHANNEL_PROTOCOL_RESERVED,
  CHANNEL_PROTOCOL_IPMISMBUS,
  CHANNEL_PROTOCOL_KCS,
  CHANNEL_PROTOCOL_SMIC,
  CHANNEL_PROTOCOL_BT10,
  CHANNEL_PROTOCOL_BT15,
  CHANNEL_PROTOCOL_TMODE,
  CHANNEL_PROTOCOL_MAX_TYPE
} H2O_IPMI_CHANNEL_PROTOCOL_TYPE;

//
// Get Channel Info Command Data Structure
//
typedef struct {
  UINT8   ChannelNumber :4;
  UINT8   Reserved      :4;
  UINT8   ChannelMidiumType :7;
  UINT8   Reserved1         :1;
  UINT8   ChannelProtocolType :5;
  UINT8   Reserved2           :3;
  UINT8   SessionCount   :6;
  UINT8   SessionSupport :2;
  UINT8   VendorId[3];
  UINT8   AuxChannelInfo[2];
} H2O_IPMI_GET_CHANNEL_INFO;

#pragma pack()

#endif
