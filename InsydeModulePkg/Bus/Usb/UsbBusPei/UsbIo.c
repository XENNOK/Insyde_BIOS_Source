/** @file
  Usb Io PPI

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

#include "UsbBus.h"
#include "UsbHelper.h"

/**

  Pei Usb Control Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  Request               Request packet
  @param  Direction             IN/OUT/NODATA
  @param  Timeout               Time out value in millisecond
  @param  Data                  Data buffer
  @param  DataLength            Data length

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbControlTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     EFI_USB_DEVICE_REQUEST         *Request,
  IN     EFI_USB_DATA_DIRECTION         Direction,
  IN     UINT32                         Timeout,
  IN OUT VOID                           *Data,      OPTIONAL
  IN     UINTN                          DataLength  OPTIONAL
  )
{
  EFI_STATUS                   Status;
  PEI_USB_DEVICE               *PeiUsbDev;
  PEI_USB3_HOST_CONTROLLER_PPI *UsbHcPpi;
  UINT32                       TransferResult;

  PeiUsbDev = PEI_USB_DEVICE_FROM_THIS (This);
  UsbHcPpi  = PeiUsbDev->UsbHcPpi;
  Status = UsbHcPpi->ControlTransfer (
                       PeiServices,
                       UsbHcPpi,
                       PeiUsbDev->DeviceAddress,
                       PeiUsbDev->DeviceSpeed,
                       (PeiUsbDev->MaxPacketSize0 == 9) ? 0x200 : PeiUsbDev->MaxPacketSize0,
                       Request,
                       Direction,
                       Data,
                       &DataLength,
                       Timeout,
                       &PeiUsbDev->Translator,
                       &TransferResult
                       );
  return Status;
}

/**

  Pei Usb Bulk Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  DeviceEndpoint        Device Endpoint
  @param  Data                  Data buffer
  @param  DataLength            Data length
  @param  Timeout               Time out value in millisecond

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbBulkTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  )
{
  EFI_STATUS                   Status;
  PEI_USB_DEVICE               *PeiUsbDev;
  PEI_USB3_HOST_CONTROLLER_PPI *UsbHcPpi;
  UINT32                       TransferResult;
  UINTN                        MaxPacketLength;
  UINT8                        DataToggle;
  UINT8                        OldToggle;
  EFI_USB_ENDPOINT_DESCRIPTOR  *EndpointDescriptor;
  UINT8                        EndpointIndex;

  PeiUsbDev     = PEI_USB_DEVICE_FROM_THIS (This);
  UsbHcPpi      = PeiUsbDev->UsbHcPpi;
  EndpointIndex = 0;
  while (EndpointIndex < MAX_ENDPOINT) {
    Status = PeiUsbGetEndpointDescriptor (PeiServices, This, EndpointIndex, &EndpointDescriptor);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }
    if (EndpointDescriptor->EndpointAddress == DeviceEndpoint) {
      break;
    }
    EndpointIndex++;
  }
  if (EndpointIndex == MAX_ENDPOINT) {
    return EFI_INVALID_PARAMETER;
  }
  MaxPacketLength = PeiUsbDev->EndpointDesc[EndpointIndex]->MaxPacketSize;
  if ((PeiUsbDev->DataToggle & (1 << EndpointIndex)) != 0) {
    DataToggle = 1;
  } else {
    DataToggle = 0;
  }
  OldToggle = DataToggle;
  Status = UsbHcPpi->BulkTransfer (
                       PeiServices,
                       UsbHcPpi,
                       PeiUsbDev->DeviceAddress,
                       DeviceEndpoint,
                       PeiUsbDev->DeviceSpeed,
                       MaxPacketLength,
                       &Data,
                       DataLength,
                       &DataToggle,
                       Timeout,
                       &PeiUsbDev->Translator,
                       &TransferResult
                       );
  if (OldToggle != DataToggle) {
    PeiUsbDev->DataToggle = (UINT8) (PeiUsbDev->DataToggle ^ (1 << EndpointIndex));
  }
  return Status;
}

/**

  Pei Usb Sync Interrupt Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  DeviceEndpoint        Device Endpoint
  @param  Data                  Data buffer
  @param  DataLength            Data length
  @param  Timeout               Time out value in millisecond

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbSyncInterruptTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  )
{
  EFI_STATUS                     Status;
  PEI_USB_DEVICE                 *PeiUsbDev;
  PEI_USB3_HOST_CONTROLLER_PPI   *UsbHcPpi;
  UINT32                         TransferResult;
  UINTN                          MaxPacketLength;
  UINT8                          DataToggle, OldToggle;
  EFI_USB_ENDPOINT_DESCRIPTOR    *EndpointDescriptor;
  UINT8                          EndpointIndex;

  PeiUsbDev = PEI_USB_DEVICE_FROM_THIS (This);
  UsbHcPpi = PeiUsbDev->UsbHcPpi;
  EndpointIndex = 0;
  while (EndpointIndex < MAX_ENDPOINT) {
    Status = PeiUsbGetEndpointDescriptor (PeiServices, This, EndpointIndex, &EndpointDescriptor);
    if ( EFI_ERROR(Status)) {
      return EFI_INVALID_PARAMETER;
    }
    if (EndpointDescriptor->EndpointAddress == DeviceEndpoint) {
      break;
    }
    EndpointIndex++;
  }
  if (EndpointIndex == MAX_ENDPOINT) {
    return EFI_INVALID_PARAMETER;
  }
  MaxPacketLength = PeiUsbDev->EndpointDesc[EndpointIndex]->MaxPacketSize;
  if ((PeiUsbDev->DataToggle & (1 << EndpointIndex)) != 0) {
    DataToggle = 1;
  } else {
    DataToggle = 0;
  }
  OldToggle = DataToggle;
  Status = UsbHcPpi->SyncInterruptTransfer (
                       PeiServices,
                       UsbHcPpi,
                       PeiUsbDev->DeviceAddress,
                       DeviceEndpoint,
                       PeiUsbDev->DeviceSpeed,
                       MaxPacketLength,
                       Data,
                       DataLength,
                       &DataToggle,
                       Timeout,
                       &PeiUsbDev->Translator,
                       &TransferResult
                       );
  if (OldToggle != DataToggle) {
    PeiUsbDev->DataToggle = (UINT8)(PeiUsbDev->DataToggle ^ (1 << EndpointIndex));
  }
  return Status;
}

/**

  Retrieves the interface Descriptor for that controller.

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  Indicates the calling context.
  @param  InterfaceDescriptor   A pointer to the caller allocated USB interface
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
PeiUsbGetInterfaceDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB3_IO_PPI                   *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR      **InterfaceDescriptor
  )
{
  PEI_USB_DEVICE  *PeiUsbDev;
  PeiUsbDev             = PEI_USB_DEVICE_FROM_THIS (This);
  *InterfaceDescriptor  = PeiUsbDev->InterfaceDesc;
  return EFI_SUCCESS;
}

/**
 
  Retrieves the endpoint Descriptor for a given endpoint.
 
  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  Indicates the calling context.
  @param  EndpointIndex         Indicates which endpoint descriptor to retrieve.
                                The valid range is 0..15.
  @param  EndpointDescriptor    A pointer to the caller allocated USB Endpoint
                                Descriptor of a USB controller.
 
  @retval EFI_SUCCESS           The endpoint descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER EndpointIndex is not valid.
                                EndpointDescriptor is NULL.
  @retval EFI_NOT_FOUND         The endpoint descriptor cannot be found.
                                The device may not be correctly configured.
 
**/
EFI_STATUS
EFIAPI
PeiUsbGetEndpointDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB3_IO_PPI                   *This,
  IN  UINT8                             EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR       **EndpointDescriptor
  )
{
  PEI_USB_DEVICE  *PeiUsbDev;
  PeiUsbDev = PEI_USB_DEVICE_FROM_THIS (This);
  if (EndpointIndex >= PeiUsbDev->InterfaceDesc->NumEndpoints) {
    return EFI_NOT_FOUND;
  }
  *EndpointDescriptor = PeiUsbDev->EndpointDesc[EndpointIndex];
  return EFI_SUCCESS;
}

