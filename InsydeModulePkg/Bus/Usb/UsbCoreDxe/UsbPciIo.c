/** @file
  PCI I/O Abstraction Driver for USB

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

#include "UsbCoreDxe.h"
#include "UsbPciIo.h"

/**

  Performs a PCI Configuration Read Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoPciRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  )
{
  if (mPrivate->SmmReady) {
    //
    // Use direct PCI/IO access for SMM capable system
    //
    IoWrite32 (PCI_CONFIG_INDEX_PORT, (UINT32)(PciAddress + Offset + 0x80000000));
    //
    // Read data from PCI data port
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)Buffer = IoRead8 (PCI_CONFIG_DATA_PORT);
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)Buffer = IoRead16 (PCI_CONFIG_DATA_PORT);
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)Buffer = IoRead32 (PCI_CONFIG_DATA_PORT);
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI root bridge protocol for native system
    //
    mPrivate->RootBridgeIo->Pci.Read (
                                  mPrivate->RootBridgeIo,
                                  (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                  EFI_PCI_ADDRESS (((PciAddress & 0xff0000) >> 16), ((PciAddress & 0xf800) >> 11), ((PciAddress & 0x0700) >> 8), Offset),
                                  1,
                                  Buffer
                                  );
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI Configuration Write Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoPciWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  )
{
  if (mPrivate->SmmReady) {
    //
    // Use direct PCI/IO access for SMM capable system
    //
    IoWrite32 (PCI_CONFIG_INDEX_PORT, (UINT32)(PciAddress + Offset + 0x80000000));
    //
    // Write data to PCI data port
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        IoWrite8 (PCI_CONFIG_DATA_PORT, *(UINT8*)Buffer);
        break;
      case EfiPciIoWidthUint16:
        IoWrite16 (PCI_CONFIG_DATA_PORT, *(UINT16*)Buffer);
        break;
      case EfiPciIoWidthUint32:
        IoWrite32 (PCI_CONFIG_DATA_PORT, *(UINT32*)Buffer);
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI root bridge protocol for native system
    //
    mPrivate->RootBridgeIo->Pci.Write (
                                  mPrivate->RootBridgeIo,
                                  (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                  EFI_PCI_ADDRESS (((PciAddress & 0xff0000) >> 16), ((PciAddress & 0xf800) >> 11), ((PciAddress & 0x0700) >> 8), Offset),
                                  1,
                                  Buffer
                                  );
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI Memory Read Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoMemRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT8 *Bar, Buf[8];

  if (mPrivate->SmmReady) {
    //
    // Use direct PCI/IO access for SMM capable system
    //
    IoWrite32 (PCI_CONFIG_INDEX_PORT, (UINT32)(PciAddress + PCI_CONFIG_BAR_BASE + (BarIndex * 4) + 0x80000000));
    //
    // Read BAR from PCI data port
    //
    Bar = (UINT8*)((IoRead32 (PCI_CONFIG_DATA_PORT) & ~0x0F) + (UINTN)Offset);
    *(UINT32*)Buf = *(UINT32*)((UINTN)Bar & ~0x03);
    if (((UINTN)Bar & 0x03) && (Width > EfiPciIoWidthUint8)) {
      *(UINT32*)(Buf + 4) = *(UINT32*)((UINTN)(Bar + 0x04) & ~0x03);
    }
    Offset &= 0x03;
    //
    // Read data from PCI data port
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)Buffer = Buf[Offset];
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)Buffer = *(UINT16*)&Buf[Offset];
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)Buffer = *(UINT32*)&Buf[Offset];
        break;
      default:
        return EFI_UNSUPPORTED;
    }
  } else {
    //
    // Use UEFI root bridge protocol for native system
    //
    Bar = NULL;
    mPrivate->RootBridgeIo->Pci.Read (
                                  mPrivate->RootBridgeIo,
                                  EfiPciWidthUint32,
                                  EFI_PCI_ADDRESS (((PciAddress & 0xff0000) >> 16), ((PciAddress & 0xf800) >> 11), ((PciAddress & 0x0700) >> 8), PCI_CONFIG_BAR_BASE + (BarIndex * 4)),
                                  1,
                                  &Bar
                                  );
    Bar = (UINT8*)((UINTN)Bar & ~0x0F) + Offset;
    mPrivate->RootBridgeIo->Mem.Read (
                                  mPrivate->RootBridgeIo,
                                  (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                  (UINT64)(UINTN)Bar,
                                  1,
                                  Buffer
                                  );
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI Memory Write Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoMemWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT8 *Bar, Buf[8];

  if (mPrivate->SmmReady) {
    //
    // Use direct PCI/IO access for SMM capable system
    //
    IoWrite32 (PCI_CONFIG_INDEX_PORT, (UINT32)(PciAddress + PCI_CONFIG_BAR_BASE + (BarIndex * 4) + 0x80000000));
    //
    // Read BAR from PCI data port
    //
    Bar = (UINT8*)((IoRead32 (PCI_CONFIG_DATA_PORT) & ~0x0F) + (UINTN)Offset);
    if ((UINTN)Bar & 0x03) {
      *(UINT32*)Buf = *(UINT32*)((UINTN)Bar & ~0x03);
      if (Width > EfiPciIoWidthUint8) {
        *(UINT32*)(Buf + 4) = *(UINT32*)((UINTN)(Bar + 0x04) & ~0x03);
      }
    }
    Offset &= 0x03;
    //
    // Merge data to temp buffer
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        Buf[Offset] = *(UINT8*)Buffer;
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)&Buf[Offset] = *(UINT16*)Buffer;
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)&Buf[Offset] = *(UINT32*)Buffer;
        break;
      default:
        return EFI_UNSUPPORTED;
    }
    *(UINT32*)((UINTN)Bar & ~0x03) = *(UINT32*)&Buf[0];
    if (((UINTN)Bar & 0x03) && (Width > EfiPciIoWidthUint8)) {
      *(UINT32*)((UINTN)(Bar + 0x04) & ~0x03) = *(UINT32*)&Buf[4];
    }
  } else {
    //
    // Use UEFI root bridge protocol for native system
    //
    Bar = NULL;
    mPrivate->RootBridgeIo->Pci.Read (
                                  mPrivate->RootBridgeIo,
                                  EfiPciWidthUint32,
                                  EFI_PCI_ADDRESS (((PciAddress & 0xff0000) >> 16), ((PciAddress & 0xf800) >> 11), ((PciAddress & 0x0700) >> 8), PCI_CONFIG_BAR_BASE + (BarIndex * 4)),
                                  1,
                                  &Bar
                                  );
    Bar = (UINT8*)((UINTN)Bar & ~0x0F) + Offset;
    mPrivate->RootBridgeIo->Mem.Write (
                                  mPrivate->RootBridgeIo,
                                  (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                  (UINT64)(UINTN)Bar,
                                  1,
                                  Buffer
                                  );
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI I/O Read Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun, -1 indicates pure IO access
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoIoRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT16 Port;

  if (mPrivate->SmmReady) {
    //
    // Use direct PCI/IO access for SMM capable system
    //
    IoWrite32 (PCI_CONFIG_INDEX_PORT, (UINT32)(PciAddress + PCI_CONFIG_BAR_BASE + (BarIndex * 4) + 0x80000000));
    //
    // Read BAR from PCI data port
    //
    Port = (UINT16)((IoRead32 (PCI_CONFIG_DATA_PORT) & ~0x03) + Offset);
    //
    // Read data from IO BAR
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)Buffer = IoRead8 (Port);
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)Buffer = IoRead16 (Port);
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)Buffer = IoRead32 (Port);
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI root bridge protocol for native system
    //
    mPrivate->RootBridgeIo->Pci.Read (
                                  mPrivate->RootBridgeIo,
                                  EfiPciWidthUint16,
                                  EFI_PCI_ADDRESS (((PciAddress & 0xff0000) >> 16), ((PciAddress & 0xf800) >> 11), ((PciAddress & 0x0700) >> 8), PCI_CONFIG_BAR_BASE + (BarIndex * 4)),
                                  1,
                                  &Port
                                  );
    Port = (UINT16)((Port & ~0x03) + Offset);
    mPrivate->RootBridgeIo->Io.Read (
                                  mPrivate->RootBridgeIo,
                                  (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                  (UINT64)(UINTN)Port,
                                  1,
                                  Buffer
                                  );
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI I/O Write Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoIoWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT16 Port;

  if (mPrivate->SmmReady) {
    //
    // Use direct PCI/IO access for SMM capable system
    //
    IoWrite32 (PCI_CONFIG_INDEX_PORT, (UINT32)(PciAddress + PCI_CONFIG_BAR_BASE + (BarIndex * 4) + 0x80000000));
    //
    // Read BAR from PCI data port
    //
    Port = (UINT16)((IoRead32 (PCI_CONFIG_DATA_PORT) & ~0x03) + Offset);
    //
    // Read data from IO BAR
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        IoWrite8 (Port, *(UINT8*)Buffer);
        break;
      case EfiPciIoWidthUint16:
        IoWrite16 (Port, *(UINT16*)Buffer);
        break;
      case EfiPciIoWidthUint32:
        IoWrite32 (Port, *(UINT32*)Buffer);
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI root bridge protocol for native system
    //
    mPrivate->RootBridgeIo->Pci.Read (
                                  mPrivate->RootBridgeIo,
                                  EfiPciWidthUint16,
                                  EFI_PCI_ADDRESS (((PciAddress & 0xff0000) >> 16), ((PciAddress & 0xf800) >> 11), ((PciAddress & 0x0700) >> 8), PCI_CONFIG_BAR_BASE + (BarIndex * 4)),
                                  1,
                                  &Port
                                  );
    Port = (UINT16)((Port & ~0x03) + Offset);
    mPrivate->RootBridgeIo->Io.Write (
                                  mPrivate->RootBridgeIo,
                                  (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                                  (UINT64)(UINTN)Port,
                                  1,
                                  Buffer
                                  );
  }
  return EFI_SUCCESS;
}