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

#include <Library/SmmOemSvcKernelLib.h>

/**
  Provide OEM to modify the code according to OEM specification, 
  when the system loss the power and be forced into S5 status.
  
  @param  Base on OEM design.
  
  @retval EFI_UNSUPPORTED       Returns unsupported by default.
  @retval EFI_SUCCESS           The service is customized in the project.
  @retval EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval Others                Depends on customization.
**/
EFI_STATUS
OemSvcS5AcLossCallback (
  VOID
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
