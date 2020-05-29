/** @file
  PCI I/O Abstraction Driver Header for USB

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

#ifndef _USB_PCI_IO_H
#define _USB_PCI_IO_H

#define PCI_CONFIG_INDEX_PORT           0xcf8
#define PCI_CONFIG_DATA_PORT            0xcfc
#define PCI_CONFIG_BAR_BASE             0x10

//
// PCI Register Read
//
EFI_STATUS
EFIAPI
PciIoPciRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI Register Write
//
EFI_STATUS
EFIAPI
PciIoPciWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI Memory Bar Based Read
//
EFI_STATUS
EFIAPI
PciIoMemRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI Memory Bar Based Write
//
EFI_STATUS
EFIAPI
PciIoMemWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI I/O Bar Based Read
//
EFI_STATUS
EFIAPI
PciIoIoRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );
//
// PCI I/O Bar Based Write
//
EFI_STATUS
EFIAPI
PciIoIoWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  );

#endif
