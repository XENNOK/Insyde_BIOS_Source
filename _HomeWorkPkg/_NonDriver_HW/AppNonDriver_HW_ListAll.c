/** @file
  

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

#include "AppNonDriver_HW.h"

EFI_STATUS
DeviceName (
  IN     UINT8     TargetClassCode[3],
  IN OUT CHAR16    *TargetDeviceName
)
{
  CHAR16 *CorrespondClass00[] = {
           L"on-VGA unclassified device",
           L"VGA compatible unclassified device"
           };
  CHAR16 *CorrespondClass01[] = {
           L"SCSI storage controller",
           L"IDE interface",
           L"Floppy disk controller",
           L"IPI bus controller",
           L"RAID bus controller",
           L"ATA controller",
           L"SATA controller",
           L"Serial Attached SCSI controller",
           L"Non-Volatile memory controller",
           L"Mass storage controller"
           };
  CHAR16 *CorrespondClass02[] = {
           L"Ethernet controller",
           L"Token ring network controller",
           L"FDDI network controller",
           L"ATM network controller",
           L"ISDN controller",
           L"WorldFip controller",
           L"PICMG controller",
           L"Infiniband controller",
           L"Fabric controller",
           L"Network controller"
           };
  CHAR16 *CorrespondClass03[] = {
           L"VGA compatible controller",
           L"XGA compatible controller",
           L"3D controller",
           L"Display controller"
           };     
  CHAR16 *CorrespondClass04[] = {
           L"Multimedia video controller",
           L"Multimedia audio controller",
           L"Computer telephony device",
           L"Audio device",
           L"Multimedia controller"
           };
  CHAR16 *CorrespondClass05[] = {
           L"RAM memory",
           L"FLASH memory",
           L"Memory controller"
           };
  CHAR16 *CorrespondClass06[] = {
           L"Host bridge",
           L"ISA bridge",
           L"EISA bridge",
           L"MicroChannel bridge",
           L"PCI bridge",
           L"PCMCIA bridge",
           L"NuBus bridge",
           L"CardBus bridge",
           L"RACEway bridge",
           L"Semi-transparent PCI-to-PCI bridge",
           L"InfiniBand to PCI host bridge",
           L"Bridge"
           };
  CHAR16 *CorrespondClass07[] = {
           L"Serial controller",
           L"Parallel controller",
           L"Multiport serial controller",
           L"Modem",
           L"GPIB controller",
           L"Smard Card controller",
           L"Communication controller"
           };
  CHAR16 *CorrespondClass08[] = {
           L"PIC",
           L"DMA controller",
           L"Timer",
           L"RTC",
           L"PCI Hot-plug controller",
           L"SD Host controller",
           L"IOMMU",
           L"System peripheral"
           };
  CHAR16 *CorrespondClass09[] = {
           L"Keyboard controller",
           L"Digitizer Pen",
           L"Mouse controller",
           L"Scanner controller",
           L"Gameport controller",
           L"Input device controller"
           };
  CHAR16 *CorrespondClass0A[] = {
           L"Generic Docking Station",
           L"Docking Station"
           };
  CHAR16 *CorrespondClass0B[] = {
           L"386",
           L"486",
           L"Pentium",
           L"Unknown",
           L"Unknown",
           L"Alpha",
           L"Power PC",
           L"MIPS",
           L"Co-processor"
           };
  CHAR16 *CorrespondClass0C[] = {
           L"FireWire (IEEE 1394)",
           L"ACCESS Bus",
           L"SSA",
           L"USB controller",
           L"Fibre Channel",
           L"SMBus",
           L"InfiniBand",
           L"IPMI Interface",
           L"SERCOS interface",
           L"CANBUS"
           };
  CHAR16 *CorrespondClass0D[] = {
           L"IRDA controller",
           L"Consumer IR controller",
           L"RF controller",
           L"Bluetooth",
           L"Broadband",
           L"802.1a controller",
           L"802.1b controller",
           L"Wireless controller"
           };
  CHAR16 *CorrespondClass0E[] = {
           L"I2O"
           };
  CHAR16 *CorrespondClass0F[] = {
           L"Satellite TV controller",
           L"Satellite audio communication controller",
           L"Satellite voice communication controller",
           L"Satellite data communication controller"
           };
  CHAR16 *CorrespondClass10[] = {
           L"Network and computing encryption device",
           L"	Entertainment encryption device",
           L"Encryption controller"
           };
  CHAR16 *CorrespondClass11[] = {
           L"DPIO module",
           L"Performance counters",
           L"Communication synchronizer",
           L"Signal processing management",
           L"Signal processing controller"
           };
  CHAR16 *CorrespondClass12[] = {
           L"Processing accelerators"
           };
  CHAR16 *CorrespondClass13[] = {
           L"Non-Essential Instrumentation"
           };
  CHAR16 *CorrespondClass40[] = {
           L"Coprocessor"
           };
  CHAR16 *CorrespondClassFF[] = {
           L"Unassigned class"
           };

  
  switch(TargetClassCode[2]) {
    case 0x00:
        StrCpy(TargetDeviceName, CorrespondClass00[TargetClassCode[1]]);
      break;
    case 0x01:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass01[9]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass01[TargetClassCode[1]]);
      }
      break;
    case 0x02:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass02[9]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass02[TargetClassCode[1]]);
      }
      break;
    case 0x03:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass03[3]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass03[TargetClassCode[1]]);
      }
      break;
    case 0x04:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass04[4]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass04[TargetClassCode[1]]);
      }
      break;
    case 0x05:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass05[2]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass05[TargetClassCode[1]]);
      }
      break;
    case 0x06:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass06[11]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass06[TargetClassCode[1]]);
      }
      break;
    case 0x07:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass07[6]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass07[TargetClassCode[1]]);
      }
      break;
    case 0x08:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass08[7]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass08[TargetClassCode[1]]);
      }
      break;
    case 0x09:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass09[5]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass09[TargetClassCode[1]]);
      }
      break;
    case 0x0A:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass0A[1]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass0A[TargetClassCode[1]]);
      }
      break;
    case 0x0B:
      StrCpy(TargetDeviceName, CorrespondClass0B[TargetClassCode[1]]);
      break;
    case 0x0C:
      StrCpy(TargetDeviceName, CorrespondClass0C[TargetClassCode[1]]);
      break;
    case 0x0D:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass0D[7]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass0D[TargetClassCode[1]]);
      }
      break;
    case 0x0E:
      StrCpy(TargetDeviceName, CorrespondClass0E[TargetClassCode[1]]);
      break;
    case 0x0F:
      StrCpy(TargetDeviceName, CorrespondClass0F[TargetClassCode[1]]);
      break;
    case 0x10:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass10[2]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass10[TargetClassCode[1]]);
      }
      break;
    case 0x11:
      if (TargetClassCode[1] == 0x80) {
        StrCpy(TargetDeviceName, CorrespondClass11[4]);
      } else {
        StrCpy(TargetDeviceName, CorrespondClass11[TargetClassCode[1]]);
      }
      break;
    case 0x12:
      StrCpy(TargetDeviceName, CorrespondClass12[TargetClassCode[1]]);
      break;
    case 0x13:
      StrCpy(TargetDeviceName, CorrespondClass13[TargetClassCode[1]]);
      break;
    case 0x40:
      StrCpy(TargetDeviceName, CorrespondClass40[TargetClassCode[1]]);
      break;
    default:
      StrCpy(TargetDeviceName, CorrespondClassFF[0]);
      break;
  }

return EFI_SUCCESS;
}
/**

  This code Handle key event and do corresponding func in Right Frame.
  
  @param[in]  *InputEx             Key event Protocol.
  @param[in]  TotalItems           Numbers of items
  @param[in]  **ItemLists          Item lists.
  
*/
UINT16
SelectPciItems (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT16                                  TotalItems,
  IN PCIDEV                                 *HPciDev
  )
{
  EFI_KEY_DATA       HKey;
  UINT16              ItemIndex;
  UINT16             ItemCounts;
  UINT16              PageNumber;
  UINT8              ItemPosition;
  UINT16              MaxPages;
  UINT8              MaxPageItemNumber;
  UINT8              ItemsPerPage;
  UINT8              CleanLine;
  UINT16             SelectedItem;

  CHAR16             *DevName;

  DevName = AllocateZeroPool(40);
  
  ItemsPerPage = ITEMS_PER_HUGEPAGE;
  PageNumber = 0;
  ItemPosition = 0;
  SelectedItem = NO_SELECT;

  if (TotalItems <= ItemsPerPage) {
    MaxPages = 1;
    MaxPageItemNumber =  (UINT8)TotalItems;
  } else if (TotalItems % ItemsPerPage == 0) {
    MaxPages = (TotalItems / ItemsPerPage);
    MaxPageItemNumber =  ITEMS_PER_HUGEPAGE;
  } else {
    MaxPages = (TotalItems / ItemsPerPage) + 1;
    MaxPageItemNumber = TotalItems % ItemsPerPage;
  }

  SetCursorPosColor (
    EFI_WHITE |EFI_BACKGROUND_BLACK,
    0,
    HUGE_TITLE_OFFSET
    );
  Print (L"            DevName             | =BUS= | =DEV= | =NUM= | ===VID=== | ===DID===\n"); 
         
  while (1) {
    
    EN_CURSOR (
      FALSE
      );
    
    ItemCounts = PageNumber * ITEMS_PER_HUGEPAGE;
    
    if (PageNumber == (MaxPages  -1)) {
      ItemsPerPage = MaxPageItemNumber;
    } else {
      ItemsPerPage = ITEMS_PER_HUGEPAGE;
    }
    
    for (ItemIndex = 0; ItemIndex < ItemsPerPage; ItemIndex++) {
            

      DeviceName (
        HPciDev[ItemCounts + ItemIndex].Pci.Hdr.ClassCode,
        DevName
        );
      
      SetCursorPosColor (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK,
        0,
        (ItemIndex + DATA_FRAME_OFFSET)
        );

      
      Print (
        L" %-30s |  %03d  |  %03d  |  %03d  |   %04X    |   %04X   \n",
        DevName,
        HPciDev[ItemCounts + ItemIndex].BusNum,
        HPciDev[ItemCounts + ItemIndex].DevNum,
        HPciDev[ItemCounts + ItemIndex].FunNum,
        HPciDev[ItemCounts + ItemIndex].Pci.Hdr.VendorId,
        HPciDev[ItemCounts + ItemIndex].Pci.Hdr.DeviceId
        );
      
      if (PageNumber == MaxPages - 1) {
        for (CleanLine = (ITEMS_PER_HUGEPAGE-1); CleanLine >= ItemsPerPage; CleanLine--) {

            CleanFrame (
              EFI_BACKGROUND_BLACK,
              (CleanLine + DATA_FRAME_OFFSET),
              (CleanLine + DATA_FRAME_OFFSET) + 1,
              0,
              HUGE_FRAME_HORI_MAX
              );
        }
      }
    }

    
    DeviceName (
      HPciDev[ItemCounts + ItemPosition].Pci.Hdr.ClassCode,
      DevName
      );
    
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_LIGHTGRAY,
      0,
      (ItemPosition + DATA_FRAME_OFFSET)
      );

    Print (
      L" %-30s |  %03d  |  %03d  |  %03d  |   %04X    |   %04X   \n",
      DevName,
      HPciDev[ItemCounts + ItemPosition].BusNum,
      HPciDev[ItemCounts + ItemPosition].DevNum,
      HPciDev[ItemCounts + ItemPosition].FunNum,
      HPciDev[ItemCounts + ItemPosition].Pci.Hdr.VendorId,
      HPciDev[ItemCounts + ItemPosition].Pci.Hdr.DeviceId
      );
    SetCursorPosColor (
      EFI_WHITE | EFI_BACKGROUND_BLACK,
      0,
      (ItemPosition + DATA_FRAME_OFFSET)
      );
    
    HKey = keyRead (
             InputEx
             );
 
    if ((HKey.Key.ScanCode != 0 ) &&
        !((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
          (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED))) {
         
		  switch (HKey.Key.ScanCode) {
        
			case SCAN_UP:
        ItemPosition--;
        
        if (ItemPosition == 0xFF) {
          
          PageNumber --;
          
          if (PageNumber == 0xFFFF) {
            PageNumber = MaxPages-1;
            ItemPosition = MaxPageItemNumber-1;
          }else{
            ItemPosition = ITEMS_PER_HUGEPAGE-1;
          }
        }
			  break;
			case SCAN_DOWN:
        ItemPosition++;
        
        if (ItemPosition >= ItemsPerPage) {
          ItemPosition=0;
          PageNumber++;
          
          if (PageNumber >= MaxPages) {
            PageNumber = 0;
          }
        }				  
			  break;
		  }
    }  else if (HKey.Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      //
      // return ItemCounts+ItemPosition
      //
		  SelectedItem = ItemCounts + ItemPosition;
      break;
    } else if ((HKey.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
  	  ((HKey.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) ||
  	   (HKey.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED)) &&
  	  (HKey.Key.ScanCode == SCAN_F1)) {
  	   SelectedItem = NO_SELECT;
       break;
	  }
  }
  
    CleanFrame (
      EFI_BACKGROUND_BLACK,
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      0,
      HUGE_FRAME_HORI_MAX
      );
  FreePool(DevName);
  return SelectedItem;
}



/*
  List All Pci Handler

  @param[in]  *HydraBridgeIo       EFI_HYDRA_BRIDGE_IO_PROTOCOL.
  @param[in]  *InputEx             Key event Protocol.

  @return EFI_STATUS
*/
EFI_STATUS
ListAllPciHandler (
  IN EFI_HYDRA_BRIDGE_IO_PROTOCOL           *HydraBridgeIo,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx,
  IN UINT8                                  SelectedFunc
  )
{
  EFI_KEY_DATA      HKey;
  PCIDEV            HPciDev[100];
  PCIDEV            *PciDevPtr;
  UINT16            TotalDeviceNumber;
  UINT16            ItemIndex;
  
  PciDevPtr = HPciDev;

  switch (SelectedFunc) {
   case AllPciDev:
     TotalDeviceNumber = HydraBridgeIo->HPci.GrepAll (
                                               PciDevPtr
                                               );
     break;
   case VgaPciDev:
     TotalDeviceNumber = HydraBridgeIo->HPci.GrepVGA (
                                               PciDevPtr
                                               );
     break;
   case WlanPciDev:
     TotalDeviceNumber = HydraBridgeIo->HPci.GrepWLAN (
                                           PciDevPtr
                                           );
   break;
   default:
    TotalDeviceNumber = 0;
    break;
  }
  

    if (TotalDeviceNumber == 0) {
    SetCursorPosColor (
      EFI_WHITE |EFI_BACKGROUND_RED,
      0,
      3
      );
    Print (L"NO Devices FOUND !!\n");
    Print (L"Press any key to contionue...\n");
    HKey = keyRead (InputEx);
    return EFI_NOT_FOUND;
  }
    
  while (TRUE) {
    CleanFrame (
      EFI_BACKGROUND_BLACK,
      HUGE_TITLE_OFFSET,
      HUGE_TAIL_OFFSET,
      0,
      HUGE_FRAME_HORI_MAX
      );
    
  
  
    ItemIndex = SelectPciItems (
                InputEx,
                TotalDeviceNumber,
                PciDevPtr
                );
  
    if (ItemIndex == NO_SELECT) {
      break;
    }

    DataFrameHandler (
      HydraBridgeIo,
      InputEx,
      PciDevPtr,
      ItemIndex
      );
    
  }
return EFI_SUCCESS;
}
