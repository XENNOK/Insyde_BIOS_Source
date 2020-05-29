/** @file
  Header file for the SMBIOS Memory Driver.
  This driver will determine memory configuration information from the chipset
  and memory and create SMBIOS Memory structures appropriately.
 
@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
#ifndef _SMBIOS_MEMORY_H_
#define _SMBIOS_MEMORY_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>

#include <Guid/DataHubRecords.h>
///
/// This is the generated header file which includes whatever needs to be exported (strings + IFR)
///
#include "SmbiosMemoryStrDefs.h"

///
/// Driver Consumed Protocol Prototypes
///
#include <Protocol/DataHub.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/MemInfo.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/FrameworkHii.h>
#include <Protocol/SaPlatformPolicy.h>


///
/// Driver private data
///

#define EFI_MEMORY_SUBCLASS_DRIVER_GUID \
  { \
    0x1767CEED, 0xDB82, 0x47cd, 0xBF, 0x2B, 0x68, 0x45, 0x8A, 0x8C, 0xCF, 0xFF \
  }

///
/// Memory
///
#define MEM_FRQCY_BIT_SHIFT 1

#define MAD_DIMM_CH0        0x5004
#define MAD_DIMM_CH1        0x5008
////[-start-130322-IB10040025-add]//
//#define MAX_SOCKETS         4
////[-end-130322-IB10040025-add]//
///
/// Memory module type definition in DDR3 SPD Data
///
#define DDR_MTYPE_SPD_OFFSET  3     ///< Module type, offset 3, bits (3:0)
#define DDR_MTYPE_SPD_MASK    0x0F  ///< Module Type mask
#define DDR_MTYPE_RDIMM       0x01  ///< Registered DIMM Memory
#define DDR_MTYPE_UDIMM       0x02  ///< Unbuffered DIMM Memory
#define DDR_MTYPE_SODIMM      0x03  ///< Small Outline DIMM Memory
#define DDR_MTYPE_MICRO_DIMM  0x04  ///< Micro-DIMM Memory
#define DDR_MTYPE_MINI_RDIMM  0x05  ///< Mini Registered DIMM Memory
#define DDR_MTYPE_MINI_UDIMM  0x06  ///< Mini Unbuffered DIMM Memory
//[-start-130610-IB05400414-add]//
#define DDR_MTYPE_72B_SOUDIMM 0x08  ///< Unbuffered 72-bit Small Outline DIMM
#define DDR_MTYPE_72B_SORDIMM 0x09  ///< Registered 72-bit Small Outline DIMM
//[-end-130610-IB05400414-add]//
///
/// Maximum rank memory size supported by the memory controller
/// 2 GB in terms of KB
///
#define MAX_RANK_CAPACITY (4 * 1024 * 1024)

#ifndef MEMORY_ASSET_TAG
#define MEMORY_ASSET_TAG  L"9876543210"
#endif
///
/// Memory Module Manufacture ID List Structure
///
typedef struct {
  UINT8   Index;
  UINT8   ManufactureId;
  CHAR16  *ManufactureName;
} MEMORY_MODULE_MANUFACTURE_LIST;

///
/// Row configuration data structure
///
typedef struct {
  EFI_PHYSICAL_ADDRESS  BaseAddress;
  UINT64                RowLength;  ///< Size of Row in bytes
} DDR_ROW_CONFIG;

///
/// Prototypes
///
/**
    This driver will determine memory configuration information from the chipset
    and memory and report the memory configuration info to the DataHub.
    
    @param[in] ImageHandle   - Handle for the image of  this driver
    @param[in] SystemTable   - Pointer to the EFI System Table
  
    @retval EFI_SUCCESS          - if the data is successfully reported
    @retval EFI_NOT_FOUND        - if the HOB list could not be located.
    @retval EFI_OUT_OF_RESOURCES - if not able to get resouces.
**/
EFI_STATUS
EFIAPI
SmbiosMemoryEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
;

#endif
