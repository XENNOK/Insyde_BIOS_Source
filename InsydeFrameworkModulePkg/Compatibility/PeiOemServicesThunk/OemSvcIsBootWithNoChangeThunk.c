/** @file
  Determine if "Boot with no change" is true according to project characteristic.

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
  Determine if "Boot with no change" is true according to project characteristic.

  @param  *IsNoChange           If IsNoChange == TRUE, then boot mode will be set to 
                                BOOT_ASSUMING_NO_CONFIGURATION_CHANGES which might reduce the POST time.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcIsBootWithNoChangeThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  BOOLEAN                               *IsNoChange;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  IsNoChange = VA_ARG (Marker, BOOLEAN *);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcIsBootWithNoChange (
             IsNoChange
             );

  return Status;
}
