/** @file
  Header file for RapidStart Gfx PPI

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Mobile Silicon Support Module" and is
  licensed for Intel Mobile CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PEI_GFX_PPI_H_
#define _PEI_GFX_PPI_H_

#define PEI_GFX_PPI_GUID \
  { \
    0xc4d6994a, 0xaa3d, 0x47c0, 0xae, 0xa, 0xb0, 0xbf, 0xad, 0xba, 0xa6, 0x13 \
  }

extern EFI_GUID gPeiGfxPpiGuid;

#define DSS_RANGE                0x01 //< Dss Range is 1
#define GFX_RESTORE_RANGE        0x02 ///< Gfx Memory restore Range is 2
#define FB_RESTORE_RANGE         250  ///< The Max FB Range is 16
#define MAX_GFX_MEMORY_RANGES   (DSS_RANGE + GFX_RESTORE_RANGE + FB_RESTORE_RANGE)

#pragma pack(1)

typedef struct {
   UINT64         BaseAddress;
   UINT64         Size;
} GFX_MEM_RANGE_ENTRY;

typedef struct {
   UINT16                    NumOfEntry;
   UINT8                     Reserved[2];
   GFX_MEM_RANGE_ENTRY       MemRangeEntry[MAX_GFX_MEMORY_RANGES];
} GFX_RESTORE_MEM_TABLE;

#pragma pack()

typedef
EFI_STATUS
(EFIAPI * PEI_GFX_PPI_GET_DSS_ADDRESS_RANGE) (
  IN CONST EFI_PEI_SERVICES        **PeiServices,
  OUT GFX_RESTORE_MEM_TABLE        **DssAddressRange
);

typedef
EFI_STATUS
(EFIAPI * PEI_GFX_PPI_GET_RESTORE_MEM_TABLE) (
  IN CONST EFI_PEI_SERVICES        **PeiServices,
  OUT GFX_RESTORE_MEM_TABLE        **GfxRestoreMemTable
);

typedef
EFI_STATUS
(EFIAPI * PEI_GFX_PPI_GET_RESTORE_FB_RANGE) (
  IN CONST EFI_PEI_SERVICES        **PeiServices,
  OUT GFX_RESTORE_MEM_TABLE        **FbRestoreMemTable
);

typedef
EFI_STATUS
(EFIAPI * PEI_GFX_PPI_RESTORE_DISPLAY) (
    IN CONST EFI_PEI_SERVICES         **PeiServices
);

#pragma pack(1)

typedef struct _PEI_GFX_PPI {
  PEI_GFX_PPI_RESTORE_DISPLAY          RestoreDisplay;
  PEI_GFX_PPI_GET_DSS_ADDRESS_RANGE    GetDssAddressRange;
  PEI_GFX_PPI_GET_RESTORE_MEM_TABLE    GetRestoreMemTable;
  PEI_GFX_PPI_GET_RESTORE_FB_RANGE     GetRestoreFbRange;
} PEI_GFX_PPI;

#pragma pack()

#endif
