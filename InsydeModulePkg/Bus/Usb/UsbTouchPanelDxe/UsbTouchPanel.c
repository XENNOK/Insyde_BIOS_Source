/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbTouchPanel.h"

#include <PostCode.h>

//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
UsbTouchPanelDriverBindingEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  );

EFI_STATUS
EFIAPI
UsbTouchPanelDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbTouchPanelDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbTouchPanelDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  );

EFI_DRIVER_BINDING_PROTOCOL gUsbTouchPanelDriverBinding = {
  UsbTouchPanelDriverBindingSupported,
  UsbTouchPanelDriverBindingStart,
  UsbTouchPanelDriverBindingStop,
  0x12,
  NULL,
  NULL
};

//
// helper functions
//
STATIC
BOOLEAN
IsUsbTouchPanel (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  EFI_USB_CORE_PROTOCOL             *UsbCore
  );

STATIC
EFI_STATUS
InitializeUsbTouchPanelDevice (
  IN  USB_TOUCH_PANEL_DEV               *UsbTouchPanelDevice
  );

STATIC
VOID
EFIAPI
UsbTouchPanelWaitForInput (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  );

STATIC
VOID
EFIAPI
DisconnectUsbTouchPanel (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  );
//
// Touch Panel interrupt handler
//
STATIC
EFI_STATUS
EFIAPI
OnTouchPanelInterruptComplete (
  IN  VOID                              *Data,
  IN  UINTN                             DataLength,
  IN  VOID                              *Context,
  IN  UINT32                            Result
  );

//
// Touch Panel Protocol
//
STATIC
EFI_STATUS
EFIAPI
GetTouchPanelState (
  IN   EFI_ABSOLUTE_POINTER_PROTOCOL    *This,
  OUT  EFI_ABSOLUTE_POINTER_STATE       *TouchPanelState
  );

STATIC
EFI_STATUS
EFIAPI
UsbTouchPanelReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL      *This,
  IN BOOLEAN                            ExtendedVerification
  );

STATIC
VOID
EFIAPI
UsbTouchPanelRecoveryHandler (
  IN UINTN                              Event,
  IN VOID                               *Context
  );

STATIC
VOID
TouchPanelReportStatusCode (
  IN USB_TOUCH_PANEL_DEV                *UsbTouchPanelDevice,
  IN EFI_STATUS_CODE_TYPE               CodeType,
  IN EFI_STATUS_CODE_VALUE              Value
  );


/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS
  @retval others

