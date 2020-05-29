/** @file
  I2c enumerate driver

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

#include "I2cDxe.h"
#include "I2cEnum.h"

//
//  EFI_DRIVER_BINDING_PROTOCOL instance
//
STATIC EFI_DRIVER_BINDING_PROTOCOL      mI2cEnumDriverBinding = {
  I2cEnumDriverSupported,
  I2cEnumDriverStart,
  I2cEnumDriverStop,
  0x10,
  NULL,
  NULL
};

//
// Template for I2C Enumerate Child Device.
//
STATIC I2C_ENUM_CONTEXT                 mI2cEnumContextTemplate = {
  I2C_ENUMERATE_SIGNATURE,
  {                     // I2c Enumerate Protocol
    I2cEnumerate,       // I2c Enumerate
    I2cGetBusFrequency  // I2c GetBusFrequency
  },
  NULL                  // Device List
};

//
// Driver name table 
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE           mI2cEnumDriverNameTable[] = {
  { "eng;en", (CHAR16 *) L"I2C Enumerate Driver" },
  { NULL , NULL }
};

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL        mI2cEnumComponentName = {
  (EFI_COMPONENT_NAME_GET_DRIVER_NAME) I2cEnumComponentNameGetDriverName,
  (EFI_COMPONENT_NAME_GET_CONTROLLER_NAME) I2cEnumComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL       mI2cEnumComponentName2 = {
  I2cEnumComponentNameGetDriverName,
  I2cEnumComponentNameGetControllerName,
  "en"
};

STATIC  UINT64          *mControllerTable = NULL;
STATIC  LIST_ENTRY      mI2cDeviceTable;
STATIC  EFI_EVENT       mI2cIoNotificationEvent = NULL;
STATIC  VOID            *mI2cIoRegistration = NULL;

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 4646 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
I2cEnumComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL      *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mI2cEnumDriverNameTable,
           DriverName,
           (BOOLEAN)(This != &mI2cEnumComponentName2)
           );
}

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
  )
{
  return EFI_UNSUPPORTED;
}

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
  )
{
  EFI_STATUS                    Status;
  EFI_I2C_ENUMERATE_PROTOCOL    *I2cEnumerate;
  EFI_I2C_HOST_PROTOCOL         *I2cHost;
  //
  // Determine if the I2C Host Protocol is available
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cHostProtocolGuid,
                  (VOID **) &I2cHost,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  //  Determine if the I2c Enumerate Protocol is NOT available
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cEnumerateProtocolGuid,
                  (VOID **) &I2cEnumerate,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }
  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                    Status;
  I2C_ENUM_CONTEXT              *I2cEnum;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINT64                        HcToken;
  H2O_I2C_DEVICE_CONTEXT        *I2cDev;
  LIST_ENTRY                    *Node;
  BOOLEAN                       DeviceFound;

  I2cEnum = NULL;
  I2cDev  = NULL;
  HcToken = 0;
  DeviceFound = FALSE;
  //
  // Determine if the I2C controller is available for installing I2C enumerate protocol by examing device path
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Get the I2C host controller node from device path
  //
  if ((DevicePathType (DevicePath) == HARDWARE_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == HW_MEMMAP_DP)) {
    //
    // The controller in the form of MMIO
    //
    HcToken = ((MEMMAP_DEVICE_PATH*)DevicePath)->StartingAddress;
  } else {
    //
    // The controller in the form of PCI
    //
    while (!IsDevicePathEnd (DevicePath)) {
      if ((DevicePathType (DevicePath) == HARDWARE_DEVICE_PATH) &&
          (DevicePathSubType (DevicePath) == HW_PCI_DP)) {
        HcToken = LShiftU64 (HcToken, 16);
        HcToken |= (UINT64)((((PCI_DEVICE_PATH*)DevicePath)->Device << 8) + (((PCI_DEVICE_PATH*)DevicePath)->Function));
      }
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }
  if (HcToken == 0) {
    return EFI_UNSUPPORTED;
  }
  //
  // Searching for matched controller in the device list 
  //
  Node = &mI2cDeviceTable;
  do {
    Node = GetFirstNode (Node);
    I2cDev = H2O_I2C_DEVICE_CONTEXT_FROM_CONTROLLER_LINK (Node);
    if (mControllerTable[I2cDev->Controller] == HcToken) {
      DeviceFound = TRUE;
      break;
    }
  } while (!IsNodeAtEnd (&mI2cDeviceTable, Node));
  if (!DeviceFound) {
    return EFI_NO_MAPPING;
  }
  //
  // Setting up and Install I2C enumerate protocol
  //
  I2cEnum = AllocatePool (sizeof (I2C_ENUM_CONTEXT));
  if (I2cEnum == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (I2cEnum, &mI2cEnumContextTemplate, sizeof (I2C_ENUM_CONTEXT));
  I2cEnum->DeviceList = I2cDev;
  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiI2cEnumerateProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID**)&I2cEnum->I2cEnumerate
                  );
  return Status;
}

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
  )
{
  EFI_STATUS                  Status;
  EFI_I2C_ENUMERATE_PROTOCOL  *I2cEnumerate;
  I2C_ENUM_CONTEXT            *I2cEnum;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cEnumerateProtocolGuid,
                  (VOID **) &I2cEnumerate,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    gBS->UninstallProtocolInterface (
           Controller,
           &gEfiI2cEnumerateProtocolGuid,
           (VOID*)I2cEnumerate
           );
    I2cEnum = I2C_ENUMERATE_CONTEXT_FROM_ENUMERATE_PROTOCOL (I2cEnumerate);
    //
    // No more child now, free bus context data.
    //
    FreePool (I2cEnum);
  }
  return Status;
}

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
  )
{
  EFI_STATUS                    Status;
  I2C_ENUM_CONTEXT              *I2cEnum;
  H2O_I2C_DEVICE_CONTEXT        *I2cDev;

  Status = EFI_SUCCESS;
  if (Device == NULL) return EFI_INVALID_PARAMETER;
  I2cEnum = I2C_ENUMERATE_CONTEXT_FROM_ENUMERATE_PROTOCOL (This);
  if (*Device == NULL) {
    //
    // Return first entry
    //
    I2cDev = I2cEnum->DeviceList;
    if (I2cDev == NULL) {
      Status = EFI_NO_MAPPING;
    }
  } else {
    //
    // Return next entry
    //
    I2cDev = H2O_I2C_DEVICE_CONTEXT_FROM_I2C_DEVICE (*Device);
    if (IsNodeAtEnd (&I2cEnum->DeviceList->NextDevice, &I2cDev->NextDevice) || IsNull (&I2cEnum->DeviceList->NextDevice, &I2cDev->NextDevice)) {
      I2cDev = NULL;
      Status = EFI_NO_MAPPING;
    } else {
      I2cDev = H2O_I2C_DEVICE_CONTEXT_FROM_DEVICE_LINK (GetFirstNode (&I2cDev->NextDevice));
    }
  }
  if (I2cDev) {
    *Device = &I2cDev->I2cDevice;
  }
  return Status;
}

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
EFIAPI
I2cGetBusFrequency (
  IN CONST EFI_I2C_ENUMERATE_PROTOCOL   *This,
  IN UINTN                              I2cBusConfiguration,
  OUT UINTN                             *BusClockHertz
  )
{
  UINT16        *BusClockHertzTable;
  UINTN         Total;

  if (BusClockHertz == NULL) return EFI_INVALID_PARAMETER;
  BusClockHertzTable = (UINT16*)PcdGetPtr (PcdI2cBusSpeedTable);
  if (BusClockHertzTable == NULL) return EFI_NO_MAPPING;
  for (Total = 0; BusClockHertzTable[Total] != 0; Total ++);
  if (I2cBusConfiguration >= Total) return EFI_NO_MAPPING;
  *BusClockHertz = (UINTN)BusClockHertzTable[I2cBusConfiguration];
  return EFI_SUCCESS;
}

/**

  Returns the current state information for the adapter

  @param[in]    This                    A pointer to the EFI_ADAPTER_INFORMATION_PROTOCOL instance
  @param[in]    InformationType         A pointer to an EFI_GUID that defines the contents of
                                        InformationBlock. The caller must use the InformationType
                                        to specify the information it needs to retrieve from this
                                        service and to determine how to parse the InformationBlock.
                                        The driver should not attempt to free InformationType
  @param[out]   InformationBlock        This service returns a pointer to the buffer with the
                                        InformationBlock structure which contains details about the
                                        data specific to InformationType. This structure is defined
                                        based on the type of data returned, and will be different for
                                        different data types.
                                        This service and caller decode this structure and its contents
                                        based on InformationType.
                                        This buffer is allocated by this service, and it is the
                                        responsibility of the caller to free it after using it
  @param[out]   InformationBlockSize    The driver returns the size of the InformationBlock in bytes

**/
EFI_STATUS
EFIAPI
I2cAipGetInformation (
  IN EFI_ADAPTER_INFORMATION_PROTOCOL   *This,
  IN EFI_GUID                           *InformationType,
  OUT VOID                              **InformationBlock,
  OUT UINTN                             *InformationBlockSize
  )
{
  H2O_I2C_DEVICE_CONTEXT        *I2cDev;
  VOID                          *Buffer;

  if (This == NULL || InformationBlock == NULL || InformationBlockSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (InformationType == NULL || CompareGuid (InformationType, &gI2cHidDeviceInfoGuid) == FALSE) {
    return EFI_UNSUPPORTED;
  }
  I2cDev = H2O_I2C_DEVICE_CONTEXT_FROM_AIP (This);
  Buffer = AllocatePool (sizeof (H2O_I2C_HID_DEVICE));
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Buffer, &I2cDev->I2cHidDevice, sizeof (H2O_I2C_HID_DEVICE));
  *InformationBlock     = Buffer;
  *InformationBlockSize = sizeof (H2O_I2C_HID_DEVICE);
  return EFI_SUCCESS;
}