/**

  Pei Usb PortReset

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbPortReset (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_USB3_IO_PPI                    *This
  )
{
  PEI_USB_DEVICE  *PeiUsbDev;
  EFI_STATUS      Status;
  UINT8           Address;

  PeiUsbDev = PEI_USB_DEVICE_FROM_THIS (This);
  ResetRootPort (
    PeiUsbDev,
    PeiUsbDev->DeviceAddress
    );
  //
  // Set address
  //
  Address                   = PeiUsbDev->DeviceAddress;
  PeiUsbDev->DeviceAddress  = 0;
  Status = PeiUsbSetDeviceAddress (
             PeiServices,
             This,
             Address
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  PeiUsbDev->DeviceAddress = Address;
  //
  // Set default configuration
  //
  Status = PeiUsbSetConfiguration (
             PeiServices,
             This
             );
  return Status;
}

/**

  Clear Endpoint Halt

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  EndpointAddress       Endpoint Address

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbClearEndpointHalt (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *This,
  IN UINT8                    EndpointAddress
  )
{
  EFI_STATUS                  Status;
  PEI_USB_DEVICE              *PeiUsbDev;
  EFI_USB_ENDPOINT_DESCRIPTOR *EndpointDescriptor;
  UINT8                       EndpointIndex;

  EndpointIndex = 0;
  PeiUsbDev     = PEI_USB_DEVICE_FROM_THIS (This);

  while (EndpointIndex < MAX_ENDPOINT) {
    Status = PeiUsbGetEndpointDescriptor (PeiServices, This, EndpointIndex, &EndpointDescriptor);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    if (EndpointDescriptor->EndpointAddress == EndpointAddress) {
      break;
    }

    EndpointIndex++;
  }

  if (EndpointIndex == MAX_ENDPOINT) {
    return EFI_INVALID_PARAMETER;
  }

  Status = PeiUsbClearDeviceFeature (
             PeiServices,
             This,
             USB_TARGET_ENDPOINT,
             USB_FEATURE_ENDPOINT_HALT,
             EndpointAddress
             );

  //
  // set data toggle to zero.
  //
  if ((PeiUsbDev->DataToggle & (1 << EndpointIndex)) != 0) {
    PeiUsbDev->DataToggle = (UINT8) (PeiUsbDev->DataToggle ^ (1 << EndpointIndex));
  }

  return Status;
}