//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _I2C_HID_POLICY_H_
#define _I2C_HID_POLICY_H_

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_I2C_HID_DEVICES_POLICY_PROTOCOL  EFI_I2C_HID_DEVICES_POLICY_PROTOCOL;

#define MAX_SUPPORTED_DEVICES    8              // The definition will be declared in a header file at platform layer,
                                                // then the definition here will be removed.

//
// The protocol interface is located by HID over I2C device drivers
// to get the I2C devices information.
//
struct _EFI_I2C_HID_DEVICES_POLICY_PROTOCOL {
  UINT8   Revision;
  UINT64  HidDevicesCfg[MAX_SUPPORTED_DEVICES]; // Bit  0 ~  6 : Slave address
                                                // Bit  7      : Interrupt GPIO pin active level, 0 = low active, 1 = high active
                                                // Bit  8 ~ 15 : Interrupt GPIO pin number
                                                // Bit 16 ~ 31 : HID descriptor register number
                                                // Bit 32 ~ 47 : Device type, 
                                                //               0x0000 -> (AutoDetect)
                                                //               0x0d00 -> Touch panel
                                                //               0x0102 -> Mouse
                                                //               0x0106 -> Keyboard
                                                // Bit 48 ~ 51 : Host controller number 
                                                //               0x00      -> (AutoDetect)
                                                //               0x01~0x0f -> I2C host controller 0~14 (One based)
                                                //
                                                // Bit 52 ~ 55 : Device Speed Override  (Local Device speed which will override the I2cBusSpeed)
                                                //               0x01      -> V_SPEED_STANDARD
                                                //               0x02      -> V_SPEED_FAST  
                                                //               0x03      -> V_SPEED_HIGH
};

extern EFI_GUID gEfiI2cHidDevicesPolicyProtocolGuid;

#endif
