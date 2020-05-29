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

  StaticResourceAffinityTable.h

Abstract:

  ACPI Static resource definition table definition, defined at
  http://microsoft.com/hwdev/design/srat.htm.

--*/

#ifndef _STATIC_RESOURCE_AFFINITY_TABLE_H_
#define _STATIC_RESOURCE_AFFINITY_TABLE_H_

//
// Include files
//
#include "Tiano.h"

//
// Ensure proper structure formats
//
#pragma pack(1)
//
// SRAT Revision (defined in spec)
//
#define EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE_REVISION  0x01

//
// SRAT structure types.
// All other values between 0x02 an 0xFF are reserved and
// will be ignored by OSPM.
//
#define EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY  0x00
#define EFI_ACPI_MEMORY_AFFINITY                      0x01

//
// SRAT Structure Definitions
//
//
// Processor Local APIC/SAPIC Affinity Structure Definition
//
typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT8   ProximityDomain;
  UINT8   ApicId;
  UINT32  Flags;
  UINT8   LocalSapicEid;
  UINT8   Reserved[7];
} EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_AFFINITY_STRUCTURE;

//
// Local APIC/SAPIC Flags.  All other bits are reserved and must be 0.
//
#define EFI_ACPI_PROCESSOR_LOCAL_APIC_SAPIC_ENABLED (1 << 0)

//
// Memory Affinity Structure Definition
//
typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT8   ProximityDomain;
  UINT8   Reserved1[5];
  UINT32  AddressBaseLow;
  UINT32  AddressBaseHigh;
  UINT32  LengthLow;
  UINT32  LengthHigh;
  UINT32  MemoryType;
  UINT32  Flags;
  UINT64  Reserved2;
} EFI_ACPI_MEMORY_AFFINITY_STRUCTURE;

//
// Memory Flags.  All other bits are reserved and must be 0.
//
#define EFI_ACPI_MEMORY_ENABLED               (1 << 0)
#define EFI_ACPI_MEMORY_HOT_REMOVE_SUPPORTED  (1 << 1)

//
// Static Resource Affinity Table header definition.  The rest of the table
// must be defined in a platform specific manner.
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT32                      TableRevision;
  UINT64                      Reserved;
} EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE_HEADER;

#pragma pack()

#endif
