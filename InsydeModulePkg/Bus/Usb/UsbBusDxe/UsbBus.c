/** @file
  USB Bus Driver

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

#include "UsbBus.h"
#include "Hub.h"
#include "UsbHelper.h"

#include <PostCode.h>

#ifndef MDEPKG_NDEBUG
UINTN                                   gUSBDebugLevel  = EFI_D_INFO;
UINTN                                   gUSBErrorLevel  = EFI_D_ERROR;
#endif
//
// The UsbBusProtocol is just used to locate USB_BUS_CONTROLLER
// structure in the UsbBusDriverControllerDriverStop(). Then we can
// Close all opened protocols and release this structure.
//
USB_BUS_PRIVATE                         *mPrivate;
STATIC EFI_GUID                         mUsbBusProtocolGuid = EFI_USB_BUS_PROTOCOL_GUID;
STATIC USB_CHANGE_FEATURE_MAP           mPortChangeMap[8] = {
                                          { USB_PORT_STAT_C_CONNECTION,  EfiUsbPortConnectChange     },
                                          { USB_PORT_STAT_C_ENABLE,      EfiUsbPortEnableChange      },
                                          { USB_PORT_STAT_C_SUSPEND,     EfiUsbPortSuspendChange     },
                                          { USB_PORT_STAT_C_OVERCURRENT, EfiUsbPortOverCurrentChange },
                                          { USB_PORT_STAT_C_RESET,       EfiUsbPortResetChange       },
                                          { USB_PORT_STAT_C_BH_RESET,    EfiUsbPortBhResetChange     },
                                          { USB_PORT_STAT_C_LINK_STATE,  EfiUsbPortLinkStateChange   },
                                          { USB_PORT_STAT_C_CONFIG_ERROR,EfiUsbPortConfigErrorChange },
                                        };
STATIC UINT16                           mSpecialStallDeviceTable[] = {
                                          0x054c, 0x05c9, 1000,  // Sony ODD DRX-S90U, 1 second stall
                                          0x0000, 0x0000, 0
                                        };
extern EFI_USB_IO_PROTOCOL              mUsbIoInterface;
//
// USB Bus Driver Guid
//
EFI_GUID  mUsbBusDriverGuid = {
  0x347b6711, 0xe458, 0x43c8, 0x96, 0x36, 0xf1, 0x73, 0xf6, 0x98, 0xb3, 0x29
};

STATIC
UINTN
DevicePathSize (
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );

STATIC
EFI_DEVICE_PATH_PROTOCOL *
CopyDevicePath (
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *Src1,
  IN     EFI_DEVICE_PATH_PROTOCOL       *Src2
  );

STATIC
VOID
EFIAPI
ConnectControllerCallback (
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );

STATIC
VOID
EFIAPI
FreeUsbIoDeviceCallback (
  IN     UINTN                          Event,
  IN     VOID                           *Context
  );

STATIC
VOID
FreeUsbIoControllerDevice(
  IN     USB_IO_CONTROLLER_DEVICE       *UsbController
  );

STATIC
VOID
InsertUsbIoDevAddressConvertTable(
  IN     USB_IO_DEVICE                  *UsbIoDev,
  IN     BOOLEAN                        IsRootHub
  );

STATIC
VOID
RemoveUsbIoDevAddressConvertTable(
  IN     USB_IO_DEVICE                  *UsbIoDev
  );

STATIC
VOID
RemoveUsbIoControllerDevAddressConvertTable(
  IN     USB_IO_CONTROLLER_DEVICE       *UsbController
  );

//
// USB bus entry point
//
EFI_STATUS
EFIAPI
UsbBusDriverEntryPoint (
  IN     EFI_HANDLE                     ImageHandle,
  IN     EFI_SYSTEM_TABLE               *SystemTable
  );

//
// EFI_DRIVER_BINDING_PROTOCOL Protocol Interface
//
EFI_STATUS
EFIAPI
UsbBusControllerDriverSupported (
  IN     EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN     EFI_HANDLE                     Controller,
  IN     EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbBusControllerDriverStart (
  IN     EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN     EFI_HANDLE                     Controller,
  IN     EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbBusControllerDriverStop (
  IN     EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN     EFI_HANDLE                     Controller,
  IN     UINTN                          NumberOfChildren,
  IN     EFI_HANDLE                     *ChildHandleBuffer
  );

EFI_DRIVER_BINDING_PROTOCOL gUsbBusDriverBinding = {
  UsbBusControllerDriverSupported,
  UsbBusControllerDriverStart,
  UsbBusControllerDriverStop,
  0x18,
  NULL,
  NULL
};

//
// Internal use only
//
STATIC
EFI_STATUS
ReportUsbStatusCode (
  IN     USB_BUS_CONTROLLER_DEVICE      *UsbBusDev,
  IN     EFI_STATUS_CODE_TYPE           Type,
  IN     EFI_STATUS_CODE_VALUE          Code
  );

//
// Supported function
//
VOID
InitializeUsbIoInstance (
  IN     USB_IO_CONTROLLER_DEVICE       *UsbIoController
  );

STATIC
USB_IO_CONTROLLER_DEVICE*
CreateUsbIoControllerDevice (
  IN     USB_BUS_CONTROLLER_DEVICE      *UsbBusDev
  );

//
// USB Device Configuration / Deconfiguration
//
STATIC
EFI_STATUS
UsbDeviceConfiguration (
  IN     USB_IO_CONTROLLER_DEVICE       *ParentHubController,
  IN     EFI_HANDLE                     HostController,
  IN     UINT8                          ParentPort,
  IN     USB_IO_DEVICE                  *UsbIoDev
  );

//
// Usb Bus enumeration function
//
STATIC
VOID
EFIAPI
RootHubEnumeration (
  IN     UINTN                          SyncEnumeration,
  IN     VOID                           *Context
  );

STATIC
VOID
HubEnumeration (
  IN     UINTN                          SyncEnumeration,
  IN     VOID                           *Context
  );

STATIC
EFI_STATUS
UsbSetTransactionTranslator (
  IN     USB_IO_CONTROLLER_DEVICE       *ParentHubController,
  IN     UINT8                          ParentPort,
  IN OUT USB_IO_DEVICE                  *Device
  );

STATIC
EFI_STATUS
ReleasePortToCHC (
  IN     USB_BUS_CONTROLLER_DEVICE      *UsbBusDev,
  IN     UINT8                          PortNum
  );

EFI_STATUS
ResetRootPort (
  IN     USB_IO_CONTROLLER_DEVICE       *UsbIoController,
  IN     UINT8                          PortNum,
  OUT    UINT32                         *HubPortStatus
  );

EFI_STATUS
ResetHubPort (
  IN     USB_IO_CONTROLLER_DEVICE       *UsbIoController,
  IN     UINT8                          PortIndex,
  OUT    UINT32                         *HubPortStatus
  );

/**
  
  Check is it special device which needs extra stall to waiting for further reaction
  
  @param  UsbDevice             Pointer of USB_IO_DEVICE
    
  @retval TRUE                  It is special stall device
  @retval FALSE                 It is not special stall device
  
**/
STATIC
BOOLEAN
IsSpecialStallDevice (
  IN  USB_IO_DEVICE             *UsbDevice
  )
{
  UINTN Index;

  for (Index = 0; mSpecialStallDeviceTable[Index] != 0; Index += 3) {
    if (mSpecialStallDeviceTable[Index] == UsbDevice->DeviceDescriptor.IdVendor && mSpecialStallDeviceTable[Index + 1] == UsbDevice->DeviceDescriptor.IdProduct) {
      mPrivate->UsbCore->Stall (mSpecialStallDeviceTable[Index + 2] * 1000);
      return TRUE;
    }
  }
  return FALSE;
}

/**

  Uses USB I/O to check whether the device is a USB Keyboard device.

  @param  UsbIo                 Points to a USB I/O protocol instance.
  
**/  
STATIC
BOOLEAN
IsPauseInProgress (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  
  if ((mPrivate->UsbCore->IsCsmEnabled() == EFI_SUCCESS) && (PAUSE_IN_PROGRESS)) {
    //
    // Get the Default interface descriptor, currently we
    // assume it is interface 1
    //
    Status = UsbIo->UsbGetInterfaceDescriptor (
                      UsbIo,
                      &InterfaceDescriptor
                      );
    if (!EFI_ERROR (Status) &&
        InterfaceDescriptor.InterfaceClass == CLASS_HID &&
        InterfaceDescriptor.InterfaceSubClass == SUBCLASS_BOOT &&
        InterfaceDescriptor.InterfaceProtocol == PROTOCOL_KEYBOARD
        ) {
      return TRUE;
    }
  }
  return FALSE;
}

/**

  Allocate address for usb device

  @param  AddressPool           Pool of usb device address

  @retval Usb device address

**/
STATIC
UINT8
UsbAllocateDeviceAddress (
  IN UINT8                              DeviceSpeed,
  IN UINT8                              *AddressPool
  )
{
  UINT8 ByteIndex;
  UINT8 BitIndex;
  UINTN MaxSupportedDevices;
  //
  // XHC support 256 devices
  //
  MaxSupportedDevices = (DeviceSpeed == EFI_USB_SPEED_SUPER) ? (256 >> 3) : (128 >> 3);
  for (ByteIndex = 0; ByteIndex < MaxSupportedDevices; ByteIndex++) {
    for (BitIndex = 0; BitIndex < 8; BitIndex++) {
      if ((AddressPool[ByteIndex] & (1 << BitIndex)) == 0) {
        //
        // Found one, covert to address, and mark it use
        //
        AddressPool[ByteIndex] |= (1 << BitIndex);
        return (UINT8) (ByteIndex * 8 + BitIndex);
      }
    }
  }
  return 0;
}

/**

  Free address for usb device

  @param  DevAddress            Usb device address
  @param  AddressPool           Pool of usb device address

**/
STATIC
VOID
UsbFreeDeviceAddress (
  IN UINT8     DevAddress,
  IN UINT8     *AddressPool
  )
{
  UINT8 WhichByte;
  UINT8 WhichBit;
  //
  // Locate the position
  //
  WhichByte = (UINT8) (DevAddress / 8);
  WhichBit  = (UINT8) (DevAddress & 0x7);

  AddressPool[WhichByte] &= (~(1 << WhichBit));
}

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS
  @retval others

