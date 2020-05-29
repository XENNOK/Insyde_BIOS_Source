/** @file
  Header file for Framework PEIM to PchMeUma

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2010 - 2012 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PCH_ME_UMA_H_
#define _PCH_ME_UMA_H_

#include <MeAccess.h>
#include <HeciRegs.h>
#include <IndustryStandard/Pci.h>

#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PerformanceLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Ppi/Stall.h>
#include <Ppi/Wdt.h>
#include <Ppi/PchMeUma.h>
#ifdef PTT_FLAG
#include <Ppi/MePlatformPolicyPei.h>
#endif
#include <Guid/MeDataHob.h>

///
/// LPT PCI Register Definition(s)
///
#define R_MESEG_BASE  0x70

//
// ME FW communication timeout value definitions
//
#define DID_TIMEOUT_MULTIPLIER    0x1388
#define MUSZV_TIMEOUT_MULTIPLIER  0x1388
#define CPURV_TIMEOUT_MULTIPLIER  0x32
#define STALL_1_MILLISECOND       1000
#define STALL_100_MICROSECONDS    100

//
// Function Prototype(s)
//

/**
  This procedure will read and return the amount of ME UMA requested
  by ME ROM from the HECI device.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Pointer to the FFS file header

  @retval UINT32                  Return ME UMA Size
  @retval EFI_SUCCESS             Do not check for ME UMA
**/
UINT32
MeSendUmaSize (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_FFS_FILE_HEADER         *FfsHeader
  );

/**
  Init and Install ME Hob

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS
**/
EFI_STATUS
InstallMeHob (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
;

/**
  This procedure will determine whether or not the CPU was replaced
  during system power loss or via dynamic fusing.
  Calling this procedure could result in a warm reset (if ME FW is requesting one).

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Not used.
  @param[out] ForceFullTraining   When set = 0x1, MRC will be forced to perform a full
                                  memory training cycle.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
CpuReplacementCheck (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_FFS_FILE_HEADER         *FfsHeader,
  OUT      UINT8                       *ForceFullTraining
  );

/**
  This procedure will configure the ME Host General Status register,
  indicating that DRAM Initialization is complete and ME FW may
  begin using the allocated ME UMA space.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Pointer to the FFS file header
  @param[in] MrcBootMode          MRC BootMode
  @param[in] InitStat             H_GS[27:24] Status
  @param[in] FtpmStolenBase       The base of FTPM

  @retval EFI_SUCCESS
**/
EFI_STATUS
MeConfigDidReg (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN 	   EFI_FFS_FILE_HEADER    *FfsHeader,
  IN 	   MRC_BOOT_MODE_T        MrcBootMode,
  IN 	   UINT8                  InitStat,
  IN 	   UINT32                 FtpmStolenBase,
  IN 	   UINT32                 MeUmaSize
  );

/**
  This procedure will enforce the BIOS Action that was requested by ME FW
  as part of the DRAM Init Done message.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MrcBootMode          MRC BootMode
  @param[in] BiosAction           Me requests BIOS to act

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
HandleMeBiosAction (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       MRC_BOOT_MODE_T             MrcBootMode,
  IN       UINT8                       BiosAction
  );

/**
  This procedure will issue a Non-Power Cycle, Power Cycle, or Global Rest.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] ResetType            Type of reset to be issued.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PerformReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT8                       ResetType
  );

/**
  This procedure will clear the DISB.

  @param[in] None

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
ClearDisb (
  VOID
  );

/**
  This procedure will clear the DISB.

  @param[in] None

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
SetDISB (
  VOID
  );

#endif
