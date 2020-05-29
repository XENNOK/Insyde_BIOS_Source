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

  UsbHostController.h

Abstract:

  Usb Host Controller PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide Usb Host Contrller access services.

--*/

#ifndef _PEI_USB_HOST_CONTROLLER_PPI_H_
#define _PEI_USB_HOST_CONTROLLER_PPI_H_

#include "usb.h"

#define PEI_USB_HOST_CONTROLLER_PPI_GUID \
  { \
    0x652b38a9, 0x77f4, 0x453f, 0x89, 0xd5, 0xe7, 0xbd, 0xc3, 0x52, 0xfc, 0x53 \
  }

EFI_FORWARD_DECLARATION (PEI_USB_HOST_CONTROLLER_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_CONTROL_TRANSFER) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              DeviceType,
  IN     UINT8                              MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST             * Request,
  IN     EFI_USB_DATA_DIRECTION             TransferDirection,
  IN OUT VOID                               *Data OPTIONAL,
  IN OUT UINTN                              *DataLength OPTIONAL,
  IN     UINTN                              TimeOut,
  OUT    UINT32                             *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_BULK_TRANSFER) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  IN  UINT8                                 DeviceAddress,
  IN  UINT8                                 EndPointAddress,
  IN  UINT8                                 MaximumPacketLength,
  IN OUT VOID                               *Data,
  IN OUT UINTN                              *DataLength,
  IN OUT UINT8                              *DataToggle,
  IN  UINTN                                 TimeOut,
  OUT UINT32                                *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_SYNC_INT_TRANSFER) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  IN  UINT8                                 DeviceAddress,
  IN  UINT8                                 EndPointAddress,
  IN  UINT8                                 MaximumPacketLength,
  IN OUT VOID                               *Data,          
  IN OUT UINTN                              *DataLength,    
  IN OUT UINT8                              *DataToggle,    
  IN  UINTN                                 TimeOut,        
  OUT UINT32                                *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_NUMBER) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  OUT UINT8                                 *PortNumber
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_STATUS) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  IN  UINT8                                 PortNumber,
  OUT EFI_USB_PORT_STATUS                   * PortStatus
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_SET_ROOTHUB_PORT_FEATURE) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  IN UINT8                                  PortNumber,
  IN EFI_USB_PORT_FEATURE                   PortFeature
  );

typedef
EFI_STATUS
(EFIAPI *PEI_USB_HOST_CONTROLLER_CLEAR_ROOTHUB_PORT_FEATURE) (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI            * This,
  IN UINT8                                  PortNumber,
  IN EFI_USB_PORT_FEATURE                   PortFeature
  );

typedef struct _PEI_USB_HOST_CONTROLLER_PPI {
  PEI_USB_HOST_CONTROLLER_CONTROL_TRANSFER            ControlTransfer;
  PEI_USB_HOST_CONTROLLER_BULK_TRANSFER               BulkTransfer;
  PEI_USB_HOST_CONTROLLER_SYNC_INT_TRANSFER           SyncIntTransfer;    
  PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_NUMBER     GetRootHubPortNumber;
  PEI_USB_HOST_CONTROLLER_GET_ROOTHUB_PORT_STATUS     GetRootHubPortStatus;
  PEI_USB_HOST_CONTROLLER_SET_ROOTHUB_PORT_FEATURE    SetRootHubPortFeature;
  PEI_USB_HOST_CONTROLLER_CLEAR_ROOTHUB_PORT_FEATURE  ClearRootHubPortFeature;
} PEI_USB_HOST_CONTROLLER_PPI;

extern EFI_GUID gPeiUsbHostControllerPpiGuid;

#endif
