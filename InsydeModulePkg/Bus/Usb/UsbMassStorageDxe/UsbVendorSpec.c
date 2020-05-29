/** @file
  The vendor specific devices list

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbMassImpl.h"
#include "UsbVendorSpec.h"

STATIC USB_VENDOR_SPEC mUsbStor[] = {
  { 0x03EE, 0x0000, USB_MASS_STORE_CBI0 },      // Mitsumi USB CD-R/RW Drive
  { 0x03EE, 0x6901, USB_MASS_STORE_CBI0 },      // Mitsumi USB Floppy
  { 0x03F0, 0x0107, USB_MASS_STORE_BOT  },      // HP USB CD-Writer Plus
  { 0x03F0, 0x2001, USB_MASS_STORE_CBI0 },      // HP USB Floppy
  { 0x03F0, 0x4002, USB_MASS_STORE_BOT  },      // Hewlett-Packard Digital Camera
  { 0x03F0, 0x6102, USB_MASS_STORE_BOT  },      // Hewlett-Packard Digital Camera
  { 0x0409, 0x002C, USB_MASS_STORE_BOT  },      // NEC Clik!-USB Drive
  { 0x0409, 0x0040, USB_MASS_STORE_CBI0 },      // NEC USB Floppy
  { 0x0424, 0x0FDC, USB_MASS_STORE_CBI0 },      // SMSC USB Floppy
  { 0x04B3, 0x4427, USB_MASS_STORE_BOT  },      // IBM USB CD-ROM Drive
  { 0x04BB, 0x0301, USB_MASS_STORE_CBI0 },      // USB Storage Device
  { 0x04CB, 0x0100, USB_MASS_STORE_CBI0 },      // FujiFilm FinePix Digital Camera
  { 0x04CE, 0x0002, USB_MASS_STORE_BOT  },      // ScanLogic USB Storage Device
  { 0x04DA, 0x0B01, USB_MASS_STORE_CBI0 },      // Panasonic USB CD-R/RW Drive
  { 0x04DA, 0x1B00, USB_MASS_STORE_CBI1 },      // USB Reader Writer for SD Memory Card
  { 0x04DA, 0x0B03, USB_MASS_STORE_BOT  },      // Panasonic USB SuperDisk 240MB
  { 0x04E6, 0x0001, USB_MASS_STORE_CBI1 },      // USB ATAPI Storage Device
  { 0x04E6, 0x0002, USB_MASS_STORE_BOT  },      // USB SCSI Storage Device
  { 0x04E6, 0x000A, USB_MASS_STORE_BOT  },      // USB CompactFlash Reader
  { 0x04E6, 0x0101, USB_MASS_STORE_BOT  },      // USB ATAPI Storage Device
  { 0x054C, 0x0022, USB_MASS_STORE_BOT  },      // Sony USB HiFD Drive
  { 0x054C, 0x0023, USB_MASS_STORE_CBI1 },      // Sony USB CD-R/RW Drive
  { 0x054C, 0x0024, USB_MASS_STORE_CBI1 },      // Sony Mavica Digital Still Camera
  { 0x054C, 0x0025, USB_MASS_STORE_CBI1 },      // Sony USB Memory Stick Walkman
  { 0x054C, 0x002C, USB_MASS_STORE_CBI0 },      // Sony USB Floppy
  { 0x054C, 0x0037, USB_MASS_STORE_CBI0 },      // Sony MG Memory Stick Reader/Writer
  { 0x054C, 0x0046, USB_MASS_STORE_CBI0 },      // Sony USB Network Walkman
  { 0x054C, 0x004A, USB_MASS_STORE_CBI0 },      // Sony USB Memory Stick Hi-Fi System
  { 0x054C, 0x0058, USB_MASS_STORE_CBI0 },      // Sony MG Memory Stick CLIE
  { 0x057B, 0x0000, USB_MASS_STORE_CBI0 },      // Y-E Data USB Floppy
  { 0x057B, 0x0001, USB_MASS_STORE_CBI0 },      // Y-E Data USB Floppy
  { 0x057B, 0x0010, USB_MASS_STORE_BOT  },      // Y-E Data USB Memory Stick Reader
  { 0x059B, 0x0001, USB_MASS_STORE_BOT  },      // Iomega USB Zip 100"                                         
  { 0x059B, 0x0030, USB_MASS_STORE_BOT  },      // Iomega USB Zip 250"                                         
  { 0x059B, 0x0031, USB_MASS_STORE_BOT  },      // Iomega USB Zip 100"                                         
  { 0x059B, 0x0032, USB_MASS_STORE_BOT  },      // Iomega USB Bus Powered Zip 250"                             
  { 0x059B, 0x0060, USB_MASS_STORE_BOT  },      // Iomega USB Click!Dock
  { 0x059F, 0xA601, USB_MASS_STORE_CBI1 },      // LaCie USB Hard Drive
  { 0x059F, 0xA602, USB_MASS_STORE_CBI1 },      // LaCie USB CD-R/RW Drive
  { 0x0644, 0x0000, USB_MASS_STORE_CBI0 },      // TEAC USB Floppy
  { 0x0644, 0x1000, USB_MASS_STORE_BOT  },      // TEAC USB CD-ROM Drive
  { 0x0693, 0x0002, USB_MASS_STORE_BOT  },      // USB SmartMedia Reader/Writer
  { 0x0693, 0x0003, USB_MASS_STORE_BOT  },      // USB CompactFlash Reader/Writer
  { 0x0693, 0x0005, USB_MASS_STORE_BOT  },      // USB Dual Slot Reader/Writer
  { 0x0693, 0x0006, USB_MASS_STORE_BOT  },      // USB SM PCCard R/W and SPD
  { 0x0693, 0x0007, USB_MASS_STORE_BOT  },      // FlashGate ME (Authenticated)
  { 0x0693, 0x000A, USB_MASS_STORE_BOT  },      // USB SDCard/MMC Reader/Writer
  { 0x0718, 0x0002, USB_MASS_STORE_BOT  },      // Imation SuperDisk USB 120MB
  { 0x0718, 0x0003, USB_MASS_STORE_BOT  },      // Imation SuperDisk USB 120MB (Authenticated)
  { 0x0781, 0x0001, USB_MASS_STORE_CBI1 },      // SanDisk USB ImageMate
  { 0x0781, 0x0002, USB_MASS_STORE_BOT  },      // SanDisk USB ImageMate (Authenticated)
  { 0x0781, 0x0100, USB_MASS_STORE_CBI1 },      // SanDisk USB ImageMate Multimedia Card Reader
  { 0x0892, 0x0101, USB_MASS_STORE_BOT  },      // DioGraphy USB Smartdio Reader/Writer
  { 0x08EC, 0x0010, USB_MASS_STORE_BOT  },      // DiskOnKey USB personal storage device
  { 0x0BF6, 0x1234, USB_MASS_STORE_BOT  },      // Addonics USB Storage Device
  { 0x0BF6, 0x0103, USB_MASS_STORE_BOT  },      // Addonics USB Storage Device
  { 0x55AA, 0x0102, USB_MASS_STORE_BOT  },      // USB SuperDisk
  { 0x55AA, 0x0103, USB_MASS_STORE_BOT  },      // OnSpec USB IDE Hard Drive
  { 0x55AA, 0x1234, USB_MASS_STORE_BOT  },      // OnSpec USB CD-R/RW Drive
  { 0x55AA, 0xB004, USB_MASS_STORE_BOT  },      // OnSpec USB MMC/SD Reader/Writer
  { 0x55AA, 0xB200, USB_MASS_STORE_BOT  },      // OnSpec USB Compact Flash Reader (Authenticated)
  { 0x55AA, 0xB204, USB_MASS_STORE_BOT  },      // OnSpec USB MMC/ SD Reader (Authenticated)
  { 0x55AA, 0xB207, USB_MASS_STORE_BOT  },      // OnSpec USB Memory Stick Reader (Authenticated)
};                                            
                                              
EFI_STATUS
UsbVendorSpecificDevice (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  UINT16                    VendorID,
  IN  UINT16                    ProductID,
  OUT UINT8                     *Protocol
  )
{
  UINTN                     Index;

  for (Index = 0; Index < sizeof(mUsbStor) / sizeof (USB_VENDOR_SPEC); Index ++) {
    if (VendorID == mUsbStor[Index].VendorID && ProductID == mUsbStor[Index].ProductID) {
      *Protocol = mUsbStor[Index].Protocol;
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}