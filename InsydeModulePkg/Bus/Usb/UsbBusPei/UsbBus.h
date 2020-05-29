/** @file
  Usb Bus Peim definition

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

#ifndef _PEI_USB_BUS_H
#define _PEI_USB_BUS_H

#include <PiPei.h>
#include <Ppi/Stall.h>
#include <Ppi/Usb3HostController.h>
#include <Ppi/Usb3Io.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#define MAX_ENDPOINT              16
#define STALL_1_MILLI_SECOND      1000        // stall 1 ms
#define TIMEOUT_VALUE             3000

#define PEI_USB_DEVICE_SIGNATURE        SIGNATURE_32 ('U', 's', 'b', 'D')
typedef struct {
  UINTN                                 Signature;
  EFI_PEI_SERVICES                      **PeiServices;
  EFI_PEI_STALL_PPI                     *StallPpi;
  PEI_USB3_IO_PPI                       UsbIoPpi;
  EFI_PEI_PPI_DESCRIPTOR                UsbIoPpiList;
  PEI_USB3_HOST_CONTROLLER_PPI          *UsbHcPpi;
  UINT8                                 DeviceAddress;
  UINT8                                 MaxPacketSize0;
  UINT8                                 DeviceSpeed;
  UINT8                                 DataToggle;
  UINT8                                 IsHub;
  UINT8                                 DownStreamPortNo;
  UINT8                                 HubDepth;
  UINT8                                 MultiTT;
  UINTN                                 AllocateAddress;
  UINT8                                 ConfigurationData[1024];
  EFI_USB_CONFIG_DESCRIPTOR             *ConfigDesc;
  EFI_USB_INTERFACE_DESCRIPTOR          *InterfaceDesc;
  EFI_USB_ENDPOINT_DESCRIPTOR           *EndpointDesc[MAX_ENDPOINT];
  EFI_USB3_HC_TRANSACTION_TRANSLATOR    Translator;
} PEI_USB_DEVICE;

#define PEI_USB_DEVICE_FROM_THIS(a)     CR (a, PEI_USB_DEVICE, UsbIoPpi, PEI_USB_DEVICE_SIGNATURE)


//
// Peim UsbIo prototype
//
EFI_STATUS
EFIAPI
PeiUsbControlTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     EFI_USB_DEVICE_REQUEST         *Request,
  IN     EFI_USB_DATA_DIRECTION         Direction,
  IN     UINT32                         Timeout,
  IN OUT VOID                           *Data,      OPTIONAL
  IN     UINTN                          DataLength  OPTIONAL
  );

EFI_STATUS
EFIAPI
PeiUsbBulkTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  );

EFI_STATUS
PeiUsbSyncInterruptTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  );

EFI_STATUS
EFIAPI
PeiUsbGetInterfaceDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB3_IO_PPI                   *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR      **InterfaceDescriptor
  );
 
EFI_STATUS
EFIAPI
PeiUsbGetEndpointDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB3_IO_PPI                   *This,
  IN  UINT8                             EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR       **EndpointDescriptor
  );

EFI_STATUS
EFIAPI
PeiUsbPortReset (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_USB3_IO_PPI                    *This
  );

EFI_STATUS
EFIAPI
PeiUsbClearEndpointHalt (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_USB3_IO_PPI                    *This,
  IN UINT8                              EndpointAddress
  );

VOID
ResetRootPort (
  IN PEI_USB_DEVICE                     *PeiUsbDevice,
  IN UINT8                              PortNum
  );

#endif
