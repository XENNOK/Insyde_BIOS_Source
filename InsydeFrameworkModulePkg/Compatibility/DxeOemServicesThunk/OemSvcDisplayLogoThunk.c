/** @file
  Provide OEM to display Logo or not.  

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
  Provide OEM to display Logo or not.

  @param  *QuietBoot            TRUE: Display Logo
                                FALSE: Don't display Logo

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcDisplayLogoThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  BOOLEAN                               *QuietBoot = NULL;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  QuietBoot = VA_ARG (Marker, BOOLEAN  *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcDisplayLogo (QuietBoot);

  return Status;
}