/**

  Sets state information for an adapter

  @param[in]    This                    A pointer to the EFI_ADAPTER_INFORMATION_PROTOCOL instance
  @param[in]    InformationType         A pointer to an EFI_GUID that defines the contents of
                                        InformationBlock. The caller must use the InformationType
                                        to specify the information it wants the service.
  @param[in]    InformationBlock        A pointer to the InformationBlock structure which contains
                                        details about the data specific to InformationType.
                                        This structure is defined based on the type of data sent,
                                        and will be different for different data types.
                                        The driver and caller decode this structure and its contents
                                        based on InformationType. This buffer is allocated by the caller.
                                        It is the responsibility of the caller to free it after the caller
                                        has set the requested parameters
  @param[in]    InformationBlockSize    The size of the InformationBlock in bytes

**/
EFI_STATUS
EFIAPI
I2cAipSetInformation (
  IN EFI_ADAPTER_INFORMATION_PROTOCOL   *This,
  IN EFI_GUID                           *InformationType,
  IN VOID                               *InformationBlock,
  IN UINTN                              InformationBlockSize
  )
{
  return EFI_UNSUPPORTED;
}

/**

  Get a list of supported information types for this instance of the protocol

  @param[in]    This                    A pointer to the EFI_ADAPTER_INFORMATION_PROTOCOL instance
  @param[out]   InfoTypesBuffer         A pointer to the list of InformationType GUIDs that
                                        are supported by This. This buffer is allocated by this service,
                                        and it is the responsibility of the caller to free it after using it
  @param[out]   InfoTypesBufferCount    A pointer to the number of GUID pointers present in InfoTypesBuffer
**/
EFI_STATUS
EFIAPI
I2cAipGetSupportedTypes (
  IN EFI_ADAPTER_INFORMATION_PROTOCOL   *This,
  OUT EFI_GUID                          **InfoTypesBuffer,
  OUT UINTN                             *InfoTypesBufferCount
  )
{
  EFI_GUID      *Buffer;

  if (This == NULL || InfoTypesBuffer == NULL || InfoTypesBufferCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Buffer = (EFI_GUID*)AllocatePool (sizeof (EFI_GUID));
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Buffer, &gI2cHidDeviceInfoGuid, sizeof (EFI_GUID));
  *InfoTypesBuffer      = Buffer;
  *InfoTypesBufferCount = 1;
  return EFI_SUCCESS;
}

