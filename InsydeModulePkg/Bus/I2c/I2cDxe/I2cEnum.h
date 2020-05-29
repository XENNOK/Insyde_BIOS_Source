/** @file
  Private data structures for I2c enumerate driver

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

#ifndef __I2C_ENUM_H__
#define __I2C_ENUM_H__

#include <Uefi.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/I2cIo.h>
#include <Protocol/I2cEnumerate.h>
#include <Protocol/AdapterInformation.h>
#include <I2cHidDeviceInfo.h>

#define I2C_ENUMERATE_SIGNATURE         SIGNATURE_32 ('I', '2', 'C', 'E')
#define H2O_I2C_DEVICE_SIGNATURE        SIGNATURE_32 ('H', '2', 'O', 'D')

//
// H2O I2C device
//
typedef struct {
  //
  // Signature
  //
  UINT32                                Signature;
  //
  // Link to next controller
  //
  LIST_ENTRY                            NextController;
  //
  // Link to next device
  //
  LIST_ENTRY                            NextDevice;
  //
  // I2C data for this device
  //
  EFI_I2C_DEVICE                        I2cDevice;
  //
  // I2C device GUID
  //
  EFI_GUID                              DeviceGuid;
  //
  // I2C device slave address
  //
  UINT32                                SlaveAddress;
  //
  // Host controller number, 0 based
  //
  UINT32                                Controller;
  //
  // I2C HID specific data
  //
  H2O_I2C_HID_DEVICE                    I2cHidDevice;
  //
  // EFI Adapter information protocol for I2C HID device
  //
  EFI_ADAPTER_INFORMATION_PROTOCOL      Aip;
} H2O_I2C_DEVICE_CONTEXT;

#define H2O_I2C_DEVICE_CONTEXT_FROM_CONTROLLER_LINK(a)  CR (a, H2O_I2C_DEVICE_CONTEXT, NextController, H2O_I2C_DEVICE_SIGNATURE)
#define H2O_I2C_DEVICE_CONTEXT_FROM_DEVICE_LINK(a)      CR (a, H2O_I2C_DEVICE_CONTEXT, NextDevice, H2O_I2C_DEVICE_SIGNATURE)
#define H2O_I2C_DEVICE_CONTEXT_FROM_I2C_DEVICE(a)       CR (a, H2O_I2C_DEVICE_CONTEXT, I2cDevice, H2O_I2C_DEVICE_SIGNATURE)
#define H2O_I2C_DEVICE_CONTEXT_FROM_AIP(a)              CR (a, H2O_I2C_DEVICE_CONTEXT, Aip, H2O_I2C_DEVICE_SIGNATURE)
//
// H2O I2C device in PCD
//
#pragma pack(1)
typedef struct {
  //
  // The unique GUID specific for this device, it will be part of device path node
  //
  EFI_GUID                              Guid;
  //
  // Slave address
  //
  UINT32                                SlaveAddress;
  //
  // Hardware revision
  //
  UINT32                                HardwareRevision;
  //
  // Interrupt GPIO pin active level, 0 = low active, 1 = high active
  //
  UINT8                                 I2cHidGpioLevel;
  //
  // Interrupt GPIO pin number
  //
  UINT8                                 I2cHidGpioPin;
  //
  // HID descriptor register number
  //
  UINT16                                I2cHidDescReg;
  //
  // HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
  //
  UINT16                                I2cHidClassType;
  //
  // Host controller number, 0 based
  //
  UINT8                                 I2cController;
  //
  // Bus configuration, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
  //
  UINT8                                 I2cBusConfiguration;
  //
  // The GPIO host controller
  //
  UINT8                                 I2cHidGpioHc;
} H2O_I2C_DEVICE_PCD;
#pragma pack()

//
// I2C enumerate context
//
typedef struct {
  //
  // Signature
  //
  UINT32                                Signature;
  //
  // I2C enumerate protocol
  //
  EFI_I2C_ENUMERATE_PROTOCOL            I2cEnumerate;
  //
  // H2O I2C devices
  //
  H2O_I2C_DEVICE_CONTEXT                *DeviceList;
} I2C_ENUM_CONTEXT;

#define I2C_ENUMERATE_CONTEXT_FROM_ENUMERATE_PROTOCOL(a)        CR (a, I2C_ENUM_CONTEXT, I2cEnumerate, I2C_ENUMERATE_SIGNATURE)

EFI_STATUS
EFIAPI
I2cEnumComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL      *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  );

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
I2cEnumComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL      *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle        OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  );

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Since ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
I2cEnumDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
I2cEnumDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
     same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
     EFI_HANDLE. In addition, all of these handles must have been created in this driver's
     Start() function, and the Start() function must have called OpenProtocol() on
     ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
I2cEnumDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  );

/**
  Enumerate the I2C devices

  This function enables the caller to traverse the set of I2C devices
  on an I2C bus.

  @param[in]  This              The platform data for the next device on
                                the I2C bus was returned successfully.
  @param[in, out] Device        Pointer to a buffer containing an
                                EFI_I2C_DEVICE structure.  Enumeration is
                                started by setting the initial EFI_I2C_DEVICE
                                structure pointer to NULL.  The buffer
                                receives an EFI_I2C_DEVICE structure pointer
                                to the next I2C device.

  @retval EFI_SUCCESS           The platform data for the next device on
                                the I2C bus was returned successfully.
  @retval EFI_INVALID_PARAMETER Device is NULL
  @retval EFI_NO_MAPPING        *Device does not point to a valid
                                EFI_I2C_DEVICE structure returned in a
                                previous call Enumerate().

**/
EFI_STATUS
EFIAPI
I2cEnumerate (
  IN CONST EFI_I2C_ENUMERATE_PROTOCOL   *This,
  IN OUT CONST EFI_I2C_DEVICE           **Device
  );

/**
  Get the requested I2C bus frequency for a specified bus configuration.

  This function returns the requested I2C bus clock frequency for the
  I2cBusConfiguration.  This routine is provided for diagnostic purposes
  and is meant to be called after calling Enumerate to get the
  I2cBusConfiguration value.

  @param[in] This                 Pointer to an EFI_I2C_ENUMERATE_PROTOCOL
                                  structure.
  @param[in] I2cBusConfiguration  I2C bus configuration to access the I2C
                                  device
  @param[out] *BusClockHertz      Pointer to a buffer to receive the I2C
                                  bus clock frequency in Hertz

  @retval EFI_SUCCESS           The I2C bus frequency was returned
                                successfully.
  @retval EFI_INVALID_PARAMETER BusClockHertz was NULL
  @retval EFI_NO_MAPPING        Invalid I2cBusConfiguration value

**/
EFI_STATUS
I2cGetBusFrequency (
  IN CONST EFI_I2C_ENUMERATE_PROTOCOL   *This,
  IN UINTN                              I2cBusConfiguration,
  OUT UINTN                             *BusClockHertz
  );

/**
  The user Entry Point for I2C enumerate module. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeI2cEnum (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  );

/**
  This is the unload handle for I2C enumerate module.

  Disconnect the driver specified by ImageHandle from all the devices in the handle database.
  Uninstall all the protocols installed in the driver entry point.

  @param[in] ImageHandle           The drivers' driver image.

  @retval    EFI_SUCCESS           The image is unloaded.
  @retval    Others                Failed to unload the image.

**/
EFI_STATUS
EFIAPI
I2cEnumUnload (
  IN EFI_HANDLE                         ImageHandle
  );

//
// The GUID of I2C device token space
//
extern EFI_GUID gI2cDeviceTokenSpaceGuid;

#endif  //  __I2C_ENUM_H__
