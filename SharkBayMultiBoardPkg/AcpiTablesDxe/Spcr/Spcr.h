/** @file
 This file describes the contents of the ACPI Serial Port Console Redirection
  Table (SPCR).  Some additional ACPI 2.0 values are defined in Acpi2_0.h.
  All changes to the Spcr contents should be done in this file
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPCR_H_
#define _SPCR_H_

//
// SPCR Definitions, see TBD specification for details.
//

#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>

#define EFI_ACPI_SPCR_TABLE_FLOW_CONTROL_NONE      0x0


//
// Base Address
//
#define EFI_ACPI_SPCR_COM_BASE_ADDRESS_BIT_WIDTH      0x08
#define EFI_ACPI_SPCR_COM_BASE_ADDRESS_BIT_OFFSET     0x00
#define EFI_ACPI_SPCR_COM_BASE_ADDRESS_ADDRESS        0x00000000000003F8

//
// Interrupt
//
#define EFI_ACPI_SPCR_COM_IRQ                         4
#define EFI_ACPI_SPCR_COM_GLOBAL_SYSTEM_INTERRUPT     4

//
// PCI Device
//
#define EFI_ACPI_SPCR_COM_DEVICE_ID                   0xFFFF
#define EFI_ACPI_SPCR_COM_VENDOR_ID                   0xFFFF
#define EFI_ACPI_SPCR_COM_BUS_NUM                     0x0
#define EFI_ACPI_SPCR_COM_DEVICE_NUM                  0x0
#define EFI_ACPI_SPCR_COM_FUNCTION_NUM                0x0
//
// Apply to SPCR.ACT
//
#define SPCR_INTERFACE_TYPE         EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_INTERFACE_TYPE_16550
#define SPCR_RESERVED01             {0, 0, 0}
#define SPCR_BASE_ADDRESS           {0x01, 0x08, 0x00, 0x00, 0x00000000000003F8}
#define SPCR_INTERRUPT_TYPE         EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_INTERRUPT_TYPE_8259
#define SPCR_IRQ_NUM                0x04
#define SPCR_GLOBAL_SYS_INTERRUPT   0x0
#define SPCR_BAUD_RATE              EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_BAUD_RATE_115200
#define SPCR_PARITY                 EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_PARITY_NO_PARITY
#define SPCR_STOP_BITS              EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_STOP_BITS_1
#define SPCR_FLOW_CONTROL           EFI_ACPI_SPCR_TABLE_FLOW_CONTROL_NONE
#define SPCR_TERMINAL_TYPE          EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_TERMINAL_TYPE_ANSI
#define SPCR_RESERVED02             0
#define SPCR_PCI_DID                0xFFFF
#define SPCR_PCI_VID                0xFFFF
#define SPCR_PCI_BUS                0x0
#define SPCR_PCI_DEV                0x0
#define SPCR_PCI_FUNC               0x0
#define SPCR_PCI_FLAGS              0x0
#define SPCR_PCI_SEG                0x0
#define SPCR_RESERVED03             0


#endif
