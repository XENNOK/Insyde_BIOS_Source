/** @file
  This code defines ACPI DMA Remapping table related definitions.
  See the System Agent BIOS specification for definition of the table.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _DMA_REMAPPING_TABLE_H_
#define _DMA_REMAPPING_TABLE_H_

#include <Uefi.h>
#include <IndustryStandard/Acpi30.h>

#pragma pack(1)
///
/// DMAR table signature
///
#define EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE   0x52414D44  ///< "DMAR"
#define EFI_ACPI_DMAR_TABLE_REVISION        1
#define EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH  0x10
#define EFI_ACPI_RMRR_HEADER_LENGTH         0x18
#define MAX_PCI_DEPTH                       5

typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT16  Reserved;
  UINT8   EnumId;
  UINT8   StartBusNumber;
  UINT8   PciPath[2];
} EFI_ACPI_DEV_SCOPE_STRUCTURE;

typedef struct {
  UINT16                        Type;
  UINT16                        Length;
  UINT8                         Flags;
  UINT8                         Reserved;
  UINT16                        SegmentNum;
  EFI_PHYSICAL_ADDRESS          RegisterBaseAddress;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[1];
} EFI_ACPI_DRHD_ENGINE1_STRUCT;

typedef struct {
  UINT16                        Type;
  UINT16                        Length;
  UINT8                         Flags;
  UINT8                         Reserved;
  UINT16                        SegmentNum;
  EFI_PHYSICAL_ADDRESS          RegisterBaseAddress;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[9];
} EFI_ACPI_DRHD_ENGINE2_STRUCT;

typedef struct {
  UINT16                        Type;
  UINT16                        Length;
  UINT16                        Reserved;
  UINT16                        SegmentNum;
  EFI_PHYSICAL_ADDRESS          RmrBaseAddress;
  EFI_PHYSICAL_ADDRESS          RmrLimitAddress;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[3];
} EFI_ACPI_RMRR_USB_STRUC;

typedef struct {
  UINT16                        Type;
  UINT16                        Length;
  UINT16                        Reserved;
  UINT16                        SegmentNum;
  EFI_PHYSICAL_ADDRESS          RmrBaseAddress;
  EFI_PHYSICAL_ADDRESS          RmrLimitAddress;
  EFI_ACPI_DEV_SCOPE_STRUCTURE  DeviceScope[1];
} EFI_ACPI_RMRR_IGD_STRUC;

typedef struct {
  UINT16                        Type;
  UINT16                        Length;
  UINT8                         Reserved[3];
  UINT8                         AcpiDeviceNumber;
  UINT8                         AcpiObjectName[20];
} EFI_ACPI_ANDD_STRUC;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER   Header;
  UINT8                         HostAddressWidth;
  UINT8                         Flags;
  UINT8                         Reserved[10];
  EFI_ACPI_DRHD_ENGINE1_STRUCT  DrhdEngine1;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  DrhdEngine2;
  EFI_ACPI_RMRR_USB_STRUC       RmrrUsb;
  EFI_ACPI_RMRR_IGD_STRUC       RmrrIgd;
  EFI_ACPI_ANDD_STRUC           AnddI2C0;
  EFI_ACPI_ANDD_STRUC           AnddI2C1;
  EFI_ACPI_ANDD_STRUC           AnddSpi0;
  EFI_ACPI_ANDD_STRUC           AnddSpi1;
  EFI_ACPI_ANDD_STRUC           AnddUa00;
  EFI_ACPI_ANDD_STRUC           AnddUa01;
  EFI_ACPI_ANDD_STRUC           AnddSdhc;
} EFI_ACPI_DMAR_TABLE;

#pragma pack()

#endif