**/
EFI_STATUS
EFIAPI
UsbBusDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_USB_CORE_PROTOCOL                 *UsbCore;
  BOOLEAN                               InSmram;
  EFI_HANDLE                            Handle;
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
  // Check the phase of instance
  //
  UsbCore->IsInSmm (&InSmram);
  if (!InSmram) {
    //
    // Now in boot service
    //
    Status = UsbCore->AllocateBuffer(
                        sizeof (USB_BUS_PRIVATE),
                        ALIGNMENT_32,
                        (VOID **)&mPrivate
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mPrivate->Signature     = USB_BUS_SIGNATURE;
    mPrivate->UsbCore       = UsbCore;
    mPrivate->NumLowSpeedHc = -1;
    CopyMem (
      &mPrivate->UsbIoInterface,
      &mUsbIoInterface,
      sizeof (EFI_USB_IO_PROTOCOL)
      );
    //
    // Install protocol for internal use. Pass in a NULL to install to a new handle
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &mUsbBusDriverGuid,
                    EFI_NATIVE_INTERFACE,
                    mPrivate
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Install protocols
    //
    Status = EfiLibInstallDriverBindingComponentName2 (
               ImageHandle,
               SystemTable,
               &gUsbBusDriverBinding,
               ImageHandle,
               &gUsbBusComponentName,
               &gUsbBusComponentName2
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Register module in DXE instance
    //
    UsbCore->ModuleRegistration (ImageHandle);
  } else {
    //
    // SMM instance, disable DEBUG message out
    //
#ifndef MDEPKG_NDEBUG
    gUSBDebugLevel      = 0;
    gUSBErrorLevel      = 0;
#endif
    //
    // Now in SMM, get private storage first
    //
    Status = gBS->LocateProtocol (
                    &mUsbBusDriverGuid,
                    NULL,
                    (VOID **)&mPrivate
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Register module in SMM instance
    //
    UsbCore->ModuleRegistration (ImageHandle);
    //
    // Setup Smm address convert table for Smm security policy
    //
    UsbCore->InsertAddressConvertTable (
               ACT_FUNCTION_POINTER,
               &mPrivate->UsbIoInterface,
               sizeof(EFI_USB_IO_PROTOCOL) / sizeof(VOID*)
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_BODY,
               mPrivate,
               sizeof (USB_BUS_PRIVATE)
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &mPrivate,
               1
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &mPrivate->UsbCore,
               1
               );
  }
  return Status;
}

/**

  Test to see if this driver supports ControllerHandle. Any ControllerHandle
  that has UsbHcProtocol installed will be supported.

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to test
  @param  RemainingDevicePath   Device Path Protocol instance pointer

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_UNSUPPORTED       This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbBusControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  )
{
  EFI_STATUS                 Status;
  EFI_DEVICE_PATH_PROTOCOL   *ParentDevicePath;
  EFI_USB3_HC_PROTOCOL       *Usb3Hc;
  EFI_DEV_PATH_PTR           Node;
  //
  // Check Device Path
  //
  if (RemainingDevicePath != NULL) {
    Node.DevPath = RemainingDevicePath;
    if (Node.DevPath->Type != MESSAGING_DEVICE_PATH ||
        Node.DevPath->SubType != MSG_USB_DP         ||
        DevicePathNodeLength(Node.DevPath) != sizeof(USB_DEVICE_PATH)) {
      return EFI_UNSUPPORTED;
    }
  }
  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    return EFI_ALREADY_STARTED;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  //
  // Check whether USB Host Controller Protocol is already
  // installed on this handle. If it is installed, we can start
  // USB Bus Driver now.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsb3HcProtocolGuid,
                  (VOID **)&Usb3Hc,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->CloseProtocol (
    Controller,
    &gEfiUsb3HcProtocolGuid,
    This->DriverBindingHandle,
    Controller
    );

  return EFI_SUCCESS;
}

/**

  Starting the Usb Bus Driver

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to test
  @param  RemainingDevicePath   Not used

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_DEVICE_ERROR      This driver cannot be started due to device Error
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory resources

**/
EFI_STATUS
EFIAPI
UsbBusControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  USB_IO_DEVICE             *RootHub;
  USB_IO_CONTROLLER_DEVICE  *RootHubController;
  UINT8                     MaxSpeed;
  UINT8                     PortNumber;
  UINT8                     Is64BitCapable;
  EFI_USB3_HC_PROTOCOL      *Usb3Hc;
  EFI_USB3_HC_PROTOCOL      *Usb3LowSpeedHc;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINT8                     UsbClassCReg[4];
  UINTN                     Index;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  USB_HC_DESC               *HcDesc;
  UINTN                     SegNum;
  UINTN                     BusNum;
  UINTN                     DevNum;
  UINTN                     FuncNum;
  //
  // Locate the Host Controller Interface
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsb3HcProtocolGuid,
                  (VOID **)&Usb3Hc,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto exit;
  }
  Usb3Hc->GetCapability (
            Usb3Hc,
            &MaxSpeed,
            &PortNumber,
            &Is64BitCapable
            );
  UsbCore = mPrivate->UsbCore;
  //
  // For USB2 HC with CHC, we should waiting for CHC installed for the USB1.1 port routing
  //
  if (mPrivate->NumLowSpeedHc == -1) {
    mPrivate->NumLowSpeedHc = 0;
    //
    // Collect whole handles of low speed HC on first time 
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (EFI_ERROR (Status)) {
      Status = EFI_UNSUPPORTED;
      goto close_hc_protocol;
    }
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **)&PciIo
                      );
      if (EFI_ERROR (Status)) {
        Status = EFI_UNSUPPORTED;
        goto close_hc_protocol;
      }
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint8,
                            9,
                            3,
                            UsbClassCReg
                            );
      if (EFI_ERROR (Status)) {
        Status = EFI_UNSUPPORTED;
        goto close_hc_protocol;
      }
      //
      // Test whether the controller is low speed HC or not
      //
      if (UsbClassCReg[2] == 0x0C && UsbClassCReg[1] == 0x03 && UsbClassCReg[0] <= 0x10) {
        PciIo->GetLocation (
                 PciIo,
                 &SegNum,
                 &BusNum,
                 &DevNum,
                 &FuncNum
                 );
        HcDesc = &mPrivate->LowSpeedHc[mPrivate->NumLowSpeedHc];
        HcDesc->Controller = HandleBuffer[Index];
        HcDesc->SegNum     = (UINT8)SegNum;
        HcDesc->BusNum     = (UINT8)BusNum;
        HcDesc->DevNum     = (UINT8)DevNum;
        HcDesc->FuncNum    = (UINT8)FuncNum;
        mPrivate->NumLowSpeedHc ++;
      }
    }
    gBS->FreePool (HandleBuffer);
  }
  if (MaxSpeed == EFI_USB_SPEED_HIGH) {
    Status = gBS->HandleProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (EFI_ERROR (Status)) {
      Status = EFI_UNSUPPORTED;
      goto close_hc_protocol;
    }
    PciIo->GetLocation (
             PciIo,
             &SegNum,
             &BusNum,
             &DevNum,
             &FuncNum
             );
    for (Index = 0, HcDesc = mPrivate->LowSpeedHc; Index < (UINTN)mPrivate->NumLowSpeedHc; Index ++, HcDesc ++) {
      if (HcDesc->SegNum == (UINT8)SegNum && HcDesc->BusNum == (UINT8)BusNum && HcDesc->DevNum == (UINT8)DevNum) {
        //
        // Check is the HC driver been installed in the controller 
        //
        Status = gBS->HandleProtocol (
                        HcDesc->Controller,
                        &gEfiUsb3HcProtocolGuid,
                        (VOID **)&Usb3LowSpeedHc
                        );
        if (EFI_ERROR (Status)) {
          //
          // Try to connect the HC
          //
          gBS->ConnectController (
                 HcDesc->Controller,
                 NULL,
                 NULL,
                 FALSE
                 );
        }
      }
    }
  }
  //
  // Allocate USB_BUS_CONTROLLER_DEVICE structure
  //
  Status = UsbCore->AllocateBuffer (
                      sizeof (USB_BUS_CONTROLLER_DEVICE),
                      ALIGNMENT_32,
                      (VOID **) &UsbBusDev
                      );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto close_hc_protocol;
  }
  UsbBusDev->Signature       = USB_BUS_DEVICE_SIGNATURE;
  UsbBusDev->AddressPool[0]  = 1;
  UsbBusDev->Usb3HCInterface = Usb3Hc;
  //
  // Get the Device Path Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &UsbBusDev->DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    goto free_usbbusdev;
  }
  //
  // Attach EFI_USB_BUS_PROTOCOL to controller handle,
  // for locate UsbBusDev later
  //
  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &mUsbBusProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &UsbBusDev->BusIdentify
                  );

  if (EFI_ERROR (Status)) {
    goto close_devicepath_protocol;
  }
  //
  // Add root hub to the tree
  //
  Status = UsbCore->AllocateBuffer (
                      sizeof (USB_IO_DEVICE),
                      ALIGNMENT_32,
                      (VOID **) &RootHub
                      );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto uninstall_protocol;
  }
  RootHub->BusController  = UsbBusDev;
  RootHub->DeviceAddress  = UsbAllocateDeviceAddress (
                              UsbBusDev->Root->DeviceSpeed,
                              UsbBusDev->AddressPool
                              );
  UsbBusDev->Root         = RootHub;
  //
  // Allocate Root Hub Controller
  //
  RootHubController = CreateUsbIoControllerDevice (
                        UsbBusDev
                        );
  if (RootHubController == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto free_roothub;
  }
  RootHubController->DownstreamPorts  = PortNumber;
  RootHubController->UsbDevice        = RootHub;
  RootHubController->IsUsbHub         = TRUE;
  //
  // In order to prevent the UEFI OS boot hang, allocate the buffer of DevicePath in BS for UEFI OS
  //
  RootHubController->UefiDevicePath   = CopyDevicePath(NULL, UsbBusDev->DevicePath, NULL);
  //
  // Allocate the buffer of DevicePath in reserved memory for legacy mode hot-plug
  //
  RootHubController->DevicePath       = CopyDevicePath(UsbCore, UsbBusDev->DevicePath, NULL);
  RootHubController->HostController   = Controller;
  if (RemainingDevicePath != NULL) {
    RootHubController->RemainingDevicePath = CopyDevicePath(UsbCore, (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)RemainingDevicePath + sizeof(USB_DEVICE_PATH)), NULL);
  }

  RootHub->NumOfInterfaces            = 1;
  RootHub->UsbController[0]           = RootHubController;
  RootHub->DeviceSpeed                = MaxSpeed;
  //
  // Report Status Code here since we will reset the host controller
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_IO_BUS_USB | EFI_IOB_PC_RESET,
    UsbBusDev->DevicePath
    );
  //
  // Start USB Host Controller
  //
  UsbBusDev->Usb3HCInterface->SetState (
                                UsbBusDev->Usb3HCInterface,
                                EfiUsbHcStateOperational
                                );
  //
  // Do root hub enumeration at first time (high speed only)
  //
  RootHubEnumeration(1, RootHubController);
  //
  // Register Callback for HC_EVENT_CONNECT_CHANGE Event
  //
  UsbBusDev->Usb3HCInterface->RegisterEvent (
                                UsbBusDev->Usb3HCInterface,
                                HC_EVENT_CONNECT_CHANGE,
                                RootHubEnumeration,
                                RootHubController,
                                &RootHubController->HubNotify
                                );
  //
  // Setup Smm address convert table for Smm security policy
  //
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_BODY,
             UsbBusDev,
             sizeof (USB_BUS_CONTROLLER_DEVICE)
             );
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbBusDev->Usb3HCInterface,
             1
             );
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbBusDev->Root,
             1
             );
  //
  // Insert IoDevice and attached IoDeviceController into SMM convert table
  //
  InsertUsbIoDevAddressConvertTable(RootHub, TRUE);
  Status = EFI_SUCCESS;
  goto exit;

free_roothub:
  UsbCore->FreeBuffer (
             sizeof (USB_IO_DEVICE),
             RootHub
             );
uninstall_protocol:
  gBS->UninstallProtocolInterface (
         Controller,
         &mUsbBusProtocolGuid,
         &UsbBusDev->BusIdentify
         );
close_devicepath_protocol:
  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
free_usbbusdev:
  UsbCore->FreeBuffer (
             sizeof (USB_BUS_CONTROLLER_DEVICE),
             UsbBusDev
             );
