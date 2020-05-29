/** @file
  This OemService provides OEM to decide the method of recovery request. 
  When DXE loader found that the DXE-core of firmware volume is corrupt, it will force system to restart. 
  This service will be called to set the recovery requests before system restart. 
  To design the recovery requests according to OEM specification.

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
  This OemService provides OEM to detect the recovery mode. 
  OEM designs the rule to detect that boot mode is recovery mode or not, 
  The rule bases on the recovery request which defined in OemService "OemSetRecoveryRequest".

  @param[in, out]  *IsRecovery        If service detects that the current system is recovery mode.
                                      This parameter will return TRUE, else return FALSE.

  @retval          EFI_UNSUPPORTED    Returns unsupported by default.
  @retval          EFI_SUCCESS        The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
  @retval          Others             Depends on customization.
**/
EFI_STATUS
OemSvcDetectRecoveryRequest (
  IN OUT BOOLEAN                               *IsRecovery
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
