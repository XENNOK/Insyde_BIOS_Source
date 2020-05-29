/** @file
  PCH Pcie SMM Driver Header

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

@copyright
  Copyright (c) 2010 - 2013 Intel Corporation. All rights reserved
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
#ifndef _PCH_PCIE_SMM_H
#define _PCH_PCIE_SMM_H

#include <EfiScriptLib.h>
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/SmmIchnDispatchEx.h>
#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <Library/PchPciExpressHelpersLib.h>
//
// Prototypes
//

/**
  Program Common Clock and ASPM of Downstream Devices

  @param[in] PciePortNum          Pcie Root Port Number

  @retval EFI_SUCCESS             Function complete successfully
**/
EFI_STATUS
PchPcieSmi (
  IN  UINT8         PciePortNum
  );

/**
  PCIE Hotplug SMI call back function for each Root port

  @param[in] DispatchHandle       Handle of this dispatch function
  @param[in] DispatchContext      Pointer to the dispatch function's context.
                                  The DispatchContext fields are filled in by the dispatching driver
                                  prior to invoke this dispatch function
**/
VOID
EFIAPI
PchPcieSmiHandlerFunction (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        *DispatchContext
  );

/**
  PCIE Link Active State Change Hotplug SMI call back function for all Root ports

  @param[in] DispatchHandle       Handle of this dispatch function
  @param[in] DispatchContext      Pointer to the dispatch function's context.
                                  The DispatchContext fields are filled in by the dispatching driver
                                  prior to invoke this dispatch function
**/
VOID
EFIAPI
PchPcieLinkActiveStateChange (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        *DispatchContext
  );

/**
  Register PCIE Hotplug SMI dispatch function to handle Hotplug enabling

  @param[in] ImageHandle          The image handle of this module
  @param[in] SystemTable          The EFI System Table

  @retval EFI_SUCCESS             The function completes successfully
**/
EFI_STATUS
EFIAPI
InitializePchPcieSmm (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  );
#endif