close_hc_protocol:
  gBS->CloseProtocol (
         Controller,
         &gEfiUsb3HcProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
exit:
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
UsbBusControllerDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN UINTN                           NumberOfChildren,
  IN EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS                Status;
  EFI_USB_CORE_PROTOCOL     *UsbCore;  
  USB_IO_DEVICE             *RootHub;
  USB_IO_CONTROLLER_DEVICE  *RootHubController;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  EFI_USB_BUS_PROTOCOL      *UsbIdentifier;
  UINT8                     Index2;
  USB_IO_CONTROLLER_DEVICE  *UsbController;
  USB_IO_DEVICE             *UsbIoDev;
  USB_IO_CONTROLLER_DEVICE  *HubController;
  UINTN                     Index;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  //
  // Initialize global variables
  //
  UsbCore = mPrivate->UsbCore;
  if (NumberOfChildren > 0) {

    for (Index = 0; Index < NumberOfChildren; Index++) {
      Status = gBS->OpenProtocol (
                      ChildHandleBuffer[Index],
                      &gEfiUsbIoProtocolGuid,
                      (VOID **) &UsbIo,
                      This->DriverBindingHandle,
                      Controller,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
      if (EFI_ERROR (Status)) {
        //
        // We are here since the handle passed in does not support
        // UsbIo protocol. There are several reasons that will cause
        // this.
        // For combo device such as keyboard, it may have 2 devices
        // in one, namely, keyboard and mouse. If we deconfigure one
        // of them, the other will be freed at the same time. This will
        // cause the status error. But this is the correct behavior.
        // For hub device, if we deconfigure hub first, the other chile
        // device will be disconnected also, this will also provide us
        // a status error. Now we will only report EFI_SUCCESS since Uhc
        // driver will be disconnected at the second time.(pls see
        // CoreDisconnectController for details)
        //
        continue;
      }

      UsbController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (UsbIo);
      UsbIoDev      = UsbController->UsbDevice;
      HubController = UsbController->Parent;
      UsbDeviceDeConfiguration (UsbIoDev);
      for (Index2 = 0; Index2 < HubController->DownstreamPorts; Index2++) {
        if (HubController->Children[Index2] == UsbIoDev) {
          HubController->Children[Index2] = NULL;
        }
      }
    }

    return EFI_SUCCESS;
  }
  //
  // Get the USB_BUS_CONTROLLER_DEVICE
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &mUsbBusProtocolGuid,
                  (VOID **) &UsbIdentifier,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  UsbBusDev = USB_BUS_CONTROLLER_DEVICE_FROM_THIS (UsbIdentifier);

  //
  // Stop USB Host Controller
  //
  // Report Status Code here since we will reset the host controller
  //
  ReportUsbStatusCode (
    UsbBusDev,
    EFI_PROGRESS_CODE,
    EFI_IO_BUS_USB | EFI_IOB_PC_RESET
    );
  //
  // Uninstall USB Bus Protocol
  //
  gBS->UninstallProtocolInterface (
        Controller,
        &mUsbBusProtocolGuid,
        &UsbBusDev->BusIdentify
        );

  UsbBusDev->Usb3HCInterface->SetState (
                                UsbBusDev->Usb3HCInterface,
                                EfiUsbHcStateHalt
                                );
  //
  // Deconfiguration all its devices
  //
  RootHub           = UsbBusDev->Root;
  RootHubController = RootHub->UsbController[0];
  UsbBusDev->Usb3HCInterface->UnregisterEvent (
                                UsbBusDev->Usb3HCInterface,
                                RootHubController->HubNotify
                                );
  for (Index2 = 0; Index2 < RootHubController->DownstreamPorts; Index2++) {
    if (RootHubController->Children[Index2]) {
      UsbDeviceDeConfiguration (RootHubController->Children[Index2]);
      RootHubController->Children[Index2] = NULL;
    }
  }
  if (RootHubController->UefiDevicePath != NULL) {
    gBS->FreePool(RootHubController->UefiDevicePath);
  }
  if (RootHubController->DevicePath != NULL) {
    UsbCore->FreeBuffer (
               DevicePathSize (RootHubController->DevicePath),
               RootHubController->DevicePath
               );
  }
  if (RootHubController->RemainingDevicePath != NULL) {
    UsbCore->FreeBuffer (
               DevicePathSize(RootHubController->RemainingDevicePath),
               RootHubController->RemainingDevicePath
               );
  }
  UsbCore->FreeBuffer (
             sizeof (USB_IO_CONTROLLER_DEVICE),
             RootHubController
             );
  UsbCore->FreeBuffer (
             sizeof (USB_IO_DEVICE),
             RootHub
             );
  UsbCore->FreeBuffer (
             sizeof (USB_BUS_CONTROLLER_DEVICE),
             UsbBusDev
             );
  //
  // Remove Smm address convert table
  //
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_BODY,
             UsbBusDev
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbBusDev->Usb3HCInterface
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbBusDev->Root
             );
  //
  // Remove IoDevice and attached IoDeviceController from SMM convert table
  //
  RemoveUsbIoDevAddressConvertTable(RootHub);
  RemoveUsbIoControllerDevAddressConvertTable(RootHubController);
  //
  // Close USB_HC_PROTOCOL & DEVICE_PATH_PROTOCOL
  // Opened by this Controller
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiUsb3HcProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return EFI_SUCCESS;
}

