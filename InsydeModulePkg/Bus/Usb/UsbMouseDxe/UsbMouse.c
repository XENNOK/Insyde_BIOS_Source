/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbMouse.h"
#include <PostCode.h>

//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
UsbMouseDriverBindingEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
EFIAPI
UsbMouseDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbMouseDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbMouseDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI
UsbMouseUsbBindingSupported (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );

EFI_STATUS
EFIAPI
UsbMouseUsbBindingStart (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  );

EFI_STATUS
EFIAPI
UsbMouseUsbBindingStop (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  );

EFI_DRIVER_BINDING_PROTOCOL gUsbMouseDriverBinding = {
  UsbMouseDriverBindingSupported,
  UsbMouseDriverBindingStart,
  UsbMouseDriverBindingStop,
  0x10,
  NULL,
  NULL
};

//
// helper functions
//
STATIC
BOOLEAN
IsUsbMouse (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  EFI_USB_CORE_PROTOCOL   *UsbCore
  );

STATIC
EFI_STATUS
InitializeUsbMouseDevice (
  IN  USB_MOUSE_DEV           *UsbMouseDevice
  );

STATIC
VOID
EFIAPI
UsbMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

//
// Mouse interrupt handler
//
STATIC
EFI_STATUS
EFIAPI
OnMouseInterruptComplete (
  IN  VOID        *Data,
  IN  UINTN       DataLength,
  IN  VOID        *Context,
  IN  UINT32      Result
  );

//
// Mouse Protocol
//
STATIC
EFI_STATUS
EFIAPI
GetMouseState (
  IN   EFI_SIMPLE_POINTER_PROTOCOL  *This,
  OUT  EFI_SIMPLE_POINTER_STATE     *MouseState
  );

STATIC
EFI_STATUS
EFIAPI
UsbMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  );

typedef struct {
  UINT16        IdVendor;
  UINT16        IdProduct;
} MOUSE_ID_TABLE;

STATIC MOUSE_ID_TABLE IncompatibleBootProtocolMouse[] = {
  { 0x1BCF, 0x0007 },   // KTMS258U
  { 0x0000, 0x0000 }    // End of list
};

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS
  @retval others

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  BOOLEAN                       InSmram;
  //
  // Locate UsbCore protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiUsbCoreProtocolGuid,
                  NULL,
                  (VOID **)&UsbCore
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Register module in DXE/SMM instance
  //
  UsbCore->ModuleRegistration (ImageHandle);
  //
  // Check the phase of instance
  //
  UsbCore->IsInSmm (&InSmram);
  if (!InSmram) {
    //
    // Now in boot service, install protocols
    //
    Status = EfiLibInstallDriverBindingComponentName2 (
               ImageHandle,
               SystemTable,
               &gUsbMouseDriverBinding,
               ImageHandle,
               &gUsbMouseComponentName,
               &gUsbMouseComponentName2
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    //
    // Register UsbBinding for legacy hot plug mechanism
    //
    Status = UsbCore->RegisterUsbBindingProtocol(
                        UsbMouseUsbBindingSupported,
                        UsbMouseUsbBindingStart,
                        UsbMouseUsbBindingStop
                        );
  }
  return Status;
}

