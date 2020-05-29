/** @file
  Project dependent initial code in PlatformStage1.

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

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>

/**
  Project dependent initial code in PlatformStage1.

  @param  Base on OEM design.

  @retval EFI_SUCCESS           Always returns success.
**/
EFI_STATUS
OemSvcInitPlatformStage1Thunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  VA_END (Marker);
  
  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcInitPlatformStage1 ();

  return Status;

}
