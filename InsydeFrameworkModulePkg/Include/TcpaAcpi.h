//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  TcpaAcpi.h

Abstract:

  TCPA ACPI table definitions.

--*/

#ifndef _TCPA_ACPI_H_
#define _TCPA_ACPI_H_

#include <IndustryStandard/Acpi.h>

//
// Based on TCG ACPI Specification Version 1.00
//
#define TCG_PLATFORM_TYPE_CLIENT   0
#define TCG_PLATFORM_TYPE_SERVER   1

#pragma pack (push, 1)
typedef struct _EFI_TCG_CLIENT_ACPI_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT16                                  PlatformClass;
  UINT32                                  Laml;
  EFI_PHYSICAL_ADDRESS                    Lasa;
} EFI_TCG_CLIENT_ACPI_TABLE;

typedef struct _EFI_TCG_SERVER_ACPI_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT16                                  PlatformClass;
  UINT16                                  Reserved0;
  UINT64                                  Laml;
  EFI_PHYSICAL_ADDRESS                    Lasa;
  UINT16                                  SpecRev;
  UINT8                                   DeviceFlags;
  UINT8                                   InterruptFlags;
  UINT8                                   Gpe;
  UINT8                                   Reserved1[3];
  UINT32                                  GlobalSysInt;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  BaseAddress;
  UINT32                                  Reserved2;
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  ConfigAddress;
  UINT8                                   PciSegNum;
  UINT8                                   PciBusNum;
  UINT8                                   PciDevNum;
  UINT8                                   PciFuncNum;
} EFI_TCG_SERVER_ACPI_TABLE;
#pragma pack (pop)

#endif