/**

  Test to see if this driver supports ControllerHandle. Any ControllerHandle
  that has UsbHcProtocol installed will be supported.

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to test
  @param  RemainingDevicePath   Not used

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_UNSUPPORTED       This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                    OpenStatus;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_STATUS                    Status;
  EFI_DEV_PATH_PTR              Node;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  //
  // Check Device Path
  //
  if (RemainingDevicePath != NULL) {
    Node.DevPath = RemainingDevicePath;
    if (Node.DevPath->Type != MESSAGING_DEVICE_PATH ||
        Node.DevPath->SubType != MSG_USB_CLASS_DP   ||
        DevicePathNodeLength(Node.DevPath) != sizeof(USB_CLASS_DEVICE_PATH) ||
        Node.UsbClass->DeviceClass != CLASS_HID) {
      return EFI_UNSUPPORTED;
    }
  }
  OpenStatus = gBS->OpenProtocol (
                      Controller,
                      &gEfiUsbIoProtocolGuid,
                      (VOID **)&UsbIo,
                      This->DriverBindingHandle,
                      Controller,
                      EFI_OPEN_PROTOCOL_BY_DRIVER
                      );
  if (EFI_ERROR (OpenStatus) && (OpenStatus != EFI_ALREADY_STARTED)) {
    return EFI_UNSUPPORTED;
  }

  if (OpenStatus == EFI_ALREADY_STARTED) {
    return EFI_ALREADY_STARTED;
  }
  
  //
  // Get the device path for CheckIgnoredDevice
  //
  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Setup the data for UsbLegacy
  //
  Status = gBS->LocateProtocol (
                  &gEfiUsbCoreProtocolGuid,
                  NULL,
                  (VOID **)&UsbCore
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 1)
  //
  // Filter out the USB devices which in the UsbIgnoreDevice list
  //
  Status = UsbCore->CheckIgnoredDevice(DevicePath, UsbIo);
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
#endif
  //
  // Use the USB I/O protocol interface to see the Controller is
  // the Mouse controller that can be managed by this driver.
  //
  Status = EFI_SUCCESS;
  if (!IsUsbMouse (UsbIo, UsbCore)) {
    Status = EFI_UNSUPPORTED;
  }

ErrorExit:
  gBS->CloseProtocol (
        Controller,
        &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );
  return Status;
}

/**

  Starting the Usb Bus Driver

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to test
  @param  RemainingDevicePath   Not used

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_UNSUPPORTED       This driver does not support this device.
  @retval EFI_DEVICE_ERROR      This driver cannot be started due to device error
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory resources
  @retval EFI_ALREADY_STARTED   Thios driver has been started

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_USB_IO_PROTOCOL         *UsbIo;
  EFI_USB_CORE_PROTOCOL       *UsbCore;
  EFI_USB_ENDPOINT_DESCRIPTOR EndpointDesc;
  USB_MOUSE_DEV               *UsbMouseDevice;
  UINT8                       EndpointNumber;
  UINT8                       Index;
  UINT8                       EndpointAddr;
  UINT8                       PollingInterval;
  UINT8                       PacketSize;
  BOOLEAN                     EndpointFound;
  USB_DEVICE                  UsbDevice;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  UsbMouseDevice  = NULL;
  UsbCore         = NULL;
  Status          = EFI_SUCCESS;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **)&UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Locate EFI_USB_CORE_PROTOCOL for further use
  //
  Status = gBS->LocateProtocol (
                  &gEfiUsbCoreProtocolGuid,
                  NULL,
                  (VOID **)&UsbCore
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  Status = UsbCore->AllocateBuffer (
                        sizeof (USB_MOUSE_DEV),
                        ALIGNMENT_32,
                        (VOID **)&UsbMouseDevice
                        );
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  UsbMouseDevice->UsbIo               = UsbIo;
  UsbMouseDevice->UsbCore             = UsbCore;
  UsbMouseDevice->Signature           = USB_MOUSE_DEV_SIGNATURE;

  //
  // Get the Device Path Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &UsbMouseDevice->DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  //
  // Report Status Code here since USB mouse will be detected next.
  //
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT)
    );
  
  //
  // Get interface & endpoint descriptor
  //
  UsbIo->UsbGetInterfaceDescriptor (
           UsbIo,
           &UsbMouseDevice->InterfaceDescriptor
           );

  EndpointNumber = UsbMouseDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0, EndpointFound = FALSE; Index < EndpointNumber; Index++) {
    UsbIo->UsbGetEndpointDescriptor (
             UsbIo,
             Index,
             &EndpointDesc
             );

    if ((EndpointDesc.Attributes & 0x03) == 0x03 && (EndpointDesc.EndpointAddress & 0x80)) {
      //
      // We only care interrupt-in endpoint here
      //
      UsbMouseDevice->IntEndpointDescriptor = EndpointDesc;
      EndpointFound = TRUE;
      break;
    }
  }

  if (!EndpointFound) {
    //
    // Report Status Code to indicate that there is no USB mouse
    //
    MouseReportStatusCode (
      UsbMouseDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_DETECTED)
      );
    //
    // No interrupt endpoint, then error
    //
    Status = EFI_UNSUPPORTED;
    goto ErrorExit;
  }

  //
  // Report Status Code here since USB mouse has be detected.
  //
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_DETECTED)
    );

  Status = InitializeUsbMouseDevice (UsbMouseDevice);
  if (EFI_ERROR (Status)) {
    MouseReportStatusCode (
      UsbMouseDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_MOUSE | EFI_P_EC_INTERFACE_ERROR)
      );

    goto ErrorExit;
  }

  UsbMouseDevice->SimplePointerProtocol.GetState  = GetMouseState;
  UsbMouseDevice->SimplePointerProtocol.Reset     = UsbMouseReset;
  UsbMouseDevice->SimplePointerProtocol.Mode      = &UsbMouseDevice->Mode;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  UsbMouseWaitForInput,
                  UsbMouseDevice,
                  &((UsbMouseDevice->SimplePointerProtocol).WaitForInput)
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiSimplePointerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &UsbMouseDevice->SimplePointerProtocol
                  );

  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }
  
  //
  // After Enabling Async Interrupt Transfer on this mouse Device
  // we will be able to get key data from it. Thus this is deemed as
  // the enable action of the mouse
  //
  
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_ENABLE)
    );

  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbMouseDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8) (UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize);

  Status = UsbIo->UsbAsyncInterruptTransfer (
                    UsbIo,
                    EndpointAddr,
                    TRUE,
                    PollingInterval,
                    PacketSize,
                    OnMouseInterruptComplete,
                    UsbMouseDevice
                    );

  if (!EFI_ERROR (Status)) {

    UsbMouseDevice->ControllerNameTable = NULL;
    AddUnicodeString2 (
      LANGUAGE_CODE_ENGLISH_ISO639,
      gUsbMouseComponentName.SupportedLanguages,
      &UsbMouseDevice->ControllerNameTable,
      CONTROLLER_DRIVER_NAME,
      TRUE
      );
    AddUnicodeString2 (
      LANGUAGE_CODE_ENGLISH_RFC4646,
      gUsbMouseComponentName2.SupportedLanguages,
      &UsbMouseDevice->ControllerNameTable,
      CONTROLLER_DRIVER_NAME,
      FALSE
      );
    //
    // Insert UsbIoDevice into UsbCore for UsbLegacy
    //
    UsbDevice.UsbHID.Type     = USB_CORE_USB_HID;
    UsbDevice.UsbHID.UsbIo    = UsbIo;
    UsbDevice.UsbHID.Instance = UsbMouseDevice;
    UsbDevice.UsbHID.SyncLED  = NULL;
    UsbCore->InsertUsbDevice (
               &UsbDevice
               );
    //
    // Setup Smm address convert table for Smm security policy
    //
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_BODY,
               UsbMouseDevice,
               sizeof (USB_MOUSE_DEV)
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &UsbMouseDevice->UsbIo,
               1
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &UsbMouseDevice->UsbCore,
               1
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &UsbMouseDevice->RecoveryPollingHandle,
               1
               );
    return EFI_SUCCESS;
  }

  //
  // If submit error, uninstall that interface
  //
  Status = EFI_DEVICE_ERROR;
  gBS->UninstallProtocolInterface (
        Controller,
        &gEfiSimplePointerProtocolGuid,
        &UsbMouseDevice->SimplePointerProtocol
        );

ErrorExit:
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
          Controller,
          &gEfiUsbIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );

    if (UsbMouseDevice != NULL) {

      if ((UsbMouseDevice->SimplePointerProtocol).WaitForInput != NULL) {
        gBS->CloseEvent ((UsbMouseDevice->SimplePointerProtocol).WaitForInput);
      }
      UsbCore->FreeBuffer (
                 sizeof (USB_MOUSE_DEV),
                 UsbMouseDevice
                 );
    }
  }

  return Status;
}

/**

  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to stop driver on
  @param  NumberOfChildren      Number of Children in the ChildHandleBuffer
  @param  ChildHandleBuffer     List of handles for the children we need to stop.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval others

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  )
{
  EFI_STATUS                  Status;
  USB_MOUSE_DEV               *UsbMouseDevice;
  EFI_SIMPLE_POINTER_PROTOCOL *SimplePointerProtocol;
  EFI_USB_IO_PROTOCOL         *UsbIo;
  EFI_USB_CORE_PROTOCOL       *UsbCore;

  //
  // Get our context back.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID **)&SimplePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  UsbMouseDevice = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (SimplePointerProtocol);
  UsbIo   = UsbMouseDevice->UsbIo;
  UsbCore = UsbMouseDevice->UsbCore;
  //
  // Set the UsbIo to NULL to preventing async interrupt transfer restarted 
  // by recovery handler after deleting the async interrupt transfer
  //
  UsbMouseDevice->UsbIo = NULL;

  gBS->CloseProtocol (
        Controller,
        &gEfiSimplePointerProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );
  //
  // Remove the recovery handler before deleting the async interrupt transfer to preventing
  // async interrupt transfer restarted by recovery handler
  //
  UsbCore->RemovePeriodicTimer(UsbMouseDevice->RecoveryPollingHandle);
  //
  // Uninstall the Asyn Interrupt Transfer from this device
  // will disable the mouse data input from this device
  //
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_DISABLE)
    );

  //
  // Delete Mouse Async Interrupt Transfer
  //
  UsbIo->UsbAsyncInterruptTransfer (
           UsbIo,
           UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
           FALSE,
           UsbMouseDevice->IntEndpointDescriptor.Interval,
           0,
           NULL,
           NULL
           );

  gBS->CloseEvent (UsbMouseDevice->SimplePointerProtocol.WaitForInput);
  
  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &UsbMouseDevice->SimplePointerProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiUsbIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (UsbMouseDevice->ControllerNameTable) {
    FreeUnicodeStringTable (UsbMouseDevice->ControllerNameTable);
  }

  UsbCore->FreeBuffer (
             sizeof (USB_MOUSE_DEV),
             UsbMouseDevice
             );
  //
  // Remove UsbIoDevice from UsbCore for UsbLegacy
  //
  UsbCore->RemoveUsbDevice (
             UsbIo
             );
  //
  // Remove Smm address convert table
  //
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_BODY,
             UsbMouseDevice
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbMouseDevice->UsbIo
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbMouseDevice->UsbCore
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbMouseDevice->RecoveryPollingHandle
             );
  return EFI_SUCCESS;
}

/**

  Supported for UsbIo(called from UsbCore).
  
  @param  UsbIo                 UsbIo

  @retval EFI_UNSUPPORTED
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbMouseUsbBindingSupported (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  )
{
  EFI_STATUS Status;
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 1)
  //
  // Filter out the USB devices which in the UsbIgnoreDevice list
  //
  Status = UsbCore->CheckIgnoredDevice(DevicePath, UsbIo);
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif
  Status = EFI_UNSUPPORTED;
  if (IsUsbMouse (UsbIo, UsbCore)) {
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**

  Supported for UsbIo(called from UsbCore).
  
  @param  UsbIo                 UsbIo
  @param  UsbCore               UsbCore
  @param  Handle                The instance of UsbKeyboard for output

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbMouseUsbBindingStart (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  )
{
  EFI_STATUS                  Status;
  EFI_USB_ENDPOINT_DESCRIPTOR EndpointDesc;
  USB_MOUSE_DEV               *UsbMouseDevice;
  UINT8                       EndpointNumber;
  UINT8                       Index;
  UINT8                       EndpointAddr;
  UINT8                       PollingInterval;
  UINT8                       PacketSize;
  BOOLEAN                     EndpointFound;
  USB_DEVICE                  UsbDevice;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  UsbMouseDevice  = NULL;
  Status = UsbCore->AllocateBuffer (
                        sizeof (USB_MOUSE_DEV),
                        ALIGNMENT_32,
                        (VOID **)&UsbMouseDevice
                        );
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  UsbMouseDevice->UsbIo               = UsbIo;
  UsbMouseDevice->UsbCore             = UsbCore;
  UsbMouseDevice->Signature           = USB_MOUSE_DEV_SIGNATURE;
  //
  // Get interface & endpoint descriptor
  //
  UsbIo->UsbGetInterfaceDescriptor (
           UsbIo,
           &UsbMouseDevice->InterfaceDescriptor
           );

  EndpointNumber = UsbMouseDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0, EndpointFound = FALSE; Index < EndpointNumber; Index++) {
    UsbIo->UsbGetEndpointDescriptor (
             UsbIo,
             Index,
             &EndpointDesc
             );

    if ((EndpointDesc.Attributes & 0x03) == 0x03 && (EndpointDesc.EndpointAddress & 0x80)) {
      //
      // We only care interrupt-in endpoint here
      //
      UsbMouseDevice->IntEndpointDescriptor = EndpointDesc;
      EndpointFound = TRUE;
      break;
    }
  }

  if (!EndpointFound) {
    //
    // No interrupt endpoint, then error
    //
    Status = EFI_UNSUPPORTED;
    goto ErrorExit;
  }

  Status = InitializeUsbMouseDevice (UsbMouseDevice);
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbMouseDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8) (UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize);

  Status = UsbIo->UsbAsyncInterruptTransfer (
                    UsbIo,
                    EndpointAddr,
                    TRUE,
                    PollingInterval,
                    PacketSize,
                    OnMouseInterruptComplete,
                    UsbMouseDevice
                    );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Insert UsbIoDevice into UsbCore for UsbLegacy
  //
  UsbDevice.UsbHID.Type     = USB_CORE_USB_HID;
  UsbDevice.UsbHID.UsbIo    = UsbIo;
  UsbDevice.UsbHID.Instance = UsbMouseDevice;
  UsbDevice.UsbHID.SyncLED  = NULL;
  UsbCore->InsertUsbDevice (
             &UsbDevice
             );
  return EFI_SUCCESS;

ErrorExit:
  if (UsbMouseDevice != NULL) {
    UsbCore->FreeBuffer (
               sizeof (USB_MOUSE_DEV),
               UsbMouseDevice
               );
  }
  return Status;
}

/**

  Supported for UsbIo(called from UsbCore).
  
  @param  Handle                The instance of UsbKeyboard allocated by UsbIoBindingStart

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbMouseUsbBindingStop (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  )
{
  USB_MOUSE_DEV               *UsbMouseDevice;
  EFI_USB_CORE_PROTOCOL       *UsbCore;  

  UsbMouseDevice = (USB_MOUSE_DEV*)Handle;
  if (UsbIo != UsbMouseDevice->UsbIo) {
    //
    // Due to UsbCore may pass wrong Handle for us
    //
    return EFI_NOT_FOUND;
  }
  UsbCore = UsbMouseDevice->UsbCore;
  //
  // Remove the recovery handler before deleting the async interrupt transfer to preventing
  // async interrupt transfer restarted by recovery handler
  //
  UsbCore->RemovePeriodicTimer(UsbMouseDevice->RecoveryPollingHandle);
  //
  // Delete Mouse Async Interrupt Transfer
  //
  UsbIo->UsbAsyncInterruptTransfer (
           UsbIo,
           UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
           FALSE,
           UsbMouseDevice->IntEndpointDescriptor.Interval,
           0,
           NULL,
           NULL
           );
  UsbCore->FreeBuffer (
             sizeof (USB_MOUSE_DEV),
             UsbMouseDevice
             );
  //
  // Remove UsbIoDevice from UsbCore for UsbLegacy
  //
  UsbCore->RemoveUsbDevice (
             UsbIo
             );
  return EFI_SUCCESS;
}

/**

  Tell if a Usb Controller is a mouse

  @param  UsbIo                 UsbIo protocol pointer.
  @param  UsbCore               UsbCore protocol pointer

  @retval TRUE                  It is a mouse
  @retval FALSE                 It is not a mouse

**/
BOOLEAN
IsUsbMouse (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  EFI_USB_CORE_PROTOCOL   *UsbCore
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  EFI_USB_HID_DESCRIPTOR        MouseHidDesc;
  UINT8                         *ReportDesc;
  UINT16                        DescriptorLength;
  BOOLEAN                       MouseDeviceFound;

  //
  // Get the Default interface descriptor, now we only
  // suppose it is interface 1
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (
                    UsbIo,
                    &InterfaceDescriptor
                    );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if ((InterfaceDescriptor.InterfaceClass == CLASS_HID) &&
      (InterfaceDescriptor.InterfaceProtocol == PROTOCOL_MOUSE)) {
    return TRUE;
  }
  if ((InterfaceDescriptor.InterfaceClass == CLASS_HID) &&
      (InterfaceDescriptor.InterfaceSubClass == 0) &&
      (InterfaceDescriptor.InterfaceProtocol == 0)) {
    Status = UsbCore->UsbGetHidDescriptor (
                        UsbIo,
                        InterfaceDescriptor.InterfaceNumber,
                        &MouseHidDesc
                        );
    if (EFI_ERROR (Status) || MouseHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
      return FALSE;
    }
    DescriptorLength = MouseHidDesc.HidClassDesc[0].DescriptorLength;
    Status = UsbCore->AllocateBuffer (
                        DescriptorLength,
                        ALIGNMENT_32,
                        (VOID **)&ReportDesc
                        );
    if (EFI_ERROR(Status)) {                    
      return FALSE;
    }
    MouseDeviceFound = FALSE;
    Status = UsbCore->UsbGetReportDescriptor (
                        UsbIo,
                        InterfaceDescriptor.InterfaceNumber,
                        DescriptorLength,
                        ReportDesc
                        );
    if (!EFI_ERROR (Status) &&
        (MatchHidDeviceType (ReportDesc, DescriptorLength, MOUSE_CLASS) || 
         MatchHidDeviceType (ReportDesc, DescriptorLength, POINTER_CLASS)) &&
        !MatchHidDeviceType (ReportDesc, DescriptorLength, (HID_UP_DIGITIZERS << 8) | HID_DIGITIZERS_TOUCH_SCREEN) &&
        !MatchHidDeviceType (ReportDesc, DescriptorLength, (HID_UP_DIGITIZERS << 8) | HID_DIGITIZERS_PEN)) {
      MouseDeviceFound = TRUE;
    }
    UsbCore->FreeBuffer (
               DescriptorLength,
               ReportDesc
               );
    if (MouseDeviceFound) return TRUE;
  }
  return FALSE;
}

