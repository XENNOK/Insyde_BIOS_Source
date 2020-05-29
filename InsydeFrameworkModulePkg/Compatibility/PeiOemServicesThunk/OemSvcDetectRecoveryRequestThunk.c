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

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>

/**
  This OemService provides OEM to detect the recovery mode. 
  OEM designs the rule to detect that boot mode is recovery mode or not, 
  The rule bases on the recovery request which defined in OemService "OemSetRecoveryRequest".

  @param  *IsRecovery           If service detects that the current system is recovery mode.
                                This parameter will return TRUE, else return FALSE.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcDetectRecoveryRequestThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  BOOLEAN                               *IsRecovery;  
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  IsRecovery = VA_ARG (Marker, BOOLEAN *);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcDetectRecoveryRequest (
             IsRecovery
             );

  return Status;
}
