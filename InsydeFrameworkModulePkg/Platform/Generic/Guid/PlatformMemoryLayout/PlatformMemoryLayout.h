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

  PlatformMemoryLayout.h

Abstract:

  GUID used by platform drivers to keep track of memory layout across system boots.

--*/

#ifndef _PLATFORM_MEMORY_LAYOUT_H_
#define _PLATFORM_MEMORY_LAYOUT_H_

//
// GUID, used for variable vendor GUID and the GUIDed HOB.
//
#define EFI_PLATFORM_MEMORY_LAYOUT_GUID \
  { \
    0x41c7b74e, 0xb839, 0x40d9, 0xa0, 0x56, 0xe3, 0xca, 0xfc, 0x98, 0xa, 0xac \
  }

//
// Variable name
//
#define EFI_PLATFORM_MEMORY_LAYOUT_NAME L"Platform Memory Layout"

//
// Variable attributes
//
#define EFI_PLATFORM_MEMORY_LAYOUT_ATTRIBUTES (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

//
// Structure for tracking a single DIMM state
//
typedef struct {
  UINT8 Present : 1;
  UINT8 Configured : 1;
  UINT8 Disabled : 1;
  UINT8 Reserved : 5;
} EFI_DIMM_STATE;

//
// Structure for tracking DIMM layout information
// This must be packed.
//
#pragma pack(1)
typedef struct {
  UINT8           DimmSets;
  UINT8           DimmsPerSet;
  UINT8           RowsPerSet;
  EFI_DIMM_STATE  State[1];
} EFI_DIMM_LAYOUT;
#pragma pack()
//
// The format of the variable or GUIDed HOB is a single EFI_DIMM_LAYOUT
// structure followed by an array of (DimmSets * DimmsPerSet) of
// EFI_DIMM_STATES structures.
//
extern EFI_GUID gEfiPlatformMemoryLayoutGuid;

#endif
