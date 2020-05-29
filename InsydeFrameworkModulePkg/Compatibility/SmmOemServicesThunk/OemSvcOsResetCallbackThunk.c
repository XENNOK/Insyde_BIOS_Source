/** @file
  Os reset callback function.

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
  Os reset callback function.
  
**/
EFI_STATUS
OemSvcOsResetCallbackThunk (
  IN SMM_OEM_SERVICES_PROTOCOL            *OemServices,
  IN UINTN                                NumOfArgs,
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
  Status = OemSvcOsResetCallback ();

  return Status;
}
