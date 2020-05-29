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
  PCI command register operations supporting functions declaration for PCI Bus module.

Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _EFI_PCI_COMMAND_H_
#define _EFI_PCI_COMMAND_H_

//
// The PCI Command register bits owned by PCI Bus driver.
//
// They should be cleared at the beginning. The other registers
// are owned by chipset, we should not touch them.
//
#define EFI_PCI_COMMAND_BITS_OWNED                          ( \
                EFI_PCI_COMMAND_IO_SPACE                    | \
                EFI_PCI_COMMAND_MEMORY_SPACE                | \
                EFI_PCI_COMMAND_BUS_MASTER                  | \
                EFI_PCI_COMMAND_MEMORY_WRITE_AND_INVALIDATE | \
                EFI_PCI_COMMAND_VGA_PALETTE_SNOOP           | \
                EFI_PCI_COMMAND_FAST_BACK_TO_BACK             \
                )

//
// The PCI Bridge Control register bits owned by PCI Bus driver.
//
// They should be cleared at the beginning. The other registers
// are owned by chipset, we should not touch them.
//
#define EFI_PCI_BRIDGE_CONTROL_BITS_OWNED                   ( \
                EFI_PCI_BRIDGE_CONTROL_ISA                  | \
                EFI_PCI_BRIDGE_CONTROL_VGA                  | \
                EFI_PCI_BRIDGE_CONTROL_VGA_16               | \
                EFI_PCI_BRIDGE_CONTROL_FAST_BACK_TO_BACK      \
                )

//
// The PCCard Bridge Control register bits owned by PCI Bus driver.
//
// They should be cleared at the beginning. The other registers
// are owned by chipset, we should not touch them.
//
#define EFI_PCCARD_BRIDGE_CONTROL_BITS_OWNED                ( \
                EFI_PCI_BRIDGE_CONTROL_ISA                  | \
                EFI_PCI_BRIDGE_CONTROL_VGA                  | \
                EFI_PCI_BRIDGE_CONTROL_FAST_BACK_TO_BACK      \
                )


#define EFI_GET_REGISTER      1
#define EFI_SET_REGISTER      2
#define EFI_ENABLE_REGISTER   3
#define EFI_DISABLE_REGISTER  4

/**
  Operate the PCI register via PciIo function interface.

  @param PciIoDevice     Pointer to instance of PCI_IO_DEVICE.
  @param Command         Operator command.
  @param Offset          The address within the PCI configuration space for the PCI controller.
  @param Operation       Type of Operation.
  @param PtrCommand      Return buffer holding old PCI command, if operation is not EFI_SET_REGISTER.
  @param SaveScriptTable Save the Pci operation to S3 Boot script table.

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
  );

/**
  Check the cpability supporting by given device.

  @param PciIoDevice   Pointer to instance of PCI_IO_DEVICE.

  @retval TRUE         Cpability supportted.
  @retval FALSE        Cpability not supportted.

**/
BOOLEAN
PciCapabilitySupport (
  IN PCI_IO_DEVICE  *PciIoDevice
  );

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
  );

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
  );

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
  );

/**
  Macro that reads command register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[out]           Pointer to the 16-bit value read from command register.

  @return status of PciIo operation

**/
#define PCI_READ_COMMAND_REGISTER(a,b) \
        PciOperateRegister (a, 0, PCI_COMMAND_OFFSET, EFI_GET_REGISTER, b, FALSE)

/**
  Macro that writes command register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The 16-bit value written into command register.

  @return status of PciIo operation

**/
#define PCI_SET_COMMAND_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_COMMAND_OFFSET, EFI_SET_REGISTER, NULL, TRUE)

/**
  Macro that enables command register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The enabled value written into command register.

  @return status of PciIo operation

**/
#define PCI_ENABLE_COMMAND_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_COMMAND_OFFSET, EFI_ENABLE_REGISTER, NULL, TRUE)

/**
  Macro that disalbes command register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The disabled value written into command register.

  @return status of PciIo operation

**/
#define PCI_DISABLE_COMMAND_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_COMMAND_OFFSET, EFI_DISABLE_REGISTER, NULL, TRUE)

/**
  Macro that reads PCI bridge control register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[out]           The 16-bit value read from control register.

  @return status of PciIo operation

**/
#define PCI_READ_BRIDGE_CONTROL_REGISTER(a,b) \
        PciOperateRegister (a, 0, PCI_BRIDGE_CONTROL_REGISTER_OFFSET, EFI_GET_REGISTER, b, FALSE)

/**
  Macro that writes PCI bridge control register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The 16-bit value written into control register.

  @return status of PciIo operation

**/
#define PCI_SET_BRIDGE_CONTROL_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_BRIDGE_CONTROL_REGISTER_OFFSET, EFI_SET_REGISTER, NULL, TRUE)

/**
  Macro that enables PCI bridge control register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The enabled value written into command register.

  @return status of PciIo operation

**/
#define PCI_ENABLE_BRIDGE_CONTROL_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_BRIDGE_CONTROL_REGISTER_OFFSET, EFI_ENABLE_REGISTER, NULL, TRUE)

/**
 Macro that disalbes PCI bridge control register.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The disabled value written into command register.

  @return status of PciIo operation

**/
#define PCI_DISABLE_BRIDGE_CONTROL_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_BRIDGE_CONTROL_REGISTER_OFFSET, EFI_DISABLE_REGISTER, NULL, TRUE)

//
// Test Command, skip S3 Save Script Table.
//

/**
  Macro that set command register for testing. It won't add to the S3 boot script table.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The 16-bit value written into command register.

  @return status of PciIo operation

**/
#define PCI_SET_TEST_COMMAND_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_COMMAND_OFFSET, EFI_SET_REGISTER, NULL, FALSE)


/**
  Macro that set PCI bridge control register for testing. It won't add to the S3 boot script table.

  @param a[in]            Pointer to instance of PCI_IO_DEVICE.
  @param b[in]            The 16-bit value written into control register.

  @return status of PciIo operation

**/
#define PCI_SET_TEST_BRIDGE_CONTROL_REGISTER(a,b) \
        PciOperateRegister (a, b, PCI_BRIDGE_CONTROL_REGISTER_OFFSET, EFI_SET_REGISTER, NULL, FALSE)

#endif
