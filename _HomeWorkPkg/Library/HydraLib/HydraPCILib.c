/** @file
  
grep pci device and dump pci devices data

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

#include "HydraLibInternal.h"
#include <Library/HydraLib.h>

/**

  Dump pci devices data.
  @param[in]       *PciRootBridgeIo     PciRootBridegIo Protocol
  @param[in][out]  *HPciDev             pci devices struct
  @param[in]       DataIndex            Index of pci devices struct

*/
VOID
DumpPCIData (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN OUT PCIDEV                          *HPciDev,
  IN UINTN                               DataIndex
  )
{
  UINT8 OFFSET;
  UINT8 ROWS;
  UINT8 COLS;
  EFI_STATUS Status;
  
  OFFSET = 0;
  
  Status  = PciRootBridgeIo->Pci.Read (
                               PciRootBridgeIo,
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
      
      Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
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
        SetCursorPosColor (
          EFI_BLACK | EFI_BACKGROUND_RED,
          41,
          0
          );
        Print (
          L"DumpError"
          );
      }
      
      OFFSET = OFFSET + 1;
      
    }
  }
}

/**

  Grep pci devices.
  @param[in]       *PciRootBridgeIo     PciRootBridegIo Protocol
  @param[in][out]  *HPciDev             pci devices struct

*/
UINT16 
GrepPCIDevices (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN OUT PCIDEV                          *HPciDev
  )
{
  EFI_STATUS   Status;
  UINT16       BusNum;
  UINT8        DevNum;
  UINT8        FunNum;
  UINT16       TotalDevNum;
  
  TotalDevNum = 0;
  
  Print(L"GrepPCIDevices...\n");
  
  for (BusNum=0; BusNum <= PCI_MAX_BUS; BusNum++) {
    for (DevNum=0; DevNum <= PCI_MAX_DEVICE; DevNum++) {
      for (FunNum=0; FunNum <= PCI_MAX_FUNC; FunNum++) {
        
        Status  = PciRootBridgeIo->Pci.Read (
                                     PciRootBridgeIo,
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
           !(((HPciDev[TotalDevNum].Pci.Hdr.ClassCode[2] == 0x06) && 
              (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[1]<=0x04)) ||
             ((HPciDev[TotalDevNum].Pci.Hdr.ClassCode[2] == 0x08) &&
              (HPciDev[TotalDevNum].Pci.Hdr.ClassCode[1]<=0x03)))) {
              
          HPciDev[TotalDevNum].BusNum = BusNum;
          HPciDev[TotalDevNum].DevNum = DevNum;
          HPciDev[TotalDevNum].FunNum = FunNum;
          
          DumpPCIData (
            PciRootBridgeIo,
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
