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

  MemoryIntrinsics.h

Abstract:

  Memory Intrinsics PPI as defined in EFI 2.0

--*/

#ifndef _PEI_MEMORY_INTRINSICS_PPI_H
#define _PEI_MEMORY_INTRINSICS_PPI_H

#define PEI_MEMORY_INTRINSICS_PPI \
  {0x3836f7e5, 0xc6b8, 0x4329, 0xb2, 0x3a, 0x2, 0x6a, 0xec, 0xc1, 0x2c, 0x8}

EFI_FORWARD_DECLARATION (EFI_PEI_MEMORY_SUPPORT_INTRINICS_PPI);

typedef
EFI_STATUS
(EFIAPI *PEIM_ALLOCATE_PAGES) (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN EFI_ALLOCATE_TYPE          Type,
  IN EFI_MEMORY_TYPE            MemoryType,
  IN UINTN                      Pages,
  IN OUT EFI_PHYSICAL_ADDRESS   *Memory
  );

typedef
EFI_STATUS
(EFIAPI *PEIM_FREE_PAGES) (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS       Memory,
  IN UINTN                      Pages
  );

typedef struct _EFI_PEI_MEMORY_SUPPORT_INTRINICS_PPI {
  PEIM_ALLOCATE_PAGES                    PeiAllocatePage;
  PEIM_FREE_PAGES                        PeiFreePage;
} EFI_PEI_MEMORY_SUPPORT_INTRINICS_PPI;


extern EFI_GUID gPeiMemoryIntrinsicsPpiGuid;

#endif
