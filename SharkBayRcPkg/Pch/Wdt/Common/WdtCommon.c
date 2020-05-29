/** @file
  Library that contains common parts of WdtPei and WdtDxe. Not a standalone module.

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
  Copyright (c) 2010 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  WdtCommon.c

@brief
  Library that contains common parts of WdtPei and WdtDxe. Not a standalone module.

**/
#include <WdtCommon.h>

UINT8     mAllowExpectedReset = 0;
///
/// mWdtHobGuid is linked and used in WdtPei and WdtDxe
///
EFI_GUID  mWdtHobGuid = WDT_HOB_GUID;

/**
  Reads LPC bridge to get Watchdog Timer address

  @param[in] none

  @retval UINT32                  Watchdog's address
**/
UINT32
WdtGetAddress (
  VOID
  )
{
  UINT32  Address;

  Address = (MmioRead32 (
              MmPciAddress (0,
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              R_PCH_LPC_ACPI_BASE)) & B_PCH_LPC_ACPI_BASE_BAR)
              + R_PCH_OC_WDT_CTL;

  return Address;
}

/**
  Reloads WDT with new timeout value and starts it. Also sets Unexpected Reset bit, which
  causes the next reset to be treated as watchdog expiration - unless AllowKnownReset()
  function was called too.

  @param[in] TimeoutValue         Time in seconds before WDT times out. Supported range = 1 - 1024.

  @retval EFI_SUCCESS             if everything's OK
  @retval EFI_INVALID_PARAMETER   if TimeoutValue parameter is wrong
**/
EFI_STATUS
EFIAPI
WdtReloadAndStart (
  IN  UINT32  TimeoutValue
  )
{
  UINT32  Readback;

  DEBUG ((EFI_D_INFO, "\n(Wdt) ReloadAndStartTimer(%d)\n", TimeoutValue));

  if ((TimeoutValue > B_PCH_OC_WDT_CTL_TOV_MASK) || (TimeoutValue == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Readback = IoRead32 (WdtGetAddress ());
  Readback |= (B_PCH_OC_WDT_CTL_EN | B_PCH_OC_WDT_CTL_FORCE_ALL | B_PCH_OC_WDT_CTL_ICCSURV);
  if (mAllowExpectedReset == 0) {
    Readback |= B_PCH_OC_WDT_CTL_UNXP_RESET_STS;
  }

#if defined EFI_DEBUG && !defined USE_WDT_IN_DEBUG_BIOS
  ///
  /// in Debug mode, WDT will not be turned on. This is to prevent platform reboots triggered
  /// by WDT expiration, which can be expected when processor is halted for debugging
  ///
  Readback &= ~(B_PCH_OC_WDT_CTL_EN | B_PCH_OC_WDT_CTL_FORCE_ALL | B_PCH_OC_WDT_CTL_UNXP_RESET_STS);
  DEBUG ((EFI_D_INFO, "(Wdt) Wdt disabled in Debug BIOS\n"));

#endif

  Readback &= ~(B_PCH_OC_WDT_CTL_TOV_MASK);
  Readback |= ((TimeoutValue - 1) & B_PCH_OC_WDT_CTL_TOV_MASK);
  IoWrite32 (WdtGetAddress (), Readback);
  Readback |= B_PCH_OC_WDT_CTL_RLD;
  IoWrite32 (WdtGetAddress (), Readback);
  return EFI_SUCCESS;
}

/**
  Disables WDT timer.

  @param[in] None

  @retval None
**/
VOID
EFIAPI
WdtDisable (
  VOID
  )
{
  UINT32  Readback;

  DEBUG ((EFI_D_INFO, "(Wdt) DisableTimer\n"));

  Readback = IoRead32 (WdtGetAddress ());
  Readback &= ~(B_PCH_OC_WDT_CTL_EN | B_PCH_OC_WDT_CTL_FORCE_ALL | B_PCH_OC_WDT_CTL_UNXP_RESET_STS);
  IoWrite32 (WdtGetAddress (), Readback);
}

/**
  Returns WDT failure status.

  @param[in] None

  @retval V_PCH_OC_WDT_CTL_STATUS_FAILURE   If there was WDT expiration or unexpected reset
  @retval V_PCH_OC_WDT_CTL_STATUS_OK        Otherwise
**/
UINT8
EFIAPI
WdtCheckStatus (
  VOID
  )
{
  UINT32  Readback;

  DEBUG ((EFI_D_INFO, "(Wdt) CheckTimerStatus\n"));

  Readback = IoRead32 (WdtGetAddress ());

  DEBUG ((EFI_D_INFO, "(Wdt) Readback = (%x)\n", Readback));

  if (Readback & B_PCH_OC_WDT_CTL_FAILURE_STS) {
    DEBUG ((EFI_D_INFO, "(Wdt) Status = FAILURE\n"));
    return V_PCH_OC_WDT_CTL_STATUS_FAILURE;
  } else {
    return V_PCH_OC_WDT_CTL_STATUS_OK;
  }
}

/**
  Normally, each reboot performed while watchdog runs is considered a failure.
  This function allows platform to perform expected reboots with WDT running, 
  without being interpreted as failures.
  In DXE phase, it is enough to call this function any time before reset.
  In PEI phase, between calling this function and performing reset, ReloadAndStart() 
  must not be called.

  @param[in] None

  @retval None
**/
VOID
EFIAPI
WdtAllowKnownReset (
  VOID
  )
{
  UINT32  Readback;

  DEBUG ((EFI_D_INFO, "(Wdt) AllowKnownReset\n"));

  mAllowExpectedReset = 1;

  Readback            = IoRead32 (WdtGetAddress ());
  Readback &= ~(B_PCH_OC_WDT_CTL_UNXP_RESET_STS | B_PCH_OC_WDT_CTL_FORCE_ALL);
  IoWrite32 (WdtGetAddress (), Readback);
}

/**
  Returns information if WDT coverage for the duration of BIOS execution
  was requested by an OS application

  @param[in] None

  @retval TRUE                    if WDT was requested
  @retval FALSE                   if WDT was not requested
**/
UINT8
EFIAPI
IsWdtRequired (
  VOID
  )
{
  UINT32  Readback;

  DEBUG ((EFI_D_INFO, "(Wdt) IsWdtRequired"));

  Readback = IoRead32 (WdtGetAddress ());

  if ((Readback & B_PCH_OC_WDT_CTL_AFTER_POST) != 0) {
    DEBUG ((EFI_D_INFO, " - yes\n"));
    return TRUE;
  } else {
    DEBUG ((EFI_D_INFO, " - no\n"));
    return FALSE;
  }

}

/**
  Returns WDT enabled/disabled status.

  @param[in] None

  @retval TRUE                    if WDT is enabled
  @retval FALSE                   if WDT is disabled
**/
UINT8
EFIAPI
IsWdtEnabled (
  VOID
  )
{
  UINT32  Readback;

  DEBUG ((EFI_D_INFO, "(Wdt) IsWdtEnabled"));

  Readback = IoRead32 (WdtGetAddress ());

  if ((Readback & B_PCH_OC_WDT_CTL_EN) != 0) {
    DEBUG ((EFI_D_INFO, " - yes\n"));
    return TRUE;
  } else {
    DEBUG ((EFI_D_INFO, " - no\n"));
    return FALSE;
  }

}