/**

  Initialize the Usb Mouse Device.

  @param  UsbMouseDevice        Device instance to be initialized

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Init error.
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory

**/
STATIC
EFI_STATUS
InitializeUsbMouseDevice (
  IN  USB_MOUSE_DEV           *UsbMouseDevice
  )
{
  EFI_USB_IO_PROTOCOL       *UsbIo;
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  UINT8                     Protocol;
  BOOLEAN                   ProtocolChange;
  EFI_STATUS                Status;
  EFI_USB_HID_DESCRIPTOR    MouseHidDesc;
  UINT8                     *ReportDesc;
  EFI_USB_DEVICE_DESCRIPTOR DeviceDescriptor;
  BOOLEAN                   IncompatibleBootProtocol;
  UINTN                     Index;
  REPORT_GROUP              *ReportGroup;
  UINT16                    DescriptorLength;

  UsbIo   = UsbMouseDevice->UsbIo;
  UsbCore = UsbMouseDevice->UsbCore;

  //
  // Get HID descriptor
  //
  Status = UsbCore->UsbGetHidDescriptor (
                      UsbIo,
                      UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                      &MouseHidDesc
                      );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get Report descriptor
  //
  if (MouseHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
    return EFI_UNSUPPORTED;
  }
  DescriptorLength = MouseHidDesc.HidClassDesc[0].DescriptorLength;
  Status = UsbCore->AllocateBuffer (
                      DescriptorLength,
                      ALIGNMENT_32,
                      (VOID **)&ReportDesc
                      );
  
  if (EFI_ERROR(Status)) {                    
    return EFI_OUT_OF_RESOURCES;
  }

  Status = UsbCore->UsbGetReportDescriptor (
                      UsbIo,
                      UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                      DescriptorLength,
                      ReportDesc
                      );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Set appropriate protocol to the mouse interface
  //
  Protocol = REPORT_PROTOCOL;
  if (UsbMouseDevice->InterfaceDescriptor.InterfaceSubClass == SUBCLASS_BOOT) {
    Status = UsbCore->UsbGetProtocolRequest (
                        UsbIo,
                        UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                        &Protocol
                        );
    if (!EFI_ERROR (Status)) {
      //
      // Stall 1ms to waiting for slow hub devices
      //
      UsbCore->Stall (1000);
      //
      // Get device descriptor for VID/PID for Incompatible BootProtocol Mouse detection
      //
      UsbIo->UsbGetDeviceDescriptor (
               UsbIo,
               &DeviceDescriptor
               );
      //
      // Incompatible BootProtocol Mouse detection
      //
      ProtocolChange           = FALSE;
      IncompatibleBootProtocol = FALSE;
      for (Index = 0; IncompatibleBootProtocolMouse[Index].IdVendor != 0; Index ++) {
        if (IncompatibleBootProtocolMouse[Index].IdVendor == DeviceDescriptor.IdVendor && IncompatibleBootProtocolMouse[Index].IdProduct == DeviceDescriptor.IdProduct) {
          IncompatibleBootProtocol = TRUE;
          break;
        }
      }
      if (!IncompatibleBootProtocol) {
        if (Protocol != BOOT_PROTOCOL) {
          Protocol = BOOT_PROTOCOL;
          ProtocolChange = TRUE;
        }
      } else if (Protocol != REPORT_PROTOCOL) {
        Protocol = REPORT_PROTOCOL;
        ProtocolChange = TRUE;
      }
      if (ProtocolChange) {
        UsbCore->UsbSetProtocolRequest (
                   UsbIo,
                   UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                   Protocol
                   );
      }
    }
  }
  if (Protocol == REPORT_PROTOCOL) {
    //
    // Parse report descriptor when target protocol is report protocol
    //
    Status = ParseReportDescriptor (
               ReportDesc,
               DescriptorLength,
               &UsbMouseDevice->ReportFieldInfo
               );
    if (EFI_ERROR (Status)) {
      goto ErrorExit;
    }
    //
    // Searching for mouse report
    //
    for (Index = 0, ReportGroup = NULL; Index < UsbMouseDevice->ReportFieldInfo.Total; Index ++) {
      if (UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataValid &&
          (UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataClass == MOUSE_CLASS || 
           UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataClass == POINTER_CLASS) &&
          UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataType == HID_MAIN_ITEM_TAG_INPUT &&
          UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataSize > 0) {
        ReportGroup =  &UsbMouseDevice->ReportFieldInfo.ReportGroup[Index];
        break;
      }
    }
    if (ReportGroup == NULL) {
      Status = EFI_UNSUPPORTED;
      goto ErrorExit;
    }
  } else {
    //
    // Set ReportFieldInfo for boot protocol when target protocol is boot protocol
    //
    UsbMouseDevice->ReportFieldInfo.Total = 1;
    ReportGroup = &UsbMouseDevice->ReportFieldInfo.ReportGroup[0];
    ReportGroup->DataValid = TRUE;
    ReportGroup->DataClass = MOUSE_CLASS;
    ReportGroup->DataType  = HID_MAIN_ITEM_TAG_INPUT;
    ReportGroup->DataSize  = 3;
    ReportGroup->Id        = 0;
    ReportGroup->Data.Mouse.FieldButton1.DataValid = TRUE;
    ReportGroup->Data.Mouse.FieldButton1.BitLength = 1;
    ReportGroup->Data.Mouse.FieldButton1.BitOffset = 0;
    ReportGroup->Data.Mouse.FieldButton1.Min       = 0;
    ReportGroup->Data.Mouse.FieldButton1.Max       = 1;
    ReportGroup->Data.Mouse.FieldButton2.DataValid = TRUE;
    ReportGroup->Data.Mouse.FieldButton2.BitLength = 1;
    ReportGroup->Data.Mouse.FieldButton2.BitOffset = 1;
    ReportGroup->Data.Mouse.FieldButton2.Min       = 0;
    ReportGroup->Data.Mouse.FieldButton2.Max       = 1;
    ReportGroup->Data.Mouse.FieldX.DataValid       = TRUE;
    ReportGroup->Data.Mouse.FieldX.BitLength       = 8;
    ReportGroup->Data.Mouse.FieldX.BitOffset       = 8;
    ReportGroup->Data.Mouse.FieldX.Min             = 0x80;
    ReportGroup->Data.Mouse.FieldX.Max             = 0x7f;
    ReportGroup->Data.Mouse.FieldY.DataValid       = TRUE;
    ReportGroup->Data.Mouse.FieldY.BitLength       = 8;
    ReportGroup->Data.Mouse.FieldY.BitOffset       = 16;
    ReportGroup->Data.Mouse.FieldY.Min             = 0x80;
    ReportGroup->Data.Mouse.FieldY.Max             = 0x7f;
    ReportGroup->Data.Mouse.FieldZ.DataValid       = TRUE;
    ReportGroup->Data.Mouse.FieldZ.BitLength       = 8;
    ReportGroup->Data.Mouse.FieldZ.BitOffset       = 24;
    ReportGroup->Data.Mouse.FieldZ.Min             = 0x80;
    ReportGroup->Data.Mouse.FieldZ.Max             = 0x7f;
  }

  UsbMouseDevice->ReportId = ReportGroup->Id;
  if (ReportGroup->Data.Mouse.FieldButton1.DataValid) {
    UsbMouseDevice->Mode.LeftButton = TRUE;
  }
  if (ReportGroup->Data.Mouse.FieldButton2.DataValid) {
    UsbMouseDevice->Mode.RightButton = TRUE;
  }
  if (ReportGroup->Data.Mouse.FieldX.DataValid) {
    UsbMouseDevice->Mode.ResolutionX = 8;
  }
  if (ReportGroup->Data.Mouse.FieldY.DataValid) {
    UsbMouseDevice->Mode.ResolutionY = 8;
  }
  if (ReportGroup->Data.Mouse.FieldZ.DataValid) {
    UsbMouseDevice->Mode.ResolutionZ = 8;
  }
  //
  // Set indefinite Idle rate for USB Mouse
  //
  UsbCore->UsbSetIdleRequest (
             UsbIo,
             UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
             0,
             0
             );
  if (UsbMouseDevice->RecoveryPollingHandle) {
    UsbCore->RemovePeriodicTimer(UsbMouseDevice->RecoveryPollingHandle);
    UsbMouseDevice->RecoveryPollingHandle = NULL;
  }
ErrorExit:
  UsbCore->FreeBuffer (
             DescriptorLength,
             ReportDesc
             );
  return Status;
}

