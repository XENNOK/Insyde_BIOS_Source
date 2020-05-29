//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SerialPortConsoleRedirectionTable.h

Abstract:

  ACPI Serial Port Console Redirection Table as defined in TBD spec.
  Can't find spec...

--*/

#ifndef _SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_H_
#define _SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_H_

//
// Include files
//
#include "Tiano.h"
#include "Acpi2_0.h"

//
// Ensure proper structure formats
//
#pragma pack(1)
//
// SPCR Revision (defined in spec)
//
#define EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_REVISION 0x01

//
// SPCR Structure Definition
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT8                                   InterfaceType;
  UINT8                                   Reserved1[3];
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE  BaseAddress;
  UINT8                                   InterruptType;
  UINT8                                   Irq;
  UINT32                                  GlobalSystemInterrupt;
  UINT8                                   BaudRate;
  UINT8                                   Parity;
  UINT8                                   StopBits;
  UINT8                                   FlowControl;
  UINT8                                   TerminalType;
  UINT8                                   Reserved;
  UINT16                                  PciDeviceId;
  UINT16                                  PciVendorId;
  UINT8                                   PciBusNumber;
  UINT8                                   PciDeviceNumber;
  UINT8                                   PciFunctionNumber;
  UINT32                                  PciFlags;
  UINT8                                   PciSegment;
  UINT32                                  Reserved2;
} EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE;

#pragma pack()

//
// SPCR Definitions
//

//
// Interface Type, 2-255 reserved
//
#define EFI_ACPI_SPCR_TABLE_INTERFACE_TYPE_16550   0
#define EFI_ACPI_SPCR_TABLE_INTERFACE_TYPE_16450   1

//
// Interrupt Type, Bits[3-7] reserved
//
#define EFI_ACPI_SPCR_TABLE_INTERRUPT_TYPE_8259    0x1
#define EFI_ACPI_SPCR_TABLE_INTERRUPT_TYPE_APIC    0x2
#define EFI_ACPI_SPCR_TABLE_INTERRUPT_TYPE_SAPIC   0x4

//
// Baud Rate, 0-2, 5, 8-255 reserved
//
#define EFI_ACPI_SPCR_TABLE_BAUD_RATE_9600         3
#define EFI_ACPI_SPCR_TABLE_BAUD_RATE_19200        4
#define EFI_ACPI_SPCR_TABLE_BAUD_RATE_57600        6
#define EFI_ACPI_SPCR_TABLE_BAUD_RATE_115200       7

//
// Parity, 1-255 reserved
//
#define EFI_ACPI_SPCR_TABLE_PARITY_NO_PARITY       0

//
// Stop Bits, 0, 2-255 reserved
//
#define EFI_ACPI_SPCR_TABLE_STOP_BITS_1            1

//
// Flow Control, Bits[3:7] reserved
//
#define EFI_ACPI_SPCR_TABLE_FLOW_CONTROL_NONE      0x0
#define EFI_ACPI_SPCR_TABLE_FLOW_CONTROL_DCD       0x1
#define EFI_ACPI_SPCR_TABLE_FLOW_CONTROL_RTS_CTS   0x2
#define EFI_ACPI_SPCR_TABLE_FLOW_CONTROL_XON_XOFF  0x4

//
// Terminal Type, 4-255 reserved
//
#define EFI_ACPI_SPCR_TABLE_TERMINAL_TYPE_VT100      0
#define EFI_ACPI_SPCR_TABLE_TERMINAL_TYPE_VT100_PLUS 1
#define EFI_ACPI_SPCR_TABLE_TERMINAL_TYPE_VT_UTF8    2
#define EFI_ACPI_SPCR_TABLE_TERMINAL_TYPE_ANSI       3

#endif