**/
EFI_STATUS
EFIAPI
UsbTouchPanelDriverBindingEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  //
  // Install binding protocols
  //
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gUsbTouchPanelDriverBinding,
           ImageHandle,
           &gUsbTouchPanelComponentName,
           &gUsbTouchPanelComponentName2
           );
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
UsbTouchPanelDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
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
  // the Touch Panel controller that can be managed by this driver.
  //
  Status = EFI_SUCCESS;
  if (!IsUsbTouchPanel (UsbIo, UsbCore)) {
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
UsbTouchPanelDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_USB_IO_PROTOCOL         *UsbIo;
  EFI_USB_CORE_PROTOCOL       *UsbCore;
  EFI_USB_ENDPOINT_DESCRIPTOR EndpointDesc;
  USB_TOUCH_PANEL_DEV         *UsbTouchPanelDevice;
  UINT8                       EndpointNumber;
  UINT8                       Index;
  UINT8                       EndpointAddr;
  UINT8                       PollingInterval;
  UINT8                       PacketSize;
  BOOLEAN                     EndpointFound;
  EFI_EVENT                   LegacyBootEvent;
  EFI_EVENT                   ExitBootServicesEvent;
  
  POST_CODE (BDS_CONNECT_USB_DEVICE);

  UsbTouchPanelDevice   = NULL;
  UsbCore               = NULL;
  Status                = EFI_SUCCESS;
  LegacyBootEvent       = NULL;
  ExitBootServicesEvent = NULL;

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
                      sizeof (USB_TOUCH_PANEL_DEV),
                      ALIGNMENT_32,
                      (VOID **)&UsbTouchPanelDevice
                      );
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  UsbTouchPanelDevice->UsbIo               = UsbIo;
  UsbTouchPanelDevice->UsbCore             = UsbCore;
  UsbTouchPanelDevice->Signature           = USB_TOUCH_PANEL_DEV_SIGNATURE;

  //
  // Get the Device Path Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &UsbTouchPanelDevice->DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Get interface & endpoint descriptor
  //
  UsbIo->UsbGetInterfaceDescriptor (
           UsbIo,
           &UsbTouchPanelDevice->InterfaceDescriptor
           );

  EndpointNumber = UsbTouchPanelDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0, EndpointFound = FALSE; Index < EndpointNumber; Index++) {
    UsbIo->UsbGetEndpointDescriptor (
             UsbIo,
             Index,
             &EndpointDesc
             );

    if ((EndpointDesc.Attributes & 0x03) == 0x03 && (EndpointDesc.EndpointAddress & 0x80)) {
      //
      // We only care interrupt endpoint here
      //
      UsbTouchPanelDevice->IntEndpointDescriptor = EndpointDesc;
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

  Status = InitializeUsbTouchPanelDevice (UsbTouchPanelDevice);
  if (EFI_ERROR (Status)) {
    TouchPanelReportStatusCode (
      UsbTouchPanelDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_TOUCH_PANEL | EFI_P_EC_INTERFACE_ERROR)
      );

    goto ErrorExit;
  }

  UsbTouchPanelDevice->AbsolutePointerProtocol.GetState  = GetTouchPanelState;
  UsbTouchPanelDevice->AbsolutePointerProtocol.Reset     = UsbTouchPanelReset;
  UsbTouchPanelDevice->AbsolutePointerProtocol.Mode      = &UsbTouchPanelDevice->Mode;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  UsbTouchPanelWaitForInput,
                  UsbTouchPanelDevice,
                  &((UsbTouchPanelDevice->AbsolutePointerProtocol).WaitForInput)
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiAbsolutePointerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &UsbTouchPanelDevice->AbsolutePointerProtocol
                  );

  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }
  
  //
  // After Enabling Async Interrupt Transfer on this Touch Panel Device
  // we will be able to get key data from it. Thus this is deemed as
  // the enable action of the Touch Panel
  //
  
  TouchPanelReportStatusCode (
    UsbTouchPanelDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_TOUCH_PANEL | EFI_P_PC_ENABLE)
    );

  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbTouchPanelDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbTouchPanelDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8)UsbTouchPanelDevice->IntEndpointDescriptor.MaxPacketSize;
  if (PacketSize < UsbTouchPanelDevice->ReportFieldInfo.BiggestInputReportGroup->DataSize) {
    PacketSize = (UINT8)UsbTouchPanelDevice->ReportFieldInfo.BiggestInputReportGroup->DataSize;
  }
  Status = UsbIo->UsbAsyncInterruptTransfer (
                    UsbIo,
                    EndpointAddr,
                    TRUE,
                    PollingInterval,
                    PacketSize,
                    OnTouchPanelInterruptComplete,
                    UsbTouchPanelDevice
                    );

  if (!EFI_ERROR (Status)) {

    UsbTouchPanelDevice->ControllerNameTable = NULL;
    AddUnicodeString2 (
      LANGUAGE_CODE_ENGLISH_ISO639,
      gUsbTouchPanelComponentName.SupportedLanguages,
      &UsbTouchPanelDevice->ControllerNameTable,
      CONTROLLER_DRIVER_NAME,
      TRUE
      );
    AddUnicodeString2 (
      LANGUAGE_CODE_ENGLISH_RFC4646,
      gUsbTouchPanelComponentName2.SupportedLanguages,
      &UsbTouchPanelDevice->ControllerNameTable,
      CONTROLLER_DRIVER_NAME,
      FALSE
      );
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DisconnectUsbTouchPanel,
                    (VOID *) Controller,
                    &gEfiEventLegacyBootGuid,
                    &LegacyBootEvent
                    );
    if (EFI_ERROR (Status)) {
      goto ErrorExit;
    }
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DisconnectUsbTouchPanel,
                    (VOID *) Controller,
                    &gEfiEventExitBootServicesGuid,
                    &ExitBootServicesEvent
                    );
    if (EFI_ERROR (Status)) {
      goto ErrorExit;
    }
    return EFI_SUCCESS;
  }

  //
  // If submit error, uninstall that interface
  //
  Status = EFI_DEVICE_ERROR;
  gBS->UninstallProtocolInterface (
        Controller,
        &gEfiAbsolutePointerProtocolGuid,
        &UsbTouchPanelDevice->AbsolutePointerProtocol
        );