/**

  Signal handlers for I2cIo protocol installation event

  @param[in]    Event           Event type                
  @param[in]    Context         Context for the event

**/
VOID
EFIAPI
I2cIoNotifyFunction (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  EFI_STATUS                    Status;
  UINTN                         BufferSize;
  EFI_HANDLE                    Handle;
  EFI_I2C_IO_PROTOCOL           *I2cIo;
  VOID                          *Interface;
  I2C_DEVICE_CONTEXT            *I2cDeviceContext;
  H2O_I2C_DEVICE_CONTEXT        *I2cDev;
  
  while (TRUE) {
    BufferSize = sizeof (EFI_HANDLE);
    Status = gBS->LocateHandle (
                    ByRegisterNotify,
                    NULL,
                    mI2cIoRegistration,
                    &BufferSize,
                    &Handle
                    );
    if (EFI_ERROR (Status)) {
      //
      // If no more notification events exist
      //
      return;
    }
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiI2cIoProtocolGuid,
                    (VOID**) &I2cIo
                    );
    if (EFI_ERROR (Status)) {
      //
      // The I2cIo protocol not found, go next event
      //
      continue;
    }
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiAdapterInformationProtocolGuid,
                    &Interface
                    );
    if (!EFI_ERROR (Status)) {
      //
      // The AdapterInfo installed, go next event 
      //
      continue;
    }
    //
    // Although the H2O_I2C_DEVICE_CONTEXT can be easily get by I2C_DEVICE_CONTEXT_FROM_PROTOCOL (I2cIo) and 
    // H2O_I2C_DEVICE_CONTEXT_FROM_I2C_DEVICE (I2cDeviceContext->I2cDevice), However we should not use this way 
    // to make the code can be separated from standard EDKII I2cDxe driver
    //
    I2cDeviceContext = I2C_DEVICE_CONTEXT_FROM_PROTOCOL (I2cIo);
    I2cDev = H2O_I2C_DEVICE_CONTEXT_FROM_I2C_DEVICE (I2cDeviceContext->I2cDevice);
    //
    // Install AIP if it is I2C HID device
    //
    if (I2cDev->I2cHidDevice.ClassType != 0) {
      I2cDev->Aip.GetInformation    = I2cAipGetInformation;
      I2cDev->Aip.SetInformation    = I2cAipSetInformation;
      I2cDev->Aip.GetSupportedTypes = I2cAipGetSupportedTypes;
      Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &gEfiAdapterInformationProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID**)&I2cDev->Aip
                      );
    }
  }
}