/**

  It is called whenever there is data received from async interrupt
  transfer.

  @param  Data                  Data received.
  @param  DataLength            Length of Data
  @param  Context               Passed in context
  @param  Result                Async Interrupt Transfer result

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
EFIAPI
OnMouseInterruptComplete (
  IN  VOID        *Data,
  IN  UINTN       DataLength,
  IN  VOID        *Context,
  IN  UINT32      Result
  )
{
  USB_MOUSE_DEV         *UsbMouseDevice;
  EFI_USB_IO_PROTOCOL   *UsbIo;
  EFI_USB_CORE_PROTOCOL *UsbCore;
  UINT8                 EndpointAddr;
  UINT32                UsbResult;
  UINTN                 Mode;
  UINTN                 Count;
  INT8                  MouseCode[4];
  UINTN                 Index;
  REPORT_GROUP          *ReportGroup;

  UsbMouseDevice  = (USB_MOUSE_DEV *) Context;
  UsbIo           = UsbMouseDevice->UsbIo;
  UsbCore         = UsbMouseDevice->UsbCore;
  if (UsbIo == NULL) return EFI_DEVICE_ERROR;
  //
  // Get current mode
  //
  UsbCore->GetMode(&Mode);
  //
  // Analyzes the Result and performs corresponding action.
  //
  if (Result != EFI_USB_NOERROR) {
    //
    // Some errors happen during the process
    //
    MouseReportStatusCode (
      UsbMouseDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_MOUSE | EFI_P_EC_INPUT_ERROR)
      );

    if ((Result & EFI_USB_ERR_STALL) == EFI_USB_ERR_STALL) {
      EndpointAddr = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;

      UsbCore->UsbClearEndpointHalt (
                 UsbIo,
                 EndpointAddr,
                 &UsbResult
                 );
    }

    UsbIo->UsbAsyncInterruptTransfer (
             UsbIo,
             UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
             FALSE,
             0,
             0,
             NULL,
             NULL
             );
    //
    // Check is the device be detached
    //
    if (UsbCore->CheckDeviceDetached(UsbIo) == EFI_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }
    UsbCore->InsertPeriodicTimer(
               USB_CORE_ONCE_TIMER,
               UsbMouseRecoveryHandler,
               UsbMouseDevice,
               EFI_USB_INTERRUPT_DELAY / 10000,
               &UsbMouseDevice->RecoveryPollingHandle
               );

    return EFI_DEVICE_ERROR;
  }
  if (DataLength == 0 || Data == NULL) {
    return EFI_SUCCESS;
  }
  ReportGroup = &UsbMouseDevice->ReportFieldInfo.ReportGroup[0];
  if (UsbMouseDevice->ReportId != 0) {
    for (Index = 0; Index < UsbMouseDevice->ReportFieldInfo.Total; Index++) {
      if ((UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].Id == (*(UINT8*)Data)) &&
          (UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataValid)) {
        ReportGroup = &UsbMouseDevice->ReportFieldInfo.ReportGroup[Index];
        break;
      }
    }
  }
  if (Mode != USB_CORE_RUNTIME_MODE) {
    UsbMouseDevice->StateChanged = TRUE;
    //
    // Check mouse Data
    //
    UsbMouseDevice->State.LeftButton  = (BOOLEAN) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton1);
    UsbMouseDevice->State.RightButton = (BOOLEAN) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton2);
    UsbMouseDevice->State.RelativeMovementX += (INT8) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldX);
    UsbMouseDevice->State.RelativeMovementY += (INT8) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldY);
    if (ReportGroup->Data.Mouse.FieldZ.DataValid) {
      UsbMouseDevice->State.RelativeMovementZ += (INT8) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldZ);
    }
  } else {
    //
    // Insert mouse KBC code for legacy mode
    //
    MouseCode[0] = (INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton1);
    MouseCode[0] |= ((INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton2)) << 1;
    MouseCode[0] |= 0x08;
    MouseCode[1] = (INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldX);
    MouseCode[2] = -((INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldY));
    if (MouseCode[1] < 0) MouseCode[0] |= 0x10;
    if (MouseCode[2] < 0) MouseCode[0] |= 0x20;
    MouseCode[3] = 0;
    Count = (EBDA(EBDA_PS2_AUX_TYPE) == 0x03) ? 4 : 3;
    UsbCore->InsertKbcKeyCode(USB_CORE_MOUSE_CODE, (UINT8 *)MouseCode, Count);
  }
  return EFI_SUCCESS;
}

/**

  Get the mouse state, see SIMPLE POINTER PROTOCOL.
  
  @param  This                  Protocol instance pointer.
  @param  MouseState            Current mouse state
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_NOT_READY

**/
STATIC
EFI_STATUS
EFIAPI
GetMouseState (
  IN   EFI_SIMPLE_POINTER_PROTOCOL  *This,
  OUT  EFI_SIMPLE_POINTER_STATE     *MouseState
  )
{
  USB_MOUSE_DEV *MouseDev;

  if (MouseState == NULL) {
    return EFI_DEVICE_ERROR;
  }

  MouseDev = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (This);

  if (!MouseDev->StateChanged) {
    return EFI_NOT_READY;
  }

  CopyMem (
    MouseState,
    &MouseDev->State,
    sizeof (EFI_SIMPLE_POINTER_STATE)
    );

  //
  // Clear previous move state
  //
  MouseDev->State.RelativeMovementX = 0;
  MouseDev->State.RelativeMovementY = 0;
  MouseDev->State.RelativeMovementZ = 0;

  MouseDev->StateChanged            = FALSE;

  return EFI_SUCCESS;
}

