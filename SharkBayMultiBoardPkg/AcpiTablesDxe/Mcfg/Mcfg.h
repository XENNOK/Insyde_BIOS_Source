/** @file

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

#ifndef _MCFG_H
#define _MCFG_H

//
// Statements that include other files
//
#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi20.h>
#include <IndustryStandard/Acpi30.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>

//
// MCFG Definitions
//

#define EFI_ACPI_OEM_MCFG_REVISION 0x00000001

//
// Define the number of allocation structures so that we can build the table structure.
//

#define EFI_ACPI_ALLOCATION_STRUCTURE_COUNT           1

//
// MCFG structure
//

//
// Ensure proper structure formats
//
#pragma pack (1)

//
// MCFG Table structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER      Header;
  UINT64                           Reserved;		// IB0910010++
#if EFI_ACPI_ALLOCATION_STRUCTURE_COUNT > 0
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  AllocationStructure[EFI_ACPI_ALLOCATION_STRUCTURE_COUNT];
#endif
} EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_DESCRIPTION_TABLE;

#pragma pack ()

//
// PCI Express base address, and end bus number
//
#ifndef PLATFORM_PCIEX_BASE_ADDRESS
#define PCIEXPRESS_BASE_ADDRESS     0x00000000E0000000
#define PCIEXPRESS_END_BUS_NUMBER   0xFF
#else

#ifndef PLATFORM_PCIEX_BASE_ADDRESS_64MB
#define PCIEXPRESS_BASE_ADDRESS     0x00000000F0000000
#define PCIEXPRESS_END_BUS_NUMBER   0x7F
#else
#define PCIEXPRESS_BASE_ADDRESS     0x00000000F0000000
#define PCIEXPRESS_END_BUS_NUMBER   0x3F
#endif

#endif

#endif
