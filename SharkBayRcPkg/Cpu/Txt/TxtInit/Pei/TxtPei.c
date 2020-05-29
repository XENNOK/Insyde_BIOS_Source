/** @file
  This is the main PEIM file for TXT.  It represents an abstract outline of the
  steps required during PEI for enabling TXT.  Each individual step is further
  abstracted behind a function call interface.  This is intended to minimize
  the need to modify this file when porting TXT to future platforms.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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

#include "TxtPeiLib.h"
#include <Library/HobLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Guid/SaDataHob.h>

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiTxtMemoryUnlockedPpiGuid,
  NULL
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  DprUpdate
};

/**
 
  This is the entry point to the TXT PEIM.  The TXT PEIM checks for an TXT
  capable platform and determines whether SCLEAN should be run.  If so,
  it launches the BIOS ACM to run SCLEAN (which will reset the platform).
  If not, the PEIM checks to see if the platform is resuming from S3.

  If the platform is resuming from S3, this code will register a callback
  so that SCHECK will be run when BootScript is done restoring the platform's
  configuration.

  @param[in] FfsHeader     - A pointer the the FFS File containing this PEIM.
  @param[in] PeiServices   - A Pointer to the PEI Services Table.

  @exception EFI_UNSUPPORTED   - If the platform is not TXT capable.
  @retval EFI_SUCCESS       - In all other cases not listed above.

**/
EFI_STATUS
EFIAPI
PeimEntry (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS          Status;
  TXT_PEI_LIB_CONTEXT ctx;
  BOOLEAN             TxtEnvInitFail;
  TxtEnvInitFail = FALSE;

  ///
  /// Install PPI to tell memory code that it can run.
  /// Do it always.
  ///
  Status = PeiServicesInstallPpi (&mPpiList);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize the TXT PEI Lib functions
  ///
  Status = InitializeTxtPeiLib (
            &ctx,
            PeiServices
            );

  if (EFI_ERROR (Status)) {
    TxtEnvInitFail = TRUE;
    DEBUG ((EFI_D_ERROR, "TXTPEI::PEI Lib initialization failure\n"));
  }
  ///
  /// Determine TPM presence. If TPM is not present - disabling TXT through TxtInfoHob by setting TxtMode=0
  /// Incase TXT had been enabled but TPM was removed suddenly. Although TPM presence is precondition of this module
  /// since all commands executed by BIOS ACM don't depend on TPM state.
  /// TPM_NV_read will be successfully executed even if TPM is disabled
  /// and/or deactivated because all indices defined for BIOS ACM
  /// usage don't require authorization. TPM_ResetEstablishmentBit
  /// doesn't depend on TPM state at all and can
  /// be executed with disabled/deactivated TPM always.
  /// Case when TPM is completely not functional is not considered.
  ///
  Status = IsTpmPresent (&ctx);
  if (EFI_ERROR (Status)) {
    ///
    /// If TPM is not present / not supported, set TxtMode=0 incase TPM was removed after TXT enabled
    ///
    if (Status == EFI_UNSUPPORTED) {
      DEBUG ((EFI_D_WARN, "TXTPEI::TPM Support is Disabled in BIOS! Disabling TXT! TxtMode=%x\n", ctx.Hob->Data.TxtMode));
    } else {
      DEBUG ((EFI_D_WARN, "TXTPEI::TPM is not present! Disabling TXT! TxtMode=%x\n", ctx.Hob->Data.TxtMode));
    }

    TxtEnvInitFail = TRUE;
  }
  ///
  /// Detect TXT capable Processor & PCH
  ///
  if (!IsTxtChipset (&ctx)) {
    DEBUG ((EFI_D_WARN, "TXTPEI::Platform or PCH is not TXT capable\n"));
    return EFI_UNSUPPORTED;
  } else if (!IsTxtProcessor ()) {
    DEBUG ((EFI_D_WARN, "TXTPEI::Processor is not TXT capable\n"));
    return EFI_UNSUPPORTED;
  } else {
    DEBUG ((EFI_D_WARN, "TXTPEI::Processor, PCH & Platform is TXT capable\n"));
    ///
    /// If Txt Lib or TPM is initiated successful, disable TxT support.
    ///
    if (TxtEnvInitFail) {
      UnlockMemory (&ctx);
      ctx.Hob->Data.TxtMode = 0;
      ASSERT (TRUE);
      return EFI_UNSUPPORTED;
    }
  }
  ///
  /// Memory is supposed to lock if system is TxT capable.
  /// Check if we need to run SCLEAN. TxT BIOS spec Section 6.2.5
  ///
  if (IsEstablishmentBitAsserted (&ctx) && IsTxtWakeError (&ctx)) {

    DEBUG ((EFI_D_INFO, "TXTPEI::EstablishmentBit is set\n"));
    ///
    /// If TXTRESET is set , we must clean TXTRESET bit otherwise SCLEAN
    /// will fail
    ///
    if (IsTxtResetSet (&ctx)) {
      DoGlobalReset ();
    }
    ///
    /// Setup and Launch SCLEAN
    ///
    DEBUG ((EFI_D_INFO, "TXTPEI::Entering SCLEAN\n"));

    DoSclean (&ctx);

    ///
    /// Reset platform - performed by DoSclean, should not return to execute the following dead looping
    ///
    EFI_DEADLOOP ();
  } else {
    ///
    /// Unlock memory, and then continue running
    ///
    DEBUG ((EFI_D_INFO, "TXTPEI::EstablishmentBit not asserted - Unlock Memory\n"));
    AsmWriteMsr64 (0x2e6, 0);
  }

  Status = PeiServicesNotifyPpi (&mNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Fix up pointers since they are located in real memory now.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
DprUpdate (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *Ppi
  )
{
  TXT_INFO_HOB        *TxtInfoHob;
  SA_DATA_HOB         *SaDataHob;
  DPR_DIRECTORY_ENTRY *DprDirectory;
  UINT16              Index;

  TxtInfoHob   = NULL;
  SaDataHob    = NULL;
  DprDirectory = NULL;
  Index        = 0;

  //
  // Get TxtInfoHob
  //
  TxtInfoHob = (TXT_INFO_HOB *)GetFirstGuidHob (&gTxtInfoHobGuid);
  if (TxtInfoHob == NULL) {
    return EFI_NOT_FOUND;
  }
  TxtInfoHob->Data.TxtDprMemoryBase = 0;

  //
  // Get SaDataHob
  //
  SaDataHob = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob == NULL) {
    return EFI_NOT_FOUND;
  }
  DprDirectory = (DPR_DIRECTORY_ENTRY *)&(SaDataHob->DprDirectory[0]);

  //
  // Find TxT DPR Directory
  //
  for (Index=0; Index<DPR_DIRECTORY_MAX; Index++) {
    if (DprDirectory[Index].Type == DPR_DIRECTORY_TYPE_TXT) {
      TxtInfoHob->Data.TxtDprMemoryBase = (EFI_PHYSICAL_ADDRESS)DprDirectory[Index].PhysBase;
      break;
    }
  }

  return EFI_SUCCESS;
}