/**

  Reset the mouse device, see SIMPLE POINTER PROTOCOL.
  
  @param  This                  Protocol instance pointer.
  @param  ExtendedVerification  Ignored here
  
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
UsbMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  USB_MOUSE_DEV       *UsbMouseDevice;
  EFI_USB_IO_PROTOCOL *UsbIo;

  UsbMouseDevice  = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (This);

  UsbIo           = UsbMouseDevice->UsbIo;

  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_RESET)
    );

  ZeroMem (
    &UsbMouseDevice->State,
    sizeof (EFI_SIMPLE_POINTER_STATE)
    );
  UsbMouseDevice->StateChanged = FALSE;

  return EFI_SUCCESS;
}

/**

  Event notification function for SIMPLE_POINTER.WaitForInput event
  Signal the event if there is input from mouse

  @param  Event                 Wait Event
  @param  Context               Passed parameter to event handler

**/
STATIC
VOID
EFIAPI
UsbMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  USB_MOUSE_DEV *UsbMouseDevice;

  UsbMouseDevice = (USB_MOUSE_DEV *) Context;

  //
  // Someone is waiting on the mouse event, if there's
  // input from mouse, signal the event
  //
  if (UsbMouseDevice->StateChanged) {
    gBS->SignalEvent (Event);
  }
}

