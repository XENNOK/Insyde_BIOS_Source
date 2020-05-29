/** @file

  Implementation file for SPS Policy functionality in DXE Phase.
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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/SpsPolicy.h>

///
/// Global variables
///
SPS_POLICY_PROTOCOL                    *mSpsMePolicy = NULL;

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
  EFI_STATUS       Status;

  if (SpsPolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSpsMePolicy != NULL) {
    *SpsPolicy = mSpsMePolicy;
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, (VOID **) &mSpsMePolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[DXE SPS POLICY LIB] ERROR: Locate SPS Policy Protocol => (%r)\n", Status));
    return Status;
  }
  *SpsPolicy = mSpsMePolicy;

  return EFI_SUCCESS;
}