ErrorExit:
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
          Controller,
          &gEfiUsbIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );

    if (UsbTouchPanelDevice != NULL) {

      if ((UsbTouchPanelDevice->AbsolutePointerProtocol).WaitForInput != NULL) {
        gBS->CloseEvent ((UsbTouchPanelDevice->AbsolutePointerProtocol).WaitForInput);
      }
      UsbCore->FreeBuffer (
                 sizeof (USB_TOUCH_PANEL_DEV),
                 UsbTouchPanelDevice
                 );
    }
    if (LegacyBootEvent != NULL) {
      gBS->CloseEvent (LegacyBootEvent);
    }
    if (ExitBootServicesEvent != NULL) {
      gBS->CloseEvent (ExitBootServicesEvent);
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
UsbTouchPanelDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  )
{
  EFI_STATUS                    Status;
  USB_TOUCH_PANEL_DEV           *UsbTouchPanelDevice;
  EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointerProtocol;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  //
  // Get our context back.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiAbsolutePointerProtocolGuid,
                  (VOID **)&AbsolutePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  UsbTouchPanelDevice = USB_TOUCH_PANEL_DEV_FROM_TOUCH_PANEL_PROTOCOL (AbsolutePointerProtocol);
  UsbIo   = UsbTouchPanelDevice->UsbIo;
  UsbCore = UsbTouchPanelDevice->UsbCore;
  //
  // Set the UsbIo to NULL to preventing async interrupt transfer restarted 
  // by recovery handler after deleting the async interrupt transfer
  //
  UsbTouchPanelDevice->UsbIo = NULL;

  gBS->CloseProtocol (
        Controller,
        &gEfiAbsolutePointerProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );
  //
  // Remove the recovery handler before deleting the async interrupt transfer to preventing
  // async interrupt transfer restarted by recovery handler
  //
  UsbCore->RemovePeriodicTimer(UsbTouchPanelDevice->RecoveryPollingHandle);
  //
  // Uninstall the Asyn Interrupt Transfer from this device
  // will disable the Touch Panel data input from this device
  //
  TouchPanelReportStatusCode (
    UsbTouchPanelDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_TOUCH_PANEL | EFI_P_PC_DISABLE)
    );

  //
  // Delete Touch Panel Async Interrupt Transfer
  //
  UsbIo->UsbAsyncInterruptTransfer (
           UsbIo,
           UsbTouchPanelDevice->IntEndpointDescriptor.EndpointAddress,
           FALSE,
           UsbTouchPanelDevice->IntEndpointDescriptor.Interval,
           0,
           NULL,
           NULL
           );

  gBS->CloseEvent (UsbTouchPanelDevice->AbsolutePointerProtocol.WaitForInput);

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiAbsolutePointerProtocolGuid,
                  &UsbTouchPanelDevice->AbsolutePointerProtocol
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

  if (UsbTouchPanelDevice->ControllerNameTable) {
    FreeUnicodeStringTable (UsbTouchPanelDevice->ControllerNameTable);
  }

  UsbCore->FreeBuffer (
             sizeof (USB_TOUCH_PANEL_DEV),
             UsbTouchPanelDevice
             );
  return EFI_SUCCESS;
}

/**

  Event notification function for disconnect USB Touch Panel

  @param  Event                 Legacy Boot event or Exit Boot Service event
  @param  Context               Controller handler of USB Touch Panel

**/
STATIC
VOID
EFIAPI
DisconnectUsbTouchPanel (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  )
{
  gBS->DisconnectController ((EFI_HANDLE) Context, NULL, NULL);
}

/**

  Tell if a Usb Controller is a Touch Panel

  @param  UsbIo                 Protocol instance pointer.

  @retval TRUE                  It is a Touch Panel
  @retval FALSE                 It is not a Touch Panel

**/
BOOLEAN
IsUsbTouchPanel (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  EFI_USB_CORE_PROTOCOL             *UsbCore
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  EFI_USB_HID_DESCRIPTOR        TouchPanelHidDesc;
  UINT8                         *ReportDesc;
  BOOLEAN                       TouchDev = FALSE;

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
  if (InterfaceDescriptor.InterfaceClass == CLASS_HID) {
    Status = UsbCore->UsbGetHidDescriptor (
                        UsbIo,
                        InterfaceDescriptor.InterfaceNumber,
                        &TouchPanelHidDesc
                        );
    if (EFI_ERROR (Status) || TouchPanelHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
      return FALSE;
    }
    Status = UsbCore->AllocateBuffer (
                        TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
                        ALIGNMENT_32,
                        (VOID **)&ReportDesc
                        );
    if (EFI_ERROR(Status)) {                    
      return FALSE;
    }
    Status = UsbCore->UsbGetReportDescriptor (
                        UsbIo,
                        InterfaceDescriptor.InterfaceNumber,
                        TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
                        ReportDesc
                        );
    if (!EFI_ERROR (Status)) {
      TouchDev = MatchHidDeviceType (
                   ReportDesc,
                   TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
                   (HID_UP_DIGITIZERS << 8) | HID_DIGITIZERS_TOUCH_SCREEN
                   );
      if (!TouchDev) {
        TouchDev = MatchHidDeviceType (
                     ReportDesc,
                     TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
                     (HID_UP_DIGITIZERS << 8) | HID_DIGITIZERS_PEN
                     );
      }
    }
    UsbCore->FreeBuffer (
               TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
               ReportDesc
               );
    if (TouchDev) return TRUE;
  }
  return FALSE;
}

