/** @file
  Provide OEM to set the maximum count of user enters the password.

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
  The callback function for checking the password failed. Provide OEM to add the project characteristic code.

  @param  Base on OEM design.

  @retval EFI_SUCCESS           Always returns success
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcCheckPasswordFailCallBackThunk (
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
  Status = OemSvcCheckPasswordFailCallBack ();

  return Status;
}

