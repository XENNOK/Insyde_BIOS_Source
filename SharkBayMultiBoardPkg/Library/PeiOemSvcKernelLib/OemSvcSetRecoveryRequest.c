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
//[-start-130124-IB10130023-add]//
#include <Library/CmosLib.h>

#include <ChipsetCmos.h>


#define CMOS_ISRECOVERY                           0x55
//[-end-130124-IB10130023-add]//

/**
  This OemService provides OEM to decide the method of recovery request. 
  When DXE loader found that the DXE-core of firmware volume is corrupt, it will force system to restart. 
  This service will be called to set the recovery requests before system restart. 
  To design the recovery requests according to OEM specification.

  @param  Based on OEM design.

  @retval EFI_UNSUPPORTED      Returns unsupported by default.
  @retval EFI_SUCCESS          The service is customized in the project.
  @retval EFI_MEDIA_CHANGED    The value of IN OUT parameter is changed. 
  @retval Others               Depends on customization.
**/
EFI_STATUS
OemSvcSetRecoveryRequest (
  VOID
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  
//[-start-130124-IB10130022-add]//
  WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, RecoveryFlag, CMOS_ISRECOVERY);
//[-end-130124-IB10130022-add]//
  return EFI_UNSUPPORTED;
}

