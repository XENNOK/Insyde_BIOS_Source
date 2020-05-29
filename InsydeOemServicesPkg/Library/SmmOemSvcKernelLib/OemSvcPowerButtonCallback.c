/** @file
  This service belongs to the event of power button. 
  When power button event is triggered, this service will be called. 
  OEM can add specific code in this service, if they want to do something when power button event occur. 

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
  This service belongs to the event of power button. 
  When power button event is triggered, this service will be called. 
  OEM can add specific code in this service, if they want to do something when power button event occur. 

  @param  Base on OEM design.
  
  @retval EFI_UNSUPPORTED       Returns unsupported by default.
  @retval EFI_SUCCESS           The service is customized in the project.
  @retval EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcPowerButtonCallback (
  VOID
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
