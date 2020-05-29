/** @file
  Provide OEM to check update platform display device or not.

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
  Provide OEM to check update platform display device or not.

  @param  *SkipOriginalCode     TRUE:  skip platform display device update
                                FALSE: need do platform display device update

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcBootDisplayDeviceReplaceThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN UINTN                              NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  BOOLEAN                               *SkipOriginalCode;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  SkipOriginalCode = VA_ARG (Marker, BOOLEAN *);
  VA_END (Marker);
  
  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcBootDisplayDeviceReplace (SkipOriginalCode);
  
  return Status; 
}

