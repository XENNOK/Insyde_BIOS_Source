/** @file
  PCI_05 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_STATUS                           Status;
  UINT64                               PciAddress; 
  UINT8                                PciData8;            // read buffer
  UINT16                               Bus;
  UINT16                               Device;
  UINT16                               Func;
  UINT16                               Offset;
  UINT8                                ID_Buffer[4];        // store vendor_ID & device_ID
  UINT8                                Class_Buffer[3];     // store class_code

  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
  
  if (EFI_ERROR (Status)) {
    Print(L"FAIL\n");
    return Status;
  }

  Print(L"|----BUS----DEV----FUN----VID----DID----|\n");

  PciData8 = 0;
  for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
    for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
      for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
        for (Offset = 0; Offset <= 3; Offset++) {                   // get vendor_ID & device_ID
          PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
          Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,  // This
                                  EfiPciWidthUint8, // Width
                                  PciAddress,       // Address
                                  1,                // Count
                                  &PciData8         // *Buffer
                                  );
          ID_Buffer[Offset] = PciData8;                                  
        }
        for (Offset = 9; Offset <= 11; Offset++) {                  // get class_code(interface、sub_class、base_class)
          PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
          Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,  // This
                                  EfiPciWidthUint8, // Width
                                  PciAddress,       // Address
                                  1,                // Count
                                  &PciData8         // *Buffer
                                  );
          Class_Buffer[Offset-9] = PciData8;                                  
        }
        if (ID_Buffer[0] != 0xff && ID_Buffer[1] != 0xff) {
          if (Class_Buffer[2] == 6 && Class_Buffer[1] <= 4) {
            continue;
          }
          if (Class_Buffer[2] == 8 && Class_Buffer[1] <= 3) {
            continue;
          }
          Print(L"|%7.2x%7.2x%7.2x%5.2x%.2x%5.2x%-6.2x|\n",
            Bus, Device, Func, ID_Buffer[1], ID_Buffer[0], ID_Buffer[3], ID_Buffer[2]
          );
        }
      }
    }
  }

  Print(L"|---------------------------------------|\n");

  return Status;
}

/*
  <Uefi.h>--><Uefi/UefiSpec.h>
  LocateProtocol
  <Uefi.h>--><Uefi/UefiBaseType.h>
  EFI_ERROR()
  <Uefi.h>--><Uefi/UefiBaseType.h>--><Base.h>
  NULL

  <Library/UefiBootServicesTableLib.h>
  gBS

  <Protocol/PciRootBridgeIo.h>
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL->Pci.Read
  gEfiPciRootBridgeIoProtocolGuid
  EFI_PCI_ADDRESS()
  EfiPciWidthUint8

  <IndustryStandard/Pci22.h>
  PCI_MAX_BUS
  PCI_MAX_DEVICE
  PCI_MAX_FUNC
*/