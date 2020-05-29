/** @file
  Provide OEM to customize resolution and SCU resolution. .

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>


/**
  Get OEM customization resolution and SCU resolution. 
  Customization resolution provides OEM to define logo resolution directly or call GetLogoResolution () function to get logo resolution automatically.

  @param  **OemLogoResolutionTable    A double pointer to OEM_LOGO_RESOLUTION_DEFINITION

  @retval EFI_SUCCESS                 Get resolution information successful.
  @retval Others                      Base on OEM design.
**/
EFI_STATUS
OemSvcLogoResolutionThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  OEM_LOGO_RESOLUTION_DEFINITION        **OemLogoResolutionTable;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  OemLogoResolutionTable    = VA_ARG (Marker, OEM_LOGO_RESOLUTION_DEFINITION **);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcLogoResolution (
             OemLogoResolutionTable
             );

  return Status;
}

