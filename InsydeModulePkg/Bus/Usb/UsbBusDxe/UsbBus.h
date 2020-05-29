/** @file
  Header file for USB bus driver Interface

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

#ifndef _EFI_USB_BUS_H
#define _EFI_USB_BUS_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/UsbCore.h>
#include <Guid/StatusCodeDataTypeId.h>

#ifndef MDEPKG_NDEBUG
extern UINTN  gUSBDebugLevel;
extern UINTN  gUSBErrorLevel;
#endif

#define MAX_LOW_SPEED_HC                16

#define MICROSECOND                     10000
#define ONESECOND                       (1000 * MICROSECOND)
#define BUSPOLLING_PERIOD               ONESECOND
//
// Legacy pause related definitions
//
#define CLASS_HID                       3
#define SUBCLASS_BOOT                   1
#define PROTOCOL_KEYBOARD               1
#define PAUSE_IN_PROGRESS               ((*(UINT8*)(UINTN)0x418) & 0x08)
//
// We define some maximun value here
//
#define USB_MAXCONFIG                   8
#define USB_MAXALTSETTING               4
#define USB_MAXINTERFACES               32
#define USB_MAXENDPOINTS                16
#define USB_MAXSTRINGS                  16
#define USB_MAXLANID                    16
#define USB_MAXCHILDREN                 32

#define IMAGE_DRIVER_NAME               L"Usb Bus Driver"
#define USB_IO_CONTROLLER_SIGNATURE     SIGNATURE_32 ('u', 's', 'b', 'd')

typedef struct {
  UINT16                                ChangedBit;
  EFI_USB_PORT_FEATURE                  Feature;
} USB_CHANGE_FEATURE_MAP;

typedef struct {
  LIST_ENTRY                            Link;
  UINT16                                StringIndex;
  CHAR16                                *String;
} STR_LIST_ENTRY;

typedef struct {
  LIST_ENTRY                            Link;
  UINT16                                Toggle;
  EFI_USB_ENDPOINT_DESCRIPTOR           EndpointDescriptor;
} ENDPOINT_DESC_LIST_ENTRY;

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB_INTERFACE_DESCRIPTOR          InterfaceDescriptor;
  EFI_USB_HID_DESCRIPTOR                HidDescriptor;
  UINT8                                 Reserved[0x100];
  LIST_ENTRY                            EndpointDescListHead;
} INTERFACE_DESC_LIST_ENTRY;

typedef struct {
  LIST_ENTRY                            Link;
  EFI_USB_CONFIG_DESCRIPTOR             CongfigDescriptor;
  LIST_ENTRY                            InterfaceDescListHead;
  UINTN                                 ActiveInterface;
} CONFIG_DESC_LIST_ENTRY;

//
// Forward declaring
//
struct usb_io_device;

//
// This is used to form the USB Controller Handle
//
typedef struct usb_io_controller_device {
  UINTN                                 Signature;
  //
  // Keep the UsbIo here due to UsbCore will locate it without reference the structure
  //
  EFI_USB_IO_PROTOCOL                   UsbIo;
  EFI_HANDLE                            Handle;
  EFI_DEVICE_PATH_PROTOCOL              *UefiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *RemainingDevicePath;
  EFI_HANDLE                            HostController;
  UINT8                                 CurrentConfigValue;
  UINT8                                 InterfaceNumber;
  UINT8                                 AlternateSetting;
  BOOLEAN                               Inactive;
  struct usb_io_device                  *UsbDevice;
  
  BOOLEAN                               IsUsbHub;
  BOOLEAN                               IsManagedByDriver;
  BOOLEAN                               PortResetProcessing;

  //
  // Fields specified for USB Hub
  //
  EFI_EVENT                             HubNotify;
  UINT8                                 HubEndpointAddress;
  UINT16                                StatusChangePort;
  UINT8                                 DownstreamPorts;
  UINT8                                 MultiTT;
  UINT8                                 ParentPort;
  struct usb_io_controller_device       *Parent;
  struct usb_io_device                  *Children[USB_MAXCHILDREN];
} USB_IO_CONTROLLER_DEVICE;

#define USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS(a) \
  CR (a, \
      USB_IO_CONTROLLER_DEVICE, \
      UsbIo, \
      USB_IO_CONTROLLER_SIGNATURE \
      )

//
// This is used to keep the topology of USB bus
//
struct _usb_bus_controller_device;

typedef struct usb_io_device {
  UINT8                                 DeviceAddress;
  BOOLEAN                               IsConfigured;
  UINT8                                 DeviceSpeed;
  UINT8                                 HubDepth;
  EFI_USB3_HC_TRANSACTION_TRANSLATOR    Translator;
  EFI_USB_DEVICE_DESCRIPTOR             DeviceDescriptor;
  LIST_ENTRY                            ConfigDescListHead;
  CONFIG_DESC_LIST_ENTRY                *ActiveConfig;
  BOOLEAN                               LangIDConfigured;
  UINT16                                LangID[USB_MAXLANID];
  struct _usb_bus_controller_device     *BusController;

  //
  // Track the controller handle
  //
  UINT8                                 NumOfInterfaces;
  USB_IO_CONTROLLER_DEVICE              *UsbController[USB_MAXINTERFACES];

} USB_IO_DEVICE;

//
// Usb Bus Controller device strcuture
//
#define EFI_USB_BUS_PROTOCOL_GUID \
  { \
    0x2B2F68CC, 0x0CD2, 0x44cf, 0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75 \
  }

typedef struct _EFI_USB_BUS_PROTOCOL {
  UINT64  Reserved;
} EFI_USB_BUS_PROTOCOL;

#define USB_BUS_DEVICE_SIGNATURE        SIGNATURE_32 ('u', 'b', 'u', 's')

typedef struct _usb_bus_controller_device {
  UINTN                                 Signature;
  EFI_USB_BUS_PROTOCOL                  BusIdentify;
  EFI_USB3_HC_PROTOCOL                  *Usb3HCInterface;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  UINT8                                 AddressPool[32];
  USB_IO_DEVICE                         *Root;
} USB_BUS_CONTROLLER_DEVICE;

#define USB_BUS_CONTROLLER_DEVICE_FROM_THIS(a)  CR (a, USB_BUS_CONTROLLER_DEVICE, BusIdentify, USB_BUS_DEVICE_SIGNATURE)

typedef struct {
  EFI_HANDLE                            Controller;
  UINT8                                 SegNum;
  UINT8                                 BusNum;
  UINT8                                 DevNum;
  UINT8                                 FuncNum;
} USB_HC_DESC;

typedef struct {
  UINTN                                 Signature;
  EFI_USB_IO_PROTOCOL                   UsbIoInterface;
  EFI_USB_CORE_PROTOCOL                 *UsbCore;
  INTN                                  NumLowSpeedHc;
  USB_HC_DESC                           LowSpeedHc[MAX_LOW_SPEED_HC];
} USB_BUS_PRIVATE;

extern  USB_BUS_PRIVATE                 *mPrivate;
#define USB_BUS_SIGNATURE               SIGNATURE_32 ('b', 'u', 's', 'p')

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL      gUsbBusDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gUsbBusComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gUsbBusComponentName2;



/**

  Remove Device, Device Handles, Uninstall Protocols.

  @param  UsbIoDev              The device to be deconfigured.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
UsbDeviceDeConfiguration (
  IN USB_IO_DEVICE     *UsbIoDevice
  );

#endif
