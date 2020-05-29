/** @file
  USB I/O Abstraction Driver

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbBus.h"
#include "Hub.h"
#include "UsbHelper.h"

//
// USB I/O Support Function Prototypes
//
STATIC
EFI_STATUS
EFIAPI
UsbControlTransfer (
  IN       EFI_USB_IO_PROTOCOL        *This,
  IN       EFI_USB_DEVICE_REQUEST     *Request,
  IN       EFI_USB_DATA_DIRECTION     Direction,
  IN       UINT32                     Timeout,
  IN OUT   VOID                       *Data, OPTIONAL
  IN       UINTN                      DataLength, OPTIONAL
  OUT      UINT32                     *Status
  );

STATIC
EFI_STATUS
EFIAPI
UsbBulkTransfer (
  IN       EFI_USB_IO_PROTOCOL     *This,
  IN       UINT8                   DeviceEndpoint,
  IN OUT   VOID                    *Data,
  IN OUT   UINTN                   *DataLength,
  IN       UINTN                   Timeout,
  OUT      UINT32                  *Status
  );

STATIC
EFI_STATUS
EFIAPI
UsbAsyncInterruptTransfer (
  IN EFI_USB_IO_PROTOCOL                 * This,
  IN UINT8                               DeviceEndpoint,
  IN BOOLEAN                             IsNewTransfer,
  IN UINTN                               PollingInterval, OPTIONAL
  IN UINTN                               DataLength, OPTIONAL
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK     InterruptCallBack, OPTIONAL
  IN VOID                                *Context OPTIONAL
  );

STATIC
EFI_STATUS
EFIAPI
UsbSyncInterruptTransfer (
  IN       EFI_USB_IO_PROTOCOL     *This,
  IN       UINT8                   DeviceEndpoint,
  IN OUT   VOID                    *Data,
  IN OUT   UINTN                   *DataLength,
  IN       UINTN                   Timeout,
  OUT      UINT32                  *Status
  );

STATIC
EFI_STATUS
EFIAPI
UsbIsochronousTransfer (
  IN       EFI_USB_IO_PROTOCOL     *This,
  IN       UINT8                   DeviceEndpoint,
  IN OUT   VOID                    *Data,
  IN       UINTN                   DataLength,
  OUT      UINT32                  *Status
  );

STATIC
EFI_STATUS
EFIAPI
UsbAsyncIsochronousTransfer (
  IN        EFI_USB_IO_PROTOCOL                 * This,
  IN        UINT8                               DeviceEndpoint,
  IN OUT    VOID                                *Data,
  IN        UINTN                               DataLength,
  IN        EFI_ASYNC_USB_TRANSFER_CALLBACK     IsochronousCallBack,
  IN        VOID                                *Context OPTIONAL
  );

extern
EFI_STATUS
EFIAPI
UsbPortReset (
  IN EFI_USB_IO_PROTOCOL     *This
  );

STATIC
EFI_STATUS
EFIAPI
UsbGetDeviceDescriptor (
  IN  EFI_USB_IO_PROTOCOL           *This,
  OUT EFI_USB_DEVICE_DESCRIPTOR     *DeviceDescriptor
  );

STATIC
EFI_STATUS
EFIAPI
UsbGetActiveConfigDescriptor (
  IN  EFI_USB_IO_PROTOCOL           *This,
  OUT EFI_USB_CONFIG_DESCRIPTOR     *ConfigurationDescriptor
  );

STATIC
EFI_STATUS
EFIAPI
UsbGetInterfaceDescriptor (
  IN  EFI_USB_IO_PROTOCOL              *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR     *InterfaceDescriptor
  );

STATIC
EFI_STATUS
EFIAPI
UsbGetEndpointDescriptor (
  IN  EFI_USB_IO_PROTOCOL             *This,
  IN  UINT8                           EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR     *EndpointDescriptor
  );

STATIC
EFI_STATUS
EFIAPI
UsbGetStringDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *This,
  IN  UINT16                  LangID,
  IN  UINT8                   StringIndex,
  OUT CHAR16                  **String
  );

STATIC
EFI_STATUS
EFIAPI
UsbGetSupportedLanguages (
  IN  EFI_USB_IO_PROTOCOL      *This,
  OUT UINT16                   **LangIDTable,
  OUT UINT16                   *TableSize
  );

//
// USB I/O Interface structure
//
EFI_USB_IO_PROTOCOL  mUsbIoInterface = {
  UsbControlTransfer,
  UsbBulkTransfer,
  UsbAsyncInterruptTransfer,
  UsbSyncInterruptTransfer,
  UsbIsochronousTransfer,
  UsbAsyncIsochronousTransfer,
  UsbGetDeviceDescriptor,
  UsbGetActiveConfigDescriptor,
  UsbGetInterfaceDescriptor,
  UsbGetEndpointDescriptor,
  UsbGetStringDescriptor,
  UsbGetSupportedLanguages,
  UsbPortReset
};

/**

  Initialize the instance of UsbIo controller

  @param  UsbIoController       A pointer to controller structure of UsbIo

**/
VOID
InitializeUsbIoInstance (
  IN USB_IO_CONTROLLER_DEVICE     *UsbIoController
  )
{
  //
  // Copy EFI_USB_IO protocol instance
  //
  CopyMem (
    &UsbIoController->UsbIo,
    &mPrivate->UsbIoInterface,
    sizeof (EFI_USB_IO_PROTOCOL)
    );
}