EFI_STATUS
SetDeviceMode (
  IN  USB_TOUCH_PANEL_DEV               *UsbTouchPanelDevice
  )
{
  EFI_STATUS              Status;
  REPORT_GROUP            *ReportGroup;
  EFI_USB_CORE_PROTOCOL   *UsbCore;
  UINT8                   *ReportData;

  if ((UsbTouchPanelDevice == NULL) ||
      (UsbTouchPanelDevice->UsbIo == NULL) ||
      (UsbTouchPanelDevice->UsbCore == NULL) ||
      (UsbTouchPanelDevice->ReportFieldInfo.DeviceConfigurationReportGroup == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  ReportGroup = UsbTouchPanelDevice->ReportFieldInfo.DeviceConfigurationReportGroup;
  //
  // Since Device Configuration must have its own report (see HUTRR34.pdf), Report ID must be
  // reported by the USB touch device and can not be the reserved value of 0.
  //
  if ((ReportGroup->Id == 0) || (!(ReportGroup->DataValid))) {
    return EFI_UNSUPPORTED;
  }
  UsbCore = UsbTouchPanelDevice->UsbCore;
  Status = UsbCore->AllocateBuffer (
                      ReportGroup->DataSize,
                      ALIGNMENT_32,
                      (VOID **)&ReportData
                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = UsbCore->UsbGetReportRequest (
                      UsbTouchPanelDevice->UsbIo,
                      UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
                      ReportGroup->Id,
                      HID_FEATURE_REPORT,
                      ReportGroup->DataSize,
                      ReportData
                      );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }
  if (GetReportFieldValue (ReportData, ReportGroup->DataSize, &ReportGroup->Data.TouchPanelMode.FieldDeviceMode) == HID_DIGITIZERS_DEVICE_MODE_SINGLE_INPUT) {
    goto EXIT;
  }
  Status = SetReportFieldValue (
             ReportData,
             ReportGroup->DataSize,
             &ReportGroup->Data.TouchPanelMode.FieldDeviceMode,
             HID_DIGITIZERS_DEVICE_MODE_SINGLE_INPUT
             );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }
  if (ReportGroup->Data.TouchPanelMode.FieldDeviceId.DataValid) {
    //
    // Use a Device ID of 0 to configure all the report groups (i.e. top-level collections)
    // at the same time regardless of Device ID (see HUTRR34.pdf).
    //
    Status = SetReportFieldValue (
               ReportData,
               ReportGroup->DataSize,
               &ReportGroup->Data.TouchPanelMode.FieldDeviceId,
               0
               );
    if (EFI_ERROR (Status)) {
      goto EXIT;
    }
  }
  Status = UsbCore->UsbSetReportRequest (
                      UsbTouchPanelDevice->UsbIo,
                      UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
                      ReportGroup->Id,
                      HID_FEATURE_REPORT,
                      ReportGroup->DataSize,
                      ReportData
                      );
EXIT:
  UsbCore->FreeBuffer (
             ReportGroup->DataSize,
             ReportData
             );
  return Status;
}

/**

  The workaround for some incompatible device

  @param  UsbTouchPanelDevice   Device instance to be initialized

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Init error.
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory
  
**/
STATIC
EFI_STATUS
GetFeatureData (
  IN  USB_TOUCH_PANEL_DEV           *UsbTouchPanelDevice
  )
{
  EFI_STATUS                    Status;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;
  UINT8                         *FeatureData;
  UINT16                        *LangIDTable;
  UINT16                        TableSize;
  UINT16                        *String;
  
  if (UsbTouchPanelDevice->ReportFieldInfo.FirstFeatureID != 0 && UsbTouchPanelDevice->ReportFieldInfo.MaximumFeatureLength != 0) {
    UsbIo   = UsbTouchPanelDevice->UsbIo;
    UsbCore = UsbTouchPanelDevice->UsbCore;
    Status = UsbIo->UsbGetDeviceDescriptor (
                      UsbIo,
                      &DeviceDescriptor
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (DeviceDescriptor.StrProduct != 0) {
      Status = UsbCore->AllocateBuffer (
                          UsbTouchPanelDevice->ReportFieldInfo.MaximumFeatureLength,
                          ALIGNMENT_32,
                          (VOID **)&FeatureData
                          );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      UsbCore->UsbGetReportRequest (
                 UsbIo,
                 UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
                 (UINT8)UsbTouchPanelDevice->ReportFieldInfo.FirstFeatureID,
                 HID_FEATURE_REPORT,
                 (UINT16)UsbTouchPanelDevice->ReportFieldInfo.MaximumFeatureLength,
                 FeatureData
                 );
      Status = UsbIo->UsbGetSupportedLanguages (
                        UsbIo,
                        &LangIDTable,
                        &TableSize
                        );
      if (EFI_ERROR (Status)) {
        goto ErrorExit;
      }
      UsbCore->UsbGetReportRequest (
                 UsbIo,
                 UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
                 (UINT8)UsbTouchPanelDevice->ReportFieldInfo.FirstFeatureID,
                 HID_FEATURE_REPORT,
                 (UINT16)UsbTouchPanelDevice->ReportFieldInfo.MaximumFeatureLength,
                 FeatureData
                 );
      UsbIo->UsbGetStringDescriptor (
               UsbIo,
               LangIDTable[0],
               DeviceDescriptor.StrProduct,
               &String
               );
ErrorExit:
      UsbCore->FreeBuffer (
                UsbTouchPanelDevice->ReportFieldInfo.MaximumFeatureLength,
                FeatureData
                );
    }
  }
  return EFI_SUCCESS;
}

/**

  Initialize the Usb Touch Panel Device.

  @param  UsbTouchPanelDevice   Device instance to be initialized

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Init error.
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory

**/
STATIC
EFI_STATUS
InitializeUsbTouchPanelDevice (
  IN  USB_TOUCH_PANEL_DEV               *UsbTouchPanelDevice
  )
{
  EFI_USB_IO_PROTOCOL     *UsbIo;
  EFI_USB_CORE_PROTOCOL   *UsbCore;
  EFI_STATUS              Status;
  EFI_USB_HID_DESCRIPTOR  TouchPanelHidDesc;
  UINT8                   *ReportDesc;
  REPORT_GROUP            *ReportGroup;

  UsbIo   = UsbTouchPanelDevice->UsbIo;
  UsbCore = UsbTouchPanelDevice->UsbCore;

  //
  // Get HID descriptor
  //
  Status = UsbCore->UsbGetHidDescriptor (
                      UsbIo,
                      UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
                      &TouchPanelHidDesc
                      );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get Report descriptor
  //
  if (TouchPanelHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
    return EFI_UNSUPPORTED;
  }

  Status = UsbCore->AllocateBuffer (
                      TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
                      ALIGNMENT_32,
                      (VOID **)&ReportDesc
                      );
  
  if (EFI_ERROR(Status)) {                    
    return EFI_OUT_OF_RESOURCES;
  }

  Status = UsbCore->UsbGetReportDescriptor (
                      UsbIo,
                      UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
                      TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
                      ReportDesc
                      );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Initialize with "not found" values before parsing.
  //
  UsbTouchPanelDevice->ReportFieldInfo.BiggestInputReportGroup = NULL;
  UsbTouchPanelDevice->ReportFieldInfo.DeviceConfigurationReportGroup = NULL;
  //
  // Parse report descriptor
  //
  Status = ParseReportDescriptor (
             ReportDesc,
             TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
             &UsbTouchPanelDevice->ReportFieldInfo
             );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Since we do not know which report data will be sent by the USB device (and we will interpret
  // the different report data by the report group with the correct Report ID anyway), the default
  // Mode parameters are set based on the report group with the biggest input data size for now.
  //
  ReportGroup = UsbTouchPanelDevice->ReportFieldInfo.BiggestInputReportGroup;

  if ((ReportGroup == NULL) ||
      (!(ReportGroup->DataValid)) ||
      (ReportGroup->DataSize == 0)) {
    Status = EFI_UNSUPPORTED;
    goto ErrorExit;
  }
  UsbTouchPanelDevice->ReportId = ReportGroup->Id;
  UsbTouchPanelDevice->Mode.AbsoluteMinX = ReportGroup->Data.TouchPanel.FieldX.Min;
  UsbTouchPanelDevice->Mode.AbsoluteMinY = ReportGroup->Data.TouchPanel.FieldY.Min;
  UsbTouchPanelDevice->Mode.AbsoluteMaxX = ReportGroup->Data.TouchPanel.FieldX.Max;
  UsbTouchPanelDevice->Mode.AbsoluteMaxY = ReportGroup->Data.TouchPanel.FieldY.Max;
  UsbTouchPanelDevice->Mode.Attributes   = 0;
  if (ReportGroup->Data.TouchPanel.FieldAltActive.DataValid) {
    UsbTouchPanelDevice->Mode.Attributes |= EFI_ABSP_SupportsAltActive;
  }
  //
  // Set indefinite Idle rate for USB Touch Panel
  //
  UsbCore->UsbSetIdleRequest (
             UsbIo,
             UsbTouchPanelDevice->InterfaceDescriptor.InterfaceNumber,
             0,
             0
             );
  //
  // If Device Configuration feature report is found, this touch device has multiple operating
  // modes (mouse, single-input, multi-input, etc.).  Some devices only send "correct" report data
  // (i.e. absolute coordinates for digitizer as opposed to relative displacements for mouse) when
  // they are operating in single-input mode.  However, some other devices do not change operating
  // mode after SetDeviceMode (), but can still work with their report data; thus, we do not check
  // the return status of SetDeviceMode ().
  //
  if (UsbTouchPanelDevice->ReportFieldInfo.DeviceConfigurationReportGroup != NULL) {
    SetDeviceMode (UsbTouchPanelDevice);
  }
  //
  // Some incompatible device will postpone the first event around 10 seconds. To launch GetFeature 
  // command twice with first feature report ID and maximum feature length, and launch GetString to
  // get vendor string to workaround this issue
  //
  GetFeatureData (UsbTouchPanelDevice);

  if (UsbTouchPanelDevice->RecoveryPollingHandle) {
    UsbCore->RemovePeriodicTimer(UsbTouchPanelDevice->RecoveryPollingHandle);
    UsbTouchPanelDevice->RecoveryPollingHandle = NULL;
  }
ErrorExit:
  UsbCore->FreeBuffer (
             TouchPanelHidDesc.HidClassDesc[0].DescriptorLength,
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
OnTouchPanelInterruptComplete (
  IN  VOID                              *Data,
  IN  UINTN                             DataLength,
  IN  VOID                              *Context,
  IN  UINT32                            Result
  )
{
  USB_TOUCH_PANEL_DEV             *UsbTouchPanelDevice;
  EFI_USB_IO_PROTOCOL             *UsbIo;
  EFI_USB_CORE_PROTOCOL           *UsbCore;
  UINT8                           EndpointAddr;
  UINT32                          UsbResult;
  UINTN                           Mode;
  REPORT_GROUP                    *ReportGroup;
  UINTN                           Index;
  EFI_ABSOLUTE_POINTER_STATE      AbsPtrState;
  UINT8                           DifferentCase;

  UsbTouchPanelDevice  = (USB_TOUCH_PANEL_DEV *) Context;
  UsbIo           = UsbTouchPanelDevice->UsbIo;
  UsbCore         = UsbTouchPanelDevice->UsbCore;
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
    TouchPanelReportStatusCode (
      UsbTouchPanelDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_TOUCH_PANEL | EFI_P_EC_INPUT_ERROR)
      );

    if ((Result & EFI_USB_ERR_STALL) == EFI_USB_ERR_STALL) {
      EndpointAddr = UsbTouchPanelDevice->IntEndpointDescriptor.EndpointAddress;

      UsbCore->UsbClearEndpointHalt (
                 UsbIo,
                 EndpointAddr,
                 &UsbResult
                 );
    }

    UsbIo->UsbAsyncInterruptTransfer (
             UsbIo,
             UsbTouchPanelDevice->IntEndpointDescriptor.EndpointAddress,
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
               UsbTouchPanelRecoveryHandler,
               UsbTouchPanelDevice,
               EFI_USB_INTERRUPT_DELAY / 10000,
               &UsbTouchPanelDevice->RecoveryPollingHandle
               );
    return EFI_DEVICE_ERROR;
  }

  if (DataLength == 0 || Data == NULL) {
    return EFI_SUCCESS;
  }
  if (Mode != USB_CORE_RUNTIME_MODE) {
    //
    // USB Device Class Definition for Human Interface Devices (HID) specification version 1.11
    // page 36 states that a Report ID value of 0 is reserved.  However, since Report ID is
    // optional, our saved value of Report ID can be 0 and it means that no Report ID item tags
    // were present in the report descriptor.  If no Report ID was declared, we assume only one
    // report group exists and continue using the first report group (see page 18 of above
    // specification).
    //
    if (UsbTouchPanelDevice->ReportId == 0) {
      ReportGroup = &(UsbTouchPanelDevice->ReportFieldInfo.ReportGroup[0]);
    } else {
      ReportGroup = NULL;
      for (Index = 0; Index < UsbTouchPanelDevice->ReportFieldInfo.Total; Index++) {
        if ((UsbTouchPanelDevice->ReportFieldInfo.ReportGroup[Index].Id == (*(UINT8*)Data)) &&
            (UsbTouchPanelDevice->ReportFieldInfo.ReportGroup[Index].DataValid)) {
          ReportGroup = &UsbTouchPanelDevice->ReportFieldInfo.ReportGroup[Index];
          break;
        }
      }
      if (!ReportGroup) return EFI_NOT_FOUND;
      if (UsbTouchPanelDevice->ReportId != ReportGroup->Id) {
        //
        // Reset the Mode parameters due to report ID changed
        //
        UsbTouchPanelDevice->ReportId = ReportGroup->Id;
        //
        // Set limit by report descriptor
        //
        UsbTouchPanelDevice->Mode.AbsoluteMinX = ReportGroup->Data.TouchPanel.FieldX.Min;
        UsbTouchPanelDevice->Mode.AbsoluteMinY = ReportGroup->Data.TouchPanel.FieldY.Min;
        UsbTouchPanelDevice->Mode.AbsoluteMaxX = ReportGroup->Data.TouchPanel.FieldX.Max;
        UsbTouchPanelDevice->Mode.AbsoluteMaxY = ReportGroup->Data.TouchPanel.FieldY.Max;
        UsbTouchPanelDevice->Mode.Attributes   = 0;
        if (ReportGroup->Data.TouchPanel.FieldAltActive.DataValid) {
          UsbTouchPanelDevice->Mode.Attributes |= EFI_ABSP_SupportsAltActive;
        }
      }
    }
    if (DataLength < ReportGroup->DataSize) {
      //
      // Partial packet data is not allowed
      //
      return EFI_NOT_FOUND;
    }
    if (ReportGroup->Data.TouchPanel.FieldContactCount.DataValid &&
        GetReportFieldValue (Data, DataLength, &ReportGroup->Data.TouchPanel.FieldContactCount) == 0) {
      //
      // Dummy input data, keep the previous position but clear the button states
      //
      AbsPtrState.CurrentX = UsbTouchPanelDevice->State.CurrentX;
      AbsPtrState.CurrentY = UsbTouchPanelDevice->State.CurrentY;
      AbsPtrState.CurrentZ = 0;
      AbsPtrState.ActiveButtons = 0;
    } else {
      //
      // Got input data, renew the position and button states
      //
      AbsPtrState.CurrentX = (UINT32) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.TouchPanel.FieldX);
      AbsPtrState.CurrentY = (UINT32) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.TouchPanel.FieldY);
      AbsPtrState.CurrentZ = 0;
      AbsPtrState.ActiveButtons = 0;
      if (GetReportFieldValue (Data, DataLength, &ReportGroup->Data.TouchPanel.FieldTouchActive)) {
        AbsPtrState.ActiveButtons |= EFI_ABSP_TouchActive;
      }
      if (GetReportFieldValue (Data, DataLength, &ReportGroup->Data.TouchPanel.FieldAltActive)) {
        AbsPtrState.ActiveButtons |= EFI_ABS_AltActive;
      }
    }
    //
    // Determine whether the current state data differ from the last state data and decide how much
    // data need to be updated if they are different.
    //
    DifferentCase = 0;
    if (UsbTouchPanelDevice->State.CurrentX != AbsPtrState.CurrentX) {
      DifferentCase = 1;
    } else if (UsbTouchPanelDevice->State.CurrentY != AbsPtrState.CurrentY) {
      DifferentCase = 2;
    } else if (UsbTouchPanelDevice->State.CurrentZ != AbsPtrState.CurrentZ) {
      DifferentCase = 3;
    } else if (UsbTouchPanelDevice->State.ActiveButtons != AbsPtrState.ActiveButtons) {
      DifferentCase = 4;
    }
    switch (DifferentCase) {
    case 1:
      UsbTouchPanelDevice->State.CurrentX = AbsPtrState.CurrentX;
    case 2:
      UsbTouchPanelDevice->State.CurrentY = AbsPtrState.CurrentY;
    case 3:
      UsbTouchPanelDevice->State.CurrentZ = AbsPtrState.CurrentZ;
    case 4:
      if ((UsbTouchPanelDevice->State.ActiveButtons != 0) || (AbsPtrState.ActiveButtons != 0)) {
        UsbTouchPanelDevice->State.ActiveButtons = AbsPtrState.ActiveButtons;
        UsbTouchPanelDevice->StateChanged = TRUE;
      }
      break;
    }
  }
  return EFI_SUCCESS;
}

/**

  Get the Touch Panel state, see ABSOLUTE POINTER PROTOCOL.
  
  @param  This                  Protocol instance pointer.
  @param  Touch PanelState      Current Touch Panel state

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_NOT_READY

**/
STATIC
EFI_STATUS
EFIAPI
GetTouchPanelState (
  IN   EFI_ABSOLUTE_POINTER_PROTOCOL    *This,
  OUT  EFI_ABSOLUTE_POINTER_STATE       *TouchPanelState
  )
{
  USB_TOUCH_PANEL_DEV *TouchPanelDev;

  if (TouchPanelState == NULL) {
    return EFI_DEVICE_ERROR;
  }

  TouchPanelDev = USB_TOUCH_PANEL_DEV_FROM_TOUCH_PANEL_PROTOCOL (This);


  if (!TouchPanelDev->StateChanged) {
    return EFI_NOT_READY;
  }

  CopyMem (
    TouchPanelState,
    &TouchPanelDev->State,
    sizeof (EFI_ABSOLUTE_POINTER_STATE)
    );

  TouchPanelDev->StateChanged            = FALSE;

  return EFI_SUCCESS;
}

/**

  Reset the Touch Panel device, see ABSOLUTE POINTER PROTOCOL.
  
  @param  This                  Protocol instance pointer.
  @param  ExtendedVerification  Ignored here
  
  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
UsbTouchPanelReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL      *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  USB_TOUCH_PANEL_DEV       *UsbTouchPanelDevice;
  EFI_USB_IO_PROTOCOL       *UsbIo;

  UsbTouchPanelDevice  = USB_TOUCH_PANEL_DEV_FROM_TOUCH_PANEL_PROTOCOL (This);

  UsbIo           = UsbTouchPanelDevice->UsbIo;

  TouchPanelReportStatusCode (
    UsbTouchPanelDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_TOUCH_PANEL | EFI_P_PC_RESET)
    );

  ZeroMem (
    &UsbTouchPanelDevice->State,
    sizeof (EFI_ABSOLUTE_POINTER_STATE)
    );
  UsbTouchPanelDevice->StateChanged = FALSE;

  return EFI_SUCCESS;
}

/**

  Event notification function for ABSOLUTE_POINTER.WaitForInput event
  Signal the event if there is input from Touch Panel

  @param  Event         Wait Event
  @param  Context       Passed parameter to event handler

**/
STATIC
VOID
EFIAPI
UsbTouchPanelWaitForInput (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  )
{
  USB_TOUCH_PANEL_DEV *UsbTouchPanelDevice;

  UsbTouchPanelDevice = (USB_TOUCH_PANEL_DEV *) Context;

  //
  // Someone is waiting on the TouchPanel event, if there's
  // input from Touch Panel, signal the event
  //
  if (UsbTouchPanelDevice->StateChanged) {
    gBS->SignalEvent (Event);
  }
}

/**

  Timer handler for Delayed Recovery timer.
  
  @param  Event         The Delayed Recovery event.
  @param  Context       Points to the USB_KB_DEV instance.
  
**/   
STATIC
VOID
EFIAPI
UsbTouchPanelRecoveryHandler (
  IN    UINTN                           Event,
  IN    VOID                            *Context
  )
{
  USB_TOUCH_PANEL_DEV   *UsbTouchPanelDevice;
  EFI_USB_IO_PROTOCOL   *UsbIo;
  UINT8                 PacketSize;

  UsbTouchPanelDevice = (USB_TOUCH_PANEL_DEV *) Context;
  UsbTouchPanelDevice->RecoveryPollingHandle = NULL;
  UsbIo = UsbTouchPanelDevice->UsbIo;
  if (UsbIo == NULL) return;
  PacketSize      = (UINT8)UsbTouchPanelDevice->IntEndpointDescriptor.MaxPacketSize;
  if (PacketSize < UsbTouchPanelDevice->ReportFieldInfo.BiggestInputReportGroup->DataSize) {
    PacketSize = (UINT8)UsbTouchPanelDevice->ReportFieldInfo.BiggestInputReportGroup->DataSize;
  }
  UsbIo->UsbAsyncInterruptTransfer (
           UsbIo,
           UsbTouchPanelDevice->IntEndpointDescriptor.EndpointAddress,
           TRUE,
           UsbTouchPanelDevice->IntEndpointDescriptor.Interval,
           PacketSize,
           OnTouchPanelInterruptComplete,
           UsbTouchPanelDevice
           );
}

/**

  Report Status Code in Usb Bot Driver

  @param  DevicePath    Use this to get Device Path
  @param  CodeType      Status Code Type
  @param  CodeValue     Status Code Value

**/
STATIC
VOID
TouchPanelReportStatusCode (
  IN USB_TOUCH_PANEL_DEV                *UsbTouchPanelDevice,
  IN EFI_STATUS_CODE_TYPE               CodeType,
  IN EFI_STATUS_CODE_VALUE              Value
  )
{
  UINTN      Mode;
  BOOLEAN    InSmm;

  UsbTouchPanelDevice->UsbCore->GetMode(&Mode);
  if (Mode != USB_CORE_RUNTIME_MODE) {
    //
    // Only report status code during POST and non-SMM mode
    //
    UsbTouchPanelDevice->UsbCore->IsInSmm(&InSmm);
    if (!InSmm) {
      ReportStatusCodeWithDevicePath (
        CodeType,
        Value,
        UsbTouchPanelDevice->DevicePath
        );
    }
  }
}
