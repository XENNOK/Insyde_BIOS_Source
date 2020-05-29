/** @file
  Interface definition details for PCH Me UMA.

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
  Copyright (c) 2010 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _PCH_ME_UMA_PPI_H_
#define _PCH_ME_UMA_PPI_H_

#define PCH_ME_UMA_PPI_GUID \
  { 0x8C376010, 0x2400, 0x4D7D, { 0xB4, 0x7B, 0x9D, 0x85, 0x1D, 0xF3, 0xC9, 0xD1 }}

extern EFI_GUID gPchMeUmaPpiGuid;

///
/// Revision
///
#define PCH_ME_UMA_PPI_REVISION 1

///
/// define the MRC recommended boot modes.
///
typedef enum {
  s3Boot, ///< In current implementation, bmS3 == bmWarm
  warmBoot,
  coldBoot,
  fastBoot,
} MRC_BOOT_MODE_T;

/**
  This procedure will read and return the amount of ME UMA requested
  by ME ROM from the HECI device.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] FfsHeader            Pointer to the FFS file header

  @retval UINT32                  Return ME UMA Size
  @retval EFI_SUCCESS             Do not check for ME UMA

**/
typedef
UINT32
(EFIAPI *ME_SEND_UMA_SIZE) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_FFS_FILE_HEADER         *FfsHeader
  );

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
typedef
EFI_STATUS
(EFIAPI *CPU_REPLACEMENT_CHECK) (
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
typedef
EFI_STATUS
(EFIAPI *ME_CONFIG_DID_REG) (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_FFS_FILE_HEADER          *FfsHeader,
  IN MRC_BOOT_MODE_T              MrcBootMode,
  IN UINT8                        InitStat,
  IN UINT32                       FtpmStolenBase,
  IN UINT32                       MeUmaSize
  );

/**
  This procedure will enforce the BIOS Action that was requested by ME FW
  as part of the DRAM Init Done message.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MrcBootMode          MRC BootMode
  @param[in] BiosAction           Me requests BIOS to act

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
typedef
EFI_STATUS
(EFIAPI *HANDLE_ME_BIOS_ACTION) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       MRC_BOOT_MODE_T             MrcBootMode,
  IN       UINT8                       BiosAction
  );

///
/// ME UMA PPI
/// This PPI provides an interface to get the ME UMA size and performs the DID handshake with ME
///
typedef struct PCH_ME_UMA_PPI {
  ///
  /// This procedure will read and return the amount of ME UMA requested by ME ROM from the
  /// HECI device
  ///
  ME_SEND_UMA_SIZE                MeSendUmaSize;
  ///
  /// This procedure will determine whether or not the CPU was replaced during system power loss
  /// or via dynamic fusing
  ///
  CPU_REPLACEMENT_CHECK           CpuReplacementCheck;
  ///
  /// This procedure will configure the ME Host General Status register, indicating that DRAM
  /// Initialization is complete and ME FW may begin using the allocated ME UMA space
  ///
  ME_CONFIG_DID_REG               MeConfigDidReg;
  ///
  /// This procedure will enforce the BIOS Action that was requested by ME FW as part of the
  /// DRAM Init Done message
  ///
  HANDLE_ME_BIOS_ACTION           HandleMeBiosAction;
} PCH_ME_UMA_PPI;

#endif
