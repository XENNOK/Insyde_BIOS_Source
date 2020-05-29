/** @file

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _I2C_HID_DEVICE_INFO_GUID_H_
#define _I2C_HID_DEVICE_INFO_GUID_H_

//
// H2O I2C HID device
//
typedef struct {
  //
  // Structure length
  //
  UINTN                                 Length;
  //
  // Pointer to the corresponding EFI_I2C_DEVICE
  //
  EFI_I2C_DEVICE                        *I2cDevice;
  //
  // The value of I2C HID descriptor register
  //
  UINT32                                DescReg;
  //
  // The class type pf I2C HID device
  //
  UINT16                                ClassType;
  //
  // The GPIO pin for IOC interrupt
  //
  UINT8                                 GpioPin;
  //
  // The GPIO level setting
  //
  UINT8                                 GpioLevel;
  //
  // The GPIO Host Controller
  //
  UINT8                                 GpioHc;  
} H2O_I2C_HID_DEVICE;

extern EFI_GUID gI2cHidDeviceInfoGuid;

#endif

