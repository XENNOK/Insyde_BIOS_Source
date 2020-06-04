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
  UINT16                               BusNum;
  UINT16                               DeviceNum;
  UINT16                               FuncNum;
  UINT16                               Counter;
  UINT16                               ClassCounter;
  UINT8                                ID_Buffer[4];        // store vendor_ID & device_ID
  UINT8                                Class_Buffer[3];     // store class_code
  
  
  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
  
  if (EFI_ERROR (Status)) {
    Print(L"FAIL\n");
    return Status;
  }

  Print(L"|---BUSNum--DEVNum--FUNNum--VIDNum--DIDNum---|\n");

  PciData8 = 0;
  for (BusNum = 0; BusNum <= PCI_MAX_BUS; BusNum++) {
    for (DeviceNum = 0; DeviceNum <= PCI_MAX_DEVICE; DeviceNum++) {
      for (FuncNum = 0; FuncNum <= PCI_MAX_FUNC; FuncNum++) {
        for (Counter = 0; Counter <= 3; Counter++) {                   // get vendor_ID & device_ID
          PciAddress = EFI_PCI_ADDRESS (BusNum, DeviceNum, FuncNum, Counter);
          Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,  // This
                                  EfiPciWidthUint8, // Width
                                  PciAddress,       // Address
                                  1,                // Count
                                  &PciData8         // *Buffer
                                  );
          ID_Buffer[Counter] = PciData8;                                  
        }
        for (ClassCounter = 9; ClassCounter <= 11; ClassCounter++) {                  // get class_code(interface、sub_class、base_class)
          PciAddress = EFI_PCI_ADDRESS (BusNum, DeviceNum, FuncNum, ClassCounter);
          Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,  // This
                                  EfiPciWidthUint8, // Width
                                  PciAddress,       // Address
                                  1,                // Count
                                  &PciData8         // *Buffer
                                  );
          Class_Buffer[ClassCounter-9] = PciData8;                                  
        }
   
        if (ID_Buffer[0] != 0xff && ID_Buffer[1] != 0xff&&  
            !(((Class_Buffer[2] == 0x06) &&                 // spec P224
              (Class_Buffer[1]<=0x04)) ||                   //濾掉不是device
             ((Class_Buffer[2] == 0x08) &&
              (Class_Buffer[1]<=0x03)))) {
    
          Print(L"|%7.2x%8.2x%8.2x%7.2x%.2x%6.2x%-6.2x|\n",
            BusNum, DeviceNum, FuncNum, ID_Buffer[1], ID_Buffer[0], ID_Buffer[3], ID_Buffer[2]
          );
          Print(L"|--------------------------------------------|\n");
        }
      }
    }
  }
   return Status;
}
