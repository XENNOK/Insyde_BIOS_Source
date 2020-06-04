/** @file

  Non-Driver Model Driver Homework
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include "NonDriver_HW.h"

EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraDrv;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *RootBridgeIo;
/**

  Dump pci devices data.
  @param[in]       *PciRootBridgeIo     PciRootBridegIo Protocol
  @param[in][out]  *HPciDev             pci devices struct
  @param[in]       DataIndex            Index of pci devices struct

*/
VOID
DumpData (
  IN OUT PCIDEV                          *HPciDev,
  IN UINTN                               DataIndex
  )
{
  UINT8 OFFSET;
  UINT8 ROWS;
  UINT8 COLS;
  EFI_STATUS Status;
  
  OFFSET = 0;
  
  Status  = RootBridgeIo->Pci.Read (
                                RootBridgeIo,
                                EfiPciWidthUint32,
                                EFI_PCI_ADDRESS (
                                  HPciDev[DataIndex].BusNum,
                                  HPciDev[DataIndex].DevNum,
                                  HPciDev[DataIndex].FunNum,
                                  0x00
                                  ),
                                (sizeof(HPciDev[DataIndex].Pci) / sizeof(UINT32)),
                                &HPciDev[DataIndex].Pci
                                );
  if (EFI_ERROR (Status)) {
    Print(
      L"DumpPCIData Pci.Read ERROR\n"
      );
  }
  
  for (ROWS=0; ROWS <= DATA_ARRAY_ROW_MAX; ROWS++) {
    for (COLS=0; COLS <= DATA_ARRAY_COLS_MAX; COLS++) {
      
      Status = RootBridgeIo->Pci.Read (
                                   RootBridgeIo,
                                   EfiPciWidthUint8,
                                   EFI_PCI_ADDRESS (
                                     HPciDev[DataIndex].BusNum,
                                     HPciDev[DataIndex].DevNum,
                                     HPciDev[DataIndex].FunNum,
                                     OFFSET
                                     ),
                                   1,
                                   &HPciDev[DataIndex].PcidevData[ROWS][COLS]
                                   );
      
      if (EFI_ERROR (Status)) {
        Print (
          L"DumpError"
          );
      }
      
      OFFSET = OFFSET + 1;
      
    }
  }
}


/**
  Grep VGA pci devices.

  @param[in][out]  *HPciDev             pci devices struct

*/
UINT16 
GrepWLANPCI (
  IN OUT PCIDEV                          *HPciDev
  )
{
  EFI_STATUS   Status;
  UINT16       BusNum;
  UINT8        DevNum;
  UINT8        FunNum;
  UINT16       TotalDevNum;
  

  TotalDevNum = 0;
  
  for (BusNum=0; BusNum <= PCI_MAX_BUS; BusNum++) {
    for (DevNum=0; DevNum <= PCI_MAX_DEVICE; DevNum++) {
      for (FunNum=0; FunNum <= PCI_MAX_FUNC; FunNum++) {
        
        Status  = RootBridgeIo->Pci.Read (
                                      RootBridgeIo,
                                      EfiPciWidthUint32,
                                      EFI_PCI_ADDRESS (
                                        BusNum,
                                        DevNum,
                                        FunNum,
                                        0x00
                                        ),
                                      (sizeof(HPciDev[TotalDevNum].Pci) / sizeof(UINT32)),
                                      &HPciDev[TotalDevNum].Pci
                                      );
        if (EFI_ERROR (Status)) {
          Print(
            L"Pci.Read ERROR\n"
            );
        }
        
        if((HPciDev[TotalDevNum].Pci.Hdr.VendorId != 0xffff) && 
           (((HPciDev[TotalDevNum].Pci.Hdr.ClassCode[2] == 0x0D) && 
             (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[1] == 0x20) &&
             (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[0] == 0x00)) ||
            ((HPciDev[TotalDevNum].Pci.Hdr.ClassCode[2] == 0x0D) && 
             (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[1] == 0x21) &&
             (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[0] == 0x00)) ||
            ((HPciDev[TotalDevNum].Pci.Hdr.ClassCode[2] == 0x0D) && 
             (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[1] == 0x80) &&
             (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[0] == 0x00)))) {
              
          HPciDev[TotalDevNum].BusNum = BusNum;
          HPciDev[TotalDevNum].DevNum = DevNum;
          HPciDev[TotalDevNum].FunNum = FunNum;
          
          DumpData (
            HPciDev,
            TotalDevNum
            );
          
          TotalDevNum++;
        }
      }
    }
  }
  return TotalDevNum;
}

/**
  Grep VGA pci devices.

  @param[in][out]  *HPciDev             pci devices struct

*/
UINT16 
GrepVGAPCI (
  IN OUT PCIDEV                          *HPciDev
  )
{
  EFI_STATUS   Status;
  UINT16       BusNum;
  UINT8        DevNum;
  UINT8        FunNum;
  UINT16       TotalDevNum;
  

  TotalDevNum = 0;
  
  for (BusNum=0; BusNum <= PCI_MAX_BUS; BusNum++) {
    for (DevNum=0; DevNum <= PCI_MAX_DEVICE; DevNum++) {
      for (FunNum=0; FunNum <= PCI_MAX_FUNC; FunNum++) {
        
        Status  = RootBridgeIo->Pci.Read (
                                      RootBridgeIo,
                                      EfiPciWidthUint32,
                                      EFI_PCI_ADDRESS (
                                        BusNum,
                                        DevNum,
                                        FunNum,
                                        0x00
                                        ),
                                      (sizeof(HPciDev[TotalDevNum].Pci) / sizeof(UINT32)),
                                      &HPciDev[TotalDevNum].Pci
                                      );
        if (EFI_ERROR (Status)) {
          Print(
            L"Pci.Read ERROR\n"
            );
        }
        
        if((HPciDev[TotalDevNum].Pci.Hdr.VendorId != 0xffff) && 
           (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[2] == 0x03) && 
           (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[1] == 0x00) &&
           (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[0] <= 0x01)) {
              
          HPciDev[TotalDevNum].BusNum = BusNum;
          HPciDev[TotalDevNum].DevNum = DevNum;
          HPciDev[TotalDevNum].FunNum = FunNum;
          
          DumpData (
            HPciDev,
            TotalDevNum
            );
          
          TotalDevNum++;
        }
      }
    }
  }
  return TotalDevNum;
}


