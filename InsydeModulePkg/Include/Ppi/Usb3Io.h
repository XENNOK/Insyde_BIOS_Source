/** @file
  Usb3 IO PPI Header

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_USB3_IO_PPI_H_
#define _PEI_USB3_IO_PPI_H_

#include <Protocol/Usb3HostController.h>

///
/// Global ID for the PEI_USB3_IO_PPI.
///
#define PEI_USB3_IO_PPI_GUID \
  { \
    0x9b732119, 0xc5f7, 0x4224, { 0xa8, 0x08, 0xe1, 0x23, 0x46, 0x82, 0xec, 0xb3 } \
  }

///
/// Forward declaration for the PEI_USB3_IO_PPI.
///
typedef struct _PEI_USB3_IO_PPI  PEI_USB3_IO_PPI;

/**
  Submits control transfer to a target USB device.

  @param[in]     PeiServices   The pointer to the PEI Services Table.
  @param[in]     This          The pointer to this instance of the PEI_USB3_IO_PPI.
  @param[in]     Request       A pointer to the USB device request that will be 
                               sent to the USB device.
  @param[in]     Direction     Specifies the data direction for the transfer. There 
                               are three values available: 
                               EfiUsbDataIn, EfiUsbDataOut and EfiUsbNoData.
  @param[in]     TimeOut       Indicates the maximum time, in milliseconds, that 
                               the transfer is allowed to complete.
  @param[in,out] Data          A pointer to the buffer of data that will be 
                               transmitted to or received from the USB device.
  @param[in]     DataLength    On input, indicates the size, in bytes, of the data 
                               buffer specified by Data.
                               
  @retval EFI_SUCCESS             The control transfer was completed successfully.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES    The control transfer could not be completed due 
                                  to a lack of resources.
  @retval EFI_TIMEOUT             The control transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR        The control transfer failed due to host controller 
                                  or device error.
                                  Caller should check TransferResult for detailed 
                                  error information.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_CONTROL_TRANSFER)(
  IN     EFI_PEI_SERVICES        **PeiServices,
  IN     PEI_USB3_IO_PPI         *This,
  IN     EFI_USB_DEVICE_REQUEST  *Request,
  IN     EFI_USB_DATA_DIRECTION  Direction,
  IN     UINT32                  Timeout,
  IN OUT VOID                    *Data OPTIONAL,
  IN     UINTN                   DataLength  OPTIONAL
  );

/**
  Submits bulk transfer to a target USB device.

  @param[in] PeiServices       The pointer to the PEI Services Table.
  @param[in] This              The pointer to this instance of the PEI_USB3_IO_PPI.
  @param[in] EndPointAddress   The endpoint address.
  @param[in] Data              The data buffer to be transfered.
  @param[in] DataLength        The length of data buffer.
  @param[in] TimeOut           The timeout for the transfer, in milliseconds.

  @retval EFI_SUCCESS             The bulk transfer completed successfully.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES    The bulk transfer could not be completed due to 
                                  a lack of resources.
  @retval EFI_TIMEOUT             The bulk transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR        The bulk transfer failed due to host controller 
                                  or device error.
                                  Caller should check TransferResult for detailed 
                                  error information.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_BULK_TRANSFER)(
  IN EFI_PEI_SERVICES  **PeiServices,
  IN PEI_USB3_IO_PPI   *This,
  IN UINT8             DeviceEndpoint,
  IN OUT VOID          *Data,
  IN OUT UINTN         *DataLength,
  IN UINTN             Timeout
  );

/**
  Submits sync interrupt transfer to a target USB device.

  @param[in] PeiServices       The pointer to the PEI Services Table.
  @param[in] This              The pointer to this instance of the PEI_USB3_IO_PPI.
  @param[in] EndPointAddress   The endpoint address.
  @param[in] Data              The data buffer to be transfered.
  @param[in] DataLength        The length of data buffer.
  @param[in] TimeOut           The timeout for the transfer, in milliseconds.

  @retval EFI_SUCCESS             The bulk transfer completed successfully.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES    The bulk transfer could not be completed due to 
                                  a lack of resources.
  @retval EFI_TIMEOUT             The bulk transfer failed due to timeout.
  @retval EFI_DEVICE_ERROR        The bulk transfer failed due to host controller 
                                  or device error.
                                  Caller should check TransferResult for detailed 
                                  error information.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_SYNC_INTERRUPT_TRANSFER)(
  IN EFI_PEI_SERVICES  **PeiServices,
  IN PEI_USB3_IO_PPI   *This,
  IN UINT8             DeviceEndpoint,
  IN OUT VOID          *Data,
  IN OUT UINTN         *DataLength,
  IN UINTN             Timeout
  );

/**
  Get interface descriptor from a USB device.

  @param[in] PeiServices           The pointer to the PEI Services Table.
  @param[in] This                  The pointer to this instance of the PEI_USB3_IO_PPI.
  @param[in] InterfaceDescriptor   The interface descriptor.

  @retval EFI_SUCCESS             The interface descriptor was returned.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_DEVICE_ERROR        A device error occurred, the function failed to 
                                  get the interface descriptor.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_GET_INTERFACE_DESCRIPTOR)(
  IN EFI_PEI_SERVICES              **PeiServices,
  IN PEI_USB3_IO_PPI               *This,
  IN EFI_USB_INTERFACE_DESCRIPTOR  **InterfaceDescriptor
  );

/**
  Get endpoint descriptor from a USB device.

  @param[in] PeiServices          The pointer to the PEI Services Table.
  @param[in] This                 The pointer to this instance of the PEI_USB3_IO_PPI.
  @param[in] EndPointIndex        The index of the end point.
  @param[in] EndpointDescriptor   The endpoint descriptor.

  @retval EFI_SUCCESS             The endpoint descriptor was returned.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_DEVICE_ERROR        A device error occurred, the function failed to 
                                  get the endpoint descriptor.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_GET_ENDPOINT_DESCRIPTOR)(
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB3_IO_PPI                *This,
  IN UINT8                          EndpointIndex,
  IN EFI_USB_ENDPOINT_DESCRIPTOR    **EndpointDescriptor
  );

/**
  Issue a port reset to the device.

  @param[in] PeiServices   The pointer to the PEI Services Table.
  @param[in] This          The pointer to this instance of the PEI_USB3_IO_PPI.

  @retval EFI_SUCCESS             The port reset was issued successfully.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_DEVICE_ERROR        Device error occurred.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_PORT_RESET)(
  IN EFI_PEI_SERVICES  **PeiServices,
  IN PEI_USB3_IO_PPI   *This
  );

/**
  Clear endpoint halt to a target endpoint.

  @param[in] PeiServices           The pointer to the PEI Services Table.
  @param[in] This                  The pointer to this instance of the PEI_USB3_IO_PPI.
  @param[in] EndPointAddress       The endpoint address.

  @retval EFI_SUCCESS             The interface descriptor was returned.
  @retval EFI_INVALID_PARAMETER   Some parameters are invalid.
  @retval EFI_DEVICE_ERROR        A device error occurred, the function failed to 
                                  get the interface descriptor.

**/
typedef
EFI_STATUS
(EFIAPI *PEI_USB3_CLEAR_ENDPOINT_HALT)(
  IN EFI_PEI_SERVICES  **PeiServices,
  IN PEI_USB3_IO_PPI   *This,
  IN UINT8             DeviceEndpoint
  );

///
/// This PPI contains a set of services to interact with the USB host controller.
/// These interfaces are modeled on the UEFI 2.3 specification EFI_USB_IO_PROTOCOL.
/// Refer to section 16.2.4 of the UEFI 2.3 Specification for more information on 
/// these interfaces.
///
struct _PEI_USB3_IO_PPI {
  PEI_USB3_CONTROL_TRANSFER         UsbControlTransfer;
  PEI_USB3_BULK_TRANSFER            UsbBulkTransfer;
  PEI_USB3_SYNC_INTERRUPT_TRANSFER  UsbSyncInterruptTransfer;
  PEI_USB3_GET_INTERFACE_DESCRIPTOR UsbGetInterfaceDescriptor;
  PEI_USB3_GET_ENDPOINT_DESCRIPTOR  UsbGetEndpointDescriptor;
  PEI_USB3_PORT_RESET               UsbPortReset;
  PEI_USB3_CLEAR_ENDPOINT_HALT      UsbClearEndpointHalt;
};

extern EFI_GUID gPeiUsb3IoPpiGuid;

#endif
