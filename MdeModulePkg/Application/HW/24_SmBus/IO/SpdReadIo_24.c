/** @file
  SpdReadIo_24 C Source File

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

#include "SpdReadIo_24.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_STATUS                           Status;
  UINT64                               PciAddress; 
  UINT8                                PciData8;            // read buffer
  UINT32                               PciData32;           
  UINT16                               Bus;
  UINT16                               Device;
  UINT16                               Func;
  UINT16                               Offset;
  UINT8                                Class_Buffer[3];     // store class_code

  EFI_CPU_IO2_PROTOCOL                 *CpuIo2;
  UINT8                                Buffer;

  UINT64                               SmbusBaseIoAddress;
  UINT64                               SmBusOffset; 
  UINT64                               SlaveAddress;
  // UINT8                                Buffer[256];

  SmbusBaseIoAddress = 0;

  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &CpuIo2);

  for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
    for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
      for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
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

        //
        // class code -> SMBus Controller (0x0, 0x5, 0xc)
        //
        if (Class_Buffer[0] == 0x0 && Class_Buffer[1] == 0x5 && Class_Buffer[2] == 0xc) {
          for (Offset = 0; Offset < 256; Offset++) {
            PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
            Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,  // This
                                    EfiPciWidthUint8, // Width
                                    PciAddress,       // Address
                                    1,                // Count
                                    &PciData8         // *Buffer
                                    );
            // Print(L"%3.2x", PciData8);
            // if ((Offset % 16) == 15) Print(L"\n");                                
          }

          Offset = SMBUS_BASE_ADDRESS;
          PciAddress = EFI_PCI_ADDRESS (Bus, Device, Func, Offset);
          Status = PciRootBridgeIo->Pci.Read (
                                          PciRootBridgeIo,   // This
                                          EfiPciWidthUint32, // Width
                                          PciAddress,        // Address
                                          1,                 // Count
                                          &PciData32         // *Buffer
                                          );
          SmbusBaseIoAddress = (UINT64)PciData32 & 0xffe0;
          // Print(L"SmbusBaseIoAddress->%x\n", SmbusBaseIoAddress);

        }
      }
    }
  }

  for (SlaveAddress = 0xa0; SlaveAddress <= 0xa6; SlaveAddress += 2) {
    Print(L"Address: 0x%x", SlaveAddress);
    for (SmBusOffset = 0; SmBusOffset < 256; SmBusOffset++) {
      
      //
      // Host Status (00h)
      // Writing 0xfe is for clean register.
      //
      Buffer = 0xfe;
      CpuIo2->Io.Write (
                    CpuIo2,
                    EfiCpuIoWidthUint8,
                    SmbusBaseIoAddress,     
                    1,
                    &Buffer   
                    );
      do {
        Buffer = 0x0;
        CpuIo2->Io.Read (
                      CpuIo2,
                      EfiCpuIoWidthUint8,
                      SmbusBaseIoAddress,     
                      1,
                      &Buffer   
                      );
      } while (Buffer != 0x40);

      //
      // Transmit Slave Address (04h)
      // Bit 0 = 1 -> Read  SPD
      // Bit 0 = 0 -> Write SPD
      //
      Buffer = (UINT8)SlaveAddress + 1;
      CpuIo2->Io.Write (
                    CpuIo2,
                    EfiCpuIoWidthUint8,
                    SmbusBaseIoAddress + 4,     
                    1,
                    &Buffer   
                    );
      do {
        Buffer = 0x0;
        CpuIo2->Io.Read (
                      CpuIo2,
                      EfiCpuIoWidthUint8,
                      SmbusBaseIoAddress,     
                      1,
                      &Buffer   
                      );
      } while ((Buffer & 0x1) != 0x0);

      //
      // Host Command (03h)
      // Select SMBus offset.
      //
      Buffer = (UINT8)SmBusOffset;
      CpuIo2->Io.Write (
                    CpuIo2,
                    EfiCpuIoWidthUint8,
                    SmbusBaseIoAddress + 3,     
                    1,
                    &Buffer   
                    );
      do {
        Buffer = 0x0;
        CpuIo2->Io.Read (
                      CpuIo2,
                      EfiCpuIoWidthUint8,
                      SmbusBaseIoAddress,     
                      1,
                      &Buffer   
                      );
      } while ((Buffer & 0x1) != 0x0);

      //
      // Host Control (02h)
      // Select read mode and start command.
      //
      Buffer = 0x48;
      CpuIo2->Io.Write (
                    CpuIo2,
                    EfiCpuIoWidthUint8,
                    SmbusBaseIoAddress + 2,     
                    1,
                    &Buffer   
                    );

      while (TRUE) {
        Buffer = 0x0;
        CpuIo2->Io.Read (
                      CpuIo2,
                      EfiCpuIoWidthUint8,
                      SmbusBaseIoAddress,     
                      1,
                      &Buffer   
                      );
        if ((Buffer & 0x1) == 0x0 && Buffer != 0x42) {
          Print(L" ---> Device Error !!!\n");
          goto NEXT;
        } else if ((Buffer & 0x1) == 0x0 && Buffer == 0x42) {
          break;
        }
      }

      //
      // Host Data 0 (05h)
      // The place to get data.
      //
      CpuIo2->Io.Read (
                   CpuIo2,
                   EfiCpuIoWidthUint8,
                   SmbusBaseIoAddress + 5,     
                   1,
                   &Buffer   
                   );
      if (SmBusOffset == 0) {
        Print(L"\n");
      }
      Print(L"%3.2x", Buffer);
      if (SmBusOffset % 16 == 15) {
        Print(L"\n");
      }
    }

    NEXT:;
  }
  
  return Status;
}