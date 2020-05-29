/** @file
  The OemService creates the firmware volume hob, stores the base address 
  and size of PEI firmware volume The base address corresponds to the parameter bootmode.

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
  The OemService creates the firmware volume hob, stores the base address 
  and size of PEI firmware volume The base address corresponds to the parameter bootmode.

  @param  *BootMode             Point to EFI_BOOT_MODE.

  @retval EFI_SUCCESS           Store firmware volume info success.
  @retval Others                Create firmware volume hob failed.
**/
EFI_STATUS
OemSvcBootModeCreateFvThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_BOOT_MODE                         *BootMode;  
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  BootMode = VA_ARG (Marker, EFI_BOOT_MODE *);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcBootModeCreateFv (
             (*BootMode)
             );

  return Status;
}
