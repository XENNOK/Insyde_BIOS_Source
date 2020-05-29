/** @file
  Implementation file for Watchdog Timer functionality

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
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
  WdtPei.c

@brief
  Implementation file for Watchdog Timer functionality

**/
#include <Ppi/PchReset.h>
#include <WdtCommon.h>
#include <Ppi/Wdt.h>
#include <Ppi/EndOfPeiPhase.h>

EFI_STATUS
EFIAPI
WdtPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  );

static WDT_PPI                    mWdtPpi = {
  WdtReloadAndStart,
  WdtCheckStatus,
  WdtDisable,
  WdtAllowKnownReset,
  IsWdtRequired,
  IsWdtEnabled
};

static PCH_RESET_CALLBACK_PPI     mPchResetCallbackPpi = { WdtPchResetCallback };

EFI_STATUS
EndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_PPI_DESCRIPTOR     mInstallWdtPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gWdtPpiGuid,
  &mWdtPpi
};

static EFI_PEI_PPI_DESCRIPTOR     mInstallPchResetCallbackPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPchResetCallbackPpiGuid,
  &mPchResetCallbackPpi
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndOfPeiCallback
};

#define MINIMUM_TIMEOUT_AT_S3_EXIT  10  ///< seconds

/**
  Reads PCH registers to check if platform wakes from S3/S4

  @param[in] None

  @retval TRUE                    if platfrom wakes from S3/S4
  @retval FALSE                   otherwise
**/
UINT8
IsWakeFromS3_S4 (
  VOID
  )
{
  UINT32  Address;
  UINT16  SleepType;

  Address = MmioRead32 (
              MmPciAddress (0,
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              R_PCH_LPC_ACPI_BASE)
              ) & B_PCH_LPC_ACPI_BASE_BAR;

  if (IoRead16 (Address + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_WAK) {
    SleepType = IoRead16 (Address + R_PCH_ACPI_PM1_CNT) & B_PCH_ACPI_PM1_CNT_SLP_TYP;
    if ((SleepType == V_PCH_ACPI_PM1_CNT_S3) || (SleepType == V_PCH_ACPI_PM1_CNT_S4)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Initializes watchdog failure bits.
  If there was an unexpected reset, enforces WDT expiration.
  Stores initial WDT state in a HOB, it is useful in flows with S3/S4 resume.
  Stops watchdog. 
  Installs watchdog PPI for other modules to use.

  @param[in] FfsHeader            Pointer to Firmware File System file header.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             When everything is OK
**/
EFI_STATUS
WdtPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE                   FileHandle,
  IN CONST EFI_PEI_SERVICES                **PeiServices
  )
{
  UINT32      Readback;
  EFI_STATUS  Status;
  UINT16      TimeoutValue;
  UINT8       Active;
  WDT_HOB     *WdtHobPtr;

#ifndef WDT_SUPPORT_ENABLED
  ///
  /// clear status bits and disable watchdog, then lock the register
  ///
  IoWrite32 (WdtGetAddress (), (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS));
  IoWrite32 (WdtGetAddress (), B_PCH_OC_WDT_CTL_LCK);
#endif

  Readback = IoRead32 (WdtGetAddress ());

  DEBUG ((EFI_D_INFO, "(WDT) Readback = 0x%08x\n", Readback));
  ///
  /// Write current Wdt settings to a HOB, they may be be needed in S3/S4 resume paths
  ///
  if (Readback & B_PCH_OC_WDT_CTL_EN) {
    Active        = 1;
    TimeoutValue  = (UINT16) ((Readback & B_PCH_OC_WDT_CTL_TOV_MASK) + 1);
  } else {
    Active        = 0;
    TimeoutValue  = 0;
  }

  Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof (WDT_HOB), (VOID **) &WdtHobPtr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  WdtHobPtr->Header.Name  = mWdtHobGuid;
  WdtHobPtr->Active       = Active;
  WdtHobPtr->TimeoutValue = TimeoutValue;
  ///
  /// If there was a WDT expiration, set Failure Status and clear timeout status bits
  /// Timeout status bits are cleared by writing '1'
  ///
  if (Readback & (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS)) {
    DEBUG ((EFI_D_ERROR, "(WDT) Expiration detected.\n", Readback));
    Readback |= B_PCH_OC_WDT_CTL_FAILURE_STS;
    Readback |= (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS);
    Readback &= ~(B_PCH_OC_WDT_CTL_UNXP_RESET_STS);
  } else {
    ///
    /// If there was unexpected reset but no WDT expiration and no resume from S3/S4,
    /// clear unexpected reset status and enforce expiration. This is to inform Firmware
    /// which has no access to unexpected reset status bit, that something went wrong.
    ///
    if ((Readback & B_PCH_OC_WDT_CTL_UNXP_RESET_STS) && !IsWakeFromS3_S4 ()) {
#if defined EFI_DEBUG && !defined USE_WDT_IN_DEBUG_BIOS
      DEBUG ((EFI_D_ERROR, "(WDT) Unexpected reset detected and ignored.\n"));
      Readback &= ~(B_PCH_OC_WDT_CTL_FAILURE_STS | B_PCH_OC_WDT_CTL_UNXP_RESET_STS);
      Readback |= (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS);
#else
      DEBUG ((EFI_D_ERROR, "(WDT) Unexpected reset detected. Enforcing Wdt expiration.\n"));
      WdtReloadAndStart (1);
      while (1) {
        ///
        /// wait for reboot caused by WDT expiration
        ///
      }
#endif
    } else {
      ///
      /// No WDT expiration and no unexpected reset - clear Failure status
      ///
      DEBUG ((EFI_D_INFO, "(WDT) Status OK.\n", Readback));
      Readback &= ~(B_PCH_OC_WDT_CTL_FAILURE_STS);
      Readback |= (B_PCH_OC_WDT_CTL_ICCSURV_STS | B_PCH_OC_WDT_CTL_NO_ICCSURV_STS);
    }
  }

  IoWrite32 (WdtGetAddress (), Readback);
  ///
  /// register an event for EndOfPei. It will support Wdt in resume from S3.
  ///
  Status  = (**PeiServices).NotifyPpi (PeiServices, &mNotifyList);

  Status  = (**PeiServices).InstallPpi (PeiServices, &mInstallWdtPpi);

  Status  = (**PeiServices).InstallPpi (PeiServices, &mInstallPchResetCallbackPpi);

  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Support for WDT in S3 resume.
  If WDT was enabled during S0->S3 transition, this function will turn on WDT
  just before waking OS. Timeout value will be overridden if it was too small. 

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             When everything is OK
  @retval EFI_NOT_FOUND           WdtHob is not found
**/
EFI_STATUS
EndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  WDT_HOB       *WdtHob;
  EFI_STATUS    Status;
  EFI_BOOT_MODE BootMode;

  DEBUG ((EFI_D_INFO, "(WDT) EndOfPeiCallback\n"));
  Status = (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);
  WdtHob = GetFirstGuidHob (&mWdtHobGuid);
  if (WdtHob == NULL) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((EFI_D_INFO, "(WDT) BootMode %d, Hob, active %d, ToV %d\n", BootMode, WdtHob->Active, WdtHob->TimeoutValue));

  if (BootMode == BOOT_ON_S3_RESUME) {
    if (WdtHob->Active == 1) {
      if (WdtHob->TimeoutValue < MINIMUM_TIMEOUT_AT_S3_EXIT) {
        WdtReloadAndStart (MINIMUM_TIMEOUT_AT_S3_EXIT);
      } else {
        WdtReloadAndStart (WdtHob->TimeoutValue);
      }
    } else {
      WdtDisable ();
    }
  }

  return EFI_SUCCESS;
}

/**
  WDT call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
WdtPchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  WdtAllowKnownReset ();
  return EFI_SUCCESS;
}
