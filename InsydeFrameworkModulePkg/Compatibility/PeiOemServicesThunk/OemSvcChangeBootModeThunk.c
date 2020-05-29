/** @file
  This OemService is queried to assign the default boot mode. 
  OEM can use this service to assign default boot mode, 
  and use the parameter SkipPolicy to control normal boot mode identification. 
  If SkipPolicy be set as TRUE, the final boot mode is the same as the parameter "bootmode" which is assigned by this service.

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
  This OemService is queried to assign the default boot mode. 
  OEM can use this service to assign default boot mode, 
  and use the parameter SkipPolicy to control normal boot mode identification. 
  If SkipPolicy be set as TRUE, the final boot mode is the same as the parameter "bootmode" which is assigned by this service.

  @param  *BootMode             Assign default boot mode.
  @param  *SkipPolicy           If SkipPolicy == TRUE, then normal boot mode identification will be skipped.

  @retval EFI_SUCCESS           Always returns success.
**/
EFI_STATUS
OemSvcChangeBootModeThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_BOOT_MODE                         *BootMode;
  BOOLEAN                               *SkipPriorityPolicy;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  BootMode = VA_ARG (Marker, EFI_BOOT_MODE *);
  SkipPriorityPolicy = VA_ARG (Marker, BOOLEAN *);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcChangeBootMode (
             BootMode,
             SkipPriorityPolicy
             );

  return Status;
}
