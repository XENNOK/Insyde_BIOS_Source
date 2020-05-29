/** @file
  Header file for the PCH SERIAL GPIO Driver.

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
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

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
  PchSerialGpio.h

@brief
  Header file for the PCH SERIAL GPIO Driver.

**/
#ifndef _PCH_SERIAL_GPIO_H_
#define _PCH_SERIAL_GPIO_H_

///
/// Driver Produced Protocols
///
#include <Protocol/SerialGpio.h>
#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
///
/// Private data structure definitions for the driver
///
#define PCH_SERIAL_GPIO_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('S', 'G', 'P', 'O')

#define SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL(a) \
  CR ( \
  a, \
  SERIAL_GPIO_INSTANCE, \
  SerialGpioProtocol, \
  PCH_SERIAL_GPIO_PRIVATE_DATA_SIGNATURE \
  )

///
/// Only when CurrentActiveSerialGpio == SERIAL_GPIO_PIN_CLEARED,
/// can the next GPIO be register as serail GPIO using SerialGpioProtocol
///
#define SERIAL_GPIO_PIN_CLEARED 0xFF
///
/// This is the old values in GPIO related registers for recovery when unregister this serial GPIO pin.
///
typedef struct {
  UINT32  SavedGpioUseSelValue;
  UINT32  SavedGpioBlinkValue;
  UINT32  SavedGpioSerBlinkValue;
  UINT32  SavedGpioIoSelValue;
} SERIAL_GPIO_REGISTERS_TO_RECOVER;

typedef struct {
  UINTN                             Signature;
  EFI_HANDLE                        Handle;
  UINT8                             CurrentActiveSerialGpio;  ///< can only have one pin at one time
  SERIAL_GPIO_REGISTERS_TO_RECOVER  RegistersToRecover;
  EFI_SERIAL_GPIO_PROTOCOL          SerialGpioProtocol;
} SERIAL_GPIO_INSTANCE;

//
// Function prototypes used by the SERIAL_GPIO protocol.
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
EFI_STATUS
EFIAPI
PchSerialGpioRegister (
  IN EFI_SERIAL_GPIO_PROTOCOL       *This,
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
EFI_STATUS
EFIAPI
PchSerialGpioUnRegister (
  IN EFI_SERIAL_GPIO_PROTOCOL       *This,
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
EFI_STATUS
EFIAPI
PchSerialGpioSendData (
  IN     EFI_SERIAL_GPIO_PROTOCOL   *This,
  IN     UINT8                      GpioPinIndex,
  IN     UINT8                      DataRate,
  IN     UINTN                      DataCountInByte,
  IN OUT UINT8                      *Buffer
  );

/**
  This function sends the dword/word/byte through the serial GPIO pin.

  @param[in] This                 Pointer to the EFI_SERIAL_GPIO_PROTOCOL instance.
  @param[in] DataWidth            The data width. 0: byte; 1: word; 2: reserved; 3: dword.
  @param[in] Data                 Data buffer that contains the data (<= UINT32)

  @retval EFI_SUCCESS             SERIAL_GPIO command completes successfully.
  @retval EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
**/
EFI_STATUS
SendSerialGpioSend (
  IN     EFI_SERIAL_GPIO_PROTOCOL   *This,
  IN     SERIAL_GPIO_DATA_WIDTH     DataWidth,
  IN     UINT8                      *Data
  );

/**
  Wait PCH serial GPIO Busy bit being cleared by PCH chipset.

  @param[in] None.

  @retval EFI_SUCCESS             SERIAL GPIO BUSY bit is cleared.
  @retval EFI_DEVICE_ERROR        Time out while waiting the SERIAL GPIO BUSY bit to be cleared.
                                  It's not safe to send next data block on the SERIAL GPIO interface.
**/
EFI_STATUS
WaitForSerialGpioNotBusy (
  VOID
  );

/**
  Entry point for the SERIAL_GPIO host controller driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_SUCCESS             Initialization complete.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
InstallPchSerialGpio (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  );

#endif
