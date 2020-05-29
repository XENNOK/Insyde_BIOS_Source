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

  PlatformDimm.h

Abstract:

  PPI for reading platform information about the SDRAM DIMM configuration.

--*/

#ifndef _PEI_PLATFORM_DIMM_H_
#define _PEI_PLATFORM_DIMM_H_

#define PEI_PLATFORM_DIMM_PPI_GUID \
  { \
    0xc0d26551, 0x2374, 0x422a, 0x97, 0x22, 0xac, 0x23, 0xb7, 0xf3, 0xdc, 0xd8 \
  }

EFI_FORWARD_DECLARATION (PEI_PLATFORM_DIMM_PPI);

//
// This structure describes the state of a single DIMM.
// If Present is set, the DIMM socket is populated.
// If Disabled is set, the DIMM should not be used.
//
typedef struct {
  UINT8 Present : 1;
  UINT8 Disabled : 1;
  UINT8 Reserved : 6;
} PEI_PLATFORM_DIMM_STATE;

//
// Function declarations
//
typedef
EFI_STATUS
(EFIAPI *PEI_GET_DIMM_STATE) (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN PEI_PLATFORM_DIMM_PPI              * This,
  IN      UINT8                         Dimm,
  OUT     PEI_PLATFORM_DIMM_STATE       * State
  );

typedef
EFI_STATUS
(EFIAPI *PEI_SET_DIMM_STATE) (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN PEI_PLATFORM_DIMM_PPI              * This,
  IN      UINT8                         Dimm,
  IN      PEI_PLATFORM_DIMM_STATE       * State
  );

typedef
EFI_STATUS
(EFIAPI *PEI_READ_SPD) (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN PEI_PLATFORM_DIMM_PPI              * This,
  IN      UINT8                         Dimm,
  IN      UINT8                         Offset,
  IN      UINTN                         Count,
  OUT     UINT8                         *Buffer
  );

typedef struct _PEI_PLATFORM_DIMM_PPI {
  UINT8               DimmSockets;
  UINT8               DimmSegments;
  UINT8               MemoryChannels;
  PEI_GET_DIMM_STATE  GetDimmState;
  PEI_SET_DIMM_STATE  SetDimmState;
  PEI_READ_SPD        ReadSpd;
} PEI_PLATFORM_DIMM_PPI;

extern EFI_GUID gPeiPlatformDimmPpiGuid;

#endif
