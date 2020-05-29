/** @file
  This OemService queries to determine if it needed to load default setup menu.

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
  This OemService queries to determine if it needed to load default setup menu. 
  If LoadDefault == TRUE, system will erase the "Setup" variable. 
  It provides a hardware method to load the default setup setting. 

  @param  *LoadDefaultPtr       Loading default setup menu or not.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcLoadDefaultSetupMenuThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN UINTN                                 NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  BOOLEAN                               *LoadDefaultPtr;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  LoadDefaultPtr = VA_ARG (Marker, BOOLEAN *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcLoadDefaultSetupMenu (
             LoadDefaultPtr
             );

  return Status;
}
