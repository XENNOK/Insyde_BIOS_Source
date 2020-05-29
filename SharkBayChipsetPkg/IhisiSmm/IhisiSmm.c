/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IhisiSmm.h"

//
//Subfunction
//
SMI_SUB_FUNCTION
mFunctionTable[] = {
                   VATS_FUNCTIONS,
                   FBTS_FUNCTIONS,
                   FETS_FUNCTIONS,
                   COMMON_REGION_FUNCTIONS,
                   SECURE_FLASH_FUNCTION,
                   OEM_COMMON_FEATURE_FUNCTIONS,
//[-start-130325-IB08340099-add]//
                   BIOSGCI_FUNCTIONS
//[-end-130325-IB08340099-add]//
};

/**
  Ihisi driver entry point to initialize all of IHISI relative services

  @param[in] ImageHandle        The firmware allocated handle for the UEFI image.
  @param[in] SystemTable        A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
IhisiEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{

  EFI_STATUS                            Status;

  Status = IhisiLibInit (mFunctionTable, sizeof (mFunctionTable) / sizeof (SMI_SUB_FUNCTION));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = VatsLibInit ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FbtsInit ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-120913-IB05300329-add]//
  Status = OemCommonFeatureInit ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
//[-end-120913-IB05300329-add]//

//[-start-130325-IB08340099-add]//
    Status = BiosGCIInit ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
//[-end-130325-IB08340099-add]//

  return EFI_SUCCESS;
}
