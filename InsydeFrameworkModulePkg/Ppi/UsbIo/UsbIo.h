//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UsbIo.h

Abstract:

  Usb access PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide Usb access services.

--*/

#ifndef _PEI_USB_IO_PPI_H_
#define _PEI_USB_IO_PPI_H_

#include "usb.h"

#define PEI_USB_IO_PPI_GUID \
  { \
    0x7c29785c, 0x66b9, 0x49fc, 0xb7, 0x97, 0x1c, 0xa5, 0x55, 0xe, 0xf2, 0x83 \
  }

EFI_FORWARD_DECLARATION (PEI_USB_IO_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_USB_CONTROL_TRANSFER) (
  IN EFI_PEI_SERVICES                           **PeiServices,
  IN PEI_USB_IO_PPI                             * This,
  IN EFI_USB_DEVICE_REQUEST                     * Request,
  IN EFI_USB_DATA_DIRECTION                     Direction,
  IN UINT32                                     Timeout,
  IN OUT VOID                                   *Data OPTIONAL,
  IN UINTN                                      DataLength  OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_BULK_TRANSFER) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_IO_PPI                 * This,
  IN UINT8                          DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN UINTN                          Timeout
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_SYNC_INT_TRANSFER) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_IO_PPI                 * This,
  IN UINT8                          DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN UINTN                          Timeout
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_GET_INTERFACE_DESCRIPTOR) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_IO_PPI                 * This,
  IN EFI_USB_INTERFACE_DESCRIPTOR   **InterfaceDescriptor
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_GET_ENDPOINT_DESCRIPTOR) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_IO_PPI                 * This,
  IN UINT8                          EndpointIndex,
  IN EFI_USB_ENDPOINT_DESCRIPTOR    **EndpointDescriptor
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_PORT_RESET) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB_IO_PPI                 * This
  );

typedef struct _PEI_USB_IO_PPI {
  PEI_USB_CONTROL_TRANSFER          UsbControlTransfer;
  PEI_USB_BULK_TRANSFER             UsbBulkTransfer;
  PEI_USB_SYNC_INT_TRANSFER         UsbSyncIntTransfer;
  PEI_USB_GET_INTERFACE_DESCRIPTOR  UsbGetInterfaceDescriptor;
  PEI_USB_GET_ENDPOINT_DESCRIPTOR   UsbGetEndpointDescriptor;
  PEI_USB_PORT_RESET                UsbPortReset;
} PEI_USB_IO_PPI;

extern EFI_GUID gPeiUsbIoPpiGuid;

#endif
