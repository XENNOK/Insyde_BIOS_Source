/** @file
  This driver is responsible for the registration of child drivers and the abstraction of the PCH SMI sources.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

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
  PchxSmmHelpers.h

@brief
  This driver is responsible for the registration of child drivers
  and the abstraction of the PCH SMI sources.

**/
#ifndef _PCHX_SMM_HELPERS_H_
#define _PCHX_SMM_HELPERS_H_

#include <PchSmm.h>
#include <Library/PchPlatformLib.h>

/**
  Initialize bits that aren't necessarily related to an SMI source.

  @param[in] None

  @retval EFI_SUCCESS             SMI source initialization completed.
  @retval Asserts                 Global Smi Bit is not enabled successfully.
**/
EFI_STATUS
PchSmmInitHardware (
  VOID
  );

/**
  Enables the PCH to generate SMIs. Note that no SMIs will be generated
  if no SMI sources are enabled. Conversely, no enabled SMI source will
  generate SMIs if SMIs are not globally enabled. This is the main
  switchbox for SMI generation.

  @param[in] None

  @retval EFI_SUCCESS             Enable Global Smi Bit completed
**/
EFI_STATUS
PchSmmEnableGlobalSmiBit (
  VOID
  );

/**
  Clears the SMI after all SMI source have been processed.
  Note that this function will not work correctly (as it is
  written) unless all SMI sources have been processed.
  A revision of this function could manually clear all SMI
  status bits to guarantee success.

  @param[in] None

  @retval EFI_SUCCESS             Clears the SMIs completed
  @retval Asserts                 EOS was not set to a 1
**/
EFI_STATUS
PchSmmClearSmi (
  VOID
  );

/**
  Set the SMI EOS bit after all SMI source have been processed.

  @param[in] None

  @retval FALSE                   EOS was not set to a 1; this is an error
  @retval TRUE                    EOS was correctly set to a 1
**/
BOOLEAN
PchSmmSetAndCheckEos (
  VOID
  );

/**
  Determine whether an ACPI OS is present (via the SCI_EN bit)

  @param[in] None

  @retval TRUE                    ACPI OS is present
  @retval FALSE                   ACPI OS is not present
**/
BOOLEAN
PchSmmGetSciEn (
  VOID
  );

/**
  Read a specifying bit with the register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number

  @retval TRUE                    The bit is enabled
  @retval FALSE                   The bit is disabled
**/
BOOLEAN
ReadBitDesc (
  const PCH_SMM_BIT_DESC *BitDesc
  );

/**
  Write a specifying bit with the register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number
  @param[in] ValueToWrite         The value to be wrote
  @param[in] WriteClear           If the rest bits of the register is write clear

  @retval None
**/
VOID
WriteBitDesc (
  const PCH_SMM_BIT_DESC  *BitDesc,
  const BOOLEAN           ValueToWrite,
  const BOOLEAN           WriteClear
  );

#endif