/**

  Configurate a new device attached to the usb bus

  @param  ParentHubController   Parent Hub which this device is connected.
  @param  HostController        Host Controller handle
  @param  ParentPort            Parent Hub port which this device is connected.
  @param  UsbIoDev              The device to be configured.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_OUT_OF_RESOURCES

**/
STATIC
EFI_STATUS
UsbDeviceConfiguration (
  IN     USB_IO_CONTROLLER_DEVICE       *ParentHubController,
  IN     EFI_HANDLE                     HostController,
  IN     UINT8                          ParentPort,
  IN     USB_IO_DEVICE                  *UsbIoDev
  )
{
  UINT8                     DevAddress;
  UINT8                     Index;
  EFI_STATUS                Status;
  UINT32                    Result;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINT8                     NumOfInterface;
  USB_IO_CONTROLLER_DEVICE  *FirstController;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT8                     UsbDevicePath[sizeof(USB_DEVICE_PATH) + END_DEVICE_PATH_LENGTH];
  USB_DEVICE_PATH           *UsbNode;
  EFI_USB_HID_DESCRIPTOR    *HidDescriptor;
  
  POST_CODE (BDS_CONNECT_USB_BUS);

  UsbBusDev = UsbIoDev->BusController;
  UsbCore   = mPrivate->UsbCore;
  //
  // Build the Usb device path
  //
  UsbNode = (USB_DEVICE_PATH*)UsbDevicePath;
  UsbNode->Header.Type      = MESSAGING_DEVICE_PATH;
  UsbNode->Header.SubType   = MSG_USB_DP;
  SetDevicePathNodeLength (&UsbNode->Header, sizeof(USB_DEVICE_PATH));
  UsbNode->InterfaceNumber  = 0;
  UsbNode->ParentPortNumber = ParentPort;
  SetDevicePathEndNode ((EFI_DEVICE_PATH_PROTOCOL*)(UsbNode + 1));
  DevicePath = CopyDevicePath(UsbCore, ParentHubController->DevicePath, &UsbNode->Header);
  UsbSetTransactionTranslator (
    ParentHubController,
    ParentPort,
    UsbIoDev
    );

  //
  // Since a USB device must have at least one interface,
  // so create this instance first
  //
  FirstController                   = CreateUsbIoControllerDevice (UsbBusDev);
  if (FirstController == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  FirstController->UsbDevice        = UsbIoDev;
  UsbIoDev->UsbController[0]        = FirstController;
  FirstController->InterfaceNumber  = 0;
  FirstController->ParentPort       = ParentPort;
  FirstController->Parent           = ParentHubController;
  FirstController->HostController   = HostController;

  InitializeUsbIoInstance (FirstController);

  DEBUG ((gUSBDebugLevel, "Configuration Usb Device at 0x%x...\n", ParentPort));

  //
  // Ensure we used the correctly USB I/O instance
  //
  UsbIo = &FirstController->UsbIo;

  //
  // First retrieve the 1st 8 bytes of
  // in order to get the MaxPacketSize for Endpoint 0
  //
  for (Index = 0; Index < 3; Index++) {
    UsbIoDev->DeviceDescriptor.MaxPacketSize0 = 8;
    //
    // Some wireless keyboard doesn't accept 8 bytes length for GetDescriptor command,
    // set the length to sizeof (EFI_USB_DEVICE_DESCRIPTOR) for it
    //
    Status = UsbCore->UsbGetDescriptor (
                        UsbIo,
                        (USB_DESC_TYPE_DEVICE << 8),
                        0,
                        (UsbIoDev->DeviceSpeed == EFI_USB_SPEED_LOW) ? sizeof (EFI_USB_DEVICE_DESCRIPTOR) : 8,
                        &UsbIoDev->DeviceDescriptor,
                        &Result
                        );
    if (!EFI_ERROR (Status)) {
      DEBUG (
        (gUSBDebugLevel,
        "Get Device Descriptor Success, MaxPacketSize0 = 0x%x\n",
        UsbIoDev->DeviceDescriptor.MaxPacketSize0)
        );
      break;
    }
    //
    // Stall 500ms if GetDescriptor error
    //
    UsbCore->Stall(500 * 1000);
  }

  if (Index == 3) {
    ReportUsbStatusCode (
      UsbBusDev,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_IO_BUS_USB | EFI_IOB_EC_READ_ERROR
      );
    DEBUG ((gUSBErrorLevel, "Get Device Descriptor Fail when configing\n"));
    Status = EFI_DEVICE_ERROR;
    goto free_first_controller;
  }

  DevAddress = UsbAllocateDeviceAddress (
                 UsbIoDev->BusController->Root->DeviceSpeed,
                 UsbIoDev->BusController->AddressPool
                 );
  if (DevAddress == 0) {
    DEBUG ((gUSBErrorLevel, "Cannot allocate address\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto free_first_controller;
  }
  //
  // Stall 1ms after GetDescriptor for specific device compatibility(0.1ms minimum)
  //
  UsbCore->Stall (1000);
  
  for (Index = 0, Status = EFI_DEVICE_ERROR; Index < 3 && EFI_ERROR (Status); Index++) {
    Status = UsbSetDeviceAddress (UsbIo, DevAddress, &Result);
    if (EFI_ERROR (Status)) {
      //
      // Stall 500ms if SetAddress error
      //
      UsbCore->Stall (500 * 1000);
    }
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((gUSBErrorLevel, "Set address error\n"));
    ReportUsbStatusCode (
      UsbBusDev,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_IO_BUS_USB | EFI_IOB_EC_WRITE_ERROR
      );
    //
    // Free device address which allocated to the device
    //
    UsbFreeDeviceAddress (
      DevAddress,
      UsbIoDev->BusController->AddressPool
      );
    Status = EFI_DEVICE_ERROR;
    goto free_first_controller;
  }

  UsbIoDev->DeviceAddress = DevAddress;

  //
  // SetAddress Complete Time by Spec, Max 50ms
  //
  UsbCore->Stall (10 * 1000);

  //
  // Get the whole device descriptor
  //
  Status = UsbCore->UsbGetDescriptor (
                      UsbIo,
                      (USB_DESC_TYPE_DEVICE << 8),
                      0,
                      sizeof (EFI_USB_DEVICE_DESCRIPTOR),
                      &UsbIoDev->DeviceDescriptor,
                      &Result
                      );

  if (EFI_ERROR (Status)) {
    DEBUG ((gUSBErrorLevel, "Get whole Device Descriptor error\n"));
    ReportUsbStatusCode (
      UsbBusDev,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_IO_BUS_USB | EFI_IOB_EC_READ_ERROR
      );
    //
    // Return EFI_UNSUPPORTED to keep the device address
    //
    Status = EFI_UNSUPPORTED;
    goto free_first_controller;
  }
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 0)
  //
  // Temporary set IsConfigured to TRUE for CheckIgnoredDevice
  //
  UsbIoDev->IsConfigured = TRUE;
  //
  // Knock off the ignored device after get VID/PID
  //
  Status = UsbCore->CheckIgnoredDevice(DevicePath, UsbIo);
  if (EFI_ERROR(Status)) {
    //
    // Return EFI_UNSUPPORTED to keep the device address
    //
    Status = EFI_UNSUPPORTED;
    goto free_first_controller;
  }
  //
  // Set IsConfigured back
  //
  UsbIoDev->IsConfigured = FALSE;
#endif
  //
  // Get & parse all configurations for this device, including
  // all configuration descriptors, all interface descriptors, all
  // endpoint descriptors
  //
  Status = UsbGetAllConfigurations (UsbIoDev);

  if (EFI_ERROR (Status)) {
    DEBUG ((gUSBErrorLevel, "Failed to get device configuration\n"));
    ReportUsbStatusCode (
      UsbBusDev,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_IO_BUS_USB | EFI_IOB_EC_READ_ERROR
      );
    //
    // Return EFI_UNSUPPORTED to keep the device address
    //
    Status = EFI_UNSUPPORTED;
    goto free_first_controller;
  }
  //
  // Stall 1ms after GetDescriptor for specific device compatibility(0.1ms minimum)
  //
  UsbCore->Stall (1000);
  //
  // Set the 1st configuration value
  //
  Status = UsbSetDefaultConfiguration (UsbIoDev);
  if (EFI_ERROR (Status)) {
    DEBUG ((gUSBErrorLevel, "Failed to set device configuration\n"));
    ReportUsbStatusCode (
      UsbBusDev,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      EFI_IO_BUS_USB | EFI_IOB_EC_WRITE_ERROR
      );
    //
    // Return EFI_UNSUPPORTED to keep the device address
    //
    Status = EFI_UNSUPPORTED;
    goto free_configurations;
  }

  UsbIoDev->IsConfigured = TRUE;
  //
  // Create USB_IO_CONTROLLER_DEVICE for
  // each detected interface
  //
  FirstController->CurrentConfigValue = UsbIoDev->ActiveConfig->CongfigDescriptor.ConfigurationValue;
  FirstController->AlternateSetting   = GetFirstAlternateSetting (&FirstController->UsbIo);
  //
  // Register the HID descriptor to UsbCore if available
  //
  if ((HidDescriptor = GetHidDescriptor (&FirstController->UsbIo)) != NULL) {
    UsbCore->RegisterHidDescriptor (&FirstController->UsbIo, HidDescriptor);
  }
  NumOfInterface                      = UsbIoDev->ActiveConfig->CongfigDescriptor.NumInterfaces;
  //
  // Maximum interfaces checking
  //
  if (NumOfInterface > USB_MAXINTERFACES) {
    NumOfInterface = USB_MAXINTERFACES;
  }
  UsbIoDev->NumOfInterfaces           = NumOfInterface;
  //
  // Backup the device pathes 
  //
  FirstController->DevicePath = DevicePath;
  if (FirstController->Parent->RemainingDevicePath) {
    FirstController->RemainingDevicePath = CopyDevicePath(UsbCore, FirstController->Parent->RemainingDevicePath, NULL);
  }
  for (Index = 1; Index < NumOfInterface; Index++) {
    UsbIoController                     = CreateUsbIoControllerDevice (UsbBusDev);
    if (UsbIoController == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto free_configurations;
    }
    UsbIoController->UsbDevice          = UsbIoDev;
    UsbIoController->CurrentConfigValue = UsbIoDev->ActiveConfig->CongfigDescriptor.ConfigurationValue;
    UsbIoController->InterfaceNumber    = UsbNode->InterfaceNumber = Index;
    UsbIoDev->UsbController[Index]      = UsbIoController;
    UsbIoController->ParentPort         = ParentPort;
    UsbIoController->Parent             = ParentHubController;
    UsbIoController->HostController     = HostController;
    //
    // First copy the USB_IO Protocol instance
    //
    InitializeUsbIoInstance (UsbIoController);
    UsbIoController->AlternateSetting   = GetFirstAlternateSetting (&UsbIoController->UsbIo);
    //
    // Register the HID descriptor to UsbCore if available
    //
    if ((HidDescriptor = GetHidDescriptor (&UsbIoController->UsbIo)) != NULL) {
      UsbCore->RegisterHidDescriptor (&UsbIoController->UsbIo, HidDescriptor);
    }
    //
    // Allocate new DevicePath for this interface
    //
    DevicePath = CopyDevicePath(UsbCore, ParentHubController->DevicePath, &UsbNode->Header);
    //
    // Backup the device pathes 
    //
    UsbIoController->DevicePath = DevicePath;
    if (UsbIoController->Parent->RemainingDevicePath) {
      UsbIoController->RemainingDevicePath = CopyDevicePath(UsbCore, UsbIoController->Parent->RemainingDevicePath, NULL);
    }
  }
  //
  // Insert IoDevice and attached IoDeviceController into SMM convert table
  //
  InsertUsbIoDevAddressConvertTable(UsbIoDev, FALSE);
  Status = EFI_SUCCESS;
  goto exit;
free_configurations:
  //
  // Free all resouces allocated for all its configurations
  //
  UsbDestroyAllConfiguration (UsbIoDev);
free_first_controller:
  UsbCore->FreeBuffer (
             DevicePathSize(DevicePath),
             DevicePath
             );
  UsbCore->FreeBuffer(
             sizeof (USB_IO_CONTROLLER_DEVICE),
             FirstController
             );
  UsbIoDev->UsbController[0] = NULL;
exit:
  return Status;
}

/**

  Remove Device, Device Handles, Uninstall Protocols.

  @param  UsbIoDev              The device to be deconfigured.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
UsbDeviceDeConfiguration (
  IN USB_IO_DEVICE     *UsbIoDev
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbController;
  UINT8                     index;
  USB_IO_DEVICE             *ChildDevice;
  UINT8                     Index;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  UINTN                     Mode;
  BOOLEAN                   PauseInProgress;

  //
  // Double check UsbIoDevice exists
  //
  if (UsbIoDev == NULL) {
    return EFI_SUCCESS;
  }

  UsbBusDev       = UsbIoDev->BusController;
  UsbCore         = mPrivate->UsbCore;
  PauseInProgress = FALSE;
  UsbCore->GetMode(&Mode);
  //
  // Disable to device(used for Xhci)
  //
  UsbBusDev->Usb3HCInterface->CancelIo (
                                UsbBusDev->Usb3HCInterface,
                                UsbIoDev->DeviceAddress,
                                0,
                                0,
                                NULL
                                );
  for (index = 0; index < UsbIoDev->NumOfInterfaces; index++) {
    //
    // Check if it is a hub, if so, de configuration all its
    // downstream ports
    //
    UsbController = UsbIoDev->UsbController[index];

    //
    // Check the controller pointer
    //
    if (UsbController == NULL) {
      continue;
    }
    //
    // Set the inactive flag to signal this device is going to be removed 
    //
    UsbController->Inactive = TRUE;
    if (UsbController->IsUsbHub) {
      DEBUG ((gUSBDebugLevel, "Hub Deconfig, First Deconfig its downstream ports\n"));
      //
      // First Remove interrupt transfer request for the status
      // change port
      //
      UsbIo = &UsbController->UsbIo;
      UsbIo->UsbAsyncInterruptTransfer (
              UsbIo,
              UsbController->HubEndpointAddress,
              FALSE,
              0,
              0,
              NULL,
              NULL
              );

      if (NULL != UsbController->HubNotify) {
        UsbBusDev->Usb3HCInterface->UnregisterEvent(
                                      UsbBusDev->Usb3HCInterface,
                                      UsbController->HubNotify
                                      );
      }

      for (Index = 0; Index < UsbController->DownstreamPorts; Index++) {
        if (UsbController->Children[Index]) {
          ChildDevice = UsbController->Children[Index];
          UsbDeviceDeConfiguration (ChildDevice);
          UsbController->Children[Index] = NULL;
        }
      }
    }
    //
    // Unregister HID descriptor whether available or not
    //
    UsbCore->UnregisterHidDescriptor (&UsbController->UsbIo);
    if (Mode != USB_CORE_RUNTIME_MODE) {
      //
      // Disconnect Usb devices during native Non-SMM mode for hot plug
      //
      if (IsPauseInProgress(&UsbController->UsbIo)) {
        //
        // Disconnecting directly due to pause loop is under CSM16 and no risk on
        // UEFI device conntection
        //
        gBS->RestoreTPL (TPL_NOTIFY);
        ConnectControllerCallback(
          DISCONNECT_CONTROLLER,
          UsbController
          );
        gBS->RaiseTPL (TPL_HIGH_LEVEL);
        PauseInProgress = TRUE;
      } else {
        //
        // Use timer callback to disconnect the device to make the disconnection under
        // appropriate TPL 
        //
        UsbCore->RegisterNonSmmCallback(
                   ConnectControllerCallback,
                   DISCONNECT_CONTROLLER,
                   UsbController
                   );
      }
    } else {
      //
      // Disconnect Usb devices during legacy mode for hot plug
      //
      UsbCore->DisconnectUsbDevices(
                 &UsbController->UsbIo
                 );
      //
      // Free IoControllerDevice
      //
      FreeUsbIoControllerDevice(UsbController);
    }
    UsbIoDev->UsbController[index] = NULL;
  }
  if (Mode != USB_CORE_RUNTIME_MODE && !PauseInProgress) {
    //
    // Use timer callback to free the allocated resources to make the operation under
    // appropriate TPL 
    //
    UsbCore->RegisterNonSmmCallback(
               FreeUsbIoDeviceCallback,
               0,
               UsbIoDev
               );
  } else {
    //
    // Free UsbIo device resources immediately
    //
    FreeUsbIoDeviceCallback(0, UsbIoDev);
  }
  return EFI_SUCCESS;
}
/**

  Whenever hub interrupt occurs, this routine will be called to check
  which event happens.

  @param  Data                  Hub interrupt transfer data.
  @param  DataLength            The length of the Data.
  @param  Context               Hub Controller Device.
  @param  Result                Hub interrupt transfer status.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
EFIAPI
OnHubInterruptComplete (
  IN  VOID          *Data,
  IN  UINTN         DataLength,
  IN  VOID          *Context,
  IN  UINT32        Result
  )
{
  USB_IO_CONTROLLER_DEVICE  *HubController;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINT32                    UsbResult;
  BOOLEAN                   Disconnected;
  EFI_STATUS                Status;

  HubController = (USB_IO_CONTROLLER_DEVICE *) Context;
  UsbIo         = &HubController->UsbIo;

  //
  // If something error in this interrupt transfer,
  //
  if (Result != EFI_USB_NOERROR) {
    if ((Result & EFI_USB_ERR_STALL) == EFI_USB_ERR_STALL) {
      mPrivate->UsbCore->UsbClearEndpointHalt (
                           UsbIo,
                           HubController->HubEndpointAddress,
                           &UsbResult
                           );
    }
    //
    // Delete & Submit this interrupt again
    //
    UsbIo->UsbAsyncInterruptTransfer (
             UsbIo,
             HubController->HubEndpointAddress,
             FALSE,
             0,
             0,
             NULL,
             NULL
             );

    //
    // try to detect if the hub itself was disconnected or not
    //
    Status = IsDeviceDisconnected (
              HubController,
              &Disconnected
              );

    if (!EFI_ERROR (Status) && Disconnected == TRUE) {
      DEBUG ((gUSBErrorLevel, "Hub is disconnected\n"));
      return EFI_DEVICE_ERROR;
    }
    //
    // Hub ports < 7
    //
    UsbIo->UsbAsyncInterruptTransfer (
             UsbIo,
             HubController->HubEndpointAddress,
             TRUE,
             128,
             (HubController->DownstreamPorts / 8) + 1,
             OnHubInterruptComplete,
             HubController
             );

    return EFI_DEVICE_ERROR;
  }

  if (DataLength == 0 || Data == NULL) {
    return EFI_SUCCESS;
  }
  CopyMem (
    &HubController->StatusChangePort,
    Data,
    (HubController->DownstreamPorts / 8) + 1
    );
  HubEnumeration(0, HubController);
  return EFI_SUCCESS;
}

/**

  This is USB RootHub enumerator

  @param  Event                 Indicating which event is signaled
  @param  Context               actually it is a USB_IO_DEVICE

**/
STATIC
VOID
EFIAPI
RootHubEnumeration (
  IN UINTN         SyncEnumeration,
  IN VOID          *Context
  )
{
  USB_IO_CONTROLLER_DEVICE  *HubController;
  EFI_USB_PORT_STATUS       HubPortStatus;
  EFI_STATUS                Status;
  UINT8                     Index;
  UINT8                     Index1;
  USB_IO_DEVICE             *UsbIoDev;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  EFI_HANDLE                HostController;
  USB_IO_DEVICE             *OldUsbIoDev;
  USB_IO_DEVICE             *NewDevice;
  USB_IO_CONTROLLER_DEVICE  *NewController;
  UINT8                     Index2;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINTN                     Mode;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  EFI_USB_BUS_PROTOCOL      *UsbBusProtocol;
  USB_BUS_CONTROLLER_DEVICE *CHCHubController;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT8                     UsbDevicePath[sizeof(USB_DEVICE_PATH) + END_DEVICE_PATH_LENGTH];
  USB_DEVICE_PATH           *UsbNode;
  UINTN                     RetryCount;
  
  HubController   = (USB_IO_CONTROLLER_DEVICE *) Context;
  HostController  = HubController->HostController;
  UsbBusDev       = HubController->UsbDevice->BusController;
  //
  // Skip the root port enumeration if it is triggered by root port reset to
  // preventing port state from destroyed by unnecessary port enumeration
  //
  if (!SyncEnumeration && HubController->PortResetProcessing) return;
  //
  // Root hub has the address 1
  //
  UsbIoDev = HubController->UsbDevice;
  
  for (Index = 0; Index < HubController->DownstreamPorts && Index < USB_MAXCHILDREN; Index++) {
    UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                  UsbBusDev->Usb3HCInterface,
                                  Index,
                                  (EFI_USB_PORT_STATUS *) &HubPortStatus
                                  );
    if (HubPortStatus.PortChangeStatus == 0) {
      continue;
    }
    if (!SyncEnumeration) {
      //
      // Stall 350ms(300ms minimum) to waiting for signal stable. Hot-plug only
      //
      mPrivate->UsbCore->Stall(350 * 1000);
      //
      // Refresh port status due to some devices will defer the port connection signal
      //
      UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                    UsbBusDev->Usb3HCInterface,
                                    Index,
                                    (EFI_USB_PORT_STATUS *) &HubPortStatus
                                    );
    }
    //
    // Clear some change status
    //
    for (Index2 = 0; Index2 < sizeof(mPortChangeMap) / sizeof(USB_CHANGE_FEATURE_MAP); Index2 ++) {
      if (HubPortStatus.PortChangeStatus & mPortChangeMap[Index2].ChangedBit) {
        //
        // Clear Hub port change bit
        //
        UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                      UsbBusDev->Usb3HCInterface,
                                      Index,
                                      mPortChangeMap[Index2].Feature
                                      );
      }
    }
    //
    // Check the overcurrent status
    //
    if (HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_OVERCURRENT) {
      //
      // Bypass if overcurrent
      //
      if (HubPortStatus.PortStatus & USB_PORT_STAT_OVERCURRENT) {
        continue;
      }
    }
    if (IsPortConnectChange (HubPortStatus.PortChangeStatus) || IsPortResetChange (HubPortStatus.PortChangeStatus)) {
      if (IsPortConnect (HubPortStatus.PortStatus)) {
        //
        // There is something connected to this port
        //
        DEBUG ((gUSBDebugLevel, "Something connected to Root Hub at Port0x%x\n", Index));

        ReportUsbStatusCode (
          UsbBusDev,
          EFI_PROGRESS_CODE,
          EFI_IO_BUS_USB | EFI_IOB_PC_HOTPLUG
          );
        //
        // if there is something physically detached, but still logically
        // attached...
        //
        OldUsbIoDev = HubController->Children[Index];

        if (NULL != OldUsbIoDev) {
          UsbDeviceDeConfiguration (OldUsbIoDev);
          HubController->Children[Index] = NULL;
        }
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 0)
        //
        // Check is it should be ignored before port reset
        //
        UsbNode = (USB_DEVICE_PATH*)UsbDevicePath;
        UsbNode->Header.Type      = MESSAGING_DEVICE_PATH;
        UsbNode->Header.SubType   = MSG_USB_DP;
        SetDevicePathNodeLength (&UsbNode->Header, sizeof(USB_DEVICE_PATH));
        UsbNode->InterfaceNumber  = 0;
        UsbNode->ParentPortNumber = Index;
        SetDevicePathEndNode ((EFI_DEVICE_PATH_PROTOCOL*)(UsbNode + 1));
        DevicePath = CopyDevicePath(mPrivate->UsbCore, HubController->DevicePath, &UsbNode->Header);
        Status = mPrivate->UsbCore->CheckIgnoredDevice(DevicePath, NULL);
        mPrivate->UsbCore->FreeBuffer (
                             DevicePathSize(DevicePath),
                             DevicePath
                             );
        if (EFI_ERROR(Status)) {
          //
          // This device should be skipped!
          //
          continue;
        }
#endif
        //
        // Set retry once for the situation of device configuration error
        //
        RetryCount = 1;
FAIL_RETRY:
        //
        // Reset the port and get the exact speed
        //
        Status = ResetRootPort(
                   HubController,
                   Index,
                   (UINT32*)&HubPortStatus
                   );
        if (EFI_ERROR (Status)) {
          //
          // Ignore enumeration when reset error
          //
          continue;
        }
        if (!IsPortConnect (HubPortStatus.PortStatus)) {
          //
          // Ignore enumeration when port disconnected
          //
          if (SyncEnumeration && (HubController->UsbDevice->DeviceSpeed == EFI_USB_SPEED_HIGH) && (HubPortStatus.PortStatus & USB_PORT_STAT_OWNER)) {
            //
            // The device is full/low speed and has released the ownership from EHC to CHC
            // Do sync root hub enumerate from CHC if SyncEnumeration on
            //
            HandleBuffer = NULL;
            Status = gBS->LocateHandleBuffer (
                            ByProtocol,
                            &mUsbBusProtocolGuid,
                            NULL,
                            &HandleCount,
                            &HandleBuffer
                            );
            if (!EFI_ERROR (Status) && HandleBuffer) {
              for (Index1 = 0; Index1 < (UINT8)HandleCount; Index1 ++) {
                Status = gBS->HandleProtocol (
                                HandleBuffer[Index1],
                                &mUsbBusProtocolGuid,
                                (VOID **)&UsbBusProtocol
                                );
                if (EFI_ERROR (Status)) {
                  continue;
                }
                CHCHubController = USB_BUS_CONTROLLER_DEVICE_FROM_THIS(UsbBusProtocol);
                if (CHCHubController->Root->DeviceSpeed == EFI_USB_SPEED_FULL) {
                  //
                  // This is full/low speed HC
                  //
                  RootHubEnumeration (SyncEnumeration, CHCHubController->Root->UsbController[0]);
                }
              }
              gBS->FreePool (HandleBuffer);
            }
          }
          Index = 0xff;
          continue;
        }
        Status = mPrivate->UsbCore->AllocateBuffer(
                                      sizeof (USB_IO_DEVICE),
                                      ALIGNMENT_32,
                                      (VOID **)&NewDevice
                                      );
        if (EFI_ERROR (Status)) {
          return;
        }
        //
        // Initialize some fields by copying data from its parents
        //
        NewDevice->DeviceDescriptor.MaxPacketSize0  = 8;
        NewDevice->BusController                    = UsbIoDev->BusController;
        //
        // Setup the device speed
        //
        if (HubPortStatus.PortStatus & USB_PORT_STAT_LOW_SPEED) {
          DEBUG ((gUSBDebugLevel, "Low Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_LOW;
        } else if (HubPortStatus.PortStatus & USB_PORT_STAT_HIGH_SPEED) {
          DEBUG ((gUSBDebugLevel, "High Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_HIGH;
        } else if (HubPortStatus.PortStatus & USB_PORT_STAT_SUPER_SPEED) {
          DEBUG ((gUSBDebugLevel, "Super Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_SUPER;
        } else {
          DEBUG ((gUSBDebugLevel, "Full Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_FULL;
        }
        //
        // Remove the RemainingDevicePath once initialization finish
        //
        if (!SyncEnumeration && HubController->RemainingDevicePath) {
          mPrivate->UsbCore->FreeBuffer (
                               DevicePathSize(HubController->RemainingDevicePath),
                               HubController->RemainingDevicePath
                               );
          HubController->RemainingDevicePath = NULL;
        }
        //
        // Configure that device
        //
        Status = UsbDeviceConfiguration (
                   HubController,
                   HostController,
                   Index,
                   NewDevice
                   );
        //
        // Add this device to the usb bus tree
        //
        HubController->Children[Index] = NewDevice;
        if (EFI_ERROR (Status)) {
          //
          // Keep the resource to occupy the device address if device in the address state
          //
          if (Status != EFI_UNSUPPORTED) {
            mPrivate->UsbCore->FreeBuffer(
                                 sizeof (USB_IO_DEVICE),
                                 NewDevice
                                 );
            HubController->Children[Index] = NULL;
            //
            // Sometime the device will reconnected after first time port reset and caused device config error
            // This is device firmware failure, we make a port reset again to workaround this issue
            //
            if (Status == EFI_DEVICE_ERROR) {
              UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                            UsbBusDev->Usb3HCInterface,
                                            Index,
                                            (EFI_USB_PORT_STATUS *) &HubPortStatus
                                            );
              if (HubPortStatus.PortChangeStatus == USB_PORT_STAT_C_CONNECTION) {
                Index --;
              } else if (RetryCount != 0) {
                //
                // Retry once upon device configuration error to workaround some USB 3.0 devices
                // plugged in USB 2.0 port after reset from OS
                //
                RetryCount --;
                goto FAIL_RETRY;
              }
            }
          }
          continue;
        }
        for (Index2 = 0; Index2 < NewDevice->NumOfInterfaces; Index2++) {
          //
          // If this device is hub, add to the hub index
          //
          NewController = NewDevice->UsbController[Index2];
      
          mPrivate->UsbCore->GetMode(&Mode);
          if (Mode != USB_CORE_RUNTIME_MODE) {
            //
            // Connect the controller to the driver image
            //
            if (IsPauseInProgress(&NewController->UsbIo)) {
              //
              // Connecting directly due to pause loop is under CSM16 and no risk on
              // UEFI device conntection
              //
              gBS->RestoreTPL (TPL_NOTIFY);
              ConnectControllerCallback(
                CONNECT_CONTROLLER,
                NewController
                );
              gBS->RaiseTPL (TPL_HIGH_LEVEL);
            } else {
              //
              // Use timer callback to connect the device to make the connection under
              // appropriate TPL 
              //
              mPrivate->UsbCore->RegisterNonSmmCallback(
                                   ConnectControllerCallback,
                                   CONNECT_CONTROLLER,
                                   NewController
                                   );
            }
          } else {
            //
            // Connect Usb devices during legacy mode for hot plug
            //
            mPrivate->UsbCore->ConnectUsbDevices (
                                 &NewController->UsbIo,
                                 NewController->DevicePath
                                 );
          }
      
          if (IsHub (NewController)) {
      
            NewController->IsUsbHub = TRUE;
            //
            // Configure Hub
            //
            Status = DoHubConfig (NewController);
            if (EFI_ERROR (Status)) {
              continue;
            }
            //
            // Add request to do query hub status
            // change endpoint.
            //
            UsbIo = &NewController->UsbIo;
            if (SyncEnumeration) {
              //
              // Do Hub Enumeration
              //
              NewController->StatusChangePort = 0xffff;
              HubEnumeration(SyncEnumeration, NewController);
            }
            UsbIo->UsbAsyncInterruptTransfer (
                     UsbIo,
                     NewController->HubEndpointAddress,
                     TRUE,
                     128,
                     (NewController->DownstreamPorts / 8) + 1,
                     OnHubInterruptComplete,
                     NewController
                     );
          }
        }
        if (IsSpecialStallDevice (NewDevice)) {
          Index = 0xff;
        }
      } else {
        //
        // Something disconnected from USB root hub
        //
        DEBUG ((gUSBDebugLevel, "Something disconnected from Root Hub at Port0x%x\n", Index));
      
        OldUsbIoDev = HubController->Children[Index];
      
        UsbDeviceDeConfiguration (OldUsbIoDev);
      
        HubController->Children[Index] = NULL;
      }
    }
  }
}

/**

  This is Usb Hub enumerator

  @param  Event                 Indicating which event is signaled
  @param  Context               actually it is a USB_IO_DEVICE

**/
STATIC
VOID
HubEnumeration (
  IN UINTN         SyncEnumeration,
  IN VOID          *Context
  )
{
  USB_IO_CONTROLLER_DEVICE  *HubController;
  EFI_USB_PORT_STATUS       HubPortStatus;
  EFI_STATUS                Status;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  EFI_HANDLE                HostController;
  USB_IO_DEVICE             *OldUsbIoDev;
  USB_IO_DEVICE             *NewDevice;
  USB_IO_CONTROLLER_DEVICE  *NewController;
  UINT16                    StatusChangePort;
  UINT8                     Index;
  UINT8                     Index2;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINTN                     Mode;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT8                     UsbDevicePath[sizeof(USB_DEVICE_PATH) + END_DEVICE_PATH_LENGTH];
  USB_DEVICE_PATH           *UsbNode;
  UINTN                     RetryCount;

  HubController    = (USB_IO_CONTROLLER_DEVICE *) Context;
  HostController   = HubController->HostController;
  UsbBusDev        = HubController->UsbDevice->BusController;
  StatusChangePort = HubController->StatusChangePort; 
  HubController->StatusChangePort = 0;
  //
  // Skip the hub port enumeration if it is triggered by hub port reset to
  // preventing port state from destroyed by unnecessary port enumeration
  //
  if (!SyncEnumeration && HubController->PortResetProcessing) return;
  //
  // Scan which port has status change
  // Bit 0 stands for hub itself, other bit stands for
  // the corresponding port
  //
  for (Index = 1; Index <= HubController->DownstreamPorts && Index <= USB_MAXCHILDREN; Index++) {
    if (!(StatusChangePort & (1 << Index))) {
      continue;
    }
    //
    // Check which event took place at that port
    //
    UsbIo = &HubController->UsbIo;
    Status = HubGetPortStatus (
               UsbIo,
               Index,
               (UINT32 *) &HubPortStatus
               );
   
    if (EFI_ERROR (Status) || HubPortStatus.PortChangeStatus == 0) {
      continue;
    }
    if (!SyncEnumeration) {
      //
      // Stall 350ms(300ms minimum) to waiting for signal stable. Hot-plug only
      //
      mPrivate->UsbCore->Stall(350 * 1000);
      //
      // Refresh port status due to some devices will defer the port connection signal
      //
      HubGetPortStatus (
        UsbIo,
        Index,
        (UINT32 *) &HubPortStatus
        );
    }
    //
    // Clear some change status
    //
    for (Index2 = 0; Index2 < sizeof(mPortChangeMap) / sizeof(USB_CHANGE_FEATURE_MAP); Index2 ++) {
      if (HubPortStatus.PortChangeStatus & mPortChangeMap[Index2].ChangedBit) {
        //
        // Clear Hub port change bit
        //
        HubClearPortFeature (
          UsbIo,
          Index,
          mPortChangeMap[Index2].Feature
          );
      }
    }
    //
    // Check the overcurrent status
    //
    if (HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_OVERCURRENT) {
      //
      // Bypass if overcurrent
      //
      if (HubPortStatus.PortStatus & USB_PORT_STAT_OVERCURRENT) {
        continue;
      }
    }
    if (IsPortConnectChange (HubPortStatus.PortChangeStatus) || IsPortResetChange (HubPortStatus.PortChangeStatus)) {
      if (IsPortConnect (HubPortStatus.PortStatus)) {
   
        DEBUG ((gUSBDebugLevel, "New Device Connect on Hub port \n"));
   
        ReportUsbStatusCode (
          UsbBusDev,
          EFI_PROGRESS_CODE,
          EFI_IO_BUS_USB | EFI_IOB_PC_HOTPLUG
          );
        //
        // if there is something physically detached, but still logically
        // attached...
        //
        OldUsbIoDev = HubController->Children[Index - 1];
   
        if (NULL != OldUsbIoDev) {
          UsbDeviceDeConfiguration (OldUsbIoDev);
          HubController->Children[Index - 1] = NULL;
        }
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 0)
        //
        // Check is it should be ignored before port reset
        //
        UsbNode = (USB_DEVICE_PATH*)UsbDevicePath;
        UsbNode->Header.Type      = MESSAGING_DEVICE_PATH;
        UsbNode->Header.SubType   = MSG_USB_DP;
        SetDevicePathNodeLength (&UsbNode->Header, sizeof(USB_DEVICE_PATH));
        UsbNode->InterfaceNumber  = 0;
        UsbNode->ParentPortNumber = Index - 1;
        SetDevicePathEndNode ((EFI_DEVICE_PATH_PROTOCOL*)(UsbNode + 1));
        DevicePath = CopyDevicePath(mPrivate->UsbCore, HubController->DevicePath, &UsbNode->Header);
        Status = mPrivate->UsbCore->CheckIgnoredDevice(DevicePath, NULL);
        mPrivate->UsbCore->FreeBuffer (
                             DevicePathSize(DevicePath),
                             DevicePath
                             );
        if (EFI_ERROR(Status)) {
          //
          // This device should be skipped!
          //
          continue;
        }
#endif
        //
        // Set retry once for the situation of device configuration error
        //
        RetryCount = 1;
FAIL_RETRY:
        //
        // There is something connected to this port,
        // reset that port
        //
        Status = ResetHubPort (
                   HubController,
                   Index,
                   (UINT32*)&HubPortStatus
                   );
        if (EFI_ERROR (Status)) {
          //
          // Ignore enumeration when reset error
          //
          continue;
        }
        if (!IsPortConnect (HubPortStatus.PortStatus)) {
          //
          // Ignore enumeration when port disconnected
          //
          Index = 0;
          continue;
        }
        Status = mPrivate->UsbCore->AllocateBuffer(
                                      sizeof (USB_IO_DEVICE),
                                      ALIGNMENT_32,
                                      (VOID **)&NewDevice
                                      );
        if (EFI_ERROR (Status)) {
          continue;
        }
        //
        // Initialize some fields
        //
        NewDevice->DeviceDescriptor.MaxPacketSize0  = 8;
        NewDevice->BusController                    = HubController->UsbDevice->BusController;
        //
        // Setup the device speed
        //
        if (HubPortStatus.PortStatus & USB_PORT_STAT_LOW_SPEED) {
          DEBUG ((gUSBDebugLevel, "Low Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_LOW;
        } else if (HubPortStatus.PortStatus & USB_PORT_STAT_HIGH_SPEED) {
          DEBUG ((gUSBDebugLevel, "High Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_HIGH;
        } else if (HubPortStatus.PortStatus & USB_PORT_STAT_SUPER_SPEED) {
          DEBUG ((gUSBDebugLevel, "Super Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_SUPER;
        } else {
          DEBUG ((gUSBDebugLevel, "Full Speed Device Attached to Hub\n"));
          NewDevice->DeviceSpeed = EFI_USB_SPEED_FULL;
        }
        //
        // Remove the RemainingDevicePath once initialization finish
        //
        if (!SyncEnumeration && HubController->RemainingDevicePath) {
          mPrivate->UsbCore->FreeBuffer (
                               DevicePathSize(HubController->RemainingDevicePath),
                               HubController->RemainingDevicePath
                               );
          HubController->RemainingDevicePath = NULL;
        }
        //
        // Setup the topology level
        //
        NewDevice->HubDepth = HubController->UsbDevice->HubDepth + 1;
        if (NewDevice->HubDepth <= 5) {
          //
          // Configure that device
          //
          Status = UsbDeviceConfiguration (
                     HubController,
                     HostController,
                     (Index - 1),
                     NewDevice
                     );
          //
          // Add this device to the usb bus tree. Index is begin from 1 due to 0 is Hub self,
          //
          HubController->Children[Index - 1] = NewDevice;
        } else {
          //
          // The HUB depth large then 5, unsupported
          //
          Status = EFI_NOT_FOUND;
        }
        if (EFI_ERROR (Status)) {
          //
          // Keep the resource to occupy the device address if device in the address state
          //
          if (Status != EFI_UNSUPPORTED) {
            mPrivate->UsbCore->FreeBuffer(
                                 sizeof (USB_IO_DEVICE),
                                 NewDevice
                                 );
            HubController->Children[Index - 1] = NULL;
            //
            // Sometime the device will reconnected after first time port reset and caused device config error
            // This is device firmware failure, we make a port reset again to workaround this issue
            //
            if (Status == EFI_DEVICE_ERROR) {
              Status = HubGetPortStatus (
                         UsbIo,
                         Index,
                         (UINT32 *) &HubPortStatus
                         );
              if (!EFI_ERROR (Status) && HubPortStatus.PortChangeStatus == USB_PORT_STAT_C_CONNECTION) {
                Index --;
              } else if (RetryCount != 0) {
                //
                // Retry once upon device configuration error to workaround some USB 3.0 devices
                // plugged in USB 2.0 port after reset from OS
                //
                RetryCount --;
                goto FAIL_RETRY;
              }
            }
          }
          continue;
        }
        //
        // Setup interfaces derive from this NewDevice
        //
        for (Index2 = 0; Index2 < NewDevice->NumOfInterfaces; Index2++) {
          //
          // If this device is hub, add to the hub index
          //
          NewController = NewDevice->UsbController[Index2];
   
          mPrivate->UsbCore->GetMode(&Mode);
          if (Mode != USB_CORE_RUNTIME_MODE) {
            //
            // Connect the controller to the driver image
            //
            if (IsPauseInProgress(&NewController->UsbIo)) {
              //
              // Connecting directly due to pause loop is under CSM16 and no risk on
              // UEFI device conntection
              //
              gBS->RestoreTPL (TPL_NOTIFY);
              ConnectControllerCallback(
                CONNECT_CONTROLLER,
                NewController
                );
              gBS->RaiseTPL (TPL_HIGH_LEVEL);
            } else {
              //
              // Use timer callback to connect the device to make the connection under
              // appropriate TPL 
              //
              mPrivate->UsbCore->RegisterNonSmmCallback(
                                   ConnectControllerCallback,
                                   CONNECT_CONTROLLER,
                                   NewController
                                   );
            }
          } else {
            //
            // Connect Usb devices during legacy mode for hot plug
            //
            mPrivate->UsbCore->ConnectUsbDevices (
                                 &NewController->UsbIo,
                                 NewController->DevicePath
                                 );
          }
   
          if (IsHub (NewController)) {
   
            NewController->IsUsbHub = TRUE;
            //
            // Configure Hub
            //
            Status = DoHubConfig (NewController);
            if (EFI_ERROR (Status)) {
              continue;
            }
            //
            // Add request to do query hub status
            // change endpoint
            //
            UsbIo = &NewController->UsbIo;
            if (SyncEnumeration) {
              //
              // Do Hub Enumeration
              //
              NewController->StatusChangePort = 0xffff;
              HubEnumeration(SyncEnumeration, NewController);
            }
            UsbIo->UsbAsyncInterruptTransfer (
                     UsbIo,
                     NewController->HubEndpointAddress,
                     TRUE,
                     128,
                     (NewController->DownstreamPorts / 8) + 1,
                     OnHubInterruptComplete,
                     NewController
                     );
          }
        }
        if (IsSpecialStallDevice (NewDevice)) {
          Index = 0;
        }
      } else {
        //
        // Something disconnected from USB hub
        //
        DEBUG ((gUSBDebugLevel, "Something Device Detached on Hub port\n"));
   
        OldUsbIoDev = HubController->Children[Index - 1];
   
        UsbDeviceDeConfiguration (OldUsbIoDev);
   
        HubController->Children[Index - 1] = NULL;
   
      }
    }
  }
}

/**

  Allocate a structure for USB_IO_CONTROLLER_DEVICE

  @param  UsbBusDev             Pointer of USB_BUS_CONTROLLER_DEVICE

  @retval                       A pointer to a USB_IO_CONTROLLER_DEVICE structure,
                                Or NULL.

**/
STATIC
USB_IO_CONTROLLER_DEVICE *
CreateUsbIoControllerDevice (
  IN     USB_BUS_CONTROLLER_DEVICE      *UsbBusDev
  )
{
  EFI_STATUS                Status;
  USB_IO_CONTROLLER_DEVICE  *UsbIoControllerDev;

  //
  // Allocate USB_IO_CONTROLLER_DEVICE structure
  //
  Status = mPrivate->UsbCore->AllocateBuffer(
                                sizeof (USB_IO_CONTROLLER_DEVICE),
                                ALIGNMENT_32,
                                (VOID **)&UsbIoControllerDev
                                );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  UsbIoControllerDev->Signature = USB_IO_CONTROLLER_SIGNATURE;
  return UsbIoControllerDev;
}

/**

  Resets and reconfigures the USB controller.  This function will
  work for all USB devices except USB Hub Controllers.

  @param  This                  Indicates the calling context.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
EFIAPI
UsbPortReset (
  IN EFI_USB_IO_PROTOCOL      *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *ParentIoDev;
  USB_IO_DEVICE             *UsbIoDev;
  USB_IO_CONTROLLER_DEVICE  *ParentController;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  UINT8                     HubPort;
  UINT32                    Status;
  EFI_STATUS                Result;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINT8                     Address;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);

  if (IsHub (UsbIoController)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Since at this time, this device has already been configured,
  // it needs to be re-configured.
  //
  ParentController  = UsbIoController->Parent;
  ParentIoDev       = ParentController->UsbDevice;
  UsbIoDev          = UsbIoController->UsbDevice;
  HubPort           = UsbIoController->ParentPort;
  UsbBusDev         = UsbIoDev->BusController;
  //
  // Disable device (used for Xhci) before port reset to preventing some
  // Xhci from failed on port reset issue
  //
  UsbBusDev->Usb3HCInterface->CancelIo (
                                UsbBusDev->Usb3HCInterface,
                                UsbIoDev->DeviceAddress,
                                0,
                                0,
                                NULL
                                );
  //
  // Signal the PortResetProcessing flag to preventing unnecessary hub port enumeration
  //
  ParentController->PortResetProcessing = TRUE;
  if (ParentIoDev->DeviceAddress == 1) {
    DEBUG ((gUSBDebugLevel, "Reset from Root Hub 0x%x\n", HubPort));
    ResetRootPort (ParentController, HubPort, NULL);
  } else {
    DEBUG ((gUSBDebugLevel, "Reset from Hub, Addr 0x%x\n", ParentIoDev->DeviceAddress));
    ResetHubPort (ParentController, HubPort + 1, NULL);
  }
  //
  // Clears the PortResetProcessing flag for normal operation
  //
  ParentController->PortResetProcessing = FALSE;
  //
  // Re-config that USB device
  //
  UsbIo = &UsbIoController->UsbIo;
  //
  // Assign a unique address to this device
  //
  Address                 = UsbIoDev->DeviceAddress;
  UsbIoDev->DeviceAddress = 0;
  Result                  = UsbSetDeviceAddress (UsbIo, Address, &Status);
  UsbIoDev->DeviceAddress = Address;
  if (EFI_ERROR (Result)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Get & parse all configurations for this device (used for Xhci)
  //
  Result = UsbGetAllConfigurations (UsbIoDev);
  if (EFI_ERROR (Result)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Stall 1ms after GetDescriptor for specific device compatibility(0.1ms minimum)
  //
  mPrivate->UsbCore->Stall (1000);
  //
  // Set the device to the default configuration
  //
  Result = UsbSetDefaultConfiguration (UsbIoDev);
  if (EFI_ERROR (Result)) {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**

  Reset Root Hub port.

  @param  UsbBusDev             Bus controller of the device.
  @param  PortNum               The given port to be reset.
  @param  PortStatus            Pointer of the port status for output

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
ResetRootPort (
  IN     USB_IO_CONTROLLER_DEVICE       *UsbIoController,
  IN     UINT8                          PortNum,
  OUT    UINT32                         *PortStatus
  )
{
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  EFI_STATUS                Status;
  EFI_USB_PORT_STATUS       HubPortStatus;
  UINTN                     Timeout;
  BOOLEAN                   PortReleased;

  PortReleased = FALSE;
  UsbBusDev = UsbIoController->UsbDevice->BusController;
  //
  // Only high/full speed device need to port reset if the device controlled by EHCI
  //
  UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                UsbBusDev->Usb3HCInterface,
                                PortNum,
                                &HubPortStatus
                                );
  //
  // Check is it in suspend state 
  //
  if (HubPortStatus.PortStatus & USB_PORT_STAT_SUSPEND) {
    //
    // Clear suspend root port
    //
    Status = UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                           UsbBusDev->Usb3HCInterface,
                                           PortNum,
                                           EfiUsbPortSuspend
                                           );
    mPrivate->UsbCore->Stall (50 * 1000);
  }
  if (!(HubPortStatus.PortStatus & USB_PORT_STAT_OWNER) && !(HubPortStatus.PortStatus & USB_PORT_STAT_HIGH_SPEED)) {
    //
    // Release the port to CHC directly if low speed device attached in EHCI port
    //
    ReleasePortToCHC (UsbBusDev, PortNum);
    //
    // Get latest port status to know whether the HC has CHC or not
    //
    UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                  UsbBusDev->Usb3HCInterface,
                                  PortNum,
                                  &HubPortStatus
                                  );
    if (HubPortStatus.PortStatus & USB_PORT_STAT_OWNER) PortReleased = TRUE;
  }
  if (!PortReleased) {
    //
    // Reset root port
    //
    Status = UsbBusDev->Usb3HCInterface->SetRootHubPortFeature (
                                           UsbBusDev->Usb3HCInterface,
                                           PortNum,
                                           EfiUsbPortReset
                                           );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    //
    // Stall 50ms for the duration of port reset
    //
    mPrivate->UsbCore->Stall (50 * 1000);
    //
    // For EHCI and UHCI, the port reset must be cleared by software.
    //
    Status = UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                           UsbBusDev->Usb3HCInterface,
                                           PortNum,
                                           EfiUsbPortReset
                                           );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    //
    // Stall 2ms for port reset signal stable
    //
    mPrivate->UsbCore->Stall (2 * 1000);
    //
    // For XHCI and OHCI, the port reset is cleared by host controller.
    // It is needed to waiting for this bit for the completion of
    // port reset in 50ms timeout
    //
    Timeout = 50;
    do {
      UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                    UsbBusDev->Usb3HCInterface,
                                    PortNum,
                                    &HubPortStatus
                                    );
      if (!(HubPortStatus.PortStatus & USB_PORT_STAT_RESET)) break;
      mPrivate->UsbCore->Stall (1000);
      Timeout --;
    } while (Timeout > 0);
    //
    // Clear the port reset change 
    //
    Status = UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                           UsbBusDev->Usb3HCInterface,
                                           PortNum,
                                           EfiUsbPortResetChange
                                           );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    //
    // Get latest port status
    //
    UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                  UsbBusDev->Usb3HCInterface,
                                  PortNum,
                                  &HubPortStatus
                                  );
    if (!(HubPortStatus.PortStatus & USB_PORT_STAT_OWNER)) {
      //
      // EHC Port Owner. Proceed full speed device detection procedure
      //
      if ((HubPortStatus.PortStatus & USB_PORT_STAT_HIGH_SPEED) && !(HubPortStatus.PortStatus & USB_PORT_STAT_ENABLE)) {
        //
        // Stall 10 ms due to we just issued a "enabled test" for full speed device and need to stall awhile
        //
        mPrivate->UsbCore->Stall (10 * 1000);
        //
        // Release ownership to CHC due to this is full/low speed device attached in EHC
        //
        ReleasePortToCHC (UsbBusDev, PortNum);
        //
        // Get latest port status to know whether the HC has CHC or not
        //
        UsbBusDev->Usb3HCInterface->GetRootHubPortStatus (
                                      UsbBusDev->Usb3HCInterface,
                                      PortNum,
                                      &HubPortStatus
                                      );
        if (HubPortStatus.PortStatus & USB_PORT_STAT_OWNER) PortReleased = TRUE;
        //
        // Full speed device attached in EHCI port
        //
        HubPortStatus.PortStatus &= ~USB_PORT_STAT_HIGH_SPEED;
      }
    } else {
      //
      // CHC Port Owner. Set the enable manually(after reset) because CHC can't enable automatically
      //
      Status = UsbBusDev->Usb3HCInterface->SetRootHubPortFeature (
                                             UsbBusDev->Usb3HCInterface,
                                             PortNum,
                                             EfiUsbPortEnable
                                             );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      //
      // Stall 20ms after enabled the port
      //
      mPrivate->UsbCore->Stall (20 * 1000);
    }
  }
  if (!PortReleased) {
    if (HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_ENABLE) {
      Status = UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                             UsbBusDev->Usb3HCInterface,
                                             PortNum,
                                             EfiUsbPortEnableChange
                                             );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
    }
    if (HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_CONNECTION) {
      Status = UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                             UsbBusDev->Usb3HCInterface,
                                             PortNum,
                                             EfiUsbPortConnectChange
                                             );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
    }
  }
  //
  // Return the HubPortStatus to caller
  //
  if (PortStatus) *PortStatus = *(UINT32*)&HubPortStatus;
  return EFI_SUCCESS;
}

/**

  Reset Hub port.

  @param  UsbIoController       The USB_IO_CONTROLLER_DEVICE instance.
  @param  PortIndex             The given port to be reset.
  @param  PortStatus            Pointer of the port status for output

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
ResetHubPort (
  IN     USB_IO_CONTROLLER_DEVICE       *UsbIoController,
  IN     UINT8                          PortIndex,
  OUT    UINT32                         *PortStatus
  )
{
  EFI_STATUS                Status;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  EFI_USB_PORT_STATUS       HubPortStatus;
  UINTN                     Number;
  UINTN                     Index;
  UINTN                     ResetRetry;

  ASSERT (UsbIoController->IsUsbHub == TRUE);

  UsbBusDev = UsbIoController->UsbDevice->BusController;
  UsbIo = &UsbIoController->UsbIo;
  ResetRetry = 0;
RESET_RETRY:
  //
  // Reset hub port
  //
  HubSetPortFeature (
    UsbIo,
    PortIndex,
    EfiUsbPortReset
    );

  mPrivate->UsbCore->Stall (20 * 1000);

  //
  // Wait for port reset complete
  //
  Number = 2000;
  do {
    Status = HubGetPortStatus (
               UsbIo,
               PortIndex,
               (UINT32 *) &HubPortStatus
               );
    if (Status == EFI_DEVICE_ERROR && ResetRetry < 2) { 
      //
      // Reset port again to recover the babble error on Intel virtual HUB 
      //
      ResetRetry ++;
      goto RESET_RETRY;
    } 
    if (EFI_ERROR (Status) || (!EFI_ERROR (Status) && (HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_RESET))) break;
    mPrivate->UsbCore->Stall (1000);
    Number --;
  } while (Number > 0);

  if (EFI_ERROR (Status) || Number == 0) {
    //
    // Cannot reset port, return error
    //
    return EFI_DEVICE_ERROR;
  }

  Number = 10;
  do {
    for (Index = 0; Index < sizeof(mPortChangeMap) / sizeof(USB_CHANGE_FEATURE_MAP); Index ++) {
      if (HubPortStatus.PortChangeStatus & mPortChangeMap[Index].ChangedBit) {
        //
        // Clear Hub port change bit
        //
        HubClearPortFeature (
          UsbIo,
          PortIndex,
          mPortChangeMap[Index].Feature
          );
      }
    }
    //
    // Stall 50 ms to waiting for signal stable
    //
    mPrivate->UsbCore->Stall (50 * 1000);
    if ((HubPortStatus.PortChangeStatus & USB_PORT_STAT_C_CONNECTION) && ResetRetry < 2) {
      //
      // Reset port again to recover the device reconnected during the HUB port reset period 
      //
      ResetRetry ++;
      goto RESET_RETRY;
    }
    //
    // Read status again
    //
    HubGetPortStatus (
      UsbIo,
      PortIndex,
      (UINT32 *) &HubPortStatus
      );
    Number --;
  } while (HubPortStatus.PortChangeStatus != 0 && Number > 0);
  //
  // Return the HubPortStatus to caller
  //
  if (PortStatus) {
    *PortStatus = *(UINT32*)&HubPortStatus;
    //
    // SuperSpeed Hub Status Conversion
    //
    if (UsbIoController->UsbDevice->DeviceSpeed == EFI_USB_SPEED_SUPER) {
      (*PortStatus) &= ~0x0000FFE0;
      if (HubPortStatus.PortStatus & USB_PORT_STAT_SUPER_SPEED_POWER) {
        //
        // Convert Port Power Status
        //
        (*PortStatus) |= USB_PORT_STAT_POWER;
      }
      if ((HubPortStatus.PortStatus & USB_PORT_STAT_SUPER_SPEED_MASK) == 0) {
        //
        // Convert Port Speed Status
        //
        (*PortStatus) |= USB_PORT_STAT_SUPER_SPEED;
      }
    }
  }
  return EFI_SUCCESS;
}

/**

  Report a error Status code of USB bus driver controller

  @param  UsbBusDev             USB_BUS_CONTROLLER_DEVICE
  @param  Type                  EFI_STATUS_CODE_TYPE
  @param  Code                  EFI_STATUS_CODE_VALUE

**/
STATIC
EFI_STATUS
ReportUsbStatusCode (
  IN USB_BUS_CONTROLLER_DEVICE     *UsbBusDev,
  IN EFI_STATUS_CODE_TYPE          Type,
  IN EFI_STATUS_CODE_VALUE         Code
  )
{
  EFI_STATUS Status;
  UINTN      Mode;
  BOOLEAN    InSmm;

  Status = EFI_SUCCESS;
  mPrivate->UsbCore->GetMode(&Mode);
  if (Mode != USB_CORE_RUNTIME_MODE) {
    //
    // Only report status code during POST and non-SMM mode
    //
    mPrivate->UsbCore->IsInSmm(&InSmm);
    if (!InSmm) {
      Status = ReportStatusCodeWithDevicePath (
                 Type,
                 Code,
                 UsbBusDev->DevicePath
                 );
    }
  }
  return Status;
}

/**

  Set Transaction Translator parameter

  @param  ParentHubController   Controller structure of the parent Hub device
  @param  ParentPort            Number of parent port
  @param  Device                Structure of the device

  @retval EFI_SUCCESS           Success
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate resources

**/
STATIC
EFI_STATUS
UsbSetTransactionTranslator (
  IN USB_IO_CONTROLLER_DEVICE     *ParentHubController,
  IN UINT8                        ParentPort,
  IN OUT USB_IO_DEVICE            *Device
  )
{
  UINT8 DeviceAddress;
  UINT8 Port;
  UINT8 MultiTT;
  //
  // Inherit Route String from parent
  //
  Device->Translator.RouteString = ParentHubController->UsbDevice->Translator.RouteString;
  //
  // Initial the route string for XHC
  //
  if (ParentHubController->IsUsbHub && Device->HubDepth != 0) {
    Device->Translator.RouteString &= ~(0x0f << (4 * (Device->HubDepth - 1)));
    Device->Translator.RouteString |= (ParentPort + 1) << (4 * (Device->HubDepth - 1));
  }
  //
  // Inherit RootHub Port Number from parent
  //
  if (Device->HubDepth == 0) {
    //
    // Level 1 device, records the RootHub Port Number
    //
    Device->Translator.RootHubPortNumber = ParentPort + 1;
  } else {
    //
    // Not level 1 device, inherit RootHub Port Number from parent
    //
    Device->Translator.RootHubPortNumber = ParentHubController->UsbDevice->Translator.RootHubPortNumber;
    if (Device->DeviceSpeed == EFI_USB_SPEED_LOW || Device->DeviceSpeed == EFI_USB_SPEED_FULL) {
      //
      // Only full/low speed device need 
      //
      if (EFI_USB_SPEED_HIGH == ParentHubController->UsbDevice->DeviceSpeed) {
        //
        // Parent is high speed, then parent is our translator
        //
        DeviceAddress = ParentHubController->UsbDevice->DeviceAddress;
        Port          = ParentPort + 1;
        MultiTT       = ParentHubController->MultiTT;
      } else {
        //
        // Use parent's translator.
        //
        DeviceAddress = ParentHubController->UsbDevice->Translator.TranslatorHubAddress;
        Port          = ParentHubController->UsbDevice->Translator.TranslatorPortNumber;
        MultiTT       = ParentHubController->UsbDevice->Translator.MultiTT;
      }
      Device->Translator.TranslatorHubAddress  = DeviceAddress;
      Device->Translator.TranslatorPortNumber  = Port;
      Device->Translator.MultiTT               = MultiTT;
    }
  }
  return EFI_SUCCESS;
}

/**

  Set bit to release the port owner to CHC

  @param  UsbBusDev             UsbBus controller structure of the device
  @param  PortNum               Number of the port

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

**/
STATIC
EFI_STATUS
ReleasePortToCHC (
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev,
  UINT8                     PortNum
  )
{
  EFI_STATUS           Status;
  //
  // Full/Low speed device need to stall 1 ms between last ClearRootHubPortFeature and ReleasePortToCHC
  //
  mPrivate->UsbCore->Stall (1000);
  Status = UsbBusDev->Usb3HCInterface->SetRootHubPortFeature (
                                         UsbBusDev->Usb3HCInterface,
                                         PortNum,
                                         EfiUsbPortOwner
                                         );
  mPrivate->UsbCore->Stall (100 * 1000);
  //
  // The EfiUsbPortOwner will cause PortConnectChange set. Clear it!!!
  //
  Status = UsbBusDev->Usb3HCInterface->ClearRootHubPortFeature (
                                         UsbBusDev->Usb3HCInterface,
                                         PortNum,
                                         EfiUsbPortConnectChange
                                         );
  return Status;
}

/**

  Calculate the size of device path

  @param  DevicePath            Pointer of EFI_DEVICE_PATH_PROTOCOL

  @retval Size of device path

**/
STATIC
UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }
  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    DevicePath = NextDevicePathNode (DevicePath);
  }
  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}

/**

  Function is used to append a Src1 and Src2 together.

  @param  UsbCore               UsbCore protocol
  @param  Src1                  A pointer to a device path data structure.
  @param  Src2                  A pointer to a device path data structure.

  @retval A pointer to the new device path is returned.
          NULL is returned if space for the new device path could not be allocated from pool.
          It is up to the caller to free the memory used by Src1 and Src2 if they are no longer needed.

**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
CopyDevicePath (
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *Src1,
  IN     EFI_DEVICE_PATH_PROTOCOL       *Src2
  )
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINTN                     Size1;
  UINTN                     Size2;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *SecondDevicePath;

  //
  // Allocate space for the combined device path. It only has one end node of
  // length EFI_DEVICE_PATH_PROTOCOL
  //
  Size1 = DevicePathSize (Src1);
  Size2 = DevicePathSize (Src2);
  Size  = Size1 + Size2;

  if (Size1 != 0 && Size2 != 0) {
    Size -= sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }

  if (UsbCore == NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    Size,
                    (VOID **) &NewDevicePath
                    );
  } else {
    Status = UsbCore->AllocateBuffer(
                        Size,
                        ALIGNMENT_32,
                        (VOID **)&NewDevicePath
                        );
  }
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  CopyMem (NewDevicePath, Src1, Size1);

  //
  // Over write Src1 EndNode and do the copy
  //
  if (Size1 != 0) {
    SecondDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) ((CHAR8 *) NewDevicePath + (Size1 - sizeof (EFI_DEVICE_PATH_PROTOCOL)));
  } else {
    SecondDevicePath = NewDevicePath;

  }

  if (Size2 != 0) {
    CopyMem (SecondDevicePath, Src2, Size2);
  }

  return NewDevicePath;
}

/**

  This callback got called by timer during outside of SMM

  @param  Event                 Type
  @param  Context               Context

**/
STATIC
VOID
EFIAPI
ConnectControllerCallback(
  IN     UINTN                Event,
  IN     VOID                 *Context
  )
{
  EFI_STATUS               Status;
  USB_IO_CONTROLLER_DEVICE *UsbController;
  EFI_USB3_HC_PROTOCOL     *Usb3HcProtocol;
  UINTN                    Index;
  
  UsbController = (USB_IO_CONTROLLER_DEVICE*)Context;
  if (Event == CONNECT_CONTROLLER) {
    //
    // In order to prevent the UEFI OS boot hang, allocate the buffer of DevicePath in BS for UEFI OS
    //
    UsbController->UefiDevicePath = CopyDevicePath(NULL, UsbController->DevicePath, NULL);
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &UsbController->Handle,
                    &gEfiDevicePathProtocolGuid,
                    UsbController->UefiDevicePath,
                    &gEfiUsbIoProtocolGuid,
                    &UsbController->UsbIo,
                    NULL
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->OpenProtocol (
                      UsbController->HostController,
                      &gEfiUsb3HcProtocolGuid,
                      (VOID **)&Usb3HcProtocol,
                      gUsbBusDriverBinding.DriverBindingHandle,
                      UsbController->Handle,
                      EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                      );
      if (!EFI_ERROR (Status)) {
        gBS->ConnectController (
               UsbController->Handle,
               NULL,
               UsbController->RemainingDevicePath,
               TRUE
               );
      }
    }
    if (EFI_ERROR (Status)) {
      ReportUsbStatusCode (
        UsbController->UsbDevice->BusController,
        EFI_ERROR_CODE | EFI_ERROR_MINOR,
        EFI_IO_BUS_USB | EFI_IOB_EC_INTERFACE_ERROR
        );
      //
      // Remove the ptr placed in UsbIoDev table
      //
      for (Index = 0; Index < UsbController->UsbDevice->NumOfInterfaces; Index ++) {
        if (UsbController->UsbDevice->UsbController[Index] == UsbController) {
          UsbController->UsbDevice->UsbController[Index] = NULL;
          break;
        }
      }
      if (UsbController->UefiDevicePath != NULL) {
        gBS->FreePool(UsbController->UefiDevicePath);
      }
      //
      // Free IoControllerDevice due to error
      //
      FreeUsbIoControllerDevice(UsbController);
    }
  } else {
    gBS->DisconnectController (
           UsbController->Handle,
           NULL,
           NULL
           );
    //
    // remove child handle reference to the USB_HC_PROTOCOL
    //
    gBS->CloseProtocol (
           UsbController->HostController,
           &gEfiUsb3HcProtocolGuid,
           gUsbBusDriverBinding.DriverBindingHandle,
           UsbController->Handle
           );
    //
    // Uninstall DEVICE_PATH_PROTOCOL installed on this handle
    //
    Status = gBS->UninstallProtocolInterface (
                    UsbController->Handle,
                    &gEfiDevicePathProtocolGuid,
                    UsbController->UefiDevicePath
                    );
    if (!EFI_ERROR(Status) && UsbController->UefiDevicePath != NULL) {
      gBS->FreePool(UsbController->UefiDevicePath);
    }
    //
    // Uninstall EFI_USB_IO_PROTOCOL along due to it may not be unstalled(caused by someone still opened the protocol)
    //
    gBS->UninstallProtocolInterface (
           UsbController->Handle,
           &gEfiUsbIoProtocolGuid,
           &UsbController->UsbIo
           );
    //
    // Free IoControllerDevice
    //
    FreeUsbIoControllerDevice(UsbController);
  }
}

/**

  This callback got called by timer during outside of SMM

  @param  Event                 Type
  @param  Context               Context

**/
STATIC
VOID
EFIAPI
FreeUsbIoDeviceCallback(
  IN     UINTN                Event,
  IN     VOID                 *Context
  )
{
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  USB_IO_DEVICE             *UsbIoDev;
  USB_BUS_CONTROLLER_DEVICE *UsbBusDev;
  
  UsbCore   = mPrivate->UsbCore;
  UsbIoDev  = (USB_IO_DEVICE*)Context;
  UsbBusDev = UsbIoDev->BusController;
  //
  // Free address for later use
  //
  UsbFreeDeviceAddress (
    UsbIoDev->DeviceAddress,
    UsbBusDev->AddressPool
    );
  //
  // Free all resouces allocated for all its configurations
  //
  UsbDestroyAllConfiguration (UsbIoDev);
  UsbCore->FreeBuffer (
             sizeof (USB_IO_DEVICE),
             UsbIoDev
             );
  //
  // Remove Smm address convert table
  //
  RemoveUsbIoDevAddressConvertTable(UsbIoDev);
}

/**

  Free IoControllerDevice

  @param  UsbController         UsbController

**/
STATIC
VOID
FreeUsbIoControllerDevice(
  IN     USB_IO_CONTROLLER_DEVICE       *UsbController
  )
{
  EFI_USB_CORE_PROTOCOL    *UsbCore;
  
  UsbCore = mPrivate->UsbCore;
  if (UsbController->DevicePath != NULL) {
    UsbCore->FreeBuffer (
               DevicePathSize(UsbController->DevicePath),
               UsbController->DevicePath
               );
  }
  if (UsbController->RemainingDevicePath != NULL) {
    UsbCore->FreeBuffer (
               DevicePathSize(UsbController->RemainingDevicePath),
               UsbController->RemainingDevicePath
               );
  }
  UsbCore->FreeBuffer (
             sizeof (USB_IO_CONTROLLER_DEVICE),
             UsbController
             );
  //
  // Clear the signature of UsbController to signal it was freed
  //
  UsbController->Signature = 0;
  //
  // Remove Smm address convert table
  //
  RemoveUsbIoControllerDevAddressConvertTable(UsbController);
}

/**

  Insert IoDevice and attached IoControllerDevices into SMM convert table

  @param  UsbIoDev              UsbIoDevice

**/
STATIC
VOID
InsertUsbIoDevAddressConvertTable(
  IN     USB_IO_DEVICE                  *UsbIoDev,
  IN     BOOLEAN                        IsRootHub
  )
{
  EFI_USB_CORE_PROTOCOL    *UsbCore;
  UINTN                    Index;
  
  UsbCore = mPrivate->UsbCore;
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_BODY,
             UsbIoDev,
             sizeof (USB_IO_DEVICE)
             );
  if (!IsRootHub) {
    //
    // The ConfigDescList only present in non root hub
    //
    UsbCore->InsertAddressConvertTable (
               ACT_LINKING_LIST,
               &UsbIoDev->ConfigDescListHead,
               1
               );
  }
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbIoDev->ActiveConfig,
             1
             );
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbIoDev->BusController,
             1
             );
  UsbCore->InsertAddressConvertTable (
             ACT_INSTANCE_POINTER,
             UsbIoDev->UsbController,
             UsbIoDev->NumOfInterfaces
             );
  for (Index = 0; Index < UsbIoDev->NumOfInterfaces; Index++) {
    if (!IsRootHub) {
      //
      // The body of UsbIo protocol only present in non root hub
      //
      UsbCore->InsertAddressConvertTable (
                 ACT_FUNCTION_POINTER,
                 &UsbIoDev->UsbController[Index]->UsbIo,
                 sizeof(EFI_USB_IO_PROTOCOL) / sizeof(VOID*)
                 );
    }
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_BODY,
               UsbIoDev->UsbController[Index],
               sizeof (USB_IO_CONTROLLER_DEVICE)
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &UsbIoDev->UsbController[Index]->UsbDevice,
               1
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               &UsbIoDev->UsbController[Index]->Parent,
               1
               );
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_POINTER,
               UsbIoDev->UsbController[Index]->Children,
               USB_MAXCHILDREN
               );
  }
}

/**

  Remove IoDevice from SMM convert table

  @param  UsbIoDev              UsbIoDevice

**/
STATIC
VOID
RemoveUsbIoDevAddressConvertTable(
  IN     USB_IO_DEVICE                  *UsbIoDev
  )
{
  EFI_USB_CORE_PROTOCOL    *UsbCore;
  
  UsbCore = mPrivate->UsbCore;
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_BODY,
             UsbIoDev
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_LINKING_LIST,
             &UsbIoDev->ConfigDescListHead
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbIoDev->ActiveConfig
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbIoDev->BusController
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             UsbIoDev->UsbController
             );
}

/**

  Remove IoControllerDevice from SMM convert table

  @param  UsbController         UsbController

**/
STATIC
VOID
RemoveUsbIoControllerDevAddressConvertTable(
  IN     USB_IO_CONTROLLER_DEVICE       *UsbController
  )
{
  EFI_USB_CORE_PROTOCOL    *UsbCore;
  
  UsbCore = mPrivate->UsbCore;
  UsbCore->RemoveAddressConvertTable (
             ACT_FUNCTION_POINTER,
             &UsbController->UsbIo
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_BODY,
             UsbController
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbController->UsbDevice
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             &UsbController->Parent
             );
  UsbCore->RemoveAddressConvertTable (
             ACT_INSTANCE_POINTER,
             UsbController->Children
             );
}