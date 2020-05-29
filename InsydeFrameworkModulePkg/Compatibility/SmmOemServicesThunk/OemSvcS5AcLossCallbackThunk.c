/** @file
  Provide OEM to modify the code according to OEM specification, 
  when the system loss the power and be forced into S5 status. 

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
  Provide OEM to modify the code according to OEM specification, 
  when the system loss the power and be forced into S5 status.
  
  @param  Base on OEM design.
  
  @retval EFI_SUCCESS           Always returns success.
**/
EFI_STATUS
OemSvcS5AcLossCallbackThunk (
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
  Status = OemSvcS5AcLossCallback ();

  return Status;
}
