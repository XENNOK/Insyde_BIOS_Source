/** @file
 H2O IPMI Serial header file.

 This files contains Serial related definitions and structures defined in IPMI
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
#ifndef _H2O_IPMI_SERIAL_H_
#define _H2O_IPMI_SERIAL_H_


#pragma pack(1)

//
// Serial/Modem Command
//
#define H2O_IPMI_CMD_SET_SERIAL_CONFIG                0x10
#define H2O_IPMI_CMD_GET_SERIAL_CONFIG                0x11
#define H2O_IPMI_CMD_SET_SERIAL_MUX                   0x12
#define H2O_IPMI_CMD_SET_SOL_CONFIG                   0x21
#define H2O_IPMI_CMD_GET_SOL_CONFIG                   0x22

#define H2O_IPMI_PARAM_SYSTEM_SERIAL_ASSOCIATION      0x33


//
//  Structure to "SERIAL_PORT_ASSOCIATION"
//
typedef struct {
  UINT8  Selector;
  UINT8  ControllerNum  :4;
  UINT8  ChannelNum     :4;
  UINT8  ControllerChannelNum    :4;
  UINT8  Reserved                :2;
  UINT8  SerialControllerChannel :1;
  UINT8  SerialPortSharing       :1;
} H2O_IPMI_SERIAL_PORT_ASSOCIATION;

typedef struct {
  UINT8  Channel  :4;
  UINT8  Reserved :4;
  UINT8  Parameter;
} H2O_IPMI_SET_SERIAL_LAN_HEADER;

typedef struct {
  H2O_IPMI_SET_SERIAL_LAN_HEADER    Header;
  H2O_IPMI_SERIAL_PORT_ASSOCIATION  Data;
} H2O_IPMI_SET_SERIAL_PORT_ASSOCIATION;


#pragma pack()


#endif

