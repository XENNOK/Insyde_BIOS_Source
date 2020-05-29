/** @file
  This service is called when disable ACPI mode.

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

#include "SmmOemServicesThunk.h"
#include <Library/SmmOemSvcKernelLib.h>

/**
  This service is called when disable ACPI mode. 
  It provides OEM to add the project charastic code.

  @param  Base on OEM design.

  @retval EFI_SUCCESS           Always returns success.
**/
EFI_STATUS
OemSvcDisableAcpiCallbackThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  VA_END (Marker);
  
  //
  // make a call to SmmOemSvcKernelLib internally
  //
  Status = OemSvcDisableAcpiCallback ();

  return Status;
}
