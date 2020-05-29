//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/** @file
  Header file for the RapidStartDxePolicyInit Driver.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
#ifndef _RAPID_START_DXE_PLATFORM_POLICY_H_
#define _RAPID_START_DXE_PLATFORM_POLICY_H_
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CmosLib.h>
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <Protocol/RapidStartPlatformPolicy.h>
#include <Protocol/SetupUtility.h>
#include <ChipsetSetupConfig.h>
#include <ChipsetCmos.h>

/**
  Initilize Intel RapidStart DXE Platform Policy

  @param[in] ImageHandle - Image handle of this driver.
  @param[in] SystemTable - Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
RapidStartDxePolicyInitEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN OUT EFI_SYSTEM_TABLE *SystemTable
  ); 

VOID
DumpRapidStartPolicy (
  IN      RAPID_START_PLATFORM_POLICY_PROTOCOL        *RapidStartPlatformPolicy
  );

#endif
