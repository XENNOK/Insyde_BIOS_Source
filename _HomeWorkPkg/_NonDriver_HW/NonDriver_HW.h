/** @file
  

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/


#ifndef _NON_DRIVER_H_
#define _NON_DRIVER_H_
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>


#define SET_CUR_POS(x,y) gST->ConOut->SetCursorPosition (gST->ConOut, (x), (y))
#define SET_COLOR(x) gST->ConOut->SetAttribute (gST->ConOut, (x))
#define CLEAN_SCREEN(VOID) gST->ConOut->ClearScreen(gST->ConOut)
#define EN_CURSOR(x) gST->ConOut->EnableCursor(gST->ConOut, (x))

#define DATA_ARRAY_COLS_MAX 15
#define DATA_ARRAY_ROW_MAX  15

extern EFI_GUID gEfiHydraPciProtocolGuid;

#define HYDRA_PROTOCOL_GUID \
  { 0xafe31749, 0x762c, 0x411c, 0x7c, 0x09, 0x8d, 0x6e, 0x2d, 0x07, 0x88, 0x49 }

#define EFI_HYDRA_ADDRESS(bus, dev, func, reg) \
  (UINT64) ( \
  (((UINTN) bus) << 24) | \
  (((UINTN) dev) << 16) | \
  (((UINTN) func) << 8) | \
  (((UINTN) (reg)) < 256 ? ((UINTN) (reg)) : (UINT64) (LShiftU64 ((UINT64) (reg), 32))))

typedef struct __PCIDEV {
  UINT16     BusNum;
  UINT8      DevNum;
  UINT8      FunNum;
  PCI_TYPE00 Pci;
  UINT8      PcidevData[16][16]; 
} PCIDEV;

typedef struct _EFI_HYDRA_BRIDGE_IO_PROTOCOL EFI_HYDRA_BRIDGE_IO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_HYDRA_BRIDGE_IO_PROTOCOL_IO_MEM)(
  IN     EFI_HYDRA_BRIDGE_IO_PROTOCOL             *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

typedef
  UINT16 
(EFIAPI *EFI_HYDRA_BRIDGE_IO_PROTOCOL_GREP_PCI)(
  IN OUT PCIDEV                          *HPciDev
  );

typedef
 VOID
(EFIAPI *EFI_HYDRA_BRIDGE_IO_PROTOCOL_DUMP_PCI)(
  IN OUT PCIDEV                          *HPciDev,
  IN UINTN                               DataIndex
  );

typedef struct {
  //
  // Read PCI controller registers in the PCI root bridge memory space.
  //
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_IO_MEM  Read;
  //
  // Write PCI controller registers in the PCI root bridge memory space.
  //
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_IO_MEM  Write;
  //
  // Grep PCI Devices and Data.
  //
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_GREP_PCI GrepAll;
  //
  // Grep VGA PCI Devices and Data.
  //
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_GREP_PCI GrepVGA;
  //
  // Grep WLAN PCI Devices and Data.
  //
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_GREP_PCI GrepWLAN;
  //
  // Dump PCI Data
  //
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_DUMP_PCI Dump;
} EFI_HYDRA_BRIDGE_IO_PROTOCOL_ACCESS;


struct _EFI_HYDRA_BRIDGE_IO_PROTOCOL {
  EFI_HYDRA_BRIDGE_IO_PROTOCOL_ACCESS  HPci;
};

EFI_STATUS
ReadPci (
  EFI_HYDRA_BRIDGE_IO_PROTOCOL            *This,
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  UINT64                                   Address,
  UINTN                                    Count,
  UINTN                                   *Buffer
  );

EFI_STATUS
WritePci (
  EFI_HYDRA_BRIDGE_IO_PROTOCOL            *This,
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,  
  UINT64                                   Address,
  UINTN                                    Count,
  UINTN                                   *Buffer
  );

EFI_STATUS 
ProtocolUnLoad(
  IN EFI_HANDLE       ImageHandle
  );

#endif
