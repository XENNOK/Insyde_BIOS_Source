/** @file

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

#ifndef _USB_TOUCH_PANEL_H_
#define _USB_TOUCH_PANEL_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/HidDescriptorLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/UsbCore.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/EventGroup.h>
#include <Guid/EventLegacyBios.h>

#define CLASS_HID                       3
#define SUBCLASS_NONE                   0
#define PROTOCOL_NONE                   0

#define BOOT_PROTOCOL                   0
#define REPORT_PROTOCOL                 1

#define IMAGE_DRIVER_NAME               L"Usb Touch Panel Driver"
#define CONTROLLER_DRIVER_NAME          L"Generic Usb Touch Panel"
#define USB_TOUCH_PANEL_DEV_SIGNATURE   SIGNATURE_32 ('t', 'u', 'p', 'l')

#define EFI_PERIPHERAL_TOUCH_PANEL      (EFI_PERIPHERAL_UNSPECIFIED | 0x00001000)

typedef struct {
  UINTN                         Signature;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR   IntEndpointDescriptor;
  UINT8                         ReportId;
  EFI_ABSOLUTE_POINTER_PROTOCOL AbsolutePointerProtocol;
  EFI_ABSOLUTE_POINTER_STATE    State;
  EFI_ABSOLUTE_POINTER_MODE     Mode;
  BOOLEAN                       StateChanged;
  REPORT_FIELD_INFO             ReportFieldInfo;
  EFI_UNICODE_STRING_TABLE      *ControllerNameTable;
  EFI_HANDLE                    RecoveryPollingHandle;
} USB_TOUCH_PANEL_DEV;

#define USB_TOUCH_PANEL_DEV_FROM_TOUCH_PANEL_PROTOCOL(a) \
    CR(a, USB_TOUCH_PANEL_DEV, AbsolutePointerProtocol, USB_TOUCH_PANEL_DEV_SIGNATURE)

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL  gUsbTouchPanelDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL  gUsbTouchPanelComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gUsbTouchPanelComponentName2;

#endif
