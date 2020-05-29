//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _USB_PORT_INFO_H_
#define _USB_PORT_INFO_H_

#define EFI_USB_PORT_INFO_PROTOCOL_GUID \
  {0x242f5719, 0x94f8, 0x4044, 0xa8, 0x3f, 0x5d, 0xc7, 0xa3, 0x14, 0x40, 0x36}

#pragma pack (1)
typedef struct {
 EFI_PHYSICAL_ADDRESS PciDeviceAddress;
 UINT8    USBPort;
 UINT16   VendorId;
 UINT16   ProductId;
 UINT8    Reserve[3];
} PORT_INFO_STRUCT;
#pragma pack ()

typedef struct {
  UINT8                     USBPortCount;
  PORT_INFO_STRUCT          *PortInfo;
} EFI_USB_PORT_INFO_PROTOCOL;

extern EFI_GUID gEfiUsbPortInfoProtocolGuid;

#endif
