//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _CORRECTED_PLATFORM_ERROR_POLLING_TABLE_H_
#define _CORRECTED_PLATFORM_ERROR_POLLING_TABLE_H_

//
// Include files
//
#include "Tiano.h"
#include "Acpi2_0.h"
#include "Acpi3_0.h"

//
// Ensure proper structure formats
//
#pragma pack(1)
//
// CPEP Revision (defined in spec)
//
#define EFI_ACPI_CORRECTED_PLATFORM_ERROR_POLLING_TABLE_REVISION 0x01

//
// SPCR Structure Definition
//

//
// According to the number of the "EFI_ACPI_PROCESSOR_LOCAL_APIC_COUNT"
// to define NumberOfCPEP
//
#define NumberOfCPEP 4

typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT8   ProcessorID;
  UINT8   ProcessorEID;
  UINT64  PollingInterval;
} EFI_ACPI_2_0_CORRECTED_PLATFORM_ERROR_POLLING_STRUCTURE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                                         Header;
  UINT8                                                               Reversed[8];
  EFI_ACPI_2_0_CORRECTED_PLATFORM_ERROR_POLLING_STRUCTURE             CPEP[NumberOfCPEP];
} EFI_ACPI_CORRECTED_PLATFORM_ERROR_POLLING_TABLE;

#pragma pack()

#endif
