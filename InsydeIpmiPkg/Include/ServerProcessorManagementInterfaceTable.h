/** @file
 H2O IPMI Misc module header file that contains ACPI SPMI table definitions.

 SPMI is an ACPI table related to IPMI specification and BIOS needs to publish
 to OS during POST. This header file is the definition of SPMI table since there
 is no existed header file in EDK II commmon header file.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _SERVER_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_H_
#define _SERVER_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_H_


#include <IndustryStandard/Acpi.h>


//
// Ensure proper structure formats
//
#pragma pack (1)

//
// Server Processor Management Interface Table definition.
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER            Header;
  UINT8                                  InterfaceType;
  UINT8                                  Reserved_37;
  UINT16                                 SpecificationRevision;
  UINT8                                  InterruptType;
  UINT8                                  GPE;
  UINT8                                  Reserved_42;
  UINT8                                  PCIDeviceFlag;
  UINT32                                 GlobalSystemInterrupt;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE BaseAddress;
  UINT8                                  PCISegmentGroup_UID1;
  UINT8                                  PCIBusNumber_UID2;
  UINT8                                  PCIDeviceNumber_UID3;
  UINT8                                  PCIFunctionNumber_UID4;
  UINT8                                  Reserved_64;
} EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE;

#pragma pack ()

//
// SPMI Revision
//
#define EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION  0x05

//
// Interface Type
//
#define EFI_ACPI_SPMI_INTERFACE_TYPE_RESERVED   0
#define EFI_ACPI_SPMI_INTERFACE_TYPE_KCS        1
#define EFI_ACPI_SPMI_INTERFACE_TYPE_SMIC       2
#define EFI_ACPI_SPMI_INTERFACE_TYPE_BT         3
#define EFI_ACPI_SPMI_INTERFACE_TYPE_SSIF       4

//
// SPMI Specfication Revision
//
#define EFI_ACPI_SPMI_SPECIFICATION_REVISION    0x0150

//
// SPMI Interrupt Type
//
#define EFI_ACPI_SPMI_INTERRUPT_TYPE_SCI        0x1
#define EFI_ACPI_SPMI_INTERRUPT_TYPE_IOAPIC     0x2

//
// Default ACPI OEM ID
//
#define EFI_ACPI_OEM_ID                  'I','N','S','Y','D','E'                       // Manufacturer goes here
#define EFI_ACPI_OEM_TABLE_ID            SIGNATURE_64('H','2','O',' ','B','I','O','S') // Board name goes here
#define EFI_ACPI_OEM_REVISION            0x00000001
#define EFI_ACPI_CREATOR_ID              SIGNATURE_32('A','C','P','I')
#define EFI_ACPI_CREATOR_REVISION        0x00040000


#endif

