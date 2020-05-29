/** @file

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

/** 
  PCI command register operations supporting functions implementation for PCI Bus module.

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciBus.h"

/**
  Operate the PCI register via PciIo function interface.

  @param PciIoDevice    Pointer to instance of PCI_IO_DEVICE.
  @param Command        Operator command.
  @param Offset         The address within the PCI configuration space for the PCI controller.
  @param Operation      Type of Operation.
  @param PtrCommand     Return buffer holding old PCI command, if operation is not EFI_SET_REGISTER.

  @return Status of PciIo operation.

**/
EFI_STATUS
PciOperateRegister (
  IN  PCI_IO_DEVICE *PciIoDevice,
  IN  UINT16        Command,
  IN  UINT8         Offset,
  IN  UINT8         Operation,
  OUT UINT16        *PtrCommand,
  IN  BOOLEAN       SaveScriptTable
  )
{
  UINT16              OldCommand;
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINTN               Segment;
  UINTN               Bus;
  UINTN               Device;
  UINTN               Function;
  UINT64              PciAddress;

  OldCommand  = 0;
  PciIo       = &PciIoDevice->PciIo;

  if (Operation != EFI_SET_REGISTER) {
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint16,
                          Offset,
                          1,
                          &OldCommand
                          );

    if (Operation == EFI_GET_REGISTER) {
      *PtrCommand = OldCommand;
      return Status;
    }
  }

  if (Operation == EFI_ENABLE_REGISTER) {
    OldCommand = (UINT16) (OldCommand | Command);
  } else if (Operation == EFI_DISABLE_REGISTER) {
    OldCommand = (UINT16) (OldCommand & ~(Command));
  } else {
    OldCommand = Command;
  }

  if (SaveScriptTable) {
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    PciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Function, Offset);
    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint16,
      PciAddress,
      1,
      &OldCommand
      );
  }

  return PciIo->Pci.Write (
                      PciIo,
                      EfiPciIoWidthUint16,
                      Offset,
                      1,
                      &OldCommand
                      );
}

