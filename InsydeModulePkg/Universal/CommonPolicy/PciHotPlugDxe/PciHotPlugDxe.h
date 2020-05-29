/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PCI_HOT_PLUG_H_
#define _PCI_HOT_PLUG_H_

#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DxeOemSvcKernelLib.h>

#define EFI_PCIHOTPLUG_DRIVER_PRIVATE_SIGNATURE SIGNATURE_32 ('G', 'L', 'U', 'P')

#define ACPI(NodeUID) \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (0x0A03), NodeUID\
  }

#define PCI(device, function) \
  { \
    HARDWARE_DEVICE_PATH, HW_PCI_DP, (UINT8) (sizeof (PCI_DEVICE_PATH)), (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8), \
      (UINTN) function, (UINTN) device \
  }

#define END \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 \
  }

#define LPC(eisaid, function) \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (eisaid), function \
  }

#ifndef SIZE_4K 
#define SIZE_4K  0x00001000
#endif
#ifndef SIZE_16M 
#define SIZE_16M 0x1000000
#endif
typedef struct PCIE_HOT_PLUG_DEVICE_PATH {
  ACPI_HID_DEVICE_PATH      PciRootBridgeNode;
  PCI_DEVICE_PATH           PciRootPortNode;
  EFI_DEVICE_PATH_PROTOCOL  EndDeviceNode;
} PCIE_HOT_PLUG_DEVICE_PATH;

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle; // Handle for protocol this driver installs on
  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  HotPlugInitProtocol;
} PCI_HOT_PLUG_INSTANCE;

EFI_STATUS
EFIAPI
GetRootHpcList (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *This,
  OUT UINTN                             *PhpcCount,
  OUT EFI_HPC_LOCATION                  **PhpcList
  );

EFI_STATUS
EFIAPI
InitializeRootHpc (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  *This,
  IN  EFI_DEVICE_PATH_PROTOCOL        *PhpcDevicePath,
  IN  UINT64                          PhpcPciAddress,
  IN  EFI_EVENT                       Event, OPTIONAL
  OUT EFI_HPC_STATE                   *PhpcState
  );

EFI_STATUS
EFIAPI
GetResourcePadding (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  *This,
  IN  EFI_DEVICE_PATH_PROTOCOL        *PhpcDevicePath,
  IN  UINT64                          PhpcPciAddress,
  OUT EFI_HPC_STATE                   *PhpcState,
  OUT VOID                            **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES      *Attributes
  );

VOID
ProcessHotplugBridgeInfoTable (
  IN HOT_PLUG_BRIDGE_INFO  *HotPlugBridgeInfoTable
);

#endif
