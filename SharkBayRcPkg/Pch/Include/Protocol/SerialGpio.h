/** @file
  This file defines the EFI Serial GPIO Interface Protocol which implements the Intel(R) Serial Data over GPIO Pin functionality Protocol Interface.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SerialGpio.h

@brief
  This file defines the EFI Serial GPIO Interface Protocol which implements the
  Intel(R) Serial Data over GPIO Pin functionality Protocol Interface.

**/
#ifndef _EFI_SERIAL_GPIO_H_
#define _EFI_SERIAL_GPIO_H_

///
/// Define the Serial GPIO protocol GUID
///
#define EFI_SERIAL_GPIO_PROTOCOL_GUID \
  { \
    0xf52c3858, 0x5ef8, 0x4d41, \
    { \
      0x83, 0x4e, 0xc3, 0x9e, 0xef, 0x8a, 0x45, 0xa3 \
    } \
  }
//
// Extern the GUID for protocol users.
//
extern EFI_GUID                           gEfiSerialGpioProtocolGuid;
//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SERIAL_GPIO_PROTOCOL  EFI_SERIAL_GPIO_PROTOCOL;

//
// This is the max number of GPIO pins in this ICH chipset that support Blink feature
// 0~31 GPIO in ICH8M support blink feature
//
#define SERIAL_GPIO_MAX_PIN_NUMBER  32
#define SERIAL_GPIO_MAX_DATA_RATE   63
#define WAIT_TIME                   100000
#define WAIT_PERIOD                 10

///
/// Serial GPIO protocol data structures and definitions
///
typedef enum {
  EnumSerialGpioDataByte,
  EnumSerialGpioDataWord,
  EnumSerialGpioDataUndefined,
  EnumSerialGpioDataDword,
  EnumSerialGpioDataMax
} SERIAL_GPIO_DATA_WIDTH;

//
// Protocol member functions
//
/**
  Register for one GPIO Pin that will be used as serial GPIO.
  For PCH only GPIO0~31 will have the capability to be used as serail GPIO.
  The caller of this procedure need to be very clear of whPch GPIO should be used as serail GPIO,
  it should not be input, native, conflict with other GPIO, or Index > 31 on the caller's platform.

  @param[in] This                    Pointer to the EFI_SERIAL_GPIO_PROTOCOL instance.
  @param[in] SerialGpioPinIndex      The GPIO pin Index that will be used as serial GPIO for data sending.

  @retval EFI_SUCCESS             Opcode initialization on the SERIAL_GPIO host controller completed.
  @retval EFI_ACCESS_DENIED       The SERIAL_GPIO configuration interface is locked.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource available to initialize the device.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
  @retval EFI_INVALID_PARAMETER   SerialGpioPinIndex is out of range
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SERIAL_GPIO_REGISTER) (
  IN EFI_SERIAL_GPIO_PROTOCOL       * This,
  IN UINT8                          SerialGpioPinIndex
  );

/**
  Unregister for one GPIO Pin that has been used as serial GPIO, and recover the registers before
  registering.

  @param[in] This                 Pointer to the EFI_SERIAL_GPIO_PROTOCOL instance.
  @param[in] SerialGpioPinIndex   The GPIO pin Index that will be used as serial GPIO for data sending.

  @retval EFI_SUCCESS             Opcode initialization on the SERIAL_GPIO host controller completed.
  @retval EFI_ACCESS_DENIED       The SERIAL_GPIO configuration interface is locked.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource available to initialize the device.
  @retval EFI_DEVICE_ERROR        Device error, operation failed.
  @retval EFI_INVALID_PARAMETER   Invalid function parameters
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SERIAL_GPIO_UNREGISTER) (
  IN EFI_SERIAL_GPIO_PROTOCOL       * This,
  IN UINT8                          SerialGpioPinIndex
  );

/**
  Execute SERIAL_GPIO commands from the host controller.

  @param[in] This                 Pointer to the EFI_SERIAL_GPIO_PROTOCOL instance.
  @param[in] GpioPinIndex         Index of the GPIO pin.
  @param[in] DataRate             The data rate for serail data transfering. 1 ~ SERIAL_GPIO_MAX_DATA_RATE; 1: 128ns intervals; ...; 8: 8*128 = 1024ns intervals, default value;...
  @param[in] DataCountInByte      Number of bytes of the data will be transmitted through the GPIO pin.
  @param[in, out] Buffer          Pointer to caller-allocated buffer containing the dada sent through the GPIO pin.

  @retval EFI_SUCCESS             Execute succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, GPIO serial data sent failed.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SERIAL_GPIO_SEND_DATA) (
  IN     EFI_SERIAL_GPIO_PROTOCOL   * This,
  IN     UINT8                      GpioPinIndex,
  IN     UINT8                      DataRate,
  IN     UINTN                      DataCountInByte,
  IN OUT UINT8                      *Buffer
  );

///
/// Protocol definition
///
/// This Protocol allows a platform module to execute the IntelR Serial Data over
/// GPIO Pin functionality Protocol Interface.
/// The caller will first call the SerialGpioRegister() function to configure the GPIO
/// to be used. Then the caller will execute one or more calls to the SerialGpioSendData()
/// function to perform serial GPIO activities. Finally, the caller will use the
/// SerialGpioUnRegister() function to un-register and allow other consumers to utilize
/// the serial GPIO services.
/// If the serial GPIO capabilities are in use by another caller, the registration
/// function will return an error.
///
struct _EFI_SERIAL_GPIO_PROTOCOL {
  EFI_SERIAL_GPIO_REGISTER    SerialGpioRegister;   ///< Register for one GPIO pin that will be used as serial GPIO.
  EFI_SERIAL_GPIO_SEND_DATA   SerialGpioSendData;   ///< Execute SERIAL_GPIO commands from the host controller.
  EFI_SERIAL_GPIO_UNREGISTER  SerialGpioUnRegister; ///< Un-register the current GPIO pin used for serial GPIO, and recovers the registers before registering.
};

#endif
