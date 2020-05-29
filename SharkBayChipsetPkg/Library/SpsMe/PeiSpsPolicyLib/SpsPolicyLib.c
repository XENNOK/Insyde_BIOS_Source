/** @file

  Implementation file for SPS Policy functionality in PEI Phase.
  Note: Only for SPS.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

#include <Ppi/SpsPolicy.h>

/**
  Get SPS ME Policy.

  @param[out] SpsPolicy           Point of SPS Policy.

  @retval EFI_SUCCESS             SPS Policy Found.
**/
EFI_STATUS
GetSpsPolicy (
  OUT VOID                             **SpsPolicy
  )
{
  SPS_POLICY_PPI   *TempPeiSpsMePolicy;
  EFI_STATUS       Status;

  if (SpsPolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TempPeiSpsMePolicy = NULL;

  Status = PeiServicesLocatePpi (
             &gSpsPolicyPpiGuid,           // GUID
             0,                            // INSTANCE
             NULL,                         // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &TempPeiSpsMePolicy // PPI
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[PEI SPS POLICY LIB] ERROR: Locate SPS Policy PPI Fail! (%r).\n", Status));
    return Status;
  }

  *SpsPolicy = TempPeiSpsMePolicy;
  if ((TempPeiSpsMePolicy == NULL) || (*SpsPolicy == NULL)) {
    ///
    ///BUGBUG: Can not Locate SPS PEI POLICY PPI.
    ///  Status          = EFI_SUCCESS
    ///  PeiSpsMePolicy  = 0x00000000;
    ///
    DEBUG ((DEBUG_ERROR, "[PEI SPS POLICY LIB] ERROR: Locate Success but address invaild (%x)\n", *SpsPolicy));
    return EFI_NOT_STARTED;
  }

  return EFI_SUCCESS;
}