/**
  The user entry point for the I2C enumerate module. The user code starts with
  this function.

  @param[in]    ImageHandle     The firmware allocated handle for the EFI image.
  @param[in]    SystemTable     A pointer to the EFI System Table.

  @retval       EFI_SUCCESS     The entry point is executed successfully.
  @retval       other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeI2cEnum (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                    Status;
  UINTN                         TokenNumber;
  UINTN                         Total;
  UINT64                        *ControllerTable;
  H2O_I2C_DEVICE_CONTEXT        *I2cDev;
  H2O_I2C_DEVICE_CONTEXT        *I2cDevHead;
  H2O_I2C_DEVICE_PCD            *I2cDevicePcd;
  BOOLEAN                       HidDeviceFound;
  LIST_ENTRY                    *BaseNode;
  LIST_ENTRY                    *NewNode;
  LIST_ENTRY                    *Node;

  HidDeviceFound   = FALSE;
  mControllerTable = NULL;
  InitializeListHead (&mI2cDeviceTable);
  //
  // Initial I2C controller number lookup table
  //
  ControllerTable = (UINT64*)PcdGetPtr (PcdI2cControllerTable);
  if (ControllerTable == NULL) {
    return EFI_NO_MAPPING;
  }
  for (Total = 0; ControllerTable[Total] != 0; Total ++);
  if (Total == 0) {
    return EFI_NO_MAPPING;
  }
  mControllerTable = AllocateZeroPool (Total * sizeof (UINT64));
  if (mControllerTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (mControllerTable, ControllerTable, Total * sizeof (UINT64));
  //
  // Initial I2C devices from PCD
  //
  for (TokenNumber = 0; (TokenNumber = LibPcdGetNextToken (&gI2cDeviceTokenSpaceGuid, TokenNumber)) != 0; ) {
    I2cDevicePcd = (H2O_I2C_DEVICE_PCD*)LibPcdGetExPtr (&gI2cDeviceTokenSpaceGuid, TokenNumber);
    I2cDev = AllocateZeroPool (sizeof (H2O_I2C_DEVICE_CONTEXT));
    if (I2cDev == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    InitializeListHead (&I2cDev->NextController);
    InitializeListHead (&I2cDev->NextDevice);
    CopyGuid (&I2cDev->DeviceGuid, &I2cDevicePcd->Guid);
    I2cDev->Signature                     = H2O_I2C_DEVICE_SIGNATURE;
    I2cDev->I2cDevice.DeviceIndex         = (UINT32)TokenNumber;
    I2cDev->I2cDevice.HardwareRevision    = I2cDevicePcd->HardwareRevision;
    I2cDev->I2cDevice.I2cBusConfiguration = I2cDevicePcd->I2cBusConfiguration;
    I2cDev->I2cDevice.SlaveAddressCount   = 1;
    I2cDev->I2cDevice.SlaveAddressArray   = &I2cDev->SlaveAddress;
    I2cDev->I2cDevice.DeviceGuid          = &I2cDev->DeviceGuid;
    I2cDev->SlaveAddress                  = I2cDevicePcd->SlaveAddress;
    I2cDev->Controller                    = I2cDevicePcd->I2cController;
    I2cDev->I2cHidDevice.Length           = sizeof (H2O_I2C_HID_DEVICE);
    I2cDev->I2cHidDevice.I2cDevice        = &I2cDev->I2cDevice;
    I2cDev->I2cHidDevice.DescReg          = I2cDevicePcd->I2cHidDescReg;
    I2cDev->I2cHidDevice.ClassType        = I2cDevicePcd->I2cHidClassType;
    I2cDev->I2cHidDevice.GpioPin          = I2cDevicePcd->I2cHidGpioPin;
    I2cDev->I2cHidDevice.GpioLevel        = I2cDevicePcd->I2cHidGpioLevel;
    I2cDev->I2cHidDevice.GpioHc           = I2cDevicePcd->I2cHidGpioHc;
    //
    // Add record into list
    //
    BaseNode = &mI2cDeviceTable;
    NewNode  = &I2cDev->NextController;
    if (!IsListEmpty (&mI2cDeviceTable)) {
      //
      // Searching for appropriate host controller to append new record
      //
      Node = BaseNode;
      do {
        Node = GetFirstNode (Node);
        I2cDevHead = H2O_I2C_DEVICE_CONTEXT_FROM_CONTROLLER_LINK (Node);
        if (I2cDevHead->Controller == I2cDev->Controller) {
          BaseNode = &I2cDevHead->NextDevice;
          NewNode  = &I2cDev->NextDevice;
          break;
        }
      } while (!IsNodeAtEnd (&mI2cDeviceTable, Node));
    }
    InsertTailList (
      BaseNode,
      NewNode
      );
    if (I2cDev->I2cHidDevice.ClassType != 0) {
      //
      // Set the flag to indicates the HID device found
      //
      HidDeviceFound = TRUE;
    }
  }
  if (IsListEmpty (&mI2cDeviceTable)) {
    return EFI_NO_MAPPING;
  }
  //
  // HID device specfic routine
  //
  if (HidDeviceFound) {
    //
    // Registers the I2cIo protocol notification use to installing HID specific protocol
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    I2cIoNotifyFunction,
                    NULL,
                    &mI2cIoNotificationEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->RegisterProtocolNotify (
                    &gEfiI2cIoProtocolGuid,
                    mI2cIoNotificationEvent,
                    &mI2cIoRegistration
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &mI2cEnumDriverBinding,
             NULL,
             &mI2cEnumComponentName,
             &mI2cEnumComponentName2
             );
  return Status;
}

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
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *DeviceHandleBuffer;
  UINTN                         DeviceHandleCount;
  UINTN                         Index;
  LIST_ENTRY                    *Node1;
  LIST_ENTRY                    *Node2;
  H2O_I2C_DEVICE_CONTEXT        *I2cDev;
  VOID                          *Interface;

  if (mControllerTable) {
    FreePool (mControllerTable);
    mControllerTable = NULL;
  }
  while (!IsListEmpty (&mI2cDeviceTable)) {
    Node1 = GetFirstNode (&mI2cDeviceTable);
    I2cDev = H2O_I2C_DEVICE_CONTEXT_FROM_CONTROLLER_LINK (Node1);
    Node2 = &I2cDev->NextDevice;
    while (!IsListEmpty (Node2)) {
      Node2 = GetFirstNode (Node2);
      RemoveEntryList (Node2);
      FreePool (H2O_I2C_DEVICE_CONTEXT_FROM_DEVICE_LINK (Node2));
    }
    RemoveEntryList (Node1);
    FreePool (I2cDev);
  }
  if (mI2cIoNotificationEvent) {
    gBS->CloseEvent (mI2cIoNotificationEvent);
    mI2cIoNotificationEvent = NULL;
  }
  //
  // Get the list of all I2C Enumerate protocol in the handle database.
  // If there is an error getting the list, then the unload
  // operation fails.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiI2cEnumerateProtocolGuid,
                  NULL,
                  &DeviceHandleCount,
                  &DeviceHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < DeviceHandleCount; Index++) {
      //
      // Uninstall I2C enumerate protocol in the handle
      //
      Status = gBS->HandleProtocol (
                      DeviceHandleBuffer[Index],
                      &gEfiI2cEnumerateProtocolGuid,
                      &Interface
                      );
      if (!EFI_ERROR (Status)) {
        gBS->UninstallProtocolInterface (
               DeviceHandleBuffer[Index],
               &gEfiI2cEnumerateProtocolGuid,
               Interface
               );
      }
    }
    gBS->FreePool (DeviceHandleBuffer);
  }
  //
  // Get the list of all I2C IO protocol in the handle database.
  // If there is an error getting the list, then the unload
  // operation fails.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiI2cIoProtocolGuid,
                  NULL,
                  &DeviceHandleCount,
                  &DeviceHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < DeviceHandleCount; Index++) {
      //
      // Uninstall EfiAdapterInformation protocol in the handle
      //
      Status = gBS->HandleProtocol (
                      DeviceHandleBuffer[Index],
                      &gEfiAdapterInformationProtocolGuid,
                      &Interface
                      );
      if (!EFI_ERROR (Status)) {
        gBS->UninstallProtocolInterface (
               DeviceHandleBuffer[Index],
               &gEfiAdapterInformationProtocolGuid,
               Interface
               );
      }
    }
    gBS->FreePool (DeviceHandleBuffer);
  }
  //
  // Uninstall all the protocols installed in the driver entry point
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  mI2cEnumDriverBinding.DriverBindingHandle,
                  &gEfiDriverBindingProtocolGuid,
                  &mI2cEnumDriverBinding,
                  &gEfiComponentNameProtocolGuid,
                  &mI2cEnumComponentName,
                  &gEfiComponentName2ProtocolGuid,
                  &mI2cEnumComponentName2,
                  NULL
                  );
  return Status;
}
