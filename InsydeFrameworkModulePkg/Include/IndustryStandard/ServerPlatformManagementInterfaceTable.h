//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_H_
#define _SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_H_

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
// DBGP Revision (defined in spec)
//
#define EFI_ACPI_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_REVISION 0x05

//
// SPCR Structure Definition
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT8                                   IntelligentPlatformManagementInterface;
  UINT8                                   InterfaceType;
  UINT16                                  SpecificationRevision;
  UINT8                                   InterruptType;
  UINT8                                   GPE;
  UINT8                                   Reserved;
  UINT8                                   PciDeviceFlag;
  UINT32                                  GlobalSystemInterrupt;
  UINT8                                   PciSegmentGroupNumber;
  UINT8                                   PciBusNumber;
  UINT8                                   PciDeviceNumber;
  UINT8                                   PciFunctionNumber;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE  BaseAddress;
} EFI_ACPI_DEBUG_PORT_TABLE;

#pragma pack()

#endif
