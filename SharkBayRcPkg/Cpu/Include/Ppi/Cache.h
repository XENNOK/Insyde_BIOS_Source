/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Cache.h

@brief
  Cache PPI defined in EFI 2.0
  This code abstracts Cache Settings
**/
#ifndef _PEI_CACHE_PPI_H_
#define _PEI_CACHE_PPI_H_

#define PEI_CACHE_PPI_GUID \
  { \
    0x9be4bc2, 0x790e, 0x4dea, 0x8b, 0xdc, 0x38, 0x5, 0x16, 0x98, 0x39, 0x44 \
  }
//[-start-120605-IB10820064-modify]//
typedef struct  _PEI_CACHE_PPI PEI_CACHE_PPI;
//[-end-120605-IB10820064-modify]//

typedef enum _EFI_MEMORY_CACHE_TYPE
{
  EfiCacheTypeUncacheable   = 0,
  EfiCacheTypeWriteCombining= 1,
  EfiCacheTypeReserved2     = 2,
  EfiCacheTypeReserved3     = 3,
  EfiCacheTypeWriteThrough  = 4,
  EfiCacheTypeWriteProtected= 5,
  EfiCacheTypeWriteBack     = 6,
  EfiCacheTypeMaximumType   = 7
} EFI_MEMORY_CACHE_TYPE;

typedef
EFI_STATUS
(EFIAPI *PEI_SET_CACHE_PPI) (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN PEI_CACHE_PPI                      * This,
  IN  EFI_PHYSICAL_ADDRESS              MemoryAddress,
  IN  UINT64                            MemoryLength,
  IN  EFI_MEMORY_CACHE_TYPE             MemoryCacheType
  );

typedef
EFI_STATUS
(EFIAPI *PEI_RESET_CACHE_PPI) (
  IN CONST EFI_PEI_SERVICES                  **PeiServices,
  IN PEI_CACHE_PPI                      * This
  );

typedef
EFI_STATUS
(EFIAPI *PEI_ACTIVATE_CACHE_PPI) (
  IN CONST EFI_PEI_SERVICES                  **PeiServices,
  IN PEI_CACHE_PPI                      * This
  );

///
/// Platform code can use this interface to arrange MTRR configuration in PEI phase.
///
struct _PEI_CACHE_PPI {
  ///
  /// Set MTRR configuration for a memory region.
  ///
  PEI_SET_CACHE_PPI      SetCache;
  ///
  /// Clean up MTRR in MTRR buffer.
  ///
  PEI_RESET_CACHE_PPI    ResetCache;
  ///
  /// Disable NEM if in NEM mode, and sync MTRR configuration from buffer to MSR.
  ///
  PEI_ACTIVATE_CACHE_PPI ActivateCache;
};

extern EFI_GUID gPeiCachePpiGuid;

#endif
