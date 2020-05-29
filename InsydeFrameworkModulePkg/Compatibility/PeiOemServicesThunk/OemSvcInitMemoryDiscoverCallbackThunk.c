/** @file
  CPU/Chipset/Platform Initialization when PeiMemoryDiscoveredPpi be installed according to project characteristic.

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
  CPU/Chipset/Platform Initialization when PeiMemoryDiscoveredPpi be installed according to project characteristic.

  @param  *BootMode             A pointer to EFI_BOOT_MODE.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInitMemoryDiscoverCallbackThunk (
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
  Status = OemSvcInitMemoryDiscoverCallback (
             BootMode
             );

  return Status;
}