/**
  Check the cpability supporting by given device.

  @param PciIoDevice   Pointer to instance of PCI_IO_DEVICE.

  @retval TRUE         Cpability supportted.
  @retval FALSE        Cpability not supportted.

**/
BOOLEAN
PciCapabilitySupport (
  IN PCI_IO_DEVICE  *PciIoDevice
  )
{
  if ((PciIoDevice->Pci.Hdr.Status & EFI_PCI_STATUS_CAPABILITY) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Locate capability register block per capability ID.

  @param PciIoDevice       A pointer to the PCI_IO_DEVICE.
  @param CapId             The capability ID.
  @param Offset            A pointer to the offset returned.
  @param NextRegBlock      A pointer to the next block returned.

  @retval EFI_SUCCESS      Successfuly located capability register block.
  @retval EFI_UNSUPPORTED  Pci device does not support capability.
  @retval EFI_NOT_FOUND    Pci device support but can not find register block.

**/
EFI_STATUS
LocateCapabilityRegBlock (
  IN PCI_IO_DEVICE  *PciIoDevice,
  IN UINT8          CapId,
  IN OUT UINT8      *Offset,
  OUT UINT8         *NextRegBlock OPTIONAL
  )
{
  UINT8   CapabilityPtr;
  UINT16  CapabilityEntry;
  UINT8   CapabilityID;
  UINT32  Temp;
  UINTN   Count;

  //
  // To check the cpability of this device supports
  //
  if (!PciCapabilitySupport (PciIoDevice)) {
    return EFI_UNSUPPORTED;
  }

  if (*Offset != 0) {
    CapabilityPtr = *Offset;
  } else {

    CapabilityPtr = 0;
    if (IS_CARDBUS_BRIDGE (&PciIoDevice->Pci)) {

      PciIoDevice->PciIo.Pci.Read (
                               &PciIoDevice->PciIo,
                               EfiPciIoWidthUint8,
                               EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR,
                               1,
                               &CapabilityPtr
                               );
    } else {

      PciIoDevice->PciIo.Pci.Read (
                               &PciIoDevice->PciIo,
                               EfiPciIoWidthUint32,
                               PCI_CAPBILITY_POINTER_OFFSET,
                               1,
                               &Temp
                               );
      //
      // Do not get byte read directly, because some PCI card will return 0xFF
      // when perform PCI-Express byte read, while return correct 0x00
      // when perform PCI-Express dword read, or PCI dword read.
      //
      CapabilityPtr = (UINT8)Temp;
    }
  }

  Count = 0;
  while (CapabilityPtr > 0x3F && Count++ < ((0x100 - 0x40) / sizeof(UINT32))) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFC;
    PciIoDevice->PciIo.Pci.Read (
                             &PciIoDevice->PciIo,
                             EfiPciIoWidthUint16,
                             CapabilityPtr,
                             1,
                             &CapabilityEntry
                             );

    CapabilityID = (UINT8) CapabilityEntry;

    if (CapabilityID == CapId) {
      *Offset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (UINT8) (CapabilityEntry >> 8);
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (UINT8) (CapabilityEntry >> 8);
  }

  return EFI_NOT_FOUND;
}

/**
  Locate PciExpress capability register block per capability ID.

  @param PciIoDevice       A pointer to the PCI_IO_DEVICE.
  @param CapId             The capability ID.
  @param Offset            A pointer to the offset returned.
  @param NextRegBlock      A pointer to the next block returned.

  @retval EFI_SUCCESS      Successfuly located capability register block.
  @retval EFI_UNSUPPORTED  Pci device does not support capability.
  @retval EFI_NOT_FOUND    Pci device support but can not find register block.

**/
EFI_STATUS
LocatePciExpressCapabilityRegBlock (
  IN     PCI_IO_DEVICE *PciIoDevice,
  IN     UINT16        CapId,
  IN OUT UINT32        *Offset,
     OUT UINT32        *NextRegBlock OPTIONAL
  )
{
  UINT32  CapabilityPtr;
  UINT32  CapabilityEntry;
  UINT16  CapabilityID;
  UINTN   Count;

  //
  // To check the capability of this device supports
  //
  if (!PciIoDevice->IsPciExp) {
    return EFI_UNSUPPORTED;
  }

  if (*Offset != 0) {
    CapabilityPtr = *Offset;
  } else {
    CapabilityPtr = EFI_PCIE_CAPABILITY_BASE_OFFSET;
  }

  Count = 0;
  while (CapabilityPtr != 0 && Count++ < ((0x1000 - 0x100) / sizeof(UINT32))) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFFC;
    PciIoDevice->PciIo.Pci.Read (
                             &PciIoDevice->PciIo,
                             EfiPciIoWidthUint32,
                             CapabilityPtr,
                             1,
                             &CapabilityEntry
                             );

    CapabilityID = (UINT16) CapabilityEntry;

    if (CapabilityID == CapId) {
      *Offset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (CapabilityEntry >> 20) & 0xFFF;
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (CapabilityEntry >> 20) & 0xFFF;
  }

  return EFI_NOT_FOUND;
}
/**
  Locate Capability register by PCI address.
  Pci devices need to get capabilitty reg, but the PciDeviceIo is not ready.
  Using the PciRootBridgeIo protocol instead of PciDeviceIo protocol.

  @param PciRootBridgeIo     - Pointer to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param Pci                 - Pointer to device's configuration space buffer.
  @param Bus                 - Bus number.
  @param Device              - Device number.
  @param Func                - Function number.
  @param CapId               - The capability ID.
  @param Offset              - A pointer to the offset.
                               As input: the default offset;
                               As output: the offset of the found block.
  @param NextRegBlock        - An optional pointer to return the value of next block.

  @retval EFI_UNSUPPORTED     - The Pci Io device is not supported.
  @retval EFI_NOT_FOUND       - The Pci Io device cannot be found.
  @retval EFI_SUCCESS         - The Pci Io device is successfully located.

**/  
EFI_STATUS
LocateCapabilityRegBlockByAddress (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  IN     PCI_TYPE00                       *Pci,
  IN     UINT8                            Bus,
  IN     UINT8                            Device,
  IN     UINT8                            Func,
  IN     UINT8                            CapId,
  IN OUT UINT8                            *Offset,
     OUT UINT8                            *NextRegBlock OPTIONAL
  )
{
  UINT8      CapabilityPtr;
  UINT16     CapabilityEntry;
  UINT8      CapabilityID;
  UINT32     Register;
  UINT64     Address;
  
  //
  // To check the capability of this device supports
  //
  if (!(Pci->Hdr.Status & EFI_PCI_STATUS_CAPABILITY)) {
    return EFI_UNSUPPORTED;
  }

  if (*Offset != 0) {
    CapabilityPtr = *Offset;
  } else {

    CapabilityPtr = 0;
    if (IS_CARDBUS_BRIDGE (Pci)) {
      Address = EFI_PCI_ADDRESS (Bus, Device, Func, EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR);
    } else {
      Address = EFI_PCI_ADDRESS (Bus, Device, Func, PCI_CAPBILITY_POINTER_OFFSET);
    }
    //
    // Do not get byte read directly, because some PCI card will return 0xFF
    // when perform PCI-Express byte read, while return correct 0x00
    // when perform PCI-Express dword read, or PCI dword read.
    //
    PciRootBridgeIo->Pci.Read (
                           PciRootBridgeIo,
                           EfiPciWidthUint32,
                           Address,
                           1,
                           &Register
                           );
    CapabilityPtr = (UINT8)Register;      
  }

  while (CapabilityPtr > 0x3F) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFC;
    Address = EFI_PCI_ADDRESS (Bus, Device, Func, CapabilityPtr);
    PciRootBridgeIo->Pci.Read (
                           PciRootBridgeIo,
                           EfiPciWidthUint16,
                           Address,
                           1,
                           &CapabilityEntry
                           );
    CapabilityID = (UINT8) CapabilityEntry;

    if (CapabilityID == CapId) {
      *Offset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (UINT8) (CapabilityEntry >> 8);
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (UINT8) (CapabilityEntry >> 8);
  }

  return EFI_NOT_FOUND;
}
