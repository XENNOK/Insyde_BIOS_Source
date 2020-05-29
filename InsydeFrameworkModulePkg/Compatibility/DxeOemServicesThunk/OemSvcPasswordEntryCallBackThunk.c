/** @file
  Provide OEM to execute specific process, before user inputs the password.

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
  Provide OEM to execute specific process, before user inputs the password.
  
  @param  Base on OEM design.

  @retval EFI_SUCCESS           Always returns success
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcPasswordEntryCallBackThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{ 
  VA_LIST                               Marker;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  VA_END (Marker);
  
  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcPasswordEntryCallBack ();

  return Status;
}
