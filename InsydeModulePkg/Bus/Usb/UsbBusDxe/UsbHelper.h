/** @file
  Helper functions for USB

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

#ifndef _USB_HELPER_H
#define _USB_HELPER_H

#pragma pack(1)

typedef struct {
  UINT8   Length;
  UINT8   DescriptorType;
  UINT16  SupportedLanID[1];
} EFI_USB_SUPPORTED_LANGUAGES;

#pragma pack()

EFI_STATUS
UsbGetStringtable (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

EFI_STATUS
UsbGetAllConfigurations (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

EFI_STATUS
UsbSetConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev,
  IN  UINTN                             ConfigurationValue
  );

EFI_STATUS
UsbSetDefaultConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

VOID
UsbDestroyAllConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

BOOLEAN
IsPortConnect (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortEnable (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortInReset (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortPowerApplied (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortSuspend (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortConnectChange (
  IN UINT16                             PortChangeStatus
  );

BOOLEAN
IsPortEnableDisableChange (
  IN UINT16                             PortChangeStatus
  );

BOOLEAN
IsPortResetChange (
  IN UINT16                             PortChangeStatus
  );

BOOLEAN
IsPortSuspendChange (
  IN UINT16                             PortChangeStatus
  );

EFI_STATUS
IsDeviceDisconnected (
  IN USB_IO_CONTROLLER_DEVICE           *UsbIoController,
  IN OUT BOOLEAN                        *Disconnected
  );

UINT8
GetFirstAlternateSetting (
  IN EFI_USB_IO_PROTOCOL                *This
  );

EFI_USB_HID_DESCRIPTOR*
GetHidDescriptor (
  IN EFI_USB_IO_PROTOCOL                *This
  );

INTERFACE_DESC_LIST_ENTRY*
FindInterfaceListEntry (
  IN EFI_USB_IO_PROTOCOL                *This
  );

ENDPOINT_DESC_LIST_ENTRY*
FindEndPointListEntry (
  IN EFI_USB_IO_PROTOCOL                *This,
  IN UINT8                              EndPointAddress
  );

VOID
SetDataToggleBit (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              EndpointAddr,
  IN UINT8                              DataToggle
  );

EFI_STATUS
UsbSetDeviceAddress (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            AddressValue,
  OUT UINT32                            *Status
  );

#endif
