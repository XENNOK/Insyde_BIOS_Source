/** @file
  Provide OEM to modify the task when the recovery failed.

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

#include <Library/PeiOemSvcKernelLib.h>

/**
  This OemService call at the end of the Recovery Mode. 
  When flash recovery BIOS failed, this service will be called. 
  It provides OEM to modify the task when the recovery failed.
   
  @param  Based on OEM design.

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
  @retval EFI_SUCCESS           The service is customized in the project.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcHookWhenRecoveryFail (
  VOID		
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
