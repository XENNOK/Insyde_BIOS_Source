/** @file
  UsbSerialControllerIo Protocol Definitions

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
#ifndef _USB_SERIAL_CONTROLLER_IO_H_
#define _USB_SERIAL_CONTROLLER_IO_H_

#include <Uefi.h>

#define USB_SERIAL_CONTROLLER_IO_PROTOCOL_GUID \
  { \
  0x1CCA0562, 0xE3C2, 0x4F0D, 0x93, 0x82, 0x89, 0, 0x27, 0x35, 0x26, 0x4F \
  }

typedef struct _USB_SERIAL_CONTROLLER_IO_PROTOCOL USB_SERIAL_CONTROLLER_IO_PROTOCOL;

typedef
EFI_STATUS
(*READ_USB_SERIAL_PORT) (
  IN  USB_SERIAL_CONTROLLER_IO_PROTOCOL  *UsbSCIo,
  IN  UINT8                              Offset,
  OUT UINT8                              *Data
  );

typedef
EFI_STATUS
(*WRITE_USB_SERIAL_PORT) (
  IN USB_SERIAL_CONTROLLER_IO_PROTOCOL  *UsbSCIo,
  IN UINT8                              Offset,
  IN UINT8                              Data
  );

typedef
EFI_STATUS
(*USB_SERIAL_CONTROLLER_RECEIVE) (
  IN  USB_SERIAL_CONTROLLER_IO_PROTOCOL  *UsbSCIo,
  IN  UINT8                              *Recieve
  );

typedef
EFI_STATUS
(*USB_SERIAL_CONTROLLER_TRANSFER) (
  IN  USB_SERIAL_CONTROLLER_IO_PROTOCOL  *UsbSCIo,
  IN  UINT8                              Tranfer
  );

typedef
EFI_STATUS
(*USB_SERIAL_CONTROLLER_GET_FIFO_STATE) (
  IN  USB_SERIAL_CONTROLLER_IO_PROTOCOL  *UsbSCIo,
  OUT UINT32                             *Control
  );

typedef struct _USB_SERIAL_CONTROLLER_IO_PROTOCOL {
  READ_USB_SERIAL_PORT                  ReadUsbSerialPort;
  WRITE_USB_SERIAL_PORT                 WriteUsbSerialPort;
  USB_SERIAL_CONTROLLER_RECEIVE         UsbSerialControllerReceive;
  USB_SERIAL_CONTROLLER_TRANSFER        UsbSerialControlllerTransfer;
  USB_SERIAL_CONTROLLER_GET_FIFO_STATE  UsbSerialControllerGetFifoState;
} USB_SERIAL_CONTROLLER_IO_PROTOCOL;


extern EFI_GUID gUsbSerialControllerIoProtocolGuid;

#endif
