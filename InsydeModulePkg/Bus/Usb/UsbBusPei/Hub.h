/** @file
  Constants definitions for Usb Hub Peim

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

#ifndef _PEI_HUB_H
#define _PEI_HUB_H

//
// Hub feature numbers
//
#define C_HUB_LOCAL_POWER   0
#define C_HUB_OVER_CURRENT  1

//
// Hub class code & sub class code
//
#define CLASS_CODE_HUB      0x09
#define SUB_CLASS_CODE_HUB  0

//
// Hub Status & Hub Change bit masks
//
#define HUB_STATUS_LOCAL_POWER  0x0001
#define HUB_STATUS_OVERCURRENT  0x0002

#define HUB_CHANGE_LOCAL_POWER  0x0001
#define HUB_CHANGE_OVERCURRENT  0x0002

//
// Hub Characteristics
//
#define HUB_CHAR_LPSM     0x0003
#define HUB_CHAR_COMPOUND 0x0004
#define HUB_CHAR_OCPM     0x0018

//
// Hub specific request
//
#define HUB_CLEAR_FEATURE               0x01
#define HUB_CLEAR_FEATURE_REQ_TYPE      0x20

#define HUB_CLEAR_FEATURE_PORT          0x01
#define HUB_CLEAR_FEATURE_PORT_REQ_TYPE 0x23

#define HUB_GET_BUS_STATE               0x02
#define HUB_GET_BUS_STATE_REQ_TYPE      0xA3

#define HUB_GET_DESCRIPTOR              0x06
#define HUB_GET_DESCRIPTOR_REQ_TYPE     0xA0

#define HUB_GET_HUB_STATUS              0x00
#define HUB_GET_HUB_STATUS_REQ_TYPE     0xA0

#define HUB_GET_PORT_STATUS             0x00
#define HUB_GET_PORT_STATUS_REQ_TYPE    0xA3

#define HUB_SET_DESCRIPTOR              0x07
#define HUB_SET_DESCRIPTOR_REQ_TYPE     0x20

#define HUB_SET_HUB_FEATURE             0x03
#define HUB_SET_HUB_FEATURE_REQ_TYPE    0x20

#define HUB_SET_PORT_FEATURE            0x03
#define HUB_SET_PORT_FEATURE_REQ_TYPE   0x23

#define HUB_SET_HUB_DEPTH               0x0C
#define HUB_SET_HUB_DEPTH_REQ_TYPE      0x20

EFI_STATUS
PeiHubGetPortStatus (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  IN  UINT8                     Port,
  OUT UINT32                    *PortStatus
  );

EFI_STATUS
PeiHubSetPortFeature (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  IN  UINT8                     Port,
  IN  UINT8                     Value
  );

EFI_STATUS
PeiHubSetHubFeature (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  IN  UINT8                     Value
  );

EFI_STATUS
PeiHubGetHubStatus (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  OUT UINT32                    *HubStatus
  );

EFI_STATUS
PeiHubClearPortFeature (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  IN  UINT8                     Port,
  IN  UINT8                     Value
  );

EFI_STATUS
PeiHubClearHubFeature (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  IN  UINT8                     Value
  );

EFI_STATUS
PeiGetHubDescriptor (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_USB3_IO_PPI            *UsbIoPpi,
  IN UINTN                      DescriptorSize,
  OUT EFI_USB_HUB_DESCRIPTOR    *HubDescriptor
  );

EFI_STATUS
DoHubConfig (
  IN PEI_USB_DEVICE             *PeiUsbDevice
  );

VOID
ResetHubPort (
  IN PEI_USB_DEVICE             *PeiUsbDevice,
  IN UINT8                      PortNum
  );

#endif


