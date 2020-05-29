/** @file
  Common Command Header for USB

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

#ifndef _USB_CMDS_H
#define _USB_CMDS_H

//
// HID class protocol request
//
#define EFI_USB_GET_REPORT_REQUEST    0x01
#define EFI_USB_GET_IDLE_REQUEST      0x02
#define EFI_USB_GET_PROTOCOL_REQUEST  0x03
#define EFI_USB_SET_REPORT_REQUEST    0x09
#define EFI_USB_SET_IDLE_REQUEST      0x0a
#define EFI_USB_SET_PROTOCOL_REQUEST  0x0b
//
// Get Device Descriptor
//
EFI_STATUS
EFIAPI
UsbGetDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Value,
  IN  UINT16                  Index,
  IN  UINT16                  DescriptorLength,
  OUT VOID                    *Descriptor,
  OUT UINT32                  *Status
  );

//
// Set Device Descriptor
//
EFI_STATUS
EFIAPI
UsbSetDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Value,
  IN  UINT16                  Index,
  IN  UINT16                  DescriptorLength,
  IN  VOID                    *Descriptor,
  OUT UINT32                  *Status
  );

//
// Get device Interface
//
EFI_STATUS
EFIAPI
UsbGetDeviceInterface (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Index,
  OUT UINT8                   *AltSetting,
  OUT UINT32                  *Status
  );

//
// Set device interface
//
EFI_STATUS
EFIAPI
UsbSetDeviceInterface (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  InterfaceNo,
  IN  UINT16                  AltSetting,
  OUT UINT32                  *Status
  );

//
// Get device configuration
//
EFI_STATUS
EFIAPI
UsbGetDeviceConfiguration (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  OUT UINT8                   *ConfigValue,
  OUT UINT32                  *Status
  );

//
// Set device configuration
//
EFI_STATUS
EFIAPI
UsbSetDeviceConfiguration (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  Value,
  OUT UINT32                  *Status
  );

//
//  Set Device Feature
//
EFI_STATUS
EFIAPI
UsbSetDeviceFeature (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Recipient,
  IN  UINT16                  Value,
  IN  UINT16                  Target,
  OUT UINT32                  *Status
  );

//
// Clear Device Feature
//
EFI_STATUS
EFIAPI
UsbClearDeviceFeature (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Recipient,
  IN  UINT16                  Value,
  IN  UINT16                  Target,
  OUT UINT32                  *Status
  );

//
//  Get Device Status
//
EFI_STATUS
EFIAPI
UsbGetDeviceStatus (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Recipient,
  IN  UINT16                  Target,
  OUT UINT16                  *DevStatus,
  OUT UINT32                  *Status
  );

//
// The following APIs are not basic library, but they are common used.
//
//
// Usb Get String
//
EFI_STATUS
EFIAPI
UsbGetString (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  LangID,
  IN  UINT8                   Index,
  IN  VOID                    *Buf,
  IN  UINTN                   BufSize,
  OUT UINT32                  *Status
  );

//
// Clear endpoint stall
//
EFI_STATUS
EFIAPI
UsbClearEndpointHalt (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   EndpointNo,
  OUT UINT32                  *Status
  );

//
// Get HID Descriptor
//
EFI_STATUS
EFIAPI
UsbGetHidDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   InterfaceNum,
  OUT VOID                    *HidDescriptor
  );

//
// Get Report Descriptor for HID
//
EFI_STATUS
EFIAPI
UsbGetReportDescriptor (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   InterfaceNum,
  IN  UINT16                  DescriptorSize,
  OUT UINT8                   *DescriptorBuffer
  );

//
// Get Protocol for HID
//
EFI_STATUS
EFIAPI
UsbGetProtocolRequest (
  IN EFI_USB_IO_PROTOCOL      *UsbIo,
  IN UINT8                    Interface,
  IN UINT8                    *Protocol
  );

//
// Set Protocol for HID
//
EFI_STATUS
EFIAPI
UsbSetProtocolRequest (
  IN EFI_USB_IO_PROTOCOL      *UsbIo,
  IN UINT8                    Interface,
  IN UINT8                    Protocol
  );

//
// Set Idle for HID
//
EFI_STATUS
EFIAPI
UsbSetIdleRequest (
  IN EFI_USB_IO_PROTOCOL      *UsbIo,
  IN UINT8                    Interface,
  IN UINT8                    ReportId,
  IN UINT8                    Duration
  );

//
// Get Idle for HID
//
EFI_STATUS
EFIAPI
UsbGetIdleRequest (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT8                   Interface,
  IN  UINT8                   ReportId,
  OUT UINT8                   *Duration
  );

//
// Set Report for HID
//
EFI_STATUS
EFIAPI
UsbSetReportRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   ReportType,
  IN UINT16                  ReportLen,
  IN UINT8                   *Report
  );

//
// Get Report for HID
//
EFI_STATUS
EFIAPI
UsbGetReportRequest (
  IN EFI_USB_IO_PROTOCOL     *UsbIo,
  IN UINT8                   Interface,
  IN UINT8                   ReportId,
  IN UINT8                   ReportType,
  IN UINT16                  ReportLen,
  IN UINT8                   *Report
  );

#endif