/**

  Timer handler for Delayed Recovery timer.
  
  @param  Event                 The Delayed Recovery event.
  @param  Context               Points to the USB_KB_DEV instance.
  
**/    
VOID
EFIAPI
UsbMouseRecoveryHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  )
{
  USB_MOUSE_DEV       *UsbMouseDevice;
  EFI_USB_IO_PROTOCOL *UsbIo;

  UsbMouseDevice = (USB_MOUSE_DEV *) Context;
  UsbMouseDevice->RecoveryPollingHandle = NULL;
  UsbIo = UsbMouseDevice->UsbIo;
  if (UsbIo == NULL) return;
  UsbIo->UsbAsyncInterruptTransfer (
           UsbIo,
           UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
           TRUE,
           UsbMouseDevice->IntEndpointDescriptor.Interval,
           UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize,
           OnMouseInterruptComplete,
           UsbMouseDevice
           );
}

/**

  Report Status Code in Usb Bot Driver

  @param  DevicePath            Use this to get Device Path
  @param  CodeType              Status Code Type
  @param  CodeValue             Status Code Value

**/
VOID
MouseReportStatusCode (
  IN USB_MOUSE_DEV             *UsbMouseDevice,
  IN EFI_STATUS_CODE_TYPE      CodeType,
  IN EFI_STATUS_CODE_VALUE     Value
  )
{
  UINTN      Mode;
  BOOLEAN    InSmm;

  UsbMouseDevice->UsbCore->GetMode(&Mode);
  if (Mode != USB_CORE_RUNTIME_MODE) {
    //
    // Only report status code during POST and non-SMM mode
    //
    UsbMouseDevice->UsbCore->IsInSmm(&InSmm);
    if (!InSmm) {
      ReportStatusCodeWithDevicePath (
        CodeType,
        Value,
        UsbMouseDevice->DevicePath
        );
    }
  }
}
