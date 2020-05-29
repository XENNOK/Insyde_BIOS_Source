/** @file
  Header file for initialization of GT PowerManagement

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

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
#ifndef _GRAPHICS_INIT_H_
#define _GRAPHICS_INIT_H_

//#include "EdkIIGlueDxe.h"
#include <Uefi.h>
#include <Guid/DxeServices.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <SaAccess.h>
#include <EfiScriptLib.h>
#include <Protocol/SaPlatformPolicy.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <ExitPmAuth.h>
#include "SaInit.h"
#include <PchAccess.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>

///
/// Data definitions
///
///
/// GT RELATED EQUATES
///
#define GTT_MEM_ALIGN     22
#define GTTMMADR_SIZE_4MB 0x400000
#define GT_WAIT_TIMEOUT   3000     ///< ~3 seconds

///
/// PAVP Modes
///
#define PAVP_LITE_MODE      1
#define PAVP_SERPENT_MODE   2
#define PAVP_PCM_SIZE_1_MB  1

EFI_STATUS
PavpInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Initialize PAVP feature of SystemAgent.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS         - PAVP initialization complete
**/
;

/**
  Initialize PAVP.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS - PAVP initialization complete
**/
EFI_STATUS
PmInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Initialize GT PowerManagement of SystemAgent.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS         - GT Power Management initialization complete
**/
;

EFI_STATUS
GraphicsInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Initialize GT PowerManagement of SystemAgent.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS         - GT Power Management initialization complete
**/
;

VOID
PollGtReady (
  UINT64 Base,
  UINT32 Offset,
  UINT32 Mask,
  UINT32 Result
  )
/**
  "Poll Status" for GT Readiness

  @param[in] Base            - Base address of MMIO
  @param[in] Offset          - MMIO Offset
  @param[in] Mask            - Mask
  @param[in] Result          - Value to wait for
**/
;

EFI_STATUS
PostPmInitExitPmAuth (
  VOID
  )
/**
  Do Post GT PM Init Steps after VBIOS Initialization.

  @retval EFI_SUCCESS          Succeed.
**/
;
#endif