/**

  This function is used to manage a USB device with a control transfer pipe.

  @param  This                  Indicates calling context.
  @param  Request               A pointer to the USB device request that will be sent to
                                the USB device.
  @param  Direction             Indicates the data direction.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  Timeout               Indicates the transfer should be completed within this time
                                frame.
  @param  DataLength            The size, in bytes, of the data buffer specified by Data.
  @param  Status                A pointer to the result of the USB transfer.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES
  @retval EFI_TIMEOUT
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
EFIAPI
UsbControlTransfer (
  IN       EFI_USB_IO_PROTOCOL        *This,
  IN       EFI_USB_DEVICE_REQUEST     *Request,
  IN       EFI_USB_DATA_DIRECTION     Direction,
  IN       UINT32                     Timeout,
  IN OUT   VOID                       *Data, OPTIONAL
  IN       UINTN                      DataLength, OPTIONAL
  OUT      UINT32                     *Status
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  EFI_STATUS                RetStatus;
  USB_IO_DEVICE             *UsbIoDev;
  UINTN                     MaxPacketLength;
  UINT32                    TransferResult;
  BOOLEAN                   Disconnected;
  EFI_USB_HID_DESCRIPTOR    *HidDescriptor;
  //
  // Parameters Checking
  //
  if (Status == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // leave the HostController's ControlTransfer
  // to perform other parameters checking
  //
  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  //
  // Check is device going to be removed
  //
  if (UsbIoController->Inactive) {
    *Status = EFI_USB_ERR_NOTEXECUTE;
    return EFI_NOT_FOUND;
  }
  //
  // The definition of MaxPacketSize0 has changed by USB 3.0 spec
  //
  MaxPacketLength = (UsbIoDev->DeviceDescriptor.BcdUSB == 0x300) ? (1 << UsbIoDev->DeviceDescriptor.MaxPacketSize0) : UsbIoDev->DeviceDescriptor.MaxPacketSize0;

  if (Request->Request     == USB_DEV_CLEAR_FEATURE && 
      Request->RequestType == 0x02                  && 
      Request->Value       == USB_FEATURE_ENDPOINT_HALT) {
    //
    // Reduce the remove delay time for system response
    //
    IsDeviceDisconnected (UsbIoController, &Disconnected);
    if (!EFI_ERROR (Status) && Disconnected == TRUE) {
      DEBUG ((gUSBErrorLevel, "Device is disconnected when trying reset\n"));
      return EFI_NOT_FOUND;
    }
  }
  //
  // Enter critical section to privent SMI interfere the Usb transport
  //
  mPrivate->UsbCore->EnterCriticalSection();
  //
  // using HostController's ControlTransfer to complete the request
  //
  RetStatus = UsbIoDev->BusController->Usb3HCInterface->ControlTransfer (
                                                          UsbIoDev->BusController->Usb3HCInterface,
                                                          UsbIoDev->DeviceAddress,
                                                          UsbIoDev->DeviceSpeed,
                                                          MaxPacketLength,
                                                          Request,
                                                          Direction,
                                                          Data,
                                                          &DataLength,
                                                          (UINTN) Timeout,
                                                          &UsbIoDev->Translator,
                                                          &TransferResult
                                                          );

  *Status = TransferResult;
  //
  // Special process after control transfer for clear feature command
  //
  if (Request->Request     == USB_DEV_CLEAR_FEATURE &&
      Request->RequestType == 0x02                  &&
      Request->Value       == USB_FEATURE_ENDPOINT_HALT) {
    if (RetStatus == EFI_SUCCESS) {
      //
      // This is a UsbClearEndpointHalt request
      // Need to clear data toggle
      // Request.Index == EndpointAddress
      //
      SetDataToggleBit (
        This,
        (UINT8) Request->Index,
        0
        );
    } else {
      //
      // Set default configuration again to workaround some firmware failure
      //
      UsbSetDefaultConfiguration (UsbIoDev);
    }
  }
  //
  // Special process after control transfer for set interface command
  //
  if (RetStatus            == EFI_SUCCESS                    &&
      Request->Request     == USB_DEV_SET_INTERFACE          &&
      Request->RequestType == USB_DEV_SET_INTERFACE_REQ_TYPE &&
      Request->Index       == UsbIoController->InterfaceNumber) {
    //
    // This is a UsbSetInterface request, need to switch to appropriate AlternateSetting
    //
    UsbIoController->AlternateSetting = (UINT8)Request->Value;
    //
    // Register the HID descriptor to UsbCore if available
    //
    if ((HidDescriptor = GetHidDescriptor (This)) != NULL) {
      mPrivate->UsbCore->RegisterHidDescriptor (This, HidDescriptor);
    }
  }
  //
  // Leave critical section to processing DPC(SMI pending procedure)
  //
  mPrivate->UsbCore->LeaveCriticalSection();
  return RetStatus;
}

/**

  This function is used to manage a USB device with the bulk transfer pipe.

  @param  This                  Indicates calling context.
  @param  DeviceEndpoint        The destination USB device endpoint to which the device
                                request is being sent.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            On input, the size, in bytes, of the data buffer
                                specified by Data.  On output, the number of bytes that
                                were actually transferred.
  @param  Timeout               Indicates the transfer should be completed within this
                                time frame.
  @param  Status                This parameter indicates the USB transfer status.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES
  @retval EFI_TIMEOUT
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
EFIAPI
UsbBulkTransfer (
  IN       EFI_USB_IO_PROTOCOL     *This,
  IN       UINT8                   DeviceEndpoint,
  IN OUT   VOID                    *Data,
  IN OUT   UINTN                   *DataLength,
  IN       UINTN                   Timeout,
  OUT      UINT32                  *Status
  )
{
  USB_IO_DEVICE             *UsbIoDev;
  UINTN                     MaxPacketLength;
  UINT8                     DataToggle;
  UINT8                     OldToggle;
  EFI_STATUS                RetStatus;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  ENDPOINT_DESC_LIST_ENTRY  *EndPointListEntry;
  UINT8                     DataBuffersNumber;
  UINT32                    TransferResult;

  DataBuffersNumber = 1;
  UsbIoController   = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev          = UsbIoController->UsbDevice;
  //
  // UsbHCInterface  = UsbIoDev->BusController->UsbHCInterface;
  //
  // Parameters Checking
  //
  if ((DeviceEndpoint & 0x7F) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DeviceEndpoint & 0x7F) > 15) {
    return EFI_INVALID_PARAMETER;
  }

  if (Status == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EndPointListEntry = FindEndPointListEntry (
                        This,
                        DeviceEndpoint
                        );

  if (EndPointListEntry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((EndPointListEntry->EndpointDescriptor.Attributes & 0x03) != 0x02) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check is device going to be removed
  //
  if (UsbIoController->Inactive) {
    *Status = EFI_USB_ERR_NOTEXECUTE;
    return EFI_NOT_FOUND;
  }
  MaxPacketLength = EndPointListEntry->EndpointDescriptor.MaxPacketSize;
  DataToggle      = (UINT8)EndPointListEntry->Toggle;
  OldToggle       = DataToggle;
  //
  // Enter critical section to privent SMI interfere the Usb transport
  //
  mPrivate->UsbCore->EnterCriticalSection();
  //
  // using HostController's BulkTransfer to complete the request
  //
  RetStatus = UsbIoDev->BusController->Usb3HCInterface->BulkTransfer (
                                                          UsbIoDev->BusController->Usb3HCInterface,
                                                          UsbIoDev->DeviceAddress,
                                                          DeviceEndpoint,
                                                          UsbIoDev->DeviceSpeed,
                                                          MaxPacketLength,
                                                          DataBuffersNumber,
                                                          &Data,
                                                          DataLength,
                                                          &DataToggle,
                                                          Timeout,
                                                          &UsbIoDev->Translator,
                                                          NULL,
                                                          NULL,
                                                          &TransferResult
                                                          );
  if (OldToggle != DataToggle) {
    //
    // Write the toggle back
    //
    EndPointListEntry->Toggle = DataToggle;
  }
  *Status = TransferResult;
  //
  // Leave critical section to processing DPC(SMI pending procedure)
  //
  mPrivate->UsbCore->LeaveCriticalSection();
  return RetStatus;
}

/*++
 
  Usb Sync Interrupt Transfer
 
  @param  This                  Indicates calling context.
  @param  DeviceEndpoint        The destination USB device endpoint to which the device
                                request is being sent.
  @param  Data                  A pointer to the buffer of data that will be transmitted
                                to USB device or received from USB device.
  @param  DataLength            On input, the size, in bytes, of the data buffer
                                specified by Data.  On output, the number of bytes that
                                were actually transferred.
  @param  Timeout               Indicates the transfer should be completed within this
                                time frame.
  @param  Status                This parameter indicates the USB transfer status.
 
  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES
  @retval EFI_TIMEOUT
  @retval EFI_DEVICE_ERROR

--*/
STATIC
EFI_STATUS
EFIAPI
UsbSyncInterruptTransfer (
  IN       EFI_USB_IO_PROTOCOL     *This,
  IN       UINT8                   DeviceEndpoint,
  IN OUT   VOID                    *Data,
  IN OUT   UINTN                   *DataLength,
  IN       UINTN                   Timeout,
  OUT      UINT32                  *Status
  )
{
  USB_IO_DEVICE             *UsbIoDev;
  UINTN                     MaxPacketLength;
  UINT8                     DataToggle;
  UINT8                     OldToggle;
  EFI_STATUS                RetStatus;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  ENDPOINT_DESC_LIST_ENTRY  *EndPointListEntry;
  //
  // Parameters Checking
  //
  if ((DeviceEndpoint & 0x7F) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DeviceEndpoint & 0x7F) > 15) {
    return EFI_INVALID_PARAMETER;
  }

  if (Status == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EndPointListEntry = FindEndPointListEntry (
                        This,
                        DeviceEndpoint
                        );

  if (EndPointListEntry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((EndPointListEntry->EndpointDescriptor.Attributes & 0x03) != 0x03) {
    return EFI_INVALID_PARAMETER;
  }
  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  //
  // Check is device going to be removed
  //
  if (UsbIoController->Inactive) {
    *Status = EFI_USB_ERR_NOTEXECUTE;
    return EFI_NOT_FOUND;
  }
  UsbIoDev        = UsbIoController->UsbDevice;
  MaxPacketLength = EndPointListEntry->EndpointDescriptor.MaxPacketSize;
  DataToggle      = (UINT8)EndPointListEntry->Toggle;
  OldToggle       = DataToggle;
  //
  // Enter critical section to privent SMI interfere the Usb transport
  //
  mPrivate->UsbCore->EnterCriticalSection();
  //
  // using HostController's SyncInterruptTransfer to complete the request
  //
  RetStatus = UsbIoDev->BusController->Usb3HCInterface->InterruptTransfer (
                                                          UsbIoDev->BusController->Usb3HCInterface,
                                                          UsbIoDev->DeviceAddress,
                                                          DeviceEndpoint,
                                                          UsbIoDev->DeviceSpeed,
                                                          MaxPacketLength,
                                                          Data,
                                                          DataLength,
                                                          &DataToggle,
                                                          Timeout,
                                                          &UsbIoDev->Translator,
                                                          NULL,
                                                          NULL,
                                                          0,
                                                          Status
                                                          );
  if (OldToggle != DataToggle) {
    //
    // Write the toggle back
    //
    EndPointListEntry->Toggle = DataToggle;
  }
  //
  // Leave critical section to processing DPC(SMI pending procedure)
  //
  mPrivate->UsbCore->LeaveCriticalSection();
  return RetStatus;
}

/*++

  Usb Async Interrput Transfer

  @param  This                  Indicates calling context.
  @param  DeviceEndpoint        The destination USB device endpoint to which the
                                device request is being sent.
  @param  IsNewTransfer         If TRUE, a new transfer will be submitted to USB
                                controller.  If FALSE,  the interrupt transfer is
                                deleted from the device's interrupt transfer queue.
  @param  PollingInterval       Indicates the periodic rate, in milliseconds, that
                                the transfer is to be executed.
  @param  DataLength            Specifies the length, in bytes, of the data to be
                                received from the USB device.
  @param  InterruptCallBack     The Callback function.  This function is called if
                                the asynchronous interrupt transfer is completed.
  @param  Context               Passed to InterruptCallback 
  
  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES

--*/
STATIC
EFI_STATUS
EFIAPI
UsbAsyncInterruptTransfer (
  IN EFI_USB_IO_PROTOCOL                 * This,
  IN UINT8                               DeviceEndpoint,
  IN BOOLEAN                             IsNewTransfer,
  IN UINTN                               PollingInterval, OPTIONAL
  IN UINTN                               DataLength, OPTIONAL
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK     InterruptCallBack, OPTIONAL
  IN VOID                                *Context OPTIONAL
  )
{
  USB_IO_DEVICE             *UsbIoDev;
  UINTN                     MaxPacketLength;
  UINT8                     DataToggle;
  EFI_STATUS                RetStatus;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  ENDPOINT_DESC_LIST_ENTRY  *EndPointListEntry;
  //
  // Check endpoint
  //
  if ((DeviceEndpoint & 0x7F) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DeviceEndpoint & 0x7F) > 15) {
    return EFI_INVALID_PARAMETER;
  }

  EndPointListEntry = FindEndPointListEntry (
                        This,
                        DeviceEndpoint
                        );

  if (EndPointListEntry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((EndPointListEntry->EndpointDescriptor.Attributes & 0x03) != 0x03) {
    return EFI_INVALID_PARAMETER;
  }

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  //
  // Enter critical section to privent SMI interfere the Usb transport
  //
  mPrivate->UsbCore->EnterCriticalSection();
  //
  // UsbHCInterface  = UsbIoDev->BusController->UsbHCInterface;
  //
  if (!IsNewTransfer) {
    //
    // Delete this transfer
    //
    RetStatus = UsbIoDev->BusController->Usb3HCInterface->CancelIo (
                                                            UsbIoDev->BusController->Usb3HCInterface,
                                                            UsbIoDev->DeviceAddress,
                                                            DeviceEndpoint,
                                                            PollingInterval,
                                                            &DataToggle
                                                            );
    if (RetStatus == EFI_SUCCESS) {
      //
      // We need to store the toggle value
      //
      EndPointListEntry->Toggle = DataToggle;
    }
  } else {
    MaxPacketLength = EndPointListEntry->EndpointDescriptor.MaxPacketSize;
    DataToggle      = (UINT8)EndPointListEntry->Toggle;
    RetStatus = UsbIoDev->BusController->Usb3HCInterface->InterruptTransfer (
                                                            UsbIoDev->BusController->Usb3HCInterface,
                                                            UsbIoDev->DeviceAddress,
                                                            DeviceEndpoint,
                                                            UsbIoDev->DeviceSpeed,
                                                            MaxPacketLength,
                                                            NULL,
                                                            &DataLength,
                                                            &DataToggle,
                                                            0,
                                                            &UsbIoDev->Translator,
                                                            InterruptCallBack,
                                                            Context,
                                                            PollingInterval,
                                                            NULL
                                                            );
  }
  //
  // Leave critical section to processing DPC(SMI pending procedure)
  //
  mPrivate->UsbCore->LeaveCriticalSection();
  return RetStatus;
}

/**

  Usb Isochronous Transfer

  @param  This                  Indicates calling context.
  @param  DeviceEndpoint        The destination USB device endpoint to which the
                                device request is being sent.
  @param  Data                  A pointer to the buffer of data that will be
                                transmitted to USB device or received from USB device.
  @param  DataLength            The size, in bytes, of the data buffer specified by
                                Data.
  @param  Status                This parameter indicates the USB transfer status.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES
  @retval EFI_TIMEOUT
  @retval EFI_DEVICE_ERROR
  @retval EFI_UNSUPPORTED

**/
STATIC
EFI_STATUS
EFIAPI
UsbIsochronousTransfer (
  IN       EFI_USB_IO_PROTOCOL     *This,
  IN       UINT8                   DeviceEndpoint,
  IN OUT   VOID                    *Data,
  IN       UINTN                   DataLength,
  OUT      UINT32                  *Status
  )
{
  //
  // Currently we don't support this transfer
  //
  return EFI_UNSUPPORTED;
}

/*++

  Usb Async Isochronous Transfer

  @param  This                  EFI_USB_IO_PROTOCOL
  @param  DeviceEndpoint        DeviceEndpoint number
  @param  Data                  Data to transfer
  @param  DataLength            DataLength
  @param  IsochronousCallBack   Isochronous CallBack function
  @param  Context               Passed to IsochronousCallBack function

  @retval EFI_UNSUPPORTED       Unsupported now

--*/
STATIC
EFI_STATUS
EFIAPI
UsbAsyncIsochronousTransfer (
  IN        EFI_USB_IO_PROTOCOL                 * This,
  IN        UINT8                               DeviceEndpoint,
  IN OUT    VOID                                *Data,
  IN        UINTN                               DataLength,
  IN        EFI_ASYNC_USB_TRANSFER_CALLBACK     IsochronousCallBack,
  IN        VOID                                *Context OPTIONAL
  )
{
  //
  // Currently we don't support this transfer
  //
  return EFI_UNSUPPORTED;
}

/**

  Retrieves the USB Device Descriptor.

  @param  This                  Indicates the calling context.
  @param  DeviceDescriptor      A pointer to the caller allocated USB Device
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetDeviceDescriptor (
  IN  EFI_USB_IO_PROTOCOL           *This,
  OUT EFI_USB_DEVICE_DESCRIPTOR     *DeviceDescriptor
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;

  //
  // This function just wrapps UsbGetDeviceDescriptor.
  //
  if (DeviceDescriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;

  if (!UsbIoDev->IsConfigured) {
    return EFI_NOT_FOUND;
  }

  CopyMem (
    DeviceDescriptor,
    &UsbIoDev->DeviceDescriptor,
    sizeof (EFI_USB_DEVICE_DESCRIPTOR)
    );

  return EFI_SUCCESS;
}

/**

  Retrieves the current USB configuration Descriptor.

  @param  This                          Indicates the calling context.
  @param  ConfigurationDescriptor       A pointer to the caller allocated USB active
                                        Configuration Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetActiveConfigDescriptor (
  IN  EFI_USB_IO_PROTOCOL           *This,
  OUT EFI_USB_CONFIG_DESCRIPTOR     *ConfigurationDescriptor
  )
{
  USB_IO_DEVICE             *UsbIoDev;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;

  //
  // This function just wrapps UsbGetActiveConfigDescriptor.
  //
  if (ConfigurationDescriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;

  if (!UsbIoDev->IsConfigured) {
    return EFI_NOT_FOUND;
  }

  CopyMem (
    ConfigurationDescriptor,
    &(UsbIoDev->ActiveConfig->CongfigDescriptor),
    sizeof (EFI_USB_CONFIG_DESCRIPTOR)
    );

  return EFI_SUCCESS;
}

/**

  Retrieves the interface Descriptor for that controller.

  @param  This                  Indicates the calling context.
  @param  InterfaceDescriptor   A pointer to the caller allocated USB interface
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetInterfaceDescriptor (
  IN  EFI_USB_IO_PROTOCOL              *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR     *InterfaceDescriptor
  )
{
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  if (InterfaceDescriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  InterfaceListEntry = FindInterfaceListEntry (This);

  if (InterfaceListEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  CopyMem (
    InterfaceDescriptor,
    &(InterfaceListEntry->InterfaceDescriptor),
    sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    );

  return EFI_SUCCESS;
}

/**
 
  Retrieves the endpoint Descriptor for a given endpoint.
 
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
STATIC
EFI_STATUS
EFIAPI
UsbGetEndpointDescriptor (
  IN  EFI_USB_IO_PROTOCOL             *This,
  IN  UINT8                           EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR     *EndpointDescriptor
  )
{
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;
  LIST_ENTRY                *EndpointListHead;
  ENDPOINT_DESC_LIST_ENTRY  *EndpointListEntry;

  if (EndpointDescriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (EndpointIndex > 15) {
    return EFI_INVALID_PARAMETER;
  }

  InterfaceListEntry = FindInterfaceListEntry (This);

  if (InterfaceListEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  EndpointListHead  = (LIST_ENTRY *) (&InterfaceListEntry->EndpointDescListHead);
  EndpointListEntry = (ENDPOINT_DESC_LIST_ENTRY *) (EndpointListHead->ForwardLink);

  if (EndpointIndex >= InterfaceListEntry->InterfaceDescriptor.NumEndpoints) {
    return EFI_NOT_FOUND;
  }
  //
  // Loop all endpoint descriptor to get match one.
  //
  while (EndpointIndex != 0) {
    EndpointListEntry = (ENDPOINT_DESC_LIST_ENTRY *) (EndpointListEntry->Link.ForwardLink);
    EndpointIndex--;
  }

  CopyMem (
    EndpointDescriptor,
    &EndpointListEntry->EndpointDescriptor,
    sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
    );

  return EFI_SUCCESS;
}

/**

  Get all the languages that the USB device supports

  @param  This                  Indicates the calling context.
  @param  LangIDTable           Language ID for the string the caller wants to get.
  @param  TableSize             The size, in bytes, of the table LangIDTable.

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetSupportedLanguages (
  IN  EFI_USB_IO_PROTOCOL     *This,
  OUT UINT16                  **LangIDTable,
  OUT UINT16                  *TableSize
  )
{
  USB_IO_DEVICE             *UsbIoDev;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  UINTN                     Index;
  BOOLEAN                   Found;
  EFI_STATUS                Status;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  //
  // Get the LangID table if it is not been configured
  //
  if (!UsbIoDev->LangIDConfigured) {
    //
    // Get all string table if applicable
    //
    Status = UsbGetStringtable (UsbIoDev);
    //
    // Set the LangIDConfigured as TRUE even we get fail from UsbGetStringtable
    //
    UsbIoDev->LangIDConfigured = TRUE;
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  }
  if (UsbIoDev->LangID[0] == 0) {
    return EFI_NOT_FOUND;
  }
  Found           = FALSE;
  Index           = 0;
  //
  // Loop language table
  //
  while (UsbIoDev->LangID[Index]) {
    Found = TRUE;
    Index++;
  }

  if (!Found) {
    return EFI_NOT_FOUND;
  }

  *LangIDTable  = UsbIoDev->LangID;
  *TableSize    = (UINT16) Index;

  return EFI_SUCCESS;
}

/**

  Get a given string descriptor

  @param  This                  Indicates the calling context.
  @param  LangID                The Language ID for the string being retrieved.
  @param  StringIndex           The ID of the string being retrieved.
  @param  String                A pointer to a buffer allocated by this function
                                with AllocatePool() to store the string.  If this
                                function returns EFI_SUCCESS, it stores the string
                                the caller wants to get.  The caller should release
                                the string buffer with FreePool() after the string
                                is not used any more.
  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND
  @retval EFI_OUT_OF_RESOURCES
  @retval EFI_UNSUPPORTED

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetStringDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *This,
  IN  UINT16                  LangID,
  IN  UINT8                   StringIndex,
  OUT CHAR16                  **String
  )
{
  UINT32                    Result;
  EFI_STATUS                Status;
  EFI_USB_STRING_DESCRIPTOR *StrDescriptor;
  UINT8                     *Buffer;
  UINT16                    TempBuffer;
  USB_IO_DEVICE             *UsbIoDev;
  UINT8                     Index;
  BOOLEAN                   Found;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  UINT8                     Length;

  if (StringIndex == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Search LanguageID, check if it is supported by this device
  //
  if (LangID == 0) {
    return EFI_NOT_FOUND;
  }

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  UsbCore         = mPrivate->UsbCore;
  //
  // Get the LangID table if it is not been configured
  //
  if (!UsbIoDev->LangIDConfigured) {
    //
    // Get all string table if applicable
    //
    Status = UsbGetStringtable (UsbIoDev);
    //
    // Set the LangIDConfigured as TRUE even we get fail from UsbGetStringtable
    //
    UsbIoDev->LangIDConfigured = TRUE;
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  }
  if (UsbIoDev->LangID[0] == 0) {
    return EFI_NOT_FOUND;
  }
  Found           = FALSE;
  Index           = 0;
  while (UsbIoDev->LangID[Index]) {
    if (UsbIoDev->LangID[Index] == LangID) {
      Found = TRUE;
      break;
    }

    Index++;
  }

  if (!Found) {
    return EFI_NOT_FOUND;
  }
  //
  // Get String Length
  //
  Status = UsbCore->UsbGetString (
                      This,
                      LangID,
                      StringIndex,
                      &TempBuffer,
                      2,
                      &Result
                      );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  StrDescriptor = (EFI_USB_STRING_DESCRIPTOR *) &TempBuffer;

  if (StrDescriptor->Length == 0) {
    return EFI_UNSUPPORTED;
  }
  //
  // Use EFI AllocatePool to allocate memory, don't use it during runtime
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  StrDescriptor->Length,
                  (VOID **)&Buffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (Buffer, StrDescriptor->Length);
  
  Status = UsbCore->UsbGetString (
                      This,
                      LangID,
                      StringIndex,
                      Buffer,
                      StrDescriptor->Length,
                      &Result
                      );

  if (EFI_ERROR (Status)) {
    gBS->FreePool(Buffer);
    return EFI_NOT_FOUND;
  }

  StrDescriptor = (EFI_USB_STRING_DESCRIPTOR *) Buffer;

  for (Index = 0, Length = StrDescriptor->Length - 2; Index < Length; Index += 2) {
    *(UINT16*)(Buffer + Index) = *(UINT16*)(Buffer + Index + 2);
  }
  *(UINT16*)(Buffer + Index) = 0;
  *String = (CHAR16*)Buffer;
  return EFI_SUCCESS;
}