/**

  Grep pci devices.

  @param[in][out]  *HPciDev             pci devices struct

*/
UINT16 
GrepPCIDevice (
  IN OUT PCIDEV                          *HPciDev
  )
{
  EFI_STATUS   Status;
  UINT16       BusNum;
  UINT8        DevNum;
  UINT8        FunNum;
  UINT16       TotalDevNum;
  

  TotalDevNum = 0;
  
  for (BusNum=0; BusNum <= PCI_MAX_BUS; BusNum++) {
    for (DevNum=0; DevNum <= PCI_MAX_DEVICE; DevNum++) {
      for (FunNum=0; FunNum <= PCI_MAX_FUNC; FunNum++) {
        
        Status  = RootBridgeIo->Pci.Read (
                                      RootBridgeIo,
                                      EfiPciWidthUint32,
                                      EFI_PCI_ADDRESS (
                                        BusNum,
                                        DevNum,
                                        FunNum,
                                        0x00
                                        ),
                                      (sizeof(HPciDev[TotalDevNum].Pci) / sizeof(UINT32)),
                                      &HPciDev[TotalDevNum].Pci
                                      );
        if (EFI_ERROR (Status)) {
          Print(
            L"Pci.Read ERROR\n"
            );
        }
        
        if (HPciDev[TotalDevNum].Pci.Hdr.VendorId != 0xffff) {

          HPciDev[TotalDevNum].BusNum = BusNum;
          HPciDev[TotalDevNum].DevNum = DevNum;
          HPciDev[TotalDevNum].FunNum = FunNum;
          
          DumpData (
            HPciDev,
            TotalDevNum
            );
          
          TotalDevNum++;
        }
      }
    }
  }
  return TotalDevNum;
}


EFI_STATUS
ReadPci (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL             *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN UINT64                                   Address,
  IN UINTN                                    Count,
  IN OUT UINTN                                *Buffer
  )
{
  EFI_STATUS                             Status;
  
  RootBridgeIo->Pci.Read (
                      RootBridgeIo,
                      Width,
                      Address,
                      Count,
                      Buffer
                      );
  
  return EFI_SUCCESS;
}

EFI_STATUS
WritePci (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL             *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN UINT64                                   Address,
  IN UINTN                                    Count,
  IN OUT UINTN                                *Buffer
  ) 
{
  EFI_STATUS                             Status;
  
  RootBridgeIo->Pci.Write (
                      RootBridgeIo,
          					  Width,
          					  Address,
          					  Count,
          					  Buffer
          					  );
  return EFI_SUCCESS;
}



EFI_STATUS ProtocolUnLoad(
  IN EFI_HANDLE       ImageHandle
)
{
  EFI_STATUS Status;
  
  Status = gBS->UninstallProtocolInterface(
                  ImageHandle,
                  &gEfiHydraPciProtocolGuid,
                  HydraDrv
                  );
  if (EFI_ERROR(Status)) {
        Print(L"UninstallProtocolInterface error %d!\n", Status);
  }
  FreePool(HydraDrv);
  CLEAN_SCREEN();
  return Status;
}


EFI_STATUS
HydraMain (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                   Status;
  VOID                         *HydraInterface;
  EFI_LOADED_IMAGE_PROTOCOL    *ImageInterface = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
        				  NULL,
        				  &RootBridgeIo
        				  );
  if (EFI_ERROR (Status)) {
    Print(L"EventSignal: RootBridgeIo ERROR %r!\n", Status);
    return Status;
  }
  
  Status = gBS->LocateProtocol(
                  &gEfiHydraPciProtocolGuid,
                  NULL,
                  (VOID*)&HydraDrv
                  );
  if (Status != EFI_NOT_FOUND) {
    Print(L"EventSignal: EFI_ALREADY_STARTED %r!\n", Status);
    return EFI_ALREADY_STARTED;
  } else if (EFI_ERROR (Status)) {
    Print(L"EventSignal: %s!\n",Status);
    return Status;
  }
  
  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&ImageInterface
                  );
  if (Status == EFI_SUCCESS) {
     ImageInterface->Unload = ProtocolUnLoad;
  } 
  
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof ( EFI_HYDRA_BRIDGE_IO_PROTOCOL ),
                  &HydraDrv
                  );  
  if (EFI_ERROR (Status)) {
    Print(L"AllocatePool Fail\n");
    return Status;
  } 
  
  HydraDrv->HPci.Write = WritePci;
  HydraDrv->HPci.Read  = ReadPci;
  HydraDrv->HPci.GrepAll = GrepPCIDevice;
  HydraDrv->HPci.GrepVGA = GrepVGAPCI;
  HydraDrv->HPci.GrepWLAN = GrepWLANPCI;
  HydraDrv->HPci.Dump = DumpData;
  
  CLEAN_SCREEN();
  SET_CUR_POS(0, 0);
  Status = gBS->InstallProtocolInterface(
                  &ImageHandle,
                  &gEfiHydraPciProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  HydraDrv
                  );
  if (EFI_ERROR(Status)) {
        Print(L"InstallProtocolInterface error %d!\n", Status);
    }
  
  

  return EFI_SUCCESS;
}
