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
  PchSerialGpio.c

@brief
  PCH SERIAL GPIO Driver implements the SERIAL GPIO Interface.
  Usage model for this protocol is:
  1. locate this protocol by guid  variable gEfiSerialGpioProtocolGuid
  2. Use SerialGpioRegister to register for one serial GPIO pin.
  3. Send data using SerialGpioSendData.
  4. If another GPIO need to send serial data, 
     the former one need to be unregistered using SerialGpioUnRegister since PCH have only one set of registers for serial GPIO data sending.
     And register the new GPIO pin for Serial Gpio data sending.
 
  

**/
#include "PchSerialGpio.h"

///
/// Global variables
///
UINT32  mPchGpioBase;

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
  )
{
  UINT32                GpioSerBlinkValue;
  UINT32                GpioUseSelValue;
  UINT32                GpioIoSelValue;
  UINT32                GpioBlinkValue;
  SERIAL_GPIO_INSTANCE  *SerialGpioInstance;
  PCH_SERIES            PchSeries;

  if (SerialGpioPinIndex >= SERIAL_GPIO_MAX_PIN_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }

  SerialGpioInstance = SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL (This);
  if (SerialGpioInstance->CurrentActiveSerialGpio != SERIAL_GPIO_PIN_CLEARED) {
    DEBUG (
      (EFI_D_ERROR,
      "You have to unregister the former serial GPIO %d registered, then try to register this GPIO pin %d\n",
      SerialGpioInstance->CurrentActiveSerialGpio,
      SerialGpioPinIndex)
      );
  }

  GpioUseSelValue = 0;
  GpioIoSelValue = 0;
  PchSeries  = GetPchSeries();

  ///
  /// Read out values original in serial GPIO registers.
  ///
  if (PchSeries == PchH) {
    GpioUseSelValue   = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_USE_SEL));
    GpioIoSelValue    = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_IO_SEL));
  }

  if (PchSeries == PchLp) {
    GpioUseSelValue   = IoRead32 ((UINTN) (mPchGpioBase + (R_PCH_GP_N_CONFIG0 + (SerialGpioPinIndex * 0x08))));
  }

  GpioBlinkValue    = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_BLINK));
  GpioSerBlinkValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SER_BLINK));
  

  SerialGpioInstance->RegistersToRecover.SavedGpioUseSelValue   = GpioUseSelValue;
  SerialGpioInstance->RegistersToRecover.SavedGpioBlinkValue    = GpioBlinkValue;
  SerialGpioInstance->RegistersToRecover.SavedGpioSerBlinkValue = GpioSerBlinkValue;
  if (PchSeries == PchH) {
    SerialGpioInstance->RegistersToRecover.SavedGpioIoSelValue    = GpioIoSelValue;
  }

  ///
  /// Modify settings in serial GPIO registers.
  ///
  ///
  /// Serial GPIO will have to be selected as GPIO, not native
  ///
  if (PchSeries == PchH) {
    GpioUseSelValue |= (1 << SerialGpioPinIndex);
  }

  if (PchSeries == PchLp) {
    GpioUseSelValue |= B_PCH_GPIO_OWN0_GPIO_USE_SEL;
  }

  ///
  /// Serial GPIO will have no Blink setting
  ///
  GpioBlinkValue &= (~(1 << SerialGpioPinIndex));

  ///
  /// Serial GPIO will have to enable serial Binlk setting
  ///
  GpioSerBlinkValue |= (1 << SerialGpioPinIndex);

  ///
  /// Serial GPIO will have to be output
  ///
  if (PchSeries == PchH) {
    GpioIoSelValue &= (~(1 << SerialGpioPinIndex));
  }

  if (PchSeries == PchLp) {
   GpioUseSelValue &= (~B_PCH_GPIO_OWN0_GPIO_IO_SEL);
  }

  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  if (PchSeries == PchH) {
    IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_USE_SEL), GpioUseSelValue);
    IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_IO_SEL), GpioIoSelValue);
  }

  if (PchSeries == PchLp) {
    IoWrite32 ((UINTN) (mPchGpioBase + (R_PCH_GP_N_CONFIG0 + (SerialGpioPinIndex * 0x08))), GpioUseSelValue);
  }

  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_BLINK), GpioBlinkValue);
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SER_BLINK), GpioSerBlinkValue);
 
  ///
  /// Record this GPIO index to private data structure
  ///
  SerialGpioInstance->CurrentActiveSerialGpio = SerialGpioPinIndex;

  return EFI_SUCCESS;
}

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
  )
{
  UINT32                GpioSerBlinkValue;
  UINT32                GpioUseSelValue;
  UINT32                GpioIoSelValue;
  UINT32                GpioBlinkValue;
  SERIAL_GPIO_INSTANCE  *SerialGpioInstance;
  PCH_SERIES            PchSeries;

  SerialGpioInstance = SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL (This);
  if (SerialGpioInstance == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((SerialGpioPinIndex != SerialGpioInstance->CurrentActiveSerialGpio) ||
      (SerialGpioPinIndex >= SERIAL_GPIO_MAX_PIN_NUMBER)
      ) {
    return EFI_INVALID_PARAMETER;
  }

  GpioUseSelValue = 0;
  GpioIoSelValue = 0;
  PchSeries  = GetPchSeries();

  GpioUseSelValue   = SerialGpioInstance->RegistersToRecover.SavedGpioUseSelValue;
  GpioBlinkValue    = SerialGpioInstance->RegistersToRecover.SavedGpioBlinkValue;
  GpioSerBlinkValue = SerialGpioInstance->RegistersToRecover.SavedGpioSerBlinkValue;

  if (PchSeries == PchH) {
    GpioIoSelValue    = SerialGpioInstance->RegistersToRecover.SavedGpioIoSelValue;
  }

  ///
  /// At least to clear the serial Blink property
  ///
  GpioSerBlinkValue &= (~(1 << SerialGpioInstance->CurrentActiveSerialGpio));

  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Write values with original values in serial GPIO registers.
  ///

  if (PchSeries == PchH) {
    IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_USE_SEL), GpioUseSelValue);
    IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_IO_SEL), GpioIoSelValue);
  }

  if (PchSeries == PchLp) {
    IoWrite32 ((UINTN) (mPchGpioBase + (R_PCH_GP_N_CONFIG0 + (SerialGpioPinIndex * 0x08))), GpioUseSelValue);
  }
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_BLINK), GpioBlinkValue);
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SER_BLINK), GpioSerBlinkValue);

  ///
  /// Clear the GPIO index in private data structure
  ///
  SerialGpioInstance->CurrentActiveSerialGpio = SERIAL_GPIO_PIN_CLEARED;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS            Status;
  UINTN                 DataCountInDword;
  UINTN                 DataCountLeftInByte;
  UINTN                 Index;
  UINT32                GpioSbCmdStsValue;
  SERIAL_GPIO_INSTANCE  *SerialGpioInstance;

  SerialGpioInstance = SERIAL_GPIO_INSTANCE_FROM_SERIAL_GPIO_PROTOCOL (This);
  if (SerialGpioInstance == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check if the parameters are valid.
  ///
  if ((Buffer == NULL) ||
      (DataRate > SERIAL_GPIO_MAX_DATA_RATE) ||
      (GpioPinIndex != SerialGpioInstance->CurrentActiveSerialGpio)
      ) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Make sure it's safe to program the serial GPIO.
  ///
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// set data rate
  ///
  GpioSbCmdStsValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_CMDSTS));
  GpioSbCmdStsValue &= (~B_PCH_GPIO_SB_CMDSTS_DRS_MASK);
  GpioSbCmdStsValue |= (DataRate << 16);
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_CMDSTS), GpioSbCmdStsValue);
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  DataCountInDword    = DataCountInByte / 4;
  DataCountLeftInByte = DataCountInByte % 4;
  for (Index = 0; Index < DataCountInDword; Index++) {
    Status = SendSerialGpioSend (
              This,
              EnumSerialGpioDataDword,
              (Buffer + Index * 4)
              );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }

  for (Index = 0; Index < DataCountLeftInByte; Index++) {
    Status = SendSerialGpioSend (
              This,
              EnumSerialGpioDataByte,
              (Buffer + DataCountInDword * 4 + Index)
              );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

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
  )
{
  UINT32  DataInDword;
  UINT32  GpioSbCmdStsValue;

  ///
  /// Wait the SERIAL GPIO BUSY to be cleared.
  ///
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// set data length
  ///
  GpioSbCmdStsValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_CMDSTS));
  GpioSbCmdStsValue &= (~B_PCH_GPIO_SB_CMDSTS_DLS_MASK);
  GpioSbCmdStsValue |= (DataWidth << 22);
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_CMDSTS), GpioSbCmdStsValue);
  ///
  /// Set Data
  ///
  DataInDword = *(UINT32 *) Data;
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_DATA), DataInDword);
  ///
  /// Set GO to start transmit
  ///
  GpioSbCmdStsValue |= B_PCH_GPIO_SB_CMDSTS_GO;
  IoWrite32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_CMDSTS), GpioSbCmdStsValue);
  if (WaitForSerialGpioNotBusy () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

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
  )
{
  UINTN   WaitTicks;
  UINTN   WaitCount;
  UINT32  GpioSbCmdStsValue;

  ///
  /// Convert the wait period allowed into to tick count
  ///
  WaitCount = WAIT_TIME / WAIT_PERIOD;
  ///
  /// Wait for the SERIAL_GPIO cycle to complete.
  ///
  for (WaitTicks = 0; WaitTicks < WaitCount; WaitTicks++) {
    GpioSbCmdStsValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SB_CMDSTS));
    if ((GpioSbCmdStsValue & B_PCH_GPIO_SB_CMDSTS_BUSY) == 0) {
      return EFI_SUCCESS;
    }

    PchPmTimerStall (WAIT_PERIOD);
  }

  return EFI_DEVICE_ERROR;
}

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
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS            Status;
  SERIAL_GPIO_INSTANCE  *SerialGpioInstance;
  UINT32                GpioSerBlinkValue;
  UINT32                GpioUseSelValue;
  UINT32                GpioIoSelValue;
  UINT32                GpioBlinkValue;
  PCH_SERIES            PchSeries;

  DEBUG ((EFI_D_INFO, "InstallPchSerialGpio() Start\n"));

  ///
  /// Locate CPU IO protocol
  ///
  if (!IsPchSupported ()) {
    DEBUG ((EFI_D_ERROR, "SERIAL GPIO Protocol not supported due to no proper PCH LPC found!\n"));
    return EFI_UNSUPPORTED;
  }

  GpioUseSelValue = 0;
  GpioIoSelValue = 0;
  PchSeries  = GetPchSeries();

  ///
  /// PCH RCBA must be initialized prior to run this driver.
  ///
  mPchGpioBase = MmioRead16 (
                  MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_GPIO_BASE)
                  ) & B_PCH_LPC_GPIO_BASE_BAR;
  ASSERT (mPchGpioBase != 0);

  ///
  /// Allocate Runtime memory for the SERIAL_GPIO protocol instance.
  ///
  SerialGpioInstance = AllocateRuntimePool (sizeof (SERIAL_GPIO_INSTANCE));
  if (SerialGpioInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (PchSeries == PchH) {
    GpioUseSelValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_USE_SEL));
    GpioIoSelValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_IO_SEL));
  }
  GpioBlinkValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_BLINK));
  GpioSerBlinkValue = IoRead32 ((UINTN) (mPchGpioBase + R_PCH_GPIO_SER_BLINK));
  SerialGpioInstance->Signature = PCH_SERIAL_GPIO_PRIVATE_DATA_SIGNATURE;
  SerialGpioInstance->Handle = NULL;
  SerialGpioInstance->CurrentActiveSerialGpio = SERIAL_GPIO_PIN_CLEARED;

  if (PchSeries == PchH) {
    SerialGpioInstance->RegistersToRecover.SavedGpioUseSelValue = GpioUseSelValue;
    SerialGpioInstance->RegistersToRecover.SavedGpioIoSelValue = GpioIoSelValue;
  }
  SerialGpioInstance->RegistersToRecover.SavedGpioBlinkValue = GpioBlinkValue;
  SerialGpioInstance->RegistersToRecover.SavedGpioSerBlinkValue = GpioSerBlinkValue;

  SerialGpioInstance->SerialGpioProtocol.SerialGpioRegister = PchSerialGpioRegister;
  SerialGpioInstance->SerialGpioProtocol.SerialGpioSendData = PchSerialGpioSendData;
  SerialGpioInstance->SerialGpioProtocol.SerialGpioUnRegister = PchSerialGpioUnRegister;
  ///
  /// Install the EFI_SERIAL_GPIO_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(SerialGpioInstance->Handle),
                  &gEfiSerialGpioProtocolGuid,
                  &(SerialGpioInstance->SerialGpioProtocol),
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    FreePool (SerialGpioInstance);
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_INFO, "InstallPchSerialGpio() End\n"));

  return EFI_SUCCESS;
}
