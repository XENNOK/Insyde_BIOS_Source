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
  This OemServices provides OEM to set the maximum count of user enters the password.

  @param  *ReportPasswordCount  A pointer to the count of entering the password.

  @retval EFI_SUCCESS           Always returns success
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetMaxCheckPasswordCountThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{ 
  VA_LIST                               Marker;

  UINTN                                 *ReportPasswordCount;
  EFI_STATUS                            Status;
  
  VA_START (Marker, NumOfArgs);
  ReportPasswordCount = VA_ARG (Marker, UINTN *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcGetMaxCheckPasswordCount (
             ReportPasswordCount
             );

  return Status;
}

