/** @file
  Provide OEM to define the protect regions when flash the recovery Bios by PeiCrisis method.

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
  This OemService provides OEM to define the protect regions when flash the recovery Bios by PeiCrisis method. 
  The detail of setting protect regions is described in document "PEI Crisis Feature Integration Guide ".  

  @param[out]  *Count                The number of protect regions are defined.
  @param[out]  *UseEcIdle            If UseEcIdle == TRUE, that EC will be idled, when flash the Bios.
  @param[out]  *CrisisProtectTable   The table defines the protected regions when flash the bois.
  
  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           The service is customized in the project.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Depends on customization.
**/
EFI_STATUS
OemSvcGetProtectTable (
  OUT UINTN                                 *Count,
  OUT BOOLEAN                               *UseEcIdle,
  OUT FLASH_ENTRY                           **CrisisProtectTable
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
